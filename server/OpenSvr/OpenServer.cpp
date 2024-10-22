#include "stdafx.h"
#include "OpenServer.h"
#include "ServerSession.h"
#include "MapServerSession.h"
#include "PacketHandler.h"
#include "ServerSessionFactory.h"
#include "HttpQueryResult.h"
#include <json/reader.h>
#include <json/value.h>
#include "ServerSessionManager.h"
#include "PHPSessionFactory.h"
#include "PHPSession.h"
#include "../Common/MsgDefine.h"
#include "IOCPServer.h"
#include "NetworkObject.h"
#include "../ToolKit/IniFile.h"
#include "../ToolKit/CrashReporter.h"
#include "PHPSessionManager.h"
#include "../Common/CommonLogic.h"
#include "HttpRequest.h"
#include "../ToolKit/xml/tinyxml.h"
#include "../Common/sdk/QuickSDK/QuickEncrypt.h"
#include "../ToolKit/MD5.h"
//#include "../user_info.pb.h"

OpenServer g_OpenServer;
OpenServerThread g_OpenServerThread;

#ifdef _DEBUG		
#	if defined(_M_IX86)	
//#		pragma comment(lib,"stlportd.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine_d.lib")
#		pragma comment(lib,"../bin/ToolKit_d.lib")
#		pragma comment(lib,"../bin/Common_d.lib")
#		pragma comment(lib,"../bin/Jsoncpp_d.lib")
#	elif defined(_M_X64)
//#		pragma comment(lib,"stlport_x64d.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64_d.lib")
#		pragma comment(lib,"../bin/ToolKit64_d.lib")
#		pragma comment(lib,"../bin/Common64_d.lib")
#		pragma comment(lib,"../bin/Jsoncpp64_d.lib")
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
#		pragma comment(lib,"../bin/Jsoncpp.lib")
#	elif defined(_M_X64)
//#		pragma comment(lib,"stlport_x64.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64.lib")
#		pragma comment(lib,"../bin/ToolKit64.lib")
#		pragma comment(lib,"../bin/Common64.lib")
#		pragma comment(lib,"../bin/Jsoncpp64.lib")
#		pragma comment(lib,"../bin/libprotobuf64.lib") 
#	else	
#		error "Can not supported on this architecture."
#	endif	
#endif

NetworkObject* CreateServerSideAcceptedObject();
VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject );

NetworkObject* CreateClientSideAcceptedObject();
VOID DestroyClientSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyClientSideConnectedObject( NetworkObject * pNetworkObject );

void LogPrinter(const char* msg);
void HttpCallBackQueryResult( DWORD dwIndex, HttpQueryResult * pData );

bool Json2Object(Json::Value &json_object, const char *pszKey, const char *pszFuncName, Json::Value &object)
{
	Json::Value json_value = json_object[pszKey];
	if (json_value.isNull() || !json_value.isObject())
	{
		OPENLOG(ERROR_LEVEL, "[%s]解析Json字段出错[%s]!", pszFuncName, pszKey);
		return false;
	}

	object = json_value;

	return true;
}

// Json字段转符串
bool Json2String(Json::Value &json_object, const char *pszKey, const char *pszFuncName, std::string &strValue)
{
	Json::Value json_value = json_object[pszKey];
	if (json_value.isNull() || !json_value.isString())
	{
		OPENLOG(ERROR_LEVEL, "[%s]解析Json字段出错[%s]!", pszFuncName, pszKey);
		return false;
	}

	strValue = json_value.asString();

	return true;
}

bool Json2Int(Json::Value &json_object, const char *pszKey, const char *pszFuncName, int &nValue)
{
	Json::Value json_value = json_object[pszKey];
	if (json_value.isNull() || !json_value.isInt())
	{
		OPENLOG(ERROR_LEVEL, "[%s]解析Json字段出错[%s]!", pszFuncName, pszKey);
		return false;
	}

	nValue = json_value.asInt();

	return true;
}

unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

// bool connect(char *host, int port, int timeout)
// {
// 	TIMEVAL Timeout;
// 	Timeout.tv_sec = timeout;
// 	Timeout.tv_usec = 0;
// 	struct sockaddr_in address;  /* the libc network address data structure */
// 
// 	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 
// 	address.sin_addr.s_addr = inet_addr(host); /* assign the address */
// 	address.sin_port = htons(port);            /* translate int2port num */
// 	address.sin_family = AF_INET;
// 
// 	//set the socket in non-blocking
// 	unsigned long iMode = 1;
// 	int iResult = ioctlsocket(sock, FIONBIO, &iMode);
// 	if (iResult != NO_ERROR)
// 	{
// 		printf("ioctlsocket failed with error: %ld\n", iResult);
// 	}
// 
// 	if (connect(sock, (struct sockaddr *)&address, sizeof(address)) == false)
// 	{
// 		return false;
// 	}
// 
// 	// restart the socket mode
// 	iMode = 0;
// 	iResult = ioctlsocket(sock, FIONBIO, &iMode);
// 	if (iResult != NO_ERROR)
// 	{
// 		printf("ioctlsocket failed with error: %ld\n", iResult);
// 	}
// 
// 	fd_set Write, Err;
// 	FD_ZERO(&Write);
// 	FD_ZERO(&Err);
// 	FD_SET(sock, &Write);
// 	FD_SET(sock, &Err);
// 
// 	// check if the socket is ready
// 	select(0, NULL, &Write, &Err, &Timeout);
// 	if (FD_ISSET(sock, &Write))
// 	{
// 		return true;
// 	}
// 
// 	return false;
// }

#define MESSAGE_TYPE_SHIFT ((sizeof(size_t)-1) * 8)
#define PTYPE_SOCKET 6

