#include "StdAfx.h"
#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <crtdbg.h>
#include "HandlerFromGate.h"
#include "GateServerSession.h"
#include "MapSvrQuery.h"
#include "AccountInfoManager.h"
#include "AccountInfo.h"
#include "ObjectFactory.h"
#include "PlayerManager.h"
#include "Player.h"
#include "../Common/Const.h"
#include "../Common/MsgDefine.h"
#include "../Common/CommonLogic.h"
#include "EmailSystem.h"
#include "TimeoutManager.h"
#include "ChatManager.h"
#include "SysTimesManager.h"
#include "../ToolKit/DBCFileManager.h"
#include "../ToolKit/Rand.h"
#include "OpenServerSession.h"
#include "../ToolKit/MiniDump.h"
#include "../Jsoncpp/include/json/reader.h"
#include "../Jsoncpp/src/lib_json/json_batchallocator.h"
#include "GMManager.h"
#include "../Common/DataDefine.h"
#include "NetBase.h"
#include "header.pb.h"
#include "login.pb.h"
#include "dgame_define.pb.h"
#include "server.pb.h"
#include "ConfigManager.h"
#include <atlenc.h>
#include "PacketHandler.h"
#include "Battle/BattleManager.h"
#include "Rank/RankManager.h"
#include "Shop/ShopManager.h"
#include "Fuben/FubenManager.h"
#include "Item/ItemManager.h"
#include "EquipmentManager.h"
#include "Hero/HeroManager.h"
#include "Friend/FriendManager.h"
#include "Treasure/TreasureManager.h"
#include "Rebel/RebelManager.h"
#include "PlayerBaseDataManager.h"
#include "Recycling/RecyclingManager.h"
#include "Duobao/DuobaoManager.h"
#include "Spirit/SpiritManager.h"
#include "Land/LandManager.h"
#include "Email/EmailManager.h"
#include "Guild/GuildManager.h"
#include "Arena/ArenaManager.h"
#include "Achievement/AchievementManager.h"
#include "DailyMission/DailyMissionManager.h"
#include "WorldBoss/WorldBossManager.h"
#include "RewardCenter/RewardCenterManager.h"
#include "MapServer.h"
#include "Logic/LogicManager.h"
#include "SevenDayActivity/SevenDayManager.h"
#include "ZhanBu/ZhanBuManager.h"
#include "ScoreMatch/ScoreMatchManager.h"
#include "Warcraft/WarcraftManager.h"
#include "Title/TitleManager.h"
#include "Activity/ActivityManager.h"
#include "RedPoint/RedPointManager.h"
#include "Recharge/RechargeManager.h"
#include "PVP/PVPManager.h"
#include "AttackCity/AttackCityManager.h"
#include "EndlessLand/EndlessLandManager.h"
#include "ToolKit/sha256.h"
#include "Gem/GemManager.h"
#include "TeamDungeon/TeamDungeonManager.h"
#include "KingMatch/KingMatchManager.h"
#include "Activity/Christmas.h"
#include "GameLogic.h"
#include "Turntable/TurntableManager.h"
#include "WorldLevelTask/WorldLevelTaskManager.h"
#include "ResourceWar/ResourceWarManager.h"
#include "Guild/GuildWarMgr.h"
#include "HangUpDrop/HangUpDropMgr.h"

HandlerFromGate::HandlerFromGate(void)
{

}

HandlerFromGate::~HandlerFromGate(void)
{

}

// BOOL CheckRoleName(const char* pszRoleName)
// {
// 	// 检测长度
// 	int nRoleLength = (int)strlen(pszRoleName);
// 	if (nRoleLength >= MAX_ROLE_LEN)
// 	{
// 		return FALSE;
// 	}
// 
// 	//4 检测字符串是否合法
// 	if (!IsLegalName(pszRoleName, nRoleLength+1))
// 	{
// 		return FALSE;
// 	}
// 
// 	// 检测敏感字库
//   	if (/*g_MapServer.MatchRoleName(pszRoleName) ||*/ g_MapServer.MatchSqlCmd(pszRoleName))
//   	{
//   		return FALSE;
//   	}
// 
// 	return TRUE;
// }


BOOL CheckAccountName(const char* pszAccountName)
{
	int nAccountLen = strlen(pszAccountName);

	// 检测长度
	if (strlen(pszAccountName) >= MAX_ACCOUNT_LEN)
		return FALSE;

	// 计算数字跟字母数量
	int nNumberCount = 0;
	int nCharCount = 0;
	for (int i = 0; i < nAccountLen; ++i)
	{
		if (CheckIsNumber(pszAccountName[i]))
			nNumberCount++;

		if (CheckIsChar(pszAccountName[i]))
			nCharCount++;
	}

	// 不能纯数字
	if (nNumberCount >= nAccountLen)
		return FALSE;

	// 木有数字或者字母
// 	if (!nNumberCount || !nCharCount)
// 		return FALSE;

	// 有非数字或字母
	if (nNumberCount + nCharCount < nAccountLen)
		return FALSE;

// 	wchar_t wcTemp[MAX_ACCOUNT_LEN];
// 	memset(wcTemp,0,sizeof(wcTemp));
// 	if (MultiByteToWideChar(CP_UTF8, 0, pszAccountName, -1, wcTemp, MAX_ACCOUNT_LEN) == 0)
// 	{
// 		return FALSE;
// 	}
// 
// 	//unicode-->CP_ACP
// 	char szDest[MAX_ACCOUNT_LEN];
// 	memset(szDest,0,sizeof(szDest));
// 	if (WideCharToMultiByte(CP_ACP, 0, wcTemp, -1, szDest, MAX_ACCOUNT_LEN, 0, 0) == 0)
// 	{
// 		return FALSE;
// 	}
// 
// 	//4 检测字符串是否合法
// 	if (!IsLegalName(szDest, sizeof(szDest)))
// 	{
// 		return FALSE;
// 	}

	return TRUE;
}

//心跳包
VOID HandlerFromGate::OnMSG_C2S_HEART_BEAT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_HEART_BEAT_REQ* pRecv = (MSG_C2S_HEART_BEAT_REQ*)pMsg;

	// 响应
	MSG_S2C_HEART_BEAT_RESP msg;
	msg.m_dwValue = pRecv->m_dwValue;
	pSession->SendPacket(dwGuid, msg.GetType(), &msg);

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);

	// 如果找不到目标,则不处理
	if (!pPlayer)
		return;

	DWORD dwCurTime = time(NULL);

	///////////////////////////////////////////////////////////////
	// 防外挂逻辑

	// 做时间误差检测
	if (pRecv->m_dwValue >= dwCurTime + 60)
	{
		DATETIME datetime(pRecv->m_dwValue);
		char szDateTime[64] = { 0 };
		datetime.toString(szDateTime);
		MAPLOG(ERROR_LEVEL, "客户端跟服务器的时间有误差[role_id:%d,role_name:%s,client_time:%s]", 
			pPlayer->GetRoleId(), pPlayer->GetUTF8RoleName(), szDateTime);

		// 把用户踢下线
		AccountInfo *pAccount = pPlayer->GetAccountInfo();
		AccountInfoManager::Instance()->KickoutAccount(pAccount);
	}
	else
	{
		DWORD dwLastHeartBeatTime = pPlayer->GetLastHeartBeatTime();
		DWORD dwInterval = pRecv->m_dwValue - dwLastHeartBeatTime;
		pPlayer->SetLastHeartBeatTime(dwCurTime);
		if (dwLastHeartBeatTime)
		{
			// 如是两次间隔时间大于指定时间(1分钟),则重置数据
			if (dwCurTime >= dwLastHeartBeatTime + 60)
			{
				pPlayer->SetHeartBeatFastTimes(0);
			}
			else
			{
				// 两次包间隔时间过快,记录过快的数量
				if (dwInterval <= g_GetConfigValue(143))
				{
					WORD wTimes = pPlayer->GetHeartBeatFastTimes() + 1;
					MAPLOG(ERROR_LEVEL, "心跳过快[account:%s,Interval:%d, times:%d]", pPlayer->GetAccID(), dwInterval, wTimes);
					pPlayer->SetHeartBeatFastTimes(wTimes);
				}

				// 暂时3次
				if (pPlayer->GetHeartBeatFastTimes() >= g_GetConfigValue(203))
				{
					pPlayer->SendErrorMsg(3);
					MSG_M2G_CLIENT_KICKOUT_CMD msg;
					msg.m_dwErrorCode = 0;
					pSession->SendPacket(dwGuid, msg.GetType(), &msg);
					MAPLOG(ERROR_LEVEL, "心跳包频率过快,被踢掉[role_id:%d,name:%s,interval:%d]",
						pPlayer->GetRoleId(), pPlayer->GetUTF8RoleName(), dwInterval);

					// 把用户踢下线
					AccountInfo *pAccount = pPlayer->GetAccountInfo();
					AccountInfoManager::Instance()->KickoutAccount(pAccount);
				}
			}
		}
	}
}

VOID HandlerFromGate::OnMSG_C2S_ACCOUNT_REGISTER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_ACCOUNT_REGISTER_REQ* pRecv = (MSG_C2S_ACCOUNT_REGISTER_REQ*)pMsg;

	const char *pszAccount = pRecv->m_szAccount;
	const char *pszPassword = pRecv->m_szPassword;

	// 帐号及密码不能为空
	if (!strlen(pszAccount) || !strlen(pszPassword))
	{
		MAPLOG(GUI_LEVEL, "账号或密码不合法[Guid:%d,account:%s,password:%s]", dwGuid, pszAccount, pszPassword);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_PASSWORD_INVALID);
		return;
	}

	// 用户名注册
	if (1 == pRecv->m_byType)
	{

	}
	// 手机号注册
	else
	{

	}

	// todo::检测帐号的合法性

}

VOID HandlerFromGate::OnMSG_C2S_ACCOUNT_PASSWORD_CHANGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_ACCOUNT_PASSWORD_CHANGE_REQ* pRecv = (MSG_C2S_ACCOUNT_PASSWORD_CHANGE_REQ*)pMsg;

	const char *pszAccount = pRecv->m_szAccount;
	const char *pszPassword = pRecv->m_szPassword;

	// todo::检测帐号的合法性

	// todo::检测帐号及密码的合法性
	if (!strlen(pszAccount) || !strlen(pszPassword))
	{
		MAPLOG(GUI_LEVEL, "账号或密码不合法[Guid:%d,account:%s,password:%s]", dwGuid, pszAccount, pszPassword);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_PASSWORD_INVALID);
		return;
	}
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_ACTIVITY_GIFT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_TAKE_ACTIVITY_GIFT_REQ* pRecv = (MSG_C2S_TAKE_ACTIVITY_GIFT_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

// 	if (pPlayer->GetLogicManager())
// 		pPlayer->GetLogicManager()->OnNetTakeActivityGift(pRecv->m_szCode);

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetTakeActivityGift_New(pRecv->m_szCode);
}

VOID HandlerFromGate::OnMSG_C2S_ACTIVITY_NOTICE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_ACTIVITY_NOTICE_REQ* pRecv = (MSG_C2S_ACTIVITY_NOTICE_REQ*)pMsg;

	MSG_S2C_ACTIVITY_NOTICE_RESP msg;
	msg.m_wNoticeId = g_GetConfigValue(126);
	// 	WORD wCurNoticeId = g_GetConfigValue(126);
	// 	if (wCurNoticeId && wCurNoticeId != m_pPlayer->GetProperty(ROLE_PRO_LAST_ACTIVITY_NOTICE_ID))
	// 	{
	// 		m_pPlayer->SetProperty(ROLE_PRO_LAST_ACTIVITY_NOTICE_ID, wCurNoticeId);
	// 		msg.m_wNoticeId = wCurNoticeId;
	// 	}
	//m_pPlayer->SendPacket(&msg);
	pSession->SendPacket(dwGuid, msg.GetType(), &msg);

// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// 	if (pPlayer->GetLogicManager())
// 		pPlayer->GetLogicManager()->OnNetActivityNotice();
}

VOID HandlerFromGate::OnMSG_C2S_HERO_GOTO_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_HERO_GOTO_REQ* pRecv = (MSG_C2S_HERO_GOTO_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetGoTo(pRecv->m_wScene, pRecv->m_tSrcPos, pRecv->m_tDestPos);
}

VOID HandlerFromGate::OnMSG_C2S_ENTER_SCENE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_ENTER_SCENE_REQ* pRecv = (MSG_C2S_ENTER_SCENE_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetEnterScene(pRecv->m_wScene, pRecv->m_dwContext);
}

VOID HandlerFromGate::OnMSG_C2S_LEAVE_SCENE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_LEAVE_SCENE_REQ* pRecv = (MSG_C2S_LEAVE_SCENE_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetLeaveScene(pRecv->m_wScene);
}

VOID HandlerFromGate::OnMSG_C2S_SCENE_ITEM_LOCK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_SCENE_ITEM_LOCK_REQ* pRecv = (MSG_C2S_SCENE_ITEM_LOCK_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

 	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetLockItem(pRecv->m_wScene, pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_SCENE_ITEM_PICK_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_SCENE_ITEM_PICK_UP_REQ* pRecv = (MSG_C2S_SCENE_ITEM_PICK_UP_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetPickUpItem(pRecv->m_wScene, pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_NEW_ROLE_GUIDE_UPDATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_NEW_ROLE_GUIDE_UPDATE_REQ* pRecv = (MSG_C2S_NEW_ROLE_GUIDE_UPDATE_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetNewRoleGuideUpdate(pRecv->m_nKey, pRecv->m_nValue);
}




VOID HandlerFromGate::OnMSG_C2S_AUTHENTICATION_CODE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_AUTHENTICATION_CODE_REQ* pRecv = (MSG_C2S_AUTHENTICATION_CODE_REQ*)pMsg;

	MSG_M2O_AUTHENTICATION_CODE_REQ open_msg;
	strcpy_s(open_msg.m_szMobile, sizeof(open_msg.m_szMobile), pRecv->m_szMobile);
	open_msg.m_dwTemplId = g_GetConfigValue(144);
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, open_msg.GetType(), &open_msg);

	MSG_S2C_AUTHENTICATION_CODE_RESP msg;
	pSession->SendPacket(dwGuid, msg.GetType(), &msg);
}

VOID HandlerFromGate::OnMSG_C2S_MOBILE_BIND_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_MOBILE_BIND_REQ* pRecv = (MSG_C2S_MOBILE_BIND_REQ*)pMsg;

	// 检测是否已在登录中
	AccountInfo *pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(PLATFORM_TEST, pRecv->m_szAccount);
	if (!pAccountInfo)
	{
		MAPLOG(GUI_LEVEL, "[%s]帐号还没登录,不能进行手机绑定[%s]", __FUNCTION__, pRecv->m_szAccount);
		return;
	}

	// 取认证码数据
	const MobileAuthData *pAuthData = g_MapServer.GetMobileAuthData(pRecv->m_szMobile);
	if (!pAuthData)
	{
		MAPLOG(GUI_LEVEL, "[%s]找不到手机认证码[%s]", __FUNCTION__, pRecv->m_szMobile);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_BIND_AUTH_CODE_INVALID);
		return;
	}
	// 检测认证码
	if (pAuthData->wCode != pRecv->m_wCode)
	{
		MAPLOG(GUI_LEVEL, "[%s]手机认证码错误[%s]", __FUNCTION__, pRecv->m_szMobile);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_BIND_AUTH_CODE_INVALID);
		return;
	}

	// 删除已使用的认证码信息
	g_MapServer.RemoveMobileAuthData(pRecv->m_szMobile);

	pAccountInfo->SetMobile(pRecv->m_szMobile);

	// 同步数据库
	AccountMobileModify* pQuery = AccountMobileModify::ALLOC();
	pQuery->SetIndex(MAP_QUERY_CHANGE_MOBILE);
	pQuery->SetThreadIndex(0);
	pQuery->SetAccID(pRecv->m_szAccount);
	pQuery->SetMobile(pRecv->m_szMobile);
	pQuery->SetZoneID(0);
	pQuery->SetKey(dwGuid);
	g_MapServer.MapDBQuery(pQuery);

	MSG_S2C_MOBILE_BIND_RESP msg;
	pSession->SendPacket(dwGuid, msg.GetType(), &msg);
}

