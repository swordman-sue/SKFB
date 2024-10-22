/**\file
   IOCP管理器
   \author Kevin
   \date 2014/12/24
   \note 
*/
#pragma once

#pragma comment( lib, "ws2_32.lib" )

#include <windows.h>
#include <map>

class NetworkObject;
class IoHandler;

typedef std::map<DWORD, IoHandler*>		IOHANDLER_MAP;
typedef std::pair<DWORD, IoHandler*>	IOHANDLER_MAP_PAIR;
typedef IOHANDLER_MAP::iterator			IOHANDLER_MAP_ITER;

typedef NetworkObject* (*fnCallBackCreateAcceptedObject)();
typedef VOID (*fnCallBackDestroyAcceptedObject)( NetworkObject *pNetworkObject );
typedef VOID (*fnCallBackDestroyConnectedObject)( NetworkObject *pNetworkObject );

typedef VOID (*fnCallBackLog)( const char *szMsg,...);

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
typedef struct tagIOHANDLER_DESC
{
	DWORD								dwIoHandlerKey;					
	DWORD								dwMaxAcceptSession;				
	DWORD								dwMaxConnectSession;			
	DWORD								dwSendBufferSize;				
	DWORD								dwRecvBufferSize;				
	DWORD								dwTimeOut;						
	DWORD								dwMaxPacketSize;				
	DWORD								dwNumberOfIoThreads;			
	DWORD								dwNumberOfAcceptThreads;		
	DWORD								dwNumberOfConnectThreads;		
	BOOL								bSecurity;
	fnCallBackCreateAcceptedObject		fnCreateAcceptedObject;			
	fnCallBackDestroyAcceptedObject		fnDestroyAcceptedObject;		
	fnCallBackDestroyConnectedObject	fnDestroyConnectedObject;
} IOHANDLER_DESC, *LPIOHANDLER_DESC;

//-------------------------------------------------------------------------------------------------
// IO管理器
//-------------------------------------------------------------------------------------------------
class IOCPServer
{
	friend unsigned __stdcall send_thread( LPVOID param );

public:
	IOCPServer();
	virtual ~IOCPServer();

	BOOL				Init( LPIOHANDLER_DESC lpDesc, DWORD dwNumberofIoHandlers, fnCallBackLog fnWriteLog = NULL);
	BOOL				StartListen( DWORD dwIoHandlerKey, char *pIP, WORD wPort );
	VOID				Update();
	VOID				Shutdown();
	DWORD				Connect( DWORD dwIoHandlerKey, NetworkObject *pNetworkObject, char *pszIP, WORD wPort );
	BOOL				IsListening( DWORD dwIoHandlerKey );
	DWORD				GetNumberOfConnections( DWORD dwIoHandlerKey );

	VOID				WriteLog(const char *szLog,...);

private:
	// Winsock服务的初始化
	BOOL				InitWinsock();

	VOID				CreateIoHandler( LPIOHANDLER_DESC lpDesc );

	
	BOOL				m_bShutdown;
	IOHANDLER_MAP		m_mapIoHandlers;

	fnCallBackLog		m_fnWriteLog;
};
