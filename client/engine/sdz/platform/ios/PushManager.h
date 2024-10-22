//
//  PushManager.h
//  basesdk
//
//  Created by administrator on 13-9-8.
//  Copyright (c) 2013年 zlgame. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//
static const int DEVICE_TYPE = 4;
//
static const int MSG_CODE_SETTAG = 101;
static const int MSG_CODE_SETALIAS = 102;
static const int MSG_CODE_STARTPUSH = 103;
//
static const int INVOKE_SUCCESS_STATUS = 1;
static const int INVOKE_ING_STATUS = 2;
static const int INVOKE_FAIL_STATUS = 3;
static const int INVOKE_UPDATE_STATUS = 4;

#define INVOKE_STATUS           @"Status"
#define INVOKE_CONTENT          @"Content"
#define DEVICE_TYPE_NAME        @"DeviceType"
#define API_KEY_NAME            @"APP_KEY"
#define CHANNEL_ID_NAME         @"ChannelId"
#define APP_ID_NAME             @"AppId"
#define USER_ID_NAME            @"UserId"
#define TIMER_TIME_NAME         @"TimerTime"
#define HANDLE_CODE             @"HandleCode"
#define HANDLE_CONTENT          @"HandleContent"
#define ACTIVITY_STATUS         @"AcStatus"
#define ACTIVITY_CODE           @"AcCode"
#define DEVICETOKEN             @"DeviceToken"
#define PUSH_TAG                @"PushTag"
#define TAG_LEN                 @"TagLen"
#define PUSH_ALIAS              @"PushAlias"


@interface PushManager : NSObject
{
    @private
        unsigned int fun_id;
}

- (void) setFunId:(unsigned int) lua_fun_id;

+ (void) StartMsgPush:(unsigned int) lua_fun_id;
+ (NSString *) StopMsgPush;
+ (NSString *) ResumeMsgPush;
+ (NSString *) GetApiKey;
+ (void) SetTag:(NSString *) tag :(unsigned int) lua_fun_id;
+ (void) SetAlias:(NSString *) alias :(unsigned int) lua_fun_id;
+ (NSString *) GetAppInfo;
+ (NSString *) SetTimerTask:(NSString *) taskTime :(NSString *) taskName :(NSString *) taskLast :(NSString *) taskPre :(NSString *) timer;
+ (NSString *) GetActivityStatus;
+ (NSString *) CancelTimerTask:(NSString *) taskTime;

+ (int) AddlocalNotification:(NSString *) notiValue :(NSString *) notiName :(NSString *) notiLast :(NSString *) notiPre :(NSString *) notiTimer;
+ (int) RemovelocalNotification:(NSString *) taskTime;
+ (int) SetTaskLen:(int) taskLen;
+ (int) GetTaskLen;
+ (NSString *) SetActivityCode:(NSString *) acStatus;
+ (NSString *) GetActivityCode;
+ (int) IsHaveTimerTask:(NSString *) taskTime;

- (void) addtagsAliasCallback:(int)iResCode tags:(NSSet*)tags alias:(NSString*)alias;
- (void) deltagsAliasCallback:(int)iResCode tags:(NSSet*)tags alias:(NSString*)alias;

+ (void) StartMsgPushCallback:(NSString *) data;
+ (void) SetTagCallback:(unsigned int) fun_id :(NSString *) data;
+ (void) SetAliasCallback:(unsigned int) fun_id :(NSString *) data;

+ (void)SetLocalTag:(NSString *)tag;
+ (int)GetTagLen;
+ (void)SetTagLen:(int)len;
+ (BOOL)IsHaveAlias:(NSString *)alias;
+ (void)SetLocalAlias:(NSString *)alias;
+ (BOOL)IsExist:(NSArray *)array :(NSString *)obj;
+ (NSArray *)GetCurrentServerTag:(NSString *)server :(int)len;
+ (NSMutableSet *)GetOtherServerTags:(NSString *)server :(int)len;
+ (void)ClearTags;

@end
