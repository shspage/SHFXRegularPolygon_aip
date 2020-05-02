
#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_osx.h"
#include "../imgui/examples/imgui_impl_opengl2.h"
#include <stdio.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#include "../Source/myImGuiDialog.h"
#include "../Source/myDialogConfig.h"
#import "MyWindowController.h"

// --------
@interface MyObjCBase : NSObject
+ (int) runModal:(MyParms*)parms completion:(std::function<void(void)>)callbackFunc;
@end

@implementation MyObjCBase
+ (int) runModal:(MyParms*)parms completion:(std::function<void(void)>)callbackFunc
{
    int result = 0;
    
    NSRect frame = NSMakeRect(0, 0, kMyDialogWidth, kMyDialogHeight);
    
    //append "| NSWindowStyleMaskResizable" if it needs
    NSWindow* window = [[NSWindow alloc] initWithContentRect:frame
                                         styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable
                                         backing:NSBackingStoreBuffered
                                         defer:YES];
    MyWindowController* dialog = [[MyWindowController alloc] initWithWindow:window];

    if(dialog){
        result = [dialog runModal:parms completion:callbackFunc];
        [dialog releaseDialog];
        dialog = nil;
    }
    return result;
}
@end

// ----------
int myImGuiDialog::runModal(MyParms* parms, std::function<void(void)> callbackFunc)
{
    return [MyObjCBase runModal:parms completion:callbackFunc];
}
