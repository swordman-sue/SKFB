#include "stdafx.h"
#include "PlayerBaseDataManager.h"
#include "Player.h"
#include "Friend\FriendManager.h"
#include "CommonLogic.h"
#include "MapSvrQuery.h"
#include "MapServer.h"
#include "Hero\HeroManager.h"
#include "RewardCenter\RewardCenterManager.h"
#include "Guild\GuildManager.h"

/*
�ܽ᣺
vector�ǿ��Կ��ٵ���������ɾ��Ԫ��,�����Կ��ٵط�������Ԫ��
list�ǿ��Կ��ٵ������еط����ɾ��Ԫ��,����ֻ�ܿ��ٵط����ʼ������Ԫ��
deque�ڿ�ʼ��������Ԫ�ض�һ����,���ṩ��������ʷ���,��vectorһ��ʹ��[]��������Ԫ��,������������ٶȱȲ���vector��,��Ϊ��Ҫ�ڲ��������ת
dequeҲ�б����ռ�.����,����deque��Ҫ�������ռ�,���Կ��Ա����Ԫ�ر�vector����,���ҲҪע��һ��.���о�����ǰ��ͺ������Ԫ��ʱ������Ҫ�ƶ��������Ԫ��,��������Ҳ�ܸߡ�

�����ʵ��ʹ��ʱ�����ѡ����������������һ����Ӧ���������Ҫ������һ��Ӧ��ѭ����
��ԭ��
1���������Ҫ��Ч���漴��ȡ�������ں������ɾ����Ч�ʣ�ʹ��vector
2���������Ҫ�����Ĳ����ɾ�������������漴��ȡ����Ӧʹ��list
3���������Ҫ�漴��ȡ�����ҹ����������ݵĲ����ɾ������Ӧʹ��deque��


1��vector  �������Ŀռ�洢,����ʹ��[]�����������ٵķ��������Ԫ�أ����ٵ���ĩβ����Ԫ�أ�
�����������м����Ĳ��룬ɾ��Ԫ��Ҫ�����������һ��ʼ����Ŀռ䲻���Ļ�����һ�����·������ռ䣬Ȼ�󿽱������ܿ���.
2��deque ��СƬ��������СƬ��������������ʵ�����ڲ���һ��map��ָ�룬��Ϊ֪�����ͣ�
���Ի��ǿ���ʹ��[]��ֻ���ٶ�û��vector�죩���ٵķ��������Ԫ�أ����ٵ��ڿ�ʼ��ĩβ����Ԫ�أ�
����Ĳ��룬ɾ��Ԫ��Ҫ�����ռ�����·���Ҫ��vector��,���·���ռ��ԭ�е�Ԫ�ز���Ҫ��������deque������������ɽ�deque�ȸ��Ƶ�vector��������ڸ��ƻ�deque��
3��list   ��ÿ��Ԫ�ؼ��������������������Ԫ�ز���vector�죬����Ĳ���Ԫ�ر�vector�죬��ÿ��Ԫ�ط���ռ䣬���Բ����ڿռ䲻�������·�������
4��set �ڲ�Ԫ��Ψһ����һ��ƽ�����ṹ���洢����˱�����ʱ��������ˣ�����Ҳ�ȽϿ��Ŷ��
5��map һ��һ��ӳ��Ľ�ϣ�key�����ظ���
6��stack ������������������������ʹ�ã�stl��Ĭ�ϵ��ڲ�������deque���Ƚ������ֻ��һ�����ڣ������������
7��queue �������Ƶ�deque���ڲ�����һ��ʹ��list�ϼ򵥡��Ƚ��ȳ��������������

������ѡ��˳���������͵�һЩ׼��
1.���������Ҫ�������һ��������vectorҪ��list�õö� ��

2.���������֪Ҫ�洢Ԫ�صĸ�����vector ����һ����list�õ�ѡ��

3.���������Ҫ�Ĳ�ֻ�����������˲����ɾ��Ԫ����list��ȻҪ��vector��

4.����������Ҫ�������ײ������ɾ��Ԫ�ط���vectorҪ��deque�á�

5.���ֻ�����׵��ײ���β����������Ԫ�أ���ѡ��deque.

6.���ֻ��Ҫ�ڶ�ȡ����ʱ���������м�λ�ò���Ԫ�أ�Ȼ����Ҫ�������Ԫ�أ���ɿ�������ʱ��Ԫ�ض��뵽һ��List������
���ŶԴ�����������ѧ��ʹ���ʺ�˳����ʣ�Ȼ��������list�������Ƶ�һ��vector������
*/