OpenServer::OpenServer(void)
: m_pIOCPServer(NULL),
m_pLogfile(NULL),
m_bShutDown(FALSE)
{
	memset(m_szServerName, 0, sizeof(m_szServerName));

	// https://yun.tim.qq.com/v5/tlssmssvr/sendsms?sdkappid=qqqqqq&random=xxxx
	// for test
	CHttpRequest http_request; 
	//  	//CHttpRequest *http = new CHttpRequest(); http://oss.vanggame.com:8080/pay/obtainOrder
	//string context = "userID=912734&productID=0&productName=60钻&productDesc=充值6元可获60钻石&money=6&roleID=16530&roleName=完美重击&serverID=23&serverName=三胖子专服&extension={\"goods_id\":0,\"cp_channel_type\":10701,\"cp_order_id\":0,\"recharge_type\":2,\"ip\":\"192.168.1.3\",\"role_id\":16530,\"recharge_id\":2001}&notifyUrl=http://17m13880g2.iok.la:10197/recharge_callback_ios_ly_xt&sign=0b34401c30ccd4e2c87052838cb69002&signType=md5";
	//http_request.SendRequest_Get("oss.vanggame.com", 8081, "/pay/obtainOrder", context);
	
	

	//string ddd = "http://oss.vanggame.com:8080/pay/obtainOrder?userID=912734&productID=0&productName=60钻&productDesc=充值6元可获60钻石&money=6&roleID=16530&roleName=完美重击&serverID=23&serverName=三胖子专服&extension={"goods_id":0,"cp_channel_type":10701,"cp_order_id":0,"recharge_type":2,"ip":"192.168.1.3","role_id":16530,"recharge_id":2001}&notifyUrl=http://17m13880g2.iok.la:10197/recharge_callback_ios_ly_xt&sign=0b34401c30ccd4e2c87052838cb69002&signType=md5";
	//http_request.SendRequest_Post("oss.vanggame.com", 8080, "/pay/obtainOrder", "token=qqqqqq&uid=xxxx&product_code=xxxx");
	// http://checkuser.sdk.quicksdk.net/v2/checkUserInfo
	//http_request.SendRequest_Post("checkuser.sdk.quicksdk.net", 80, "/post_test?id=kevin&pw=78", "id=kevin&pw=78");
	//http_request.SendRequest_Post("usrsys.vivo.com.cn", 443, "/sdk/user/auth.do", "token=qqqqqq&uid=xxxx&product_code=xxxx");

// 	int nullValue = 0;
// 	Json::Value root;
// 	root["type"] = nullValue;
// 	root["name"] = "Xin Ma";
// 	root["pwd"] = "123456";
// 
// 	string tmpstr = root.toStyledString();
// 
// 	char buff[1024] = { 0 };
// 	strcpy_s(buff, strlen(tmpstr.c_str()) + 1, tmpstr.c_str());
// 	cout << "buff_root :" << buff << endl;
// 
// 	Json::Value root2;
// 	Json::Reader reader;
// 
// 	if (reader.parse(buff, root2))
// 	{
// 		if (nullValue == root2["type"].asInt())
// 		{
// 			string MyName = root2["name"].asString();
// 			string MyPwd = root2["pwd"].asString();
// 		}
// 	}
// 
// 	if (reader.parse("{\"result\":\"1\", \"desc\" : \"Validated User\" }", root2))
// 	{
// 		string strTTT = root2["result"].asString();
// 		if (nullValue == root2["result"].asInt())
// 		{
// 			string MyName = root2["name"].asString();
// 			string MyPwd = root2["desc"].asString();
// 		}
// 	}

	// 8df713d326d2993df3bd659af6be8061
	//string strOldSign = "中国";
	//MD5::Digest digest = MD5::Encode(strOldSign);
	//string strNewSign = digest.HexString();
}

OpenServer::~OpenServer(void)
{

}

