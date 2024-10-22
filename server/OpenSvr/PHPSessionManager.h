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
	//std::hash_map<DWORD, PHPSession*>		m_GUIDMap;			//PHP�б�
	typedef std::map<DWORD, PHPSession*>	Guild_Map;
	Guild_Map								m_GUIDMap;			//PHP�б�
	time_t									m_tNowTime;			//ʱ���
	time_t									m_tLastTime;		//ʱ���
};