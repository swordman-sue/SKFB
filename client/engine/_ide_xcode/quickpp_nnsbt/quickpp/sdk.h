
#import <Foundation/Foundation.h>
#import "BaseSdk.h"
#import "CommonCrypto/CommonDigest.h"

@interface Sdk : BaseSdk
{
@private
    NSString* loginData;
    NSMutableDictionary* qkFuncTypeDict;
}

@property (nonatomic, assign) NSString* loginData;

-(void) SDKInit:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
-(void) SDKLogin:(NSString *)info;
-(void) SDKLogout:(NSString *)info;
-(void) SDKSwitchAccount:(NSString *)info;
-(void) SDKTD:(NSString *) info;
-(void) SDKCommonHandle:(NSString *)info;
-(void) SDKToUserCenter:(NSString *)info;
-(void) SDKCheckUpdate:(NSString *)url;
-(void) SDKExitApp:(NSString *) info;
-(id) GetPropertie:(NSString *) key;

/**
 AppDelegate相关事件处理
 **/

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


@end