VOID HandlerFromGate::OnMSG_C2S_SDK_LOGIN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_SDK_LOGIN_REQ* pRecv = (MSG_C2S_SDK_LOGIN_REQ*)pMsg;

	MAPLOG(GUI_LEVEL, "[%s]请求登录[terminal_type:%d, cp_channel_type:%d, sdk_channel_type:%d, account:%s, Guid:%d, ip:%s, deviceid:%s]!", __FUNCTION__,
		pRecv->m_byTerminalType, pRecv->m_dwCPChannelType, pRecv->m_dwSDKChannelType, pRecv->m_szAccount, dwGuid, pRecv->m_szIP, pRecv->m_szDeviceId);

	// 检测版本号
// 	DWORD dwServerVersion = 1;
// 	if (pRecv->m_dwVersion < dwServerVersion)
// 	{
// 		MAPLOG(GUI_LEVEL, "客户端与服务器的版本不匹配[client_vesion:%d,server_version:%d]",
// 			pRecv->m_dwVersion, dwServerVersion);
// 		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_VERSION_INVALID);
// 		return;
// 	}

	// 检测是否角色数据加载完
	if (!g_MapServer.IsLoadRoleBaseData())
	{
		MAPLOG(ERROR_LEVEL, "角色数据还没加载![account:%s]", pRecv->m_szAccount);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_SERVER_INVALID);
		return;
	}

	// 检测token
	if (!pRecv->m_szToken[0])
	{
		MAPLOG(ERROR_LEVEL, "[%s]token为空!", __FUNCTION__);
		return;
	}

	// 检测帐号
	if (!pRecv->m_szAccount[0])
	{
		MAPLOG(ERROR_LEVEL, "[%s]帐号为空!", __FUNCTION__);
		return;
	}

	// 检测是否白名单
	if (g_MapServer.IsCheckAccountWhite() && (!g_pCfgMgr->IsAccountWhite(pRecv->m_szAccount) && !g_pCfgMgr->IsIPWhite(pRecv->m_szIP)))
	{
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_SERVER_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]不是帐号白名单不能登录[account:%s]!", __FUNCTION__, pRecv->m_szAccount);
		return;
	}

	// 向SDK认证
	MSG_M2O_CHECK_USER_INFO_REQ msg;
	msg.m_byTerminalType = pRecv->m_byTerminalType;
	msg.m_dwCPChannelType = pRecv->m_dwCPChannelType;
	msg.m_dwSDKChannelType = pRecv->m_dwSDKChannelType;
	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount), pRecv->m_szAccount);
	strcpy_s(msg.m_szToken, sizeof(msg.m_szToken), pRecv->m_szToken);
	strcpy_s(msg.m_szIP, sizeof(msg.m_szIP), pRecv->m_szIP);
	strcpy_s(msg.m_szDeviceId, sizeof(msg.m_szDeviceId), pRecv->m_szDeviceId);
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);
}


VOID HandlerFromGate::OnMSG_C2S_LOGIN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
//  	MSG_C2S_LOGIN_REQ* pRecv = (MSG_C2S_LOGIN_REQ*)pMsg;
//  
//  	MAPLOG(GUI_LEVEL, "[%s]请求登录[platform_type:%d, channel:%d,account:%s, token:%s]!", __FUNCTION__,
//  		 pRecv->m_byPlatformType, pRecv->m_dwChannelType, pRecv->m_szAccount, pRecv->m_szToken);
//  
//  	// 检测token
//  	if (!pRecv->m_szToken[0])
//  	{
//  		MAPLOG(ERROR_LEVEL, "[%s]token为空!", __FUNCTION__);
//  		return;
//  	}
//  
//  	// 检测帐号
//  	if (!pRecv->m_szAccount[0])
//  	{
//  		MAPLOG(ERROR_LEVEL, "[%s]帐号为空!", __FUNCTION__);
//  		return;
//  	}
//  
//  	// 检测是否白名单
//  	if (g_MapServer.IsCheckAccountWhite() && !g_pCfgMgr->IsAccountWhite(pRecv->m_szAccount))
//  	{
//  		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_SERVER_INVALID);
//  		MAPLOG(GUI_LEVEL, "[%s]不是帐号白名单不能登录[account:%s]!", __FUNCTION__, pRecv->m_szAccount);
//  		return;
//  	}
//  
//  	// 向SDK认证
//  	MSG_M2O_CHECK_USER_INFO_REQ msg;
//  	//msg.m_wLoginType = pRecv->m_wLoginType;
//  	msg.m_byPlatformType = pRecv->m_byPlatformType;
//  	msg.m_dwChannelType = pRecv->m_dwChannelType;
//  	strcpy_s(msg.m_szAccount, sizeof(msg.m_szAccount), pRecv->m_szAccount);
//  	strcpy_s(msg.m_szToken, sizeof(msg.m_szToken), pRecv->m_szToken);
//  	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);
}


VOID HandlerFromGate::OnMSG_C2S_SDK_ORDER_ID_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_SDK_ORDER_ID_REQ* pRecv = (MSG_C2S_SDK_ORDER_ID_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	CHECK_POINTER(pPlayer);

	pPlayer->GetRechargeManager()->OnNetOrderId(pRecv->m_byRechargeType, pRecv->m_wRechargeId, pRecv->m_dwContext);
}

VOID HandlerFromGate::OnMSG_C2S_RECHARGE_SUCCESS_REPORT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_RECHARGE_SUCCESS_REPORT_REQ* pRecv = (MSG_C2S_RECHARGE_SUCCESS_REPORT_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	CHECK_POINTER(pPlayer);

	//pPlayer->GetRechargeManager()->OnNetRechargeSuccessReport(pRecv->m_wRechargeId, pRecv->m_dwChannel, pRecv->m_szIP, pRecv->m_szOrderId);
}


VOID HandlerFromGate::OnMSG_C2S_LOGIN_ACCOUNT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	// 登录类型(0=测试,1=用户名注册,2=用户名登录,3=手机登录,4=手机注册,5=一键注册)

	MSG_C2S_LOGIN_ACCOUNT_REQ* pRecv = (MSG_C2S_LOGIN_ACCOUNT_REQ*)pMsg;

	const char *pszAccount = pRecv->m_szAccount;
	const char *pszPassword = pRecv->m_szPassword;
	DWORD dwChannelUID = pRecv->m_dwCPChannelID * 100000 + pRecv->m_dwSDKChannelID;

	// 检测是否角色数据加载完
	if (!g_MapServer.IsLoadRoleBaseData())
	{
		MAPLOG(ERROR_LEVEL, "角色数据还没加载![account:%s,password:%s]", pszAccount, pszPassword);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_SERVER_INVALID);
		return;
	}

	char szAccount[64] = { 0 };

	// 一键注册
	if (LOGIN_ONE_KEY == pRecv->m_byLoginType)
	{
		sprintf_s(szAccount, "a%d%.5d", time(NULL), g_Rand(1,1000000));
		pszAccount = szAccount;
	}

	// 检测用户名的合法性
	if (LOGIN_USER_NAME_REGISTER == pRecv->m_byLoginType && !CheckAccountName(pszAccount))
	{
		MAPLOG(ERROR_LEVEL, "非法的帐户名[account:%s,password:%s]", pszAccount, pszPassword);
		return;
	}

	// 检测帐号及密码的合法性
	if (!strlen(pszAccount) || !strlen(pszPassword))
	{
		MAPLOG(ERROR_LEVEL, "账号或密码为空[Guid:%d,account:%s,password:%s]", dwGuid, pszAccount, pszPassword);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_PASSWORD_INVALID);
		return;
	}

	// 检测是否在待登录状态
	if (g_MapServer.FindWaitingLoginAccount(pszAccount, dwChannelUID))
	{
		MAPLOG(GUI_LEVEL, "[%s]重复登录帐号[%s]", __FUNCTION__, pszAccount);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_REPEAT_LOGIN);
		return;
	}

	// 手机注册
	if (LOGIN_MOBILE_REGISTER == pRecv->m_byLoginType)
	{
		// 取认证码数据
		const MobileAuthData *pAuthData = g_MapServer.GetMobileAuthData(pszAccount);
		if (!pAuthData)
		{
			MAPLOG(GUI_LEVEL, "[%s]找不到手机认证码[%s]", __FUNCTION__, pszAccount);
			g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_AUTH_CODE_INVALID);
			return;
		}
		// 检测认证码
		if (pAuthData->wCode != pRecv->m_wCode)
		{
			MAPLOG(GUI_LEVEL, "[%s]手机认证码错误[%s]", __FUNCTION__, pszAccount);
			g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_AUTH_CODE_INVALID);
			return;
		}
		g_MapServer.RemoveMobileAuthData(pszAccount);
	}

	// 检测是否已在登录中
	AccountInfo *pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByAccID(dwChannelUID, pszAccount);
	if (pAccountInfo)
	{
		// 密码不正确(手机注册不需要检测密码)
		if (LOGIN_MOBILE_REGISTER != pRecv->m_byLoginType && strcmp(pszPassword, pAccountInfo->GetPassword()) != 0)
		{
			MAPLOG(GUI_LEVEL, "密码不正确[account:%s,password:%s]", pszAccount, pszPassword);
			g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_PASSWORD_INVALID);
			return;
		}

		// 把已登录的玩家踢掉
		if (dwGuid != pAccountInfo->GetGuid())
		{
			// 非角色正在下线状态
			if (pAccountInfo->GetUserState() != USER_LOGOUTING_STATE)
			{
				// 通知已登录的玩家下线
				g_MapServer.SendErrorMsgToUser(pAccountInfo->GetGuid(), ERROR_LOGIN_KICK_OUT);

				//踢掉在线玩家
				AccountInfoManager::Instance()->KickoutAccount(pAccountInfo);
			}

			// 进入待登录列表
			LoginAccountInfo temp;
			memset(&temp, 0, sizeof(temp));
			temp.dwGuid = dwGuid;
			strcpy_s(temp.szAccountName, sizeof(temp.szAccountName), pszAccount);
			strcpy_s(temp.szPassword, sizeof(temp.szPassword), pszPassword);
			temp.dwTime = time(NULL);
			temp.dwChannelUID = pAccountInfo->GetChannelUID();
			temp.byLoginType = pRecv->m_byLoginType;
			temp.byTerminalType = pRecv->m_byTerminalType;
			temp.dwServerId = pAccountInfo->GetZoneID();
			temp.bIsSDK = false;
			g_MapServer.AddWaitingLoginAccount(&temp);
		}
		else
		{
			// 通知重复登录
			Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
			if (pPlayer)
				g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_REPEAT_LOGIN);
		}
	}
	else
	{
		// 检测帐号
		CheckAccountQuery* pQuery = CheckAccountQuery::ALLOC();
		pQuery->SetIndex(MAP_QUERY_CHECK_ACCOUNT);
		pQuery->SetThreadIndex(0);
		pQuery->SetKey(dwGuid);
		pQuery->SetAccID(pszAccount);
		pQuery->SetPassword(pszPassword);
		pQuery->SetChannelUID(dwChannelUID);
		pQuery->SetLoginType(pRecv->m_byLoginType);
		pQuery->SetTerminalype(pRecv->m_byTerminalType);
		g_MapServer.MapDBQuery(pQuery);
	}

	MAPLOG(GUI_LEVEL, "普通登录[LoginType:%d,CPChannelType:%d, SDKChannelType:%d,account:%s,password:%s,ip:%s,deviceid:%s]", 
		pRecv->m_byLoginType, pRecv->m_dwCPChannelID, pRecv->m_dwSDKChannelID, pszAccount, pszPassword, pRecv->m_szIP, pRecv->m_szDeviceId);

	// 记录客户端信息
	ClientInfo tClientInfo;
	tClientInfo.byTerminalType = 0;
	strcpy_s(tClientInfo.szDeviceId, sizeof(tClientInfo.szDeviceId), pRecv->m_szDeviceId);
	strcpy_s(tClientInfo.szIP, sizeof(tClientInfo.szIP), pRecv->m_szIP);
	CGameLogic::Instance()->AddClientInfo(dwChannelUID, pRecv->m_szAccount, tClientInfo);
}

VOID HandlerFromGate::OnMSG_C2S_LOGIN_SELECT_SERVER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{

	MSG_C2S_LOGIN_SELECT_SERVER_REQ* pRecv = (MSG_C2S_LOGIN_SELECT_SERVER_REQ*)pMsg;

	// 检测选择的服务器是否开放
	if (!g_MapServer.GetServerInfo(pRecv->m_dwSelectZoneId))
	{
		MAPLOG(ERROR_LEVEL, "选择服务器还木有开放[ServerId:%d]", pRecv->m_dwSelectZoneId);
		g_MapServer.SendErrorMsgToUser(dwGuid, ERROR_LOGIN_SERVER_INVALID);
		return;
	}

	// 检测是否已在登录中
	AccountInfo *pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwGuid);
	if (!pAccountInfo)
	{
		MAPLOG(ERROR_LEVEL, "还没登录,不能选择服务器[Guid:%d]", dwGuid);
		return;
	}

	// 检测当前状态(等待选择服务器)
	if (pAccountInfo->GetUserState() != USER_SELECT_SERVER_STATE)
	{
		MAPLOG(ERROR_LEVEL, "状态不对,不能选择服务器[account:%s, state:%d]",
			pAccountInfo->GetAccID(),pAccountInfo->GetUserState());
		return;
	}

	// 查询选择的服务器对应的角色
	SelectCharListQuery* pQuery = SelectCharListQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_CHAR_LIST);
	pQuery->SetThreadIndex(pAccountInfo->GetDBIndex());
	pQuery->SetKey(dwGuid);
	pQuery->SetPlatform(pAccountInfo->GetChannelUID());
	pQuery->SetZoneID(pRecv->m_dwSelectZoneId);
	pQuery->SetAccID(pAccountInfo->GetAccID());
	g_MapServer.MapDBQuery(pQuery);

	// 更新帐号状态
	pAccountInfo->SetUserState(USER_SELECT_ROLE_LIST);

	// 记录最新登录的服务器
	pAccountInfo->SetZoneId(pRecv->m_dwSelectZoneId);
}

VOID HandlerFromGate::OnMSG_C2S_LOGIN_CREATE_ROLE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_LOGIN_CREATE_ROLE_REQ* pRecv = (MSG_C2S_LOGIN_CREATE_ROLE_REQ*)pMsg;

	// 检测是否已在登录中
	AccountInfo *pAccountInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwGuid);
	if (!pAccountInfo)
	{
		MAPLOG(GUI_LEVEL, "还没登录,不能选择服务器[Guid:%d]", dwGuid);
		return;
	}

	// 检测当前状态(等待选择服务器)
	if (pAccountInfo->GetUserState() != USER_READY_CREATE_ROLE_STATE)
	//if (pAccountInfo->GetUserState() != USER_CREATE_ROLE_STATE)
  	{
  		MAPLOG(GUI_LEVEL, "状态不对,不能创建角色[account:%s, state:%d]",
			pAccountInfo->GetAccID(), pAccountInfo->GetUserState());
  		return;
  	}

	// 检测是否已有角色
	if (PlayerBaseDataManager::Instance()->GetPlayerBaseDataByAccount(pAccountInfo->GetZoneID(), 
		pAccountInfo->GetChannelUID(), pAccountInfo->GetAccID()))
	{
		MAPLOG(ERROR_LEVEL, "此服已有角色,不能创建角色[account:%s, state:%d, serverid:%d]",
			pAccountInfo->GetAccID(), pAccountInfo->GetUserState(), pAccountInfo->GetZoneID());
		return;
	}
 
 	// todo::检测选择的服务器是否合法

	char szRoleName[MAX_ROLE_LEN] = { 0 };

	// 新手引导创建(todo::目前已不使用)
	if (pRecv->m_byType)
	{
		while (true)
		{
			DWORD dwNameId = g_Rand(1, 100000000);
			sprintf_s(szRoleName, sizeof(szRoleName)-1, "%d", dwNameId);
			if (!PlayerBaseDataManager::Instance()->GetPlayerBaseDataByName(szRoleName))
				break;
		}
	}
	else
	{
		// todo::检测名称
		if (0 == pRecv->m_szRoleName[0])
		{
			g_MapServer.SendErrorMsgToUser(pAccountInfo->GetGuid(), ERROR_LOGIN_ROLENAME_INVALID);
			MAPLOG(GUI_LEVEL, "角色名称不合法[account:%s]", pAccountInfo->GetAccID());
			return;
		}

		strcpy_s(szRoleName, sizeof(szRoleName)-1, pRecv->m_szRoleName);

		// 检测是否已被使用
 		if (PlayerBaseDataManager::Instance()->GetPlayerBaseDataByName(szRoleName))
 		{
 			// 通知玩家名称已被使用
 			g_MapServer.SendErrorMsgToUser(pAccountInfo->GetGuid(), ERROR_LOGIN_ENAME_EXIST);
 			MAPLOG(GUI_LEVEL, "角色重名[account:%s]", pAccountInfo->GetAccID());
 			return;
 		}
	}

	// 检测名称是否已被使用
	if (g_MapServer.IsReadyToUseRoleName(szRoleName))
	{
		// 通知玩家名称已被使用
		g_MapServer.SendErrorMsgToUser(pAccountInfo->GetGuid(), ERROR_LOGIN_ENAME_EXIST);
		MAPLOG(GUI_LEVEL, "角色重名[account:%s]", pAccountInfo->GetAccID());
		return;
	}

	// 检测是否已在创建中
	if (g_MapServer.IsInCreatingRole(pAccountInfo->GetAccID(), pAccountInfo->GetChannelUID(), pAccountInfo->GetZoneID()))
	{
		g_MapServer.SendErrorMsgToUser(pAccountInfo->GetGuid(), ERROR_ROLE_ALREADY_EXIST);
		MAPLOG(GUI_LEVEL, "正在创建角色中[account:%s,Platform:%d, ServerId:%d]", 
			pAccountInfo->GetAccID(), pAccountInfo->GetChannelUID(), pAccountInfo->GetZoneID());
		return;
	}


	// 进入创建角色状态
	pAccountInfo->SetUserState(USER_CREATE_ROLE_STATE);
 
 	// 请求创建角色
	CreateCharInfoQuery* pQuery = CreateCharInfoQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_CREATE_CHAR);
	pQuery->SetThreadIndex(pAccountInfo->GetDBIndex());
 	pQuery->SetKey(dwGuid);
	pQuery->SetChannelUID(pAccountInfo->GetChannelUID());
 	pQuery->SetZoneID(pAccountInfo->GetZoneID());
 	pQuery->SetAccID(pAccountInfo->GetAccID());
	pQuery->SetRoleName(szRoleName);
	pQuery->SetGender(pRecv->m_bySex);
	pQuery->SetCreateType(pRecv->m_byType);
	pQuery->SetTerminalType(pAccountInfo->GetTerminalType());
 	g_MapServer.MapDBQuery(pQuery);

	// 加入准备使用列表
	g_MapServer.AddReadyToUseRoleName(szRoleName);

	// 记录为正在创建
	g_MapServer.AddCreatingRoleData(pAccountInfo->GetAccID(), pAccountInfo->GetChannelUID(), pAccountInfo->GetZoneID());
}

