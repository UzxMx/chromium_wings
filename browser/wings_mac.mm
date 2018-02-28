#include "wings/browser/wings.h"
#include "wings/app/resource.h"

#include <algorithm>

#include "base/logging.h"
#import "base/mac/foundation_util.h"
#import "base/mac/scoped_nsobject.h"
#include "base/mac/sdk_forward_declarations.h"
#include "base/strings/string_piece.h"
#include "base/strings/sys_string_conversions.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/web_contents.h"
#import "ui/base/cocoa/underlay_opengl_hosting_window.h"
#include "url/gurl.h"

// Receives notification that the window is closing so that it can start the
// tear-down process. Is responsible for deleting itself when done.
@interface WingsWindowDelegate : NSObject<NSWindowDelegate> {
@private
  wings::Wings* wings_;
}
- (id)initWithWings:(wings::Wings*)wings;
@end

@implementation WingsWindowDelegate

- (id)initWithWings:(wings::Wings*)wings {
  if (self = [super init]) {
    wings_ = wings;
  }
  return self;
}

- (BOOL)windowShouldClose:(id)sender {
  NSWindow* window = base::mac::ObjCCastStrict<NSWindow>(sender);
  [window autorelease];
  // Don't leave a dangling pointer if the window lives beyond
  // this method. See crbug.com/719830.
  [window setDelegate:nil];
  delete wings_;
  [self release];

  return YES;
}

- (void)performAction:(id)sender {
  wings_->ActionPerformed([sender tag]);
}

- (void)takeURLStringValueFrom:(id)sender {
  wings_->URLEntered(base::SysNSStringToUTF8([sender stringValue]));
}

@end

@interface CrWingsWindow : UnderlayOpenGLHostingWindow {
 @private
  wings::Wings* wings_;
}
- (void)setWings:(wings::Wings*)wings;
- (void)showDevTools:(id)sender;
@end

@implementation CrWingsWindow

- (void)setWings:(wings::Wings*)wings {
  wings_ = wings;
}

- (void)showDevTools:(id)sender {
  wings_->ShowDevTools();
}

@end

namespace {

NSString* kWindowTitle = @"Wings";

// Layout constants (in view coordinates)
const CGFloat kButtonWidth = 72;
const CGFloat kURLBarHeight = 24;

// The minimum size of the window's content (in view coordinates)
const CGFloat kMinimumWindowWidth = 400;
const CGFloat kMinimumWindowHeight = 300;

void MakeWingsButton(NSRect* rect,
                     NSString* title,
                     NSView* parent,
                     int control,
                     NSView* target,
                     NSString* key,
                     NSUInteger modifier) {
  base::scoped_nsobject<NSButton> button(
      [[NSButton alloc] initWithFrame:*rect]);
  [button setTitle:title];
  [button setBezelStyle:NSSmallSquareBezelStyle];
  [button setAutoresizingMask:(NSViewMaxXMargin | NSViewMinYMargin)];
  [button setTarget:target];
  [button setAction:@selector(performAction:)];
  [button setTag:control];
  [button setKeyEquivalent:key];
  [button setKeyEquivalentModifierMask:modifier];
  [parent addSubview:button];
  rect->origin.x += kButtonWidth;
}

} // namespace

