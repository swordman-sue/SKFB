#pragma once

#include "../Common/Const.h"
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"

class ServerSession;
class MapServerSession;

//�������Ự����
class ServerSessionFactory : public Singleton<ServerSessionFactory>
{
public:
	ServerSessionFactory(void);
	~ServerSessionFactory(void);

public:
	VOID				Init();
	VOID				Release();
	ServerSession *		AllocServerSession( SERVER_TYPE eServerType );
	VOID				FreeServerSession( ServerSession * pServerSession );

private:
	CMemoryPoolFactory<MapServerSession>* m_pMapServerPool;
};
