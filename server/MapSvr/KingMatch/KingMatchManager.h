/**\file
王者赛管理器
\author Kevin
\date 2017/10/31
\note
*/



#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include <deque>
class Player;

class CKingMatchManager : public Singleton<CKingMatchManager>
{
public:
	CKingMatchManager();
	~CKingMatchManager();

	void Init();
	void Release();

	void Update();

	// 登出
	void OnLogout(Player *pPlayer);

	// 登录
	void OnLogin(Player *pPlayer);

	// 活动开始
	bool DoActivityStart();

	// 活动结束
	bool DoActivityOver();

	// 处理活动奖励
	void DoActivityReward();

	// 发放赛季段位奖励
	void DoSeasonStageReward();

	// 发放赛季排行奖励
	void DoSeasonRankReward();

	// 存储数据到数据库
	void SaveDataToDB();

	// 加载数据
	void LoadDataFromDB(WORD wCurSeason, WORD wDataBelongSeason, const char *pData, int size);

	// 加载数据请求
	void LoadDataRequest();

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// fot gm
	void SetStageForTest(Player *pPlayer, WORD wStage);
public:

	// 开始战斗
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// 结束战斗
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

public:

	// 请求玩家的王者赛数据
	void OnNetPlayerKingMatchData(Player *pPlayer);

	// 匹配目标
	void OnNetMatchTarget(Player *pPlayer);

	// 领取赛季任务奖励
	void OnNetTakeSesionTaskReward(Player *pPlayer, WORD wWinTimes);

	// 请求最近赛季排行榜
	void OnNetRecentlySesionRank(Player *pPlayer);

public:
	// 设置当前赛季
	void SetCurSeason(WORD wSeason);

	// 是否有此活动
	bool IsHaveActivity(WORD wServerId);

	// 处理结束结束
	bool DoBattleOver(Player *pPlayer, BYTE byIsWin);
protected:

	// 发送玩家王者数据
	void SendKingMatchData(Player *pPlayer);

	// 是否进行中
	bool IsActivityIng();

	// 是否活动开始
	bool IsActivityStart();

	// 是否活动结束
	bool IsActivityOver();

	// 是否活动发放奖励
	bool IsActivityReward();

	// 是否在段位列表中
	bool IsInStageList(Player *pPlayer);

	// 添加段位数据
	void AddStageData(WORD wServerId, DWORD dwRoleId, WORD wStageLevel = 1, WORD wStar = 0, WORD wBattleTimes = 0);

	// 删除段位数据
	void RemoveStageData(WORD wStageLevel, DWORD dwRoleId);

	// 取段位数据
	KingMatchBattleTarget* GetStageData(WORD wStageLevel, DWORD dwRoleId);

	// 取段位数据
	KingMatchBattleTarget* GetStageData(DWORD dwRoleId);

	// 取段位等级列表
	const map<DWORD, KingMatchBattleTarget>* GetStageLevelList(WORD wStageLevel);

	// 取玩家段位等级
	WORD GetPlayerStageLevel(DWORD dwRoleId);

	// 添加赛季排行榜
	void AddSeasonRank(WORD wSeason, const vector<RoleKingMatchSeasonRank> &vRankList);

	// 取总的挑战目标人数
	DWORD GetTotalBattleTargetNum();

protected:

	// 检测是否已经赛季领取过
	bool IsTakeSeasonTaskReward(const PlayerKingMatchData &tKingMatchData, WORD wWinTimes);

private:

	UINT64 m_lProceesSaveTime;

	bool m_bIsLoadDBFinish; // 是否已加载数据完成
	BYTE m_byActivityStatus; // 活动状态(0.空闲,1.活动中,2.等待奖励)
	DWORD m_dwActivityOverTime; // 活动结束时间

	WORD m_wCurSeason;			// 当前赛季
	WORD m_wDataBelongSeason;	// 数据所属赛季

	// 段位玩家列表
	map<WORD, map<DWORD, KingMatchBattleTarget>> m_StageDataList; // 段位数据

	//map<DWORD, WORD> m_PlayerStageLevelList; // 玩家段位等级列表

	// 近期赛季排行榜列表
	typedef map<WORD, vector<RoleKingMatchSeasonRank>> LastSeasonRankList;
	LastSeasonRankList m_LastSeasonRankList; // 最近赛季排行榜
};