#include "StdAfx.h"
#include "DBResultHandler.h"
#include "MapServer.h"
#include "GateServerSession.h"
#include "AccountInfo.h"
#include "AccountInfoManager.h"
#include "MapSvrQuery.h"
#include "Player.h"
#include "PlayerManager.h"
//#include "ServerRestrictions.h"
#include "../Common/MsgDefine.h"
#include "OpenServerSession.h"
#include "ObjectFactory.h"
#include "../Common/CommonLogic.h"
#include "../ToolKit/MiniDump.h"
#include "ChatManager.h"
#include "login.pb.h"
#include "PlayerBaseDataManager.h"
#include "Friend/FriendManager.h"
#include "Arena/ArenaManager.h"
#include "Rank/RankManager.h"
#include "GMManager.h"
#include "WorldBoss/WorldBossManager.h"
#include "Land/LandManager.h"
#include "Guild/GuildManager.h"
#include "KingMatch/KingMatchManager.h"
#include "TeamDungeon/TeamDungeonManager.h"
#include "GameLogic.h"
#include "Turntable/TurntableManager.h"
#include "ScoreMatch/ScoreMatchManager.h"
#include "ResourceWar/ResourceWarManager.h"
#include "Guild/GuildWarMgr.h"
#include "Logic/LogicManager.h"


DBResultHandler::DBResultHandler(void)
{

}

DBResultHandler::~DBResultHandler(void)
{

}

// ����˺Ž��
void DBResultHandler::ProcessCheckAccountQueryResult(QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION 
 	CheckAccountQuery* pQResult = (CheckAccountQuery*)pData; 
 
 	// ���ݿ����ʧ��
 	if (0 != pQResult->GetQueryResult())
 	{
 		MAPLOG(GUI_LEVEL,"�������ʺ����ݿ����ʧ��[Account:%s,Password:%s,Error:%d]",
 			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
 		return;
 	}
 
	AccountInfo* pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID());
 	if (pAccountInfo)
 	{
		// �ظ���¼
 		MAPLOG(GUI_LEVEL,"�ظ���¼[Account:%s]", pQResult->GetAccID());
 		return;
 	}

	// ����˺��Ƿ���
	if (pQResult->IsLock() && (!pQResult->GetUnlockTime() || time(NULL) < pQResult->GetUnlockTime()))
	{
		g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_ACCOUNT_LOCK);
		return;
	}

	// �ʺ��Ѵ���
	if (pQResult->GetResultRowNum())
	{
		// �ֻ�ע�᲻��Ҫ��֤����
		if (LOGIN_MOBILE_REGISTER != pQResult->GetLoginType() && strcmp(pQResult->GetOldPassword(), pQResult->GetPassword()) != 0)
		{
			if (LOGIN_USER_NAME_REGISTER == pQResult->GetLoginType() || LOGIN_ONE_KEY == pQResult->GetLoginType())
			{
				// �ʺ��Ѵ���
				g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_LOGIN_ACCOUNT_EXIST);
				MAPLOG(GUI_LEVEL, "�ʺ��Ѵ���[Account:%s]", pQResult->GetAccID());
			}
			else
			{
				// ���벻��ȷ, ֪ͨ���
				g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_LOGIN_PASSWORD_INVALID);
				MAPLOG(GUI_LEVEL, "���벻��ȷ[Account:%s]", pQResult->GetAccID());
			}
			return;
		}

		//����AccountInfo��Ϣ
		AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
		if (!pNewInfo)
		{
			MAPLOG(ERROR_LEVEL, "�ڴ治�㣬�޷�����AccountInfo��Ϣ");
			return;
		}

		// ������ֻ�ע�����޸�����
		if (LOGIN_MOBILE_REGISTER == pQResult->GetLoginType())
		{
			AccountPasswordModify* pQuery = AccountPasswordModify::ALLOC();
			pQuery->SetIndex(MAP_QUERY_CHANGE_PASSWORD);
			pQuery->SetThreadIndex(0);
			pQuery->SetAccID(pQResult->GetAccID());
			pQuery->SetPassword(pQResult->GetPassword());
			pQuery->SetZoneID(pQResult->GetZoneID());
			pQuery->SetKey(pQResult->GetKey());
			g_MapServer.MapDBQuery(pQuery);
		}

		pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(),
			pQResult->GetPassword(), pQResult->GetKey(), pQResult->GetChannelUID(), pQResult->GetTerminalType(), pQResult->GetMobile());
		pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

		//�����˺Ź�����
		AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

		// ��¼��Ӧ
		MSG_S2C_LOGIN_ACCOUNT_RESP msg;
		strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount) - 1, pQResult->GetAccID());
		msg.m_byIsBindMobile = strlen(pQResult->GetMobile()) ? true : false;
		msg.m_wLastLoginServerNum = 0;
		LastLoginServerData *pServerData;
		const PlayerBaseData *pPlayerBaseData = NULL;
		for (int i = 0; i < MAX_LOGIN_SERVER_LIST; ++i)
		{
			pServerData = pQResult->GetLastLoginServer(i);
			if (pServerData && pServerData->dwServerId)
			{
				// ��������¼���ķ�������Ϣ
				pPlayerBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(pServerData->dwServerId, pQResult->GetChannelUID(), pQResult->GetAccID());
				if (!pPlayerBaseData)
				{
					MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ�����ɫ��������[Account:%s,ChannelUID:%d,ServerId:%d]",
						__FUNCTION__, pQResult->GetAccID(), pQResult->GetChannelUID(), pServerData->dwServerId);
					continue;
				}
				// todo::���������ĵ�Դ����
				pServerData->wLevel = pPlayerBaseData->wLevel;
				pServerData->dwHeadIcon = pPlayerBaseData->wHeadIcon;
				strcpy_s(pServerData->szRoleName, sizeof(pServerData->szRoleName), pPlayerBaseData->szRoleName);

				msg.m_wLastLoginServerNum++;
				msg.m_LastLoginServerList[i] = *pServerData;
			}
		}

		g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);
	}
	else
	{
		// ��¼����(0=����,1=�û���ע��,2=�û�����¼,3=�ֻ���¼,4=�ֻ�ע��)
		// ����¼����(�û�����¼�������ֻ��ŵ�¼)
		if (LOGIN_USER_NAME_LOGIN == pQResult->GetLoginType() || LOGIN_MOBILE_LOGIN == pQResult->GetLoginType())
		{
			g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_ACCOUNT_NOT_REGISTER);
			MAPLOG(GUI_LEVEL, "�ʺŻ�ûע��,���ܵ�¼[%s]", pQResult->GetAccID());
			return;
		}

		// �����ʺ�
		CreateAccountQuery* pQuery = CreateAccountQuery::ALLOC();
		pQuery->SetIndex(MAP_QUERY_CREATE_ACCOUNT);
		pQuery->SetThreadIndex(0);
		pQuery->SetAccID(pQResult->GetAccID());
		pQuery->SetPassword(pQResult->GetPassword());
		pQuery->SetChannelUID(pQResult->GetChannelUID());
		pQuery->SetZoneID(pQResult->GetZoneID());
		pQuery->SetKey(pQResult->GetKey());
		pQuery->SetLoginType(pQResult->GetLoginType());
		pQuery->SetTerminalType(pQResult->GetTerminalType());
		g_MapServer.MapDBQuery(pQuery);
	}

