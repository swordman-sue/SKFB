#import "PushApplication.h"

@class RootViewController;

@interface AppController : PushApplication {
    UIWindow *window;
    RootViewController *viewController;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, assign) RootViewController *viewController;
@property (nonatomic, assign) int netWorkStatesCode;
@property (nonatomic, assign) int netWorkStatesDetailCode;

@end

