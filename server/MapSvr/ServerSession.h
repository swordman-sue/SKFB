#pragma once
#include "NetworkObject.h"
#include "../Common/Const.h"
#include "google/protobuf/message_lite.h"
#include "google/protobuf/message.h"

class NetMsgBody;
class google::protobuf::Message;

//服务器会话类
class ServerSession : public NetworkObject
{
public:
	ServerSession(void);
	virtual ~ServerSession(void);

public:
	virtual VOID		Init();
	virtual VOID		Release();
	VOID				Update();

	virtual VOID		OnAccept( DWORD dwNetworkIndex );
	virtual VOID		OnDisconnect();
	virtual	VOID		OnRecv( BYTE *pMsg, WORD wSize ){}
	virtual VOID		OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );

	inline VOID			SetSessionIndex( DWORD dwIdx ) { m_dwSessionIndex = dwIdx; }
	inline DWORD		GetSessionIndex() { return m_dwSessionIndex; }

	BOOL				SendPacket(DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg);
	BOOL				SendPacket(DWORD dwGUID, WORD wMsgID, unsigned char * pData, WORD wDataSize);

	virtual SERVER_TYPE	GetServerType() { return UNKNOWN_SERVER;}
	VOID				SetIPPort(char* pszIP,WORD wPort) { strncpy(m_szServerIP,pszIP,sizeof(m_szServerIP)); m_wPort = wPort; }
	VOID				TryToConnect();
	BOOL				IsConnected() { return m_bIsConnected;	}
	VOID				Clone( ServerSession * pSession );

private:
	inline VOID			_setIsConnector( BOOL bIsConnector ) { m_bIsConnector = bIsConnector; }
	inline BOOL			_isConnector() { return m_bIsConnector; }
	VOID				_setConnected( BOOL val ) { m_bIsConnected = val; }

private:
	DWORD				m_dwSessionIndex;
	BOOL				m_bIsConnector;
	BOOL				m_bIsTriedToConnect;
	BOOL				m_bIsConnected;
	char				m_szServerIP[IP_LEN];
	WORD				m_wPort;
};
