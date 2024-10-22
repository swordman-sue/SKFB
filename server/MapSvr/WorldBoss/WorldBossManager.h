/**\file
世界BOSS管理器
\author Kevin
\date 2016/11/1
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;
class CRankManager;

struct WorldBossRankList
{
	RoleRankList vRankList[WORLD_BOSS_RANK_TYPE_NUM][CAMP_NUM];
};

enum ActivityStatus
{
	ACTIVITY_IDLE = 0,				// 空闲
	ACTIVITY_ING = 1,				// 活动中
	ACTIVITY_WAIT_FOR_REWARD = 2,	// 等待发奖励
};

class CWorldBossManager : public Singleton<CWorldBossManager>
{
	friend CRankManager;

public:
	CWorldBossManager();
	~CWorldBossManager();

	void Init();

	void Update();

	void OnNewDay();

	// 更新星期几
	void UpdateWeekDay();

	// 活动开始
	void ActivityStart();

	// 活动结束
	void ActivityOver();

	// 是否在活动中
	bool IsActivity() { return m_nActivityStatus != ACTIVITY_IDLE; };

	void SetBossLevel(WORD wServerId,WORD wBossLevel);
	WORD GetBossLevel(WORD wServerId);

	// 测试使用
	void SetGM(bool bGM) { m_bGM = bGM; };
	void AddRankDataEX(Player *pPlayer, BYTE byRankType, DWORD dwValue) { AddRankData(pPlayer, byRankType, dwValue); };

public:
	// 选择阵营
	void OnNetSelectCamp(Player *pPlayer, BYTE byCamp);

	// 请求世界BOSS数据
	void OnNetWorldBossData(Player *pPlayer);

	// 领取奖励
	void OnNetReward(Player *pPlayer, BYTE byRewardType, WORD *pAwardList, BYTE byAwardNum);

	// 请求领取记录
	void OnNetRewardRecord(Player *pPlayer, BYTE byRewardType);

	// 请求战报
	void OnNetBattleRecord(Player *pPlayer);

	// 购买挑战次数
	void OnNetBuyBattleTimes(Player *pPlayer, BYTE byBuyTimes);

	// 开始战斗
	bool OnBattleStart(Player *pPlayer, WORD wBossLevel);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

public:

	// 取排行榜列表
	AllRoleRankList* GetTypeRankList(BYTE byRankType);

	// 添加排行榜
	void AddRankData(BYTE byRankType, WORD wFindId, const RoleRankList &vRankList);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// 重置排行榜数据
	void ResetRankData(WORD wServerId);

protected:
	// 是否活动开始时间
	bool IsStartTime();

	// 是否活动结束时间
	bool IsOverTime();

	// 是否发奖励时间
	bool IsRewardTime();

	// 是否BOSS已死亡
	bool IsBossDead(WORD wServerId);

	// 当前BOSS剩余血量
	DWORD GetBossRemainHP(WORD wServerId);

	// 取功勋
	DWORD GetExploit();

	// 添加排行数据
	void AddRankData(Player *pPlayer, BYTE byRankType, DWORD dwValue);

	// 更新BOSS伤害
	void UpdateBossDamage(WORD wServerId, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 初始化Boss
	void InitBoss(WORD wServerId = 0);

	// 处理排行奖励
	void DoRankReward();

	// 取排行列表
	RoleRankList* GetRankList(DWORD dwServerId, BYTE byRanType, BYTE byCamp);

	// 取排行列表
	RoleRankList* GetRankList(Player *pPlayer, BYTE byRanType, BYTE byCamp);

	// 取玩家排行数据
	RoleRankData* GetPlayerRankData(DWORD dwServerId, BYTE byRanType, BYTE byCamp, DWORD dwRoleId);

	// 取玩家排行数据
	RoleRankData* GetPlayerRankData(Player *pPlayer, BYTE byRanType);

	// 取玩家世界BOSS数据
	PlayerWorldBoseData* GetPlayerWorldBossData(DWORD dwRoleId);

	// 发送世界BOSS数据
	void SendWorldBossData(Player *pPlayer);

	// 发送奖励领取记录
	void SendRewardRecord(Player *pPlayer, BYTE byRewardType);

	// 添加战报
	void AddBattleRecord(WORD wServerId, const char *pszLuckAttackName, const char *pszKillName);

	// 更新挑战次数
	void UpdateBattleTimes(DWORD dwRoleId);

	// 取世界BOSS数据
	WorldBossData& GetWorldBossData(WORD wServerId);

	// 更新排行榜
	void UpdateRank(DWORD dwServerId, BYTE byRanType, BYTE byCamp);

private:
	// 玩家世界BOSS数据列表 [role_id, data]
	map<DWORD, PlayerWorldBoseData> m_PlayerWorldBossDataList;	

	// 所有排行列表
	//[ServerId][RankType][Camp] 服务器ID,排行类型,阵营
	map<DWORD, WorldBossRankList> m_AllRankList;

	// 世界BOSS数据列表[server_id, data]
	map<DWORD, WorldBossData> m_WorldBossList;

	UINT64 m_lProcessTime;

	bool m_bActivity;		// 是否在活动中
	bool m_bGM;				// 是否在使用GM

	int m_nWeekDay;			// 星期几 

	int m_nActivityStatus;		// 活动状态(0=空闲,1=活动中,2=等待发奖励)
	int m_nActivityTime;		// 活动时间
// 	DWORD m_dwStartTime;		// 活动开始时间
// 	DWORD m_dwOverTime;			// 活动结束时间
// 	DWORD m_dwWaitForRewardTime;// 等级发奖励时间

	AllRoleRankList m_vTempList; // 临时列表[server_id,data]
};