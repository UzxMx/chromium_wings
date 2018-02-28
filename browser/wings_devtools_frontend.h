#ifndef WINGS_BROWSER_WINGS_DEVTOOLS_FRONTEND_H_H
#define WINGS_BROWSER_WINGS_DEVTOOLS_FRONTEND_H_H

#include <memory>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/public/browser/web_contents_observer.h"
#include "wings/browser/wings_devtools_bindings.h"

namespace content {
class WebContents;
}

namespace wings {

class Wings;

class WingsDevToolsFrontend : public WingsDevToolsDelegate,
                              public content::WebContentsObserver {
 public:
  static WingsDevToolsFrontend* Show(content::WebContents* inspected_contents);

  void Activate();
  void Focus();
  void InspectElementAt(int x, int y);
  void Close() override;

  Wings* frontend_wings() const { return frontend_wings_; }

 private:
  // WebContentsObserver overrides
  void DocumentAvailableInMainFrame() override;
  void WebContentsDestroyed() override;

  WingsDevToolsFrontend(Wings* frontend_wings, content::WebContents* inspected_contents);
  ~WingsDevToolsFrontend() override;
  Wings* frontend_wings_;
  std::unique_ptr<WingsDevToolsBindings> devtools_bindings_;

  DISALLOW_COPY_AND_ASSIGN(WingsDevToolsFrontend);
};

} // namespace wings

#endif
