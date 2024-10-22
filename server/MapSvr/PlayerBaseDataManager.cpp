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
总结：
vector是可以快速地在最后添加删除元素,并可以快速地访问任意元素
list是可以快速地在所有地方添加删除元素,但是只能快速地访问最开始与最后的元素
deque在开始和最后添加元素都一样快,并提供了随机访问方法,像vector一样使用[]访问任意元素,但是随机访问速度比不上vector快,因为它要内部处理堆跳转
deque也有保留空间.另外,由于deque不要求连续空间,所以可以保存的元素比vector更大,这点也要注意一下.还有就是在前面和后面添加元素时都不需要移动其它块的元素,所以性能也很高。

因此在实际使用时，如何选择这三个容器中哪一个，应根据你的需要而定，一般应遵循下面
的原则：
1、如果你需要高效的随即存取，而不在乎插入和删除的效率，使用vector
2、如果你需要大量的插入和删除，而不关心随即存取，则应使用list
3、如果你需要随即存取，而且关心两端数据的插入和删除，则应使用deque。


1。vector  （连续的空间存储,可以使用[]操作符）快速的访问随机的元素，快速的在末尾插入元素，
但是在序列中间岁间的插入，删除元素要慢，而且如果一开始分配的空间不够的话，有一个重新分配更大空间，然后拷贝的性能开销.
2。deque （小片的连续，小片间用链表相连，实际上内部有一个map的指针，因为知道类型，
所以还是可以使用[]，只是速度没有vector快）快速的访问随机的元素，快速的在开始和末尾插入元素，
随机的插入，删除元素要慢，空间的重新分配要比vector快,重新分配空间后，原有的元素不需要拷贝。对deque的排序操作，可将deque先复制到vector，排序后在复制回deque。
3。list   （每个元素间用链表相连）访问随机元素不如vector快，随机的插入元素比vector快，对每个元素分配空间，所以不存在空间不够，重新分配的情况
4。set 内部元素唯一，用一棵平衡树结构来存储，因此遍历的时候就排序了，查找也比较快的哦。
5。map 一对一的映射的结合，key不能重复。
6。stack 适配器，必须结合其他的容器使用，stl中默认的内部容器是deque。先进后出，只有一个出口，不允许遍历。
7。queue 是受限制的deque，内部容器一般使用list较简单。先进先出，不允许遍历。

下面是选择顺序容器类型的一些准则
1.如果我们需要随机访问一个容器则vector要比list好得多 。

2.如果我们已知要存储元素的个数则vector 又是一个比list好的选择。

3.如果我们需要的不只是在容器两端插入和删除元素则list显然要比vector好

4.除非我们需要在容器首部插入和删除元素否则vector要比deque好。

5.如果只在容易的首部和尾部插入数据元素，则选择deque.

6.如果只需要在读取输入时在容器的中间位置插入元素，然后需要随机访问元素，则可考虑输入时将元素读入到一个List容器，
接着对此容器重新拍学，使其适合顺序访问，然后将排序后的list容器复制到一个vector容器中
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


// 玩家登录
void PlayerBaseDataManager::OnPlayerLogin(Player *pPlayer)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(pPlayer->GetServerId(), pPlayer->GetCharID(), false);
	if (pBaseData)
	{
		// 处理好友数据
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

		// 奖励数据
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

// 玩家下线
void PlayerBaseDataManager::OnPlayerLogout(Player *pPlayer)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(pPlayer->GetServerId(), pPlayer->GetCharID(), false);
	if (!pBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]逻辑出错!!!玩家下线时找不到基本数据[name:%s]", __FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	pPlayer->FillBaseData(pBaseData);
	pBaseData->dwOfflineTime = time(NULL);
}

// 添加服务器
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

