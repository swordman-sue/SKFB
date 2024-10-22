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

// 检测账号结果
void DBResultHandler::ProcessCheckAccountQueryResult(QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION 
 	CheckAccountQuery* pQResult = (CheckAccountQuery*)pData; 
 
 	// 数据库操作失败
 	if (0 != pQResult->GetQueryResult())
 	{
 		MAPLOG(GUI_LEVEL,"请求检测帐号数据库操作失败[Account:%s,Password:%s,Error:%d]",
 			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
 		return;
 	}
 
	AccountInfo* pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID());
 	if (pAccountInfo)
 	{
		// 重复登录
 		MAPLOG(GUI_LEVEL,"重复登录[Account:%s]", pQResult->GetAccID());
 		return;
 	}

	// 检测账号是否被锁
	if (pQResult->IsLock() && (!pQResult->GetUnlockTime() || time(NULL) < pQResult->GetUnlockTime()))
	{
		g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_ACCOUNT_LOCK);
		return;
	}

	// 帐号已存在
	if (pQResult->GetResultRowNum())
	{
		// 手机注册不需要验证密码
		if (LOGIN_MOBILE_REGISTER != pQResult->GetLoginType() && strcmp(pQResult->GetOldPassword(), pQResult->GetPassword()) != 0)
		{
			if (LOGIN_USER_NAME_REGISTER == pQResult->GetLoginType() || LOGIN_ONE_KEY == pQResult->GetLoginType())
			{
				// 帐号已存在
				g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_LOGIN_ACCOUNT_EXIST);
				MAPLOG(GUI_LEVEL, "帐号已存在[Account:%s]", pQResult->GetAccID());
			}
			else
			{
				// 密码不正确, 通知玩家
				g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_LOGIN_PASSWORD_INVALID);
				MAPLOG(GUI_LEVEL, "密码不正确[Account:%s]", pQResult->GetAccID());
			}
			return;
		}

		//创建AccountInfo信息
		AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
		if (!pNewInfo)
		{
			MAPLOG(ERROR_LEVEL, "内存不足，无法创建AccountInfo信息");
			return;
		}

		// 如果是手机注册则修改密码
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

		//加入账号管理器
		AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

		// 登录响应
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
				// 填充最近登录过的服务器信息
				pPlayerBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(pServerData->dwServerId, pQResult->GetChannelUID(), pQResult->GetAccID());
				if (!pPlayerBaseData)
				{
					MAPLOG(IMPORTANT_LEVEL, "[%s]找不到角色基本数据[Account:%s,ChannelUID:%d,ServerId:%d]",
						__FUNCTION__, pQResult->GetAccID(), pQResult->GetChannelUID(), pServerData->dwServerId);
					continue;
				}
				// todo::这样处理会改到源数据
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
		// 登录类型(0=测试,1=用户名注册,2=用户名登录,3=手机登录,4=手机注册)
		// 检测登录类型(用户名登录，或者手机号登录)
		if (LOGIN_USER_NAME_LOGIN == pQResult->GetLoginType() || LOGIN_MOBILE_LOGIN == pQResult->GetLoginType())
		{
			g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_ACCOUNT_NOT_REGISTER);
			MAPLOG(GUI_LEVEL, "帐号还没注册,不能登录[%s]", pQResult->GetAccID());
			return;
		}

		// 创建帐号
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

// 检测账号结果(sdk)
void DBResultHandler::ProcessSDKAccountCheckResult(QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION 
	CheckAccountQuery* pQResult = (CheckAccountQuery*)pData;

	// 数据库操作失败
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(IMPORTANT_LEVEL, "请求检测帐号数据库操作失败[Account:%s,Password:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
		return;
	}

	// 检测是否重复登录
	AccountInfo* pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID());
	if (pAccountInfo)
	{
		MAPLOG(GUI_LEVEL, "重复登录[Account:%s]", pQResult->GetAccID());
		return;
	}

	// 检测账号是否被锁(如果设置解锁时间则需要检测是否解锁)
	if (pQResult->IsLock() && (!pQResult->GetUnlockTime() || time(NULL) < pQResult->GetUnlockTime()))
	{
		g_MapServer.SendErrorMsgToUser(pQResult->GetKey(), ERROR_ACCOUNT_LOCK);
		return;
	}

	// 帐号已存在
	if (pQResult->GetResultRowNum())
	{
		//创建AccountInfo信息
		AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
		if (!pNewInfo)
		{
			MAPLOG(ERROR_LEVEL, "内存不足，无法创建AccountInfo信息");
			return;
		}

		pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(),
			pQResult->GetPassword(), pQResult->GetKey(), pQResult->GetChannelUID(), pQResult->GetTerminalType(), pQResult->GetMobile());
		pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

		//加入账号管理器
		AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

		// 登录响应
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
				// 填充最近登录过的服务器信息
				pPlayerBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(pServerData->dwServerId, pQResult->GetChannelUID(), pQResult->GetAccID());
				if (!pPlayerBaseData)
				{
					MAPLOG(IMPORTANT_LEVEL, "[%s]找不到角色基本数据[Account:%s,ChannelUID:%d,ServerId:%d]", 
						__FUNCTION__, pQResult->GetAccID(), pQResult->GetChannelUID(), pServerData->dwServerId);
					continue;
				}
				// todo::这样处理会改到源数据
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
		// 创建帐号
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

