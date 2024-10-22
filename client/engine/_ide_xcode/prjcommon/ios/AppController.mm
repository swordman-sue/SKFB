/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>
#import "AppController.h"
#import "CCEAGLView-ios.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "SdkManager.h"
#import "RootViewController.h"
#import "platform/sdktools.h"
#import "platform/systemtools.h"
#import <Bugly/Bugly.h>
#import "platform/ios/AFNetworking/MyAFNetworking.h"

void uncaughtExceptionHandler(NSException*exception)
{
    NSLog(@"CRASH: %@", exception);
    NSLog(@"Stack Trace: %@",[exception callStackSymbols]);
}

@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

static SdkManager *sdkManager = SdkManager::sharedInstance();

// cocos2d application instance
static AppDelegate s_sharedApplication;
@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    //初始化bugly
    NSDictionary *infoPlistDic = [[NSBundle mainBundle] infoDictionary];
    NSString* buglyAPPID = [infoPlistDic objectForKey:@"BuglyAPPID"];
    if (nil != buglyAPPID && not [buglyAPPID isEqualToString:@""])
    {
        [Bugly startWithAppId:buglyAPPID];
    }
    
    // 设置异常回调
    NSSetUncaughtExceptionHandler(&uncaughtExceptionHandler);

    // Override point for customization after application launch.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];

    // Init the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGBA8
                                     depthFormat: GL_DEPTH24_STENCIL8_OES
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples: 0];

    // Use RootViewController manage CCEAGLView 
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = eaglView;
    viewController.view.multipleTouchEnabled = YES;

    // Set RootViewController to window
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:viewController];
    }

    [window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden:YES];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];

    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView(eaglView);
    cocos2d::Director::getInstance()->setOpenGLView(glview);
//没有推送证书时在宏定义中填上这个
#ifndef __NO_PUSH__
    [PushApplication PushInit:self :launchOptions];
#endif

    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk setUiWindow:window];
        [basesdk setViewController:viewController];
        [basesdk SDKInit:application didFinishLaunchingWithOptions:launchOptions];
    }    

    cocos2d::Application::getInstance()->run();

    // 派发didFinishLaunchingWithOptions事件
    [[NSNotificationCenter defaultCenter] postNotificationName: @"didFinishLaunchingWithOptions"
                                          object: nil
                                          userInfo: [NSDictionary dictionaryWithObjectsAndKeys: launchOptions,@"launchOptions",nil]];
    
    // 申请跳转到设置界面的权限
    float epsilon = 1e-6;
    float version = SystemTools::GetIOSVersion();
    if (version - 10.0f > epsilon || fabs(version - 10.0f) <= epsilon)
    {
        UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
        [center requestAuthorizationWithOptions:UNAuthorizationOptionAlert | UNAuthorizationOptionBadge | UNAuthorizationOptionSound completionHandler:^(BOOL granted, NSError * _Nullable error) {
        }];
    }
    else if (version >= 8)
    {
        UIUserNotificationSettings *notificationSettings = [UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert | UIUserNotificationTypeBadge | UIUserNotificationTypeSound categories:nil];
        [[UIApplication sharedApplication] registerUserNotificationSettings:notificationSettings];
    }
    
    // 注册网络状态变化监听器
    [self registNetworkChangeEvent];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
     //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->pause(); */
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk applicationWillResignActive:application];
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
     //We don't need to call this method any more. It will interupt user defined game pause&resume logic
	cocos2d::Application::getInstance()->applicationWillEnterForeground();
    [[NSNotificationCenter defaultCenter] postNotificationName:@"applicationDidBecomeActive"
                                                        object:nil
                                                      userInfo:nil];
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk applicationDidBecomeActive:application];
        
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
#ifndef __ZL_APPSTORE__
	NSLog(@"run in background true");
	UIApplication* app = [UIApplication sharedApplication];
	[app setKeepAliveTimeout:600 handler:^{}];
