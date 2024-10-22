#pragma once

#include "../ToolKit/Singleton.h"
#include <hash_map>
#include <map>
#include <string>
class NetMsgBody;
class ServerSession;
class User;

using namespace std;

//��Ϣ������ԭ��
typedef VOID(*fnHandler)(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
typedef VOID(*fnHandler_C)(User * pSession, NetMsgBody *pMsg);

//����˴�����
class FunctionMap
{
public:
	void Add(DWORD dwKey,fnHandler pfnHandler)
	{
		IF_NOT (m_HandlerMap.insert(std::make_pair(dwKey,pfnHandler)).second)
		{
			GATELOG(ERROR_LEVEL, "�����Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d����ͨ������Ϊ��Ϣ ID �ظ�����", dwKey);
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

//�ͻ��˴�����
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


//��Ϣ�����࣬������ServerSession��User���͹�������Ϣ
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
	FunctionMap		m_FunctionMapMap;		//��ͼ��������Ϣ����
	FunctionMapC	m_FunctionMapUser;		//�����Ϣ����
};
