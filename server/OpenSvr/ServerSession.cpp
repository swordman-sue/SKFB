#include "stdafx.h"
#include "ServerSession.h"
#include "../Common/Const.h"
#include "../Common/MsgDefine.h"
#include "OpenServer.h"
#include "NetBase.h"
#include "header.pb.h"
#include "google/protobuf/message.h"
#include "server.pb.h"

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

 BOOL ServerSession::SendPacket( DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg)
 {
 	static unsigned char buffer[sizeof(dwGUID)+sizeof(wMsgID)+2*MAX_PACK_SIZE]={0};
 
 	unsigned char* pBuffer = buffer;
 	//填充包头信息
 	short nBuffHeadSize = 0;
	DWORD dwSeriesId = 0;
 
	if (NetMsgHeadHandler::EncodeServerHead(dwGUID, dwSeriesId,wMsgID, pBuffer, nBuffHeadSize))
 	{
 		return FALSE;
 	}
 
 	pBuffer +=nBuffHeadSize;
 
 	short nBuffBodySize = 0;
 	//填充包体信息
 	if(pMsg->EncodeMsg(pBuffer,nBuffBodySize))
 	{
 		return FALSE;
 	}
 
 	return NetworkObject::Send( (BYTE*)buffer, nBuffHeadSize+nBuffBodySize );
 }
 
 BOOL ServerSession::SendPacket( DWORD dwGUID, WORD wMsgID, unsigned char * pData, WORD wDataSize )
 {
 	static unsigned char buffer[sizeof(dwGUID)+sizeof(wMsgID)+2*MAX_PACK_SIZE]={0};
 
 	unsigned char* pBuffer = buffer;
 	//填充包头信息
 	short nBuffHeadSize = 0;
	DWORD dwSeriesId = 0;
 
	if (NetMsgHeadHandler::EncodeServerHead(dwGUID, dwSeriesId, wMsgID, pBuffer, nBuffHeadSize))
 	{
 		return FALSE;
 	}
 
 	pBuffer +=nBuffHeadSize;
 
 	//填充包体信息
 	memcpy(pBuffer,pData,wDataSize);
 
 	return NetworkObject::Send( (BYTE*)buffer, nBuffHeadSize+wDataSize );
 }

// BOOL ServerSession::SendPacket(DWORD dwGUID, google::protobuf::Message *pMsg)
// {
// 	ProtoBasic basic;
// 	int uCmdLen = pMsg->ByteSize();
// 	protocol::Header header;
// 	header.set_msg_full_name(pMsg->GetDescriptor()->full_name());
// 	basic.dwHeaderLen = header.ByteSize();
// 	basic.dwPackLen = sizeof(basic) + uCmdLen + basic.dwHeaderLen;
// 	basic.dwGUID = dwGUID;
// 
// 	string sResponse("");
// 	sResponse.assign((const char*)&basic, sizeof(basic));
// 	header.AppendToString(&sResponse);
// 	pMsg->AppendToString(&sResponse);
// 	SendPacket((unsigned char*)sResponse.data(), sResponse.size());
// 
// 	return TRUE;
// }
// 
// BOOL ServerSession::SendPacket(unsigned char * pData, WORD wDataSize )
// {
// 	static unsigned char buffer[2*MAX_PACK_SIZE]={0};
// 	unsigned char* pBuffer = buffer;
// 
// 	//填充包体信息
// 	memcpy(pBuffer,pData,wDataSize);
// 
// 	return NetworkObject::Send( (BYTE*)buffer, wDataSize );
// }

BOOL ServerSession::PreSendPacket(BYTE* pBuff,int nBuffLen)
{
	return NetworkObject::PreSend(pBuff, nBuffLen);
}


VOID ServerSession::Init()
{

}

VOID ServerSession::Release()
{

}