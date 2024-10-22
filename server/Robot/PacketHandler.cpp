#include "stdafx.h"
#include "Robot.h"
#include "PacketHandler.h"
#include "RobotSession.h"
#include "../Common/MessageIdDefine.h"
#include "../Common/PacketStruct_Code.h"
#include "../Common/MsgDefine.h"
#include "../Common/CommonLogic.h"
#include "../Common/MsgRegister.h"

PacketHandler::PacketHandler(void)
{
	m_FunctionMap.Clear();
}

PacketHandler::~PacketHandler(void)
{
	m_FunctionMap.Clear();
}

BOOL PacketHandler::Resister()
{
	//�ڴ����ע����Ҫ�������Ϣ
	AddHandler(S2C_LOGIN_ACCOUNT_RESP,			OnMSG_S2C_LOGIN_ACCOUNT_RESP);
	AddHandler(S2C_LOGIN_SELECT_SERVER_RESP,	OnMSG_S2C_LOGIN_SELECT_SERVER_RESP);
	AddHandler(S2C_LOGIN_CREATE_ROLE_RESP,		OnMSG_S2C_LOGIN_CREATE_ROLE_RESP);
	AddHandler(S2C_LOGIN_ENTER_GAME_NOTIFY,		OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY);
	AddHandler(S2C_GAME_ERROR_NOTIFY,			OnMSG_S2C_GAME_ERROR_NOTIFY);
	AddHandler(S2C_ROLE_INFO_NOTIFY,			OnMSG_S2C_ROLE_INFO_NOTIFY);
	AddHandler(S2C_ATTACK_CITY_ENTER_RESP,		OnMSG_S2C_ATTACK_CITY_ENTER_RESP);
	

	AddHandler(S2C_BATTLE_START_RESP,			OnS2C_BATTLE_START_RESP);

	return TRUE;
}

BOOL PacketHandler::AddHandler( DWORD dwMsgID, fnHandler fnHandler )
{
	m_FunctionMap.Add(dwMsgID, fnHandler);
	return TRUE;
}

void PacketHandler::ParsePacket( RobotSession * pSession, unsigned char *pMsg, WORD wSize )
{
	//�԰����н���
	unsigned char* pInBuff = pMsg;
	WORD wMsgID;
	DWORD dwSeriesId;

	//������ͷ
	if (NetMsgHeadHandler::DecodeClientHead(pInBuff, wSize, dwSeriesId, wMsgID))
	{
		printf("��ͷ��������ParsePacketUser\n");
		return;
	}

	NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
	if(!pMsgBody)
	{
		//printf("δ�������Ϣ�ṹMsgID=%d\n",dwMsgID);
		return;
	}

	WORD wHeadSize = sizeof(wMsgID)+sizeof(dwSeriesId);
	 
	//��������
	pMsgBody->DecodeMsg(pInBuff + wHeadSize, wSize - wHeadSize);

	fnHandler pfnHandler = m_FunctionMap.Find(wMsgID);

	if(pfnHandler)
		pfnHandler(pSession, pMsgBody);
}

VOID PacketHandler::OnMSG_M2C_ROBOT_LOGIN_ACK(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg)
		return;

	if (!pSession->GetOwner())
		return;

	pSession->GetOwner()->OnRobotLoginAck((MSG_M2C_ROBOT_LOGIN_ACK*)pMsg);
}

VOID PacketHandler::OnMSG_S2C_LOGIN_ACCOUNT_RESP(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_LOGIN_ACCOUNT_RESP(pMsg);
}

VOID PacketHandler::OnMSG_S2C_LOGIN_SELECT_SERVER_RESP(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_LOGIN_SELECT_SERVER_RESP(pMsg);
}

VOID PacketHandler::OnMSG_S2C_LOGIN_CREATE_ROLE_RESP(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_LOGIN_CREATE_ROLE_RESP(pMsg);
}

VOID PacketHandler::OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY(pMsg);
}

VOID PacketHandler::OnMSG_S2C_GAME_ERROR_NOTIFY(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_GAME_ERROR_NOTIFY(pMsg);
}

VOID PacketHandler::OnMSG_S2C_ROLE_INFO_NOTIFY(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_ROLE_INFO_NOTIFY(pMsg);
}

VOID PacketHandler::OnMSG_S2C_ATTACK_CITY_ENTER_RESP(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_ATTACK_CITY_ENTER_RESP(pMsg);
}

VOID PacketHandler::OnS2C_BATTLE_START_RESP(RobotSession * pSession, NetMsgBody *pMsg)
{
	if (!pSession || !pMsg || !pSession->GetOwner())
	{
		printf("[%s]�߼�����\n", __FUNCTION__);
		return;
	}

	pSession->GetOwner()->OnMSG_S2C_BATTLE_START_RESP(pMsg);
}

