#pragma once
#include "ServerSession.h"

class OpenServerSession : public ServerSession
{
public:
	OpenServerSession(void);
	virtual ~OpenServerSession(void);

public:
	virtual SERVER_TYPE	GetServerType() { return OPEN_SERVER;}
	virtual VOID		OnRecv( BYTE *pMsg, WORD wSize );
	virtual VOID		OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
	virtual VOID		OnDisconnect();
};
