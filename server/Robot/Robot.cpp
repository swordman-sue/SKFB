#include "stdafx.h"
#include "RobotManager.h"
#include "Robot.h"
#include "BattleFactory.h"
#include "ChatMsgDef.h"
#include "GameObject.h"
#include "Chief.h"
#include "../ToolKit/IniFile.h"
#include "../ToolKit/Rand.h"
#include "../Common/CommonLogic.h"

Robot::Robot()
{

}

Robot::~Robot()
{

}

BOOL Robot::Init(RobotEntry* pRobotEntry)
{
	if (!pRobotEntry)
		return FALSE;

	m_pRobotEntry = pRobotEntry;
	m_cRobotSession.SetOwner(this);
	memset(m_tCharListInfo, 0, sizeof(m_tCharListInfo));
	m_enRobotStat = RS_INIT;
	m_enRobotAIStat = RAS_NONE;
	m_enRobotBattleStat = RBS_NONE;
	m_dwIntervalTime = 0;
	m_dwHeartBeatTime = 0;
	m_dwIdleTime = 0;
	m_dwHeartBeatTime = 0;
	m_dwEnterIntervalTime = (DWORD)time(NULL);
	m_GameObjectMap.clear();
	m_pChief = NULL;
	m_pEnemyChief = NULL;
	m_bEnterGame = FALSE;
	m_dwLastGotoTime = 0;

	m_dwRandIntervalTime = rand() % 5000;

	return TRUE;
}

VOID Robot::Release()
{
	m_GameObjectMap.clear();

	if (m_cRobotSession.IsConnected())
		m_cRobotSession.Disconnect();
}

VOID Robot::Update(DWORD dwTick)
{
	if (m_bEnterGame)
		UpdateHeartBeat(dwTick);
	//UpdateAIStat(dwTick);
}

VOID Robot::Test()
{
	MSG_C2S_BATTLE_START_REQ msg;
	msg.m_byType = BATTLE_MAIN_MAP;
	msg.m_dwParam1 = 1;
	msg.m_dwParam2 = 1;
	SendPacket(&msg);
}

VOID Robot::OnConnect(BOOL bSuccess, DWORD dwNetworkIndex )
{
	if (!m_pRobotEntry)
		return;

	//连接成功
	if (bSuccess)
	{
		if (RobotManager::Instance()->IsEnableSecurity())
		{
			m_cRobotSession.EnableSecurity();
		}

		//进行登录处理
		ProcessRobotLoginSyn();
	}
	else
	{
		printf("机器人(AccID=%s)连接服务器失败！\n", m_pRobotEntry->m_szAccID);
	}
}

VOID Robot::OnDisconnect()
{
	if (!m_pRobotEntry)
		return;

	//printf("机器人(AccID=%s)与服务器断开连接！\n", m_pRobotEntry->m_szAccID);
	RobotManager::Instance()->OnDisconnect(this);
}

//接收登录结果事件
VOID Robot::OnRobotLoginAck(MSG_M2C_ROBOT_LOGIN_ACK* pMsg)
{
	if (!pMsg || !m_pRobotEntry)
		return;

	//登录失败
	if (pMsg->m_dwErrorCode != 0)
	{	
		printf("机器人(AccID=%s)登录失败！！！\n", m_pRobotEntry->m_szAccID);

		//与服务器断开连接
		m_cRobotSession.Disconnect();
	}
}

VOID Robot::OnMSG_S2C_LOGIN_ACCOUNT_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]收到登录响应\n", m_pRobotEntry->m_szAccID);

	MSG_C2S_LOGIN_SELECT_SERVER_REQ send_msg;
	send_msg.m_dwSelectZoneId = RobotManager::Instance()->GetSelectZoneId();
	SendPacket(&send_msg);
}