// 创建账号结果
void DBResultHandler::ProcessCreateAccountQueryResult(QueryResult * pData)
{
	CreateAccountQuery* pQResult = (CreateAccountQuery*)pData;

	// 数据库操作失败
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "数据库创建账号失败[AccountName:%s,Password:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
		return;
	}

	// 重复创建帐号 todo::可能连接的点击造成
	if (AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID()))
	{
		// 重复创建帐号
		MAPLOG(GUI_LEVEL, "重复创建帐号[AccountName:%s,Password:%s]",
			pQResult->GetAccID(), pQResult->GetPassword());
		return;	
	}

	//创建AccountInfo信息
	AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
	if (!pNewInfo)
	{
		MAPLOG(ERROR_LEVEL, "内存不足，无法创建AccountInfo信息");
		return;
	}

	pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(), pQResult->GetPassword(),
		pQResult->GetKey(), pQResult->GetChannelUID(), pQResult->GetTerminalType(), NULL);
	pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

	//加入账号管理器
	AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

	// 登录响应
	MSG_S2C_LOGIN_ACCOUNT_RESP msg;
	//msg.m_dwVersion = g_MapServer.GetClientVersion();
	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount) - 1, pQResult->GetAccID());
	msg.m_byIsBindMobile = strlen(pQResult->GetMobile()) ? true : false;
	msg.m_wLastLoginServerNum = 0;
	memset(msg.m_LastLoginServerList, 0, sizeof(msg.m_LastLoginServerList));
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// 统计玩家账号注册
	//CGameLogic::Instance()->OnRegisterStatistics(pQResult->GetChannelUID(), pQResult->GetAccID(), pQResult->GetZoneID());
}

// 创建账号结果(sdk)
void DBResultHandler::ProcessSDKCreateAccountResult(QueryResult * pData)
{
	CreateAccountQuery* pQResult = (CreateAccountQuery*)pData;

	// 数据库操作失败
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "数据库创建账号失败[AccountName:%s,Password:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetQueryResult());
		return;
	}

	// 重复创建帐号 todo::可能连接的点击造成
	if (AccountInfoManager::Instance()->GetAccountInfoByAccID(pQResult->GetChannelUID(), pQResult->GetAccID()))
	{
		// 重复创建帐号
		MAPLOG(GUI_LEVEL, "重复创建帐号[AccountName:%s,Password:%s]",
			pQResult->GetAccID(), pQResult->GetPassword());
		return;
	}

	//创建AccountInfo信息
	AccountInfo* pNewInfo = ObjectFactory::Instance()->AllocAccountInfo();
	if (!pNewInfo)
	{
		MAPLOG(ERROR_LEVEL, "内存不足，无法创建AccountInfo信息");
		return;
	}

	pNewInfo->Create(pQResult->GetZoneID(), pQResult->GetAccID(), pQResult->GetPassword(), pQResult->GetKey(), 
		pQResult->GetChannelUID(), pQResult->GetTerminalType(), NULL);
	pNewInfo->SetUserState(USER_SELECT_SERVER_STATE);

	//加入账号管理器
	AccountInfoManager::Instance()->AddAccountInfo(pNewInfo);

	// 登录响应
	MSG_S2C_SDK_LOGIN_RESP msg;
	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount) - 1, pQResult->GetAccID());
	msg.m_byIsBindMobile = strlen(pQResult->GetMobile()) ? true : false;
	msg.m_wLastLoginServerNum = 0;
	memset(msg.m_LastLoginServerList, 0, sizeof(msg.m_LastLoginServerList));
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// 统计玩家账号注册
	//CGameLogic::Instance()->OnRegisterStatistics(pQResult->GetChannelUID(), pQResult->GetAccID(), pQResult->GetZoneID());
}

