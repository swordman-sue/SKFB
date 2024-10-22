#include "StdAfx.h"
#include "User.h"
#include "UserManager.h"
#include "HandlerFromMap.h"
#include "ServerSession.h"
#include "MapServerSession.h"
#include "../Common/MsgDefine.h"
//#include "server.pb.h"
#include "header.pb.h"
#include <string>
#include "NetBase.h"

HandlerFromMap::HandlerFromMap(void)
{
}

HandlerFromMap::~HandlerFromMap(void)
{
}

VOID HandlerFromMap::OnNetMessage(WORD dwMsgID, unsigned char * pData, WORD wDataSize, NetMsgBody *pMsg)
{
	// ��������
	if (S2C_CHAT_NOTIFY == dwMsgID)
	{
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize);
	}
	// ϵͳ����
	else if (S2C_SYSTEM_NOTICE == dwMsgID)
	{
		MSG_S2C_SYSTEM_NOTICE* pRecv = (MSG_S2C_SYSTEM_NOTICE*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, pRecv->m_wServerId);
	}
	// �µ�һ��
	else if (S2C_NEW_DAY_NOTIFY == dwMsgID)
	{
		MSG_S2C_NEW_DAY_NOTIFY* pRecv = (MSG_S2C_NEW_DAY_NOTIFY*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, 0);
	}
	// ����BOSS���ʼ
	else if (S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY == dwMsgID)
	{	
		MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY* pRecv = (MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, 0);
	}
	// ����BOSS�����
	else if (S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY == dwMsgID)
	{
		MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY* pRecv = (MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, 0);
	}
	else
	{
		GATELOG(DEBUG_LEVEL, "ľ�д����Map��Ϣ(msg_id=%d)", dwMsgID);
	}
}

VOID HandlerFromMap::OnMSG_M2G_CLIENT_KICKOUT_CMD(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	//�յ�MapSvr���͵ĶϿ�������Ϣ

	User* pUser = UserManager::Instance()->FindUserByGUID(dwGuid);
	if (pUser == NULL)
	{
		// ������������Ѿ�������
		return;
	}

	//����MapSvr���ͶϿ����ӱ�־
	pUser->SetDisConnectFromMap(TRUE);

	pUser->Disconnect();
}

VOID HandlerFromMap::OnMSG_M2G_HEART_BEAT(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_G2M_HEART_BEAT Msg;
	g_GateServer.GetMapSession()->SendPacket(0, Msg.GetType(), &Msg);
}

VOID HandlerFromMap::OnMSG_M2G_CLIENT_INFO(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	User* pUser = UserManager::Instance()->FindUserByGUID(dwGuid);
	if (pUser == NULL)
	{
		// ������������Ѿ�������
		GATELOG(DEBUG_LEVEL, "����Ѿ�ľ������(guid=%d)", dwGuid);
		return;
	}

	MSG_M2G_CLIENT_INFO* pRecv = (MSG_M2G_CLIENT_INFO*)pMsg;

	pUser->SetServerId(pRecv->m_wServerId);
}