// 添加离线数据
void PlayerBaseDataManager::AddPlayerBaseData(DWORD dwInitServerId, const PlayerBaseData &tBaseData)
{
	DWORD dwCurServerId = g_MapServer.GetCurServerId(dwInitServerId, tBaseData.dwRoleId);

	// todo::考虑是否需要调整为内存池管理
	PlayerBaseData *pBaseData = new PlayerBaseData();
	pBaseData->Init();
	memcpy_s(pBaseData, sizeof(PlayerCommonData), &tBaseData, sizeof(PlayerCommonData));

	pBaseData->vToBattleHeroList = tBaseData.vToBattleHeroList;

	// id列表
	AllPlayerBaseDataMap::iterator iter_id = m_AllPlayerBaseDataList.find(dwCurServerId);
	if (iter_id == m_AllPlayerBaseDataList.end())
	{
		PlayerBaseDataMap offline_list;
		m_AllPlayerBaseDataList.insert(make_pair(dwCurServerId, offline_list));
		iter_id = m_AllPlayerBaseDataList.find(dwCurServerId);
	}

	// 名字列表
	AllPlayerBaseDataNameMap::iterator iter_name = m_AllPlayerBaseDataNameList.find(dwCurServerId);
	if (iter_name == m_AllPlayerBaseDataNameList.end())
	{
		PlayerBaseDataNameMap offline_list;
		m_AllPlayerBaseDataNameList.insert(make_pair(dwCurServerId, offline_list));
		iter_name = m_AllPlayerBaseDataNameList.find(dwCurServerId);
	}

	iter_id->second.insert(make_pair(pBaseData->dwRoleId, pBaseData));

	iter_name->second.insert(make_pair(pBaseData->szRoleName, pBaseData));

	// ID列表
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

	// 帐号列表
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

	// 服务器ID列表
	if (m_mServerIdList.find(tBaseData.dwRoleId) == m_mServerIdList.end())
	{
		m_mServerIdList.insert(make_pair(tBaseData.dwRoleId, dwCurServerId));
	}

	if (m_mInitServerIdList.find(tBaseData.dwRoleId) == m_mInitServerIdList.end())
	{
		m_mInitServerIdList.insert(make_pair(tBaseData.dwRoleId, dwInitServerId));
	}
	
}

// 移除离线数据
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

// 取玩家列表
const PlayerBaseDataMap* PlayerBaseDataManager::GetPlayerList(DWORD dwServerId)
{
	AllPlayerBaseDataMap::iterator iter = m_AllPlayerBaseDataList.find(dwServerId);
	return iter != m_AllPlayerBaseDataList.end() ? &iter->second : NULL;
}

// 取玩基本数据(ID)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	return GetPlayerBaseDataInnerById(dwServerId, dwRoleId, bUpdate);
}

// 取玩基本数据(名称)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate)
{
	return GetPlayerBaseDataInnerByName(dwServerId, pszRoleName, bUpdate);
}

// 取玩基本数据(名称)
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

// 取玩基本数据(Id)
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

// 取玩基本数据(account)
const PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataByAccount(DWORD dwInitServerId, DWORD dwChannelId, const char *pszAccount)
{
	DWORD dwCurServerId = g_MapServer.GetCurServerId(dwInitServerId);

	if (!dwCurServerId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]参数dwCurServerId==0", __FUNCTION__);
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

// 取用户所在服务器
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

// 取角色所在的服务器ID
DWORD PlayerBaseDataManager::GetServerIdByRoleId(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_mServerIdList.find(dwRoleId);
	if (iter == m_mServerIdList.end())
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到玩家的服务器ID[role_id:%d]", __FUNCTION__, dwRoleId);
		return 0;
	}

	return iter->second;
}

// 取角色初始所在的服务器ID
DWORD PlayerBaseDataManager::GetInitServerIdByRoleId(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_mInitServerIdList.find(dwRoleId);
	if (iter == m_mInitServerIdList.end())
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到玩家的服务器ID[role_id:%d]", __FUNCTION__, dwRoleId);
		return 0;
	}

	return iter->second;
}


// 添加赠送体力数据
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

// 添加申请好友数据
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

// 添加奖励
void PlayerBaseDataManager::AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		if (m_dwSeriesId >= INVALIDID)
			m_dwSeriesId = 1;

		RewardData tmp = tReward;
		tmp.dwUId = m_dwSeriesId++;

		// 临时存储,等待存储成功返回后要删除,防止还没存储之前用户登录,导致奖励数据丢失
		pBaseData->vRewardList.push_back(tmp);

		// 插入数据库
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
		MAPLOG(IMPORTANT_LEVEL, "[%s]添加奖励时找不到基本玩家数据[server_id:%d,role_id:%d]", __FUNCTION__, dwServerId, dwRoleId);
	}
}

