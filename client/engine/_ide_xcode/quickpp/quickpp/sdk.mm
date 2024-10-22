
#import "sdk.h"
#import "AppController.h"
#import "RootViewController.h"
#import "platform/ios/json_kit/JSONKit.h"
#import "platform/ios/MacroHelper.h"
#import "QKGameInfo.h"
#import "SBJson.h"
#import <SMPCQuickSDK/SMPCQuickSDK.h>

#define PRODUCT_CODE        @"33332429532015072921019874893307"
#define PRODUCT_KEY         @"63098293"

@interface Sdk()
-(BOOL) callQKFunction:(NSString *) funcType;
-(void) setUserInfo:(BOOL)isCreateRole :(NSString*) info;
-(void) initQKNotifiers;
-(void)smpcQpLoginResult:(NSNotification *)notify;
-(void)smpcQpLogoutResult:(NSNotification *)notify;
-(void)smpcQpRechargeResult:(NSNotification *)notify;
@end

@implementation Sdk

-(void) SDKInit:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // 初始化qk操作函数映射表
    qkFuncTypeDict = [[NSMutableDictionary alloc] init];
    [qkFuncTypeDict setValue:@(SMPC_QUICK_SDK_FUNC_TYPE_ENTER_USER_CENTER) forKey:FuncType_ENTER_USER_CENTER];
    [qkFuncTypeDict setValue:@(SMPC_QUICK_SDK_FUNC_TYPE_ENTER_BBS) forKey:FuncType_ENTER_BBS];
    [qkFuncTypeDict setValue:@(SMPC_QUICK_SDK_FUNC_TYPE_SHOW_TOOLBAR) forKey:FuncType_SHOW_TOOLBAR];
    [qkFuncTypeDict setValue:@(SMPC_QUICK_SDK_FUNC_TYPE_HIDE_TOOLBAR) forKey:FuncType_HIDE_TOOLBAR];
    [qkFuncTypeDict setValue:@(SMPC_QUICK_SDK_FUNC_TYPE_ENTER_CUSTOMER_CENTER) forKey:FuncType_Customer_Service];

    // 监听QK相关通知
    [self initQKNotifiers];
    
    // 初始化
    SMPCQuickSDKInitConfigure *cfg = [[SMPCQuickSDKInitConfigure alloc] init];
    cfg.productCode = PRODUCT_CODE;
    cfg.productKey = PRODUCT_KEY;
    [[SMPCQuickSDK defaultInstance] initWithConfig:cfg application:application didFinishLaunchingWithOptions:launchOptions];
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
        
        // 登陆账号
        int error = [[SMPCQuickSDK defaultInstance] login];
        if (error != 0)
        {
            NSLog(@"SDKLogin failed error_code:%d",error);
        }
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

    // 注销账号
    [[SMPCQuickSDK defaultInstance] logout];
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
    SMPCQuickSDKGameRoleInfo* qkRoleInfo = [QKGameInfo parseBaseRoleInfo:info];
    SMPCQuickSDKPayOrderInfo* qkOrderInfo = [QKGameInfo parseOrderInfo:info];

    int error = [[SMPCQuickSDK defaultInstance] payOrderInfo:qkOrderInfo roleInfo:qkRoleInfo];
    if (error!=0)
    {
        NSLog(@"SDKTD failed error_code:%d", error);
    }
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
            [self setUserInfo:type == CT_CREATE_ROLE :info];
            break;
            
        case CT_CALL_FUNC:
            NSString* funcType = [jsonDict valueForKey:@"func_type"];
            
            // 调用操作函数
            if ([self callQKFunction:funcType])
            {
                [BaseSdk SDKCommonHandleCallBack:SDK_COMMON_CALLFUNC_SUCCESS :funcType];
            }
            else
            {
                [BaseSdk SDKCommonHandleCallBack:SDK_COMMON_CALLFUNC_FAILED :funcType];
            }
            break;
    }
}

-(void) SDKToUserCenter:(NSString *)info
{
    [self callQKFunction:FuncType_ENTER_USER_CENTER];
}

-(void) SDKCheckUpdate:(NSString *)url
{
    
}

-(void) SDKExitApp:(NSString *) info
{
}

-(id) GetPropertie:(NSString *) key
{
    if ([key isEqualToString:@"CHANNEL_ID"])
    {
        int channelType = [[SMPCQuickSDK defaultInstance] channelType];
        return @(channelType);
    }
    return [super GetPropertie:key];
}

