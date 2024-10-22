/**\file
排行榜管理器
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "ToolKit\Timer.h"
class Player;


class CRankManager : public Singleton<CRankManager>
{
public:
	CRankManager();
	~CRankManager();

	void Init();

	void ConfigReload();

	void InitRechargeRankRewardTime();

	void OnNewDay();

	void Update();


	void SaveAllDataToDB();

	// 每次只存储某一种类型的排行榜
	void SaveDataToDB(BYTE byRankType);

	void LoadDataFromDB(BYTE byRankType, const char *pData, int size);

	// 加载数据请求
	void LoadDataRequest();

	// 合服
	void OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId);

public:

	// 请求排行榜列表
	void OnNetRankList(Player *pPlayer, BYTE byRankType, BYTE bySubType);

	// 请求工会排行榜列表
	void OnNetGuildRankList(Player *pPlayer, BYTE byRankType);

	// 请求我的排名
	void OnNetMyRank(Player *pPlayer, BYTE byRankType);

public:

	// 添加角色排行榜数据
	void AddRoleRankData(BYTE byRankType, Player *pPlayer, DWORD dwValue, DWORD dwParam1 = 0, DWORD dwParam2 = 0, BYTE byMaxRankNum = 50);

	// 添加角色排行榜数据(合服使用)
	void AddRoleRankDataEx(BYTE byRankType, const RoleRankData &tRankData, WORD wMaxRankNum = 50);

	// 取排行名
	WORD GetRoleRank(DWORD dwRoleId, DWORD dwServerId, BYTE byRankType);

	// 取排行值
	DWORD GetRoleRankValue(DWORD dwRoleId, DWORD dwServerId, BYTE byRankType);

	// 删除某个玩家
	void RemoveRoleRankData(BYTE byRankType, DWORD dwServerId, DWORD dwRoleId);

public:
	// 添加工会排行榜数据
	void AddGuildRankData(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum = MAX_GUILD_RANK_MUM);

	// 添加工会排行榜数据(合服使用)
	void AddGuildRankDataEx(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum = MAX_GUILD_RANK_MUM);

	// 删除工会排行数据
	void RemoveGuildRandData(DWORD dwServerId,DWORD dwGuildId, BYTE byRankType);

	// 取工会排名
	WORD GetGuildRank(DWORD dwGuildId, DWORD dwServerId, BYTE byType);

	// 取服务器角色排行榜列表
	const RoleRankList* GetServerRoleRankListEx(WORD wServerId, BYTE byRankType);

	// 取玩家排行榜数据
	const RoleRankData* GetRoleRankDataEx(WORD dwServerId, BYTE byRankType, DWORD dwRoleId);

	// 清空排行榜数据
	void ResetRoleRank(WORD wServerId, BYTE byRankType);

	// 清空排行榜数据
	void ResetRoleRank(BYTE byRankType);

	// 清空服务器角色排行数据
	void ResetServerRoleRankData(WORD wServerId);

	// 更新角色排行榜
	void UpdateRoleRankEx(WORD wServerId, BYTE byRankType);

	// 7天排行奖励(主线,战力,竞技场排行榜奖励)
	void OnSevenDayRankReward(DWORD dwServerId);

	// 取排行榜列表
	const AllRoleRankList* GetRankListEx(BYTE byRankType);

protected:

	// 更新角色排行榜
	void UpdateRoleRank(WORD wServerId, BYTE byRankType);

	// 取服务器角色排行榜列表
	RoleRankList* GetServerRoleRankList(WORD wServerId, BYTE byRankType);

	// 取玩家排行榜数据
	RoleRankData* GetRoleRankData(WORD dwServerId, BYTE byRankType, DWORD dwRoleId);

	// 查找玩家排行榜数据
	RoleRankData* FindRoleRankData(RoleRankList *pRankList, DWORD dwRoleId);

	// 取排行榜列表
	AllRoleRankList* GetRankList(BYTE byRankType);

	// 检测是否在活动中
	bool IsInActivity(WORD wServerId);

	// 是否全服排行
	bool IsAllServerRank(WORD wRank);

protected:
	// 更新角色排行榜
	void UpdateGuildRank(WORD wServerId, BYTE byRankType);

	// 取服务器工会排行榜列表
	GuildRankList* GetGuildRankList(WORD wServerId, BYTE byRankType);

	// 取工会排行榜数据
	GuildRankData* GetGuildRankData(WORD dwServerId, BYTE byRankType, DWORD dwGuildId);

	// 查找工会排行榜数据
	GuildRankData* FindGuildRankData(GuildRankList *pRankList, DWORD dwGuildId);

protected:
	// 处理无尽之地每日排行奖励
	void OnEndlessLandDailyRankReward();

	// 处理无尽之地每周排行奖励
	void OnEndlessLandWeekRankReward();



	// 处理功勋排行榜奖励
	void DoExploitRankReward();

	// 处理伤害排行榜奖励
	void DoDamageRankReward();


	// 充值排行榜奖励
	void DoRechargeRankReward();
	
private:
	// 个人排行榜列表
	AllRoleRankList m_AllRoleRankList[RANK_ROLE_MAX];	// rank_type, server_rank_list

	// 工会排行榜列表
	AllGuildRankList m_AllGuildRankList[GUILD_RANK_MAX]; // rank_type, server_rank_list

	// 临时列表
	RoleRankList m_TempRankList;

	UINT64 m_lProceesSaveTime;

	// 无尽之地每日排行奖励
	CDateTimers m_EndlessLandDailyRankRewardTimer;	

	// 无尽之地每周排行奖励
	CDateTimers m_EndlessLandWeekRankRewardTimer;

	// 7天活动排行奖励(主线,战力,竞技场排行榜奖励)
	CDateTimers m_SevenDayRankRewardTimer;

	// 充值排行榜相关数据
	CDateTimers m_RechargeRankRewardTimer;
};