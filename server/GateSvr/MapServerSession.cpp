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
	//�Ͽ���MapSvr������
	UserManager::Instance()->OnMapSvrDisconnect();

	//���û���汾
	ServerSession::OnDisconnect();

	GATELOG(GUI_LEVEL, "The MapSvr disconnected.");
}

VOID MapServerSession::OnAccept( DWORD dwNetworkIndex )
{
	ServerSession::OnAccept(dwNetworkIndex);

	g_GateServer.SetMapSession(this);

	GATELOG(GUI_LEVEL, "Accept MapSvr Connection.");

	// �յ������������Ӻ��ٿ����ͻ��˼���
	g_GateServer.StartClientListen();
}