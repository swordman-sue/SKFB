/**\file
活动管理器
\author Kevin
\date 2017/1/5
\note
*/

#pragma once
#include "DataDefine.h"
#include "LimitActivity.h"
#include "SuperDiscount.h"
#include "FestivalActivity.h"
#include "AccurateRecommend.h"
#include "FacebookActivity.h"
#include "SevenDayLogin.h"
#include "RechargeReward.h"
#include "TotalRechargeReward.h"
#include "HotSaleGoods.h"
#include "CampRecruit.h"
#include "UnlockActivity.h"
#include "RechargeAndCostRankActivity.h"

class Player;
struct TQT_Config;
struct Activity_Config;
struct ActivityBase_Config;


class CActivityManager
{
public:
	CActivityManager();
	~CActivityManager();

	void Init(Player *pPlayer);

	void OnNewDay();

	// 登出
	void OnLogout();

	///// 签到模块
public:
	// 签到
	void OnNetSignIn();

	// 签到
	void OnNetSignInForTest();

	// 请求签到数据
	void OnNetSignInDataReq();

	// 发送签到数据
	void SendSignInData();

	// 领取豪华签到奖励
	void OnNetTakeLuxurySignReward(WORD wSignId);

	// 请求豪华签到数据
	void OnNetLuxurySignData();

	// 发送豪华签到数据
	void SendLuxurySignData();

	// 初始化豪华签到数据
	void InitLuxurySignInData();

	// 豪华签到充值
	void LuxurySignInRecharge(WORD wRechangeId);

	//// 迎财神(金币)
public:
	// 领取奖励请求
	void OnNetYCSReward();

	// 请求迎财神数据
	void OnNetYCSDataReq();

	// 发送迎财神数据
	void SendYCSData();

	//// 铜雀台(体力)
public:
	// 领取奖励请求
	void OnNetTQTReward();

	// 请求铜雀台数据
	void OnNetTQTDataReq();

	// 发送铜雀台数据
	void SendTQTData();

	// 是否铜雀台活动时间
	bool IsTQTActivityTime(int nTime);

	// 通过时间取铜雀台活动配置
	const TQT_Config * GetTQTCfgByTime(int nTime);

	// VIP礼包
public:
	// VIP礼包数据请求
	void OnNetVipGiftDataReq();

	// 每日礼包领取
	void OnNetVipDailyGiftTake(WORD wVipLevel);

	// 每周礼包购买
	void OnNetVipWeekGiftBuy(WORD wGiftId);

	// 发送VIP礼包数据
	void SendVipGiftData();

	// 刷新周礼包
	void RefreshWeekGift();

	// 领取今日推荐充值的奖励
	void OnNetTakeTodayRecommendReward();

	// 重置今日推荐充值数据
	void ResetTodayRecommendRechargeData();

	// 等级商店
public:
	// 等级商店活动数据请求
	void OnNetLevelShopActivityDataReq();
	
	// 角色等级提升
	void RoleLevelUp(WORD wFrom, WORD wTo);

	// 取等级商店数据
	const LevelShopActivityData* GetLevelShopActivityData(WORD wRoleLevel);

// 等级礼包
public:
	// 等级礼包数据请求
	void OnNetLevelGiftDataReq();

	// 等级礼包领取
	void OnNetTakeLevelGift(WORD wLevel);

	// 是否已领取等级礼包
	bool IsTakeLevelGift(WORD wLevel);

	// 开服基金
public:
	// 开服基金数据请求
	void OnNetOpenServerFundDataReq();

	// 开服基金购买
	void OnNetOpenServerFundBuy();

	// 开服基金奖励领取
	void OnNetOpenServerFundRewardTake(WORD wRewardId);

	// 全民福利领取
	void OnNetAllPeopleWelfareTake(WORD wWelfareId);

	// 发送开服基金数据
	void SendOpenServerFundData();

	// 是否已领取基金奖励
	bool IsTakeFundReward(WORD wRewardId);

	// 是否已领取全民福利
	bool IsTakeAllPeopleWelfare(WORD wWelfareId);

// 限时优惠活动
public:
	// 请求限时优惠活动数据
	void OnNetLimitPreferentialReq();

	// 领取限时优惠福利
	void OnNetTakeLimitPreferentialWelfare(WORD wWelfareId);

	// 发送限时优惠活动数据
	void SendLimitPreferentialData();

// 次日奖励
public:
	// 次日奖励数据请求
	void OnNetNextDayRewardData();

