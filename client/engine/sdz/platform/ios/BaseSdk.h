//
//  BaseSdk.h
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//

#import <Foundation/Foundation.h>

@class UIWindow;
@class RootViewController;

//相关回调码
//成功
static int SDK_SUCCESS_CODE = 0;
//失败
static int SDK_FAILURE_CODE = 1;
//取消
static int SDK_CANCEL_CODE = 2;
//登出
static int SDK_LOGOUT_CODE = 3;
//充值成功
static int SDK_RECHARGE_CODE = 4;
//切换账号回调
static int SDK_SWITCH_ACCOUNT_CODE = 5;
//调用QK操作函数成功
static int SDK_COMMON_CALLFUNC_SUCCESS = 6;
//调用QK操作函数失败
static int SDK_COMMON_CALLFUNC_FAILED = 7;

//SDKCommonHandle中的操作类型
typedef enum {
    // 创角
    CT_CREATE_ROLE = 1,
    // 进入游戏
    CT_ENTER_GAME = 2,
    // 角色升级
    CT_LEVEL_UP = 3,
    // 调用操作函数
    CT_CALL_FUNC = 4,
    
}SdkCommonHandleType;

//SDKCommonHandle中的操作函数类型(字符串)
static NSString* FuncType_UNDEFINED = @"FuncType_UNDEFINED";
static NSString* FuncType_ENTER_BBS = @"FuncType_ENTER_BBS";
static NSString* FuncType_ENTER_USER_CENTER = @"FuncType_ENTER_USER_CENTER";
static NSString* FuncType_SHOW_TOOLBAR = @"FuncType_SHOW_TOOLBAR";
static NSString* FuncType_HIDE_TOOLBAR = @"FuncType_HIDE_TOOLBAR";
static NSString* FuncType_Customer_Service = @"FuncType_Customer_Service";
static NSString* FuncType_SWITCH_ACCOUNT = @"FuncType_SWITCH_ACCOUNT";
static NSString* FuncType_REAL_NAME_REGISTER = @"FuncType_REAL_NAME_REGISTER";
static NSString* FuncType_ANTI_ADDICTION_QUERY = @"FuncType_ANTI_ADDICTION_QUERY";
static NSString* FuncType_SHARE = @"FuncType_SHARE";
static NSString* FuncType_NOTICE = @"FuncType_NOTICE";
static NSString* FuncType_BIND_ACCOUNT = @"FuncType_BIND_ACCOUNT";
static NSString* FuncType_FB_GET_INVITABLE_FRIENDS = @"FuncType_FB_GET_INVITABLE_FRIENDS";
static NSString* FuncType_FB_GET_INACTIVE_FRIENDS = @"FuncType_FB_GET_INACTIVE_FRIENDS";
static NSString* FuncType_FB_INVITE_FRIENDS = @"FuncType_FB_INVITE_FRIENDS";
static NSString* FuncType_FB_RECALL_FRIENDS = @"FuncType_FB_RECALL_FRIENDS";
static NSString* FuncType_ADJUST_EVENT_TRACK = @"FuncType_ADJUST_EVENT_TRACK";
static NSString* FuncType_CUSTOM_EVENT_TRACK = @"FuncType_CUSTOM_EVENT_TRACK";
static NSString* FuncType_PICTRUE_EVENT_OPEN = @"FuncType_PICTRUE_EVENT_OPEN";
static NSString* FuncType_PICTRUE_EVENT_SEND = @"FuncType_PICTRUE_EVENT_SEND";

static NSString* FuncType_UPLOAD_ADVERTISING  = @"FuncType_UPLOAD_ADVERTISING"; /*加载广告*/
static NSString* FuncType_PLAY_ADVERTISING  = @"FuncType_PLAY_ADVERTISING"; /*播放广告*/
static NSString* FuncType_END_ADVERTISING  = @"FuncType_END_ADVERTISING"; /*播放广告*/


