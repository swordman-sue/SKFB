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

//定义引入相关库文件
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

//定义全局网关服务器对象
GateServer g_GateServer;
//定义全局网关服务器线程对象
GateServerThread g_GateServerThread;

//定义网络引擎用回调函数接口
NetworkObject* CreateClientSideAcceptedObject();
NetworkObject* CreateServerSideAcceptedObject();
VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyClientSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyClientSideConnectedObject( NetworkObject * pNetworkObject );
VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject );

//定义日志对象写入日志接口
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

//初始化网关服务器
BOOL GateServer::Init()
{
// 	WORD size1 = 2000;
// 	WORD size = htons(size1);

	//网络引擎用参数
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

	//载入ini配置文件
	IniFile inifile;
	inifile.Init("GateSvr.ini");

	//1 网络连接IP和端口配置信息
	inifile.GetStrVal("GateSvr","GateSvr_ClientIP",m_szClientGateIP,sizeof(m_szClientGateIP));
	if (0 == strcmp(m_szClientGateIP, "127.0.0.1"))
	{
		GetLocalIP(m_szClientGateIP, sizeof(m_szClientGateIP)-1);
	}

	m_dwClientGatePort = inifile.GetIntVal("GateSvr","GateSvr_ClientPORT");
	inifile.GetStrVal("GateSvr","GateSvr_ServerIP",m_szServerGateIP,sizeof(m_szServerGateIP));
	m_dwServerGatePort = inifile.GetIntVal("GateSvr","GateSvr_ServerPORT");
	inifile.GetStrVal("GateSvr", "ServerName", m_szServerName, sizeof(m_szServerName)-1);

	//2 服务端完成端口配置信息
	IOCPHANDLER[0].dwMaxAcceptSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_ACCEPT_SESSION");
	IOCPHANDLER[0].dwMaxConnectSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_CONNECT_SESSION");
	IOCPHANDLER[0].dwSendBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","SEND_BUFFER_SIZE");
	IOCPHANDLER[0].dwRecvBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","RECV_BUFFER_SIZE");
	IOCPHANDLER[0].dwMaxPacketSize = inifile.GetIntVal("SERVER_IOHANDLER","MAX_PACKET_SIZE");
	IOCPHANDLER[0].dwAcceptThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_ACCEPT_THREADS");
	IOCPHANDLER[0].dwConnectThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_CONNECT_THREADS");
	IOCPHANDLER[0].dwTimeout = inifile.GetIntVal("SERVER_IOHANDLER","TIME_OUT");
	IOCPHANDLER[0].bSecurity = inifile.GetIntVal("SERVER_IOHANDLER","SECURITY");

	//3 客户端完成端口配置信息
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

	//4 网络检测配置信息
	m_dwCheckTimeGap = inifile.GetIntVal("CheckTimeout","CheckTimeGap");
	m_dwSocketTimeOut = inifile.GetIntVal("CheckTimeout","SocketTimeOut");
	m_dwConnectTimeOut = inifile.GetIntVal("CheckTimeout","ConnectTimeOut");

	//得到处理器数量
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DWORD dwProcessNum;
	dwProcessNum = si.dwNumberOfProcessors;

	//初始化日志对象
	m_pLogfile = new LogFile;
	m_pLogfile->Init("log","GateSvr",LOG_LEVEL(inifile.GetIntVal("Log","Level")));
	m_bIsShowSpecialLog = inifile.GetIntVal("log", "IsShowSpecialLog");

#ifdef _DEBUG
	GATELOG(GUI_LEVEL,"GateServer(Debug) Starting.");
#else
	GATELOG(GUI_LEVEL, "GateServer(Release) Starting.");
#endif

	//打开崩溃时堆栈打印功能
	INIT_CRASH_REPORTER(LogPrinter);

	std::string logBuffer;
	logBuffer = "Init Objects ......................... ";

	//初始化玩家管理器
	UserFactory::Instance()->Init();
	UserManager::Instance();
	logBuffer += "OK";
	GATELOG(GUI_LEVEL, logBuffer.c_str());

	//注册消息处理函数
	logBuffer = "Register Message Handlers ............ ";
	PacketHandler::Instance()->Resister();
	logBuffer += "OK";
	GATELOG(GUI_LEVEL, logBuffer.c_str());

	//初始化服务器会话工厂
	ServerSessionFactory::Instance()->Init();

	//初始化网络引擎
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

	//开启服务器监听
	StartServerListen();

	//初始化完成
	GATELOG(GUI_LEVEL, "");
	GATELOG(GUI_LEVEL, "GateServer Init Finished!");
	GATELOG(GUI_LEVEL, "");

	return TRUE;
}

//释放网关服务器
void GateServer::Release()
{
	//释放网络引擎
	if(m_pIOCPServer)
	{
		m_pIOCPServer->Shutdown();
		delete m_pIOCPServer;
		m_pIOCPServer = NULL;
	}

	//释放消息解析器
	PacketHandler::DestroyInstance();

	//释放玩家管理器
	UserManager::Instance()->Release(UserFactory::Instance());
	UserManager::DestroyInstance();
	UserFactory::Instance()->Release();
	UserFactory::DestroyInstance();

	//释放服务器会话工厂
	ServerSessionFactory::Instance()->Release();
	ServerSessionFactory::DestroyInstance();

	//释放日志对象
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

//更新网关服务器
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
			GATELOG(GUI_LEVEL, "[%s]主逻辑处理一帧消耗时间(%d)毫秒", szBuffer,dwUseTime);
		}
	}

	return TRUE;
}

//开启服务器监听
void GateServer::StartServerListen()
{
	if(!m_pIOCPServer->IsListening(SERVER_IOHANDLER))
	{
		m_pIOCPServer->StartListen(SERVER_IOHANDLER,m_szServerGateIP,(WORD)m_dwServerGatePort);

		GATELOG(GUI_LEVEL, "GateServer Start Server Listening.");
	}
}

//开启客户端监听
void GateServer::StartClientListen()
{
	if(!m_pIOCPServer->IsListening(CLIENT_IOHANDLER))
	{
		m_pIOCPServer->StartListen(CLIENT_IOHANDLER,m_szClientGateIP,(WORD)m_dwClientGatePort);
		GATELOG(GUI_LEVEL, "GateServer Start Client Listening.");
	}
}

//创建客户端网络对象
NetworkObject * CreateClientSideAcceptedObject()
{
	return UserFactory::Instance()->AllocUser();
}

//创建服务器网络对象
NetworkObject * CreateServerSideAcceptedObject()
{
	return ServerSessionFactory::Instance()->AllocServerSession(MAP_SERVER);
}

//销毁客户端网络对象
VOID DestroyClientSideAcceptedObject( NetworkObject * pNetworkObject )
{
	UserFactory::Instance()->FreeUser((UserSession*)pNetworkObject);
}

//销毁服务器网络对象
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


	// 如果是网络消息,则判断是否需要显示
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