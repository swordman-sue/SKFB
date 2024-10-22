#include "stdafx.h"
#include "ServerSessionFactory.h"
#include "MapServerSession.h"

ServerSessionFactory::ServerSessionFactory(void)
:	m_pMapServerPool(NULL)
{

}

ServerSessionFactory::~ServerSessionFactory(void)
{
	Release();
}

VOID ServerSessionFactory::Init()
{

	m_pMapServerPool = new CMemoryPoolFactory<MapServerSession>;

	m_pMapServerPool->Initialize(100,50);
}

VOID ServerSessionFactory::Release()
{
	if(m_pMapServerPool)
	{
		delete m_pMapServerPool;
		m_pMapServerPool = NULL;
	}
}

ServerSession * ServerSessionFactory::AllocServerSession( SERVER_TYPE eServerType )
{
	ServerSession * pSession = NULL;
	switch( eServerType )
	{
	case MAP_SERVER:
		{
			pSession = m_pMapServerPool->Alloc();
			if (pSession)
			{
				pSession->Init();
			}
			return pSession;
		}
		break;
	}
	return NULL;
}

VOID ServerSessionFactory::FreeServerSession( ServerSession * pServerSession )
{

	switch( pServerSession->GetServerType() )
	{
	case MAP_SERVER:
		if (pServerSession)
		{
			pServerSession->Release();
		}
		return m_pMapServerPool->Free( (MapServerSession *)pServerSession );

	default:
		{
			//–¥»’÷æ
			//OPENLOG("")
		}
	}
}
