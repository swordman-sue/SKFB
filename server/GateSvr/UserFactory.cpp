#include "StdAfx.h"
#include "UserFactory.h"
#include "User.h"

UserFactory::UserFactory(void)
:	m_pUserPool(NULL),
	m_pPlayerKeyGenerator(NULL)
{

}

UserFactory::~UserFactory(void)
{

}

VOID UserFactory::Init()
{
	m_pUserPool = new CMemoryPoolFactory<User>;
	m_pUserPool->Initialize(200, 100);

	m_pPlayerKeyGenerator = new ObjKeyGenerator;
	m_pPlayerKeyGenerator->Create(1, 30000);
}

VOID UserFactory::Release()
{
	if(m_pUserPool)
	{
		delete m_pUserPool;
		m_pUserPool = NULL;
	}
	if(m_pPlayerKeyGenerator)
	{
		delete m_pPlayerKeyGenerator;
		m_pPlayerKeyGenerator = NULL;
	}
}

UserSession* UserFactory::AllocUser()
{
	User* pUser = m_pUserPool->Alloc();
	pUser->Init();
	DWORD dwKey = m_pPlayerKeyGenerator->Getkey();
	pUser->SetGUID(dwKey);
	return pUser;
}

VOID UserFactory::FreeUser(UserSession* pUser)
{
	User* pAuthUser = (User*)pUser;
	m_pPlayerKeyGenerator->RestoreKey(pAuthUser->GetGUID());
	m_pUserPool->Free(pAuthUser);
}