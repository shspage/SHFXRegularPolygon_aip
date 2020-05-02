
#ifndef MyWindowController_h
#define MyWindowController_h

#import <Cocoa/Cocoa.h>
#include <functional>
#include "../Source/MyParms.h"

@interface MyWindowController : NSWindowController{
}
- (instancetype)initWithWindow:(NSWindow*)window;
- (int) runModal:(MyParms*)parms completion:(std::function<void(void)>)callbackFunc;
- (void) releaseDialog;
@end

#endif /* MyWindowController_h */
