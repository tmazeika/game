#import "AppDelegate.h"
#import "GameViewController.mm"

@implementation AppDelegate

- (void)applicationWillTerminate:(NSNotification*)notification {
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

@end