BOOL OpenServer::Init()
{
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
	}IOCPHANDLER[2];

	//设置服务器标志
	m_bShutDown = FALSE;

	//读取配置文件
	IniFile inifile;
	inifile.Init("OpenSvr.ini");

	//1 读取服务器接入信息
	inifile.GetStrVal("OpenSvr","OpenSvr_ServerIP",m_szServerGateIP,sizeof(m_szServerGateIP));
	m_dwServerGatePort = inifile.GetIntVal("OpenSvr","OpenSvr_ServerPORT");

	inifile.GetStrVal("OpenSvr","OpenSvr_PHPIP",m_szPHPIP,sizeof(m_szPHPIP));
	if (0 == strcmp(m_szPHPIP, "127.0.0.1"))
	{
		GetLocalIP(m_szPHPIP, sizeof(m_szPHPIP)-1);
	}

	// Nodejs服务
	inifile.GetStrVal("NodeJs", "ServerIP", m_szNodeJsServerIP, sizeof(m_szNodeJsServerIP));
	if (0 == strcmp(m_szNodeJsServerIP, "127.0.0.1"))
	{
		GetLocalIP(m_szNodeJsServerIP, sizeof(m_szNodeJsServerIP)-1);
	}
	m_dwNodeJsServerPort = inifile.GetIntVal("NodeJs", "ServerPort");
	inifile.GetStrVal("NodeJs", "method", m_szSmsMethod, sizeof(m_szSmsMethod));

	// 数据统计
	inifile.GetStrVal("Statistics", "ServerIP", m_szStatisticsServerIP, sizeof(m_szStatisticsServerIP));
	if (0 == strcmp(m_szStatisticsServerIP, "127.0.0.1"))
	{
		GetLocalIP(m_szStatisticsServerIP, sizeof(m_szStatisticsServerIP)-1);
	}
	m_dwStatisticsServerPort = inifile.GetIntVal("Statistics", "ServerPort");
	m_bIsOpenStatistics = inifile.GetIntVal("Statistics", "is_open_statistics");
	m_dwStatisticsAppKey = inifile.GetIntVal("Statistics", "statistics_app_id");
	
	
	// Quicksdk相关
	inifile.GetStrVal("QuickSDK_Android", "Md5Key", m_szQuickSDKMd5Key, sizeof(m_szQuickSDKMd5Key));
	inifile.GetStrVal("QuickSDK_Android", "CallbackKey", m_szQuickSDKCallbackKey, sizeof(m_szQuickSDKCallbackKey));
	inifile.GetStrVal("QuickSDK_Android", "ProductCode", m_szQuickSDKProductCode, sizeof(m_szQuickSDKProductCode));

	inifile.GetStrVal("QuickSDK_IOS", "Md5Key", m_szQuickSDKMd5KeyIOS, sizeof(m_szQuickSDKMd5KeyIOS));
	inifile.GetStrVal("QuickSDK_IOS", "CallbackKey", m_szQuickSDKCallbackKeyIOS, sizeof(m_szQuickSDKCallbackKeyIOS));
	inifile.GetStrVal("QuickSDK_IOS", "ProductCode", m_szQuickSDKProductCodeIOS, sizeof(m_szQuickSDKProductCodeIOS));
	
	
	m_dwPHPPort = inifile.GetIntVal("OpenSvr","OpenSvr_PHPPORT");
	m_dwHTTPPort = inifile.GetIntVal("OpenSvr","OpenSvr_HTTPPORT");
	inifile.GetStrVal("OpenSvr", "ServerName", m_szServerName, sizeof(m_szServerName)-1);

	//2 读取平台接入信息
	inifile.GetStrVal("OpenApp","AppID",m_AppID,sizeof(m_AppID));
	inifile.GetStrVal("OpenApp","AppKey",m_AppKey,sizeof(m_AppKey));
	inifile.GetStrVal("OpenApp","WebServerName",m_WebServerName,sizeof(m_WebServerName));

	//3 读取网络相关配置信息
	IOCPHANDLER[0].dwMaxAcceptSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_ACCEPT_SESSION");
	IOCPHANDLER[0].dwMaxConnectSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_CONNECT_SESSION");
	IOCPHANDLER[0].dwSendBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","SEND_BUFFER_SIZE");
	IOCPHANDLER[0].dwRecvBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","RECV_BUFFER_SIZE");
	IOCPHANDLER[0].dwMaxPacketSize = inifile.GetIntVal("SERVER_IOHANDLER","MAX_PACKET_SIZE");
	IOCPHANDLER[0].dwAcceptThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_ACCEPT_THREADS");
	IOCPHANDLER[0].dwConnectThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_CONNECT_THREADS");
	IOCPHANDLER[0].dwTimeout = inifile.GetIntVal("SERVER_IOHANDLER","TIME_OUT");
	IOCPHANDLER[0].bSecurity = inifile.GetIntVal("SERVER_IOHANDLER","SECURITY");

	IOCPHANDLER[1].dwMaxAcceptSession = inifile.GetIntVal("PHP_IOHANDLER","MAX_ACCEPT_SESSION");
	IOCPHANDLER[1].dwMaxConnectSession = inifile.GetIntVal("PHP_IOHANDLER","MAX_CONNECT_SESSION");
	IOCPHANDLER[1].dwSendBufferSize = inifile.GetIntVal("PHP_IOHANDLER","SEND_BUFFER_SIZE");
	IOCPHANDLER[1].dwRecvBufferSize = inifile.GetIntVal("PHP_IOHANDLER","RECV_BUFFER_SIZE");
	IOCPHANDLER[1].dwMaxPacketSize = inifile.GetIntVal("PHP_IOHANDLER","MAX_PACKET_SIZE");
	IOCPHANDLER[1].dwAcceptThread = inifile.GetIntVal("PHP_IOHANDLER","NUMBER_OF_ACCEPT_THREADS");
	IOCPHANDLER[1].dwConnectThread = inifile.GetIntVal("PHP_IOHANDLER","NUMBER_OF_CONNECT_THREADS");
	IOCPHANDLER[1].dwTimeout = inifile.GetIntVal("PHP_IOHANDLER","TIME_OUT");
	IOCPHANDLER[1].bSecurity = inifile.GetIntVal("PHP_IOHANDLER","SECURITY");

	//读取DBC分区配置
	if (!m_cZoneDBCFile.Load("DBC/ZoneEntry.txt"))
	{
		printf("读取分区DBC配置文件失败.\n");
		return FALSE;
	}

	//读取DBC分区配置
	if (!m_ServerConfig.Load("DBC/OpenServerConfig.txt"))
	{
		printf("读取服务器配置文件失败.\n");
		return FALSE;
	}

	//初始化日志对象
	m_pLogfile = new LogFile;
	m_pLogfile->Init("log","OpenSvr",LOG_LEVEL(inifile.GetIntVal("Log","Level")));

#ifdef _DEBUG
	OPENLOG(GUI_LEVEL,"OpenServer(Debug) Starting.");
#else
	OPENLOG(GUI_LEVEL, "OpenServer(Release) Starting.");