//查询分区角色结果
void DBResultHandler::ProcessSelectZoneRoleQueryResult(QueryResult* pData)
{
	ENTER_DUMP_FUNCTION

 	SelectCharListQuery* pQResult = (SelectCharListQuery*)pData;

	// 数据库操作失败
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "数据库查询角色列表失败[AccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
		return;
	}
 
 	DWORD dwUserKey		= pQResult->GetKey();
 	//DWORD  wResultNum	= pQResult->GetResultRowNum();
 
 	//查找玩家
 	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwUserKey);
 	if(!pAccInfo)
 	{
		MAPLOG(ERROR_LEVEL, "查询帐户角色列表返回时找不到帐号信息（GUID=%d", dwUserKey);
 		return;
 	}

	const PlayerBaseData *pPlayerBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(
		pQResult->GetZoneID(), pQResult->GetPlatform(), pQResult->GetAccID());

	MSG_S2C_LOGIN_SELECT_SERVER_RESP msg;
	msg.m_byHaveRole = pPlayerBaseData ? true : false;
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// 有角色
	if (pPlayerBaseData)
	{
		DWORD dwRoleId = pPlayerBaseData->dwRoleId;

		//创建玩家角色对象
		Player* pPlayer = (Player*)ObjectFactory::Instance()->AllocPlayer();
		if (!pPlayer)
		{
			MAPLOG(ERROR_LEVEL, "服务器内部错误,内存不足（account=%s", pAccInfo->GetAccID());
			return;
		}

		pPlayer->Create(g_MapServer.GetGateSession(), pAccInfo->GetAccID(), dwRoleId, dwUserKey, pAccInfo);

		//加入玩家管理器
		PlayerManager::Instance()->AddPlayer(pPlayer);

		//设置用户当前状态
		pAccInfo->SetUserState(USER_LOAD_STATE);
		//设置当前进入游戏的角色ID
		pAccInfo->SetCurCharID(dwRoleId);

		//向DB请求角色完整的游戏信息
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

//查询分区角色结果
// void DBResultHandler::ProcessSelectZoneRoleQueryResult(QueryResult* pData)
// {
// 	ENTER_DUMP_FUNCTION
// 
// 		SelectCharListQuery* pQResult = (SelectCharListQuery*)pData;
// 
// 	// 数据库操作失败
// 	if (0 != pQResult->GetQueryResult())
// 	{
// 		MAPLOG(ERROR_LEVEL, "数据库查询角色列表失败[AccountName:%s,Error:%d]",
// 			pQResult->GetAccID(), pQResult->GetQueryResult());
// 		return;
// 	}
// 
// 	DWORD dwUserKey = pQResult->GetKey();
// 	DWORD  wResultNum = pQResult->GetResultRowNum();
// 
// 	//查找玩家
// 	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwUserKey);
// 	if (!pAccInfo)
// 	{
// 		MAPLOG(ERROR_LEVEL, "查询帐户角色列表返回时找不到帐号信息（GUID=%d", dwUserKey);
// 		return;
// 	}
// 
// 	MSG_S2C_LOGIN_SELECT_SERVER_RESP msg;
// 	msg.m_byHaveRole = wResultNum ? true : false;
// 	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);
// 
// 	// 有角色
// 	if (wResultNum)
// 	{
// 		//创建玩家角色对象
// 		Player* pPlayer = (Player*)ObjectFactory::Instance()->AllocPlayer();
// 		if (!pPlayer)
// 		{
// 			MAPLOG(ERROR_LEVEL, "服务器内部错误,内存不足（account=%s", pAccInfo->GetAccID());
// 			return;
// 		}
// 
// 		pPlayer->Create(g_MapServer.GetGateSession(), pAccInfo->GetAccID(), pQResult->GetRoleId(), dwUserKey, pAccInfo);
// 
// 		//加入玩家管理器
// 		PlayerManager::Instance()->AddPlayer(pPlayer);
// 
// 		//设置用户当前状态
// 		pAccInfo->SetUserState(USER_LOAD_STATE);
// 		//设置当前进入游戏的角色ID
// 		pAccInfo->SetCurCharID(pQResult->GetRoleId());
// 
// 		//向DB请求角色完整的游戏信息
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

//处理创建新角色的结果
void DBResultHandler::ProcessCreateCharQueryResult(QueryResult* pData)
{
	ENTER_DUMP_FUNCTION

	CreateCharInfoQuery* pQResult = (CreateCharInfoQuery*)pData;

	// 删除正在创建角色数据
	g_MapServer.RemoveCreatingRoleData(pQResult->GetAccID(), pQResult->GetChannelUID(), pQResult->GetZoneID());

	// 数据库操作失败
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "数据库创建新角色失败[AccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
		return;
	}

	DWORD dwUserKey	= pQResult->GetKey();

	//查找玩家
	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwUserKey);
	if(!pAccInfo)
		return;

	// 重名
	if (pQResult->IsNameRepeat())
	{
		// 通知玩家名称已被使用
		g_MapServer.SendErrorMsgToUser(dwUserKey, ERROR_LOGIN_ENAME_EXIST);

		// 如果有重名则回到准备状态(因为客户端会再次发送创建角色)
		pAccInfo->SetUserState(USER_READY_CREATE_ROLE_STATE);
		
#ifdef _DEBUG
		char szCharName[MAX_ROLE_LEN] = { 0 };
		ChangeLocalChar(pQResult->GetRoleName(), szCharName, sizeof(szCharName)-1);
		MAPLOG(GUI_LEVEL, "名称已被使用,创建角色失败：AccountId=%s, RoleName=%s",
			pAccInfo->GetAccID(), szCharName);
#endif
		return;
	}

	// 角色已存在
	if (pQResult->IsRoleExist())
	{
		// 通知玩家名称已被使用
		g_MapServer.SendErrorMsgToUser(dwUserKey, ERROR_ROLE_ALREADY_EXIST);
		// 如果有重名则回到准备状态(因为客户端会再次发送创建角色)
		//pAccInfo->SetUserState(USER_READY_CREATE_ROLE_STATE);
		MAPLOG(GUI_LEVEL, "重复创建角色：AccountId=%s, server_id=%d",pAccInfo->GetAccID(), pQResult->GetZoneID());
		return;
	}

	//创建玩家角色对象
	Player* pPlayer = (Player*)ObjectFactory::Instance()->AllocPlayer();
	if (!pPlayer)
	{
		MAPLOG(GUI_LEVEL, "服务器内部错误,内存不足（account=%s", pAccInfo->GetAccID());
		return;
	}

	pPlayer->Create(g_MapServer.GetGateSession(), pAccInfo->GetAccID(), pQResult->GetCharID(), dwUserKey, pAccInfo);

	//加入玩家管理器
	PlayerManager::Instance()->AddPlayer(pPlayer);

	//设置用户当前状态
	pAccInfo->SetUserState(USER_LOAD_STATE);
	//设置当前进入游戏的角色ID
	pAccInfo->SetCurCharID(pQResult->GetCharID());

	//向DB请求角色完整的游戏信息
	SelectCharInfoQuery* pQuery = SelectCharInfoQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_CHAR_INFO);
	pQuery->SetThreadIndex(pAccInfo->GetDBIndex());
	pQuery->SetKey(dwUserKey);
	pQuery->SetCharID(pQResult->GetCharID());
	pQuery->SetChannelType(pQResult->GetChannelUID());
	g_MapServer.MapDBQuery(pQuery);

	MSG_S2C_LOGIN_CREATE_ROLE_RESP msg;
	g_MapServer.GetGateSession()->SendPacket(pQResult->GetKey(), msg.GetType(), &msg);

	// 统计玩家账号注册
	CGameLogic::Instance()->OnRegisterStatistics(pAccInfo->GetChannelUID(), pQResult->GetAccID(), pQResult->GetZoneID());

	// 统计玩家创建角色
	CGameLogic::Instance()->OnGameStatistics_CreateRole(pQResult->GetCharID(), pQResult->GetZoneID());

	LEAVE_DUMP_FUNCTION
}