	// 领取次日奖励
	void OnNetTakeNextDayReward();

// 登录奖励
public:
	// 登录奖励数据请求
	void OnNetLoginRewardData();

	// 领取登录奖励请求
	void OnNetTakeLoginReward(WORD wDays);

	// 检测是否已领取登录奖励
	bool IsTakeLoginReward(WORD wDays);

// 在线奖励
public:
	// 在线奖励数据请求
	void OnNetOnlineRewardData();

	// 领取在线奖励请求
	void OnNetTakeOnlineReward();

	// 发送在线奖励数据
	void SendOnlineRewardData();

	// 取在线时间
	DWORD GetOnlineTime();


public:
	// 取活动数据ID
	WORD GetActivityDataId(int nSystemId);

	// 取活动天数
	WORD GetActivityDay(int nSystemId);

	// 是否有红点
	bool IsHaveRedPoint(WORD wSystemId);

	// 活动开始时间(取正在进行的活动开始时间)(todo::暂时不使用,接口没完成)
	//DWORD GetActivityStartTime(int nSystemId);

public:

	// 是否活动开放
	//static bool IsActivityOpen(int nSystemId, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime);

	// 是否活动开放
	static bool IsActivityOpen(int nSystemId, Player *pPlayer);

	// 是否活动开放
	static bool IsActivityOpen(const Activity_Config *pActivityCfg, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime);

	// 检测活动时间
	static bool CheckActivityTime(const ActivityBase_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 检测活动时间(临时功能使用)
	//static bool CheckActivityTimeEx(const ActivityBase_Config *pActivityCfg, DWORD dwServerId);

	// 取活动奖励日期
	static int GetActivityRewardDate(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 取活动奖励时间
	static int GetActivityRewardTime(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 是否活动结束
	static bool IsActivityOver(DWORD dwServerId, int nSystemId, WORD wDataId, DWORD dwCreateRoleTime);

	// 取限时产出倍数
	static int GetLimitProductMultiple(WORD wMapType);

	// 今日推荐充值回调
	void OnTodayRecommendRechargeCallback();

public:
	// 取限时活动逻辑类
	CLimitActivity& GetLimitActivity() { return m_LimitActivity; };
	CSuperDiscount& GetSuperDiscount() { return m_SuperDiscount; };
	CNewSuperDiscount& GetNewSuperDiscount() { return m_NewSuperDiscount; };
	CFestivalActivity& GetFestivalActivity() { return m_FestivalActivity; };
	CAccurateRecommend& GetAccurateRecommend() { return m_AccurateRecommend; };
	CFacebookActivity& GetFacebookActivity() { return m_FacebookActivity; };
	CSevenDayLogin& GetSevenDayLogin() { return m_SevenDayLogin; };
	CRechargeReward& GetRechargeReward() { return m_RechargeReward; };
	CTotalRechargeReward& GetTotalRechargeReward() { return m_TotalRechargeReward; };
	CHotSaleGoods& GetHotSaleGoods() { return m_HotSaleGoods; };
	CCampRecruit& GetCampRecruit() { return m_CampRecruit; };
	CUnlockActivity& GetUnlockActivity() { return m_UnlockActivity; };
	CRechargeAndCostRankActivity& GetRechargeAndCostRankActivity() { return m_RechargeAndCostRankActivity; };

private:
	Player *m_pPlayer;
	SignInData *m_pSignInData;
	LoginRewardData *m_pLoginRewardData;
	PlayerLuxurySignData *m_pLuxurySignInData;
	PlayerActivityData *m_pActivityInfo;

	CLimitActivity m_LimitActivity; // 限时活动逻辑类
	CSuperDiscount m_SuperDiscount;
	CNewSuperDiscount m_NewSuperDiscount;
	CFestivalActivity m_FestivalActivity; // 节日活动
	CAccurateRecommend m_AccurateRecommend;
	CFacebookActivity m_FacebookActivity;
	CSevenDayLogin m_SevenDayLogin;
	CRechargeReward m_RechargeReward;
	CTotalRechargeReward m_TotalRechargeReward;
	CHotSaleGoods m_HotSaleGoods;
	CCampRecruit m_CampRecruit;
	CUnlockActivity m_UnlockActivity;
	CRechargeAndCostRankActivity m_RechargeAndCostRankActivity;
};