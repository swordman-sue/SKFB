//
//  PushManager.m
//  basesdk
//
//  Created by administrator on 13-9-8.
//  Copyright (c) 2013年 zlgame. All rights reserved.
//

#import "PushManager.h"
#include "zqcommon/typedef.h"
#include "luaengine/luaengine.h"
#import "SBJson.h"

#if !defined(__NO_JPUSH__)
#import "APService.h"
#endif

static unsigned int sdk_start_push_fun_id = 0;

@implementation PushManager

- (id) init
{
    if(self = [super init]){
        fun_id = 0;
    }
    return self;
}

- (void) setFunId:(unsigned int) lua_fun_id
{
    fun_id = lua_fun_id;
    //NSLog(@"PushManager begin set fun_id is %d", fun_id);
}

+ (void) StartMsgPush:(unsigned int) lua_fun_id
{
#ifndef __NO_JPUSH__
    [APService registerForRemoteNotificationTypes:(UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeBadge |UIRemoteNotificationTypeSound) categories:nil];
#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_7_1
	if ([[UIDevice currentDevice].systemVersion floatValue] >= 8.0) {
		//categories
		[APService registerForRemoteNotificationTypes:(UIUserNotificationTypeBadge |
											 UIUserNotificationTypeSound |
											 UIUserNotificationTypeAlert)
										   categories:nil];
	} else {
		//categories nil
		[APService registerForRemoteNotificationTypes:(UIRemoteNotificationTypeBadge |UIRemoteNotificationTypeSound |
											 UIRemoteNotificationTypeAlert)
										   categories:nil];
	}
#else
		 //categories nil
  	[APService registerForRemoteNotificationTypes:(UIRemoteNotificationTypeBadge |
												   UIRemoteNotificationTypeSound |
												   UIRemoteNotificationTypeAlert)
									 	categories:nil];
#endif

    if(lua_fun_id != 0)
    {
        sdk_start_push_fun_id = lua_fun_id;
    }
#endif
}

+(NSString *)StopMsgPush
{
    [[UIApplication sharedApplication] unregisterForRemoteNotifications];
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];

    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
    [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
    
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+(NSString *)ResumeMsgPush
{
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeBadge |UIRemoteNotificationTypeSound)];
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];

    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
    [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
    
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+(NSString *)GetApiKey
{
    NSString *plistPath = [[NSBundle mainBundle] pathForResource:@"PushConfig" ofType:@"plist"];
    NSMutableDictionary *data = [NSMutableDictionary dictionaryWithContentsOfFile:plistPath];
 
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    
    NSString *api_key = [data objectForKey:API_KEY_NAME];
    if (!api_key) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"" forKey:INVOKE_CONTENT];
    } else {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:api_key forKey:INVOKE_CONTENT];
    }
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
   
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+ (void) SetTag:(NSString *) tag :(unsigned int) lua_fun_id
{
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    
    if([defaults objectForKey:DEVICETOKEN] == nil) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"apns not support" forKey:INVOKE_CONTENT];
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetTagCallback:lua_fun_id :jsonStr];
        [jsonWrite release];
        return;
    }
    if ([@"" isEqualToString:tag]) {
        PushManager *push = [[PushManager alloc] init];
        [push setFunId:lua_fun_id];
#ifndef __NO_JPUSH__
        [APService setTags:[NSSet set] callbackSelector:@selector(deltagsAliasCallback:tags:alias:) object:push];
#endif
        [push release];
        return;
    }
    int len = [PushManager GetTagLen];
    NSArray *tagArr = [tag componentsSeparatedByString:@";"];
    NSString *server_tag = [tagArr objectAtIndex:0];
    NSArray *serverArr = [server_tag componentsSeparatedByString:@"_"];
    if ([serverArr count] <= 1) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"tag position error" forKey:INVOKE_CONTENT];
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetTagCallback:lua_fun_id :jsonStr];
        [jsonWrite release];
        return;
    }
    NSString *server = [serverArr objectAtIndex:1];
    NSArray *currTags = [PushManager GetCurrentServerTag:server :len];
    NSMutableArray *muArray = [[NSMutableArray alloc] init];
    NSMutableArray *addArray = [[NSMutableArray alloc] init];
    for (NSString *tmTag in tagArr) {
        if (![PushManager IsExist:currTags :tmTag]) {
            [muArray addObject:tmTag];
        }
        [addArray addObject:tmTag];
    }
    if ([currTags count] > 0 && [muArray count] <= 0) {
        NSLog(@"*******set tag tag have*******");
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"tag have" forKey:INVOKE_CONTENT];
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetTagCallback:lua_fun_id :jsonStr];
        [jsonWrite release];
        [muArray release];
        [addArray release];
        [currTags release];
        return;
    }
    NSMutableSet *haveset = [PushManager GetOtherServerTags:server :len];
    [haveset addObjectsFromArray:addArray];
    NSSet *tags = [NSSet setWithSet:haveset];
    PushManager *push = [[PushManager alloc] init];
    [push setFunId:lua_fun_id];
