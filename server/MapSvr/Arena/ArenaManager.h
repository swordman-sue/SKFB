/**\file
������������
\author Kevin
\date 2016/10/18
\note
*/

#pragma once
#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "ToolKit\Timer.h"
class Player;
class CRankManager;

typedef vector<ArenaRankData> ArenaRankMap;
typedef map<DWORD, ArenaRankMap> ArenaAllRankMap;
typedef map<DWORD, list<ArenaBattleRecord>> ArenaBattleRecordMap;

class CArenaManager : public Singleton<CArenaManager>
{
	friend CRankManager;
public:
	CArenaManager();
	~CArenaManager();

	void Init();

	void Update();

	// �洢����
	void SaveDataToDB(DWORD dwServerId);

	// �洢��������
	void SaveAllData();

	// ��������
	void LoadDataFromDB(DWORD dwServerId, const char *pData, int size);

	// ������������
	void LoadDataRequest();
	
	// �洢ս����¼����
	void SaveBattleRecord(DWORD dwRoleId);

	// ����ս����¼����
	void LoadBattleRecordFormDB(const PlayerArenaBattleRecord *pPlayerBattleRecordList, int nPlayerBattleRecordNum);

public:
	// ������սĿ���б�
	void OnNetTargetList(Player *pPlayer);

	// ����ս��
	void OnNetBattleRecordList(Player *pPlayer);

	// ս����ʼ
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId, DWORD dwIsSweep);

	// ս������
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

public:
	// ȡ��ҵ�����
	WORD GetPlayerRank(DWORD dwServerId, DWORD dwRoleId);

	// ȡ��ҵ���ʷ����
	WORD GetPlayerHistoryRank(DWORD dwServerId, DWORD dwRoleId);

	// ȡ��սĿ���б�
	bool GetTargetList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutTargetList);

	// ����Ŀ���б�
	void SendTargetList(Player *pPlayer);

	// ȡ���а��б�
	void GetRankList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutRankList, WORD wMaxNum);

	// �������н���
	void DoRankAward();

	// �����״���������
	//void DoFirstRankAward();

	// ����7�����н���
	void DoSevenDayRankAward(DWORD dwServerId);

	// ȡս����¼����
	int GetPlayerBattleRecordNum() { return m_vBattleRecordList.size(); };

	// ������ս����¼
	void AddPlayerBattleRecord(const PlayerArenaBattleRecord *pBattleRecord);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// �������а�����
	void ResetRankData(WORD wServerId);

protected:
	// ȡ���а��б�
	ArenaRankMap* GetServerRankList(DWORD dwServerId);

	// ȡ�����������
	ArenaRankData* GetPlayerRankData(DWORD dwServerId, DWORD dwRoleId);

	// ���ս��
	void AddBattleRecord(DWORD dwRoleId, const ArenaBattleRecord &tRecord);

	// ���ս��
	void AddBattleRecord(DWORD dwSrcRoleId, DWORD dwTargetId, WORD wSrcRank, WORD wTargetRank, const char *pszSrcName, 
		const char *pszTargetName, WORD wScrHeadIcon, WORD wTargetHeadIcon, WORD wScrLevel, WORD wTargetLevel, BYTE byIsWin);

	// �������а�
	void UpdateRank(DWORD dwServerId);
private:

	ArenaAllRankMap m_RankList;	// �����б�

	ArenaAllRankMap m_RankListForSendReward; // ������ʹ�õ�

	ArenaBattleRecordMap m_vBattleRecordList; // ս���б�

	UINT64 m_lProccessTime;

	DWORD m_dwDoRewardTime;

	bool m_bRankUpdate;	// �Ƿ����а��Ѹ���
	bool m_bAlreadyLoadData;	// �Ƿ��Ѽ�������

	UINT64 m_lProceesSaveTime;

	vector<DWORD> m_SaveDataServerList;	// �洢���ݵķ������б�

	CDateTimers m_RankRewardTimer;

};
