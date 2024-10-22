#include "stdafx.h"
#include "ChatManager.h"
#include "MapServer.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GMManager.h"
#include "OpenServerSession.h"
#include "AccountInfo.h"
#include "../ToolKit/DBCFileManager.h"
#include "MapSvrQuery.h"
#include "Guild/GuildManager.h"
#include "../Common/MsgDefine.h"
#include "Friend/FriendManager.h"
#include "Title/TitleManager.h"
#include "ConfigManager.h"
#include "TeamDungeon/TeamDungeonManager.h"

ChatManager::ChatManager()
{
 	m_dwSeriesId = 0;
}

ChatManager::~ChatManager()
{

}

BOOL ChatManager::Init()
{
	//��ȡ���쾲̬����
// 	const DBCFile* pDbcFile = DBCFileManager::Instance()->GetDBC(DBC_CHAT_ENTRY);
// 	if(!pDbcFile)
// 		return FALSE;
// 
// 	m_pChatEntry = (ChatEntry*)pDbcFile->GetRecord(1);
// 	if (!m_pChatEntry)
// 		return FALSE;
// 
 	m_dwSeriesId = 0;

	return TRUE;
}

VOID ChatManager::Release()
{

}

VOID ChatManager::Update()
{

}


//��������
VOID ChatManager::OnNetChat(Player* pPlayer, const ChatReqData *pReqData)
{
	if (!pPlayer || !pReqData)
  		return;

  
  	// �������ݼ�� 
//  	ChangeLocalChar((char*)pChatInfo->szChatText, m_szChatText, sizeof(m_szChatText));
// 	if (g_MapServer.MatchSqlCmd(m_szChatText))
// 	{
// 		pPlayer->SendErrorMsg(ERROR_CHAT_SENSITIVE_WORD);
// 		MAPLOG(GUI_LEVEL, "����������������:CharName=%s, ChatText:%s", pPlayer->GetUTF8RoleName(), m_szChatText);
// 		return;
// 	}


	WORD wFromMsgId = C2S_CHAT_REQ;

	DWORD dwCurTick = time(NULL);
	DWORD dwLastChatTime = pPlayer->GetLastChatTime();

	// ��⹫��CD(��) 
	if (dwCurTick < dwLastChatTime + g_GetConfigValue(218))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "����CD��[role:%d,last:%d,cur:%d]...", pPlayer->GetRoleId(), dwLastChatTime, dwCurTick);
		return;
	}

	// ����Ƿ��ڽ�����
	if (time(NULL) < pPlayer->GetProperty(ROLE_PRO_FORBID_TO_TALK_OVER_TIME))
	{
		pPlayer->SendErrorMsg(ERROR_CHAT_FORBID_TO_TALK);
		return;
	}

	pPlayer->SetLastChatTime(dwLastChatTime);
	
	MSG_S2C_CHAT_NOTIFY msg;
	memset(&msg.m_tNotifyData, 0, sizeof(msg.m_tNotifyData));
	msg.m_tNotifyData.byChatType			= pReqData->byChatType;
	msg.m_tNotifyData.dwVoiceId				= pReqData->byIsVoice ? GetNewSeriesId() : 0;
	msg.m_tNotifyData.tSenderData.dwRoleID	= pPlayer->GetCharID();
	msg.m_tNotifyData.tSenderData.byVipLevel = pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);
	msg.m_tNotifyData.tSenderData.dwHeadIcon = pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE);
	msg.m_tNotifyData.tSenderData.wServerId = pPlayer->GetServerId();
	msg.m_tNotifyData.tSenderData.dwCurUseNicknameId = pPlayer->GetTitleManager()->GetCurUseNicknameId();
	strncpy_s(msg.m_tNotifyData.szChatText, MAX_CHAT_TEXT_LEN, pReqData->szChatText, MAX_CHAT_TEXT_LEN);
	strncpy_s(msg.m_tNotifyData.tSenderData.szRoleName, MAX_ROLE_LEN, pPlayer->GetRoleName(), MAX_ROLE_LEN);
	msg.m_byGMLevel = pPlayer->GetVipGMLevel();

	int nErrorCode;

	memcpy_s(&m_tChatRecord, sizeof(ChatNotifyData), &msg.m_tNotifyData, sizeof(ChatNotifyData));
	m_tChatRecord.dwTime = time(NULL);
	m_tChatRecord.byGMLevel = pPlayer->GetVipGMLevel();

	// ����Ƶ��
	if (CHAT_TYPE_WORLD == pReqData->byChatType)
	{
		nErrorCode = OnWorldChat(pPlayer, &msg);
	}
	// ˽��Ƶ��
	else if (CHAT_TYPE_P2P == pReqData->byChatType)
	{
		nErrorCode = OnP2PChat(pPlayer, &msg, pReqData->szTargetName);
	}
	// ����Ƶ��
	else if (CHAT_TYPE_GUILD == pReqData->byChatType)
	{
		nErrorCode = OnGuildChat(pPlayer, &msg);
	}
	// ����Ƶ��
