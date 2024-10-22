#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"
#include "../ToolKit/ObjKeyGenerator.h"
#include "UserSession.h"

class User;
class UserSession;

class UserFactory : public Singleton<UserFactory>
{
public:
	UserFactory(void);
	~UserFactory(void);

public:
	VOID			Init();
	VOID			Release();
	UserSession*	AllocUser();
	VOID			FreeUser(UserSession* pUser);

private:
	CMemoryPoolFactory<User>*	m_pUserPool;				//����ڴ��
	ObjKeyGenerator*			m_pPlayerKeyGenerator;		//���GUID������
};
