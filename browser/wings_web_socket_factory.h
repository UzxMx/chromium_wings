#ifndef WINGS_BROWSER_WINGS_WEB_SOCKET_FACTORY_H_
#define WINGS_BROWSER_WINGS_WEB_SOCKET_FACTORY_H_

#include "net/socket/server_socket.h"

namespace wings {

class WingsWebSocketFactory {
public:
  virtual ~WingsWebSocketFactory() {}

  virtual std::unique_ptr<net::ServerSocket> CreateForHttpServer() = 0;
};

} // namespace wings

#endif
