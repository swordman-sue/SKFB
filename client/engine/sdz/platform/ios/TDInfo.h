//
//  TDInfo.h
//
//  Created by zlgame on 2017/10/23.
//
//

#ifndef TDInfo_h
#define TDInfo_h

#import <Foundation/Foundation.h>

@interface TDInfo : NSObject
{
}

@property (nonatomic, assign) NSString* orderID;
@property (nonatomic, assign) NSString* goodsID;
@property (nonatomic, assign) NSString* goodsName;
@property (nonatomic, assign) NSString* rawRechargeId;
// 商品对应的游戏币名
@property (nonatomic, assign) NSString* goodsCurrencyName;
// 商品数量
@property (nonatomic, assign) int goodsNum;
// 商品对应的游戏币数量
@property (nonatomic, assign) int goodsAmount;
// 商品赠送的游戏币数量
@property (nonatomic, assign) int goodsAmountEx;
// 商品对应的货币价格
@property (nonatomic, assign) double money;
// 商品对应的货币类型
@property (nonatomic, assign) int moneyType;
// sdk支付成功后，通知服务端发货的回调地址
@property (nonatomic, assign) NSString* notifyUrl;
@property (nonatomic, assign) NSString* extrasParams;
@property (nonatomic, assign) NSString* account;
@property (nonatomic, assign) NSString* serverId;
@property (nonatomic, assign) NSString* serverName;
@property (nonatomic, assign) NSString* roleID;
@property (nonatomic, assign) NSString* roleName;
@property (nonatomic, assign) NSString* roleLevel;
@property (nonatomic, assign) NSString* ip;
@property (nonatomic, assign) NSString* vipLevel;
@property (nonatomic, assign) NSString* goodsDescription;
@property (nonatomic, assign) NSString* sign;
@property (nonatomic, assign) NSString* tdTime;
@property (nonatomic, assign) NSString* h5url;


+(TDInfo*) parseTDInfo:(NSString*) jsondata;

@end

#endif /* TDInfo_h */
