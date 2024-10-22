#include "stdafx.h"
#include "PHPSession.h"
#include "ServerSessionManager.h"
#include "MapServerSession.h"
#include <json/reader.h>
#include <json/value.h>
#include "../Common/MsgDefine.h"
#include "../Common/Http_Util.h"
#include "json/writer.h"
#include "../Common/CommonLogic.h"
#include "PHPSessionManager.h"
#include "ServerSession.h"
#include "../Common/sdk/QuickSDK/QuickEncrypt.h"
#include "../ToolKit/MD5.h"
#include "../ToolKit/xml/tinyxml.h"

PHPSession::PHPSession(void)
{

}

PHPSession::~PHPSession(void)
{

}

//接收消息事件
VOID PHPSession::OnRecv( BYTE *pMsg, WORD wSize )
{
	if (wSize >= 4096)
	{
		OPENLOG(ERROR_LEVEL, "Web服务器收到请求的数据过大[size:%d]", wSize);
		return;
	}

	const char *pszIP = GetIP();

	//处理来自渠道的发货请求
	char szMsgBuf[4096] = {0};
	memcpy(szMsgBuf,pMsg,wSize);
	szMsgBuf[wSize] = '\0';
	CHttpParser HttpParse(szMsgBuf, wSize);
	string szUrl = HttpParse.get_uri();
	string strParam = HttpParse.get_param_string();
	CHttpParamParser ParamParser;
	ParamParser.parse(strParam.c_str(), (int)strParam.size());

	string strMsgId = ParamParser.get_param("msg_id");
// 	if (!strMsgId.size())
// 	{
// 		OPENLOG(GUI_LEVEL, "Web服务器收到请求非法的请求[url:%s]");
// 		return;
// 	}
	
	DWORD dwMsgId = atoi(strMsgId.c_str());

	OPENLOG(GUI_LEVEL, "Web服务器收到请求事件[url:%s,msg:%d,size:%d]", szUrl.c_str(), dwMsgId, wSize);

	// android
	if (strcmp(szUrl.c_str(), "/recharge_callback_quicksdk_android") == 0)
	{
		OnQuickSDKRechargeCallback_Android(&HttpParse);
	}
	// ios
	else if (strcmp(szUrl.c_str(), "/recharge_callback_quicksdk_ios") == 0)
	{
		OnQuickSDKRechargeCallback_Ios(&HttpParse);
	}
	// 通用充值回调
	else if (strcmp(szUrl.c_str(), "/recharge_callback_common") == 0)
	{
		OnCommonRechargeCallback(&HttpParse);
	}
	// 云顶ios
	else if (strcmp(szUrl.c_str(), "/recharge_callback_yd_ios") == 0)
	{
		OnYDRechargeCallback_Ios(&HttpParse);
	}
	// 压力测试接口
	else if (strcmp(szUrl.c_str(), "/web_server_test") == 0)
	{
		CHttpResponseMaker resp_maker;
		char szRespBuff[256] = { 0 };
		resp_maker.make("SUCCESS", 7, szRespBuff, sizeof(szRespBuff));
		PreSend((BYTE*)szRespBuff, strlen(szRespBuff));

		OPENLOG(ERROR_LEVEL, "收到压力测试包[url:%s,ip:%s]", szUrl.c_str(), GetIP());
	}
	// 在线玩家数量
	else if (strcmp(szUrl.c_str(), "/online_player_num") == 0)
	{
		MSG_O2M_ONLINE_PLAYER_NUM_REQ msg;
		g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);
	}
	// 发放礼包通知
	else if (strcmp(szUrl.c_str(), "/send_gift_notify") == 0)
	{

	}
	// YGame充值回调
	else if (strcmp(szUrl.c_str(), "/ygame_recharge_callback") == 0)
	{
		OnYGameRechargeCallback(&HttpParse);
	}
	// YGame cashback coin to user
	else if (strcmp(szUrl.c_str(), "/ygame_cashback_notify") == 0)
	{
		OnYGameCashbackNotify(&HttpParse);
	}
	// YGame bonus in game
	else if (strcmp(szUrl.c_str(), "/ygame_bonus_notify") == 0)
	{
		OnYGameBonusNotify(&HttpParse);
	}
	else
	{
		OPENLOG(GUI_LEVEL, "Web服务器收到无法解析的请求事件[url:%s,msg_id:%d]", szUrl.c_str(),dwMsgId);
		return;
	}

}

