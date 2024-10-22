#pragma once

#include "../ToolKit/Singleton.h"
#include <hash_map>
#include <map>

class User;
class UserFactory;
class MapServerSession;
class ServerSession;

class UserManager : public Singleton<UserManager>
{
public:
	UserManager(void);
	~UserManager(void);

	VOID	Release(UserFactory* pUserFactory);

public:
	bool	AddUser(User* pUser);
	bool	RemoveUser(DWORD dwGUID);
	User*	FindUserByGUID(DWORD dwGUID);
	DWORD	GetPlayerNum() { return (DWORD)m_GUIDMap.size(); }
	void	OnMapSvrDisconnect();
	void	Update();

	void	SendPacket(WORD dwMsgID, unsigned char * pData, WORD wDataSize, DWORD dwServerId = 0);
private:
	void	CheckTimeOut();

private:
	//std::hash_map<DWORD, User*>		m_GUIDMap;			//����б�
	typedef std::map<DWORD, User*>	UserMap;				//����б�
	UserMap							m_GUIDMap;
	time_t							m_tNowTime;			//ʱ���
	time_t							m_tLastTime;		//ʱ���

	User*							m_UserLoopList[50000];
	DWORD							m_UserLoopListCount;

	UINT64							m_ulCheckTimeOutTimer;
};