//	LEAVE_DUMP_FUNCTION
}

// ����˺Ž��(sdk)
void DBResultHandler::ProcessSDKAccountCheckResult(QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION 
	CheckAccountQuery* pQResult = (CheckAccountQuery*)pData;

	// ���ݿ����ʧ��
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(IMPORTANT_LEVEL, "�������ʺ����ݿ����ʧ��[Account:%s,Password:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
		return;
	}

	// ����Ƿ��ظ���¼
	AccountInfo* pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID());
	if (pAccountInfo)
	{
		MAPLOG(GUI_LEVEL, "�ظ���¼[Account:%s]", pQResult->GetAccID());
		return;
	}

	// ����˺��Ƿ���(������ý���ʱ������Ҫ����Ƿ����)
	if (pQResult->IsLock() && (!pQResult->GetUnlockTime() || time(NULL) < pQResult->GetUnlockTime()))
	{
		g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_ACCOUNT_LOCK);
		return;
	}

	// �ʺ��Ѵ���
	if (pQResult->GetResultRowNum())
	{
		//����AccountInfo��Ϣ
		AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
		if (!pNewInfo)
		{
			MAPLOG(ERROR_LEVEL, "�ڴ治�㣬�޷�����AccountInfo��Ϣ");
			return;
		}

		pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(),
			pQResult->GetPassword(), pQResult->GetKey(), pQResult->GetChannelUID(), pQResult->GetTerminalType(), pQResult->GetMobile());
		pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

		//�����˺Ź�����
		AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

		// ��¼��Ӧ
		MSG_S2C_SDK_LOGIN_RESP msg;
		strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount) - 1, pQResult->GetAccID());
		msg.m_byIsBindMobile = strlen(pQResult->GetMobile()) ? true : false;
		msg.m_wLastLoginServerNum = 0;
		LastLoginServerData *pServerData;
		const PlayerBaseData *pPlayerBaseData = NULL;
		for (int i = 0; i < MAX_LOGIN_SERVER_LIST; ++i)
		{
			pServerData = pQResult->GetLastLoginServer(i);
			if (pServerData && pServerData->dwServerId)
			{
				// ��������¼���ķ�������Ϣ
				pPlayerBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(pServerData->dwServerId, pQResult->GetChannelUID(), pQResult->GetAccID());
				if (!pPlayerBaseData)
				{
					MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ�����ɫ��������[Account:%s,ChannelUID:%d,ServerId:%d]", 
						__FUNCTION__, pQResult->GetAccID(), pQResult->GetChannelUID(), pServerData->dwServerId);
					continue;
				}
				// todo::���������ĵ�Դ����
				pServerData->wLevel = pPlayerBaseData->wLevel;
				pServerData->dwHeadIcon = pPlayerBaseData->wHeadIcon;
				strcpy_s(pServerData->szRoleName, sizeof(pServerData->szRoleName), pPlayerBaseData->szRoleName);

				msg.m_wLastLoginServerNum++;
				msg.m_LastLoginServerList[i] = *pServerData;
			}
		}

		g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);
	}
	else
	{
		// �����ʺ�
		CreateAccountQuery* pQuery = CreateAccountQuery::ALLOC();
		pQuery->SetIndex(DB_SDK_CREATE_ACCOUNT);
		pQuery->SetThreadIndex(0);
		pQuery->SetAccID(pQResult->GetAccID());
		pQuery->SetPassword(pQResult->GetPassword());
		pQuery->SetChannelUID(pQResult->GetChannelUID());
		pQuery->SetZoneID(pQResult->GetZoneID());
		pQuery->SetKey(pQResult->GetKey());
		pQuery->SetLoginType(pQResult->GetLoginType());
		pQuery->SetTerminalType(pQuery->GetTerminalType());
		g_MapServer.MapDBQuery(pQuery);
	}
}

// �����˺Ž��
void DBResultHandler::ProcessCreateAccountQueryResult(QueryResult * pData)
{
	CreateAccountQuery* pQResult = (CreateAccountQuery*)pData;

	// ���ݿ����ʧ��
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "���ݿⴴ���˺�ʧ��[AccountName:%s,Password:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
		return;
	}

	// �ظ������ʺ� todo::�������ӵĵ�����
	if (AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID()))
	{
		// �ظ������ʺ�
		MAPLOG(GUI_LEVEL, "�ظ������ʺ�[AccountName:%s,Password:%s]",
			pQResult->GetAccID(), pQResult->GetPassword());
		return;	
	}

	//����AccountInfo��Ϣ
	AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
	if (!pNewInfo)
	{
		MAPLOG(ERROR_LEVEL, "�ڴ治�㣬�޷�����AccountInfo��Ϣ");
		return;
	}

	pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(), pQResult->GetPassword(),
		pQResult->GetKey(), pQResult->GetChannelUID(), pQResult->GetTerminalType(), NULL);
	pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

	//�����˺Ź�����
	AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

	// ��¼��Ӧ
	MSG_S2C_LOGIN_ACCOUNT_RESP msg;
	//msg.m_dwVersion = g_MapServer.GetClientVersion();
	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount) - 1, pQResult->GetAccID());
	msg.m_byIsBindMobile = strlen(pQResult->GetMobile()) ? true : false;
	msg.m_wLastLoginServerNum = 0;
	memset(msg.m_LastLoginServerList, 0, sizeof(msg.m_LastLoginServerList));
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// ͳ������˺�ע��
	//CGameLogic::Instance()->OnRegisterStatistics(pQResult->GetChannelUID(), pQResult->GetAccID(), pQResult->GetZoneID());
}

// �����˺Ž��(sdk)
void DBResultHandler::ProcessSDKCreateAccountResult(QueryResult * pData)
{
	CreateAccountQuery* pQResult = (CreateAccountQuery*)pData;

	// ���ݿ����ʧ��
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "���ݿⴴ���˺�ʧ��[AccountName:%s,Password:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
		return;
	}

	// �ظ������ʺ� todo::�������ӵĵ�����
	if (AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID()))
	{
		// �ظ������ʺ�
		MAPLOG(GUI_LEVEL, "�ظ������ʺ�[AccountName:%s,Password:%s]",
			pQResult->GetAccID(), pQResult->GetPassword());
		return;
	}

	//����AccountInfo��Ϣ
	AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
	if (!pNewInfo)
	{
		MAPLOG(ERROR_LEVEL, "�ڴ治�㣬�޷�����AccountInfo��Ϣ");
		return;
	}

	pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetKey(), 
		pQResult->GetChannelUID(), pQResult->GetTerminalType(), NULL);
	pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

	//�����˺Ź�����
	AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

	// ��¼��Ӧ
	MSG_S2C_SDK_LOGIN_RESP msg;
	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount) - 1, pQResult->GetAccID());
	msg.m_byIsBindMobile = strlen(pQResult->GetMobile()) ? true : false;
	msg.m_wLastLoginServerNum = 0;
	memset(msg.m_LastLoginServerList, 0, sizeof(msg.m_LastLoginServerList));
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// ͳ������˺�ע��
	//CGameLogic::Instance()->OnRegisterStatistics(pQResult->GetChannelUID(), pQResult->GetAccID(), pQResult->GetZoneID());
}

