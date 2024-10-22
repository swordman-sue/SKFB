#pragma once
#include "NetworkObject.h"
#include "../Common/Const.h"

class NetMsgBody;

class ServerSession : public NetworkObject
{
public:
	ServerSession(void);
	virtual ~ServerSession(void);

public:
	VOID						Update();

public:
	virtual int					GetObjectType(){return OBJECT_TYTE_SERVER;}
	virtual VOID				OnAccept( DWORD dwNetworkIndex );
	virtual VOID				OnDisconnect();
	virtual	VOID				OnRecv( BYTE *pMsg, WORD wSize ){}
	virtual VOID				OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );

	inline VOID					SetSessionIndex( DWORD dwIdx ) { m_dwSessionIndex = dwIdx; }
	inline DWORD				GetSessionIndex() { return m_dwSessionIndex; }

	BOOL						SendPacket(DWORD dwGUID, WORD dwMsgID, NetMsgBody * pMsg, DWORD dwSeriesId = 0);
	BOOL						SendPacket(DWORD dwGUID, WORD dwMsgID, unsigned char * pData, WORD wDataSize, DWORD dwSeriesId = 0);

	virtual SERVER_TYPE			GetServerType() { return UNKNOWN_SERVER;}

	VOID						TryToConnect();

	BOOL						IsConnected() { return m_bIsConnected;	}

	virtual VOID				Init();
	virtual VOID				Release();

private:
	VOID						_setConnected( BOOL val ) { m_bIsConnected = val; }
	DWORD						m_dwSessionIndex;
	BOOL						m_bIsConnected;
};
