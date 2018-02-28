#include "wings/browser/wings_browser_main_parts.h"
#include "wings/browser/wings_application_mac.h"

#import <Cocoa/Cocoa.h>

#include "base/mac/bundle_locations.h"
#include "base/mac/scoped_nsobject.h"
#include "base/mac/sdk_forward_declarations.h"

namespace wings {

void WingsBrowserMainParts::PreMainMessageLoopStart() {
  // Force the NSApplication subclass to be used.
  [WingsCrApplication sharedApplication];

  // base::scoped_nsobject<NSNib> nib(
  //     [[NSNib alloc] initWithNibNamed:@"MainMenu"
  //                              bundle:base::mac::FrameworkBundle()]);
  // NSArray* top_level_objects = nil;
  // [nib instantiateWithOwner:NSApp topLevelObjects:nil];
  // for (NSObject* object : top_level_objects)
  //   [object retain];
}

}  // namespace content
