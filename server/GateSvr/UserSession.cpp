#include "StdAfx.h"
#include "UserSession.h"
#include "../Common/PacketStruct_Code.h"

UserSession::UserSession(void)
{

}

UserSession::~UserSession(void)
{

}

//发送消息
BOOL UserSession::SendPacket(WORD wMsgID, NetMsgBody * pMsg)
{
	_ASSERT(pMsg != NULL);

	DWORD dwSeriesId = 0;
	static unsigned char buffer[sizeof(wMsgID)+sizeof(dwSeriesId)+2 * MAX_PACK_SIZE] = { 0 };
	unsigned char* pBuffer = buffer;
	

	//填充包头信息
	short nBuffHeadSize = 0;
	if (NetMsgHeadHandler::EncodeClientHead(wMsgID, dwSeriesId, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	//偏移消息包指针
	pBuffer += nBuffHeadSize;
	short nBuffBodySize = 0;

	//填充包体信息
	if (pMsg->EncodeMsg(pBuffer, nBuffBodySize))
	{
		return FALSE;
	}

	//发送消息
	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + nBuffBodySize);
}

//发送消息
BOOL UserSession::SendPacket(WORD wMsgID, unsigned char * pData, WORD wDataSize)
{
	DWORD dwSeriesId = 0;
	static unsigned char buffer[sizeof(wMsgID)+sizeof(dwSeriesId)+2 * MAX_PACK_SIZE] = { 0 };
	unsigned char* pBuffer = buffer;

	//填充包头信息
	short nBuffHeadSize = 0;
	if (NetMsgHeadHandler::EncodeClientHead(wMsgID, dwSeriesId, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	//偏移消息包指针
	pBuffer += nBuffHeadSize;

	//填充包体信息
	memcpy(pBuffer, pData, wDataSize);

	//===test
	//OutputDebug(buffer, nBuffHeadSize+wDataSize, dwMsgID);

	//发送消息
	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + wDataSize);
}

VOID UserSession::Init()
{

}

VOID UserSession::Release()
{

}

//断开连接事件处理
VOID UserSession::OnDisconnect()
{

}

//接入连接事件处理
VOID UserSession::OnAccept( DWORD dwNetworkIndex )
{
	SetSessionIndex(dwNetworkIndex);
}

//16进制格式打印消息包(测试用)
VOID UserSession::OutputDebug(BYTE* pbyMsg, UINT nSize, USHORT shMsgID)
{
	if (!pbyMsg || !nSize)
		return;

	char szBuf[1024*12] = {0};
	sprintf(szBuf, "消息ID:%d\n", shMsgID);
	OutputDebugString(szBuf);

	memset(szBuf, 0, sizeof(szBuf));
	for (UINT i = 0; i < nSize; ++i)
	{
		char szTemp[64] = {0};
		sprintf(szTemp, " %.2x", pbyMsg[i]);

		strcat(szBuf, szTemp);
		if ((i+1) % 16 == 0)
			strcat(szBuf, "\n");
	}

	OutputDebugString(szBuf);
	OutputDebugString("\n");
}