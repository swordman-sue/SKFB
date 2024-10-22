#pragma once

#include <windows.h>
#include <WinSock2.h>

#define SEND_POSTED			1
#define RECV_POSTED			2
#define ACCEPT_POSTED		3
#define DISCONNECT_POSTED	4

class SendBuffer;
class RecvBuffer;
class SessionPool;
class NetworkObject;
class Session;
class RC4Encry;

// OVERLAPPED
struct OVERLAPPEDEX : public OVERLAPPED
{
	DWORD		dwIoSize;
	DWORD		dwFlags;
	DWORD		dwOperationType;
	Session*	pSession;
};

//=============================================================================================================================

//=============================================================================================================================
class Session
{
	friend unsigned __stdcall io_thread( LPVOID param );
	friend class SessionPool;

public:
	
	Session( DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOut, BOOL bSecurity );
	virtual ~Session();

	VOID					Init();
	VOID					Reuse( HANDLE hIOCP );
	BOOL					Send( BYTE *pMsg, WORD wSize );
	BOOL					PreSend(BYTE* pBuff,int nBuffLen); // 直接发送不经缓存
	BOOL					PreSend();
	BOOL					PreRecv();
	BOOL					PreAccept( SOCKET listenSocket );
	SOCKET					CreateSocket();
	BOOL					ProcessRecvdPacket( DWORD dwMaxPacketSize );
	VOID					BindNetworkObject( NetworkObject *pNetworkObject );
	VOID					UnbindNetworkObject();
	VOID					OnAccept();
	VOID					OnConnect( BOOL bSuccess );
	VOID					OnLogString( char *pszLog, ... );

	inline VOID				SetSocket( SOCKET socket ) { m_socket = socket; }
	inline VOID				SetSockAddr( SOCKADDR_IN& sockaddr ) { m_sockaddr = sockaddr; }
	inline VOID				CloseSocket() { closesocket( m_socket ); m_socket = INVALID_SOCKET; }
	inline NetworkObject*	GetNetworkObject() { return m_pNetworkObject; }
	inline SendBuffer*		GetSendBuffer() { return m_pSendBuffer; }				
	inline RecvBuffer*		GetRecvBuffer() { return m_pRecvBuffer; }				
	inline SOCKET			GetSocket()		{ return m_socket; }					
	inline SOCKADDR*		GetSockAddr()	{ return (SOCKADDR*)&m_sockaddr; }	
	inline char*			GetIP() { return inet_ntoa( m_sockaddr.sin_addr ); }

	/*

	inline DWORD			GetIdleTick() const	{ return m_dwTimeOut? m_dwLastSyncTick + m_dwTimeOut : 0; }
	inline BOOL				IsOnIdle()
	{ return m_dwTimeOut ? m_dwLastSyncTick + m_dwTimeOut < GetTickCount() : FALSE; }

	*/

	inline DWORD			GetIndex() { return m_dwIndex; }
	inline BOOL				IsAcceptSocket() { return m_bAcceptSocket; }
	inline VOID				SetAcceptSocketFlag() { m_bAcceptSocket = TRUE; }
	VOID					Remove() { InterlockedExchange( (long*)&m_bRemove, TRUE ); }
	inline VOID				ResetKillFlag() { InterlockedExchange( (long *)&m_bRemove, FALSE ); }
	inline BOOL				ShouldBeRemoved() { return m_bRemove; }
	VOID					Disconnect( BOOL bGracefulDisconnect );
	inline BOOL				HasDisconnectOrdered() { return m_bDisconnectOrdered; }

	//腾讯接入
	BOOL					IsNeedHttp() { return m_bNeedHttp; }
	VOID					SetNeedHttp() { m_bNeedHttp = TRUE; }

	BOOL					IsHttpOk() { return m_bHttpOk; }
	VOID					SetHttpOk() { m_bHttpOk = TRUE; }

	// 取发送\接收缓存数据长度
	DWORD					GetSendBufferDataLength();
	DWORD					GetRecvBufferDataLength();

	// 取发送\接收缓存大小
	DWORD					GetSendBufferSize();
	DWORD					GetRecvBufferSize();

private:
	VOID					SetIndex( DWORD index ) { m_dwIndex = index; }
	//inline VOID				ResetTimeOut() { m_dwLastSyncTick = GetTickCount(); }

	NetworkObject			*m_pNetworkObject;
	SendBuffer				*m_pSendBuffer;
	RecvBuffer				*m_pRecvBuffer;

	SOCKET					m_socket;
	SOCKADDR_IN				m_sockaddr;

	OVERLAPPEDEX			m_sendIoData;
	OVERLAPPEDEX			m_recvIoData;
	OVERLAPPEDEX			m_acceptIoData;
	OVERLAPPEDEX			m_disconnectIoData;
	//DWORD					m_dwLastSyncTick;
	BOOL					m_bRemove;
	//DWORD					m_dwTimeOut;
	DWORD					m_dwIndex;
	BOOL					m_bAcceptSocket;
	BOOL					m_bDisconnectOrdered;
	WORD					m_nRemainSize;

private:
	//腾讯接入HTTP包头解析
	BOOL					m_bNeedHttp;			//是否需要Http解析
	BOOL					m_bHttpOk;				//Http解析是否完成

	BOOL					ProcessHttpPacket( DWORD dwMaxPacketSize );
	BOOL					ProcessNormalPacket( DWORD dwMaxPacketSize );

	// Security
public:
	VOID					EnableSecurity();

private:
	BOOL					IsSecurityEnabled() const;
	VOID					EncryptData(BYTE* outData, const BYTE* inData, int len);
	VOID					DecryptData(BYTE* outData, const BYTE* inData, int len);

private:
	RC4Encry				*m_pEncryptor;
	RC4Encry				*m_pDecryptor;
	BOOL					m_bSecurityEanbled;
};