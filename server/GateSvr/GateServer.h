#pragma once

#include "../Common/Const.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/LogFile.h"

class IOCPServer;
class NetMsgBody;
class MapServerSession;

//���ط�����
class GateServer
{
	//���Ӷ�����
	enum 
	{ 
		SERVER_IOHANDLER = 0,		//��������
		CLIENT_IOHANDLER = 1		//�ͻ��˲�
	};

public:
	GateServer(void);
	~GateServer(void);

public:
	BOOL					Init();
	void					Release();
	void					Run();
	BOOL					Update();

	void					StartClientListen();
	void					StartServerListen();

	void					SetShutDown(BOOL bShutDown = TRUE) { m_bShutDown = bShutDown; }
	BOOL					IsShutDown() { return m_bShutDown; }

	MapServerSession*		GetMapSession() { return m_pMapSession; }
	void					SetMapSession(MapServerSession* pSession) { m_pMapSession = pSession; } 

	void					WriteLog(LOG_LEVEL eLevel, const char* log,const char* szFuncName = NULL);
	static void				WriteLog(const char* log,...);

	BOOL					IsEnableClientSecurity() { return m_bEnableSecurity; }
	DWORD					GetCheckTimeGap() { return m_dwCheckTimeGap; }
	DWORD					GetSocketTimeOut() { return m_dwSocketTimeOut; }
	DWORD					GetConnectTimeOut() { return m_dwConnectTimeOut; }

private:
	IOCPServer*				m_pIOCPServer;					//	�����������
	MapServerSession*		m_pMapSession;					//	��ͼ�������Ự���� 
	char					m_szClientGateIP[32];			//	�ͻ�������IP
	DWORD					m_dwClientGatePort;				//	�ͻ������ض˿�
	char					m_szServerGateIP[32];			//	����������IP
	DWORD					m_dwServerGatePort;				//	���������ض˿�
	BOOL					m_bShutDown;
	LogFile					*m_pLogfile;					//	��־�ļ�����
	BOOL					m_bEnableSecurity;
	DWORD					m_dwCheckTimeGap;				//	���ͻ��˳�ʱʱ����
	DWORD					m_dwSocketTimeOut;				//	socket��ʱ
	DWORD					m_dwConnectTimeOut;				//	socket���ӳ�ʱ
	bool					m_bIsShowSpecialLog;			// �Ƿ���ʾ��������־

	char					m_szServerName[64];	// ����������
};

//����ȫ�����ط���������
extern GateServer g_GateServer; 

//���ط������߳�
class GateServerThread : public ThreadBase
{
public:
	GateServerThread() { }
	virtual ~GateServerThread() { }
public:
	virtual void			EndThread();
	virtual DWORD			run();
	BOOL					IsShutDown();

};

//����ȫ�����ط������̶߳���
extern GateServerThread	g_GateServerThread;