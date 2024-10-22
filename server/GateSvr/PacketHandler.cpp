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

//处理由地图服务器发送过来的消息,根据消息ID范围，进行处理
void PacketHandler::ParsePacketMap( ServerSession * pSession, unsigned char *pMsg, WORD wSize )
{
	//对包进行解析
	unsigned char* pInBuff = pMsg;
	DWORD dwGuid;
	WORD  wMsgID;
	DWORD dwSeriesId = 0;

	//解析包头
	if (NetMsgHeadHandler::DecodeServerHead(pInBuff, wSize, dwGuid, dwSeriesId, wMsgID))
	{
		printf("包头解析错误ParsePacketMap\n");
		return;
	}

	WORD wHeadLength = sizeof(dwGuid)+sizeof(dwSeriesId)+sizeof(wMsgID);
// 	unsigned char* pBodyBuff = pInBuff + wHeadLength;
// 	WORD wBodyLength = wSize - wHeadLength;

	//MapSvr发送给GateSvr的消息
	if (wMsgID >= M2G_MIN && wMsgID <= M2G_MAX)
	{
		NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
		if (!pMsgBody)
		{
			printf("未定义的消息结构MsgID=%d\n", wMsgID);
			return;
		}

		//解析包体
		pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength);
		//Gate处理由Map发过来的消息
		fnHandler pfnHandler = m_FunctionMapMap.Find(wMsgID);
		if (pfnHandler)
			pfnHandler(pSession, pMsgBody, dwGuid);
		else
		{
			GATELOG(DEBUG_LEVEL, "查找消息 Handler 失败，ParsePacketMap将无法处理消息（MSGID=%d）", wMsgID);
		}
	}
	//MapSvr发送给客户端的消息
	else if (wMsgID >= C2M_MIN && wMsgID <= C2M_MAX)
	{
		if (dwGuid)
		{
			//Gate把消息转发给玩家
			User* pUser = UserManager::Instance()->FindUserByGUID(dwGuid);
			if (pUser)
			{
				pUser->SendPacket(wMsgID, pInBuff + wHeadLength, wSize - wHeadLength);
#ifdef _DEBUG
				// 心跳包不输出
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
				printf("未定义的消息结构MsgID=%d\n", wMsgID);
				return;
			}
			//解析包体
			if (pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength) == -1)
			{
				printf("消息解析出错MsgID=%d\n", wMsgID);
				return;
			}
			HandlerFromMap::OnNetMessage(wMsgID, pInBuff + wHeadLength, wSize - wHeadLength, pMsgBody);
		}
	}
	//无效的消息包
	else
	{
		GATELOG(DEBUG_LEVEL, "消息ID定义范围出错，ParsePacketMap将无法处理消息(MSGID=%d)", wMsgID);
	}
}

//处理由玩家发送过来的消息,根据消息ID范围，进行处理
void PacketHandler::ParsePacketUser( User * pSession, unsigned char *pMsg, WORD wSize )
{
	//对包进行解析
	unsigned char* pInBuff = pMsg;
	WORD wMsgID;
	DWORD dwSeriesId;

	//解析包头
	if (NetMsgHeadHandler::DecodeClientHead(pInBuff, wSize, dwSeriesId, wMsgID))
	{
		printf("包头解析错误ParsePacketUser(MsgId:%d,Size:%d)\n", wMsgID, wSize);
		return;
	}

	WORD wHeadSize = sizeof(wMsgID)+sizeof(dwSeriesId);

	//客户端发送给GateSvr的消息
	if (wMsgID >= C2G_MIN && wMsgID <= C2G_MAX)
	{
		NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
		if (!pMsgBody)
		{
			printf("未定义的消息结构MsgID=%d\n", wMsgID);
			return;
		}

		//解析包体
		pMsgBody->DecodeMsg(pInBuff + wHeadSize, wSize - wHeadSize);
		//Gate处理由Client发过来的消息
		fnHandler_C pfnHandler = m_FunctionMapUser.Find(wMsgID);

		if (pfnHandler)
			pfnHandler(pSession, pMsgBody);
		else
		{
			GATELOG(DEBUG_LEVEL, "查找消息 Handler 失败，ParsePacketUser将无法处理消息（MSGID=%d）", wMsgID);
		}
	}
	//客户端发送给MapSvr的消息
	else if (wMsgID >= C2M_MIN && wMsgID <= C2M_MAX)
	{
		//Gate把消息转发给Map处理
		pSession->SendToMapServer(wMsgID, dwSeriesId, pInBuff + wHeadSize, wSize - wHeadSize);

 //#ifdef _DEBUG
		// 心跳包不输出
		if (wMsgID != C2S_HEART_BEAT_REQ && wMsgID != C2S_SERVER_TIME_REQ)
		{
			const char *pszIP = pSession->GetIP();
			NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
			GATELOG(SPECAIL_LEVEL, "recv_msg(guid:%d,id:%d,size:%d,name:%s,ip:%s)", pSession->GetGUID(),
				wMsgID, wSize, pMsgBody ? pMsgBody->GetClassName() : "nuknow", pszIP ? pszIP : "unknow_ip");
		}
 //#endif
	}
	//无效的消息包
	else
	{
		GATELOG(DEBUG_LEVEL, "消息ID定义范围出错，ParsePacketUser将无法处理消息（MSGID=%d）", wMsgID);
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