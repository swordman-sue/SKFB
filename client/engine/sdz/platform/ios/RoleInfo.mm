//
//  RoleInfo.mm
//
//  Created by zlgame on 2017/10/23.
//
//

#import "RoleInfo.h"
#import "JSONKit.h"
#import <Foundation/Foundation.h>

@implementation RoleInfo

+(RoleInfo*) parseRoleInfo:(NSString*) jsondata
{
    NSDictionary* roleInfoDict = [jsondata objectFromJSONString];
    
    RoleInfo* roleInfo = [[RoleInfo alloc] init];
    [roleInfo autorelease];

    roleInfo.account = [roleInfoDict valueForKey:@"account"];// 账号
    roleInfo.serverID = [roleInfoDict valueForKey:@"server_id"];// 服务器ID，其值必须为数字字符串
    roleInfo.serverName = [roleInfoDict valueForKey:@"server_name"];// 服务器名称
    roleInfo.roleName = [roleInfoDict valueForKey:@"role_name"];// 角色名称
    roleInfo.roleID = [roleInfoDict valueForKey:@"role_id"];// 角色ID
    roleInfo.roleLevel = [roleInfoDict valueForKey:@"role_level"];// 等级
    roleInfo.vipLevel = [roleInfoDict valueForKey:@"vip_level"];// VIP等级
    roleInfo.vipExp = [roleInfoDict valueForKey:@"vip_exp"];// VIP经验
    roleInfo.gameMoney = [roleInfoDict valueForKey:@"game_money"];// 角色现有金额
    roleInfo.gameMoneyII = [roleInfoDict valueForKey:@"game_money_ii"];// 角色现有金额
    roleInfo.guildName = [roleInfoDict valueForKey:@"guild_name"];// 公会名
    roleInfo.roleCreateTime = [roleInfoDict valueForKey:@"role_create_time"];// UC与1881渠道必传，值为10位数时间戳
    roleInfo.guildID = [roleInfoDict valueForKey:@"guild_id"];// 360渠道参数，设置帮派id，必须为整型字符串
    roleInfo.sex = [roleInfoDict valueForKey:@"sex"];// 360渠道参数
    roleInfo.power = [roleInfoDict valueForKey:@"battle_value"];// 360渠道参数，设置角色战力，必须为整型字符串
    roleInfo.guildRoleID = [roleInfoDict valueForKey:@"guild_role_id"];// 360渠道参数，设置角色在帮派中的id
    roleInfo.guildRoleName = [roleInfoDict valueForKey:@"guild_role_name"];// 360渠道参数，设置角色在帮派中的名称
    roleInfo.profession = [roleInfoDict valueForKey:@"profession"];// 360渠道参数，设置角色职业id，必须为整型字符串
    roleInfo.professionName = [roleInfoDict valueForKey:@"profession_name"];// 360渠道参数，设置角色职业名称
    roleInfo.friendList = [roleInfoDict valueForKey:@"friend_list"];// 360渠道参数，设置好友关系列表
    roleInfo.onlineTime = [roleInfoDict valueForKey:@"online_time"];
    roleInfo.levelupTime = [roleInfoDict valueForKey:@"levelup_time"];//角色等级变化时间
    return roleInfo;
}

@end