//断开连接事件
VOID PHPSession::OnDisconnect()
{
	//调用基类版本
	ServerSession::OnDisconnect();

	//删除玩家
	PHPSessionManager::Instance()->RemoveUser(GetGUID());
}

//接受接入事件
VOID PHPSession::OnAccept( DWORD dwNetworkIndex )
{
	ServerSession::OnAccept(dwNetworkIndex);
	PHPSessionManager::Instance()->AddUser(this);

	//开启腾讯接入
	SetNeedHttp();

	m_tCreateTime = time(NULL);
}

// quicksdk充值回调
void PHPSession::OnQuickSDKRechargeCallback_Android(CHttpParser *pHttpParser)
{
	string strMd5Key = g_OpenServer.GetQuickSDKMd5Key();
	string strCallbackKey = g_OpenServer.GetQuickSDKCallbackKey();

	DoQuickRechargeCallback(pHttpParser, strMd5Key, strCallbackKey);
}

// quicksdk充值回调
void PHPSession::OnQuickSDKRechargeCallback_Ios(CHttpParser *pHttpParser)
{
	string strMd5Key = g_OpenServer.GetQuickSDKMd5KeyIOS();
	string strCallbackKey = g_OpenServer.GetQuickSDKCallbackKeyIOS();

	DoQuickRechargeCallback(pHttpParser, strMd5Key, strCallbackKey);
}

// 云顶_ios 充值回调
void PHPSession::OnYDRechargeCallback_Ios(CHttpParser *pHttpParser)
{
	string strYDOrderId		= pHttpParser->get_param("yd_order_id");
	string strYDUserId		= pHttpParser->get_param("yd_user_id");
	string strAppRoleId		= pHttpParser->get_param("app_role_id");
	string strServerId		= pHttpParser->get_param("server_id");
	string strPrice			= pHttpParser->get_param("price");
	string strGoodsCode		= pHttpParser->get_param("goods_code");
	string strPayTime		= pHttpParser->get_param("pay_time");
	string strIP			= pHttpParser->get_param("ip");
	string strCPOrderId		= pHttpParser->get_param("cp_order_id");
	string strRechargeId	= pHttpParser->get_param("recharge_id");
	string strRechargeType	= pHttpParser->get_param("recharge_type");
	string strYDChannelId = pHttpParser->get_param("yd_channel_id");


	RechargeCallbackData callbackData;
	memset(&callbackData, 0, sizeof(callbackData));
	callbackData.byStatus = 0;
	callbackData.nAmount = atoi(strPrice.c_str());
	callbackData.byIsTest = 0; 
	callbackData.dwChannel = CHANNEL_IOS_YD;
	callbackData.dwRoleId = atoi(strAppRoleId.c_str());
	callbackData.wRechargeId = atoi(strRechargeId.c_str());
	callbackData.wRechargeType = atoi(strRechargeType.c_str());
	callbackData.wGoodsId = atoi(strGoodsCode.c_str());
	callbackData.dwCPOrderId = atoi(strCPOrderId.c_str());
	callbackData.dwYDChannelId = atoi(strYDChannelId.c_str());
	callbackData.wCurrencyType = 0;
	callbackData.byIsCheckRechargeStrId = 0;
	callbackData.byIsNotCheck = 0;
	strcpy_s(callbackData.szPayTime, sizeof(callbackData.szPayTime), strPayTime.c_str());
	strcpy_s(callbackData.szIP, sizeof(callbackData.szIP), strIP.c_str());
	strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), strYDOrderId.c_str());
	

	MSG_O2M_RECHARGE_SEND_GOODS_REQ msg;
	msg.m_CallbackData = callbackData;
	g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);

	OPENLOG(GUI_LEVEL, "[%s]转发quick充值通知发货消息给gameserver[role_id:%d,recharge_id:%d,channel:%d,SDKOrderId:%s]",
		__FUNCTION__, callbackData.dwRoleId, callbackData.wRechargeId, callbackData.dwChannel, callbackData.szSDKOrderId);
}

