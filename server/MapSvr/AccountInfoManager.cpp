////////////////////////////////////////////////////////////////////////////////
//	Filename     :AccountInfoManager.cpp
//	Created by   :Kevin
//	Created date :2014/12/17
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AccountInfoManager.h"
#include "ObjectFactory.h"
#include "AccountInfo.h"
#include "PlayerManager.h"
#include "Player.h"
#include "..\Common\CommonLogic.h"

AccountInfoManager::AccountInfoManager(void)
{
	//初始化DBThread服务的玩家人数
	memset(m_dwMapDBUser, 0, sizeof(m_dwMapDBUser));
}

AccountInfoManager::~AccountInfoManager(void)
{

}

//以ZoneID,AccID为KEY查找玩家
AccountInfo* AccountInfoManager::GetAccountInfoByAccID(DWORD dwPlatform, const char* pszAccID)
{
	AccKey tAccKey = {0};

	tAccKey.m_dwPlatform = dwPlatform;
	strncpy(tAccKey.m_szAccID, pszAccID, sizeof(tAccKey.m_szAccID));
	AccIDMapIter it = m_AccountInfoByAccIDMap.find(tAccKey);

	if(it!=m_AccountInfoByAccIDMap.end())
		return it->second;
	else
		return NULL;
}

//以GUID为KEY查找玩家
AccountInfo* AccountInfoManager::GetAccountInfoByGUID(DWORD dwGUID)
{
	GuidMapIter it = m_AccountInfoByGUIDMap.find(dwGUID);

	if(it!=m_AccountInfoByGUIDMap.end())
		return it->second;
	else
		return NULL;
}

VOID AccountInfoManager::AddAccountInfo(AccountInfo* pAccountInfo)
{
   CHECK_POINTER(pAccountInfo);

   AccountInfo* pInfo = GetAccountInfoByAccID(pAccountInfo->GetChannelUID(), pAccountInfo->GetAccID());
	if(pInfo)
	{
		MAPLOG(ERROR_LEVEL, "AddAccountInfo失败!");
		return;
	}

	//分配DBThread索引号
	pAccountInfo->SetDBIndex(GetMapDBIndex());
	
	//追加至管理器
	AccKey tAccKey = {0};
	tAccKey.m_dwPlatform = pAccountInfo->GetChannelUID();
	strncpy(tAccKey.m_szAccID, pAccountInfo->GetAccID(), sizeof(tAccKey.m_szAccID));
	m_AccountInfoByAccIDMap.insert(std::make_pair(tAccKey,pAccountInfo));

	m_AccountInfoByGUIDMap.insert(std::make_pair(pAccountInfo->GetGuid(),pAccountInfo));
}

VOID AccountInfoManager::RemoveAccountInfo(AccountInfo* pAccountInfo)
{
	if(!pAccountInfo)
		return;

	//释放DBThread索引号
	ReleaseMapDBIndex(pAccountInfo->GetDBIndex());

	AccKey tAccKey = {0};
	tAccKey.m_dwPlatform = pAccountInfo->GetChannelUID();
	strncpy(tAccKey.m_szAccID, pAccountInfo->GetAccID(), sizeof(tAccKey.m_szAccID));
	m_AccountInfoByAccIDMap.erase(tAccKey);
	m_AccountInfoByGUIDMap.erase(pAccountInfo->GetGuid());
}

VOID AccountInfoManager::RemoveAccountInfoByGUID(DWORD dwGUID)
{
	AccountInfo* pInfo = GetAccountInfoByGUID(dwGUID);
	if(!pInfo)
		return;

	//释放DBThread索引号
	ReleaseMapDBIndex(pInfo->GetDBIndex());

	AccKey tAccKey = {0};
	tAccKey.m_dwPlatform = pInfo->GetChannelUID();
	strncpy(tAccKey.m_szAccID, pInfo->GetAccID(), sizeof(tAccKey.m_szAccID));
	m_AccountInfoByAccIDMap.erase(tAccKey);
	m_AccountInfoByGUIDMap.erase(pInfo->GetGuid());
}

VOID AccountInfoManager::Release()
{
	for (AccIDMapIter it = m_AccountInfoByAccIDMap.begin();it != m_AccountInfoByAccIDMap.end(); ++it)
	{
		if(it->second)
			ObjectFactory::Instance()->FreeAccountInfo(it->second);
	}

	m_AccountInfoByAccIDMap.clear();
	m_AccountInfoByGUIDMap.clear();
}

