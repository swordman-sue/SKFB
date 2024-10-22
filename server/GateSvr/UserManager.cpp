#include "StdAfx.h"
#include "UserManager.h"
#include "User.h"
#include "UserFactory.h"
#include "MapServerSession.h"
#include <time.h>

UserManager::UserManager(void)
{
	time( &m_tNowTime );
	m_tLastTime = m_tNowTime;

	m_UserLoopListCount = 0;

	m_ulCheckTimeOutTimer = GetTickCount64();
}

UserManager::~UserManager(void)
{

}

//添加玩家到管理器
bool UserManager::AddUser(User* pUser)
{
	_ASSERT(pUser != NULL);

	if(!pUser)
		return false;
	
	//查找是否已经存在此玩家
	UserMap::iterator it =	m_GUIDMap.find(pUser->GetGUID());

	//不可能出现重复GUID的情况
	_ASSERT(it==m_GUIDMap.end());

	if(it != m_GUIDMap.end())
		return false;

	//加入玩家列表
	m_GUIDMap.insert(std::make_pair(pUser->GetGUID(),pUser));

	// 更新遍历列表
	pUser->SetIndex(m_UserLoopListCount);
	m_UserLoopList[m_UserLoopListCount] = pUser;
	++m_UserLoopListCount;

	return true;
}

//通过GUID查找玩家
User* UserManager::FindUserByGUID(DWORD dwGUID)
{
	UserMap::iterator itguid = m_GUIDMap.find(dwGUID);
	if(itguid != m_GUIDMap.end())
		return itguid->second;

	return NULL;
}

//从管理器删除玩家
bool UserManager::RemoveUser(DWORD dwGUID)
{
	User* pRemoveUser = FindUserByGUID(dwGUID);
	if (m_UserLoopListCount && pRemoveUser)
	{
		User *pLastUser = m_UserLoopList[m_UserLoopListCount - 1];
		pLastUser->SetIndex(pRemoveUser->GetIndex());
		m_UserLoopList[pRemoveUser->GetIndex()] = pLastUser;
		--m_UserLoopListCount;
	}

	m_GUIDMap.erase(dwGUID);

	return true;
}

//释放管理器
VOID UserManager::Release( UserFactory* pUserFactory )
{
	UserMap::iterator it;

	User* pUser = NULL;
	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();++it)
	{
		pUser = it->second;
		if(pUser)
			pUserFactory->FreeUser(pUser);
	}

	m_GUIDMap.clear();
	m_UserLoopListCount = 0;
}

//地图服务器断开连接事件处理
void UserManager::OnMapSvrDisconnect()
{
	UserMap::iterator it;
	User* pUser = NULL;

	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();)
	{
		pUser = it->second;

		++it;

		if(pUser)
		{
			pUser->SetDisConnectFromMap(TRUE);
			pUser->Disconnect();
		}
	}
}

//连接超时检测
void UserManager::CheckTimeOut()
{
	//time_t tTimeGap;
	time(&m_tNowTime);

	// 10秒检测一次
	if (GetTickCount64() >= m_ulCheckTimeOutTimer + 5000)
	{
		User* pUser = NULL;
		for (DWORD i = 0; i < m_UserLoopListCount; ++i)
		{
			pUser = m_UserLoopList[i];
			if (!pUser)
				continue;

			if (m_tNowTime >= pUser->GetStamp() + g_GateServer.GetSocketTimeOut())
			{
				GATELOG(GUI_LEVEL, "心跳包超时被断开[GUID:%d]", pUser->GetGUID());

				//客户端超时
				pUser->Disconnect();
			}	
		}

		m_ulCheckTimeOutTimer = GetTickCount64();
	}

	m_tLastTime = m_tNowTime;
}

//连接超时检测
// void UserManager::CheckTimeOut()
// {
// 	//time_t tTimeGap;
// 	time(&m_tNowTime);
// 
// 	// 检测时间间隔
// 	if ((m_tNowTime - m_tLastTime) < g_GateServer.GetCheckTimeGap())
// 	{
// 		return;
// 	}
// 
// 	//UserMap::iterator it;
// 	User* pUser = NULL;
// 	for (UserMap::iterator it = m_GUIDMap.begin(); it != m_GUIDMap.end();)
// 	{
// 		pUser = it->second;
// 
// 		// 放在此处是为了防止原迭代器为空
// 		++it;
// 
// 		// 建立连接5分钟后再做检测
// 		if (pUser && m_tNowTime >= pUser->GetCreateTime() + 300)
// 		{
// 			if (pUser->GetSendFlag() == TRUE)
// 			{
// 				pUser->Process(DWORD(m_tNowTime));
// 				if (m_tNowTime >= pUser->GetStamp() + g_GateServer.GetSocketTimeOut())
// 				{
// 					GATELOG(GUI_LEVEL, "心跳包超时被断开[GUID:%d]", pUser->GetGUID());
// 
// 					//客户端超时
// 					pUser->Disconnect();
// 				}
// 			}
// 			else
// 			{
// 				if (m_tNowTime >= pUser->GetCreateTime() + g_GateServer.GetConnectTimeOut())
// 				{
// 					GATELOG(GUI_LEVEL, "连接超时被断开[GUID:%d]", pUser->GetGUID());
// 
// 					//客户端超时
// 					pUser->Disconnect();
// 				}
// 			}
// 		}
// 	}
// 
// 	m_tLastTime = m_tNowTime;
// }

void UserManager::Update()
{
	CheckTimeOut();
}

void UserManager::SendPacket(WORD dwMsgID, unsigned char * pData, WORD wDataSize, DWORD dwServerId)
{
	if (dwServerId)
	{
		for (DWORD i = 0; i < m_UserLoopListCount; ++i)
		{
			User *pUser = m_UserLoopList[i];
			if (pUser->GetServerId() == dwServerId)
				pUser->SendPacket(dwMsgID, pData, wDataSize);
		}
	}
	else
	{
		for (DWORD i = 0; i < m_UserLoopListCount; ++i)
		{
			m_UserLoopList[i]->SendPacket(dwMsgID, pData, wDataSize);
		}
	}
}