#endif

	// 打开崩溃时堆栈打印功能
	INIT_CRASH_REPORTER(LogPrinter);

	std::string logBuffer;
	logBuffer = "Init Objects ......................... ";

	//初始化消息处理器
	logBuffer = "Register Message Handlers ............ ";
	PacketHandler::Instance()->Resister();
	logBuffer += "OK";
	OPENLOG(GUI_LEVEL, logBuffer.c_str());

	ServerSessionFactory::Instance()->Init();
	ServerSessionManager::Instance()->Init();
	PHPSessionFactory::Instance()->Init();

 //	CHttpRequest http_request;
 //	http_request.SendRequest_Get("192.168.1.111", 7878, "/test2", "");

	//注册Http消息处理函数
	Register();

	//得到处理器数量
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DWORD dwProcessNum = si.dwNumberOfProcessors;

	//初始化Http连接
	logBuffer = "Http Connect Init .................. ";
	wchar_t wszWebServerName[64];

	MultiByteToWideChar(CP_ACP,0, (LPCSTR)GetWebServerName(), -1, wszWebServerName, sizeof(wszWebServerName)/sizeof(wchar_t));

	// 初始化Http操作相关
	if (!m_HttpRequestManager.Initialize(wszWebServerName, m_dwHTTPPort, 2 * dwProcessNum + 2, HttpCallBackQueryResult))
	{
		logBuffer += "Failed";
		OPENLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}

	logBuffer += "OK";
	OPENLOG(GUI_LEVEL, logBuffer.c_str());

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

	desc[1].dwIoHandlerKey = PHP_IOHANDLER;
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
	if (!m_pIOCPServer->Init(desc,2))
	{
		logBuffer += "Failed";
		OPENLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	logBuffer += "OK";
	OPENLOG(GUI_LEVEL, logBuffer.c_str());

	StartServerListen();
	StartPHPListen();

	//初始化完成
	OPENLOG(GUI_LEVEL, "OpenServer Init Finished!");
	m_dwAuthTime = (DWORD)time(NULL);
	return TRUE;
}

void OpenServer::Release()
{
	if(m_pIOCPServer)
	{
		m_pIOCPServer->Shutdown();
		delete m_pIOCPServer;
		m_pIOCPServer = NULL;
	}


	PacketHandler::DestroyInstance();
	ServerSessionManager::Instance()->Release(ServerSessionFactory::Instance());
	ServerSessionManager::DestroyInstance();
	ServerSessionFactory::Instance()->Release();
	ServerSessionFactory::DestroyInstance();
	PHPSessionFactory::DestroyInstance();

	if(m_pLogfile)
	{
		m_pLogfile->ShutDown();
		delete m_pLogfile;
		m_pLogfile = NULL;
	}
}

void OpenServer::Run()
{
	int nSleepCnt = 0;
	DWORD dwStatistics = 0;
	DWORD dwFps = 0;
	char szMsg[256];

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
			OPENLOG(ERROR_LEVEL, "Do Update Cost[%d]MS", dwDoUpdateAfterTick - curTick);

		if (dwStatistics >= 1000)
		{
			sprintf(szMsg, "[%s]Online_Player:%d Fps:%d Tick:%lld", m_szServerName,
				PHPSessionManager::Instance()->GetPlayerNum(), dwFps, curTick);
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

BOOL OpenServer::Update()
{
	if(m_bShutDown)
		return FALSE;

	m_pIOCPServer->Update();

	m_HttpRequestManager.Update();

	PHPSessionManager::Instance()->Update();

	//DeleteTimeoutAcc();
	return TRUE;
}

void OpenServer::StartServerListen()
{
	if(!m_pIOCPServer->IsListening(SERVER_IOHANDLER))
	{
		m_pIOCPServer->StartListen(SERVER_IOHANDLER,m_szServerGateIP,(WORD)m_dwServerGatePort);

		OPENLOG(GUI_LEVEL, "OpenServer Start Server Listening.");
	}
}

void OpenServer::StartPHPListen()
{
	if(!m_pIOCPServer->IsListening(PHP_IOHANDLER))
	{
		m_pIOCPServer->StartListen(PHP_IOHANDLER,m_szPHPIP,(WORD)m_dwPHPPort);

		OPENLOG(GUI_LEVEL, "OpenServer Start PHP Listening.");
	}
}

void OpenServer::SendMsgToMapServer(DWORD dwGUID, NetMsgBody * pMsg, DWORD dwServerId)
{
	MapServerSession *pSession = ServerSessionManager::Instance()->FindMapServer(dwServerId);
	if (!pSession)
	{
		OPENLOG(GUI_LEVEL, "找不到逻辑服务器session[serverid:%d]!", dwServerId);
		return;
	}
	pSession->SendPacket(dwGUID, pMsg->GetType(), pMsg);
}


NetworkObject * CreateServerSideAcceptedObject()
{
	return ServerSessionFactory::Instance()->AllocServerSession(MAP_SERVER);
}

VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject )
{
	ServerSessionFactory::Instance()->FreeServerSession((ServerSession*)pNetworkObject);
}

VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject )
{

}

NetworkObject * CreateClientSideAcceptedObject()
{
	return PHPSessionFactory::Instance()->AllocPHPSession();
}



VOID DestroyClientSideAcceptedObject( NetworkObject * pNetworkObject )
{
	PHPSessionFactory::Instance()->FreePHPSession((ServerSession*)pNetworkObject);
}

VOID DestroyClientSideConnectedObject( NetworkObject * pNetworkObject )
{

}

void LogPrinter(const char* msg)
{
	g_OpenServer.writeLog(ERROR_LEVEL, msg);
}

void OpenServerThread::EndThread()
{
	g_OpenServer.SetShutDown(TRUE);
}

DWORD OpenServerThread::run()
{
	IF_NOT (g_OpenServer.Init())
	{
		g_OpenServer.SetShutDown();
		OPENLOG(GUI_LEVEL, "");
		OPENLOG(GUI_LEVEL, "Please Check the Configuration and Reboot the Server!");
	}

	g_OpenServer.Run();

	g_OpenServer.Release();

	return 0;
}

