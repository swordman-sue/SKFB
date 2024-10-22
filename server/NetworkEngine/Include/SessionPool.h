#pragma once

#include <windows.h>
#include <vector>

using namespace std;

class Session;
class Session;
class SessionList;

//=============================================================================================================================

//=============================================================================================================================
class SessionPool
{
public:
	SessionPool( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOutTick, DWORD dwIndexStart, BOOL bAcceptSocket, BOOL bSecurity );
	~SessionPool();

	Session*		Alloc();
	void			Free( Session* pSession );
	int				GetLength();

private:
	VOID			Create( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwTimeOutTick, DWORD dwMaxPacketSize, DWORD dwIndexStart, BOOL bAcceptSocket, BOOL bSecurity );

	DWORD			m_dwMaxSize;
	SessionList		*m_pList;
};
