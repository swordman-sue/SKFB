/**\file
   IO逻辑处理
   \author Kevin
   \date 2014/12/24
   \note 
*/

#pragma once

#include <windows.h>
#include <process.h>
#include <vector>

using namespace std;

#define EXTRA_ACCEPTEX_NUM		10

class Session;
class SessionPool;
class Acceptor;
class Connector;
class SessionList;
class IOCPServer;
class NetworkObject;
struct tagIOHANDLER_DESC;

typedef tagIOHANDLER_DESC IOHANDLER_DESC, *LPIOHANDLER_DESC;

typedef NetworkObject* (*fnCallBackCreateAcceptedObject)();
typedef VOID (*fnCallBackDestroyAcceptedObject)( NetworkObject *pNetworkObject );
typedef VOID (*fnCallBackDestroyConnectedObject)( NetworkObject *pNetworkObject );
//typedef VOID (*fnCallBackLog)( NetworkObject pNetworkObject, char *pszLog );

//=============================================================================================================================
// IO逻辑处理(主动连接，被动连接，数据接收，数据发送)
//=============================================================================================================================
class IoHandler
{
	friend unsigned __stdcall io_thread( LPVOID param );
	friend unsigned __stdcall send_thread( LPVOID param );
	friend unsigned __stdcall accept_thread( LPVOID param );
	friend unsigned __stdcall connect_thread( LPVOID param );

public:
	IoHandler();
	~IoHandler();

	VOID				Init( IOCPServer *pIOCPServer, LPIOHANDLER_DESC lpDesc );
	BOOL				StartListen( char *pIP, WORD wPort );
	VOID				Update();
	VOID				Shutdown();
	DWORD				Connect( NetworkObject *pNetworkObject, char *pszIP, WORD wPort );
	BOOL				IsListening();
	inline DWORD		GetNumberOfConnections()		{ return m_numActiveSessions; }
	inline DWORD		GetKey() { return m_dwKey; }

private:
	Session*			AllocAcceptSession();
	Session*			AllocConnectSession();
	VOID				FreeSession( Session *pSession );
	VOID				ReuseSession( Session *pSession );
	VOID				ProcessConnectSuccessList();
	VOID				ProcessConnectFailList();
	VOID				ProcessAcceptedSessionList();
	VOID				ProcessActiveSessionList();
	VOID				KickDeadSessions();
	VOID				ProcessSend();
	VOID				KickAllSessions();

	IOCPServer			*m_pIOCPServer;
	
	Acceptor			*m_pAcceptor;	// 被动连接逻辑类
	Connector			*m_pConnector;	// 主动连接逻辑类

	SessionList			*m_pActiveSessionList;		// 活动会话(被动连接+主动连接会话)
	SessionList			*m_pAcceptedSessionList;	// 被动连接会话列表
	SessionList			*m_pConnectSuccessList;		// 主动连接会话列表(成功)
	SessionList			*m_pConnectFailList;		// 主动连接会话列表(失败)
	SessionList			*m_pTempList;				// 临时列表
	BOOL				m_bShutdown;				// 是否结束状态

	DWORD				m_dwKey;
	HANDLE				m_hIOCP;							
	HANDLE				*m_hIoThread;			// IO线程列表
	DWORD				m_numIoThreads;			// IO线程数量				
	DWORD				m_dwMaxPacketSize;		// 单个包的上限
	DWORD				m_numActiveSessions;	// 当前会话数量	
	DWORD				m_dwMaxAcceptSession;	// 会话上限
	HANDLE				m_hSendThread;			// 发送线程

	// 内存池,使用两个存池的目的是为了更好的控制上限(主动连接+被动连接)
	SessionPool			*m_pAcceptSessionPool;
	SessionPool			*m_pConnectSessionPool;
	
	fnCallBackCreateAcceptedObject		m_fnCreateAcceptedObject;
	fnCallBackDestroyAcceptedObject		m_fnDestroyAcceptedObject;
	fnCallBackDestroyConnectedObject	m_fnDestroyConnectedObject;
};