BOOL OpenServerThread::IsShutDown()
{
	return g_OpenServer.IsShutDown();
}

void OpenServer::writeLog(LOG_LEVEL eLevel, const char* log)
{
	IF_NOT (log)
		return;

	if (m_pLogfile != NULL)
	{
		m_pLogfile->InsertLog(eLevel,log);
	}

	//if (eLevel == GUI_LEVEL)
	//{
		time_t t = time(NULL);
		tm curTime;
		localtime_s(&curTime,&t);
		char szTime[32] = {0};
		sprintf_s(szTime, sizeof(szTime), "%d:%d:%d",curTime.tm_hour, curTime.tm_min, curTime.tm_sec);
		printf("[%s]%s\n",szTime,log);
	//}
}

void OpenServer::HttpOpenQueryResult( OPEN_QUERY_INDEX dwIndex, HttpQueryResult* pData )
{
	if(!pData)
	{
		return;
	}

	fnProcessQueryResult pfnHandler =  Find(dwIndex);

	if(pfnHandler)
		pfnHandler(pData);
	else
	{
		//写日志
		OPENLOG(ERROR_LEVEL,"无效的dwIndex=%d",dwIndex);
	}

	//释放资源
	pData->Release();
}

BOOL OpenServer::HttpOpenQuery(HttpQueryResult * pData)
{
	if(!pData)
		return FALSE;

	if (!m_HttpRequestManager.HttpQuery(pData))
	{
		//写日志
		OPENLOG(ERROR_LEVEL,"HttpOpenQuery执行失败");
		return FALSE;
	}

	return TRUE;
}

DWORD OpenServer::GetServerID(DWORD dwZoneID)
{
	ZoneEntry* pZoneEntry = (ZoneEntry*)m_cZoneDBCFile.GetRecord(dwZoneID);
	if (!pZoneEntry)
		return 0;

	return pZoneEntry->m_dwServerID;
}

const OpenServer_Config* OpenServer::GetServerConfig(DWORD dwKey)
{
	OpenServer_Config* pServerCfg = (OpenServer_Config*)m_ServerConfig.GetRecord(dwKey);
	return pServerCfg ? pServerCfg : NULL;
}			

void OpenServer::Register()
{
	//在这里添加消息处理函数
 	AddHandler(OPEN_QUERY_AUTH,				HttpResultHandler::ProcessMobileValidateResult);
 	AddHandler(OPEN_QUERY_LOAD_POINTCFG,    HttpResultHandler::ProcessPointCfgLoadResult);
	AddHandler(HTTP_REYUN_STATISTIC,		HttpResultHandler::ProcessReyunStatisticResult);
	AddHandler(HTTP_CHECK_USER_INFO,		HttpResultHandler::ProcessCheckUserInfoResult);
	AddHandler(HTTP_RECHARGE_SUCCESS_REPORT,HttpResultHandler::RechargeSuccessReportResult);
	AddHandler(HTTP_STATISTICS_REPORT,		HttpResultHandler::StatisticsReportResult);
// 	AddHandler(OPEN_QUERY_WEB_COMMON,		HttpResultHandler::ProcessWebCommonResult);	
}

void OpenServer::AddHandler(DWORD dwIndex, fnProcessQueryResult fnHandler)
{
	m_ProcessQueryMap.insert(std::make_pair(dwIndex,fnHandler));
}

fnProcessQueryResult OpenServer::Find(DWORD dwIndex)
{
	ProcessQueryResultMap::iterator it = m_ProcessQueryMap.find(dwIndex);
	if(it==m_ProcessQueryMap.end())
		return NULL;

	return it->second;
}

// void OpenServer::SendMsgToMapServer(DWORD dwServerId,DWORD dwGUID, google::protobuf::Message *pMsg)
// {
// // 	MapServerSession* pMapSession = ServerSessionManager::Instance()->FindMapServer(dwServerId);
// // 	if (pMapSession)
// // 	{
// // 		pMapSession->SendPacket(dwGUID,pMsg);
// // 	}
// // 	else
// // 	{
// // 		OPENLOG(ERROR_LEVEL,"请求登录返回时找不到场景Session[GUID:%d]",dwGUID);
// // 	}	
// }

// 第三方Http请求回调
void HttpCallBackQueryResult( DWORD dwIndex, HttpQueryResult * pData )
{
	g_OpenServer.HttpOpenQueryResult((OPEN_QUERY_INDEX)dwIndex,pData);
}




HttpResultHandler::HttpResultHandler(void)
{

}

HttpResultHandler::~HttpResultHandler(void)
{

}

