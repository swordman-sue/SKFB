/**\file
奖励中心管理器
\author Kevin
\date 2016/11/4
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;


class CRewardCenterManager
{
public:
	CRewardCenterManager();
	~CRewardCenterManager();

	void Init(Player *pPlayer);

public:
	// 请求奖励列表
	void OnNetRewardList();

	// 发送奖励列表
	void SendRewardList();

	// 领取奖励
	void OnNetRewardGet(DWORD *pRewardIdxList, BYTE byRewardNum);

	// 添加奖励
	static void AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward);

	// 添加奖励
	void AddReward(const RewardData &tReward, bool bNotify = true);

	// 取奖励
	bool GetReward(WORD wRewardInfoId, DWORD dwValue1, DWORD dwValue2, vector<Item> &vItemList, vector<MoneyData> &vCurrencyList, DWORD &dwTitleId);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

protected:
	// 取奖励数据
	RewardData* GetRewardData(DWORD dwRewardUId);

	// 删除奖励数据
	void RemoveRewardData(DWORD dwRewardUId);

	DWORD GetNewId();
private:
	DWORD m_dwSeriesId; // 系列ID
	RewardData m_tTmpRewardData;
	Player *m_pPlayer;
	PlayerRewardData *m_pRewardInfo;
};