//
//  RoleInfo.h
//
//  Created by zlgame on 2017/10/23.
//
//

#ifndef RoleInfo_h
#define RoleInfo_h

#import <Foundation/Foundation.h>

@interface RoleInfo : NSObject
{
}

@property (nonatomic, assign) NSString* account;
@property (nonatomic, assign) NSString* serverID;
@property (nonatomic, assign) NSString* serverName;
@property (nonatomic, assign) NSString* roleName;
@property (nonatomic, assign) NSString* roleID;
@property (nonatomic, assign) NSString* roleLevel;
@property (nonatomic, assign) NSString* vipLevel;
@property (nonatomic, assign) NSString* vipExp;
//游戏货币(充值获得）
@property (nonatomic, assign) NSString* gameMoney;
//游戏货币II
@property (nonatomic, assign) NSString* gameMoneyII;
@property (nonatomic, assign) NSString* guildID;
@property (nonatomic, assign) NSString* guildName;
@property (nonatomic, assign) NSString* roleCreateTime;
@property (nonatomic, assign) NSString* sex;
@property (nonatomic, assign) NSString* power;
@property (nonatomic, assign) NSString* guildRoleID;
@property (nonatomic, assign) NSString* guildRoleName;
@property (nonatomic, assign) NSString* profession;
@property (nonatomic, assign) NSString* professionName;
@property (nonatomic, assign) NSString* friendList;
@property (nonatomic, assign) NSString* onlineTime;
@property (nonatomic, assign) NSString* levelupTime;

+(RoleInfo*) parseRoleInfo:(NSString*) jsondata;

@end

#endif /* RoleInfo_h */