PlayerBaseDataManager::PlayerBaseDataManager()
{
	m_dwSeriesId = 1;
}

PlayerBaseDataManager::~PlayerBaseDataManager()
{
	
}

bool PlayerBaseDataManager::Init()
{
	RoleBaseDataList_Load* pQuery = RoleBaseDataList_Load::ALLOC();
	pQuery->SetIndex(DB_ROLE_BASE_DATA_LIST_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_LOAD_ROLE_INFO_LIST);
	g_MapServer.MapDBQuery(pQuery);

	return true;
}

void PlayerBaseDataManager::Release()
{
	
}


// ��ҵ�¼
void PlayerBaseDataManager::OnPlayerLogin(Player *pPlayer)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(pPlayer->GetServerId(), pPlayer->GetCharID(), false);
	if (pBaseData)
	{
		// �����������
		CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
		CHECK_POINTER(pFriendMgr);
		for (set<DWORD>::const_iterator iter = pBaseData->setFriendApplyList.begin();
			iter != pBaseData->setFriendApplyList.end(); ++iter)
		{
			if (!pFriendMgr->IsBlack(*iter))
				pFriendMgr->AddApplyData(*iter);
		}
		for (set<DWORD>::const_iterator iter = pBaseData->setFriendGiveEnergyList.begin();
			iter != pBaseData->setFriendGiveEnergyList.end(); ++iter)
		{
			pFriendMgr->SetGetEnergyState(*iter, 1);
		}
		pBaseData->setFriendApplyList.clear();
		pBaseData->setFriendGiveEnergyList.clear();

		// ��������
		CRewardCenterManager *pRewardMgr = pPlayer->GetRewardCenterManager();
		CHECK_POINTER(pRewardMgr);
		for (size_t i = 0; i < pBaseData->vRewardList.size(); ++i)
		{
			pRewardMgr->AddReward(pBaseData->vRewardList[i], false);
		}
		pBaseData->vRewardList.clear();
		pBaseData->dwOfflineTime = 0;
	}
	else
	{
		PlayerBaseData tBaseData;
		pPlayer->FillBaseData(&tBaseData);
		tBaseData.dwOfflineTime = 0;
		AddPlayerBaseData(pPlayer->GetInitServerId(), tBaseData);
	}
}

// �������
void PlayerBaseDataManager::OnPlayerLogout(Player *pPlayer)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(pPlayer->GetServerId(), pPlayer->GetCharID(), false);
	if (!pBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�߼�����!!!�������ʱ�Ҳ�����������[name:%s]", __FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	pPlayer->FillBaseData(pBaseData);
	pBaseData->dwOfflineTime = time(NULL);
}

// ��ӷ�����
void PlayerBaseDataManager::AddServer(DWORD dwServerId)
{
	AllPlayerBaseDataMap::iterator id_iter = m_AllPlayerBaseDataList.find(dwServerId);
	if (id_iter == m_AllPlayerBaseDataList.end())
	{
		PlayerBaseDataMap idList;
		m_AllPlayerBaseDataList.insert(make_pair(dwServerId, idList));
	}

	AllPlayerBaseDataNameMap::iterator name_iter = m_AllPlayerBaseDataNameList.find(dwServerId);
	if (name_iter == m_AllPlayerBaseDataNameList.end())
	{
		PlayerBaseDataNameMap nameList;
		m_AllPlayerBaseDataNameList.insert(make_pair(dwServerId, nameList));
	}
}

