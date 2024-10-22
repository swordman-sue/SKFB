#include "stdafx.h"
#include "PlayerBaseDataManager.h"
#include "Player.h"
#include "Friend\FriendManager.h"
#include "CommonLogic.h"
#include "MapSvrQuery.h"
#include "MapServer.h"
#include "Hero\HeroManager.h"

PlayerBaseDataManager::PlayerBaseDataManager()
{

}

PlayerBaseDataManager::~PlayerBaseDataManager()
{
	
}

bool PlayerBaseDataManager::Init()
{
	RoleBaseDataList_Load* pQuery = RoleBaseDataList_Load::ALLOC();
	pQuery->SetIndex(DB_ROLE_BASE_DATA_LIST_LOAD);
	pQuery->SetDBIndex(1);
	g_MapServer.MapDBQuery(pQuery);

	return true;
}

void PlayerBaseDataManager::Release()
{
	
}


// ��ҵ�¼
void PlayerBaseDataManager::OnPlayerLogin(Player *pPlayer)
{
	const PlayerBaseData *pBaseData = GetPlayerBaseData(pPlayer->GetServerId(), pPlayer->GetCharID(), false);
	if (pBaseData)
	{
		CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
		CHECK_POINTER(pFriendMgr);

		for (map<DWORD, DWORD>::const_iterator iter = pBaseData->FriendApplyList.begin();
			iter != pBaseData->FriendApplyList.end(); ++iter)
		{
			const PlayerBaseData *pFriend = GetPlayerBaseData(pPlayer->GetServerId(),iter->second);
			if (pFriend)
			{
				pFriendMgr->AddApplyData(*pFriend);
			}
		}

		for (map<DWORD, DWORD>::const_iterator iter = pBaseData->FriendGiveEnergyList.begin();
			iter != pBaseData->FriendGiveEnergyList.end(); ++iter)
		{
			pFriendMgr->SetGetEnergyState(iter->second, 1);
		}
	}
	else
	{
		PlayerBaseData tBaseData;
		pPlayer->FillBaseData(&tBaseData);
		AddPlayerBaseData(pPlayer->GetServerId(), tBaseData);
	}
}

// �������
void PlayerBaseDataManager::OnPlayerLogout(Player *pPlayer)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataEx(pPlayer->GetServerId(), pPlayer->GetCharID());
	if (!pBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�߼�����!!!�������ʱ�Ҳ�����������[name:%s]", __FUNCTION__, pPlayer->GetUTF8CharName());
		return;
	}

	pPlayer->FillBaseData((PlayerCommonData*)pBaseData);
}

// �����������
void PlayerBaseDataManager::AddPlayerBaseData(DWORD dwServerId, const PlayerBaseData &tBaseData)
{
	PlayerBaseData *pBaseData = new PlayerBaseData();
	pBaseData->Init();
	memcpy_s(pBaseData, sizeof(PlayerCommonData), &tBaseData, sizeof(PlayerCommonData));

	// id�б�
	AllPlayerBaseDataMap::iterator iter_id = m_AllPlayerBaseDataList.find(dwServerId);
	if (iter_id == m_AllPlayerBaseDataList.end())
	{
		PlayerBaseDataMap offline_list;
		m_AllPlayerBaseDataList.insert(make_pair(dwServerId, offline_list));
		iter_id = m_AllPlayerBaseDataList.find(dwServerId);
	}

	// �����б�
	AllPlayerBaseDataNameMap::iterator iter_name = m_AllPlayerBaseDataNameList.find(dwServerId);
	if (iter_name == m_AllPlayerBaseDataNameList.end())
	{
		PlayerBaseDataNameMap offline_list;
		m_AllPlayerBaseDataNameList.insert(make_pair(dwServerId, offline_list));
		iter_name = m_AllPlayerBaseDataNameList.find(dwServerId);
	}

	iter_id->second.insert(make_pair(pBaseData->dwRoleId, pBaseData));

	iter_name->second.insert(make_pair(pBaseData->szRoleName, pBaseData));
}

// �Ƴ���������
void PlayerBaseDataManager::RemovePlayerBaseData(DWORD dwServerId, DWORD dwRoleId)
{
	AllPlayerBaseDataMap::iterator iter_id = m_AllPlayerBaseDataList.find(dwServerId);
	if (iter_id != m_AllPlayerBaseDataList.end())
	{
		PlayerBaseDataMap::iterator iter = iter_id->second.find(dwRoleId);
		if (iter == iter_id->second.end())
			return;

		AllPlayerBaseDataNameMap::iterator iter_name = m_AllPlayerBaseDataNameList.find(dwServerId);
		if (iter_name != m_AllPlayerBaseDataNameList.end())
		{
			iter_name->second.erase(iter->second->szRoleName);
		}

		iter_id->second.erase(iter);
	}
}

// ȡ����б�
const PlayerBaseDataMap* PlayerBaseDataManager::GetPlayerList(DWORD dwServerId)
{
	AllPlayerBaseDataMap::iterator iter = m_AllPlayerBaseDataList.find(dwServerId);
	return iter != m_AllPlayerBaseDataList.end() ? &iter->second : NULL;
}

// ȡ���������(ID)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	return GetPlayerBaseDataEx(dwServerId, dwRoleId, bUpdate);
}

// ȡ���������(����)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseData(DWORD dwServerId, const char *pszRoleName)
{
	AllPlayerBaseDataNameMap::iterator iter = m_AllPlayerBaseDataNameList.find(dwServerId);
	if (iter == m_AllPlayerBaseDataNameList.end())
		return NULL;

	PlayerBaseDataNameMap::iterator iter1 = iter->second.find(pszRoleName);
	return iter1 != iter->second.end() ? iter1->second : NULL;
}

// ȡ���������(ID)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataEx(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	AllPlayerBaseDataMap::iterator list_iter = m_AllPlayerBaseDataList.find(dwServerId);
	if (list_iter == m_AllPlayerBaseDataList.end())
		return NULL;

	PlayerBaseDataMap::iterator data_iter = list_iter->second.find(dwRoleId);
	
	PlayerBaseData *pBaseData = data_iter != list_iter->second.end() ? data_iter->second : NULL;

	if (bUpdate && pBaseData)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
		if (pPlayer)
			pPlayer->FillBaseData(pBaseData);
	}

	return pBaseData;
}