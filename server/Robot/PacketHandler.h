#pragma once

#include <map>
#include "../ToolKit/Singleton.h"

class Robot;
class NetMsgBody;
class RobotSession;

//消息处理函数原型
typedef VOID (*fnHandler)( RobotSession * pSession, NetMsgBody *pMsg);

//处理函数列表
class FunctionMap
{
public:
	void Add(DWORD dwKey,fnHandler pfnHandler)
	{
		if (!m_HandlerMap.insert(std::make_pair(dwKey,pfnHandler)).second)
		{
			printf("添加消息 Handler 失败，消息将无法得到处理（MSGID=%d），通常是因为消息 ID 重复引起",dwKey);
		}
	}

	fnHandler Find(DWORD dwKey)
	{
		std::map<DWORD,fnHandler>::iterator it = m_HandlerMap.find(dwKey);
		if (it == m_HandlerMap.end())
		{
			//printf("查找消息 Handler 失败，消息将无法得到处理（MSGID=%d）\n",dwKey);
			return NULL;
		}

		return it->second;
	}

	void Clear() { m_HandlerMap.clear(); }

private:
	std::map<DWORD, fnHandler>	m_HandlerMap;

};

//消息处理类，处理由服务器发送过来的消息
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
	

	

	//在此定义需要处理的服务器消息
	static VOID OnMSG_M2C_ROBOT_LOGIN_ACK(RobotSession * pSession, NetMsgBody *pMsg);

private:
	FunctionMap		m_FunctionMap;		//服务器消息处理
};