//��ѯ������ɫ���
void DBResultHandler::ProcessSelectZoneRoleQueryResult(QueryResult* pData)
{
	ENTER_DUMP_FUNCTION

 	SelectCharListQuery* pQResult = (SelectCharListQuery*)pData;

	// ���ݿ����ʧ��
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ݿ��ѯ��ɫ�б�ʧ��[AccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
		return;
	}
 
 	DWORD dwUserKey		= pQResult->GetKey();
 	//DWORD  wResultNum	= pQResult->GetResultRowNum();
 
 	//�������
 	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwUserKey);
 	if(!pAccInfo)
 	{
		MAPLOG(ERROR_LEVEL, "��ѯ�ʻ���ɫ�б���ʱ�Ҳ����ʺ���Ϣ��GUID=%d", dwUserKey);
 		return;
 	}

	const PlayerBaseData *pPlayerBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(
		pQResult->GetZoneID(), pQResult->GetPlatform(), pQResult->GetAccID());

	MSG_S2C_LOGIN_SELECT_SERVER_RESP msg;
	msg.m_byHaveRole = pPlayerBaseData ? true : false;
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// �н�ɫ
	if (pPlayerBaseData)
	{
		DWORD dwRoleId = pPlayerBaseData->dwRoleId;

		//������ҽ�ɫ����
		Player* pPlayer = (Player*)ObjectFactory::Instance()->AllocPlayer();
		if (!pPlayer)
		{
			MAPLOG(ERROR_LEVEL, "�������ڲ�����,�ڴ治�㣨account=%s", pAccInfo->GetAccID());
			return;
		}

		pPlayer->Create(g_MapServer.GetGateSession(), pAccInfo->GetAccID(), dwRoleId, dwUserKey, pAccInfo);

		//������ҹ�����
		PlayerManager::Instance()->AddPlayer(pPlayer);

		//�����û���ǰ״̬
		pAccInfo->SetUserState(USER_LOAD_STATE);
		//���õ�ǰ������Ϸ�Ľ�ɫID
		pAccInfo->SetCurCharID(dwRoleId);

		//��DB�����ɫ��������Ϸ��Ϣ
		SelectCharInfoQuery* pQuery = SelectCharInfoQuery::ALLOC();
		pQuery->SetIndex(MAP_QUERY_CHAR_INFO);
		pQuery->SetThreadIndex(pAccInfo->GetDBIndex());
		pQuery->SetKey(dwUserKey);
		pQuery->SetCharID(dwRoleId);
		pQuery->SetChannelType(pQResult->GetPlatform());
		g_MapServer.MapDBQuery(pQuery);
	}
	else
	{	
		pAccInfo->SetUserState(USER_READY_CREATE_ROLE_STATE);
		//pAccInfo->SetUserState(USER_CREATE_ROLE_STATE);
	}

	LEAVE_DUMP_FUNCTION
}

//��ѯ������ɫ���
// void DBResultHandler::ProcessSelectZoneRoleQueryResult(QueryResult* pData)
// {
// 	ENTER_DUMP_FUNCTION
// 
// 		SelectCharListQuery* pQResult = (SelectCharListQuery*)pData;
// 
// 	// ���ݿ����ʧ��
// 	if (0 != pQResult->GetQueryResult())
// 	{
// 		MAPLOG(ERROR_LEVEL, "���ݿ��ѯ��ɫ�б�ʧ��[AccountName:%s,Error:%d]",
// 			pQResult->GetAccID(), pQResult->GetQueryResult());
// 		return;
// 	}
// 
// 	DWORD dwUserKey = pQResult->GetKey();
// 	DWORD  wResultNum = pQResult->GetResultRowNum();
// 
// 	//�������
// 	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwUserKey);
// 	if (!pAccInfo)
// 	{
// 		MAPLOG(ERROR_LEVEL, "��ѯ�ʻ���ɫ�б���ʱ�Ҳ����ʺ���Ϣ��GUID=%d", dwUserKey);
// 		return;
// 	}
// 
// 	MSG_S2C_LOGIN_SELECT_SERVER_RESP msg;
// 	msg.m_byHaveRole = wResultNum ? true : false;
// 	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);
// 
// 	// �н�ɫ
// 	if (wResultNum)
// 	{
// 		//������ҽ�ɫ����
// 		Player* pPlayer = (Player*)ObjectFactory::Instance()->AllocPlayer();
// 		if (!pPlayer)
// 		{
// 			MAPLOG(ERROR_LEVEL, "�������ڲ�����,�ڴ治�㣨account=%s", pAccInfo->GetAccID());
// 			return;
// 		}
// 
// 		pPlayer->Create(g_MapServer.GetGateSession(), pAccInfo->GetAccID(), pQResult->GetRoleId(), dwUserKey, pAccInfo);
// 
// 		//������ҹ�����
// 		PlayerManager::Instance()->AddPlayer(pPlayer);
// 
// 		//�����û���ǰ״̬
// 		pAccInfo->SetUserState(USER_LOAD_STATE);
// 		//���õ�ǰ������Ϸ�Ľ�ɫID
// 		pAccInfo->SetCurCharID(pQResult->GetRoleId());
// 
// 		//��DB�����ɫ��������Ϸ��Ϣ
// 		SelectCharInfoQuery* pQuery = SelectCharInfoQuery::ALLOC();
// 		pQuery->SetIndex(MAP_QUERY_CHAR_INFO);
// 		pQuery->SetThreadIndex(pAccInfo->GetDBIndex());
// 		pQuery->SetKey(dwUserKey);
// 		pQuery->SetCharID(pQResult->GetRoleId());
// 		g_MapServer.MapDBQuery(pQuery);
// 	}
// 	else
// 	{
// 
// 		pAccInfo->SetUserState(USER_READY_CREATE_ROLE_STATE);
// 		//pAccInfo->SetUserState(USER_CREATE_ROLE_STATE);
// 	}
// 
// 	LEAVE_DUMP_FUNCTION
// }

