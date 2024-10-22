/**\file
竞技场管理器
\author Kevin
\date 2016/10/18
\note
*/

#pragma once
#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "ToolKit\Timer.h"
class Player;
class CRankManager;

typedef vector<ArenaRankData> ArenaRankMap;
typedef map<DWORD, ArenaRankMap> ArenaAllRankMap;
typedef map<DWORD, list<ArenaBattleRecord>> ArenaBattleRecordMap;

class CArenaManager : public Singleton<CArenaManager>
{
	friend CRankManager;
public:
	CArenaManager();
	~CArenaManager();

	void Init();

	void Update();

	// 存储数据
	void SaveDataToDB(DWORD dwServerId);

	// 存储所有数据
	void SaveAllData();

	// 加载数据
	void LoadDataFromDB(DWORD dwServerId, const char *pData, int size);

	// 加载数据请求
	void LoadDataRequest();
	
	// 存储战斗记录数据
	void SaveBattleRecord(DWORD dwRoleId);

	// 加载战斗记录数据
	void LoadBattleRecordFormDB(const PlayerArenaBattleRecord *pPlayerBattleRecordList, int nPlayerBattleRecordNum);

public:
	// 请求挑战目标列表
	void OnNetTargetList(Player *pPlayer);

	// 请求战报
	void OnNetBattleRecordList(Player *pPlayer);

	// 战斗开始
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId, DWORD dwIsSweep);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

public:
	// 取玩家的排行
	WORD GetPlayerRank(DWORD dwServerId, DWORD dwRoleId);

	// 取玩家的历史排行
	WORD GetPlayerHistoryRank(DWORD dwServerId, DWORD dwRoleId);

	// 取挑战目标列表
	bool GetTargetList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutTargetList);

	// 发送目标列表
	void SendTargetList(Player *pPlayer);

	// 取排行榜列表
	void GetRankList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutRankList, WORD wMaxNum);

	// 处理排行奖励
	void DoRankAward();

	// 处理首次排名奖励
	//void DoFirstRankAward();

	// 处理7天排行奖励
	void DoSevenDayRankAward(DWORD dwServerId);

	// 取战斗记录数量
	int GetPlayerBattleRecordNum() { return m_vBattleRecordList.size(); };

	// 添加玩家战斗记录
	void AddPlayerBattleRecord(const PlayerArenaBattleRecord *pBattleRecord);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// 重置排行榜数据
	void ResetRankData(WORD wServerId);

protected:
	// 取排行榜列表
	ArenaRankMap* GetServerRankList(DWORD dwServerId);

	// 取玩家排行数据
	ArenaRankData* GetPlayerRankData(DWORD dwServerId, DWORD dwRoleId);

	// 添加战报
	void AddBattleRecord(DWORD dwRoleId, const ArenaBattleRecord &tRecord);

	// 添加战报
	void AddBattleRecord(DWORD dwSrcRoleId, DWORD dwTargetId, WORD wSrcRank, WORD wTargetRank, const char *pszSrcName, 
		const char *pszTargetName, WORD wScrHeadIcon, WORD wTargetHeadIcon, WORD wScrLevel, WORD wTargetLevel, BYTE byIsWin);

	// 更新排行榜
	void UpdateRank(DWORD dwServerId);
private:

	ArenaAllRankMap m_RankList;	// 排行列表

	ArenaAllRankMap m_RankListForSendReward; // 发奖励使用的

	ArenaBattleRecordMap m_vBattleRecordList; // 战报列表

	UINT64 m_lProccessTime;

	DWORD m_dwDoRewardTime;

	bool m_bRankUpdate;	// 是否排行榜已更新
	bool m_bAlreadyLoadData;	// 是否已加载数据

	UINT64 m_lProceesSaveTime;

	vector<DWORD> m_SaveDataServerList;	// 存储数据的服务器列表

	CDateTimers m_RankRewardTimer;

};
