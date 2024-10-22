#include "stdafx.h"
#include "HandlerFromMap.h"
#include "ServerSession.h"
#include "MapServerSession.h"
#include "../Common/MsgDefine.h"
#include "HttpQueryResult.h"
#include "OpenServer.h"
#include "ServerSessionManager.h"
#include "../Common/CommonLogic.h"
#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"
#include "../Common/Http_Util.h"
#include "PHPSession.h"
#include "PHPSessionManager.h"
#include "../ToolKit/MD5.h"
#include "../server.pb.h"
//#include "../user_info.pb.h"
#include <atlenc.h>
#include "../Common/Const.h"
#include "../ToolKit/sha256.h"

HandlerFromMap::HandlerFromMap(void)
{

}

HandlerFromMap::~HandlerFromMap(void)
{

}

VOID HandlerFromMap::OnMSG_M2O_SERVERID_CMD(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_SERVERID_CMD* pRecv = (MSG_M2O_SERVERID_CMD*)pMsg;

	MapServerSession* pMapSession = (MapServerSession*)pSession;

	pMapSession->SetServerID(pRecv->m_dwServerID);
	pMapSession->SetValid(true);

	ServerSessionManager::Instance()->AddMapServer(pMapSession);
}

VOID HandlerFromMap::OnMSG_M2O_HEART_BEAT(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	if (pSession)
	{
		MSG_O2M_HEART_BEAT Msg;
		pSession->SendPacket(0, Msg.GetType(), &Msg);
	}
}

VOID HandlerFromMap::OnM2O_AUTHENTICATION_CODE_REQ(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_AUTHENTICATION_CODE_REQ* pRecv = (MSG_M2O_AUTHENTICATION_CODE_REQ*)pMsg;

	HttpAuthenticationCode* pQuery = HttpAuthenticationCode::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,向第三方请求身份验证失败!");
		return;
	}

	char szRandom[16] = { 0 };
	WORD wCode = g_Rand(1000, 9999);
	sprintf_s(szRandom, sizeof(szRandom), "%d", wCode);
	char szContext[128] = { 0 };
	sprintf_s(szContext, sizeof(szContext), "phoneNumber=%s&&random=%d&&ext=%s&&templId=%d", 
		pRecv->m_szMobile, wCode, pRecv->m_szMobile, pRecv->m_dwTemplId);
	pQuery->SetGUID(dwGuid);
	pQuery->AddParam("phoneNumber", pRecv->m_szMobile);
	pQuery->AddParam("random", szRandom);
	pQuery->AddParam("ext", pRecv->m_szMobile);
	pQuery->SetMobile(pRecv->m_szMobile);
	pQuery->SetIndex(OPEN_QUERY_AUTH);
	pQuery->SetRequestType(HTTP_REQUEST_GET);
	pQuery->SetHost(g_OpenServer.GetNodeJsServerId());
	pQuery->SetPort(g_OpenServer.GetNodeJsServerPort());
	pQuery->SetCode(wCode);
	pQuery->SetPath(g_OpenServer.GetSmsMethod());
	pQuery->SetContext(szContext); 
	pQuery->SetHttpUrl("http://172.18.120.83:7878/SmsSender"); // todo::暂时不使用
	g_OpenServer.HttpOpenQuery(pQuery);
}

VOID HandlerFromMap::OnM2O_REYUN_STATISTIC_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_REYUN_STATISTIC_NOTIFY* pRecv = (MSG_M2O_REYUN_STATISTIC_NOTIFY*)pMsg;

	HttpReyunStatisticNotify* pQuery = HttpReyunStatisticNotify::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,行为统计通知失败!");
		return;
	}


	//OPENLOG(ERROR_LEVEL, "context:%s,path:%s", pRecv->m_szContext, pRecv->m_szPath);

	pQuery->SetIndex(HTTP_REYUN_STATISTIC);
	pQuery->SetRequestType(HTTP_REQUEST_POST);
	pQuery->SetHost("log.reyun.com");
	pQuery->SetPort(80);
	pQuery->SetPath(pRecv->m_szPath);
	pQuery->SetContext(pRecv->m_szContext);
	g_OpenServer.HttpOpenQuery(pQuery);

	//UrlEncode()

