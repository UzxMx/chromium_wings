#ifndef WINGS_BROWSER_WINGS_WEB_MANAGER_H_
#define WINGS_BROWSER_WINGS_WEB_MANAGER_H_

#include <memory>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/singleton.h"
#include "content/common/content_export.h"

namespace content {

class BrowserContext;

}

namespace wings {

class WingsWebHttpHandler;

class WingsWebManager {
public:
  static WingsWebManager* GetInstance();
  static void StartHttpHandler(content::BrowserContext* browser_context);
  static void StopHttpHandler();
  static int GetHttpHandlerPort();  

  WingsWebManager();
  virtual ~WingsWebManager();

  void SetHttpHandler(std::unique_ptr<WingsWebHttpHandler> http_handler);

private:
  std::unique_ptr<WingsWebHttpHandler> http_handler_;
};

} // namespace wings

#endif
