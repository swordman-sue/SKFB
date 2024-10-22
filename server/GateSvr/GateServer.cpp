#include "stdafx.h"
#include "GateServer.h"
#include "UserFactory.h"
#include "UserManager.h"
#include "PacketHandler.h"
#include "ServerSessionFactory.h"
#include "ServerSession.h"
#include "../Common/MsgDefine.h"
#include "IOCPServer.h"
#include "NetworkObject.h"
#include "../ToolKit/IniFile.h"
#include "../ToolKit/CrashReporter.h"
#include "MapServerSession.h"
#include "../Common/CommonLogic.h"

//����������ؿ��ļ�
#ifdef _DEBUG		
#	if defined(_M_IX86)	
//#		pragma comment(lib,"stlportd.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine_d.lib")
#		pragma comment(lib,"../bin/ToolKit_d.lib")
#		pragma comment(lib,"../bin/Common_d.lib")
#		pragma comment(lib,"../bin/libprotobuf_d.lib") 
#	elif defined(_M_X64)
//#		pragma comment(lib,"stlport_x64d.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64_d.lib")
#		pragma comment(lib,"../bin/ToolKit64_d.lib")
#		pragma comment(lib,"../bin/Common64_d.lib")
#		pragma comment(lib,"../bin/libprotobuf64_d.lib") 
#	else	
#		error "Can not supported on this architecture."
#	endif	
#else		
#	if defined(_M_IX86)
//#		pragma comment(lib,"stlport.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine.lib")
#		pragma comment(lib,"../bin/ToolKit.lib")
#		pragma comment(lib,"../bin/Common.lib")
#		pragma comment(lib,"../bin/libprotobuf.lib") 
#	elif defined(_M_X64)
//#		pragma comment(lib,"stlport_x64.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64.lib")
#		pragma comment(lib,"../bin/ToolKit64.lib")
#		pragma comment(lib,"../bin/Common64.lib")
#		pragma comment(lib,"../bin/libprotobuf64.lib") 
#	else	
#		error "Can not supported on this architecture."
#	endif	
#endif

//����ȫ�����ط���������
GateServer g_GateServer;
//����ȫ�����ط������̶߳���
GateServerThread g_GateServerThread;

//�������������ûص������ӿ�
NetworkObject* CreateClientSideAcceptedObject();
NetworkObject* CreateServerSideAcceptedObject();
VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyClientSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyClientSideConnectedObject( NetworkObject * pNetworkObject );
VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject );

//������־����д����־�ӿ�
void LogPrinter(const char* msg);

GateServer::GateServer(void)
{
	m_pIOCPServer		= NULL;
	m_pMapSession		= NULL;
	memset(m_szClientGateIP, 0, sizeof(m_szClientGateIP));
	m_dwClientGatePort	= 0;
	memset(m_szServerGateIP, 0, sizeof(m_szServerGateIP));
	m_dwServerGatePort	= 0;
	m_bShutDown			= NULL;
	m_pLogfile			= NULL;
	m_bEnableSecurity	= FALSE;
	m_dwCheckTimeGap	= 0;
	m_dwSocketTimeOut	= 0;
	m_dwConnectTimeOut	= 0;
	memset(m_szServerName, 0, sizeof(m_szServerName));
}

GateServer::~GateServer(void)
{

}

