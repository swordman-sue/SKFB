#pragma once

#include "../ToolKit/Singleton.h"
#include <hash_map>
#include <string>
class NetMsgBody;
class ServerSession;

//��Ϣ������ԭ��
typedef VOID (*fnHandler)( ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid );

//typedef VOID (*fnHandler)( ServerSession * pSession, unsigned char *pMsg, WORD wSize, DWORD dwGuid);


//����˴�����
class FunctionMap
{
public:
	void Add(DWORD dwMsgId,fnHandler pfnHandler)
	{
		IF_NOT(m_HandlerMap.insert(std::make_pair(dwMsgId, pfnHandler)).second)
		{
			OPENLOG(ERROR_LEVEL, "�����Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d����ͨ������Ϊ��Ϣ ID �ظ�����", dwMsgId);
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

//��Ϣ�����࣬������ServerSession���͹�������Ϣ
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
	FunctionMap		m_FunctionMapMap;		//��ͼ��������Ϣ����
};