// 删除奖励
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

// 添加好友数量
void PlayerBaseDataManager::AddFriendNum(DWORD dwServerId, DWORD dwRoleId, BYTE byFriendNum)
{
	PlayerBaseData *pBaseData = GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
	{
		pBaseData->byFriendNum += byFriendNum;
	}
}

// 修改角色名称
VOID PlayerBaseDataManager::RoleNameModify(DWORD dwServerId, const char* pszOldName, const char* pszNewName)
{
	// 查找服务器的名称列表
	AllPlayerBaseDataNameMap::iterator list_iter = m_AllPlayerBaseDataNameList.find(dwServerId);
	if (list_iter != m_AllPlayerBaseDataNameList.end())
	{
		// 查找名称的数据
		PlayerBaseDataNameMap::iterator iter = list_iter->second.find(pszOldName);
		if (iter != list_iter->second.end())
		{
			// 更改名称
			strcpy_s(iter->second->szRoleName, sizeof(iter->second->szRoleName) - 1, pszNewName);

			// 插入新的数据
			list_iter->second.insert(make_pair(pszNewName, iter->second));

			// 删除旧名称数据
			list_iter->second.erase(iter);
		}
	}
}

// 取用户总数量
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

// 取玩家ID列表
const vector<DWORD>* PlayerBaseDataManager::GetPlayerIdList(DWORD dwServerId)
{
	map<DWORD, vector<DWORD>>::iterator iter = m_vPlayerIdList.find(dwServerId);
	return iter != m_vPlayerIdList.end() ? &iter->second : NULL;
}

// 特殊通道接口(todo::不要滥用)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	return GetPlayerBaseDataInnerById(dwServerId, dwRoleId, bUpdate);
}

// 设置战力
void PlayerBaseDataManager::SetFightingPower(DWORD dwServerId, DWORD dwRoleId, DWORD dwFightingPower)
{
	PlayerBaseData *pBaseData =  GetPlayerBaseDataInnerById(dwServerId, dwRoleId, false);
	if (pBaseData)
		pBaseData->dwFightingPower = dwFightingPower;
}



// 是否有效帐号
bool PlayerBaseDataManager::IsInvalidAccount(DWORD dwChannelId, const char *pszAccount)
{
	char szAccountKey[MAX_ACCOUNT_LEN] = { 0 };
	sprintf_s(szAccountKey, sizeof(szAccountKey)-1, "%s_%d", pszAccount, dwChannelId);
	map<string, vector<DWORD>>::iterator iter_account = m_mapPlayerAccountList.find(szAccountKey);
	return iter_account != m_mapPlayerAccountList.end() ? true : false;
}

// 取某个帐号的角色ID列表
const vector<DWORD>* PlayerBaseDataManager::GetRoleIdList(DWORD dwChannelId, const char *pszAccount)
{
	char szAccountKey[MAX_ACCOUNT_LEN] = { 0 };
	sprintf_s(szAccountKey, sizeof(szAccountKey)-1, "%s_%d", pszAccount, dwChannelId);
	map<string, vector<DWORD>>::iterator iter = m_mapPlayerAccountList.find(szAccountKey);
	return iter != m_mapPlayerAccountList.end() ? &iter->second : NULL;
}

// 取玩基本数据(ID)
PlayerBaseData* PlayerBaseDataManager::GetPlayerBaseDataInnerById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate)
{
	// 防止部分模块没做转换
	dwServerId = g_MapServer.GetCurServerId(dwServerId);

	// todo::可能会出现工会名称没有及时刷新的问题,日后处理

	AllPlayerBaseDataMap::iterator list_iter = m_AllPlayerBaseDataList.find(dwServerId);
	if (list_iter == m_AllPlayerBaseDataList.end())
		return NULL;

	PlayerBaseDataMap::iterator data_iter = list_iter->second.find(dwRoleId);
	
	PlayerBaseData *pBaseData = data_iter != list_iter->second.end() ? data_iter->second : NULL;

	// todo::更新工会名称
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
	// 防止部分模块没做转换
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