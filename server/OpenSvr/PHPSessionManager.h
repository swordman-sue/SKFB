#pragma once

#include "../ToolKit/Singleton.h"
#include <hash_map>

class PHPSession;
class PHPSessionFactory;
class MapServerSession;
class ServerSession;

class PHPSessionManager : public Singleton<PHPSessionManager>
{
public:
	PHPSessionManager(void);
	~PHPSessionManager(void);

	VOID	Release(PHPSessionFactory* pUserFactory);

public:
	bool			AddUser(PHPSession* pUser);
	bool			RemoveUser(DWORD dwGUID);
	PHPSession*		FindUserByGUID(DWORD dwGUID);
	DWORD			GetPlayerNum() { return (DWORD)m_GUIDMap.size(); }
	void			OnMapSvrDisconnect();
	void			Update();

private:
	void			CheckTimeOut();

private:
	//std::hash_map<DWORD, PHPSession*>		m_GUIDMap;			//PHP列表
	typedef std::map<DWORD, PHPSession*>	Guild_Map;
	Guild_Map								m_GUIDMap;			//PHP列表
	time_t									m_tNowTime;			//时间戳
	time_t									m_tLastTime;		//时间戳
};