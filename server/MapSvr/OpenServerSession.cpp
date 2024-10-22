
#include "StdAfx.h"
#include "OpenServerSession.h"
#include "PacketHandler.h"
#include "../common/MsgDefine.h"
#include "login.pb.h"
#include "server.pb.h"


OpenServerSession::OpenServerSession(void)
{
}

OpenServerSession::~OpenServerSession(void)
{
}

VOID OpenServerSession::OnRecv( BYTE *pMsg, WORD wSize )
{
	PacketHandler::Instance()->ParsePacketOpen(this,(unsigned char*)pMsg,wSize);
}

VOID OpenServerSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	//�������ӿ��ŷ�����
	ServerSession::OnConnect(bSuccess, dwNetworkIndex);
	//���ӳɹ�
	if (bSuccess)
	{
		MAPLOG(GUI_LEVEL, "Connect to the OpenSvr Successfully.");

		////����ServerID��OpenServer
		MSG_M2O_SERVERID_CMD Msg;
		Msg.m_dwServerID = g_MapServer.GetServerID();
		SendPacket(0, Msg.GetType(), &Msg);
	}
}

VOID OpenServerSession::OnDisconnect()
{
	//�����û���汾
	ServerSession::OnDisconnect();
	MAPLOG(GUI_LEVEL, "The OpenSvr disconnected.");
}
