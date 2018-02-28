#ifndef WINGS_BROWSER_WINGS_DEVTOOLS_DELEGATE_H_H
#define WINGS_BROWSER_WINGS_DEVTOOLS_DELEGATE_H_H

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/public/browser/devtools_manager_delegate.h"

namespace content {

class BrowserContext;

}

namespace wings {

class WingsDevToolsManagerDelegate : public content::DevToolsManagerDelegate {
 public:
  static void StartHttpHandler(content::BrowserContext* browser_context);
  static void StopHttpHandler();
  static int GetHttpHandlerPort();

  explicit WingsDevToolsManagerDelegate(content::BrowserContext* browser_context);
  ~WingsDevToolsManagerDelegate() override;

  // DevToolsManagerDelegate implementation.
  scoped_refptr<content::DevToolsAgentHost> CreateNewTarget(const GURL& url) override;
  std::string GetDiscoveryPageHTML() override;
  std::string GetFrontendResource(const std::string& path) override;

 private:
  content::BrowserContext* browser_context_;
  DISALLOW_COPY_AND_ASSIGN(WingsDevToolsManagerDelegate);
};  

} // namespace wings

#endif
