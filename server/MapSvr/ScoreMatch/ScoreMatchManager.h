/**\file
积分赛管理器
\author Kevin
\date 2016/12/15
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;


class CScoreMatchManager : public Singleton<CScoreMatchManager>
{
public:
	CScoreMatchManager();
	~CScoreMatchManager();

	void Init();

	// 新的一天
	void OnNewDay();

	// 下线
	void OnLogout(Player *pPlayer);

	void Update();

	// 存储数据到数据库
	void SaveDataToDB();

	// 加载数据
	void LoadDataFromDB(DWORD dwCurActivityIndex, BYTE byActivityStatus, DWORD dwActivityOverTime, const char *pData, int size);

	// 加载数据请求
	void LoadDataRequest();

public:

	// 是否活动开始
	bool IsActivityStart();

	// 是否活动结束
	bool IsActivityOver();

	// 是否活动发放奖励
	bool IsActivityReward();

	// 活动开始
	bool DoActivityStart();

	// 活动结束
	bool DoActivityOver();

	// 处理活动奖励
	void DoActivityReward();

	// 处理活动奖励
	void DoActivityReward(WORD wRankType);

public:

	// 请求积分赛数据
	void OnNetScoreMatchData(Player *pPlayer);

	// 选择阵营
	void OnNetSelectCamp(Player *pPlayer, BYTE byCamp);

	// 刷新目标
	void OnNetRefreshTarget(Player *pPlayer);

	// 购买挑战次数
	void OnNetBuyBattleTimes(Player *pPlayer);

	// 领取挑战任务奖励
	void OnNetTakeBattleTaskReward(Player *pPlayer, WORD *pTaskList, WORD wTaskNum);

public:

	// 开始战斗
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// 结束战斗
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

protected:

	// 发送积分赛数据
	void SendScoreMatchData(Player *pPlayer);
	
	// 取挑战目标列表
	bool GetTargetList(Player *pPlayer, vector<ScoreMatchTarget> &vTargetList);

	// 查找挑战目标列表
	bool SearchTargetList(const vector<DWORD> *pServerList, Player *pPlayer, vector<ScoreMatchTarget> &vTargetList, 
		DWORD dwBattleValueMin, DWORD dwBattleValueMax, int nMaxTargetNum, int nMinTargetNum, const vector<ScoreMatchTarget> &vExceptList);

	// 取玩家积分赛数据
	ScoreMatchData* GetPlayerScoreMatchData(DWORD dwRoleId);

	// 是否在活动中
	bool IsInActivity() { return  1 == m_byActivityStatus; };

	// 通过阵营取排行榜类型
	WORD GetRankTypeByCamp(WORD wCamp);

	// 是否任务奖励记录
	bool IsTaskRewardRecord(DWORD dwRoleId, WORD wTaskId);

	// 添加任务奖励记录
	void AddTaskRewardRecord(DWORD dwRoleId, WORD wTaskId);

	// 处理战斗结束逻辑
	bool DoBattleOver(Player *pPlayer, BYTE byIsWin);

	// 处理刷新目标逻辑
	bool DoRefreshTarget(Player *pPlayer, bool bIsNeedCost);
private:

	std::map<DWORD, ScoreMatchData> m_PlayerScoreMatchDataList;	// 玩家积分赛数据

	DWORD	m_dwCurActivityIndex;	// 当前活动索引

	UINT64 m_lProceesSaveTime;

	bool m_bIsLoadDBFinish;		// 是否已加载数据完成
	BYTE m_byActivityStatus;	// 活动状态(0.空闲,1.活动中,2.等待奖励) 
	DWORD m_dwActivityOverTime; // 活动结束时间
};