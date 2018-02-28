#include "wings/browser/wings_web_contents_view_delegate.h"
#include "wings/browser/wings_web_contents_view_delegate_creator.h"
#include "wings/browser/renderer_host/wings_render_widget_host_view_mac_delegate.h"
#include "wings/browser/wings_devtools_frontend.h"
#include "wings/browser/wings_web_frontend.h"

#import  <Cocoa/Cocoa.h>

#include "base/command_line.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/context_menu_params.h"
#include "third_party/WebKit/public/web/WebContextMenuData.h"

using blink::WebContextMenuData;

enum {
  WingsContextMenuItemCutTag = 0,
  WingsContextMenuItemCopyTag,
  WingsContextMenuItemPasteTag,
  WingsContextMenuItemDeleteTag,
  WingsContextMenuItemOpenLinkTag,
  WingsContextMenuItemBackTag,
  WingsContextMenuItemForwardTag,
  WingsContextMenuItemReloadTag,
  WingsContextMenuItemInspectTag,
  WingsContextMenuItemWingsTag
};

@interface WingsContextMenuDelegate : NSObject<NSMenuDelegate> {
 @private
  wings::WingsWebContentsViewDelegate* delegate_;
}
@end

@implementation WingsContextMenuDelegate

- (id)initWithDelegate:(wings::WingsWebContentsViewDelegate*)delegate {
  if (self = [super init]) {
    delegate_ = delegate;
  }
  return self;
}

- (void)itemSelected:(id)sender {
  NSInteger tag = [sender tag];
  delegate_->ActionPerformed(tag);
}

@end

namespace {

NSMenuItem* MakeContextMenuItem(NSString* title,
                                NSInteger tag,
                                NSMenu* menu,
                                BOOL enabled,
                                WingsContextMenuDelegate* delegate) {
  NSMenuItem* menu_item =
      [[NSMenuItem alloc] initWithTitle:title
                                 action:@selector(itemSelected:)
                          keyEquivalent:@""];
  [menu_item setTarget:delegate];
  [menu_item setTag:tag];
  [menu_item setEnabled:enabled];
  [menu addItem:menu_item];

  return menu_item;
}

}  // namespace

namespace wings {

content::WebContentsViewDelegate* CreateWingsContentsViewDelegate(
  content::WebContents* web_contents) {
  return new WingsWebContentsViewDelegate(web_contents);
}

WingsWebContentsViewDelegate::WingsWebContentsViewDelegate(content::WebContents* web_contents) : web_contents_(web_contents) {
}

WingsWebContentsViewDelegate::~WingsWebContentsViewDelegate() {
}

void WingsWebContentsViewDelegate::ShowContextMenu(content::RenderFrameHost* render_frame_host,
                     const content::ContextMenuParams& params) {
  params_ = params;
  bool has_link = !params_.unfiltered_link_url.is_empty();
  bool has_selection = ! params_.selection_text.empty();

  NSMenu* menu = [[[NSMenu alloc] initWithTitle:@""] autorelease];
  WingsContextMenuDelegate* delegate = [[WingsContextMenuDelegate alloc] initWithDelegate:this];
  [menu setDelegate:delegate];
  [menu setAutoenablesItems:NO];

  if (params.media_type == WebContextMenuData::kMediaTypeNone && !has_link &&
      !has_selection && !params_.is_editable) {
    BOOL back_menu_enabled =
        web_contents_->GetController().CanGoBack() ? YES : NO;
    MakeContextMenuItem(@"Back",
                        WingsContextMenuItemBackTag,
                        menu,
                        back_menu_enabled,
                        delegate);

    BOOL forward_menu_enabled =
        web_contents_->GetController().CanGoForward() ? YES : NO;
    MakeContextMenuItem(@"Forward",
                        WingsContextMenuItemForwardTag,
                        menu,
                        forward_menu_enabled,
                        delegate);

    MakeContextMenuItem(@"Reload",
                        WingsContextMenuItemReloadTag,
                        menu,
                        YES,
                        delegate);

    NSMenuItem* separator = [NSMenuItem separatorItem];
    [menu addItem:separator];
  }

  // TODO

  MakeContextMenuItem(@"Inspect",
                      WingsContextMenuItemInspectTag,
                      menu,
                      YES,
                      delegate);
  MakeContextMenuItem(@"Show Wings",
                      WingsContextMenuItemWingsTag,
                      menu,
                      YES,
                      delegate);  

  NSView* parent_view = web_contents_->GetContentNativeView();
  NSEvent* currentEvent = [NSApp currentEvent];
  NSWindow* window = [parent_view window];
  NSPoint position = [window mouseLocationOutsideOfEventStream];
  NSTimeInterval eventTime = [currentEvent timestamp];
  NSEvent* clickEvent = [NSEvent mouseEventWithType:NSRightMouseDown
                                           location:position
                                      modifierFlags:NSRightMouseDownMask
                                          timestamp:eventTime
                                       windowNumber:[window windowNumber]
                                            context:nil
                                        eventNumber:0
                                         clickCount:1
                                           pressure:1.0];

  [NSMenu popUpContextMenu:menu
                 withEvent:clickEvent
                   forView:parent_view];
}

void WingsWebContentsViewDelegate::ActionPerformed(int tag) {
  switch (tag) {
    case WingsContextMenuItemCutTag:
      web_contents_->Cut();
      break;
    case WingsContextMenuItemCopyTag:
      web_contents_->Copy();
      break;
    case WingsContextMenuItemPasteTag:
      web_contents_->Paste();
      break;
    case WingsContextMenuItemDeleteTag:
      web_contents_->Delete();
      break;
    case WingsContextMenuItemOpenLinkTag: {
      LOG(INFO) << "unimplemented";
      // ShellBrowserContext* browser_context =
      //     ShellContentBrowserClient::Get()->browser_context();
      // Shell::CreateNewWindow(browser_context,
      //                        params_.link_url,
      //                        NULL,
      //                        gfx::Size());
      break;
    }
    case WingsContextMenuItemBackTag:
      web_contents_->GetController().GoToOffset(-1);
      web_contents_->Focus();
      break;
    case WingsContextMenuItemForwardTag:
      web_contents_->GetController().GoToOffset(1);
      web_contents_->Focus();
      break;
    case WingsContextMenuItemReloadTag: {
      web_contents_->GetController().Reload(content::ReloadType::NORMAL, false);
      web_contents_->Focus();
      break;
    }
    case WingsContextMenuItemInspectTag: {
      WingsDevToolsFrontend::Show(web_contents_);
      break;
    }
    case WingsContextMenuItemWingsTag: {
      WingsWebFrontend::Show(web_contents_);
      break;
    }
  }  
}

NSObject<RenderWidgetHostViewMacDelegate>* WingsWebContentsViewDelegate::CreateRenderWidgetHostViewDelegate(
    content::RenderWidgetHost* render_widget_host,
    bool is_popup) {
  return [[WingsRenderWidgetHostViewMacDelegate alloc] init];
}

} // namespace wings