void HttpResultHandler::ProcessMobileValidateResult(HttpQueryResult* pData)
{
	HttpAuthenticationCode* pQuery = (HttpAuthenticationCode*)pData;

 	int nError_Code = -1;
 	string szError_Msg = "未知错误";
	string szMobile;
 
 	// 正常响应
 	if (pQuery->GetQueryResult())
 	{
 		Json::Reader reader;
 		Json::Value root;
		Json::Value default;
 		// reader将Json字符串解析到root，root将包含Json里所有子元素
		if (reader.parse(pData->GetHttpResult(), root))
 		{
			// 取结果
			Json::Value result = root.get("result", default);
			if (!result.isNull() && result.isInt())
				nError_Code = result.asInt();		
// 			if (root.isMember("result") && root["result"].isInt())
// 				nError_Code = root["result"].asInt();

			// 取错误描述
			Json::Value errmsg = root.get("errmsg", default);
			if (!errmsg.isNull() && errmsg.isString())
				szError_Msg = errmsg.asCString();

			// 取手机号
			Json::Value ext = root.get("ext", default);
			if (!ext.isNull() && ext.isString())
				szMobile = ext.asCString();
 		}
 		else
 		{
 			OPENLOG(ERROR_LEVEL,"解析取认证结果内容出错");
 		}		
 	}
 	else
 	{
 		nError_Code = 1;
 		OPENLOG(ERROR_LEVEL,"手机号认证目标服务器无效!");
 	}
 
 	// 响应消息MapServer
	MSG_O2M_AUTHENTICATION_CODE_RESP msg;
	msg.m_wCode = pQuery->GetCode();
	msg.m_nErrorCode = nError_Code;
	strcpy_s(msg.m_szMobile, sizeof(msg.m_szMobile) - 1, pQuery->GetMobile());
	g_OpenServer.SendMsgToMapServer(pQuery->GetGUID(), &msg);
 
 	// 日志输出
 	if (0 == nError_Code)
 	{
 //#ifdef _DEBUG
		OPENLOG(GUI_LEVEL, "[取认证码成功:%s]", pQuery->GetMobile());
 //#endif
 	}
 	else
 	{
 		char szErrorText[256] = {0};
 		ChangeLocalChar(szError_Msg.c_str(), szErrorText, sizeof(szErrorText)-1);	
 		OPENLOG(ERROR_LEVEL,"[Mobile:%s, ErrorCode:%d,ErrorText:%s]", pQuery->GetMobile(),nError_Code, szErrorText);
 	}

}


void HttpResultHandler::ProcessWebCommonResult(HttpQueryResult* pData)
{
// 	HttpWebCommonData* pQuery = (HttpWebCommonData*)pData;
// 
// 	int nReturn_Code = 0;
// 	std::string strReturn_Msg = "未知错误";
// 	std::string strDataBase64 = "";
// 
// 	// 正常响应
// 	if (pQuery->GetQueryResult())
// 	{
// 		Json::Reader reader;
// 		Json::Value root;
// 		// reader将Json字符串解析到root，root将包含Json里所有子元素
// 		if (reader.parse(pData->GetHttpResult(), root))
// 		{
// 			if (root.isMember("code") && !root["code"].isNull())
// 			{
// 				nReturn_Code = atoi(root["code"].asCString());
// 			}
// 
// 			if (root.isMember("msg") && !root["msg"].isNull())
// 			{
// 				strReturn_Msg = root["msg"].asCString();
// 			}
// 
// 			if (root.isMember("data") && !root["data"].isNull())
// 			{
// 				strDataBase64 = root["data"].asCString();
// 			}
// 
// //#ifdef _DEBUG
// 			OPENLOG(GUI_LEVEL,"[ProcessWebCommonResult]HTTP请求成功返回[MsgId:%d,code:%d]",pQuery->GetMSGID(),nReturn_Code);
// //#endif
// 		}
// 		else
// 		{
// 			nReturn_Code = HTTP_TEXT_INVALID;
// 			OPENLOG(ERROR_LEVEL,"[ProcessWebCommonResult]解析Http请求结果内容出错[HttpWebCommonData(MsgId:%d,data:%s)]", 
// 				pQuery->GetMSGID(),pQuery->GetHttpResult());
// 		}
// 	}
// 	else
// 	{
// 		nReturn_Code = HTTP_URL_INVALID;
// 		OPENLOG(ERROR_LEVEL,"[HttpWebCommonData]Http请求目标url或服务器无效[(MsgId:%d)]!",pQuery->GetMSGID());
// 	}
// 
// 	// 响应消息MapServer
// 	SS::Msg_LogicCommon_O2M msg;
// 	msg.set_msg_id(pQuery->GetMSGID());
// 	msg.set_msg_code(nReturn_Code);
// 	msg.set_msg_body(strDataBase64.data());
// 	g_OpenServer.SendMsgToMapServer(1, pQuery->GetGUID(), &msg);
}


// 检测用户登录的结果(通用)
// void HttpResultHandler::CheckUserLoginResult_Common(HttpCheckUser* pQuery)
// {
// 	DWORD dwCPChannelType = pQuery->GetCPChannelType();
// 	DWORD dwSDKChannelType = pQuery->GetSDKChannelType();
// 	string strHttpResult = pQuery->GetHttpResult();
// 
// 	Json::Reader reader;
// 	Json::Value root;
// 	Json::Value default;
// 	// reader将Json字符串解析到root，root将包含Json里所有子元素
// 	if (!reader.parse(strHttpResult, root))
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果出错[Guid:%d,CPChannelType:%d]!", __FUNCTION__, pQuery->GetKey(), dwCPChannelType);
// 		OPENLOG(ERROR_LEVEL, "[%s]", strHttpResult.c_str());
// 		return;
// 	}
// 
// 	// 取结果 
// 	Json::Value result = root.get("result", default);
// 	if (result.isNull() || !result.isInt())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果result字段失败!", __FUNCTION__);
// 		return;
// 	}
// 	int nResult = result.asInt();
// 
// 	// 取错误描述
// 	Json::Value desc = root.get("desc", default);
// 	if (desc.isNull() || !desc.isString())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果desc字段失败!", __FUNCTION__);
// 		return;
// 	}
// 	string strDesc = desc.asString();
// 
// 	// 日志输出
// 	if (1 != nResult)
// 	{
// 		OPENLOG(ERROR_LEVEL, "用户登录校验失败[cp_channel_type:%d, result:%d,desc:%s]", 
// 			dwCPChannelType, nResult, strDesc.c_str());
// 		return;
// 	}
// 
// 	// 取错误描述
// 	Json::Value account = root.get("account", default);
// 	if (desc.isNull() || !desc.isString())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果desc字段失败!", __FUNCTION__);
// 		return;
// 	}
// 	string strAccount = account.asString();
// 
// 	// 响应消息MapServer
// 	MSG_O2M_CHECK_USER_INFO_RESP msg;
// 	msg.m_dwCPChannelType = dwCPChannelType;
// 	msg.m_dwSDKChannelType = dwSDKChannelType;
// 	msg.m_byTerminalType = pQuery->GetTerminalype();
// 	msg.m_nResult = nResult;
// 	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount), strAccount.c_str());
// 	strcpy_s(msg.m_szToken, sizeof(msg.m_szToken), pQuery->GetToken());
// 	strcpy_s(msg.m_szIP, sizeof(msg.m_szIP), pQuery->GetIP());
// 	strcpy_s(msg.m_szDeviceId, sizeof(msg.m_szDeviceId), pQuery->GetDeviceId());
// 	g_OpenServer.SendMsgToMapServer(pQuery->GetGUID(), &msg);
// }

