/**\file
��ع�����
\author Kevin
\date 2016/9/24
\note
*/

#pragma once
#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "LandDataDefine.h"
class Player;

typedef map<DWORD, PlayerLandInfo> LandListMap;

class CLandManager : public Singleton<CLandManager>
{
public:
	CLandManager();
	~CLandManager();

	void Init();

	void Update();

	// �洢��������
	void SaveAllDataToDB();

	// �洢����
	void SaveDataToDB(DWORD dwRoleId);

	// ��������
	void LoadDataFromDB(DWORD dwRoleId, const PlayerLandInfo &tLandInfo);

	// ������������
	void LoadDataRequest();

	// ��ӡѲ�߽���(����ʹ��)
	void PrintPatrolReward(Player *pPlayer, WORD wCityId);
public:
	// ������ҵ��������
	void OnNetPlayerLandData(Player *pPlayer, DWORD dwTargetId);

	// Ѳ������
	void OnNetPatrol(Player *pPlayer, DWORD dwHeroUID, WORD wCityId, BYTE byPatrolType, BYTE byTimeType);

	// ��ȡѲ�߽���
	void OnNetPatrolAward(Player *pPlayer, WORD wCityId);

	// �������(����,����)
	void OnNetLandLevelUp(Player *pPlayer, WORD wCityId);

	// �����������б�
	void OnNetFriendLandList(Player *pPlayer);

	// ����ǳ�����
	void OnNetCityDetails(Player *pPlayer, DWORD dwTargetId, WORD wCityId);

	// ��ѹ����
	void OnSuppressRiot(Player *pPlayer, DWORD dwFriendId, WORD wCityId);

public:
	// ս����ʼ
	bool OnBattleStar(Player *pPlayer, WORD wCityId);

	// ս������
	bool OnBattleOver(Player *pPlayer, WORD wCityId, BYTE byIsWin);

	// ��������������
	void SendPlayerLandData(Player *pPlayer, DWORD dwTargetId = 0);

	// �Ƿ��к��
	bool IsHaveRedPoint(DWORD dwRoleId);

	// ȡ����б�����
	int GetLandListSize() { return m_PlayerLandList.size(); };

	// ȡ����������
	WORD GetPlayerLandNum(DWORD dwRoleId);

	// ����ʱ�ӿ�(������)
	PlayerLandInfo* GetPlayerLandInfoEx(DWORD dwRoleId);
	void PatrolEvent_Test(DWORD dwRoleId, WORD wCityId);
protected:
	// ȡ��������Ϣ
	PlayerLandInfo* GetPlayerLandInfo(DWORD dwRoleId);

	// ȡ�ǳ���Ϣ
	LandCityData* GetCityInfo(DWORD dwRoleId, WORD wCityId);

	// ��������Ϣ
	void AddCityInfo(DWORD dwRoleId, const LandCityData &tCityData);

	// Ѳ���¼�(��ʱ��ʹ��)
	//void PatrolEvent(DWORD dwRoleId, WORD wCityId);

	// Ѳ���¼�
	bool PatrolEventEx(DWORD dwRoleId, WORD wCityId);

	// ����¼�ID
	WORD RandEventId(WORD wCityId);

	// ȡѲ����ʱ��
	DWORD GetPatrolTotalTime(BYTE byPatrolType, BYTE byTimeType);
private:
	LandListMap m_PlayerLandList;	// �������б�[roleid, data]

	UINT64 m_lProccessTime;
};
