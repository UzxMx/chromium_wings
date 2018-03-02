#ifndef WINGS_BROWSER_WINGS_WEB_HTTP_HANDLER_H_
#define WINGS_BROWSER_WINGS_WEB_HTTP_HANDLER_H_

#include <map>
#include <memory>
#include <string>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/devtools_agent_host.h"
#include "net/http/http_status_code.h"

namespace content {

class BrowserContext;

}

namespace base {
class DictionaryValue;
class Thread;
class Value;
}

namespace net {
class IPEndPoint;
class HttpServerRequestInfo;
}

namespace wings {

class WingsWebSocketFactory;
class ServerWrapper;

class WingsWebHttpHandler {
public:
  WingsWebHttpHandler(content::BrowserContext* browser_context, std::unique_ptr<WingsWebSocketFactory> server_socket_factory);
  ~WingsWebHttpHandler();

  friend class ServerWrapper;

  void OnFrontendResourceRequest(int connection_id, const std::string& path);

  void ServerStarted(std::unique_ptr<base::Thread> thread,
                     std::unique_ptr<ServerWrapper> server_wrapper,
                     std::unique_ptr<WingsWebSocketFactory> socket_factory,
                     std::unique_ptr<net::IPEndPoint> ip_address);
  void Send200(int connection_id,
               const std::string& data,
               const std::string& mime_type);  

private:
  content::BrowserContext* browser_context_;
  std::unique_ptr<base::Thread> thread_;
  std::unique_ptr<ServerWrapper> server_wrapper_;
  std::unique_ptr<WingsWebSocketFactory> socket_factory_;
  std::unique_ptr<net::IPEndPoint> server_ip_address_;
  base::WeakPtrFactory<WingsWebHttpHandler> weak_factory_;
};

} // namespace wings

#endif