// 通用充值回调
void PHPSession::OnCommonRechargeCallback(CHttpParser *pHttpParser)
{
	DoCommonRechargeCallback(pHttpParser);
}

// YGame充值回调
void PHPSession::OnYGameRechargeCallback(CHttpParser *pHttpParser)
{
	// call_game_url+'?amount='+amount+'&char_id='+char_id+'&server_id='+server_id+'&payment_id='+payment_id;	
	string strAmount	= pHttpParser->get_param("amount");		// sdk定单ID
	string strRoleId = pHttpParser->get_param("char_id");		// 角色ID
	string strServerId  = pHttpParser->get_param("server_id");	// 服务器ID
	string strPaymentId = pHttpParser->get_param("payment_id");	// 支付ID

	WORD wServerId = atoi(strServerId.c_str());
	DWORD dwRoleId = atoi(strRoleId.c_str());
	int nAmount = atoi(strAmount.c_str());

	MSG_O2M_YGAME_RECHARGE_SEND_GOODS_REQ msg;
	msg.m_wServerId = wServerId;
	msg.m_dwRoleId = dwRoleId;
	msg.m_nAmount = nAmount;
	msg.m_nDiamond = 0;
	strcpy_s(msg.m_szPaymentId, sizeof(msg.m_szPaymentId), strPaymentId.c_str());
	g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);

	OPENLOG(GUI_LEVEL, "[%s]Recv ygame recharge[server_id:%d,role_id:%d,amount:%d]",__FUNCTION__, wServerId, dwRoleId, nAmount);
}

// YGame cashback coin to user
void PHPSession::OnYGameCashbackNotify(CHttpParser *pHttpParser)
{
	string strRoleId = pHttpParser->get_param("char_id");		// 角色ID
	string strServerId = pHttpParser->get_param("server_id");	// 服务器ID
	string strDiamond = pHttpParser->get_param("diamond");		// 钻石
	string strYCoin = pHttpParser->get_param("ycoin");		// ycoin

	WORD wServerId = atoi(strServerId.c_str());
	DWORD dwRoleId = atoi(strRoleId.c_str());
	DWORD dwDiamond = atoi(strDiamond.c_str());
	int nYCoin = atoi(strYCoin.c_str());

	MSG_O2M_YGAME_CASHBACK_NOTIFY_REQ msg;
	msg.m_dwRoleId = dwRoleId;
	msg.m_wServerId = wServerId;
	msg.m_nDiamond = dwDiamond;
	msg.m_nYcoin = nYCoin;
	g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);
	OPENLOG(GUI_LEVEL, "[%s]Recv ygame cashback notify[server_id:%d,role_id:%d,ycoin:%d,diamond:%d]", __FUNCTION__, wServerId, dwRoleId, nYCoin, dwDiamond);
}

// YGame bonus in game
void PHPSession::OnYGameBonusNotify(CHttpParser *pHttpParser)
{
	string strRoleId = pHttpParser->get_param("char_id");		// 角色ID
	string strServerId = pHttpParser->get_param("server_id");	// 服务器ID
	string strDiamond = pHttpParser->get_param("diamond");		// 钻石
	
	WORD wServerId = atoi(strServerId.c_str());
	DWORD dwRoleId = atoi(strRoleId.c_str());
	DWORD dwDiamond = atoi(strDiamond.c_str());

	MSG_O2M_YGAME_BONUS_NOTIFY_REQ msg;
	msg.m_dwRoleId = dwRoleId;
	msg.m_wServerId = wServerId;
	msg.m_dwDiamond = dwDiamond;
	g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);
	OPENLOG(GUI_LEVEL, "[%s]Recv ygame cashback notify[server_id:%d,role_id:%d,diamond:%d]", __FUNCTION__, wServerId, dwRoleId, dwDiamond);
}

