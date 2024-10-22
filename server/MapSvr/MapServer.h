#pragma once

#include "../Common/Const.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/LogFile.h"
#include <hash_map>
#include <map>
#include "PlayerManager.h"
#include "KeyWodFilter/KeyWodFilter.h"
#include <deque>
//#include "dgame_define.pb.h"

class IOCPServer;
class GateServerSession;
class OpenServerSession;
class NetworkObject;
class QueryForUser;
class QueryResult;
class NetMsgBody;
struct LoginAccountInfo;
//class google::protobuf::Message;

//DB查询结果处理函数指针
typedef void (*fnProcessQueryResult)( QueryResult * pData );
//typedef std::hash_map<DWORD, fnProcessQueryResult> QueryResultMap;
typedef std::map<DWORD, fnProcessQueryResult> QueryResultMap;

//游戏配置
struct GameConfig
{
	//网络相关
	DWORD			m_dwMaxAcceptSession;
	DWORD			m_dwMaxConnectSession;
	DWORD			m_dwSendBufferSize;
	DWORD			m_dwRecvBufferSize;
	DWORD			m_dwMaxPacketSize;
	DWORD			m_dwAcceptThread;
	DWORD			m_dwConnectThread;
	DWORD			m_dwTimeout;
	BOOL			m_bSecurity;

	//内存相关
	DWORD			m_dwStaticSceneNum;
	DWORD			m_dwInstanceNum;
	DWORD			m_dwPlayerPoolSize;
	DWORD			m_dwNPCPoolSize;
	DWORD			m_dwItemPoolSize;
	DWORD			m_dwFunctionNPCPoolSize;
	DWORD			m_dwPetTrainerPoolSize;
	DWORD			m_dwPetBabyPoolSize;
	DWORD			m_dwCompanionPoolSize;
	DWORD			m_dwMissionPoolSize;
	DWORD			m_dwAuraPoolSize;
	DWORD			m_dwBattlePoolSize;
	DWORD			m_dwTeamPoolSize;
	DWORD			m_dwMoveSpeed;
	DWORD			m_dwRunSpeed;
	DWORD			m_dwPlayerMaxLv;
};

enum BeatStatus
{
	BeatStatus_None,
	BeatStatus_Send,
	BeatStatus_Wait,
	BeatStatus_NewConnect,
};

struct ServerBeat
{
	DWORD m_dwTime;
	BeatStatus eStatus;
	DWORD m_dwConnectNum;

};

// 唯一管理
class UIDMananger
{
public:
	UIDMananger() { m_dwEquipmentUId = 0; };
	~UIDMananger() {};

	DWORD GetEquipmentUID() { return m_dwEquipmentUId++; };
protected:

private:
	DWORD m_dwEquipmentUId;		// 装备唯一ID
};

//地图服务器
class MapServer
{
public:
	MapServer(void);
	~MapServer(void);
public:
	BOOL					Init();
	void					Release();
	void					Run();
	BOOL					Update(DWORD dwDeltaTick);
	void					SaveAllDataToDB();
	void					ReloadInitFile();
	void					ReloadConfig();

	void					UpdateOpenBeat(DWORD dwDeltaTick );
	void					UpdateGateBeat(DWORD dwDeltaTick );
	void                    InitGateBeat();
	void                    InitOpenBeat();
	void					UpdateWaitingLoginAccount();

	SERVER_TYPE				GetServerType() { return MAP_SERVER; }
	// 请求数据库操作
	BOOL					MapDBQuery(QueryForUser * pData);
	// 数据库操作结果调用
	VOID					DBResult( MAP_QUERY_INDEX dwIndex, QueryResult* pData );

	void					SetShutDown(BOOL bShutDown = TRUE) { m_bShutDown = bShutDown; }
	BOOL					IsShutDown() { return m_bShutDown; }
	BOOL					IsConnectToOpenSvr() { return m_bConOpenSvrFlg; }
	BOOL					LoadLuaFiles(bool bFirstLoad = false);

	void					ConnectToGateServer();
	void					ConnectToOpenServer();
	void                    SendOpen2Beat();
	void                    SendGate2Beat();
	void					ConnectToServer( NetworkObject * pNetworkObject, char * pszIP, WORD wPort );