//SDKCommonHandle中的操作函数类型
typedef enum {
    EFuncType_UNDEFINED = 0,
    EFuncType_ENTER_BBS,
    EFuncType_ENTER_USER_CENTER,
    EFuncType_SHOW_TOOLBAR,
    EFuncType_HIDE_TOOLBAR,
    EFuncType_Customer_Service,
    EFuncType_SWITCH_ACCOUNT,
    EFuncType_REAL_NAME_REGISTER,
    EFuncType_ANTI_ADDICTION_QUERY,
    EFuncType_SHARE,
    EFuncType_NOTICE,
    EFuncType_BIND_ACCOUNT,
    EFuncType_FB_GET_INVITABLE_FRIENDS,
    EFuncType_FB_INVITE_FRIENDS,
    EFuncType_FB_GET_INACTIVE_FRIENDS,
    EFuncType_FB_RECALL_FRIENDS,
    EFuncType_ADJUST_EVENT_TRACK,
    EFuncType_CUSTOM_EVENT_TRACK,
    EFuncType_PICTRUE_EVENT_OPEN,
    EFuncType_PICTRUE_EVENT_SEND,
    
    EFuncType_UPLOAD_ADVERTISING,
    EFuncType_PLAY_ADVERTISING,
    EFuncType_END_ADVERTISING,
    
}SDKCommonHandleFuncType;

//SDKKeyDataHandle中的操作类型
typedef enum {
    // 角色信息
    KDT_USER_INFO = 1,
    // 创角
    KDT_CREATE_ROLE = 2,
    // 登陆
    KDT_LOGIN_ROLE = 3,
    // 注销
    KDT_LOGOUT_ROLE = 4,
    // 角色等级变化
    KDT_LEVEL_UP = 5,
    // vip等级变化
    KDT_VIP_LEVEL_UP = 6,
    // 虚拟货币变化
    KDT_VIRTUAL_CURRENCY_CHANGE = 7,
    
}SdkKeyDataHandleType;

@interface BaseSdk : NSObject
{
    NSMutableDictionary * properties;
}

@property (nonatomic, assign) UIWindow *uiWindow;
@property (nonatomic, assign) RootViewController *viewController;

//重写构造函数
-(id) init;
-(void)dealloc;

-(void) SDKInit:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
+(void) SDKInitFinishCallBack:(int) code;

-(void) SDKLogin:(NSString *)info;
+(void) SdKLoginPannelCallBack:(int) code :(NSString *) data;
+(void) SdKLoginPannelCallBack:(int) code;

-(void) SDKLogout:(NSString *)info;

-(void) SDKSwitchAccount:(NSString *)info;

-(void) SDKTD:(NSString *) info;
+(void) SDKTDFinishCallBack:(int) code :(NSString *) data;
+(void) SDKTDFinishCallBack:(int) code;

-(void) SDKCommonHandle:(NSString *)info;
+(void) SDKCommonHandleCallBack:(int) code :(NSString *) data;
+(void) SDKCommonHandleCallBack:(int) code;

-(void) SDKKeyDataHandle:(NSString *)info;

-(void) SDKToUserCenter:(NSString *)info;

-(void) SDKCheckUpdate:(NSString *)url;

-(void) SDKExitApp:(NSString *) info;

-(id)   GetPropertie:(NSString *) key;
-(void) SetPropertie:(NSString *) key :(id) value;

-(void)applicationWillResignActive:(UIApplication *)application;

-(void)applicationDidEnterBackground:(UIApplication *)application;

-(void)applicationWillEnterForeground:(UIApplication *)application;

-(void)applicationDidBecomeActive:(UIApplication *)application;

-(void)applicationWillTerminate:(UIApplication *)application;

-(void)didRegisterForRemoteNotificationsWithDeviceToken:(UIApplication *)application :(NSData *)deviceToken;

-(void)didFailToRegisterForRemoteNotificationsWithError:(UIApplication *)application :(NSError *)error;

-(UIInterfaceOrientationMask)supportedInterfaceOrientationsForWindow:(UIApplication *)application :(UIWindow *)window;

-(BOOL)handleOpenURL:(UIApplication *)application :(NSURL *)url;

-(BOOL)openURL:(UIApplication *)application :(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation;

-(BOOL)openURL:(UIApplication *)app :(NSURL *)url options:(NSDictionary<NSString*, id> *)options;

-(BOOL)continueUserActivity:(UIApplication *)application :(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray * restorableObjects))restorationHandler;

-(void)didReceiveLocalNotification:(UIApplication *)application :(UILocalNotification *)notification;

-(void)didReceiveRemoteNotification:(UIApplication *)application :(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandle;

-(void)didReceiveRemoteNotification:(UIApplication *)application :(nonnull NSDictionary *)userInfo;

@end
