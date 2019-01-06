#include "wings/browser/wings_web_manager.h"
#include "wings/browser/wings_web_http_handler.h"
#include "wings/browser/wings_web_socket_factory.h"

#include "base/bind.h"
#include "base/memory/ptr_util.h"
#include "base/message_loop/message_loop.h"
#include "content/browser/devtools/devtools_agent_host_impl.h"
#include "content/browser/devtools/devtools_http_handler.h"
#include "content/browser/devtools/devtools_pipe_handler.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/devtools_socket_factory.h"
#include "net/base/net_errors.h"
#include "net/log/net_log_source.h"
#include "net/socket/tcp_server_socket.h"

namespace wings {

namespace {

const int kBackLog = 10;

base::subtle::Atomic32 g_last_used_port;

class TCPServerSocketFactory : public wings::WingsWebSocketFactory {
 public:
  TCPServerSocketFactory(const std::string& address, uint16_t port)
      : address_(address), port_(port) {}

 private:
  std::unique_ptr<net::ServerSocket> CreateForHttpServer() override {
    std::unique_ptr<net::ServerSocket> socket(
        new net::TCPServerSocket(nullptr, net::NetLogSource()));
    if (socket->ListenWithAddressAndPort(address_, port_, kBackLog) != net::OK)
      return std::unique_ptr<net::ServerSocket>();

    net::IPEndPoint endpoint;
    if (socket->GetLocalAddress(&endpoint) == net::OK)
      base::subtle::NoBarrier_Store(&g_last_used_port, endpoint.port());

    return socket;
  }

  std::string address_;
  uint16_t port_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocketFactory);
};

std::unique_ptr<wings::WingsWebSocketFactory>CreateSocketFactory() {
  // uint16_t port = 0;
  // Speficy the port
  uint16_t port = 37362;
  return std::unique_ptr<wings::WingsWebSocketFactory>(
      new TCPServerSocketFactory("127.0.0.1", port));
}

} // namespace

void WingsWebManager::StartHttpHandler(content::BrowserContext* browser_context) {
  WingsWebManager* manager = WingsWebManager::GetInstance();
  manager->SetHttpHandler(std::make_unique<WingsWebHttpHandler>(browser_context, CreateSocketFactory()));
}

void WingsWebManager::StopHttpHandler() {
  LOG(INFO) << "unimplemented";
}

int WingsWebManager::GetHttpHandlerPort() {
  return base::subtle::NoBarrier_Load(&g_last_used_port);
}

WingsWebManager* WingsWebManager::GetInstance() {
  return base::Singleton<WingsWebManager>::get();
}

WingsWebManager::WingsWebManager() {}

WingsWebManager::~WingsWebManager() {}

void WingsWebManager::SetHttpHandler(std::unique_ptr<WingsWebHttpHandler> http_handler) {
  http_handler_ = std::move(http_handler);
}

} // namespace wings
