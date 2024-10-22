#pragma once

#include "../ToolKit/Singleton.h"
#include <map>
#include <hash_map>
#include "header.pb.h"

class NetMsgBody;
class GateServerSession;
class OpenServerSession;
class protocol::Header;

//���ط�������Ϣ������ԭ�� 
typedef VOID (*fnHandler)( GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid );

//���ط�������Ϣ������
class FunctionMap
{
public:
	void Add(DWORD dwMsgId,fnHandler pfnHandler)
	{
		IF_NOT(m_HandlerMap.insert(std::make_pair(dwMsgId, pfnHandler)).second)
		{
			MAPLOG(ERROR_LEVEL, "�����Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d����ͨ������Ϊ��Ϣ ID �ظ�����", dwMsgId);
		}
	}

	fnHandler Find(DWORD dwMsgId)
	{
		std::map<DWORD, fnHandler>::iterator it = m_HandlerMap.find(dwMsgId);
		IF_NOT (it != m_HandlerMap.end())
		{
			MAPLOG(DEBUG_LEVEL, "������Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d��", dwMsgId);
			return NULL;
		}

		return it->second;
	}

private:
	//std::hash_map<DWORD,fnHandler>	m_HandlerMap;
	std::map<DWORD,fnHandler>	m_HandlerMap;
	//std::map<string,fnHandler>	m_HandlerMap;
};

//���ŷ�������Ϣ������ԭ��
typedef VOID(*fnHandler_O)(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

//���ŷ�������Ϣ��
class FunctionMapO
{
public:
	void Add(DWORD dwMsgId,fnHandler_O pfnHandler)
	{
		IF_NOT(m_HandlerMap.insert(std::make_pair(dwMsgId, pfnHandler)).second)
		{
			MAPLOG(ERROR_LEVEL, "�����Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d����ͨ������Ϊ��Ϣ ID �ظ�����", dwMsgId);
		}
	}

	fnHandler_O Find(DWORD dwMsgId)
	{
		std::map<DWORD, fnHandler_O>::iterator it = m_HandlerMap.find(dwMsgId);
		IF_NOT(it != m_HandlerMap.end())
		{
			MAPLOG(DEBUG_LEVEL, "������Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d��", dwMsgId);
			return NULL;
		}

		return it->second;
	}

private:
	//std::hash_map<DWORD,fnHandler_O>	m_HandlerMap;
	std::map<DWORD, fnHandler_O>	m_HandlerMap;
};

//��Ϣ������(������Gate/Open���͹�������Ϣ)
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
	FunctionMap		m_FunctionMapGate;		//���ط�������Ϣ����
	FunctionMapO	m_FunctionMapOpen;		//Open��������Ϣ����
};
