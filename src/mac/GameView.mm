#import "GameView.h"

@implementation GameView

- (void)drawRect:(NSRect)dirtyRect {
    [NSGraphicsContext saveGraphicsState];
    NSRect rect = NSMakeRect(100, 100, 100, 100);
    [[NSColor systemGreenColor] setFill];
    NSFrameRect(rect);
    [NSGraphicsContext restoreGraphicsState];
}

@end
