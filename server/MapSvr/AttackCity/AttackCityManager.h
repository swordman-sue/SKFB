/**\file
���ǹ�����
\author Kevin
\date 2017/07/03
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;

// ��������������
struct ServerAttackCityData
{
	DWORD dwServerId;				// ������ID
	DWORD dwRefreshMonsterTime;		// ���ˢ��ʱ��
	WORD wFromChapter;				// ���λ��ʼ���½�
	DWORD dwNO1RoleId;				// ��һ�����Id
	DWORD dwKillBossTime;			// ��ɱBOSSʱ��

	map<DWORD, TreasureBox> mTreasureBoxList; // �����б�
	map<WORD, AttackCityChapter> mWaveDataList; // �������б�
	map<DWORD, Player*> mPlayerList;

	map<DWORD, Player*> mBattlePlayerList; // ����ս��������б�(BOSS��)
};

class CAttackCityManager : public Singleton<CAttackCityManager>
{
public:
	CAttackCityManager();
	~CAttackCityManager();

	// ��ʼ��
	void Init();

	// ��ʼ
	void Start();

	void Update();

	// �ǳ�
	void OnLogout(Player *pPlayer);

	// ս����ʼ
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// ս������
	bool OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum, DWORD dwHPRemainRate);

	// ��ɱ��ǰ���й���(����ʹ��)
	void OnKillCurAllMonster(DWORD dwServerId);

////// ������Ϣ
public:
	// ������������
	void OnNetAttackCityData(Player *pPlayer);

	// ���빥������
	void OnNetAttackCityEnter(Player *pPlayer);

	// �뿪��������
	void OnNetAttackCityLeave(Player *pPlayer);

	// ��������
	void OnNetGoto(Player *pPlayer, MapPos tSrcPos, MapPos tDestPos);

	// ��ȡBoss����
	void OnNetTakeBossReward(Player *pPlayer, WORD wChapter);

	// ����
	void OnNetEncourage(Player *pPlayer);

	// ��������
	void OnNetTreasureBoxLock(Player *pPlayer, DWORD dwTreasureBoxUId);

	// ����ʰȡ
	void OnNetTreasureBoxPickUp(Player *pPlayer, DWORD dwTreasureBoxUId);

	// ��������
	void OnNetRelive(Player *pPlayer, BYTE byIsFreeRelive);

public:
	// �Ƿ��ڳ���
	bool IsInCity(DWORD dwServerId, DWORD dwRoleId);

	// �㲥��Ϣ
	void SendToAll(NetMsgBody *pMsg, DWORD dwServerId);

	// �����ͼ�����
	bool RandMapPos(MapPos &tMapPos,WORD wKey);

	// ��������(todo::��ʱûʹ�õ�)
	void CreateMonster();

	// ��������
	void CreateMonster(DWORD dwServerId);

	// ɾ������
	void RemoveMonster(DWORD dwServerId, WORD wChapter, DWORD dwMonsterUId);

	// ��������
	void CreateTreasureBox();

	// ��������
	void CreateTreasureBox(DWORD dwServerId);

	// ����Boss����
	void CreateBossBox(DWORD dwServerId, WORD wChapter);

	// ��ǰ�����Ƿ���ȫ������
	bool IsAllMonsterDead(DWORD dwServerId);

	// �Ƿ����������
	bool IsMonsterDead(const AttackCityMap &tMap);

	DWORD GetNewSeriesId() { return ++m_dwSeriesId; };

	// ���͹����б�
	void SendMonsterList(Player *pPlayer,WORD wChapter = 0, bool bIsToAll = false);

	// ���ͱ����б�
	void SendTreasureBoxList(Player *pPlayer);

	// ������ҹ�������
	void SendPlayerAttackCityData(Player *pPlayer);

	// ���͸���������б�
	void SendTopRankList(Player *pPlayer, bool bIsToAll = false);

	// �������˶�ͬ������
	void SynRankToAll(DWORD dwServerId);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);
protected:
	// ȡ����б�
	map<DWORD, Player*>& GetPlayerList(DWORD dwServerId);

	// ȡ�½��б�
	map<WORD, AttackCityChapter>& GetAttackCityChapterList(DWORD dwServerId);

	// ȡ��������
	ServerAttackCityData& GetAttackCityData(DWORD dwServerId);

	// ȡ�ؿ�����
	AttackCityMap* GetAttackCityMap(DWORD dwServerId, DWORD dwMapUId);

	// ȡ����
	TreasureBox* GetTreasureBox(DWORD dwServerId, DWORD dwUId);

	// ȡ��ҹ�������
	PlayerAttackCityData& GetPlayerAttackCityData(DWORD dwRoleId);

	// ��ӱ���
	bool AddTreasureBox(DWORD dwServerId, const TreasureBox &tBox);

	// ɾ������
	void RemoveTreasureBox(DWORD dwServerId, DWORD dwUId);

	// �Ƿ�����ס������
	bool IsLockBox(DWORD dwRoleId);

	// �����ס��������
	void AddLockBoxPlayer(DWORD dwRoleId);

	// ɾ����ס��������
	void RemoveLockBoxPlayer(DWORD dwRoleId);

	// ���¹����˺�
	void UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList,  MonsterData *pMonsterList, BYTE byMonsterNum);

	// �Ƿ��������
	bool IsMonsterDead(vector<MonsterData> &vMonsterList);

	// �����뿪����
	void DoLeaveAttackCity(Player *pPlayer);

	// �����������
	bool OnAllMonsterDead(DWORD dwServerId, WORD wChapter, Player *pKiller);

	// �������
	DWORD CalculateScore(DWORD dwBattleScore, DWORD dwKillScore, WORD wEncourageTimes, bool bIsKiller);

	// ɾ��������սBOSS�����
	void RemoveBattleBossPlayer(DWORD dwServerId, DWORD dwRoleId);

	// �����սBOSS�����
	void AddBattleBossPlayer(Player *pPlayer);
protected:
	// ���ʼ
	void OnActivityStart();

	// ���
	void OnActivityIng();

	// �����
	void OnActivityOver();

	// ������
	void OnReward();
private:

	bool m_bStart;

	// ���������������б�[server_id,data]
	map<DWORD, ServerAttackCityData> m_SvrAttackCityDataList;

	// ������ס���ӵ�����б�
	map<DWORD, DWORD> m_LockBoxPlayerList;

	// ��ҹ��������б�
	map<DWORD, PlayerAttackCityData> m_PlayerAttackCityDataList;

	DWORD m_dwSeriesId; // ϵ��ID

	DWORD m_dwRefreshTreasureBoxCD; // ˢ����CD(��)
	DWORD m_dwProcessRefreshBoxTimer; 

	DWORD m_dwRefreshMonsterCD; // ˢ��CD(��)
	//DWORD m_dwProcessMonsterTimer;

	BYTE m_byActivityStatus; // �״̬(����,���,�ȴ�����)
	DWORD m_dwActivityOverTime; // �����ʱ��
};