//处理查询角色信息的结果
void DBResultHandler::ProcessSelectCharInfoQueryResult(QueryResult* pData)
{
	SelectCharInfoQuery* pQResult = (SelectCharInfoQuery*)pData;
	DWORD dwUserKey		= pQResult->GetKey();
	BYTE bySelectType = pQResult->GetSelectType();

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwUserKey);
	if (!pPlayer)
	{
		MAPLOG(GUI_LEVEL, "查询角色的信息返回时，玩家已下线[SelectType:%d]", bySelectType);
		return;
	}

	// 检测数据库操作是否成功 happiness success, money
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "查询角色的信息失败[SelectType:%d]", bySelectType);
		return;
	}

	// 登录查询
	if (!bySelectType)
	{
		//玩家必须是加载状态才可以
		AccountInfo* pAccInfo = pPlayer->GetAccountInfo();
		if (!pAccInfo)
		{
			MAPLOG(ERROR_LEVEL, "查询角色的信息返回时，找不到角色的帐号数据");
			return;
		}

		if (pAccInfo->GetUserState() != USER_LOAD_STATE)
		{
			MAPLOG(ERROR_LEVEL, "查询角色的信息返回时用户状态不对[UserState:%d]", pAccInfo->GetUserState());
			return;
		}

		//设置当前用户状态
		pAccInfo->SetUserState(USER_AT_GAME_STATE);

		//触发玩家加载事件
		pPlayer->OnLoad(pQResult->GetRoleFullInfo());

		//触发玩家登录事件
		pPlayer->OnLogin();

		// 请求更新最近登录的服务器状态
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
	// 逻辑查询
	else
	{
		pPlayer->SelectTargetDetailsCallBack(pQResult->GetRequestType(), &pQResult->GetRoleFullInfo());
	}
}

//处理更新角色信息的结果
void DBResultHandler::ProcessUpdateCharQueryResult(QueryResult* pData)
{
	UpdateRoleInfoQuery* pQResult = (UpdateRoleInfoQuery*)pData;

	DWORD dwUserKey	= pQResult->GetKey();

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwUserKey);
	if(!pPlayer)
		return;

	if (pQResult->GetQueryResult() != 0)
	{
		MAPLOG(ERROR_LEVEL, "保存数据失败CharID=%u", pPlayer->GetCharID());
	}

	//下线处理
	if(pQResult->GetSaveType() == SAVE_DB_TYPE_LOGOUT)
	{
		AccountInfoManager::Instance()->ProcessDBLogoutAck(pPlayer->GetAccountInfo());
	}
}

// 帐号密码修改结果
void DBResultHandler::ProcessAccountPasswordModifyResult(QueryResult * pData)
{
	AccountPasswordModify* pQResult = (AccountPasswordModify*)pData; 
	if (0 == pQResult->GetQueryResult())
	{
		
		MAPLOG(GUI_LEVEL,"修改帐号密码成功![AcccountName:%s,Password:%s]",
			pQResult->GetAccID(), pQResult->GetPassword());
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "修改帐号密码失败![AcccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
	}
}

// 帐号手机号修改结果
void DBResultHandler::ProcessAccountMobileChangeResult(QueryResult * pData)
{
	AccountMobileModify* pQResult = (AccountMobileModify*)pData;
	if (0 == pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "修改手机号成功![account:%s,mobile:%s]",pQResult->GetAccID(), pQResult->GetMobile());
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "修改手机号失败![account:%s,Error:%d]", pQResult->GetAccID(), pQResult->GetQueryResult());
	}
}

// 更新最近登录服务器
void DBResultHandler::ProcessUpdateLastLoginServer(QueryResult * pData)
{
	UpdateLastLoginServer* pQResult = (UpdateLastLoginServer*)pData;
	if (0 == pQResult->GetQueryResult())
	{

		MAPLOG(GUI_LEVEL, "更新最近登录服务器成功![AcccountName:%s,ServerId:%d]", 
			pQResult->GetAccID(), pQResult->GetServerID());
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "更新最近登录服务器失败![AcccountName:%s,Error:%d]",
			pQResult->GetAccID(), pQResult->GetQueryResult());
	}
}

// 存储全局数据
void DBResultHandler::SaveGolbalDataResult(QueryResult * pData)
{
	ServerGolbalData_Save* pQResult = (ServerGolbalData_Save*)pData; 
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储全局数据失败![Error:%d]", pQResult->GetQueryResult());
	}	
}

