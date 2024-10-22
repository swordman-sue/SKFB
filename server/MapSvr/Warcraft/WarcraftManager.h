
/**\file
魔兽争霸管理器
\author Kevin
\date 2016/12/31
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "..\Arena\ArenaManager.h"
class Player;


class CWarcraftManager : public Singleton<CWarcraftManager>
{
public:
	CWarcraftManager();
	~CWarcraftManager();

	void Init();

	void OnNewDay();

	void Update();

public:

	// 请求争霸赛数据
	void OnNetWarcraftData(Player *pPlayer);

	// 购买挑战次数
	void OnNetBuyBattleTimes(Player *pPlayer);

	// 请求挑战目标
	void OnNetBattleTargetList(Player *pPlayer);

	// 请求排行榜列表
	void OnNetRankList(Player *pPlayer);

	// 助威
	void OnNetCheer(Player *pPlayer, DWORD dwTargetId);
public:

	// 开始战斗
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// 结束战斗
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

public:

	// 发送争霸赛数据
	void SendWarcraftData(Player *pPlayer);

	// 发送目标列表
	void SendTargetList(Player *pPlayer);

	// 取挑战目标列表
	bool GetTargetList(DWORD dwRoleId, vector<WarcraftRankData> *pOutTargetList);

	// 添加竞技场排行榜数据
	void AddArenaRankData(DWORD dwRoleId, WORD wRank);

protected:
	// 取玩家排行数据
	WarcraftRankData* GetPlayerRankData(DWORD dwRoleId);

private:
	
	// 竞技场排行榜玩家列表
	map<DWORD, WORD> m_ArenaRankList; 

	// 争霸赛排行榜玩家列表
	vector<WarcraftRankData> m_RankList;
};