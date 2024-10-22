#pragma once

class QueryResult;
class AccountInfo;

class DBResultHandler
{
public:
	DBResultHandler(void);
	~DBResultHandler(void);

public:
	// ����˺Ž��
	static void ProcessCheckAccountQueryResult(QueryResult * pData);	

	// ����˺Ž��(sdk)
	static void ProcessSDKAccountCheckResult(QueryResult * pData);

	// �����˺Ž��
	static void ProcessCreateAccountQueryResult(QueryResult * pData);

	// �����˺Ž��(sdk)
	static void ProcessSDKCreateAccountResult(QueryResult * pData);

	// ��ѯ��������ɫ���
	static void ProcessSelectZoneRoleQueryResult(QueryResult* pData);

	// ������ɫ���
	static void ProcessCreateCharQueryResult(QueryResult* pData);		

	// ѡ���ɫ���
	static void ProcessSelectCharInfoQueryResult(QueryResult* pData);	

	// ���½�ɫ��Ϣ
	static void ProcessUpdateCharQueryResult(QueryResult* pData);

	// ���½�ɫ����
	//static void DoUpdateRolePropertyQueryResult(QueryResult * pData);

	// �ʺ������޸Ľ��
	static void ProcessAccountPasswordModifyResult(QueryResult * pData); 

	// �ʺ��ֻ����޸Ľ��
	static void ProcessAccountMobileChangeResult(QueryResult * pData);

	// ���������¼������
	static void ProcessUpdateLastLoginServer(QueryResult * pData);


	// �洢ȫ������
	static void SaveGolbalDataResult(QueryResult * pData);
	
	// ����ȫ������
	static void LoadGolbalDataResult(QueryResult * pData);

	// ���ؽ�ɫ���������б�
	static void LoadRoleBaseDataListResult(QueryResult * pData);

	// ���º�������
	static void UpdateFriendDataResult(QueryResult * pData);

	// �洢���������ݽ��
	static void SaveArenaDataResult(QueryResult * pData);

	// ���ؾ��������ݽ��
	static void LoadArenaDataResult(QueryResult * pData);

	// �洢��ɫ���а����ݽ��
	static void SaveRoleRankDataResult(QueryResult * pData);

	// ���ؽ�ɫ���а����ݽ��
	static void LoadRoleRankDataResult(QueryResult * pData);

	// �洢�ʼ�����
	static void SaveEmailDataResult(QueryResult * pData);

	// ���ز�ɾ��GM�¼����
	static void LoadAndDeleteGMEventResult(QueryResult * pData);

	// ���ط������б���
	static void LoadServerListResult(QueryResult * pData);

	// �洢������ս����¼���ݽ��
	static void SaveArenaBattleRecordResult(QueryResult * pData);

	// ���ؾ��������ݽ��
	static void LoadArenaBattleRecordResult(QueryResult * pData);

	// �洢��ɫ������־���
	static void SaveRoleCurrencyLogsResult(QueryResult * pData);

	// �洢������ݽ��
	static void SaveLandDataResult(QueryResult * pData);

	// ����������ݽ��
	static void LoadLandDataResult(QueryResult * pData);
	
	// ���������а�鿴�б���½��
	static void UpdateArenaRankViewListResult(QueryResult * pData);

	// ������Խ��
	static void SaveLeaveMessageResult(QueryResult * pData);

	// �洢�����������ݽ��
	static void SaveRewardCenterDataResult(QueryResult * pData);

	// ���³ƺ���ʱ���ݽ��
	static void UpdateRoleTempDataResult(QueryResult * pData);

	// �洢��ɫ���߼�¼���
	static void SaveRoleOfflineRecordResult(QueryResult * pData);

	// �洢��ɫ���߼�¼���
	static void SaveRoleLevelRecordResult(QueryResult * pData);

	// �洢��ɫ��ֵ��¼���
	static void SaveRoleRechargeRecordResult(QueryResult * pData);

	// �洢�ؿ�ʧ�ܼ�¼���
	static void SaveRoleMapLoseRecordResult(QueryResult * pData);

	// �洢�ؿ�ʧ�ܼ�¼���
	static void SaveRoleFirstPassMapRecordResult(QueryResult * pData);

	// �洢�̵깺���¼���
	static void SaveShopBuyRecordResult(QueryResult * pData);

	// �洢��Ʒ��ʧ��¼���
	static void SaveItemLoseRecordResult(QueryResult * pData);

	// �洢��Ʒ������¼���
	static void SaveItemOperationRecordResult(QueryResult * pData);

	// �洢�������������¼���
	static void SaveOfflinePlayerNumRecordResult(QueryResult * pData);
		
	// �洢�������ݽ��
	static void SaveGuildDataResult(QueryResult * pData);

	// ���ع������ݽ��
	static void LoadGuildDataResult(QueryResult * pData);

	// �洢�����¼
	static void SaveChatRecordResult(QueryResult * pData);

	// ���������¼
	static void LoadChatRecordResult(QueryResult * pData);

	// �洢��־���
	static void SaveLogsResult(QueryResult * pData);

	// �洢�ؿ����۽��
	static void SaveMapCommentResult(QueryResult * pData);

	// ���عؿ����۽��
	static void LoadMapCommentResult(QueryResult * pData);

	// �����ʺŽ��
	static void UpdateAccountLockResult(QueryResult * pData);

	// �洢���������ݽ��
	static void SaveKingMatchDataResult(QueryResult * pData);
	
	// �������������ݽ��
	static void LoadKingMatchDataResult(QueryResult * pData);

	// �洢���������ݽ��
	static void SaveScoreMatchDataResult(QueryResult * pData);
	
	// ���ػ��������ݽ��
	static void LoadScoreMatchDataResult(QueryResult * pData);

	// �洢��Ӹ������ݽ��
	static void SaveTeamDungeonRankDataResult(QueryResult * pData);

	// ������Ӹ������ݽ��
	static void LoadTeamDungeonRankDataResult(QueryResult * pData);

	// �洢ת�����ݽ��
	static void SaveTurntableDataResult(QueryResult * pData);

	// ����ת�����ݽ��
	static void LoadTurntableDataResult(QueryResult * pData);

	// ���������������
	static void UpdateNewRoleGuideResult(QueryResult * pData);

	// �洢��Դս���ݽ��
	static void SaveResourceWarDataResult(QueryResult * pData);

	// ������Դս���ݽ��
	static void LoadResourceWarDataResult(QueryResult * pData);

	// �洢����ս���ݽ��
	static void SaveGuildWarDataResult(QueryResult * pData);

	// ���ع���ս���ݽ��
	static void LoadGuildWarDataResult(QueryResult * pData);

	// �������������ݽ��
	static void SelectActivityCodeResult(QueryResult * pData);

	// ���¼��������ݽ��
	static void UpdateActivityCodeResult(QueryResult * pData);

	// ��Ϸͳ���¼����
	static void GameStatisticsEventResult(QueryResult * pData);

	// �洢GM������¼
	static void GMOperationRecordSaveResult(QueryResult * pData);
	
};