VOID Robot::OnMSG_S2C_LOGIN_SELECT_SERVER_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]收到选择服务器响应\n", m_pRobotEntry->m_szAccID);

	MSG_S2C_LOGIN_SELECT_SERVER_RESP *pRecv = (MSG_S2C_LOGIN_SELECT_SERVER_RESP*)pMsg;

	// 木有角色
	if (!pRecv->m_byHaveRole)
	{
		MSG_C2S_LOGIN_CREATE_ROLE_REQ send_msg;
		strncpy(send_msg.m_szRoleName, m_pRobotEntry->m_szAccID, sizeof(send_msg.m_szRoleName));
		send_msg.m_bySex = 1;
		send_msg.m_byType = 0;
		SendPacket(&send_msg);
	}
}

VOID Robot::OnMSG_S2C_LOGIN_CREATE_ROLE_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]收到创建角色响应\n", m_pRobotEntry->m_szAccID);
}

VOID Robot::OnMSG_S2C_GAME_ERROR_NOTIFY(NetMsgBody *pMsg)
{
	MSG_S2C_GAME_ERROR_NOTIFY *pRecv = (MSG_S2C_GAME_ERROR_NOTIFY*)pMsg;

	printf("[Account=%s]收到错误通知[error_code=%d]\n", 
		m_pRobotEntry->m_szAccID, pRecv->m_wErrorCode);
}

VOID Robot::OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY(NetMsgBody *pMsg)
{
	printf("[Account=%s]收到进入游戏通知\n", m_pRobotEntry->m_szAccID);
	
	MSG_C2S_RANK_LIST_REQ msg1;
	msg1.m_byType = 5,
	SendPacket(&msg1);

	MSG_C2S_RANK_LIST_REQ msg2;
	msg2.m_byType = 6,
	SendPacket(&msg2);

	m_bEnterGame = TRUE;
	m_dwLastGotoTime = GetTickCount();
}

VOID Robot::OnMSG_S2C_ROLE_INFO_NOTIFY(NetMsgBody *pMsg)
{
	MSG_S2C_ROLE_INFO_NOTIFY *pRecv = (MSG_S2C_ROLE_INFO_NOTIFY*)pMsg;

	// 使用GM添加精灵
	MSG_C2M_GM_REQ gm_msg;
	gm_msg.m_wCmd = GM_ADD_SPIRIT;
	gm_msg.m_dwValue1 = 50022;
	SendPacket(&gm_msg);

	// 进入攻城
	if (RobotManager::Instance()->IsTestAttackCity())
	{
		MSG_C2S_ATTACK_CITY_ENTER_REQ enter_msg;
		SendPacket(&enter_msg);
	}

	printf("[Account=%s]收到角色基本信息\n", m_pRobotEntry->m_szAccID);
}

VOID Robot::OnMSG_S2C_ATTACK_CITY_ENTER_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]已经进入怪物攻城\n", m_pRobotEntry->m_szAccID);
}

VOID Robot::OnMSG_S2C_BATTLE_START_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]收到请求战斗响应\n", m_pRobotEntry->m_szAccID);

	MSG_C2S_BATTLE_END_REQ msg;
	msg.m_byType = BATTLE_MAIN_MAP;
	msg.m_nWinValue = true;
	msg.m_byStar = 3;
	SendPacket(&msg);
}

//加入到GameObject列表
VOID Robot::AddToGameObjectMap(GameObject* pGameObject)
{
	if (!pGameObject)
		return;

	GameObjectMapIter iter = m_GameObjectMap.find(pGameObject->GetObjectKey());
	if (iter != m_GameObjectMap.end())
		return;

	m_GameObjectMap.insert(std::make_pair(pGameObject->GetObjectKey(), pGameObject));
}

//从GameObject列表中删除
VOID Robot::RemoveFromGameObjectMap(GameObject* pGameObject)
{
	if (!pGameObject)
		return;

	GameObjectMapIter iter = m_GameObjectMap.find(pGameObject->GetObjectKey());
	if (iter == m_GameObjectMap.end())
		return;

	m_GameObjectMap.erase(iter);
}

