/**\file
   ���������߼�����
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
	SessionList		*m_pConnectingList;				// �������ӵ��б�
	HANDLE			m_hThread[MAX_CONNECT_THREAD];	// �߳��б�
	DWORD			m_numThreads;					// �߳�����
	HANDLE			m_hEvent[2];					// �¼��б�
	BOOL			m_bShutdown;					// �رձ�־
};