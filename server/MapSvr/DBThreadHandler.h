#pragma once

class SqlDatabase;
class QueryResult;

class DBThreadHandler
{
public:
	DBThreadHandler(void){}
	~DBThreadHandler(void){}
public:
/////////////////////////////////////
// �ʺż���ɫ���
	// ����˺�
	static void DoCheckAccountQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// �ʺż��(sdk)(todo::���Ż�)
	static void DoSDKAccountCheck(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �����˺�
	static void DoCreateAccountQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �����˺�(sdk)
	static void DoSDKCreateAccount(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ��ѯ������ɫ(todo::���Ż�)
	static void DoSelectZoneRoleQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ������ɫ
	static void DoCreateRoleQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ��ѯ��ɫ��Ϣ
	static void DoSelectRoleInfoQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���½�ɫ��Ϣ
	static void DoUpdateRoleInfoQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���½�ɫ����
	//static void DoUpdateRolePropertyQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �ʺ������޸�
	static void ProcessAccountPasswordModify(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �ֻ����޸�
	static void ProcessAccountMobileChange(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���������¼�ķ�����
	static void DoUpateLastLoginServer(SqlDatabase* pSqlDatabase, QueryResult * pData);


	// �洢ȫ������
	static void SaveGolbalData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ����ȫ������
	static void LoadGolbalData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ؽ�ɫ���������б�
	static void LoadRoleBaseDataList(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���º�������
	static void UpdateFriendData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢����������
	static void SaveArenaData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ؾ���������
	static void LoadArenaData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢���а�����
	static void SaveRoleRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �������а�����
	static void LoadRoleRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢�ʼ�����
	static void SaveEmailData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ز�ɾ��GM�¼�
	static void  LoadAndDeleteGMEvent(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ط������б�
	static void LoadServerList(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢������ս����¼����
	static void SaveArenaBattleRecordData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ؾ�����ս����¼����
	static void LoadArenaBattleRecordData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �����ɫ������־
	static void SaveRoleCurencyLogs(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢�������
	static void SaveLandData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �����������
	static void LoadLandData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���������а�鿴�б����
	static void UpdateArenaRankViewList(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢����
	static void SaveLeaveMessage(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢������������
	static void SaveRewardCenterData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���½�ɫ��ʱ����
	static void UpdateRoleTempData(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// �洢��ɫ���߼�¼
	static void SaveRoleOfflineRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��ɫ�ȼ���¼
	static void SaveRoleLevelRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��ɫ��ֵ��¼
	static void SaveRoleRechargeRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢�ؿ�ʧ�ܼ�¼
	static void SaveRoleMapLoseRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢�״�ͨ�ؼ�¼
	static void SaveRoleFirstPassMapRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// �洢�̵깺���¼
	static void SaveShopBuyRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��������
	static void SaveGuildData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ع�������
	static void LoadGuildData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢�����¼
	static void SaveChatRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���������¼
	static void LoadChatRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��־
	static void SaveLogs(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢�ؿ�����
	static void SaveMapComment(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���عؿ�����
	static void LoadMapComment(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��Ʒ��ʧ��¼
	static void SaveItemLoseRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��Ʒ������¼
	static void SaveItemOperationRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �����ʺ���
	static void UpdateAccountLock(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// �洢����������
	static void SaveKingMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ��������������
	static void LoadKingMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢����������
	static void SaveScoreMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ػ���������
	static void LoadScoreMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// �洢��Ӹ�������
	static void SaveTeamDungeonRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ������Ӹ�������
	static void LoadTeamDungeonRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢���������������
	static void SaveOfflinePlayerNumRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// ���ºϷ����� 
	static void UpdateServerData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢ת������
	static void SaveTurntableData(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// ����ת������
	static void LoadTurntableData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ������������
	static void UpdateNewRoleGuide(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢��Դս����
	static void SaveResourceWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ������Դս����
	static void LoadResourceWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢����ս����
	static void SaveGuildWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���ع���ս����
	static void LoadGuildWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ��������������
	static void SelectActivityCode(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ���¼���������
	static void UpdateActivityCode(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// ��Ϸͳ���¼�
	static void GameStatisticsEvent(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// �洢GM������¼
	static void GMOperationRecordSave(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// ͨ�ýӿ�	
public:

	static void UpdateRetentionRate(SqlDatabase* pSqlDatabase, QueryResult * pQuery, DWORD dwCreateRoleTime, DWORD Days, const char *pszKeyName);
};