// �����������
void PlayerBaseDataManager::AddPlayerBaseData(DWORD dwInitServerId, const PlayerBaseData &tBaseData)
{
	DWORD dwCurServerId = g_MapServer.GetCurServerId(dwInitServerId, tBaseData.dwRoleId);

	// todo::�����Ƿ���Ҫ����Ϊ�ڴ�ع���
	PlayerBaseData *pBaseData = new PlayerBaseData();
	pBaseData->Init();
	memcpy_s(pBaseData, sizeof(PlayerCommonData), &tBaseData, sizeof(PlayerCommonData));

	pBaseData->vToBattleHeroList = tBaseData.vToBattleHeroList;

	// id�б�
	AllPlayerBaseDataMap::iterator iter_id = m_AllPlayerBaseDataList.find(dwCurServerId);
	if (iter_id == m_AllPlayerBaseDataList.end())
	{
		PlayerBaseDataMap offline_list;
		m_AllPlayerBaseDataList.insert(make_pair(dwCurServerId, offline_list));
		iter_id = m_AllPlayerBaseDataList.find(dwCurServerId);
	}

	// �����б�
	AllPlayerBaseDataNameMap::iterator iter_name = m_AllPlayerBaseDataNameList.find(dwCurServerId);
	if (iter_name == m_AllPlayerBaseDataNameList.end())
	{
		PlayerBaseDataNameMap offline_list;
		m_AllPlayerBaseDataNameList.insert(make_pair(dwCurServerId, offline_list));
		iter_name = m_AllPlayerBaseDataNameList.find(dwCurServerId);
	}

	iter_id->second.insert(make_pair(pBaseData->dwRoleId, pBaseData));

	iter_name->second.insert(make_pair(pBaseData->szRoleName, pBaseData));

	// ID�б�
	map<DWORD, vector<DWORD>>::iterator id_list_iter = m_vPlayerIdList.find(dwCurServerId);
	if (id_list_iter != m_vPlayerIdList.end())
	{
		id_list_iter->second.push_back(pBaseData->dwRoleId);
	}
	else
	{
		vector<DWORD> vIdList;
		vIdList.push_back(pBaseData->dwRoleId);
		m_vPlayerIdList.insert(make_pair(dwCurServerId, vIdList));
	}

	// �ʺ��б�
	char szAccountKey[MAX_ACCOUNT_LEN] = { 0 };
	sprintf_s(szAccountKey, sizeof(szAccountKey)-1, "%s_%d", tBaseData.szAccount, tBaseData.dwPlatform);
	map<string, vector<DWORD>>::iterator iter_account = m_mapPlayerAccountList.find(szAccountKey);
	if (iter_account != m_mapPlayerAccountList.end())
	{
		iter_account->second.push_back(tBaseData.dwRoleId);
	}
	else
	{
		vector<DWORD> vIdList;
		vIdList.push_back(pBaseData->dwRoleId);
		m_mapPlayerAccountList.insert(make_pair(szAccountKey, vIdList));
	}

	// ������ID�б�
	if (m_mServerIdList.find(tBaseData.dwRoleId) == m_mServerIdList.end())
	{
		m_mServerIdList.insert(make_pair(tBaseData.dwRoleId, dwCurServerId));
	}

	if (m_mInitServerIdList.find(tBaseData.dwRoleId) == m_mInitServerIdList.end())
	{
		m_mInitServerIdList.insert(make_pair(tBaseData.dwRoleId, dwInitServerId));
	}
	
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
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	return GetPlayerBaseDataInnerById(dwServerId, dwRoleId, bUpdate);
}

// ȡ���������(����)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate)
{
	return GetPlayerBaseDataInnerByName(dwServerId, pszRoleName, bUpdate);
}

// ȡ���������(����)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataByName(const char *pszRoleName)
{
	AllPlayerBaseDataNameMap::iterator iter = m_AllPlayerBaseDataNameList.begin();
	for (; iter != m_AllPlayerBaseDataNameList.end(); ++iter)
	{
		PlayerBaseDataNameMap::iterator iter1 = iter->second.find(pszRoleName);
		if (iter1 != iter->second.end())
			return iter1->second;
	}
	return NULL;
}

// ȡ���������(Id)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataById(DWORD dwRoleId)
{
	AllPlayerBaseDataMap::iterator iter = m_AllPlayerBaseDataList.begin();
	for (; iter != m_AllPlayerBaseDataList.end(); ++iter)
	{
		PlayerBaseDataMap::iterator iter1 = iter->second.find(dwRoleId);
		if (iter1 != iter->second.end())
			return iter1->second;
	}
	return NULL;
}

// ȡ���������(account)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataByAccount(DWORD dwInitServerId, DWORD dwChannelId, const char *pszAccount)
{
	DWORD dwCurServerId = g_MapServer.GetCurServerId(dwInitServerId);

	if (!dwCurServerId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����dwCurServerId==0", __FUNCTION__);
		return NULL;
	}

	const vector<DWORD> *pRoleIdList = GetRoleIdList(dwChannelId, pszAccount);
	if (!pRoleIdList)
		return NULL;

	vector<DWORD>::const_iterator iter = pRoleIdList->begin();
	for (; iter != pRoleIdList->end(); ++iter)
	{
		DWORD dwRoleId = *iter;
		if (dwInitServerId == GetInitServerIdByRoleId(dwRoleId))
		{
			return GetPlayerBaseDataInnerById(dwCurServerId, dwRoleId, false);
		}
	}

	return NULL;
}

