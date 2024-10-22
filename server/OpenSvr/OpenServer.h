#pragma once

#include "../Common/Const.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/LogFile.h"
#include "../ToolKit/DBCFile.h"
#include "HttpRequestManager.h"
#include <hash_map>
#include "HttpQueryResult.h"
#include <map>
#include "server.pb.h"
#include "json/value.h"
#include "../Common/DBCDefine.h"

typedef void (*fnProcessQueryResult)( HttpQueryResult * pData );
//typedef std::hash_map<DWORD,fnProcessQueryResult>	ProcessQueryResultMap;
typedef std::map<DWORD,fnProcessQueryResult>	ProcessQueryResultMap;

class IOCPServer;
class NetMsgBody;
class MapServerSession;
//class HttpAuthQuery;
class google::protobuf::Message;

//typedef std::map<DWORD,HttpAuthQuery>  AccMap;
//typedef std::map<DWORD,AccMap>  AllAcc;

// Json�ֶ�תobject
bool Json2Object(Json::Value &json_object, const char *pszKey, const char *pszFuncName, Json::Value &object);

// Json�ֶ�תsring
bool Json2String(Json::Value &json_object, const char *pszKey, const char *pszFuncName, std::string &strValue);

// Json�ֶ�תint
bool Json2Int(Json::Value &json_object, const char *pszKey, const char *pszFuncName, int &nValue);

class NetworkObject;

// ��������
enum
{
	CHANNEL_IOS_HANFENG_1 = 10004,		// ����1(���ߴ��Ҷ�)
	CHANNEL_IOS_HANFENG_2 = 10005,		// ����2(Ӣ�۴���ս)
	CHANNEL_IOS_HANFENG_3 = 10006,		// ����3(���ܾ���)
	CHANNEL_IOS_HANFENG_4 = 10007,		// ����4(���� ����ս��)
	CHANNEL_IOS_HANFENG_5 = 10008,		// ����5(����Ӣ��)
	CHANNEL_IOS_HANFENG_6 = 10009,		// ����6(��������)

	CHANNEL_IOS_HUIYU = 10101,		// ����

	CHANNEL_IOS_YUWAN_1 = 10201,		// ����1(��������)
	CHANNEL_IOS_YUWAN_2 = 10202,		// ����2(Ӣ�۳����޵�)

	CHANNEL_IOS_29_1 = 10301,		// 29��
	CHANNEL_IOS_29_2 = 10302,		// 29��
	CHANNEL_IOS_29_3 = 10303,		// 29��
	CHANNEL_IOS_29_4 = 10304,		// 29��
	CHANNEL_IOS_29_5 = 10305,		// 29��
	CHANNEL_IOS_29_6 = 10306,		// 29��

	CHANNEL_IOS_YD = 10401,		// �ƶ�ios

	
	CHANNEL_XMGT_IOS_XT_1 = 10501,		// �����̨ios(��ͨ1)
	CHANNEL_XMGT_IOS_XT_2 = 10502,		// �����̨ios(��ͨ2)
	CHANNEL_XMGT_IOS_XT_3 = 10503,		// �����̨ios(��ͨ2)
	CHANNEL_XMGT_IOS_BEGIN = 10501,
	CHANNEL_XMGT_IOS_END = 10599,

	CHANNEL_XMGT_ANDROID_XT = 10601,		// �����̨android(��ͨ)
	CHANNEL_XMGT_ANDROID_BEGIN = 10601,
	CHANNEL_XMGT_ANDROID_END = 10699,

	CHANNEL_IOS_LY_XT = 10701,		// IOS����(��ͨ)

	CHANNEL_IOS_LY_WANMI_1 = 10801,		// ��������(����Ӣ���ֻ���)
	CHANNEL_IOS_LY_WANMI_2 = 10802,		// ��������(������Ӣ��)
	CHANNEL_IOS_LY_WANMI_3 = 10803,		// ��������(����Ӣ��֮�ռ��Ծ�)
	CHANNEL_IOS_LY_WANMI_4 = 10804,		// ��������(����Ӣ��֮�ռ��Ծ�)
	CHANNEL_IOS_LY_WANMI_5 = 10805,		// ��������(����Ӣ��֮�ռ��Ծ�)

	CHANNEL_IOS_LY_QINGYUAN_1 = 10901,		// ��ԴIOS����(�籩����)
	CHANNEL_IOS_LY_QINGYUAN_2 = 10902,		// ��ԴIOS����(����Ӣ��)

	CHANNEL_IOS_LY_YUNRUI = 11001,		// ����IOS����

	CHANNEL_IOS_LY_TIANYU = 11101,		// ����IOS����

	CHANNEL_IOS_LY_HAODONG = 11201,		// �ƶ�IOS����

	CHANNEL_CGAME_BT_BEGIN = 20301,	// cgame_bt
	CHANNEL_CGAME_BT_END = 20399,	// cgame_bt

	CHANNEL_GL_ENGLIST_IOS_BEGIN = 20001,		// ȫ��Ӣ�İ汾(IOS)
	CHANNEL_GL_ENGLIST_IOS_END = 20099,
	CHANNEL_GL_ENGLIST_ANDROID_BEGIN = 20101,	// ȫ��Ӣ�İ汾(��׿)
	CHANNEL_GL_ENGLIST_ANDROID_END = 20199,

	CP_CHANNEL_TYPE_BEGIN = 10000,
	CP_CHANNEL_TYPE_END = 50000,
};

class OpenServer
{
	enum { SERVER_IOHANDLER = 0,PHP_IOHANDLER = 1, };
public:
	OpenServer(void);
	~OpenServer(void);

public:
	BOOL					Init();
	void					Release();
	void					Run();
	BOOL					Update();

