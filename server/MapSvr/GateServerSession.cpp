#include "StdAfx.h"
#include "GateServerSession.h"
#include "PacketHandler.h"

GateServerSession::GateServerSession(void)
{

}

GateServerSession::~GateServerSession(void)
{

}

VOID GateServerSession::OnRecv( BYTE *pMsg, WORD wSize )
{
	PacketHandler::Instance()->ParsePacketGate(this,(unsigned char*)pMsg,wSize);
}

VOID GateServerSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	//�����������ط�����
	ServerSession::OnConnect(bSuccess,dwNetworkIndex);
	//���ӳɹ�
	if ( bSuccess )
	{
		MAPLOG(GUI_LEVEL, "Connect to the GateSvr Successfully.");
	}
}

VOID GateServerSession::OnDisconnect()
{
	//AccountInfoManager::Instance()->OnGateSvrDisconnect();

	//�����û���汾
	ServerSession::OnDisconnect();
	MAPLOG(GUI_LEVEL, "The GateSvr disconnected.");
}