// 	CHttpParamStringMaker stringMaker;
// 	stringMaker.add_param("app_id", 20160602);
// 	stringMaker.add_param("data", pRecv->m_szContext);
// 	stringMaker.add_param("path", pRecv->m_szPath);
// 	string strContext = stringMaker.get_params();
// 
//  	pQuery->SetIndex(HTTP_REYUN_STATISTIC);
// 	pQuery->SetRequestType(HTTP_REQUEST_POST);
//  	pQuery->SetHost("192.168.1.111");
//  	pQuery->SetPort(20181);
//  	//pQuery->SetPath(pRecv->m_szPath);
// 	pQuery->SetContext(strContext.c_str());
//  	pQuery->SetPath("/reyun_statistics");
//  	//pQuery->AddParam("data", pRecv->m_szContext);
//  	g_OpenServer.HttpOpenQuery(pQuery);
}

VOID HandlerFromMap::OnM2O_CHECK_USER_INFO_REQ(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_CHECK_USER_INFO_REQ* pRecv = (MSG_M2O_CHECK_USER_INFO_REQ*)pMsg;

	HttpCheckUser* pQuery = HttpCheckUser::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,检测用户信息失败!");
		return;
	}

	// 设置客户端信息
	pQuery->SetKey(dwGuid);
	pQuery->SetIP(pRecv->m_szIP);
	pQuery->SetDeviceId(pRecv->m_szDeviceId);


	// 翔通(新马港台澳)
// 	if ((pRecv->m_dwCPChannelType >= CHANNEL_XMGT_IOS_BEGIN && pRecv->m_dwCPChannelType <= CHANNEL_XMGT_IOS_END) || 
// 		(pRecv->m_dwCPChannelType >= CHANNEL_XMGT_ANDROID_BEGIN && pRecv->m_dwCPChannelType <= CHANNEL_XMGT_ANDROID_END))
// 	{
// 		OnCheckUserInfo_XMGT(pQuery, pRecv, dwGuid);
// 	}
	// cgame_bt 20301 todo::其实也是用quick_sdk, 但是为了数据互通,特殊处理
// 	else if (pRecv->m_dwCPChannelType == 99995)
// 	{
// 		string strProductCode = IsAndroid(pRecv->m_byTerminalType) ? g_OpenServer.GetQuickSDKProductCode() : g_OpenServer.GetQuickSDKProductCodeIOS();
// 
// 		char szContext[1024] = { 0 };
// 		sprintf_s(szContext, sizeof(szContext), "token=%s&uid=%s&product_code=%s",
// 			pRecv->m_szToken, pRecv->m_szAccount, strProductCode.c_str());
// 		pQuery->SetIndex(HTTP_CHECK_USER_INFO);
// 		pQuery->SetRequestType(HTTP_REQUEST_GET);
// 		pQuery->SetHost("checkuser.sdk.quicksdk.net");
// 		pQuery->SetPort(80);
// 		pQuery->SetPath("/v2/checkUserInfo");
// 		pQuery->SetContext(szContext);
// 		pQuery->SetGUID(dwGuid);
// 		pQuery->SetAccount(pRecv->m_szAccount);
// 		pQuery->SetToken(pRecv->m_szToken);
// 		pQuery->SetCPChannelType(pRecv->m_dwCPChannelType);
// 		pQuery->SetSDKChannelType(pRecv->m_dwSDKChannelType);
// 		pQuery->SetTerminalType(pRecv->m_byTerminalType);
// 		g_OpenServer.HttpOpenQuery(pQuery);
// 	}
	// 走通用逻辑(通过nodejs处理)  普通渠道ID段: 20000 - 29999
	if (pRecv->m_dwCPChannelType > 20000)
	{
		CHttpParamStringMaker stringMaker;
		stringMaker.add_param("terminal_type", pRecv->m_byTerminalType);
		stringMaker.add_param("channel_type", pRecv->m_dwCPChannelType);
		stringMaker.add_param("account", pRecv->m_szAccount);
		stringMaker.add_param("token", pRecv->m_szToken);
		stringMaker.add_param("ip", pRecv->m_szIP);

		string strContext = stringMaker.get_params();
		pQuery->SetIndex(HTTP_CHECK_USER_INFO);
		pQuery->SetRequestType(HTTP_REQUEST_POST);
		pQuery->SetHost(g_OpenServer.GetNodeJsServerId());
		pQuery->SetPort(g_OpenServer.GetNodeJsServerPort());
		pQuery->SetPath("/check_user_info");
		pQuery->SetContext(strContext.c_str());
		pQuery->SetGUID(dwGuid);
		pQuery->SetAccount(pRecv->m_szAccount);
		pQuery->SetToken(pRecv->m_szToken);
		pQuery->SetCPChannelType(pRecv->m_dwCPChannelType);
		pQuery->SetSDKChannelType(pRecv->m_dwSDKChannelType);
		pQuery->SetTerminalType(pRecv->m_byTerminalType);
		g_OpenServer.HttpOpenQuery(pQuery);
		//OnCheckUserInfo_Common(pQuery, pRecv, dwGuid);
	}
	// Quick_SDK quicksdk专用ID段: 10000 - 19999
	else
	{
		string strProductCode = IsAndroid(pRecv->m_byTerminalType) ? g_OpenServer.GetQuickSDKProductCode() : g_OpenServer.GetQuickSDKProductCodeIOS();

		char szContext[1024] = { 0 };
		sprintf_s(szContext, sizeof(szContext), "token=%s&uid=%s&product_code=%s",
			pRecv->m_szToken, pRecv->m_szAccount, strProductCode.c_str());
		pQuery->SetIndex(HTTP_CHECK_USER_INFO);
		pQuery->SetRequestType(HTTP_REQUEST_GET);
		pQuery->SetHost("checkuser.sdk.quicksdk.net");
		pQuery->SetPort(80);
		pQuery->SetPath("/v2/checkUserInfo");
		pQuery->SetContext(szContext);
		pQuery->SetGUID(dwGuid);
		pQuery->SetAccount(pRecv->m_szAccount);
		pQuery->SetToken(pRecv->m_szToken);
		pQuery->SetCPChannelType(pRecv->m_dwCPChannelType);
		pQuery->SetSDKChannelType(pRecv->m_dwSDKChannelType);
		pQuery->SetTerminalType(pRecv->m_byTerminalType);
		g_OpenServer.HttpOpenQuery(pQuery);
	}
}

