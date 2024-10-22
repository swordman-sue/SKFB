#pragma once

#include <map>
#include "../ToolKit/Singleton.h"

class Robot;
class NetMsgBody;
class RobotSession;

//��Ϣ������ԭ��
typedef VOID (*fnHandler)( RobotSession * pSession, NetMsgBody *pMsg);

//�������б�
class FunctionMap
{
public:
	void Add(DWORD dwKey,fnHandler pfnHandler)
	{
		if (!m_HandlerMap.insert(std::make_pair(dwKey,pfnHandler)).second)
		{
			printf("�����Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d����ͨ������Ϊ��Ϣ ID �ظ�����",dwKey);
		}
	}

	fnHandler Find(DWORD dwKey)
	{
		std::map<DWORD,fnHandler>::iterator it = m_HandlerMap.find(dwKey);
		if (it == m_HandlerMap.end())
		{
			//printf("������Ϣ Handler ʧ�ܣ���Ϣ���޷��õ�����MSGID=%d��\n",dwKey);
			return NULL;
		}

		return it->second;
	}

	void Clear() { m_HandlerMap.clear(); }

private:
	std::map<DWORD, fnHandler>	m_HandlerMap;

};

//��Ϣ�����࣬�����ɷ��������͹�������Ϣ
class PacketHandler : public Singleton<PacketHandler>
{
public:
	PacketHandler(void);
	~PacketHandler(void);

public:
	BOOL Resister();

	BOOL AddHandler( DWORD dwMsgID, fnHandler fnHandler );

	void ParsePacket( RobotSession * pSession, unsigned char *pMsg, WORD wSize );

private:
	static VOID OnMSG_S2C_LOGIN_ACCOUNT_RESP(RobotSession * pSession, NetMsgBody *pMsg);
	static VOID OnMSG_S2C_LOGIN_SELECT_SERVER_RESP(RobotSession * pSession, NetMsgBody *pMsg);
	static VOID OnMSG_S2C_LOGIN_CREATE_ROLE_RESP(RobotSession * pSession, NetMsgBody *pMsg);
	static VOID OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY(RobotSession * pSession, NetMsgBody *pMsg);
	static VOID OnMSG_S2C_GAME_ERROR_NOTIFY(RobotSession * pSession, NetMsgBody *pMsg);
	static VOID OnMSG_S2C_ROLE_INFO_NOTIFY(RobotSession * pSession, NetMsgBody *pMsg);
	static VOID OnMSG_S2C_ATTACK_CITY_ENTER_RESP(RobotSession * pSession, NetMsgBody *pMsg);
	

	static VOID OnS2C_BATTLE_START_RESP(RobotSession * pSession, NetMsgBody *pMsg);
	

	

	//�ڴ˶�����Ҫ����ķ�������Ϣ
	static VOID OnMSG_M2C_ROBOT_LOGIN_ACK(RobotSession * pSession, NetMsgBody *pMsg);

private:
	FunctionMap		m_FunctionMap;		//��������Ϣ����
};