#ifndef __NO_JPUSH__
    [APService setTags:tags callbackSelector:@selector(addtagsAliasCallback:tags:alias:) object:push];
#endif
    [push release];
    [haveset release];
    [muArray release];
    [addArray release];
    [currTags release];
}

+ (BOOL)IsExist:(NSArray *)array :(NSString *)obj
{
    for (NSString *cu in array) {
        if ([cu isEqualToString:obj]) {
            return YES;
        }
    }
    return NO;
}

+ (NSArray *)GetCurrentServerTag:(NSString *)server :(int)len
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSMutableArray *resultArr = [[NSMutableArray alloc] init];
    for (int i = 1; i <= len; i++) {
        NSString *temp = [NSString stringWithFormat:@"%@%d", PUSH_TAG, i];
        NSString *temp_tag = [defaults objectForKey:temp];
        if (temp_tag && ![@"" isEqualToString:temp_tag]) {
            NSArray *serverArr = [temp_tag componentsSeparatedByString:@"_"];
            if ([serverArr count] > 1) {
                NSString *temp_server = [serverArr objectAtIndex:1];
                if ([@"" isEqualToString:temp_server] || [temp_server isEqualToString:server]) {
                    [resultArr addObject:temp_tag];
                }
            } else {
                [resultArr addObject:temp_tag];
            }
        }
    }
    return resultArr;
}

+ (NSMutableSet *)GetOtherServerTags:(NSString *)server :(int)len
{
    NSMutableSet *muset = [[NSMutableSet alloc] init];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    for (int i = 1; i <= len; i++) {
        NSString *temp = [NSString stringWithFormat:@"%@%d", PUSH_TAG, i];
        NSString *temp_tag = [defaults objectForKey:temp];
        if (temp_tag && ![@"" isEqualToString:temp_tag]) {
            NSArray *serverArr = [temp_tag componentsSeparatedByString:@"_"];
            if ([serverArr count] <= 1 || ![[serverArr objectAtIndex:1] isEqualToString:server]) {
                [muset addObject:temp_tag];
            }
        }
    }
    return muset;
}

+ (void)ClearTags
{
    int len = [PushManager GetTagLen];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    for (int i = 1; i <= len; i++) {
        NSString *temp = [NSString stringWithFormat:@"%@%d", PUSH_TAG, i];
        [defaults removeObjectForKey:temp];
    }
    [PushManager SetTagLen:0];
}

+ (void)SetLocalTag:(NSString *)tag
{
    int len = [PushManager GetTagLen];
    int tag_len = len + 1;
    NSString *temp = [NSString stringWithFormat:@"%@%d", PUSH_TAG, tag_len];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:tag forKey:temp];
    [defaults synchronize];
    [PushManager SetTagLen:tag_len];
}

+ (int)GetTagLen
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *num = [defaults objectForKey:TAG_LEN];
    if(num){
        return [num intValue];
    } else {
        return 0;
    }
}

+ (void)SetTagLen:(int)len
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *invoke_len = [NSNumber numberWithInt:len];
    [defaults setObject:invoke_len forKey:TAG_LEN];
    [defaults synchronize];
}