//处理机器人登录请求
VOID Robot::ProcessRobotLoginSyn()
{
	if (!m_pRobotEntry)
		return;

	printf("机器人(AccID=%s)进行登录请求！！！\n", m_pRobotEntry->m_szAccID);

	//先发送http验证包
// 	char szMsg[128];
// 	strcpy(szMsg, "tgw_l7_forward\r\nHost: 192.168.0.184:2003\r\n\r\n");
// 	SendStringPacket(szMsg);

	MSG_C2S_LOGIN_ACCOUNT_REQ msg;
	msg.m_byType = LOGIN_USER_NAME_REGISTER;
	strncpy(msg.m_szAccount, m_pRobotEntry->m_szAccID, sizeof(msg.m_szAccount));
	strncpy(msg.m_szPassword, "123456", sizeof(msg.m_szPassword));
	SendPacket(&msg);
}

//处理角色选择通知
VOID Robot::ProcessChooseCharCmd()
{
	if (!m_tCharListInfo[0].m_dwCharID)
		return;

// 	MSG_C2M_CHOOSE_CHAR_SYN msg;
// 	msg.dwCharID = m_tCharListInfo[0].m_dwCharID;
// 	SendPacket(&msg);
}

//发送战斗投降通知
VOID Robot::ProcessBattleSurrenderOptCmd()
{
// 	MSG_C2M_BATTLE_SURRENDER_OPT_CMD msg;
// 	SendPacket(&msg);
}

//发送战斗准备确认通知
VOID Robot::ProcessBattlePrepareAffirmCmd()
{
// 	MSG_C2M_BATTLE_PREPARE_AFFIRM_CMD msg;
// 	SendPacket(&msg);
}

//切换AI
VOID Robot::ChangeAI()
{
	m_enRobotAIStat = RAS_NONE;
	m_dwIntervalTime = CHANGE_AI_INTERVAL;
}

//随机AI状态
VOID Robot::RandomAIStat()
{
	DWORD dwChangce = Rand::Instance()->urand(1,1000000);
	if (dwChangce <= RAO_IDLE)
		m_enRobotAIStat = RAS_IDLE;
	else if (dwChangce <= RAO_BATTLE)
		m_enRobotAIStat = RAS_BATTLE;
	else
		m_enRobotAIStat = RAS_CHAT;
	
	OnEnterAIStat();
}

//处理进入AI状态
VOID Robot::OnEnterAIStat()
{
	if (m_enRobotAIStat <= RAS_NONE || m_enRobotAIStat >= RAS_MAX)
		return;

	switch(m_enRobotAIStat)
	{
	case RAS_IDLE:
		{
			OnEnterIdleStat();
		}
		break;

	case RAS_BATTLE:
		{
			OnEnterBattleStat();
		}
		break;
	}
}

//处理进入空闲状态
VOID Robot::OnEnterIdleStat()
{
	m_dwIdleTime = Rand::Instance()->urand(1, MAX_IDLE_TIME);
	m_dwIdleTime *= 1000;
}

//处理进入战斗状态
VOID Robot::OnEnterBattleStat()
{
// 	//进入匹配战斗请求
// 	DWORD dwTime = time(NULL) - m_dwEnterIntervalTime ; 
// 	if(dwTime< ENTER_BATTALE_INTERVAL_TIME)
// 		return;
// 		
// 	
// 	if (RobotBattle_MATCHING == GetBattleType())
// 	{
// 		MSG_C2M_BATTLE_MATCHING_SYN msg;
// 		SendPacket(&msg);
// 		RobotManager::Instance()->AddSendNum();
// 		m_enRobotAIStat = RAS_BATTLE_SYN;
// 	}
// 	//进入天梯战斗请求
// 	if (RobotBattle_LADDER == GetBattleType())
// 	{
// 		MSG_C2M_BATTLE_LADDER_SYN msg;
// 		SendPacket(&msg);
// 		RobotManager::Instance()->AddSendNum();
// 		m_enRobotAIStat = RAS_BATTLE_SYN;
// 	}
// 
// 	//设置战斗状态
// 	m_enRobotBattleStat = RBS_MATING;
// 	m_dwEnterIntervalTime = time(NULL);
}

