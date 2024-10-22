//
//  PushApplication.m
//  basesdk
//
//  Created by administrator on 13-9-7.
//  Copyright (c) 2013年 许顺华. All rights reserved.
//

#import "PushApplication.h"
#import "SBJson.h"
#import "logutil.h"

@implementation PushApplication

@synthesize globalLaunchOptions;

+ (void) PushInit:(id) delegate :(NSDictionary *)launchOptions
{
    [[UIApplication sharedApplication] setApplicationIconBadgeNumber:0];
    [PushManager SetActivityCode:@"0"];
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeBadge |UIRemoteNotificationTypeSound)];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    NSLog(@"deviceToken is:%@", deviceToken);
#ifndef __NO_JPUSH__
    [APService setupWithOption:globalLaunchOptions];
    [APService registerDeviceToken:deviceToken];
#endif

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *devId = [[deviceToken description] stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"<>"]];
	devId = [devId stringByReplacingOccurrencesOfString:@" " withString:@""];
    [defaults setObject:devId forKey:DEVICETOKEN];
    [defaults synchronize];
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
    [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
    
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [PushManager StartMsgPushCallback:jsonStr];
    [jsonWrite release];
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    LogUtil::LogError("Error in registration. Error: %s", [[error localizedDescription] UTF8String]);
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
    NSNumber *num = [NSNumber numberWithInteger:[error code]];
    [jsonDic setObject:num forKey:INVOKE_CONTENT];
    
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    [PushManager StartMsgPushCallback:jsonStr];
    [jsonWrite release];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
	[[UIApplication sharedApplication] setApplicationIconBadgeNumber:0];
    NSLog(@"receive remote notification %@", userInfo);
#ifndef __NO_JPUSH__
    [APService handleRemoteNotification:userInfo];
#endif
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification
{
    NSLog(@"receive local notification %@", [notification userInfo]);
	[[UIApplication sharedApplication] setApplicationIconBadgeNumber:0];
    NSDictionary *info = notification.userInfo;
    NSString *taskTime = [info objectForKey:TIMER_TIME_NAME];

    [PushManager SetActivityCode:taskTime];
}

- (void)networkDidReceiveMessage:(NSNotification *)notification {
    NSDictionary * userInfo = [notification userInfo];
    NSLog(@"receive userInfo is:%@", userInfo);
    //NSString *title = [userInfo valueForKey:@"title"];
    //NSString *content = [userInfo valueForKey:@"content"];
    //NSLog(@"receive title is:%@", title);
    //NSLog(@"receive content is:%@", content);
}
    
- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_7_1
- (void)application:(UIApplication *)application
didRegisterUserNotificationSettings:
(UIUserNotificationSettings *)notificationSettings {
	
}

// Called when your app has been activated by the user selecting an action from
// a local notification.
// A nil action identifier indicates the default action.
// You should call the completion handler as soon as you've finished handling
// the action.
- (void)application:(UIApplication *)application
handleActionWithIdentifier:(NSString *)identifier
forLocalNotification:(UILocalNotification *)notification
  completionHandler:(void (^)())completionHandler {
	
}

// Called when your app has been activated by the user selecting an action from
// a remote notification.
// A nil action identifier indicates the default action.
// You should call the completion handler as soon as you've finished handling
// the action.
- (void)application:(UIApplication *)application
handleActionWithIdentifier:(NSString *)identifier
forRemoteNotification:(NSDictionary *)userInfo
  completionHandler:(void (^)())completionHandler {
	
}
#endif

- (void)dealloc
{
    [super dealloc];
}

@end
