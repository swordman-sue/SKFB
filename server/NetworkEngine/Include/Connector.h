/**\file
   请求连接逻辑处理
   \author Kevin
   \date 2014/12/24
   \note 
*/
#pragma once

#include <windows.h>

#define MAX_CONNECT_THREAD	16

class IoHandler;
class Session;
class SessionList;

//=============================================================================================================================

//=============================================================================================================================
class Connector
{
	friend unsigned __stdcall connect_thread( LPVOID param );

public:
	Connector(void);
	~Connector(void);

	VOID			Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads );
	VOID			Connect( Session *pSession );
	VOID			Shutdown();

private:
	IoHandler		*m_pIoHandler;
	SessionList		*m_pConnectingList;				// 请求连接的列表
	HANDLE			m_hThread[MAX_CONNECT_THREAD];	// 线程列表
	DWORD			m_numThreads;					// 线程数量
	HANDLE			m_hEvent[2];					// 事件列表
	BOOL			m_bShutdown;					// 关闭标志
};