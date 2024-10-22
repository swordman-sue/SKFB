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

//DB��ѯ���������ָ��
typedef void (*fnProcessQueryResult)( QueryResult * pData );
//typedef std::hash_map<DWORD, fnProcessQueryResult> QueryResultMap;
typedef std::map<DWORD, fnProcessQueryResult> QueryResultMap;

//��Ϸ����
struct GameConfig
{
	//�������
	DWORD			m_dwMaxAcceptSession;
	DWORD			m_dwMaxConnectSession;
	DWORD			m_dwSendBufferSize;
	DWORD			m_dwRecvBufferSize;
	DWORD			m_dwMaxPacketSize;
	DWORD			m_dwAcceptThread;
	DWORD			m_dwConnectThread;
	DWORD			m_dwTimeout;
	BOOL			m_bSecurity;

	//�ڴ����
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

// Ψһ����
class UIDMananger
{
public:
	UIDMananger() { m_dwEquipmentUId = 0; };
	~UIDMananger() {};

	DWORD GetEquipmentUID() { return m_dwEquipmentUId++; };
protected:

private:
	DWORD m_dwEquipmentUId;		// װ��ΨһID
};

//��ͼ������
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
	// �������ݿ����
	BOOL					MapDBQuery(QueryForUser * pData);
	// ���ݿ�����������
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

	// ���տͻ�����Ϣ�ص�
	void					RecvClientMsgCallback(DWORD dwGuid, WORD wMsgID);
	// ��Ӳ���ı��ɫ���ݵ���Ϣ
	void					AddNotChangeRoleInfoMsg(WORD wMsgID);
	// ע�᲻��ı��ɫ���ݵ���Ϣ
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

	// �����Ҫ���µ����
	void					AddUpdatePlayer(DWORD dwRoleId, DWORD dwServerId, bool isSave = true);

	// �洢���µ��������
	void					SaveUpdatePlayerData();

	// �洢�����������
	void					SaveOnlinePlayerNum();

	// ȡ���һ������ÿ��ǩ�����·�(ǩ��ϵͳ�߼��·�)
	BYTE					GetSignLastResetMonth();

	// ȡ�Ѿ�����������
	DWORD					GetAlreadyOpenServerDay(DWORD dwServerId);

	// ȡ�������б�
	const map<DWORD, ServerInfo>& GetServerList() { return m_ServerList; };

	// ȡ��������Ϣ
	const ServerInfo* GetServerInfo(DWORD dwServerId);
	void LoadServerList(const ServerInfo *pServerList, int nServerNum);

	// ���ط������б�����
	void		LoadServerListRequest();

	// ���ط�����ȫ����������
	void		LoadGolbalDataRequest();

	// ���ط�����ȫ�����ݻص�
	void		LoadGolbalDataCallback(WORD wServerId, int nKey, DWORD dwValue);

	// �洢������ȫ������
	void		SaveGolbalData(WORD wServerId, int nKey, DWORD dwValue);

	// ���·�����ȫ������
	void		UpdateGolbalData(WORD wServerId, int nKey, DWORD dwValue);

	// ȡȫ������
	ServerGlobalData& GetServerGlobalData(WORD wServerId);

	// ��Ӽ�������ȡ��¼
	void		AddActivationCodeRecord(const char *pszCode, DWORD dwRoleId);

	// �Ƿ񼤻�����ȡ��¼
	bool		IsActivationCodeRecord(const char *pszCode);

	// ����ֻ���֤����
	void		AddMobileAuthData(const MobileAuthData &tAuthData);

	// ɾ���ֻ�����֤����
	void		RemoveMobileAuthData(const char *pszMobile);

	// ȡ�ֻ���֤����
	const MobileAuthData* GetMobileAuthData(const char *pszMobile);

	// ����ؿ������б�
	vector<MapComment>* GetMapCommentList(DWORD dwServerId, DWORD dwChapter);

	// ��ӹؿ�����
	void AddComment(DWORD dwServerId, DWORD dwChapter);

