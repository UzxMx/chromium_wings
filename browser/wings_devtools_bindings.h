#ifndef WINGS_BROWSER_WINGS_DEVTOOLS_BINDINGS_H_
#define WINGS_BROWSER_WINGS_DEVTOOLS_BINDINGS_H_

#include <memory>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "content/public/browser/web_contents_observer.h"
#include "net/url_request/url_fetcher_delegate.h"

#if !defined(OS_ANDROID)
#include "content/public/browser/devtools_frontend_host.h"
#endif

namespace base {
class Value;
}

namespace content {

class NavigationHandle;

class WebContents;
} // namespace content

namespace wings {
class WingsDevToolsDelegate {
 public:
  virtual void Close() = 0;
  virtual ~WingsDevToolsDelegate(){};
};

class WingsDevToolsBindings : public content::WebContentsObserver,
                              public content::DevToolsAgentHostClient,
                              public net::URLFetcherDelegate {
public:
  WingsDevToolsBindings(content::WebContents* devtools_contents,
                        content::WebContents* inspected_contents,
                        WingsDevToolsDelegate* delegate);

  void InspectElementAt(int x, int y);
  virtual void Attach();

  void CallClientFunction(const std::string& function_name,
                          const base::Value* arg1,
                          const base::Value* arg2,
                          const base::Value* arg3);
  ~WingsDevToolsBindings() override;

  content::WebContents* inspected_contents() { return inspected_contents_; }

private:
  // content::DevToolsAgentHostClient implementation.
  void AgentHostClosed(content::DevToolsAgentHost* agent_host) override;
  void DispatchProtocolMessage(content::DevToolsAgentHost* agent_host,
                               const std::string& message) override;

  void HandleMessageFromDevToolsFrontend(const std::string& message);

  // WebContentsObserver overrides
  void ReadyToCommitNavigation(content::NavigationHandle* navigation_handle) override;
  void WebContentsDestroyed() override;

  // net::URLFetcherDelegate overrides.
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  void SendMessageAck(int request_id, const base::Value* arg1);

  content::WebContents* inspected_contents_;
  WingsDevToolsDelegate* delegate_;
  scoped_refptr<content::DevToolsAgentHost> agent_host_;
  int inspect_element_at_x_;
  int inspect_element_at_y_;
#if !defined(OS_ANDROID)
  std::unique_ptr<content::DevToolsFrontendHost> frontend_host_;
#endif
  using PendingRequestsMap = std::map<const net::URLFetcher*, int>;
  PendingRequestsMap pending_requests_;
  base::DictionaryValue preferences_;
  using ExtensionsAPIs = std::map<std::string, std::string>;
  ExtensionsAPIs extensions_api_;
  base::WeakPtrFactory<WingsDevToolsBindings> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(WingsDevToolsBindings);  
};
}

#endif
