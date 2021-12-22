#import "GameViewController.h"
#import "GameView.mm"

@implementation GameViewController

- (instancetype)init:(NSRect)frame {
    if (self = [super init]) {
        _frame = frame;
    }
    return self;
}

- (void)loadView {
    self.view = [[GameView alloc] initWithFrame:_frame];
}

@end
