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

	//���ӳɹ�
	if (bSuccess)
	{
		if (RobotManager::Instance()->IsEnableSecurity())
		{
			m_cRobotSession.EnableSecurity();
		}

		//���е�¼����
		ProcessRobotLoginSyn();
	}
	else
	{
		printf("������(AccID=%s)���ӷ�����ʧ�ܣ�\n", m_pRobotEntry->m_szAccID);
	}
}

VOID Robot::OnDisconnect()
{
	if (!m_pRobotEntry)
		return;

	//printf("������(AccID=%s)��������Ͽ����ӣ�\n", m_pRobotEntry->m_szAccID);
	RobotManager::Instance()->OnDisconnect(this);
}

//���յ�¼����¼�
VOID Robot::OnRobotLoginAck(MSG_M2C_ROBOT_LOGIN_ACK* pMsg)
{
	if (!pMsg || !m_pRobotEntry)
		return;

	//��¼ʧ��
	if (pMsg->m_dwErrorCode != 0)
	{	
		printf("������(AccID=%s)��¼ʧ�ܣ�����\n", m_pRobotEntry->m_szAccID);

		//��������Ͽ�����
		m_cRobotSession.Disconnect();
	}
}

VOID Robot::OnMSG_S2C_LOGIN_ACCOUNT_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]�յ���¼��Ӧ\n", m_pRobotEntry->m_szAccID);

	MSG_C2S_LOGIN_SELECT_SERVER_REQ send_msg;
	send_msg.m_dwSelectZoneId = RobotManager::Instance()->GetSelectZoneId();
	SendPacket(&send_msg);
}

VOID Robot::OnMSG_S2C_LOGIN_SELECT_SERVER_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]�յ�ѡ���������Ӧ\n", m_pRobotEntry->m_szAccID);

	MSG_S2C_LOGIN_SELECT_SERVER_RESP *pRecv = (MSG_S2C_LOGIN_SELECT_SERVER_RESP*)pMsg;

	// ľ�н�ɫ
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
	printf("[Account=%s]�յ�������ɫ��Ӧ\n", m_pRobotEntry->m_szAccID);
}

VOID Robot::OnMSG_S2C_GAME_ERROR_NOTIFY(NetMsgBody *pMsg)
{
	MSG_S2C_GAME_ERROR_NOTIFY *pRecv = (MSG_S2C_GAME_ERROR_NOTIFY*)pMsg;

	printf("[Account=%s]�յ�����֪ͨ[error_code=%d]\n", 
		m_pRobotEntry->m_szAccID, pRecv->m_wErrorCode);
}

VOID Robot::OnMSG_S2C_LOGIN_ENTER_GAME_NOTIFY(NetMsgBody *pMsg)
{
	printf("[Account=%s]�յ�������Ϸ֪ͨ\n", m_pRobotEntry->m_szAccID);
	
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

	// ʹ��GM��Ӿ���
	MSG_C2M_GM_REQ gm_msg;
	gm_msg.m_wCmd = GM_ADD_SPIRIT;
	gm_msg.m_dwValue1 = 50022;
	SendPacket(&gm_msg);

	// ���빥��
	if (RobotManager::Instance()->IsTestAttackCity())
	{
		MSG_C2S_ATTACK_CITY_ENTER_REQ enter_msg;
		SendPacket(&enter_msg);
	}

	printf("[Account=%s]�յ���ɫ������Ϣ\n", m_pRobotEntry->m_szAccID);
}

VOID Robot::OnMSG_S2C_ATTACK_CITY_ENTER_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]�Ѿ�������﹥��\n", m_pRobotEntry->m_szAccID);
}

VOID Robot::OnMSG_S2C_BATTLE_START_RESP(NetMsgBody *pMsg)
{
	printf("[Account=%s]�յ�����ս����Ӧ\n", m_pRobotEntry->m_szAccID);

	MSG_C2S_BATTLE_END_REQ msg;
	msg.m_byType = BATTLE_MAIN_MAP;
	msg.m_nWinValue = true;
	msg.m_byStar = 3;
	SendPacket(&msg);
}