- (void)addtagsAliasCallback:(int)iResCode tags:(NSSet*)tags alias:(NSString*)alias
{
    //NSLog(@"add tags call back code is:%d  tags is:%@", iResCode, tags);
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    
    if (iResCode == 0)
    {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];     
        [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
        [PushManager ClearTags];
        for (NSString *tmTag in tags) {
            [PushManager SetLocalTag:tmTag];
        }
        
    } else
    {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        NSNumber *code = [NSNumber numberWithInt:iResCode];
        [jsonDic setObject:code forKey:INVOKE_CONTENT];
    }
    if (fun_id != 0) {
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetTagCallback:fun_id :jsonStr];
        [jsonWrite release];
    }
}

- (void)deltagsAliasCallback:(int)iResCode tags:(NSSet*)tags alias:(NSString*)alias
{
    //NSLog(@"del tags call back code is:%d  fun_id is:%d", iResCode, fun_id);
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];

    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    
    if (iResCode == 0)
    {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
        [PushManager ClearTags];
    } else
    {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        NSNumber *code = [NSNumber numberWithInt:iResCode];
        [jsonDic setObject:code forKey:INVOKE_CONTENT];
    }
    if (fun_id != 0) {
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetTagCallback:fun_id :jsonStr];
        [jsonWrite release];
    }
}

+ (void) SetAlias:(NSString *) alias :(unsigned int) lua_fun_id
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    if([defaults objectForKey:DEVICETOKEN] == nil) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"apns not support" forKey:INVOKE_CONTENT];
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetAliasCallback:lua_fun_id :jsonStr];
        [jsonWrite release];
        
    }else if ([PushManager IsHaveAlias:alias]) {

        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"have" forKey:INVOKE_CONTENT];
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetAliasCallback:lua_fun_id :jsonStr];
        [jsonWrite release];
        
    } else {
        //NSLog(@"begin set alias......");
        PushManager *push = [[PushManager alloc] init];
        [push setFunId:lua_fun_id];
#ifndef __NO_JPUSH__
        [APService setAlias:alias callbackSelector:@selector(setPushAliasCallback:tags:alias:) object:push];
#endif
        [push release];
    }
}

+ (BOOL)IsHaveAlias:(NSString *)alias
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *temp = [defaults objectForKey:PUSH_ALIAS];
    if(nil != temp && [temp isEqualToString:alias]){
        return YES;
    } else {
        return NO;
    }
}

+ (void)SetLocalAlias:(NSString *)alias
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:alias forKey:PUSH_ALIAS];
    [defaults synchronize];
}

- (void)setPushAliasCallback:(int)iResCode tags:(NSSet*)tags alias:(NSString*)alias
{
    //NSLog(@"set alias call back code is: %d  fun_id is:%d", iResCode, fun_id);
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];

    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    
    if (iResCode == 0)
    {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
        [PushManager SetLocalAlias:alias];
    } else
    {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        NSNumber *code = [NSNumber numberWithInt:iResCode];
        [jsonDic setObject:code forKey:INVOKE_CONTENT];
    }
    if (fun_id != 0) {
        SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
        NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
        [PushManager SetAliasCallback:fun_id :jsonStr];
        [jsonWrite release];
    }
}

