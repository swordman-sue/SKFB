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
	//主动连接网关服务器
	ServerSession::OnConnect(bSuccess,dwNetworkIndex);
	//连接成功
	if ( bSuccess )
	{
		MAPLOG(GUI_LEVEL, "Connect to the GateSvr Successfully.");
	}
}

VOID GateServerSession::OnDisconnect()
{
	//AccountInfoManager::Instance()->OnGateSvrDisconnect();

	//最后调用基类版本
	ServerSession::OnDisconnect();
	MAPLOG(GUI_LEVEL, "The GateSvr disconnected.");
}