// 检测用户登录的结果(翔通)
// void HttpResultHandler::CheckUserLoginResult_XMGT(HttpCheckUser* pQuery)
// {
// 	DWORD dwCPChannelType = pQuery->GetCPChannelType();
// 	DWORD dwSDKChannelType = pQuery->GetSDKChannelType();
// 	string strHttpResult = pQuery->GetHttpResult();
// 	//const char *pAcount = pQuery->GetAccount();
// 
// 	Json::Reader reader;
// 	Json::Value root;
// 	Json::Value default;
// 	// reader将Json字符串解析到root，root将包含Json里所有子元素
// 	if (!reader.parse(strHttpResult, root))
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果出错!",__FUNCTION__);
// 		return;
// 	}
// 
// 	// 取结果 
// 	Json::Value status = root.get("status", default);
// 	if (status.isNull() || !status.isInt())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果status字段失败!",__FUNCTION__);
// 		return;
// 	}
// 	int nResult = status.asInt();
// 
// 	// 取错误描述
// // 	Json::Value msg = root.get("msg", default);
// // 	if (msg.isNull() || !msg.isString())
// // 	{
// // 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果msg字段失败!", __FUNCTION__);
// // 		return;
// // 	}
// // 	string strMsg = msg.asString();
// 
// 	// 登录校验失败
// 	if (0 != nResult)
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]IOS用户登录校验失败[path:%s,result:%d]",__FUNCTION__,pQuery->GetPath(), nResult);
// 		return;
// 	}
// 
// 	// 登录校验成功
// 	Json::Value data = root.get("data", default);
// 	if (data.isNull() || !data.isObject())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果data字段失败!",__FUNCTION__);
// 		return;
// 	}
// 
// 	// 唯一标示
// 	Json::Value id = data.get("id", default);
// 	if (id.isNull() || !id.isInt())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果data_id字段失败!", __FUNCTION__);
// 		return;
// 	}
// 
// 	// 用户名
// 	Json::Value name = data.get("name", default);
// 	if (name.isNull() || !name.isString())
// 	{
// 		OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果data_name字段失败!", __FUNCTION__);
// 		return;
// 	}
// 
// 	// 响应消息MapServer
// 	MSG_O2M_CHECK_USER_INFO_RESP msg_resp;
// 	msg_resp.m_dwCPChannelType = dwCPChannelType;
// 	msg_resp.m_dwSDKChannelType = dwSDKChannelType;
// 	msg_resp.m_byTerminalType = pQuery->GetTerminalype();
// 	msg_resp.m_nResult = 1;	// 统一错误码
// 	sprintf_s(msg_resp.m_szAccount, sizeof(msg_resp.m_szAccount), "%d", id.asInt());
// 	strcpy_s(msg_resp.m_szToken, sizeof(msg_resp.m_szToken), pQuery->GetToken());
// 	strcpy_s(msg_resp.m_szIP, sizeof(msg_resp.m_szIP), pQuery->GetIP());
// 	strcpy_s(msg_resp.m_szDeviceId, sizeof(msg_resp.m_szDeviceId), pQuery->GetDeviceId());
// 	g_OpenServer.SendMsgToMapServer(pQuery->GetGUID(), &msg_resp);
// }


void  HttpResultHandler::ProcessPointCfgLoadResult(HttpQueryResult* pData)
{
	OPENLOG(ERROR_LEVEL, "Http请求响应(index:%d, cost:%d)", pData->GetIndex(), GetTickCount() - pData->GetRequestTime());
}

void  HttpResultHandler::ProcessReyunStatisticResult(HttpQueryResult* pData)
{
	HttpReyunStatisticNotify* pQuery = (HttpReyunStatisticNotify*)pData;

	int nStatus = -1;
	string strResult = "未知错误";

	// 正常响应
	if (pQuery->GetQueryResult())
	{
		Json::Reader reader;
		Json::Value root;
		Json::Value default;
		// reader将Json字符串解析到root，root将包含Json里所有子元素
		if (reader.parse(pData->GetHttpResult(), root))
		{
			// 取结果 
			Json::Value status = root.get("status", default);
			if (!status.isNull() && status.isInt())
				nStatus = status.asInt();

			// 取错误描述
			Json::Value result = root.get("result", default);
			if (!result.isNull() && result.isString())
				strResult = result.asString();
			
			// 日志输出
			if (0 == nStatus)
			{
#ifdef _DEBUG
				OPENLOG(GUI_LEVEL, "行为统计成功[path:%s]", pQuery->GetPath());
#endif
			}
			else
			{
				OPENLOG(ERROR_LEVEL, "行为统计失败[path:%s, status:%d,result:%s]",
					pQuery->GetPath(), nStatus, strResult.c_str());
			}
		}
		else
		{
			OPENLOG(ERROR_LEVEL, "解析行为统计结果内容出错");
		}
	}
	else
	{
		OPENLOG(ERROR_LEVEL, "行为统计目标服务器无效!");
	}
}

