#pragma once
#include "ServerSession.h"

class GateServerSession : public ServerSession
{
public:
	GateServerSession(void);
	virtual ~GateServerSession(void);

public:
	virtual SERVER_TYPE	GetServerType() { return GATE_SERVER;}
	virtual VOID		OnRecv( BYTE *pMsg, WORD wSize );
	virtual VOID		OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
	virtual VOID		OnDisconnect();
};
