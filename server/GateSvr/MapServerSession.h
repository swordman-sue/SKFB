#pragma once

#include "ServerSession.h"


class MapServerSession : public ServerSession
{
public:
	MapServerSession(void);
	virtual ~MapServerSession(void);

public:
	virtual	VOID				OnRecv( BYTE *pMsg, WORD wSize );
	virtual SERVER_TYPE			GetServerType() { return MAP_SERVER;}
	virtual VOID				OnDisconnect();
	virtual VOID				OnAccept( DWORD dwNetworkIndex );
};
