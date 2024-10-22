#include "StdAfx.h"
#include "HandlerFromOpen.h"
#include "../Common/MsgDefine.h"
#include "MapServer.h"
#include "GateServerSession.h"
#include "AccountInfoManager.h"
#include "AccountInfo.h"
#include "ObjectFactory.h"
#include "MapSvrQuery.h"
#include "PlayerManager.h"
#include "Player.h"
#include "OpenServerSession.h"
#include "../Common/CommonLogic.h"
#include "../ToolKit/DBCFileManager.h"
#include "../ToolKit/DBCFile.h"
#include "../Common/Const.h"
#include "ConfigManager.h"
#include "../Common/DBCDefine.h"
#include "server.pb.h"
#include "login.pb.h"
//#include "../user_info.pb.h"
#include <atlenc.h>
//#include "web_enum_define.pb.h"
#include "PacketHandler.h"
#include "Recharge/RechargeManager.h"
#include "Jsoncpp/include/json/value.h"
#include "GameLogic.h"

HandlerFromOpen::HandlerFromOpen(void)
{

}

HandlerFromOpen::~HandlerFromOpen(void)
{

}

VOID HandlerFromOpen::OnMSG_O2M_HEART_BEAT(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	g_MapServer.InitOpenBeat();
}

VOID HandlerFromOpen::OnMSG_O2M_AUTHENTICATION_CODE(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_AUTHENTICATION_CODE_RESP *pRecv = (MSG_O2M_AUTHENTICATION_CODE_RESP*)pMsg;

	// 检测数据是否合法
	if (pRecv->m_nErrorCode != 0)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取手机认证码失败![mobile:%s,error_code:%d]",
			__FUNCTION__, pRecv->m_szMobile, pRecv->m_nErrorCode);
		return;
	}

	MobileAuthData tAuthData;
	tAuthData.dwTime = time(NULL);
	tAuthData.strMobile = pRecv->m_szMobile;
	tAuthData.wCode = pRecv->m_wCode;
	g_MapServer.AddMobileAuthData(tAuthData);
}

// CP订单号请求
VOID HandlerFromOpen::OnNetCPOrderId(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_CP_ORDER_ID_REQ *pRecv = (MSG_O2M_CP_ORDER_ID_REQ*)pMsg;

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByRoleId(pRecv->m_dwRoleId);
	if (pPlayer)
	{
		CRechargeManager *pRechargeMgr = pPlayer->GetRechargeManager();
		if (pRechargeMgr)
			pRechargeMgr->OnNetOrderIdFromYD(dwGuid);
	}
	else
	{
		MSG_M2O_CP_ORDER_ID_RESP resp;
		resp.m_dwRoleId = pRecv->m_dwRoleId;
		resp.m_dwOrderId = time(NULL);
		g_MapServer.GetOpenSession()->SendPacket(dwGuid, resp.GetType(), &resp);
	}
}

// 在线玩家数量
VOID HandlerFromOpen::OnNetOnLinePlayerNum(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_ONLINE_PLAYER_NUM_REQ *pRecv = (MSG_O2M_ONLINE_PLAYER_NUM_REQ*)pMsg;

	MSG_M2O_ONLINE_PLAYER_NUM_RESP resp;
	resp.m_wServerNum = 0;
	const map<DWORD, ServerInfo> &vServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = vServerList.begin();
	for (; iter != vServerList.end(); ++iter)
	{
		vector<Player*> vPlayerList;
		PlayerManager::Instance()->GetAllPlayer(vPlayerList, iter->second.dwServerId);
		resp.m_PlayerNumList[resp.m_wServerNum] = vPlayerList.size();
		resp.m_ServerIdList[resp.m_wServerNum] = iter->second.dwServerId;
		++resp.m_wServerNum;
	}
	pSession->SendPacket(dwGuid, resp.GetType(), &resp);
}

// YGame充值发货请求
VOID HandlerFromOpen::OnMsgYameRechargeSendGoodsReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_YGAME_RECHARGE_SEND_GOODS_REQ *pRecv = (MSG_O2M_YGAME_RECHARGE_SEND_GOODS_REQ*)pMsg;

	CRechargeManager::OnNetRechargeCallbackForYGame(dwGuid, pRecv->m_wServerId, pRecv->m_dwRoleId, pRecv->m_nAmount, pRecv->m_szPaymentId);
}

// YGame提现通知请求
VOID HandlerFromOpen::OnMsgYameCashbackNotifyReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_YGAME_CASHBACK_NOTIFY_REQ *pRecv = (MSG_O2M_YGAME_CASHBACK_NOTIFY_REQ*)pMsg;

	CRechargeManager::OnNetYGameCashbackNotify(dwGuid, pRecv->m_wServerId, pRecv->m_dwRoleId, pRecv->m_nYcoin, pRecv->m_nDiamond);
}

// YGame奖励通知请求
VOID HandlerFromOpen::OnMsgYameBonusNotifyReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_YGAME_BONUS_NOTIFY_REQ *pRecv = (MSG_O2M_YGAME_BONUS_NOTIFY_REQ*)pMsg;

	CRechargeManager::OnNetYGameBonusNotify(dwGuid, pRecv->m_wServerId, pRecv->m_dwRoleId, pRecv->m_dwDiamond);
}

