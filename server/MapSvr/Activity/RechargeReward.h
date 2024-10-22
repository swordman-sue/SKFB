/**\file
充值回馈
\author Kevin
\date 2018/2/1
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CRechargeReward
{
public:
	CRechargeReward(){};
	~CRechargeReward(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// 请求充值回馈数据
	void OnNetRechargeRewardData();

	// 领取充值奖励
	void OnNetTakeRechargeReward(WORD wDay);

	// 领取额外奖励
	void OnNetTakeExtraReward();

public:
	// 充值回调
	void OnRechargeCallback(WORD wRechargeId);

	// 是否有红点
	bool IsHaveRedPoint();

	// 发送充值回馈数据
	void SendRechargeRewardData();

	// 取每日数据
	WORD GetDailyData(DWORD dwDay);

	// 设置每日数据
	void SetDailyData(DWORD dwDay, WORD wValue);

	// 取已充值的天数
	WORD GetAlreadyRechargeDay();

protected:
	
	// 更新活动数据
	void UpdateActivityData();

private:
	Player *m_pPlayer;
	RecahrgeRewardData *m_pRechargeRewardData;
	CActivityManager *m_pActivityMgr;

	enum
	{
		REWARD_STATUS_NOT_YET = 0,	// 还没充值
		REWARD_STATUS_RECHARGE = 1,		// 已充值(可领取)
		REWARD_STATUS_TAKE = 2,		// 已领取
	};
};