#import <Cocoa/Cocoa.h>
#import "AppDelegate.mm"

int main(int argc, const char* argv[]) {
    NSApplication* app = [NSApplication sharedApplication];
    AppDelegate* delegate = [[AppDelegate new] autorelease];
    [app setDelegate:delegate];
    return NSApplicationMain(argc, argv);
}