//更新AI状态
VOID Robot::UpdateAIStat(DWORD dwTick)
{
	if (m_enRobotStat != RS_LOAD_FINISH )
		return;
	if(m_enRobotAIStat == RAS_BATTLE_ACK )
		return;

	switch(m_enRobotAIStat)
	{
	case RAS_NONE:
		{
			ProcessNoneStat(dwTick);
		}
		break;

	case RAS_IDLE:
		{
			ProcessIdleStat(dwTick);
		}
		break;
	case RAS_BATTLE:
		{
			OnEnterBattleStat();
		}
		break;
	case RAS_BATTLE_SYN:
		{
			//ProcessBattleStat(dwTick);
		}
		break; 
	case RAS_CHAT:
		{
			ProcessChatStat();
		}
		break;
	}
}

//处理空状态
VOID Robot::ProcessNoneStat(DWORD dwTick)
{
	//消耗状态切换间隔时间
	if (m_dwIntervalTime && m_dwIntervalTime > dwTick)
	{
		m_dwIntervalTime -= dwTick;
		return;
	}

	m_dwIntervalTime = 0;

	//随机新的AI状态
	RandomAIStat();
}

//处理空闲状态
VOID Robot::ProcessIdleStat(DWORD dwTick)
{
	if (m_dwIdleTime > dwTick)
	{
		m_dwIdleTime -= dwTick;
	}
	else
	{
		m_dwIdleTime = 0;
		ChangeAI();
	}
}

//处理聊天状态
VOID Robot::ProcessChatStat()
{
	/*if (RobotManager::Instance()->GetOpenChat())
	{
		char	szDes[512];

		//发送聊天请求
		MSG_C2M_CHAT_SYN msg;
		msg.tChatReq.byChatType = CHAT_TYPE_WORLD;

		//设定聊天内容
		BYTE byChatIndex = Rand::Instance()->urand(0, sizeof(pszChatMsg)/sizeof(pszChatMsg[0])-1);
		memset(szDes, 0, sizeof(szDes));
		WideCharToMultiByte(CP_UTF8, 0, pszChatMsg[byChatIndex], -1 , szDes, sizeof(szDes), 0, 0);

		strncpy(msg.tChatReq.szChatText, szDes, sizeof(msg.tChatReq.szChatText));
		msg.tChatReq.szChatText[MAX_CHAT_TEXT_LEN] = 0;
		msg.tChatReq.shChatTextSize = (short)strlen(msg.tChatReq.szChatText);

		//设定聊天扩展物品
		msg.tChatReq.byChatExpandType = CHAT_EXPAND_TYPE_NONE;
		msg.tChatReq.shChatParaNum = 0;

		SendPacket(&msg);
	}*/

	ChangeAI();
}

//处理战斗状态
VOID Robot::ProcessBattleStat(DWORD dwTick)
{

	//if (m_pChief)
		//m_pChief->ProcessAI();
}

