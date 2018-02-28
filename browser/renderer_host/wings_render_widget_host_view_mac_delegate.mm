#import "wings/browser/renderer_host/wings_render_widget_host_view_mac_delegate.h"

#include "base/command_line.h"

@interface WingsRenderWidgetHostViewMacDelegate () {
  BOOL drop_events_;
}
@end

@implementation WingsRenderWidgetHostViewMacDelegate

- (id)init {
  if ((self = [super init])) {
    drop_events_ = false;
  }
  return self;
}
- (BOOL)handleEvent:(NSEvent*)event {
  return drop_events_;
}

- (void)beginGestureWithEvent:(NSEvent*)event {
}

- (void)endGestureWithEvent:(NSEvent*)event {
}

- (void)touchesMovedWithEvent:(NSEvent*)event {
}

- (void)touchesBeganWithEvent:(NSEvent*)event {
}

- (void)touchesCancelledWithEvent:(NSEvent*)event {
}

- (void)touchesEndedWithEvent:(NSEvent*)event {
}

- (void)rendererHandledWheelEvent:(const blink::WebMouseWheelEvent&)event
                         consumed:(BOOL)consumed {
}

- (void)rendererHandledGestureScrollEvent:(const blink::WebGestureEvent&)event
                                 consumed:(BOOL)consumed {
}

- (void)rendererHandledOverscrollEvent:(const ui::DidOverscrollParams&)params {
}

@end
