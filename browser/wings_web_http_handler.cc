#include "wings/browser/wings_web_http_handler.h"
#include "wings/browser/wings.h"
#include "wings/browser/wings_web_socket_factory.h"
#include "wings/browser/wings_web_frontend_host.h"
#include "wings/browser/wings_web_frontend.h"

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <utility>

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/files/file_util.h"
#include "base/guid.h"
#include "base/json/json_writer.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ref_counted_memory.h"
#include "base/message_loop/message_loop.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/task_scheduler/post_task.h"
#include "base/threading/thread.h"
#include "base/values.h"
#include "build/build_config.h"
#include "content/browser/devtools/devtools_http_handler.h"
#include "content/browser/devtools/devtools_manager.h"
#include "content/browser/devtools/grit/devtools_resources.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/devtools_external_agent_proxy_delegate.h"
#include "content/public/browser/devtools_manager_delegate.h"
#include "content/public/browser/devtools_socket_factory.h"
#include "content/public/common/content_client.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/user_agent.h"
#include "net/base/escape.h"
#include "net/base/io_buffer.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/server/http_server.h"
#include "net/server/http_server_request_info.h"
#include "net/server/http_server_response_info.h"
#include "net/socket/server_socket.h"
// #include "third_party/brotli/include/brotli/decode.h"
#include "v8/include/v8-version-string.h"

namespace wings {

namespace {

const char kWingsWebHandlerThreadName[] = "Wings_WebHandlerThread";

const int32_t kSendBufferSizeForWingsWeb = 256 * 1024 * 1024;  // 256Mb
const int32_t kReceiveBufferSizeForWingsWeb = 100 * 1024 * 1024;  // 100Mb

} // namespace

class ServerWrapper : net::HttpServer::Delegate {
public:
  ServerWrapper(base::WeakPtr<WingsWebHttpHandler> handler,
                std::unique_ptr<net::ServerSocket> socket);
  int GetLocalAddress(net::IPEndPoint* address);

  void AcceptWebSocket(int connection_id,
                       const net::HttpServerRequestInfo& request);
  void SendOverWebSocket(int connection_id, const std::string& message);
  void SendResponse(int connection_id,
                    const net::HttpServerResponseInfo& response);
  void Send200(int connection_id,
               const std::string& data,
               const std::string& mime_type);
  void Send404(int connection_id);
  void Send500(int connection_id, const std::string& message);
  void Close(int connection_id);

  ~ServerWrapper() override {}

private:
  // net::HttpServer::Delegate implementation.
  void OnConnect(int connection_id) override {}
  void OnHttpRequest(int connection_id,
                     const net::HttpServerRequestInfo& info) override;
  void OnWebSocketRequest(int connection_id,
                          const net::HttpServerRequestInfo& info) override;
  void OnWebSocketMessage(int connection_id,
                          const std::string& data) override;
  void OnClose(int connection_id) override;

