/**\file
������������
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

	// �µ�һ��
	void OnNewDay();

	// ����
	void OnLogout(Player *pPlayer);

	void Update();

	// �洢���ݵ����ݿ�
	void SaveDataToDB();

	// ��������
	void LoadDataFromDB(DWORD dwCurActivityIndex, BYTE byActivityStatus, DWORD dwActivityOverTime, const char *pData, int size);

	// ������������
	void LoadDataRequest();

public:

	// �Ƿ���ʼ
	bool IsActivityStart();

	// �Ƿ�����
	bool IsActivityOver();

	// �Ƿ����Ž���
	bool IsActivityReward();

	// ���ʼ
	bool DoActivityStart();

	// �����
	bool DoActivityOver();

	// ��������
	void DoActivityReward();

	// ��������
	void DoActivityReward(WORD wRankType);

public:

	// �������������
	void OnNetScoreMatchData(Player *pPlayer);

	// ѡ����Ӫ
	void OnNetSelectCamp(Player *pPlayer, BYTE byCamp);

	// ˢ��Ŀ��
	void OnNetRefreshTarget(Player *pPlayer);

	// ������ս����
	void OnNetBuyBattleTimes(Player *pPlayer);

	// ��ȡ��ս������
	void OnNetTakeBattleTaskReward(Player *pPlayer, WORD *pTaskList, WORD wTaskNum);

public:

	// ��ʼս��
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// ����ս��
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

protected:

	// ���ͻ���������
	void SendScoreMatchData(Player *pPlayer);
	
	// ȡ��սĿ���б�
	bool GetTargetList(Player *pPlayer, vector<ScoreMatchTarget> &vTargetList);

	// ������սĿ���б�
	bool SearchTargetList(const vector<DWORD> *pServerList, Player *pPlayer, vector<ScoreMatchTarget> &vTargetList, 
		DWORD dwBattleValueMin, DWORD dwBattleValueMax, int nMaxTargetNum, int nMinTargetNum, const vector<ScoreMatchTarget> &vExceptList);

	// ȡ��һ���������
	ScoreMatchData* GetPlayerScoreMatchData(DWORD dwRoleId);

	// �Ƿ��ڻ��
	bool IsInActivity() { return  1 == m_byActivityStatus; };

	// ͨ����Ӫȡ���а�����
	WORD GetRankTypeByCamp(WORD wCamp);

	// �Ƿ���������¼
	bool IsTaskRewardRecord(DWORD dwRoleId, WORD wTaskId);

	// �����������¼
	void AddTaskRewardRecord(DWORD dwRoleId, WORD wTaskId);

	// ����ս�������߼�
	bool DoBattleOver(Player *pPlayer, BYTE byIsWin);

	// ����ˢ��Ŀ���߼�
	bool DoRefreshTarget(Player *pPlayer, bool bIsNeedCost);
private:

	std::map<DWORD, ScoreMatchData> m_PlayerScoreMatchDataList;	// ��һ���������

	DWORD	m_dwCurActivityIndex;	// ��ǰ�����

	UINT64 m_lProceesSaveTime;

	bool m_bIsLoadDBFinish;		// �Ƿ��Ѽ����������
	BYTE m_byActivityStatus;	// �״̬(0.����,1.���,2.�ȴ�����) 
	DWORD m_dwActivityOverTime; // �����ʱ��
};