//�������½�ɫ�Ľ��
void DBResultHandler::ProcessCreateCharQueryResult(QueryResult* pData)
{
	ENTER_DUMP_FUNCTION

	CreateCharInfoQuery* pQResult = (CreateCharInfoQuery*)pData;

	// ɾ�����ڴ�����ɫ����
	g_MapServer.RemoveCreatingRoleData(pQResult->GetAccID(), pQResult->GetChannelUID(), pQResult->GetZoneID());

	// ���ݿ����ʧ��
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "���ݿⴴ���½�ɫʧ��[AccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
		return;
	}

	DWORD dwUserKey	= pQResult->GetKey();

	//�������
	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwUserKey);
	if(!pAccInfo)
		return;

	// ����
	if (pQResult->IsNameRepeat())
	{
		// ֪ͨ��������ѱ�ʹ��
		g_MapServer.SendErrorMsgToUser(dwUserKey, ERROR_LOGIN_ENAME_EXIST);

		// �����������ص�׼��״̬(��Ϊ�ͻ��˻��ٴη��ʹ�����ɫ)
		pAccInfo->SetUserState(USER_READY_CREATE_ROLE_STATE);
		
#ifdef _DEBUG
		char szCharName[MAX_ROLE_LEN] = { 0 };
		ChangeLocalChar(pQResult->GetRoleName(), szCharName, sizeof(szCharName)-1);
		MAPLOG(GUI_LEVEL, "�����ѱ�ʹ��,������ɫʧ�ܣ�AccountId=%s, RoleName=%s",
			pAccInfo->GetAccID(), szCharName);
#endif
		return;
	}

	// ��ɫ�Ѵ���
	if (pQResult->IsRoleExist())
	{
		// ֪ͨ��������ѱ�ʹ��
		g_MapServer.SendErrorMsgToUser(dwUserKey, ERROR_ROLE_ALREADY_EXIST);
		// �����������ص�׼��״̬(��Ϊ�ͻ��˻��ٴη��ʹ�����ɫ)
		//pAccInfo->SetUserState(USER_READY_CREATE_ROLE_STATE);
		MAPLOG(GUI_LEVEL, "�ظ�������ɫ��AccountId=%s, server_id=%d",pAccInfo->GetAccID(), pQResult->GetZoneID());
		return;
	}

	//������ҽ�ɫ����
	Player* pPlayer = (Player*)ObjectFactory::Instance()->AllocPlayer();
	if (!pPlayer)
	{
		MAPLOG(GUI_LEVEL, "�������ڲ�����,�ڴ治�㣨account=%s", pAccInfo->GetAccID());
		return;
	}

	pPlayer->Create(g_MapServer.GetGateSession(), pAccInfo->GetAccID(), pQResult->GetCharID(), dwUserKey, pAccInfo);

	//������ҹ�����
	PlayerManager::Instance()->AddPlayer(pPlayer);

	//�����û���ǰ״̬
	pAccInfo->SetUserState(USER_LOAD_STATE);
	//���õ�ǰ������Ϸ�Ľ�ɫID
	pAccInfo->SetCurCharID(pQResult->GetCharID());

	//��DB�����ɫ��������Ϸ��Ϣ
	SelectCharInfoQuery* pQuery = SelectCharInfoQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_CHAR_INFO);
	pQuery->SetThreadIndex(pAccInfo->GetDBIndex());
	pQuery->SetKey(dwUserKey);
	pQuery->SetCharID(pQResult->GetCharID());
	pQuery->SetChannelType(pQResult->GetChannelUID());
	g_MapServer.MapDBQuery(pQuery);

	MSG_S2C_LOGIN_CREATE_ROLE_RESP msg;
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// ͳ������˺�ע��
	CGameLogic::Instance()->OnRegisterStatistics(pAccInfo->GetChannelUID(), pQResult->GetAccID(), pQResult->GetZoneID());

	// ͳ����Ҵ�����ɫ
	CGameLogic::Instance()->OnGameStatistics_CreateRole(pQResult->GetCharID(), pQResult->GetZoneID());

	LEAVE_DUMP_FUNCTION
}

//�����ѯ��ɫ��Ϣ�Ľ��
void DBResultHandler::ProcessSelectCharInfoQueryResult(QueryResult* pData)
{
	SelectCharInfoQuery* pQResult = (SelectCharInfoQuery*)pData;
	DWORD dwUserKey		= pQResult->GetKey();
	BYTE bySelectType = pQResult->GetSelectType();

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwUserKey);
	if (!pPlayer)
	{
		MAPLOG(GUI_LEVEL, "��ѯ��ɫ����Ϣ����ʱ�����������[SelectType:%d]", bySelectType);
		return;
	}

	// ������ݿ�����Ƿ�ɹ� happiness success, money
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "��ѯ��ɫ����Ϣʧ��[SelectType:%d]", bySelectType);
		return;
	}

	// ��¼��ѯ
	if (!bySelectType)
	{
		//��ұ����Ǽ���״̬�ſ���
		AccountInfo* pAccInfo = pPlayer->GetAccountInfo();
		if (!pAccInfo)
		{
			MAPLOG(ERROR_LEVEL, "��ѯ��ɫ����Ϣ����ʱ���Ҳ�����ɫ���ʺ�����");
			return;
		}

		if (pAccInfo->GetUserState() != USER_LOAD_STATE)
		{
			MAPLOG(ERROR_LEVEL, "��ѯ��ɫ����Ϣ����ʱ�û�״̬����[UserState:%d]", pAccInfo->GetUserState());
			return;
		}

		//���õ�ǰ�û�״̬
		pAccInfo->SetUserState(USER_AT_GAME_STATE);

		//������Ҽ����¼�
		pPlayer->OnLoad(pQResult->GetRoleFullInfo());

		//������ҵ�¼�¼�
		pPlayer->OnLogin();

		// ������������¼�ķ�����״̬
		{
			UpdateLastLoginServer* pQuery = UpdateLastLoginServer::ALLOC();
			pQuery->SetIndex(MAP_UPDATE_LAST_LOGIN_SERVER);
			pQuery->SetThreadIndex(0);
			pQuery->SetKey(pPlayer->GetGuid());
			pQuery->SetAccID(pPlayer->GetAccID());
			pQuery->SetServerID(pAccInfo->GetZoneID());
			pQuery->SetChannelType(pPlayer->GetChannelType());
			g_MapServer.MapDBQuery(pQuery);
		}
	}
	// �߼���ѯ
	else
	{
		pPlayer->SelectTargetDetailsCallBack(pQResult->GetRequestType(), &pQResult->GetRoleFullInfo());
	}
}

//������½�ɫ��Ϣ�Ľ��
void DBResultHandler::ProcessUpdateCharQueryResult(QueryResult* pData)
{
	UpdateRoleInfoQuery* pQResult = (UpdateRoleInfoQuery*)pData;

	DWORD dwUserKey	= pQResult->GetKey();

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwUserKey);
	if(!pPlayer)
		return;

	if (pQResult->GetQueryResult() != 0)
	{
		MAPLOG(ERROR_LEVEL, "��������ʧ��CharID=%u", pPlayer->GetCharID());
	}

	//���ߴ���
	if(pQResult->GetSaveType() == SAVE_DB_TYPE_LOGOUT)
	{
		AccountInfoManager::Instance()->ProcessDBLogoutAck(pPlayer->GetAccountInfo());
	}
}

// �ʺ������޸Ľ��
void DBResultHandler::ProcessAccountPasswordModifyResult(QueryResult * pData)
{
	AccountPasswordModify* pQResult = (AccountPasswordModify*)pData; 
	if (0 == pQResult->GetQueryResult())
	{
		
		MAPLOG(GUI_LEVEL,"�޸��ʺ�����ɹ�![AcccountName:%s,Password:%s]",
			pQResult->GetAccID(), pQResult->GetPassword());
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "�޸��ʺ�����ʧ��![AcccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
	}
}

