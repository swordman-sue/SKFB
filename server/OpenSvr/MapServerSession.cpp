#include "stdafx.h"
#include "MapServerSession.h"
#include "PacketHandler.h"
#include "ServerSessionManager.h"
#include "../Common/MsgDefine.h"

MapServerSession::MapServerSession(void)
{

}

MapServerSession::~MapServerSession(void)
{

}

VOID MapServerSession::OnRecv( BYTE *pMsg, WORD wSize )
{
	PacketHandler::Instance()->ParsePacketMap(this,(unsigned char*)pMsg,wSize);
}

VOID MapServerSession::OnDisconnect()
{
	//调用基类版本
	ServerSession::OnDisconnect();

	if (IsValid())
	{
		ServerSessionManager::Instance()->RemoveMapServer(GetServerID());
	}

	OPENLOG(GUI_LEVEL, "The MapSvr disconnected.");
}

VOID MapServerSession::OnAccept( DWORD dwNetworkIndex )
{
	ServerSession::OnAccept(dwNetworkIndex);

	OPENLOG(GUI_LEVEL, "Accept MapSvr Connection.");
}

VOID MapServerSession::Init()
{
	m_bValid = false;
	m_dwServerID = 0;
}

VOID MapServerSession::Release()
{
	m_bValid = false;
	m_dwServerID = 0;
}
