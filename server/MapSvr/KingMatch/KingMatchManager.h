/**\file
������������
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

	// �ǳ�
	void OnLogout(Player *pPlayer);

	// ��¼
	void OnLogin(Player *pPlayer);

	// ���ʼ
	bool DoActivityStart();

	// �����
	bool DoActivityOver();

	// ��������
	void DoActivityReward();

	// ����������λ����
	void DoSeasonStageReward();

	// �����������н���
	void DoSeasonRankReward();

	// �洢���ݵ����ݿ�
	void SaveDataToDB();

	// ��������
	void LoadDataFromDB(WORD wCurSeason, WORD wDataBelongSeason, const char *pData, int size);

	// ������������
	void LoadDataRequest();

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// fot gm
	void SetStageForTest(Player *pPlayer, WORD wStage);
public:

	// ��ʼս��
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// ����ս��
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

public:

	// ������ҵ�����������
	void OnNetPlayerKingMatchData(Player *pPlayer);

	// ƥ��Ŀ��
	void OnNetMatchTarget(Player *pPlayer);

	// ��ȡ����������
	void OnNetTakeSesionTaskReward(Player *pPlayer, WORD wWinTimes);

	// ��������������а�
	void OnNetRecentlySesionRank(Player *pPlayer);

public:
	// ���õ�ǰ����
	void SetCurSeason(WORD wSeason);

	// �Ƿ��д˻
	bool IsHaveActivity(WORD wServerId);

	// �����������
	bool DoBattleOver(Player *pPlayer, BYTE byIsWin);
protected:

	// ���������������
	void SendKingMatchData(Player *pPlayer);

	// �Ƿ������
	bool IsActivityIng();

	// �Ƿ���ʼ
	bool IsActivityStart();

	// �Ƿ�����
	bool IsActivityOver();

	// �Ƿ����Ž���
	bool IsActivityReward();

	// �Ƿ��ڶ�λ�б���
	bool IsInStageList(Player *pPlayer);

	// ��Ӷ�λ����
	void AddStageData(WORD wServerId, DWORD dwRoleId, WORD wStageLevel = 1, WORD wStar = 0, WORD wBattleTimes = 0);

	// ɾ����λ����
	void RemoveStageData(WORD wStageLevel, DWORD dwRoleId);

	// ȡ��λ����
	KingMatchBattleTarget* GetStageData(WORD wStageLevel, DWORD dwRoleId);

	// ȡ��λ����
	KingMatchBattleTarget* GetStageData(DWORD dwRoleId);

	// ȡ��λ�ȼ��б�
	const map<DWORD, KingMatchBattleTarget>* GetStageLevelList(WORD wStageLevel);

	// ȡ��Ҷ�λ�ȼ�
	WORD GetPlayerStageLevel(DWORD dwRoleId);

	// ����������а�
	void AddSeasonRank(WORD wSeason, const vector<RoleKingMatchSeasonRank> &vRankList);

	// ȡ�ܵ���սĿ������
	DWORD GetTotalBattleTargetNum();

protected:

	// ����Ƿ��Ѿ�������ȡ��
	bool IsTakeSeasonTaskReward(const PlayerKingMatchData &tKingMatchData, WORD wWinTimes);

private:

	UINT64 m_lProceesSaveTime;

	bool m_bIsLoadDBFinish; // �Ƿ��Ѽ����������
	BYTE m_byActivityStatus; // �״̬(0.����,1.���,2.�ȴ�����)
	DWORD m_dwActivityOverTime; // �����ʱ��

	WORD m_wCurSeason;			// ��ǰ����
	WORD m_wDataBelongSeason;	// ������������

	// ��λ����б�
	map<WORD, map<DWORD, KingMatchBattleTarget>> m_StageDataList; // ��λ����

	//map<DWORD, WORD> m_PlayerStageLevelList; // ��Ҷ�λ�ȼ��б�

	// �����������а��б�
	typedef map<WORD, vector<RoleKingMatchSeasonRank>> LastSeasonRankList;
	LastSeasonRankList m_LastSeasonRankList; // ����������а�
};