// 	else if (CHAT_TYPE_TEAM == pReqData->byChatType)
// 	{
// 		nErrorCode = OnTeamChat(pPlayer, &msg);
// 	}
	// ���
	else if (CHAT_TYPE_TEAM == pReqData->byChatType)
	{
		nErrorCode = OnTeamHallChat(pPlayer, &msg);
	}
	else 
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "�޷�ʶ�����������:ChatType:%d",pReqData->byChatType);
		return;
	}

	if (nErrorCode == ERROR_CODE_OK)
	{
		pPlayer->SendPacket(&MSG_S2C_CHAT_RESP());

		if (pReqData->byIsVoice)
		{
			AddVoice(GetNewSeriesId(), pReqData->szVoice);
		}

		// �洢���ݿ�
		ChatRecord_Save* pQuery = ChatRecord_Save::ALLOC();
		pQuery->SetChatType(pReqData->byChatType);
		pQuery->SetSender(pPlayer->GetRoleName());
		pQuery->SetTarget(pReqData->szTargetName);
		pQuery->SetContext(pReqData->szChatText);
		pQuery->SetServerId(pPlayer->GetServerId());
		pQuery->SetIndex(DB_CHAT_RECORD_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_LOG);
		g_MapServer.MapDBQuery(pQuery);
	}
	else
	{
		pPlayer->SendErrorMsg(nErrorCode);
	}
}

// ��������
VOID ChatManager::OnNetChatVoice(Player* pPlayer, DWORD dwVoiceId)
{
	ChatVoice *pVoice = GetVoice(dwVoiceId);
	if (!pVoice)
	{
		MAPLOG(GUI_LEVEL, "�������������ѹ���:VoiceId:%d", dwVoiceId);
		pPlayer->SendErrorMsg(ERROR_CHAT_VOICE_INVALID);
		return;
	}

	MSG_S2C_CHAT_VOICE_RESP msg;
	memset(msg.m_szVoice, 0, sizeof(msg.m_szVoice));
	strcpy_s(msg.m_szVoice, sizeof(msg.m_szVoice) - 1, pVoice->szVoice);
}

// ���������¼
void ChatManager::OnNetChatRecordList(Player* pPlayer, BYTE byChatType)
{
	// ����Ƶ��
	if (CHAT_TYPE_WORLD == byChatType)
	{
		MSG_S2C_CHAT_RECORD_LIST_RESP msg;
		msg.m_byChatType = byChatType;
		msg.m_wRecordNum = 0;
		map<DWORD, list<ChatRecord>>::iterator map_iter = m_ChatRecordList.find(pPlayer->GetServerId());
		if (map_iter != m_ChatRecordList.end())
		{
			for (list<ChatRecord>::iterator list_iter = map_iter->second.begin(); list_iter != map_iter->second.end(); ++list_iter)
			{
				if (msg.m_wRecordNum >= MAX_CHAT_RECORD_COUNT)
					break;

				msg.m_arRecordList[msg.m_wRecordNum++] = *list_iter;
			}
		}
		pPlayer->SendPacket(&msg);
	}
	// ����Ƶ��
	else if (CHAT_TYPE_GUILD == byChatType)
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
		if (pGuild)
		{
			pGuild->SendChatRecordList(pPlayer);
		}
	}
	else
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_CHAT_RECORD_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д��������Ƶ������[chat_type:%d]", __FUNCTION__, byChatType);
	}
}

