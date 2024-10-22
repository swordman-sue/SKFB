/**\file
ʥ�����߼���
\author Kevin
\date 2017/12/13
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

// ʥ����BOSS����
struct ChristmasBossData
{
	WORD wBossLevel;	// Boss�ȼ�
	DWORD dwCurHP;		// ��ǰHP
	DWORD dwDeadTime;	// ����ʱ��(0=��ʾ������)
};

// ʥ��������
struct ChristmasData
{
	// �ͼ�BOSS����
	ChristmasBossData tLowerBossData;

	// �߼�BOSS����
	ChristmasBossData tHighBossData;
};

// ʥ����boss����
enum
{
	CHRISTMAS_BOSS_LOWER = 1,	// �ͼ���
	CHRISTMAS_BOSS_HIGH,		// �߼���
};

class CChristmas : public Singleton<CChristmas>
{
public:
	CChristmas();
	~CChristmas();

	void Init();

	// �ǳ�
	void OnLogout(Player *pPlayer);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// ��������(��ؿ�)
	void ResetData();

public:

	// ����ʥ��������
	void OnNetChristmasData(Player *pPlayer);

	// ����ս��
	void OnNetEnterBattle(Player *pPlayer);

	// �뿪ս��
	void OnNetLeaveBattle(Player *pPlayer);

	// ����Boss
	void OnNetAttackBoss(Player *pPlayer, WORD wBossType, DWORD dwSnowBallId);

	// ��ȡ����
	void OnNetTakeBossReward(Player *pPlayer, WORD wBossType);

public:

	// ����ʥ������
	void SendChristmasData(Player *pPlayer);

	// ��ʼ��BOSS
	//void InitBoss(WORD wBossType, WORD wBossLevel = 1);

protected:

	// ɾ��������ս�����
	void RemoveBattlePlayer(DWORD dwServerId, DWORD dwRoleId);

	// ���������ս�����
	void AddBattlePlayer(Player *pPlayer);

	// �Ƿ�������ս��
	bool IsInBattle(DWORD dwServerId, DWORD dwRoleId);

	// ȡBOSS����
	ChristmasBossData& GetBossData(DWORD dwServerId, WORD wBossType);

	// ������Ϣ��ս���е����
	void SendMsgToBattlePlayer(DWORD dwServerId, NetMsgBody *pMsg);

	// ʥ��������
	ChristmasData& GetChristmasData(DWORD dwServerId);

private:

	map<DWORD, ChristmasData> m_ChristmasDataList; // ʥ���������б�[server_id, data]

	// ������ս������б�
	map<DWORD, map<DWORD, Player*>> m_BattlePlayerList;
};