VOID HandlerFromOpen::OnMsg_LogicCommon_O2M(OpenServerSession * pSession, unsigned char *pMsg, DWORD dwGuid, WORD wSize)
{
// 	SS::Msg_LogicCommon_O2M msg;
// 	if ( !msg.ParseFromArray(pMsg, wSize))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]解析web回调通用消息出错!!!(%s)!",__FUNCTION__,msg.GetDescriptor()->full_name().c_str());
// 		return;
// 	}
// 
// 	DWORD dwMsgId = msg.msg_id();
// 	DWORD dwReturnCode = msg.msg_code();
// 	const char *pszReturnDesc = msg.msg_desc().data();
// 	const char *pszReturnBody = msg.msg_body().data();
// 	DWORD dwReturnBodySize = msg.msg_body().size();
// 
// 	if (HTTP_URL_INVALID == msg.msg_code())
// 	{
// 		Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 		if (!pPlayer)
// 		{
// 			MAPLOG(ERROR_LEVEL,"[%s]处理web回调时找不到用户[Guid:%d]", __FUNCTION__, dwGuid);
// 			return;
// 		}
// 
// 		CS::Msg_TargetServerInvalid_S2C error_msg;
// 		const HttpUrl_Config* pHttpUrlCfg = CConfigManager::Instance()->GetHttpUrl(dwMsgId);	
// 		if (pHttpUrlCfg)
// 		{
// 			error_msg.set_url(pHttpUrlCfg->pszUrl);
// 			error_msg.set_url_desc(pHttpUrlCfg->pszUrlDesc);
// 		}
// 		pPlayer->SendPacket(&error_msg);
// 
// 		MAPLOG(ERROR_LEVEL,"请求的URL无效或服务器没有开启[MsgId:%d]",dwMsgId);
// 		return;
// 	}
// 	else if (HTTP_TEXT_INVALID == msg.msg_code())
// 	{
// 		// todo::对用户进行反馈(目前没处理)
// 
// 		MAPLOG(ERROR_LEVEL,"[%s]Http响应的内容出错[MsgId:%d]", __FUNCTION__,dwMsgId);
// 		return;
// 	}
// 	else
// 	{
// //#ifdef _DEBUG
// 		MAPLOG(GUI_LEVEL,"消息收到成功响应[id:%d, code:%d]", dwMsgId, dwReturnCode);
// //#endif
// 	}
// 
// // 	switch (dwMsgId)
// // 	{
// // 		case msg_load_pointcfg:
// // 			COpenMsgCallBack::LoadPointCfg(dwGuid, dwReturnCode, pszReturnDesc, pszReturnBody, dwReturnBodySize);
// // 			break;
// // 
// // 		// todo::如果有需要特殊处理的,可以截获处理
// // 		
// // 		default:
// // 			COpenMsgCallBack::CommonCallBack(dwGuid, dwReturnCode, pszReturnDesc, pszReturnBody, dwReturnBodySize, dwMsgId);
// // 			break;
// // 	}
}

// 登录响应
VOID HandlerFromOpen::OnMsgCheckUserInfoResp(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_CHECK_USER_INFO_RESP *pRecv = (MSG_O2M_CHECK_USER_INFO_RESP*)pMsg;

	// 1为成功,其他值为失败
	if (1 != pRecv->m_nResult)
	{
		MAPLOG(ERROR_LEVEL, "[%s]登录帐号失败[account:%s,cp_channel_type:%d,result:%d]", 
			__FUNCTION__, pRecv->m_szAccount, pRecv->m_dwCPChannelType, pRecv->m_nResult);
		return;
	}

	const char *pszAccount = pRecv->m_szAccount;

	DWORD dwChannelUID = pRecv->m_dwCPChannelType * 100000 + pRecv->m_dwSDKChannelType;

	// 检测是否在待登录状态
	if (g_MapServer.FindWaitingLoginAccount(pszAccount, dwChannelUID))
	{
		MAPLOG(GUI_LEVEL, "[%s]重复登录帐号[%s]", __FUNCTION__, pszAccount);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_REPEAT_LOGIN);
		return;
	}

	g_MapServer.OnAccountLogin(pRecv->m_szAccount, "sdk", dwChannelUID, dwGuid, pRecv->m_byTerminalType);

	MAPLOG(GUI_LEVEL, "SDK请求登录响应[channel:%d,account:%s]", dwChannelUID, pszAccount);

	// 记录客户端信息
	ClientInfo tClientInfo;
	tClientInfo.byTerminalType = pRecv->m_byTerminalType;
	strcpy_s(tClientInfo.szDeviceId, sizeof(tClientInfo.szDeviceId), pRecv->m_szDeviceId);
	strcpy_s(tClientInfo.szIP, sizeof(tClientInfo.szIP), pRecv->m_szIP);
	CGameLogic::Instance()->AddClientInfo(dwChannelUID, pRecv->m_szAccount, tClientInfo);
}

// 充值发货请求
VOID HandlerFromOpen::OnMsgRechargeSendGoodsReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_O2M_RECHARGE_SEND_GOODS_REQ *pRecv = (MSG_O2M_RECHARGE_SEND_GOODS_REQ*)pMsg;

	CRechargeManager::OnNetRechargeCallbackEx(dwGuid, pRecv->m_CallbackData);

	// 发货响应
	MSG_M2O_RECHARGE_SEND_GOODS_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);
}

