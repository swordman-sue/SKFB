/**\file
���а������
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "ToolKit\Timer.h"
class Player;


class CRankManager : public Singleton<CRankManager>
{
public:
	CRankManager();
	~CRankManager();

	void Init();

	void ConfigReload();

	void InitRechargeRankRewardTime();

	void OnNewDay();

	void Update();


	void SaveAllDataToDB();

	// ÿ��ֻ�洢ĳһ�����͵����а�
	void SaveDataToDB(BYTE byRankType);

	void LoadDataFromDB(BYTE byRankType, const char *pData, int size);

	// ������������
	void LoadDataRequest();

	// �Ϸ�
	void OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId);

public:

	// �������а��б�
	void OnNetRankList(Player *pPlayer, BYTE byRankType, BYTE bySubType);

	// ���󹤻����а��б�
	void OnNetGuildRankList(Player *pPlayer, BYTE byRankType);

	// �����ҵ�����
	void OnNetMyRank(Player *pPlayer, BYTE byRankType);

public:

	// ��ӽ�ɫ���а�����
	void AddRoleRankData(BYTE byRankType, Player *pPlayer, DWORD dwValue, DWORD dwParam1 = 0, DWORD dwParam2 = 0, BYTE byMaxRankNum = 50);

	// ��ӽ�ɫ���а�����(�Ϸ�ʹ��)
	void AddRoleRankDataEx(BYTE byRankType, const RoleRankData &tRankData, WORD wMaxRankNum = 50);

	// ȡ������
	WORD GetRoleRank(DWORD dwRoleId, DWORD dwServerId, BYTE byRankType);

	// ȡ����ֵ
	DWORD GetRoleRankValue(DWORD dwRoleId, DWORD dwServerId, BYTE byRankType);

	// ɾ��ĳ�����
	void RemoveRoleRankData(BYTE byRankType, DWORD dwServerId, DWORD dwRoleId);

public:
	// ��ӹ������а�����
	void AddGuildRankData(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum = MAX_GUILD_RANK_MUM);

	// ��ӹ������а�����(�Ϸ�ʹ��)
	void AddGuildRankDataEx(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum = MAX_GUILD_RANK_MUM);

	// ɾ��������������
	void RemoveGuildRandData(DWORD dwServerId,DWORD dwGuildId, BYTE byRankType);

	// ȡ��������
	WORD GetGuildRank(DWORD dwGuildId, DWORD dwServerId, BYTE byType);

	// ȡ��������ɫ���а��б�
	const RoleRankList* GetServerRoleRankListEx(WORD wServerId, BYTE byRankType);

	// ȡ������а�����
	const RoleRankData* GetRoleRankDataEx(WORD dwServerId, BYTE byRankType, DWORD dwRoleId);

	// ������а�����
	void ResetRoleRank(WORD wServerId, BYTE byRankType);

	// ������а�����
	void ResetRoleRank(BYTE byRankType);

	// ��շ�������ɫ��������
	void ResetServerRoleRankData(WORD wServerId);

	// ���½�ɫ���а�
	void UpdateRoleRankEx(WORD wServerId, BYTE byRankType);

	// 7�����н���(����,ս��,���������а���)
	void OnSevenDayRankReward(DWORD dwServerId);

	// ȡ���а��б�
	const AllRoleRankList* GetRankListEx(BYTE byRankType);

protected:

	// ���½�ɫ���а�
	void UpdateRoleRank(WORD wServerId, BYTE byRankType);

	// ȡ��������ɫ���а��б�
	RoleRankList* GetServerRoleRankList(WORD wServerId, BYTE byRankType);

	// ȡ������а�����
	RoleRankData* GetRoleRankData(WORD dwServerId, BYTE byRankType, DWORD dwRoleId);

	// ����������а�����
	RoleRankData* FindRoleRankData(RoleRankList *pRankList, DWORD dwRoleId);

	// ȡ���а��б�
	AllRoleRankList* GetRankList(BYTE byRankType);

	// ����Ƿ��ڻ��
	bool IsInActivity(WORD wServerId);

	// �Ƿ�ȫ������
	bool IsAllServerRank(WORD wRank);

protected:
	// ���½�ɫ���а�
	void UpdateGuildRank(WORD wServerId, BYTE byRankType);

	// ȡ�������������а��б�
	GuildRankList* GetGuildRankList(WORD wServerId, BYTE byRankType);

	// ȡ�������а�����
	GuildRankData* GetGuildRankData(WORD dwServerId, BYTE byRankType, DWORD dwGuildId);

	// ���ҹ������а�����
	GuildRankData* FindGuildRankData(GuildRankList *pRankList, DWORD dwGuildId);

protected:
	// �����޾�֮��ÿ�����н���
	void OnEndlessLandDailyRankReward();

	// �����޾�֮��ÿ�����н���
	void OnEndlessLandWeekRankReward();



	// ����ѫ���а���
	void DoExploitRankReward();

	// �����˺����а���
	void DoDamageRankReward();


	// ��ֵ���а���
	void DoRechargeRankReward();
	
private:
	// �������а��б�
	AllRoleRankList m_AllRoleRankList[RANK_ROLE_MAX];	// rank_type, server_rank_list

	// �������а��б�
	AllGuildRankList m_AllGuildRankList[GUILD_RANK_MAX]; // rank_type, server_rank_list

	// ��ʱ�б�
	RoleRankList m_TempRankList;

	UINT64 m_lProceesSaveTime;

	// �޾�֮��ÿ�����н���
	CDateTimers m_EndlessLandDailyRankRewardTimer;	

	// �޾�֮��ÿ�����н���
	CDateTimers m_EndlessLandWeekRankRewardTimer;

	// 7�����н���(����,ս��,���������а���)
	CDateTimers m_SevenDayRankRewardTimer;

	// ��ֵ���а��������
	CDateTimers m_RechargeRankRewardTimer;
};