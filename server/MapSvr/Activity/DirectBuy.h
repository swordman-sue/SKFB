/**\file
直购活动逻辑类
\author Kevin
\date 2017/8/31
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CDirectBuy
{
public:
	CDirectBuy(){};
	~CDirectBuy(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// 请求超值折扣数据
	void OnNetSuperDiscountData();

	// 领取购买奖励
	void OnNetTakeBuyReward();

	// 领取累积奖励
	void OnNetTakeAccumulateReward();

public:
	// 充值回调
	void OnRechargeCallback();

	// 是否有红点
	bool IsHaveRedPoint();

protected:
	// 发送超值折扣数据
	void SendSuperDiscountData();

	// 更新数据
	void UpdateSuperDiscountData();

private:
	Player *m_pPlayer;
	SuperDiscountData *m_pSuperDiscountData;
	CActivityManager *m_pActivityMgr;

	enum
	{
		REWARD_STATUS_NOT_YET = 0,	// 还没购买
		REWARD_STATUS_BUY = 1,		// 已购买(可领取)
		REWARD_STATUS_TAKE = 2,		// 已领取
	};
};