#ifndef WINGS_BROWSER_WINGS_WEB_FRONTEND_H_
#define WINGS_BROWSER_WINGS_WEB_FRONTEND_H_

#include <memory>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {
class WebContents;
}

namespace wings {

class Wings;
class WingsWebBindings;

class WingsWebFrontend : public content::WebContentsObserver {
 public:
  static WingsWebFrontend* Show(content::WebContents* inspected_contents);

  void Activate();
  void Focus();
  void InspectElementAt(int x, int y);
  void Close();

  Wings* frontend_wings() const { return frontend_wings_; }

 private:
  // WebContentsObserver overrides
  void DocumentAvailableInMainFrame() override;
  void WebContentsDestroyed() override;

  WingsWebFrontend(Wings* frontend_wings, content::WebContents* inspected_contents);
  ~WingsWebFrontend() override;
  Wings* frontend_wings_;
  std::unique_ptr<WingsWebBindings> bindings_;

  DISALLOW_COPY_AND_ASSIGN(WingsWebFrontend);
};

} // namespace wings

#endif
