#pragma once

class ServerSession;
class NetMsgBody;
class MSG_M2O_CHECK_USER_INFO_REQ;

class HandlerFromMap
{
public:
	HandlerFromMap(void);
	~HandlerFromMap(void);

public:
	//在此声明消息回调处理函数
	//static VOID OnMsg_ServerRegister_G2O(ServerSession * pSession, unsigned char *pMsg, WORD wSize, DWORD dwGuid);
	//static VOID OnMsg_Heart_Beat_G2O(ServerSession * pSession, unsigned char *pMsg, WORD wSize, DWORD dwGuid);

	static VOID OnMSG_M2O_SERVERID_CMD(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
	static VOID OnMSG_M2O_HEART_BEAT(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_AUTHENTICATION_CODE_REQ(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_REYUN_STATISTIC_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// 登录检测
	static VOID	OnM2O_CHECK_USER_INFO_REQ(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_RECHARGE_SEND_GOODS_RESP(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_YGAME_RECHARGE_SEND_GOODS_RESP(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);	

	static VOID	OnM2O_YGAME_CASHBACK_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_YGAME_BONUS_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_SDK_ORDER_ID_REQ(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);	

	static VOID	OnM2O_RECHARGE_SUCCESS_REPORT_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);	

	static VOID	OnM2O_STATISTICS_NOTIFY(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	static VOID	OnM2O_ONLINE_PLAYER_NUM_RESP(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);


public:

	// 逻辑通用消息
	static VOID OnNetLogicCommon(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);


protected:

	// 检测用户信息(新马港台澳)
	//static void OnCheckUserInfo_XMGT(HttpCheckUser* pQuery, MSG_M2O_CHECK_USER_INFO_REQ* pRecv, DWORD dwGuid);

	// 检测用户信息(通用)
	//static void OnCheckUserInfo_Common(HttpCheckUser* pQuery, MSG_M2O_CHECK_USER_INFO_REQ* pRecv, DWORD dwGuid);
};
