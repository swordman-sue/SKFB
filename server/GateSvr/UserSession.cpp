#include "StdAfx.h"
#include "UserSession.h"
#include "../Common/PacketStruct_Code.h"

UserSession::UserSession(void)
{

}

UserSession::~UserSession(void)
{

}

//������Ϣ
BOOL UserSession::SendPacket(WORD wMsgID, NetMsgBody * pMsg)
{
	_ASSERT(pMsg != NULL);

	DWORD dwSeriesId = 0;
	static unsigned char buffer[sizeof(wMsgID)+sizeof(dwSeriesId)+2 * MAX_PACK_SIZE] = { 0 };
	unsigned char* pBuffer = buffer;
	

	//����ͷ��Ϣ
	short nBuffHeadSize = 0;
	if (NetMsgHeadHandler::EncodeClientHead(wMsgID, dwSeriesId, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	//ƫ����Ϣ��ָ��
	pBuffer += nBuffHeadSize;
	short nBuffBodySize = 0;

	//��������Ϣ
	if (pMsg->EncodeMsg(pBuffer, nBuffBodySize))
	{
		return FALSE;
	}

	//������Ϣ
	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + nBuffBodySize);
}

//������Ϣ
BOOL UserSession::SendPacket(WORD wMsgID, unsigned char * pData, WORD wDataSize)
{
	DWORD dwSeriesId = 0;
	static unsigned char buffer[sizeof(wMsgID)+sizeof(dwSeriesId)+2 * MAX_PACK_SIZE] = { 0 };
	unsigned char* pBuffer = buffer;

	//����ͷ��Ϣ
	short nBuffHeadSize = 0;
	if (NetMsgHeadHandler::EncodeClientHead(wMsgID, dwSeriesId, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	//ƫ����Ϣ��ָ��
	pBuffer += nBuffHeadSize;

	//��������Ϣ
	memcpy(pBuffer, pData, wDataSize);

	//===test
	//OutputDebug(buffer, nBuffHeadSize+wDataSize, dwMsgID);

	//������Ϣ
	return NetworkObject::Send((BYTE*)buffer, nBuffHeadSize + wDataSize);
}

VOID UserSession::Init()
{

}

VOID UserSession::Release()
{

}

//�Ͽ������¼�����
VOID UserSession::OnDisconnect()
{

}

//���������¼�����
VOID UserSession::OnAccept( DWORD dwNetworkIndex )
{
	SetSessionIndex(dwNetworkIndex);
}

//16���Ƹ�ʽ��ӡ��Ϣ��(������)
VOID UserSession::OutputDebug(BYTE* pbyMsg, UINT nSize, USHORT shMsgID)
{
	if (!pbyMsg || !nSize)
		return;

	char szBuf[1024*12] = {0};
	sprintf(szBuf, "��ϢID:%d\n", shMsgID);
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