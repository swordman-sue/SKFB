#pragma once

#include "../ToolKit/Singleton.h"
#include <hash_map>
#include <map>
#include <string>
class NetMsgBody;
class ServerSession;
class User;

using namespace std;

//消息处理函数原型
typedef VOID(*fnHandler)(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
typedef VOID(*fnHandler_C)(User * pSession, NetMsgBody *pMsg);

//服务端处理类
class FunctionMap
{
public:
	void Add(DWORD dwKey,fnHandler pfnHandler)
	{
		IF_NOT (m_HandlerMap.insert(std::make_pair(dwKey,pfnHandler)).second)
		{
			GATELOG(ERROR_LEVEL, "添加消息 Handler 失败，消息将无法得到处理（MSGID=%d），通常是因为消息 ID 重复引起", dwKey);
		}
	}

	fnHandler Find(DWORD dwKey)
	{
		std::map<DWORD,fnHandler>::iterator it = m_HandlerMap.find(dwKey);
		if (it == m_HandlerMap.end())
			return NULL;

		return it->second;
	}

private:
	std::map<DWORD,fnHandler>			m_HandlerMap;
};

//客户端处理类
class FunctionMapC
{
public:
	void Add(DWORD dwKey,fnHandler_C pfnHandler)
	{
		m_HandlerMap.insert(std::make_pair(dwKey,pfnHandler));
	}

	fnHandler_C Find(DWORD dwKey)
	{
		std::map<DWORD,fnHandler_C>::iterator it = m_HandlerMap.find(dwKey);
		if(it==m_HandlerMap.end())
			return NULL;

		return it->second;
	}

private:
	//std::hash_map<DWORD,fnHandler_C>	m_HandlerMap;
	std::map<DWORD,fnHandler_C>	m_HandlerMap;
};


//消息处理类，处理由ServerSession，User发送过来的消息
class PacketHandler : public Singleton<PacketHandler>
{
public:
	PacketHandler(void);
	~PacketHandler(void);

public:
	BOOL Resister();
	BOOL AddHandlerMap( WORD dwMsgID, fnHandler fnHandler );
	BOOL AddHandlerUser( WORD dwMsgID, fnHandler_C fnHandler );
	void ParsePacketMap( ServerSession * pSession, unsigned char *pMsg, WORD wSize );
	void ParsePacketUser( User * pSession, unsigned char *pMsg, WORD wSize );

private:
	BOOL ResisterMap();
	BOOL ResisterUser();

private:
	FunctionMap		m_FunctionMapMap;		//地图服务器消息处理
	FunctionMapC	m_FunctionMapUser;		//玩家消息处理
};
