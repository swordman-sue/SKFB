/**\file
   IO�߼�����
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
// IO�߼�����(�������ӣ��������ӣ����ݽ��գ����ݷ���)
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
	
	Acceptor			*m_pAcceptor;	// ���������߼���
	Connector			*m_pConnector;	// ���������߼���

	SessionList			*m_pActiveSessionList;		// ��Ự(��������+�������ӻỰ)
	SessionList			*m_pAcceptedSessionList;	// �������ӻỰ�б�
	SessionList			*m_pConnectSuccessList;		// �������ӻỰ�б�(�ɹ�)
	SessionList			*m_pConnectFailList;		// �������ӻỰ�б�(ʧ��)
	SessionList			*m_pTempList;				// ��ʱ�б�
	BOOL				m_bShutdown;				// �Ƿ����״̬

	DWORD				m_dwKey;
	HANDLE				m_hIOCP;							
	HANDLE				*m_hIoThread;			// IO�߳��б�
	DWORD				m_numIoThreads;			// IO�߳�����				
	DWORD				m_dwMaxPacketSize;		// ������������
	DWORD				m_numActiveSessions;	// ��ǰ�Ự����	
	DWORD				m_dwMaxAcceptSession;	// �Ự����
	HANDLE				m_hSendThread;			// �����߳�

	// �ڴ��,ʹ��������ص�Ŀ����Ϊ�˸��õĿ�������(��������+��������)
	SessionPool			*m_pAcceptSessionPool;
	SessionPool			*m_pConnectSessionPool;
	
	fnCallBackCreateAcceptedObject		m_fnCreateAcceptedObject;
	fnCallBackDestroyAcceptedObject		m_fnDestroyAcceptedObject;
	fnCallBackDestroyConnectedObject	m_fnDestroyConnectedObject;
};