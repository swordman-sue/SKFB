/**\file
组队副本管理器
\author Kevin
\date 2017/6/19
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include <deque>
class Player;
struct TeamDungeonTask_Config;

// 特殊情况处理: 1.最一个答题的玩家,还没答题就下线

class CTeamDungeonManager : public Singleton<CTeamDungeonManager>
{
public:
	CTeamDungeonManager();
	~CTeamDungeonManager();

	void Init();
	void Release();

	void Update();

	// 登录
	void OnLogin(Player *pPlayer);

	// 登出
	void OnLogout(Player *pPlayer);

	void SaveDataToDB();

	void LoadDataFromDB(const char *pData, int size);

	// 加载数据请求
	void LoadDataRequest();

public:


	// 进入场景
	void OnNetEnterScene(WORD wSceneType, DWORD dwContext, Player *pPlayer);

	// 离开场景
	void OnNetLeaveScene(WORD wSceneType, Player *pPlayer);

	// 行走请求
	void OnNetGoto(WORD wSceneType, Player *pPlayer, MapPos tSrcPos, MapPos tDestPos);

	// 聊天
	void Chat(NetMsgBody *pMsg);

// 通用模块消息
public:

	// 开始战斗(怪物)
	bool OnMonsterBattleStart(Player *pPlayer, DWORD dwTargetId);

	// 战斗结束(怪物)
	bool OnMonsterBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 开始战斗(Boss)
	bool OnBossBattleStart(Player *pPlayer);

	// 战斗结束(Boss)
	bool OnBossBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 物品锁定
	void OnNetLockItem(Player *pPlayer, DWORD dwSceneItemUId);

	// 物品拾取
	void OnNetPickUpItem(Player *pPlayer, DWORD dwSceneItemUId);

	// 快捷聊天
	void OnNetShortcutChat(Player *pPlayer, WORD wChatId);

// 本模块消息
public:

	// 请求组队副本数据
	void OnNetTeamDungeonData(Player *pPlayer);

	// 创建队伍
	void OnNetCreate(Player *pPlayer, WORD wDungeonId);

	// 匹配队友
	void OnNetMatch(Player *pPlayer, WORD wDungeonId, BYTE byIsTeam);

	// 取消匹配队友
	void OnNetCancelMatch(Player *pPlayer);

	// 组队邀请
	void OnNetInvite(Player *pPlayer, DWORD dwTargetId);

	// 接受组队邀请
	void OnNetAcceptInvite(Player *pPlayer, DWORD dwTeamId);

	// 加入准备队伍
	void OnNetJoinInReadyTeam(Player *pPlayer, DWORD dwTeamId, WORD wDungeonId);

	// 离开准备状态的队伍(todo::会引起崩溃)
	void OnNetLeaveReadyTeam(Player *pPlayer);

	// 请求玩家推荐列表
	void OnNetPlayerRecommendList(Player *pPlayer);

	// 抽奖
	void OnNetLottery(Player *pPlayer, WORD wTimes, BYTE byIsCostItem);

	// 答题
	void OnNetAnswerQuestion(Player *pPlayer, BYTE byIsRight, BYTE byAnswer);

	// 对话结束
	void OnNetDialogOver(Player *pPlayer);

	// 下一个NPC
	void OnNetNextNPC(Player *pPlayer, WORD wNpcId);

	// Boss任务开始
	void OnNetBossTaskStart(Player *pPlayer);

	// 接受任务
	void OnNetAcceptTask(Player *pPlayer);

	// 任务开始(主要针对自己组队成功的情况)
	void OnNetTaskStart(Player *pPlayer);

	// 请求排行榜列表
	void OnNetRankList(Player *pPlayer, WORD wDungeonId);

	// 请求抽奖记录
	void OnNetLotteryRecord(Player *pPlayer);

public:
	// 离开大厅
	void DoLeaveHall(Player *pPlayer);

	// 是否在大厅
	bool IsInHall(DWORD dwRoleId);

	// 取个人查找数量
	DWORD GetPersonSearchListNum(DWORD dwDungeonId);

	// 取队伍查找列表数量
	DWORD GetTeamSearchListNum(DWORD dwDungeonId);

	// 取队伍查找列表数量
	DWORD GetTeamSearchListNum(DWORD dwDungeonId, WORD wTeamMemberNum);

	// 新任务
	bool OnNewTask(TaskTeamData &tTeamData, BYTE byIsNewTask = true);

	// 发送消息匹配队伍成员
	void SendMsgToMatchTeamMember(MatchTeamData *pTeamData, NetMsgBody *pMsg);


// 匹配池相关接口
protected:

	// 处理队伍取消匹配
	void DoTeamCancelMatch(WORD wDungeonId, DWORD dwTeamId);

	// 处理离开队伍匹配池
	void DoLeaveTeamMatchList(Player *pPlayer);

	// 查找匹配目标
	bool FindMatchTarget(WORD wDungeonId, WORD wNeedTargetNum, vector<MatchTeamData*> &vTeamList, vector<Player*> &vPersonList);

	// 取队伍查找列表
	map<DWORD, MatchTeamData>* GetTeamSearchList(DWORD dwDungeonId);

	// 取散人查找列表
	map<DWORD, Player*>* GetPersonSearchList(DWORD dwDungeonId);

	// 取匹配中的队伍数据
	vector<MatchTeamData*> GetTeamSearchData(DWORD dwDungeonId, WORD wMemberNum);

	// 删除匹配中的队伍数据
	void RemoveTeamSearchData(DWORD dwDungeonId, DWORD dwTeamId);

	// 是否在匹配池
	bool IsInMatchList(Player *pPlayer);

	// 队伍准备中相关接口
protected:

	// 添加准备队伍数据
	void AddReadyTeamData(DWORD dwTeamId, const ReadyTeamData &tTeamData);

	// 取准备中的队伍数据
	ReadyTeamData* GetReadyTeamData(DWORD dwTeamId);

	// 处理离开准备中队伍
	void DoLeaveReadyTeam(Player *pPlayer);

	// 发送消息准备状态队伍成员
	void SendMsgToReadyTeamMember(ReadyTeamData *pTeamData, NetMsgBody *pMsg);

	// 是否在准备队伍中
	bool IsInReadyTeam(Player *pPlayer);

	// 删除准备队伍成员
	void RemoveReadyTeamMember(ReadyTeamData *pTeamData, DWORD dwRoleId);

	// 删除准备中的队伍数据
	void RemoveReadyTeam(DWORD dwTeamId);

	// 队伍任务中相关接口
protected:


	// 添加大厅玩家
	void AddHallPlayer(Player *pPlayer);

	// 删除大厅玩家
	void RemoveHallPlayer(Player *pPlayer);

	// 广播大厅玩家消息
	void SendToHallPlayer(NetMsgBody *pMsg);

	// 添加任务队伍数据
	void AddTaskTeamData(DWORD dwTeamId, const TaskTeamData &tTeamData);

	// 取任务队伍数据
	TaskTeamData* GetTaskTeamData(DWORD dwTeamId);

	// 创建任务
	bool CreateTask(TaskTeamData &tTeamData, BYTE byIsNewTask = true);

	// 是否怪物死亡
	bool IsMonsterDead(vector<MonsterData> &vMonsterList);

	// 列新怪物伤害
	void UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 处理完成阶段任务完成
	void DoFinishStageTask(TaskTeamData &tTeamData, const TeamDungeonTask_Config *pTaskCfg);

	// 删除任务中的队伍
	void RemoveTeamTask(DWORD dwTeamId);

	// 随机坐标点
	bool RandMapPos(MapPos &tMapPos, vector<DWORD> vRankPosList);

	// 发送消息队伍成员
	void SendMsgToTeamMember(TaskTeamData *pTeamData, NetMsgBody *pMsg, DWORD dwMemberId = 0);

	// 创建任务队伍
	bool CreateTaskTeam(WORD wDungeonId, vector<Player*> &vPlayerList);

	// 处理离开任务中的队伍
	void DoLeaveTaskTeam(Player *pPlayer);

	// 创建BOSS
	bool CreateBoss(TaskTeamData *pTeamData);

	// 处理BOSS奖励
	void DoBossReward(TaskTeamData *pTeamData, DWORD dwKillerId = 0);

	// 是否在任务队伍中
	bool IsInTaskTeam(Player *pPlayer);

	// 取额外奖励列表
	void GetExtraRewardMemberList(WORD wTaskType, TaskTeamData *pTeamData, vector<DWORD>& vExtraRewardMemberList);

	// 超时答题
	void TimeOutAnswerQuestion(TaskTeamData *pTeamData);

protected:

	// 添加排行榜数据
	void AddRankData(WORD wDungeonId, const TeamDungeonRank &tRankData);

	// 添加抽奖记录
	void AddLotteryRecord(const TeamDungeonLotteryRecord &tLotteryRecord);

	// 广播匹配目标数量
	void BroadcastMatchTargetNum(WORD wDungeonId);

private:
	// 匹配池(个人)
	map<DWORD, map<DWORD, Player*>> m_PersonMatchList;// dungeon_id, role_id, player 

	// 匹配池(队伍)
	map<DWORD, map<DWORD, MatchTeamData>> m_TeamMatchList;// dungeon_id, team_id, match_team_data

	// 准备状态的队伍列表(1.可以自己拉人组成队伍,2.也可以以队伍作为单位进入匹配池)
	map<DWORD, ReadyTeamData> m_ReadyTeamList; // team_id, team_data

	// 任务中的队伍列表
	map<DWORD, TaskTeamData> m_TaskTeamList;  // team_id, team_data

	// 大厅玩家列表
	map<DWORD, Player*> m_HallPlayerList; // 玩家列表 role_id, player

	// 正在挑战BOSS的队伍列表
	map<DWORD, DWORD> m_BattleBossTeamList; 

	// 排行榜列表
	map<WORD, vector<TeamDungeonRank>> m_TeamDungeonRankList; 

	// 抽奖记录
	deque<TeamDungeonLotteryRecord> m_LotteryRecordList;
};