//��ʼ�����ط�����
BOOL GateServer::Init()
{
// 	WORD size1 = 2000;
// 	WORD size = htons(size1);

	//���������ò���
	struct
	{
		DWORD dwMaxAcceptSession;
		DWORD dwMaxConnectSession;
		DWORD dwSendBufferSize;
		DWORD dwRecvBufferSize;
		DWORD dwMaxPacketSize;
		DWORD dwAcceptThread;
		DWORD dwConnectThread;
		DWORD dwTimeout;
		BOOL bSecurity;
	} IOCPHANDLER[2];

	//����ini�����ļ�
	IniFile inifile;
	inifile.Init("GateSvr.ini");

	//1 ��������IP�Ͷ˿�������Ϣ
	inifile.GetStrVal("GateSvr","GateSvr_ClientIP",m_szClientGateIP,sizeof(m_szClientGateIP));
	if (0 == strcmp(m_szClientGateIP, "127.0.0.1"))
	{
		GetLocalIP(m_szClientGateIP, sizeof(m_szClientGateIP)-1);
	}

	m_dwClientGatePort = inifile.GetIntVal("GateSvr","GateSvr_ClientPORT");
	inifile.GetStrVal("GateSvr","GateSvr_ServerIP",m_szServerGateIP,sizeof(m_szServerGateIP));
	m_dwServerGatePort = inifile.GetIntVal("GateSvr","GateSvr_ServerPORT");
	inifile.GetStrVal("GateSvr", "ServerName", m_szServerName, sizeof(m_szServerName)-1);

	//2 �������ɶ˿�������Ϣ
	IOCPHANDLER[0].dwMaxAcceptSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_ACCEPT_SESSION");
	IOCPHANDLER[0].dwMaxConnectSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_CONNECT_SESSION");
	IOCPHANDLER[0].dwSendBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","SEND_BUFFER_SIZE");
	IOCPHANDLER[0].dwRecvBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","RECV_BUFFER_SIZE");
	IOCPHANDLER[0].dwMaxPacketSize = inifile.GetIntVal("SERVER_IOHANDLER","MAX_PACKET_SIZE");
	IOCPHANDLER[0].dwAcceptThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_ACCEPT_THREADS");
	IOCPHANDLER[0].dwConnectThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_CONNECT_THREADS");
	IOCPHANDLER[0].dwTimeout = inifile.GetIntVal("SERVER_IOHANDLER","TIME_OUT");
	IOCPHANDLER[0].bSecurity = inifile.GetIntVal("SERVER_IOHANDLER","SECURITY");

	//3 �ͻ�����ɶ˿�������Ϣ
	IOCPHANDLER[1].dwMaxAcceptSession = inifile.GetIntVal("CLIENT_IOHANDLER","MAX_ACCEPT_SESSION");
	IOCPHANDLER[1].dwMaxConnectSession = inifile.GetIntVal("CLIENT_IOHANDLER","MAX_CONNECT_SESSION");
	IOCPHANDLER[1].dwSendBufferSize = inifile.GetIntVal("CLIENT_IOHANDLER","SEND_BUFFER_SIZE");
	IOCPHANDLER[1].dwRecvBufferSize = inifile.GetIntVal("CLIENT_IOHANDLER","RECV_BUFFER_SIZE");
	IOCPHANDLER[1].dwMaxPacketSize = inifile.GetIntVal("CLIENT_IOHANDLER","MAX_PACKET_SIZE");
	IOCPHANDLER[1].dwAcceptThread = inifile.GetIntVal("CLIENT_IOHANDLER","NUMBER_OF_ACCEPT_THREADS");
	IOCPHANDLER[1].dwConnectThread = inifile.GetIntVal("CLIENT_IOHANDLER","NUMBER_OF_CONNECT_THREADS");
	IOCPHANDLER[1].dwTimeout = inifile.GetIntVal("CLIENT_IOHANDLER","TIME_OUT");
	IOCPHANDLER[1].bSecurity = inifile.GetIntVal("CLIENT_IOHANDLER","SECURITY");

	m_bEnableSecurity = IOCPHANDLER[1].bSecurity;

	//4 ������������Ϣ
	m_dwCheckTimeGap = inifile.GetIntVal("CheckTimeout","CheckTimeGap");
	m_dwSocketTimeOut = inifile.GetIntVal("CheckTimeout","SocketTimeOut");
	m_dwConnectTimeOut = inifile.GetIntVal("CheckTimeout","ConnectTimeOut");

	//�õ�����������
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DWORD dwProcessNum;
	dwProcessNum = si.dwNumberOfProcessors;

	//��ʼ����־����
	m_pLogfile = new LogFile;
	m_pLogfile->Init("log","GateSvr",LOG_LEVEL(inifile.GetIntVal("Log","Level")));
	m_bIsShowSpecialLog = inifile.GetIntVal("log", "IsShowSpecialLog");

#ifdef _DEBUG
	GATELOG(GUI_LEVEL,"GateServer(Debug) Starting.");
#else
	GATELOG(GUI_LEVEL, "GateServer(Release) Starting.");
#endif

	//�򿪱���ʱ��ջ��ӡ����
	INIT_CRASH_REPORTER(LogPrinter);

	std::string logBuffer;
	logBuffer = "Init Objects ......................... ";

	//��ʼ����ҹ�����
	UserFactory::Instance()->Init();
	UserManager::Instance();
	logBuffer += "OK";
	GATELOG(GUI_LEVEL, logBuffer.c_str());

	//ע����Ϣ������
	logBuffer = "Register Message Handlers ............ ";
	PacketHandler::Instance()->Resister();
	logBuffer += "OK";
	GATELOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ���������Ự����
	ServerSessionFactory::Instance()->Init();

	//��ʼ����������
	logBuffer = "Network Module Init .................. ";
	IOHANDLER_DESC desc[2];

	desc[0].dwIoHandlerKey = SERVER_IOHANDLER;
	desc[0].dwMaxAcceptSession = IOCPHANDLER[0].dwMaxAcceptSession;
	desc[0].dwMaxConnectSession = IOCPHANDLER[0].dwMaxConnectSession;
	desc[0].dwMaxPacketSize = IOCPHANDLER[0].dwMaxPacketSize;
	desc[0].dwNumberOfAcceptThreads = IOCPHANDLER[0].dwAcceptThread;
	desc[0].dwNumberOfConnectThreads = IOCPHANDLER[0].dwConnectThread;
	desc[0].dwNumberOfIoThreads = 2*dwProcessNum+2;
	desc[0].dwRecvBufferSize = IOCPHANDLER[0].dwRecvBufferSize;
	desc[0].dwSendBufferSize = IOCPHANDLER[0].dwSendBufferSize;
	desc[0].dwTimeOut = IOCPHANDLER[0].dwTimeout;
	desc[0].bSecurity = IOCPHANDLER[0].bSecurity;
	desc[0].fnCreateAcceptedObject = CreateServerSideAcceptedObject;
	desc[0].fnDestroyAcceptedObject = DestroyServerSideAcceptedObject;
	desc[0].fnDestroyConnectedObject = DestroyServerSideConnectedObject;

	desc[1].dwIoHandlerKey = CLIENT_IOHANDLER;
	desc[1].dwMaxAcceptSession = IOCPHANDLER[1].dwMaxAcceptSession;
	desc[1].dwMaxConnectSession = IOCPHANDLER[1].dwMaxConnectSession;
	desc[1].dwMaxPacketSize = IOCPHANDLER[1].dwMaxPacketSize;
	desc[1].dwNumberOfAcceptThreads = IOCPHANDLER[1].dwAcceptThread;
	desc[1].dwNumberOfConnectThreads = IOCPHANDLER[1].dwConnectThread;
	desc[1].dwNumberOfIoThreads = 2*dwProcessNum+2;
	desc[1].dwRecvBufferSize = IOCPHANDLER[1].dwRecvBufferSize;
	desc[1].dwSendBufferSize = IOCPHANDLER[1].dwSendBufferSize;
	desc[1].dwTimeOut = IOCPHANDLER[1].dwTimeout;
	desc[1].bSecurity = IOCPHANDLER[1].bSecurity;
	desc[1].fnCreateAcceptedObject = CreateClientSideAcceptedObject;
	desc[1].fnDestroyAcceptedObject = DestroyClientSideAcceptedObject;
	desc[1].fnDestroyConnectedObject = DestroyClientSideConnectedObject;

	m_pIOCPServer = new IOCPServer;
	if (!m_pIOCPServer->Init(desc,2,WriteLog))
	{
		logBuffer += "Failed";
		GATELOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	logBuffer += "OK";
	GATELOG(GUI_LEVEL, logBuffer.c_str());

	//��������������
	StartServerListen();

	//��ʼ�����
	GATELOG(GUI_LEVEL, "");
	GATELOG(GUI_LEVEL, "GateServer Init Finished!");
	GATELOG(GUI_LEVEL, "");

	return TRUE;
}

//�ͷ����ط�����
void GateServer::Release()
{
	//�ͷ���������
	if(m_pIOCPServer)
	{
		m_pIOCPServer->Shutdown();
		delete m_pIOCPServer;
		m_pIOCPServer = NULL;
	}

	//�ͷ���Ϣ������
	PacketHandler::DestroyInstance();

	//�ͷ���ҹ�����
	UserManager::Instance()->Release(UserFactory::Instance());
	UserManager::DestroyInstance();
	UserFactory::Instance()->Release();
	UserFactory::DestroyInstance();

	//�ͷŷ������Ự����
	ServerSessionFactory::Instance()->Release();
	ServerSessionFactory::DestroyInstance();

	//�ͷ���־����
	if(m_pLogfile)
	{
		m_pLogfile->ShutDown();

		delete m_pLogfile;
		m_pLogfile = NULL;
	}
}

void GateServer::Run()
{
	char szTitile[128];
	char szMsg[256];

	GetConsoleTitleA(szTitile, sizeof(szTitile));
	DWORD dwStatistics = 0;
	DWORD dwFps = 0;

	int nSleepCnt = 0;

	//DWORD dwSendBuffDataLength, dwRecvBuffDataLength;

	while(TRUE)
	{
		static UINT64 lastUpdateTick = GetTickCount64();

		UINT64 curTick = GetTickCount64();

		UINT64 deltaTick;

		if (curTick>=lastUpdateTick)
		{
			deltaTick = curTick - lastUpdateTick;
		}
		else
		{
			deltaTick = 0xFFFFFFFF - lastUpdateTick + curTick + 1;
		}

		lastUpdateTick = curTick;

		if(!Update()) break;

		UINT64 dwDoUpdateAfterTick = GetTickCount64();
		if (dwDoUpdateAfterTick > curTick)
			GATELOG(ERROR_LEVEL, "Do Update Cost[%u]MS", dwDoUpdateAfterTick - curTick);

		if (dwStatistics >= 1000)
		{
			sprintf(szMsg, "[%s]Online_Player:%d Fps:%d,Tick:%lld", m_szServerName,
				UserManager::Instance()->GetPlayerNum(), dwFps, curTick);
			SetConsoleTitleA(szMsg);
			dwStatistics -= 1000;
			dwFps = 0;
		}
		else
		{
			dwStatistics += deltaTick;
			++dwFps;
		}

		if (++nSleepCnt >= 10)
		{
			Sleep(1);
			nSleepCnt = 0;
		}
	}
}

//�������ط�����
BOOL GateServer::Update()
{
	if(m_bShutDown)
		return FALSE;

	static UINT64 dwLastUpdateTime = GetTickCount64();
	static UINT64 dwUseTime = 0;

	UINT64 dwCurTime = GetTickCount64();

	m_pIOCPServer->Update();

	UserManager::Instance()->Update();

	if (dwCurTime >= dwLastUpdateTime + 500)
	{
		dwLastUpdateTime = dwCurTime;
		dwUseTime = GetTickCount64() - dwCurTime;
		if (dwUseTime)
		{
			time_t tnow = time(NULL);
			tm t;
			_localtime64_s(&t,&tnow);
			char szBuffer[128];
			sprintf(szBuffer,"%d-%d-%d %d:%d:%d",t.tm_year + 1900,t.tm_mon + 1,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
			GATELOG(GUI_LEVEL, "[%s]���߼�����һ֡����ʱ��(%d)����", szBuffer,dwUseTime);
		}
	}

	return TRUE;
}

//��������������
void GateServer::StartServerListen()
{
	if(!m_pIOCPServer->IsListening(SERVER_IOHANDLER))
	{
		m_pIOCPServer->StartListen(SERVER_IOHANDLER,m_szServerGateIP,(WORD)m_dwServerGatePort);

		GATELOG(GUI_LEVEL, "GateServer Start Server Listening.");
	}
}

//�����ͻ��˼���
void GateServer::StartClientListen()
{
	if(!m_pIOCPServer->IsListening(CLIENT_IOHANDLER))
	{
		m_pIOCPServer->StartListen(CLIENT_IOHANDLER,m_szClientGateIP,(WORD)m_dwClientGatePort);
		GATELOG(GUI_LEVEL, "GateServer Start Client Listening.");
	}
}

//�����ͻ����������
NetworkObject * CreateClientSideAcceptedObject()
{
	return UserFactory::Instance()->AllocUser();
}

//�����������������
NetworkObject * CreateServerSideAcceptedObject()
{
	return ServerSessionFactory::Instance()->AllocServerSession(MAP_SERVER);
}

//���ٿͻ����������
VOID DestroyClientSideAcceptedObject( NetworkObject * pNetworkObject )
{
	UserFactory::Instance()->FreeUser((UserSession*)pNetworkObject);
}

//���ٷ������������
VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject )
{
	ServerSessionFactory::Instance()->FreeServerSession((ServerSession*)pNetworkObject);
}

VOID DestroyClientSideConnectedObject( NetworkObject * pNetworkObject )
{

}

VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject )
{

}

