#pragma once

class OpenServerSession;
class NetMsgBody;

class HandlerFromOpen
{
public:
	HandlerFromOpen(void);
	~HandlerFromOpen(void);

	static VOID OnMSG_O2M_HEART_BEAT(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID OnMSG_O2M_AUTHENTICATION_CODE(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// 检测用户信息响应
	static VOID OnMsgCheckUserInfoResp(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// 充值发货请求
	static VOID OnMsgRechargeSendGoodsReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	//此处添加消息处理函数
	static VOID OnMsg_LogicCommon_O2M(OpenServerSession * pSession, unsigned char *pMsg, DWORD dwGuid, WORD wSize);

	// CP订单号请求
	static VOID OnNetCPOrderId(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// 在线玩家数量
	static VOID OnNetOnLinePlayerNum(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// YGame充值发货请求
	static VOID OnMsgYameRechargeSendGoodsReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// YGame提现通知请求
	static VOID OnMsgYameCashbackNotifyReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// YGame奖励通知请求
	static VOID OnMsgYameBonusNotifyReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
};
