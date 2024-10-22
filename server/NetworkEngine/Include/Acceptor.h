/**\file
   接收请求逻辑处理
   \author Kevin
   \date 2014/12/24
   \note 
*/

#pragma once

#include <windows.h>
#include <process.h>
#include <Ws2tcpip.h>

#define MAX_ACCEPT_THREAD	16

class Session;
class IoHandler;
class SessionPool;

typedef struct tagIOHANDLER_DESC IOHANDLER_DESC, *LPIOHANDLER_DESC;


class Acceptor
{
	friend unsigned __stdcall accept_thread( LPVOID param );

public:
	Acceptor();
	~Acceptor();

	VOID				Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads );
	BOOL				StartListen( char *pIP, WORD wPort );
	VOID				Shutdown();
	inline BOOL			IsListening() { return m_listenSocket != INVALID_SOCKET; }
	VOID				SuspendListenThread();
	VOID				ResumeListenThread();
	inline SOCKET		GetListenSocket() { return m_listenSocket; }

private:
	BOOL				CreateListenSocket( char *pIP, WORD wPort );

	IoHandler			*m_pIoHandler;
	HANDLE				m_hThread[MAX_ACCEPT_THREAD];
	DWORD				m_numThreads;
	HANDLE				m_hIOCP;		// 与监听连接相关的完成端口
	SOCKET				m_listenSocket;	// 监听连接Socket
	SOCKADDR_IN			m_sockaddr;
};