// 调用QuickSDK扩展函数
-(BOOL) callQKFunction:(NSString *) funcType
{
    NSNumber* obj = [qkFuncTypeDict valueForKey:funcType];
    if (nil == obj)
    {
        return NO;
    }
    
    unsigned int intObj = [obj unsignedIntValue];
    SMPC_QUICK_SDK_FUNC_TYPE qkFuncType = (SMPC_QUICK_SDK_FUNC_TYPE)intObj;
    if ([[SMPCQuickSDK defaultInstance] isFunctionTypeSupported:qkFuncType])
    {
        switch (qkFuncType) {
            case SMPC_QUICK_SDK_FUNC_TYPE_ENTER_USER_CENTER:
                [[SMPCQuickSDK defaultInstance] enterUserCenter];
                return YES;

            case SMPC_QUICK_SDK_FUNC_TYPE_ENTER_BBS:
                [[SMPCQuickSDK defaultInstance] enterBBS];
                return YES;

            case SMPC_QUICK_SDK_FUNC_TYPE_SHOW_TOOLBAR:
                [[SMPCQuickSDK defaultInstance] showToolBar:SMPC_QUICK_SDK_TOOLBAR_TOP_LEFT];
                return YES;

            case SMPC_QUICK_SDK_FUNC_TYPE_HIDE_TOOLBAR:
                [[SMPCQuickSDK defaultInstance] hideToolBar];
                return YES;

            case SMPC_QUICK_SDK_FUNC_TYPE_ENTER_CUSTOMER_CENTER:
                [[SMPCQuickSDK defaultInstance] enterCustomerCenter];
                return YES;

            default:
                break;
        }
    }
    return NO;
}

// 向渠道提交用户信息。在创建游戏角色、进入游戏和角色升级3个地方调用此接口
-(void) setUserInfo:(BOOL)isCreateRole :(NSString *)info
{
    SMPCQuickSDKGameRoleInfo* qkRoleInfo = [QKGameInfo parseDetailRoleInfo:info];
    [[SMPCQuickSDK defaultInstance] updateRoleInfoWith:qkRoleInfo isCreate:isCreateRole];
}

// 监听QK相关通知
-(void) initQKNotifiers
{
    //初始化通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpInitResult:) name:kSmpcQuickSDKNotiInitDidFinished object:nil];
    //登陆
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpLoginResult:) name:kSmpcQuickSDKNotiLogin object:nil];
    //注销
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpLogoutResult:) name:kSmpcQuickSDKNotiLogout object:nil];
    //充值结果
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpRechargeResult:) name:kSmpcQuickSDKNotiRecharge object:nil];
}

// 初始化通知
-(void)smpcQpInitResult:(NSNotification *)notify {
    NSLog(@"QK初始化回调 notify:%@",notify);

    NSDictionary *userInfo = notify.userInfo;
    int errorCode = [userInfo[kSmpcQuickSDKKeyError] intValue];
    switch (errorCode) {
        case SMPC_QUICK_SDK_ERROR_NONE:
        {
            NSLog(@"QK 初始化成功");
            
            [BaseSdk SDKInitFinishCallBack:SDK_SUCCESS_CODE];
        }
            break;
            
        case SMPC_QUICK_SDK_ERROR_INIT_FAILED:
        default:
        {
            //初始化失败
            NSLog(@"QK 渠道初始化失败");
            
            [BaseSdk SDKInitFinishCallBack:SDK_FAILURE_CODE];
        }
            break;
    }
}