VOID HandlerFromGate::OnMSG_C2S_ROLE_NAME_MODIFY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_ROLE_NAME_MODIFY_REQ* pRecv = (MSG_C2S_ROLE_NAME_MODIFY_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	CHECK_POINTER(pPlayer);

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetRoleNameModify(pRecv->m_szRoleName);
}

VOID HandlerFromGate::OnMSG_C2S_START_GAME_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_START_GAME_REQ* pRecv = (MSG_C2S_START_GAME_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	CHECK_POINTER(pPlayer);

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetStartGame();
}



VOID HandlerFromGate::OnMSG_C2S_ROLE_DETAILS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	MSG_C2S_ROLE_DETAILS_REQ* pRecv = (MSG_C2S_ROLE_DETAILS_REQ*)pMsg;

	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	CHECK_POINTER(pPlayer);

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetRoleDetails(pRecv->m_dwTargetId, pRecv->m_byType);
}


VOID HandlerFromGate::OnMSG_C2S_ROLE_PRO_RECOVER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ROLE_PRO_RECOVER_REQ* pRecv = (MSG_C2S_ROLE_PRO_RECOVER_REQ*)pMsg;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetPropertyRecover(pRecv->m_byType);
}

VOID HandlerFromGate::OnMSG_C2S_HEAD_IMAGE_REPLACE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HEAD_IMAGE_REPLACE_REQ* pRecv = (MSG_C2S_HEAD_IMAGE_REPLACE_REQ*)pMsg;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetReplaceHeadImage(pRecv->m_dwHeadImageId);
}




VOID HandlerFromGate::OnMSG_C2S_BATTLE_START_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_BATTLE_START_REQ* pRecv = (MSG_C2S_BATTLE_START_REQ*)pMsg;

	CBattleManager::Instance()->OnBattleStar(pPlayer, pRecv->m_byType, 
		pRecv->m_dwParam1, pRecv->m_dwParam2, pRecv->m_dwParam3, pRecv->m_dwParam4, pRecv->m_dwPower, pRecv->m_szSign);
}


VOID HandlerFromGate::OnMSG_C2S_BATTLE_END_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_BATTLE_END_REQ* pRecv = (MSG_C2S_BATTLE_END_REQ*)pMsg;
	
	CBattleManager::Instance()->OnBattleOver(pPlayer, pRecv->m_byType, pRecv->m_nWinValue, pRecv->m_byStar, pRecv->m_dwContext, 
		pRecv->m_dwContext1, pRecv->m_MonsterList, pRecv->m_byMonsterNum, pRecv->m_dwTotalDamge, pRecv->m_dwPower, pRecv->m_szSign);
}

VOID HandlerFromGate::OnMSG_C2S_HELP_BATTLE_TARGET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HELP_BATTLE_TARGET_REQ* pRecv = (MSG_C2S_HELP_BATTLE_TARGET_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetHelpBattleTargetList();
}



VOID HandlerFromGate::OnMSG_C2S_RANK_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RANK_LIST_REQ* pRecv = (MSG_C2S_RANK_LIST_REQ*)pMsg;

	CRankManager::Instance()->OnNetRankList(pPlayer, pRecv->m_byType, pRecv->m_bySubType);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_RANK_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_RANK_LIST_REQ* pRecv = (MSG_C2S_GUILD_RANK_LIST_REQ*)pMsg;

	CRankManager::Instance()->OnNetGuildRankList(pPlayer, pRecv->m_byRankType);
}

VOID HandlerFromGate::OnMSG_C2S_MY_RANK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_MY_RANK_REQ* pRecv = (MSG_C2S_MY_RANK_REQ*)pMsg;

	CRankManager::Instance()->OnNetMyRank(pPlayer, pRecv->m_wRankType);
}





VOID HandlerFromGate::OnMSG_C2S_RANK_LIKE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

// 	MSG_C2S_RANK_LIKE_REQ* pRecv = (MSG_C2S_RANK_LIKE_REQ*)pMsg;
// 
// 	CRankManager::Instance()->OnNetLike(pPlayer, pRecv->m_byType, pRecv->m_dwTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_RANK_LEAVE_MESSAGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// 	MSG_C2S_RANK_LEAVE_MESSAGE_REQ* pRecv = (MSG_C2S_RANK_LEAVE_MESSAGE_REQ*)pMsg;
// 
// 	CRankManager::Instance()->OnNetLeaveMessage(pPlayer, pRecv->m_szText);
}

VOID HandlerFromGate::OnMSG_C2S_RANK_LEAVE_MESSAGE_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// 	MSG_C2S_RANK_LEAVE_MESSAGE_LIST_REQ* pRecv = (MSG_C2S_RANK_LEAVE_MESSAGE_LIST_REQ*)pMsg;
// 
// 	CRankManager::Instance()->OnNetLeaveMessageList(pPlayer);
}


// 郭富城
VOID HandlerFromGate::OnMSG_C2M_GM_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2M_GM_REQ* pRecv = (MSG_C2M_GM_REQ*)pMsg;

	//ChatManager::Instance()->OnNetChat(pPlayer, &pRecv->m_tData);
	GMManager::Instance()->OnNetGMMsg(pPlayer, pRecv->m_wCmd, pRecv->m_dwValue1, pRecv->m_dwValue2, pRecv->m_dwValue3);
}

VOID HandlerFromGate::OnMSG_C2S_CHAT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHAT_REQ* pRecv = (MSG_C2S_CHAT_REQ*)pMsg;

	ChatManager::Instance()->OnNetChat(pPlayer, &pRecv->m_tData);
}

VOID HandlerFromGate::OnMSG_C2S_CHAT_VOICE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHAT_VOICE_REQ* pRecv = (MSG_C2S_CHAT_VOICE_REQ*)pMsg;

	//ChatManager::Instance()->OnNetChatVoice(pPlayer, pRecv->m_dwVoiceId);
}

VOID HandlerFromGate::OnMSG_C2S_CHAT_RECORD_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHAT_RECORD_LIST_REQ* pRecv = (MSG_C2S_CHAT_RECORD_LIST_REQ*)pMsg;

	ChatManager::Instance()->OnNetChatRecordList(pPlayer, pRecv->m_byChatType);
}



VOID HandlerFromGate::OnMSG_C2S_SHOP_GOODS_BUY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SHOP_GOODS_BUY_REQ* pRecv = (MSG_C2S_SHOP_GOODS_BUY_REQ*)pMsg;

	pPlayer->GetShopManager()->OnGoodsBuy(pRecv->m_byType, pRecv->m_wParam1, pRecv->m_wParam2, pRecv->m_wParam3);
}

VOID HandlerFromGate::OnMSG_C2S_SHOP_GOODS_SELL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SHOP_GOODS_SELL_REQ* pRecv = (MSG_C2S_SHOP_GOODS_SELL_REQ*)pMsg;

	pPlayer->GetShopManager()->OnGoodsSell(pRecv->m_byObjType, pRecv->m_GoodsList, pRecv->m_byGoodsNum);
}

VOID HandlerFromGate::OnMSG_C2S_SHOP_GOODS_BUY_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ* pRecv = (MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ*)pMsg;

	pPlayer->GetShopManager()->OnGoodsBuyRecord(pRecv->m_byType);
}

VOID HandlerFromGate::OnMSG_C2S_SHOP_LIMIT_GOODS_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SHOP_LIMIT_GOODS_LIST_REQ* pRecv = (MSG_C2S_SHOP_LIMIT_GOODS_LIST_REQ*)pMsg;

	pPlayer->GetShopManager()->OnNetLimitGoodsList(pRecv->m_byType);
}

VOID HandlerFromGate::OnMSG_C2S_SHOP_REFRESH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SHOP_REFRESH_REQ* pRecv = (MSG_C2S_SHOP_REFRESH_REQ*)pMsg;

	pPlayer->GetShopManager()->OnGoodsRefresh(pRecv->m_byShopType, pRecv->m_byRefreshType);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_SHOP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_SHOP_DATA_REQ* pRecv = (MSG_C2S_HERO_SHOP_DATA_REQ*)pMsg;

	pPlayer->GetShopManager()->OnHeroShopReq();
}

VOID HandlerFromGate::OnMSG_C2S_AWAKEN_SHOP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_AWAKEN_SHOP_DATA_REQ* pRecv = (MSG_C2S_AWAKEN_SHOP_DATA_REQ*)pMsg;

	pPlayer->GetShopManager()->OnNetAwakenShopReq();
}

VOID HandlerFromGate::OnMSG_C2S_STAR_SOUL_SHOP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_STAR_SOUL_SHOP_DATA_REQ* pRecv = (MSG_C2S_STAR_SOUL_SHOP_DATA_REQ*)pMsg;

	pPlayer->GetShopManager()->OnNetStarSoulShopReq();
}

VOID HandlerFromGate::OnMSG_C2S_DIRECT_SHOP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DIRECT_SHOP_DATA_REQ* pRecv = (MSG_C2S_DIRECT_SHOP_DATA_REQ*)pMsg;

	pPlayer->GetShopManager()->OnNetDirectBuyShopReq();
}

VOID HandlerFromGate::OnMSG_C2S_CRYSTAL_SHOP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CRYSTAL_SHOP_DATA_REQ* pRecv = (MSG_C2S_CRYSTAL_SHOP_DATA_REQ*)pMsg;

	pPlayer->GetShopManager()->OnNetCrystalShopData();

}



VOID HandlerFromGate::OnMSG_C2S_TAKE_DIRECT_BUY_GOODS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_DIRECT_BUY_GOODS_REQ* pRecv = (MSG_C2S_TAKE_DIRECT_BUY_GOODS_REQ*)pMsg;

	pPlayer->GetShopManager()->OnNetTakeDirectBuyGoods(pRecv->m_wGoodsId);
}

VOID HandlerFromGate::OnMSG_C2S_FB_MAIN_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_MAIN_DATA_REQ* pRecv = (MSG_C2S_FB_MAIN_DATA_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetMainFB().OnNetFBData();
}

VOID HandlerFromGate::OnMSG_C2S_FB_DAILY_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_DAILY_DATA_REQ* pRecv = (MSG_C2S_FB_DAILY_DATA_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetDaily().OnDailyDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_FB_HERO_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_HERO_DATA_REQ* pRecv = (MSG_C2S_FB_HERO_DATA_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetHeroFB().OnHeroFBData();
}

VOID HandlerFromGate::OnMSG_C2S_FB_ELITE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_ELITE_DATA_REQ* pRecv = (MSG_C2S_FB_ELITE_DATA_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetEliteFB().OnNetFBData();
}

VOID HandlerFromGate::OnMSG_C2S_FB_RESET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_RESET_REQ* pRecv = (MSG_C2S_FB_RESET_REQ*)pMsg;

	pPlayer->GetFubenManager()->OnNetFBReset(pRecv->m_byMapType, pRecv->m_dwParam1, pRecv->m_dwParam2);
}

VOID HandlerFromGate::OnMSG_C2S_FB_ONE_KEY_TAKE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_ONE_KEY_TAKE_REWARD_REQ* pRecv = (MSG_C2S_FB_ONE_KEY_TAKE_REWARD_REQ*)pMsg;

	pPlayer->GetFubenManager()->OnNetOneKeyTakeReward(pRecv->m_byFBType, pRecv->m_wChapter);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ* pRecv = (MSG_C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetMainFB().OnNetTakeChapterReward(pRecv->m_wRewardLevel);
}







VOID HandlerFromGate::OnMSG_C2S_FB_CLEAN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_CLEAN_REQ* pRecv = (MSG_C2S_FB_CLEAN_REQ*)pMsg;

	pPlayer->GetFubenManager()->OnNetSweep(pRecv->m_byFBType, pRecv->m_dwParam1, pRecv->m_dwParam2);
}



VOID HandlerFromGate::OnMSG_C2S_TOWER_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOWER_DATA_REQ* pRecv = (MSG_C2S_TOWER_DATA_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetTower().OnTowerFBData();
}

VOID HandlerFromGate::OnMSG_C2S_TOWER_BUY_TIMES_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOWER_BUY_TIMES_REQ* pRecv = (MSG_C2S_TOWER_BUY_TIMES_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetTower().OnNetBattleTimesBuy();
}

VOID HandlerFromGate::OnMSG_C2S_TOWER_RESET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOWER_RESET_REQ* pRecv = (MSG_C2S_TOWER_RESET_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetTower().OnNetFBReset(false);
}

VOID HandlerFromGate::OnMSG_C2S_TOWER_SELECT_PRO_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOWER_SELECT_PRO_REQ* pRecv = (MSG_C2S_TOWER_SELECT_PRO_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetTower().OnNetSelectPro(pRecv->m_byStar, pRecv->m_byProIndex);
}

VOID HandlerFromGate::OnMSG_C2S_TOWER_BUY_STAR_GOODS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOWER_BUY_STAR_GOODS_REQ* pRecv = (MSG_C2S_TOWER_BUY_STAR_GOODS_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetTower().OnNetBuyStarGoods();
}

VOID HandlerFromGate::OnMSG_C2S_TOWER_ONE_KEY_CLEAN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOWER_ONE_KEY_CLEAN_REQ* pRecv = (MSG_C2S_TOWER_ONE_KEY_CLEAN_REQ*)pMsg;

	pPlayer->GetFubenManager()->GetTower().OnNetOneKeyClean(pRecv->m_byIsToHistoryMap);
}




VOID HandlerFromGate::OnMSG_C2S_FB_AWARD_GET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FB_AWARD_GET_REQ* pRecv = (MSG_C2S_FB_AWARD_GET_REQ*)pMsg;

	pPlayer->GetFubenManager()->OnNetTakeReward(pRecv->m_byAwardType, pRecv->m_byFBType, pRecv->m_wChapter, pRecv->m_byIndex);
}


VOID HandlerFromGate::OnMSG_C2S_ITEM_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ITEM_LIST_REQ* pRecv = (MSG_C2S_ITEM_LIST_REQ*)pMsg;

	if (pPlayer->GetItemManager())
		pPlayer->GetItemManager()->OnNetItemList();
}

VOID HandlerFromGate::OnMSG_C2S_ITEM_USE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ITEM_USE_REQ* pRecv = (MSG_C2S_ITEM_USE_REQ*)pMsg;

	if (pPlayer->GetItemManager())
		pPlayer->GetItemManager()->OnNetUseItem(pRecv->m_dwItemId, pRecv->m_wUseNum, pRecv->m_dwGiftItemId);
}


