#import "GameView.h"

@implementation GameView

- (void)drawRect:(NSRect)dirtyRect {
    NSSize size = self.window.frame.size;
    NSBitmapImageRep* imageRep = [[[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes:nil
                          pixelsWide:(long) size.width
                          pixelsHigh:(long) size.height
                       bitsPerSample:8
                     samplesPerPixel:4
                            hasAlpha:YES
                            isPlanar:NO
                      colorSpaceName:NSCalibratedRGBColorSpace
                        bitmapFormat:(NSBitmapFormat) 0
                         bytesPerRow:(long) (4 * size.width)
                        bitsPerPixel:32
    ] autorelease];

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext
            graphicsContextWithBitmapImageRep:imageRep]];

    unsigned char* data = [imageRep bitmapData];
    for (int i = 0; i < (long) size.width * (long) size.height * 4; i += 4) {
        data[i + 0] = 0x00; // red
        data[i + 1] = 0x00; // green
        data[i + 2] = i % 0xff; // blue
        data[i + 3] = 0xff; // alpha
    }

    [NSGraphicsContext restoreGraphicsState];

    NSImage* image = [[[NSImage alloc] initWithSize:size] autorelease];
    [image addRepresentation:imageRep];
    [image drawAtPoint:NSMakePoint(0, 0)
              fromRect:NSMakeRect(0, 0, size.width, size.height)
             operation:NSCompositingOperationSourceOver
              fraction:1];
}

@end
