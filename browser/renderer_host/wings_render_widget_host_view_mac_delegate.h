#ifndef WINGS_BROWSER_RENDERER_HOST_WINGS_RENDER_HOST_VIEW_MAC_DELEGATE_H_
#define WINGS_BROWSER_RENDERER_HOST_WINGS_RENDER_HOST_VIEW_MAC_DELEGATE_H_

#import <Cocoa/Cocoa.h>

#include <memory>

#include "base/mac/scoped_nsobject.h"
#import "content/public/browser/render_widget_host_view_mac_delegate.h"

@interface WingsRenderWidgetHostViewMacDelegate
    : NSObject<RenderWidgetHostViewMacDelegate> {
}

@end

#endif
