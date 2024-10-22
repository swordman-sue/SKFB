//
//  AppDelegate.m
//
//
//

#import "AppDelegate.h"
#import <SMPCQuickSDK/SMPCQuickSDK.h>
#import "MacroHelper.h"
#import "ViewController.h"



@interface AppDelegate () {
    
}

@property (nonatomic, strong) ViewController *viewController;

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    NSLog(@"%@",[[NSBundle mainBundle] bundlePath]);
    // Override point for customization after application launch.
    
    NSString *ibStr = !IS_DEVICE_IPAD ? @"ViewController_iPhone" : @"ViewController_iPad";
    self.viewController = [[ViewController alloc] initWithNibName:ibStr bundle:nil];
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [self.window setRootViewController:_viewController];
    [self.window makeKeyAndVisible];
    //获取自定义参数值
    NSString *customstr = [[SMPCQuickSDK defaultInstance] getConfigValue:@"nihao"];

    //监听初始化完成事件
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(smpcQpInitResult:)
                                                 name:kSmpcQuickSDKNotiInitDidFinished
                                               object:nil];
    //初始化
    SMPCQuickSDKInitConfigure *cfg = [[SMPCQuickSDKInitConfigure alloc] init];
    cfg.productKey = PRODUCT_KEY;
    cfg.productCode = PRODUCT_CODE;
    [[SMPCQuickSDK defaultInstance] initWithConfig:cfg application:application didFinishLaunchingWithOptions:launchOptions];//通常cp调用这个初始化接口
//    [[SMPCQuickSDK defaultInstance] initWithConfig:application didFinishLaunchingWithOptions:launchOptions];//QuickSDK测试使用的初始化，使用该初始化方案时，QuickSDK会使用打包工具上选择的产品对应后台配置的SMPCQuickSDKInitConfigure，所以适合于不调试母包工程直接出渠道包和渠道调试工程的情况。
    return YES;
}
- (void)smpcQpInitResult:(NSNotification *)notify {
    NSLog(@"init result:%@",notify);
    NSDictionary *userInfo = notify.userInfo;
    int errorCode = [userInfo[kSmpcQuickSDKKeyError] intValue];
    switch (errorCode) {
        case SMPC_QUICK_SDK_ERROR_NONE:
        {
            NSLog(@"初始化成功");
        }
            break;
        case SMPC_QUICK_SDK_ERROR_INIT_FAILED:
        default:
        {
            //初始化失败
            NSLog(@"渠道初始化失败");
        }
            break;
    }
    
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    switch (alertView.tag) {
        case 1000: {
            
        }   break;
        default:
            break;
    }
}
- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    NSLog(@"%s",__FUNCTION__);
    [[SMPCQuickSDK defaultInstance] applicationWillResignActive:application];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"%s",__FUNCTION__);
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    [[SMPCQuickSDK defaultInstance] applicationDidEnterBackground:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    [[SMPCQuickSDK defaultInstance] applicationWillEnterForeground:application];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    [[SMPCQuickSDK defaultInstance] applicationDidBecomeActive:application];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    [[SMPCQuickSDK defaultInstance] applicationWillTerminate:application];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    [[SMPCQuickSDK defaultInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    [[SMPCQuickSDK defaultInstance] application:application didFailToRegisterForRemoteNotificationsWithError:error];
}

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(nullable UIWindow *)window
{
    [[SMPCQuickSDK defaultInstance] application:application supportedInterfaceOrientationsForWindow:window];
    return UIInterfaceOrientationMaskAll;
}
- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url{
    [[SMPCQuickSDK defaultInstance] openURL:url application:application];
    return YES;
}
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation{
    [[SMPCQuickSDK defaultInstance] openURL:url sourceApplication:sourceApplication application:application annotation:annotation];
    return YES;
}
- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString*, id> *)options{
    [[SMPCQuickSDK defaultInstance] openURL:url application:app options:options];
    return YES;
}
- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray * restorableObjects))restorationHandler{
    [[SMPCQuickSDK defaultInstance] application:application continueUserActivity:userActivity restorationHandler:restorationHandler];
    return YES;
}
@end