	void					WriteLog(LOG_LEVEL eLevel, const char* log);
	static void				WriteLog(const char* log,...);
	int						GetPlayerNum() const;
	GateServerSession*		GetGateSession() { return m_pGateServerSession; }
	OpenServerSession*		GetOpenSession() { return m_pOpenServerSession; }
	void					SendMsgToGate(DWORD dwGUID, WORD wMsgID, NetMsgBody * pMsg);
	//void					SendMsgToGate(DWORD dwGUID, google::protobuf::Message* pMsg, DWORD dwCode);
	bool					SendMsgToOpen(const char*pszAccountId, DWORD dwGUID, DWORD dwMsgId, const unsigned char *pszMsg, int nMsgSize);

	void					SendErrorMsgToUser(DWORD dwGUID, WORD wErrorCode, WORD wFromMsgId = 0);

	// 接收客户端消息回调
	void					RecvClientMsgCallback(DWORD dwGuid, WORD wMsgID);
	// 添加不会改变角色数据的消息
	void					AddNotChangeRoleInfoMsg(WORD wMsgID);
	// 注册不会改变角色数据的消息
	void					ResisterNotChangeRoleInfoMsg();

	DWORD					GetServerID() { return m_dwServerID; }
	void					ChangeOpenSvrTime(DWORD dwOpenSvrDate);
	DWORD					GetOpenSvrTime() { return m_dwOpenSvrTime; }
	BOOL					IsMerged() { return m_bIsMerged; }
	VOID					AddOpenSvrTime(int nDay) {	m_dwOpenSvrTime = m_dwOpenSvrTime+nDay*SECOND_OF_DAY ; }
	VOID                    SetMaxPlayerNum(DWORD dwNum){ m_dwMaxPlayerNum = dwNum;}
	DWORD                   GetMaxPlayerNum(){return m_dwMaxPlayerNum;}
	BOOL                    IsRobotTest(){return m_bTestRobot;}
	VOID                    SetRobotTest(){ m_bTestRobot = TRUE;}
	VOID                    WriteLogFlag(BOOL IsWrite){ m_bIsWriteLog = IsWrite; }
	BOOL                    CanWriteLog(){return m_bIsWriteLog; }
	BOOL					IsOpenGM() { return m_bIsOpenGM; };
	VOID					SetIsOpenGM(bool bOpenGM) { m_bIsOpenGM = bOpenGM; };
	BOOL					IsCheckMsgSeriesId() { return m_bIsCheckMsgSeriesId; };
	BOOL					IsReportRecharge() { return m_bIsReportRecharge; };
	BOOL					IsCheckAccountWhite() { return m_bIsCheckAccountWhite; };
	BOOL					IsCheckPower() { return m_bIsCheckPower; };
	DWORD					GetClientVersion() { return m_dwClientVersion; };
	DWORD					GetServerBelongVersion() { return m_dwServerBelongVersion; };
	BOOL					IsExamineServer() { return m_bIsExamineServer; };
	BOOL					IsUseOldPower() { return m_bIsUseOldPower; };
	BOOL					IsLCYExamineSvr() { return m_bIsLCYExamineSvr; };
	DWORD					GetCurServerId(DWORD dwInitServerId, DWORD dwRoleId = 0);
	BOOL					IsLoadRoleBaseData() { return m_bIsLoadRoleBaseData; };
	VOID					SetIsLoadRoleBaseData(bool bIsLoad) { m_bIsLoadRoleBaseData = bIsLoad; };
	BOOL					IsYGame() { return m_bIsYGame; };
	BOOL					IsUse3rdStatisticsData() { return m_bIsUse3rdStatisticsData; };
	DWORD					GetCurMaxOnlineRoleNum() { return m_dwCurMaxOnlineRoleNum; };
	
	VOID					AddWaitingLoginAccount(const LoginAccountInfo *pInfo);
	VOID					RemoveWaitingLoginAccount(const char *pszAccountName, DWORD dwChannelType);
	const LoginAccountInfo* FindWaitingLoginAccount(const char *pszAccountName, DWORD dwChannelType);
	BOOL					OnAccountLogin(const char *pszAccount, const char *pszPassword, DWORD dwChannel, DWORD dwGuid, BYTE byTerminalType);

	BOOL					MatchRoleName(const char* szName);
	BOOL					MatchSqlCmd(const char* szCmd);

	// 添加需要更新的玩家
	void					AddUpdatePlayer(DWORD dwRoleId, DWORD dwServerId, bool isSave = true);

	// 存储更新的玩家数据
	void					SaveUpdatePlayerData();

	// 存储在线玩家数量
	void					SaveOnlinePlayerNum();

	// 取最近一次重置每日签到的月份(签到系统逻辑月份)
	BYTE					GetSignLastResetMonth();

	// 取已经开服的天数
	DWORD					GetAlreadyOpenServerDay(DWORD dwServerId);

