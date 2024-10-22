#include "StdAfx.h"
#include "User.h"
#include "UserManager.h"
#include "GateServer.h"
#include "MapServerSession.h"
#include "../Common/MsgDefine.h"
#include "PacketHandler.h"
//#include "server.pb.h"
#include "google/protobuf/message.h"
#include "NetBase.h"
#include "header.pb.h"
//#include "login.pb.h"
//#include "dgame_define.pb.h"


User::User(void)
:	m_bDisconnectFromMap(FALSE)
{

}

User::~User(void)
{

}

//初始化玩家
VOID User::Init()
{
	m_bDisconnectFromMap = FALSE;
	m_bSendFlag = FALSE;
	m_tCreateTime = 0;
	m_tStamp = 0;

	//m_dwProcessTime = (DWORD)time(NULL);
	m_ulProcessTime = GetTickCount64();
	m_dwCountIntervalTime = 10000;
	m_dwCurPacketCount = 0;
	m_dwMaxPacketCount = 0;
	m_dwTotalPacketCount = 0;

	m_dwIndex = 0;
	m_dwServerId = 0;
}

//释放玩家
VOID User::Release()
{

}

//接收消息事件处理(此时接收到的是一个逻辑完整的包)
VOID User::OnRecv( BYTE *pMsg, WORD wSize )
{
	//解析处理消息
	PacketHandler::Instance()->ParsePacketUser( this, (unsigned char*)pMsg, wSize );

	//记录接收客户端数据的时间
	//time(&m_tStamp);

	m_dwTotalPacketCount++;
	m_dwCurPacketCount++;

	UINT64 ulTime = GetTickCount64();
	// 统计的间隔时间(目前10秒=10000) 相当于每10毫秒一个包
	if (ulTime >= m_ulProcessTime + m_dwCountIntervalTime)
	{
		if (m_dwCurPacketCount > m_dwMaxPacketCount)
		{
			m_dwMaxPacketCount = m_dwCurPacketCount;

			// 监控统计信息
			GATELOG(GUI_LEVEL, "client packet info[GUID:%d,PacketCount:%d,TotalCount:%d]", GetGUID(),
				m_dwMaxPacketCount, m_dwTotalPacketCount);
		}

		// 如果发包频率超过10毫秒一个，那就是非法攻击,则主动断开
		if (m_dwCurPacketCount > 1000)
		{
			GATELOG(GUI_LEVEL, "发包频率过快,主动断开连接[GUID:%d,PacketCount:%d,TotalCount:%d]", GetGUID(),
				m_dwMaxPacketCount, m_dwTotalPacketCount);

			Disconnect();
		}

		m_ulProcessTime = ulTime;
		m_dwCurPacketCount = 0;
	}

	// 收到客户端消息
	//GATELOG(GUI_LEVEL, "收到客户端的消息[GUID:%d]",GetGUID());
}

//接入连接事件处理
VOID User::OnAccept( DWORD dwNetworkIndex )
{
	UserSession::OnAccept(dwNetworkIndex);
	UserManager::Instance()->AddUser(this);

	if (g_GateServer.IsEnableClientSecurity())
	{
		EnableSecurity();
	}

	//开启腾讯接入
	//SetNeedHttp();

	time(& m_tCreateTime);
	m_tStamp = m_tCreateTime;
}

VOID User::Process(DWORD dwTime)
{
// 	if (dwTime >= m_dwProcessTime + m_dwCountIntervalTime)
// 	{
// 		if (m_dwCurPacketCount > m_dwMaxPacketCount)
// 		{
// 			m_dwMaxPacketCount =  m_dwCurPacketCount;
// 
// 			// 监控统计信息
// 			GATELOG(GUI_LEVEL, "数据统计[GUID:%d,PacketCount:%d,TotalCount:%d]",GetGUID(),
// 				m_dwMaxPacketCount,m_dwTotalPacketCount);
// 		}
// 		m_dwProcessTime = dwTime;
// 		m_dwCurPacketCount = 0;
// 	}
}

//断开连接事件处理
VOID User::OnDisconnect()
{
	//如果m_bDisConnectFromMap为FALSE表示是客户端主动和GateServer断开连接，这时必须通知MapSvr处理玩家下线
	//如果m_bDisConnectFromMap为TRUE表示是MapSvr发送的玩家下线消息，这时就不应该发送消息通知MapSvr了
	if (!m_bDisconnectFromMap)
	{
		//发消息通知MapSvr断开连接
		MSG_G2M_CLIENT_DISCONNECT_CMD msg;
		SendToMapServer(&msg);
	}

	//删除玩家
	UserManager::Instance()->RemoveUser(GetGUID());
}

//发送消息至MapServer
BOOL User::SendToMapServer(WORD dwMsgID, DWORD dwSeriesId, unsigned char * pData, WORD wDataSize)
{
	_ASSERT(g_GateServer.GetMapSession() != NULL && pData != NULL && wDataSize < MAX_PACK_SIZE);

	if (!g_GateServer.GetMapSession())
		return FALSE;

	return g_GateServer.GetMapSession()->SendPacket(GetGUID(), dwMsgID, pData, wDataSize, dwSeriesId);
}

//发送消息至MapServer
BOOL  User::SendToMapServer(NetMsgBody * pMsg)
{
	_ASSERT(g_GateServer.GetMapSession() != NULL && pMsg != NULL);

	if (!g_GateServer.GetMapSession())
		return FALSE;

	return g_GateServer.GetMapSession()->SendPacket(GetGUID(), pMsg->GetType(), pMsg);
}


//发送消息至Client
BOOL User::SendPacket(WORD dwMsgID, NetMsgBody * pMsg)
{
	m_bSendFlag = TRUE;
	//time(&m_tStamp);
	return UserSession::SendPacket(dwMsgID, pMsg);
}

//发送消息至Client
BOOL User::SendPacket(WORD dwMsgID, unsigned char * pData, WORD wDataSize)
{
	m_bSendFlag = TRUE;
	//time(&m_tStamp);
	time(&m_tStamp);
	return UserSession::SendPacket(dwMsgID, pData, wDataSize);
}