// 登陆/切换账号通知
-(void)smpcQpLoginResult:(NSNotification *)notify
{
    BOOL bSwitchAccount = nil != loginData ? YES : NO;
    NSString* key = !bSwitchAccount ? @"登陆账号" : @"切换账号";

    int error = [[[notify userInfo] objectForKey:kSmpcQuickSDKKeyError] intValue];
    if (error == 0)
    {
        NSLog(@"QK%@ 成功", key);

        NSDictionary *userInfo = [notify userInfo];
        NSString *uid = [[SMPCQuickSDK defaultInstance] userId];
        NSString *user_name = [[SMPCQuickSDK defaultInstance] userNick];
        NSString *user_token = userInfo[kSmpcQuickSDKKeyUserToken];
        
        if (nil != loginData)
        {
            [loginData release];
        }
        loginData = [NSString stringWithFormat:@"{\"user_id\":\"%@\",\"user_name\":\"%@\",\"token\":\"%@\"}", uid, user_name, user_token];
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
    else
    {
        NSLog(@"QK%@ 失败 error_code:%d", key, error);
        
        if (!bSwitchAccount)
        {
            // 登陆失败回调
            [BaseSdk SdKLoginPannelCallBack:SDK_FAILURE_CODE];
        }
    }
}

// 注销账号通知
-(void)smpcQpLogoutResult:(NSNotification *)notify
{
    NSDictionary *userInfo = notify.userInfo;
    int errorCode = [userInfo[kSmpcQuickSDKKeyError] intValue];
    switch (errorCode) {
        case SMPC_QUICK_SDK_ERROR_NONE:
        {
            NSLog(@"QK注销成功");
            
            // 注销成功
            if (nil != loginData)
            {
                [loginData release];
                loginData = nil;
            }
            
            // 注销成功回调
            [BaseSdk SdKLoginPannelCallBack:SDK_LOGOUT_CODE];
        }
            break;

        case SMPC_QUICK_SDK_ERROR_LOGOUT_FAIL:
        default:
        {
            //注销失败
            NSLog(@"注销失败");
        }
            break;
    }
}

// 支付通知
-(void)smpcQpRechargeResult:(NSNotification *)notify
{
    NSLog(@"QK支付结果%@", notify);

    NSDictionary *userInfo = notify.userInfo;
    int error = [[userInfo objectForKey:kSmpcQuickSDKKeyError] intValue];
    switch (error)
    {
        case SMPC_QUICK_SDK_ERROR_NONE:
        {
            NSString *orderID = userInfo[kSmpcQuickSDKKeyOrderId];
            NSString *cpOrderID = userInfo[kSmpcQuickSDKKeyCpOrderId];
            NSString *extraParams = userInfo[kSmpcQuickSDKKeyExtraParams] != nil ? userInfo[kSmpcQuickSDKKeyExtraParams] : @"";
            NSLog(@"QK支付成功, sdkOrderID:%@, cpOrderID:%@ extraParams:%@", orderID, cpOrderID, extraParams);
            
            NSMutableDictionary* jsonDict = [NSMutableDictionary dictionary];
            [jsonDict setObject:orderID forKey:@"sdk_order_id"];
            [jsonDict setObject:cpOrderID forKey:@"cp_order_id"];
            [jsonDict setObject:extraParams forKey:@"extra_params"];
            SBJsonWriter* jsonWriter = [[SBJsonWriter alloc] init];
            [jsonWriter autorelease];
            NSString* payData = [jsonWriter stringWithObject:jsonDict];
            
            // 支付成功回调
            [BaseSdk SDKTDFinishCallBack:SDK_SUCCESS_CODE :payData];
        }
            break;
    
        case SMPC_QUICK_SDK_ERROR_RECHARGE_CANCELLED:
        {
            NSLog(@"支付取消");

            // 支付取消回调
            [BaseSdk SDKTDFinishCallBack:SDK_CANCEL_CODE];
        }
            break;
            
        case SMPC_QUICK_SDK_ERROR_RECHARGE_FAILED:
        {
            // 支付失败
            NSString *orderID = userInfo[kSmpcQuickSDKKeyOrderId];
            NSString *cpOrderID = userInfo[kSmpcQuickSDKKeyCpOrderId];
            NSLog(@"QK支付失败, sdkOrderID:%@, cpOrderID:%@", orderID, cpOrderID);
            
            // 支付失败回调
            [BaseSdk SDKTDFinishCallBack:SDK_FAILURE_CODE];
        }
            break;
        
        default:
            break;
    }
}

/**
 AppDelegate相关事件处理
 **/

-(void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    NSLog(@"%s",__FUNCTION__);
    [[SMPCQuickSDK defaultInstance] applicationWillResignActive:application];
}

-(void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"%s",__FUNCTION__);
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    [[SMPCQuickSDK defaultInstance] applicationDidEnterBackground:application];
}

-(void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    [[SMPCQuickSDK defaultInstance] applicationWillEnterForeground:application];
}

-(void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    [[SMPCQuickSDK defaultInstance] applicationDidBecomeActive:application];
}

-(void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    [[SMPCQuickSDK defaultInstance] applicationWillTerminate:application];
}

-(void)didRegisterForRemoteNotificationsWithDeviceToken:(UIApplication *)application :(NSData *)deviceToken
{
    [[SMPCQuickSDK defaultInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

-(void)didFailToRegisterForRemoteNotificationsWithError:(UIApplication *)application :(NSError *)error
{
    [[SMPCQuickSDK defaultInstance] application:application didFailToRegisterForRemoteNotificationsWithError:error];
}

-(UIInterfaceOrientationMask)supportedInterfaceOrientationsForWindow:(UIApplication *)application :(UIWindow *)window
{
    [[SMPCQuickSDK defaultInstance] application:application supportedInterfaceOrientationsForWindow:window];
    return UIInterfaceOrientationMaskAll;
}

-(BOOL)handleOpenURL:(UIApplication *)application :(NSURL *)url
{
    [[SMPCQuickSDK defaultInstance] openURL:url application:application];
    return YES;
}

-(BOOL)openURL:(UIApplication *)application :(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    [[SMPCQuickSDK defaultInstance] openURL:url sourceApplication:sourceApplication application:application annotation:annotation];
    return YES;
}

-(BOOL)openURL:(UIApplication *)app :(NSURL *)url options:(NSDictionary<NSString*, id> *)options
{
    [[SMPCQuickSDK defaultInstance] openURL:url application:app options:options];
    return YES;
}

-(BOOL)continueUserActivity:(UIApplication *)application :(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray * restorableObjects))restorationHandler
{
    [[SMPCQuickSDK defaultInstance] application:application continueUserActivity:userActivity restorationHandler:restorationHandler];
    return YES;
}

-(void)dealloc
{
    if (nil != qkFuncTypeDict)
    {
        [qkFuncTypeDict release];
        qkFuncTypeDict = nil;
    }
    
    if (nil != loginData)
    {
        [loginData release];
        loginData = nil;
    }
    
    [super dealloc];
}

@end