void PHPSession::DoQuickRechargeCallback(CHttpParser *pHttpParser, string strMd5Key, string strCallbackKey)
{
	// 检测黑白名单 check(ip) todo::不是必需的

	// 解析参数 parse(nt_data,sign,md5Sign)
	string strSrcNtData = pHttpParser->get_param("nt_data");
	string strSrcSign = pHttpParser->get_param("sign");
	string strSrcMd5Sign = pHttpParser->get_param("md5Sign");


	// 校验签名 md5(nt_data+sign+md5key)
	string strData = strSrcNtData + strSrcSign + strMd5Key;
	MD5::Digest md5_sign_local = MD5::Encode(strData);
	string stMd5SignLocal = md5_sign_local.HexString();
	if (strcmp(stMd5SignLocal.c_str(), strSrcMd5Sign.c_str()) != 0)
	{
		OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值回调签名出错[local:%s,quicksdk:%s]", __FUNCTION__,
			stMd5SignLocal.c_str(), strSrcMd5Sign.c_str());
		return;
	}

	// 解密参数 decode(nt_data)
	//string strCallbackKey = g_OpenServer.GetQuickSDKCallbackKey();
	QuickEncrypt quickEncrypt;
	string strNtData = quickEncrypt.decode(strSrcNtData, strCallbackKey);

	OPENLOG(ERROR_LEVEL, "[%s]quick_sdk充值回调nt_data内容[%s]", __FUNCTION__, strNtData.c_str());

	RechargeCallbackData callbackData;
	memset(&callbackData, 0, sizeof(callbackData));
	callbackData.byStatus = 2; // todo::不太记得为啥会这么写,也许是当时手误滴!
	callbackData.byIsCheckRechargeStrId = 0;
	callbackData.byIsNotCheck = 0;

	TiXmlDocument doc("recharge.xml");
	doc.Parse(strNtData.c_str());
	TiXmlElement* quicksdkMessage = doc.FirstChildElement("quicksdk_message");
	if (quicksdkMessage)
	{
		TiXmlElement* message = quicksdkMessage->FirstChildElement("message");
		if (message)
		{
			TiXmlElement *is_text = message->FirstChildElement("is_test");
			TiXmlElement *channel = message->FirstChildElement("channel");
			TiXmlElement *channel_uid = message->FirstChildElement("channel_uid");
			TiXmlElement *game_order = message->FirstChildElement("game_order");
			TiXmlElement *order_no = message->FirstChildElement("order_no");
			TiXmlElement *pay_time = message->FirstChildElement("pay_time");
			TiXmlElement *amount = message->FirstChildElement("amount");
			TiXmlElement *status = message->FirstChildElement("status");
			TiXmlElement *extras_params = message->FirstChildElement("extras_params");

			if (is_text && is_text->GetText())
				callbackData.byIsTest = atoi(is_text->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析is_test字段无效", __FUNCTION__);

			if (channel && channel->GetText())
				callbackData.dwChannel = atoi(channel->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析channel字段无效", __FUNCTION__);

			if (channel_uid && channel_uid->GetText())
				strcpy_s(callbackData.szChannelUid, sizeof(callbackData.szChannelUid), channel_uid->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析channel_uid字段无效", __FUNCTION__);

			if (game_order && game_order->GetText())
				callbackData.dwCPOrderId = atoi(game_order->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析game_order字段无效", __FUNCTION__);

			if (order_no && order_no->GetText())
				strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), order_no->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析order_no字段无效", __FUNCTION__);

			if (pay_time && pay_time->GetText())
				strcpy_s(callbackData.szPayTime, sizeof(callbackData.szPayTime), pay_time->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析pay_time字段无效", __FUNCTION__);

			if (amount && amount->GetText())
				callbackData.nAmount = atoi(amount->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析amount字段无效", __FUNCTION__);

			if (status && status->GetText())
				callbackData.byStatus = atoi(status->GetText());
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析status字段无效", __FUNCTION__);

			if (extras_params && extras_params->GetText())
			{
				string strExtrasParams = extras_params->GetText();

				OPENLOG(ERROR_LEVEL, "[%s]extras_params字段内容[%s]", __FUNCTION__, strExtrasParams.c_str());

				Json::Reader reader;
				Json::Value json_object;
				if (!reader.parse(strExtrasParams.c_str(), json_object))
				{
					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params字段出错");
					return;
				}

				// role_id
				Json::Value json_role_id = json_object["role_id"];
				if (json_role_id.isNull() || !json_role_id.isInt())
				{
					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params_role_id字段参数出错!");
					return;
				}

				// recharge_id
				Json::Value json_recharge_id = json_object["recharge_id"];
				if (json_recharge_id.isNull() || !json_recharge_id.isInt())
				{
					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params_recharge_id字段参数出错!");
					return;
				}

				// recharge_type
				Json::Value json_recharge_type = json_object["recharge_type"];
				if (json_recharge_type.isNull() || !json_recharge_type.isInt())
				{
					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params_recharge_type字段参数出错!");
					return;
				}

				// goods_id
				Json::Value json_goods_id = json_object["goods_id"];
				if (json_goods_id.isNull() || !json_goods_id.isInt())
				{
					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params_goods_id字段参数出错!");
					return;
				}

				// yd_channel_id
// 				Json::Value json_yd_channel_id = json_object["channel_type"];
// 				if (json_yd_channel_id.isNull() || !json_yd_channel_id.isInt())
// 				{
// 					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params_yd_channel_id字段参数出错!");
// 					return;
// 				}

				// ip
				Json::Value json_ip = json_object["ip"];
				if (json_ip.isNull() || !json_ip.isString())
				{
					OPENLOG(ERROR_LEVEL, "解析sdk_extras_params_ip字段参数出错!");
					return;
				}

				callbackData.dwRoleId = json_role_id.asInt();
				callbackData.wRechargeId = json_recharge_id.asInt();
				callbackData.wRechargeType = json_recharge_type.asInt();
				callbackData.wGoodsId = json_goods_id.asInt();
				callbackData.dwYDChannelId = 0; // json_yd_channel_id.asInt();
				strcpy_s(callbackData.szIP, sizeof(callbackData.szIP), json_ip.asString().c_str());

				OPENLOG(ERROR_LEVEL, "extras_params字段内容[role_id:%d, recharge_id:%d, recharge_type:%d,ip:%s]",
					callbackData.dwRoleId, callbackData.wRechargeId, callbackData.wRechargeType, callbackData.szIP);
			}
			else
				OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析extras_params字段无效", __FUNCTION__);
		}
		else
		{
			OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析message节点无效", __FUNCTION__);
		}
	}
	else
	{
		OPENLOG(ERROR_LEVEL, "[%s]quicksdk充值通知解析quicksdk_message节点无效", __FUNCTION__);
	}

	// 0是成功 如果充值失败时直接回复，不需要通知game_server
	if (0 != callbackData.byStatus)
	{
		OPENLOG(ERROR_LEVEL, "[%s]quick_sdk充值失败通知[role_id:%d,recharge_id:%d,channel:%d,SDKOrderId:%s,status:%d]",
			__FUNCTION__, callbackData.dwRoleId, callbackData.wRechargeId, callbackData.dwChannel, callbackData.szSDKOrderId, callbackData.byStatus);

		CHttpResponseMaker resp_maker;
		char szRespBuff[256] = { 0 };
		resp_maker.make("SUCCESS", 7, szRespBuff, sizeof(szRespBuff));
		PreSend((BYTE*)szRespBuff, strlen(szRespBuff));
	}
	else
	{
		MSG_O2M_RECHARGE_SEND_GOODS_REQ msg;
		msg.m_CallbackData = callbackData;
		g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);

		OPENLOG(GUI_LEVEL, "[%s]转发quick充值通知发货消息给gameserver[role_id:%d,recharge_id:%d,channel:%d,SDKOrderId:%s]",
			__FUNCTION__, callbackData.dwRoleId, callbackData.wRechargeId, callbackData.dwChannel, callbackData.szSDKOrderId);
	}
}

// 处理通用充值回调
void PHPSession::DoCommonRechargeCallback(CHttpParser *pHttpParser)
{
	string strSDKOrderId = pHttpParser->get_param("sdk_order_id");		// sdk定单ID
	//string strSDKUserId = pHttpParser->get_param("sdk_user_id");	
	string strAppRoleId = pHttpParser->get_param("app_role_id");		// 角色ID
	//string strServerId = pHttpParser->get_param("server_id");
	string strPrice = pHttpParser->get_param("price");					// 发货金额（分）
	string strGoodsId = pHttpParser->get_param("goods_id");			// 商品ID
	string strPayTime = pHttpParser->get_param("pay_time");				// 支付时间
	string strIP = pHttpParser->get_param("ip");						// 支付的客户端地址
	string strCPOrderId = pHttpParser->get_param("cp_order_id");		// CP的定单ID
	string strRechargeId = pHttpParser->get_param("recharge_id");		// 充值ID
	string strRechargeType = pHttpParser->get_param("recharge_type");	// 充值类型
	string strYDChannelId = pHttpParser->get_param("yd_channel_id");	// 云顶渠道ID
	string strCPChannelType = pHttpParser->get_param("cp_channel_type");		// CP自定义的渠道类型
	string strCurrencyType = pHttpParser->get_param("currency_type");		// 货币类型
	string strIsMyCard = pHttpParser->get_param("is_my_card");				// 是否mycard充值
	string strIsCheckRechargeStrId = pHttpParser->get_param("is_check_recharge_str_id");	// 是否检测充值字串ID
	string strRechargeStrId = pHttpParser->get_param("recharge_str_id");	// 充值字串ID
	string strIsNotCheck = pHttpParser->get_param("is_not_check");	// 是否免检检测

	RechargeCallbackData callbackData;
	memset(&callbackData, 0, sizeof(callbackData));
	callbackData.byStatus = 0;
	callbackData.nAmount = atoi(strPrice.c_str());
	callbackData.byIsTest = 0;
	callbackData.dwChannel = atoi(strCPChannelType.c_str());;
	callbackData.dwRoleId = atoi(strAppRoleId.c_str());
	callbackData.wRechargeId = atoi(strRechargeId.c_str());
	callbackData.wRechargeType = atoi(strRechargeType.c_str());
	callbackData.wGoodsId = atoi(strGoodsId.c_str());
	callbackData.dwCPOrderId = atoi(strCPOrderId.c_str());
	callbackData.dwYDChannelId = atoi(strYDChannelId.c_str());
	callbackData.wCurrencyType = atoi(strCurrencyType.c_str());
	callbackData.byIsMyCard = atoi(strIsMyCard.c_str());
	callbackData.byIsCheckRechargeStrId = atoi(strIsCheckRechargeStrId.c_str());
	callbackData.byIsNotCheck = atoi(strIsNotCheck.c_str());
	strcpy_s(callbackData.szPayTime, sizeof(callbackData.szPayTime), strPayTime.c_str());
	strcpy_s(callbackData.szIP, sizeof(callbackData.szIP), strIP.c_str());
	strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), strSDKOrderId.c_str());
	strcpy_s(callbackData.szRechargeStrId, sizeof(callbackData.szRechargeStrId), strRechargeStrId.c_str());


	MSG_O2M_RECHARGE_SEND_GOODS_REQ msg;
	msg.m_CallbackData = callbackData;
	g_OpenServer.SendMsgToMapServer(GetGUID(), &msg);

	OPENLOG(GUI_LEVEL, "[%s]转发充值通知发货消息给gameserver[role_id:%d,recharge_id:%d,cp_channel_type:%d,SDKOrderId:%s,is_check_str_id:%d]",
		__FUNCTION__, callbackData.dwRoleId, callbackData.wRechargeId, callbackData.dwChannel, 
		callbackData.szSDKOrderId, callbackData.byIsCheckRechargeStrId);
}