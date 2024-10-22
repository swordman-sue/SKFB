#pragma once

#include "../Common/Const.h"
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"

class PHPSession;
class ServerSession;
class ObjKeyGenerator;

//PHP�Ự����
class PHPSessionFactory : public Singleton<PHPSessionFactory>
{
public:
	PHPSessionFactory(void);
	~PHPSessionFactory(void);

public:
	VOID				Init();
	VOID				Release();
	PHPSession *		AllocPHPSession();
	VOID				FreePHPSession( ServerSession * pServerSession );

private:
	CMemoryPoolFactory<PHPSession>*	m_pPHPSessionPool;
	ObjKeyGenerator*			m_pSessionKeyGenerator;		//���GUID������
};