	// �洢�ؿ���������
	void SaveMapCommentData(DWORD dwServerId, WORD dwChapter);

	// ���عؿ���������
	void LoadMapCommentData(DWORD dwFindId, const MapComments *pComments);

	// �ؿ�������������
	void MapCommentDataRequest();

	// ȡ�ؿ���������
	DWORD GetMapCommentNum() { return m_MapCommentList.size(); };

	// ����ͳ���ϱ�
	//void OnStatisticsReport(WORD wType, const char *pszContext);

	// ����ͳ���ϱ�
	void OnStatisticsReport(DWORD dwRoleId, DWORD dwObjectId, DWORD dwOjbectNum, bool bIsAdd, WORD wFromId = 0);

	// ���ս���Ƿ�Ϸ�
	BOOL CheckPowerIsValid(DWORD dwServerPower, DWORD dwClientPower);

	// ���׼��ʹ�õĽ�ɫ����
	void AddReadyToUseRoleName(const char *pszRoleName);

	// �Ƿ�׼��ʹ�õĽ�ɫ����
	bool IsReadyToUseRoleName(const char *pszRoleName);

	// ������ڴ�����ɫ����
	void AddCreatingRoleData(const char *pszAccount, DWORD dwChannelId,  DWORD dwServerId);

	// ɾ������׼����ɫ����
	void RemoveCreatingRoleData(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId);

	// �Ƿ����ڴ�����ɫ 
	bool IsInCreatingRole(const char *pszAccount, DWORD dwChannelId, DWORD dwServerId);

	// ���Ť���齱��¼
	void AddNDLotteryRecord(DWORD dwServerId, NDLotteryRecord &tRecord);

	// ȡŤ���齱��¼�б�
	const deque<NDLotteryRecord>* GetNDLotteryRecord(DWORD dwServerId);
	
	// ����Ϸ��߼�
	void OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId);

	// �������
public:
	VOID				ShowPlayerList();
	VOID				StopServer();

protected:
	// �µ�һ��
	VOID					OnNewDay();

	// �µ�һ��
	VOID					OnNewWeek();

	// �µ�һ��
	VOID					OnNewMonth();

private:
	void					Register();
	void					AddHandler(DWORD dwIndex, fnProcessQueryResult fnHandler);
	fnProcessQueryResult	Find(DWORD dwIndex);