namespace wings {

void Wings::PlatformInitialize(const gfx::Size& default_window_size) {
}

void Wings::PlatformCleanUp() {
}

void Wings::PlatformCreateWindow(int width, int height) {
  height += kURLBarHeight;
  NSRect initial_window_bounds = NSMakeRect(0, 0, width, height);
  NSRect content_rect = initial_window_bounds;
  NSUInteger style_mask = NSTitledWindowMask |
                          NSClosableWindowMask |
                          NSMiniaturizableWindowMask |
                          NSResizableWindowMask;
  CrWingsWindow* window = [[CrWingsWindow alloc] initWithContentRect:content_rect
              styleMask:style_mask
                backing:NSBackingStoreBuffered
                  defer:NO];
  window_ = window;
  [window setWings:this];
  [window_ setTitle:kWindowTitle];
  NSView* content = [window_ contentView];

  // If the window is allowed to get too small, it will wreck the view bindings.
  NSSize min_size = NSMakeSize(kMinimumWindowWidth, kMinimumWindowHeight);
  min_size = [content convertSize:min_size toView:nil];
  // Note that this takes window coordinates.
  [window_ setContentMinSize:min_size]; 

  // Set the wings window to participate in Lion Fullscreen mode. Set
  // Setting this flag has no effect on Snow Leopard or earlier.
  NSUInteger collectionBehavior = [window_ collectionBehavior];
  collectionBehavior |= NSWindowCollectionBehaviorFullScreenPrimary;
  [window_ setCollectionBehavior:collectionBehavior];  

  // Rely on the window delegate to clean us up rather than immediately
  // releasing when the window gets closed. We use the delegate to do
  // everything from the autorelease pool so the shell isn't on the stack
  // during cleanup (ie, a window close from javascript).
  [window_ setReleasedWhenClosed:NO]; 

  WingsWindowDelegate* delegate = [[WingsWindowDelegate alloc] initWithWings:this];
  [window_ setDelegate:delegate];

  NSRect button_frame =
      NSMakeRect(0, NSMaxY(initial_window_bounds) - kURLBarHeight,
                 kButtonWidth, kURLBarHeight);
  MakeWingsButton(&button_frame, @"Back", content, IDC_NAV_BACK,
                  (NSView*)delegate, @"[", NSCommandKeyMask);
  MakeWingsButton(&button_frame, @"Forward", content, IDC_NAV_FORWARD,
                  (NSView*)delegate, @"]", NSCommandKeyMask);
  MakeWingsButton(&button_frame, @"Reload", content, IDC_NAV_RELOAD,
                  (NSView*)delegate, @"r", NSCommandKeyMask);
  MakeWingsButton(&button_frame, @"Stop", content, IDC_NAV_STOP,
                  (NSView*)delegate, @".", NSCommandKeyMask);

  button_frame.size.width =
      NSWidth(initial_window_bounds) - NSMinX(button_frame);
  base::scoped_nsobject<NSTextField> url_edit_view(
      [[NSTextField alloc] initWithFrame:button_frame]);
  [content addSubview:url_edit_view];
  [url_edit_view setAutoresizingMask:(NSViewWidthSizable | NSViewMinYMargin)];
  [url_edit_view setTarget:delegate];
  [url_edit_view setAction:@selector(takeURLStringValueFrom:)];
  [[url_edit_view cell] setWraps:NO];
  [[url_edit_view cell] setScrollable:YES];
  url_edit_view_ = url_edit_view.get();                                    
  [window_ makeKeyAndOrderFront:nil];
}

void Wings::PlatformSetContents() {
  NSView* web_view = web_contents_->GetNativeView();
  [web_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];

  NSView* content = [window_ contentView];
  [content addSubview:web_view];

  NSRect frame = [content bounds];
  frame.size.height -= kURLBarHeight;
  [web_view setFrame:frame];
  [web_view setNeedsDisplay:YES];  
}

void Wings::PlatformResizeSubViews() {
  // Not needed; subviews are bound.
}

void Wings::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  LOG(INFO) << "unimplemented";
}

void Wings::PlatformSetAddressBarURL(const GURL& url) {
  LOG(INFO) << "unimplemented"; 
}

void Wings::PlatformSetIsLoading(bool loading) {
  LOG(INFO) << "unimplemented";
}

void Wings::PlatformSetTitle(const base::string16& title) {
  LOG(INFO) << "unimplemented";
}

void Wings::ActionPerformed(int control) {
  LOG(INFO) << "unimplemented";
}

void Wings::URLEntered(const std::string& url_string) {
  if (!url_string.empty()) {
    GURL url(url_string);
    if (!url.has_scheme())
      url = GURL("http://" + url_string);
    LoadURL(url);
  }
}

} // namespace wings
