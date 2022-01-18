#import <AppKit/AppKit.h>

static bool running = true;

static uint8_t* buffer = nil;

@interface WindowDelegate : NSObject <NSWindowDelegate>

@end

@implementation WindowDelegate

- (void)windowWillClose:(id)sender {
    running = false;
}

@end

int main(int argc, const char* argv[]) {
    NSWindow* window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 512, 512)
                  styleMask:
                      NSWindowStyleMaskClosable |
                      NSWindowStyleMaskMiniaturizable |
                      NSWindowStyleMaskResizable |
                      NSWindowStyleMaskTitled
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [window setTitle:@"game"];
    [window center];
    [window makeKeyAndOrderFront:nil];
    [window setDelegate:[WindowDelegate new]];

    size_t lastBufferSize = 0;

    while (running) {
        // draw bitmap
        int bitmapWidth = (int) window.contentView.bounds.size.width;
        int bitmapHeight = (int) window.contentView.bounds.size.height;
        int bytesPerPixel = 4;
        long pitch = bitmapWidth * bytesPerPixel;
        size_t bufferSize = pitch * bitmapHeight;
        if (bufferSize != lastBufferSize) {
            lastBufferSize = bufferSize;
            free(buffer);
            buffer = (uint8_t*) malloc(bufferSize);
        }
        for (size_t i = 0; i < bufferSize; i += 4) {
            buffer[i + 0] = 0xff; // red
            buffer[i + 1] = 0x00; // green
            buffer[i + 2] = 0x33; // blue
            buffer[i + 3] = 0xff; // alpha
        }
        @autoreleasepool {
            NSBitmapImageRep* imageRep = [[[NSBitmapImageRep alloc]
                initWithBitmapDataPlanes:&buffer
                              pixelsWide:bitmapWidth
                              pixelsHigh:bitmapHeight
                           bitsPerSample:8
                         samplesPerPixel:4
                                hasAlpha:YES
                                isPlanar:NO
                          colorSpaceName:NSDeviceRGBColorSpace
                             bytesPerRow:pitch
                            bitsPerPixel:bytesPerPixel * 8
            ] autorelease];
            NSSize imageSize = NSMakeSize(bitmapWidth, bitmapHeight);
            NSImage* image = [[[NSImage alloc]
                initWithSize:imageSize
            ] autorelease];
            [image addRepresentation:imageRep];
            window.contentView.layer.contents = image;
        }

        // handle events
        NSEvent* event;
        do {
            event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                       untilDate:nil
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:YES];
            switch ([event type]) {
                default:
                    [NSApp sendEvent:event];
            }
        } while (event != nil);
    }
}