VOID HandlerFromMap::OnM2O_RECHARGE_SEND_GOODS_RESP(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_RECHARGE_SEND_GOODS_RESP* pRecv = (MSG_M2O_RECHARGE_SEND_GOODS_RESP*)pMsg;

	PHPSession *pRespSession = PHPSessionManager::Instance()->FindUserByGUID(dwGuid);
	if (!pRespSession)
	{
		OPENLOG(GUI_LEVEL, "找不到session[Guid:%d]!",dwGuid);
		return;
	}

	UINT64 lCurTime = g_GetCurrentTime();

	CHttpResponseMaker resp_maker;
	char szRespBuff[256] = { 0 };
	resp_maker.make("SUCCESS", 7, szRespBuff, sizeof(szRespBuff));
	pRespSession->PreSend((BYTE*)szRespBuff, strlen(szRespBuff));

	DWORD dwCostTime = g_GetCurrentTime() - lCurTime;
	if (dwCostTime)
	{
		OPENLOG(ERROR_LEVEL, "响应quck_sdk充值通知消耗[%d]毫秒!", dwCostTime);
	}

	// todo::进入等待回收列表
}

VOID HandlerFromMap::OnM2O_YGAME_RECHARGE_SEND_GOODS_RESP(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_YGAME_RECHARGE_SEND_GOODS_RESP* pRecv = (MSG_M2O_YGAME_RECHARGE_SEND_GOODS_RESP*)pMsg;

	PHPSession *pRespSession = PHPSessionManager::Instance()->FindUserByGUID(dwGuid);
	if (!pRespSession)
	{
		OPENLOG(GUI_LEVEL, "找不到session[Guid:%d]!", dwGuid);
		return;
	}

	UINT64 lCurTime = g_GetCurrentTime();

	CHttpResponseMaker resp_maker;
	char szRespBuff[256] = { 0 };
	resp_maker.make("SUCCESS", 7, szRespBuff, sizeof(szRespBuff));
	pRespSession->PreSend((BYTE*)szRespBuff, strlen(szRespBuff));

	DWORD dwCostTime = g_GetCurrentTime() - lCurTime;
	if (dwCostTime){
		OPENLOG(ERROR_LEVEL, "Resp_YGame_Recharge_Send_Goods[%d]ms!", dwCostTime);
	}
}

