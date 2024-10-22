#include "stdafx.h"
#include "MapServer.h"
#include "DBThread.h"
#include "PacketHandler.h"
#include "GateServerSession.h"
#include "OpenServerSession.h"
#include "QueryForUser.h"
#include "DBResultHandler.h"
#include "ObjectFactory.h"
#include "AccountInfoManager.h"
#include "PlayerManager.h"
#include "MapSvrLuaErrorProc.h"
#include "TimeoutManager.h"
#include "SysTimesManager.h"
#include "ChatManager.h"
#include "StopMessage.h"
#include "GMManager.h"
//#include "ServerRestrictions.h"
#include "../Common/DataDefine.h"
#include "IOCPServer.h"
#include "../LuaEngine/LuaInterface.h"
#include "../ToolKit/IniFile.h"
#include "../ToolKit/DBCFileManager.h"
#include "../ToolKit/CrashReporter.h"
#include "../ToolKit/MemoryPool.h"
#include "../ToolKit/Rand.h"
#include "SqlDatabase.h"
#include "ConfigManager.h"
#include "MapSvrQuery.h"
#include "AccountInfo.h"
#include "../ToolKit/MiniDump.h"
#include "KeyWodFilter/KeyWodFilter.h"
#include "../Common/CommonLogic.h"

// ����Protobuff�����
#include <iostream>
#include <fstream>
#include <string>
#include "header.pb.h"
#include "Player.h"
#include "server.pb.h"
#include "NetBase.h"
//#include "SceneManager.h"
#include "login.pb.h"
#include "server.pb.h"
//#include "UserPointRule.pb.h"
#include <atlenc.h>
#include "PlayerBaseDataManager.h"
#include "Rebel/RebelManager.h"
#include "Rank/RankManager.h"
#include "Land/LandManager.h"
#include "Guild/GuildManager.h"
#include "WorldBoss/WorldBossManager.h"
#include "Arena/ArenaManager.h"
#include "Warcraft/WarcraftManager.h"
#include "Framework/TimeManager.h"
#include "PVP/PVPManager.h"
#include "AttackCity/AttackCityManager.h"
#include "Jsoncpp/include/json/value.h"
#include "KingMatch/KingMatchManager.h"
#include "TeamDungeon/TeamDungeonManager.h"
#include "Activity/Christmas.h"
#include "GameLogic.h"
#include "Turntable/TurntableManager.h"
#include "ScoreMatch/ScoreMatchManager.h"
#include "ResourceWar/ResourceWarManager.h"
#include "Guild/GuildWarMgr.h"

//����������ؿ��ļ�
#ifdef _DEBUG		
#	if defined(_M_IX86)	
//#		pragma comment(lib,"stlportd.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine_d.lib")
#		pragma comment(lib,"../bin/ToolKit_d.lib")
#		pragma comment(lib,"../bin/Common_d.lib")
#		pragma comment(lib,"../bin/LuaEngine_d.lib")
#		pragma comment(lib,"../bin/MySqlDB_d.lib")
#		pragma comment(lib,"../bin/Jsoncpp_d.lib")
#	elif defined(_M_X64)	
//#		pragma comment(lib,"stlport_x64d.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64_d.lib")
#		pragma comment(lib,"../bin/ToolKit64_d.lib")
#		pragma comment(lib,"../bin/Common64_d.lib")
#		pragma comment(lib,"../bin/LuaEngine64_d.lib")
#		pragma comment(lib,"../bin/MySqlDB64_d.lib")
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
#		pragma comment(lib,"../bin/LuaEngine.lib")
#		pragma comment(lib,"../bin/MySqlDB.lib")
#		pragma comment(lib,"../bin/Jsoncpp.lib")
#	elif defined(_M_X64)	
//#		pragma comment(lib,"stlport_x64.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64.lib")
#		pragma comment(lib,"../bin/ToolKit64.lib")
#		pragma comment(lib,"../bin/Common64.lib")
#		pragma comment(lib,"../bin/LuaEngine64.lib")
#		pragma comment(lib,"../bin/MySqlDB64.lib")
#		pragma comment(lib,"../bin/Jsoncpp64.lib")
#		pragma comment(lib,"../bin/libprotobuf64.lib") 
#	else	
#		error "Can not supported on this architecture."
#	endif	
#endif	


#define ROLENAME_FILE "RoleNameFilter.txt"
#define SQLCMD_FILE "SqlCmdFilter.txt"
#define CHAT_FILE "ChatFilter.txt"

//����ȫ�ֵ�ͼ����������
MapServer g_MapServer;
//����ȫ�ֵ�ͼ�������̶߳���
MapServerThread g_MapServerThread;

//�������������ûص������ӿ�
NetworkObject * CreateServerSideAcceptedObject();
VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject );
VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject );

//������־����д����־�ӿ�
void LogPrinter(const char* msg);

//����DB��ѯ�������ص��ӿ�
void Map_CallBackQueryResult( DWORD dwIndex, QueryResult * pData );

MapServer::MapServer()
{
// 	char szSignUTF8[128] = { 0 };
// 	char *pppp = "30db206bfd3fea7ef0db929998642c8ea54cc7042a779c5a0d9897358f6e9505";
// 	UrlEncode(pppp, szSignUTF8, 128, 'a');
// 
// 	char szBuff[2048] = { 0 };
// 	HttpPostRequest("https://yun.tim.qq.com/v5/tlssmssvr/sendsms", 33333, "/post_test", "", szBuff, 2048);
	
	//HttpPostRequest("192.168.1.111", 33333, "post_test", "", szBuff, 2048);
	//int nMax = 0;
	//int nRandValue = g_Rand(1, nMax);
	// DWORD dwGoBackPlayerNum = GetGoBackPlayerNum();

	int nHeroSize = sizeof(HeroInfo);
	int nGuildSize = sizeof(GuildData);
	int nGuildBaseSize = sizeof(GuildBaseData);

	memset(m_szDBIP, 0, sizeof(m_szDBIP));
	memset(m_szDBName, 0, sizeof(m_szDBName));
	memset(m_szUserName, 0, sizeof(m_szUserName));
	memset(m_szPassword, 0, sizeof(m_szPassword));
	m_pIOCPServer		 = NULL;
	m_pGateServerSession = NULL;
	memset(m_szServerGateIP, 0, sizeof(m_szServerGateIP));
	m_dwServerGatePort	 = 0;
	m_pOpenServerSession = NULL;
	memset(m_szServerOpenIP, 0, sizeof(m_szServerOpenIP));
	m_dwServerOpenPort	 = 0;
	m_bConOpenSvrFlg	 = FALSE;
	m_bShutDown			 = FALSE;
	m_pLogfile			 = NULL;
	m_dwServerID		 = 0;
	m_dwOpenSvrTime		 = 0;
	m_bIsMerged			 = FALSE;
	m_ProcessQueryMap.clear();
	memset(&m_tGameConfig, 0, sizeof(m_tGameConfig));
	m_lProcessSaveTimer = 0;
	m_dwCurDBThreadIndex = 0;
	m_lSaveUpdatePlayerTime = 0;
	memset(m_szServerName, 0, sizeof(m_szServerName));
	//memset(m_szReYunAppId, 0, sizeof(m_szReYunAppId));
	m_dwTestSendMsgToNodejsTimes = 1;
	m_bLoadServerList = false;
	m_bIsUseOldPower = false;
	m_ServerList.clear();
	m_dwCurMaxOnlineRoleNum = 0;
}

MapServer::~MapServer(void)
{

}

// �������ݿ�
 void TestMySql()
 {	
	struct TempData
	{
		DWORD dwRoleId;
		char szRoleName[32];
		int nLevel;
	};

	int nTempDatSize = sizeof(TempData);

	TempData data;
	memset(&data,0,sizeof(data));
	data.dwRoleId = 44;
	data.nLevel = 55;
	strcpy(data.szRoleName,"KevinChen");

 	SqlDatabase db;
 	db.Connect("jinxikeji","192.168.1.94","root","debug123456");
	char szFrom[64] = {0};
	strcpy(szFrom,"Kevin\"Chen");
	int nFromLen = strlen(szFrom);
	char szTo[sizeof(TempData)*2+1] = {0};
	//char szTo1[64] = {0};
	//memcpy(szTo1, (char*)&data, sizeof(TempData));
	int nResult = db.escape_string(szTo, (char*)&data, nTempDatSize);
	int nToLen = strlen(szTo);

	DWORD dwUserId = 3;

	char *pQuery = db.GetQuery();
	sprintf(pQuery, "Insert Into product_info(UserId,ProductId,ProductInfo) VALUE (%d,%d,'%s')", dwUserId,2,szTo);
	int nRet = db.Execute();
	if(nRet == -1)
	{
		int nErrorNo = db.errorno();
		const char *pszErrorDesc = db.error();
		return;
	}
	db.CloseRecordset();
	
	sprintf(pQuery, "select * from product_info where UserId=%d",dwUserId);
	nRet = db.OpenRecordset();
	if(nRet == -1)
	{
		int nErrorNo = db.errorno();
		const char *pszErrorDesc = db.error();
		return;
	}
	TempData selectdata;
	memset(&selectdata,0,sizeof(selectdata));
	if(db.Fetch())
	{
		DWORD dwUserId;
		DWORD dwProductId;
		db.GetDWORD(0,	dwUserId);
		db.GetDWORD(1,	dwProductId);
		db.GetBlob(2,	(unsigned char*)&selectdata, sizeof(TempData)-4);
	}

	TempData *pTemp = (TempData *)&selectdata;
	DWORD dwRoleId = pTemp->dwRoleId;
 }


 void TestKeyWordFilter()
 {
	 char szBuff[64] = {0};
	 strcpy(szBuff, "����Ʒ������Kevin");

	 if (g_MapServer.MatchRoleName(szBuff))
	 {
		 MAPLOG(GUI_LEVEL, "��������!");
	 }
	 else
	 {
		MAPLOG(GUI_LEVEL, "��������!");	
	 }
 }