// ȡ�û����ڷ�����
DWORD PlayerBaseDataManager::GetServerId(DWORD dwRoleId)
{
	AllPlayerBaseDataMap::iterator iter = m_AllPlayerBaseDataList.begin();
	for (; iter != m_AllPlayerBaseDataList.end(); ++iter)
	{
		PlayerBaseDataMap::iterator iter1 = iter->second.find(dwRoleId);
		if (iter1 != iter->second.end())
			return iter->first;
	}
	return 0;
}

// ȡ��ɫ���ڵķ�����ID
DWORD PlayerBaseDataManager::GetServerIdByRoleId(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_mServerIdList.find(dwRoleId);
	if (iter == m_mServerIdList.end())
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ�����ҵķ�����ID[role_id:%d]", __FUNCTION__, dwRoleId);
		return 0;
	}

	return iter->second;
}

// ȡ��ɫ��ʼ���ڵķ�����ID
DWORD PlayerBaseDataManager::GetInitServerIdByRoleId(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_mInitServerIdList.find(dwRoleId);
	if (iter == m_mInitServerIdList.end())
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ�����ҵķ�����ID[role_id:%d]", __FUNCTION__, dwRoleId);
		return 0;
	}

	return iter->second;
}


// ���������������
void PlayerBaseDataManager::AddGiveEnergyData(DWORD dwServerId, DWORD dwRoleId, DWORD dwFriendId)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		//pBaseData->AddGiveEnergyData(dwFriendId);
		set<DWORD>::iterator iter = pBaseData->setFriendGiveEnergyList.find(dwRoleId);
		if (iter == pBaseData->setFriendGiveEnergyList.end())
		{
			pBaseData->setFriendGiveEnergyList.insert(dwRoleId);
		}
	}
}

// ��������������
void PlayerBaseDataManager::AddFriendApplyData(DWORD dwServerId, DWORD dwRoleId, DWORD dwTargetId)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		//pBaseData->AddFriendApplyData(dwTargetId);

		set<DWORD>::iterator iter = pBaseData->setFriendApplyList.find(dwTargetId);
		if (iter == pBaseData->setFriendApplyList.end())
		{
			pBaseData->setFriendApplyList.insert(dwTargetId);
		}
	}
}

// ��ӽ���
void PlayerBaseDataManager::AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		if (m_dwSeriesId >= INVALIDID)
			m_dwSeriesId = 1;

		RewardData tmp = tReward;
		tmp.dwUId = m_dwSeriesId++;

		// ��ʱ�洢,�ȴ��洢�ɹ����غ�Ҫɾ��,��ֹ��û�洢֮ǰ�û���¼,���½������ݶ�ʧ
		pBaseData->vRewardList.push_back(tmp);

		// �������ݿ�
		RewardCenterData_Save* pQuery = RewardCenterData_Save::ALLOC();
		pQuery->SetIndex(DB_REWARD_CENTER_DATA_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_CENTER_REWARD);
		pQuery->SetRoleId(dwRoleId);
		pQuery->SetServerId(dwServerId);
		pQuery->SetRewardData(tmp);
		g_MapServer.MapDBQuery(pQuery);
	}
	else
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]��ӽ���ʱ�Ҳ��������������[server_id:%d,role_id:%d]", __FUNCTION__, dwServerId, dwRoleId);
	}
}

// ɾ������
void PlayerBaseDataManager::RemoveReward(DWORD dwServerId, DWORD dwRoleId, DWORD dwRewardUId)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		vector<RewardData>::iterator iter = pBaseData->vRewardList.begin();
		for (; iter != pBaseData->vRewardList.end(); ++iter)
		{
			if (iter->dwUId == dwRewardUId)
			{
				pBaseData->vRewardList.erase(iter);
				break;
			}
		}
	}
}

// ��Ӻ�������
void PlayerBaseDataManager::AddFriendNum(DWORD dwServerId, DWORD dwRoleId, BYTE byFriendNum)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		pBaseData->byFriendNum += byFriendNum;
	}
}

// �޸Ľ�ɫ����
VOID PlayerBaseDataManager::RoleNameModify(DWORD dwServerId, const char* pszOldName, const char* pszNewName)
{
	// ���ҷ������������б�
	AllPlayerBaseDataNameMap::iterator list_iter = m_AllPlayerBaseDataNameList.find(dwServerId);
	if (list_iter != m_AllPlayerBaseDataNameList.end())
	{
		// �������Ƶ�����
		PlayerBaseDataNameMap::iterator iter = list_iter->second.find(pszOldName);
		if (iter != list_iter->second.end())
		{
			// ��������
			strcpy_s(iter->second->szRoleName, sizeof(iter->second->szRoleName) - 1, pszNewName);

			// �����µ�����
			list_iter->second.insert(make_pair(pszNewName, iter->second));

			// ɾ������������
			list_iter->second.erase(iter);
		}
	}
}