VOID HandlerFromMap::OnM2O_YGAME_CASHBACK_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_YGAME_CASHBACK_NOTIFY_RESP* pRecv = (MSG_M2O_YGAME_CASHBACK_NOTIFY_RESP*)pMsg;

	PHPSession *pRespSession = PHPSessionManager::Instance()->FindUserByGUID(dwGuid);
	if (!pRespSession)
	{
		OPENLOG(GUI_LEVEL, "找不到session[Guid:%d]!", dwGuid);
		return;
	}

	DWORD lCurTime = g_GetCurrentTime();

	CHttpResponseMaker resp_maker;
	char szRespBuff[256] = { 0 };
	resp_maker.make("SUCCESS", 7, szRespBuff, sizeof(szRespBuff));
	pRespSession->PreSend((BYTE*)szRespBuff, strlen(szRespBuff));

	DWORD dwCostTime = g_GetCurrentTime() - lCurTime;
	if (dwCostTime){
		OPENLOG(ERROR_LEVEL, "Resp_YGame_Cashback_Notify_Cost[%d]ms!", dwCostTime);
	}
}

VOID HandlerFromMap::OnM2O_YGAME_BONUS_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_YGAME_BONUS_NOTIFY_RESP* pRecv = (MSG_M2O_YGAME_BONUS_NOTIFY_RESP*)pMsg;

	PHPSession *pRespSession = PHPSessionManager::Instance()->FindUserByGUID(dwGuid);
	if (!pRespSession)
	{
		OPENLOG(GUI_LEVEL, "找不到session[Guid:%d]!", dwGuid);
		return;
	}

	UINT64 lCurTime = g_GetCurrentTime();

	CHttpResponseMaker resp_maker;
	char szRespBuff[256] = { 0 };
	resp_maker.make("SUCCESS", 7, szRespBuff, sizeof(szRespBuff));
	pRespSession->PreSend((BYTE*)szRespBuff, strlen(szRespBuff));

	DWORD dwCostTime = g_GetCurrentTime() - lCurTime;
	if (dwCostTime){
		OPENLOG(ERROR_LEVEL, "Resp_YGame_Bonus_Notify_Cost[%d]ms!", dwCostTime);
	}
}

VOID HandlerFromMap::OnM2O_SDK_ORDER_ID_REQ(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_SDK_ORDER_ID_REQ* pRecv = (MSG_M2O_SDK_ORDER_ID_REQ*)pMsg;

	HttpCheckUser* pQuery = HttpCheckUser::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,检测用户信息失败!");
		return;
	}

	// Singnature计算方法示例：
	// signature = to_lower_case(md5_hex(appId = XX&cpId = XX&cpOrderNumber = XX&extInfo = XX&notifyUrl = XXX&
	// orderAmount = XXX&orderDesc = XXX&orderTime = XXX&orderTitle = XXX&version = XXX&to_lower_case(md5_hex(App - key))))


	LinkString lingString;
	lingString.AddParam("appId", "3aa78491742530019c4e00129644f541");
	lingString.AddParam("cpId", "20140905155653932745");
	lingString.AddParam("cpOrderNumber", time(NULL));
	std::string strLinkString = lingString.CreateLinkString();

	MD5::Digest digest = MD5::Encode(strLinkString);
	string strSignature = digest.HexString();

// 	char szContext[256] = { 0 };
// 	sprintf_s(szContext, sizeof(szContext), "token=%s", pRecv->m_szToken);
// 	pQuery->SetIndex(HTTP_CHECK_USER_INFO);
// 	pQuery->SetRequestType(HTTP_REQUEST_GET);
// 	pQuery->SetHost("192.168.1.111");
// 	pQuery->SetPort(16888);
// 	pQuery->SetPath("/order_id_vivo");
// 	pQuery->SetContext(szContext);
// 	pQuery->SetGUID(dwGuid);
// 	g_OpenServer.HttpOpenQuery(pQuery);
}

