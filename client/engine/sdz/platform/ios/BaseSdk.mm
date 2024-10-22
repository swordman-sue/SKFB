//
//  BaseSdk.m
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//

#import "BaseSdk.h"
#import "platform/sdktools.h"
#import "platform/ios/device/openudid/AnotherOpenUDID.h"

#include "luaengine/luaengine.h"
#include "platform/ios/basesdkhelper.h"
#include "platform/ios/PlatformUtil.h"
#include "filesystem/setting.h"

@implementation BaseSdk

-(id)init
{
    self = [super init];
    if (self)
    {
        //获取sdkinfo.plist属性
        properties = [[NSMutableDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"sdkinfo" ofType:@"plist"]] retain];
        
        //获取info.plist属性
        NSDictionary *dic = [[NSBundle mainBundle] infoDictionary];
        
        //添加包名
        [properties setObject:[dic objectForKey:@"CFBundleIdentifier"] forKey:@"PACKAGE_NAME"];
        //版本号(1.0.0)
        [properties setObject:[dic objectForKey:@"CFBundleShortVersionString"] forKey:@"APP_VERSION"];
        //版本号(100)
        [properties setObject:[dic objectForKey:@"CFBundleVersion"] forKey:@"APP_VERSION_CODE"];
        //游戏名称
        [properties setObject:[dic objectForKey:@"CFBundleDisplayName"] forKey:@"APP_NAME"];
        //获取设备ID
        [properties setObject:[AnotherOpenUDID value] forKey:@"DEVICE_ID"];

        //设备modle
        NSString *device_model =[[UIDevice currentDevice] model];
        NSString *system_name =[[UIDevice currentDevice] systemName];
        NSString *system_version =[[UIDevice currentDevice] systemVersion];
        NSString *debice_info =[NSString stringWithFormat:@"%@|%@|%@",device_model,system_name,system_version];
        [properties setObject:debice_info forKey:@"DEVICE_MODEL"];
        [properties setObject:[AnotherOpenUDID value] forKey:@"DEVICE_IMSI"];
    }
    return self;
}
-(void) SDKInit:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [BaseSdk SDKInitFinishCallBack:SDK_SUCCESS_CODE];
}

-(void) SDKLogin:(NSString *)info
{}

-(void) SDKLogout:(NSString *)info
{}

-(void) SDKSwitchAccount:(NSString *)info
{}

-(void) SDKTD:(NSString *) info
{}

-(void) SDKCommonHandle:(NSString *)info
{}

-(void) SDKKeyDataHandle:(NSString *)info
{}

-(void) SDKToUserCenter:(NSString *)info
{}

-(void) SDKCheckUpdate:(NSString *)url
{
    [PlatformUtil GotoBrowser:url];
}

-(void) SDKExitApp:(NSString *) info
{}

+(void) SDKInitFinishCallBack:(int) code
{
    if (code == SDK_SUCCESS_CODE)
    {
        SdkTools::sdk_is_inited = true;
        Setting::GetInstance()->Set("IsSDKInitFailed", "0");
    }
    else
    {
        NSLog(@"Sdk init fail.");
        Setting::GetInstance()->Set("IsSDKInitFailed", "1");
    }
	
    if (SdkTools::sdk_init_finish_fun_id != 0)
    {
        LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_init_finish_fun_id, "i", code);
    }	
}

+(void) SdKLoginPannelCallBack:(int) code :(NSString *) data
{
    if (SdkTools::sdk_login_panel_fun_id != 0)
    {
        const char * datainfo = [data UTF8String];
        LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_login_panel_fun_id, "is", code, datainfo);
    }
}

+(void) SdKLoginPannelCallBack:(int) code
{
    [BaseSdk SdKLoginPannelCallBack:code:@""];
}

+(void) SDKTDFinishCallBack:(int) code :(NSString *) data
{
    if (SdkTools::sdk_td_finish_fun_id != 0)
    {
         const char * datainfo = [data UTF8String];
        LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_td_finish_fun_id , "is", code, datainfo);
    }
}

+(void) SDKTDFinishCallBack:(int) code
{
    [BaseSdk SDKTDFinishCallBack:code:@""];
}


+(void) SDKCommonHandleCallBack:(int) code :(NSString *) data
{
    if (SdkTools::sdk_common_fun_id != 0)
    {
        const char * datainfo = [data UTF8String];
        LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_common_fun_id, "is", code, datainfo);
    }
}

+(void) SDKCommonHandleCallBack:(int) code
{
    [BaseSdk SDKCommonHandleCallBack:code:@""];
}


-(id) GetPropertie:(NSString *) key
{
    id  pro = [properties objectForKey:key];
    return pro;
}

-(void) SetPropertie:(NSString *) key :(id) value
{
    if (nil == key || nil == value)
    {
        NSLog(@"key or value is nil when call method SetPropertie");
        return;
    }
    [properties setObject:value forKey:key];
}

-(void)applicationWillResignActive:(UIApplication *)application
{
}

-(void)applicationDidEnterBackground:(UIApplication *)application
{
}

-(void)applicationWillEnterForeground:(UIApplication *)application
{
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
}

-(void)applicationWillTerminate:(UIApplication *)application
{
}

-(void)didRegisterForRemoteNotificationsWithDeviceToken:(UIApplication *)application :(NSData *)deviceToken
{
}

-(void)didFailToRegisterForRemoteNotificationsWithError:(UIApplication *)application :(NSError *)error
{
}

-(UIInterfaceOrientationMask)supportedInterfaceOrientationsForWindow:(UIApplication *)application :(UIWindow *)window
{
    return UIInterfaceOrientationMaskAll;
}

-(BOOL)handleOpenURL:(UIApplication *)application :(NSURL *)url
{
    return YES;
}

-(BOOL)openURL:(UIApplication *)application :(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    return YES;
}

-(BOOL)openURL:(UIApplication *)app :(NSURL *)url options:(NSDictionary<NSString*, id> *)options
{
    return YES;
}

-(BOOL)continueUserActivity:(UIApplication *)application :(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray * restorableObjects))restorationHandler
{
    return YES;
}

-(void)didReceiveLocalNotification:(UIApplication *)application :(UILocalNotification *)notification
{
    
}

-(void)didReceiveRemoteNotification:(UIApplication *)application :(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandle
{
    
}

-(void)didReceiveRemoteNotification:(UIApplication *)application :(nonnull NSDictionary *)userInfo
{
    
}

-(void)dealloc
{
	[properties release];

    [super dealloc];
}

@end
