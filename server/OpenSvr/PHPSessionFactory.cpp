#include "stdafx.h"
#include "PHPSessionFactory.h"
#include "PHPSession.h"
#include "..\ToolKit\ObjKeyGenerator.h"

PHPSessionFactory::PHPSessionFactory(void)
:	m_pPHPSessionPool(NULL),
	m_pSessionKeyGenerator(NULL)
{

}

PHPSessionFactory::~PHPSessionFactory(void)
{

}

VOID PHPSessionFactory::Init()
{
	//初始化PHP会话池
	m_pPHPSessionPool = new CMemoryPoolFactory<PHPSession>;

	m_pPHPSessionPool->Initialize(100,50);

	m_pSessionKeyGenerator = new ObjKeyGenerator;
	m_pSessionKeyGenerator->Create(1, 30000);
}

VOID PHPSessionFactory::Release()
{
	if(m_pPHPSessionPool)
	{
		delete m_pPHPSessionPool;
		m_pPHPSessionPool = NULL;
	}

	if(m_pSessionKeyGenerator)
	{
		delete m_pSessionKeyGenerator;
		m_pSessionKeyGenerator = NULL;
	}
}

PHPSession * PHPSessionFactory::AllocPHPSession()
{
	PHPSession * pSession = NULL;
	
	pSession = m_pPHPSessionPool->Alloc();

	if (pSession)
	{
		pSession->Init();

		pSession->SetGUID(m_pSessionKeyGenerator->Getkey());
	}
	return pSession;
}

VOID PHPSessionFactory::FreePHPSession( ServerSession * pServerSession )
{
	if (pServerSession)
	{
		m_pSessionKeyGenerator->RestoreKey(pServerSession->GetGUID());

		pServerSession->Release();
	}

	return m_pPHPSessionPool->Free( (PHPSession *)pServerSession );
}
