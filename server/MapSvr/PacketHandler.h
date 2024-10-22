#pragma once

#include "../ToolKit/Singleton.h"
#include <map>
#include <hash_map>
#include "header.pb.h"

class NetMsgBody;
class GateServerSession;
class OpenServerSession;
class protocol::Header;

//网关服务器消息处理函数原型 
typedef VOID (*fnHandler)( GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid );

//网关服务器消息处理类
class FunctionMap
{
public:
	void Add(DWORD dwMsgId,fnHandler pfnHandler)
	{
		IF_NOT(m_HandlerMap.insert(std::make_pair(dwMsgId, pfnHandler)).second)
		{
			MAPLOG(ERROR_LEVEL, "添加消息 Handler 失败，消息将无法得到处理（MSGID=%d），通常是因为消息 ID 重复引起", dwMsgId);
		}
	}

	fnHandler Find(DWORD dwMsgId)
	{
		std::map<DWORD, fnHandler>::iterator it = m_HandlerMap.find(dwMsgId);
		IF_NOT (it != m_HandlerMap.end())
		{
			MAPLOG(DEBUG_LEVEL, "查找消息 Handler 失败，消息将无法得到处理（MSGID=%d）", dwMsgId);
			return NULL;
		}

		return it->second;
	}

private:
	//std::hash_map<DWORD,fnHandler>	m_HandlerMap;
	std::map<DWORD,fnHandler>	m_HandlerMap;
	//std::map<string,fnHandler>	m_HandlerMap;
};

//开放服务器消息处理函数原型
typedef VOID(*fnHandler_O)(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

//开放服务器消息类
class FunctionMapO
{
public:
	void Add(DWORD dwMsgId,fnHandler_O pfnHandler)
	{
		IF_NOT(m_HandlerMap.insert(std::make_pair(dwMsgId, pfnHandler)).second)
		{
			MAPLOG(ERROR_LEVEL, "添加消息 Handler 失败，消息将无法得到处理（MSGID=%d），通常是因为消息 ID 重复引起", dwMsgId);
		}
	}

	fnHandler_O Find(DWORD dwMsgId)
	{
		std::map<DWORD, fnHandler_O>::iterator it = m_HandlerMap.find(dwMsgId);
		IF_NOT(it != m_HandlerMap.end())
		{
			MAPLOG(DEBUG_LEVEL, "查找消息 Handler 失败，消息将无法得到处理（MSGID=%d）", dwMsgId);
			return NULL;
		}

		return it->second;
	}

private:
	//std::hash_map<DWORD,fnHandler_O>	m_HandlerMap;
	std::map<DWORD, fnHandler_O>	m_HandlerMap;
};

//消息处理类(处理由Gate/Open发送过来的消息)
class PacketHandler : public Singleton<PacketHandler>
{
public:
	PacketHandler(void);
	~PacketHandler(void);

public:
	BOOL Resister();

	BOOL AddHandlerGate( DWORD dwMsgId, fnHandler fnHandler);
	void ParsePacketGate( GateServerSession * pSession, unsigned char *pMsg, WORD wSize );

	BOOL AddHandlerOpen(DWORD dwMsgId, fnHandler_O fnHandler);
	void ParsePacketOpen( OpenServerSession * pSession, unsigned char *pMsg, WORD wSize );

private:
	BOOL ResisterGate();
	BOOL ResisterOpen();

private:
	FunctionMap		m_FunctionMapGate;		//网关服务器消息处理
	FunctionMapO	m_FunctionMapOpen;		//Open服务器消息处理
};
