#include "StdAfx.h"
#include "PHPSessionManager.h"
#include "PHPSession.h"
#include "PHPSessionFactory.h"
//#include "MapServerSession.h"
#include <time.h>

PHPSessionManager::PHPSessionManager(void)
{
	time( &m_tNowTime );
	m_tLastTime = m_tNowTime;
}

PHPSessionManager::~PHPSessionManager(void)
{

}

//添加玩家到管理器
bool PHPSessionManager::AddUser(PHPSession* pUser)
{
	_ASSERT(pUser != NULL);

	if(!pUser)
		return false;
	
	//查找是否已经存在此玩家
	Guild_Map::iterator it = m_GUIDMap.find(pUser->GetGUID());

	//不可能出现重复GUID的情况
	_ASSERT(it==m_GUIDMap.end());

	if(it != m_GUIDMap.end())
		return false;

	//加入玩家列表
	m_GUIDMap.insert(std::make_pair(pUser->GetGUID(),pUser));

	return true;
}

//通过GUID查找玩家
PHPSession* PHPSessionManager::FindUserByGUID(DWORD dwGUID)
{
	Guild_Map::iterator itguid = m_GUIDMap.find(dwGUID);
	if(itguid != m_GUIDMap.end())
		return itguid->second;

	return NULL;
}

//从管理器删除玩家
bool PHPSessionManager::RemoveUser(DWORD dwGUID)
{
	m_GUIDMap.erase(dwGUID);

	return true;
}

//释放管理器
VOID PHPSessionManager::Release( PHPSessionFactory* pUserFactory )
{
	Guild_Map::iterator it;

	PHPSession* pUser = NULL;
	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();++it)
	{
		pUser = it->second;
		if(pUser)
			pUserFactory->FreePHPSession(pUser);
	}

	m_GUIDMap.clear();
}

//地图服务器断开连接事件处理
void PHPSessionManager::OnMapSvrDisconnect()
{
	Guild_Map::iterator it;
	PHPSession* pUser = NULL;

	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();)
	{
		pUser = it->second;

		++it;

		if(pUser)
		{
			//pUser->SetDisConnectFromMap(TRUE);
			pUser->Disconnect();
		}
	}
}

//连接超时检测
void PHPSessionManager::CheckTimeOut()
{
  	time( &m_tNowTime );

	// 检测时间间隔 todo::暂定5秒
	if (m_tNowTime >= m_tLastTime + 5)
	{
		PHPSession* pSession = NULL;
		for (Guild_Map::iterator it = m_GUIDMap.begin(); it != m_GUIDMap.end();)
		{
			pSession = it->second;

			// 放在此处是为了防止原迭代器为空
			++it;

			if (m_tNowTime >= pSession->GetCreateTime() + 120)
			{
				OPENLOG(GUI_LEVEL, "连接超时被断开[GUID:%d]", pSession->GetGUID());

				//客户端超时
				pSession->Disconnect();
			}
		}

		m_tLastTime = m_tNowTime;
	}
}

void PHPSessionManager::Update()
{
	CheckTimeOut();
}