VOID ChatManager::ChangeLocalChar(char* pszSrc, char* pszDst, size_t nDstSize)
{
	if (!pszSrc || !pszDst || nDstSize <= 0)	
		return;

	wchar_t wszTmp[1024];
	MultiByteToWideChar(CP_UTF8,0, (LPCSTR)pszSrc, -1, wszTmp, 1024);
	WideCharToMultiByte(CP_ACP, 0, wszTmp, -1 , (LPSTR)pszDst, (int)nDstSize, 0, 0);
}


VOID ChatManager::SendBroadcastMsg(DWORD dwBroadCastId, DWORD dwValue1, const char *pszText1, 
								   DWORD dwValue2, const char *pszText2, DWORD dwValue3, const char *pszText3)
{
	//static MSG_M2C_SYS_BROADCAST msg;
}

// ���͹㲥��Ϣ
VOID ChatManager::SendBroadcastMsg(DWORD dwBroadCastId, const char *pszText)
{
// 	static MSG_M2C_SYS_BROADCAST msg;
// 	msg.m_dwBroadCastId = dwBroadCastId;
// 	memset(msg.m_szText, 0, sizeof(msg.m_szText));
// 	if (pszText)
// 		strcpy_s(msg.m_szText, sizeof(msg.m_szText), pszText);
// 
// 	PlayerManager::Instance()->SendPacketAround(&msg);
}

// ����Ƶ��
int ChatManager::OnWorldChat(Player* pPlayer, NetMsgBody *pMsg)
{
	if (!CheckWorldChat(pPlayer))
		return ERROR_UNKNOW;

	PlayerManager::Instance()->SendWorldChat(pMsg, pPlayer->GetServerId(), pPlayer->GetRoleId());

	AddWorldChatRecord(pPlayer->GetServerId(), m_tChatRecord);

	pPlayer->ModifyProperty(ROLE_PRO_CHAT_TIMES, 1);

	return ERROR_CODE_OK;
}

// ˽��
int ChatManager::OnP2PChat(Player* pPlayer, NetMsgBody *pMsg, const char *pszTargetName)
{
	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharName(pszTargetName);
 	if (!pTarget)
 	{
		// Ŀ��û���߻��߲�����
		MAPLOG(GUI_LEVEL, "Ŀ�겻���ڻ�������");
		return ERROR_CHAT_TARGET_INVALID;
 	}

	// ���ܶ��Լ�˵��
	if (pTarget->GetCharID() == pPlayer->GetCharID())
	{
		MAPLOG(GUI_LEVEL, "[%s]���ܶ��Լ�˵��[%s]",__FUNCTION__, pPlayer->GetUTF8RoleName());
		return ERROR_CHAT_CAN_NOT_SAY_TO_SELF;
	}

	// ����໥�Ƿ����ں�������
	if (pPlayer->GetFriendManager()->IsBlack(pTarget->GetRoleId()) || 
		pTarget->GetFriendManager()->IsBlack(pPlayer->GetRoleId()))
	{
		//return ERROR_CODE_OK;
		return ERROR_CHAT_IN_TARGET_BLACK_LIST;
	}

	pTarget->SendPacket(pMsg);
	pPlayer->SendPacket(pMsg);
  
	return ERROR_CODE_OK;
}

// ����
int ChatManager::OnGuildChat(Player* pPlayer, NetMsgBody *pMsg)
{
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		return ERROR_CHAT_GUILD_INVALID;
	}

	pGuild->Chat(pMsg);

	pGuild->AddChatRecord(m_tChatRecord);

	return ERROR_CODE_OK;
}

// ����
int ChatManager::OnTeamChat(Player* pPlayer, NetMsgBody *pMsga)
{
	return ERROR_CODE_OK;
}

// ��Ӵ���
int ChatManager::OnTeamHallChat(Player* pPlayer, NetMsgBody *pMsg)
{
	CTeamDungeonManager::Instance()->Chat(pMsg);

	// ��ȷ���Ƿ���Ҫ��¼�����¼
	//pGuild->AddChatRecord(m_tChatRecord);

	return ERROR_CODE_OK;
}

// �������
void ChatManager::AddVoice(DWORD dwVoiceId, const char *pszVoice)
{
	ChatVoice voice;
	strncpy_s(voice.szVoice, MAX_CHAT_VOICE_LEN - 1, pszVoice, MAX_CHAT_VOICE_LEN - 1);
	m_VoiceList.insert(make_pair(dwVoiceId, voice));

	if (m_VoiceList.size() >= 10000)
	{
		// todo::���
	}
}

