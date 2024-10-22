#include "StdAfx.h"
#include "MapServerSession.h"
#include "PacketHandler.h"
#include "UserManager.h"
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
	//断开和MapSvr的连接
	UserManager::Instance()->OnMapSvrDisconnect();

	//调用基类版本
	ServerSession::OnDisconnect();

	GATELOG(GUI_LEVEL, "The MapSvr disconnected.");
}

VOID MapServerSession::OnAccept( DWORD dwNetworkIndex )
{
	ServerSession::OnAccept(dwNetworkIndex);

	g_GateServer.SetMapSession(this);

	GATELOG(GUI_LEVEL, "Accept MapSvr Connection.");

	// 收到场景服的链接后再开启客户端监听
	g_GateServer.StartClientListen();
}