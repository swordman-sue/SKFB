#include "StdAfx.h"
#include "PacketHandler.h"
#include "HandlerFromMap.h"
#include "HandlerFromClient.h"
#include "../Common/MsgDefine.h"
#include "UserSession.h"
#include "User.h"
#include "UserManager.h"
#include "NetBase.h"
#include "header.pb.h"
#include "../Common/MsgRegister.h"

PacketHandler::PacketHandler(void)
{
}

PacketHandler::~PacketHandler(void)
{
}

BOOL PacketHandler::Resister()
{
	ResisterMap();
	ResisterUser();
	return TRUE;
}

BOOL PacketHandler::AddHandlerMap( WORD wMsgID, fnHandler fnHandler )
{
	m_FunctionMapMap.Add(wMsgID,fnHandler);
	return TRUE;
}

BOOL PacketHandler::AddHandlerUser( WORD wMsgID, fnHandler_C fnHandler )
{
	m_FunctionMapUser.Add(wMsgID,fnHandler);
	return TRUE;
}

//�����ɵ�ͼ���������͹�������Ϣ,������ϢID��Χ�����д���
void PacketHandler::ParsePacketMap( ServerSession * pSession, unsigned char *pMsg, WORD wSize )
{
	//�԰����н���
	unsigned char* pInBuff = pMsg;
	DWORD dwGuid;
	WORD  wMsgID;
	DWORD dwSeriesId = 0;

	//������ͷ
	if (NetMsgHeadHandler::DecodeServerHead(pInBuff, wSize, dwGuid, dwSeriesId, wMsgID))
	{
		printf("��ͷ��������ParsePacketMap\n");
		return;
	}

	WORD wHeadLength = sizeof(dwGuid)+sizeof(dwSeriesId)+sizeof(wMsgID);
// 	unsigned char* pBodyBuff = pInBuff + wHeadLength;
// 	WORD wBodyLength = wSize - wHeadLength;

	//MapSvr���͸�GateSvr����Ϣ
	if (wMsgID >= M2G_MIN && wMsgID <= M2G_MAX)
	{
		NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
		if (!pMsgBody)
		{
			printf("δ�������Ϣ�ṹMsgID=%d\n", wMsgID);
			return;
		}

		//��������
		pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength);
		//Gate������Map����������Ϣ
		fnHandler pfnHandler = m_FunctionMapMap.Find(wMsgID);
		if (pfnHandler)
			pfnHandler(pSession, pMsgBody, dwGuid);
		else
		{
			GATELOG(DEBUG_LEVEL, "������Ϣ Handler ʧ�ܣ�ParsePacketMap���޷�������Ϣ��MSGID=%d��", wMsgID);
		}
	}
	//MapSvr���͸��ͻ��˵���Ϣ
	else if (wMsgID >= C2M_MIN && wMsgID <= C2M_MAX)
	{
		if (dwGuid)
		{
			//Gate����Ϣת�������
			User* pUser = UserManager::Instance()->FindUserByGUID(dwGuid);
			if (pUser)
			{
				pUser->SendPacket(wMsgID, pInBuff + wHeadLength, wSize - wHeadLength);
#ifdef _DEBUG
				// �����������
				if (wMsgID != S2C_HEART_BEAT_RESP && wMsgID != S2C_SERVER_TIME_RESP)
				{
					NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
					GATELOG(GUI_LEVEL, "send_msg(guid:%d,id:%d,size:%d,name:%s)",
						dwGuid, wMsgID, wSize, pMsgBody ? pMsgBody->GetClassName() : "nuknow");
				}
#endif
			}
		}
		else
		{
			NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
			if (!pMsgBody)
			{
				printf("δ�������Ϣ�ṹMsgID=%d\n", wMsgID);
				return;
			}
			//��������
			if (pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength) == -1)
			{
				printf("��Ϣ��������MsgID=%d\n", wMsgID);
				return;
			}
			HandlerFromMap::OnNetMessage(wMsgID, pInBuff + wHeadLength, wSize - wHeadLength, pMsgBody);
		}
	}
	//��Ч����Ϣ��
	else
	{
		GATELOG(DEBUG_LEVEL, "��ϢID���巶Χ����ParsePacketMap���޷�������Ϣ(MSGID=%d)", wMsgID);
	}
}

//��������ҷ��͹�������Ϣ,������ϢID��Χ�����д���
void PacketHandler::ParsePacketUser( User * pSession, unsigned char *pMsg, WORD wSize )
{
	//�԰����н���
	unsigned char* pInBuff = pMsg;
	WORD wMsgID;
	DWORD dwSeriesId;

	//������ͷ
	if (NetMsgHeadHandler::DecodeClientHead(pInBuff, wSize, dwSeriesId, wMsgID))
	{
		printf("��ͷ��������ParsePacketUser(MsgId:%d,Size:%d)\n", wMsgID, wSize);
		return;
	}

	WORD wHeadSize = sizeof(wMsgID)+sizeof(dwSeriesId);

	//�ͻ��˷��͸�GateSvr����Ϣ
	if (wMsgID >= C2G_MIN && wMsgID <= C2G_MAX)
	{
		NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
		if (!pMsgBody)
		{
			printf("δ�������Ϣ�ṹMsgID=%d\n", wMsgID);
			return;
		}

		//��������
		pMsgBody->DecodeMsg(pInBuff + wHeadSize, wSize - wHeadSize);
		//Gate������Client����������Ϣ
		fnHandler_C pfnHandler = m_FunctionMapUser.Find(wMsgID);

		if (pfnHandler)
			pfnHandler(pSession, pMsgBody);
		else
		{
			GATELOG(DEBUG_LEVEL, "������Ϣ Handler ʧ�ܣ�ParsePacketUser���޷�������Ϣ��MSGID=%d��", wMsgID);
		}
	}
	//�ͻ��˷��͸�MapSvr����Ϣ
	else if (wMsgID >= C2M_MIN && wMsgID <= C2M_MAX)
	{
		//Gate����Ϣת����Map����
		pSession->SendToMapServer(wMsgID, dwSeriesId, pInBuff + wHeadSize, wSize - wHeadSize);

 //#ifdef _DEBUG
		// �����������
		if (wMsgID != C2S_HEART_BEAT_REQ && wMsgID != C2S_SERVER_TIME_REQ)
		{
			const char *pszIP = pSession->GetIP();
			NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
			GATELOG(SPECAIL_LEVEL, "recv_msg(guid:%d,id:%d,size:%d,name:%s,ip:%s)", pSession->GetGUID(),
				wMsgID, wSize, pMsgBody ? pMsgBody->GetClassName() : "nuknow", pszIP ? pszIP : "unknow_ip");
		}
 //#endif
	}
	//��Ч����Ϣ��
	else
	{
		GATELOG(DEBUG_LEVEL, "��ϢID���巶Χ����ParsePacketUser���޷�������Ϣ��MSGID=%d��", wMsgID);
	}
}

BOOL PacketHandler::ResisterMap()
{
	AddHandlerMap(M2G_CLIENT_KICKOUT_CMD, HandlerFromMap::OnMSG_M2G_CLIENT_KICKOUT_CMD);
	AddHandlerMap(M2G_HEART_BEAT, HandlerFromMap::OnMSG_M2G_HEART_BEAT);
	AddHandlerMap(M2G_CLIENT_INFO, HandlerFromMap::OnMSG_M2G_CLIENT_INFO);
	
	return TRUE;
}

BOOL PacketHandler::ResisterUser()
{
	return TRUE;
}