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

	// ����û���Ϣ��Ӧ
	static VOID OnMsgCheckUserInfoResp(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// ��ֵ��������
	static VOID OnMsgRechargeSendGoodsReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	//�˴������Ϣ������
	static VOID OnMsg_LogicCommon_O2M(OpenServerSession * pSession, unsigned char *pMsg, DWORD dwGuid, WORD wSize);

	// CP����������
	static VOID OnNetCPOrderId(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// �����������
	static VOID OnNetOnLinePlayerNum(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// YGame��ֵ��������
	static VOID OnMsgYameRechargeSendGoodsReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// YGame����֪ͨ����
	static VOID OnMsgYameCashbackNotifyReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);

	// YGame����֪ͨ����
	static VOID OnMsgYameBonusNotifyReq(OpenServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
};
