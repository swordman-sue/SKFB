/**\file
��һ������ݹ�����
\author Kevin
\date 2016/8/29
\note
*/

#pragma once

#include "DataDefine.h"
#include <map>
#include <hash_map>
#include "ToolKit\Singleton.h"

class Player;

typedef std::hash_map<DWORD, PlayerBaseData*>	PlayerBaseDataMap;
typedef std::hash_map<DWORD, PlayerBaseDataMap>		AllPlayerBaseDataMap;

typedef std::hash_map<string, PlayerBaseData*>	PlayerBaseDataNameMap;
typedef std::hash_map<DWORD, PlayerBaseDataNameMap>	AllPlayerBaseDataNameMap;

class PlayerBaseDataManager : public Singleton<PlayerBaseDataManager>
{
public:
	PlayerBaseDataManager();
	~PlayerBaseDataManager();

	// ��ʼ��
	bool				Init();

	// �ͷ�
	void				Release();

	// ��ҵ�¼
	void				OnPlayerLogin(Player *pPlayer);

	// �������
	void				OnPlayerLogout(Player *pPlayer);

	// ��ӷ�����
	void				AddServer(DWORD dwServerId);

public:
	// �����һ�������
	void AddPlayerBaseData(DWORD dwInitServerId, const PlayerBaseData &tBaseData);

	// �Ƴ���������(Ŀǰ�Ѳ�ʹ��)
	void RemovePlayerBaseData(DWORD dwServerId, DWORD dwRoleId);

	// ȡ����б�
	const PlayerBaseDataMap* GetPlayerList(DWORD dwServerId);

	// ȡ���������(ID)
	const PlayerBaseData* GetPlayerBaseDataById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);

	// ȡ���������(����)
	const PlayerBaseData* GetPlayerBaseDataByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate = true);

	// ȡ���������(����)
	const PlayerBaseData* GetPlayerBaseDataByName(const char *pszRoleName);

	// ȡ���������(Id)
	const PlayerBaseData* GetPlayerBaseDataById(DWORD dwRoleId);

	// ȡ���������(account)
	const PlayerBaseData* GetPlayerBaseDataByAccount(DWORD dwInitServerId, DWORD dwChannelId, const char *pszAccount);

	// ȡ�û����ڷ�����
	DWORD GetServerId(DWORD dwRoleId);

	// ȡ��ɫ���ڵķ�����ID
	DWORD GetServerIdByRoleId(DWORD dwRoleId);

	// ȡ��ɫ��ʼ���ڵķ�����ID
	DWORD GetInitServerIdByRoleId(DWORD dwRoleId);

	// ģ���߼��ӿ�
public:
	// ��Ӻ������͵���������
	void AddGiveEnergyData(DWORD dwServerId, DWORD dwRoleId, DWORD dwFriendId);

	// ��������������
	void AddFriendApplyData(DWORD dwServerId, DWORD dwRoleId, DWORD dwTargetId);

	// ��ӽ���
	void AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward);

	// ��ӽ���
	//void AddReward(DWORD dwServerId, WORD wMinLevel, WORD wMaxLevel, const RewardData &tReward);

	// ɾ������
	void RemoveReward(DWORD dwServerId, DWORD dwRoleId, DWORD dwRewardUId);

	// ��Ӻ�������
	void AddFriendNum(DWORD dwServerId, DWORD dwRoleId, BYTE byFriendNum = 1);

	// �޸Ľ�ɫ����
	VOID RoleNameModify(DWORD dwServerId, const char* pszOldName, const char* pszNewName);

	// ȡ�û�������
	DWORD GetTotalPlayerNum();

	// ȡ���ID�б�
	const vector<DWORD>* GetPlayerIdList(DWORD dwServerId);

	// ����ͨ���ӿ�(todo::��Ҫ����)
	PlayerBaseData* GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);

	// ����ս��
	void SetFightingPower(DWORD dwServerId, DWORD dwRoleId, DWORD dwFightingPower);

	// �Ƿ���Ч�ʺ�
	bool IsInvalidAccount(DWORD dwChannelId, const char *pszAccount);

	// ȡĳ���ʺŵĽ�ɫID�б�
	const vector<DWORD>* GetRoleIdList(DWORD dwChannelId, const char *pszAccount);

protected:
	PlayerBaseData* GetPlayerBaseDataInnerById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);
	PlayerBaseData* GetPlayerBaseDataInnerByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate = true);

private:
	AllPlayerBaseDataMap m_AllPlayerBaseDataList;				// ������һ��������б�(RoleId, Data)
	AllPlayerBaseDataNameMap m_AllPlayerBaseDataNameList;		// ������һ��������б�(RoleName,Data)

	map<DWORD, vector<DWORD>> m_vPlayerIdList;	// ���ID�б�(��Ҫ��Ϊ����ʹ��)[server_id, role_id_list] 

	map<string, vector<DWORD>> m_mapPlayerAccountList;	// �ʺ��б�(��Ҫ��Ϊ����ʹ��)[account, role_id_list]

	map<DWORD, DWORD> m_mServerIdList;		//  ������ID�б�[role_id, server_id]

	map<DWORD, DWORD> m_mInitServerIdList;	//  ��ʼ������ID�б�[role_id, server_id] ������ɫʱ���ڷ�����

	DWORD m_dwSeriesId; // ϵ��ID
};

const PlayerBaseData* g_GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);

const PlayerBaseData* g_GetPlayerBaseData(DWORD dwRoleId, bool bUpdate = true);