//��ʼ����ͼ������
BOOL MapServer::Init()
{
	m_dwCurMaxOnlineRoleNum = 0;
	m_bIsLoadRoleBaseData = false;
	m_bIsWriteLog = FALSE;
	m_bTestRobot = false;
	m_dwMaxPlayerNum = 9999;
	m_OpenBeat.m_dwConnectNum = 0;
	m_GateBeat.m_dwConnectNum = 0;
	m_lProcessNewDayTimer = GetTickCount64();
	time_t t = time(NULL);
	tm localtime;
	localtime_s(&localtime,&t);
	m_nLastYearDay = localtime.tm_yday;
	m_WaitingLoginAccountList.clear();
	m_AccumulatePointsRule.clear();
	m_bDoRankActivityReward = true;
	m_lProcessSaveTimer = GetTickCount64();
	m_lSaveUpdatePlayerTime = GetTickCount64();
	//m_dwOpenServerFundBuyTimes = 0;
	memset(&m_GlobalData, 0, sizeof(m_GlobalData));
	m_GlobalDataList.clear();
	m_lRecordOfflinePlayerNumTime = GetTickCount64();

	//���������������
	srand(GetTickCount());
	//����ini�����ļ�
	IniFile inifile;
	inifile.Init("MapSvr.ini");

	//1 ���ط���������
	inifile.GetStrVal("GateSvr","GateSvr_ServerIP",m_szServerGateIP,sizeof(m_szServerGateIP));
	m_dwServerGatePort = inifile.GetIntVal("GateSvr","GateSvr_ServerPORT");

	//2 ���ŷ���������
	m_bConOpenSvrFlg = (BOOL)inifile.GetIntVal("OpenSvr","ConnectToOpenSvr");
	inifile.GetStrVal("OpenSvr","OpenSvr_ServerIP",m_szServerOpenIP,sizeof(m_szServerOpenIP));
	m_dwServerOpenPort = inifile.GetIntVal("OpenSvr","OpenSvr_ServerPORT");

	//3 ���ݿ�����
	inifile.GetStrVal("DB","DBIP",m_szDBIP,sizeof(m_szDBIP));
	inifile.GetStrVal("DB","DBNAME",m_szDBName,sizeof(m_szDBName));
	m_nDBPort = inifile.GetIntVal("DB", "DBPORT", 3306);
	inifile.GetStrVal("DB","USERNAME",m_szUserName,sizeof(m_szUserName));
	inifile.GetStrVal("DB","PASSWORD",m_szPassword,sizeof(m_szPassword));
	int nDBThreadNum = inifile.GetIntVal("DB", "DBTHREADNUM");

	//4 ��Ϸ�������
	m_dwServerID = inifile.GetIntVal("MapSvr","ServerID");
	DWORD dwOpenSvrTime = inifile.GetIntVal("MapSvr","OpenSvrTime");
	if (!dwOpenSvrTime)
	{
		printf("��ȡ����ʱ��ʧ��!!\n");
		return FALSE;
	}
	ChangeOpenSvrTime(dwOpenSvrTime);
	m_bIsMerged = (BOOL)inifile.GetIntVal("MapSvr","IsMerge");
	m_bTestRobot  = (BOOL)inifile.GetIntVal("MapSvr","RobotTest");
	inifile.GetStrVal("MapSvr", "ServerName", m_szServerName, sizeof(m_szServerName)-1);

	m_bIsOpenGM = inifile.GetIntVal("MapSvr", "IsOpenGM");
	m_bIsCheckMsgSeriesId = inifile.GetIntVal("MapSvr", "IsCheckMsgSeriesId");
	m_bIsReportRecharge = inifile.GetIntVal("MapSvr", "IsReportRecharge");
	m_bIsCheckAccountWhite = inifile.GetIntVal("MapSvr", "IsCheckAccountWhite");
	m_bIsCheckPower = inifile.GetIntVal("MapSvr", "IsCheckPower");
	m_dwClientVersion = inifile.GetIntVal("MapSvr", "ClientVersion");
	m_bIsExamineServer = inifile.GetIntVal("MapSvr", "IsExamineServer");
	m_bIsUseOldPower = inifile.GetIntVal("MapSvr", "IsUseOldPower");
	m_bIsLCYExamineSvr = inifile.GetIntVal("MapSvr", "IsLCYExamineSvr");
	m_bIsYGame = inifile.GetIntVal("MapSvr", "IsYGame");
	m_bIsShowSpecialLog = inifile.GetIntVal("MapSvr", "IsShowSpecialLog");
	m_bIsUse3rdStatisticsData = inifile.GetIntVal("MapSvr", "IsUse3rdStatisticsData");
	m_dwServerBelongVersion = inifile.GetIntVal("MapSvr", "ServerBelongVersion");
	


	
	//inifile.GetStrVal("MapSvr", "ReYunAppId", m_szReYunAppId, sizeof(m_szReYunAppId)-1);
	//m_bIsOpenReyunStatistics = inifile.GetIntVal("MapSvr", "IsOpenReyunStatistics");
	

	//5 ��������
	m_tGameConfig.m_dwMaxAcceptSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_ACCEPT_SESSION");
	m_tGameConfig.m_dwMaxConnectSession = inifile.GetIntVal("SERVER_IOHANDLER","MAX_CONNECT_SESSION");
	m_tGameConfig.m_dwSendBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","SEND_BUFFER_SIZE");
	m_tGameConfig.m_dwRecvBufferSize = inifile.GetIntVal("SERVER_IOHANDLER","RECV_BUFFER_SIZE");
	m_tGameConfig.m_dwMaxPacketSize = inifile.GetIntVal("SERVER_IOHANDLER","MAX_PACKET_SIZE");
	m_tGameConfig.m_dwAcceptThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_ACCEPT_THREADS");
	m_tGameConfig.m_dwConnectThread = inifile.GetIntVal("SERVER_IOHANDLER","NUMBER_OF_CONNECT_THREADS");
	m_tGameConfig.m_dwTimeout = inifile.GetIntVal("SERVER_IOHANDLER","TIME_OUT");
	m_tGameConfig.m_bSecurity = inifile.GetIntVal("SERVER_IOHANDLER","SECURITY");

	//6 �ڴ�����
	m_tGameConfig.m_dwStaticSceneNum = inifile.GetIntVal("ScenePool","StaticSceneNum");
	m_tGameConfig.m_dwInstanceNum = inifile.GetIntVal("ScenePool","InstanceNum");
	m_tGameConfig.m_dwPlayerPoolSize = inifile.GetIntVal("MemoryPool","PlayerPoolSize");
	m_tGameConfig.m_dwNPCPoolSize = inifile.GetIntVal("MemoryPool","NPCPoolSize");
	m_tGameConfig.m_dwItemPoolSize = inifile.GetIntVal("MemoryPool","ItemPoolSize");
	m_tGameConfig.m_dwFunctionNPCPoolSize = inifile.GetIntVal("MemoryPool","FunctionNPCPoolSize");
	m_tGameConfig.m_dwMissionPoolSize = inifile.GetIntVal("MemoryPool","MissionPoolSize");
	m_tGameConfig.m_dwAuraPoolSize = inifile.GetIntVal("MemoryPool","AuraPoolSize");
	m_tGameConfig.m_dwBattlePoolSize = inifile.GetIntVal("MemoryPool","BattlePoolSize");
	m_tGameConfig.m_dwTeamPoolSize = inifile.GetIntVal("MemoryPool","TeamPoolSize");
	m_tGameConfig.m_dwMoveSpeed = inifile.GetIntVal("Speed","MoveSpeed");
	m_tGameConfig.m_dwRunSpeed = inifile.GetIntVal("Speed","RunSpeed");
	m_tGameConfig.m_dwPlayerMaxLv = inifile.GetIntVal("MaxLv","PlayerMaxLv");

	// ��Ϸ�߼���ʼ��
	CGameLogic::Instance()->Init(&inifile);

	// ��ʼ�� LOG
	std::string logBuffer;
	m_pLogfile = new LogFile;
	m_pLogfile->Init("log","MapSvr", LOG_LEVEL(inifile.GetIntVal("Log","Level")));

#ifdef _DEBUG
	MAPLOG(GUI_LEVEL, "MapServer(Debug) Starting.");
#else
	MAPLOG(GUI_LEVEL, "MapServer(Release) Starting.");
#endif

	// �򿪱���ʱ��ջ��ӡ����
	INIT_CRASH_REPORTER(LogPrinter);
	// ���ڴ��й©��⹦��
	INIT_MEMORY_LEAK_DETECT(LogPrinter);

	//��ʼ��DB
	logBuffer = "DB Module Init ....................... ";

	//ע��DB��ѯ���������
	Register();

	//�趨DB��ʼ����Ϣ
	DatabaseDesc dbdesc;
	dbdesc.pszDatabaseIP = m_szDBIP;
	dbdesc.pszDatabaseName = m_szDBName;
	dbdesc.pszUserName = m_szUserName;
	dbdesc.pszUserPassword = m_szPassword;
	dbdesc.nPort = m_nDBPort;
	dbdesc.fnErrorMessage = NULL;					//��ʱ��Ϊ�գ��Ժ󽫴���д�뵽��־�ļ���
	dbdesc.fnQueryResult = Map_CallBackQueryResult;	//����DB��ѯ����ص��������ӿ�

	//��ʼ��DB�̹߳�����
	if (!(DBThreadManager::Instance()->Initialize(dbdesc, max(nDBThreadNum,MAX_DB_CONN))))
	{
		logBuffer += "Failed" ;
		MAPLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	
	//��ʼ��DBC
// 	logBuffer = "Load DBC Files ....................... ";
// 	//��ʼ��DBC�ļ�������
// 	if (!DBCFileManager::Instance()->LoadAll())
// 	{
// 		logBuffer += "Failed";
// 		MAPLOG(GUI_LEVEL, logBuffer.c_str());
// 		return FALSE;
// 	}
// 	logBuffer += "OK";
// 	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	CConfigManager::Instance()->Init();

	// todo::��ʱ��������
	ServerInfo server;
	server.dwOpenTime = time(NULL);
	m_ServerList.insert(make_pair(1, server));

	//��ʼ��Lua�ű�
	logBuffer = "Load Lua Files ....................... ";
	LuaInterface::Instance()->CreateErrorProc<MapSvrLuaErrorProc>();
	if (!LuaInterface::Instance()->RegisterAll() || !this->LoadLuaFiles(true))
	{
		logBuffer += "Failed";
		MAPLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ�����߼�����ģ��....
	//��ʼ�����󹤳�
	logBuffer = "Init ObjectFactory ................... ";
	ObjectFactory::Instance()->Init(m_tGameConfig.m_dwPlayerPoolSize);
	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ���ʺŹ�����
	logBuffer = "Init AccountInfoManager .............. ";
	AccountInfoManager::Instance();
	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ����ҹ�����
	logBuffer = "Init PlayerManager ................... ";
	if (!PlayerManager::Instance()->Init())
	{
		logBuffer += "Failed";
		MAPLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ��ϵͳʱ�������
	SystemTimeManager::Instance()->Init();

	//��ʼ���������
	logBuffer = "Init ChatManager ..................... ";
	if (!ChatManager::Instance()->Init())
	{
		logBuffer += "Failed";
		MAPLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ��GM������
	logBuffer = "Init GMManager ...................... ";
	if (!GMManager::Instance()->Init())
	{
		logBuffer += "Failed";
		MAPLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ������������
// 	logBuffer = "Init SceneManager ...................... ";
// 	if (!CSceneManager::Instance()->Init())
// 	{
// 		logBuffer += "Failed";
// 		MAPLOG(GUI_LEVEL, logBuffer.c_str());
// 		return FALSE;
// 	}
// 	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	StopMessage::Instance()->Init();
	//ServerRestrictions::Instance()->Init();
	CKingMatchManager::Instance()->Init();
	PlayerBaseDataManager::Instance()->Init();
	CRebelManager::Instance()->Init();
	CWorldBossManager::Instance()->Init();
	CWarcraftManager::Instance()->Init();
	CRankManager::Instance()->Init();
	CArenaManager::Instance()->Init();
	CGuildManager::Instance()->Init();
	CTimeManager::Instance()->Init();
	CPVPManager::Instance()->Init();
	CAttackCityManager::Instance()->Init();
	CTeamDungeonManager::Instance()->Init();
	CChristmas::Instance()->Init();
	CTurntableManager::Instance()->Init();
	CScoreMatchManager::Instance()->Init();
	CResourceWarManager::Instance()->Init();
	CGuildWarMgr::Instance()->Init();
	

	// ע�᲻�ı��ɫ��Ϣ����Ϣ
	ResisterNotChangeRoleInfoMsg();
	
	
	//�õ�����������
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DWORD dwProcessNum = si.dwNumberOfProcessors;

	//���������������
	IOHANDLER_DESC desc[1];
	desc[0].dwIoHandlerKey = 0;
	desc[0].dwMaxAcceptSession = m_tGameConfig.m_dwMaxAcceptSession;
	desc[0].dwMaxConnectSession = m_tGameConfig.m_dwMaxConnectSession;
	desc[0].dwMaxPacketSize = m_tGameConfig.m_dwMaxPacketSize;
	desc[0].dwNumberOfAcceptThreads = m_tGameConfig.m_dwAcceptThread;
	desc[0].dwNumberOfConnectThreads = m_tGameConfig.m_dwConnectThread;
	desc[0].dwNumberOfIoThreads = 2*dwProcessNum+1;
	desc[0].dwRecvBufferSize = m_tGameConfig.m_dwRecvBufferSize;
	desc[0].dwSendBufferSize = m_tGameConfig.m_dwSendBufferSize;
	desc[0].dwTimeOut = m_tGameConfig.m_dwTimeout;
	desc[0].bSecurity = m_tGameConfig.m_bSecurity;
	desc[0].fnCreateAcceptedObject = CreateServerSideAcceptedObject;
	desc[0].fnDestroyAcceptedObject = DestroyServerSideAcceptedObject;
	desc[0].fnDestroyConnectedObject = DestroyServerSideConnectedObject;

	//��ʼ����Ϣ������
	logBuffer = "Register Message Handlers ............ ";
	PacketHandler::Instance()->Resister();
	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	//��ʼ����������
	logBuffer = "Init Network Engine .................. ";
	m_pIOCPServer = new IOCPServer;

	MAPLOG(GUI_LEVEL, "Begin Network Inits");
	if(!m_pIOCPServer->Init(desc,1,WriteLog))
	{
		logBuffer += "Failed";
		MAPLOG(GUI_LEVEL, logBuffer.c_str());
		return FALSE;
	}
	MAPLOG(GUI_LEVEL, "End Network Inits");

	//��ʼ�������ط������Ự����(����һ��)
	m_pGateServerSession = new GateServerSession;
	m_pGateServerSession->SetIPPort(m_szServerGateIP,m_dwServerGatePort);
	//�������ط�����

	MAPLOG(GUI_LEVEL, "Begin ConnectToGateServer");
	ConnectToGateServer();
	MAPLOG(GUI_LEVEL, "End ConnectToGateServer");
	InitGateBeat();

	//��ʼ���뿪�ŷ������Ự����(����һ��)
 	if (m_bConOpenSvrFlg)
 	{
 		m_pOpenServerSession = new OpenServerSession;
 		m_pOpenServerSession->SetIPPort(m_szServerOpenIP,m_dwServerOpenPort);
 		//���ӿ��ŷ�����
 		ConnectToOpenServer();
 		InitOpenBeat();
 	}

	logBuffer += "OK";
	MAPLOG(GUI_LEVEL, logBuffer.c_str());

	MAPLOG(GUI_LEVEL, "");
	MAPLOG(GUI_LEVEL, "MapServer Init Finished!");
	MAPLOG(GUI_LEVEL, "");

	// ��ʼ���ؼ��ֹ�����
	m_Filter.Initialize(ROLENAME_FILE, SQLCMD_FILE, CHAT_FILE);

	//m_nDoRewardTime = CConfigManager::Instance()->GetServerCfg(CONFIG_DO_REWARD_TIME, 2000);

	// �������ݿ����ݼ���
	LoadGolbalDataRequest();
	
	// ��ʱ���ε�
	//CArenaManager::Instance()->LoadDataRequest();
	CRankManager::Instance()->LoadDataRequest();
	
	LoadServerListRequest();
	CLandManager::Instance()->LoadDataRequest();
	CGuildManager::Instance()->LoadDataRequest();
	CResourceWarManager::Instance()->LoadDataRequest();
	CGuildWarMgr::Instance()->LoadDataRequest();
	MapCommentDataRequest();

	//TestKeyWordFilter();

	// �ؼ����ݴ�С���
	MAPLOG(GUI_LEVEL, "DataInfo");
	MAPLOG(GUI_LEVEL, "Player:%d(KB)", sizeof(Player)/1024);
	MAPLOG(GUI_LEVEL, "PlayerBaseData:%d(B)", sizeof(PlayerBaseData));

	// todo::����ʹ��
// 	PlayerBaseData tBaseData;
// 	for (int i = 1; i <= 1000000; ++i)
// 	{
// 		tBaseData.Init();
// 		tBaseData.dwRoleId = 1000000 + i;
// 		tBaseData.wLevel = g_Rand(1, 100);
// 		tBaseData.wHeadIcon = 30300;
// 		tBaseData.dwFightingPower = g_Rand(1, 1000000);
// 		sprintf_s(tBaseData.szRoleName, sizeof(tBaseData.szRoleName), "test_%d", i);
// 		PlayerBaseDataManager::Instance()->AddPlayerBaseData(i / 200000, tBaseData);
// 	}

	return TRUE;
}

void MapServer::ReloadInitFile()
{
	//����ini�����ļ�
	IniFile inifile;
	inifile.Init("MapSvr.ini");

	// �Ƿ���GM
	m_bIsOpenGM = inifile.GetIntVal("MapSvr", "IsOpenGM");

	// �Ƿ�����Ϣϵ��Id
	m_bIsCheckMsgSeriesId = inifile.GetIntVal("MapSvr", "IsCheckMsgSeriesId");

	// �Ƿ��ϱ���ֵ����
	m_bIsReportRecharge = inifile.GetIntVal("MapSvr", "IsReportRecharge");

	// �Ƿ����ʺŰ�����
	m_bIsCheckAccountWhite = inifile.GetIntVal("MapSvr", "IsCheckAccountWhite");

	// �Ƿ���ս��
	m_bIsCheckPower = inifile.GetIntVal("MapSvr", "IsCheckPower");

	// �ͻ��˰汾��
	m_dwClientVersion = inifile.GetIntVal("MapSvr", "ClientVersion");

	// �Ƿ���˷�
	m_bIsExamineServer = inifile.GetIntVal("MapSvr", "IsExamineServer");

	// �Ƿ�ʹ����ս��
	m_bIsUseOldPower = inifile.GetIntVal("MapSvr", "IsUseOldPower");

	m_bIsLCYExamineSvr = inifile.GetIntVal("MapSvr", "IsLCYExamineSvr");

	m_bIsYGame = inifile.GetIntVal("MapSvr", "IsYGame");

	m_bIsShowSpecialLog = inifile.GetIntVal("MapSvr", "IsShowSpecialLog");

	m_bIsUse3rdStatisticsData = inifile.GetIntVal("MapSvr", "IsUse3rdStatisticsData");

	m_dwServerBelongVersion = inifile.GetIntVal("MapSvr", "ServerBelongVersion");

	// ����appid
	//inifile.GetStrVal("MapSvr", "ReYunAppId", m_szReYunAppId, sizeof(m_szReYunAppId)-1);

	// �Ƿ�������ͳ��
	//m_bIsOpenReyunStatistics = inifile.GetIntVal("MapSvr", "IsOpenReyunStatistics");

	// ��Ϸ�߼���ʼ��
	CGameLogic::Instance()->ReloadInitFile(&inifile);
}

void MapServer::ReloadConfig()
{
	CRankManager::Instance()->ConfigReload();
}

//�ͷŵ�ͼ������
void MapServer::Release()
{
	// ������������
	SaveAllDataToDB();

	//�߳��������
	AccountInfoManager::Instance()->KickoutAllAccount();
	AccountInfoManager::Instance()->Release();
	AccountInfoManager::DestroyInstance();

	//�ͷ���ҹ�����
	PlayerManager::Instance()->Release();
	PlayerManager::DestroyInstance();

	//�ͷ�DB�̹߳�����
	DBThreadManager::Instance()->Release();
	DBThreadManager::DestroyInstance();

	GMManager::Instance()->Release();
	GMManager::DestroyInstance();

	ChatManager::Instance()->Release();
	ChatManager::DestroyInstance();
	
	//�ͷ����������
	Rand::DestroyInstance();

	//�ͷ���Ϣ������
	PacketHandler::DestroyInstance();

	//�ͷ�Lua�ű�������
	LuaInterface::DestroyInstance();

	//�ͷ�DBC�ļ�������
	DBCFileManager::DestroyInstance();

	//�ͷ���������
	if(m_pIOCPServer)
	{
		m_pIOCPServer->Shutdown();
		delete m_pIOCPServer;
		m_pIOCPServer = NULL;
	}

	//�ͷ����ط������Ự����
	if(m_pGateServerSession)
	{
		delete m_pGateServerSession;
		m_pGateServerSession = NULL;
	}

	//�ͷſ��ŷ������Ự����
 	if(m_pOpenServerSession)
 	{
 		delete m_pOpenServerSession;
 		m_pOpenServerSession = NULL;
 	}

	//�ͷ���־�ļ�����
	if(m_pLogfile)
	{
		m_pLogfile->ShutDown();

		delete m_pLogfile;
		m_pLogfile = NULL;
	}
}

void MapServer::Run()
{
	char szTitile[128];
	char szMsg[256];

	GetConsoleTitle(szTitile, sizeof(szTitile));
	DWORD dwStatistics = 0;
	DWORD dwFps = 0;

	int nSleepCnt = 0;
	//DWORD dwSendBuffDataLength, dwRecvBuffDataLength;

	while(TRUE)
	{
		static UINT64 lastUpdateTick = GetTickCount64();

		UINT64 curTick = GetTickCount64();

		DWORD deltaTick;

		if (curTick>=lastUpdateTick)
		{
			deltaTick = curTick - lastUpdateTick;
		}
		else
		{
			deltaTick = 0xFFFFFFFF - lastUpdateTick + curTick + 1;
		}

		lastUpdateTick = curTick;

		if(!Update(deltaTick)) break;

		DWORD dwCostTick = GetTickCount64() - curTick;
		if (dwCostTick > 100)
			MAPLOG(ERROR_LEVEL, "Do Update Cost[%d]MS", dwCostTick);

		if (dwStatistics >= 1000)
		{
			//��ʾ����������fps 
			sprintf(szMsg, "[%s]Online_Player:%d Fps:%d Tick:%lld", m_szServerName,
				PlayerManager::Instance()->GetPlayerNum(), dwFps, curTick);
			SetConsoleTitle(szMsg);
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

//���µ�ͼ������
BOOL MapServer::Update(DWORD dwDeltaTick)
{
	if(m_bShutDown)
		return FALSE;

	m_pIOCPServer->Update();

	ENTER_DUMP_FUNCTION

	// �����¼�
	UINT64 ulCurTime = GetTickCount64();
	//if (lCurTime >= m_lProcessNewDayTimer + 1000)
 	{
 		time_t t = time(NULL);
 		tm localtime;
 		localtime_s(&localtime,&t);
 		if (localtime.tm_yday != m_nLastYearDay)
 		{
			// �µ�һ��
 			OnNewDay();

			// ��һ��
			if (1 == localtime.tm_wday)
				OnNewWeek();

			// �µ��·�
			if (1 == localtime.tm_mday)
				OnNewMonth();

			m_nLastYearDay = localtime.tm_yday;
 		}		
 	}

	DBThreadManager::Instance()->Update();
	//PlayerManager::Instance()->Update(dwDeltaTick);
	//TimeoutManager::Instance()->Update(dwDeltaTick);
	//SystemTimeManager::Instance()->Update(dwDeltaTick);
	//StopMessage::Instance()->Update(dwDeltaTick);
	
	UpdateOpenBeat(dwDeltaTick);
	UpdateGateBeat(dwDeltaTick);
	UpdateWaitingLoginAccount();

	// �߼�ģ�鴦��(500���봦��һ��)
	if (ulCurTime >= m_lProcessLogicTimer + 1000)
	{
		if (m_bLoadServerList && m_bIsLoadRoleBaseData)
		{
			CRebelManager::Instance()->Update();
			CLandManager::Instance()->Update();
			GMManager::Instance()->Update();
			CGuildManager::Instance()->Update();
			CTimeManager::Instance()->Update();

			CAttackCityManager::Instance()->Update();
			CPVPManager::Instance()->Update();
			CWorldBossManager::Instance()->Update();
			CArenaManager::Instance()->Update();
			CRankManager::Instance()->Update();
			CKingMatchManager::Instance()->Update();
			CTeamDungeonManager::Instance()->Update();
			CTurntableManager::Instance()->Update();
			CScoreMatchManager::Instance()->Update();
			CResourceWarManager::Instance()->Update();
			CGuildWarMgr::Instance()->Update();
		}

		m_lProcessLogicTimer = ulCurTime;

		// ���Դ���
		//OnStatisticsReport(m_dwTestSendMsgToNodejsTimes++, 1000, 1, 1);
	}

	// 1����һ��
	if (ulCurTime >= m_lSaveUpdatePlayerTime + 60000)
	{
		SaveUpdatePlayerData();
		m_lSaveUpdatePlayerTime = ulCurTime;
	}

	// 2���Ӽ�¼һ��
	if (ulCurTime >= m_lRecordOfflinePlayerNumTime + 120000)
	{
		SaveOnlinePlayerNum();
		m_lRecordOfflinePlayerNumTime = ulCurTime;
	}

	LEAVE_DUMP_FUNCTION
	
	return TRUE;
}

void MapServer::SaveAllDataToDB()
{
	CArenaManager::Instance()->SaveAllData();
	CResourceWarManager::Instance()->SaveDataToDB();
}

//����DB��ѯ����
BOOL MapServer::MapDBQuery(QueryForUser * pData)
{
	if(!pData)
		return FALSE;

	int nDBIndex = pData->GetThreadIndex();
 	//��DB�̹߳�����Ͷ��DB��ѯ����
	if (nDBIndex >= MAX_DB_CONN)
	{
		//д��־
		MAPLOG(ERROR_LEVEL, "MapDBQueryԽ��[DBIndex:%d]", nDBIndex);
		return FALSE;
	}
//  	if(nDBIndex<0 || nDBIndex>=MAX_DB_CONN || !(DBThreadManager::Instance()->DBQuery(nDBIndex,pData)))
//  	{
//  		//д��־
//  		MAPLOG(ERROR_LEVEL,"MapDBQueryִ��ʧ��[DBIndex:%d]",nDBIndex);
//  		return FALSE;
//  	}

	// todo::����Ϊͳһ����
//  	if (++m_dwCurDBThreadIndex >= MAX_DB_CONN)
//  		m_dwCurDBThreadIndex = 0;
//  	pData->SetThreadIndex(m_dwCurDBThreadIndex);
 
	if (!nDBIndex)
	{
		// todo::���ж��̵߳�����
		if (++m_dwCurDBThreadIndex >= MAX_DB_CONN)
			m_dwCurDBThreadIndex = 0;
		nDBIndex = m_dwCurDBThreadIndex;
	}

	pData->SetThreadIndex(nDBIndex);
	
	//��DB�̹߳�����Ͷ��DB��ѯ����
	if (!(DBThreadManager::Instance()->DBQuery(pData->GetThreadIndex(), pData)))
 	{
 		//д��־
		MAPLOG(ERROR_LEVEL, "MapDBQueryִ��ʧ��[DBIndex:%d]", pData->GetThreadIndex());
 		return FALSE;
 	}

	return TRUE;
}

//����DB��ѯ���
VOID MapServer::DBResult( MAP_QUERY_INDEX dwIndex, QueryResult* pData )
{
	if(!pData)
	{
		//д��־
		MAPLOG(ERROR_LEVEL,"pData==NULL,dwIndex=%d",dwIndex);
		return;
	}

	//����DB�����������ӿ�
	fnProcessQueryResult pfnHandler =  Find(dwIndex);

	if(pfnHandler){
		// ���������ĺ�ʱ
		DWORD dwCostTime = GetTickCount64() - pData->GetInitTime();
		if (dwCostTime >= 100)
			MAPLOG(GUI_LEVEL, "[Index=%d]���ݿ������ʱ[%d]����", dwIndex,dwCostTime);

		//����DB��ѯ���
		pfnHandler(pData);
	}
	else{
		//д��־
		MAPLOG(ERROR_LEVEL,"��Ч��dwIndex=%d",dwIndex);
	}

	//�ͷ���Դ
	pData->Release();
}

//��ͼ�������Ƿ��ж�״̬
BOOL MapServerThread::IsShutDown()
{
	return g_MapServer.IsShutDown();
}

//����Lua�ļ�
BOOL MapServer::LoadLuaFiles(bool bFirstLoad)
{
	// �������� Lua �ű�
	const char LUA_SCR_DIR[] = "Lua";
	if (!LuaInterface::Instance()->LoadDir(LUA_SCR_DIR, bFirstLoad))
		return FALSE;

	return TRUE;
}

//�������ط�����
void MapServer::ConnectToGateServer()
{
	m_pGateServerSession->TryToConnect();	
}

//���ӿ��ŷ�����
void MapServer::ConnectToOpenServer()
{
	m_pOpenServerSession->TryToConnect();
}

//����ָ���ķ�����(NetworkObject����)
void MapServer::ConnectToServer( NetworkObject * pNetworkObject, char * pszIP, WORD wPort )
{
	m_pIOCPServer->Connect(0,pNetworkObject,pszIP,wPort);
}

void MapServer::SendOpen2Beat()
{
	 MSG_M2O_HEART_BEAT Msg;
	 m_pOpenServerSession->SendPacket(0,Msg.GetType(),&Msg);	
}

void MapServer::SendGate2Beat()
{
	MSG_M2G_HEART_BEAT Msg;
	m_pGateServerSession->SendPacket(0, Msg.GetType(), &Msg);
}

//д����־
void MapServer::WriteLog(LOG_LEVEL eLevel, const char* log)
{
	IF_NOT (log)
		return;

	int eOldLevel = eLevel;
	if (eLevel == IMPORTANT_LEVEL)
	{
		eLevel = ERROR_LEVEL;
	}

	if (m_pLogfile != NULL)
	{
		m_pLogfile->InsertLog(eLevel, log);
	}

	if (ERROR_LEVEL == eLevel)
	{
		// ��ɫ
		if(ERROR_LEVEL != m_byCurLogColor)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 	
				FOREGROUND_RED | FOREGROUND_INTENSITY);
			m_byCurLogColor = eLevel;
		}
	}
	else if (WARNING_LEVEL == eLevel)
	{
		// ��ɫ
		if(WARNING_LEVEL != m_byCurLogColor)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 	
				FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			m_byCurLogColor = eLevel;
		}
	}
	else
	{
		// ��ɫ
		if(ERROR_LEVEL == m_byCurLogColor || WARNING_LEVEL == m_byCurLogColor)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),	
				FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			m_byCurLogColor = eLevel;
		}
	}

	//if (eLevel == GUI_LEVEL)
	//{
	// �����������Ϣ,���ж��Ƿ���Ҫ��ʾ
	if (eLevel != SPECAIL_LEVEL || m_bIsShowSpecialLog)
	{
		time_t t = time(NULL);
		tm curTime;
		localtime_s(&curTime, &t);
		char szTime[32] = { 0 };
		sprintf_s(szTime, sizeof(szTime), "%d:%d:%d", curTime.tm_hour, curTime.tm_min, curTime.tm_sec);
		printf("[%s]%s\n", szTime, log);
	}
	//}

	// ֻ�洢������־
	if (ERROR_LEVEL == eLevel)
	{
		Log_Save* pQuery = Log_Save::ALLOC();
		pQuery->SetIndex(DB_LOG_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_LOG);
		pQuery->SetContext(log);
		pQuery->SetLevel(eOldLevel);
		g_MapServer.MapDBQuery(pQuery);
	}
}

void MapServer::WriteLog(const char* log,...)
{
	MAPLOG(GUI_LEVEL,log);
}

//��÷������������
int MapServer::GetPlayerNum() const
{
	return PlayerManager::Instance()->GetPlayerNum();
}

void MapServer::SendMsgToGate(DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg)
{
	if (m_pGateServerSession)
		m_pGateServerSession->SendPacket(dwGUID, wMsgID, pMsg);
}

bool MapServer::SendMsgToOpen(const char*pszAccountId, DWORD dwGUID, DWORD dwMsgId, const unsigned char *pszMsg, int nMsgSize)
{
// 	CHECK_POINTER_RET(m_pGateServerSession, false);
// 
// 	const char *pBaseUrl = CConfigManager::Instance()->GetUrlById(dwMsgId);
// 	CHECK_POINTER_RET(pBaseUrl, false);
// 
// 	string strAccountId = pszAccountId;
// 
// 	static char szBase64SendData[MAX_MSG_BUFF_LEN] = {0};
// 	memset(szBase64SendData,0,sizeof(szBase64SendData));
// 	int nBase64SendDataLen = sizeof(szBase64SendData)-1;
// 	MyBase64Encode((const unsigned char*)pszMsg, nMsgSize, szBase64SendData, &nBase64SendDataLen);
// 
// 	SS::Msg_LogicCommon_M2O send;
// 	send.set_msg_id(dwMsgId);
// 	send.set_userid(strAccountId);
// 	send.set_msg_body(szBase64SendData);
// 	send.set_url(pBaseUrl);
// 
// 	return m_pOpenServerSession->SendPacket(dwGUID, &send);
	return false;
}

void MapServer::SendErrorMsgToUser(DWORD dwGUID, WORD wErrorCode, WORD wFromMsgId)
{
	static MSG_S2C_GAME_ERROR_NOTIFY error_msg;
	error_msg.m_wErrorCode = wErrorCode;
	error_msg.m_wFromMsgId = wFromMsgId;
	m_pGateServerSession->SendPacket(dwGUID, error_msg.GetType(), &error_msg);
}

// ���տͻ�����Ϣ�ص�
void MapServer::RecvClientMsgCallback(DWORD dwGuid, WORD wMsgID)
{
	if (m_NotChangeRoleInfoMsgList.find(wMsgID) != m_NotChangeRoleInfoMsgList.end())
		return;

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (pPlayer)
	{
		pPlayer->RoleInfoUpdate();
	}
}

// ��Ӳ���ı��ɫ���ݵ���Ϣ
void MapServer::AddNotChangeRoleInfoMsg(WORD wMsgID)
{
	if (m_NotChangeRoleInfoMsgList.find(wMsgID) != m_NotChangeRoleInfoMsgList.end())
	{
		MAPLOG(ERROR_LEVEL, "����ظ��Ĳ��ı��ɫ���ݵ���Ϣ[MsgId:%d]", wMsgID);
		return;
	}

	m_NotChangeRoleInfoMsgList.insert(make_pair(wMsgID, wMsgID));
}

//ת������ʱ��
void MapServer::ChangeOpenSvrTime(DWORD dwOpenSvrDate)
{
	if (!dwOpenSvrDate)
		return;

	m_dwOpenSvrTime = dwOpenSvrDate;
}

DWORD MapServer::GetCurServerId(DWORD dwInitServerId, DWORD dwRoleId)
{
	//return dwInitServerId;
	const ServerInfo_Config  *pServerInfoCfg = g_pCfgMgr->GetServerInfo(dwInitServerId);
	if (!pServerInfoCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ�����������Ϣ����[server_id:%d,role_id:%d]", dwInitServerId, dwRoleId);
		return 0;
	}

	if (!pServerInfoCfg->dwCurServerId)
	{
		MAPLOG(ERROR_LEVEL, "���õ�ǰ�ķ�����IDΪ��[%d]", dwInitServerId);
		return 0;
	}

	return pServerInfoCfg->dwCurServerId;
}

VOID MapServer::AddWaitingLoginAccount(const LoginAccountInfo *pInfo)
{
	if (FindWaitingLoginAccount(pInfo->szAccountName, pInfo->dwChannelUID))
	{
		MAPLOG(ERROR_LEVEL,"��ӵ�¼��Ϣ���뻺���б�ʧ��[%s]",pInfo->szAccountName);
		return;
	}

	string strAccountKey = MakeAccountKey(pInfo->dwChannelUID, pInfo->szAccountName);
	m_WaitingLoginAccountList.insert(make_pair(strAccountKey, *pInfo));
}

VOID MapServer::RemoveWaitingLoginAccount(const char *pszAccountName, DWORD dwChannelType)
{
	string strAccountKey = MakeAccountKey(dwChannelType, pszAccountName);
	m_WaitingLoginAccountList.erase(strAccountKey);
}

const LoginAccountInfo* MapServer::FindWaitingLoginAccount(const char *pszAccountName, DWORD dwChannelType)
{
	string strAccountKey = MakeAccountKey(dwChannelType, pszAccountName);
	std::map<String, LoginAccountInfo>::iterator iter = m_WaitingLoginAccountList.find(strAccountKey);
	return iter != m_WaitingLoginAccountList.end() ? &(iter->second) : NULL;
}

BOOL MapServer::OnAccountLogin(const char *pszAccount, const char *pszPassword, DWORD dwChannel, DWORD dwGuid, BYTE byTerminalType)
{
	// ����Ƿ��ڴ���¼״̬
// 	if (g_MapServer.FindWaitingLoginAccount(pszAccount))
// 	{
// 		MAPLOG(GUI_LEVEL, "[%s]�ظ���¼�ʺ�[%s]", __FUNCTION__, pszAccount);
// 		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_REPEAT_LOGIN);
// 		return FALSE;
// 	}

	// ����Ƿ����ڵ�¼��
	AccountInfo *pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(dwChannel, pszAccount);
	if (pAccountInfo)
	{
		// ���ѵ�¼������ߵ�
		if (dwGuid != pAccountInfo->GetGuid())
		{
			// �ǽ�ɫ��������״̬
			if (pAccountInfo->GetUserState() != USER_LOGOUTING_STATE)
			{
				// ֪ͨ�ѵ�¼���������
				g_MapServer.SendErrorMsgToUser(pAccountInfo->GetGuid(), ERROR_LOGIN_KICK_OUT);

				//�ߵ��������
				AccountInfoManager::Instance()->KickoutAccount(pAccountInfo);
			}

			// �������¼�б�
			LoginAccountInfo temp;
			memset(&temp, 0, sizeof(temp));
			temp.dwGuid = dwGuid;
			strcpy_s(temp.szAccountName, sizeof(temp.szAccountName), pszAccount);
			strcpy_s(temp.szPassword, sizeof(temp.szPassword), pszPassword);
			temp.dwTime = time(NULL);
			temp.dwChannelUID = pAccountInfo->GetChannelUID();
			temp.byLoginType = 0; // pRecv->m_byType;
			temp.dwServerId = pAccountInfo->GetZoneID();
			temp.bIsSDK = true;
			g_MapServer.AddWaitingLoginAccount(&temp);
		}
		else
		{
			// ֪ͨ�ظ���¼
			Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
			if (pPlayer)
				g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_REPEAT_LOGIN);
		}
	}
	else
	{
		// ����ʺ�
		CheckAccountQuery* pQuery = CheckAccountQuery::ALLOC();
		pQuery->SetIndex(DB_SDK_ACCOUNT_CHECK);
		pQuery->SetThreadIndex(0);
		pQuery->SetKey(dwGuid);
		pQuery->SetAccID(pszAccount);
		pQuery->SetPassword(pszPassword);
		pQuery->SetChannelUID(dwChannel);
		pQuery->SetLoginType(0/*pRecv->m_byType*/);
		pQuery->SetTerminalype(byTerminalType);
		g_MapServer.MapDBQuery(pQuery);
	}

	return TRUE;
}

 BOOL MapServer::MatchRoleName(const char* szName)
 {
 	//CLock l(&m_FilterLock);
 	return m_Filter.MatchRoleName(szName);
 }

 BOOL MapServer::MatchSqlCmd(const char* szCmd)
 {
 	return m_Filter.MatchSqlCmd(szCmd);
 }

 // �����Ҫ���µ����
 void MapServer::AddUpdatePlayer(DWORD dwRoleId, DWORD dwServerId, bool isSave)
 {
	 map<DWORD, DWORD>::iterator iter = m_UpdatePlayerList.find(dwRoleId);
	 if (iter != m_UpdatePlayerList.end())
		 return;

	 m_UpdatePlayerList.insert(make_pair(dwRoleId, dwServerId));

	 // ��100�������洢
	 if (isSave && m_UpdatePlayerList.size() >= 50)
	 {
		 SaveUpdatePlayerData();
	 }
 }

 // �洢���µ��������
 void MapServer::SaveUpdatePlayerData()
 {
	 if (!m_UpdatePlayerList.size())
		 return;

	 map<DWORD, DWORD>::iterator iter = m_UpdatePlayerList.begin();
	 for (; iter != m_UpdatePlayerList.end(); ++iter)
	 {
		 Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(iter->first);
		 if (pPlayer)
			 pPlayer->OnSave(SAVE_DB_TYPE_TIMING);
	 }

	 MAPLOG(GUI_LEVEL, "������ɫ��Ϣ�洢[num:%d]", m_UpdatePlayerList.size());

	 m_UpdatePlayerList.clear();
 }

 // �洢�����������
 void MapServer::SaveOnlinePlayerNum()
 {
	 // ����Ϊֱ�Ӵ洢���ݿ�
	 //OnlinePlayerNumRecord_Save* pQuery = OnlinePlayerNumRecord_Save::ALLOC();
	 //pQuery->SetIndex(DB_OFFLINE_PLAYER_NUM_RECORD_SAVE);
	 //pQuery->SetThreadIndex(DB_THREAD_COMMON);
	 DWORD dwCurMaxOnlineRoleNum = 0;
	 for (map<DWORD, ServerInfo>::iterator iter = m_ServerList.begin(); iter != m_ServerList.end(); ++iter)
	 {
		 // �Ѿ����Ϸ�, ������
		 DWORD dwCurServerId = g_MapServer.GetCurServerId(iter->second.dwServerId);
		 if (dwCurServerId != iter->second.dwServerId)
			 continue;

		 //pQuery->AddServerOnlineData(dwCurServerId, PlayerManager::Instance()->GetPlayerNum(dwCurServerId));
		 DWORD dwPlayerNum = PlayerManager::Instance()->GetPlayerNum(dwCurServerId);
		 CGameLogic::Instance()->OnGameStatistics_Online(dwCurServerId, dwPlayerNum);
		 dwCurMaxOnlineRoleNum += dwPlayerNum;
	 }
	 //g_MapServer.MapDBQuery(pQuery);

	 m_dwCurMaxOnlineRoleNum = max(dwCurMaxOnlineRoleNum, m_dwCurMaxOnlineRoleNum);
 }

//  BYTE MapServer::GetSignLastResetMonth()
//  {
// 	 return GetAlreadyOpenServerDay() / 30 + 1;
//  }

 // ȡ�Ѿ�����������
 DWORD MapServer::GetAlreadyOpenServerDay(DWORD dwServerId)
 {
	 const ServerInfo *pServerInfo = GetServerInfo(dwServerId);
	 if (!pServerInfo)
	 {
		 MAPLOG(GUI_LEVEL, "�Ҳ�����������Ϣ[ServerId:%d]", dwServerId);
		 return 0;
	 }

	 return GetPassDays(pServerInfo->dwOpenTime, time(NULL));
 }

 // ȡ��������Ϣ
 const ServerInfo* MapServer::GetServerInfo(DWORD dwServerId)
 {
	 map<DWORD, ServerInfo>::iterator iter = m_ServerList.find(dwServerId);
	 return iter != m_ServerList.end() ? &iter->second : NULL;
 }

 void MapServer::LoadServerList(const ServerInfo *pServerList, int nServerNum)
 {
	 m_ServerList.clear();
	for (int i = 0; i < nServerNum; ++i)
	{
		m_ServerList.insert(make_pair(pServerList[i].dwServerId, pServerList[i]));
		PlayerBaseDataManager::Instance()->AddServer(pServerList[i].dwServerId);
	}
	MAPLOG(GUI_LEVEL, "�ɹ����ط������б�[num:%d]", nServerNum);

	if (!m_bLoadServerList)
	{
		CArenaManager::Instance()->LoadDataRequest();

		CAttackCityManager::Instance()->Start();

		CKingMatchManager::Instance()->LoadDataRequest();

		CScoreMatchManager::Instance()->LoadDataRequest();

		m_bLoadServerList = true;
	}
 }

 // ���ط������б�����
 void MapServer::LoadServerListRequest()
 {
	 ServerList_Load* pQuery = ServerList_Load::ALLOC();
	 pQuery->SetIndex(DB_SERVER_LIST_LOAD);
	 pQuery->SetThreadIndex(0);
	 g_MapServer.MapDBQuery(pQuery);
 }

 // ���ÿ������������
//  void MapServer::SetOpenServerFundBuyTimes(DWORD dwBuyTimes)
//  {
// 	 m_dwOpenServerFundBuyTimes = dwBuyTimes;
//  }

 // ���ط�����ȫ������
 void MapServer::LoadGolbalDataRequest()
 {
	 ServerGolbalData_Load* pQuery = ServerGolbalData_Load::ALLOC();
	 pQuery->SetIndex(MAP_SERVER_GOLBALDATA_LOAD);
	 pQuery->SetThreadIndex(DB_THREAD_GOLBAL_DATA);
	 MapDBQuery(pQuery);
 }

 // ���ط�����ȫ�����ݻص�
 void MapServer::LoadGolbalDataCallback(WORD wServerId, int nKey, DWORD dwValue)
 {
	 ServerGlobalData &tGlobalData = GetServerGlobalData(wServerId);

	 if (GOLBAL_DATA_WORLD_BOSS_LEVEL == nKey)
	 { 
		 if (!CWorldBossManager::Instance()->IsActivity())
			 CWorldBossManager::Instance()->SetBossLevel(wServerId, max(1, dwValue));
	 }
	 else if (GOLBAL_DATA_FUND_BUY_TIMES == nKey)
	 {
		 tGlobalData.dwOpenServerFundBuyTimes = dwValue;
	 }
	 else if (GOLBAL_DATA_CHAPTER_PASS_RECORD == nKey)
	 {
		 tGlobalData.wChapterPassRecord = dwValue;
	 }
	 else if (GOLBAL_LIMIT_PREFERENTIAL_GOODS_BUY_TIMES == nKey)
	 {
		 tGlobalData.dwLimitActivityGoodsBuyTimes = dwValue;
	 }
	 else if (GOLBAL_FIGHTING_POWER_RECORD_IDX == nKey)
	 {
		 tGlobalData.dwFightingPowerRecordIdx = dwValue;
	 }
	 else if (GOLBAL_ATTCK_CITY_CHAPTER == nKey)
	 {
		 tGlobalData.wAttackCityChapter = dwValue;
	 }
	 else if (GOLBAL_ATTCK_CITY_RANK_NO1_ROLE_ID == nKey)
	 {
		 tGlobalData.dwAttackCityRankNO1RoleId = dwValue;
	 }
	 else if (GOLBAL_CHRISTMAS_LOWER_BOSS_LEVEL == nKey)
	 {
		 tGlobalData.wChristmasLowerBossLevel = dwValue;
	 }
	 else if (GOLBAL_CHRISTMAS_HIGH_BOSS_LEVEL == nKey)
	 {
		 tGlobalData.wChristmasHighBossLevel = dwValue;
	 }
	 else if (GOLBAL_CHRISTMAS_RESET_TIME == nKey)
	 {
		 tGlobalData.dwChristmasLastResetTime = dwValue;
	 }
	 else
	 {
		 MAPLOG(ERROR_LEVEL, "ľ������յ�ȫ������[Key:%d]", nKey);
	 }
 }

 // �洢������ȫ������
 void MapServer::SaveGolbalData(WORD wServerId, int nKey, DWORD dwValue)
 {
	 ServerGolbalData_Save* pQuery = ServerGolbalData_Save::ALLOC();
	 pQuery->SetIndex(MAP_SERVER_GOLBALDATA_SAVE);
	 pQuery->SetThreadIndex(DB_THREAD_GOLBAL_DATA);
	 pQuery->AddGolbalData(wServerId,nKey, dwValue);
	 MapDBQuery(pQuery);
 }

 // ���·�����ȫ������
 void MapServer::UpdateGolbalData(WORD wServerId, int nKey, DWORD dwValue)
 {
	 ServerGlobalData &tGlobalData = GetServerGlobalData(wServerId);
	 switch (nKey)
	 {
		 // ���������
	 case GOLBAL_DATA_FUND_BUY_TIMES:
		 tGlobalData.dwOpenServerFundBuyTimes = dwValue;
		 break;

		 // �½�ͨ�ؼ�¼
	 case GOLBAL_DATA_CHAPTER_PASS_RECORD:
		 tGlobalData.wChapterPassRecord = dwValue;
		 break;

		 // ��ʱ���Ʒ�������
	 case GOLBAL_LIMIT_PREFERENTIAL_GOODS_BUY_TIMES:
		 tGlobalData.dwLimitActivityGoodsBuyTimes = dwValue;
		 break;

		 // ս����¼����
	 case GOLBAL_FIGHTING_POWER_RECORD_IDX:
		 tGlobalData.dwFightingPowerRecordIdx = dwValue;
		 break;

		 // ���﹥��׼��Ҫ��ս���½�
	 case GOLBAL_ATTCK_CITY_CHAPTER:
		 tGlobalData.wAttackCityChapter = dwValue;
		 break;

		 // ���﹥���ϴλ������һ�����
	 case GOLBAL_ATTCK_CITY_RANK_NO1_ROLE_ID:
		 tGlobalData.dwAttackCityRankNO1RoleId = dwValue;
		 break;

		 // ʥ���ڵͼ�boss�ȼ�
	 case GOLBAL_CHRISTMAS_LOWER_BOSS_LEVEL:
		 tGlobalData.wChristmasLowerBossLevel = dwValue;
		 break;

		 // ʥ���ڸ߼�boss�ȼ�
	 case GOLBAL_CHRISTMAS_HIGH_BOSS_LEVEL:
		 tGlobalData.wChristmasHighBossLevel = dwValue;
		 break;

		// ʥ������������ʱ��
	 case GOLBAL_CHRISTMAS_RESET_TIME:
		 tGlobalData.dwChristmasLastResetTime = dwValue;
		 break;

	 default:
		 MAPLOG(ERROR_LEVEL, "[%s]ľ������յ�ȫ������[Key:%d]", __FUNCTION__, nKey);
		 return;
		 break;
	 }

	 SaveGolbalData(wServerId, nKey, dwValue);
 }

 // ȡȫ������
 ServerGlobalData& MapServer::GetServerGlobalData(WORD wServerId)
 { 
	 map<DWORD, ServerGlobalData>::iterator iter = m_GlobalDataList.find(wServerId);
	 if (iter == m_GlobalDataList.end())
	 {
		 ServerGlobalData tData;
		 memset(&tData, 0, sizeof(tData));
		 tData.wAttackCityChapter = 1; // �ӵ�һ����ʼ��ս
		 m_GlobalDataList.insert(make_pair(wServerId, tData));
		 iter = m_GlobalDataList.find(wServerId);
	 }
	 return iter->second;
 };

 // ��Ӽ�������ȡ��¼
 void MapServer::AddActivationCodeRecord(const char *pszCode, DWORD dwRoleId)
 {
	 m_ActivationCodeTakeRecord.insert(make_pair(pszCode, dwRoleId));
 }

 // �Ƿ񼤻�����ȡ��¼
 bool MapServer::IsActivationCodeRecord(const char *pszCode)
 {
	 map<string, DWORD>::iterator iter = m_ActivationCodeTakeRecord.find(pszCode);
	 return iter != m_ActivationCodeTakeRecord.end() ? true : false;
 }

 // ����ֻ�����֤����
 void MapServer::AddMobileAuthData(const MobileAuthData &tAuthData)
 {
	 map<string, MobileAuthData>::iterator iter = m_MobileAuteDataList.find(tAuthData.strMobile);
	 if (iter != m_MobileAuteDataList.end())
	 {
		 iter->second.dwTime = time(NULL);
		 iter->second.wCode = tAuthData.wCode;
	 }
	 else
	 {
		 m_MobileAuteDataList.insert(make_pair(tAuthData.strMobile, tAuthData));
	 }
 }

 // ɾ���ֻ�����֤����
 void MapServer::RemoveMobileAuthData(const char *pszMobile)
 {
	 m_MobileAuteDataList.erase(pszMobile);
 }

 // �Ƿ�ȴ�ע���ֻ���
 const MobileAuthData* MapServer::GetMobileAuthData(const char *pszMobile)
 {
	 map<string, MobileAuthData>::iterator iter = m_MobileAuteDataList.find(pszMobile);
	 if (iter != m_MobileAuteDataList.end())
	 {
		 DWORD dwPassTime = time(NULL) - iter->second.dwTime;
		 // 10����
		 if (dwPassTime < 60 * 10)
			 return &iter->second;
	 }

	 return NULL;
 }

#define MAKE_MAP_COMMENT_ID(server_id,chapter_id)  100000 * server_id + chapter_id

 // ����ؿ������б�
 vector<MapComment>* MapServer::GetMapCommentList(DWORD dwServerId, DWORD dwChapter)
 {
	 DWORD dwFindId = MAKE_MAP_COMMENT_ID(dwServerId, dwChapter);
	 map<DWORD, vector<MapComment>>::iterator iter = m_MapCommentList.find(dwFindId);
	 return iter != m_MapCommentList.end() ? &iter->second : NULL;
 }

 // ��ӹؿ�����
 void MapServer::AddComment(DWORD dwServerId, DWORD dwChapter)
 {
	 DWORD dwFindId = MAKE_MAP_COMMENT_ID(dwServerId, dwChapter);
	 m_MapCommentList.insert(make_pair(dwFindId, vector<MapComment>()));
 }

 // �洢�ؿ���������
 void MapServer::SaveMapCommentData(DWORD dwServerId, WORD dwChapter)
 {
	 const vector<MapComment> *pCommentList = GetMapCommentList(dwServerId, dwChapter);
	 if (pCommentList && pCommentList->size())
	 {
		 DWORD dwFindId = MAKE_MAP_COMMENT_ID(dwServerId, dwChapter);
		 MapComment_Save* pQuery = MapComment_Save::ALLOC();
		 pQuery->SetIndex(DB_MAP_COMMENT_DATA_SAVE);
		 pQuery->SetThreadIndex(DB_THREAD_MAP_COMMENT);
		 pQuery->SetId(dwFindId);
		 vector<MapComment>::const_iterator iter = pCommentList->begin();
		 for (; iter != pCommentList->end(); ++iter)
		 {
			 pQuery->AddComment(&*iter);
		 }
		 MapDBQuery(pQuery);
	 }
 }

 // ���عؿ���������
 void MapServer::LoadMapCommentData(DWORD dwFindId, const MapComments *pComments)
 {
	 vector<MapComment> vCommetList;
	 for (WORD i = 0; i < pComments->wCommentNum; ++i)
	 {
		 vCommetList.push_back(pComments->arCommentList[i]);
	 }
	 m_MapCommentList.insert(make_pair(dwFindId, vCommetList));
 }

 // �ؿ�������������
 void MapServer::MapCommentDataRequest()
 {
	 MapComment_Load* pQuery = MapComment_Load::ALLOC();
	 pQuery->SetIndex(DB_MAP_COMMENT_DATA_LOAD);
	 pQuery->SetThreadIndex(DB_THREAD_MAP_COMMENT);
	 g_MapServer.MapDBQuery(pQuery);
 }

 // ����ͳ���ϱ�
//  void MapServer::OnStatisticsReport(WORD wType, const char *pszContext)
//  {
// 	 MSG_M2O_STATISTICS_NOTIFY msg;
// 	 msg.m_wType = wType;
// 	 strcpy_s(msg.m_szContext, sizeof(msg.m_szContext), pszContext);
// 	 m_pOpenServerSession->SendPacket(0, msg.GetType(), &msg);
//  }

 // ����ͳ���ϱ�
 void MapServer::OnStatisticsReport(DWORD dwRoleId, DWORD dwObjectId, DWORD dwOjbectNum, bool bIsAdd, WORD wFromId)
 {
	 // todo::��ʱ�������� д������
	 if (bIsAdd)
	 {
		 MAPLOG(SPECAIL_LEVEL, "��ӵ���[RoleId:%d,ObjectId:%d,OjbectNum:%d]", dwRoleId, dwObjectId, dwOjbectNum);
	 }
	 else
	 {
		 MAPLOG(SPECAIL_LEVEL, "ɾ������[RoleId:%d,ObjectId:%d,OjbectNum:%d]", dwRoleId, dwObjectId, dwOjbectNum);
	 }

	 BYTE byObjType = GetObejctTypeById(dwObjectId);
	 // װ��,Ӣ��,����,���� ��Ʒ�ʼ�¼(��ɫ�����ϲż�¼)
	 if (byObjType == OBJECT_EQUIPMENT || byObjType == OBJECT_HERO || byObjType == OBJECT_TREASURE || byObjType == OBJECT_CARD)
	 {
		 // �ڰ�λ����Ʒ��
		DWORD dwQuality = dwObjectId * 0.01;
		if (dwQuality)
			dwQuality = dwQuality % 10;

		// ��ɫ���µ�Ʒ�ʲ���Ҫ��¼
		if (dwQuality < QUALITY_PURPLE)
			return;
	 }

	 // ����Ϊֱ�Ӵ洢���ݿ�
	 ItemOperationRecord_Save* pQuery = ItemOperationRecord_Save::ALLOC();
	 pQuery->SetIndex(DB_ROLE_ITEM_OPERATION_RECORD_SAVE);
	 pQuery->SetThreadIndex(DB_THREAD_COMMON);
	 ItemOperationRecord tOperationRecord;
	 tOperationRecord.wFromId = wFromId;
	 tOperationRecord.dwRoleId = dwRoleId;
	 tOperationRecord.dwItemId = dwObjectId;
	 tOperationRecord.dwItemNum = dwOjbectNum;
	 tOperationRecord.dwTime = time(NULL);
	 tOperationRecord.bIsAdd = bIsAdd;
	 pQuery->SetRecord(tOperationRecord);
	 g_MapServer.MapDBQuery(pQuery);

// 	 Json::Value root;
// 	 root["role_id"] = int(dwRoleId);
// 	 root["object_id"] = int(dwObjectId);
// 	 root["object_num"] = int(dwOjbectNum);
// 	 root["is_add"] = int(bIsAdd);
// 	 string strContext = root.toStyledString();
// 	 OnStatisticsReport(wType, strContext.c_str());
 }

 // ���ս���Ƿ�Ϸ�
 BOOL MapServer::CheckPowerIsValid(DWORD dwServerPower, DWORD dwClientPower)
 {
	 const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(220);
	 if (pServerCfg && IsCheckPower())
	 {
		 DWORD dwRate = pServerCfg->GetListValue(0);	// ������
		 DWORD dwValue = pServerCfg->GetListValue(1);	// ���ֵ

		 if (dwClientPower > (dwServerPower + dwServerPower * dwRate + dwValue))
			 return false;
	 }

	 return true;
 }

 // ���׼��ʹ�õĽ�ɫ����
 void MapServer::AddReadyToUseRoleName(const char *pszRoleName)
 {
	 m_ReadyToUseRoleNameList.insert(make_pair(pszRoleName, DWORD(time(NULL))));
 }

 // �Ƿ�׼��ʹ�õĽ�ɫ����
 bool MapServer::IsReadyToUseRoleName(const char *pszRoleName)
 {
	 std::map<std::string, DWORD>::iterator iter = m_ReadyToUseRoleNameList.find(pszRoleName);
	 return iter != m_ReadyToUseRoleNameList.end() ? true : false;
 }

 // ������ڴ�����ɫ����
 void MapServer::AddCreatingRoleData(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId)
 {
	 char szKey[128] = { 0 };
	 sprintf_s(szKey, sizeof(szKey), "%s_%d_%d", pszAccount, dwChannelId, dwServerId);
	 m_CreatingRoleList.insert(make_pair(szKey, DWORD(time(NULL))));
 }

 // ɾ������׼����ɫ����
 void MapServer::RemoveCreatingRoleData(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId)
 {
	 char szKey[128] = { 0 };
	 sprintf_s(szKey, sizeof(szKey), "%s_%d_%d", pszAccount, dwChannelId, dwServerId);
	 m_CreatingRoleList.erase(szKey);
 }

 // �Ƿ����ڴ�����ɫ 
 bool MapServer::IsInCreatingRole(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId)
 {
	 char szKey[128] = { 0 };
	 sprintf_s(szKey, sizeof(szKey), "%s_%d_%d", pszAccount, dwChannelId, dwServerId);

	 std::map<std::string, DWORD>::iterator iter = m_CreatingRoleList.find(szKey);
	 return iter != m_CreatingRoleList.end() ? true : false;
 }

 // ���Ť���齱��¼
 void MapServer::AddNDLotteryRecord(DWORD dwServerId, NDLotteryRecord &tRecord)
 {
	 map<DWORD, deque<NDLotteryRecord>>::iterator iter = m_LotteryRecordList.find(dwServerId);
	 if (iter != m_LotteryRecordList.end())
	 {
		 iter->second.push_back(tRecord);

		 // ֻȡ���10����¼ todo::��Ҫ����Ϊ����
		 if (iter->second.size() > 10)
		 {
			 iter->second.pop_front();
		 }
	 }
	 else
	 {
		 deque<NDLotteryRecord> list;
		 list.push_back(tRecord);
		 m_LotteryRecordList.insert(make_pair(dwServerId, list));
	 }
 }

 // ȡŤ���齱��¼�б�
 const deque<NDLotteryRecord>* MapServer::GetNDLotteryRecord(DWORD dwServerId)
 {
	 map<DWORD, deque<NDLotteryRecord>>::iterator iter = m_LotteryRecordList.find(dwServerId);
	 return iter != m_LotteryRecordList.end() ? &iter->second : NULL;
 }

 // ����Ϸ��߼�
 void MapServer::OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId)
 {
	 // 	 DWORD dwFromServerId = pParamList[0];	// ���ϵķ�����ID
	 // 	 DWORD dwToServerId = pParamList[1];	// �Ͻ�ȥ�ķ�����ID

	 /*
		ս��			ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�ȼ�			ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�����Ǽ�		ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�����Ǽ�		ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�����ղ�ֵ		ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�޾�֮��		ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		������			ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		ת��			ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�Ծ�			ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�Ѿ��˺�		ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		�Ѿ���ѫ		ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		����BOSS�˺�	ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		����BOSS����	ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������
		����			ֱ���õ�ǰ��Ҫ�Ϸ��ķ����������а��������

		����			����Ϊȫ�������ˣ������Ҫ���´�����
		�ռ�����		��������ǿ�������У����ô���
	 */

	 UINT64 lCurTime = GetTickCount64();
	 MAPLOG(ERROR_LEVEL, "��ʼ�Ϸ�[FromServerId:%d,ToServerId:%d]", dwFromServerId, dwToServerId);

	 // �������а�
	 CRankManager::Instance()->OnMergeServer(dwFromServerId, dwToServerId);

	 // ��������������(�������Ķ�����)
	 CArenaManager::Instance()->ResetRankData(dwFromServerId);
	 CArenaManager::Instance()->ResetRankData(dwToServerId);

	 //CWorldBossManager::Instance()->ResetRankData(dwFromServerId);

	 CGuildManager::Instance()->OnMergeServer(dwFromServerId, dwToServerId);	 

	 // ���ݴ洢
	 CRankManager::Instance()->SaveAllDataToDB();
	 CArenaManager::Instance()->SaveAllData();

	 DWORD dwCostTime = GetTickCount64() - lCurTime;
	 MAPLOG(ERROR_LEVEL, "�Ϸ�����[FromServerId:%d,ToServerId:%d,CostTime:%d]", dwFromServerId, dwToServerId, dwCostTime);
 }


VOID MapServer::OnNewDay()
{
	PlayerManager::Instance()->OnNewDay();
	CRankManager::Instance()->OnNewDay();
	CWorldBossManager::Instance()->OnNewDay();
	CGuildManager::Instance()->OnNewDay();
	CScoreMatchManager::Instance()->OnNewDay();
	CGuildWarMgr::Instance()->OnNewDay();

	// ֪ͨ�������
	MSG_S2C_NEW_DAY_NOTIFY msg;
	g_MapServer.SendMsgToGate(0, S2C_NEW_DAY_NOTIFY, &msg);

	// ����ͳ���¼�
	CGameLogic::Instance()->OnGameStatistics_NewDay();

	m_dwCurMaxOnlineRoleNum = 0;

	MAPLOG(ERROR_LEVEL, "�µ�һ���¼�");
}

// �µ�һ��
VOID MapServer::OnNewWeek()
{
	PlayerManager::Instance()->OnNewWeek();
	CGuildManager::Instance()->OnNewWeek();

	MAPLOG(ERROR_LEVEL, "�µ�һ���¼�");
}

// �µ�һ��
VOID MapServer::OnNewMonth()
{
	PlayerManager::Instance()->OnNewMonth();

	MAPLOG(ERROR_LEVEL, "�µ�һ���¼�");
}

//ע��DB��ѯ���������
void MapServer::Register()
{
	//ע��DB��ѯ���������
	AddHandler(MAP_QUERY_CHECK_ACCOUNT,				DBResultHandler::ProcessCheckAccountQueryResult);
	AddHandler(DB_SDK_ACCOUNT_CHECK,				DBResultHandler::ProcessSDKAccountCheckResult);
	AddHandler(MAP_QUERY_CREATE_ACCOUNT,			DBResultHandler::ProcessCreateAccountQueryResult);
	AddHandler(DB_SDK_CREATE_ACCOUNT,				DBResultHandler::ProcessSDKCreateAccountResult);
	
	AddHandler(MAP_QUERY_CHAR_LIST,					DBResultHandler::ProcessSelectZoneRoleQueryResult);
	AddHandler(MAP_QUERY_CREATE_CHAR,				DBResultHandler::ProcessCreateCharQueryResult);
	AddHandler(MAP_QUERY_CHAR_INFO,					DBResultHandler::ProcessSelectCharInfoQueryResult);
	AddHandler(MAP_QUERY_UPDATE_CHAR,				DBResultHandler::ProcessUpdateCharQueryResult);
	AddHandler(MAP_QUERY_CHANGE_PASSWORD,			DBResultHandler::ProcessAccountPasswordModifyResult);
	AddHandler(MAP_QUERY_CHANGE_MOBILE,				DBResultHandler::ProcessAccountMobileChangeResult);
	
	
	AddHandler(MAP_UPDATE_LAST_LOGIN_SERVER,		DBResultHandler::ProcessUpdateLastLoginServer);
	AddHandler(MAP_SERVER_GOLBALDATA_SAVE,			DBResultHandler::SaveGolbalDataResult);
	AddHandler(MAP_SERVER_GOLBALDATA_LOAD,			DBResultHandler::LoadGolbalDataResult);
	AddHandler(DB_ROLE_BASE_DATA_LIST_LOAD,			DBResultHandler::LoadRoleBaseDataListResult);
	AddHandler(DB_FRIEND_DATA_UPDATE,				DBResultHandler::UpdateFriendDataResult);
	AddHandler(DB_ARENA_DATA_SAVE,					DBResultHandler::SaveArenaDataResult);
	AddHandler(DB_ARENA_DATA_LOAD,					DBResultHandler::LoadArenaDataResult);
	AddHandler(DB_ROLE_RANK_DATA_SAVE,				DBResultHandler::SaveRoleRankDataResult);
	AddHandler(DB_ROLE_RANK_DATA_LOAD,				DBResultHandler::LoadRoleRankDataResult);
	AddHandler(MAP_QUERY_INSERT_EMAIL,				DBResultHandler::SaveEmailDataResult);
	AddHandler(DB_GM_EVENT_DATA_LOAD,				DBResultHandler::LoadAndDeleteGMEventResult);
	AddHandler(DB_SERVER_LIST_LOAD,					DBResultHandler::LoadServerListResult);
	AddHandler(DB_ARENA_BATTLE_RECORD_SAVE,			DBResultHandler::SaveArenaBattleRecordResult);
	AddHandler(DB_ARENA_BATTLE_RECORD_LOAD,			DBResultHandler::LoadArenaBattleRecordResult);
	AddHandler(DB_ROLE_CURRENCY_LOGS_SAVE,			DBResultHandler::SaveRoleCurrencyLogsResult);
	AddHandler(DB_LAND_DATA_SAVE,					DBResultHandler::SaveLandDataResult);
	AddHandler(DB_LAND_DATA_LOAD,					DBResultHandler::LoadLandDataResult);
	AddHandler(DB_ARENA_RANK_VIEW_LIST_UPDATE,		DBResultHandler::UpdateArenaRankViewListResult);
	AddHandler(DB_LEAVE_MESSAGE_SAVE,				DBResultHandler::SaveLeaveMessageResult);
	AddHandler(DB_REWARD_CENTER_DATA_SAVE,			DBResultHandler::SaveRewardCenterDataResult);
	AddHandler(DB_ROLE_TEMP_DATA_UPDATE,			DBResultHandler::UpdateRoleTempDataResult);
	AddHandler(DB_ROLE_OFFLINE_RECORD_SAVE,			DBResultHandler::SaveRoleOfflineRecordResult);
	AddHandler(DB_ROLE_LEVEL_RECORD_SAVE,			DBResultHandler::SaveRoleLevelRecordResult);
	AddHandler(DB_ROLE_RECHARGE_RECORD_SAVE,		DBResultHandler::SaveRoleRechargeRecordResult);
	AddHandler(DB_ROLE_MAP_LOSE_RECORD_SAVE,		DBResultHandler::SaveRoleMapLoseRecordResult);
	AddHandler(DB_ROLE_FIRST_PASS_MAP_RECORD_SAVE,	DBResultHandler::SaveRoleFirstPassMapRecordResult);
	AddHandler(DB_SHOP_BUY_RECORD_SAVE,				DBResultHandler::SaveShopBuyRecordResult);
	AddHandler(DB_ROLE_ITEM_LOSE_RECORD_SAVE,		DBResultHandler::SaveItemLoseRecordResult);
	AddHandler(DB_ROLE_ITEM_OPERATION_RECORD_SAVE,	DBResultHandler::SaveItemOperationRecordResult);
	AddHandler(DB_ROLE_ACCOUNT_LOCK_UPDATE,			DBResultHandler::UpdateAccountLockResult);
	AddHandler(DB_OFFLINE_PLAYER_NUM_RECORD_SAVE,	DBResultHandler::SaveOfflinePlayerNumRecordResult);
	
	
	AddHandler(DB_GUILD_DATA_SAVE,					DBResultHandler::SaveGuildDataResult);
	AddHandler(DB_GUILD_DATA_LOAD,					DBResultHandler::LoadGuildDataResult);
	AddHandler(DB_CHAT_RECORD_SAVE,					DBResultHandler::SaveChatRecordResult);
	AddHandler(DB_CHAT_RECORD_LOAD,					DBResultHandler::LoadChatRecordResult);
	AddHandler(DB_LOG_SAVE,							DBResultHandler::SaveLogsResult);
	AddHandler(DB_MAP_COMMENT_DATA_SAVE,			DBResultHandler::SaveMapCommentResult);
	AddHandler(DB_MAP_COMMENT_DATA_LOAD,			DBResultHandler::LoadMapCommentResult);
	
	AddHandler(DB_KING_MATCH_DATA_SAVE,				DBResultHandler::SaveKingMatchDataResult);
	AddHandler(DB_KING_MATCH_DATA_LOAD,				DBResultHandler::LoadKingMatchDataResult);

	AddHandler(DB_SCORE_MATCH_DATA_SAVE,			DBResultHandler::SaveScoreMatchDataResult);
	AddHandler(DB_SCORE_MATCH_DATA_LOAD,			DBResultHandler::LoadScoreMatchDataResult);

	AddHandler(DB_TEAM_DUNGEON_RANK_DATA_SAVE,		DBResultHandler::SaveTeamDungeonRankDataResult);
	AddHandler(DB_TEAM_DUNGEON_RANK_DATA_LOAD,		DBResultHandler::LoadTeamDungeonRankDataResult);

	AddHandler(DB_TURNTABLE_DATA_SAVE,				DBResultHandler::SaveTurntableDataResult);
	AddHandler(DB_TURNTABLE_DATA_LOAD,				DBResultHandler::LoadTurntableDataResult);

	AddHandler(DB_NEW_ROLE_GUIDE_UPDATE,			DBResultHandler::UpdateNewRoleGuideResult);

	AddHandler(DB_RESOURCE_WAR_DATA_SAVE,			DBResultHandler::SaveResourceWarDataResult);
	AddHandler(DB_RESOURCE_WAR_DATA_LOAD,			DBResultHandler::LoadResourceWarDataResult);

	AddHandler(DB_GUILD_WAR_DATA_SAVE,				DBResultHandler::SaveGuildWarDataResult);
	AddHandler(DB_GUILD_WAR_DATA_LOAD,				DBResultHandler::LoadGuildWarDataResult);

	AddHandler(DB_ACTIVATION_CODE_SELECT,			DBResultHandler::SelectActivityCodeResult);
	AddHandler(DB_ACTIVATION_CODE_UPDATE,			DBResultHandler::UpdateActivityCodeResult);
	
	AddHandler(DB_GAME_STATISTICS_EVENT,			DBResultHandler::GameStatisticsEventResult);

	AddHandler(DB_GM_OPERATION_RECORD_SAVE,			DBResultHandler::GMOperationRecordSaveResult);
}

//���DB��ѯ���������
void MapServer::AddHandler(DWORD dwIndex, fnProcessQueryResult fnHandler)
{
	m_ProcessQueryMap.insert(std::make_pair(dwIndex,fnHandler));
}

//����DB��ѯ���������
fnProcessQueryResult MapServer::Find(DWORD dwIndex)
{
	QueryResultMap::iterator it = m_ProcessQueryMap.find(dwIndex);
	if(it==m_ProcessQueryMap.end())
		return NULL;

	return it->second;
}

void MapServer::UpdateOpenBeat(DWORD dwDeltaTick )
{
  	m_OpenBeat.m_dwTime += dwDeltaTick;
  	switch(m_OpenBeat.eStatus)
  	{
  	case BeatStatus_Send:
  		{
  			if (m_OpenBeat.m_dwTime>= 30*1000)
  			{
  				SendOpen2Beat();
  				m_OpenBeat.eStatus = BeatStatus_Wait;
  				m_OpenBeat.m_dwTime = 0;
  			}	
  		}
  		break;
  	case BeatStatus_Wait:
  		{
  			if (m_OpenBeat.m_dwTime>= 20*1000)
  			{
  				GetOpenSession()->Disconnect(TRUE);
  				m_OpenBeat.eStatus = BeatStatus_NewConnect;
  				m_OpenBeat.m_dwTime = 0;
  			}	
  		}
  		break;
  	case BeatStatus_NewConnect:
  		{
			if (m_OpenBeat.m_dwTime >= 10 * 1000)
			{
				ConnectToOpenServer();
				MSG_M2O_SERVERID_CMD Msg;
				Msg.m_dwServerID = g_MapServer.GetServerID();
				GetOpenSession()->SendPacket(0, Msg.GetType(), &Msg);
				m_OpenBeat.eStatus = BeatStatus_Send;
				m_OpenBeat.m_dwTime = 0;
			}
  		}
  		break;
  	}
}
void  MapServer::UpdateGateBeat(DWORD dwDeltaTick )
{
	m_GateBeat.m_dwTime += dwDeltaTick;
	switch(m_GateBeat.eStatus)
	{
	case BeatStatus_Send:
		{
			// 30�뷢��
			if (m_GateBeat.m_dwTime>= 30*1000)
			{
				SendGate2Beat();
				m_GateBeat.eStatus = BeatStatus_Wait;
				m_GateBeat.m_dwTime = 0;
			}	
		}
		break;
	case BeatStatus_Wait:
		{
			// �������ȴ�30��,ľ���յ���Ӧ��Ͽ�����
			if (m_GateBeat.m_dwTime>= 20*1000)
			{
				GetGateSession()->OnDisconnect();
				m_GateBeat.eStatus = BeatStatus_NewConnect;
				m_GateBeat.m_dwTime = 0;

			}	
		}
		break;
	case BeatStatus_NewConnect:
		{
			// �յ���������ʱ,�ȴ�10��ִ��
			if (m_GateBeat.m_dwTime>= 10*1000)
			{
				ConnectToGateServer();
				MAPLOG(GUI_LEVEL, "Connect to the GeteServer agin. ConnectNum=%d",++m_GateBeat.m_dwConnectNum);
				m_GateBeat.eStatus = BeatStatus_Send;
				m_GateBeat.m_dwTime = 0;
			}	

		}
		break;
	}

}
void  MapServer::InitOpenBeat()
{
	m_OpenBeat.eStatus = BeatStatus_Send;
	m_OpenBeat.m_dwTime = 0;
}

void MapServer::UpdateWaitingLoginAccount()
{
	if (!m_WaitingLoginAccountList.size())
		return;

	DWORD dwCurTime = time(NULL);
 	std::map<String, LoginAccountInfo>::iterator iter = m_WaitingLoginAccountList.begin();
 	for (; iter != m_WaitingLoginAccountList.end(); iter++)
 	{
 		// 1��ĵȴ�ʱ��
 		if (dwCurTime >= iter->second.dwTime + 1)
 		{
			LoginAccountInfo *pLoginInfo = &(iter->second);

			// ������������,����ǰһ�εĵ�¼���ڵȴ����ݴ洢�������
			if (AccountInfoManager::Instance()->GetAccountInfoByAccID(pLoginInfo->dwChannelUID, pLoginInfo->szAccountName))
			{
				// ���½���ȴ�ʱ��
				pLoginInfo->dwTime = dwCurTime; // time(NULL);
				//g_MapServer.SendErrorMsgToUser(iter->second.dwGuid, ERROR_LOGIN_PLAYER_FULL);
				MAPLOG(GUI_LEVEL, "�������ߵ���һ��ڻ�����[AccountName:%s,RetryTimes:%d]", pLoginInfo->szAccountName, pLoginInfo->dwRetryTimes++);

				// ���Զ���޷���¼ʱ,ֱ���õ�(todo::�ݶ�5��)
				if (pLoginInfo->dwRetryTimes >= 5)
				{
					m_WaitingLoginAccountList.erase(iter);
					return;
				}
				else
				{
					continue;
				}
			}

			// ����ʺ�
			CheckAccountQuery* pQuery = CheckAccountQuery::ALLOC();
			pQuery->SetIndex(pLoginInfo->bIsSDK ? DB_SDK_ACCOUNT_CHECK : MAP_QUERY_CHECK_ACCOUNT);
			pQuery->SetThreadIndex(0);
			pQuery->SetKey(pLoginInfo->dwGuid);
			pQuery->SetAccID(pLoginInfo->szAccountName);
			pQuery->SetPassword(pLoginInfo->szPassword);
			pQuery->SetChannelUID(pLoginInfo->dwChannelUID);
			pQuery->SetLoginType(pLoginInfo->byLoginType);
			pQuery->SetTerminalype(pLoginInfo->byTerminalType);
			g_MapServer.MapDBQuery(pQuery);

			MAPLOG(GUI_LEVEL,"�������¼�����б�[%s]",pLoginInfo->szAccountName);
			m_WaitingLoginAccountList.erase(iter);
 			break;
 		}
 	}
}

void  MapServer::InitGateBeat()
{
	m_GateBeat.eStatus = BeatStatus_Send;
	m_GateBeat.m_dwTime = 0;
}

//��ֹ��ͼ�������߳�
void MapServerThread::EndThread()
{
	g_MapServer.SetShutDown(TRUE);
}

//���е�ͼ�������߳�
DWORD MapServerThread::run()
{
	IF_NOT (g_MapServer.Init())
	{
		g_MapServer.SetShutDown();
		MAPLOG(GUI_LEVEL, "");
		MAPLOG(GUI_LEVEL, "Please Check the Configuration and Reboot the Server!");
	}

	g_MapServer.Run();

	g_MapServer.Release();

	return 0;
}

NetworkObject * CreateServerSideAcceptedObject()
{
	return NULL;
}

VOID DestroyServerSideAcceptedObject( NetworkObject * pNetworkObject )
{

}

VOID DestroyServerSideConnectedObject( NetworkObject * pNetworkObject )
{

}

void LogPrinter(const char* msg)
{
	g_MapServer.WriteLog(ERROR_LEVEL, msg);
}

//��ͼ�������ص���������
void Map_CallBackQueryResult( DWORD dwIndex, QueryResult * pData )
{
	g_MapServer.DBResult((MAP_QUERY_INDEX)dwIndex,pData);
}


VOID MapServer::ShowPlayerList()
{
	PlayerManager::Instance()->ShowPlayerList();
}

VOID MapServer::StopServer()
{
	//ChatManager::Instance()->SendBroadcastMsg(BROADCAST_ID_1);
}

//  ע����Ҫ���½�ɫ��Ϣ����Ϣ
void MapServer::ResisterNotChangeRoleInfoMsg()
{
// 	AddNotChangeRoleInfoMsg(C2S_HEART_BEAT_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_SERVER_TIME_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_SDK_ORDER_ID_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_RECHARGE_SUCCESS_REPORT_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_ROLE_DETAILS_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_HERO_GOTO_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_EMAIL_LIST_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_CHAT_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_CHAT_RECORD_LIST_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_HELP_BATTLE_TARGET_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_EQUIPMENT_LIST_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_ATTACK_CITY_GOTO_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ);
// 	AddNotChangeRoleInfoMsg(C2S_RED_POINT_LIST_REQ);
	

	AddNotChangeRoleInfoMsg(C2S_LOGIN_ACCOUNT_REQ);
	AddNotChangeRoleInfoMsg(C2S_LOGIN_SELECT_SERVER_REQ);
	AddNotChangeRoleInfoMsg(C2S_LOGIN_CREATE_ROLE_REQ);
	AddNotChangeRoleInfoMsg(C2S_START_GAME_REQ);
	AddNotChangeRoleInfoMsg(C2S_ACCOUNT_REGISTER_REQ);
	AddNotChangeRoleInfoMsg(C2S_ACCOUNT_PASSWORD_CHANGE_REQ);
	AddNotChangeRoleInfoMsg(C2S_ACTIVITY_NOTICE_REQ);
	AddNotChangeRoleInfoMsg(C2S_AUTHENTICATION_CODE_REQ);
	AddNotChangeRoleInfoMsg(C2S_MOBILE_BIND_REQ);
	AddNotChangeRoleInfoMsg(C2S_SDK_LOGIN_REQ);
	AddNotChangeRoleInfoMsg(C2S_SDK_ORDER_ID_REQ);
	AddNotChangeRoleInfoMsg(C2S_RECHARGE_SUCCESS_REPORT_REQ);
	AddNotChangeRoleInfoMsg(C2S_HEART_BEAT_REQ);
	AddNotChangeRoleInfoMsg(C2S_SERVER_TIME_REQ);
	AddNotChangeRoleInfoMsg(C2S_ROLE_DETAILS_REQ);
	AddNotChangeRoleInfoMsg(C2S_HERO_GOTO_REQ);
	AddNotChangeRoleInfoMsg(C2S_ENTER_SCENE_REQ);
	AddNotChangeRoleInfoMsg(C2S_LEAVE_SCENE_REQ);
	AddNotChangeRoleInfoMsg(C2S_SCENE_ITEM_LOCK_REQ);
	AddNotChangeRoleInfoMsg(C2S_SCENE_ITEM_PICK_UP_REQ);
	AddNotChangeRoleInfoMsg(C2S_EMAIL_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_CHAT_REQ);
	AddNotChangeRoleInfoMsg(C2S_CHAT_RECORD_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_EQUIPMENT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_EQUIPMENT_INFO_REQ);
	AddNotChangeRoleInfoMsg(C2S_EQUIPMENT_FRAGMENT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_RANK_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_RANK_LEAVE_MESSAGE_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_RANK_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_MY_RANK_REQ);
	AddNotChangeRoleInfoMsg(C2S_SHOP_GOODS_BUY_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_SHOP_LIMIT_GOODS_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_FB_MAIN_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_FB_AWARD_GET_REQ);
	AddNotChangeRoleInfoMsg(C2S_FB_DAILY_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_FB_HERO_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_FB_ELITE_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ);
	AddNotChangeRoleInfoMsg(C2S_ITEM_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_HERO_BASE_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_HERO_INFO_REQ);
	AddNotChangeRoleInfoMsg(C2S_FRAGMENT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_HERO_COLLECTION_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_HERO_SUMMON_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_FRIEND_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_BLACK_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_FRIEND_APPLY_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_FRIEND_RECOMMEND_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_FRIEND_TARGET_FIND_REQ);
	AddNotChangeRoleInfoMsg(C2S_TREASURE_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_REBEL_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_RECYCLE_DECOMPOSE_RETURN_REQ);
	AddNotChangeRoleInfoMsg(C2S_RECYCLE_RESET_RETURN_REQ);
	AddNotChangeRoleInfoMsg(C2S_DUOBAO_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_DUOBAO_TARGET_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_TOWER_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_PLAYER_LAND_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_FRIEND_LAND_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_LAND_CITY_DETAILS_REQ);
	AddNotChangeRoleInfoMsg(C2S_HEART_OF_WORLD_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_SPIRIT_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_DATE_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_APPLY_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_KICKOUT_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_LEAVE_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_LEAVE_MESSAGE_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_SEARCH_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_APPLY_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_LEAVEMSG_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_MEMBER_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_EVENT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_SKILL_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_FB_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_FB_MAP_REWARD_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_FB_DAMAGE_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_MONSTER_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_GUILD_MY_MAP_REWARD_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_ARENA_TARGET_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_ARENA_BATTLE_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_ACHIEVEMENT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_DAILY_MISSION_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_WORLD_BOSS_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_WORLD_BOSS_REWARD_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_WORLD_BOSS_BATTLE_RECORD_REQ);
	AddNotChangeRoleInfoMsg(C2S_REWARD_CENTER_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_SIGN_IN_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_LUXURY_SIGN_IN_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_YCS_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_TQT_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_SEVEN_DAY_TARGET_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_SCORE_MATCH_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_SCORE_MATCH_REFRESH_TARGET_REQ);
	AddNotChangeRoleInfoMsg(C2S_ROLE_TITLE_INFO_REQ);
	AddNotChangeRoleInfoMsg(C2S_RED_POINT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_OPEN_SERVER_FUND_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_LEVEL_GIFT_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_PLAYER_LEVEL_MESSAGE_REQ);
	AddNotChangeRoleInfoMsg(C2S_MAP_COMMENT_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_RECHARGE_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_MONTH_CARD_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_PVP_SEARCH_TARGET_REQ);
	AddNotChangeRoleInfoMsg(C2S_PVP_READY_FINISH_REQ);
	AddNotChangeRoleInfoMsg(C2S_PVP_USE_SKILL_REQ);
	AddNotChangeRoleInfoMsg(C2S_PVP_HP_CHANGE_REQ);
	AddNotChangeRoleInfoMsg(C2S_PVP_CREATE_BUFF_REQ);
	AddNotChangeRoleInfoMsg(C2S_PLAYER_PVP_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_PVP_CANCEL_SEARCH_TARGET_REQ);
	AddNotChangeRoleInfoMsg(C2S_LIMIT_FIRST_RECHARGE_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_LIMIT_RECRUIT_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_ATTACK_CITY_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_ATTACK_CITY_ENTER_REQ);
	AddNotChangeRoleInfoMsg(C2S_ATTACK_CITY_LEAVE_REQ);
	AddNotChangeRoleInfoMsg(C2S_ATTACK_CITY_GOTO_REQ);
	AddNotChangeRoleInfoMsg(C2S_TREASURE_BOX_LOCK_REQ);
	AddNotChangeRoleInfoMsg(C2S_TREASURE_BOX_PICK_UP_REQ);
	AddNotChangeRoleInfoMsg(C2S_ATTACK_CITY_RELIVE_REQ);
	AddNotChangeRoleInfoMsg(C2S_ENDLESS_LAND_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
	AddNotChangeRoleInfoMsg(C2S_ENDLESS_LAND_COLLISION_REQ);
	AddNotChangeRoleInfoMsg(C2S_NEXT_DAY_REWARD_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_SUPER_DISCOUNT_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_FESTIVAL_ACTIVITY_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_ACCURATE_RECOMMEND_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_GEM_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_DIALOG_OVER_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_NEXT_NPC_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_LEAVE_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_CREATE_TEAM_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_INVITE_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_MATCH_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ);
	AddNotChangeRoleInfoMsg(C2S_LOGIN_REWARD_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_JOIN_IN_REQ);

	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_TASK_START_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_RANK_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ);
	AddNotChangeRoleInfoMsg(C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ);


	AddNotChangeRoleInfoMsg(C2S_LOGIN_REQ);

	AddNotChangeRoleInfoMsg(C2S_CHRISTMAS_DATA_REQ);
	AddNotChangeRoleInfoMsg(C2S_CHRISTMAS_BATTLE_ENTER_REQ);
	AddNotChangeRoleInfoMsg(C2S_CHRISTMAS_BATTLE_LEAVE_REQ);

	AddNotChangeRoleInfoMsg(C2S_NEW_SUPER_DISCOUNT_DATA_REQ);
}

