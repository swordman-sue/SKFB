#pragma once

#include <windows.h>

#define MAX_PACK_SIZE 25600 // 原5120
enum OBJECT_TYTE
{
	OBJECT_TYTE_NONE,
	OBJECT_TYTE_SERVER,
	OBJECT_TYTE_USERSESSION,
	OBJECT_TYTE_USER,
};

class Session;

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
class NetworkObject
{
	friend class Session;
	friend class IoHandler;

public:
	NetworkObject();
	virtual ~NetworkObject();

	VOID			Disconnect( BOOL bGracefulDisconnect = TRUE );
	BOOL			Send( BYTE *pMsg, WORD wSize );
	BOOL			PreSend(BYTE* pBuff,int nBuffLen); // 直接发送不经缓存
	VOID			Redirect( NetworkObject *pNetworkObject );
	char*			GetIP();
	VOID			EnableSecurity();

	VOID			SetNeedHttp();
	SOCKET			GetSocket();
	SOCKADDR*		GetSockAddr();
	virtual int     GetObjectType(){return OBJECT_TYTE_NONE;}
	Session*		GetSession() { return m_pSession; };

	// 取发送\接收缓存数据长度
	DWORD					GetSendBufferDataLength();
	DWORD					GetRecvBufferDataLength();

	// 取发送\接收缓存大小
	DWORD					GetSendBufferSize();
	DWORD					GetRecvBufferSize();

private:
	virtual VOID	OnAccept( DWORD dwNetworkIndex ) {}
	virtual VOID	OnDisconnect() {}
	virtual	VOID	OnRecv( BYTE *pMsg, WORD wSize ) = 0;
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex ) {}
	virtual VOID	OnLogString( char *pszLog ) {}

	inline VOID		SetSession( Session *pSession ) { m_pSession = pSession; }


	Session			*m_pSession;
};