// �ʺ��ֻ����޸Ľ��
void DBResultHandler::ProcessAccountMobileChangeResult(QueryResult * pData)
{
	AccountMobileModify* pQResult = (AccountMobileModify*)pData;
	if (0 == pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "�޸��ֻ��ųɹ�![account:%s,mobile:%s]",pQResult->GetAccID(), pQResult->GetMobile());
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "�޸��ֻ���ʧ��![account:%s,Error:%d]", pQResult->GetAccID(), pQResult->GetQueryResult());
	}
}

// ���������¼������
void DBResultHandler::ProcessUpdateLastLoginServer(QueryResult * pData)
{
	UpdateLastLoginServer* pQResult = (UpdateLastLoginServer*)pData;
	if (0 == pQResult->GetQueryResult())
	{

		MAPLOG(GUI_LEVEL, "���������¼�������ɹ�![AcccountName:%s,ServerId:%d]", 
			pQResult->GetAccID(), pQResult->GetServerID());
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "���������¼������ʧ��![AcccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
	}
}

// �洢ȫ������
void DBResultHandler::SaveGolbalDataResult(QueryResult * pData)
{
	ServerGolbalData_Save* pQResult = (ServerGolbalData_Save*)pData; 
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢ȫ������ʧ��![Error:%d]", pQResult->GetQueryResult());
	}	
}

// ����ȫ������
void DBResultHandler::LoadGolbalDataResult(QueryResult * pData)
{
	ServerGolbalData_Load* pQResult = (ServerGolbalData_Load*)pData; 
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "����ȫ������ʧ��![Error:%d]", pQResult->GetQueryResult());
		return;
	}	

	const ServerGolbalData *pGolbalDataList = pQResult->GetGolbalDataList();
	BYTE byGolbalDataNum = pQResult->GetGolbalDataNum();

	for (BYTE i = 0; i < byGolbalDataNum; ++i)
	{
		const ServerGolbalData &tData = pGolbalDataList[i];
		g_MapServer.LoadGolbalDataCallback(tData.wServerId, tData.nKey, tData.dwValue);
	}

	MAPLOG(GUI_LEVEL, "����ȫ�����ݳɹ�!");
}

// ���ؽ�ɫ���������б�
void DBResultHandler::LoadRoleBaseDataListResult(QueryResult * pData)
{
	RoleBaseDataList_Load* pQResult = (RoleBaseDataList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ؽ�ɫ���������б�ʧ��![Error:%d]", pQResult->GetQueryResult());
	}

	WORD wListNum = pQResult->GetListNum();
	DBPlayerBaseData *pList = pQResult->GetList();

	DWORD dwMinRoleId = 0;
	if (wListNum)
	{
		PlayerBaseData tBaseData;
		for (WORD i = 0; i < wListNum; ++i)
		{
			DBPlayerBaseData *pDBBaseData = &pList[i];
			tBaseData.Init();
			memcpy_s(&tBaseData, sizeof(PlayerCommonData), pDBBaseData, sizeof(PlayerCommonData));
			//tBaseData.dwOfflineTime = pList[i].dwOfflineTime;
			for (BYTE k = 0; k < pDBBaseData->byToBattleHeroNum; ++k)
			{
				tBaseData.vToBattleHeroList.push_back(pDBBaseData->arToBattleHeroList[k]);
			}
			PlayerBaseDataManager::Instance()->AddPlayerBaseData(pDBBaseData->wServerId, tBaseData);

			// ��¼���μ��ص����ID
			dwMinRoleId = max(dwMinRoleId, pDBBaseData->dwRoleId);
		}

		DWORD dwCurPlayerNum = PlayerBaseDataManager::Instance()->GetTotalPlayerNum();
		DWORD dwTotalPlayerNum = pQResult->GetTotalDataNum();

		MAPLOG(ERROR_LEVEL, "load role info[cur_role:%d,total_role:%d]", dwCurPlayerNum, dwTotalPlayerNum);

		RoleBaseDataList_Load* pQuery = RoleBaseDataList_Load::ALLOC();
		pQuery->SetIndex(DB_ROLE_BASE_DATA_LIST_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_LOAD_ROLE_INFO_LIST);
		pQuery->SetMinRoleId(dwMinRoleId);
		pQuery->SetTotalDataNum(pQResult->GetTotalDataNum());
		g_MapServer.MapDBQuery(pQuery);
	}
	// �Ѿ�ľ�����ݿɶ�ȡ
	else
	{
		DWORD dwTotalPlayerNum = PlayerBaseDataManager::Instance()->GetTotalPlayerNum();
		if (dwTotalPlayerNum == pQResult->GetTotalDataNum())
		{
			MAPLOG(IMPORTANT_LEVEL, "Load role basedata success![num:%d]", dwTotalPlayerNum);

			// ��Ϊ�߼��������������ģ�������,���ڱ�������ģ������ݶ�������������
			//CKingMatchManager::Instance()->LoadDataRequest();
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "���ؽ�ɫ�������ݵ��ڴ������ݿ��������ƥ��![db_num:%d, mem_num:%d]",
				pQResult->GetTotalDataNum(), dwTotalPlayerNum);
		}

		// ������
		g_MapServer.SetIsLoadRoleBaseData(true);
	}
}

