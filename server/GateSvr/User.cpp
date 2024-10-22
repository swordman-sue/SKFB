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

//��ʼ�����
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

//�ͷ����
VOID User::Release()
{

}

//������Ϣ�¼�����(��ʱ���յ�����һ���߼������İ�)
VOID User::OnRecv( BYTE *pMsg, WORD wSize )
{
	//����������Ϣ
	PacketHandler::Instance()->ParsePacketUser( this, (unsigned char*)pMsg, wSize );

	//��¼���տͻ������ݵ�ʱ��
	//time(&m_tStamp);

	m_dwTotalPacketCount++;
	m_dwCurPacketCount++;

	UINT64 ulTime = GetTickCount64();
	// ͳ�Ƶļ��ʱ��(Ŀǰ10��=10000) �൱��ÿ10����һ����
	if (ulTime >= m_ulProcessTime + m_dwCountIntervalTime)
	{
		if (m_dwCurPacketCount > m_dwMaxPacketCount)
		{
			m_dwMaxPacketCount = m_dwCurPacketCount;

			// ���ͳ����Ϣ
			GATELOG(GUI_LEVEL, "client packet info[GUID:%d,PacketCount:%d,TotalCount:%d]", GetGUID(),
				m_dwMaxPacketCount, m_dwTotalPacketCount);
		}

		// �������Ƶ�ʳ���10����һ�����Ǿ��ǷǷ�����,�������Ͽ�
		if (m_dwCurPacketCount > 1000)
		{
			GATELOG(GUI_LEVEL, "����Ƶ�ʹ���,�����Ͽ�����[GUID:%d,PacketCount:%d,TotalCount:%d]", GetGUID(),
				m_dwMaxPacketCount, m_dwTotalPacketCount);

			Disconnect();
		}

		m_ulProcessTime = ulTime;
		m_dwCurPacketCount = 0;
	}

	// �յ��ͻ�����Ϣ
	//GATELOG(GUI_LEVEL, "�յ��ͻ��˵���Ϣ[GUID:%d]",GetGUID());
}

//���������¼�����
VOID User::OnAccept( DWORD dwNetworkIndex )
{
	UserSession::OnAccept(dwNetworkIndex);
	UserManager::Instance()->AddUser(this);

	if (g_GateServer.IsEnableClientSecurity())
	{
		EnableSecurity();
	}

	//������Ѷ����
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
// 			// ���ͳ����Ϣ
// 			GATELOG(GUI_LEVEL, "����ͳ��[GUID:%d,PacketCount:%d,TotalCount:%d]",GetGUID(),
// 				m_dwMaxPacketCount,m_dwTotalPacketCount);
// 		}
// 		m_dwProcessTime = dwTime;
// 		m_dwCurPacketCount = 0;
// 	}
}

//�Ͽ������¼�����
VOID User::OnDisconnect()
{
	//���m_bDisConnectFromMapΪFALSE��ʾ�ǿͻ���������GateServer�Ͽ����ӣ���ʱ����֪ͨMapSvr�����������
	//���m_bDisConnectFromMapΪTRUE��ʾ��MapSvr���͵����������Ϣ����ʱ�Ͳ�Ӧ�÷�����Ϣ֪ͨMapSvr��
	if (!m_bDisconnectFromMap)
	{
		//����Ϣ֪ͨMapSvr�Ͽ�����
		MSG_G2M_CLIENT_DISCONNECT_CMD msg;
		SendToMapServer(&msg);
	}

	//ɾ�����
	UserManager::Instance()->RemoveUser(GetGUID());
}

//������Ϣ��MapServer
BOOL User::SendToMapServer(WORD dwMsgID, DWORD dwSeriesId, unsigned char * pData, WORD wDataSize)
{
	_ASSERT(g_GateServer.GetMapSession() != NULL && pData != NULL && wDataSize < MAX_PACK_SIZE);

	if (!g_GateServer.GetMapSession())
		return FALSE;

	return g_GateServer.GetMapSession()->SendPacket(GetGUID(), dwMsgID, pData, wDataSize, dwSeriesId);
}

//������Ϣ��MapServer
BOOL  User::SendToMapServer(NetMsgBody * pMsg)
{
	_ASSERT(g_GateServer.GetMapSession() != NULL && pMsg != NULL);

	if (!g_GateServer.GetMapSession())
		return FALSE;

	return g_GateServer.GetMapSession()->SendPacket(GetGUID(), pMsg->GetType(), pMsg);
}


//������Ϣ��Client
BOOL User::SendPacket(WORD dwMsgID, NetMsgBody * pMsg)
{
	m_bSendFlag = TRUE;
	//time(&m_tStamp);
	return UserSession::SendPacket(dwMsgID, pMsg);
}

//������Ϣ��Client
BOOL User::SendPacket(WORD dwMsgID, unsigned char * pData, WORD wDataSize)
{
	m_bSendFlag = TRUE;
	//time(&m_tStamp);
	time(&m_tStamp);
	return UserSession::SendPacket(dwMsgID, pData, wDataSize);
}