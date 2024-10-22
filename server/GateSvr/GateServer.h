#pragma once

#include "../Common/Const.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/LogFile.h"

class IOCPServer;
class NetMsgBody;
class MapServerSession;

//网关服务器
class GateServer
{
	//连接端类型
	enum 
	{ 
		SERVER_IOHANDLER = 0,		//服务器侧
		CLIENT_IOHANDLER = 1		//客户端侧
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
	IOCPServer*				m_pIOCPServer;					//	网络引擎对象
	MapServerSession*		m_pMapSession;					//	地图服务器会话对象 
	char					m_szClientGateIP[32];			//	客户端网关IP
	DWORD					m_dwClientGatePort;				//	客户端网关端口
	char					m_szServerGateIP[32];			//	服务器网关IP
	DWORD					m_dwServerGatePort;				//	服务器网关端口
	BOOL					m_bShutDown;
	LogFile					*m_pLogfile;					//	日志文件对象
	BOOL					m_bEnableSecurity;
	DWORD					m_dwCheckTimeGap;				//	检查客户端超时时间间隔
	DWORD					m_dwSocketTimeOut;				//	socket超时
	DWORD					m_dwConnectTimeOut;				//	socket连接超时
	bool					m_bIsShowSpecialLog;			// 是否显示特殊类日志

	char					m_szServerName[64];	// 服务器名称
};

//声明全局网关服务器对象
extern GateServer g_GateServer; 

//网关服务器线程
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

//声明全局网关服务器线程对象
extern GateServerThread	g_GateServerThread;