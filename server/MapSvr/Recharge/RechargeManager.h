/**\file
充值逻辑管理器
\author Kevin
\date 2017/03/02
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
struct Recharge_Config;


class CRechargeManager
{
public:
	CRechargeManager();
	~CRechargeManager();

	void Init(Player *pPlayer);

	void OnNewDay();

public:
	// 请求充值数据
	void OnNetRechargeData();

	// 充值请求(此接口已经不使用,目前只做为GM使用)
	void OnNetRecharge(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId);

	// 充值请求(购买)
	void OnNetRechargeEx(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId, DWORD dwContext);

	// 请求月卡数据
	void OnNetMonthCardData();

	// 月卡购买(todo::目前已无效)
	void OnNetMonthCardBuy(WORD wCardId);

	// 领取月卡奖励
	void OnNetTakeMonthCardReward(WORD wCardId);

	// 领取限时充值奖励
	void OnNetTakeLimitRechargeReward(WORD wDataId);

	// 请求订单号
	void OnNetOrderId(BYTE byRechargeType, WORD wRechargeId, DWORD dwContext);

	// 充值成功上报
	void OnNetRechargeSuccessReport(WORD wRechargeId, DWORD dwChannel, const char *pszIP, const char *pszOrderId);

	// 请求订单号
	void OnNetOrderIdFromYD(DWORD dwGuid);


public:
	// 充值请求回调
	static void OnNetRechargeCallbackEx(DWORD dwGuid, const RechargeCallbackData &callbackData, bool bIsNeedResp = true);

	// YGame充值请求回调
	static void OnNetRechargeCallbackForYGame(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nAmount, const char *pszPaymentId);

	// YGame提现通知
	static void OnNetYGameCashbackNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nYCoin, DWORD dwDiamond);

	// YGame奖励通知
	static void OnNetYGameBonusNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, DWORD dwDiamond);

	// 充值请求回调
	bool OnNetRechargeCallback(const RechargeCallbackData &callbackData);

	// 超值折扣充任回调(走通用逻辑)
	bool OnNetSuperDiscountCallback(const RechargeCallbackData &callbackData);

	// 新超值折扣充任回调(走通用逻辑)
	bool OnNetNewSuperDiscountCallback(const RechargeCallbackData &callbackData);

	// 普通充值回调
	bool OnNetCommonRechargeCallback(const RechargeCallbackData &callbackData);

	// 月卡购买回调
	bool OnNetMonthCardBuyCallback(const RechargeCallbackData &callbackData);

	// 直购优惠回调(走通用逻辑)
	bool OnNetDirectBuyCallback(const RechargeCallbackData &callbackData);

	// 今日推荐回调(走通用逻辑)
	bool OnNetTodayRecommendCallback(const RechargeCallbackData &callbackData);

	// 精准推荐回调(走通用逻辑)
	bool OnNetAccurateRecommendCallback(const RechargeCallbackData &callbackData);

	// 热卖商品(走通用逻辑)
	bool OnNetHotSaleGoodsCallback(const RechargeCallbackData &callbackData);

	// 处理充值回调
	bool DoRechargeCallback(const RechargeCallbackData &callbackData, bool bAddDiamond = false);

	// 发送充值数据
	void SendRechargeData();

	// 发送月卡数据
	void SendMonthCardData();

	// 添加充值数据
	void AddRechargeData(WORD wRechargeId);

	// 是否首充
	//bool IsFirstRecharge(WORD wRechargeId);

	// 添加月卡数据
	void AddMonthCardData(const MonthCard *pMonthCard);

	// 取充值次数
	WORD GetRechargeTimes(WORD wRechargeId);

	// 是否有红点
	bool IsHaveRedPoint(WORD wSystemId);

	// 删除过期月卡
	void DeleteExpiredMonthCard();

	// 是否订单记录
	bool IsOrderRecord(DWORD dwOrderId);

	// 添加订单记录
	void AddOrderRecord(DWORD dwOrderId);

	// 存储充值记录
	void SaveRechargeRecord(WORD wRechargeId, WORD wRechargeType, int nAmount, int nRechargeDiamond, const char *pszPayTime, const char *pszSDKOrderId, WORD wFrom);

	// 检测AppleIapID
	bool CheckAppleIapId(const RechargeCallbackData &callbackData);

	// 检测充值结果
	bool CheckRechargeResult(int nCfgAmount, const RechargeCallbackData &callbackData);

	// 是否今日首充
	bool IsTodayFirstRecharge();

	// 是否首充
	bool IsFirstRecharge();

	// 越南版本使用
protected:
	// 月卡购买回调
	bool DoMonthCard(WORD wCardId);

	// 普通充值
	bool DoCommonRecharge(const Recharge_Config *pRechargeCfg);

	// 超值折扣充任回调(走通用逻辑)
	bool DoNetSuperDiscount(const Recharge_Config *pRechargeCfg);

	// 新超值折扣充任回调
	bool DoNetNewSuperDiscount(const Recharge_Config *pRechargeCfg);

	// 直购优惠回调
	bool DoDirectBuy(const Recharge_Config *pRechargeCfg, WORD wGoodsId);

	// 今日推荐回调
	bool DoTodayRecommend(const Recharge_Config *pRechargeCfg);

	// 精准推荐回调
	bool DoAccurateRecommend(const Recharge_Config *pRechargeCfg, WORD wGoodsId);

	// 热卖商品
	bool DoHotSaleGoods(const Recharge_Config *pRechargeCfg, WORD wGoodsId);



	// 处理充值回调
	bool DoRechargeCallbackEx(const Recharge_Config *pRechargeCfg);

protected:

	// 取月卡数据
	MonthCard* GetMonthCard(WORD wMonthCardId);

	// 查找普通充值数据
	CommonRecharge* FindCommonRecharge(WORD wRechargeId);

	// 领取充值额度
	DWORD GetRechargeAmount(WORD wRechargeId, BYTE byIsMyCard = false);

	// 领取充值额度
	DWORD GetRechargeDiamond(WORD wRechargeId, BYTE byIsMyCard = false);
private:

	Player *m_pPlayer;
	RoleRechargeData *m_pRechargeData;
	DWORD m_dwSeriesId; // 系列ID
	char m_szOrderId[MAX_ORDER_ID_LEN];

	// 最后一次请求订单时间
	DWORD m_dwLastRequestOrderTime;

	DWORD m_dwContext;
};