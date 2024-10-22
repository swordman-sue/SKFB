#pragma once

#include "../ToolKit/Singleton.h"
#include "../Common/Const.h"
#include "../ToolKit/MemoryPool.h"

class ServerSession;
class MapServerSession;

class ServerSessionFactory : public Singleton<ServerSessionFactory>
{
public:
	ServerSessionFactory(void);
	~ServerSessionFactory(void);

public:
	VOID				Init();
	VOID				Release();
	ServerSession*		AllocServerSession(SERVER_TYPE eServerType);
	VOID				FreeServerSession(ServerSession* pServerSession);

private:
	CMemoryPoolFactory<MapServerSession>* m_pMapServerPool;
};