  base::WeakPtr<WingsWebHttpHandler> handler_;
  std::unique_ptr<net::HttpServer> server_;
};

ServerWrapper::ServerWrapper(base::WeakPtr<WingsWebHttpHandler> handler,
                             std::unique_ptr<net::ServerSocket> socket)
    : handler_(handler),
      server_(new net::HttpServer(std::move(socket), this)) {}

int ServerWrapper::GetLocalAddress(net::IPEndPoint* address) {
  return server_->GetLocalAddress(address);
}

void ServerWrapper::AcceptWebSocket(int connection_id,
                                    const net::HttpServerRequestInfo& request) {
  server_->SetSendBufferSize(connection_id, kSendBufferSizeForWingsWeb);
  server_->SetReceiveBufferSize(connection_id, kReceiveBufferSizeForWingsWeb);
  server_->AcceptWebSocket(connection_id, request);
}

void ServerWrapper::SendOverWebSocket(int connection_id,
                                      const std::string& message) {
  server_->SendOverWebSocket(connection_id, message);
}

void ServerWrapper::SendResponse(int connection_id,
                                 const net::HttpServerResponseInfo& response) {
  server_->SendResponse(connection_id, response);
}

void ServerWrapper::Send200(int connection_id,
                            const std::string& data,
                            const std::string& mime_type) {
  server_->Send200(connection_id, data, mime_type);
}

void ServerWrapper::Send404(int connection_id) {
  server_->Send404(connection_id);
}

void ServerWrapper::Send500(int connection_id,
                            const std::string& message) {
  server_->Send500(connection_id, message);
}

void ServerWrapper::Close(int connection_id) {
  server_->Close(connection_id);
}

static std::string PathWithoutParams(const std::string& path) {
  size_t query_position = path.find("?");
  if (query_position != std::string::npos)
    return path.substr(0, query_position);
  return path;
}

static std::string GetMimeType(const std::string& filename) {
  if (base::EndsWith(filename, ".html", base::CompareCase::INSENSITIVE_ASCII)) {
    return "text/html";
  } else if (base::EndsWith(filename, ".css",
                            base::CompareCase::INSENSITIVE_ASCII)) {
    return "text/css";
  } else if (base::EndsWith(filename, ".js",
                            base::CompareCase::INSENSITIVE_ASCII)) {
    return "application/javascript";
  } else if (base::EndsWith(filename, ".png",
                            base::CompareCase::INSENSITIVE_ASCII)) {
    return "image/png";
  } else if (base::EndsWith(filename, ".gif",
                            base::CompareCase::INSENSITIVE_ASCII)) {
    return "image/gif";
  } else if (base::EndsWith(filename, ".json",
                            base::CompareCase::INSENSITIVE_ASCII)) {
    return "application/json";
  } else if (base::EndsWith(filename, ".svg",
                            base::CompareCase::INSENSITIVE_ASCII)) {
    return "image/svg+xml";
  }
  LOG(ERROR) << "GetMimeType doesn't know mime type for: "
             << filename
             << " text/plain will be returned";
  return "text/plain";
}

void ServerWrapper::OnHttpRequest(int connection_id,
                   const net::HttpServerRequestInfo& info) {
  server_->SetSendBufferSize(connection_id, kSendBufferSizeForWingsWeb);

  if (base::StartsWith(info.path, "/json", base::CompareCase::SENSITIVE)) {
    LOG(INFO) << "unimplemented";
    return;
  }

  if (info.path.empty() || info.path == "/") {
    // Discovery page request.
    LOG(INFO) << "unimplemented";
    return;
  }  

  if (!base::StartsWith(info.path, "/wings/",
                        base::CompareCase::SENSITIVE)) {
    server_->Send404(connection_id);
    return;
  }  

  std::string filename = PathWithoutParams(info.path.substr(7));
  std::string mime_type = GetMimeType(filename);  

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::BindOnce(&WingsWebHttpHandler::OnFrontendResourceRequest,
                     handler_, connection_id, filename));  
}

void ServerWrapper::OnWebSocketRequest(int connection_id,
                        const net::HttpServerRequestInfo& info) {
  LOG(INFO) << "unimplemented";
}

void ServerWrapper::OnWebSocketMessage(int connection_id,
                        const std::string& data) {
  LOG(INFO) << "unimplemented";
}

void ServerWrapper::OnClose(int connection_id) {
  LOG(INFO) << "unimplemented";
}

void TerminateOnUI(std::unique_ptr<base::Thread> thread,
                   std::unique_ptr<ServerWrapper> server_wrapper,
                   std::unique_ptr<WingsWebSocketFactory> socket_factory) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (server_wrapper)
    thread->task_runner()->DeleteSoon(FROM_HERE, std::move(server_wrapper));
  if (socket_factory)
    thread->task_runner()->DeleteSoon(FROM_HERE, std::move(socket_factory));
  if (thread) {
    base::PostTaskWithTraits(
        FROM_HERE, {base::MayBlock(), base::TaskPriority::BACKGROUND},
        BindOnce([](std::unique_ptr<base::Thread>) {}, std::move(thread)));
  }
}