//���뵽GameObject�б�
VOID Robot::AddToGameObjectMap(GameObject* pGameObject)
{
	if (!pGameObject)
		return;

	GameObjectMapIter iter = m_GameObjectMap.find(pGameObject->GetObjectKey());
	if (iter != m_GameObjectMap.end())
		return;

	m_GameObjectMap.insert(std::make_pair(pGameObject->GetObjectKey(), pGameObject));
}

//��GameObject�б���ɾ��
VOID Robot::RemoveFromGameObjectMap(GameObject* pGameObject)
{
	if (!pGameObject)
		return;

	GameObjectMapIter iter = m_GameObjectMap.find(pGameObject->GetObjectKey());
	if (iter == m_GameObjectMap.end())
		return;

	m_GameObjectMap.erase(iter);
}

//��������˵�¼����
VOID Robot::ProcessRobotLoginSyn()
{
	if (!m_pRobotEntry)
		return;

	printf("������(AccID=%s)���е�¼���󣡣���\n", m_pRobotEntry->m_szAccID);

	//�ȷ���http��֤��
// 	char szMsg[128];
// 	strcpy(szMsg, "tgw_l7_forward\r\nHost: 192.168.0.184:2003\r\n\r\n");
// 	SendStringPacket(szMsg);

	MSG_C2S_LOGIN_ACCOUNT_REQ msg;
	msg.m_byType = LOGIN_USER_NAME_REGISTER;
	strncpy(msg.m_szAccount, m_pRobotEntry->m_szAccID, sizeof(msg.m_szAccount));
	strncpy(msg.m_szPassword, "123456", sizeof(msg.m_szPassword));
	SendPacket(&msg);
}

//�����ɫѡ��֪ͨ
VOID Robot::ProcessChooseCharCmd()
{
	if (!m_tCharListInfo[0].m_dwCharID)
		return;

// 	MSG_C2M_CHOOSE_CHAR_SYN msg;
// 	msg.dwCharID = m_tCharListInfo[0].m_dwCharID;
// 	SendPacket(&msg);
}

//����ս��Ͷ��֪ͨ
VOID Robot::ProcessBattleSurrenderOptCmd()
{
// 	MSG_C2M_BATTLE_SURRENDER_OPT_CMD msg;
// 	SendPacket(&msg);
}

//����ս��׼��ȷ��֪ͨ
VOID Robot::ProcessBattlePrepareAffirmCmd()
{
// 	MSG_C2M_BATTLE_PREPARE_AFFIRM_CMD msg;
// 	SendPacket(&msg);
}

//�л�AI
VOID Robot::ChangeAI()
{
	m_enRobotAIStat = RAS_NONE;
	m_dwIntervalTime = CHANGE_AI_INTERVAL;
}

//���AI״̬
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

//�������AI״̬
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

//����������״̬
VOID Robot::OnEnterIdleStat()
{
	m_dwIdleTime = Rand::Instance()->urand(1, MAX_IDLE_TIME);
	m_dwIdleTime *= 1000;
}

//�������ս��״̬
VOID Robot::OnEnterBattleStat()
{
// 	//����ƥ��ս������
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
// 	//��������ս������
// 	if (RobotBattle_LADDER == GetBattleType())
// 	{
// 		MSG_C2M_BATTLE_LADDER_SYN msg;
// 		SendPacket(&msg);
// 		RobotManager::Instance()->AddSendNum();
// 		m_enRobotAIStat = RAS_BATTLE_SYN;
// 	}
// 
// 	//����ս��״̬
// 	m_enRobotBattleStat = RBS_MATING;
// 	m_dwEnterIntervalTime = time(NULL);
}

//����AI״̬
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

//�����״̬
VOID Robot::ProcessNoneStat(DWORD dwTick)
{
	//����״̬�л����ʱ��
	if (m_dwIntervalTime && m_dwIntervalTime > dwTick)
	{
		m_dwIntervalTime -= dwTick;
		return;
	}

	m_dwIntervalTime = 0;

	//����µ�AI״̬
	RandomAIStat();
}

//�������״̬
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