	// 取服务器列表
	const map<DWORD, ServerInfo>& GetServerList() { return m_ServerList; };

	// 取服务器信息
	const ServerInfo* GetServerInfo(DWORD dwServerId);
	void LoadServerList(const ServerInfo *pServerList, int nServerNum);

	// 加载服务器列表请求
	void		LoadServerListRequest();

	// 加载服务器全局数据请求
	void		LoadGolbalDataRequest();

	// 加载服务器全局数据回调
	void		LoadGolbalDataCallback(WORD wServerId, int nKey, DWORD dwValue);

	// 存储服务器全局数据
	void		SaveGolbalData(WORD wServerId, int nKey, DWORD dwValue);

	// 更新服务器全局数据
	void		UpdateGolbalData(WORD wServerId, int nKey, DWORD dwValue);

	// 取全局数据
	ServerGlobalData& GetServerGlobalData(WORD wServerId);

	// 添加激活码领取记录
	void		AddActivationCodeRecord(const char *pszCode, DWORD dwRoleId);

	// 是否激活码领取记录
	bool		IsActivationCodeRecord(const char *pszCode);

	// 添加手机认证数据
	void		AddMobileAuthData(const MobileAuthData &tAuthData);

	// 删除手机号认证数据
	void		RemoveMobileAuthData(const char *pszMobile);

	// 取手机认证数据
	const MobileAuthData* GetMobileAuthData(const char *pszMobile);

	// 请求关卡评论列表
	vector<MapComment>* GetMapCommentList(DWORD dwServerId, DWORD dwChapter);

	// 添加关卡数据
	void AddComment(DWORD dwServerId, DWORD dwChapter);

	// 存储关卡评论数据
	void SaveMapCommentData(DWORD dwServerId, WORD dwChapter);

	// 加载关卡评论数据
	void LoadMapCommentData(DWORD dwFindId, const MapComments *pComments);

	// 关卡评论数据请求
	void MapCommentDataRequest();

	// 取关卡评论数量
	DWORD GetMapCommentNum() { return m_MapCommentList.size(); };

	// 数据统计上报
	//void OnStatisticsReport(WORD wType, const char *pszContext);

	// 数据统计上报
	void OnStatisticsReport(DWORD dwRoleId, DWORD dwObjectId, DWORD dwOjbectNum, bool bIsAdd, WORD wFromId = 0);

	// 检测战力是否合法
	BOOL CheckPowerIsValid(DWORD dwServerPower, DWORD dwClientPower);

	// 添加准备使用的角色名称
	void AddReadyToUseRoleName(const char *pszRoleName);

	// 是否准备使用的角色名称
	bool IsReadyToUseRoleName(const char *pszRoleName);

	// 添加正在创建角色数据
	void AddCreatingRoleData(const char *pszAccount, DWORD dwChannelId,  DWORD dwServerId);

	// 删除正在准备角色数据
	void RemoveCreatingRoleData(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId);

	// 是否正在创建角色 
	bool IsInCreatingRole(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId);

	// 添加扭蛋抽奖记录
	void AddNDLotteryRecord(DWORD dwServerId, NDLotteryRecord &tRecord);

	// 取扭蛋抽奖记录列表
	const deque<NDLotteryRecord>* GetNDLotteryRecord(DWORD dwServerId);
	
	// 处理合服逻辑
	void OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId);

	// 命令相关
public:
	VOID				ShowPlayerList();
	VOID				StopServer();

protected:
	// 新的一天
	VOID					OnNewDay();

	// 新的一周
	VOID					OnNewWeek();

	// 新的一月
	VOID					OnNewMonth();

private:
	void					Register();
	void					AddHandler(DWORD dwIndex, fnProcessQueryResult fnHandler);
	fnProcessQueryResult	Find(DWORD dwIndex);