VOID HandlerFromGate::OnMSG_C2S_ITEM_FP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ITEM_FP_REQ* pRecv = (MSG_C2S_ITEM_FP_REQ*)pMsg;

	if (pPlayer->GetItemManager())
		pPlayer->GetItemManager()->OnNetFP(pRecv->m_byType);
}

VOID HandlerFromGate::OnMSG_C2S_ITEM_COMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ITEM_COMPOSE_REQ* pRecv = (MSG_C2S_ITEM_COMPOSE_REQ*)pMsg;

	if (pPlayer->GetItemManager())
		pPlayer->GetItemManager()->OnNetCompose(pRecv->m_wItemId);
}

VOID HandlerFromGate::OnMSG_C2S_ITEM_DECOMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ITEM_DECOMPOSE_REQ* pRecv = (MSG_C2S_ITEM_DECOMPOSE_REQ*)pMsg;

	if (pPlayer->GetItemManager())
		pPlayer->GetItemManager()->OnNetDecompose(pRecv->m_wItemId, pRecv->m_wItemNum);
}





VOID HandlerFromGate::OnMSG_C2S_EMAIL_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EMAIL_LIST_REQ* pRecv = (MSG_C2S_EMAIL_LIST_REQ*)pMsg;

 	if (pPlayer->GetEmailManager())
		pPlayer->GetEmailManager()->OnNetEmailList();
}

VOID HandlerFromGate::OnMSG_C2S_HERO_BASE_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_BASE_LIST_REQ* pRecv = (MSG_C2S_HERO_BASE_LIST_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetHeroBaseList();
}

VOID HandlerFromGate::OnMSG_C2S_HERO_INFO_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_INFO_REQ* pRecv = (MSG_C2S_HERO_INFO_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetHeroInfo(pRecv->m_dwUId);
}

// VOID HandlerFromGate::OnMSG_C2S_FRAGMENT_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
// {
// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// // 	MSG_C2S_FRAGMENT_LIST_REQ* pRecv = (MSG_C2S_FRAGMENT_LIST_REQ*)pMsg;
// // 
// // 	if (pPlayer->GetHeroManager())
// // 		pPlayer->GetHeroManager()->OnFragmentList();
// }

