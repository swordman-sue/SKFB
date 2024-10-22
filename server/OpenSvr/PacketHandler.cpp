#include "stdafx.h"
#include "PacketHandler.h"
#include "HandlerFromMap.h"
#include "../Common/MsgDefine.h"
#include "NetBase.h"
#include "../header.pb.h"
#include <string>
#include "server.pb.h"
#include "../Common/MsgRegister.h"
//#include "../user_info.pb.h"

PacketHandler::PacketHandler(void)
{

}

PacketHandler::~PacketHandler(void)
{

}

BOOL PacketHandler::Resister()
{
	ResisterMap();
	return TRUE;
}

BOOL PacketHandler::AddHandlerMap(WORD dwMsgID, fnHandler fnHandler)
{
	m_FunctionMapMap.Add(dwMsgID, fnHandler);
	return TRUE;
}

//处理由地图服务器发送过来的消息,根据消息ID范围，进行处理
void PacketHandler::ParsePacketMap(ServerSession * pSession, unsigned char *pMsg, WORD wSize)
{
	//对包进行解析
	unsigned char* pInBuff = pMsg;
	DWORD dwGuid = 0;
	WORD wMsgID = 0;
	DWORD dwSeriesId = 0;

	//解析包头
	if (NetMsgHeadHandler::DecodeServerHead(pInBuff, wSize, dwGuid, dwSeriesId, wMsgID))
	{
		printf("包头解析错误ParsePacketMap\n");
		return;
	}

	if (wMsgID >= M2O_MIN && wMsgID <= M2O_MAX)
	{
		NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
		if (!pMsgBody)
		{
			printf("未定义的消息结构MsgID=%d\n", wMsgID);
			return;
		}

		WORD wHeadLength = sizeof(dwGuid)+sizeof(dwSeriesId)+sizeof(wMsgID);

		//解析包体
		pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength);
		//Open处理由Map发过来的消息
		fnHandler pfnHandler = m_FunctionMapMap.Find(wMsgID);
		if (pfnHandler)
		{
			pfnHandler(pSession, pMsgBody, dwGuid);

//#ifdef _DEBUG
			OPENLOG(GUI_LEVEL, "Msg From Map(Guid:%d, MsgId:%d,MsgSize:%d).", dwGuid, wMsgID, wSize);
//#endif
		}
		else
		{
			OPENLOG(DEBUG_LEVEL, "查找消息 Handler 失败，ParsePacketMap将无法处理消息（MSGID=%d）", wMsgID);
		}
	}
	else
	{
		OPENLOG(DEBUG_LEVEL, "消息ID定义范围出错，ParsePacketMap将无法处理消息（MSGID=%d）", wMsgID);
	}
}

BOOL PacketHandler::ResisterMap()
{
//	#define RESISTER_MSG(MSG_NAME) AddHandlerMap(SS::MSG_NAME::descriptor()->full_name(), HandlerFromMap::On##MSG_NAME);
//    #define RESISTER_MSG_EXP(MSG_NSP, MSG_NAME) AddHandlerMap(MSG_NSP::MSG_NAME::descriptor()->full_name(), HandlerFromMap::On##MSG_NAME);


	//在此注册与Map间通讯的消息
	AddHandlerMap(M2O_SERVERID_CMD, HandlerFromMap::OnMSG_M2O_SERVERID_CMD);
	AddHandlerMap(M2O_HEART_BEAT,	HandlerFromMap::OnMSG_M2O_HEART_BEAT);

	AddHandlerMap(M2O_LOGIC_COMMON_REQ, HandlerFromMap::OnNetLogicCommon);

	AddHandlerMap(M2O_AUTHENTICATION_CODE_REQ, HandlerFromMap::OnM2O_AUTHENTICATION_CODE_REQ);
	AddHandlerMap(M2O_REYUN_STATISTIC_NOTIFY, HandlerFromMap::OnM2O_REYUN_STATISTIC_NOTIFY);

	AddHandlerMap(M2O_CHECK_USER_INFO_REQ, HandlerFromMap::OnM2O_CHECK_USER_INFO_REQ);
	AddHandlerMap(M2O_RECHARGE_SEND_GOODS_RESP, HandlerFromMap::OnM2O_RECHARGE_SEND_GOODS_RESP);

	AddHandlerMap(M2O_RECHARGE_SUCCESS_REPORT_NOTIFY, HandlerFromMap::OnM2O_RECHARGE_SUCCESS_REPORT_NOTIFY);
	AddHandlerMap(M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY, HandlerFromMap::OnM2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY);

	AddHandlerMap(M2O_STATISTICS_NOTIFY,				HandlerFromMap::OnM2O_STATISTICS_NOTIFY);
	AddHandlerMap(M2O_ONLINE_PLAYER_NUM_RESP,			HandlerFromMap::OnM2O_ONLINE_PLAYER_NUM_RESP);
	
	AddHandlerMap(M2O_YGAME_RECHARGE_SEND_GOODS_RESP,	HandlerFromMap::OnM2O_YGAME_RECHARGE_SEND_GOODS_RESP);
	AddHandlerMap(M2O_YGAME_CASHBACK_NOTIFY_RESP,		HandlerFromMap::OnM2O_YGAME_CASHBACK_NOTIFY);
	AddHandlerMap(M2O_YGAME_BONUS_NOTIFY_RESP,			HandlerFromMap::OnM2O_YGAME_BONUS_NOTIFY);
	
	return TRUE;
}