void  HttpResultHandler::ProcessCheckUserInfoResult(HttpQueryResult* pData)
{
	HttpCheckUser* pQuery = (HttpCheckUser*)pData;

	DWORD dwCPChannelType = pQuery->GetCPChannelType();
	DWORD dwSDKChannelType = pQuery->GetSDKChannelType();
	string strHttpResult = pQuery->GetHttpResult();
	int nResult = 0;
	string strAccount;

	OPENLOG(GUI_LEVEL, "检测用户信息结果[cp_channel:%d,sdk_channel:%d,TerminalType:%d]",
		dwCPChannelType, dwSDKChannelType, pQuery->GetTerminalype());

	// 走通用逻辑 普通渠道ID段: 20000 - 29999
	if (dwCPChannelType >= 20000)
	{
		//CheckUserLoginResult_Common(pQuery);

		Json::Reader reader;
		Json::Value root;
		Json::Value default;
		// reader将Json字符串解析到root，root将包含Json里所有子元素
		if (!reader.parse(strHttpResult, root))
		{
			OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果出错[Guid:%d,CPChannelType:%d]!", 
				__FUNCTION__, pQuery->GetKey(), dwCPChannelType);
			//OPENLOG(ERROR_LEVEL, "[%s]", strHttpResult.c_str());
			return;
		}

		// 取结果 
		Json::Value result = root.get("result", default);
		if (result.isNull() || !result.isInt())
		{
			OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果result字段失败!", __FUNCTION__);
			return;
		}
		nResult = result.asInt();

		// 取错误描述
		Json::Value desc = root.get("desc", default);
		if (desc.isNull() || !desc.isString())
		{
			OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果desc字段失败!", __FUNCTION__);
			return;
		}
		string strDesc = desc.asString();

		// 日志输出
		if (1 != nResult)
		{
			OPENLOG(ERROR_LEVEL, "用户登录校验失败[cp_channel_type:%d, result:%d,desc:%s]",
				dwCPChannelType, nResult, strDesc.c_str());
			return;
		}

		// 取错误描述
		Json::Value account = root.get("account", default);
		if (desc.isNull() || !desc.isString())
		{
			OPENLOG(ERROR_LEVEL, "[%s]解析用户登录校验结果desc字段失败!", __FUNCTION__);
			return;
		}
		strAccount = account.asString();
	}
	// quick_sdk  quicksdk专用ID段: 10000 - 19999
	else
	{
		// 取响应正文
		int nIdx = (int)strHttpResult.find("\r\n\r\n");
		if (nIdx >= 0)
		{
			string strResult = strHttpResult.substr(nIdx + 4, 1);
			nResult = atoi(strResult.c_str());
		}
		strAccount = pQuery->GetAccount();
	}

	// 响应消息MapServer
	MSG_O2M_CHECK_USER_INFO_RESP msg;
	msg.m_dwCPChannelType = dwCPChannelType;
	msg.m_dwSDKChannelType = dwSDKChannelType;
	msg.m_byTerminalType = pQuery->GetTerminalype();
	msg.m_nResult = nResult;
	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount), strAccount.c_str());
	strcpy_s(msg.m_szToken, sizeof(msg.m_szToken), pQuery->GetToken());
	strcpy_s(msg.m_szIP, sizeof(msg.m_szIP), pQuery->GetIP());
	strcpy_s(msg.m_szDeviceId, sizeof(msg.m_szDeviceId), pQuery->GetDeviceId());
	g_OpenServer.SendMsgToMapServer(pQuery->GetGUID(), &msg);
}

void  HttpResultHandler::RechargeSuccessReportResult(HttpQueryResult* pData)
{
	HttpRechargeSuccessReport* pQuery = (HttpRechargeSuccessReport*)pData;
	
	const char *pHttpResult = pQuery->GetHttpResult();

	Json::Reader reader;
	Json::Value json_object;
	if (!reader.parse(pHttpResult, json_object))
	{
		OPENLOG(ERROR_LEVEL, "[%s]解析充值成功上报结果内容出错[%s]", __FUNCTION__, pHttpResult);
		return;
	}

	int nResult = 0;
	if (!Json2Int(json_object, "rcode", __FUNCTION__, nResult))
		return;

	string strDesc = "";
	if (!Json2String(json_object, "msg", __FUNCTION__, strDesc))
		return;

// 	int nResult = 0;
// 	if (!Json2Int(json_object, "result", __FUNCTION__, nResult))
// 		return;
// 
// 	string strDesc = "";
// 	if (!Json2String(json_object, "desc", __FUNCTION__, strDesc))
// 		return;

	OPENLOG(GUI_LEVEL, "充值成功上报结果[result:%d,desc:%s]", nResult, strDesc.c_str());

}

void  HttpResultHandler::StatisticsReportResult(HttpQueryResult* pData)
{
	HttpStatisticsReport* pQuery = (HttpStatisticsReport*)pData;

	const char *pHttpResult = pQuery->GetHttpResult();

	Json::Reader reader;
	Json::Value json_object;
	if (!reader.parse(pHttpResult, json_object))
	{
		OPENLOG(ERROR_LEVEL, "[%s]解析StatisticsReportResult格式结果内容出错", __FUNCTION__);
		return;
	}

	int nResult = 0;
	if (!Json2Int(json_object, "result", __FUNCTION__, nResult))
		return;

	if (nResult != 200)
	{
		OPENLOG(ERROR_LEVEL, "[%s]数据统计上报结果出错", __FUNCTION__);
	}
}