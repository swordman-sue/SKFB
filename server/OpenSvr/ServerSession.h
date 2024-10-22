#pragma once
#include "NetworkObject.h"
#include "../Common/Const.h"
#include "server.pb.h"

class NetMsgBody;
class google::protobuf::Message;

//服务器会话类
class ServerSession : public NetworkObject
{
public:
	ServerSession(void);
	virtual ~ServerSession(void);

public:
	VOID				Update();

public:
	virtual VOID		OnAccept( DWORD dwNetworkIndex );
	virtual VOID		OnDisconnect();
	virtual	VOID		OnRecv( BYTE *pMsg, WORD wSize ){}
	virtual VOID		OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );

	inline VOID			SetSessionIndex( DWORD dwIdx ) { m_dwSessionIndex = dwIdx; }
	inline DWORD		GetSessionIndex() { return m_dwSessionIndex; }

	BOOL				SendPacket( DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg);
	BOOL				SendPacket( DWORD dwGUID, WORD wMsgID, unsigned char * pData, WORD wDataSize );

	//BOOL				SendPacket(DWORD dwGUID, google::protobuf::Message *pMsg);
	//BOOL				SendPacket(unsigned char * pData, WORD wDataSize );

	// 直接发送不经缓存
	BOOL				PreSendPacket(BYTE* pBuff,int nBuffLen);

	virtual SERVER_TYPE	GetServerType() { return UNKNOWN_SERVER;}

	VOID				TryToConnect();

	BOOL				IsConnected() { return m_bIsConnected;	}

	virtual VOID		Init();
	virtual VOID		Release();

	DWORD				GetGUID() { return m_dwGUID; }
	VOID				SetGUID(DWORD dwGUID) { m_dwGUID = dwGUID; }
private:
	VOID				_setConnected( BOOL val ) { m_bIsConnected = val; }
	DWORD				m_dwSessionIndex;
	BOOL				m_bIsConnected;

	DWORD				m_dwGUID;
};