// ���º�������
void DBResultHandler::UpdateFriendDataResult(QueryResult * pData)
{
	FriendDataUpdate_Load* pQResult = (FriendDataUpdate_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "���º�������ʧ��![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(pQResult->GetUpdateRoleId());
	if (pPlayer)
	{
		pPlayer->GetFriendManager()->UpdateFriendDataCallback(pQResult->GetOperateType(), pQResult->GetFriendData());
	}
	else
	{
		
	}

	MAPLOG(GUI_LEVEL, "�ɹ����º�������![type:%d]", pQResult->GetOperateType());
}

// �洢���������
void DBResultHandler::SaveArenaDataResult(QueryResult * pData)
{
	ArenaDataList_Save* pQResult = (ArenaDataList_Save*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢����������ʧ��![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢����������!");
}

// ���ؾ��������ݽ��
void DBResultHandler::LoadArenaDataResult(QueryResult * pData)
{
	ArenaDataList_Load* pQResult = (ArenaDataList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ؾ���������ʧ��![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	CArenaManager::Instance()->LoadDataFromDB(pQResult->GetServerId(), pQResult->GetDataBuff(), pQResult->GetDataBuffSize());
}

// �洢��ɫ���а����ݽ��
void DBResultHandler::SaveRoleRankDataResult(QueryResult * pData)
{
	RoleRankList_Save* pQResult = (RoleRankList_Save*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "Save rank data fail![RankType:%d,Error:%d]", pQResult->GetRankType(), pQResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "Save rank data success[RankType:%d]!", pQResult->GetRankType());
}

// ���ؽ�ɫ���а����ݽ��
void DBResultHandler::LoadRoleRankDataResult(QueryResult * pData)
{
	RoleRankList_Load* pQResult = (RoleRankList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "Load rank data fail![RankType:%d,Error:%d]", pQResult->GetRankType(), pQResult->GetQueryResult());
		return;
	}

	CRankManager::Instance()->LoadDataFromDB(pQResult->GetRankType(), pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "Load rank data success[rank_type:%d,size:%d]!", pQResult->GetRankType(), pQResult->GetDataBuffSize());
}

// �洢�ʼ�����
void DBResultHandler::SaveEmailDataResult(QueryResult * pData)
{
	InsertEmailQuery* pQResult = (InsertEmailQuery*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�ʼ�����ʧ��![RoleId:%d,Error:%d]", pQResult->GetEmailInfo().dwID, pQResult->GetQueryResult());
		return;
	}

	//MAPLOG(GUI_LEVEL, "�洢�ʼ��ɹ�!");
}

// ���ز�ɾ��GM�¼����
void DBResultHandler::LoadAndDeleteGMEventResult(QueryResult * pData)
{
	GMEventList_Load* pQResult = (GMEventList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ز�ɾ��GM�¼�����ʧ��![Error:%d]", pQResult->GetQueryResult());
		return;
	}

// 	if (pQResult->GetEventNum())
// 		MAPLOG(ERROR_LEVEL, "���GM�¼�[EventNum:%d]!", pQResult->GetEventNum());

	for (int i = 0; i < pQResult->GetEventNum(); ++i)
	{
		const GMEventData *pEventData = pQResult->GetEventData(i);
		if (pEventData)
		{
			GMManager::Instance()->OnCmdFromDB(pEventData->wEventId, pEventData->arParamList, 10, pEventData->szText, pEventData->szText1);
		}
		//MAPLOG(ERROR_LEVEL, "����GM�¼�[idx:%d]!", i);
	}
}

// ���ط������б���
void DBResultHandler::LoadServerListResult(QueryResult * pData)
{
	ServerList_Load* pQResult = (ServerList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ط������б�ʧ��![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	g_MapServer.LoadServerList(pQResult->GetServerList(), pQResult->GetServerNun());
}

// �洢������ս����¼���ݽ��
void DBResultHandler::SaveArenaBattleRecordResult(QueryResult * pData)
{
	ArenaBattleRecord_Save* pResult = (ArenaBattleRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢������ս����¼����ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// ���ؾ��������ݽ��
void DBResultHandler::LoadArenaBattleRecordResult(QueryResult * pData)
{
	ArenaBattleRecord_Load* pResult = (ArenaBattleRecord_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ؾ�����ս����¼����ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	const PlayerArenaBattleRecord *pPlayerBattleRecordList = pResult->GetPlayerBattleRecordList();
	int nPlayerBattleRecrodNum = pResult->GetPlayerBattleRecordNum();

	DWORD dwMinRoleId = 0;
	if (nPlayerBattleRecrodNum)
	{
		for (int i = 0; i < nPlayerBattleRecrodNum; ++i)
		{
			CArenaManager::Instance()->AddPlayerBattleRecord(&pPlayerBattleRecordList[i]);

			// ��¼���μ��ص����ID
			dwMinRoleId = max(dwMinRoleId, pPlayerBattleRecordList[i].dwRoleId);
		}

		ArenaBattleRecord_Load* pQuery = ArenaBattleRecord_Load::ALLOC();
		pQuery->SetIndex(DB_ARENA_BATTLE_RECORD_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_ARENA_BATTLE_RECORD);
		pQuery->SetMinRoleId(dwMinRoleId);
		pQuery->SetTotalBattleRecordNum(pResult->GetTotalBattleRecordNum());
		g_MapServer.MapDBQuery(pQuery);
	}
	// �Ѿ�ľ�����ݿɶ�ȡ
	else
	{
		int nTotalPlayerBattleRecordNum = CArenaManager::Instance()->GetPlayerBattleRecordNum();
		if (nTotalPlayerBattleRecordNum == pResult->GetTotalBattleRecordNum())
		{
			MAPLOG(GUI_LEVEL, "�ɹ����ؾ�����ս����¼�б�![num:%d]", nTotalPlayerBattleRecordNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "���ؾ�����ս����¼���ڴ������ݿ��������ƥ��![db_num:%d, mem_num:%d]",
				pResult->GetTotalBattleRecordNum(), nTotalPlayerBattleRecordNum);
		}
	}
}

// �洢��ɫ������־���
void DBResultHandler::SaveRoleCurrencyLogsResult(QueryResult * pData)
{
	RoleCurrencyLogs_Save* pResult = (RoleCurrencyLogs_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��ɫ������־ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// �洢������ݽ��
void DBResultHandler::SaveLandDataResult(QueryResult * pData)
{
	LandData_Save* pResult = (LandData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// ����������ݽ��
void DBResultHandler::LoadLandDataResult(QueryResult * pData)
{
	LandData_Load* pResult = (LandData_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�����������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	WORD wLandNum = pResult->GetLandListNum();
	int nTotalLandNum = pResult->GetTotalLandNum();
	const PlayerLandInfo *pLandList = pResult->GetLandList();
	const DWORD *pRoleIdList = pResult->GetRoleIdList();

	DWORD dwMinRoleId = 0;
	if (wLandNum > 0)
	{
		for (int i = 0; i < wLandNum; ++i)
		{
			CLandManager::Instance()->LoadDataFromDB(pRoleIdList[i], pLandList[i]);

			// ��¼���μ��ص����ID
			dwMinRoleId = max(dwMinRoleId, pRoleIdList[i]);
		}

		LandData_Load* pQuery = LandData_Load::ALLOC();
		pQuery->SetIndex(DB_LAND_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_LAND);
		pQuery->SetMinRoleId(dwMinRoleId);
		pQuery->SetTotalLandNum(nTotalLandNum);
		g_MapServer.MapDBQuery(pQuery);
	}
	// �Ѿ�ľ�����ݿɶ�ȡ
	else
	{
		int nLandListSize = CLandManager::Instance()->GetLandListSize();
		if (nLandListSize == nTotalLandNum)
		{
			MAPLOG(GUI_LEVEL, "�ɹ������������![num:%d]", nLandListSize);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "����������ݵ��ڴ������ݿ��������ƥ��![db_num:%d, mem_num:%d]",
				nTotalLandNum, nLandListSize);
		}
	}
}

// ���������а�鿴�б���½��
void DBResultHandler::UpdateArenaRankViewListResult(QueryResult * pData)
{
	LandData_Load* pResult = (LandData_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���¾��������а�鿴�б�ʧ��![Error:%d]", pResult->GetQueryResult());
	}
	else
	{
		MAPLOG(GUI_LEVEL, "���¾��������а�鿴�б�ɹ�!");
	}
}

// ������Խ��
void DBResultHandler::SaveLeaveMessageResult(QueryResult * pData)
{
	LeaveMessage_Save* pResult = (LeaveMessage_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��������ʧ��![Error:%d]", pResult->GetQueryResult());
	}
}

// �洢�����������ݽ��
void DBResultHandler::SaveRewardCenterDataResult(QueryResult * pData)
{
	RewardCenterData_Save* pResult = (RewardCenterData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢������������ʧ��![Error:%d,role_id:%d]", 
			pResult->GetQueryResult(), pResult->GetRoleId());
		return;
	}

	PlayerBaseDataManager::Instance()->RemoveReward(pResult->GetServerId(), pResult->GetRoleId(), pResult->GetRewardData().dwUId);
}

// ���³ƺ����ݽ��
void DBResultHandler::UpdateRoleTempDataResult(QueryResult * pData)
{
	RoleTmpData_Update* pResult = (RoleTmpData_Update*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���½�ɫ��ʱ����ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "���½�ɫ��ʱ���ݳɹ�!");
}

// �洢��ɫ���߼�¼���
void DBResultHandler::SaveRoleOfflineRecordResult(QueryResult * pData)
{
	RoleOfflineRecord_Save* pResult = (RoleOfflineRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��ɫ���߼�¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�洢��ɫ���߼�¼�ɹ�!");
}

// �洢��ɫ�ȼ���¼���
void DBResultHandler::SaveRoleLevelRecordResult(QueryResult * pData)
{
	RoleLevelRecord_Save* pResult = (RoleLevelRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��ɫ�ȼ���¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�洢��ɫ�ȼ���¼�ɹ�!");
}

// �洢��ɫ��ֵ��¼���
void DBResultHandler::SaveRoleRechargeRecordResult(QueryResult * pData)
{
	RoleRechargeRecord_Save* pResult = (RoleRechargeRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��ɫ��ֵ��¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�洢��ɫ��ֵ��¼�ɹ�!");
}

// �洢�ؿ�ʧ�ܼ�¼���
void DBResultHandler::SaveRoleMapLoseRecordResult(QueryResult * pData)
{
	RoleMapLoseRecord_Save* pResult = (RoleMapLoseRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�ؿ�ʧ�ܼ�¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	//MAPLOG(GUI_LEVEL, "�洢�ؿ�ʧ�ܼ�¼�ɹ�!");
}

// �洢�ؿ�ʧ�ܼ�¼���
void DBResultHandler::SaveRoleFirstPassMapRecordResult(QueryResult * pData)
{
	RoleFirstPassMapRecord_Save* pResult = (RoleFirstPassMapRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�״�ͨ�ؼ�¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	//MAPLOG(GUI_LEVEL, "�洢�״�ͨ�ؼ�¼�ɹ�!");
}

// �洢�̵깺���¼���
void DBResultHandler::SaveShopBuyRecordResult(QueryResult * pData)
{
	ShopBuyRecord_Save* pResult = (ShopBuyRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�̵깺���¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// �洢��Ʒ��ʧ��¼���
void DBResultHandler::SaveItemLoseRecordResult(QueryResult * pData)
{
	ItemLoseRecord_Save* pResult = (ItemLoseRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��Ʒ��ʧ��¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// �洢��Ʒ������¼���
void DBResultHandler::SaveItemOperationRecordResult(QueryResult * pData)
{
	ItemOperationRecord_Save* pResult = (ItemOperationRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��Ʒ������¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// �洢�������������¼���
void DBResultHandler::SaveOfflinePlayerNumRecordResult(QueryResult * pData)
{
	OnlinePlayerNumRecord_Save* pResult = (OnlinePlayerNumRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�������������¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}


// �洢�������ݽ��
void DBResultHandler::SaveGuildDataResult(QueryResult * pData)
{
	GuildData_Save* pResult = (GuildData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�洢�������ݳɹ�![GuildId:%d]", pResult->GetKey());
}

// ���ع������ݽ��
void DBResultHandler::LoadGuildDataResult(QueryResult * pData)
{
	GuildData_Load* pResult = (GuildData_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ع�������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	int nGuildNum = pResult->GetGuildNum();
	int nTotalGuildNum = pResult->GetTotalGuildNum();
	const GuildDataBuff *pGuildList = pResult->GetGuildList();

	DWORD dwMinGuildId = 0;
	if (nGuildNum)
	{
		for (int i = 0; i < nGuildNum; ++i)
		{
			const GuildDataBuff *pData = &pGuildList[i];

			CGuildManager::Instance()->LoadDataFromDB(pData->szBuff, pData->nBuffSize);
			// ��¼���μ��ص����ID
			dwMinGuildId = max(dwMinGuildId, pData->dwGuildId);
		}

		GuildData_Load* pQuery = GuildData_Load::ALLOC();
		pQuery->SetIndex(DB_GUILD_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_GUILD);
		pQuery->SetMinGuildId(dwMinGuildId);
		pQuery->SetTotalGuildNum(nTotalGuildNum);
		pQuery->SetKey(pResult->GetKey());
		g_MapServer.MapDBQuery(pQuery);
	}
	// �Ѿ�ľ�����ݿɶ�ȡ
	else
	{
		int nAlreadyLoadGuildNum = CGuildManager::Instance()->GetGuildNum();
		if (nAlreadyLoadGuildNum == nTotalGuildNum)
		{
			MAPLOG(GUI_LEVEL, "�ɹ����ع����б�![num:%d]", nTotalGuildNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "���ع����б����![db_num:%d, mem_num:%d]", nTotalGuildNum, nAlreadyLoadGuildNum);
		}
		MAPLOG(GUI_LEVEL, "�����ϵ��ID[id:%d]", CGuildManager::Instance()->GetSeriesId());
	}
}

// �洢�����¼
void DBResultHandler::SaveChatRecordResult(QueryResult * pData)
{
	ChatRecord_Save* pResult = (ChatRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�����¼ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// ���������¼
void DBResultHandler::LoadChatRecordResult(QueryResult * pData)
{
	
}

// �洢��־���
void DBResultHandler::SaveLogsResult(QueryResult * pData)
{
	Log_Save* pResult = (Log_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��־ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// �洢�ؿ����۽��
void DBResultHandler::SaveMapCommentResult(QueryResult * pData)
{
	MapComment_Save* pResult = (MapComment_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢�ؿ���������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// ���عؿ����۽��
void DBResultHandler::LoadMapCommentResult(QueryResult * pData)
{
	MapComment_Load* pResult = (MapComment_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���عؿ���������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	WORD wListNum = pResult->GetListNum();
  	const MapComments *pList = pResult->GetList();

	DWORD dwMinId = 0;
	if (wListNum)
	{
		//PlayerBaseData tBaseData;
		for (WORD i = 0; i < wListNum; ++i)
		{
			const MapComments *pComments = &pList[i];	
			g_MapServer.LoadMapCommentData(pComments->dwId, pComments);

			// ��¼���μ��ص����ID
			dwMinId = max(dwMinId, pComments->dwId);
		}

		MapComment_Load* pQuery = MapComment_Load::ALLOC();
		pQuery->SetIndex(DB_MAP_COMMENT_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_MAP_COMMENT);
		pQuery->SetMinId(dwMinId);
		pQuery->SetTotalDataNum(pResult->GetTotalDataNum());
		g_MapServer.MapDBQuery(pQuery);
	}
	// �Ѿ�ľ�����ݿɶ�ȡ
	else
	{
		DWORD dwTotalCommentNum = g_MapServer.GetMapCommentNum();
		if (dwTotalCommentNum == pResult->GetTotalDataNum())
		{
			MAPLOG(GUI_LEVEL, "�ɹ����عؿ������б�![num:%d]", dwTotalCommentNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "���عؿ��������ݵ��ڴ������ݿ��������ƥ��![db_num:%d, mem_num:%d]",
				pResult->GetTotalDataNum(), dwTotalCommentNum);
		}
	}
}

// �����ʺŽ��
void DBResultHandler::UpdateAccountLockResult(QueryResult * pData)
{
	AccountLock_Update* pResult = (AccountLock_Update*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�����ʺ���ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// �洢���������ݽ��
void DBResultHandler::SaveKingMatchDataResult(QueryResult * pData)
{
	KingMatchData_Save* pResult = (KingMatchData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢����������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢����������!");
}

// �������������ݽ��
void DBResultHandler::LoadKingMatchDataResult(QueryResult * pData)
{
	KingMatchData_Load* pQResult = (KingMatchData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "��������������ʧ��![ErrorId:%d,ErrorMsg:%s]", pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CKingMatchManager::Instance()->LoadDataFromDB(pQResult->GetCurSeason(), pQResult->GetDataBelongSeason(),
		pQResult->GetDataBuff(), pQResult->GetDataBuffSize());


	MAPLOG(GUI_LEVEL, "�����ݿ�ɹ���������������[cur_season:%d,data_belong_season:%d]!",
		pQResult->GetCurSeason(), pQResult->GetDataBelongSeason());
}

// �洢���������ݽ��
void DBResultHandler::SaveScoreMatchDataResult(QueryResult * pData)
{
	ScoreMatchData_Save* pResult = (ScoreMatchData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢����������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢����������!");
}

// ���ػ��������ݽ��
void DBResultHandler::LoadScoreMatchDataResult(QueryResult * pData)
{
	ScoreMatchData_Load* pQResult = (ScoreMatchData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ػ���������ʧ��![ErrorId:%d,ErrorMsg:%s]", pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	DWORD dwActivityIdx = pQResult->GetCurActivityIndex();
	BYTE byActivityStatus = pQResult->GetActivityStatus();
	DWORD dwOverTime = pQResult->GetActivityOverTime();

	CScoreMatchManager::Instance()->LoadDataFromDB(dwActivityIdx,byActivityStatus,dwOverTime,pQResult->GetDataBuff(),pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "�����ݿ�ɹ����ػ���������[ActivityIdx:%d,ActivityStatus:%d]!",dwActivityIdx, byActivityStatus);
}

// �洢��Ӹ������ݽ��
void DBResultHandler::SaveTeamDungeonRankDataResult(QueryResult * pData)
{
	TeamDungeonRankData_Save* pResult = (TeamDungeonRankData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��Ӹ�����������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢��Ӹ�����������!");
}

// ������Ӹ����������ݽ��
void DBResultHandler::LoadTeamDungeonRankDataResult(QueryResult * pData)
{
	TeamDungeonRankData_Load* pQResult = (TeamDungeonRankData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "������Ӹ�����������ʧ��![ErrorId:%d,ErrorMsg:%s]",
			pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CTeamDungeonManager::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "�����ݿ�ɹ�������Ӹ�����������[data_size:%d]!", pQResult->GetDataBuffSize());
}

// �洢ת�����ݽ��
void DBResultHandler::SaveTurntableDataResult(QueryResult * pData)
{
	TurntableData_Save* pResult = (TurntableData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢ת������ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢ת������!");
}

// ����ת�����ݽ��
void DBResultHandler::LoadTurntableDataResult(QueryResult * pData)
{
	TurntableData_Load* pQResult = (TurntableData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "����ת������ʧ��![ErrorId:%d,ErrorMsg:%s]",
			pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CTurntableManager::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "�����ݿ�ɹ�����ת������[data_size:%d]!", pQResult->GetDataBuffSize());
}

// ���������������
void DBResultHandler::UpdateNewRoleGuideResult(QueryResult * pData)
{
	NewRoleGuide_Update* pQResult = (NewRoleGuide_Update*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���������������ʧ��![ErrorId:%d,ErrorMsg:%s]",
			pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}
}

// �洢��Դս���ݽ��
void DBResultHandler::SaveResourceWarDataResult(QueryResult * pData)
{
	ResourceWarData_Save* pResult = (ResourceWarData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢��Դս����ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢��Դս����!");
}

// ������Դս���ݽ��
void DBResultHandler::LoadResourceWarDataResult(QueryResult * pData)
{
	ResourceWarData_Load* pQResult = (ResourceWarData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "������Դս����ʧ��![ErrorId:%d,ErrorMsg:%s]", pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CResourceWarManager::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "�����ݿ�ɹ�������Դս����[DataSize:%d]!", pQResult->GetDataBuffSize());
}

// �洢����ս���ݽ��
void DBResultHandler::SaveGuildWarDataResult(QueryResult * pData)
{ 
	GuildWarData_Save* pResult = (GuildWarData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢����ս����ʧ��![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "�ɹ��洢����ս����!");
}

// ���ع���ս���ݽ��
void DBResultHandler::LoadGuildWarDataResult(QueryResult * pData)
{
	GuildWarData_Load* pQResult = (GuildWarData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���ع���ս����ʧ��![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}

	CGuildWarMgr::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "�����ݿ�ɹ����ع���սս����[DataSize:%d]!", pQResult->GetDataBuffSize());
}

// ��������������
void DBResultHandler::SelectActivityCodeResult(QueryResult * pData)
{
	ActivationCode_Select* pQResult = (ActivationCode_Select*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "��������������ʧ��![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByRoleId(pQResult->GetRoleID());
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "�������������ݻص�ʱ�Ҳ����������![RoleId:%d]", pQResult->GetRoleID());
		return;
	}

	pPlayer->GetLogicManager()->OnTakeActivityGiftCallback(pQResult->GetActivationCode(), pQResult->GetResult(),
		pQResult->GetStatus(), pQResult->GetRewardId(), pQResult->GetExpireTime(), pQResult->GetBatch(), pQResult->GetType());
}

// ���¼��������ݽ��
void DBResultHandler::UpdateActivityCodeResult(QueryResult * pData)
{
	ActivationCode_Update* pQResult = (ActivationCode_Update*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "���¼���������ʧ��![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}
}

// ��Ϸͳ���¼����
void DBResultHandler::GameStatisticsEventResult(QueryResult * pData)
{
	CGameStatistics_Event* pQResult = (CGameStatistics_Event*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "��Ϸͳ���¼�ʧ��![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}
}

// �洢GM������¼
void DBResultHandler::GMOperationRecordSaveResult(QueryResult * pData)
{
	CGMOperationRecord_Save* pQResult = (CGMOperationRecord_Save*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "�洢GM������¼ʧ��![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}
}