void LogPrinter(const char* msg)
{
	g_GateServer.WriteLog(ERROR_LEVEL, msg);
}

void GateServerThread::EndThread()
{
	g_GateServer.SetShutDown(TRUE);
}

DWORD GateServerThread::run()
{
	IF_NOT (g_GateServer.Init())
	{
		g_GateServer.SetShutDown();
		GATELOG(GUI_LEVEL, "");
		GATELOG(GUI_LEVEL, "Please Check the Configuration and Reboot the Server!");
	}

	g_GateServer.Run();

	g_GateServer.Release();

	return 0;
}

BOOL GateServerThread::IsShutDown()
{
	return g_GateServer.IsShutDown();
}

void GateServer::WriteLog(LOG_LEVEL eLevel, const char* log, const char* szFuncName)
{
	IF_NOT (log)
		return;

	if (m_pLogfile != NULL)
	{
		m_pLogfile->InsertLog(eLevel,log,szFuncName);
	}


	// �����������Ϣ,���ж��Ƿ���Ҫ��ʾ
	if (eLevel != SPECAIL_LEVEL || m_bIsShowSpecialLog)
	{
		time_t t = time(NULL);
		tm curTime;
		localtime_s(&curTime, &t);
		char szTime[32] = { 0 };
		sprintf_s(szTime, sizeof(szTime), "%d:%d:%d", curTime.tm_hour, curTime.tm_min, curTime.tm_sec);
		//printf("[%s]%s\n",szTime,log);

		if (szFuncName)
		{
			printf("[%s][%s]:%s\n", szTime, szFuncName, log);
		}
		else
		{
			printf("[%s]%s\n", szTime, log);
		}
	}
}

void GateServer::WriteLog(const char* log,...)
{
	GATELOG(GUI_LEVEL,log);
}