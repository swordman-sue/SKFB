#include "StdAfx.h"
#include "ServerSession.h"
#include "GateServer.h"
#include "../Common/Const.h"
#include "../Common/MsgDefine.h"
#include "NetBase.h"

ServerSession::ServerSession(void)
:	m_dwSessionIndex ( 0 ),
m_bIsConnected ( FALSE )
{

}

ServerSession::~ServerSession(void)
{

}

VOID ServerSession::Update()
{
	//��ʱ����ping������Ӧ�ķ�����
}

VOID ServerSession::OnAccept( DWORD dwNetworkIndex )
{
	_setConnected( TRUE );
	SetSessionIndex( dwNetworkIndex );
}

VOID ServerSession::OnDisconnect()
{
	_setConnected( FALSE );
}

VOID ServerSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	if( bSuccess )
	{
		_setConnected( TRUE );
		SetSessionIndex( dwNetworkIndex );
	}
}

BOOL ServerSession::SendPacket(DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg, DWORD dwSeriesId)
{
	static unsigned char buffer[sizeof(dwGUID)+sizeof(dwSeriesId)+sizeof(wMsgID)+2 * MAX_PACK_SIZE] = { 0 };

	unsigned char* pBuffer = buffer;
	//����ͷ��Ϣ
	short nBuffHeadSize = 0;

	if (NetMsgHeadHandler::EncodeServerHead(dwGUID, dwSeriesId, wMsgID, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	pBuffer += nBuffHeadSize;

	short nBuffBodySize = 0;
	//��������Ϣ
	if (pMsg->EncodeMsg(pBuffer, nBuffBodySize))
	{
		return FALSE;
	}

	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + nBuffBodySize);
}

BOOL ServerSession::SendPacket(DWORD dwGUID, WORD wMsgID, unsigned char * pData, WORD wDataSize, DWORD dwSeriesId)
{
	static unsigned char buffer[sizeof(dwGUID)+sizeof(dwSeriesId)+sizeof(wMsgID)+2 * MAX_PACK_SIZE] = { 0 };

	unsigned char* pBuffer = buffer;
	//����ͷ��Ϣ
	short nBuffHeadSize = 0;

	if (NetMsgHeadHandler::EncodeServerHead(dwGUID, dwSeriesId, wMsgID, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	pBuffer += nBuffHeadSize;

	//��������Ϣ
	memcpy(pBuffer, pData, wDataSize);

	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + wDataSize);
}

VOID ServerSession::Init()
{

}

VOID ServerSession::Release()
{

}