#endif
    cocos2d::Application::getInstance()->applicationDidEnterBackground();
	[[NSNotificationCenter defaultCenter] postNotificationName:@"applicationDidEnterBackground"
														object:nil
													  userInfo:nil];
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk applicationDidEnterBackground:application];
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"applicationWillEnterForeground"
                                                        object:nil
                                                      userInfo:nil];
    cocos2d::Application::getInstance()->applicationWillEnterForeground();

    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk applicationWillEnterForeground:application];
        
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */

    s_sharedApplication.applicationWillTerminate();
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk applicationWillTerminate:application];
    }
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"handleOpenURL"
                                                        object:nil
                                                      userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:1],@"code", url, @"url", nil]];
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        return [basesdk handleOpenURL:application :url];
    }
    
    return YES;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"handleOpenURLWithSourceApplicationWithAnnotation"
                                                        object:nil
                                                      userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:1],@"code",
                                                                application, @"application",
                                                                url, @"url",
                                                                sourceApplication,@"sourceApplication",nil]];
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        return [basesdk openURL:application :url sourceApplication:sourceApplication annotation:annotation];
        
    }
    
    return YES;
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString*, id> *)options
{
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        return [basesdk openURL:app :url options:options];
    }

    return YES;
}

-(void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"didRegisterForRemoteNotificationsWithDeviceToken"
                                                        object:nil
                                                      userInfo:[NSDictionary dictionaryWithObjectsAndKeys:deviceToken,@"deviceToken", nil]];
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk didRegisterForRemoteNotificationsWithDeviceToken:application :deviceToken];
    }
}

-(void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"didFailToRegisterForRemoteNotificationsWithError"
                                                        object:nil
                                                      userInfo:[NSDictionary dictionaryWithObjectsAndKeys:error,@"error", nil]];
    
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk didFailToRegisterForRemoteNotificationsWithError:application :error];
    }
}

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"supportedInterfaceOrientationsForWindow"
                                                        object:nil
                                                      userInfo: [NSDictionary dictionaryWithObjectsAndKeys:application,@"application",window,@"window",nil]];
   
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        return [basesdk supportedInterfaceOrientationsForWindow:application :window];
    }
    
    return UIInterfaceOrientationMaskAll;
}

- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray * restorableObjects))restorationHandler
{
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        return [basesdk continueUserActivity:application :userActivity restorationHandler:restorationHandler];
    }
    
    return YES;
}

-(void)applicaiton:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification
{
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk didReceiveLocalNotification:application :notification];
    }
}

-(void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler
{
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk didReceiveRemoteNotification:application :userInfo fetchCompletionHandler:completionHandler];
    }
}

-(void)application:(UIApplication *)application didReceiveRemoteNotification:(nonnull NSDictionary *)userInfo
{
    BaseSdk *basesdk = sdkManager->GetSdk();
    if (nil != basesdk)
    {
        [basesdk didReceiveRemoteNotification:application :userInfo];
    }
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
    SdkTools::RunMemoryWarning();
}

#pragma mark - 检测网络状态变化
-(void)registNetworkChangeEvent
{
    self.netWorkStatesCode = MyAFNetworkReachabilityStatusUnknown;
    self.netWorkStatesDetailCode = SystemTools::GetNetWorkState();

    NSURL *url = [NSURL URLWithString:@"https://www.baidu.com"];
    MyAFHTTPSessionManager *manager = [[MyAFHTTPSessionManager alloc] initWithBaseURL:url];
    [manager.reachabilityManager setReachabilityStatusChangeBlock:^(MyAFNetworkReachabilityStatus status) {
        int oldNetworkStatesCode = self.netWorkStatesCode;
        self.netWorkStatesCode = status;
        
        if (oldNetworkStatesCode != self.netWorkStatesCode)
        {
            SystemTools::NetworkStateChangeNotify();
        }
        else if (self.netWorkStatesCode == MyAFNetworkReachabilityStatusReachableViaWWAN)
        {
            int oldNetWorkStatesDetailCode = self.netWorkStatesDetailCode;
            self.netWorkStatesDetailCode = SystemTools::GetNetWorkState();
            if (oldNetWorkStatesDetailCode != self.netWorkStatesDetailCode)
            {
                SystemTools::NetworkStateChangeNotify();
            }
        }
    }];
    [manager.reachabilityManager startMonitoring];
}

- (void)dealloc {
    [window release];
    [super dealloc];
}

@end
