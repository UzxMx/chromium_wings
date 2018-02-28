#ifndef WINGS_BROWSER_WINGS_APPLICATION_MAC_H_
#define WINGS_BROWSER_WINGS_APPLICATION_MAC_H_

#include "base/mac/scoped_sending_event.h"
#include "base/message_loop/message_pump_mac.h"

@interface WingsCrApplication : NSApplication<CrAppProtocol,
                                              CrAppControlProtocol> {
 @private
  BOOL handlingSendEvent_;
}

// CrAppProtocol:
- (BOOL)isHandlingSendEvent;

// CrAppControlProtocol:
- (void)setHandlingSendEvent:(BOOL)handlingSendEvent;

- (IBAction)newDocument:(id)sender;

@end

#endif  // WINGS_BROWSER_WINGS_APPLICATION_MAC_H_