// 加载全局数据
void DBResultHandler::LoadGolbalDataResult(QueryResult * pData)
{
	ServerGolbalData_Load* pQResult = (ServerGolbalData_Load*)pData; 
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载全局数据失败![Error:%d]", pQResult->GetQueryResult());
		return;
	}	

	const ServerGolbalData *pGolbalDataList = pQResult->GetGolbalDataList();
	BYTE byGolbalDataNum = pQResult->GetGolbalDataNum();

	for (BYTE i = 0; i < byGolbalDataNum; ++i)
	{
		const ServerGolbalData &tData = pGolbalDataList[i];
		g_MapServer.LoadGolbalDataCallback(tData.wServerId, tData.nKey, tData.dwValue);
	}

	MAPLOG(GUI_LEVEL, "加载全局数据成功!");
}

// 加载角色基本数据列表
void DBResultHandler::LoadRoleBaseDataListResult(QueryResult * pData)
{
	RoleBaseDataList_Load* pQResult = (RoleBaseDataList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载角色基础数据列表失败![Error:%d]", pQResult->GetQueryResult());
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

			// 记录本次加载的最大ID
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
	// 已经木有内容可读取
	else
	{
		DWORD dwTotalPlayerNum = PlayerBaseDataManager::Instance()->GetTotalPlayerNum();
		if (dwTotalPlayerNum == pQResult->GetTotalDataNum())
		{
			MAPLOG(IMPORTANT_LEVEL, "Load role basedata success![num:%d]", dwTotalPlayerNum);

			// 因为逻辑上数据依赖这个模块的数据,所在必需等这个模块的数据都加载完再请求
			//CKingMatchManager::Instance()->LoadDataRequest();
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "加载角色基本数据到内存与数据库的数量不匹配![db_num:%d, mem_num:%d]",
				pQResult->GetTotalDataNum(), dwTotalPlayerNum);
		}

		// 加载完
		g_MapServer.SetIsLoadRoleBaseData(true);
	}
}