private:
	char					m_szDBIP[IP_LEN];			//���ݿ�IP��ַ
	char					m_szDBName[BUFF_LEN];		//���ݿ���
	int						m_nDBPort;					//���ݿ�˿�
	char					m_szUserName[BUFF_LEN];		//���ݿ��û���
	char					m_szPassword[BUFF_LEN];		//���ݿ�����
	IOCPServer*				m_pIOCPServer;				//�����������
	GateServerSession*		m_pGateServerSession;		//GateSvr�Ự����
	char					m_szServerGateIP[32];		//GateSvrIP
	WORD					m_dwServerGatePort;			//GateSvr�˿�

	OpenServerSession*		m_pOpenServerSession;		//OpenSvr�Ự����
	char					m_szServerOpenIP[32];		//OpenSvrIP
	WORD					m_dwServerOpenPort;			//OpenSvr�˿�
	BOOL					m_bConOpenSvrFlg;			//�Ƿ�����OpenSvr��־
	DWORD					m_dwCurDBThreadIndex;
	
	BOOL					m_bShutDown;				//�жϱ�־
	LogFile                 *m_pLogfile;				//��־����
	DWORD					m_dwServerID;				//������ID
	DWORD					m_dwOpenSvrTime;			//����ʱ��
	BOOL					m_bIsMerged;				//�Ƿ�Ϸ���
	QueryResultMap			m_ProcessQueryMap;			//DB��ѯ����б�
	GameConfig				m_tGameConfig;				//��Ϸ������Ϣ
	ServerBeat              m_OpenBeat;
	ServerBeat              m_GateBeat;
	DWORD                   m_dwMaxPlayerNum;
	BOOL                    m_bTestRobot;
	BOOL                    m_bIsWriteLog;
	BYTE					m_byCurLogColor;
	BOOL					m_bIsOpenGM;				// �Ƿ���GM

	UINT64					m_lProcessNewDayTimer;
	int						m_nLastYearDay;

	bool					m_bDoRankActivityReward;
	int						m_nDoRewardTime;

	CKeyWordFilter			m_Filter;

	// �ȴ���¼�ʺ�
	std::map<std::string, LoginAccountInfo> m_WaitingLoginAccountList;
	std::map<std::string, DWORD> m_AccumulatePointsRule;

	// ׼��ʹ�õĽ�ɫ����
	std::map<std::string, DWORD> m_ReadyToUseRoleNameList;

	// ���ڴ����еĽ�ɫ
	std::map<std::string, DWORD> m_CreatingRoleList;

	UINT64					m_lProcessSaveTimer;

	map<DWORD, ServerInfo>		m_ServerList;	// �������б�(�ѿ�����)

	map<DWORD, DWORD>		m_UpdatePlayerList;	// ��������б� roleid, serverid
	UINT64					m_lSaveUpdatePlayerTime;

	UINT64					m_lRecordOfflinePlayerNumTime;

	map<DWORD, DWORD>		m_NotChangeRoleInfoMsgList;	// ���ı��ɫ��Ϣ����Ϣ�б� ;

	ServerGlobalData		m_GlobalData; // ȫ������
	map<DWORD, ServerGlobalData> m_GlobalDataList; // ������ȫ�������б�

	char m_szServerName[64];	// ����������

	map<string, MobileAuthData> m_MobileAuteDataList;	// �ֻ�����֤�����б�

	map<string, DWORD> m_ActivationCodeTakeRecord;	// ��������ȡ��¼[code,role_id]

	map<DWORD, vector<MapComment>> m_MapCommentList;  // �ؿ������б�[id=[server_id + map_id], comment_list]	

	map<DWORD, deque<NDLotteryRecord>> m_LotteryRecordList; // Ť���齱��¼[server_id, record_list]

	map<string, ClientInfo> m_ClientInfoList;		// �ͻ�����Ϣ�б�			

	UINT64 m_lProcessLogicTimer;

	DWORD m_dwTestSendMsgToNodejsTimes; //todo:: tor test

	DWORD m_dwCurMaxOnlineRoleNum;	// ��ǰ�����������

	bool m_bIsLoadRoleBaseData;	// �Ƿ��Ѽ��ؽ�ɫ��������

	bool m_bLoadServerList;			// �Ƿ��Ѽ����˷������б�
	bool m_bIsCheckMsgSeriesId;		// �Ƿ�����Ϣϵ��Id
	bool m_bIsReportRecharge;		// �Ƿ��ϱ���ֵ����
	bool m_bIsCheckAccountWhite;	// �Ƿ����ʺŰ�����
	bool m_bIsCheckPower;			// �Ƿ���ս��
	bool m_bIsExamineServer;		// �Ƿ���˷�
	bool m_bIsUseOldPower;			// �Ƿ�ʹ�þ�ս��
	bool m_bIsLCYExamineSvr;		// �Ƿ��ִ�Ԫ�����
	bool m_bIsYGame;				// �Ƿ�Խ�ϰ汾
	bool m_bIsShowSpecialLog;		// �Ƿ���ʾ��������־
	bool m_bIsUse3rdStatisticsData;	// �Ƿ�ͨ������������ͳ������
	DWORD m_dwClientVersion;		// �ͻ��˰汾��	
	DWORD m_dwServerBelongVersion;	// �����������汾		
};

//����ȫ�ֵ�ͼ����������
extern MapServer g_MapServer;

//��ͼ�������߳�
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

//����ȫ�ֵ�ͼ�������̶߳���
extern MapServerThread g_MapServerThread;