//��������״̬
VOID Robot::ProcessChatStat()
{
	/*if (RobotManager::Instance()->GetOpenChat())
	{
		char	szDes[512];

		//������������
		MSG_C2M_CHAT_SYN msg;
		msg.tChatReq.byChatType = CHAT_TYPE_WORLD;

		//�趨��������
		BYTE byChatIndex = Rand::Instance()->urand(0, sizeof(pszChatMsg)/sizeof(pszChatMsg[0])-1);
		memset(szDes, 0, sizeof(szDes));
		WideCharToMultiByte(CP_UTF8, 0, pszChatMsg[byChatIndex], -1 , szDes, sizeof(szDes), 0, 0);

		strncpy(msg.tChatReq.szChatText, szDes, sizeof(msg.tChatReq.szChatText));
		msg.tChatReq.szChatText[MAX_CHAT_TEXT_LEN] = 0;
		msg.tChatReq.shChatTextSize = (short)strlen(msg.tChatReq.szChatText);

		//�趨������չ��Ʒ
		msg.tChatReq.byChatExpandType = CHAT_EXPAND_TYPE_NONE;
		msg.tChatReq.shChatParaNum = 0;

		SendPacket(&msg);
	}*/

	ChangeAI();
}

//����ս��״̬
VOID Robot::ProcessBattleStat(DWORD dwTick)
{

	//if (m_pChief)
		//m_pChief->ProcessAI();
}

//������Ϣ
BOOL Robot::SendPacket(NetMsgBody* pMsg)
{
	if (!pMsg)
		return FALSE;

	static unsigned char buffer[sizeof(DWORD)+2*MAX_PACK_SIZE]={0};
	unsigned char* pBuffer = buffer;
	short nBuffHeadSize = 0;
	DWORD dwSeriesId = 0;

	//����ͷ��Ϣ
	if (NetMsgHeadHandler::EncodeClientHead(pMsg->GetType(), dwSeriesId, pBuffer, nBuffHeadSize))
	{
		return FALSE;
	}

	//��������Ϣ
	pBuffer +=nBuffHeadSize;
	short nBuffBodySize = 0;
	if(pMsg->EncodeMsg(pBuffer, nBuffBodySize))
	{
		return FALSE;
	}

	return m_cRobotSession.Send((BYTE*)buffer, nBuffHeadSize + nBuffBodySize);
}

//�����ַ�����Ϣ
BOOL Robot::SendStringPacket(char* pMsg)
{
	if (!pMsg)
		return FALSE;

	return (BOOL)send(m_cRobotSession.GetSocket(), pMsg, (INT)strlen(pMsg), 0);
}

//����������
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
// 		printf("����������(AccID=%s,TickCount:%d)\n", m_pRobotEntry->m_szAccID,GetTickCount());
// 	}


	m_dwHeartBeatTime += dwTick;
	if (m_dwHeartBeatTime >= HEART_BEAT_INTERVAL + m_dwRandIntervalTime)
	{
		m_dwHeartBeatTime -= HEART_BEAT_INTERVAL;
		m_dwHeartBeatTime -= m_dwRandIntervalTime;

		int nRandValue = g_Rand(0, 3);

		// װ���б�����
		if (0 == nRandValue)
		{
			MSG_C2S_EQUIPMENT_LIST_REQ msg;
			SendPacket(&msg);
		}
		// �������а��б�
		else if (1 == nRandValue)
		{
			MSG_C2S_RANK_LIST_REQ msg;
			msg.m_byType = g_Rand(1, 2);
			msg.m_bySubType = g_Rand(0, CAMP_NUM);
			SendPacket(&msg);
		}
		// ����
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
		// ������
		else if (3 == nRandValue)
		{
			MSG_C2S_HEART_BEAT_REQ msg;
			msg.m_dwValue = time(NULL);
			SendPacket(&msg);
		}

		printf("������Ϣ(AccID=%s, RandValue:%d, Tick:%d)\n",
			m_pRobotEntry->m_szAccID, nRandValue, GetTickCount());

		// ���� 80,200 1500,200 //== 3����һ��
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
// 	printf("������(AccID=%s)�������а�\n", m_pRobotEntry->m_szAccID);
// 	MSG_C2M_RANK_LIST_SYN_NEW msg;
// 	msg.m_byType = RANK_TYPE_FIGHT_VALUE;
// 	SendPacket(&msg);
}