VOID HandlerFromGate::OnMSG_C2S_HERO_TO_BATTLE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_TO_BATTLE_REQ* pRecv = (MSG_C2S_HERO_TO_BATTLE_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnHeroBattle(pRecv->m_byPos, pRecv->m_dwUId);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_COLLECTION_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_COLLECTION_LIST_REQ* pRecv = (MSG_C2S_HERO_COLLECTION_LIST_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetCollectionList();
}


VOID HandlerFromGate::OnMSG_C2S_HERO_LEVEL_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_LEVEL_UP_REQ* pRecv = (MSG_C2S_HERO_LEVEL_UP_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnLevelUpEx(pRecv->m_dwHeroUId, pRecv->m_arCostHeroList, pRecv->m_wCostHeroNum);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_BREAK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_BREAK_REQ* pRecv = (MSG_C2S_HERO_BREAK_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnBreak(pRecv->m_dwHeroUId, pRecv->m_arCostSameNameHeroList, pRecv->m_wCostSameNameHeroNum, 
		pRecv->m_arCostSameQualityHeroList, pRecv->m_wCostSameQualityHeroNum, pRecv->m_arCostSameProfessionHeroList, pRecv->m_wCostSameProfessionHeroNum);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_TRAIN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_TRAIN_REQ* pRecv = (MSG_C2S_HERO_TRAIN_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetTrain(pRecv->m_dwHeroUId, pRecv->m_byCostType, pRecv->m_byTrainTimes);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_SUMMON_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_SUMMON_REQ* pRecv = (MSG_C2S_HERO_SUMMON_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnSummon(pRecv->m_byType);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_DESTINY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_DESTINY_REQ* pRecv = (MSG_C2S_HERO_DESTINY_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetDestiny(pRecv->m_dwHeroUID);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_SELL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_SELL_REQ* pRecv = (MSG_C2S_HERO_SELL_REQ*)pMsg;
}

VOID HandlerFromGate::OnMSG_C2S_REINFORCEMENT_TO_BATTLE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REINFORCEMENT_TO_BATTLE_REQ* pRecv = (MSG_C2S_REINFORCEMENT_TO_BATTLE_REQ*)pMsg;

	pPlayer->GetHeroManager()->OnReinforcementBattle(pRecv->m_byPos, pRecv->m_dwUId);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_SUMMON_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_SUMMON_DATA_REQ* pRecv = (MSG_C2S_HERO_SUMMON_DATA_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetSummonData();
}

VOID HandlerFromGate::OnMSG_C2S_HERO_AWAKEN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_AWAKEN_REQ* pRecv = (MSG_C2S_HERO_AWAKEN_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetAwaken(pRecv->m_dwUId, pRecv->m_CollectionList, pRecv->m_wCostHeroNum);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_EQUIP_AWAKEN_ITEM_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_EQUIP_AWAKEN_ITEM_REQ* pRecv = (MSG_C2S_HERO_EQUIP_AWAKEN_ITEM_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetEquipAwakeningItem(pRecv->m_dwUId, pRecv->m_wAwakenItemId);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_TRAIN_REPLACE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_TRAIN_REPLACE_REQ* pRecv = (MSG_C2S_HERO_TRAIN_REPLACE_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetTrainReplace(pRecv->m_dwHeroUId);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_TRAIN_REWARD_GET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_TRAIN_REWARD_GET_REQ* pRecv = (MSG_C2S_HERO_TRAIN_REWARD_GET_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetGetTrainReward(pRecv->m_dwHeroUId, pRecv->m_byProType);
}

VOID HandlerFromGate::OnMSG_C2S_HERO_EVOLUTION_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_EVOLUTION_REQ* pRecv = (MSG_C2S_HERO_EVOLUTION_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetEvolution(pRecv->m_dwHeroUId);
}


VOID HandlerFromGate::OnMSG_C2S_HERO_VIP_SUMMON_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_VIP_SUMMON_REQ* pRecv = (MSG_C2S_HERO_VIP_SUMMON_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnNetVIPSummon(pRecv->m_dwCamp);
}


VOID HandlerFromGate::OnMSG_C2S_FRIEND_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_LIST_REQ* pRecv = (MSG_C2S_FRIEND_LIST_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnFriendList();
}

VOID HandlerFromGate::OnMSG_C2S_BLACK_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_BLACK_LIST_REQ* pRecv = (MSG_C2S_BLACK_LIST_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnBlackList();
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_APPLY_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_APPLY_LIST_REQ* pRecv = (MSG_C2S_FRIEND_APPLY_LIST_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnApplyList();
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_RECOMMEND_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_RECOMMEND_LIST_REQ* pRecv = (MSG_C2S_FRIEND_RECOMMEND_LIST_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnRecommendList();
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_ADD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_ADD_REQ* pRecv = (MSG_C2S_FRIEND_ADD_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnFriendAdd(pRecv->m_dwTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_TARGET_FIND_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_TARGET_FIND_REQ* pRecv = (MSG_C2S_FRIEND_TARGET_FIND_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnNetTargetFind(pRecv->m_szTargetName);
}



VOID HandlerFromGate::OnMSG_C2S_FRIEND_DEL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_DEL_REQ* pRecv = (MSG_C2S_FRIEND_DEL_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnFriendDel(pRecv->m_dwTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_APPLY_REPLY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_APPLY_REPLY_REQ* pRecv = (MSG_C2S_FRIEND_APPLY_REPLY_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnFriendApplyReply(pRecv->m_dwTargetId, pRecv->m_byAccept);
}

VOID HandlerFromGate::OnMSG_C2S_BLACK_ADD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_BLACK_ADD_REQ* pRecv = (MSG_C2S_BLACK_ADD_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnBlackAdd(pRecv->m_dwTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_BLACK_DEL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_BLACK_DEL_REQ* pRecv = (MSG_C2S_BLACK_DEL_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnBlackDel(pRecv->m_dwTargetId);
}


VOID HandlerFromGate::OnMSG_C2S_FRIEND_ENERGY_GIVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_ENERGY_GIVE_REQ* pRecv = (MSG_C2S_FRIEND_ENERGY_GIVE_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnEnergyGive(pRecv->m_dwTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_ENERGY_GET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_ENERGY_GET_REQ* pRecv = (MSG_C2S_FRIEND_ENERGY_GET_REQ*)pMsg;

	if (pPlayer->GetFriendManager())
		pPlayer->GetFriendManager()->OnTakeEnergy(pRecv->m_TargetList, pRecv->m_byTargetNum);
}

VOID HandlerFromGate::OnMSG_C2S_REWARD_CENTER_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REWARD_CENTER_LIST_REQ* pRecv = (MSG_C2S_REWARD_CENTER_LIST_REQ*)pMsg;

	pPlayer->GetRewardCenterManager()->OnNetRewardList();
}

VOID HandlerFromGate::OnMSG_C2S_REWARD_CENTER_GET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REWARD_CENTER_GET_REQ* pRecv = (MSG_C2S_REWARD_CENTER_GET_REQ*)pMsg;

	pPlayer->GetRewardCenterManager()->OnNetRewardGet(pRecv->m_arRewardIdList, pRecv->m_byRewardNum);
}

VOID HandlerFromGate::OnMSG_C2S_SIGN_IN_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SIGN_IN_DATA_REQ* pRecv = (MSG_C2S_SIGN_IN_DATA_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetSignInDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_SIGN_IN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SIGN_IN_REQ* pRecv = (MSG_C2S_SIGN_IN_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetSignIn();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ* pRecv = (MSG_C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetTakeLuxurySignReward(pRecv->m_wSignInId);
}

VOID HandlerFromGate::OnMSG_C2S_LUXURY_SIGN_IN_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LUXURY_SIGN_IN_DATA_REQ* pRecv = (MSG_C2S_LUXURY_SIGN_IN_DATA_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetLuxurySignData();
}


VOID HandlerFromGate::OnMSG_C2S_YCS_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_YCS_DATA_REQ* pRecv = (MSG_C2S_YCS_DATA_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetYCSDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_YCS_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_YCS_REWARD_REQ* pRecv = (MSG_C2S_YCS_REWARD_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetYCSReward();
}

VOID HandlerFromGate::OnMSG_C2S_TQT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TQT_DATA_REQ* pRecv = (MSG_C2S_TQT_DATA_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetTQTDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_TQT_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TQT_REWARD_REQ* pRecv = (MSG_C2S_TQT_REWARD_REQ*)pMsg;

	if (pPlayer->GetActivityManager())
		pPlayer->GetActivityManager()->OnNetTQTReward();
}


VOID HandlerFromGate::OnMSG_C2S_SEVEN_DAY_TARGET_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SEVEN_DAY_TARGET_LIST_REQ* pRecv = (MSG_C2S_SEVEN_DAY_TARGET_LIST_REQ*)pMsg;

	if (pPlayer->GetSevenDayManager())
		pPlayer->GetSevenDayManager()->OnNetTargetList();
}

VOID HandlerFromGate::OnMSG_C2S_SEVEN_DAY_GET_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SEVEN_DAY_GET_REWARD_REQ* pRecv = (MSG_C2S_SEVEN_DAY_GET_REWARD_REQ*)pMsg;

	if (pPlayer->GetSevenDayManager())
		pPlayer->GetSevenDayManager()->OnNetTargetAward(pRecv->m_wTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_SEVEN_DAY_COMPLETION_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SEVEN_DAY_COMPLETION_REWARD_REQ* pRecv = (MSG_C2S_SEVEN_DAY_COMPLETION_REWARD_REQ*)pMsg;

	if (pPlayer->GetSevenDayManager())
		pPlayer->GetSevenDayManager()->OnNetCompletionReward(pRecv->m_wComplecton_num);
}



VOID HandlerFromGate::OnMSG_C2S_PLAYER_ZHANBU_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PLAYER_ZHANBU_DATA_REQ* pRecv = (MSG_C2S_PLAYER_ZHANBU_DATA_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetPlayerZhanBuData();
}

VOID HandlerFromGate::OnMSG_C2S_CARD_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CARD_LIST_REQ* pRecv = (MSG_C2S_CARD_LIST_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetCardList();
}

VOID HandlerFromGate::OnMSG_C2S_FAIRY_POKEDEX_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

// 	MSG_C2S_FAIRY_POKEDEX_LIST_REQ* pRecv = (MSG_C2S_FAIRY_POKEDEX_LIST_REQ*)pMsg;
// 
// 	if (pPlayer->GetZhanBuManager())
// 		pPlayer->GetZhanBuManager()->OnNetPokedexList();
}

VOID HandlerFromGate::OnMSG_C2S_CARD_LEVEL_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CARD_LEVEL_UP_REQ* pRecv = (MSG_C2S_CARD_LEVEL_UP_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetLevelUp(pRecv->m_dwFairyUId, pRecv->m_arCostFairyList, pRecv->m_wCostFairyNum);
}

VOID HandlerFromGate::OnMSG_C2S_CARD_BREAK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CARD_BREAK_REQ* pRecv = (MSG_C2S_CARD_BREAK_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetBreak(pRecv->m_dwFairyUId);
}

VOID HandlerFromGate::OnMSG_C2S_CARD_REFRESH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CARD_REFRESH_REQ* pRecv = (MSG_C2S_CARD_REFRESH_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetCardRefresh(pRecv->m_byRefreshType);
}

VOID HandlerFromGate::OnMSG_C2S_OPEN_CARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_OPEN_CARD_REQ* pRecv = (MSG_C2S_OPEN_CARD_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetOpenCard(pRecv->m_byType, pRecv->m_arCardIdxList, pRecv->m_wCardIdxListNum);
}

VOID HandlerFromGate::OnMSG_C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ* pRecv = (MSG_C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetLowCardGroupRefreshTimesUpdate();
}

VOID HandlerFromGate::OnMSG_C2S_CELLECTION_GROUP_ACTIVATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CELLECTION_GROUP_ACTIVATE_REQ* pRecv = (MSG_C2S_CELLECTION_GROUP_ACTIVATE_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetCellectionGroupActivate(pRecv->m_wBookId, pRecv->m_wGroupId);
}

VOID HandlerFromGate::OnMSG_C2S_CELLECTION_GROUP_STAR_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CELLECTION_GROUP_STAR_UP_REQ* pRecv = (MSG_C2S_CELLECTION_GROUP_STAR_UP_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetCellectionGroupStarUp(pRecv->m_wBookId, pRecv->m_wGroupId);
}

VOID HandlerFromGate::OnMSG_C2S_CELLECTION_GROUP_RESET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CELLECTION_GROUP_RESET_REQ* pRecv = (MSG_C2S_CELLECTION_GROUP_RESET_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetCellectionGroupReset(pRecv->m_wBookId, pRecv->m_wGroupId);
}



VOID HandlerFromGate::OnMSG_C2S_COLLECTION_BOOK_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_COLLECTION_BOOK_LIST_REQ* pRecv = (MSG_C2S_COLLECTION_BOOK_LIST_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetCollectionBookList();
}

VOID HandlerFromGate::OnMSG_C2S_COLLECTION_EQUIP_CARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_COLLECTION_EQUIP_CARD_REQ* pRecv = (MSG_C2S_COLLECTION_EQUIP_CARD_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetEquipCard(pRecv->m_wFormationPos, pRecv->m_wCardSlot, pRecv->m_dwCardUID);
}

VOID HandlerFromGate::OnMSG_C2S_COLLECTION_UNLOAD_CARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_COLLECTION_UNLOAD_CARD_REQ* pRecv = (MSG_C2S_COLLECTION_UNLOAD_CARD_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetUnloadCard(pRecv->m_wFormationPos, pRecv->m_wCardSlot);
}

VOID HandlerFromGate::OnMSG_C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ* pRecv = (MSG_C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetAcivateAchievement();
}

VOID HandlerFromGate::OnMSG_C2S_FAIRY_RECRUIT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FAIRY_RECRUIT_REQ* pRecv = (MSG_C2S_FAIRY_RECRUIT_REQ*)pMsg;

	if (pPlayer->GetZhanBuManager())
		pPlayer->GetZhanBuManager()->OnNetOpenCard(pRecv->m_byRecruitType, NULL, 0);
}

VOID HandlerFromGate::OnMSG_C2S_FAIRY_POKEDEX_STAR_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

// 	MSG_C2S_FAIRY_POKEDEX_STAR_UP_REQ* pRecv = (MSG_C2S_FAIRY_POKEDEX_STAR_UP_REQ*)pMsg;
// 
// 	if (pPlayer->GetZhanBuManager())
// 		pPlayer->GetZhanBuManager()->OnNetPokedexStarUp(pRecv->m_wPokedexId);
}

VOID HandlerFromGate::OnMSG_C2S_FAIRY_POKEDEX_REVIVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

// 	MSG_C2S_FAIRY_POKEDEX_REVIVE_REQ* pRecv = (MSG_C2S_FAIRY_POKEDEX_REVIVE_REQ*)pMsg;
// 
// 	if (pPlayer->GetZhanBuManager())
// 		pPlayer->GetZhanBuManager()->OnNetPokedexRevive(pRecv->m_wPokedexId);
}

VOID HandlerFromGate::OnMSG_C2S_FAIRY_MAP_REFRESH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

// 	MSG_C2S_FAIRY_MAP_REFRESH_REQ* pRecv = (MSG_C2S_FAIRY_MAP_REFRESH_REQ*)pMsg;
// 
// 	if (pPlayer->GetZhanBuManager())
// 		pPlayer->GetZhanBuManager()->OnNetMapRefresh();
}

VOID HandlerFromGate::OnMSG_C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

//	MSG_C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ* pRecv = (MSG_C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ*)pMsg;
// 
// 	if (pPlayer->GetZhanBuManager())
// 		pPlayer->GetZhanBuManager()->OnNetAchievementActivation();
}


VOID HandlerFromGate::OnMSG_C2S_SCORE_MATCH_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SCORE_MATCH_DATA_REQ* pRecv = (MSG_C2S_SCORE_MATCH_DATA_REQ*)pMsg;

	CScoreMatchManager::Instance()->OnNetScoreMatchData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_SCORE_MATCH_SELECT_CAMP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SCORE_MATCH_SELECT_CAMP_REQ* pRecv = (MSG_C2S_SCORE_MATCH_SELECT_CAMP_REQ*)pMsg;

	CScoreMatchManager::Instance()->OnNetSelectCamp(pPlayer, pRecv->m_byCamp);
}

VOID HandlerFromGate::OnMSG_C2S_SCORE_MATCH_REFRESH_TARGET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SCORE_MATCH_REFRESH_TARGET_REQ* pRecv = (MSG_C2S_SCORE_MATCH_REFRESH_TARGET_REQ*)pMsg;

	CScoreMatchManager::Instance()->OnNetRefreshTarget(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ* pRecv = (MSG_C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ*)pMsg;

	CScoreMatchManager::Instance()->OnNetBuyBattleTimes(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ* pRecv = (MSG_C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ*)pMsg;

	CScoreMatchManager::Instance()->OnNetTakeBattleTaskReward(pPlayer, pRecv->m_TaskList, pRecv->m_wTaskNum);
}

VOID HandlerFromGate::OnMSG_C2S_WARCRAFT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WARCRAFT_DATA_REQ* pRecv = (MSG_C2S_WARCRAFT_DATA_REQ*)pMsg;

	CWarcraftManager::Instance()->OnNetWarcraftData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ* pRecv = (MSG_C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ*)pMsg;

	CWarcraftManager::Instance()->OnNetBuyBattleTimes(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_WARCRAFT_TARGET_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WARCRAFT_TARGET_LIST_REQ* pRecv = (MSG_C2S_WARCRAFT_TARGET_LIST_REQ*)pMsg;

	CWarcraftManager::Instance()->OnNetBattleTargetList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_WARCRAFT_RANK_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WARCRAFT_RANK_LIST_REQ* pRecv = (MSG_C2S_WARCRAFT_RANK_LIST_REQ*)pMsg;

	CWarcraftManager::Instance()->OnNetRankList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_WARCRAFT_CHEER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WARCRAFT_CHEER_REQ* pRecv = (MSG_C2S_WARCRAFT_CHEER_REQ*)pMsg;

	CWarcraftManager::Instance()->OnNetCheer(pPlayer, pRecv->m_dwTargetId);
}



VOID HandlerFromGate::OnMSG_C2S_USE_TITLE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_USE_TITLE_REQ* pRecv = (MSG_C2S_USE_TITLE_REQ*)pMsg;

	pPlayer->GetTitleManager()->OnNetUseTitle(pRecv->m_nTitleId);
}

VOID HandlerFromGate::OnMSG_C2S_RED_POINT_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RED_POINT_LIST_REQ* pRecv = (MSG_C2S_RED_POINT_LIST_REQ*)pMsg;

	pPlayer->GetRedPointManager()->OnNetRedPointList();
}

VOID HandlerFromGate::OnMSG_C2S_VIP_GIFT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_VIP_GIFT_DATA_REQ* pRecv = (MSG_C2S_VIP_GIFT_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetVipGiftDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_VIP_DAILY_GIFT_GET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_VIP_DAILY_GIFT_GET_REQ* pRecv = (MSG_C2S_VIP_DAILY_GIFT_GET_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetVipDailyGiftTake(pRecv->m_wVipLevel);
}

VOID HandlerFromGate::OnMSG_C2S_VIP_WEEK_GIFT_BUY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_VIP_WEEK_GIFT_BUY_REQ* pRecv = (MSG_C2S_VIP_WEEK_GIFT_BUY_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetVipWeekGiftBuy(pRecv->m_wGiftId);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ* pRecv = (MSG_C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetTakeTodayRecommendReward();
}



VOID HandlerFromGate::OnMSG_C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ* pRecv = (MSG_C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetLevelShopActivityDataReq();
}


VOID HandlerFromGate::OnMSG_C2S_ROLE_TITLE_INFO_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ROLE_TITLE_INFO_REQ* pRecv = (MSG_C2S_ROLE_TITLE_INFO_REQ*)pMsg;

	pPlayer->GetTitleManager()->OnNetRoleTitleInfo();
}

VOID HandlerFromGate::OnMSG_C2S_OPEN_SERVER_FUND_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_OPEN_SERVER_FUND_DATA_REQ* pRecv = (MSG_C2S_OPEN_SERVER_FUND_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetOpenServerFundDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_OPEN_SERVER_FUND_BUY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_OPEN_SERVER_FUND_BUY_REQ* pRecv = (MSG_C2S_OPEN_SERVER_FUND_BUY_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetOpenServerFundBuy();
}

VOID HandlerFromGate::OnMSG_C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ* pRecv = (MSG_C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetOpenServerFundRewardTake(pRecv->m_wRewardId);
}

VOID HandlerFromGate::OnMSG_C2S_ALL_PEOPLE_WELFARE_TAKE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ALL_PEOPLE_WELFARE_TAKE_REQ* pRecv = (MSG_C2S_ALL_PEOPLE_WELFARE_TAKE_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetAllPeopleWelfareTake(pRecv->m_wWelfareId);
}

VOID HandlerFromGate::OnMSG_C2S_LIMIT_PREFERENTIAL_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LIMIT_PREFERENTIAL_DATA_REQ* pRecv = (MSG_C2S_LIMIT_PREFERENTIAL_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetLimitPreferentialReq();
}

VOID HandlerFromGate::OnMSG_C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ* pRecv = (MSG_C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetTakeLimitPreferentialWelfare(pRecv->m_wWelfareId);
}

VOID HandlerFromGate::OnMSG_C2S_LEVEL_GIFT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LEVEL_GIFT_DATA_REQ* pRecv = (MSG_C2S_LEVEL_GIFT_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetLevelGiftDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_LEVEL_GIFT_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_LEVEL_GIFT_REWARD_REQ* pRecv = (MSG_C2S_TAKE_LEVEL_GIFT_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetTakeLevelGift(pRecv->m_wLevel);
}

VOID HandlerFromGate::OnMSG_C2S_NEXT_DAY_REWARD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_NEXT_DAY_REWARD_DATA_REQ* pRecv = (MSG_C2S_NEXT_DAY_REWARD_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetNextDayRewardData();
}


VOID HandlerFromGate::OnMSG_C2S_NEXT_DAY_REWARD_TAKE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_NEXT_DAY_REWARD_TAKE_REQ* pRecv = (MSG_C2S_NEXT_DAY_REWARD_TAKE_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetTakeNextDayReward();
}

VOID HandlerFromGate::OnMSG_C2S_LOGIN_REWARD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LOGIN_REWARD_DATA_REQ* pRecv = (MSG_C2S_LOGIN_REWARD_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetLoginRewardData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_LOGIN_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_LOGIN_REWARD_REQ* pRecv = (MSG_C2S_TAKE_LOGIN_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetTakeLoginReward(pRecv->m_wDays);
}

VOID HandlerFromGate::OnMSG_C2S_KING_MATCH_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_KING_MATCH_DATA_REQ* pRecv = (MSG_C2S_KING_MATCH_DATA_REQ*)pMsg;

	CKingMatchManager::Instance()->OnNetPlayerKingMatchData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_KING_MATCH_SEARCH_TARGET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_KING_MATCH_SEARCH_TARGET_REQ* pRecv = (MSG_C2S_KING_MATCH_SEARCH_TARGET_REQ*)pMsg;

	CKingMatchManager::Instance()->OnNetMatchTarget(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_KING_MATCH_TASK_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_KING_MATCH_TASK_REWARD_REQ* pRecv = (MSG_C2S_TAKE_KING_MATCH_TASK_REWARD_REQ*)pMsg;

	CKingMatchManager::Instance()->OnNetTakeSesionTaskReward(pPlayer, pRecv->m_wWinTimes);
}

VOID HandlerFromGate::OnMSG_C2S_KING_MATCH_SEASON_RANK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_KING_MATCH_SEASON_RANK_REQ* pRecv = (MSG_C2S_KING_MATCH_SEASON_RANK_REQ*)pMsg;

	CKingMatchManager::Instance()->OnNetRecentlySesionRank(pPlayer);
}



VOID HandlerFromGate::OnMSG_C2S_NOBILITY_ACTIVATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_NOBILITY_ACTIVATE_REQ* pRecv = (MSG_C2S_NOBILITY_ACTIVATE_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetAcivateNobility();
}

VOID HandlerFromGate::OnMSG_C2S_SUPER_DISCOUNT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SUPER_DISCOUNT_DATA_REQ* pRecv = (MSG_C2S_SUPER_DISCOUNT_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetSuperDiscount().OnNetSuperDiscountData();
}

VOID HandlerFromGate::OnMSG_C2S_SUPER_DISCOUNT_BUY_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SUPER_DISCOUNT_BUY_REWARD_REQ* pRecv = (MSG_C2S_SUPER_DISCOUNT_BUY_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetSuperDiscount().OnNetTakeBuyReward();
}

VOID HandlerFromGate::OnMSG_C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ* pRecv = (MSG_C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetSuperDiscount().OnNetTakeAccumulateReward(pRecv->m_wTimes);
}



VOID HandlerFromGate::OnMSG_C2S_NEW_SUPER_DISCOUNT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_NEW_SUPER_DISCOUNT_DATA_REQ* pRecv = (MSG_C2S_NEW_SUPER_DISCOUNT_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetNewSuperDiscount().OnNetSuperDiscountData();
}

VOID HandlerFromGate::OnMSG_C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ* pRecv = (MSG_C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetNewSuperDiscount().OnNetTakeBuyReward();
}

VOID HandlerFromGate::OnMSG_C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ* pRecv = (MSG_C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetNewSuperDiscount().OnNetTakeAccumulateReward();
}



VOID HandlerFromGate::OnMSG_C2S_FESTIVAL_ACTIVITY_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FESTIVAL_ACTIVITY_DATA_REQ* pRecv = (MSG_C2S_FESTIVAL_ACTIVITY_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFestivalActivity().OnNetActivityData(pRecv->m_wActivityType);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ* pRecv = (MSG_C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFestivalActivity().OnNetTakeActivityReward(pRecv->m_wActivityType, pRecv->m_wTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ* pRecv = (MSG_C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFestivalActivity().OnNetGoodsExchange(pRecv->m_wActivityType, pRecv->m_wTargetId, pRecv->m_wExchangeNum);
}

VOID HandlerFromGate::OnMSG_C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ* pRecv = (MSG_C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFestivalActivity().OnNetRedPoint();
}

VOID HandlerFromGate::OnMSG_C2S_ACCURATE_RECOMMEND_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ACCURATE_RECOMMEND_DATA_REQ* pRecv = (MSG_C2S_ACCURATE_RECOMMEND_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetAccurateRecommend().OnNetAccurateRecommendData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ* pRecv = (MSG_C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetAccurateRecommend().OnNetTakeAccurateRecommendGift(pRecv->m_wGoodsId);
}

VOID HandlerFromGate::OnMSG_C2S_GEM_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GEM_LIST_REQ* pRecv = (MSG_C2S_GEM_LIST_REQ*)pMsg;

	pPlayer->GetGemManager()->OnNetGemList();
}

VOID HandlerFromGate::OnMSG_C2S_GEM_COMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GEM_COMPOSE_REQ* pRecv = (MSG_C2S_GEM_COMPOSE_REQ*)pMsg;

	pPlayer->GetGemManager()->OnNetCompose(pRecv->m_dwGemId, pRecv->m_wGemNum);
}

VOID HandlerFromGate::OnMSG_C2S_GEM_INLAY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GEM_INLAY_REQ* pRecv = (MSG_C2S_GEM_INLAY_REQ*)pMsg;

	pPlayer->GetGemManager()->OnNetInlay(pRecv->m_wFormationSlot, pRecv->m_arGemInlayList, pRecv->m_wGemInlayNum);
}

VOID HandlerFromGate::OnMSG_C2S_GEM_UNLOAD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GEM_UNLOAD_REQ* pRecv = (MSG_C2S_GEM_UNLOAD_REQ*)pMsg;

	pPlayer->GetGemManager()->OnNetUnload(pRecv->m_wFormationSlot, pRecv->m_wGemHole);
}


VOID HandlerFromGate::OnMSG_C2S_GEM_HOLE_OPEN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GEM_HOLE_OPEN_REQ* pRecv = (MSG_C2S_GEM_HOLE_OPEN_REQ*)pMsg;

	pPlayer->GetGemManager()->OnNetHoleOpen(pRecv->m_wFormationSlot, pRecv->m_wGemHole);
}

VOID HandlerFromGate::OnMSG_C2S_GEM_DECOMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GEM_DECOMPOSE_REQ* pRecv = (MSG_C2S_GEM_DECOMPOSE_REQ*)pMsg;

	pPlayer->GetGemManager()->OnNetDecompose(pRecv->m_dwGemId, pRecv->m_dwGemNum);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetAnswerQuestion(pPlayer, pRecv->m_byIsRight, pRecv->m_byAnswer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_DIALOG_OVER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_DIALOG_OVER_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_DIALOG_OVER_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetDialogOver(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_NEXT_NPC_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_NEXT_NPC_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_NEXT_NPC_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetNextNPC(pPlayer, pRecv->m_wNpcId);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetBossTaskStart(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_LOTTERY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_LOTTERY_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_LOTTERY_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetLottery(pPlayer, pRecv->m_wTimes, pRecv->m_byIsCostItem);
}


VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_CREATE_TEAM_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_CREATE_TEAM_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_CREATE_TEAM_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetCreate(pPlayer, pRecv->m_wDungeonId);
}


VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_LEAVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_LEAVE_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_LEAVE_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetLeaveReadyTeam(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_INVITE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_INVITE_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_INVITE_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetInvite(pPlayer, pRecv->m_dwTargetId);
}


VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetAcceptInvite(pPlayer, pRecv->m_dwTeamId);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_JOIN_IN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_JOIN_IN_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_JOIN_IN_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetJoinInReadyTeam(pPlayer, pRecv->m_dwTeamId, pRecv->m_wDungeonId);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetPlayerRecommendList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_MATCH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_MATCH_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_MATCH_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetMatch(pPlayer, pRecv->m_wDungeonId, pRecv->m_byIsTeam);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetCancelMatch(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetAcceptTask(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_TASK_START_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_TASK_START_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_TASK_START_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetTaskStart(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_RANK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_RANK_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_RANK_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetRankList(pPlayer, pRecv->m_wDungeonId);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_DATA_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_DATA_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetTeamDungeonData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetShortcutChat(pPlayer, pRecv->m_wChatId);
}

VOID HandlerFromGate::OnMSG_C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ* pRecv = (MSG_C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ*)pMsg;

	CTeamDungeonManager::Instance()->OnNetLotteryRecord(pPlayer);
}










 
 
 


//  
//  



VOID HandlerFromGate::OnMSG_C2S_PLAYER_LEVEL_MESSAGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PLAYER_LEVEL_MESSAGE_REQ* pRecv = (MSG_C2S_PLAYER_LEVEL_MESSAGE_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetLeaveMessage(pRecv->m_szContact, pRecv->m_szMessage);
}

VOID HandlerFromGate::OnMSG_C2S_MAP_COMMENT_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_MAP_COMMENT_LIST_REQ* pRecv = (MSG_C2S_MAP_COMMENT_LIST_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetMapCommentList(pRecv->m_wMapId);
}

VOID HandlerFromGate::OnMSG_C2S_MAP_COMMENT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_MAP_COMMENT_REQ* pRecv = (MSG_C2S_MAP_COMMENT_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetMapComment(pRecv->m_wMapId, pRecv->m_szText);
}

VOID HandlerFromGate::OnMSG_C2S_MAP_COMMENT_LIKE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_MAP_COMMENT_LIKE_REQ* pRecv = (MSG_C2S_MAP_COMMENT_LIKE_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetMapCommentLike(pRecv->m_wMapId, pRecv->m_wCommentId);
}

VOID HandlerFromGate::OnMSG_C2S_RECHARGE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RECHARGE_DATA_REQ* pRecv = (MSG_C2S_RECHARGE_DATA_REQ*)pMsg;

	pPlayer->GetRechargeManager()->OnNetRechargeData();
}

VOID HandlerFromGate::OnMSG_C2S_RECHARGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

 	MSG_C2S_RECHARGE_REQ* pRecv = (MSG_C2S_RECHARGE_REQ*)pMsg;
 
	pPlayer->GetRechargeManager()->OnNetRechargeEx(pRecv->m_wRechargeId, pRecv->m_wRechargeType, pRecv->m_wGoodsId, pRecv->m_dwContext);
}

VOID HandlerFromGate::OnMSG_C2S_MONTH_CARD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_MONTH_CARD_DATA_REQ* pRecv = (MSG_C2S_MONTH_CARD_DATA_REQ*)pMsg;

	pPlayer->GetRechargeManager()->OnNetMonthCardData();
}

VOID HandlerFromGate::OnMSG_C2S_MONTH_CARD_BUY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_MONTH_CARD_BUY_REQ* pRecv = (MSG_C2S_MONTH_CARD_BUY_REQ*)pMsg;

	pPlayer->GetRechargeManager()->OnNetMonthCardBuy(pRecv->m_wCardId);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_MONTH_CARD_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_MONTH_CARD_REWARD_REQ* pRecv = (MSG_C2S_TAKE_MONTH_CARD_REWARD_REQ*)pMsg;

	pPlayer->GetRechargeManager()->OnNetTakeMonthCardReward(pRecv->m_wCardId);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_SEARCH_TARGET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_SEARCH_TARGET_REQ* pRecv = (MSG_C2S_PVP_SEARCH_TARGET_REQ*)pMsg;

	CPVPManager::Instance()->OnNetSearchTarget(pPlayer, pRecv->m_dwSpiritSkillId);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_READY_FINISH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_READY_FINISH_REQ* pRecv = (MSG_C2S_PVP_READY_FINISH_REQ*)pMsg;

	CPVPManager::Instance()->OnNetReadyFinish(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_USE_SKILL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_USE_SKILL_REQ* pRecv = (MSG_C2S_PVP_USE_SKILL_REQ*)pMsg;

	CPVPManager::Instance()->OnNetUseSkill(pPlayer, pRecv->m_dwObjId, pRecv->m_dwSkillId, pRecv->m_wX, pRecv->m_wY);
}


VOID HandlerFromGate::OnMSG_C2S_PVP_HP_CHANGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_HP_CHANGE_REQ* pRecv = (MSG_C2S_PVP_HP_CHANGE_REQ*)pMsg;

	CPVPManager::Instance()->OnNetHPChange(pPlayer, pRecv->m_dwSrcObjId, pRecv->m_dwTgtObjId, 
		pRecv->m_nChangeHP, pRecv->m_nDamageType, pRecv->m_dwPower, pRecv->m_szSign);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_CREATE_BUFF_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_CREATE_BUFF_REQ* pRecv = (MSG_C2S_PVP_CREATE_BUFF_REQ*)pMsg;

	CPVPManager::Instance()->OnNetCreateBuff(pPlayer, pRecv->m_dwBuffId, pRecv->m_dwSrcObjId, pRecv->m_dwTgtObjId, pRecv->m_byIsSpriteBlessBuff);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_BATTLE_RESULT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_BATTLE_RESULT_REQ* pRecv = (MSG_C2S_PVP_BATTLE_RESULT_REQ*)pMsg;

	CPVPManager::Instance()->OnNetBattleResult(pPlayer, pRecv->m_byResult);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_TAKE_BATTLE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_TAKE_BATTLE_REWARD_REQ* pRecv = (MSG_C2S_PVP_TAKE_BATTLE_REWARD_REQ*)pMsg;

	CPVPManager::Instance()->OnNetTakeBattleReward(pPlayer, pRecv->m_byType, pRecv->m_arTakeRewardList, pRecv->m_wTakeRewardNum);
}

VOID HandlerFromGate::OnMSG_C2S_PLAYER_PVP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PLAYER_PVP_DATA_REQ* pRecv = (MSG_C2S_PLAYER_PVP_DATA_REQ*)pMsg;

	CPVPManager::Instance()->OnNetPlayerPVPData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_SELECT_SPRIT_SKILL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_SELECT_SPRIT_SKILL_REQ* pRecv = (MSG_C2S_PVP_SELECT_SPRIT_SKILL_REQ*)pMsg;

	//CPVPManager::Instance()->OnNetSelectSpritSkill(pPlayer, pRecv->m_dwSkillId);
}

VOID HandlerFromGate::OnMSG_C2S_PVP_CANCEL_SEARCH_TARGET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PVP_CANCEL_SEARCH_TARGET_REQ* pRecv = (MSG_C2S_PVP_CANCEL_SEARCH_TARGET_REQ*)pMsg;

	CPVPManager::Instance()->OnNetCancelSearchTarget(pPlayer);
}



VOID HandlerFromGate::OnMSG_C2S_LIMIT_FIRST_RECHARGE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LIMIT_FIRST_RECHARGE_DATA_REQ* pRecv = (MSG_C2S_LIMIT_FIRST_RECHARGE_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetLimitActivity().OnNetFirstRechargeData();
}

VOID HandlerFromGate::OnMSG_C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ* pRecv = (MSG_C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ*)pMsg;

	//pPlayer->GetActivityManager()->GetLimitActivity().OnNetSingleRechargeData();
}

VOID HandlerFromGate::OnMSG_C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ* pRecv = (MSG_C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ*)pMsg;

	//pPlayer->GetActivityManager()->GetLimitActivity().OnNetAccumulateRechargeData();
}

VOID HandlerFromGate::OnMSG_C2S_LIMIT_RECRUIT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LIMIT_RECRUIT_DATA_REQ* pRecv = (MSG_C2S_LIMIT_RECRUIT_DATA_REQ*)pMsg;

	//pPlayer->GetActivityManager()->GetLimitActivity().OnNetRecruitData();
}


VOID HandlerFromGate::OnMSG_C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ* pRecv = (MSG_C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetLimitActivity().OnNetTakeActivityReward(pRecv->m_wDataId);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_DATA_REQ* pRecv = (MSG_C2S_ATTACK_CITY_DATA_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetAttackCityData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_ENTER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_ENTER_REQ* pRecv = (MSG_C2S_ATTACK_CITY_ENTER_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetAttackCityEnter(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_LEAVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_LEAVE_REQ* pRecv = (MSG_C2S_ATTACK_CITY_LEAVE_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetAttackCityLeave(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_GOTO_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_GOTO_REQ* pRecv = (MSG_C2S_ATTACK_CITY_GOTO_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetGoto(pPlayer, pRecv->m_tSrcPos, pRecv->m_tDestPos);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_BOX_LOCK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_BOX_LOCK_REQ* pRecv = (MSG_C2S_TREASURE_BOX_LOCK_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetTreasureBoxLock(pPlayer, pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_BOX_PICK_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_BOX_PICK_UP_REQ* pRecv = (MSG_C2S_TREASURE_BOX_PICK_UP_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetTreasureBoxPickUp(pPlayer, pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_RELIVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_RELIVE_REQ* pRecv = (MSG_C2S_ATTACK_CITY_RELIVE_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetRelive(pPlayer, pRecv->m_byIsFreeRelive);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ* pRecv = (MSG_C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetTakeBossReward(pPlayer, pRecv->m_wChapter);
}

VOID HandlerFromGate::OnMSG_C2S_ATTACK_CITY_ENCOURAGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ATTACK_CITY_ENCOURAGE_REQ* pRecv = (MSG_C2S_ATTACK_CITY_ENCOURAGE_REQ*)pMsg;

	CAttackCityManager::Instance()->OnNetEncourage(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ENDLESS_LAND_DATA_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_DATA_REQ*)pMsg;

	pPlayer->GetEndlessLandManager()->OnNetEndlessData();
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_ENTER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ENDLESS_LAND_ENTER_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_ENTER_REQ*)pMsg;

	pPlayer->GetEndlessLandManager()->OnNetEnter();
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_BATTLE_START_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// 	MSG_C2S_ENDLESS_LAND_BATTLE_START_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_BATTLE_START_REQ*)pMsg;
// 
// 	pPlayer->GetEndlessLandManager()->OnNetBattleStart();
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_BATTLE_OVER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ENDLESS_LAND_BATTLE_OVER_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_BATTLE_OVER_REQ*)pMsg;

	pPlayer->GetEndlessLandManager()->OnNetBattleOver(pRecv->m_byIsWin, pRecv->m_wWave);
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_KILL_MONSTER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ENDLESS_LAND_KILL_MONSTER_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_KILL_MONSTER_REQ*)pMsg;

	pPlayer->GetEndlessLandManager()->OnNetKillMonster(pRecv->m_byIndex, pRecv->m_dwMonsterId);
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_COLLISION_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ENDLESS_LAND_COLLISION_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_COLLISION_REQ*)pMsg;

	pPlayer->GetEndlessLandManager()->OnNetCollision();
}

VOID HandlerFromGate::OnMSG_C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ* pRecv = (MSG_C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ*)pMsg;

	pPlayer->GetEndlessLandManager()->OnNetTakeKeepKillReward(pRecv->m_wKeepKillNum);
}



VOID HandlerFromGate::OnMSG_C2S_HERO_COMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HERO_COMPOSE_REQ* pRecv = (MSG_C2S_HERO_COMPOSE_REQ*)pMsg;

	if (pPlayer->GetHeroManager())
		pPlayer->GetHeroManager()->OnCompose(pRecv->m_wHeroInfoId);
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;
 
	MSG_C2S_EQUIPMENT_LIST_REQ* pRecv = (MSG_C2S_EQUIPMENT_LIST_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnEquipmentList();
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_COMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_COMPOSE_REQ* pRecv = (MSG_C2S_EQUIPMENT_COMPOSE_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnCompose(pRecv->m_dwInfoId);
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_STRENG_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_STRENG_REQ* pRecv = (MSG_C2S_EQUIPMENT_STRENG_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnEnhance(pRecv->m_byType, pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ* pRecv = (MSG_C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnNetOneKeyEnhance(pRecv->m_bySlot);
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_STAR_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_STAR_UP_REQ* pRecv = (MSG_C2S_EQUIPMENT_STAR_UP_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnNetStarUP(pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_QUALITY_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_QUALITY_UP_REQ* pRecv = (MSG_C2S_EQUIPMENT_QUALITY_UP_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnNetQualityUP(pRecv->m_dwUID);
}



VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_REFINE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_REFINE_REQ* pRecv = (MSG_C2S_EQUIPMENT_REFINE_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnRefine(pRecv->m_dwEquipmentUID, pRecv->m_arCostList, pRecv->m_wCostListNum);
}

VOID HandlerFromGate::OnMSG_C2S_EQUIPMENT_PUT_ON_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_EQUIPMENT_PUT_ON_REQ* pRecv = (MSG_C2S_EQUIPMENT_PUT_ON_REQ*)pMsg;

	if (pPlayer->GetEquipmentManager())
		pPlayer->GetEquipmentManager()->OnEquipmentPutOn(pRecv->m_byFormationPos, pRecv->m_byEquipmentPos, pRecv->m_dwUID);
}


VOID HandlerFromGate::OnMSG_C2S_TREASURE_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_LIST_REQ* pRecv = (MSG_C2S_TREASURE_LIST_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnTreasureList();
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_STRENGTHEN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_STRENGTHEN_REQ* pRecv = (MSG_C2S_TREASURE_STRENGTHEN_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnEnhance(pRecv->m_dwTreasureUID, pRecv->m_CostItemList, pRecv->m_wCostItemNum);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_REFINE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_REFINE_REQ* pRecv = (MSG_C2S_TREASURE_REFINE_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnRefine(pRecv->m_dwTreasureUID);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_COMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_COMPOSE_REQ* pRecv = (MSG_C2S_TREASURE_COMPOSE_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnCompose(pRecv->m_dwTreasureInfoId, pRecv->m_wTreasureNum);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_PUT_ON_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_PUT_ON_REQ* pRecv = (MSG_C2S_TREASURE_PUT_ON_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnPutOn(pRecv->m_byFormationPos, pRecv->m_byTreasurePos, pRecv->m_dwUID);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_SMELT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_SMELT_REQ* pRecv = (MSG_C2S_TREASURE_SMELT_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnNetSmelt(pRecv->m_dwTreasureInfoId);
}

VOID HandlerFromGate::OnMSG_C2S_TREASURE_QUALITY_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TREASURE_QUALITY_UP_REQ* pRecv = (MSG_C2S_TREASURE_QUALITY_UP_REQ*)pMsg;

	if (pPlayer->GetTreasureManager())
		pPlayer->GetTreasureManager()->OnNetQualityUP(pRecv->m_dwTreasureUId);
}




VOID HandlerFromGate::OnMSG_C2S_REBEL_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REBEL_DATA_REQ* pRecv = (MSG_C2S_REBEL_DATA_REQ*)pMsg;

	CRebelManager::Instance()->OnRebelDataReq(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_REBEL_EXPLOIT_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REBEL_EXPLOIT_AWARD_REQ* pRecv = (MSG_C2S_REBEL_EXPLOIT_AWARD_REQ*)pMsg;

	CRebelManager::Instance()->OnRebelAwardReq(pPlayer, pRecv->m_AwardList, pRecv->m_wAwardNum);
}

VOID HandlerFromGate::OnMSG_C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ* pRecv = (MSG_C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ*)pMsg;

	CRebelManager::Instance()->OnNetAwardRecordList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_REBEL_SHARE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_REBEL_SHARE_REQ* pRecv = (MSG_C2S_REBEL_SHARE_REQ*)pMsg;

	CRebelManager::Instance()->OnNetRebelShare(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_RECYCLE_DECOMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RECYCLE_DECOMPOSE_REQ* pRecv = (MSG_C2S_RECYCLE_DECOMPOSE_REQ*)pMsg;

	pPlayer->GetRecyclingManager()->OnNetRecycleDecompose(pRecv->m_byObjType, pRecv->m_TargetList, pRecv->m_byTargetNum);
}

VOID HandlerFromGate::OnMSG_C2S_RECYCLE_RESET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RECYCLE_RESET_REQ* pRecv = (MSG_C2S_RECYCLE_RESET_REQ*)pMsg;

	pPlayer->GetRecyclingManager()->OnNetRecycleReset(pRecv->m_byObjType, pRecv->m_dwTargetUId);
}

VOID HandlerFromGate::OnMSG_C2S_RECYCLE_DECOMPOSE_RETURN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RECYCLE_DECOMPOSE_RETURN_REQ* pRecv = (MSG_C2S_RECYCLE_DECOMPOSE_RETURN_REQ*)pMsg;

	pPlayer->GetRecyclingManager()->OnNetDecomposeReturn(pRecv->m_byObjType, pRecv->m_TargetList, pRecv->m_byTargetNum);
}

VOID HandlerFromGate::OnMSG_C2S_RECYCLE_RESET_RETURN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RECYCLE_RESET_RETURN_REQ* pRecv = (MSG_C2S_RECYCLE_RESET_RETURN_REQ*)pMsg;

	pPlayer->GetRecyclingManager()->OnNetResetReturn(pRecv->m_byObjType, pRecv->m_dwTargetUId);
}

VOID HandlerFromGate::OnMSG_C2S_DUOBAO_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DUOBAO_DATA_REQ* pRecv = (MSG_C2S_DUOBAO_DATA_REQ*)pMsg;

	if (pPlayer->GetDuobaoManager())
		pPlayer->GetDuobaoManager()->OnNetDuobaoData();
}

VOID HandlerFromGate::OnMSG_C2S_DUOBAO_TARGET_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DUOBAO_TARGET_LIST_REQ* pRecv = (MSG_C2S_DUOBAO_TARGET_LIST_REQ*)pMsg;

	if (pPlayer->GetDuobaoManager())
		pPlayer->GetDuobaoManager()->OnNetTargetList(pRecv->m_wTreasureInfoId, pRecv->m_wTreasureItemId);
}

VOID HandlerFromGate::OnMSG_C2S_DUOBAO_FIVE_TIMES_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DUOBAO_FIVE_TIMES_REQ* pRecv = (MSG_C2S_DUOBAO_FIVE_TIMES_REQ*)pMsg;

	if (pPlayer->GetDuobaoManager())
		pPlayer->GetDuobaoManager()->OnNetDuoBaoFiveTimes(pRecv->m_wTreasureInfoId, pRecv->m_wTreasureItemId, pRecv->m_wDBTimes);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ* pRecv = (MSG_C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ*)pMsg;

	if (pPlayer->GetDuobaoManager())
		pPlayer->GetDuobaoManager()->OnNetTakePregrossReward(pRecv->m_PregrossIdList, pRecv->m_wPreprossIdNum);
}

VOID HandlerFromGate::OnMSG_C2S_PLAYER_LAND_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PLAYER_LAND_DATA_REQ* pRecv = (MSG_C2S_PLAYER_LAND_DATA_REQ*)pMsg;

	CLandManager::Instance()->OnNetPlayerLandData(pPlayer, pRecv->m_dwTargetId);
}

VOID HandlerFromGate::OnMSG_C2S_LAND_PATROL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LAND_PATROL_REQ* pRecv = (MSG_C2S_LAND_PATROL_REQ*)pMsg;

	CLandManager::Instance()->OnNetPatrol(pPlayer, pRecv->m_dwPatrolHeroUID, pRecv->m_wCityId, pRecv->m_byPatrolType, pRecv->m_byTimeType);
}

VOID HandlerFromGate::OnMSG_C2S_LAND_VIEW_PATROL_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	//MSG_C2S_LAND_VIEW_PATROL_AWARD_REQ* pRecv = (MSG_C2S_LAND_VIEW_PATROL_AWARD_REQ*)pMsg;

	//CLandManager::Instance()->OnNetPlayerLandData(pPlayer, pRecv->m_wCityId);
}

VOID HandlerFromGate::OnMSG_C2S_LAND_GET_PATROL_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LAND_GET_PATROL_AWARD_REQ* pRecv = (MSG_C2S_LAND_GET_PATROL_AWARD_REQ*)pMsg;

	CLandManager::Instance()->OnNetPatrolAward(pPlayer, pRecv->m_wCityId);
}

VOID HandlerFromGate::OnMSG_C2S_LAND_SKILL_LEVELUP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LAND_SKILL_LEVELUP_REQ* pRecv = (MSG_C2S_LAND_SKILL_LEVELUP_REQ*)pMsg;

	CLandManager::Instance()->OnNetLandLevelUp(pPlayer, pRecv->m_wCityId);
}

VOID HandlerFromGate::OnMSG_C2S_LAND_SUPPRESS_RIOT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LAND_SUPPRESS_RIOT_REQ* pRecv = (MSG_C2S_LAND_SUPPRESS_RIOT_REQ*)pMsg;

	CLandManager::Instance()->OnSuppressRiot(pPlayer, pRecv->m_dwFriendId, pRecv->m_wCityId);
}

VOID HandlerFromGate::OnMSG_C2S_FRIEND_LAND_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FRIEND_LAND_LIST_REQ* pRecv = (MSG_C2S_FRIEND_LAND_LIST_REQ*)pMsg;

	CLandManager::Instance()->OnNetFriendLandList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_LAND_CITY_DETAILS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_LAND_CITY_DETAILS_REQ* pRecv = (MSG_C2S_LAND_CITY_DETAILS_REQ*)pMsg;

	CLandManager::Instance()->OnNetCityDetails(pPlayer, pRecv->m_dwPlayerId, pRecv->m_dwCityId);
}


VOID HandlerFromGate::OnMSG_C2S_SPIRIT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPIRIT_DATA_REQ* pRecv = (MSG_C2S_SPIRIT_DATA_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetSpiritDataReq();
}

VOID HandlerFromGate::OnMSG_C2S_SPIRIT_TRAIN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPIRIT_TRAIN_REQ* pRecv = (MSG_C2S_SPIRIT_TRAIN_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetTrain(pRecv->m_dwSpriteId, pRecv->m_wTrainTimes);
}

VOID HandlerFromGate::OnMSG_C2S_SPRITE_STAR_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPRITE_STAR_UP_REQ* pRecv = (MSG_C2S_SPRITE_STAR_UP_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetStarUp(pRecv->m_dwSpriteId);
}


VOID HandlerFromGate::OnMSG_C2S_SPRITE_BLESS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPRITE_BLESS_REQ* pRecv = (MSG_C2S_SPRITE_BLESS_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetBless(pRecv->m_wBlessType, pRecv->m_wFormationPos, pRecv->m_dwSpriteId);
}

VOID HandlerFromGate::OnMSG_C2S_SPRITE_COMPOSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPRITE_COMPOSE_REQ* pRecv = (MSG_C2S_SPRITE_COMPOSE_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetCompose(pRecv->m_dwFragmentId);
}





VOID HandlerFromGate::OnMSG_C2S_HEART_OF_WORLD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HEART_OF_WORLD_DATA_REQ* pRecv = (MSG_C2S_HEART_OF_WORLD_DATA_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetHeartOfWorldData();
}

VOID HandlerFromGate::OnMSG_C2S_HEART_OF_WORLD_LEVEL_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HEART_OF_WORLD_LEVEL_UP_REQ* pRecv = (MSG_C2S_HEART_OF_WORLD_LEVEL_UP_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetHeartOfWorldLevelUp();
}

VOID HandlerFromGate::OnMSG_C2S_SPIRIT_ENHANCE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPIRIT_ENHANCE_REQ* pRecv = (MSG_C2S_SPIRIT_ENHANCE_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetSpiritEnhance(pRecv->m_dwSpiritId);
}

VOID HandlerFromGate::OnMSG_C2S_SPIRIT_USE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SPIRIT_USE_REQ* pRecv = (MSG_C2S_SPIRIT_USE_REQ*)pMsg;

	if (pPlayer->GetSpiritManager())
		pPlayer->GetSpiritManager()->OnNetSpiritUse(pRecv->m_dwSpiritId);
}




VOID HandlerFromGate::OnMSG_C2S_GUILD_DATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_DATE_REQ* pRecv = (MSG_C2S_GUILD_DATE_REQ*)pMsg;

	CGuildManager::Instance()->OnGuildData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_CREATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_CREATE_REQ* pRecv = (MSG_C2S_GUILD_CREATE_REQ*)pMsg;

	CGuildManager::Instance()->OnCreate(pPlayer, pRecv->m_wIconId, pRecv->m_szGuildName);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_DISMISS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_DISMISS_REQ* pRecv = (MSG_C2S_GUILD_DISMISS_REQ*)pMsg;

	CGuildManager::Instance()->OnDismiss(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_APPLY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_APPLY_REQ* pRecv = (MSG_C2S_GUILD_APPLY_REQ*)pMsg;

	CGuildManager::Instance()->OnApply(pPlayer, pRecv->m_dwGuildId);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_APPLY_CHECK_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_APPLY_CHECK_REQ* pRecv = (MSG_C2S_GUILD_APPLY_CHECK_REQ*)pMsg;

	CGuildManager::Instance()->OnApplyCheck(pPlayer, pRecv->m_dwTargetId, pRecv->m_byIsAccept);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_APPLY_CANCEL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_APPLY_CANCEL_REQ* pRecv = (MSG_C2S_GUILD_APPLY_CANCEL_REQ*)pMsg;

	CGuildManager::Instance()->OnApplyCancel(pPlayer, pRecv->m_dwGuildId);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_KICKOUT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_KICKOUT_REQ* pRecv = (MSG_C2S_GUILD_KICKOUT_REQ*)pMsg;

	CGuildManager::Instance()->OnKickOut(pPlayer, pRecv->m_dwTargetId);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_LEAVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_LEAVE_REQ* pRecv = (MSG_C2S_GUILD_LEAVE_REQ*)pMsg;

	CGuildManager::Instance()->OnLeave(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_APPOINT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_APPOINT_REQ* pRecv = (MSG_C2S_GUILD_APPOINT_REQ*)pMsg;

	CGuildManager::Instance()->OnAppoint(pPlayer, pRecv->m_dwTargetId, pRecv->m_byPos);
}

VOID HandlerFromGate::OnMSG_C2S_ACHIEVEMENT_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ACHIEVEMENT_LIST_REQ* pRecv = (MSG_C2S_ACHIEVEMENT_LIST_REQ*)pMsg;

	pPlayer->GetAchievementManager()->OnNetAchievementList();
}

VOID HandlerFromGate::OnMSG_C2S_ACHIEVEMENT_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ACHIEVEMENT_AWARD_REQ* pRecv = (MSG_C2S_ACHIEVEMENT_AWARD_REQ*)pMsg;

	pPlayer->GetAchievementManager()->OnNetAchievementAward(pRecv->m_wAchievementId);
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_BOSS_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_BOSS_DATA_REQ* pRecv = (MSG_C2S_WORLD_BOSS_DATA_REQ*)pMsg;

	CWorldBossManager::Instance()->OnNetWorldBossData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_BOSS_SELECT_CAMP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_BOSS_SELECT_CAMP_REQ* pRecv = (MSG_C2S_WORLD_BOSS_SELECT_CAMP_REQ*)pMsg;

	CWorldBossManager::Instance()->OnNetSelectCamp(pPlayer, pRecv->m_byCamp);
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_BOSS_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_BOSS_REWARD_REQ* pRecv = (MSG_C2S_WORLD_BOSS_REWARD_REQ*)pMsg;

	CWorldBossManager::Instance()->OnNetReward(pPlayer, pRecv->m_byRewardType, pRecv->m_RewardList, pRecv->m_byRewardNum);
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_BOSS_REWARD_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_BOSS_REWARD_RECORD_REQ* pRecv = (MSG_C2S_WORLD_BOSS_REWARD_RECORD_REQ*)pMsg;

	CWorldBossManager::Instance()->OnNetRewardRecord(pPlayer, pRecv->m_byRewardType);
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_BOSS_BATTLE_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_BOSS_BATTLE_RECORD_REQ* pRecv = (MSG_C2S_WORLD_BOSS_BATTLE_RECORD_REQ*)pMsg;

	CWorldBossManager::Instance()->OnNetBattleRecord(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ* pRecv = (MSG_C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ*)pMsg;

	CWorldBossManager::Instance()->OnNetBuyBattleTimes(pPlayer, pRecv->m_byBuyTimes);
}

VOID HandlerFromGate::OnMSG_C2S_DAILY_MISSION_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DAILY_MISSION_LIST_REQ* pRecv = (MSG_C2S_DAILY_MISSION_LIST_REQ*)pMsg;

	pPlayer->GetDailyMissionManager()->OnNetMissionList();
}

VOID HandlerFromGate::OnMSG_C2S_DAILY_MISSION_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DAILY_MISSION_AWARD_REQ* pRecv = (MSG_C2S_DAILY_MISSION_AWARD_REQ*)pMsg;

	pPlayer->GetDailyMissionManager()->OnNetMissionAward(pRecv->m_wMissionId);
}

VOID HandlerFromGate::OnMSG_C2S_DAILY_MISSION_POINT_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_DAILY_MISSION_POINT_AWARD_REQ* pRecv = (MSG_C2S_DAILY_MISSION_POINT_AWARD_REQ*)pMsg;

	pPlayer->GetDailyMissionManager()->OnNetPointAward(pRecv->m_wPoint);
}




VOID HandlerFromGate::OnMSG_C2S_GUILD_LEAVE_MESSAGE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_LEAVE_MESSAGE_REQ* pRecv = (MSG_C2S_GUILD_LEAVE_MESSAGE_REQ*)pMsg;

	CGuildManager::Instance()->OnLeaveMessage(pPlayer, pRecv->m_szLeaveMsg);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_SEARCH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_SEARCH_REQ* pRecv = (MSG_C2S_GUILD_SEARCH_REQ*)pMsg;

	CGuildManager::Instance()->OnSearch(pPlayer, pRecv->m_szGuildName, pRecv->m_dwGuildId);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_QY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_QY_REQ* pRecv = (MSG_C2S_GUILD_QY_REQ*)pMsg;

	CGuildManager::Instance()->OnNetQY(pPlayer, pRecv->m_byType);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_QS_AWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_QS_AWARD_REQ* pRecv = (MSG_C2S_GUILD_QS_AWARD_REQ*)pMsg;

	CGuildManager::Instance()->OnNetQSAward(pPlayer, pRecv->m_wQuanShui);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_APPLY_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_APPLY_LIST_REQ* pRecv = (MSG_C2S_GUILD_APPLY_LIST_REQ*)pMsg;

	CGuildManager::Instance()->OnGuildApplyList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_LEAVEMSG_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_LEAVEMSG_LIST_REQ* pRecv = (MSG_C2S_GUILD_LEAVEMSG_LIST_REQ*)pMsg;

	CGuildManager::Instance()->OnLeaveMsgList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_MEMBER_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_MEMBER_LIST_REQ* pRecv = (MSG_C2S_GUILD_MEMBER_LIST_REQ*)pMsg;

	CGuildManager::Instance()->OnGuildMemberList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_LIST_REQ* pRecv = (MSG_C2S_GUILD_LIST_REQ*)pMsg;

	CGuildManager::Instance()->OnGuildList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_INFO_MODIFY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_INFO_MODIFY_REQ* pRecv = (MSG_C2S_GUILD_INFO_MODIFY_REQ*)pMsg;

	CGuildManager::Instance()->OnModifyGuildInfo(pPlayer, pRecv->m_byType, pRecv->m_szText, pRecv->m_wIconId);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_EVENT_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_EVENT_LIST_REQ* pRecv = (MSG_C2S_GUILD_EVENT_LIST_REQ*)pMsg;

	CGuildManager::Instance()->OnGuildEventList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_SKILL_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_SKILL_LIST_REQ* pRecv = (MSG_C2S_GUILD_SKILL_LIST_REQ*)pMsg;

	CGuildManager::Instance()->OnNetSkillList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_SKILL_LEVEL_UP_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_SKILL_LEVEL_UP_REQ* pRecv = (MSG_C2S_GUILD_SKILL_LEVEL_UP_REQ*)pMsg;

	CGuildManager::Instance()->OnNetSkillLevelUp(pPlayer, pRecv->m_byType, pRecv->m_wSkillId, pRecv->m_wSkillLevel);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_CHAPTER_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_CHAPTER_REWARD_REQ* pRecv = (MSG_C2S_GUILD_FB_CHAPTER_REWARD_REQ*)pMsg;

	CGuildManager::Instance()->OnNetCopyChapterReward(pPlayer, pRecv->m_wChapter);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_DATA_REQ* pRecv = (MSG_C2S_GUILD_FB_DATA_REQ*)pMsg;

	CGuildManager::Instance()->OnNetGuildCopyData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_MAP_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_MAP_REWARD_REQ* pRecv = (MSG_C2S_GUILD_FB_MAP_REWARD_REQ*)pMsg;

	CGuildManager::Instance()->OnNetCopyMapReward(pPlayer, pRecv->m_wChapter, pRecv->m_byCamp, pRecv->m_byBoxIdx);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_MAP_REWARD_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_MAP_REWARD_RECORD_REQ* pRecv = (MSG_C2S_GUILD_FB_MAP_REWARD_RECORD_REQ*)pMsg;

	CGuildManager::Instance()->OnNetCopyMapRewardRecord(pPlayer, pRecv->m_wChapter, pRecv->m_byCamp);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_DAMAGE_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_DAMAGE_RECORD_REQ* pRecv = (MSG_C2S_GUILD_FB_DAMAGE_RECORD_REQ*)pMsg;

	CGuildManager::Instance()->OnNetCopyBattleRecord(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ* pRecv = (MSG_C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ*)pMsg;

	CGuildManager::Instance()->OnNetBuyCopyBattleTimes(pPlayer, pRecv->m_byBuyTimes);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_IMPEACH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_IMPEACH_REQ* pRecv = (MSG_C2S_GUILD_IMPEACH_REQ*)pMsg;

	CGuildManager::Instance()->OnNetImpeach(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_MONSTER_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_MONSTER_DATA_REQ* pRecv = (MSG_C2S_GUILD_MONSTER_DATA_REQ*)pMsg;

	CGuildManager::Instance()->OnNetMonsterData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_FB_CHAPTER_RESET_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_FB_CHAPTER_RESET_REQ* pRecv = (MSG_C2S_GUILD_FB_CHAPTER_RESET_REQ*)pMsg;

	CGuildManager::Instance()->OnNetFBChapterReset(pPlayer, pRecv->m_byIsHistoryRecord);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_MY_MAP_REWARD_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_MY_MAP_REWARD_RECORD_REQ* pRecv = (MSG_C2S_GUILD_MY_MAP_REWARD_RECORD_REQ*)pMsg;

	CGuildManager::Instance()->OnNetMyMapRewardRecord(pPlayer);
}










VOID HandlerFromGate::OnMSG_C2S_ARENA_TARGET_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ARENA_TARGET_LIST_REQ* pRecv = (MSG_C2S_ARENA_TARGET_LIST_REQ*)pMsg;

	CArenaManager::Instance()->OnNetTargetList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_ARENA_BATTLE_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ARENA_BATTLE_RECORD_REQ* pRecv = (MSG_C2S_ARENA_BATTLE_RECORD_REQ*)pMsg;

	CArenaManager::Instance()->OnNetBattleRecordList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_CHRISTMAS_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHRISTMAS_DATA_REQ* pRecv = (MSG_C2S_CHRISTMAS_DATA_REQ*)pMsg;

	CChristmas::Instance()->OnNetChristmasData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_CHRISTMAS_BATTLE_ENTER_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHRISTMAS_BATTLE_ENTER_REQ* pRecv = (MSG_C2S_CHRISTMAS_BATTLE_ENTER_REQ*)pMsg;

	CChristmas::Instance()->OnNetEnterBattle(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_CHRISTMAS_BATTLE_LEAVE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHRISTMAS_BATTLE_LEAVE_REQ* pRecv = (MSG_C2S_CHRISTMAS_BATTLE_LEAVE_REQ*)pMsg;

	CChristmas::Instance()->OnNetLeaveBattle(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_CHRISTMAS_ATTACK_BOSS_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHRISTMAS_ATTACK_BOSS_REQ* pRecv = (MSG_C2S_CHRISTMAS_ATTACK_BOSS_REQ*)pMsg;

	CChristmas::Instance()->OnNetAttackBoss(pPlayer, pRecv->m_wBossType, pRecv->m_dwSnowBallId);
}

VOID HandlerFromGate::OnMSG_C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ* pRecv = (MSG_C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ*)pMsg;

	CChristmas::Instance()->OnNetTakeBossReward(pPlayer, pRecv->m_wBossType);
}

VOID HandlerFromGate::OnMSG_C2S_FACEBOOK_ACTIVITY_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FACEBOOK_ACTIVITY_DATA_REQ* pRecv = (MSG_C2S_FACEBOOK_ACTIVITY_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetActivityData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ* pRecv = (MSG_C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetTakeAccountBindingReward();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ* pRecv = (MSG_C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetTakeShareReward();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ* pRecv = (MSG_C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetTakeCommendReward();
}



VOID HandlerFromGate::OnMSG_C2S_FACEBOOK_FRIEND_INVITE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FACEBOOK_FRIEND_INVITE_REQ* pRecv = (MSG_C2S_FACEBOOK_FRIEND_INVITE_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetFriendInvite(&pRecv->m_InviteFriendInfo);
}


VOID HandlerFromGate::OnMSG_C2S_FACEBOOK_FRIEND_SUMMON_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FACEBOOK_FRIEND_SUMMON_REQ* pRecv = (MSG_C2S_FACEBOOK_FRIEND_SUMMON_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetFriendSummon(pRecv->m_arFiendList, pRecv->m_wFriendNum);
}


VOID HandlerFromGate::OnMSG_C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ* pRecv = (MSG_C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetTakeFriendInviteReward(pRecv->m_wInviteNum);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ* pRecv = (MSG_C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetTakeFriendSummonReward(pRecv->m_wSummonNum);
}

VOID HandlerFromGate::OnMSG_C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ* pRecv = (MSG_C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetFacebookActivity().OnNetFriendInviteRecord();
}


VOID HandlerFromGate::OnMSG_C2S_ONLINE_REWARD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ONLINE_REWARD_DATA_REQ* pRecv = (MSG_C2S_ONLINE_REWARD_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetOnlineRewardData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_ONLINE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_ONLINE_REWARD_REQ* pRecv = (MSG_C2S_TAKE_ONLINE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->OnNetTakeOnlineReward();
}

VOID HandlerFromGate::OnMSG_C2S_SEVEN_DAY_LOGIN_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SEVEN_DAY_LOGIN_DATA_REQ* pRecv = (MSG_C2S_SEVEN_DAY_LOGIN_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetSevenDayLogin().OnNetSevenDayLoginData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ* pRecv = (MSG_C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetSevenDayLogin().OnNetTakeSevenDayLoginReward(pRecv->m_wDay);
}

VOID HandlerFromGate::OnMSG_C2S_RECHARGE_REWARD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RECHARGE_REWARD_DATA_REQ* pRecv = (MSG_C2S_RECHARGE_REWARD_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetRechargeReward().OnNetRechargeRewardData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_RECHARGE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_RECHARGE_REWARD_REQ* pRecv = (MSG_C2S_TAKE_RECHARGE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetRechargeReward().OnNetTakeRechargeReward(pRecv->m_wDay);
}


VOID HandlerFromGate::OnMSG_C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ* pRecv = (MSG_C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetRechargeReward().OnNetTakeExtraReward();
}

VOID HandlerFromGate::OnMSG_C2S_TOTAL_RECHARGE_REWARD_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TOTAL_RECHARGE_REWARD_DATA_REQ* pRecv = (MSG_C2S_TOTAL_RECHARGE_REWARD_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetTotalRechargeReward().OnNetTotalRechargeData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ* pRecv = (MSG_C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetTotalRechargeReward().OnNetTakeTotalRechargeReward(pRecv->m_wRewardId);
}

VOID HandlerFromGate::OnMSG_C2S_ND_LOTTERY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ND_LOTTERY_REQ* pRecv = (MSG_C2S_ND_LOTTERY_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetNDLottery(pRecv->m_wTimes, pRecv->m_byIsCostItem);
}

VOID HandlerFromGate::OnMSG_C2S_ND_LOTTERY_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_ND_LOTTERY_RECORD_REQ* pRecv = (MSG_C2S_ND_LOTTERY_RECORD_REQ*)pMsg;

	pPlayer->GetLogicManager()->OnNetNDLotteryRecordList();
}

VOID HandlerFromGate::OnMSG_C2S_HOT_SALE_GOODS_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HOT_SALE_GOODS_DATA_REQ* pRecv = (MSG_C2S_HOT_SALE_GOODS_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetHotSaleGoods().OnNetHotSaleGoodsData();
}


VOID HandlerFromGate::OnMSG_C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ* pRecv = (MSG_C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetHotSaleGoods().OnNetTakeRechargeReward(pRecv->m_wGoodsId);
}

// VOID HandlerFromGate::OnMSG_C2S_TURNTABLE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
// {
// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// 	MSG_C2S_TURNTABLE_DATA_REQ* pRecv = (MSG_C2S_TURNTABLE_DATA_REQ*)pMsg;
// 
// 	CTurntableManager::Instance()->OnNetTurntableData(pPlayer);
// }

VOID HandlerFromGate::OnMSG_C2S_TURNTABLE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TURNTABLE_DATA_REQ* pRecv = (MSG_C2S_TURNTABLE_DATA_REQ*)pMsg;

	CTurntableManager::Instance();


	CTurntableManager::Instance()->OnNetTurntableData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_TURNTABLE_LOTTERY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TURNTABLE_LOTTERY_REQ* pRecv = (MSG_C2S_TURNTABLE_LOTTERY_REQ*)pMsg;

	CTurntableManager::Instance()->OnNetTurntableLottery(pPlayer, pRecv->m_wLotteryType, pRecv->m_byIsCostItem);
}

VOID HandlerFromGate::OnMSG_C2S_CAMP_RECRUIT_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CAMP_RECRUIT_DATA_REQ* pRecv = (MSG_C2S_CAMP_RECRUIT_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetCampRecruit().OnNetRecruitData();
}

VOID HandlerFromGate::OnMSG_C2S_CAMP_RECRUIT_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CAMP_RECRUIT_REQ* pRecv = (MSG_C2S_CAMP_RECRUIT_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetCampRecruit().OnNetRecruit(pRecv->m_wRecuitType, pRecv->m_wCamp);
}


VOID HandlerFromGate::OnMSG_C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ* pRecv = (MSG_C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetCampRecruit().OnNetScoreTaskReward(pRecv->m_wCamp, pRecv->m_wRewardIdx);
}

VOID HandlerFromGate::OnMSG_C2S_CAMP_RECRUIT_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_CAMP_RECRUIT_RECORD_REQ* pRecv = (MSG_C2S_CAMP_RECRUIT_RECORD_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetCampRecruit().OnNetRecruitRecord(pRecv->m_wCamp);
}


VOID HandlerFromGate::OnMSG_C2S_UNLOCK_ACTIVITY_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_UNLOCK_ACTIVITY_DATA_REQ* pRecv = (MSG_C2S_UNLOCK_ACTIVITY_DATA_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetUnlockActivity().OnNetActivityData();
}

VOID HandlerFromGate::OnMSG_C2S_SHOP_SCORE_FINISH_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_SHOP_SCORE_FINISH_REQ* pRecv = (MSG_C2S_SHOP_SCORE_FINISH_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetUnlockActivity().OnNetShopScoreFinish();
}

VOID HandlerFromGate::OnMSG_C2S_WATCHING_VIDEO_NOTIFY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WATCHING_VIDEO_NOTIFY_REQ* pRecv = (MSG_C2S_WATCHING_VIDEO_NOTIFY_REQ*)pMsg;

	pPlayer->GetActivityManager()->GetUnlockActivity().OnNetWatchingVideoNotify();
}

VOID HandlerFromGate::OnMSG_C2S_WORLD_LEVEL_TASK_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_WORLD_LEVEL_TASK_DATA_REQ* pRecv = (MSG_C2S_WORLD_LEVEL_TASK_DATA_REQ*)pMsg;

	pPlayer->GetWorldLevelTaskManager()->OnNetWorldLevelTaskData();
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ* pRecv = (MSG_C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ*)pMsg;

	pPlayer->GetWorldLevelTaskManager()->OnNetTakeTaskReward(pRecv->m_wTaskId);
}

VOID HandlerFromGate::OnMSG_C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ* pRecv = (MSG_C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ*)pMsg;

	pPlayer->GetWorldLevelTaskManager()->OnNetTakeFinalReward();
}


VOID HandlerFromGate::OnMSG_C2S_PLAYER_RESOURCE_WAR_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_PLAYER_RESOURCE_WAR_DATA_REQ* pRecv = (MSG_C2S_PLAYER_RESOURCE_WAR_DATA_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetResourceWardData(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_ENEMY_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_ENEMY_LIST_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_ENEMY_LIST_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetEnemyList(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_BATTLE_RECORD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_BATTLE_RECORD_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_BATTLE_RECORD_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetBattleRecord(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetGiveUpOccupy(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetKeepOccupy(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_CHAPTER_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_CHAPTER_DATA_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_CHAPTER_DATA_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetChapterData(pPlayer, pRecv->m_wChapter);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_SAME_RESOURCE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_SAME_RESOURCE_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_SAME_RESOURCE_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetSameResourceList(pPlayer, pRecv->m_wResourceType);
}

VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetGuildResourceList(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ* pRecv = (MSG_C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ*)pMsg;

	CResourceWarManager::Instance()->OnNetUpgradeTechnology(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_ROLE_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_ROLE_DATA_REQ* pRecv = (MSG_C2S_GUILD_WAR_ROLE_DATA_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetGuildWarData(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ* pRecv = (MSG_C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetGuildOccupyStarList(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ* pRecv = (MSG_C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetGuildDeclareWarStarList(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_STAR_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_STAR_LIST_REQ* pRecv = (MSG_C2S_GUILD_WAR_STAR_LIST_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetStarList(pPlayer);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_DECLARE_WAR_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_DECLARE_WAR_REQ* pRecv = (MSG_C2S_GUILD_WAR_DECLARE_WAR_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetDeclareWar(pPlayer, pRecv->m_wStarId);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ* pRecv = (MSG_C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetOrganizeDefence(pPlayer, pRecv->m_wStarId, pRecv->m_wTowerType, pRecv->m_dwRoleId, pRecv->m_byIsToBattle);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_EVACUATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_EVACUATE_REQ* pRecv = (MSG_C2S_GUILD_WAR_EVACUATE_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetEvacuate(pPlayer, pRecv->m_wStarId);
}


VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ* pRecv = (MSG_C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetBuyHighStarBattleTimes(pPlayer);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ* pRecv = (MSG_C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetTakeStarReward(pPlayer, pRecv->m_wTakeType, pRecv->m_arStarList, pRecv->m_wStarNum);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_STAR_DETAIL_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_STAR_DETAIL_REQ* pRecv = (MSG_C2S_GUILD_WAR_STAR_DETAIL_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetStarDetail(pPlayer, pRecv->m_wStarId);
}

VOID HandlerFromGate::OnMSG_C2S_GUILD_WAR_TOWER_LIST_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUILD_WAR_TOWER_LIST_REQ* pRecv = (MSG_C2S_GUILD_WAR_TOWER_LIST_REQ*)pMsg;

	CGuildWarMgr::Instance()->OnNetTowerList(pPlayer, pRecv->m_wStarId);
}


VOID HandlerFromGate::OnMSG_C2S_HANG_UP_DROP_DATA_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HANG_UP_DROP_DATA_REQ* pRecv = (MSG_C2S_HANG_UP_DROP_DATA_REQ*)pMsg;

	pPlayer->GetHangUpDropManager()->OnNetHangUpDropData();
}

VOID HandlerFromGate::OnMSG_C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ* pRecv = (MSG_C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ*)pMsg;

	pPlayer->GetHangUpDropManager()->OnNetObtainReward();
}








































VOID HandlerFromGate::OnMSG_C2S_SERVER_TIME_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
 	MSG_C2S_SERVER_TIME_REQ* pRecv = (MSG_C2S_SERVER_TIME_REQ*)pMsg;
 
	time_t t = time(NULL);
	tm tLocalCurTime;
	_localtime64_s(&tLocalCurTime, &t);

 	MSG_S2C_SERVER_TIME_RESP msg;
	msg.m_dwServerTime = t;
	msg.m_wYear = tLocalCurTime.tm_year;
	msg.m_byMonth = tLocalCurTime.tm_mon;
	msg.m_byDay = tLocalCurTime.tm_mday;
	msg.m_byHour = tLocalCurTime.tm_hour;
	msg.m_byMinute = tLocalCurTime.tm_min;
	msg.m_bySecond = tLocalCurTime.tm_sec;
	msg.m_dwMakeTime = DATETIME(2016, 6, 2, 9, 30, 0);
 	pSession->SendPacket(dwGuid, msg.GetType(), &msg);

// 	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
// 	if (!pPlayer)
// 		return;
// 
// 	//服务器回发心跳包
// 	pPlayer->SendServerTimeCmd();
}

VOID HandlerFromGate::OnMSG_C2S_GUIDE_UPDATE_REQ(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
	if (!pPlayer)
		return;

	MSG_C2S_GUIDE_UPDATE_REQ* pRecv = (MSG_C2S_GUIDE_UPDATE_REQ*)pMsg;

	if (pPlayer->GetLogicManager())
		pPlayer->GetLogicManager()->OnNetGuideUpdate(pRecv->m_nKey, pRecv->m_nValue);
}




//客户端切断通知
VOID HandlerFromGate::OnMSG_G2M_CLIENT_DISCONNECT_CMD(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	AccountInfo* pAccInfo = AccountInfoManager::Instance()->GetAccountInfoByGUID(dwGuid);
	if (pAccInfo)
	{
		AccountInfoManager::Instance()->KickoutAccount(pAccInfo);
		MAPLOG(GUI_LEVEL, "客户端主线断开下线[account:%s]...", pAccInfo->GetAccID());
	}
}

VOID HandlerFromGate::OnMSG_G2M_HEART_BEAT(GateServerSession * pSession, NetMsgBody *pMsg, DWORD dwGuid)
{
	g_MapServer.InitGateBeat();
}