//发送消息
BOOL Robot::SendPacket(NetMsgBody* pMsg)
{
	if (!pMsg)
		return FALSE;

	static unsigned char buffer[sizeof(DWORD)+2*MAX_PACK_SIZE]={0};
	unsigned char* pBuffer = buffer;
	short nBuffHeadSize = 0;
	DWORD dwSeriesId = 0;

	//填充包头信息
	if (NetMsgHeadHandler::EncodeClientHead(pMsg->GetType(), dwSeriesId, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	//填充包体信息
	pBuffer +=nBuffHeadSize;
	short nBuffBodySize = 0;
	if(pMsg->EncodeMsg(pBuffer, nBuffBodySize))
	{
		return FALSE;
	}

	return m_cRobotSession.Send((BYTE*)buffer, nBuffHeadSize + nBuffBodySize);
}

//发送字符串消息
BOOL Robot::SendStringPacket(char* pMsg)
{
	if (!pMsg)
		return FALSE;

	return (BOOL)send(m_cRobotSession.GetSocket(), pMsg, (INT)strlen(pMsg), 0);
}

//更新心跳包
VOID Robot::UpdateHeartBeat(DWORD dwTick)
{
	//m_dwRandIntervalTime = 0;
// 	m_dwHeartBeatTime += dwTick;
// 	if (m_dwHeartBeatTime >= HEART_BEAT_INTERVAL + m_dwRandIntervalTime)
// 	{
// 		m_dwHeartBeatTime -= HEART_BEAT_INTERVAL;
// 		m_dwHeartBeatTime -= m_dwRandIntervalTime;
// 
// 		MSG_C2S_HEART_BEAT_REQ msg;
// 		SendPacket(&msg);
// 
// 		printf("发送心跳包(AccID=%s,TickCount:%d)\n", m_pRobotEntry->m_szAccID,GetTickCount());
// 	}


	m_dwHeartBeatTime += dwTick;
	if (m_dwHeartBeatTime >= HEART_BEAT_INTERVAL + m_dwRandIntervalTime)
	{
		m_dwHeartBeatTime -= HEART_BEAT_INTERVAL;
		m_dwHeartBeatTime -= m_dwRandIntervalTime;

		int nRandValue = g_Rand(0, 3);

		// 装备列表请求
		if (0 == nRandValue)
		{
			MSG_C2S_EQUIPMENT_LIST_REQ msg;
			SendPacket(&msg);
		}
		// 请求排行榜列表
		else if (1 == nRandValue)
		{
			MSG_C2S_RANK_LIST_REQ msg;
			msg.m_byType = g_Rand(1, 2);
			msg.m_bySubType = g_Rand(0, CAMP_NUM);
			SendPacket(&msg);
		}
		// 聊天
		else if (2 == nRandValue)
		{
			//if (g_Rand(1, 100) <= 1)
			//{
// 				MSG_C2S_CHAT_REQ msg;
// 				memset(&msg.m_tData, 0, sizeof(msg.m_tData));
// 				msg.m_tData.byChatType = CHAT_TYPE_WORLD;
// 				msg.m_tData.byIsVoice = false;
// 				sprintf_s(msg.m_tData.szChatText, MAX_CHAT_TEXT_LEN - 1, "Hello:%s:%d", m_pRobotEntry->m_szAccID, GetTickCount());
// 				SendPacket(&msg);
			//}
		}
		// 心跳包
		else if (3 == nRandValue)
		{
			MSG_C2S_HEART_BEAT_REQ msg;
			msg.m_dwValue = time(NULL);
			SendPacket(&msg);
		}

		printf("发送消息(AccID=%s, RandValue:%d, Tick:%d)\n",
			m_pRobotEntry->m_szAccID, nRandValue, GetTickCount());

		// 行走 80,200 1500,200 //== 3秒走一次
		if (GetTickCount() >= m_dwLastGotoTime + 5000)
		{
			m_dwLastGotoTime = GetTickCount();
			if (RobotManager::Instance()->IsTestAttackCity())
			{
				MSG_C2S_ATTACK_CITY_GOTO_REQ goto_msg;
				goto_msg.m_tSrcPos.wMapX = g_Rand(80, 1500);
				goto_msg.m_tSrcPos.wMapY = 200;
				goto_msg.m_tDestPos.wMapX = g_Rand(80, 1500);
				goto_msg.m_tDestPos.wMapY = 200;
				SendPacket(&goto_msg);
			}
		}
	}
}


// @see Response OnMSG_M2C_RANK_LIST_ACK_NEW
VOID Robot::UnitTest()
{
// 	printf("机器人(AccID=%s)请求排行榜\n", m_pRobotEntry->m_szAccID);
// 	MSG_C2M_RANK_LIST_SYN_NEW msg;
// 	msg.m_byType = RANK_TYPE_FIGHT_VALUE;
// 	SendPacket(&msg);
}

