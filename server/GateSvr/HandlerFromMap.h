#pragma once

class ServerSession;
class GateServerSession;
class NetMsgBody;

class HandlerFromMap
{
public:
	HandlerFromMap(void);
	~HandlerFromMap(void);

public:
	static VOID OnNetMessage(WORD dwMsgID, unsigned char * pData, WORD wDataSize, NetMsgBody *pMsg);

public:

	static VOID OnMSG_M2G_CLIENT_KICKOUT_CMD(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
	static VOID OnMSG_M2G_HEART_BEAT(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
	static VOID OnMSG_M2G_CLIENT_INFO(ServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid);
	
};
