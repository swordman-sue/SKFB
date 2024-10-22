#pragma once

#include <map>
#include "../ToolKit/Singleton.h"
class ServerSession;
class MapServerSession;
class ServerSessionFactory;

//�������Ự������
class ServerSessionManager : public Singleton<ServerSessionManager>
{
public:
	ServerSessionManager(void);
	~ServerSessionManager(void);

	VOID				Init();
	VOID				Release(ServerSessionFactory* pServerSessionFactory);

	VOID				Process();

	VOID				AddMapServer( MapServerSession * pServer);
	VOID				RemoveMapServer( DWORD dwServerID );
	MapServerSession*	FindMapServer( DWORD dwServerID );

private:
	std::map<DWORD,MapServerSession*>*	m_pMapSessionMap;
};
