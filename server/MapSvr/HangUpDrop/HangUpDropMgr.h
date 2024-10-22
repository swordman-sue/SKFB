/**\file
挂机掉落管理器
\author Kevin
\date 2019/03/20
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CHangUpDropMgr
{
public:
	CHangUpDropMgr();
	~CHangUpDropMgr();

	void Init(Player *pPlayer);

public:

	// 请求挂机掉落数据
	void OnNetHangUpDropData();

	// 请求领取奖励
	void OnNetObtainReward();

protected:

	// 发送挂机掉落数据
	void SendHangUpDropData();

	// 添加奖励物品数据
	void AddRewardItem(DWORD dwItemId, DWORD dwItemNum);

	// 检测并初始化掉落数据
	void CheckAndInitDropData();

private:
	Player *m_pPlayer;
	PlayerHangUpDropData *m_pHangUpDropData;

	// 临时奖励物品列表
	map<DWORD, DWORD> m_TmpRewardItemList;
};