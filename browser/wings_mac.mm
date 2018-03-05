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

gfx::Size window_size;

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
// const CGFloat kMinimumWindowWidth = 400;
// const CGFloat kMinimumWindowHeight = 300;

void MakeWingsButton(NSString* title,
                     NSView* parent,
                     int control,
                     NSView* target,
                     NSString* key,
                     NSUInteger modifier) {
  base::scoped_nsobject<NSButton> button(
      [[NSButton alloc] init]);
  [button setTitle:title];
  [button setBezelStyle:NSSmallSquareBezelStyle];
  [button setAutoresizingMask:(NSViewMaxXMargin | NSViewMinYMargin)];
  [button setTarget:target];
  [button setAction:@selector(performAction:)];
  [button setTag:control];
  [button setKeyEquivalent:key];
  [button setKeyEquivalentModifierMask:modifier];
  [parent addSubview:button];
}

} // namespace

namespace wings {

void Wings::PlatformInitialize(const gfx::Size& default_window_size) {
}

void Wings::PlatformCleanUp() {
}

void Wings::PlatformCreateWindow(int width, int height) {
  // height += kURLBarHeight;
  // NSRect initial_window_bounds = NSMakeRect(0, 0, width, height);
  window_size = Wings::GetWingsDefaultSize();
  NSRect initial_window_bounds = NSMakeRect(0, 0, window_size.width(), window_size.height());  
  NSUInteger style_mask = NSTitledWindowMask |
                          NSClosableWindowMask |
                          NSMiniaturizableWindowMask |
                          NSResizableWindowMask;
  CrWingsWindow* window = [[CrWingsWindow alloc] initWithContentRect:initial_window_bounds
              styleMask:style_mask
                backing:NSBackingStoreBuffered
                  defer:NO];
  window_ = window;
  [window setWings:this];
  [window_ setTitle:kWindowTitle];
  NSView* content = [window_ contentView];

  // // If the window is allowed to get too small, it will wreck the view bindings.
  // NSSize min_size = NSMakeSize(kMinimumWindowWidth, kMinimumWindowHeight);
  // min_size = [content convertSize:min_size toView:nil];
  // // Note that this takes window coordinates.target_display_provider_
  // [window_ setContentMinSize:min_size];

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

  base::scoped_nsobject<NSView> contents_container_view([[NSView alloc] initWithFrame:[content bounds]]);
  [contents_container_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
  [content addSubview:contents_container_view];
  contents_container_view_ = contents_container_view.get();

  base::scoped_nsobject<NSView> previewer_container_view([[NSView alloc] init]);
  [previewer_container_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
  [content addSubview:previewer_container_view];
  previewer_container_view_ = previewer_container_view.get();
  [previewer_container_view_ setNeedsDisplay:YES];
  previewer_container_view_.hidden = YES;

  base::scoped_nsobject<NSView> header_view([[NSView alloc] init]);
  [header_view setAutoresizingMask:(NSViewWidthSizable | NSViewMinYMargin)];
  [previewer_container_view_ addSubview:header_view];

  MakeWingsButton(@"后退", header_view, IDC_NAV_BACK,
                  (NSView*)delegate, @"[", NSCommandKeyMask);
  MakeWingsButton(@"前进", header_view, IDC_NAV_FORWARD,
                  (NSView*)delegate, @"]", NSCommandKeyMask);
  MakeWingsButton(@"刷新", header_view, IDC_NAV_RELOAD,
                  (NSView*)delegate, @"r", NSCommandKeyMask);
  // // MakeWingsButton(&button_frame, @"Stop", content, IDC_NAV_STOP,
  // //                 (NSView*)delegate, @".", NSCommandKeyMask);

  base::scoped_nsobject<NSTextField> url_edit_view(
      [[NSTextField alloc] init]);
  [header_view addSubview:url_edit_view];
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

  [contents_container_view_ addSubview:web_view];

  NSRect frame = [contents_container_view_ bounds];
  [web_view setFrame:frame];
  [web_view setNeedsDisplay:YES];  
}

void Wings::PlatformResizeSubViews() {
  // Not needed; subviews are bound.
}

void Wings::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  int id;
  switch (control) {
    case BACK_BUTTON:
      id = IDC_NAV_BACK;
      break;
    case FORWARD_BUTTON:
      id = IDC_NAV_FORWARD;
      break;
    case STOP_BUTTON:
      id = IDC_NAV_STOP;
      break;
    default:
      NOTREACHED() << "Unknown UI control";
      return;
  }
  [[[window_ contentView] viewWithTag:id] setEnabled:is_enabled];
}

void Wings::PlatformSetAddressBarURL(const GURL& url) {
  NSString* url_string = base::SysUTF8ToNSString(url.spec());
  [url_edit_view_ setStringValue:url_string];
}

void Wings::PlatformSetIsLoading(bool loading) {
  LOG(INFO) << "unimplemented";
}

void Wings::PlatformSetTitle(const base::string16& title) {
  LOG(INFO) << "unimplemented";
}

void Wings::PlatformSetPreviewerContents() {
  NSView* web_view = previewer_web_contents_->GetNativeView();
  [previewer_container_view_ addSubview:web_view];

  [web_view setNeedsDisplay:YES];

  LayoutSubviews();
}

void Wings::LayoutSubviews() {
  NSView* content = [window_ contentView];
  NSRect content_rect = [content bounds];

  NSRect left_frame;
  if (previewer_container_view_.hidden) {
    left_frame = NSMakeRect(0, 0, content_rect.size.width, content_rect.size.height);
  } else {
    left_frame = NSMakeRect(0, 0, content_rect.size.width / 2, content_rect.size.height);
  }
  [contents_container_view_ setFrame:left_frame];
  NSView* subview = [[contents_container_view_ subviews] objectAtIndex:0];
  [subview setFrame:[contents_container_view_ bounds]];

  if (!previewer_container_view_.hidden) {
    NSRect right_frame = NSMakeRect(left_frame.size.width, 0, content_rect.size.width - left_frame.size.width, content_rect.size.height);
    [previewer_container_view_ setFrame:right_frame];

    NSRect bounds = [previewer_container_view_ bounds];
    bounds.origin.y = bounds.size.height - kURLBarHeight;
    bounds.size.height = kURLBarHeight;
    NSView* header_view = [[previewer_container_view_ subviews] objectAtIndex:0];
    [header_view setFrame:bounds];
    NSRect button_frame =
        NSMakeRect(0, 0, kButtonWidth, kURLBarHeight);
    for (int i = 0; i < 3; i++) {
      [[[header_view subviews] objectAtIndex:i] setFrame:button_frame];
      button_frame.origin.x += kButtonWidth;      
    }
    button_frame.size.width = window_size.width() / 2 - button_frame.origin.x;
    [[[header_view subviews] objectAtIndex:3] setFrame:button_frame];

    subview = [[previewer_container_view_ subviews] objectAtIndex:1];
    bounds = [previewer_container_view_ bounds];
    bounds.size.height = bounds.size.height - kURLBarHeight;
    [subview setFrame:bounds];
  }
}

void Wings::OpenPreviewer(std::string& url_string) {
  previewer_container_view_.hidden = NO;
  LayoutSubviews();
  URLEntered(url_string);
}

void Wings::ClosePreviewer() {
  previewer_container_view_.hidden = YES;
  LayoutSubviews();
}

void Wings::ActionPerformed(int control) {
  switch (control) {
    case IDC_NAV_BACK:
      GoBackOrForward(-1);
      break;
    case IDC_NAV_FORWARD:
      GoBackOrForward(1);
      break;
    case IDC_NAV_RELOAD:
      Reload();
      break;
    // case IDC_NAV_STOP:
    //   Stop();
    //   break;
  }
}

void Wings::URLEntered(const std::string& url_string) {
  if (!url_string.empty()) {
    GURL url(url_string);
    if (!url.has_scheme())
      url = GURL("http://" + url_string);
    LoadURLForPreviewer(url);
  }
}

} // namespace wings
