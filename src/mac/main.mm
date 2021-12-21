#import <Cocoa/Cocoa.h>

int main() {
    [NSAutoreleasePool new];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // main menu
    NSMenu* mainMenu = [[NSMenu new] autorelease];
    NSMenuItem* mainMenuItem = [[NSMenuItem new] autorelease];
    [mainMenu addItem:mainMenuItem];
    [NSApp setMainMenu:mainMenu];

    // app menu
    NSMenu* appMenu = [[NSMenu new] autorelease];
    NSString* appName = @"game";
    NSString* quitTitle = [@"Quit " stringByAppendingString:appName];
    NSMenuItem* quitMenuItem = [
            [[NSMenuItem alloc] initWithTitle:quitTitle
                                       action:@selector(terminate:)
                                keyEquivalent:@"q"]
            autorelease];
    [appMenu addItem:quitMenuItem];
    [mainMenuItem setSubmenu:appMenu];

    NSRect frame = NSMakeRect(0, 0, 512, 512);
    NSWindow* window = [
            [[NSWindow alloc] initWithContentRect:frame
                                        styleMask:NSWindowStyleMaskTitled
                                          backing:NSBackingStoreBuffered
                                            defer:NO]
            autorelease];
    [window cascadeTopLeftFromPoint:NSMakePoint(20, 20)];
    [window setTitle:appName];
    [window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];

    return 0;
}
