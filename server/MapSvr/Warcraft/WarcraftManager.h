
/**\file
ħ�����Թ�����
\author Kevin
\date 2016/12/31
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "..\Arena\ArenaManager.h"
class Player;


class CWarcraftManager : public Singleton<CWarcraftManager>
{
public:
	CWarcraftManager();
	~CWarcraftManager();

	void Init();

	void OnNewDay();

	void Update();

public:

	// ��������������
	void OnNetWarcraftData(Player *pPlayer);

	// ������ս����
	void OnNetBuyBattleTimes(Player *pPlayer);

	// ������սĿ��
	void OnNetBattleTargetList(Player *pPlayer);

	// �������а��б�
	void OnNetRankList(Player *pPlayer);

	// ����
	void OnNetCheer(Player *pPlayer, DWORD dwTargetId);
public:

	// ��ʼս��
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// ����ս��
	bool OnBattleOver(Player *pPlayer, BYTE byIsWin);

public:

	// ��������������
	void SendWarcraftData(Player *pPlayer);

	// ����Ŀ���б�
	void SendTargetList(Player *pPlayer);

	// ȡ��սĿ���б�
	bool GetTargetList(DWORD dwRoleId, vector<WarcraftRankData> *pOutTargetList);

	// ��Ӿ��������а�����
	void AddArenaRankData(DWORD dwRoleId, WORD wRank);

protected:
	// ȡ�����������
	WarcraftRankData* GetPlayerRankData(DWORD dwRoleId);

private:
	
	// ���������а�����б�
	map<DWORD, WORD> m_ArenaRankList; 

	// ���������а�����б�
	vector<WarcraftRankData> m_RankList;
};