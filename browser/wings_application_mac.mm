#include "wings/browser/wings_application_mac.h"

#include "base/auto_reset.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"

@implementation WingsCrApplication

- (BOOL)isHandlingSendEvent {
  return handlingSendEvent_;
}

- (void)sendEvent:(NSEvent*)event {
  base::AutoReset<BOOL> scoper(&handlingSendEvent_, YES);
  [super sendEvent:event];
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent {
  handlingSendEvent_ = handlingSendEvent;
}

- (IBAction)newDocument:(id)sender {
  printf("newDocument\n");
  // content::ShellBrowserContext* browserContext =
  //     content::ShellContentBrowserClient::Get()->browser_context();
  // content::Shell::CreateNewWindow(browserContext,
  //                                 GURL(url::kAboutBlankURL),
  //                                 NULL,
  //                                 gfx::Size());
}

@end
