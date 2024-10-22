
#import <Foundation/Foundation.h>
#import "BaseSdk.h"
#import "CommonCrypto/CommonDigest.h"
#import "platform/ios/TDInfo.h"

@interface Sdk : BaseSdk
{
@private
    NSString* loginData;
    
    NSMutableDictionary* FuncTypeDict;
}

-(void) SDKInit:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
-(void) SDKLogin:(NSString *)info;
-(void) SDKLogout:(NSString *)info;
-(void) SDKSwitchAccount:(NSString *)info;
-(void) SDKTD:(NSString *) info;
-(void) SDKCommonHandle:(NSString *)info;
-(void) SDKToUserCenter:(NSString *)info;

@end