	void					StartServerListen();
	void					StartPHPListen();

	//void					SendMsgToMapServer(DWORD dwServerId,DWORD dwGUID, google::protobuf::Message *pMsg);
	void					SendMsgToMapServer(DWORD dwGUID, NetMsgBody * pMsg, DWORD dwServerId = 1);

	void					SetShutDown(BOOL bShutDown = TRUE) { m_bShutDown = bShutDown; }
	BOOL					IsShutDown() { return m_bShutDown; }

	void					writeLog(LOG_LEVEL eLevel, const char* log);

	// Http����������ؽ���ص�
	void					HttpOpenQueryResult( OPEN_QUERY_INDEX dwIndex, HttpQueryResult* pData );

	// Http�������
	BOOL					HttpOpenQuery(HttpQueryResult * pData);

	DWORD					GetServerID(DWORD dwZoneID);

	const OpenServer_Config* GetServerConfig(DWORD dwKey);

	char*					GetWebServerName() { return m_WebServerName; };

	const char*				GetNodeJsServerId() { return m_szNodeJsServerIP; };
	DWORD					GetNodeJsServerPort() { return m_dwNodeJsServerPort; };

	const char*				GetSmsMethod() { return m_szSmsMethod; };

	const char*				GetStatisticsServerId() { return m_szStatisticsServerIP; };
	DWORD					GetStatisticsServerPort() { return m_dwStatisticsServerPort; };
	bool					IsOpenStatistics() { return m_bIsOpenStatistics; };
	DWORD					GetStatisticsAppKey() { return m_dwStatisticsAppKey; };
	
	


	const char*	GetQuickSDKMd5Key() { return m_szQuickSDKMd5Key; };
	const char*	GetQuickSDKCallbackKey() { return m_szQuickSDKCallbackKey; };
	const char*	GetQuickSDKProductCode() { return m_szQuickSDKProductCode; };

	const char*	GetQuickSDKMd5KeyIOS() { return m_szQuickSDKMd5KeyIOS; };
	const char*	GetQuickSDKCallbackKeyIOS() { return m_szQuickSDKCallbackKeyIOS; };
	const char*	GetQuickSDKProductCodeIOS() { return m_szQuickSDKProductCodeIOS; };

	NetworkObject			*m_pTempNTObject;
private:
	void					Register();
	void					AddHandler(DWORD dwIndex, fnProcessQueryResult fnHandler);
	fnProcessQueryResult	Find(DWORD dwIndex);

private:
	IOCPServer*				m_pIOCPServer;
	char					m_szServerGateIP[32];
	DWORD					m_dwServerGatePort;
	char					m_szPHPIP[32];
	DWORD					m_dwPHPPort;
	DWORD					m_dwHTTPPort;
	BOOL					m_bShutDown;
	LogFile*				m_pLogfile;
	char					m_AppID[32];
	char					m_AppKey[64];
	char					m_WebServerName[64];
	HttpRequestManager		m_HttpRequestManager;	// Http���������
	ProcessQueryResultMap	m_ProcessQueryMap;
	DBCFile					m_cZoneDBCFile;
	DBCFile					m_ServerConfig;
	//AllAcc                  m_AuthMap;
	DWORD                   m_dwAuthTime;
	char					m_szServerName[64];

	// Nodejs��������IP���˿�
	char					m_szNodeJsServerIP[32];
	DWORD					m_dwNodeJsServerPort;
	char					m_szSmsMethod[32];

	// quick_sdk���
	char m_szQuickSDKMd5Key[64];
	char m_szQuickSDKCallbackKey[64];
	char m_szQuickSDKProductCode[64];

	char m_szQuickSDKMd5KeyIOS[64];
	char m_szQuickSDKCallbackKeyIOS[64];
	char m_szQuickSDKProductCodeIOS[64];

	// ����ͳ�����
	char					m_szStatisticsServerIP[32];
	DWORD					m_dwStatisticsServerPort;
	bool					m_bIsOpenStatistics;
	DWORD					m_dwStatisticsAppKey;

	// �������
	bool					m_bIsReportReyunFromNodejs;
};

extern OpenServer g_OpenServer; 


class OpenServerThread : public ThreadBase
{
public:
	OpenServerThread(){  }
	virtual ~OpenServerThread() {  }
public:
	virtual void			EndThread();
	virtual DWORD			run();
	BOOL					IsShutDown();

};

extern OpenServerThread		g_OpenServerThread;

class HttpResultHandler
{
public:
	HttpResultHandler(void);
	~HttpResultHandler(void);

public:
	static void  ProcessMobileValidateResult(HttpQueryResult* pData);
	static void  ProcessUserDataRegResult(HttpQueryResult* pData);
	static void  ProcessUserDataQryResult(HttpQueryResult* pData);
	static void  ProcessLgnDataSyncResult(HttpQueryResult* pData);
	static void  ProcessPointCfgLoadResult(HttpQueryResult* pData);
	static void  ProcessReyunStatisticResult(HttpQueryResult* pData);
	static void  ProcessCheckUserInfoResult(HttpQueryResult* pData);
	static void  RechargeSuccessReportResult(HttpQueryResult* pData);
	static void  StatisticsReportResult(HttpQueryResult* pData);

	static void ProcessWebCommonResult(HttpQueryResult* pData);

protected:

	// ����û���¼�Ľ��(ͨ��)
	static void CheckUserLoginResult_Common(HttpCheckUser* pQuery);

	// ����û���¼�Ľ��(�����̨)
	static void CheckUserLoginResult_XMGT(HttpCheckUser* pQuery);
};
