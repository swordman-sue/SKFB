/**\file
����BOSS������
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
	ACTIVITY_IDLE = 0,				// ����
	ACTIVITY_ING = 1,				// ���
	ACTIVITY_WAIT_FOR_REWARD = 2,	// �ȴ�������
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

	// �������ڼ�
	void UpdateWeekDay();

	// ���ʼ
	void ActivityStart();

	// �����
	void ActivityOver();

	// �Ƿ��ڻ��
	bool IsActivity() { return m_nActivityStatus != ACTIVITY_IDLE; };

	void SetBossLevel(WORD wServerId,WORD wBossLevel);
	WORD GetBossLevel(WORD wServerId);

	// ����ʹ��
	void SetGM(bool bGM) { m_bGM = bGM; };
	void AddRankDataEX(Player *pPlayer, BYTE byRankType, DWORD dwValue) { AddRankData(pPlayer, byRankType, dwValue); };

public:
	// ѡ����Ӫ
	void OnNetSelectCamp(Player *pPlayer, BYTE byCamp);

	// ��������BOSS����
	void OnNetWorldBossData(Player *pPlayer);

	// ��ȡ����
	void OnNetReward(Player *pPlayer, BYTE byRewardType, WORD *pAwardList, BYTE byAwardNum);

	// ������ȡ��¼
	void OnNetRewardRecord(Player *pPlayer, BYTE byRewardType);

	// ����ս��
	void OnNetBattleRecord(Player *pPlayer);

	// ������ս����
	void OnNetBuyBattleTimes(Player *pPlayer, BYTE byBuyTimes);

	// ��ʼս��
	bool OnBattleStart(Player *pPlayer, WORD wBossLevel);

	// ս������
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

public:

	// ȡ���а��б�
	AllRoleRankList* GetTypeRankList(BYTE byRankType);

	// ������а�
	void AddRankData(BYTE byRankType, WORD wFindId, const RoleRankList &vRankList);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// �������а�����
	void ResetRankData(WORD wServerId);

protected:
	// �Ƿ���ʼʱ��
	bool IsStartTime();

	// �Ƿ�����ʱ��
	bool IsOverTime();

	// �Ƿ񷢽���ʱ��
	bool IsRewardTime();

	// �Ƿ�BOSS������
	bool IsBossDead(WORD wServerId);

	// ��ǰBOSSʣ��Ѫ��
	DWORD GetBossRemainHP(WORD wServerId);

	// ȡ��ѫ
	DWORD GetExploit();

	// �����������
	void AddRankData(Player *pPlayer, BYTE byRankType, DWORD dwValue);

	// ����BOSS�˺�
	void UpdateBossDamage(WORD wServerId, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ��ʼ��Boss
	void InitBoss(WORD wServerId = 0);

	// �������н���
	void DoRankReward();

	// ȡ�����б�
	RoleRankList* GetRankList(DWORD dwServerId, BYTE byRanType, BYTE byCamp);

	// ȡ�����б�
	RoleRankList* GetRankList(Player *pPlayer, BYTE byRanType, BYTE byCamp);

	// ȡ�����������
	RoleRankData* GetPlayerRankData(DWORD dwServerId, BYTE byRanType, BYTE byCamp, DWORD dwRoleId);

	// ȡ�����������
	RoleRankData* GetPlayerRankData(Player *pPlayer, BYTE byRanType);

	// ȡ�������BOSS����
	PlayerWorldBoseData* GetPlayerWorldBossData(DWORD dwRoleId);

	// ��������BOSS����
	void SendWorldBossData(Player *pPlayer);

	// ���ͽ�����ȡ��¼
	void SendRewardRecord(Player *pPlayer, BYTE byRewardType);

	// ���ս��
	void AddBattleRecord(WORD wServerId, const char *pszLuckAttackName, const char *pszKillName);

	// ������ս����
	void UpdateBattleTimes(DWORD dwRoleId);

	// ȡ����BOSS����
	WorldBossData& GetWorldBossData(WORD wServerId);

	// �������а�
	void UpdateRank(DWORD dwServerId, BYTE byRanType, BYTE byCamp);

private:
	// �������BOSS�����б� [role_id, data]
	map<DWORD, PlayerWorldBoseData> m_PlayerWorldBossDataList;	

	// ���������б�
	//[ServerId][RankType][Camp] ������ID,��������,��Ӫ
	map<DWORD, WorldBossRankList> m_AllRankList;

	// ����BOSS�����б�[server_id, data]
	map<DWORD, WorldBossData> m_WorldBossList;

	UINT64 m_lProcessTime;

	bool m_bActivity;		// �Ƿ��ڻ��
	bool m_bGM;				// �Ƿ���ʹ��GM

	int m_nWeekDay;			// ���ڼ� 

	int m_nActivityStatus;		// �״̬(0=����,1=���,2=�ȴ�������)
	int m_nActivityTime;		// �ʱ��
// 	DWORD m_dwStartTime;		// ���ʼʱ��
// 	DWORD m_dwOverTime;			// �����ʱ��
// 	DWORD m_dwWaitForRewardTime;// �ȼ�������ʱ��

	AllRoleRankList m_vTempList; // ��ʱ�б�[server_id,data]
};