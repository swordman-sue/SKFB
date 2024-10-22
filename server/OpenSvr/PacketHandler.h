#pragma once

#include "../ToolKit/Singleton.h"
#include <hash_map>
#include <string>
class NetMsgBody;
class ServerSession;

//消息处理函数原型
typedef VOID (*fnHandler)( ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid );

//typedef VOID (*fnHandler)( ServerSession * pSession, unsigned char *pMsg, WORD wSize, DWORD dwGuid);


//服务端处理类
class FunctionMap
{
public:
	void Add(DWORD dwMsgId,fnHandler pfnHandler)
	{
		IF_NOT(m_HandlerMap.insert(std::make_pair(dwMsgId, pfnHandler)).second)
		{
			OPENLOG(ERROR_LEVEL, "添加消息 Handler 失败，消息将无法得到处理（MSGID=%d），通常是因为消息 ID 重复引起", dwMsgId);
		}
	}

	fnHandler Find(DWORD dwMsgId)
	{
		std::map<DWORD, fnHandler>::iterator it = m_HandlerMap.find(dwMsgId);
		if (it == m_HandlerMap.end())
			return NULL;

		return it->second;
	}

private:
	//std::hash_map<DWORD,fnHandler>	m_HandlerMap;
	std::map<DWORD,fnHandler>	m_HandlerMap;
};

//消息处理类，处理由ServerSession发送过来的消息
class PacketHandler : public Singleton<PacketHandler>
{
public:
	PacketHandler(void);
	~PacketHandler(void);

public:
	BOOL Resister();
	BOOL AddHandlerMap(WORD dwMsgID, fnHandler fnHandler);
	void ParsePacketMap(ServerSession * pSession, unsigned char *pMsg, WORD wSize);

private:
	BOOL ResisterMap();

private:
	FunctionMap		m_FunctionMapMap;		//地图服务器消息处理
};