VOID HandlerFromMap::OnM2O_RECHARGE_SUCCESS_REPORT_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_RECHARGE_SUCCESS_REPORT_NOTIFY* pRecv = (MSG_M2O_RECHARGE_SUCCESS_REPORT_NOTIFY*)pMsg;

	HttpRechargeSuccessReport* pQuery = HttpRechargeSuccessReport::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,充值成功上报失败!");
		return;
	}

	string strAppId = "5115";

	// sign  是  string  md5_32 位小写(app_key +timestamp)
	DWORD dwTimestamp = time(NULL);
	string strAppKey = "50x7Ssfb664gwndoL69za741oxmw0q57";
	char szSrcSign[128] = { 0 };
	sprintf_s(szSrcSign, sizeof(szSrcSign), "%s%d", strAppKey.c_str(), dwTimestamp);
	string strMd5Sign = MD5::Encode(szSrcSign).HexString();
	RechargeSuccessReportData &tReportData = pRecv->m_ReportData;

	// http://td.go.cc/pay_server
	CHttpParamStringMaker stringMaker;
	stringMaker.add_param("sign", strMd5Sign);
	stringMaker.add_param("timestamp", dwTimestamp);
	stringMaker.add_param("app_id", strAppId);
	stringMaker.add_param("channel", tReportData.dwChannel);
	stringMaker.add_param("server_id", tReportData.dwServerId);
	stringMaker.add_param("account_id", tReportData.szAccountId);
	stringMaker.add_param("role_id", tReportData.dwRoleId);
	char szRoleName[128] = { 0 };
	UrlEncode(tReportData.szRoleName, szRoleName, sizeof(szRoleName), TRUE);
	stringMaker.add_param("role_name", szRoleName);
	stringMaker.add_param("role_level", tReportData.wRoleLevel);
	stringMaker.add_param("vip_level", tReportData.wVipLevel);
	stringMaker.add_param("vip_exp", tReportData.dwVipExp);
	stringMaker.add_param("order_id", tReportData.szOrderId);
	stringMaker.add_param("iap_id", tReportData.dwIapId);
	stringMaker.add_param("iap_des", tReportData.szIapDes);
	stringMaker.add_param("currency_amount", tReportData.dwCurrencyAmount);
	stringMaker.add_param("currency_type", 1);
	stringMaker.add_param("platform_id", 2);
	stringMaker.add_param("access_type", 1);
	stringMaker.add_param("virtual_currency_amount", tReportData.dwVirtualCurrencyAmount);
	stringMaker.add_param("virtual_currency_amount_ex", tReportData.dwVirtualCurrencyAmountEx);
	stringMaker.add_param("ip", tReportData.szIP);
	string strContext = stringMaker.get_params();

	pQuery->SetIndex(HTTP_RECHARGE_SUCCESS_REPORT);
	pQuery->SetRequestType(HTTP_REQUEST_POST);
	pQuery->SetHost("td.go.cc");
	pQuery->SetPort(80);
	pQuery->SetPath("/pay_server");
	pQuery->SetContext(strContext.c_str());
	pQuery->SetGUID(dwGuid);
	g_OpenServer.HttpOpenQuery(pQuery);

	OPENLOG(GUI_LEVEL, "context[%s]", strContext.c_str());
}