//踢出玩家
VOID AccountInfoManager::KickoutAccount(AccountInfo* pAccInfo)
{
	CHECK_POINTER(pAccInfo)

	USER_STATE UserState = pAccInfo->GetUserState();

	//当前用户处于下线中状态
	if (UserState == USER_LOGOUTING_STATE)
	{	
		MAPLOG(GUI_LEVEL, "踢出玩家失败UserState == USER_LOGOUTING_STATE[%s]", pAccInfo->GetAccID());
		return;
	}
	//当前用户处于选择角色状态  todo::当前用户处于选择服务器或创建角色状态
	else if (UserState == USER_SELECT_SERVER_STATE || 
		UserState == USER_CREATE_ROLE_STATE || 
		UserState == USER_READY_CREATE_ROLE_STATE)
	{
		//移除账号信息
		RemoveAccountInfo(pAccInfo);
		ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
	}
	//当前用户处于加载角色状态
	else if (UserState == USER_LOAD_STATE)
	{
		//角色数据加载状态或者数据加载完成状态
		//1 移除玩家信息
		Player* pPlayer = pAccInfo->GetPlayer();
		if(!pPlayer)
		{
			MAPLOG(GUI_LEVEL, "KickoutAccount(USER_LOAD_STATE):查找玩家对象失败，ACCID=%s.", pAccInfo->GetAccID());
			RemoveAccountInfo(pAccInfo);
			ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
			return;
		}
		PlayerManager::Instance()->RemovePlayer(pPlayer);

		//2 移除账号信息
		RemoveAccountInfo(pAccInfo);
		ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
	}
	//当前用户处于游戏中状态
	else if (UserState == USER_AT_GAME_STATE)
	{
		//角色在游戏中，需要先保存玩家信息到DB
		Player* pPlayer = pAccInfo->GetPlayer();
		if(!pPlayer)
		{
			MAPLOG(GUI_LEVEL, "KickoutAccount(USER_AT_GAME_STATE):查找玩家对象失败，ACCID=%s.", pAccInfo->GetAccID());
			RemoveAccountInfo(pAccInfo);
			ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
			return;
		}
		
		//玩家Logout
		pPlayer->OnLogout();

		//保存数据到DB
		pPlayer->OnSave(SAVE_DB_TYPE_LOGOUT);

		//设置玩家状态为下线中,等待DB保存完毕后进行后续处理
		pAccInfo->SetUserState(USER_LOGOUTING_STATE);
	}
}

VOID AccountInfoManager::KickoutAllAccount()
{
	AccIDMapIter it;

	for (it = m_AccountInfoByAccIDMap.begin();it!=m_AccountInfoByAccIDMap.end();)
	{
		AccountInfo* pInfo = it->second;

		++it;

		if(pInfo)
		{
			KickoutAccount(pInfo);
		}
	}
}

VOID AccountInfoManager::OnGateSvrDisconnect()
{
	KickoutAllAccount();
}

VOID AccountInfoManager::ProcessDBLogoutAck(AccountInfo* pAccInfo)
{
	if (pAccInfo)
	{
		if (pAccInfo->GetUserState() == USER_LOGOUTING_STATE)
		{
			PlayerManager::Instance()->RemovePlayer(pAccInfo->GetPlayer());
			RemoveAccountInfo(pAccInfo);
			ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
		}
		else
		{
			//玩家状态不匹配，写日志
			Player* pPlayer = pAccInfo->GetPlayer();
			MAPLOG(GUI_LEVEL, "玩家下线处理状态不匹配CharID=%u,UserState=%d", pPlayer->GetCharID(), pAccInfo->GetUserState());
		}
	}
}

DWORD AccountInfoManager::GetDBThreadUserNum(BYTE byIndex)
{
	if (byIndex >= MAX_DB_CONN)
		return 0;

	return m_dwMapDBUser[byIndex];
}

int	AccountInfoManager::GetMapDBIndex()
{
	DWORD dwMinUserNum = MAXDWORD;
	int nBestIndex = 1;

	for (int i = 1;i<MAX_DB_CONN;++i)
	{
		if(m_dwMapDBUser[i]<dwMinUserNum)
		{
			dwMinUserNum = m_dwMapDBUser[i];
			nBestIndex = i;
		}
	}

	//增加服务的玩家人数
	++m_dwMapDBUser[nBestIndex];

	return nBestIndex;
}

void AccountInfoManager::ReleaseMapDBIndex(int nIndex)
{
	if(nIndex<0 || nIndex>=MAX_DB_CONN)
		return;

	if(m_dwMapDBUser[nIndex]==0)
	{
		MAPLOG(GUI_LEVEL,"ReleaseMapDBIndex调用出错");
		return;
	}

	--m_dwMapDBUser[nIndex];
}