+(NSString *)GetAppInfo
{
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];
    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    if ([defaults objectForKey:DEVICETOKEN] == nil) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"" forKey:INVOKE_CONTENT];
        [jsonDic setObject:@"" forKey:PUSH_ALIAS];
        
    } else {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:[defaults objectForKey:DEVICETOKEN] forKey:INVOKE_CONTENT];
        [jsonDic setObject:[defaults objectForKey:PUSH_ALIAS] forKey:PUSH_ALIAS];
    }    
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+(NSString *)SetTimerTask:(NSString *) taskTime :(NSString *) taskName :(NSString *) taskLast :(NSString *) taskPre :(NSString *) timer
{
    int result = -1;
    if ([@"3" isEqualToString:taskLast]) {
        NSArray *weekArr = [timer componentsSeparatedByString:@";"];
        for (NSString *tmWeek in weekArr) {
           result = [PushManager AddlocalNotification:taskTime :taskName :taskLast :taskPre :tmWeek];
            if (-1 == result) {
                break;
            }
        }
        
    } else {
       result = [PushManager AddlocalNotification:taskTime :taskName :taskLast :taskPre :timer];
    }
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];

    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    if (-1 != result) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
        
    } else {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:@"fail" forKey:INVOKE_CONTENT];
    }
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+(NSString *)GetActivityStatus
{
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];

    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
    
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
    NSString *status = [PushManager GetActivityCode];
    [jsonDic setObject:status forKey:INVOKE_CONTENT];
    
    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+(NSString *)CancelTimerTask:(NSString *) taskTime
{
    NSMutableDictionary *jsonDic = [NSMutableDictionary dictionaryWithCapacity:3];

    NSNumber *device_type = [NSNumber numberWithInt:DEVICE_TYPE];
    [jsonDic setObject:device_type forKey:DEVICE_TYPE_NAME];
    
    @try {
        if([@"ALL" isEqualToString:taskTime]){
            UIApplication *app = [UIApplication sharedApplication];
            [app cancelAllLocalNotifications];
            [PushManager SetTaskLen:0];
            NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
            [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
            [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
        } else {
            int r_len = [PushManager RemovelocalNotification:taskTime];
            if (r_len == 0) {
                NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
                [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
                [jsonDic setObject:@"not find task" forKey:INVOKE_CONTENT];
                
            } else {
                NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_SUCCESS_STATUS];
                [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
                [jsonDic setObject:@"success" forKey:INVOKE_CONTENT];
            }
        }

        
    }
    @catch (NSException *exception) {
        NSNumber *invoke_status = [NSNumber numberWithInt:INVOKE_FAIL_STATUS];
        [jsonDic setObject:invoke_status forKey:INVOKE_STATUS];
        [jsonDic setObject:[exception reason] forKey:INVOKE_CONTENT];
    }

    SBJsonWriter *jsonWrite = [[SBJsonWriter alloc] init];
    NSString *jsonStr = [jsonWrite stringWithObject:jsonDic];
    
    [jsonWrite release];
    
    return jsonStr;
}

+ (int)AddlocalNotification:(NSString *) notiTime :(NSString *) notiName :(NSString *) notiLast :(NSString *) notiPre :(NSString *) notiTimer
{
    //20 * 60
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    @try {
        NSDate *now = [NSDate date];
        NSCalendar *calendar = [NSCalendar currentCalendar];
        NSUInteger unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit | NSWeekdayCalendarUnit | NSDayCalendarUnit | NSHourCalendarUnit | NSMinuteCalendarUnit | NSSecondCalendarUnit;
        NSDateComponents *dateComponent = [calendar components:unitFlags fromDate:now];

        NSInteger hourSecond = [dateComponent hour] * 60 * 60;
        NSInteger minuteSecond = [dateComponent minute] * 60;
        NSInteger nowSeconds = hourSecond + minuteSecond + [dateComponent second];
        //
        NSInteger task_time = [notiTime intValue];
        NSInteger task_pre = [notiPre intValue];
        NSInteger noti_timer = [notiTimer intValue];
        
        NSInteger formnowSecond = task_time - nowSeconds - task_pre;
        if(task_pre < 0){
            formnowSecond = abs(task_pre);
        }
        
        if ([@"3" isEqualToString:notiLast]) {
            task_time = task_time + noti_timer;
            formnowSecond = formnowSecond + (noti_timer - [dateComponent weekday]) * 24 * 60 * 60;
        }
        NSDate *pushDate = [NSDate dateWithTimeIntervalSinceNow:formnowSecond];
        int taskLen = [PushManager GetTaskLen];
        NSString * key_time = [NSString stringWithFormat:@"%d", task_time];
        int have = [PushManager IsHaveTimerTask:key_time];
        if((have == 0) && notification != nil) {
            NSLog(@"Begin Add localNotification......");
            taskLen = taskLen + 1;
            notification.fireDate = pushDate;
            if ([@"2" isEqualToString:notiLast]) {
                notification.repeatInterval = NSDayCalendarUnit;
                
            } else if ([@"3" isEqualToString:notiLast]) {                
                notification.repeatInterval = NSWeekCalendarUnit;
                
            }           
            notification.timeZone = [NSTimeZone defaultTimeZone];
            notification.applicationIconBadgeNumber = 1;
            notification.soundName = UILocalNotificationDefaultSoundName;
            notification.alertBody = notiName;
            //notification.alertAction = @"Open";
            NSDictionary *info = [NSDictionary dictionaryWithObject:key_time forKey:TIMER_TIME_NAME];
            notification.userInfo = info;
            UIApplication *app = [UIApplication sharedApplication];
            [app scheduleLocalNotification:notification];
            [PushManager SetTaskLen:taskLen];
        }
        return taskLen;
    }
    @catch (NSException *exception) {
        NSLog(@"Add Notification Exception:%@", [exception reason]);
        return -1;
    }
    @finally {
        [notification release];
    }
}

+ (int) IsHaveTimerTask:(NSString *) taskTime
{
    UIApplication *app = [UIApplication sharedApplication];
    NSArray *localArray = [app scheduledLocalNotifications];
    if (localArray) {
        for (UILocalNotification *noti in localArray) {
            NSDictionary *dict = noti.userInfo;
            if(dict) {
                NSString *inKey = [dict objectForKey:TIMER_TIME_NAME];
                if([inKey isEqualToString:taskTime]) {
                    //NSLog(@"Have Timer Task %@", taskTime);
                    return 1;
                }
            }
        }
    }
    return 0;
}

+ (int) RemovelocalNotification:(NSString *) taskTime
{
    UIApplication *app = [UIApplication sharedApplication];
    NSArray *localArray = [app scheduledLocalNotifications];
    if (localArray) {
        for (UILocalNotification *noti in localArray) {
            NSDictionary *dict = noti.userInfo;
            if(dict) {
                NSString *inKey = [dict objectForKey:TIMER_TIME_NAME];
                if([inKey isEqualToString:taskTime]) {
                    NSLog(@"Remove Timer Task Success");
                    [app cancelLocalNotification:noti];
                    int taskLen = [PushManager GetTaskLen];
                    taskLen = taskLen - 1;
                    [PushManager SetTaskLen:taskLen];
                    return taskLen;
                }
            }
        }
    }
    return 0;
}

+ (int) SetTaskLen:(int) taskLen
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *len = [NSNumber numberWithInt:taskLen];
    [defaults setObject:len forKey:@"TaskLen"];
    return taskLen;
}

+ (int) GetTaskLen
{
    UIApplication *app = [UIApplication sharedApplication];
    NSArray *localArray = [app scheduledLocalNotifications];
    return [localArray count];
    //NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    //NSNumber *len = [defaults objectForKey:@"TaskLen"];
    //if (!len) {
    //    return 0;
    //}
    //return [len intValue];
}

+ (NSString *) SetActivityCode:(NSString *) acStatus
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setObject:acStatus forKey:TIMER_TIME_NAME];
    return acStatus;
}

+ (NSString *) GetActivityCode
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *status = [defaults objectForKey:TIMER_TIME_NAME];
    if (!status) {
        return @"0";
    }
    return status;
}

+ (void) StartMsgPushCallback:(NSString *) data
{
    if(sdk_start_push_fun_id != 0)
    {
        const char *result = [data  UTF8String];
        LuaEngine::GetInstance()->CallFunction(sdk_start_push_fun_id, "s", result);
        sdk_start_push_fun_id = 0;
    }
}

+ (void) SetTagCallback:(unsigned int) fun_id :(NSString *) data
{
    if(fun_id != 0)
    {
        const char *result = [data  UTF8String];
        LuaEngine::GetInstance()->CallFunction(fun_id, "s", result);
    }
}

+ (void) SetAliasCallback:(unsigned int) fun_id :(NSString *) data
{
    if(fun_id != 0)
    {
        const char *result = [data  UTF8String];
        LuaEngine::GetInstance()->CallFunction(fun_id, "s", result);
    }
}

@end
