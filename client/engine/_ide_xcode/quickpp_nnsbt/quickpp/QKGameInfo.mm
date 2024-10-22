//
//  QKGameInfo.m
//  quickpp
//
//  Created by zlgame on 2017/10/23.
//
//

#import "QKGameInfo.h"
#import "RoleInfo.h"
#import "TDInfo.h"

@implementation QKGameInfo

+(SMPCQuickSDKGameRoleInfo*) parseBaseRoleInfo:(NSString*) jsonData
{
    RoleInfo* roleInfo = [RoleInfo parseRoleInfo:jsonData];
    
    SMPCQuickSDKGameRoleInfo* qkRoleInfo = [[SMPCQuickSDKGameRoleInfo alloc] init];
    [qkRoleInfo autorelease];

    qkRoleInfo.serverId = roleInfo.serverID;
    qkRoleInfo.serverName = roleInfo.serverName;
    qkRoleInfo.gameRoleName = roleInfo.roleName;
    qkRoleInfo.gameRoleID = roleInfo.roleID;
    qkRoleInfo.gameUserLevel = roleInfo.roleLevel;
    qkRoleInfo.vipLevel = roleInfo.vipLevel;
    qkRoleInfo.gameUserBalance = roleInfo.gameMoney;
    qkRoleInfo.partyName = roleInfo.guildName;
    
    return qkRoleInfo;
}

+(SMPCQuickSDKGameRoleInfo*) parseDetailRoleInfo:(NSString*) jsonData
{
    return [QKGameInfo parseBaseRoleInfo:jsonData];
}

+(SMPCQuickSDKPayOrderInfo*) parseOrderInfo:(NSString*) jsonData
{
    TDInfo* tdInfo = [TDInfo parseTDInfo:jsonData];
    
    SMPCQuickSDKPayOrderInfo* qkPayInfo = [[SMPCQuickSDKPayOrderInfo alloc] init];
    [qkPayInfo autorelease];
    
    qkPayInfo.cpOrderID = tdInfo.orderID;
    qkPayInfo.goodsID = tdInfo.goodsID;
    qkPayInfo.productName = tdInfo.goodsName;
    qkPayInfo.count = tdInfo.goodsNum;
    qkPayInfo.amount = tdInfo.money;
    qkPayInfo.extrasParams = tdInfo.extrasParams;
    
    return qkPayInfo;
}

@end