// ȡ��������
ChatVoice* ChatManager::GetVoice(DWORD dwVoiceId)
{
	return NULL;
}

// �������Ƶ�������¼
void ChatManager::AddWorldChatRecord(DWORD dwServerId, const ChatRecord &tChatRecord)
{
	map<DWORD, list<ChatRecord>>::iterator iter = m_ChatRecordList.find(dwServerId);
	if (iter != m_ChatRecordList.end())
	{
		iter->second.push_back(tChatRecord);

		if (iter->second.size() > MAX_CHAT_RECORD_COUNT)
			iter->second.pop_front();
	}
	else
	{
		list<ChatRecord> vList;
		vList.push_back(tChatRecord);
		m_ChatRecordList.insert(make_pair(dwServerId, vList));
	}
}

// ��Ӵ��������¼
void ChatManager::AddTeamHallChatRecord(const ChatRecord &tChatRecord)
{
	m_TeamHallChatRecordList.push_back(tChatRecord);
}

//�趨������ʱ��
// VOID ChatManager::SetChatInterval(Player* pPlayer, BYTE byChatType)
// {
// 	if (!pPlayer || !m_pChatEntry)
// 		return;
// 
// 	if (byChatType < CHAT_TYPE_WORLD || byChatType >= CHAT_TYPE_MAX)
// 		return;
// 
// 	int nInterval = 0;
// 	switch (byChatType)
// 	{
// // 	case CHAT_TYPE_SYSTEM:
// // 		nInterval = m_pChatEntry->SystemInterval;
// // 		break;
// 
// 	case CHAT_TYPE_WORLD:
// 		nInterval = m_pChatEntry->WorldInterval;
// 		break;
// 
// 	case CHAT_TYPE_P2P:
// 		nInterval = m_pChatEntry->PersonalInterval;
// 		break;
// 
// 	default:
// 		break;
// 	}
// 
// 	if (nInterval)
// 		pPlayer->SetChatInterval((ChatType)byChatType, (DWORD)(nInterval*1000));
// }

// ȡһ���µ�ϵ��id
DWORD ChatManager::GetNewSeriesId()
{ 
	if (m_dwSeriesId >= INVALIDID)
		m_dwSeriesId = 0;

	return ++m_dwSeriesId; 
}

// ���������������
bool ChatManager::CheckWorldChat(Player* pPlayer)
{
	// �����Ҫ�Ľ�ɫ�ȼ�, vip�ȼ�����(������������һ������)
	//WORD wNeedRoleLevel = g_GetConfigValue(208);
	WORD wNeedVipLevel = g_GetConfigValue(274);
	WORD wNeedVipGMLevel = g_GetConfigValue(315);

	// ���VIPGM�ȼ�,0��ʾ����VIPGM�ȼ�
	if (wNeedVipGMLevel && pPlayer->GetVipGMLevel() < wNeedVipGMLevel)
	{
		MAPLOG(GUI_LEVEL, "[%s]����Ƶ����Ҫ��GM�ȼ�����[NeedVipGMLevel:%d,CurGMVipLevel:%d]",
			__FUNCTION__,wNeedVipGMLevel, pPlayer->GetVipGMLevel());
		return false;
	}

	// ���VIP�ȼ�(������������һ������),0��ʾ����VIP�ȼ�
	if (wNeedVipLevel && pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= wNeedVipLevel)
		return true;

	// ����ɫ�ȼ������������
	const RoleLevel_Config *pRoleLevelCfg = g_pCfgMgr->GetRoleLevel(pPlayer->GetLevel());
	if (!pRoleLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫ�ȼ�����[RoleLevel:%d]", __FUNCTION__, pPlayer->GetLevel());
		return false;
	}
	if (pPlayer->GetProperty(ROLE_PRO_CHAT_TIMES) >= pRoleLevelCfg->dwDailyChatTimesMax)
	{
		MAPLOG(GUI_LEVEL, "[%s]����Ƶ����������Ѵ�����[RoleId:%d,DailyChatTimesMax:%d]",
			__FUNCTION__,pPlayer->GetRoleId(), pRoleLevelCfg->dwDailyChatTimesMax);
		return false;
	}

	return true;
}