// ȡ�û�������
DWORD PlayerBaseDataManager::GetTotalPlayerNum()
{
	DWORD dwPlayerNum = 0;
	AllPlayerBaseDataMap::iterator iter = m_AllPlayerBaseDataList.begin();
	for (; iter != m_AllPlayerBaseDataList.end(); ++iter)
	{
		dwPlayerNum += iter->second.size();
	}
	return dwPlayerNum;
}

// ȡ���ID�б�
const vector<DWORD>* PlayerBaseDataManager::GetPlayerIdList(DWORD dwServerId)
{
	map<DWORD, vector<DWORD>>::iterator iter = m_vPlayerIdList.find(dwServerId);
	return iter != m_vPlayerIdList.end() ? &iter->second : NULL;
}

// ����ͨ���ӿ�(todo::��Ҫ����)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	return GetPlayerBaseDataInnerById(dwServerId, dwRoleId, bUpdate);
}

// ����ս��
void PlayerBaseDataManager::SetFightingPower(DWORD dwServerId, DWORD dwRoleId, DWORD dwFightingPower)
{
	PlayerBaseData *pBaseData =  GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
		pBaseData->dwFightingPower = dwFightingPower;
}



// �Ƿ���Ч�ʺ�
bool PlayerBaseDataManager::IsInvalidAccount(DWORD dwChannelId, const char *pszAccount)
{
	char szAccountKey[MAX_ACCOUNT_LEN] = { 0 };
	sprintf_s(szAccountKey, sizeof(szAccountKey)-1, "%s_%d", pszAccount, dwChannelId);
	map<string, vector<DWORD>>::iterator iter_account = m_mapPlayerAccountList.find(szAccountKey);
	return iter_account != m_mapPlayerAccountList.end() ? true : false;
}

// ȡĳ���ʺŵĽ�ɫID�б�
const vector<DWORD>* PlayerBaseDataManager::GetRoleIdList(DWORD dwChannelId, const char *pszAccount)
{
	char szAccountKey[MAX_ACCOUNT_LEN] = { 0 };
	sprintf_s(szAccountKey, sizeof(szAccountKey)-1, "%s_%d", pszAccount, dwChannelId);
	map<string, vector<DWORD>>::iterator iter = m_mapPlayerAccountList.find(szAccountKey);
	return iter != m_mapPlayerAccountList.end() ? &iter->second : NULL;
}

// ȡ���������(ID)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataInnerById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	// ��ֹ����ģ��û��ת��
	dwServerId = g_MapServer.GetCurServerId(dwServerId);

	// todo::���ܻ���ֹ�������û�м�ʱˢ�µ�����,�պ���

	AllPlayerBaseDataMap::iterator list_iter = m_AllPlayerBaseDataList.find(dwServerId);
	if (list_iter == m_AllPlayerBaseDataList.end())
		return NULL;

	PlayerBaseDataMap::iterator data_iter = list_iter->second.find(dwRoleId);
	
	PlayerBaseData *pBaseData = data_iter != list_iter->second.end() ? data_iter->second : NULL;

	// todo::���¹�������
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (pBaseData && pGuild)
		strcpy_s(pBaseData->szGuildName, sizeof(pBaseData->szGuildName) - 1, pGuild->GetGuildName());

	if (bUpdate && pBaseData)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
		if (pPlayer)
			pPlayer->FillBaseData(pBaseData);
	}

	return pBaseData;
}

PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataInnerByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate)
{
	// ��ֹ����ģ��û��ת��
	dwServerId = g_MapServer.GetCurServerId(dwServerId);

	AllPlayerBaseDataNameMap::iterator iter = m_AllPlayerBaseDataNameList.find(dwServerId);
	if (iter == m_AllPlayerBaseDataNameList.end())
		return NULL;

	PlayerBaseDataNameMap::iterator iter1 = iter->second.find(pszRoleName);
	return iter1 != iter->second.end() ? iter1->second : NULL;
}

const PlayerBaseData* g_GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	return PlayerBaseDataManager::Instance()->GetPlayerBaseDataById(dwServerId, dwRoleId, bUpdate);
}

const PlayerBaseData* g_GetPlayerBaseData(DWORD dwRoleId, bool bUpdate)
{
	DWORD dwServerId = PlayerBaseDataManager::Instance()->GetServerIdByRoleId(dwRoleId);
	return PlayerBaseDataManager::Instance()->GetPlayerBaseDataById(dwServerId, dwRoleId, bUpdate);
}