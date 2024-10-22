#pragma once

#include "RobotSession.h"
#include "../Common/MsgDefine.h"

#define MAX_IDLE_TIME					10		//最大空闲时间
#define HEART_BEAT_INTERVAL				10000	//心跳包间隔(3秒)
#define CHANGE_AI_INTERVAL				2000	//切换AI间隔时间
#define ENTER_BATTALE_INTERVAL_TIME	    60	    //申请战斗的间隔时间
 


struct RobotEntry;
class  GameObject;
class  Chief;

//机器人状态
enum RobotStat
{
	RS_INIT				= 0,
	RS_LOAD				= 1,
	RS_LOAD_FINISH		= 2,
};
enum RobotBattle
{
	RobotBattle_None		=0,
	RobotBattle_LADDER		=1,
	RobotBattle_MATCHING	=2,
};

//机器人AI状态
enum RobotAIStat
{
	RAS_NONE			= 0,
	RAS_IDLE			= 1,
	RAS_BATTLE			= 2,
	RAS_CHAT			= 3,
	RAS_BATTLE_SYN		= 4,
	RAS_BATTLE_ACK		= 5,
	RAS_MAX,

};

//机器人战斗状态
enum RobotBattleStat
{
	RBS_NONE			= 0,
	RBS_MATING			= 1,
	RBS_BATTLING		= 2,
	RBS_MAX,
};

//机器人AI状态几率
enum RobotAIOdds
{
	RAO_IDLE			= 150000,
	RAO_BATTLE			= 995000,
	RAO_CHAT			= 1000000,
};

//机器人基本信息
struct RobotBaseInfo
{
	DWORD				m_dwCharID;							//机器人角色ID
	char				m_szCharName[MAX_ROLE_LEN];		//机器人角色名
	BYTE				m_byRace;							//机器人角色种族
	BYTE				m_byVocation;						//机器人角色职业
	BYTE				m_byGender;							//机器人角色性别
};

typedef std::map<DWORD, GameObject*>	GameObjectMap;
typedef GameObjectMap::iterator			GameObjectMapIter;

//机器人对象
class Robot
{
public:
	Robot();
	~Robot();

	BOOL				Init(RobotEntry* pRobotEntry);
	VOID				Release();
	VOID				Update(DWORD dwTick);
	RobotSession*		GetRobotSession() { return &m_cRobotSession; }
	VOID				Test();

	//事件相关
	VOID				OnConnect(BOOL bSuccess, DWORD dwNetworkIndex);
	VOID				OnDisconnect();
	VOID				OnRobotLoginAck(MSG_M2C_ROBOT_LOGIN_ACK* pMsg);

	VOID				OnMSG_S2C_LOGIN_ACCOUNT_RESP(NetMsgBody *pMsg);
	VOID				OnMSG_S2C_LOGIN_SELECT_SERVER_RESP(NetMsgBody *pMsg);
	VOID				OnMSG_S2C_LOGIN_CREATE_ROLE_RESP(NetMsgBody *pMsg);
	VOID				OnMSG_S2C_GAME_ERROR_NOTIFY(NetMsgBody *pMsg);
	VOID				OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY(NetMsgBody *pMsg);
	VOID				OnMSG_S2C_ROLE_INFO_NOTIFY(NetMsgBody *pMsg);
	VOID				OnMSG_S2C_ATTACK_CITY_ENTER_RESP(NetMsgBody *pMsg);

	VOID				OnMSG_S2C_BATTLE_START_RESP(NetMsgBody *pMsg);

	VOID				AddToGameObjectMap(GameObject* pGameObject);
	VOID				RemoveFromGameObjectMap(GameObject* pGameObject);

	//处理相关
private:
	VOID				ProcessRobotLoginSyn();
	VOID				ProcessChooseCharCmd();

	VOID				ProcessBattleSurrenderOptCmd();
	VOID				ProcessBattlePrepareAffirmCmd();

	//AI相关
private:
	VOID				ChangeAI();
	VOID				RandomAIStat();
	VOID				OnEnterAIStat();
	VOID				OnEnterIdleStat();
	VOID				OnEnterBattleStat();

	VOID				UpdateAIStat(DWORD dwTick);
	VOID				ProcessNoneStat(DWORD dwTick);
	VOID				ProcessIdleStat(DWORD dwTick);
	VOID				ProcessChatStat();
	VOID				ProcessBattleStat(DWORD dwTick);

	VOID				UnitTest();
	//功能函数
public:
	BOOL				SendPacket(NetMsgBody* pMsg);
	BOOL				SendStringPacket(char* pMsg);
	VOID				UpdateHeartBeat(DWORD dwTick);

	VOID                SetBattleType(DWORD dwData){m_dwBattleType = dwData;}
	DWORD               GetBattleType(){return m_dwBattleType;}

private:
	RobotEntry*			m_pRobotEntry;
	RobotSession		m_cRobotSession;
	CharListInfo		m_tCharListInfo[MAX_CHARACTER_NUM];		//机器人角色列表
	RobotBaseInfo		m_tBaseInfo;							//机器人基本信息
	RobotStat			m_enRobotStat;							//机器人状态
	RobotAIStat			m_enRobotAIStat;						//机器人AI状态
	RobotBattleStat		m_enRobotBattleStat;					//机器人战斗状态
	DWORD				m_dwIntervalTime;						//切换AI间隔时间
	DWORD				m_dwIdleTime;							//空闲时间
	DWORD				m_dwHeartBeatTime;						//心跳时间
	DWORD				m_dwEnterIntervalTime;					//战斗申请间隔
	GameObjectMap		m_GameObjectMap;						//游戏对象列表
	Chief*				m_pChief;								//我方首领对象
	Chief*				m_pEnemyChief;							//敌方首领对象
	DWORD               m_dwBattleType;                         //战斗类型
	DWORD				m_dwRandIntervalTime;					//随机间隔时间
	BOOL				m_bEnterGame;							//是否进入游戏了
	DWORD				m_dwLastGotoTime;						//上一次行走的时间
};