#ifndef WINGS_WEB_CONTENTS_VIEW_DELEGATE_H_
#define WINGS_WEB_CONTENTS_VIEW_DELEGATE_H_

#include <memory>

#include "base/macros.h"
#include "build/build_config.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view_delegate.h"
#include "content/public/common/context_menu_params.h"

namespace ui {
class SimpleMenuModel;
}

namespace views {
class MenuRunner;
}

namespace wings {

class WingsWebContentsViewDelegate : public content::WebContentsViewDelegate {
 public:
  explicit WingsWebContentsViewDelegate(content::WebContents* web_contents);
  ~WingsWebContentsViewDelegate() override;

  // Overridden from WebContentsViewDelegate:
  void ShowContextMenu(content::RenderFrameHost* render_frame_host,
                       const content::ContextMenuParams& params) override;

#if defined(OS_MACOSX)
  void ActionPerformed(int id);
  NSObject<RenderWidgetHostViewMacDelegate>* CreateRenderWidgetHostViewDelegate(
      content::RenderWidgetHost* render_widget_host,
      bool is_popup) override;
#endif

 private:
  content::WebContents* web_contents_;
#if defined(OS_MACOSX)
  content::ContextMenuParams params_;
#endif

#if defined(TOOLKIT_VIEWS)
  std::unique_ptr<ui::SimpleMenuModel> context_menu_model_;
  std::unique_ptr<views::MenuRunner> context_menu_runner_;
#endif

  DISALLOW_COPY_AND_ASSIGN(WingsWebContentsViewDelegate);
};

}  // namespace wings

#endif
