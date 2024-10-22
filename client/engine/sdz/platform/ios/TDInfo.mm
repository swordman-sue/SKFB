//
//  TDInfo.mm
//
//  Created by zlgame on 2017/10/23.
//
//

#import "TDInfo.h"
#import "JSONKit.h"

@implementation TDInfo

+(TDInfo*) parseTDInfo:(NSString*) jsondata
{
    NSDictionary* tdInfoDict = [jsondata objectFromJSONString];

    TDInfo* tdInfo = [[TDInfo alloc] init];
    [tdInfo autorelease];

    tdInfo.orderID = [tdInfoDict valueForKey:@"order_id"];
    tdInfo.goodsID = [tdInfoDict valueForKey:@"recharge_id"];
    tdInfo.rawRechargeId = [tdInfoDict valueForKey:@"raw_recharge_id"];
    tdInfo.goodsName = [tdInfoDict valueForKey:@"goods_name"];
    tdInfo.goodsDescription = [tdInfoDict valueForKey:@"goods_desc"];
    tdInfo.goodsCurrencyName = [tdInfoDict valueForKey:@"recharge_name"];
    tdInfo.goodsNum = [[tdInfoDict valueForKey:@"recharge_num"] intValue];

    tdInfo.goodsAmount = [[tdInfoDict valueForKey:@"recharge_amount"] intValue];
    tdInfo.goodsAmountEx = [[tdInfoDict valueForKey:@"recharge_amount_ex"] intValue];

    tdInfo.money = [[tdInfoDict valueForKey:@"money"] doubleValue];
    tdInfo.moneyType = [[tdInfoDict valueForKey:@"money_type"] doubleValue];
    tdInfo.notifyUrl = [tdInfoDict valueForKey:@"notify_url"];
    tdInfo.extrasParams = [tdInfoDict valueForKey:@"extra_params"];
    tdInfo.account = [tdInfoDict valueForKey:@"account"];
    tdInfo.serverId = [tdInfoDict valueForKey:@"server_id"];
    tdInfo.serverName = [tdInfoDict valueForKey:@"server_name"];
    tdInfo.roleID = [tdInfoDict valueForKey:@"role_id"];
    tdInfo.roleName = [tdInfoDict valueForKey:@"role_name"];
    tdInfo.roleLevel = [tdInfoDict valueForKey:@"role_level"];
    tdInfo.vipLevel = [tdInfoDict valueForKey:@"vip_level"];
    tdInfo.sign = [tdInfoDict valueForKey:@"sign"];
    tdInfo.tdTime = [tdInfoDict valueForKey:@"td_time"];
    tdInfo.h5url = [tdInfoDict valueForKey:@"h5_url"];
    
    tdInfo.ip = [tdInfoDict valueForKey:@"ip"];
        
    return tdInfo;
}

@end