// 更新好友数据
void DBResultHandler::UpdateFriendDataResult(QueryResult * pData)
{
	FriendDataUpdate_Load* pQResult = (FriendDataUpdate_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(GUI_LEVEL, "更新好友数据失败![Error:%d]", pQResult->GetQueryResult());
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

	MAPLOG(GUI_LEVEL, "成功更新好友数据![type:%d]", pQResult->GetOperateType());
}

// 存储竞技场结果
void DBResultHandler::SaveArenaDataResult(QueryResult * pData)
{
	ArenaDataList_Save* pQResult = (ArenaDataList_Save*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储竞技场数据失败![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储竞技场数据!");
}

// 加载竞技场数据结果
void DBResultHandler::LoadArenaDataResult(QueryResult * pData)
{
	ArenaDataList_Load* pQResult = (ArenaDataList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载竞技场数据失败![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	CArenaManager::Instance()->LoadDataFromDB(pQResult->GetServerId(), pQResult->GetDataBuff(), pQResult->GetDataBuffSize());
}

// 存储角色排行榜数据结果
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

// 加载角色排行榜数据结果
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

// 存储邮件数据
void DBResultHandler::SaveEmailDataResult(QueryResult * pData)
{
	InsertEmailQuery* pQResult = (InsertEmailQuery*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储邮件数据失败![RoleId:%d,Error:%d]", pQResult->GetEmailInfo().dwID, pQResult->GetQueryResult());
		return;
	}

	//MAPLOG(GUI_LEVEL, "存储邮件成功!");
}

// 加载并删除GM事件结果
void DBResultHandler::LoadAndDeleteGMEventResult(QueryResult * pData)
{
	GMEventList_Load* pQResult = (GMEventList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载并删除GM事件数据失败![Error:%d]", pQResult->GetQueryResult());
		return;
	}

// 	if (pQResult->GetEventNum())
// 		MAPLOG(ERROR_LEVEL, "获得GM事件[EventNum:%d]!", pQResult->GetEventNum());

	for (int i = 0; i < pQResult->GetEventNum(); ++i)
	{
		const GMEventData *pEventData = pQResult->GetEventData(i);
		if (pEventData)
		{
			GMManager::Instance()->OnCmdFromDB(pEventData->wEventId, pEventData->arParamList, 10, pEventData->szText, pEventData->szText1);
		}
		//MAPLOG(ERROR_LEVEL, "处理GM事件[idx:%d]!", i);
	}
}

// 加载服务器列表结果
void DBResultHandler::LoadServerListResult(QueryResult * pData)
{
	ServerList_Load* pQResult = (ServerList_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载服务器列表失败![Error:%d]", pQResult->GetQueryResult());
		return;
	}

	g_MapServer.LoadServerList(pQResult->GetServerList(), pQResult->GetServerNun());
}

// 存储竞技场战斗记录数据结果
void DBResultHandler::SaveArenaBattleRecordResult(QueryResult * pData)
{
	ArenaBattleRecord_Save* pResult = (ArenaBattleRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储竞技场战斗记录数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 加载竞技场数据结果
void DBResultHandler::LoadArenaBattleRecordResult(QueryResult * pData)
{
	ArenaBattleRecord_Load* pResult = (ArenaBattleRecord_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载竞技场战斗记录数据失败![Error:%d]", pResult->GetQueryResult());
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

			// 记录本次加载的最大ID
			dwMinRoleId = max(dwMinRoleId, pPlayerBattleRecordList[i].dwRoleId);
		}

		ArenaBattleRecord_Load* pQuery = ArenaBattleRecord_Load::ALLOC();
		pQuery->SetIndex(DB_ARENA_BATTLE_RECORD_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_ARENA_BATTLE_RECORD);
		pQuery->SetMinRoleId(dwMinRoleId);
		pQuery->SetTotalBattleRecordNum(pResult->GetTotalBattleRecordNum());
		g_MapServer.MapDBQuery(pQuery);
	}
	// 已经木有内容可读取
	else
	{
		int nTotalPlayerBattleRecordNum = CArenaManager::Instance()->GetPlayerBattleRecordNum();
		if (nTotalPlayerBattleRecordNum == pResult->GetTotalBattleRecordNum())
		{
			MAPLOG(GUI_LEVEL, "成功加载竞技场战斗记录列表![num:%d]", nTotalPlayerBattleRecordNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "加载竞技场战斗记录到内存与数据库的数量不匹配![db_num:%d, mem_num:%d]",
				pResult->GetTotalBattleRecordNum(), nTotalPlayerBattleRecordNum);
		}
	}
}

// 存储角色货币日志结果
void DBResultHandler::SaveRoleCurrencyLogsResult(QueryResult * pData)
{
	RoleCurrencyLogs_Save* pResult = (RoleCurrencyLogs_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储角色货币日志失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 存储领地数据结果
void DBResultHandler::SaveLandDataResult(QueryResult * pData)
{
	LandData_Save* pResult = (LandData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储领地数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 加载领地数据结果
void DBResultHandler::LoadLandDataResult(QueryResult * pData)
{
	LandData_Load* pResult = (LandData_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载领地数据失败![Error:%d]", pResult->GetQueryResult());
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

			// 记录本次加载的最大ID
			dwMinRoleId = max(dwMinRoleId, pRoleIdList[i]);
		}

		LandData_Load* pQuery = LandData_Load::ALLOC();
		pQuery->SetIndex(DB_LAND_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_LAND);
		pQuery->SetMinRoleId(dwMinRoleId);
		pQuery->SetTotalLandNum(nTotalLandNum);
		g_MapServer.MapDBQuery(pQuery);
	}
	// 已经木有内容可读取
	else
	{
		int nLandListSize = CLandManager::Instance()->GetLandListSize();
		if (nLandListSize == nTotalLandNum)
		{
			MAPLOG(GUI_LEVEL, "成功加载领地数据![num:%d]", nLandListSize);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "加载领地数据到内存与数据库的数量不匹配![db_num:%d, mem_num:%d]",
				nTotalLandNum, nLandListSize);
		}
	}
}

// 竞技场排行榜查看列表更新结果
void DBResultHandler::UpdateArenaRankViewListResult(QueryResult * pData)
{
	LandData_Load* pResult = (LandData_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "更新竞技场排行榜查看列表失败![Error:%d]", pResult->GetQueryResult());
	}
	else
	{
		MAPLOG(GUI_LEVEL, "更新竞技场排行榜查看列表成功!");
	}
}

// 存贷留言结果
void DBResultHandler::SaveLeaveMessageResult(QueryResult * pData)
{
	LeaveMessage_Save* pResult = (LeaveMessage_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储留言内容失败![Error:%d]", pResult->GetQueryResult());
	}
}

// 存储奖励中心数据结果
void DBResultHandler::SaveRewardCenterDataResult(QueryResult * pData)
{
	RewardCenterData_Save* pResult = (RewardCenterData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储奖励中心数据失败![Error:%d,role_id:%d]", 
			pResult->GetQueryResult(), pResult->GetRoleId());
		return;
	}

	PlayerBaseDataManager::Instance()->RemoveReward(pResult->GetServerId(), pResult->GetRoleId(), pResult->GetRewardData().dwUId);
}

// 更新称号数据结果
void DBResultHandler::UpdateRoleTempDataResult(QueryResult * pData)
{
	RoleTmpData_Update* pResult = (RoleTmpData_Update*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "更新角色临时数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "更新角色临时数据成功!");
}

// 存储角色下线记录结果
void DBResultHandler::SaveRoleOfflineRecordResult(QueryResult * pData)
{
	RoleOfflineRecord_Save* pResult = (RoleOfflineRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储角色下线记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "存储角色下线记录成功!");
}

// 存储角色等级记录结果
void DBResultHandler::SaveRoleLevelRecordResult(QueryResult * pData)
{
	RoleLevelRecord_Save* pResult = (RoleLevelRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储角色等级记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "存储角色等级记录成功!");
}

// 存储角色充值记录结果
void DBResultHandler::SaveRoleRechargeRecordResult(QueryResult * pData)
{
	RoleRechargeRecord_Save* pResult = (RoleRechargeRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储角色充值记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "存储角色充值记录成功!");
}

// 存储关卡失败记录结果
void DBResultHandler::SaveRoleMapLoseRecordResult(QueryResult * pData)
{
	RoleMapLoseRecord_Save* pResult = (RoleMapLoseRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储关卡失败记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	//MAPLOG(GUI_LEVEL, "存储关卡失败记录成功!");
}

// 存储关卡失败记录结果
void DBResultHandler::SaveRoleFirstPassMapRecordResult(QueryResult * pData)
{
	RoleFirstPassMapRecord_Save* pResult = (RoleFirstPassMapRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储首次通关记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	//MAPLOG(GUI_LEVEL, "存储首次通关记录成功!");
}

// 存储商店购买记录结果
void DBResultHandler::SaveShopBuyRecordResult(QueryResult * pData)
{
	ShopBuyRecord_Save* pResult = (ShopBuyRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储商店购买记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 存储物品丢失记录结果
void DBResultHandler::SaveItemLoseRecordResult(QueryResult * pData)
{
	ItemLoseRecord_Save* pResult = (ItemLoseRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储物品丢失记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 存储物品操作记录结果
void DBResultHandler::SaveItemOperationRecordResult(QueryResult * pData)
{
	ItemOperationRecord_Save* pResult = (ItemOperationRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储物品操作记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 存储在线玩家数量记录结果
void DBResultHandler::SaveOfflinePlayerNumRecordResult(QueryResult * pData)
{
	OnlinePlayerNumRecord_Save* pResult = (OnlinePlayerNumRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储在线玩家数量记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}


// 存储工会数据结果
void DBResultHandler::SaveGuildDataResult(QueryResult * pData)
{
	GuildData_Save* pResult = (GuildData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储工会数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "存储工会数据成功![GuildId:%d]", pResult->GetKey());
}

// 加载工会数据结果
void DBResultHandler::LoadGuildDataResult(QueryResult * pData)
{
	GuildData_Load* pResult = (GuildData_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载工会数据失败![Error:%d]", pResult->GetQueryResult());
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
			// 记录本次加载的最大ID
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
	// 已经木有内容可读取
	else
	{
		int nAlreadyLoadGuildNum = CGuildManager::Instance()->GetGuildNum();
		if (nAlreadyLoadGuildNum == nTotalGuildNum)
		{
			MAPLOG(GUI_LEVEL, "成功加载工会列表![num:%d]", nTotalGuildNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "加载工会列表出错![db_num:%d, mem_num:%d]", nTotalGuildNum, nAlreadyLoadGuildNum);
		}
		MAPLOG(GUI_LEVEL, "工会的系列ID[id:%d]", CGuildManager::Instance()->GetSeriesId());
	}
}

// 存储聊天记录
void DBResultHandler::SaveChatRecordResult(QueryResult * pData)
{
	ChatRecord_Save* pResult = (ChatRecord_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储聊天记录失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 加载聊天记录
void DBResultHandler::LoadChatRecordResult(QueryResult * pData)
{
	
}

// 存储日志结果
void DBResultHandler::SaveLogsResult(QueryResult * pData)
{
	Log_Save* pResult = (Log_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储日志失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 存储关卡评论结果
void DBResultHandler::SaveMapCommentResult(QueryResult * pData)
{
	MapComment_Save* pResult = (MapComment_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储关卡评论数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 加载关卡评论结果
void DBResultHandler::LoadMapCommentResult(QueryResult * pData)
{
	MapComment_Load* pResult = (MapComment_Load*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载关卡评论数据失败![Error:%d]", pResult->GetQueryResult());
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

			// 记录本次加载的最大ID
			dwMinId = max(dwMinId, pComments->dwId);
		}

		MapComment_Load* pQuery = MapComment_Load::ALLOC();
		pQuery->SetIndex(DB_MAP_COMMENT_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_MAP_COMMENT);
		pQuery->SetMinId(dwMinId);
		pQuery->SetTotalDataNum(pResult->GetTotalDataNum());
		g_MapServer.MapDBQuery(pQuery);
	}
	// 已经木有内容可读取
	else
	{
		DWORD dwTotalCommentNum = g_MapServer.GetMapCommentNum();
		if (dwTotalCommentNum == pResult->GetTotalDataNum())
		{
			MAPLOG(GUI_LEVEL, "成功加载关卡评论列表![num:%d]", dwTotalCommentNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "加载关卡评论数据到内存与数据库的数量不匹配![db_num:%d, mem_num:%d]",
				pResult->GetTotalDataNum(), dwTotalCommentNum);
		}
	}
}

// 更新帐号结果
void DBResultHandler::UpdateAccountLockResult(QueryResult * pData)
{
	AccountLock_Update* pResult = (AccountLock_Update*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "更新帐号锁失败![Error:%d]", pResult->GetQueryResult());
		return;
	}
}

// 存储王者赛数据结果
void DBResultHandler::SaveKingMatchDataResult(QueryResult * pData)
{
	KingMatchData_Save* pResult = (KingMatchData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储王者赛数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储王者赛数据!");
}

// 加载王者赛数据结果
void DBResultHandler::LoadKingMatchDataResult(QueryResult * pData)
{
	KingMatchData_Load* pQResult = (KingMatchData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载王者赛数据失败![ErrorId:%d,ErrorMsg:%s]", pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CKingMatchManager::Instance()->LoadDataFromDB(pQResult->GetCurSeason(), pQResult->GetDataBelongSeason(),
		pQResult->GetDataBuff(), pQResult->GetDataBuffSize());


	MAPLOG(GUI_LEVEL, "从数据库成功加载王者赛数据[cur_season:%d,data_belong_season:%d]!",
		pQResult->GetCurSeason(), pQResult->GetDataBelongSeason());
}

// 存储积分赛数据结果
void DBResultHandler::SaveScoreMatchDataResult(QueryResult * pData)
{
	ScoreMatchData_Save* pResult = (ScoreMatchData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储积分赛数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储积分赛数据!");
}

// 加载积分赛数据结果
void DBResultHandler::LoadScoreMatchDataResult(QueryResult * pData)
{
	ScoreMatchData_Load* pQResult = (ScoreMatchData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载积分赛数据失败![ErrorId:%d,ErrorMsg:%s]", pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	DWORD dwActivityIdx = pQResult->GetCurActivityIndex();
	BYTE byActivityStatus = pQResult->GetActivityStatus();
	DWORD dwOverTime = pQResult->GetActivityOverTime();

	CScoreMatchManager::Instance()->LoadDataFromDB(dwActivityIdx,byActivityStatus,dwOverTime,pQResult->GetDataBuff(),pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "从数据库成功加载积分赛数据[ActivityIdx:%d,ActivityStatus:%d]!",dwActivityIdx, byActivityStatus);
}

// 存储组队副本数据结果
void DBResultHandler::SaveTeamDungeonRankDataResult(QueryResult * pData)
{
	TeamDungeonRankData_Save* pResult = (TeamDungeonRankData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储组队副本排行数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储组队副本排行数据!");
}

// 加载组队副本排行数据结果
void DBResultHandler::LoadTeamDungeonRankDataResult(QueryResult * pData)
{
	TeamDungeonRankData_Load* pQResult = (TeamDungeonRankData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载组队副本排行数据失败![ErrorId:%d,ErrorMsg:%s]",
			pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CTeamDungeonManager::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "从数据库成功加载组队副本排行数据[data_size:%d]!", pQResult->GetDataBuffSize());
}

// 存储转盘数据结果
void DBResultHandler::SaveTurntableDataResult(QueryResult * pData)
{
	TurntableData_Save* pResult = (TurntableData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储转盘数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储转盘数据!");
}

// 加载转盘数据结果
void DBResultHandler::LoadTurntableDataResult(QueryResult * pData)
{
	TurntableData_Load* pQResult = (TurntableData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载转盘数据失败![ErrorId:%d,ErrorMsg:%s]",
			pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CTurntableManager::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "从数据库成功加载转盘数据[data_size:%d]!", pQResult->GetDataBuffSize());
}

// 更新新手引导结果
void DBResultHandler::UpdateNewRoleGuideResult(QueryResult * pData)
{
	NewRoleGuide_Update* pQResult = (NewRoleGuide_Update*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "更新新手引导结果失败![ErrorId:%d,ErrorMsg:%s]",
			pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}
}

// 存储资源战数据结果
void DBResultHandler::SaveResourceWarDataResult(QueryResult * pData)
{
	ResourceWarData_Save* pResult = (ResourceWarData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储资源战数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储资源战数据!");
}

// 加载资源战数据结果
void DBResultHandler::LoadResourceWarDataResult(QueryResult * pData)
{
	ResourceWarData_Load* pQResult = (ResourceWarData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载资源战数据失败![ErrorId:%d,ErrorMsg:%s]", pQResult->GetQueryResult(), pQResult->GetQueryResultString());
		return;
	}

	CResourceWarManager::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "从数据库成功加载资源战数据[DataSize:%d]!", pQResult->GetDataBuffSize());
}

// 存储工会战数据结果
void DBResultHandler::SaveGuildWarDataResult(QueryResult * pData)
{ 
	GuildWarData_Save* pResult = (GuildWarData_Save*)pData;
	if (0 != pResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储工会战数据失败![Error:%d]", pResult->GetQueryResult());
		return;
	}

	MAPLOG(GUI_LEVEL, "成功存储工会战数据!");
}

// 加载工会战数据结果
void DBResultHandler::LoadGuildWarDataResult(QueryResult * pData)
{
	GuildWarData_Load* pQResult = (GuildWarData_Load*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "加载工会战数据失败![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}

	CGuildWarMgr::Instance()->LoadDataFromDB(pQResult->GetDataBuff(), pQResult->GetDataBuffSize());

	MAPLOG(GUI_LEVEL, "从数据库成功加载工会战战数据[DataSize:%d]!", pQResult->GetDataBuffSize());
}

// 检索激活码数据
void DBResultHandler::SelectActivityCodeResult(QueryResult * pData)
{
	ActivationCode_Select* pQResult = (ActivationCode_Select*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "检索激活码数据失败![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByRoleId(pQResult->GetRoleID());
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "检索激活码数据回调时找不到玩家数据![RoleId:%d]", pQResult->GetRoleID());
		return;
	}

	pPlayer->GetLogicManager()->OnTakeActivityGiftCallback(pQResult->GetActivationCode(), pQResult->GetResult(),
		pQResult->GetStatus(), pQResult->GetRewardId(), pQResult->GetExpireTime(), pQResult->GetBatch(), pQResult->GetType());
}

// 更新激活码数据结果
void DBResultHandler::UpdateActivityCodeResult(QueryResult * pData)
{
	ActivationCode_Update* pQResult = (ActivationCode_Update*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "更新激活码数据失败![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}
}

// 游戏统计事件结果
void DBResultHandler::GameStatisticsEventResult(QueryResult * pData)
{
	CGameStatistics_Event* pQResult = (CGameStatistics_Event*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "游戏统计事件失败![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}
}

// 存储GM操作记录
void DBResultHandler::GMOperationRecordSaveResult(QueryResult * pData)
{
	CGMOperationRecord_Save* pQResult = (CGMOperationRecord_Save*)pData;
	if (0 != pQResult->GetQueryResult())
	{
		MAPLOG(ERROR_LEVEL, "存储GM操作记录失败![ErrorId:%d]", pQResult->GetQueryResult());
		return;
	}
}