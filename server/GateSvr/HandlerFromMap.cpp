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
	// 世界聊天
	if (S2C_CHAT_NOTIFY == dwMsgID)
	{
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize);
	}
	// 系统公告
	else if (S2C_SYSTEM_NOTICE == dwMsgID)
	{
		MSG_S2C_SYSTEM_NOTICE* pRecv = (MSG_S2C_SYSTEM_NOTICE*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, pRecv->m_wServerId);
	}
	// 新的一天
	else if (S2C_NEW_DAY_NOTIFY == dwMsgID)
	{
		MSG_S2C_NEW_DAY_NOTIFY* pRecv = (MSG_S2C_NEW_DAY_NOTIFY*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, 0);
	}
	// 世界BOSS活动开始
	else if (S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY == dwMsgID)
	{	
		MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY* pRecv = (MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, 0);
	}
	// 世界BOSS活动结束
	else if (S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY == dwMsgID)
	{
		MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY* pRecv = (MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY*)pMsg;
		UserManager::Instance()->SendPacket(dwMsgID, pData, wDataSize, 0);
	}
	else
	{
		GATELOG(DEBUG_LEVEL, "木有处理的Map消息(msg_id=%d)", dwMsgID);
	}
}

VOID HandlerFromMap::OnMSG_M2G_CLIENT_KICKOUT_CMD(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	//收到MapSvr发送的断开连接消息

	User* pUser = UserManager::Instance()->FindUserByGUID(dwGuid);
	if (pUser == NULL)
	{
		// 可能由于玩家已经掉线了
		return;
	}

	//设置MapSvr发送断开连接标志
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
		// 可能由于玩家已经掉线了
		GATELOG(DEBUG_LEVEL, "玩家已经木有在线(guid=%d)", dwGuid);
		return;
	}

	MSG_M2G_CLIENT_INFO* pRecv = (MSG_M2G_CLIENT_INFO*)pMsg;

	pUser->SetServerId(pRecv->m_wServerId);
}

