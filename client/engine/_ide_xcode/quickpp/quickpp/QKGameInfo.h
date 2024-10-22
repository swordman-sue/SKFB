//
//  QKGameInfo.h
//  quickpp
//
//  Created by zlgame on 2017/10/23.
//
//

#ifndef QKGameInfo_h
#define QKGameInfo_h

#import <Foundation/Foundation.h>
#import <SMPCQuickSDK/SMPCQuickSDK.h>

//QuickSDK中用到的游戏信息
@interface QKGameInfo : NSObject

+(SMPCQuickSDKGameRoleInfo*) parseBaseRoleInfo:(NSString*) jsonData;
+(SMPCQuickSDKGameRoleInfo*) parseDetailRoleInfo:(NSString*) jsonData;
+(SMPCQuickSDKPayOrderInfo*) parseOrderInfo:(NSString*) jsonData;

@end

#endif /* QKGameInfo_h */
