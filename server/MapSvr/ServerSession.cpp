#include "StdAfx.h"
#include "ServerSession.h"
#include "../Common/Const.h"
#include "../Common/MsgDefine.h"
#include "MapServer.h"
#include "google/protobuf/message.h"
#include "google/protobuf/message_lite.h"
#include "NetBase.h"
#include "header.pb.h"

ServerSession::ServerSession(void)
:	m_dwSessionIndex ( 0 ),
m_bIsConnector ( FALSE ),
m_bIsTriedToConnect ( FALSE ),
m_bIsConnected ( FALSE )
{

}

ServerSession::~ServerSession(void)
{

}

VOID ServerSession::Init()
{

}

VOID ServerSession::Release()
{

}

VOID ServerSession::Update()
{
	//定时发送ping包给对应的服务器
}

VOID ServerSession::OnAccept( DWORD dwNetworkIndex )
{
	_setConnected( TRUE );
	_setIsConnector( FALSE );
	SetSessionIndex( dwNetworkIndex );
}

VOID ServerSession::OnDisconnect()
{
	_setConnected( FALSE );
	if( _isConnector() )
	{
		m_bIsTriedToConnect = FALSE;
		TryToConnect();
	}
}

VOID ServerSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	if( bSuccess )
	{
		_setConnected( TRUE );
		_setIsConnector( TRUE );
		SetSessionIndex( dwNetworkIndex );
	}
	else
	{
		m_bIsTriedToConnect = FALSE;
		TryToConnect();
	}
}
BOOL ServerSession::SendPacket(DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg)
{
	static unsigned char buffer[sizeof(dwGUID)+sizeof(wMsgID)+2 * MAX_PACK_SIZE] = { 0 };

	unsigned char* pBuffer = buffer;
	//填充包头信息
	short nBuffHeadSize = 0;
	DWORD dwSeriesId = 0;

	if (NetMsgHeadHandler::EncodeServerHead(dwGUID, dwSeriesId, wMsgID, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	pBuffer += nBuffHeadSize;

	short nBuffBodySize = 0;
	//填充包体信息
	if (pMsg->EncodeMsg(pBuffer, nBuffBodySize))
	{
		return FALSE;
	}

	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + nBuffBodySize);
}

BOOL ServerSession::SendPacket(DWORD dwGUID, WORD wMsgID, unsigned char * pData, WORD wDataSize)
{
	static unsigned char buffer[sizeof(dwGUID)+sizeof(wMsgID)+2 * MAX_PACK_SIZE] = { 0 };

	unsigned char* pBuffer = buffer;
	//填充包头信息
	short nBuffHeadSize = 0;
	DWORD dwSeriesId = 0;

	if (NetMsgHeadHandler::EncodeServerHead(dwGUID, dwSeriesId, wMsgID, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	pBuffer += nBuffHeadSize;

	//填充包体信息
	memcpy(pBuffer, pData, wDataSize);

	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + wDataSize);
}

VOID ServerSession::TryToConnect()
{
	if( m_bIsTriedToConnect )
		return;

	m_bIsTriedToConnect = TRUE;

	g_MapServer.ConnectToServer(this,m_szServerIP,m_wPort);
}

VOID ServerSession::Clone( ServerSession * pSession )
{
	_setConnected( pSession->IsConnected() );
	_setIsConnector( pSession->_isConnector() );
	SetSessionIndex( pSession->GetSessionIndex() );
}
