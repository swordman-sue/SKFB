#include "stdafx.h"
#include "ServerSession.h"
#include "MapServerSession.h"
#include "ServerSessionManager.h"
#include "ServerSessionFactory.h"

ServerSessionManager::ServerSessionManager(void)
{

}

ServerSessionManager::~ServerSessionManager(void)
{

}

VOID ServerSessionManager::Init()
{
	m_pMapSessionMap = new std::map<DWORD,MapServerSession* >;
}

VOID ServerSessionManager::Release(ServerSessionFactory* pServerSessionFactory)
{
	if( m_pMapSessionMap )
	{
		MapServerSession * pServer = NULL;
		std::map<DWORD,MapServerSession* >::iterator it;

		for( it = m_pMapSessionMap->begin();it!=m_pMapSessionMap->end();++it )
		{
			pServer = it->second;
			pServer->Release();
			pServerSessionFactory->FreeServerSession(pServer);
			
		}
		m_pMapSessionMap->clear();

		delete m_pMapSessionMap;
		m_pMapSessionMap = NULL;
	}
}

VOID ServerSessionManager::AddMapServer( MapServerSession * pServer )
{	
	m_pMapSessionMap->insert(std::make_pair(pServer->GetServerID(),pServer));
}

MapServerSession *	ServerSessionManager::FindMapServer( DWORD dwServerID )
{
	std::map<DWORD,MapServerSession*>::iterator it = m_pMapSessionMap->find(dwServerID);
	if(it!=m_pMapSessionMap->end())
		return it->second;
	else
		return NULL;
}

VOID ServerSessionManager::RemoveMapServer( DWORD dwServerID )
{
	std::map<DWORD,MapServerSession*>::iterator it = m_pMapSessionMap->find(dwServerID);
	if(it==m_pMapSessionMap->end())
		return;
	
	MapServerSession * pServer = it->second;
	if(!pServer)
		return;

	pServer->Release();
	m_pMapSessionMap->erase(it);
}


VOID ServerSessionManager::Process()
{

}