VOID HandlerFromMap::OnM2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY* pRecv = (MSG_M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY*)pMsg;

	HttpRechargeSuccessReport* pQuery = HttpRechargeSuccessReport::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,充值成功上报失败!");
		return;
	}

	/*
	Vid	是	int	万家用户唯一id，登陆后才有
	Ip	是	string	用户的IP信息
	imsi	否	string	imsi
	imei	否	string	手机串号
	vimei	否	string	vimei
	role_id	否	string	角色id
	role_name	否	string	角色名
	server_id	否	string	服务器id
	server_name	否	string	服务器名
	order_time	是	string	订单时间，格式：2018 - 05 - 30 10:05 : 23
	order_no	是	string	订单号
	pay	是	int	订单成功金额，单位：美分
	*/

	MyCardRechargeSuccessReportData &tReportData = pRecv->m_ReportData;

	// http://asuseraction.vanggame.com:33985
	CHttpParamStringMaker stringMaker;
	stringMaker.add_param("ip", tReportData.szIP);
	stringMaker.add_param("order_no", tReportData.szOrderNo);
	stringMaker.add_param("order_time", tReportData.szOrderTime);
	stringMaker.add_param("pay", tReportData.nPay);
	stringMaker.add_param("role_id", tReportData.dwRoleId);
	char szUTF8RoleName[MAX_ROLE_LEN+1] = { 0 };
	ChangeLocalChar(tReportData.szRoleName, szUTF8RoleName, sizeof(szUTF8RoleName));
	char szRoleName[MAX_ROLE_LEN * 2] = { 0 };
	UrlEncode(szUTF8RoleName, szRoleName, sizeof(szRoleName), TRUE);
	stringMaker.add_param("role_name", szRoleName);
	stringMaker.add_param("server_id", tReportData.dwServerId);
	stringMaker.add_param("vid", tReportData.dwVid);
	string strReadySignStr = stringMaker.get_params();

	//OPENLOG(GUI_LEVEL, "ReadySignStr[%s]", strReadySignStr.c_str());

 	char szReadySignStr[256] = { 0 };
	string strAlreadySignStr = MD5::Encode(strReadySignStr).HexString();

 	Json::Value root;
 	root["vid"] = int(tReportData.dwVid);
 	root["ip"] = tReportData.szIP;
 	root["role_id"] = g_IntToString(tReportData.dwRoleId);
	root["role_name"] = szRoleName;
 	root["server_id"] = g_IntToString(tReportData.dwServerId);
 	root["order_time"] = tReportData.szOrderTime;
 	root["order_no"] = tReportData.szOrderNo;
 	root["pay"] = g_IntToString(tReportData.nPay);
 	root["sign"] = strAlreadySignStr;
 	string strContext = root.toStyledString();

 	pQuery->SetIndex(HTTP_RECHARGE_SUCCESS_REPORT);
 	pQuery->SetRequestType(HTTP_REQUEST_POST);
 	pQuery->SetHost("asuseraction.vanggame.com");
 	pQuery->SetPort(33985);
 	pQuery->SetPath("/mycard_cb.php");
 	pQuery->SetContext(strContext.c_str());
 	pQuery->SetGUID(dwGuid);
 	g_OpenServer.HttpOpenQuery(pQuery);

	OPENLOG(GUI_LEVEL, "MyCard充值成功上报[%s]", strContext.c_str());
}

VOID HandlerFromMap::OnM2O_STATISTICS_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_STATISTICS_NOTIFY* pRecv = (MSG_M2O_STATISTICS_NOTIFY*)pMsg;

	HttpStatisticsReport* pQuery = HttpStatisticsReport::ALLOC();
	if (!pQuery)
	{
		OPENLOG(GUI_LEVEL, "资源不足,数据统计上报失败!");
		return;
	}

	// todo::暂时不上报
// 	if (!g_OpenServer.IsOpenStatistics())
// 		return;

	//string strAppKey = "20160602";
	DWORD dwAppKey = g_OpenServer.GetStatisticsAppKey();
	if (!dwAppKey)
		dwAppKey = 20160602;

	CHttpParamStringMaker stringMaker;
	stringMaker.add_param("app_key", dwAppKey);
	stringMaker.add_param("type", pRecv->m_wType);
	stringMaker.add_param("context", pRecv->m_szContext);
	string strContext = stringMaker.get_params();

	pQuery->SetIndex(HTTP_STATISTICS_REPORT);
	pQuery->SetRequestType(HTTP_REQUEST_POST);
// 	pQuery->SetHost("120.77.146.65");
// 	pQuery->SetPort(16888);
	pQuery->SetHost(g_OpenServer.GetStatisticsServerId());
	pQuery->SetPort(g_OpenServer.GetStatisticsServerPort());
	pQuery->SetPath("/statistics_from_server");
	pQuery->SetContext(strContext.c_str());
	g_OpenServer.HttpOpenQuery(pQuery);

	//OPENLOG(GUI_LEVEL, "context[%s]", strContext.c_str());
}

