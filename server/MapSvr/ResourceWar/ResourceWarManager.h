/**\file
��Դս������
\author Kevin
\date 2018/08/02
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;


class CResourceWarManager : public Singleton<CResourceWarManager>
{
public:
	CResourceWarManager();
	~CResourceWarManager();

	void Init();

	void Update();

	void OnLogin(Player *pPlayer);

	void OnLogout(Player *pPlayer);

	// �洢���ݵ����ݿ�
	void SaveDataToDB();

	// ��������
	void LoadDataFromDB(const char *pData, int nSize);

	// ������������
	void LoadDataRequest();

public:

	// ���ʼ
	bool DoActivityStart();

public:

	// ������Դս����
	void OnNetResourceWardData(Player *pPlayer);

	// ����ս��
	void OnNetBattleRecord(Player *pPlayer);

	// ��������б�
	void OnNetEnemyList(Player *pPlayer);

	// �����½�����
	void OnNetChapterData(Player *pPlayer, WORD wChapter);

	// ����ͬ����Դ����
	void OnNetSameResourceList(Player *pPlayer, WORD wResourceType);

	// ���������Դ
	void OnNetGuildResourceList(Player *pPlayer);
	
	// �����Ƽ�
	void OnNetUpgradeTechnology(Player *pPlayer);
	
	// �ӳ�ռ��
	void OnNetKeepOccupy(Player *pPlayer);

	// ����ռ��
	void OnNetGiveUpOccupy(Player *pPlayer);

public:

	// ս����ʼ
	bool OnNetBattleStart(Player *pPlayer, WORD wChapter, WORD wResourceId);

	// ս������
	bool OnNetBattleOver(Player *pPlayer, BYTE byIsWin);

public:
	// ����ռ��ʱ�����
	void DoOccupyTimeOver(const ResourceWarResouceData *pResourceWarData);

	// �����������
	void DoBaseReward(ResourceWarResouceData *pResourceWarData);

	// ������⽱��
	void DoExtraReward(ResourceWarResouceData *pResourceWarData);

	// ȡ���⽱���б�
	const vector<Item>* GetExtraRewardList(DWORD dwResourceUId, DWORD dwRewardIdx);

	// ���ռ��ʱ��(GMʹ��)
	void AddOccupyTimeForGM(Player *pPlayer, DWORD dwAddTime);

protected:

	// ����ռ����Դʱ��
	void UpdateAccupyResourceTime(DWORD dwRoleId, DWORD dwAddTime);

	// ����ռ���������
	void SendBaseReward(DWORD dwRoleId, WORD wChapter, WORD wResourceId);

	// ������Դս����
	void SendResourceWardData(Player *pPlayer);

	// �����½�����
	void SendChapterData(Player *pPlayer, WORD wChapter);

protected:

	// �Ƿ��ڻ��
	bool IsActivity();

	// ���ս��
	void AddBattleRecord(const ResourceWardBattleRecord &tRecord);

	// �����Դս����
	void AddResourceWarData(const ResourceWarResouceData &tResourceWarData);

	// �Ƴ���Դս����
	void RemoveResourceWarData(WORD wChapter, WORD wResourceId);

	// ��ӵ�������
	void AddEnemyData(DWORD dwRoleId, DWORD dwEnemyId);

	// ��ӵ�������
	void AddEnemyData(DWORD dwRoleId, const ResourceWarEnemy &tEnemy);

	// ȡ��Դս����
	ResourceWarResouceData* FindResourceWarData(WORD wChapter, WORD wResourceId);

	// ȡ��Դս����
	ResourceWarResouceData* FindResourceWarData(DWORD dwRoleId);

	// ȡ��ҵ����б�
	map<DWORD, ResourceWarEnemy>* GetEnemyList(DWORD dwRoleId);

	// �����Դ�Ƿ��ѱ�ռ��
	void IsResourceBeOccupy(WORD wChapter, WORD wResourceId);

	// ���²������Դ�Ƿ���ս
	bool UpdateAndCheckIsBeBattle(WORD wChapter, WORD wResourceId);

	// ��ӱ���ս����Դ
	void AddBeBattleResource(WORD wChapter, WORD wResourceId);

	// �Ƴ�����ս����Դ
	void RemoveBeBattleResource(WORD wChapter, WORD wResourceId);

	// �������ս�е����
	void AddInBattlePlayer(DWORD dwRoleId, WORD wChapter, WORD wResourceId);

	// �Ƴ�����ս�е����
	void RemoveInBattlePlayer(DWORD dwRoleId);

	// ȡ���ڱ���ս����Դuid
	DWORD GetBeBattleResourceUId(DWORD dwRoleId);

	// ȡ����Ѿ�ռ�����Դuid
	DWORD GetRoleAccupyResourceUId(DWORD dwRoleId);

	// ��Ӳ��������Դ����
	void AddFindRoleResourceData(DWORD dwRoleId, WORD wChapter, WORD wResourceId);

	// �Ƴ����������Դ����
	void RemoveFindRoleResourceData(DWORD dwRoleId);

	// ȡ��ҵĻ��������ӳɱ���
	int GetBaseRewardAddRate(DWORD dwRoleId);

	// ȡͬһ�½ڹ����Առ������
	WORD GetGuildAccupyMemberNum(DWORD dwRoleId);

	// ȡ�����Դս����
	RoleResourceWarData& GetRoleResourceWarData(DWORD dwRoleId);

	// ��������Դս����
	void AddRoleResourceWarData(DWORD dwRoleId, const RoleResourceWarData &tData);

private:

	bool m_bLoadDataFromDB;
	DWORD m_dwLastProccessTimer;

	////////////////////////
	// ��Ҫ�洢������

	// ��Դ�б�
	map<DWORD, map<DWORD, ResourceWarResouceData>> m_ResourceList; // [chapter_id, resource_id, data]

	// �����б�
	map<DWORD, map<DWORD, ResourceWarEnemy>> m_EnemyList;	// [role_id, enemy_role_id, data]

	// ս���б�
	deque<ResourceWardBattleRecord> m_BattleRecordList;

	// �����Դս�����б�
	map<DWORD, RoleResourceWarData> m_RoleResourceWarDataList;
	

	////////////////////////
	// ��ʱ����

	// �����Դ�����б�
	map<DWORD, DWORD> m_RoleResourceFindList;	// [role_id, chapter_id + resource_id]

	// ���ڱ���ս����Դ�б�
	map<DWORD, DWORD> m_BeBattleResourceList;	// [chapter_id + resource_id, star_battle_time]

	// ������ս�е�����б�
	map<DWORD, DWORD> m_InBattlePlayerList;		// [role_id, chapter_id + resource_id]
};