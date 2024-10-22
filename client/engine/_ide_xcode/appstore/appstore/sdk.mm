
#import "sdk.h"
#import "AppController.h"
#import "RootViewController.h"
#import "platform/ios/json_kit/JSONKit.h"
#import "platform/ios/MacroHelper.h"
#import "platform/ios/RoleInfo.h"
#import "platform/ios/TDInfo.h"
#import "SBJson.h"

@interface Sdk()
-(BOOL) callFunction:(NSString *) funcType;
-(void) setUserInfo:(BOOL)isLevelup :(NSString*) info;
-(void) initNotifiers;
-(void) SDKLoginNotificationResult:(NSDictionary *)data;
-(void) SDKLogoutNotificationResult:(NSDictionary *)data;
-(void) SDKPurchaseNotificationResult:(NSDictionary *)data;
@end

@implementation Sdk

-(void) SDKInit:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    //SDK初始化代码....
    
    //初始化操作函数映射表
    FuncTypeDict = [[NSMutableDictionary alloc] init];
    [FuncTypeDict setValue:@(EFuncType_ENTER_USER_CENTER) forKey:FuncType_ENTER_USER_CENTER];
    [FuncTypeDict setValue:@(EFuncType_ENTER_BBS) forKey:FuncType_ENTER_BBS];
    [FuncTypeDict setValue:@(EFuncType_SHOW_TOOLBAR) forKey:FuncType_SHOW_TOOLBAR];
    [FuncTypeDict setValue:@(EFuncType_HIDE_TOOLBAR) forKey:FuncType_HIDE_TOOLBAR];
    [FuncTypeDict setValue:@(EFuncType_Customer_Service) forKey:FuncType_Customer_Service];

    //监听相关通知
    [self initNotifiers];
    
    //初始化成功回调
    [BaseSdk SDKInitFinishCallBack:SDK_SUCCESS_CODE];
}

// 登陆
-(void) SDKLogin:(NSString *)info
{
    if (nil != loginData)
    {
        // 逻辑上不允许这种情况，若出现了，直接注销账号
        [self SDKLogout:@""];
    }
    else
    {
        NSLog(@"SDKLogin %@", info);
        
        // SDK登录代码....
    }
}

// 注销账号
-(void) SDKLogout:(NSString *)info
{
    if (nil == loginData)
    {
        return;
    }
    
    NSLog(@"SDKLogout %@", info);

    // SDK登出代码....
    // 若无登出接口，则直接调用登出回调
    [self SDKLogoutNotificationResult:nil];
}

// 切换账号
-(void) SDKSwitchAccount:(NSString *)info
{
    if (nil == loginData)
    {
        return;
    }
    
    NSLog(@"SDKSwitchAccount %@", info);
    
    // 切换账号
    [self SDKLogout:@""];
}

// 支付
-(void) SDKTD:(NSString *) info
{
    TDInfo* tdInfo = [TDInfo parseTDInfo:info];
    
    // SDK支付代码....
}

-(void) SDKCommonHandle:(NSString *)info
{
    NSLog(@"SDKCommonHandle %@", info);
    
    NSDictionary* jsonDict = [info objectFromJSONString];
    
    int type = [[jsonDict valueForKey:@"type"] intValue];
    switch (type)
    {
        case CT_CREATE_ROLE:
        case CT_ENTER_GAME:
        case CT_LEVEL_UP:
            // 创建角色、进入游戏、角色升级，上传角色信息
            [self setUserInfo:type == CT_LEVEL_UP :info];
            break;
            
        case CT_CALL_FUNC:
            NSString* funcType = [jsonDict valueForKey:@"func_type"];
            
            // 调用操作函数
            [self callFunction:funcType];
            break;
    }
}

-(void) SDKToUserCenter:(NSString *)info
{
    [self callFunction:FuncType_ENTER_USER_CENTER];
}

// 调用扩展函数
-(BOOL) callFunction:(NSString *) funcType
{
    NSNumber* obj = [FuncTypeDict valueForKey:funcType];
    if (nil == obj)
    {
        return NO;
    }
    
    unsigned int intObj = [obj unsignedIntValue];
    SDKCommonHandleFuncType eFuncType = (SDKCommonHandleFuncType)intObj;
    switch (eFuncType)
    {
        case EFuncType_Customer_Service:
            // 若SDK提供了客服中心接口，则在此处调用，并开启下方代码
//            [BaseSdk SDKCommonHandleCallBack:SDK_COMMON_CALLFUNC_SUCCESS :funcType];
//            return TRUE;
            break;

        default:
            break;
    }

    [BaseSdk SDKCommonHandleCallBack:SDK_COMMON_CALLFUNC_FAILED :funcType];
    return NO;
}

// 向渠道提交用户信息。在创建游戏角色、进入游戏和角色升级3个地方调用此接口
-(void) setUserInfo:(BOOL)isLevelup :(NSString *)info
{
    RoleInfo* roleInfo = [RoleInfo parseRoleInfo:info];
    NSString* roleID = [NSString stringWithFormat:@"%@", roleInfo.roleID];
    NSString* serverID = [NSString stringWithFormat:@"%@", roleInfo.serverID];

    if (isLevelup)
    {
        // SDK角色升级报道....
    }
    else
    {
        // SDK创建角色，进入游戏报道....
    }
}

// 监听SDK相关通知
-(void) initNotifiers
{
}

// 登陆通知
-(void)SDKLoginNotificationResult:(NSDictionary *)data
{
    BOOL bSwitchAccount = nil != loginData ? YES : NO;
    NSString* key = !bSwitchAccount ? @"登陆账号" : @"切换账号";
    
    NSLog(@"%@ 成功", key);

    // 获取用户id
    NSString *userid = @"";
    // 获取用户昵称
    NSString *nickname = @"";
    // 获取用户usertoken
    NSString *usertoken = @"";
    
    if (nil != loginData)
    {
        [loginData release];
    }
    loginData = [NSString stringWithFormat:@"{\"user_id\":\"%@\",\"user_name\":\"%@\",\"token\":\"%@\"}", userid, nickname, usertoken];
    [loginData retain];

    if (!bSwitchAccount)
    {
        // 登陆成功回调
        [BaseSdk SdKLoginPannelCallBack:SDK_SUCCESS_CODE :loginData];
    }
    else
    {
        // 切换账号成功的回调
        [BaseSdk SdKLoginPannelCallBack:SDK_SWITCH_ACCOUNT_CODE :loginData];
    }
}

// 注销账号通知
-(void)SDKLogoutNotificationResult:(NSNotification *)notify
{
    NSLog(@"注销账号成功");
    
    // 注销成功
    if (nil != loginData)
    {
        [loginData release];
        loginData = nil;
    }
    
    // 注销成功回调
    [BaseSdk SdKLoginPannelCallBack:SDK_LOGOUT_CODE];
}

// 支付通知
-(void)SDKPurchaseNotificationResult:(NSDictionary *)data
{
    if(true)
    {
        // 支付成功回调
        [BaseSdk SDKTDFinishCallBack:SDK_SUCCESS_CODE];
    }
    else
    {
        // 支付失败回调
        [BaseSdk SDKTDFinishCallBack:SDK_FAILURE_CODE];
    }
}

-(void)dealloc
{
    if (nil != FuncTypeDict)
    {
        [FuncTypeDict release];
        FuncTypeDict = nil;
    }
    
    if (nil != loginData)
    {
        [loginData release];
        loginData = nil;
    }
    
    [super dealloc];
}

@end