private:
	char					m_szDBIP[IP_LEN];			//数据库IP地址
	char					m_szDBName[BUFF_LEN];		//数据库名
	int						m_nDBPort;					//数据库端口
	char					m_szUserName[BUFF_LEN];		//数据库用户名
	char					m_szPassword[BUFF_LEN];		//数据库密码
	IOCPServer*				m_pIOCPServer;				//网络引擎对象
	GateServerSession*		m_pGateServerSession;		//GateSvr会话对象
	char					m_szServerGateIP[32];		//GateSvrIP
	WORD					m_dwServerGatePort;			//GateSvr端口

	OpenServerSession*		m_pOpenServerSession;		//OpenSvr会话对象
	char					m_szServerOpenIP[32];		//OpenSvrIP
	WORD					m_dwServerOpenPort;			//OpenSvr端口
	BOOL					m_bConOpenSvrFlg;			//是否连接OpenSvr标志
	DWORD					m_dwCurDBThreadIndex;
	
	BOOL					m_bShutDown;				//切断标志
	LogFile                 *m_pLogfile;				//日志对象
	DWORD					m_dwServerID;				//服务器ID
	DWORD					m_dwOpenSvrTime;			//开服时间
	BOOL					m_bIsMerged;				//是否合服过
	QueryResultMap			m_ProcessQueryMap;			//DB查询结果列表
	GameConfig				m_tGameConfig;				//游戏配置信息
	ServerBeat              m_OpenBeat;
	ServerBeat              m_GateBeat;
	DWORD                   m_dwMaxPlayerNum;
	BOOL                    m_bTestRobot;
	BOOL                    m_bIsWriteLog;
	BYTE					m_byCurLogColor;
	BOOL					m_bIsOpenGM;				// 是否开启GM

	UINT64					m_lProcessNewDayTimer;
	int						m_nLastYearDay;

	bool					m_bDoRankActivityReward;
	int						m_nDoRewardTime;

	CKeyWordFilter			m_Filter;

	// 等待登录帐号
	std::map<std::string, LoginAccountInfo> m_WaitingLoginAccountList;
	std::map<std::string, DWORD> m_AccumulatePointsRule;

	// 准备使用的角色名称
	std::map<std::string, DWORD> m_ReadyToUseRoleNameList;

	// 正在创建中的角色
	std::map<std::string, DWORD> m_CreatingRoleList;

	UINT64					m_lProcessSaveTimer;

	map<DWORD, ServerInfo>		m_ServerList;	// 服务器列表(已开服的)

	map<DWORD, DWORD>		m_UpdatePlayerList;	// 更新玩家列表 roleid, serverid
	UINT64					m_lSaveUpdatePlayerTime;

	UINT64					m_lRecordOfflinePlayerNumTime;

	map<DWORD, DWORD>		m_NotChangeRoleInfoMsgList;	// 不改变角色信息的消息列表 ;

	ServerGlobalData		m_GlobalData; // 全局数据
	map<DWORD, ServerGlobalData> m_GlobalDataList; // 服务器全局数据列表

	char m_szServerName[64];	// 服务器名称

	map<string, MobileAuthData> m_MobileAuteDataList;	// 手机号认证数据列表

	map<string, DWORD> m_ActivationCodeTakeRecord;	// 激活码领取记录[code,role_id]

	map<DWORD, vector<MapComment>> m_MapCommentList;  // 关卡评论列表[id=[server_id + map_id], comment_list]	

	map<DWORD, deque<NDLotteryRecord>> m_LotteryRecordList; // 扭蛋抽奖记录[server_id, record_list]

	map<string, ClientInfo> m_ClientInfoList;		// 客户端信息列表			

	UINT64 m_lProcessLogicTimer;

	DWORD m_dwTestSendMsgToNodejsTimes; //todo:: tor test

	DWORD m_dwCurMaxOnlineRoleNum;	// 当前最高在线人数

	bool m_bIsLoadRoleBaseData;	// 是否已加载角色基本数据

	bool m_bLoadServerList;			// 是否已加载了服务器列表
	bool m_bIsCheckMsgSeriesId;		// 是否检测消息系列Id
	bool m_bIsReportRecharge;		// 是否上报充值数据
	bool m_bIsCheckAccountWhite;	// 是否检测帐号白名单
	bool m_bIsCheckPower;			// 是否检测战力
	bool m_bIsExamineServer;		// 是否审核服
	bool m_bIsUseOldPower;			// 是否使用旧战力
	bool m_bIsLCYExamineSvr;		// 是否乐次元提审服
	bool m_bIsYGame;				// 是否越南版本
	bool m_bIsShowSpecialLog;		// 是否显示特殊类日志
	bool m_bIsUse3rdStatisticsData;	// 是否通过第三方服务统计数据
	DWORD m_dwClientVersion;		// 客户端版本号	
	DWORD m_dwServerBelongVersion;	// 服务器所属版本		
};

//声明全局地图服务器对象
extern MapServer g_MapServer;

//地图服务器线程
class MapServerThread : public ThreadBase
{
public:
	MapServerThread(){  }
	virtual ~MapServerThread() {  }
public:
	virtual void			EndThread();
	virtual DWORD			run();
	BOOL					IsShutDown();
};

//声明全局地图服务器线程对象
extern MapServerThread g_MapServerThread;
