/**\file
累充奖励
\author Kevin
\date 2018/2/7
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CTotalRechargeReward
{
public:
	CTotalRechargeReward(){};
	~CTotalRechargeReward(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// 请求累充数据
	void OnNetTotalRechargeData();

	// 领取累充奖励
	void OnNetTakeTotalRechargeReward(WORD wRewardId);

public:
	// 发送累充数据
	void SendTotalRechargeData();

	// 检测是否奖励记录
	bool IsRewardRecord(WORD wRewardId);

	// 添加奖励记录
	void AddRewardRecord(WORD wRewardId);

	// 充值通知事件
	void OnRechargeNotify(DWORD dwAmount);

	// 更新活动数据
	void UpdateActivityData();

private:
	Player *m_pPlayer;
	TotalRechargeRewardData *m_pTotalRechargeRewardData;
	CActivityManager *m_pActivityMgr;
};