VOID HandlerFromMap::OnM2O_ONLINE_PLAYER_NUM_RESP(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_ONLINE_PLAYER_NUM_RESP* pRecv = (MSG_M2O_ONLINE_PLAYER_NUM_RESP*)pMsg;

	PHPSession *pRespSession = PHPSessionManager::Instance()->FindUserByGUID(dwGuid);
	if (!pRespSession)
	{
		OPENLOG(GUI_LEVEL, "找不到session[Guid:%d]!", dwGuid);
		return;
	}

// 	Json::Value root;
// 
// 	char szKey[32] = { 0 };
// 	for (WORD i = 0; i < pRecv->m_wServerNum; ++i)
// 	{
// 		Json::Value tme;
// 		sprintf_s(szKey, sizeof(szKey), "%d", pRecv->m_ServerIdList[i]);
// 		root[szKey] = int(pRecv->m_PlayerNumList[i]);
// 	}


	Json::Value server_list;

	char szKey[32] = { 0 };
	for (WORD i = 0; i < pRecv->m_wServerNum; ++i)
	{
		Json::Value server_data;
		server_data["id"] = pRecv->m_ServerIdList[i];
		server_data["num"] = pRecv->m_PlayerNumList[i];
		server_list[i] = server_data;
	}

	string strParams = server_list.toStyledString();

	CHttpResponseMaker resp_maker;
	char szRespBuff[10240] = { 0 };

	int nParamssLen = strParams.length();

	if (nParamssLen + 100 >= sizeof(szRespBuff))
		nParamssLen = sizeof(szRespBuff)-100;

	resp_maker.make(strParams.c_str(), nParamssLen, szRespBuff, sizeof(szRespBuff));
	pRespSession->PreSend((BYTE*)szRespBuff, strlen(szRespBuff));
}

// 通用消息
VOID HandlerFromMap::OnNetLogicCommon(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_M2O_LOGIC_COMMON_REQ* pRecv = (MSG_M2O_LOGIC_COMMON_REQ*)pMsg;


}


// 检测用户信息(新马港台澳)
// void HandlerFromMap::OnCheckUserInfo_XMGT(HttpCheckUser* pQuery, MSG_M2O_CHECK_USER_INFO_REQ* pRecv, DWORD dwGuid)
// {
// 	// http://xtaccount.vanggame.com:8080/api/verify.do
// 	CHttpParamStringMaker stringMaker;
// 	stringMaker.add_param("token", pRecv->m_szToken);
// 	string strContext = stringMaker.get_params();
// 	pQuery->SetIndex(HTTP_CHECK_USER_INFO);
// 	pQuery->SetRequestType(HTTP_REQUEST_POST);
// 	pQuery->SetHost("xtaccount.vanggame.com");
// 	pQuery->SetPort(8080);
// 	pQuery->SetPath("/api/verify.do");
// 	pQuery->SetContext(strContext.c_str());
// 	pQuery->SetGUID(dwGuid);
// 	pQuery->SetAccount(pRecv->m_szAccount);
// 	pQuery->SetToken(pRecv->m_szToken);
// 	pQuery->SetCPChannelType(pRecv->m_dwCPChannelType);
// 	g_OpenServer.HttpOpenQuery(pQuery);
// }


// 检测用户信息(通用)
// void HandlerFromMap::OnCheckUserInfo_Common(HttpCheckUser* pQuery, MSG_M2O_CHECK_USER_INFO_REQ* pRecv, DWORD dwGuid)
// {
//  	CHttpParamStringMaker stringMaker;
// 	stringMaker.add_param("terminal_type", pRecv->m_byTerminalType);
// 	stringMaker.add_param("channel_type", pRecv->m_dwCPChannelType);
// 	stringMaker.add_param("account", pRecv->m_szAccount);
// 	stringMaker.add_param("token", pRecv->m_szToken);
// 	stringMaker.add_param("ip", pRecv->m_szIP);
//  
//  	string strContext = stringMaker.get_params();
//  	pQuery->SetIndex(HTTP_CHECK_USER_INFO);
//  	pQuery->SetRequestType(HTTP_REQUEST_POST);
// 	pQuery->SetHost(g_OpenServer.GetNodeJsServerId());
// 	pQuery->SetPort(g_OpenServer.GetNodeJsServerPort());
//  	pQuery->SetPath("/check_user_info");
//  	pQuery->SetContext(strContext.c_str());
//  	pQuery->SetGUID(dwGuid);
//  	pQuery->SetAccount(pRecv->m_szAccount);
//  	pQuery->SetToken(pRecv->m_szToken);
// 	pQuery->SetCPChannelType(pRecv->m_dwCPChannelType);
// 	pQuery->SetSDKChannelType(pRecv->m_dwSDKChannelType);
// 	pQuery->SetTerminalType(pRecv->m_byTerminalType);
//  	g_OpenServer.HttpOpenQuery(pQuery);
// }