void ServerStartedOnUI(base::WeakPtr<WingsWebHttpHandler> handler,
                       base::Thread* thread,
                       ServerWrapper* server_wrapper,
                       WingsWebSocketFactory* socket_factory,
                       std::unique_ptr<net::IPEndPoint> ip_address) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (handler && thread && server_wrapper) {
    handler->ServerStarted(
        std::unique_ptr<base::Thread>(thread),
        std::unique_ptr<ServerWrapper>(server_wrapper),
        std::unique_ptr<WingsWebSocketFactory>(socket_factory),
        std::move(ip_address));
  } else {
    TerminateOnUI(std::unique_ptr<base::Thread>(thread),
                  std::unique_ptr<ServerWrapper>(server_wrapper),
                  std::unique_ptr<WingsWebSocketFactory>(socket_factory));
  }
}

void StartServerOnHandlerThread(
    base::WeakPtr<WingsWebHttpHandler> handler,
    std::unique_ptr<base::Thread> thread,
    std::unique_ptr<WingsWebSocketFactory> socket_factory) {
  DCHECK(thread->task_runner()->BelongsToCurrentThread());

  std::unique_ptr<ServerWrapper> server_wrapper;
  std::unique_ptr<net::ServerSocket> server_socket =
      socket_factory->CreateForHttpServer();
  std::unique_ptr<net::IPEndPoint> ip_address(new net::IPEndPoint);
  if (server_socket) {
    server_wrapper.reset(new ServerWrapper(handler, std::move(server_socket)));
    if (server_wrapper->GetLocalAddress(ip_address.get()) != net::OK)
      ip_address.reset();
  } else {
    ip_address.reset();
  }  

  if (ip_address) {
    std::string message = base::StringPrintf(
        "\nWings web listening on ws://%s\n", ip_address->ToString().c_str());
    fprintf(stderr, "%s", message.c_str());
    fflush(stderr);
  } else {
    LOG(ERROR) << "Cannot start http server for devtools. Stop devtools.";
  }

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::BindOnce(&ServerStartedOnUI, std::move(handler), thread.release(),
                     server_wrapper.release(), socket_factory.release(),
                     std::move(ip_address)));  
}

WingsWebHttpHandler::WingsWebHttpHandler(content::BrowserContext* browser_context, std::unique_ptr<WingsWebSocketFactory> server_socket_factory)
  : browser_context_(browser_context),
    weak_factory_(this) {
  std::unique_ptr<base::Thread> thread(
      new base::Thread(kWingsWebHandlerThreadName));
  base::Thread::Options options;
  options.message_loop_type = base::MessageLoop::TYPE_IO;
  if (thread->StartWithOptions(options)) {
    base::TaskRunner* task_runner = thread->task_runner().get();
    task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&StartServerOnHandlerThread, weak_factory_.GetWeakPtr(),
                       std::move(thread), std::move(server_socket_factory)));
  }
}

WingsWebHttpHandler::~WingsWebHttpHandler() {
  TerminateOnUI(std::move(thread_), std::move(server_wrapper_),
                std::move(socket_factory_));
}

void WingsWebHttpHandler::ServerStarted(std::unique_ptr<base::Thread> thread,
                   std::unique_ptr<ServerWrapper> server_wrapper,
                   std::unique_ptr<WingsWebSocketFactory> socket_factory,
                   std::unique_ptr<net::IPEndPoint> ip_address) {
  thread_ = std::move(thread);
  server_wrapper_ = std::move(server_wrapper);
  socket_factory_ = std::move(socket_factory);
  server_ip_address_ = std::move(ip_address); 

  Wings* wings = Wings::CreateMainWindow(browser_context_, GURL(), nullptr, gfx::Size());
  wings->LoadURL(GetFrontendURL());
}

void WingsWebHttpHandler::OnFrontendResourceRequest(
    int connection_id, const std::string& path) {
  Send200(connection_id,
          WingsWebFrontendHost::GetFrontendResource(path).as_string(),
          GetMimeType(path));
}

void WingsWebHttpHandler::Send200(int connection_id,
                                  const std::string& data,
                                  const std::string& mime_type) {
  if (!thread_)
    return;
  thread_->task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&ServerWrapper::Send200,
                                base::Unretained(server_wrapper_.get()),
                                connection_id, data, mime_type));
}

} // namespace wings
