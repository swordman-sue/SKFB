#pragma once

#include "RobotSession.h"
#include "../Common/MsgDefine.h"

#define MAX_IDLE_TIME					10		//������ʱ��
#define HEART_BEAT_INTERVAL				10000	//���������(3��)
#define CHANGE_AI_INTERVAL				2000	//�л�AI���ʱ��
#define ENTER_BATTALE_INTERVAL_TIME	    60	    //����ս���ļ��ʱ��
 


struct RobotEntry;
class  GameObject;
class  Chief;

//������״̬
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

//������AI״̬
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

//������ս��״̬
enum RobotBattleStat
{
	RBS_NONE			= 0,
	RBS_MATING			= 1,
	RBS_BATTLING		= 2,
	RBS_MAX,
};

//������AI״̬����
enum RobotAIOdds
{
	RAO_IDLE			= 150000,
	RAO_BATTLE			= 995000,
	RAO_CHAT			= 1000000,
};

//�����˻�����Ϣ
struct RobotBaseInfo
{
	DWORD				m_dwCharID;							//�����˽�ɫID
	char				m_szCharName[MAX_ROLE_LEN];		//�����˽�ɫ��
	BYTE				m_byRace;							//�����˽�ɫ����
	BYTE				m_byVocation;						//�����˽�ɫְҵ
	BYTE				m_byGender;							//�����˽�ɫ�Ա�
};

typedef std::map<DWORD, GameObject*>	GameObjectMap;
typedef GameObjectMap::iterator			GameObjectMapIter;

//�����˶���
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

	//�¼����
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

	//�������
private:
	VOID				ProcessRobotLoginSyn();
	VOID				ProcessChooseCharCmd();

	VOID				ProcessBattleSurrenderOptCmd();
	VOID				ProcessBattlePrepareAffirmCmd();

	//AI���
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
	//���ܺ���
public:
	BOOL				SendPacket(NetMsgBody* pMsg);
	BOOL				SendStringPacket(char* pMsg);
	VOID				UpdateHeartBeat(DWORD dwTick);

	VOID                SetBattleType(DWORD dwData){m_dwBattleType = dwData;}
	DWORD               GetBattleType(){return m_dwBattleType;}

private:
	RobotEntry*			m_pRobotEntry;
	RobotSession		m_cRobotSession;
	CharListInfo		m_tCharListInfo[MAX_CHARACTER_NUM];		//�����˽�ɫ�б�
	RobotBaseInfo		m_tBaseInfo;							//�����˻�����Ϣ
	RobotStat			m_enRobotStat;							//������״̬
	RobotAIStat			m_enRobotAIStat;						//������AI״̬
	RobotBattleStat		m_enRobotBattleStat;					//������ս��״̬
	DWORD				m_dwIntervalTime;						//�л�AI���ʱ��
	DWORD				m_dwIdleTime;							//����ʱ��
	DWORD				m_dwHeartBeatTime;						//����ʱ��
	DWORD				m_dwEnterIntervalTime;					//ս��������
	GameObjectMap		m_GameObjectMap;						//��Ϸ�����б�
	Chief*				m_pChief;								//�ҷ��������
	Chief*				m_pEnemyChief;							//�з��������
	DWORD               m_dwBattleType;                         //ս������
	DWORD				m_dwRandIntervalTime;					//������ʱ��
	BOOL				m_bEnterGame;							//�Ƿ������Ϸ��
	DWORD				m_dwLastGotoTime;						//��һ�����ߵ�ʱ��
};