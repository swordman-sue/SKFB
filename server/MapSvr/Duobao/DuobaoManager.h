/**\file
夺宝管理器
\author Kevin
\date 2016/9/19
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

// 
class CDuobaoManager
{
public:
	CDuobaoManager();
	~CDuobaoManager();

	void Init(Player *pPlayer);

public:
	// 请求夺宝数据
	void OnNetDuobaoData();

	// 请求挑战目标列表(todo::有些消耗)
	void OnNetTargetList(WORD wTreasureInfoId, WORD wTreasureItemId);

	// 请求5次夺宝
	void OnNetDuoBaoFiveTimes(WORD wTreasureInfoId, WORD wTreasureItemId, WORD wRequestTimes = 5);

	// 领取进度奖励
	void OnNetTakePregrossReward(WORD *pPregrossIdList, WORD wPregrossIdNum);
public:
	// 发送夺宝数据
	void SendDuobaoData();

	// 挑战开始
	bool BattleStar(DWORD dwTargetId, bool bIsPlayer, WORD wTreasureInfoId, WORD wTreasureItemId);

	// 挑战结束
	bool BattleOver(bool bIsWin);

	// 设置免战结束时间 
	void SetPeaceOverTime(DWORD dwTime);

	// 是否免战中
	bool IsInPeace();

protected:
	bool DoBattleOver(DWORD dwTargetId, WORD wTreasureInfoId, WORD wTreasureItemId, BYTE byIsPlayer, bool bIsWin, BYTE &byIsGetItem);

	// 取夺宝次数
	WORD GetDoubaoTimes(WORD wQuality);

	// 设置夺宝次数
	void SetDoubaoTimes(WORD wQuality, WORD wTimes);

	// 是否进度奖励记录
	bool IsPregrossRewardRecord(WORD wPregrossId);

	// 添加进度奖励记录
	void AddPregrossRewardRecord(WORD wPregrossId);
private:

	Player *m_pPlayer;
	PlayerDuobaoInfo *m_pPlayerDuobaoInfo;
};