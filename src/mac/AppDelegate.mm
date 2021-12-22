#import "AppDelegate.h"
#import "GameViewController.mm"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    const NSRect frame = NSMakeRect(0, 0, 512, 512);
    self.window = [[NSWindow alloc]
            initWithContentRect:frame
                      styleMask:
                              NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable |
                              NSWindowStyleMaskResizable |
                              NSWindowStyleMaskTitled
                        backing:NSBackingStoreBuffered
                          defer:NO];
    GameViewController* controller = [[GameViewController alloc] init:frame];
    [self.window setContentViewController:controller];
    self.window.title = @"game";
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
}

- (void)applicationWillTerminate:(NSNotification*)notification {
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

@end
