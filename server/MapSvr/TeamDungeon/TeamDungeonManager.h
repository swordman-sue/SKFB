/**\file
��Ӹ���������
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

// �����������: 1.��һ����������,��û���������

class CTeamDungeonManager : public Singleton<CTeamDungeonManager>
{
public:
	CTeamDungeonManager();
	~CTeamDungeonManager();

	void Init();
	void Release();

	void Update();

	// ��¼
	void OnLogin(Player *pPlayer);

	// �ǳ�
	void OnLogout(Player *pPlayer);

	void SaveDataToDB();

	void LoadDataFromDB(const char *pData, int size);

	// ������������
	void LoadDataRequest();

public:


	// ���볡��
	void OnNetEnterScene(WORD wSceneType, DWORD dwContext, Player *pPlayer);

	// �뿪����
	void OnNetLeaveScene(WORD wSceneType, Player *pPlayer);

	// ��������
	void OnNetGoto(WORD wSceneType, Player *pPlayer, MapPos tSrcPos, MapPos tDestPos);

	// ����
	void Chat(NetMsgBody *pMsg);

// ͨ��ģ����Ϣ
public:

	// ��ʼս��(����)
	bool OnMonsterBattleStart(Player *pPlayer, DWORD dwTargetId);

	// ս������(����)
	bool OnMonsterBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ��ʼս��(Boss)
	bool OnBossBattleStart(Player *pPlayer);

	// ս������(Boss)
	bool OnBossBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ��Ʒ����
	void OnNetLockItem(Player *pPlayer, DWORD dwSceneItemUId);

	// ��Ʒʰȡ
	void OnNetPickUpItem(Player *pPlayer, DWORD dwSceneItemUId);

	// �������
	void OnNetShortcutChat(Player *pPlayer, WORD wChatId);

// ��ģ����Ϣ
public:

	// ������Ӹ�������
	void OnNetTeamDungeonData(Player *pPlayer);

	// ��������
	void OnNetCreate(Player *pPlayer, WORD wDungeonId);

	// ƥ�����
	void OnNetMatch(Player *pPlayer, WORD wDungeonId, BYTE byIsTeam);

	// ȡ��ƥ�����
	void OnNetCancelMatch(Player *pPlayer);

	// �������
	void OnNetInvite(Player *pPlayer, DWORD dwTargetId);

	// �����������
	void OnNetAcceptInvite(Player *pPlayer, DWORD dwTeamId);

	// ����׼������
	void OnNetJoinInReadyTeam(Player *pPlayer, DWORD dwTeamId, WORD wDungeonId);

	// �뿪׼��״̬�Ķ���(todo::���������)
	void OnNetLeaveReadyTeam(Player *pPlayer);

	// ��������Ƽ��б�
	void OnNetPlayerRecommendList(Player *pPlayer);

	// �齱
	void OnNetLottery(Player *pPlayer, WORD wTimes, BYTE byIsCostItem);

	// ����
	void OnNetAnswerQuestion(Player *pPlayer, BYTE byIsRight, BYTE byAnswer);

	// �Ի�����
	void OnNetDialogOver(Player *pPlayer);

	// ��һ��NPC
	void OnNetNextNPC(Player *pPlayer, WORD wNpcId);

	// Boss����ʼ
	void OnNetBossTaskStart(Player *pPlayer);

	// ��������
	void OnNetAcceptTask(Player *pPlayer);

	// ����ʼ(��Ҫ����Լ���ӳɹ������)
	void OnNetTaskStart(Player *pPlayer);

	// �������а��б�
	void OnNetRankList(Player *pPlayer, WORD wDungeonId);

	// ����齱��¼
	void OnNetLotteryRecord(Player *pPlayer);

public:
	// �뿪����
	void DoLeaveHall(Player *pPlayer);

	// �Ƿ��ڴ���
	bool IsInHall(DWORD dwRoleId);

	// ȡ���˲�������
	DWORD GetPersonSearchListNum(DWORD dwDungeonId);

	// ȡ��������б�����
	DWORD GetTeamSearchListNum(DWORD dwDungeonId);

	// ȡ��������б�����
	DWORD GetTeamSearchListNum(DWORD dwDungeonId, WORD wTeamMemberNum);

	// ������
	bool OnNewTask(TaskTeamData &tTeamData, BYTE byIsNewTask = true);

	// ������Ϣƥ������Ա
	void SendMsgToMatchTeamMember(MatchTeamData *pTeamData, NetMsgBody *pMsg);


// ƥ�����ؽӿ�
protected:

	// �������ȡ��ƥ��
	void DoTeamCancelMatch(WORD wDungeonId, DWORD dwTeamId);

	// �����뿪����ƥ���
	void DoLeaveTeamMatchList(Player *pPlayer);

	// ����ƥ��Ŀ��
	bool FindMatchTarget(WORD wDungeonId, WORD wNeedTargetNum, vector<MatchTeamData*> &vTeamList, vector<Player*> &vPersonList);

	// ȡ��������б�
	map<DWORD, MatchTeamData>* GetTeamSearchList(DWORD dwDungeonId);

	// ȡɢ�˲����б�
	map<DWORD, Player*>* GetPersonSearchList(DWORD dwDungeonId);

	// ȡƥ���еĶ�������
	vector<MatchTeamData*> GetTeamSearchData(DWORD dwDungeonId, WORD wMemberNum);

	// ɾ��ƥ���еĶ�������
	void RemoveTeamSearchData(DWORD dwDungeonId, DWORD dwTeamId);

	// �Ƿ���ƥ���
	bool IsInMatchList(Player *pPlayer);

	// ����׼������ؽӿ�
protected:

	// ���׼����������
	void AddReadyTeamData(DWORD dwTeamId, const ReadyTeamData &tTeamData);

	// ȡ׼���еĶ�������
	ReadyTeamData* GetReadyTeamData(DWORD dwTeamId);

	// �����뿪׼���ж���
	void DoLeaveReadyTeam(Player *pPlayer);

	// ������Ϣ׼��״̬�����Ա
	void SendMsgToReadyTeamMember(ReadyTeamData *pTeamData, NetMsgBody *pMsg);

	// �Ƿ���׼��������
	bool IsInReadyTeam(Player *pPlayer);

	// ɾ��׼�������Ա
	void RemoveReadyTeamMember(ReadyTeamData *pTeamData, DWORD dwRoleId);

	// ɾ��׼���еĶ�������
	void RemoveReadyTeam(DWORD dwTeamId);

	// ������������ؽӿ�
protected:


	// ��Ӵ������
	void AddHallPlayer(Player *pPlayer);

	// ɾ���������
	void RemoveHallPlayer(Player *pPlayer);

	// �㲥���������Ϣ
	void SendToHallPlayer(NetMsgBody *pMsg);

	// ��������������
	void AddTaskTeamData(DWORD dwTeamId, const TaskTeamData &tTeamData);

	// ȡ�����������
	TaskTeamData* GetTaskTeamData(DWORD dwTeamId);

	// ��������
	bool CreateTask(TaskTeamData &tTeamData, BYTE byIsNewTask = true);

	// �Ƿ��������
	bool IsMonsterDead(vector<MonsterData> &vMonsterList);

	// ���¹����˺�
	void UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ������ɽ׶��������
	void DoFinishStageTask(TaskTeamData &tTeamData, const TeamDungeonTask_Config *pTaskCfg);

	// ɾ�������еĶ���
	void RemoveTeamTask(DWORD dwTeamId);

	// ��������
	bool RandMapPos(MapPos &tMapPos, vector<DWORD> vRankPosList);

	// ������Ϣ�����Ա
	void SendMsgToTeamMember(TaskTeamData *pTeamData, NetMsgBody *pMsg, DWORD dwMemberId = 0);

	// �����������
	bool CreateTaskTeam(WORD wDungeonId, vector<Player*> &vPlayerList);

	// �����뿪�����еĶ���
	void DoLeaveTaskTeam(Player *pPlayer);

	// ����BOSS
	bool CreateBoss(TaskTeamData *pTeamData);

	// ����BOSS����
	void DoBossReward(TaskTeamData *pTeamData, DWORD dwKillerId = 0);

	// �Ƿ������������
	bool IsInTaskTeam(Player *pPlayer);

	// ȡ���⽱���б�
	void GetExtraRewardMemberList(WORD wTaskType, TaskTeamData *pTeamData, vector<DWORD>& vExtraRewardMemberList);

	// ��ʱ����
	void TimeOutAnswerQuestion(TaskTeamData *pTeamData);

protected:

	// ������а�����
	void AddRankData(WORD wDungeonId, const TeamDungeonRank &tRankData);

	// ��ӳ齱��¼
	void AddLotteryRecord(const TeamDungeonLotteryRecord &tLotteryRecord);

	// �㲥ƥ��Ŀ������
	void BroadcastMatchTargetNum(WORD wDungeonId);

private:
	// ƥ���(����)
	map<DWORD, map<DWORD, Player*>> m_PersonMatchList;// dungeon_id, role_id, player 

	// ƥ���(����)
	map<DWORD, map<DWORD, MatchTeamData>> m_TeamMatchList;// dungeon_id, team_id, match_team_data

	// ׼��״̬�Ķ����б�(1.�����Լ�������ɶ���,2.Ҳ�����Զ�����Ϊ��λ����ƥ���)
	map<DWORD, ReadyTeamData> m_ReadyTeamList; // team_id, team_data

	// �����еĶ����б�
	map<DWORD, TaskTeamData> m_TaskTeamList;  // team_id, team_data

	// ��������б�
	map<DWORD, Player*> m_HallPlayerList; // ����б� role_id, player

	// ������սBOSS�Ķ����б�
	map<DWORD, DWORD> m_BattleBossTeamList; 

	// ���а��б�
	map<WORD, vector<TeamDungeonRank>> m_TeamDungeonRankList; 

	// �齱��¼
	deque<TeamDungeonLotteryRecord> m_LotteryRecordList;
};