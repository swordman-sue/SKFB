#include "StdAfx.h"
#include "DBThreadHandler.h"
#include "SqlDatabase.h"
#include "QueryResult.h"
#include "MapSvrQuery.h"
#include "TimeoutManager.h"
#include "../ToolKit/DBCFileManager.h"
#include "../ToolKit/DBCFile.h"
#include "../Common/DBCDefine.h"
#include "../Common/DataDefine.h"
#include "ConfigManager.h"
#include "../ToolKit/MiniDump.h"
#include "server.pb.h"
#include "data_define.pb.h"
#include "CommonLogic.h"
#include "MapServer.h"
#include "BaseDataDefine.h"

// 检测账号
void DBThreadHandler::DoCheckAccountQuery(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ENTER_DUMP_FUNCTION

	if(!pSqlDatabase || !pData)
		return;

	if(!pSqlDatabase->GetQuery())
		return;

	CheckAccountQuery* pQuery = (CheckAccountQuery*)pData; 

	unsigned char szAccout[2 * (MAX_ACCOUNT_LEN + 1) + 1] = { 0 };
	const char *pszAccount = pQuery->GetAccID();
	int nAccountLen = strlen(pszAccount);
	unsigned int nEscapeLen = pSqlDatabase->escape_string((char*)szAccout, pszAccount, nAccountLen);
	szAccout[sizeof(szAccout)-1] = '\0';

	DWORD arLastLoginServerIdList[MAX_LOGIN_SERVER_LIST] = {0};


	//处理SQL查询
	sprintf(pSqlDatabase->GetQuery(), "Select Password,mobile,is_lock,unlock_time,last_login_zoneid,last_login_zoneid_2,last_login_zoneid_3,\
		last_login_zoneid_4 From account Where account='%s' COLLATE utf8_bin and platform=%d", szAccout, pQuery->GetChannelUID());
	int nRet = pSqlDatabase->OpenRecordset();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (-1 == nRet)
	{
		MAPLOG(ERROR_LEVEL, "[%s]查询帐号数据出错[account:%s,error:%d]", __FUNCTION__,szAccout,pSqlDatabase->errorno());
		return;
	}
	// 取原密码
	DWORD dwZoneId = 0;
	char szMobile[MAX_MOBILE_LEN] = {0};
	BYTE byIsLock = false;
	DWORD dwUnlockTime = 0;
	if (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetString(i++, pQuery->GetOldPassword(), MAX_ACCOUNT_PASSWORD_LEN);
		pSqlDatabase->GetString(i++, szMobile, MAX_MOBILE_LEN-1);
		pSqlDatabase->GetBYTE(i++, byIsLock);
		pSqlDatabase->GetDWORD(i++, dwUnlockTime);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[0]);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[1]);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[2]);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[3]);
	}
	pQuery->SetResultRowNum(nRet);
	//pQuery->SetZoneID(dwZoneId);
	pSqlDatabase->CloseRecordset();
	pQuery->SetMobile(szMobile);
	pQuery->SetIsLock(byIsLock);
	pQuery->SetUnlockTime(dwUnlockTime);

	// 只填充ServerId
	LastLoginServerData tServerData;
	memset(&tServerData, 0, sizeof(tServerData));
	for (int i = 0; i < MAX_LOGIN_SERVER_LIST; i++)
	{
		tServerData.dwServerId = arLastLoginServerIdList[i];
		pQuery->SetLastLoginServer(i, tServerData);
	}

	LEAVE_DUMP_FUNCTION
}

// SDK帐号检测
void DBThreadHandler::DoSDKAccountCheck(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	if (!pSqlDatabase->GetQuery())
		return;

	CheckAccountQuery* pQuery = (CheckAccountQuery*)pData;

	unsigned char szAccout[2 * (MAX_ACCOUNT_LEN + 1) + 1] = { 0 };
	const char *pszAccount = pQuery->GetAccID();
	int nAccountLen = strlen(pszAccount);
	unsigned int nEscapeLen = pSqlDatabase->escape_string((char*)szAccout, pszAccount, nAccountLen);
	szAccout[sizeof(szAccout)-1] = '\0';

	//处理SQL查询
	sprintf(pSqlDatabase->GetQuery(), "Select Password,mobile,is_lock,unlock_time,last_login_zoneid,last_login_zoneid_2,last_login_zoneid_3,\
			last_login_zoneid_4 From account Where account='%s' COLLATE utf8_bin and platform=%d", szAccout, pQuery->GetChannelUID());
	int nRet = pSqlDatabase->OpenRecordset();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (-1 == nRet)
	{
		MAPLOG(ERROR_LEVEL, "[%s]查询帐号数据出错[account:%s,error:%d]", __FUNCTION__, szAccout, pSqlDatabase->errorno());
		return;
	}
	// 取原密码
	DWORD arLastLoginServerIdList[MAX_LOGIN_SERVER_LIST] = { 0 };
	DWORD dwZoneId = 0;
	char szMobile[MAX_MOBILE_LEN] = { 0 };
	BYTE byIsLock = false;
	DWORD dwUnlockTime = 0;
	if (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetString(i++, pQuery->GetOldPassword(), MAX_ACCOUNT_PASSWORD_LEN);
		pSqlDatabase->GetString(i++, szMobile, MAX_MOBILE_LEN - 1);
		pSqlDatabase->GetBYTE(i++, byIsLock);
		pSqlDatabase->GetDWORD(i++, dwUnlockTime);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[0]);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[1]);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[2]);
		pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[3]);
	}
	pQuery->SetResultRowNum(nRet);
	//pQuery->SetZoneID(dwZoneId);
	pSqlDatabase->CloseRecordset();
	pQuery->SetMobile(szMobile);
	pQuery->SetIsLock(byIsLock);
	pQuery->SetUnlockTime(dwUnlockTime);

	// 只填充ServerId
	LastLoginServerData tServerData;
	memset(&tServerData, 0, sizeof(tServerData));
	for (int i = 0; i < MAX_LOGIN_SERVER_LIST; i++)
	{
		tServerData.dwServerId = arLastLoginServerIdList[i];
		pQuery->SetLastLoginServer(i, tServerData);
	}
}

// 创建账号
void DBThreadHandler::DoCreateAccountQuery(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	if (!pSqlDatabase->GetQuery())
		return;

	CHAR szRegisterTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szRegisterTime);

	CreateAccountQuery* pQuery = (CreateAccountQuery*)pData;

	char szMobile[MAX_MOBILE_LEN] = { 0 };
	if (LOGIN_MOBILE_REGISTER == pQuery->GetLoginType())
	{
		strcpy_s(szMobile, sizeof(szMobile)-1, pQuery->GetAccID());
		pQuery->SetMobile(szMobile);
	}

	sprintf(pSqlDatabase->GetQuery(), "Insert Into account(account,password,platform,register_time,mobile,last_login_zoneid) \
		  		  			  VALUE ('%s', '%s', %d, '%s', '%s', %d)"
							  , pQuery->GetAccID()
							  , pQuery->GetPassword()
							  , pQuery->GetChannelUID()
							  , szRegisterTime
							  , szMobile
							  , 0);

	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("Insert Into account");
	}
}

// 创建账号(sdk)
void DBThreadHandler::DoSDKCreateAccount(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	if (!pSqlDatabase->GetQuery())
		return;

	CHAR szRegisterTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szRegisterTime);

	CreateAccountQuery* pQuery = (CreateAccountQuery*)pData;

	char szMobile[MAX_MOBILE_LEN] = { 0 };
	if (LOGIN_MOBILE_REGISTER == pQuery->GetLoginType())
	{
		strcpy_s(szMobile, sizeof(szMobile)-1, pQuery->GetAccID());
		pQuery->SetMobile(szMobile);
	}

	sprintf(pSqlDatabase->GetQuery(), "Insert Into account(account,password,platform,register_time,mobile,last_login_zoneid) \
			VALUE ('%s', '%s', %d, '%s', '%s', %d)"
			, pQuery->GetAccID()
			, pQuery->GetPassword()
			, pQuery->GetChannelUID()
			, szRegisterTime
			, szMobile
			, 0);

	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("Insert Into account");
	}
}

//查询分区角色
void DBThreadHandler::DoSelectZoneRoleQuery(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	SelectCharListQuery* pQuery = (SelectCharListQuery*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	pQuery->SetQueryResult(0);
}

//处理创建新角色
void DBThreadHandler::DoCreateRoleQuery(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION

	if(!pSqlDatabase || !pData)
		return;

	CreateCharInfoQuery* pQuery = (CreateCharInfoQuery*)pData;

	//1 创建角色基本信息
	if(!pSqlDatabase->GetQuery())
		return;

	char szCharName[2 * (MAX_ROLE_LEN + 1) + 1] = {0};
	pSqlDatabase->escape_string(szCharName,pQuery->GetRoleName(),strlen(pQuery->GetRoleName()));

	//更新玩家登陆时间
	CHAR szCreateTime[MAX_TIME_LEN] = {0};
	DATETIME CurTime(time(NULL));
	CurTime.toString(szCreateTime);

	DWORD dwHeadImageId = 30300; // todo::暂时介么处理

	// 基本信息
	PB::RoleBaseInfo PBRoleBaseInfo;
	PBRoleBaseInfo.set_account(pQuery->GetAccID());
	PBRoleBaseInfo.set_role_name(pQuery->GetRoleName());
	//PBRoleBaseInfo.set_role_id(pQuery->GetCharID());
	PBRoleBaseInfo.set_sex(pQuery->GetGender());
	PBRoleBaseInfo.set_level(1);
	PBRoleBaseInfo.set_head_image_id(dwHeadImageId);
	PBRoleBaseInfo.set_create_role_time(time(NULL));

	// 基本信息
	string strBaseInfoData("");
	if (!PBRoleBaseInfo.AppendToString(&strBaseInfoData) || strBaseInfoData.size() > MAX_ROLE_BASE_INFO_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色基本数据系列化后过大[size:%d]", __FUNCTION__, strBaseInfoData.size());
		return;
	}

	// 基本信息
	char szBaseInfo[MAX_ROLE_BASE_INFO_SIZE * 2 + 1] = { 0 };
	pSqlDatabase->escape_string(szBaseInfo, strBaseInfoData.c_str(), strBaseInfoData.size());

	sprintf(pSqlDatabase->GetQuery()
		, "Insert Into Role(role_name,account,server_id,channel_uid,terminal_type,sex,level,head_image_id,create_time,base_info) \
			VALUE ('%s', '%s', %d, %d, %d, %d, %d, %d, '%s', '%s')"
			, szCharName
			, pQuery->GetAccID()
			, pQuery->GetZoneID()
			, pQuery->GetChannelUID()
			, pQuery->GetTerminalType()
			, pQuery->GetGender()
			, 1
			, dwHeadImageId
			, szCreateTime
			, szBaseInfo);

	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if(nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("create Role");
		return;
	}

	//得到角色ID
	pQuery->SetCharID(pSqlDatabase->GetInsertId());

	//LEAVE_DUMP_FUNCTION
}

//处理查询角色游戏信息
void DBThreadHandler::DoSelectRoleInfoQuery(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION

	UINT64 lCurTick = GetTickCount64();

	if(!pSqlDatabase || !pData)
		return;

	SelectCharInfoQuery* pQuery = (SelectCharInfoQuery*)pData;

	// SQL 查询
	if(!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "Select create_time,base_info,hero_info,email_info,friend_info,reward_info,\
							activity_info,second_info,temp_data From role Where role_id=%lu", pQuery->GetCharID());
	int nRet = pSqlDatabase->OpenRecordset();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]查询角色信息出错[error:%d]!", __FUNCTION__, pSqlDatabase->errorno());
		return;
	}

	RoleFullInfo &tFullInfo = pQuery->GetRoleFullInfo();

	// 基本信息
	char szBaseInfo[MAX_ROLE_BASE_INFO_SIZE * 2 + 1] = { 0 };
	int nBaseInfoSize = 0;
	// 英雄
	char szHeroInfo[MAX_ROLE_HERO_INFO_SIZE * 2 + 1] = { 0 };
	int nHeroInfoSize = 0;
	// 邮件
	char szEmailInfo[MAX_ROLE_EMAIL_INFO_SIZE * 2 + 1] = { 0 };
	int nEmailInfoSize = 0;
	// 好友
	char szFriendInfo[MAX_ROLE_FRIEND_INFO_SIZE * 2 + 1] = { 0 };
	int nFriendInfoSize = 0;
	// 临时数据
	char szTempDataInfo[MAX_ROLE_TEMP_DATA_SIZE * 2 + 1] = { 0 };
	int nTempDataInfoSize = 0;
	// 二级信息
	char szSecondInfo[MAX_ROLE_SECOND_INFO_SIZE_EX * 2 + 1] = { 0 };
	int nSecondInfoSize = 0;
	// 奖励
	char szRewardInfo[MAX_ROLE_REWARD_DATA_SIZE * 2 + 1] = { 0 };
	int nRewardInfoSize = 0;
	// 活动
	char szActivityInfo[MAX_ACTIVITY_DATA_SIZE * 2 + 1] = { 0 };
	int nActivityInfoSize = 0;
	char szRoleCreateTime[32];
	if (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetString(i++, szRoleCreateTime, sizeof(szRoleCreateTime));
		nBaseInfoSize			= pSqlDatabase->GetBlob(i++, (unsigned char*)szBaseInfo);
		nHeroInfoSize			= pSqlDatabase->GetBlob(i++, (unsigned char*)szHeroInfo);
		nEmailInfoSize			= pSqlDatabase->GetBlob(i++, (unsigned char*)szEmailInfo);
		nFriendInfoSize			= pSqlDatabase->GetBlob(i++, (unsigned char*)szFriendInfo);
		nRewardInfoSize			= pSqlDatabase->GetBlob(i++, (unsigned char*)szRewardInfo);
		nActivityInfoSize		= pSqlDatabase->GetBlob(i++, (unsigned char*)szActivityInfo);
		nSecondInfoSize			= pSqlDatabase->GetBlob(i++, (unsigned char*)szSecondInfo);
		nTempDataInfoSize		= pSqlDatabase->GetBlob(i++, (unsigned char*)szTempDataInfo);
	}

	// 基本信息
	PB::RoleBaseInfo PBRoleBaseInfo;
	if (PBRoleBaseInfo.ParseFromArray(szBaseInfo, nBaseInfoSize))
	{
		RoleBaseInfo &rBaseInfo = tFullInfo.tRoleBaseInfo;
		rBaseInfo.dwRoleId = pQuery->GetCharID();
		strcpy_s(rBaseInfo.szRoleName, sizeof(rBaseInfo.szRoleName) - 1, PBRoleBaseInfo.role_name().c_str());
		strcpy_s(rBaseInfo.szAccount, sizeof(rBaseInfo.szAccount) - 1, PBRoleBaseInfo.account().c_str());
		rBaseInfo.dwStamina = PBRoleBaseInfo.stamina();
		rBaseInfo.dwEnergy = PBRoleBaseInfo.energy();
		rBaseInfo.wSex = PBRoleBaseInfo.sex();
		rBaseInfo.wLevel = PBRoleBaseInfo.level();
		rBaseInfo.wDailyInitLevel = PBRoleBaseInfo.daily_init_level();
		rBaseInfo.dwExp = PBRoleBaseInfo.exp();
		rBaseInfo.dwGold = PBRoleBaseInfo.gold();
		rBaseInfo.dwDiamond = PBRoleBaseInfo.diamond();
		rBaseInfo.dwHeadImage = PBRoleBaseInfo.head_image_id();
		rBaseInfo.sVipData.wLevel = PBRoleBaseInfo.vip_level();
		rBaseInfo.sVipData.dwExp = PBRoleBaseInfo.vip_exp();
		rBaseInfo.wRiotSuppressTimes = PBRoleBaseInfo.riot_suppress_times();
		rBaseInfo.nHeroSoul = PBRoleBaseInfo.hero_soul();
		rBaseInfo.nArenaPrestige = PBRoleBaseInfo.arena_prestige();
		rBaseInfo.nGodSoul = PBRoleBaseInfo.god_soul();
		rBaseInfo.nWanted = PBRoleBaseInfo.wanted();
		rBaseInfo.nTowerPrestige = PBRoleBaseInfo.tower_prestige();
		rBaseInfo.dwFightingPower = PBRoleBaseInfo.fighting_power();
		rBaseInfo.dwRebelValue = PBRoleBaseInfo.rebel_value();
		rBaseInfo.dwCurContribution = PBRoleBaseInfo.cur_contribution();
		rBaseInfo.nHonor = PBRoleBaseInfo.honor();
		rBaseInfo.wWorldBossBattleTimes = PBRoleBaseInfo.world_boss_battle_times();
		rBaseInfo.wGuildFBRewardChapter = PBRoleBaseInfo.guild_fb_reward_chapter();
		rBaseInfo.nFairySoul = PBRoleBaseInfo.fairy_soul();
		strcpy_s(rBaseInfo.szLastLoginTime, sizeof(rBaseInfo.szLastLoginTime) - 1, PBRoleBaseInfo.last_login_time().c_str());
		strcpy_s(rBaseInfo.szLastLogoutTime, sizeof(rBaseInfo.szLastLogoutTime) - 1, PBRoleBaseInfo.last_logout_time().c_str());
		rBaseInfo.dwOnlineTime = PBRoleBaseInfo.total_online_time();
		rBaseInfo.dwLoginTimes = PBRoleBaseInfo.login_times();
		rBaseInfo.dwLastStaminaRecoverTime = PBRoleBaseInfo.last_stamina_recover_time();
		rBaseInfo.dwLastEnergyRecoverTime = PBRoleBaseInfo.last_energy_recover_time();
		rBaseInfo.dwLastWantedRecoverTime = PBRoleBaseInfo.last_wanted_recover_time();
		rBaseInfo.dwDBPeaceOverTime = PBRoleBaseInfo.duobao_peace_over_time();
		rBaseInfo.nOnceFlagList = PBRoleBaseInfo.once_flag_list();
		rBaseInfo.nGuideX = PBRoleBaseInfo.guide_x();
		rBaseInfo.nGuideY = PBRoleBaseInfo.guide_y();
		rBaseInfo.dwLeaveGuildTime = PBRoleBaseInfo.leave_guild_time();
		rBaseInfo.wLastActivityNoticeId = PBRoleBaseInfo.last_activity_notice_id();

		rBaseInfo.wFriendshipPoint = PBRoleBaseInfo.friendship_point();
		rBaseInfo.wTodayFriendshipPoint = PBRoleBaseInfo.today_friendship_point();
		rBaseInfo.nEndlessStore = PBRoleBaseInfo.endless_stone();
		rBaseInfo.wNobilityLevel = PBRoleBaseInfo.nobility_level();
		rBaseInfo.dwUpdateTimes = PBRoleBaseInfo.update_times();
		rBaseInfo.dwTotalRechargeAmount = PBRoleBaseInfo.total_recharge_amount();
		rBaseInfo.nGemCoin = PBRoleBaseInfo.gem_coin();
		rBaseInfo.nDestinyCoin = PBRoleBaseInfo.destiny_coin();
		rBaseInfo.nStarSoul = PBRoleBaseInfo.star_soul();
		rBaseInfo.dwTurntableScore = PBRoleBaseInfo.turntable_score();
		rBaseInfo.wTurntableActivityDataId = PBRoleBaseInfo.turntable_activity_data_id();
		rBaseInfo.wTodayUsedHelpBattleTimes = PBRoleBaseInfo.today_used_help_battle_times();
		rBaseInfo.wLoginDays = PBRoleBaseInfo.role_login_days();
		rBaseInfo.nCrystal = PBRoleBaseInfo.crystal();
		rBaseInfo.dwLastLoginTime = PBRoleBaseInfo.last_login_time_ex();
		rBaseInfo.dwLastLogoutTime = PBRoleBaseInfo.last_logout_time_ex();
		rBaseInfo.dwRoleCreateTime = PBRoleBaseInfo.create_role_time();
		rBaseInfo.wIsAlreadyTakeClosedTestRebate = PBRoleBaseInfo.is_already_take_closed_test_rebate();
		rBaseInfo.nScoreMatchHonor = PBRoleBaseInfo.score_match_honor();

		rBaseInfo.nResouceWarScore = PBRoleBaseInfo.resource_war_score();
		rBaseInfo.nYCoin = PBRoleBaseInfo.ycoin();

		rBaseInfo.wTodayChatTimes = PBRoleBaseInfo.today_chat_times();
		


		for (int i = 0; i < PBRoleBaseInfo.formation_pro_list_size(); ++i)
		{
			if (i >= MAX_BATTLE_HERO_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]阵型槽位越界[slot:%d]!", __FUNCTION__, i);
				continue;
			}
			int *pProList = rBaseInfo.arFormationProList[i];
			const PB::FormationProperty &tPBFormationPro = PBRoleBaseInfo.formation_pro_list(i);
			for (int k = 0; k < tPBFormationPro.pro_list_size(); ++k)
			{
				const PB::Property &tPBPro = tPBFormationPro.pro_list(k);
				if (k >= HERO_PRO_MAX)
				{
					MAPLOG(ERROR_LEVEL, "[%s]属性类型越界[type:%d]!", __FUNCTION__, k);
					continue;
				}
				pProList[tPBPro.type()] = tPBPro.value();
			}
		}

		rBaseInfo.wGuideNum = 0;
		for (int i = 0; i < PBRoleBaseInfo.guide_list_size(); ++i)
		{
			const PB::GuideData &tPBGuideData = PBRoleBaseInfo.guide_list(i);
			rBaseInfo.arGuideList[i].nKey = tPBGuideData.key();
			rBaseInfo.arGuideList[i].nValue = tPBGuideData.value();
			++rBaseInfo.wGuideNum;
		}

		rBaseInfo.wActivationTakeRecordNum = 0;
		for (int i = 0; i < PBRoleBaseInfo.activation_code_take_record_size(); ++i)
		{
			rBaseInfo.arActivationTakeRecord[rBaseInfo.wActivationTakeRecordNum++] = PBRoleBaseInfo.activation_code_take_record(i);
		}

		// todo::数据纠正处理(兼容旧数据处理)
// 		if (!rBaseInfo.dwLastLoginTime)
// 		{
// 			// 上线时间
// 			DATETIME tDataTime;
// 			if (0 == tDataTime.ParseFromString(rBaseInfo.szLastLoginTime))
// 			{
// 				rBaseInfo.dwLastLoginTime = (time_t)tDataTime;
// 				MAPLOG(ERROR_LEVEL, "最近登录时间数据纠正处理[%d]!", rBaseInfo.dwLastLoginTime);
// 			}
// 		}
// 		if (!rBaseInfo.dwLastLogoutTime)
// 		{
// 			// 下线时间
// 			DATETIME tDataTime;
// 			if (0 == tDataTime.ParseFromString(rBaseInfo.szLastLogoutTime))
// 			{
// 				rBaseInfo.dwLastLogoutTime = (time_t)tDataTime;
// 				MAPLOG(ERROR_LEVEL, "最近下线时间数据纠正处理[%d]!", rBaseInfo.dwLastLogoutTime);
// 			}
// 		}
		if (!rBaseInfo.dwRoleCreateTime)
		{
			// 角色创建时间
			DATETIME tDataTime;
			tDataTime.ParseFromString(szRoleCreateTime);
			rBaseInfo.dwRoleCreateTime = (time_t)tDataTime;
			MAPLOG(ERROR_LEVEL, "角色创建时间数据纠正处理[%d]!", rBaseInfo.dwRoleCreateTime);
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色基本数据失败[RoleId:%d]!", __FUNCTION__, pQuery->GetCharID());
	}

	// 角色二级信息
	PB::RoleSecondInfo PBSecondInfo;
	if (nSecondInfoSize && PBSecondInfo.ParseFromArray(szSecondInfo, nSecondInfoSize))
	{
		// PVP数据
		PlayerPVPData &tPVPData = tFullInfo.tSecondInfo.tPVPData;
		const ::PB::RolePVPData &tPBPVPData = PBSecondInfo.pvp_data();
		tPVPData.wTodayBattleTimes = tPBPVPData.today_battle_times();
		tPVPData.wWeekBattleTimes = tPBPVPData.week_battle_times();
		tPVPData.dwScore = tPBPVPData.score();
		tPVPData.wWeekWinTimes = tPBPVPData.win_times();
		tPVPData.dwTotalBattleTimes = tPBPVPData.total_battle_times();
		tPVPData.wLastWeekRank = tPBPVPData.last_week_rank();
		tPVPData.wDailyRewardTakeRecordNum = 0;
		for (int i = 0; i < tPBPVPData.daily_reward_take_record_size(); ++i)
		{
			tPVPData.arDailyRewardTakeRecord[tPVPData.wDailyRewardTakeRecordNum++] = tPBPVPData.daily_reward_take_record(i);
		}
		tPVPData.wWeeklyRewardTakeRecordNum = 0;
		for (int i = 0; i < tPBPVPData.weekly_reward_take_record_size(); ++i)
		{
			tPVPData.arWeeklyRewardTakeRecord[tPVPData.wWeeklyRewardTakeRecordNum++] = tPBPVPData.weekly_reward_take_record(i);
		}

		// 称号
		PlayerTitleInfo &tRoleTitleInfo = tFullInfo.tSecondInfo.tTitleInfo;
		const ::PB::RoleTitleInfo &tPBTitleInfo = PBSecondInfo.title_info();
		tRoleTitleInfo.nCurUseTitleId = tPBTitleInfo.cur_use_title_id();
		tRoleTitleInfo.nCurUseNicknameId = tPBTitleInfo.cur_use_nickname_id();
		tRoleTitleInfo.wTitleNum = 0;
		for (int i = 0; i < tPBTitleInfo.title_list_size(); ++i)
		{
			const ::PB::TitleData &tPBTitleData = tPBTitleInfo.title_list(i);
			TitleData &tTitleData = tRoleTitleInfo.arTitleList[tRoleTitleInfo.wTitleNum++];
			tTitleData.nTitleId = tPBTitleData.id();
			tTitleData.nOverTime = tPBTitleData.over_time();
		}
		// 无尽之地数据
		PlayerEndlessLandData &tRoleEndlessLandInfo = tFullInfo.tSecondInfo.tEndlessLandData;
		const ::PB::RoleEndlessLandInfo &tPBEndlessLandInfo = PBSecondInfo.endless_land_info();
		tRoleEndlessLandInfo.wIsTodayFirstBattle = tPBEndlessLandInfo.is_today_first_battle();
		tRoleEndlessLandInfo.wCurWave = tPBEndlessLandInfo.cur_wave();
		tRoleEndlessLandInfo.wTodayWaveRecord = tPBEndlessLandInfo.today_wave_record();
		tRoleEndlessLandInfo.wCurWaveKillNum = tPBEndlessLandInfo.cur_wave_kill_num();
		tRoleEndlessLandInfo.wHistoryWaveRecord = tPBEndlessLandInfo.history_wave_record();
		tRoleEndlessLandInfo.wCurContinueKillNum = tPBEndlessLandInfo.cur_keep_kill_num();
		tRoleEndlessLandInfo.wContinueKillNumRecord = tPBEndlessLandInfo.keep_kill_num_record();
		tRoleEndlessLandInfo.wTodayCollisionRewardTimes = tPBEndlessLandInfo.today_collision_reward_times();
		tRoleEndlessLandInfo.wLastTakeContinueKillReward = tPBEndlessLandInfo.last_take_continue_kill_reward();
		tRoleEndlessLandInfo.wIsFirstBattle = tPBEndlessLandInfo.is_first_battle();
		tRoleEndlessLandInfo.wTodayBattleTimes = tPBEndlessLandInfo.today_battle_times();
		// 		tRoleEndlessLandInfo.wContinueKillRewardRecordNum = 0;
		// 		for (int i = 0; i < tPBEndlessLandInfo.keep_kill_reward_record_size(); ++i)
		// 		{
		// 			tRoleEndlessLandInfo.arContinueKillRewardRecord[i] = tPBEndlessLandInfo.keep_kill_reward_record(i);
		// 			tRoleEndlessLandInfo.wContinueKillRewardRecordNum++;
		// 		}
		// 角色工会数据
		PlayerGuildData &tRoleGuildData = tFullInfo.tSecondInfo.tGuildData;
		const ::PB::RoleGuildData &tPBRoleGuildData = PBSecondInfo.guild_data();
		tRoleGuildData.wTakeChapterRewardRecordNum = 0;
		for (int i = 0; i < tPBRoleGuildData.take_chapter_reward_record_size(); ++i)
		{
			tRoleGuildData.arTakeChapterRewardRecord[i] = tPBRoleGuildData.take_chapter_reward_record(i);
			tRoleGuildData.wTakeChapterRewardRecordNum++;
		}
		// 豪华签到数据
		PlayerLuxurySignData &tLuxurySignInData = tFullInfo.tSecondInfo.tLuxurySignData;
		const ::PB::PlayerLuxurySignInData &tPBLuxurySignInData = PBSecondInfo.luxury_sign_in_data();
		tLuxurySignInData.wSignDataNum = 0;
		for (int i = 0; i < tPBLuxurySignInData.sign_in_data_list_size(); ++i)
		{
			const ::PB::LuxurySignInData &PBSignInData = tPBLuxurySignInData.sign_in_data_list(i);
			LuxurySignData &luxurySignInData = tLuxurySignInData.arSignDataList[tLuxurySignInData.wSignDataNum++];
			luxurySignInData.wSignId = PBSignInData.sign_in_id();
			luxurySignInData.wStatus = PBSignInData.status();
			luxurySignInData.wRewardId = PBSignInData.rewar_id();
		}
		// 宝石信息
		PlayerGemInfo &tGemInfo = tFullInfo.tSecondInfo.tGemInfo;
		const ::PB::PlayerGemInfo &tPBGemInfo = PBSecondInfo.gem_info();
		tGemInfo.dwHoleInitTime = tPBGemInfo.hole_init_time();
		tGemInfo.wGemNum = 0;
		for (int i = 0; i < tPBGemInfo.gem_list_size(); ++i)
		{
			const ::PB::Gem &tPBGem = tPBGemInfo.gem_list(i);
			Gem &tGem = tGemInfo.arGemList[tGemInfo.wGemNum++];
			tGem.dwId = tPBGem.id();
			tGem.dwNum = tPBGem.num();
		}
		for (int i = 0; i < tPBGemInfo.hole_open_list_size(); ++i)
		{
			tGemInfo.arHoleOpenList[i] = tPBGemInfo.hole_open_list(i);
		}
		// 登录奖励数据
		LoginRewardData &tLoginRewardData = tFullInfo.tSecondInfo.tLoginRewardData;
		const ::PB::LoginRewardData &tPBLoginRewardData = PBSecondInfo.login_reward_data();
		tLoginRewardData.wLoginDays = tPBLoginRewardData.login_days();
		tLoginRewardData.wRewardRecordNum = 0;
		for (int i = 0; i < tPBLoginRewardData.reward_record_list_size(); ++i)
		{
			tLoginRewardData.arRewardRecordList[i] = tPBLoginRewardData.reward_record_list(i);
			tLoginRewardData.wRewardRecordNum++;
		}
		// 王者赛数据
		PlayerKingMatchData &tKingMatchData = tFullInfo.tSecondInfo.tKingMatchData;
		const ::PB::RoleKingMatchData &tPBKingMatchData = PBSecondInfo.king_match_data();
		tKingMatchData.wLastSeason = tPBKingMatchData.last_season();
		tKingMatchData.wTotalBattleTimes = tPBKingMatchData.total_battle_times();
		tKingMatchData.wTotalWinTimes = tPBKingMatchData.total_win_times();
		tKingMatchData.wTodayWinTimes = tPBKingMatchData.today_win_times();
		tKingMatchData.wStageLevel = tPBKingMatchData.stage_level();
		tKingMatchData.dwContext = tPBKingMatchData.context();
		tKingMatchData.wHistoryStageLevel = tPBKingMatchData.history_stage_level();
		tKingMatchData.wSeasonRewardRecordNum = 0;
		for (int i = 0; i < tPBKingMatchData.season_task_reward_record_size(); ++i)
		{
			tKingMatchData.arSeasonTaskRewardRecord[i] = tPBKingMatchData.season_task_reward_record(i);
			tKingMatchData.wSeasonRewardRecordNum++;
		}

		// 组队副本数据
		{
			PlayerTeamDungeonData &tTeamDungeonData = tFullInfo.tSecondInfo.tTeamDungeonData;
			const ::PB::PlayerTeamDungeonData &tPBTeamDungeonData = PBSecondInfo.team_dungeon_data();
			tTeamDungeonData.wTodayBattleTimes = tPBTeamDungeonData.today_battle_times();
			tTeamDungeonData.wTodayKillRewardTimes = tPBTeamDungeonData.today_kill_reward_times();
			tTeamDungeonData.wTodayMVPRewardTimes = tPBTeamDungeonData.today_mvp_reward_times();
			for (int i = 0; i < tPBTeamDungeonData.pass_dungeon_list_size(); ++i)
			{
				tTeamDungeonData.arPassDungeonList[i] = tPBTeamDungeonData.pass_dungeon_list(i);
				tTeamDungeonData.wPassDungeonNum++;
			}
		}

		// 世界等级任务数据
		{
			PlayerWorldLevelTaskData &tRoleWorldLevelTaskData = tFullInfo.tSecondInfo.tRoleWorldLevelTaskData;
			const ::PB::RoleWorldLevelTaskInfo &tPBRoleWorldLevelTaskData = PBSecondInfo.role_world_level_task_info();
			tRoleWorldLevelTaskData.wIsAlreadyTakeFinalReward = tPBRoleWorldLevelTaskData.is_already_take_final_reward();
			tRoleWorldLevelTaskData.wTaskEventNum = 0;
			for (WORD i = 0; i < tPBRoleWorldLevelTaskData.task_event_list_size(); ++i)
			{
				const ::PB::WorldLevelTaskData &tPBTaskData = tPBRoleWorldLevelTaskData.task_event_list(i);
				WorldLevelTaskEvent &tTaskEvent = tRoleWorldLevelTaskData.arTaskEventList[tRoleWorldLevelTaskData.wTaskEventNum++];
				tTaskEvent.wTaskId = tPBTaskData.task_id();
				tTaskEvent.nContext = tPBTaskData.context();
				tTaskEvent.wIsAlreadyTake = tPBTaskData.is_already_take();
				tTaskEvent.wTaskType = tPBTaskData.task_type();
				tTaskEvent.wTaskLevel = tPBTaskData.task_level();
			}
		}

		// 挂机掉落数据
		{
			PlayerHangUpDropData &tRoleHangUpDropData = tFullInfo.tSecondInfo.tHangUpDropData;
			const ::PB::RoleHangUpDropData &tPBRoleHangUpDropData = PBSecondInfo.role_hang_up_drop_data();
			tRoleHangUpDropData.dwLastObtainRewardTime = tPBRoleHangUpDropData.last_obtain_reward_time();
		}
	}
	else if (nSecondInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]解析角色二级数据失败!", __FUNCTION__);
	}

	// 装备
	{
		const PB::RoleEquipmentInfo &PBRoleEquipmentInfo = PBSecondInfo.role_equipment_info();
		PlayerEquipmentInfo &rEquipmentInfos = tFullInfo.tEquipmentInfo;
		rEquipmentInfos.wEquipmentNum = 0;
		for (int i = 0; i < PBRoleEquipmentInfo.equipment_list_size(); ++i)
		{
			if (rEquipmentInfos.wEquipmentNum >= MAX_EQUIPMENT_NUM)
				break;

			const PB::Equipment &tPBEquipment = PBRoleEquipmentInfo.equipment_list(i);
			Equipment &tEquipment = rEquipmentInfos.sEquipmentList[rEquipmentInfos.wEquipmentNum++];
			tEquipment.dwId = tPBEquipment.uid();
			tEquipment.dwInfoId = tPBEquipment.info_id();
			tEquipment.wEnhanceLevel = tPBEquipment.enhance_level();
			tEquipment.wRefineLevel = tPBEquipment.refine_level();
			tEquipment.dwRefineExp = tPBEquipment.refine_exp();
			tEquipment.nEnhanceCostGold = tPBEquipment.enhance_cost_gold();
			tEquipment.wStar = tPBEquipment.star();
			tEquipment.wStarLevel = tPBEquipment.star_level();
			tEquipment.wRefineCostItemNum = 0;
			for (int k = 0; k < tPBEquipment.refine_cost_item_list_size(); ++k)
			{
				if (tEquipment.wRefineCostItemNum >= MAX_EQUIPMENT_REFINE_COST_ITEM)
					break;

				const PB::Item &tPBItem = tPBEquipment.refine_cost_item_list(k);
				Item *pItem = &tEquipment.arRefineCostItem[tEquipment.wRefineCostItemNum++];
				pItem->dwId = tPBItem.item_id();
				pItem->dwNum = tPBItem.item_num();
			}
		}
	}

	// 英雄
	PB::RoleHeroInfo PBRoleHeroInfo;
	if (nHeroInfoSize && PBRoleHeroInfo.ParseFromArray(szHeroInfo, nHeroInfoSize))
	{

		PlayerHeroInfo &tRoleHeroInfo = tFullInfo.tHeroInfo;
		tRoleHeroInfo.wNormalFreeSummonTimes = PBRoleHeroInfo.normal_free_summon_times();
		tRoleHeroInfo.dwNormalFreeSummonTime = PBRoleHeroInfo.normal_free_summon_time();
		tRoleHeroInfo.dwHighFreeSummonTime = PBRoleHeroInfo.high_free_summon_time();
		tRoleHeroInfo.wHighSummonTimes = PBRoleHeroInfo.high_summon_times();
		tRoleHeroInfo.wTrainTimes = PBRoleHeroInfo.train_times();

		// 英雄列表
		tRoleHeroInfo.wHeroNum = 0;
		for (int i = 0; i < PBRoleHeroInfo.hero_list_size(); ++i)
		{
			const PB::Hero &tPBHero = PBRoleHeroInfo.hero_list(i);
			HeroInfo &tHero = tRoleHeroInfo.arHeroList[tRoleHeroInfo.wHeroNum++];
			tHero.dwUID = tPBHero.uid();
			tHero.dwIntoId = tPBHero.info_id();
			tHero.wLevel = tPBHero.level();
			tHero.dwExp = tPBHero.exp();
			tHero.wQuality = tPBHero.quality();
			tHero.wBreakLevel = tPBHero.break_level();
			tHero.wDestinyLevel = tPBHero.destiny_level();
			tHero.dwDestinyExp = tPBHero.destiny_exp();
			tHero.wAwakenStar = tPBHero.awaken_star();
			tHero.wAwakenLevel = tPBHero.awaken_level();
			tHero.wQualityStage = tPBHero.quality_stage();
			tHero.wAwakenItemNum = 0;
			for (int k = 0; k < tPBHero.awaken_item_list_size(); ++k)
			{
				tHero.arAwakenItemList[tHero.wAwakenItemNum++] = tPBHero.awaken_item_list(k);
			}
			for (int k = 0; k < tPBHero.train_data_list_size(); ++k)
			{
				const PB::HeroTrainData &tTrainData = tPBHero.train_data_list(k);
				int nProType = tTrainData.pro_type();
				DWORD nProIdx = nProType - 1;
				if (nProIdx >= HERO_TRAIN_PRO_NUM)
				{
					MAPLOG(ERROR_LEVEL, "[%s]属性索引越界[ProIdx:%d]!", __FUNCTION__, nProIdx);
					continue;
				}
				tHero.arTrainProLvList[nProIdx] = tTrainData.pro_level();
				tHero.arTrainProValueList[nProIdx] = tTrainData.pro_value();
				tHero.arLastTrainTmpProList[nProIdx] = tTrainData.tmp_pro_value();
				tHero.arTrainRewardFlag[nProIdx] = tTrainData.reward_flag();
			}
			tHero.dwTrainCostItemNum = tPBHero.train_cost_item_num();
			tHero.dwDestinyCostItemNum = tPBHero.destiny_cost_item_num();
			tHero.nTrainCostDiamond = tPBHero.train_cost_item_num();
			tHero.nTrainCostGold = tPBHero.train_cost_gold();
		}
		// 阵型数据列表
		for (int i = 0; i < PBRoleHeroInfo.formation_list_size(); ++i)
		{
			if (i >= MAX_BATTLE_HERO_NUM)
				break;
			Formation &tFormation = tRoleHeroInfo.arFormationList[i];
			const PB::Formation &tPBFormation = PBRoleHeroInfo.formation_list(i);
			tFormation.dwHeroUID = tPBFormation.hero_id();
			tFormation.dwBlessSpriteId = tPBFormation.bless_sprite_id();
			for (int k = 0; k < tPBFormation.equipment_list_size(); ++k)
			{
				if (k < EQUIPMENT_TYPE_MAX)
					tFormation.arEquipmentList[k] = tPBFormation.equipment_list(k);
			}
			for (int k = 0; k < tPBFormation.treasure_list_size(); ++k)
			{
				if (k < TREASURE_TYPE_MAX)
					tFormation.arTreasureList[k] = tPBFormation.treasure_list(k);
			}
			for (int k = 0; k < tPBFormation.gem_list_size(); ++k)
			{
				if (k < MAX_GEM_HOLE_NUM)
					tFormation.arGemList[k] = tPBFormation.gem_list(k);
			}
			for (int k = 0; k < tPBFormation.card_list_size(); ++k)
			{
				if (k < MAX_CARD_SLOT_NUM)
					tFormation.arCardList[k] = tPBFormation.card_list(k);
			}
		}
		// 援军列表
		for (int i = 0; i < PBRoleHeroInfo.reinforcement_list_size(); ++i)
		{
			if (i < MAX_REINFORCEMENT_NUM)
				tRoleHeroInfo.arReinforcementList[i] = PBRoleHeroInfo.reinforcement_list(i);
		}
		// 英雄收集列表
		tRoleHeroInfo.wCollectionNum = 0;
		for (int i = 0; i < PBRoleHeroInfo.collection_list_size(); ++i)
		{
			if (tRoleHeroInfo.wCollectionNum < MAX_HERO_NUM)
				tRoleHeroInfo.arCollectionList[tRoleHeroInfo.wCollectionNum++] = PBRoleHeroInfo.collection_list(i);
		}
	}
	else if (nHeroInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色英雄数据失败!", __FUNCTION__);
	}

	// 宝物
	{
		const PB::RoleTreasureInfo &PBRoleTreasureInfo = PBSecondInfo.role_treasure_info();
		PlayerTreasureInfo &rTreasureInfos = tFullInfo.tTreasureInfo;
		rTreasureInfos.wTreasureNum = 0;
		for (int i = 0; i < PBRoleTreasureInfo.treasure_list_size(); ++i)
		{
			if (rTreasureInfos.wTreasureNum >= MAX_TREASURE_NUM)
				break;
			const PB::Treasure &tPBTreasure = PBRoleTreasureInfo.treasure_list(i);
			Treasure &tTreasure = rTreasureInfos.sTreasureList[rTreasureInfos.wTreasureNum++];
			tTreasure.dwId = tPBTreasure.uid();
			tTreasure.dwInfoId = tPBTreasure.info_id();
			tTreasure.wEnhanceLevel = tPBTreasure.enhance_level();
			tTreasure.dwEnhanceExp = tPBTreasure.enhance_exp();
			tTreasure.wRefineLevel = tPBTreasure.refine_level();
		}
	}

	// 物品
	{
		const PB::RoleItemInfo &PBRoleItemInfo = PBSecondInfo.role_item_info();
		PlayerItemInfo &rItemInfos = tFullInfo.tItemInfo;
		rItemInfos.wListNum = 0;
		for (int i = 0; i < PBRoleItemInfo.item_list_size(); ++i)
		{
			if (rItemInfos.wListNum >= MAX_ITEM_NUM)
				break;
			const PB::Item &tPBItem = PBRoleItemInfo.item_list(i);
			Item &tItem = rItemInfos.arItemList[rItemInfos.wListNum++];
			tItem.dwId = tPBItem.item_id();
			tItem.dwNum = tPBItem.item_num();
		}
	}

	// 占卜数据
	{
		const PB::RoleZhanBuData &PBRoleZhanbuInfo = PBSecondInfo.role_zhanbu_data();
		PlayerZhanBuInfo &tZhanBuInfo = tFullInfo.tZhanBuInfo;
		tZhanBuInfo.wCardNum = 0;
		for (int i = 0; i < PBRoleZhanbuInfo.card_list_size(); ++i)
		{
			if (tZhanBuInfo.wCardNum >= MAX_CARD_NUM)
				break;

			const PB::Card &tPBCard = PBRoleZhanbuInfo.card_list(i);
			Card &tCard = tZhanBuInfo.arCardList[tZhanBuInfo.wCardNum++];
			tCard.dwId = tPBCard.uid();
			tCard.dwInfoId = tPBCard.info_id();
			tCard.wLevel = tPBCard.level();
			tCard.dwExp = tPBCard.exp();
			tCard.wBreakLevel = tPBCard.break_level();
		}
		tZhanBuInfo.dwHistoryCollectionValue = PBRoleZhanbuInfo.history_collection_value();
		tZhanBuInfo.wLowCardGroupMapIdx = PBRoleZhanbuInfo.low_card_group_map_idx();
		tZhanBuInfo.wCurLowCardGroupId = PBRoleZhanbuInfo.cur_low_card_group_id();
		tZhanBuInfo.dwLowCardGroupLastRecoverTime = PBRoleZhanbuInfo.low_card_group_last_recover_time();
		tZhanBuInfo.wCurLowCardGroupHaveRefreshTimes = PBRoleZhanbuInfo.cur_low_card_group_refresh_times();
		tZhanBuInfo.wTodayLowCardGroupOpenCardTimes = PBRoleZhanbuInfo.today_low_card_group_open_card_times();
		tZhanBuInfo.wTodayLowCardGroupCostRefreshTimes = PBRoleZhanbuInfo.today_low_card_group_cost_refresh_times();
		tZhanBuInfo.wTodayHighCardGroupCostRefreshTimes = PBRoleZhanbuInfo.today_high_card_group_cost_refresh_times();
		tZhanBuInfo.dwHighCardGroupLastRefreshTime = PBRoleZhanbuInfo.high_card_group_last_refresh_time();
		tZhanBuInfo.wAchievementLevel = PBRoleZhanbuInfo.achievement_level();
		tZhanBuInfo.wLowCardGroupNum = 0;
		for (int i = 0; i < PBRoleZhanbuInfo.low_card_group_size(); ++i)
		{
			if (tZhanBuInfo.wLowCardGroupNum >= MAX_CARD_GROUP_NUM)
				break;

			const PB::CardGroupData &tPBCardGroup = PBRoleZhanbuInfo.low_card_group(i);
			CardGroupData &tCardGroup = tZhanBuInfo.arLowCardGroup[tZhanBuInfo.wLowCardGroupNum++];
			tCardGroup.dwCardId = tPBCardGroup.card_id();
			tCardGroup.wIsAlreadyOpen = tPBCardGroup.is_already_open();
		}
		tZhanBuInfo.wHighCardGroupNum = 0;
		for (int i = 0; i < PBRoleZhanbuInfo.high_card_group_size(); ++i)
		{
			if (tZhanBuInfo.wHighCardGroupNum >= MAX_CARD_GROUP_NUM)
				break;

			const PB::CardGroupData &tPBCardGroup = PBRoleZhanbuInfo.high_card_group(i);
			CardGroupData &tCardGroup = tZhanBuInfo.arHighCardGroup[tZhanBuInfo.wHighCardGroupNum++];
			tCardGroup.dwCardId = tPBCardGroup.card_id();
			tCardGroup.wIsAlreadyOpen = tPBCardGroup.is_already_open();
		}
		tZhanBuInfo.wCollectionBookNum = 0;
		for (int i = 0; i < PBRoleZhanbuInfo.collection_book_list_size(); ++i)
		{
			const PB::CollectionBook &tPBCollectionBook = PBRoleZhanbuInfo.collection_book_list(i);
			CollectionBook &tCollectionBook = tZhanBuInfo.arCollectionBookList[tZhanBuInfo.wCollectionBookNum++];
			tCollectionBook.wBookId = tPBCollectionBook.book_id();
			tCollectionBook.wCollectionGroupNum = 0;
			for (int k = 0; k < tPBCollectionBook.collection_group_list_size(); ++k)
			{
				const PB::CollectionGroup &tPBCollectionGroup = tPBCollectionBook.collection_group_list(k);
				CollectionGroup &tCollectionGroup = tCollectionBook.arCollectionGroupList[tCollectionBook.wCollectionGroupNum++];
				tCollectionGroup.wId = tPBCollectionGroup.id();
				tCollectionGroup.wStar = tPBCollectionGroup.star();
			}
		}
	}

	// 邮件
	PB::RoleEmailInfo PBRoleEmailInfo;
	if (nEmailInfoSize && PBRoleEmailInfo.ParseFromArray(szEmailInfo, nEmailInfoSize))
	{
		PlayerEmailInfo &tEmailInfos = tFullInfo.tEmailInfo;
		tEmailInfos.wIsHaveNewEmail = PBRoleEmailInfo.is_have_new_email();
		tEmailInfos.wEmailCount = 0;
		for (int i = 0; i < PBRoleEmailInfo.email_list_size(); ++i)
		{
			const PB::EmailInfo &tPBEmailInfo = PBRoleEmailInfo.email_list(i);
			EmailInfo &tEmail = tEmailInfos.arEmailList[tEmailInfos.wEmailCount++];
			tEmail.dwID = tPBEmailInfo.uid();
			tEmail.wInfoId = tPBEmailInfo.info_id();
			tEmail.dwReceiveTime = tPBEmailInfo.receive_time();
			tEmail.wContextNum = 0;
			for (int k = 0; k < tPBEmailInfo.context_list_size(); ++k)
			{
				const ::PB::EmailContext &tPBContext = tPBEmailInfo.context_list(k);
				EmailContext &tContext = tEmail.arContextList[tEmail.wContextNum++];
				tContext.wType = tPBContext.type();
				strcpy_s(tContext.szText, sizeof(tContext.szText) - 1, tPBContext.text().c_str());
			}
			strcpy_s(tEmail.szText, sizeof(tEmail.szText) - 1, tPBEmailInfo.text().c_str());
			strcpy_s(tEmail.szTitle, sizeof(tEmail.szTitle) - 1, tPBEmailInfo.title().c_str());
		}
	}
	else if (nEmailInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色邮件数据失败[RoleId:%d]!", __FUNCTION__, pQuery->GetCharID());
	}

	// 副本
	{
		const PB::RoleFBInfo &PBRoleFBInfo = PBSecondInfo.role_fb_info();
		PlayerFubenInfo &tRoleFBInfo = tFullInfo.tFubenInfo;
		// 主线副本数据
		MainFBData &tMainFBData = tRoleFBInfo.tMainFubenData;
		tMainFBData.wTakeChapterRewardRecord = PBRoleFBInfo.main_fb_data().take_chapter_reward_record();
		tMainFBData.wChapter = 0;
		for (int i = 0; i < PBRoleFBInfo.main_fb_data().chapter_list_size(); ++i)
		{
			ChapterData &tChapter = tMainFBData.arChapterList[tMainFBData.wChapter++];
			const PB::ChapterData &tPBChapter = PBRoleFBInfo.main_fb_data().chapter_list(i);
			tChapter.byMapIdx = 0;
			for (int k = 0; k < tPBChapter.map_list_size(); ++k)
			{
				MapData &tMap = tChapter.arMapList[tChapter.byMapIdx++];
				const PB::MapData &tPBMap = tPBChapter.map_list(k);
				tMap.byStar = tPBMap.star();
				tMap.byBattleTimes = tPBMap.battle_times();
				tMap.byResetTimes = tPBMap.reset_times();
				tMap.byIsGetAward = tPBMap.is_get_reward();
			}
			for (int k = 0; k < tPBChapter.star_reward_list_size(); ++k)
			{
				tChapter.arStarAwardList[k] = tPBChapter.star_reward_list(k);
			}
		}
		// 名将副本数据
		const PB::HeroFBData &tPBHeroFBData = PBRoleFBInfo.hero_fb_data();
		HeroFBData &tHeroFBData = tRoleFBInfo.tHeroFubenData;
		tHeroFBData.wChapter = 0;
		tHeroFBData.byBattleTimes = tPBHeroFBData.battle_times();
		for (int i = 0; i < tPBHeroFBData.chapter_list_size(); ++i)
		{
			HeroFubenChapterData &tChapter = tHeroFBData.arChapterList[tHeroFBData.wChapter++];
			tChapter.byMapIdx = 0;
			const PB::HeroFBChapter &tPBChapter = tPBHeroFBData.chapter_list(i);
			tChapter.byIsExtraMapPass = tPBChapter.is_pass_extra_map();
			for (int k = 0; k < tPBChapter.map_list_size(); ++k)
			{
				HeroMapData &tMap = tChapter.arMapList[tChapter.byMapIdx++];
				const PB::HeroMapData &tPBMap = tPBChapter.map_list(k);
				tMap.byBattleTimes = tPBMap.battle_times();
				tMap.byIsGetAward = tPBMap.is_get_reward();
			}
		}
		// 日常副本数据
		DailyFBData &tDailyFBData = tRoleFBInfo.tDailyFubenData;
		const PB::DailyFBData &tPBDailyFBData = PBRoleFBInfo.daily_fb_data();
		tDailyFBData.wChapterNum = 0;
		for (int i = 0; i < tPBDailyFBData.chapter_list_size(); ++i)
		{
			const ::PB::DailyChapterData &tPBChapterData = tPBDailyFBData.chapter_list(i);
			DailyChapterData &tChapter = tDailyFBData.arChapterList[tDailyFBData.wChapterNum++];
			tChapter.wChapter = tPBChapterData.chapter();
			tChapter.nMapIdxRecord = tPBChapterData.max_idx_record();
			tChapter.byBattleTimes = tPBChapterData.battle_times();
		}
		// 爬塔副本数据
		TowerFBData &tTowerFBData = tRoleFBInfo.tTowerFubenData;
		const PB::TowerFBData &tPBTowerFBData = PBRoleFBInfo.tower_fb_data();
		tTowerFBData.wCurMapLevel = 0;
		for (int i = 0; i < tPBTowerFBData.map_star_list_size(); ++i)
		{
			tTowerFBData.arMapStarList[tTowerFBData.wCurMapLevel++] = tPBTowerFBData.map_star_list(i);
		}
		tTowerFBData.wMapLevelRecord = tPBTowerFBData.map_level_record();
		tTowerFBData.byKeepThreeStarPassedRecord = tPBTowerFBData.keep_best_star_passed_record();
		tTowerFBData.bySelectProNum = 0;
		for (int i = 0; i < tPBTowerFBData.select_pro_list_size(); ++i)
		{
			Property &tPro = tTowerFBData.arSelectProList[tTowerFBData.bySelectProNum++];
			const PB::Property &tPBPro =  tPBTowerFBData.select_pro_list(i);
			tPro.wType = tPBPro.type();
			tPro.nValue = tPBPro.value();
		}
		tTowerFBData.byIsBuyStarGoods = tPBTowerFBData.is_buy_star_goods();
		tTowerFBData.byStarGoodsIndex = tPBTowerFBData.star_goods_idx();
		tTowerFBData.byIsNeedReset = tPBTowerFBData.is_need_reset();
		tTowerFBData.byTodayResetTimes = tPBTowerFBData.today_reset_times();
		tTowerFBData.wCurStar = tPBTowerFBData.cur_star();
		tTowerFBData.wStarMaxRecord = tPBTowerFBData.star_max_record();
		tTowerFBData.wEliteMapLevel = tPBTowerFBData.elite_map_level();
		tTowerFBData.byEliteMapBattleTimes = tPBTowerFBData.elite_map_battle_times();
		tTowerFBData.byBuyEliteMapBattleTimes = tPBTowerFBData.buy_elite_map_battle_times();
		tTowerFBData.byRandProNum = 0;
		for (int i = 0; i < tPBTowerFBData.rand_pro_idx_list_size(); ++i)
		{
			tTowerFBData.arRandProIdxList[tTowerFBData.byRandProNum++] = tPBTowerFBData.rand_pro_idx_list(i);
		}
		// 精英副本数据
		EliteFBData &tEilteFBData = tRoleFBInfo.tEliteFBData;
		const PB::EliteFBData &tPBEilteFBData = PBRoleFBInfo.elite_fb_data();
		tEilteFBData.wChapter = 0;
		tEilteFBData.dwLastRefreshEnemyTime = tPBEilteFBData.last_refresh_enemy_time();
		for (int i = 0; i < tPBEilteFBData.chapter_list_size(); ++i)
		{
			EliteFBChapter &tChapter = tEilteFBData.arChapterList[tEilteFBData.wChapter++];
			const PB::EliteFBChapter &tPBChapter = tPBEilteFBData.chapter_list(i);
			tChapter.byMapIdx = 0;
			for (int k = 0; k < tPBChapter.map_list_size(); ++k)
			{
				MapData &tMap = tChapter.arMapList[tChapter.byMapIdx++];
				const PB::MapData &tPBMap = tPBChapter.map_list(k);
				tMap.byStar = tPBMap.star();
				tMap.byBattleTimes = tPBMap.battle_times();
				tMap.byResetTimes = tPBMap.reset_times();
				tMap.byIsGetAward = tPBMap.is_get_reward();
			}
			for (int k = 0; k < tPBChapter.star_reward_list_size(); ++k)
			{
				tChapter.arStarAwardList[k] = tPBChapter.star_reward_list(k);
			}
			const PB::EliteMapEnemyData &tPBEnemyData = tPBChapter.enemy_data();
			tChapter.tEnemyData.wMapId = tPBEnemyData.map_id();
			tChapter.tEnemyData.wIsDead = tPBEnemyData.is_dead();
			tChapter.tEnemyData.byMonsterNum = 0;
			for (int k = 0; k < tPBEnemyData.monster_list_size(); ++k)
			{
				const PB::MonsterData &tPBMonster = tPBEnemyData.monster_list(k);
				MonsterData &tMonster = tChapter.tEnemyData.arMonsterList[tChapter.tEnemyData.byMonsterNum++];
				tMonster.byIndex = tPBMonster.idx();
				tMonster.dwDamage = tPBMonster.damage();
				tMonster.dwTotalHP = tPBMonster.total_hp();
			}
		}
	}

	// 商店
	{
		const PB::RoleShopInfo &PBRoleShopInfo = PBSecondInfo.role_shop_info();

		// 英雄商店数据
		RefreshShopData &tHeroShop = tFullInfo.tShopInfo.tHeroShop;
		const PB::HeroShopData &tPBHeroShop = PBRoleShopInfo.hero_shop();
		tHeroShop.wGoodsNum = 0;
		for (int i = 0; i < tPBHeroShop.goods_list_size(); ++i)
		{
			ShopGoodsBuyRecord &tRecord = tHeroShop.arGoodsList[tHeroShop.wGoodsNum++];
			const PB::ShopGoods &tPBGoods = tPBHeroShop.goods_list(i);
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tHeroShop.dwLastRecoverTime = tPBHeroShop.last_recover_time();
		tHeroShop.wFreeRefreshTimes = tPBHeroShop.free_refresh_times();
		tHeroShop.wCostRefreshTimes = tPBHeroShop.cost_refresh_times();
		// 工会商品数据
		const PB::GuildShopData &tPBGuildShop = PBRoleShopInfo.guild_shop();
		PlayerGuildShopData &tGuildShop = tFullInfo.tShopInfo.tGuildShop;
		tGuildShop.wAwardGoodsRecordNum = 0;
		for (int i = 0; i < tPBGuildShop.reward_goods_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBGuildShop.reward_goods_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tGuildShop.arAwardGoodsRecord[tGuildShop.wAwardGoodsRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tGuildShop.wItemGoodsRecordNum = 0;
		for (int i = 0; i < tPBGuildShop.item_goods_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBGuildShop.item_goods_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tGuildShop.arItemGoodsRecord[tGuildShop.wItemGoodsRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tGuildShop.wLimitGoodsRecordNum = 0;
		for (int i = 0; i < tPBGuildShop.limit_goods_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBGuildShop.limit_goods_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tGuildShop.arLimitGoodsRecord[tGuildShop.wLimitGoodsRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		// 商城
		const PB::MallShopData &tPBMallShop = PBRoleShopInfo.mall_shop();
		PlayerMallData &tMallShop = tFullInfo.tShopInfo.tMallShop;
		tMallShop.wItemBuyRecordNum = 0;
		for (int i = 0; i < tPBMallShop.item_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBMallShop.item_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tMallShop.arItemBuyRecord[tMallShop.wItemBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tMallShop.wGiftBuyRecordNum = 0;
		for (int i = 0; i < tPBMallShop.gift_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBMallShop.gift_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tMallShop.arGiftBuyRecord[tMallShop.wGiftBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		// 爬塔
		const PB::TowerShopData &tPBTowerShop = PBRoleShopInfo.tower_shop();
		PlayerTowerShopData &tTowerShop = tFullInfo.tShopInfo.tTowerShop;
		tTowerShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBTowerShop.buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBTowerShop.buy_record(i);
			ShopGoodsBuyRecord &tRecord = tTowerShop.arBuyRecord[tTowerShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		// 竞技场
		const PB::ArenaShopData &tPBAraneShop = PBRoleShopInfo.arena_shop();
		PlayerArenaShopData &tArenaShop = tFullInfo.tShopInfo.tArenaShop;
		tArenaShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBAraneShop.buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBAraneShop.buy_record(i);
			ShopGoodsBuyRecord &tRecord = tArenaShop.arBuyRecord[tArenaShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		// 觉醒
		const PB::AwakenShopData &tPBAwakenShop = PBRoleShopInfo.awaken_shop();
		RefreshShopData &tAwakenShop = tFullInfo.tShopInfo.tAwakenShop;
		tAwakenShop.wGoodsNum = 0;
		for (int i = 0; i < tPBAwakenShop.goods_list_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBAwakenShop.goods_list(i);
			ShopGoodsBuyRecord &tRecord = tAwakenShop.arGoodsList[tAwakenShop.wGoodsNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tAwakenShop.dwLastRecoverTime = tPBAwakenShop.last_recover_time();
		tAwakenShop.wFreeRefreshTimes = tPBAwakenShop.free_refresh_times();
		tAwakenShop.wCostRefreshTimes = tPBAwakenShop.cost_refresh_times();
		// 仙灵
		const PB::FairyShopData &tPBFairyShop = PBRoleShopInfo.fairy_shop();
		StarSoulShopData &tStarSoulShop = tFullInfo.tShopInfo.tStarSoulShop;
		tStarSoulShop.wGoodsNum = 0;
		for (int i = 0; i < tPBFairyShop.goods_list_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBFairyShop.goods_list(i);
			ShopGoodsBuyRecord &tRecord = tStarSoulShop.arGoodsList[tStarSoulShop.wGoodsNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tStarSoulShop.wUsedRefreshTimes = tPBFairyShop.used_refresh_times();
		tStarSoulShop.dwLastAutoRefreshTime = tPBFairyShop.last_auto_refresh_time();
		// 积分赛
		const PB::ScoreMatchShopData &tPBScoreMatchShop = PBRoleShopInfo.score_match_shop();
		ScoreMatchShopData &tScoreMatchShop = tFullInfo.tShopInfo.tScoreMatchShop;
		tScoreMatchShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBScoreMatchShop.buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBScoreMatchShop.buy_record(i);
			ShopGoodsBuyRecord &tRecord = tScoreMatchShop.arBuyRecord[tScoreMatchShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		// 7天商店
		const PB::SevenDayShopData &tPBSevenDayShop = PBRoleShopInfo.seven_day_shop();
		SevenDayShopData &tSevenDayShop = tFullInfo.tShopInfo.tSevenShop;
		tSevenDayShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBSevenDayShop.buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBSevenDayShop.buy_record(i);
			ShopGoodsBuyRecord &tRecord = tSevenDayShop.arBuyRecord[tSevenDayShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}

		// 等级礼包商店
		const PB::LevelGiftShopData &tPBLevelGiftShop = PBRoleShopInfo.level_gift_shop();
		LevelGiftShopData &tLevelGiftShop = tFullInfo.tShopInfo.tLevelGiftShop;
		tLevelGiftShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBLevelGiftShop.buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBLevelGiftShop.buy_record(i);
			ShopGoodsBuyRecord &tRecord = tLevelGiftShop.arBuyRecord[tLevelGiftShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		
		// 无尽之地商店
		{
			const PB::EndlessLandShopData &tPBEndlessLandShop = PBRoleShopInfo.endless_land_shop();
			EndlessLandShopData &tEndlessLandShop = tFullInfo.tShopInfo.tEndlessLandShop;
			tEndlessLandShop.wItemGoodsBuyRecordNum = 0;
			for (int i = 0; i < tPBEndlessLandShop.item_goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBEndlessLandShop.item_goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tEndlessLandShop.arItemGoodsBuyRecord[tEndlessLandShop.wItemGoodsBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
			tEndlessLandShop.wRewardGoodsBuyRecordNum = 0;
			for (int i = 0; i < tPBEndlessLandShop.reward_goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBEndlessLandShop.reward_goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tEndlessLandShop.arRewardGoodsBuyRecord[tEndlessLandShop.wRewardGoodsBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
		}

		// 直购商店
		const PB::DirectBuyShopData &tPBDirectBuyShop = PBRoleShopInfo.direct_buy_shop();
		DirectBuyShopData &tDirectBuyShop = tFullInfo.tShopInfo.tDirectBuyShop;
		tDirectBuyShop.wGoodsNum = 0;
		for (int i = 0; i < tPBDirectBuyShop.goods_list_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBDirectBuyShop.goods_list(i);
			ShopGoodsBuyRecord &tRecord = tDirectBuyShop.arGoodsList[tDirectBuyShop.wGoodsNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}
		tDirectBuyShop.wLastRefreshTime = tPBDirectBuyShop.last_refresh_time();
		tDirectBuyShop.wLastRefreshYearDay = tPBDirectBuyShop.last_refresh_year_day();
		tDirectBuyShop.wCanTakeGoodsNum = 0;
		for (int i = 0; i < tPBDirectBuyShop.can_take_goods_list_size(); ++i)
		{
			tDirectBuyShop.arCanTakeGoodsList[tDirectBuyShop.wCanTakeGoodsNum++] = tPBDirectBuyShop.can_take_goods_list(i);
		}

		// 兑换商店
		const PB::ExchangeShopData &tPBExchangeShopData = PBRoleShopInfo.exchange_shop();
		ExchangeShopData &tExchangeShop = tFullInfo.tShopInfo.tExchangeShop;
		tExchangeShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBExchangeShopData.goods_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBExchangeShopData.goods_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tExchangeShop.arBuyRecord[tExchangeShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}

		// 推荐商店
		const PB::RecommendShopData &tPBRecommendShop = PBRoleShopInfo.recommend_shop();
		RecommendShopData &tRecommendShop = tFullInfo.tShopInfo.tRecommendShop;
		tRecommendShop.wBuyRecordNum = 0;
		for (int i = 0; i < tPBRecommendShop.goods_buy_record_size(); ++i)
		{
			const PB::ShopGoods &tPBGoods = tPBRecommendShop.goods_buy_record(i);
			ShopGoodsBuyRecord &tRecord = tRecommendShop.arBuyRecord[tRecommendShop.wBuyRecordNum++];
			tRecord.wGoodsId = tPBGoods.goods_id();
			tRecord.wBuyNum = tPBGoods.goods_num();
		}

		// 组队商店
		{
			const PB::TeamDungeonShop &tPBTeamDungeonShop = PBRoleShopInfo.teamdungeon_shop();
			TeamDungeonShopData &tTeamDungeonShop = tFullInfo.tShopInfo.tTeamDungeonShop;
			tTeamDungeonShop.wNormalGoodsBuyRecordNum = 0;
			for (int i = 0; i < tPBTeamDungeonShop.normal_goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBTeamDungeonShop.normal_goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tTeamDungeonShop.arNormalGoodsBuyRecord[tTeamDungeonShop.wNormalGoodsBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
			tTeamDungeonShop.wRewardGoodsBuyRecordNum = 0;
			for (int i = 0; i < tPBTeamDungeonShop.reward_goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBTeamDungeonShop.reward_goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tTeamDungeonShop.arRewardGoodsBuyRecord[tTeamDungeonShop.wRewardGoodsBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
		}

		// 圣诞节商店
		{
			const PB::ChristmasShop &tPBChristmasShop = PBRoleShopInfo.christmas_shop();
			ChristmasShopData &tChristmasShop = tFullInfo.tShopInfo.tChristmasShop;
			tChristmasShop.wScoreExchangeGoodsBuyRecordNum = 0;
			for (int i = 0; i < tPBChristmasShop.score_exchange_goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBChristmasShop.score_exchange_goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tChristmasShop.arScoreExchangeGoodsBuyRecord[tChristmasShop.wScoreExchangeGoodsBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
			tChristmasShop.wItemExchangeGoodsBuyRecordNum = 0;
			for (int i = 0; i < tPBChristmasShop.item_exchange_goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBChristmasShop.item_exchange_goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tChristmasShop.arItemExchangeGoodsBuyRecord[tChristmasShop.wItemExchangeGoodsBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
		}

		// 水晶商店
		{
			const PB::CrystalShop &tPBCrystalShop = PBRoleShopInfo.crystal_shop();
			CrystalShopData &tCrystalShopData = tFullInfo.tShopInfo.tCrystalShopData;
			tCrystalShopData.wTodayRefreshTimes = tPBCrystalShop.today_refresh_times();
			tCrystalShopData.wGoodsNum = 0;
			for (int i = 0; i < tPBCrystalShop.goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBCrystalShop.goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tCrystalShopData.arGoodsList[tCrystalShopData.wGoodsNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
		}

		// 资源战商店
		{
			const PB::ResourceWarShop &tPBResourceWarShop = PBRoleShopInfo.resource_war_shop();
			ResourceWarShopData &tResourceWarShop = tFullInfo.tShopInfo.tResourceWarShop;
			tResourceWarShop.wBuyRecordNum = 0;
			for (int i = 0; i < tPBResourceWarShop.goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBResourceWarShop.goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tResourceWarShop.arBuyRecord[tResourceWarShop.wBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
		}

		// GM商店
		{
			const PB::GMShopData &tPBGMShop = PBRoleShopInfo.gm_shop();
			GMShopData &tGMShop = tFullInfo.tShopInfo.tGMShop;
			tGMShop.wBuyRecordNum = 0;
			for (int i = 0; i < tPBGMShop.goods_buy_record_size(); ++i)
			{
				const PB::ShopGoods &tPBGoods = tPBGMShop.goods_buy_record(i);
				ShopGoodsBuyRecord &tRecord = tGMShop.arBuyRecord[tGMShop.wBuyRecordNum++];
				tRecord.wGoodsId = tPBGoods.goods_id();
				tRecord.wBuyNum = tPBGoods.goods_num();
			}
		}
	}

	// 精灵
	{
		const PB::RoleSpiritInfo &PBRoleSpiritInfo = PBSecondInfo.role_sprite_info();

		PlayerSpriteInfo &tRoleSpriteInfo = tFullInfo.tSpiritInfo;
		tRoleSpriteInfo.tHeartOfWorld.wStage = PBRoleSpiritInfo.stage();
		tRoleSpriteInfo.tHeartOfWorld.wLevel = PBRoleSpiritInfo.level();
		tRoleSpriteInfo.dwCurUseSpiritId = PBRoleSpiritInfo.cur_use_spirit_id();
		tRoleSpriteInfo.wSpiritNum = 0;
		for (int i = 0; i < PBRoleSpiritInfo.spirit_list_size(); ++i)
		{
			const ::PB::SpiritData tPBSpirit =  PBRoleSpiritInfo.spirit_list(i);
			tRoleSpriteInfo.arSpiritList[i].dwId = tPBSpirit.id();
			tRoleSpriteInfo.arSpiritList[i].wEnhanceLevel = tPBSpirit.enhance_level();
			tRoleSpriteInfo.arSpiritList[i].wTrainStage = tPBSpirit.train_stage();
			tRoleSpriteInfo.arSpiritList[i].wTrainTimes = tPBSpirit.train_times();
			tRoleSpriteInfo.arSpiritList[i].wStarLevel = tPBSpirit.star();
			tRoleSpriteInfo.wSpiritNum++;
		}
		tRoleSpriteInfo.wPokedexNum = 0;
		for (int i = 0; i < PBRoleSpiritInfo.pokedex_list_size(); ++i)
		{
			const ::PB::SpiritPokedex tPBPokedex = PBRoleSpiritInfo.pokedex_list(i);
			tRoleSpriteInfo.arPokedexList[i].wPokedexId = tPBPokedex.id();
			tRoleSpriteInfo.arPokedexList[i].wLevel = tPBPokedex.level();
			tRoleSpriteInfo.wPokedexNum++;
		}
	}

	// 好友
	PB::RoleFriendData PBRoleFriendInfo;
	if (nFriendInfoSize && PBRoleFriendInfo.ParseFromArray(szFriendInfo, nFriendInfoSize))
	{
		PlayerFriendData &tRoleFriendInfo = tFullInfo.tFriendInfo;
		tRoleFriendInfo.wGetEnergyTimes = PBRoleFriendInfo.get_energy_times();
		for (int i = 0; i < PBRoleFriendInfo.black_list_size(); ++i)
		{
			if (tRoleFriendInfo.wBlackNum >= MAX_BLACK_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]黑名单数量超标!", __FUNCTION__);
				break;
			}
			tRoleFriendInfo.arBlackList[tRoleFriendInfo.wBlackNum++] = PBRoleFriendInfo.black_list(i);
		}
		for (int i = 0; i < PBRoleFriendInfo.apply_list_size(); ++i)
		{
			if (tRoleFriendInfo.wApplyNum >= MAX_FRIEND_APPLY_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]好友申请数量超标!", __FUNCTION__);
				break;
			}
			tRoleFriendInfo.arApplyList[tRoleFriendInfo.wApplyNum++] = PBRoleFriendInfo.apply_list(i);
		}
		tRoleFriendInfo.wFriendNum = 0;
		for (int i = 0; i < PBRoleFriendInfo.friend_list_size(); ++i)
		{
			if (tRoleFriendInfo.wFriendNum >= MAX_FRIEND_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]好友数量超标!", __FUNCTION__);
				break;
			}
			const PB::FriendData &tPBFriend = PBRoleFriendInfo.friend_list(i);
			FriendData &tFriendData = tRoleFriendInfo.arFriendList[tRoleFriendInfo.wFriendNum++];
			tFriendData.wIsGiveEnergy = tPBFriend.is_give_energy();
			tFriendData.wTakeEnergyState = tPBFriend.get_energy_state();
			tFriendData.dwFriendId = tPBFriend.role_id();
			tFriendData.wHelpBattleTimes = tPBFriend.help_battle_times();
		}
	}
	else if (nFriendInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]解析角色好友数据失败!", __FUNCTION__);
	}

	// 叛军
	{
		const PB::RoleRebelInfo &PBRoleRebelInfo = PBSecondInfo.role_rebel_info();

		PlayerRebelData &tRoleRebelInfo = tFullInfo.tRebelInfo;
		tRoleRebelInfo.wRebelLevel = PBRoleRebelInfo.rebel_level();
		tRoleRebelInfo.dwTodayExploit = PBRoleRebelInfo.today_exploit();
		tRoleRebelInfo.dwTodayDamage = PBRoleRebelInfo.today_damage();
		tRoleRebelInfo.wIsAlreadyShare = PBRoleRebelInfo.is_already_share();
		tRoleRebelInfo.wAwardRecordNum = 0;
		for (int i = 0; i < PBRoleRebelInfo.reward_get_record_list_size(); ++i)
		{
			if (tRoleRebelInfo.wAwardRecordNum >= MAX_EXPLOIT_AWARD_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]叛军奖励数量超标!", __FUNCTION__);
				break;
			}

			tRoleRebelInfo.arAwardRecordList[tRoleRebelInfo.wAwardRecordNum++] = PBRoleRebelInfo.reward_get_record_list(i);
		}
	}

	// 夺宝 
	{
		const PB::RoleDuobaoInfo &PBRoleDuobaoInfo = PBSecondInfo.role_duobao_info();

		PlayerDuobaoInfo &tRoleDuobaoInfo = tFullInfo.tDuobaoInfo;
		tRoleDuobaoInfo.dwPeaceOverTime = PBRoleDuobaoInfo.peace_over_time();
		for (int i = 0; i < PBRoleDuobaoInfo.doubao_times_list_size() && i < QUALITY_NUM; ++i)
		{
			tRoleDuobaoInfo.arDoubaoTimes[i] = PBRoleDuobaoInfo.doubao_times_list(i);
		}
		tRoleDuobaoInfo.wDailyDoubaoTimes = PBRoleDuobaoInfo.daily_duobao_times();
		tRoleDuobaoInfo.wPregrossRewardRecordNum = 0;
		for (int i = 0; i < PBRoleDuobaoInfo.pregross_reward_record_size() && i < MAX_DOUBAO_PREGROSS_NUM; ++i)
		{
			tRoleDuobaoInfo.arPregrossRewardRecord[tRoleDuobaoInfo.wPregrossRewardRecordNum++] = PBRoleDuobaoInfo.pregross_reward_record(i);
		}
	}

	// 临时数据
	PB::RoleTempDatas PBRoleTempDatas;
	if (nTempDataInfoSize && PBRoleTempDatas.ParseFromArray(szTempDataInfo, nTempDataInfoSize))
	{
		RoleTempDatas &tRoleTempDatas = tFullInfo.tTempDatas;
		tRoleTempDatas.wTempDataNum = 0;
		for (int i = 0; i < PBRoleTempDatas.temp_data_list_size(); ++i)
		{
			if (tRoleTempDatas.wTempDataNum >= MAX_ROLE_TEMP_DATA_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]临时数据数量超标!", __FUNCTION__);
				break;
			}

			RoleTempData &tTempData = tRoleTempDatas.arTempDataList[tRoleTempDatas.wTempDataNum++];
			const PB::RoleTempData &tPBTempData = PBRoleTempDatas.temp_data_list(i);
			tTempData.wType = tPBTempData.type();
			strcpy_s(tTempData.szText, sizeof(tTempData.szText), tPBTempData.text().c_str());
			tTempData.wParamNum = 0;
			for (int k = 0; k < tPBTempData.param_list_size(); ++k)
			{
				tTempData.arParamList[tTempData.wParamNum++] = tPBTempData.param_list(k);
			}
		}
	}
	else if (nTempDataInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]解析角色临时数据失败!", __FUNCTION__);
	}


	// 成就
	{
		const PB::RoleAchievemntInfo &PBRoleAchievemntInfo = PBSecondInfo.role_achievement_info();

		PlayerAchievementData &tRoleAchievementInfo = tFullInfo.tAchievementInfo;
		tRoleAchievementInfo.wAchievementNum = 0;
		for (int i = 0; i < PBRoleAchievemntInfo.achievement_list_size(); ++i)
		{
			if (tRoleAchievementInfo.wAchievementNum >= MAX_ACHIEVEMENT_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]成就数量超标!", __FUNCTION__);
				break;
			}

			const PB::AchievementData &tPBAchievement =  PBRoleAchievemntInfo.achievement_list(i);
			AchievementData &tAchievement = tRoleAchievementInfo.arAchievementList[tRoleAchievementInfo.wAchievementNum++];
			tAchievement.wId = tPBAchievement.id();
			tAchievement.byLv = tPBAchievement.level();
			tAchievement.dwValue = tPBAchievement.value();
			tAchievement.byCurRewardLevel = tPBAchievement.cur_reward_level();
		}
	}

	// 日常任务
	{
		const PB::RoleDailyMissionInfo &PBRoleDailyMissionInfo = PBSecondInfo.role_daily_mission_info();

		PlayerDailyMissionData &tRoleDailyMissionData = tFullInfo.tDailyMissionInfo;
		tRoleDailyMissionData.wPoint = PBRoleDailyMissionInfo.point();
		// 积分奖励记录列表
		tRoleDailyMissionData.wPointAwardRecordNum = 0;
		for (int i = 0; i < PBRoleDailyMissionInfo.point_reward_record_size(); ++i)
		{
			if (tRoleDailyMissionData.wPointAwardRecordNum >= MAX_DAILY_MISSION_POINT_AWARD)
			{
				MAPLOG(ERROR_LEVEL, "[%s]日常任务积分奖励数量过多!", __FUNCTION__);
				break;
			}
			tRoleDailyMissionData.arPointAwardRecord[tRoleDailyMissionData.wPointAwardRecordNum++]
				= PBRoleDailyMissionInfo.point_reward_record(i);
		}
		// 日常任务列表
		tRoleDailyMissionData.wDailyMissionNum = 0;
		for (int i = 0; i < PBRoleDailyMissionInfo.mission_list_size(); ++i)
		{
			if (tRoleDailyMissionData.wDailyMissionNum >= MAX_DAILY_MISSION_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]日常任务数量过多!", __FUNCTION__);
				break;
			}
			DailyMissionData &tMission = tRoleDailyMissionData.arDailyMissionList[tRoleDailyMissionData.wDailyMissionNum++];
			const PB::DailyMission &tDailyMission = PBRoleDailyMissionInfo.mission_list(i);
			tMission.wMissionId = tDailyMission.id();
			tMission.byIsGetAward = tDailyMission.is_get_reward();
			tMission.dwValue = tDailyMission.value();
		}
	}

	// 工会技能
	{
		const PB::RoleGuildSkillInfo &PBRoleGuildSkillInfo = PBSecondInfo.role_guild_skill_info();
		PlayerGuildSkillData &tRoleGuildSkillData = tFullInfo.tGuildSkillData;
		tRoleGuildSkillData.wSkillNum = 0;
		for (int i = 0; i < PBRoleGuildSkillInfo.skill_list_size(); ++i)
		{
			if (tRoleGuildSkillData.wSkillNum >= MAX_GUILD_SKILL_NUM)
			{
				MAPLOG(ERROR_LEVEL, "[%s]工会技能数量过多!", __FUNCTION__);
				break;
			}
			const PB::SkillData &tPBSkillData = PBRoleGuildSkillInfo.skill_list(i);
			SkillData &tSkillData = tRoleGuildSkillData.arSkillList[tRoleGuildSkillData.wSkillNum++];
			tSkillData.wSkillId = tPBSkillData.skill_id();
			tSkillData.wLevel = tPBSkillData.level();
		}
	}

	// 签到
	{
		const PB::RoleSignInData &PBRoleSignInInfo = PBSecondInfo.role_sign_in_data();
		SignInData &tRoleSignInData = tFullInfo.tSignInData;
		tRoleSignInData.wCurSignInDay = PBRoleSignInInfo.cur_sign_in_day();
		tRoleSignInData.dwLastSignInTime = PBRoleSignInInfo.last_sign_in_time();
	}

	// 奖励数据
	PB::RoleRewardData PBRoleRewardInfo;
	if (nRewardInfoSize && PBRoleRewardInfo.ParseFromArray(szRewardInfo, nRewardInfoSize))
	{
		PlayerRewardData &tRoleRewardData = tFullInfo.tRewardInfo;
		tRoleRewardData.wRewardNum = 0;
		for (int i = 0; i < PBRoleRewardInfo.reward_list_size(); ++i)
		{
			const PB::RewardData &PBReward = PBRoleRewardInfo.reward_list(i);
			RewardData *pReward = &tRoleRewardData.arRewardList[tRoleRewardData.wRewardNum++];
			pReward->dwUId = PBReward.uid();
			pReward->wInfoId = PBReward.info_id();
			pReward->dwValue1 = PBReward.value1();
			pReward->dwValue2 = PBReward.value2();
			pReward->dwTime = PBReward.time();
			strcpy_s(pReward->szText, sizeof(pReward->szText) - 1, PBReward.text().c_str());
		}
	}
	else if (nRewardInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]解析角色奖励数据失败!", __FUNCTION__);
	}

	// 活动数据
	PB::RoleActivityData PBRoleActivityInfo;
	if (nActivityInfoSize && PBRoleActivityInfo.ParseFromArray(szActivityInfo, nActivityInfoSize))
	{
		PlayerActivityData &tRoleActivityData = tFullInfo.tActivityInfo;
		// 铜雀台
		const PB::TQTData &tPBTQTData = PBRoleActivityInfo.tqt_data();
		TQTData &tTQTData = tRoleActivityData.tTQTData;
		tTQTData.dwLastGetTime = tPBTQTData.last_get_time();
		tTQTData.wTakeRecordNum = 0;
		for (int i = 0; i < tPBTQTData.take_record_list_size(); ++i)
		{
			tTQTData.arTakeRecordList[tTQTData.wTakeRecordNum++] = tPBTQTData.take_record_list(i);
		}

		// 迎财神
		const PB::YCSData &tPBYCSData = PBRoleActivityInfo.ycs_data();
		tRoleActivityData.tYCSData.byTodayGetTimes = tPBYCSData.today_get_times();
		tRoleActivityData.tYCSData.byTotalGetTimes = tPBYCSData.total_get_times();
		tRoleActivityData.tYCSData.nTotalRewardValue = tPBYCSData.total_reward_value();
		tRoleActivityData.tYCSData.dwLastGetTime = tPBYCSData.last_get_time();
		// 7天活动数据
		for (int i = 0; i < PBRoleActivityInfo.seven_day_target_list_size(); ++i)
		{
			const ::PB::SevenDayTargetData &tPBTargetData = PBRoleActivityInfo.seven_day_target_list(i);
			SevenDayTargetData &tTargetData = tRoleActivityData.arSevenDayTargetList[tRoleActivityData.wSevenDayTargetNum++];
			tTargetData.wId = tPBTargetData.target_id();
			tTargetData.dwValue = tPBTargetData.value();
			tTargetData.byIsGetReward = tPBTargetData.is_get_reward();
		}
		tRoleActivityData.wCompletionRewardRecordNum = 0;
		for (int i = 0; i < PBRoleActivityInfo.completion_reward_record_list_size(); ++i)
		{
			tRoleActivityData.arCompletionRewardRecord[i] = PBRoleActivityInfo.completion_reward_record_list(i);
			tRoleActivityData.wCompletionRewardRecordNum++;
		}
		// 等级商店活动数据
		tRoleActivityData.wLevelShopActivityDataNum = 0;
		for (int i = 0; i < PBRoleActivityInfo.level_shop_activity_data_list_size(); ++i)
		{
			const ::PB::LevelShopActivityData &tLevelShopActivityData = PBRoleActivityInfo.level_shop_activity_data_list(i);
			tRoleActivityData.arLevelShopActivityDataList[i].wLevel = tLevelShopActivityData.level();
			tRoleActivityData.arLevelShopActivityDataList[i].dwActivateTime = tLevelShopActivityData.activate_time();
			tRoleActivityData.wLevelShopActivityDataNum++;
		}
		// 等级礼包领取记录
		tRoleActivityData.wLevelGiftTakeRecordNum = 0;
		for (int i = 0; i < PBRoleActivityInfo.level_gift_take_record_list_size(); ++i)
		{
			tRoleActivityData.arLevelGiftTakeRecord[i] = PBRoleActivityInfo.level_gift_take_record_list(i);
			tRoleActivityData.wLevelGiftTakeRecordNum++;
		}
		// VIP礼包数据
		const PB::VipGiftData &tPBVipGiftData = PBRoleActivityInfo.vip_gift_data();
		VipGiftData &tVipGiftData = tRoleActivityData.tVipGiftData;
		tVipGiftData.wInitVipLevel = tPBVipGiftData.init_vip_level();
		tVipGiftData.nVipLevelRewardRecord = tPBVipGiftData.vip_level_reward_record();
		tVipGiftData.wTodayRecommendRechargeRewardId = tPBVipGiftData.today_recommend_recharge_reward_id();
		tVipGiftData.wTodayRecommendRechargeRewardStatus = tPBVipGiftData.today_recommend_recharge_reward_status();
		tVipGiftData.wWeekGiftNum = 0;
		for (int i = 0; i < tPBVipGiftData.week_gift_list_size(); ++i)
		{
			const ::PB::Gift &tPBGift = tPBVipGiftData.week_gift_list(i);
			Gift &tGift = tVipGiftData.arWeekGiftList[tVipGiftData.wWeekGiftNum++];
			tGift.wId = tPBGift.id();
			tGift.wNum = tPBGift.num();
		}
		// 充值&月卡数据
		const PB::RoleRechargeData &tPBRecharge = PBRoleActivityInfo.recharge_data();
		RoleRechargeData &tRechargeData = tRoleActivityData.tRechargeData;
		tRechargeData.wTodayRechargetTimes = tPBRecharge.today_recharge_times();
		tRechargeData.wRechargeNum = 0;
		for (int i = 0; i < tPBRecharge.recharge_list_size(); ++i)
		{
			const ::PB::CommonRecharge &pPBRecharge = tPBRecharge.recharge_list(i);
			CommonRecharge &tRecharge = tRechargeData.arRechargeList[tRechargeData.wRechargeNum++];
			tRecharge.wRechargeId = pPBRecharge.id();
			tRecharge.wRechargeTimes = pPBRecharge.times();
		}
		tRechargeData.wMontchCardNum = 0;
		for (int i = 0; i < tPBRecharge.month_card_list_size(); ++i)
		{
			const ::PB::MonthCardData &tPBMonthCard = tPBRecharge.month_card_list(i);
			MonthCard &tMonthCard = tRechargeData.arMonthCardList[tRechargeData.wMontchCardNum++];
			tMonthCard.wCardId = tPBMonthCard.card_id();
			tMonthCard.dwBuyTime = tPBMonthCard.buy_time();
			tMonthCard.dwLastTakeRewardTime = tPBMonthCard.last_take_reward_time();
		}
		tRechargeData.wCPOrderRecordNum = 0;
		for (int i = 0; i < tPBRecharge.order_record_list_size(); ++i)
		{
			tRechargeData.arCPOrderRecordList[tRechargeData.wCPOrderRecordNum++] = tPBRecharge.order_record_list(i);
		}
		// 开服基金
		const PB::OpenServerFundData &tPBFundData = PBRoleActivityInfo.open_server_fund_data();
		OpenServerFundData &tFundData = tRoleActivityData.tOpenServerFundData;
		tFundData.wIsAlreadyBuyFund = tPBFundData.is_already_buy_fund();
		tFundData.wFundRewardTakeRecordNum = 0;
		for (int i = 0; i < tPBFundData.fund_reward_take_record_size(); ++i)
		{
			tFundData.arFundRewardTakeRecord[tFundData.wFundRewardTakeRecordNum++] = tPBFundData.fund_reward_take_record(i);
		}
		// 全民福利
		const PB::AllPeopleWelfareData &tPBWelfareData = PBRoleActivityInfo.all_people_welfare_data();
		AllPeopleWelfareData &tWelfareData = tRoleActivityData.tAllPoepleWelfareData;
		tWelfareData.wTakeWelfareRecordNum = 0;
		for (int i = 0; i < tPBWelfareData.welfare_take_record_size(); ++i)
		{
			tWelfareData.arTakeWelfareRecordList[tWelfareData.wTakeWelfareRecordNum++] = tPBWelfareData.welfare_take_record(i);
		}
		// 限时活动
		const PB::LimitActivityData &tPBLimitActivityData = PBRoleActivityInfo.limit_activity_data();
		LimitActivityData &tLimitActivityData = tRoleActivityData.tLimitActivityData;
		tLimitActivityData.dwTotleRecharge = tPBLimitActivityData.total_recharge();
		tLimitActivityData.wFirstRechargeRewardRecordNum = 0;
		for (int i = 0; i < tPBLimitActivityData.first_recharge_reward_record_size(); ++i)
		{
			tLimitActivityData.arFirstRechargeRewardRecord[i] = tPBLimitActivityData.first_recharge_reward_record(i);
			tLimitActivityData.wFirstRechargeRewardRecordNum++;
		}
		// 次日奖励数据
		tRoleActivityData.wIsTakeNextDayReward = PBRoleActivityInfo.is_take_next_day_reward();
		// 超值折扣
		{
			const PB::SuperDiscountData &tPBSuperDiscount = PBRoleActivityInfo.super_discount_data();
			SuperDiscountData &tSuperDiscount = tRoleActivityData.tSuperDiscountData;
			tSuperDiscount.wStatus = tPBSuperDiscount.status();
			tSuperDiscount.wTakeAccumulateRewardTimes = tPBSuperDiscount.take_accumulate_reward_times();
			tSuperDiscount.wCurBuyTimes = tPBSuperDiscount.cur_buy_times();
			tSuperDiscount.wLastActivityDataId = tPBSuperDiscount.last_activity_data_id();
			tSuperDiscount.wTookAccumulateRewardNum = 0;
			for (int i = 0; i < tPBSuperDiscount.took_accumulate_reward_list_size(); ++i){
				tSuperDiscount.arTookAccumulateRewardList[tSuperDiscount.wTookAccumulateRewardNum++] = tPBSuperDiscount.took_accumulate_reward_list(i);
			}
		}
		// 新超值折扣
		{
			const PB::SuperDiscountData &tPBNewSuperDiscount = PBRoleActivityInfo.new_super_discount_data();
			SuperDiscountData &tNewSuperDiscount = tRoleActivityData.tNewSuperDiscountData;
			tNewSuperDiscount.wStatus = tPBNewSuperDiscount.status();
			tNewSuperDiscount.wTakeAccumulateRewardTimes = tPBNewSuperDiscount.take_accumulate_reward_times();
			tNewSuperDiscount.wCurBuyTimes = tPBNewSuperDiscount.cur_buy_times();
			tNewSuperDiscount.wLastActivityDataId = tPBNewSuperDiscount.last_activity_data_id();
		}
		// 节目活动数据
		PlayerFestivalActivityInfo &tRoleFestivalActivityInfo = tRoleActivityData.tFestivalActivityInfo;
		const PB::RoleFestivalActivityInfo &tPBRoleFestivalActivityInfo = PBRoleActivityInfo.role_festival_activity_info();
		// 循环单充
		{
			FestivalActivity &tLoopOnceRecharge = tRoleFestivalActivityInfo.tLoopOnceRecharge;
			const ::PB::FestivalActivity &tPBLoopOnceRecharge = tPBRoleFestivalActivityInfo.loop_once_recharge();
			tLoopOnceRecharge.dwDataResetTime = tPBLoopOnceRecharge.data_reset_time();
			tLoopOnceRecharge.wTargetNum = 0;
			for (int i = 0; i < tPBLoopOnceRecharge.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tLoopOnceRecharge.arTargetList[tLoopOnceRecharge.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBLoopOnceRecharge.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 循环累充
		{
			FestivalActivity &tLoopAccumulateRecharge = tRoleFestivalActivityInfo.tLoopAccumulateRecharge;
			const ::PB::FestivalActivity &tPBLoopAccumulateRecharge = tPBRoleFestivalActivityInfo.loop_accumulate_recharge();
			tLoopAccumulateRecharge.dwDataResetTime = tPBLoopAccumulateRecharge.data_reset_time();
			tLoopAccumulateRecharge.wTargetNum = 0;
			for (int i = 0; i < tPBLoopAccumulateRecharge.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tLoopAccumulateRecharge.arTargetList[tLoopAccumulateRecharge.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBLoopAccumulateRecharge.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 开服限时召唤
		{
			FestivalActivity &tLimitRecruit = tRoleFestivalActivityInfo.tLimitRecruit;
			const ::PB::FestivalActivity &tPBLimitRecruit = tPBRoleFestivalActivityInfo.limit_recruit();
			tLimitRecruit.dwDataResetTime = tPBLimitRecruit.data_reset_time();
			tLimitRecruit.wTargetNum = 0;
			for (int i = 0; i < tPBLimitRecruit.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tLimitRecruit.arTargetList[tLimitRecruit.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBLimitRecruit.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 活动单充
		{
			FestivalActivity &tActivityOnceRecharge = tRoleFestivalActivityInfo.tActivityOnceRecharge;
			const ::PB::FestivalActivity &tPBActivityOnceRecharge = tPBRoleFestivalActivityInfo.activity_once_recharge();
			tActivityOnceRecharge.dwDataResetTime = tPBActivityOnceRecharge.data_reset_time();
			tActivityOnceRecharge.wTargetNum = 0;
			for (int i = 0; i < tPBActivityOnceRecharge.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tActivityOnceRecharge.arTargetList[tActivityOnceRecharge.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBActivityOnceRecharge.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 活动单充2
		{
			FestivalActivity &tActivityOnceRecharge2 = tRoleFestivalActivityInfo.tActivityOnceRecharge2;
			const ::PB::FestivalActivity &tPBActivityOnceRecharge2 = tPBRoleFestivalActivityInfo.activity_once_recharge_2();
			tActivityOnceRecharge2.dwDataResetTime = tPBActivityOnceRecharge2.data_reset_time();
			tActivityOnceRecharge2.wTargetNum = 0;
			for (int i = 0; i < tPBActivityOnceRecharge2.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tActivityOnceRecharge2.arTargetList[tActivityOnceRecharge2.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBActivityOnceRecharge2.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 活动累充
		{
			FestivalActivity &tActivityAccumulateRecharge = tRoleFestivalActivityInfo.tActivityAccumulateRecharge;
			const ::PB::FestivalActivity &tPBActivityAccumulateRecharge = tPBRoleFestivalActivityInfo.activity_accumulate_recharge();
			tActivityAccumulateRecharge.dwDataResetTime = tPBActivityAccumulateRecharge.data_reset_time();
			tActivityAccumulateRecharge.wTargetNum = 0;
			for (int i = 0; i < tPBActivityAccumulateRecharge.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tActivityAccumulateRecharge.arTargetList[tActivityAccumulateRecharge.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBActivityAccumulateRecharge.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 打折兑换（物品打折卖钻石,钻石换物品）
		{
			FestivalActivity &tDiamondExchange = tRoleFestivalActivityInfo.tDiamondExchange;
			const ::PB::FestivalActivity &tPBDiamondExchange = tPBRoleFestivalActivityInfo.diamond_exchange();
			tDiamondExchange.dwDataResetTime = tPBDiamondExchange.data_reset_time();
			tDiamondExchange.wTargetNum = 0;
			for (int i = 0; i < tPBDiamondExchange.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tDiamondExchange.arTargetList[tDiamondExchange.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBDiamondExchange.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 活跃目标
		{
			FestivalActivity &tActiveTarget = tRoleFestivalActivityInfo.tActiveTarget;
			const ::PB::FestivalActivity &tPBActiveTarget = tPBRoleFestivalActivityInfo.active_target();
			tActiveTarget.dwDataResetTime = tPBActiveTarget.data_reset_time();
			tActiveTarget.wTargetNum = 0;
			for (int i = 0; i < tPBActiveTarget.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tActiveTarget.arTargetList[tActiveTarget.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBActiveTarget.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 材料兑换（物品换物品）
		{
			FestivalActivity &tItemExchange = tRoleFestivalActivityInfo.tItemExchange;
			const ::PB::FestivalActivity &tPBItemExchange = tPBRoleFestivalActivityInfo.item_exchange();
			tItemExchange.dwDataResetTime = tPBItemExchange.data_reset_time();
			tItemExchange.wTargetNum = 0;
			for (int i = 0; i < tPBItemExchange.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tItemExchange.arTargetList[tItemExchange.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBItemExchange.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 节日登陆
		{
			FestivalActivity &tFestivalLogin = tRoleFestivalActivityInfo.tFestivalLogin;
			const ::PB::FestivalActivity &tPBFestivalLogin = tPBRoleFestivalActivityInfo.festival_login();
			tFestivalLogin.dwDataResetTime = tPBFestivalLogin.data_reset_time();
			tFestivalLogin.wTargetNum = 0;
			for (int i = 0; i < tPBFestivalLogin.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tFestivalLogin.arTargetList[tFestivalLogin.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBFestivalLogin.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 节目活动数据
		PlayerAccurateRecommendData &tRoleAccurateRecommendData = tRoleActivityData.tAccurateRecommendData;
		const PB::RoleAccurateRecommendData &tPBRoleAccurateRecommendData = PBRoleActivityInfo.role_accurate_recommend_data();
		tRoleAccurateRecommendData.wGoodsNum = 0;
		for (int i = 0; i < tPBRoleAccurateRecommendData.goods_list_size(); ++i)
		{
			const ::PB::AccurateRecommendGoods &tPBGoods = tPBRoleAccurateRecommendData.goods_list(i);
			AccurateRecommendGoods &tGoods = tRoleAccurateRecommendData.arGoodsList[tRoleAccurateRecommendData.wGoodsNum++];
			tGoods.wGoodsId = tPBGoods.goods_id();
			tGoods.wBuyTimes = tPBGoods.buy_times();
			tGoods.wCanTakeTimes = tPBGoods.can_take_times();
		}

		// 开服登陆
		{
			FestivalActivity &tOpenServerLogin = tRoleFestivalActivityInfo.tOpenServerLogin;
			const ::PB::FestivalActivity &tPBOpenServerLogin = tPBRoleFestivalActivityInfo.open_server_login();
			tOpenServerLogin.dwDataResetTime = tPBOpenServerLogin.data_reset_time();
			tOpenServerLogin.wTargetNum = 0;
			for (int i = 0; i < tPBOpenServerLogin.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tOpenServerLogin.arTargetList[tOpenServerLogin.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBOpenServerLogin.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 开服累充
		{
			FestivalActivity &tOpenSvrAccumulateRecharge = tRoleFestivalActivityInfo.tOpenSvrAccumulateRecharge;
			const ::PB::FestivalActivity &tPBOpenSvrAccumulateRecharge = tPBRoleFestivalActivityInfo.open_svr_accumulate_recharge();
			tOpenSvrAccumulateRecharge.dwDataResetTime = tPBOpenSvrAccumulateRecharge.data_reset_time();
			tOpenSvrAccumulateRecharge.wTargetNum = 0;
			for (int i = 0; i < tPBOpenSvrAccumulateRecharge.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tOpenSvrAccumulateRecharge.arTargetList[tOpenSvrAccumulateRecharge.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBOpenSvrAccumulateRecharge.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 材料兑换（物品换物品）
		{
			FestivalActivity &tFestivalItemExchange = tRoleFestivalActivityInfo.tFestivalItemExchange;
			const ::PB::FestivalActivity &tPBFestivalItemExchange = tPBRoleFestivalActivityInfo.festival_item_exchange();
			tFestivalItemExchange.dwDataResetTime = tPBFestivalItemExchange.data_reset_time();
			tFestivalItemExchange.wTargetNum = 0;
			for (int i = 0; i < tPBFestivalItemExchange.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tFestivalItemExchange.arTargetList[tFestivalItemExchange.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBFestivalItemExchange.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 节日打折贩售
		{
			FestivalActivity &tFestivalDiscountSell = tRoleFestivalActivityInfo.tFestivalDiscountSell;
			const ::PB::FestivalActivity &tPBFestivalDiscountSell = tPBRoleFestivalActivityInfo.festival_discount_sell();
			tFestivalDiscountSell.dwDataResetTime = tPBFestivalDiscountSell.data_reset_time();
			tFestivalDiscountSell.wTargetNum = 0;
			for (int i = 0; i < tPBFestivalDiscountSell.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tFestivalDiscountSell.arTargetList[tFestivalDiscountSell.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBFestivalDiscountSell.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 节日活跃目标
		{
			FestivalActivity &tFestivalActiveTarget = tRoleFestivalActivityInfo.tFestivalActiveTarget;
			const ::PB::FestivalActivity &tPBFestivalActiveTarget = tPBRoleFestivalActivityInfo.festival_active_target();
			tFestivalActiveTarget.dwDataResetTime = tPBFestivalActiveTarget.data_reset_time();
			tFestivalActiveTarget.wTargetNum = 0;
			for (int i = 0; i < tPBFestivalActiveTarget.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tFestivalActiveTarget.arTargetList[tFestivalActiveTarget.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBFestivalActiveTarget.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 开服活跃2(同7)
		{
			FestivalActivity &tOpenServerActiveTarget = tRoleFestivalActivityInfo.tOpenServerActiveTarget;
			const ::PB::FestivalActivity &tPBOpenServerActiveTarget = tPBRoleFestivalActivityInfo.open_server_active_target();
			tOpenServerActiveTarget.dwDataResetTime = tPBOpenServerActiveTarget.data_reset_time();
			tOpenServerActiveTarget.wTargetNum = 0;
			for (int i = 0; i < tPBOpenServerActiveTarget.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tOpenServerActiveTarget.arTargetList[tOpenServerActiveTarget.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBOpenServerActiveTarget.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}
		// 节日活跃2(同14)
		{
			FestivalActivity &tFestivalActiveTarget2 = tRoleFestivalActivityInfo.tFestivalActiveTarget2;
			const ::PB::FestivalActivity &tPBFestivalActiveTarget2 = tPBRoleFestivalActivityInfo.festival_active_target_2();
			tFestivalActiveTarget2.dwDataResetTime = tPBFestivalActiveTarget2.data_reset_time();
			tFestivalActiveTarget2.wTargetNum = 0;
			for (int i = 0; i < tPBFestivalActiveTarget2.target_list_size(); ++i)
			{
				ActivityTarget &tTarget = tFestivalActiveTarget2.arTargetList[tFestivalActiveTarget2.wTargetNum++];
				const ::PB::ActivityTarget &tPBTarget = tPBFestivalActiveTarget2.target_list(i);
				tTarget.wId = tPBTarget.target_id();
				tTarget.dwContext = tPBTarget.context();
				tTarget.wTakeRewardTimes = tPBTarget.take_reward_times();
			}
		}

		// 圣诞节数据
		{
			PlayerChristmasData &tChristmasData = tRoleActivityData.tChristmasData;
			const ::PB::RoleChristmasData &tPBChristmasData = PBRoleActivityInfo.role_christmas_data();
			tChristmasData.dwTotalScore = tPBChristmasData.total_score();
			tChristmasData.dwCurScore = tPBChristmasData.cur_score();
			tChristmasData.wAlreadyTakeLowerBossRewardLevel = tPBChristmasData.already_take_lower_boss_reward_level();
			tChristmasData.wAlreadyTakeHighBossRewardLevel = tPBChristmasData.already_take_high_boss_reward_level();
			tChristmasData.dwLastResetDataTime = tPBChristmasData.last_reset_data_time();
		}

		// facebook活动数据
		{
			FacebookActivityData &tFacebookActivityData = tRoleActivityData.tFacebookActivityData;
			const ::PB::FacebookActivityData &tPBFacebookActivityData = PBRoleActivityInfo.facebook_activity_data();
			tFacebookActivityData.wIsAlreadyTakeAccountBindingReward = tPBFacebookActivityData.is_already_take_account_bingding_reward();
			tFacebookActivityData.dwReadyToShareTime = tPBFacebookActivityData.ready_to_share_time();
			tFacebookActivityData.dwReadyToCommendTime = tPBFacebookActivityData.ready_to_commend_time();
			tFacebookActivityData.wSummonSuccessFriendNum = tPBFacebookActivityData.recall_success_friend_num();
			tFacebookActivityData.wInviteSuccessFriendNum = tPBFacebookActivityData.invite_success_friend_num();
			// 召回奖励记录(已领取的)
			tFacebookActivityData.wSummonRewardRecordNum = 0;
			for (int i = 0; i < tPBFacebookActivityData.recall_reward_record_list_size(); ++i)
			{
				tFacebookActivityData.arSummonRewardRecord[i] = tPBFacebookActivityData.recall_reward_record_list(i);
				tFacebookActivityData.wSummonRewardRecordNum++;
			}
			// 邀请奖励记录(已领取的)
			tFacebookActivityData.wInviteRewardRecordNum = 0;
			for (int i = 0; i < tPBFacebookActivityData.invite_reward_record_list_size(); ++i)
			{
				tFacebookActivityData.arInviteRewardRecord[i] = tPBFacebookActivityData.invite_reward_record_list(i);
				tFacebookActivityData.wInviteRewardRecordNum++;
			}
			// 已发出召回的好友记录(游戏内好友)
			tFacebookActivityData.wSummonFriendRecordNum = 0;
			for (int i = 0; i < tPBFacebookActivityData.send_recall_friend_record_size(); ++i)
			{
				const ::PB::RecallFriendData &tPBRecallFriendData = tPBFacebookActivityData.send_recall_friend_record(i);
				RecallFriendData &tRecallFriendData = tFacebookActivityData.arSummonFriendRecord[tFacebookActivityData.wSummonFriendRecordNum++];
				tRecallFriendData.dwFriendId = tPBRecallFriendData.friend_id();
				tRecallFriendData.dwTime = tPBRecallFriendData.send_time();
			}
			// 已发出邀请的好友记录(facebook好友)
			FacebookFriendInfo &tFacebookFiendInfo = tFacebookActivityData.tSendInviteFriendInfo;
			tFacebookFiendInfo.wFriendNum = 0;
			for (int i = 0; i < tPBFacebookActivityData.send_invite_friend_record_size(); ++i)
			{
				const ::std::string &strFiendName = tPBFacebookActivityData.send_invite_friend_record(i);
				strcpy_s(tFacebookFiendInfo.arFriendList[i], sizeof(tFacebookFiendInfo.arFriendList[i]), strFiendName.c_str());
				tFacebookFiendInfo.wFriendNum++;
			}
		}

		// 在线奖励数据
		{
			OnlineRewardData &tOnlineRewardData = tRoleActivityData.tOnlineRewardData;
			const ::PB::OnlineRewardData &tPBOnlineRewardData = PBRoleActivityInfo.online_reward_data();
			tOnlineRewardData.dwOnlineTime = tPBOnlineRewardData.online_time();
			for (int i = 0; i < tPBOnlineRewardData.reward_record_list_size(); ++i)
			{
				const ::PB::OnlineRewardRecord &tRewardRecordList = tPBOnlineRewardData.reward_record_list(i);
				WORD wDays = tRewardRecordList.days();
				WORD *pRewardRecordList = tOnlineRewardData.arRewardRewardList[wDays-1];
				for (int k = 0; k < tRewardRecordList.flag_list_size(); k++)
				{
					pRewardRecordList[k] = tRewardRecordList.flag_list(k);
				}
			}
		}

		// 7天登录数据
		{
			SevenDayLoginData &tSevenDayLoginData = tRoleActivityData.tSevenDayLoginData;
			const ::PB::SevenDayLoginData &tPBSevenDayLoginData = PBRoleActivityInfo.seven_day_login_data();
			for (int i = 0; i < tPBSevenDayLoginData.daily_data_list_size(); ++i)
			{
				tSevenDayLoginData.arDailyDataList[i] = tPBSevenDayLoginData.daily_data_list(i);
			}
		}

		// 充值回馈
		{
			RecahrgeRewardData &tRechargeRewardData = tRoleActivityData.tRechargeRewardData;
			const ::PB::RechargeRewardData &tPBRechargeRewardData = PBRoleActivityInfo.recharge_reward_data();
			tRechargeRewardData.wIsTookExtraReward = tPBRechargeRewardData.is_took_extra_reward();
			tRechargeRewardData.wLastActivityDataId = tPBRechargeRewardData.last_activity_data_id();
			for (int i = 0; i < tPBRechargeRewardData.daily_data_list_size(); ++i)
			{
				tRechargeRewardData.arDailyDataList[i] = tPBRechargeRewardData.daily_data_list(i);
			}
			
		}

		// 累充奖励
		{
			TotalRechargeRewardData &tTotalRechargeRewardData = tRoleActivityData.tTotalRechargeRewardData;
			const ::PB::TotalRechargeRewardData &tPBTotalRechargeRewardData = PBRoleActivityInfo.total_recharge_reward_data();
			tTotalRechargeRewardData.dwTotalAmount = tPBTotalRechargeRewardData.total_amount();
			tTotalRechargeRewardData.wLastActivityDataId = tPBTotalRechargeRewardData.last_activity_data_id();
			tTotalRechargeRewardData.wRewardRecordNum = tPBTotalRechargeRewardData.reward_record_list_size();
			for (int i = 0; i < tPBTotalRechargeRewardData.reward_record_list_size(); ++i)
			{
				tTotalRechargeRewardData.arRewardRecordList[i] = tPBTotalRechargeRewardData.reward_record_list(i);
			}
		}

		// 热卖商品
		{
			PlayerHotSaleGoodsInfo &tPlayerHotSaleGoodsInfo = tRoleActivityData.tPlayerHotSaleGoodsInfo;
			const ::PB::RoleHotSaleGoodsData &tPBPlayerHotSaleGoodsInfo = PBRoleActivityInfo.role_hot_sale_goods();
			tPlayerHotSaleGoodsInfo.wLastActivityDataId = tPBPlayerHotSaleGoodsInfo.last_activity_data_id();
			tPlayerHotSaleGoodsInfo.wGoodsNum = tPBPlayerHotSaleGoodsInfo.goods_list_size();
			for (int i = 0; i < tPBPlayerHotSaleGoodsInfo.goods_list_size(); ++i)
			{
				const ::PB::HotSaleGoods tPBGoods = tPBPlayerHotSaleGoodsInfo.goods_list(i);
				HotSaleGoods &tGoods = tPlayerHotSaleGoodsInfo.arGoodsList[i];
				tGoods.wGoodsId = tPBGoods.goods_id();
				tGoods.wCurDataIdx = tPBGoods.cur_data_idx();
				tGoods.wBuyTimes = tPBGoods.buy_times();
				tGoods.wTakeTimes = tPBGoods.take_times();
			}
		}

		// 阵营招募
		{
			PlayerCampRecruitData &tPlayerCampRecruitData = tRoleActivityData.tPlayerCampRecruitData;
			const ::PB::RoleCampRecruitData &tPBRoleCampRecruitData = PBRoleActivityInfo.role_camp_recruit_data();
			tPlayerCampRecruitData.wCurDataId = tPBRoleCampRecruitData.cur_data_id();
			tPlayerCampRecruitData.wTodayAlreadyRecruitTimes = tPBRoleCampRecruitData.today_already_recruit_times();
			for (int i = 0; i < tPBRoleCampRecruitData.camp_score_list_size() && i < CAMP_NUM; ++i)
			{
				tPlayerCampRecruitData.arCampScoreList[i] = tPBRoleCampRecruitData.camp_score_list(i);
			}
			tPlayerCampRecruitData.wScoreTaskRewardRecordNum = 0;
			for (int i = 0; i < tPBRoleCampRecruitData.score_task_reward_record_list_size() && i < MAX_CAMP_RECRUIT_SCORE_TASK_NUM; ++i)
			{
				tPlayerCampRecruitData.arScoreTaskRewardRecordList[i] = tPBRoleCampRecruitData.score_task_reward_record_list(i);
				tPlayerCampRecruitData.wScoreTaskRewardRecordNum++;
			}
		}
		// unlock活动
		{
			UnlockActivityData &tRoleUnlockActivityData = tRoleActivityData.tUnlockActivityData;
			const ::PB::UnlockActivityData &tPBRoleUnlockActivityData = PBRoleActivityInfo.role_unlock_activity_data();
			tRoleUnlockActivityData.wIsScoreForShop = tPBRoleUnlockActivityData.is_score_for_shop();
			tRoleUnlockActivityData.wWatchingVideosTimes = tPBRoleUnlockActivityData.watching_video_times();
		}
		// 充值与消费排行榜活动
		{
			RechargeAndCostDiamondRankData &tRechargeAndCostDiamondRankData = tRoleActivityData.tRechargeAndCostDiamondRankData;
			const ::PB::RechargeAndCostRankData &tPBRechargeAndCostDiamondRankData = PBRoleActivityInfo.role_recharge_and_cost_rank_data();
			tRechargeAndCostDiamondRankData.wLastActivityDataId = tPBRechargeAndCostDiamondRankData.last_activity_data_id();
			tRechargeAndCostDiamondRankData.dwCostDiamond = tPBRechargeAndCostDiamondRankData.cost_diamond();
			tRechargeAndCostDiamondRankData.dwRechargeAmount = tPBRechargeAndCostDiamondRankData.recharge_amount();
		}
	}
	else if (nRewardInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]解析角色奖励数据失败!", __FUNCTION__);
	}

	DWORD dwCostTime = GetTickCount64() - lCurTick;
	if (dwCostTime)
		MAPLOG(GUI_LEVEL, "[%s]DoSelectRoleInfoQuery消耗[CostTick:%d]", __FUNCTION__, dwCostTime);

	//LEAVE_DUMP_FUNCTION
}


//处理更新角色游戏信息
void DBThreadHandler::DoUpdateRoleInfoQuery(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	//ENTER_DUMP_FUNCTION

	if(!pSqlDatabase || !pData)
		return;

	UpdateRoleInfoQuery* pQuery = (UpdateRoleInfoQuery*)pData;
 
 	//1 更新角色基本信息
 	if(!pSqlDatabase->GetQuery())
 		return;

	RoleFullInfo &tFullInfo = pQuery->GetRoleFullInfo();
	RoleBaseInfo &tRoleInfo = tFullInfo.tRoleBaseInfo;
	RoleSecondInfo &tSecondInfo = tFullInfo.tSecondInfo;
	DWORD dwRoleId = pQuery->GetCharID();
	DWORD dwServerId = pQuery->GetServerD();

	UINT64 lCurTickCount = GetTickCount64();

	// 角色二级信息
	PB::RoleSecondInfo tPBSecondInfo;

	// todo::处理存储之前的数据
	DWORD dwRoleIdBefore = tFullInfo.tRoleBaseInfo.dwRoleId;
	WORD wRoleLevelBefore = tFullInfo.tRoleBaseInfo.wLevel;
	char szRoleName[33] = { 0 };
	char szAccount[65] = { 0 };
	strcpy_s(szRoleName, sizeof(szRoleName),tRoleInfo.szRoleName);
	strcpy_s(szAccount, sizeof(szAccount), tRoleInfo.szAccount);

	// 基本信息
	PB::RoleBaseInfo PBRoleBaseInfo;
	PBRoleBaseInfo.set_account(tRoleInfo.szAccount);
	PBRoleBaseInfo.set_role_name(tRoleInfo.szRoleName);
	//PBRoleBaseInfo.set_role_id(tRoleInfo.dwRoleId);
	PBRoleBaseInfo.set_stamina(tRoleInfo.dwStamina);
	PBRoleBaseInfo.set_energy(tRoleInfo.dwEnergy);
	PBRoleBaseInfo.set_sex(tRoleInfo.wSex);
	PBRoleBaseInfo.set_level(tRoleInfo.wLevel);
	PBRoleBaseInfo.set_daily_init_level(tRoleInfo.wDailyInitLevel);
	PBRoleBaseInfo.set_exp(tRoleInfo.dwExp);
	PBRoleBaseInfo.set_gold(tRoleInfo.dwGold);
	PBRoleBaseInfo.set_diamond(tRoleInfo.dwDiamond);
	PBRoleBaseInfo.set_head_image_id(tRoleInfo.dwHeadImage);
	PBRoleBaseInfo.set_vip_level(tRoleInfo.sVipData.wLevel);
	PBRoleBaseInfo.set_vip_exp(tRoleInfo.sVipData.dwExp);
	PBRoleBaseInfo.set_riot_suppress_times(tRoleInfo.wRiotSuppressTimes);
	PBRoleBaseInfo.set_hero_soul(tRoleInfo.nHeroSoul);
	PBRoleBaseInfo.set_arena_prestige(tRoleInfo.nArenaPrestige);
	PBRoleBaseInfo.set_god_soul(tRoleInfo.nGodSoul);
	PBRoleBaseInfo.set_wanted(tRoleInfo.nWanted);
	PBRoleBaseInfo.set_tower_prestige(tRoleInfo.nTowerPrestige);
	PBRoleBaseInfo.set_fighting_power(tRoleInfo.dwFightingPower);
	PBRoleBaseInfo.set_rebel_value(tRoleInfo.dwRebelValue);
	PBRoleBaseInfo.set_cur_contribution(tRoleInfo.dwCurContribution);
	PBRoleBaseInfo.set_honor(tRoleInfo.nHonor);
	PBRoleBaseInfo.set_world_boss_battle_times(tRoleInfo.wWorldBossBattleTimes);
	PBRoleBaseInfo.set_guild_fb_reward_chapter(tRoleInfo.wGuildFBRewardChapter);
	PBRoleBaseInfo.set_fairy_soul(tRoleInfo.nFairySoul);
	PBRoleBaseInfo.set_last_login_time(tRoleInfo.szLastLoginTime);
	PBRoleBaseInfo.set_last_logout_time(tRoleInfo.szLastLogoutTime);
	PBRoleBaseInfo.set_total_online_time(tRoleInfo.dwOnlineTime);
	PBRoleBaseInfo.set_login_times(tRoleInfo.dwLoginTimes);
	PBRoleBaseInfo.set_last_stamina_recover_time(tRoleInfo.dwLastStaminaRecoverTime);
	PBRoleBaseInfo.set_last_energy_recover_time(tRoleInfo.dwLastEnergyRecoverTime);
	PBRoleBaseInfo.set_last_wanted_recover_time(tRoleInfo.dwLastWantedRecoverTime);
	PBRoleBaseInfo.set_duobao_peace_over_time(tRoleInfo.dwDBPeaceOverTime);
	PBRoleBaseInfo.set_once_flag_list(tRoleInfo.nOnceFlagList);
	PBRoleBaseInfo.set_guide_x(tRoleInfo.nGuideX);
	PBRoleBaseInfo.set_guide_y(tRoleInfo.nGuideY);
	PBRoleBaseInfo.set_leave_guild_time(tRoleInfo.dwLeaveGuildTime);
	PBRoleBaseInfo.set_last_activity_notice_id(tRoleInfo.wLastActivityNoticeId);
	PBRoleBaseInfo.set_friendship_point(tRoleInfo.wFriendshipPoint);
	PBRoleBaseInfo.set_today_friendship_point(tRoleInfo.wTodayFriendshipPoint);
	PBRoleBaseInfo.set_endless_stone(tRoleInfo.nEndlessStore);
	PBRoleBaseInfo.set_nobility_level(tRoleInfo.wNobilityLevel);
	PBRoleBaseInfo.set_update_times(tRoleInfo.dwUpdateTimes);
	PBRoleBaseInfo.set_total_recharge_amount(tRoleInfo.dwTotalRechargeAmount);
	PBRoleBaseInfo.set_gem_coin(tRoleInfo.nGemCoin);
	PBRoleBaseInfo.set_destiny_coin(tRoleInfo.nDestinyCoin);
	PBRoleBaseInfo.set_star_soul(tRoleInfo.nStarSoul);
	PBRoleBaseInfo.set_turntable_score(tRoleInfo.dwTurntableScore);
	PBRoleBaseInfo.set_turntable_activity_data_id(tRoleInfo.wTurntableActivityDataId);
	PBRoleBaseInfo.set_today_used_help_battle_times(tRoleInfo.wTodayUsedHelpBattleTimes);
	PBRoleBaseInfo.set_role_login_days(tRoleInfo.wLoginDays);
	PBRoleBaseInfo.set_crystal(tRoleInfo.nCrystal);
	PBRoleBaseInfo.set_last_login_time_ex(tRoleInfo.dwLastLoginTime);
	PBRoleBaseInfo.set_last_logout_time_ex(tRoleInfo.dwLastLogoutTime);
	PBRoleBaseInfo.set_create_role_time(tRoleInfo.dwRoleCreateTime);
	PBRoleBaseInfo.set_is_already_take_closed_test_rebate(tRoleInfo.wIsAlreadyTakeClosedTestRebate);
	PBRoleBaseInfo.set_score_match_honor(tRoleInfo.nScoreMatchHonor);

	PBRoleBaseInfo.set_resource_war_score(tRoleInfo.nResouceWarScore);
	PBRoleBaseInfo.set_ycoin(tRoleInfo.nYCoin);

	PBRoleBaseInfo.set_today_chat_times(tRoleInfo.wTodayChatTimes);
	
	for (int i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		PB::FormationProperty *pPBFormation = PBRoleBaseInfo.add_formation_pro_list();
		pPBFormation->set_slot(i);
		int *pProList = tRoleInfo.arFormationProList[i];
		for (int k = 0; k < HERO_PRO_MAX; ++k)
		{
			if (pProList[k])
			{
				PB::Property *pPBPro = pPBFormation->add_pro_list();
				pPBPro->set_type(k);
				pPBPro->set_value(pProList[k]);
			}
		}
	}
	for (WORD i = 0; i < tRoleInfo.wGuideNum; ++i)
	{
		PB::GuideData *pPBGuideData = PBRoleBaseInfo.add_guide_list();
		pPBGuideData->set_key(tRoleInfo.arGuideList[i].nKey);
		pPBGuideData->set_value(tRoleInfo.arGuideList[i].nValue);
	}
	for (WORD i = 0; i < tRoleInfo.wActivationTakeRecordNum; ++i)
	{
		PBRoleBaseInfo.add_activation_code_take_record(tRoleInfo.arActivationTakeRecord[i]);
	}

	// 装备信息
	{
		PB::RoleEquipmentInfo *pPBEquipmentInfo = tPBSecondInfo.mutable_role_equipment_info();
		PlayerEquipmentInfo &tEquipmentInfo = tFullInfo.tEquipmentInfo;
		for (WORD i = 0; i < tEquipmentInfo.wEquipmentNum; ++i)
		{
			Equipment &tEquipment = tEquipmentInfo.sEquipmentList[i];
			PB::Equipment *pPBEquipment = pPBEquipmentInfo->add_equipment_list();
			pPBEquipment->set_uid(tEquipment.dwId);
			pPBEquipment->set_info_id(tEquipment.dwInfoId);
			pPBEquipment->set_enhance_level(tEquipment.wEnhanceLevel);
			pPBEquipment->set_refine_level(tEquipment.wRefineLevel);
			pPBEquipment->set_refine_exp(tEquipment.dwRefineExp);
			pPBEquipment->set_enhance_cost_gold(tEquipment.nEnhanceCostGold);
			pPBEquipment->set_star(tEquipment.wStar);
			pPBEquipment->set_star_level(tEquipment.wStarLevel);
			for (WORD k = 0; k < tEquipment.wRefineCostItemNum; ++k)
			{
				PB::Item *pPBItem = pPBEquipment->add_refine_cost_item_list();
				pPBItem->set_item_id(tEquipment.arRefineCostItem[k].dwId);
				pPBItem->set_item_num(tEquipment.arRefineCostItem[k].dwNum);
			}
		}
	}

	// 英雄信息
	PB::RoleHeroInfo tPBHeroInfo; // = tPBSecondInfo.mutable_role_hero_info();
	PlayerHeroInfo &tHeroInfo = tFullInfo.tHeroInfo;
	tPBHeroInfo.set_normal_free_summon_times(tHeroInfo.wNormalFreeSummonTimes);
	tPBHeroInfo.set_normal_free_summon_time(tHeroInfo.dwNormalFreeSummonTime);
	tPBHeroInfo.set_high_free_summon_time(tHeroInfo.dwHighFreeSummonTime);
	tPBHeroInfo.set_high_summon_times(tHeroInfo.wHighSummonTimes);
	tPBHeroInfo.set_train_times(tHeroInfo.wTrainTimes);
	for (WORD i = 0; i < tHeroInfo.wCollectionNum; ++i)
	{
		tPBHeroInfo.add_collection_list(tHeroInfo.arCollectionList[i]);
	}
	for (WORD i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		tPBHeroInfo.add_reinforcement_list(tHeroInfo.arReinforcementList[i]);
	}
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &tFormation = tHeroInfo.arFormationList[i];
		PB::Formation *pPBFormation = tPBHeroInfo.add_formation_list();
		pPBFormation->set_hero_id(tFormation.dwHeroUID);
		pPBFormation->set_bless_sprite_id(tFormation.dwBlessSpriteId);
		for (WORD k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			pPBFormation->add_equipment_list(tFormation.arEquipmentList[k]);
		}
		for (WORD k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			pPBFormation->add_treasure_list(tFormation.arTreasureList[k]);
		}
		for (WORD k = 0; k < MAX_GEM_HOLE_NUM; ++k)
		{
			pPBFormation->add_gem_list(tFormation.arGemList[k]);
		}
		for (WORD k = 0; k < MAX_CARD_SLOT_NUM; ++k)
		{
			pPBFormation->add_card_list(tFormation.arCardList[k]);
		}
	}
	for (WORD i = 0; i < tHeroInfo.wHeroNum; ++i)
	{
		PB::Hero *pPBHero = tPBHeroInfo.add_hero_list();
		HeroInfo &tHero = tHeroInfo.arHeroList[i];
		pPBHero->set_uid(tHero.dwUID);
		pPBHero->set_info_id(tHero.dwIntoId);
		pPBHero->set_level(tHero.wLevel);
		pPBHero->set_exp(tHero.dwExp);
		pPBHero->set_quality(tHero.wQuality);
		pPBHero->set_break_level(tHero.wBreakLevel);
		pPBHero->set_destiny_level(tHero.wDestinyLevel);
		pPBHero->set_destiny_exp(tHero.dwDestinyExp);
		pPBHero->set_awaken_star(tHero.wAwakenStar);
		pPBHero->set_awaken_level(tHero.wAwakenLevel);
		pPBHero->set_quality_stage(tHero.wQualityStage);
		for (WORD k = 0; k < tHero.wAwakenItemNum; ++k)
		{
			pPBHero->add_awaken_item_list(tHero.arAwakenItemList[k]);
		}
		for (WORD k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			PB::HeroTrainData *pTrainData = pPBHero->add_train_data_list();
			pTrainData->set_pro_type(k + 1);
			pTrainData->set_pro_level(tHero.arTrainProLvList[k]);
			pTrainData->set_pro_value(tHero.arTrainProValueList[k]);
			pTrainData->set_tmp_pro_value(tHero.arLastTrainTmpProList[k]);
			pTrainData->set_reward_flag(tHero.arTrainRewardFlag[k]);
		}
		pPBHero->set_train_cost_item_num(tHero.dwTrainCostItemNum);
		pPBHero->set_destiny_cost_item_num(tHero.dwDestinyCostItemNum);
		pPBHero->set_train_cost_diamond(tHero.nTrainCostDiamond);
		pPBHero->set_train_cost_gold(tHero.nTrainCostGold);
	}

	// 宝物信息
	{
		PB::RoleTreasureInfo *pPBTreasureInfo = tPBSecondInfo.mutable_role_treasure_info();
		PlayerTreasureInfo &tTreasureInfo = tFullInfo.tTreasureInfo;
		for (WORD i = 0; i < tTreasureInfo.wTreasureNum; ++i)
		{
			PB::Treasure *pPBTreasure = pPBTreasureInfo->add_treasure_list();
			Treasure &tTreasure = tTreasureInfo.sTreasureList[i];
			pPBTreasure->set_uid(tTreasure.dwId);
			pPBTreasure->set_info_id(tTreasure.dwInfoId);
			pPBTreasure->set_enhance_level(tTreasure.wEnhanceLevel);
			pPBTreasure->set_enhance_exp(tTreasure.dwEnhanceExp);
			pPBTreasure->set_refine_level(tTreasure.wRefineLevel);
		}
	}

	// 物品信息
	{
		PB::RoleItemInfo *pPBItemInfo = tPBSecondInfo.mutable_role_item_info();
		PlayerItemInfo &tItemInfo = tFullInfo.tItemInfo;
		for (WORD i = 0; i < tItemInfo.wListNum; ++i)
		{
			PB::Item *pPBItem = pPBItemInfo->add_item_list();
			pPBItem->set_item_id(tItemInfo.arItemList[i].dwId);
			pPBItem->set_item_num(tItemInfo.arItemList[i].dwNum);
		}
	}

	// 占卜数据
	{
		PB::RoleZhanBuData *pPBRoleZhanbuInfo = tPBSecondInfo.mutable_role_zhanbu_data();
		const PlayerZhanBuInfo &tZhanBuInfo = tFullInfo.tZhanBuInfo;
		for (WORD i = 0; i < tZhanBuInfo.wCardNum; ++i)
		{
			PB::Card *pPBCard = pPBRoleZhanbuInfo->add_card_list();
			const Card &tCard = tZhanBuInfo.arCardList[i];
			pPBCard->set_uid(tCard.dwId);
			pPBCard->set_info_id(tCard.dwInfoId);
			pPBCard->set_level(tCard.wLevel);
			pPBCard->set_exp(tCard.dwExp);
			pPBCard->set_break_level(tCard.wBreakLevel);
		}
		pPBRoleZhanbuInfo->set_history_collection_value(tZhanBuInfo.dwHistoryCollectionValue);	 // 历史收藏值
		pPBRoleZhanbuInfo->set_low_card_group_map_idx(tZhanBuInfo.wLowCardGroupMapIdx);	// 低级卡组关卡索引
		pPBRoleZhanbuInfo->set_cur_low_card_group_id(tZhanBuInfo.wCurLowCardGroupId); // 当前低级卡组ID
		pPBRoleZhanbuInfo->set_low_card_group_last_recover_time(tZhanBuInfo.dwLowCardGroupLastRecoverTime); // 低级卡组最近恢复时间
		pPBRoleZhanbuInfo->set_cur_low_card_group_refresh_times(tZhanBuInfo.wCurLowCardGroupHaveRefreshTimes);	// 当前低级卡组拥有的刷新次数
		pPBRoleZhanbuInfo->set_today_low_card_group_open_card_times(tZhanBuInfo.wTodayLowCardGroupOpenCardTimes);	// 今日低级卡组翻卡次数
		pPBRoleZhanbuInfo->set_today_low_card_group_cost_refresh_times(tZhanBuInfo.wTodayLowCardGroupCostRefreshTimes);
		pPBRoleZhanbuInfo->set_today_high_card_group_cost_refresh_times(tZhanBuInfo.wTodayHighCardGroupCostRefreshTimes);
		pPBRoleZhanbuInfo->set_high_card_group_last_refresh_time(tZhanBuInfo.dwHighCardGroupLastRefreshTime);
		pPBRoleZhanbuInfo->set_achievement_level(tZhanBuInfo.wAchievementLevel);
		for (WORD i = 0; i < tZhanBuInfo.wLowCardGroupNum; ++i)
		{
			PB::CardGroupData *pPBGroupCard = pPBRoleZhanbuInfo->add_low_card_group();
			const CardGroupData &tGroupCard = tZhanBuInfo.arLowCardGroup[i];
			pPBGroupCard->set_card_id(tGroupCard.dwCardId);
			pPBGroupCard->set_is_already_open(tGroupCard.wIsAlreadyOpen);
		}
		for (WORD i = 0; i < tZhanBuInfo.wHighCardGroupNum; ++i)
		{
			PB::CardGroupData *pPBGroupCard = pPBRoleZhanbuInfo->add_high_card_group();
			const CardGroupData &tGroupCard = tZhanBuInfo.arHighCardGroup[i];
			pPBGroupCard->set_card_id(tGroupCard.dwCardId);
			pPBGroupCard->set_is_already_open(tGroupCard.wIsAlreadyOpen);
		}
		for (WORD i = 0; i < tZhanBuInfo.wCollectionBookNum; ++i)
		{
			PB::CollectionBook *pPBCollectionBook = pPBRoleZhanbuInfo->add_collection_book_list();
			const CollectionBook &tCollectionBook = tZhanBuInfo.arCollectionBookList[i];
			pPBCollectionBook->set_book_id(tCollectionBook.wBookId);
			for (WORD k = 0; k < tCollectionBook.wCollectionGroupNum; ++k)
			{
				::PB::CollectionGroup *pPBCollectionGroup = pPBCollectionBook->add_collection_group_list();
				const CollectionGroup &tCollectionGroup = tCollectionBook.arCollectionGroupList[k];
				pPBCollectionGroup->set_id(tCollectionGroup.wId);
				pPBCollectionGroup->set_star(tCollectionGroup.wStar);
			}
		}
	}

	// 邮件信息
	PB::RoleEmailInfo PBEmailInfo;
	PlayerEmailInfo &tEmailInfo = tFullInfo.tEmailInfo;
	PBEmailInfo.set_is_have_new_email(tEmailInfo.wIsHaveNewEmail);
	for (WORD i = 0; i < tEmailInfo.wEmailCount; ++i)
	{
		EmailInfo &tEmail = tEmailInfo.arEmailList[i];
		PB::EmailInfo *pPBEmail = PBEmailInfo.add_email_list();
		pPBEmail->set_uid(tEmail.dwID);
		pPBEmail->set_info_id(tEmail.wInfoId);
		pPBEmail->set_receive_time(tEmail.dwReceiveTime);
		for (WORD k = 0; k < tEmail.wContextNum; ++k)
		{
			EmailContext &tContext = tEmail.arContextList[k];
			PB::EmailContext *pPBContext = pPBEmail->add_context_list();
			pPBContext->set_type(tContext.wType);
			pPBContext->set_text(tContext.szText);
		}
		pPBEmail->set_text(tEmail.szText);
		pPBEmail->set_title(tEmail.szTitle);
	}

	// 副本信息
	{
		PB::RoleFBInfo *pPBFBInfo = tPBSecondInfo.mutable_role_fb_info();
		PlayerFubenInfo &tFBInfo = tFullInfo.tFubenInfo;
		// 主线副本
		PB::MainFBData *pPBMainFBData = pPBFBInfo->mutable_main_fb_data();
		pPBMainFBData->set_take_chapter_reward_record(tFBInfo.tMainFubenData.wTakeChapterRewardRecord);
		for (WORD i = 0; i < tFBInfo.tMainFubenData.wChapter; ++i)
		{
			ChapterData &tChapterData = tFBInfo.tMainFubenData.arChapterList[i];
			PB::ChapterData *pPBChapterData = pPBMainFBData->add_chapter_list();
			for (BYTE k = 0; k < tChapterData.byMapIdx; ++k)
			{
				MapData &tMapData = tChapterData.arMapList[k];
				PB::MapData *pPBMapData = pPBChapterData->add_map_list();
				pPBMapData->set_star(tMapData.byStar);
				pPBMapData->set_battle_times(tMapData.byBattleTimes);
				pPBMapData->set_reset_times(tMapData.byResetTimes);
				pPBMapData->set_is_get_reward(tMapData.byIsGetAward);
			}
			for (BYTE k = 0; k < CHAPTER_STAR_AWARD_MAX; ++k)
			{
				pPBChapterData->add_star_reward_list(tChapterData.arStarAwardList[k]);
			}
		}
		// 英雄副本
		PB::HeroFBData *pPBHeroFBData = pPBFBInfo->mutable_hero_fb_data();
		pPBHeroFBData->set_battle_times(tFBInfo.tHeroFubenData.byBattleTimes);
		for (WORD i = 0; i < tFBInfo.tHeroFubenData.wChapter; ++i)
		{
			HeroFubenChapterData &tChapterData = tFBInfo.tHeroFubenData.arChapterList[i];
			PB::HeroFBChapter *pPBChapterData = pPBHeroFBData->add_chapter_list();
			pPBChapterData->set_is_pass_extra_map(tChapterData.byIsExtraMapPass);
			for (BYTE k = 0; k < tChapterData.byMapIdx; ++k)
			{
				HeroMapData &tMapData = tChapterData.arMapList[k];
				PB::HeroMapData *pPBMapData = pPBChapterData->add_map_list();
				pPBMapData->set_battle_times(tMapData.byBattleTimes);
				pPBMapData->set_is_get_reward(tMapData.byIsGetAward);
			}
		}
		// 日常副本
		PB::DailyFBData *pPBDailyFBData = pPBFBInfo->mutable_daily_fb_data();
		for (WORD i = 0; i < tFBInfo.tDailyFubenData.wChapterNum; ++i)
		{
			DailyChapterData &tChapter = tFBInfo.tDailyFubenData.arChapterList[i];
			::PB::DailyChapterData *pPBChapter = pPBDailyFBData->add_chapter_list();
			pPBChapter->set_chapter(tChapter.wChapter);
			pPBChapter->set_max_idx_record(tChapter.nMapIdxRecord);
			pPBChapter->set_battle_times(tChapter.byBattleTimes);
		}
		// 爬塔副本数据
		PB::TowerFBData *pPBTowerFBData = pPBFBInfo->mutable_tower_fb_data();
		TowerFBData &tTowerFBData = tFBInfo.tTowerFubenData;
		for (WORD i = 0; i < tTowerFBData.wCurMapLevel; ++i)
		{
			pPBTowerFBData->add_map_star_list(tTowerFBData.arMapStarList[i]);
		}
		pPBTowerFBData->set_map_level_record(tTowerFBData.wMapLevelRecord);
		pPBTowerFBData->set_keep_best_star_passed_record(tTowerFBData.byKeepThreeStarPassedRecord);
		for (BYTE i = 0; i < tTowerFBData.bySelectProNum; ++i)
		{
			Property &tPro = tTowerFBData.arSelectProList[i];
			PB::Property *pPBPro = pPBTowerFBData->add_select_pro_list();
			pPBPro->set_type(tPro.wType);
			pPBPro->set_value(tPro.nValue);
		}
		pPBTowerFBData->set_is_buy_star_goods(tTowerFBData.byIsBuyStarGoods);
		pPBTowerFBData->set_star_goods_idx(tTowerFBData.byStarGoodsIndex);
		pPBTowerFBData->set_is_need_reset(tTowerFBData.byIsNeedReset);
		pPBTowerFBData->set_today_reset_times(tTowerFBData.byTodayResetTimes);
		pPBTowerFBData->set_cur_star(tTowerFBData.wCurStar);
		pPBTowerFBData->set_star_max_record(tTowerFBData.wStarMaxRecord);
		pPBTowerFBData->set_elite_map_level(tTowerFBData.wEliteMapLevel);
		pPBTowerFBData->set_elite_map_battle_times(tTowerFBData.byEliteMapBattleTimes);
		pPBTowerFBData->set_buy_elite_map_battle_times(tTowerFBData.byBuyEliteMapBattleTimes);
		for (BYTE i = 0; i < tTowerFBData.byRandProNum; ++i)
		{
			pPBTowerFBData->add_rand_pro_idx_list(tTowerFBData.arRandProIdxList[i]);
		}
		// 精英副本数据
		PB::EliteFBData *pPBEliteFBData = pPBFBInfo->mutable_elite_fb_data();
		pPBEliteFBData->set_last_refresh_enemy_time(tFBInfo.tEliteFBData.dwLastRefreshEnemyTime);
		for (WORD i = 0; i < tFBInfo.tEliteFBData.wChapter; ++i)
		{
			EliteFBChapter &tChapterData = tFBInfo.tEliteFBData.arChapterList[i];
			PB::EliteFBChapter *pPBChapterData = pPBEliteFBData->add_chapter_list();
			for (BYTE k = 0; k < tChapterData.byMapIdx; ++k)
			{
				MapData &tMapData = tChapterData.arMapList[k];
				PB::MapData *pPBMapData = pPBChapterData->add_map_list();
				pPBMapData->set_star(tMapData.byStar);
				pPBMapData->set_battle_times(tMapData.byBattleTimes);
				pPBMapData->set_reset_times(tMapData.byResetTimes);
				pPBMapData->set_is_get_reward(tMapData.byIsGetAward);
			}
			for (BYTE k = 0; k < CHAPTER_STAR_AWARD_MAX; ++k)
			{
				pPBChapterData->add_star_reward_list(tChapterData.arStarAwardList[k]);
			}
			PB::EliteMapEnemyData *pEnemyData = pPBChapterData->mutable_enemy_data();
			pEnemyData->set_map_id(tChapterData.tEnemyData.wMapId);
			pEnemyData->set_is_dead(tChapterData.tEnemyData.wIsDead);
			for (BYTE k = 0; k < tChapterData.tEnemyData.byMonsterNum; ++k)
			{
				PB::MonsterData *pMonsterData = pEnemyData->add_monster_list();
				pMonsterData->set_idx(tChapterData.tEnemyData.arMonsterList[k].byIndex);
				pMonsterData->set_damage(tChapterData.tEnemyData.arMonsterList[k].dwDamage);
				pMonsterData->set_total_hp(tChapterData.tEnemyData.arMonsterList[k].dwTotalHP);
			}
		}
	}

	//// 商店信息
	{
		PB::RoleShopInfo *pPBShopInfo = tPBSecondInfo.mutable_role_shop_info();
		// 英雄商店
		{
			RefreshShopData &tHeroShop = tFullInfo.tShopInfo.tHeroShop;
			PB::HeroShopData  *pPBHeroShop = pPBShopInfo->mutable_hero_shop();
			pPBHeroShop->set_last_recover_time(tHeroShop.dwLastRecoverTime);
			pPBHeroShop->set_free_refresh_times(tHeroShop.wFreeRefreshTimes);
			pPBHeroShop->set_cost_refresh_times(tHeroShop.wCostRefreshTimes);
			for (BYTE i = 0; i < tHeroShop.wGoodsNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tHeroShop.arGoodsList[i];
				PB::ShopGoods *pPBGoods = pPBHeroShop->add_goods_list();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 工会商店
		{
			PlayerGuildShopData &tGuildShop = tFullInfo.tShopInfo.tGuildShop;
			PB::GuildShopData *pPBGuildShop = pPBShopInfo->mutable_guild_shop();
			for (WORD i = 0; i < tGuildShop.wAwardGoodsRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tGuildShop.arAwardGoodsRecord[i];
				PB::ShopGoods *pPBGoods = pPBGuildShop->add_reward_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			for (WORD i = 0; i < tGuildShop.wItemGoodsRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tGuildShop.arItemGoodsRecord[i];
				PB::ShopGoods *pPBGoods = pPBGuildShop->add_item_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			for (WORD i = 0; i < tGuildShop.wLimitGoodsRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tGuildShop.arLimitGoodsRecord[i];
				PB::ShopGoods *pPBGoods = pPBGuildShop->add_limit_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 商城
		{
			PlayerMallData &tMallShop = tFullInfo.tShopInfo.tMallShop;
			PB::MallShopData *pPBMallShop = pPBShopInfo->mutable_mall_shop();
			for (WORD i = 0; i < tMallShop.wItemBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tMallShop.arItemBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBMallShop->add_item_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			for (WORD i = 0; i < tMallShop.wGiftBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tMallShop.arGiftBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBMallShop->add_gift_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 爬塔商店
		{
			PlayerTowerShopData &tTowerShop = tFullInfo.tShopInfo.tTowerShop;
			PB::TowerShopData *pPBTowerShop = pPBShopInfo->mutable_tower_shop();
			for (WORD i = 0; i < tTowerShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tTowerShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBTowerShop->add_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 竞技场商店
		{
			PlayerArenaShopData &tArenaShop = tFullInfo.tShopInfo.tArenaShop;
			PB::ArenaShopData *pPBArenaShop = pPBShopInfo->mutable_arena_shop();
			for (WORD i = 0; i < tArenaShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tArenaShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBArenaShop->add_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 觉醒商店 
		{
			RefreshShopData &tAwakenShop = tFullInfo.tShopInfo.tAwakenShop;
			PB::AwakenShopData *pPBAwakenShop = pPBShopInfo->mutable_awaken_shop();
			for (WORD i = 0; i < tAwakenShop.wGoodsNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tAwakenShop.arGoodsList[i];
				PB::ShopGoods *pPBGoods = pPBAwakenShop->add_goods_list();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			pPBAwakenShop->set_last_recover_time(tAwakenShop.dwLastRecoverTime);
			pPBAwakenShop->set_free_refresh_times(tAwakenShop.wFreeRefreshTimes);
			pPBAwakenShop->set_cost_refresh_times(tAwakenShop.wCostRefreshTimes);
		}
		// 仙灵商店 
		{
			StarSoulShopData &tStarSoulShop = tFullInfo.tShopInfo.tStarSoulShop;
			PB::FairyShopData *pPBFairyShop = pPBShopInfo->mutable_fairy_shop();
			for (WORD i = 0; i < tStarSoulShop.wGoodsNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tStarSoulShop.arGoodsList[i];
				PB::ShopGoods *pPBGoods = pPBFairyShop->add_goods_list();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			pPBFairyShop->set_used_refresh_times(tStarSoulShop.wUsedRefreshTimes);
			pPBFairyShop->set_last_auto_refresh_time(tStarSoulShop.dwLastAutoRefreshTime);
		}
		// 积分赛商店 
		{
			ScoreMatchShopData &tScoreMatchShop = tFullInfo.tShopInfo.tScoreMatchShop;
			PB::ScoreMatchShopData *pPBScoreMatchShop = pPBShopInfo->mutable_score_match_shop();
			for (WORD i = 0; i < tScoreMatchShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tScoreMatchShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBScoreMatchShop->add_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 7天商店 
		{
			SevenDayShopData &tSevenDayShop = tFullInfo.tShopInfo.tSevenShop;
			PB::SevenDayShopData *pPBSevenDayShop = pPBShopInfo->mutable_seven_day_shop();
			for (WORD i = 0; i < tSevenDayShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tSevenDayShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBSevenDayShop->add_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 等级礼包商店 
		{
			LevelGiftShopData &tLevelGiftShop = tFullInfo.tShopInfo.tLevelGiftShop;
			PB::LevelGiftShopData *pPBLevelGiftShop = pPBShopInfo->mutable_level_gift_shop();
			for (WORD i = 0; i < tLevelGiftShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tLevelGiftShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBLevelGiftShop->add_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 无尽之地商店
		{
			EndlessLandShopData &tEndlessLandShop = tFullInfo.tShopInfo.tEndlessLandShop;
			PB::EndlessLandShopData *pPBEndlessLandShop = pPBShopInfo->mutable_endless_land_shop();
			for (WORD i = 0; i < tEndlessLandShop.wItemGoodsBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tEndlessLandShop.arItemGoodsBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBEndlessLandShop->add_item_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			for (WORD i = 0; i < tEndlessLandShop.wRewardGoodsBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tEndlessLandShop.arRewardGoodsBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBEndlessLandShop->add_reward_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 直购商店
		{
			DirectBuyShopData &tDirectBuyShop = tFullInfo.tShopInfo.tDirectBuyShop;
			PB::DirectBuyShopData *pPBDirectBuyShop = pPBShopInfo->mutable_direct_buy_shop();
			for (WORD i = 0; i < tDirectBuyShop.wGoodsNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tDirectBuyShop.arGoodsList[i];
				PB::ShopGoods *pPBGoods = pPBDirectBuyShop->add_goods_list();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			pPBDirectBuyShop->set_last_refresh_time(tDirectBuyShop.wLastRefreshTime);
			pPBDirectBuyShop->set_last_refresh_year_day(tDirectBuyShop.wLastRefreshYearDay);
			for (WORD i = 0; i < tDirectBuyShop.wCanTakeGoodsNum; ++i)
			{
				pPBDirectBuyShop->add_can_take_goods_list(tDirectBuyShop.arCanTakeGoodsList[i]);
			}
		}
		// 兑换商店
		{
			ExchangeShopData &tExchangeShop = tFullInfo.tShopInfo.tExchangeShop;
			PB::ExchangeShopData *pPBExchangeShop = pPBShopInfo->mutable_exchange_shop();
			for (WORD i = 0; i < tExchangeShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tExchangeShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBExchangeShop->add_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}

		// 推荐商店
		{
			RecommendShopData &tRecommendShop = tFullInfo.tShopInfo.tRecommendShop;
			PB::RecommendShopData *pPBRecommendShop = pPBShopInfo->mutable_recommend_shop();
			for (WORD i = 0; i < tRecommendShop.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tRecommendShop.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBRecommendShop->add_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 组队商店
		{
			TeamDungeonShopData &tTeamDungeonShop = tFullInfo.tShopInfo.tTeamDungeonShop;
			PB::TeamDungeonShop *pPBTeamDungeonShop = pPBShopInfo->mutable_teamdungeon_shop();
			for (WORD i = 0; i < tTeamDungeonShop.wNormalGoodsBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tTeamDungeonShop.arNormalGoodsBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBTeamDungeonShop->add_normal_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			for (WORD i = 0; i < tTeamDungeonShop.wRewardGoodsBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tTeamDungeonShop.arRewardGoodsBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBTeamDungeonShop->add_reward_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 圣诞节商店
		{
			ChristmasShopData &tChristmasShop = tFullInfo.tShopInfo.tChristmasShop;
			PB::ChristmasShop *pPBChristmasShop = pPBShopInfo->mutable_christmas_shop();
			for (WORD i = 0; i < tChristmasShop.wScoreExchangeGoodsBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tChristmasShop.arScoreExchangeGoodsBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBChristmasShop->add_score_exchange_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
			for (WORD i = 0; i < tChristmasShop.wItemExchangeGoodsBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tChristmasShop.arItemExchangeGoodsBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBChristmasShop->add_item_exchange_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 水晶商店
		{
			CrystalShopData &tCrystalShopData = tFullInfo.tShopInfo.tCrystalShopData;
			PB::CrystalShop *pPBCrystalShopData = pPBShopInfo->mutable_crystal_shop();
			pPBCrystalShopData->set_today_refresh_times(tCrystalShopData.wTodayRefreshTimes);
			for (WORD i = 0; i < tCrystalShopData.wGoodsNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tCrystalShopData.arGoodsList[i];
				PB::ShopGoods *pPBGoods = pPBCrystalShopData->add_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// 资源战商店
		{
			ResourceWarShopData &tResourceWarShopData = tFullInfo.tShopInfo.tResourceWarShop;
			PB::ResourceWarShop *pPBResourceWarShopData = pPBShopInfo->mutable_resource_war_shop();
			for (WORD i = 0; i < tResourceWarShopData.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tResourceWarShopData.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBResourceWarShopData->add_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
		// GM商店
		{
			GMShopData &tGMShopData = tFullInfo.tShopInfo.tGMShop;
			PB::GMShopData *pPBGMShopData = pPBShopInfo->mutable_gm_shop();
			for (WORD i = 0; i < tGMShopData.wBuyRecordNum; ++i)
			{
				ShopGoodsBuyRecord &tGoods = tGMShopData.arBuyRecord[i];
				PB::ShopGoods *pPBGoods = pPBGMShopData->add_goods_buy_record();
				pPBGoods->set_goods_id(tGoods.wGoodsId);
				pPBGoods->set_goods_num(tGoods.wBuyNum);
			}
		}
	}

	// 精灵数据
	{
		PB::RoleSpiritInfo *pPBSpiritInfo = tPBSecondInfo.mutable_role_sprite_info();
		PlayerSpriteInfo &tSpriteInfo = tFullInfo.tSpiritInfo;
		pPBSpiritInfo->set_stage(tSpriteInfo.tHeartOfWorld.wStage);
		pPBSpiritInfo->set_level(tSpriteInfo.tHeartOfWorld.wLevel);
		pPBSpiritInfo->set_cur_use_spirit_id(tSpriteInfo.dwCurUseSpiritId);
		for (WORD i = 0; i < tSpriteInfo.wSpiritNum; ++i)
		{
			SpiritData &tSpirit = tSpriteInfo.arSpiritList[i];
			::PB::SpiritData *pPBSpirit = pPBSpiritInfo->add_spirit_list();
			pPBSpirit->set_id(tSpirit.dwId);
			pPBSpirit->set_enhance_level(tSpirit.wEnhanceLevel);
			pPBSpirit->set_train_stage(tSpirit.wTrainStage);
			pPBSpirit->set_train_times(tSpirit.wTrainTimes);
			pPBSpirit->set_star(tSpirit.wStarLevel);
		}
		for (WORD i = 0; i < tSpriteInfo.wPokedexNum; ++i)
		{
			SpiritPokedex &tPokedex = tSpriteInfo.arPokedexList[i];
			::PB::SpiritPokedex *pPBPokedex = pPBSpiritInfo->add_pokedex_list();
			pPBPokedex->set_id(tPokedex.wPokedexId);
			pPBPokedex->set_level(tPokedex.wLevel);
		}
	}

	// 好友数据
	PB::RoleFriendData tPBFriendInfo;
	PlayerFriendData &tFriendInfo = tFullInfo.tFriendInfo;
	tPBFriendInfo.set_get_energy_times(tFriendInfo.wGetEnergyTimes);
	for (WORD i = 0; i < tFriendInfo.wFriendNum; ++i)
	{
		FriendData &tFriend = tFriendInfo.arFriendList[i];
		PB::FriendData *pPBFriendData = tPBFriendInfo.add_friend_list();
		pPBFriendData->set_role_id(tFriend.dwFriendId);
		pPBFriendData->set_is_give_energy(tFriend.wIsGiveEnergy);
		pPBFriendData->set_get_energy_state(tFriend.wTakeEnergyState);
		pPBFriendData->set_help_battle_times(tFriend.wHelpBattleTimes);
	}
	for (WORD i = 0; i < tFriendInfo.wBlackNum; ++i)
	{
		tPBFriendInfo.add_black_list(tFriendInfo.arBlackList[i]);
	}
	for (WORD i = 0; i < tFriendInfo.wApplyNum; ++i)
	{
		tPBFriendInfo.add_apply_list(tFriendInfo.arApplyList[i]);
	}

	// 叛军数据
	{
		PB::RoleRebelInfo *pPBRebelInfo = tPBSecondInfo.mutable_role_rebel_info();
		PlayerRebelData &tRebelInfo = tFullInfo.tRebelInfo;
		pPBRebelInfo->set_rebel_level(tRebelInfo.wRebelLevel);
		pPBRebelInfo->set_today_exploit(tRebelInfo.dwTodayExploit);
		pPBRebelInfo->set_today_damage(tRebelInfo.dwTodayDamage);
		pPBRebelInfo->set_is_already_share(tRebelInfo.wIsAlreadyShare);
		for (WORD i = 0; i < tRebelInfo.wAwardRecordNum; ++i)
		{
			pPBRebelInfo->add_reward_get_record_list(tRebelInfo.arAwardRecordList[i]);
		}
	}

	// 夺宝信息
	{
		PB::RoleDuobaoInfo *pPBDuobaoInfo = tPBSecondInfo.mutable_role_duobao_info();
		PlayerDuobaoInfo &tDuobaoInfo = tFullInfo.tDuobaoInfo;
		pPBDuobaoInfo->set_peace_over_time(tDuobaoInfo.dwPeaceOverTime);
		for (WORD i = 0; i < QUALITY_NUM; ++i)
		{
			pPBDuobaoInfo->add_doubao_times_list(tDuobaoInfo.arDoubaoTimes[i]);
		}
		pPBDuobaoInfo->set_daily_duobao_times(tDuobaoInfo.wDailyDoubaoTimes);
		for (WORD i = 0; i < tDuobaoInfo.wPregrossRewardRecordNum && i < MAX_DOUBAO_PREGROSS_NUM; ++i)
		{
			pPBDuobaoInfo->add_pregross_reward_record(tDuobaoInfo.arPregrossRewardRecord[i]);
		}
	}

	// 成就信息
	{
		PB::RoleAchievemntInfo *pPBAchievementInfo = tPBSecondInfo.mutable_role_achievement_info();
		PlayerAchievementData &tAchievementInfo = tFullInfo.tAchievementInfo;
		for (WORD i = 0; i < tAchievementInfo.wAchievementNum; ++i)
		{
			AchievementData &tAchievement = tAchievementInfo.arAchievementList[i];
			PB::AchievementData *pPBAchievement = pPBAchievementInfo->add_achievement_list();
			pPBAchievement->set_id(tAchievement.wId);
			pPBAchievement->set_level(tAchievement.byLv);
			pPBAchievement->set_value(tAchievement.dwValue);
			pPBAchievement->set_cur_reward_level(tAchievement.byCurRewardLevel);
		}
	}

	// 日常任务信息
	{
		PB::RoleDailyMissionInfo *pPBDailyMissionInfo = tPBSecondInfo.mutable_role_daily_mission_info();
		PlayerDailyMissionData &tDailyMissionInfo = tFullInfo.tDailyMissionInfo;
		pPBDailyMissionInfo->set_point(tDailyMissionInfo.wPoint);
		for (WORD i = 0; i < tDailyMissionInfo.wPointAwardRecordNum; ++i)
		{
			pPBDailyMissionInfo->add_point_reward_record(tDailyMissionInfo.arPointAwardRecord[i]);
		}
		for (WORD i = 0; i < tDailyMissionInfo.wDailyMissionNum; ++i)
		{
			DailyMissionData &tMission = tDailyMissionInfo.arDailyMissionList[i];
			PB::DailyMission *pPBMission = pPBDailyMissionInfo->add_mission_list();
			pPBMission->set_id(tMission.wMissionId);
			pPBMission->set_is_get_reward(tMission.byIsGetAward);
			pPBMission->set_value(tMission.dwValue);
		}
	}

	// 工会技能数据
	{
		PB::RoleGuildSkillInfo *pPBGuildSkillInfo = tPBSecondInfo.mutable_role_guild_skill_info();
		PlayerGuildSkillData &tGuildSkillData = tFullInfo.tGuildSkillData;
		for (WORD i = 0; i < tGuildSkillData.wSkillNum; ++i)
		{
			SkillData &tSkill = tGuildSkillData.arSkillList[i];
			PB::SkillData *pSkillData = pPBGuildSkillInfo->add_skill_list();
			pSkillData->set_skill_id(tSkill.wSkillId);
			pSkillData->set_level(tSkill.wLevel);
		}
	}

	// 签到数据
	{
		PB::RoleSignInData *pPBSignInInfo = tPBSecondInfo.mutable_role_sign_in_data();
		SignInData &tSignInData = tFullInfo.tSignInData;
		pPBSignInInfo->set_cur_sign_in_day(tSignInData.wCurSignInDay);
		pPBSignInInfo->set_last_sign_in_time(tSignInData.dwLastSignInTime);
	}

	// PVP数据
	{
		PlayerPVPData &tPVPData = tSecondInfo.tPVPData;
		::PB::RolePVPData *pPBPVPData = tPBSecondInfo.mutable_pvp_data();
		pPBPVPData->set_today_battle_times(tPVPData.wTodayBattleTimes);
		pPBPVPData->set_week_battle_times(tPVPData.wWeekBattleTimes);
		pPBPVPData->set_score(tPVPData.dwScore);
		pPBPVPData->set_last_week_rank(tPVPData.wLastWeekRank);
		pPBPVPData->set_win_times(tPVPData.wWeekWinTimes);
		pPBPVPData->set_total_battle_times(tPVPData.dwTotalBattleTimes);
		for (WORD i = 0; i < tPVPData.wDailyRewardTakeRecordNum; ++i)
		{
			pPBPVPData->add_daily_reward_take_record(tPVPData.arDailyRewardTakeRecord[i]);
		}
		for (WORD i = 0; i < tPVPData.wWeeklyRewardTakeRecordNum; ++i)
		{
			pPBPVPData->add_weekly_reward_take_record(tPVPData.arWeeklyRewardTakeRecord[i]);
		}
	}
	// 称号
	{
		PlayerTitleInfo &tRoleTitleInfo = tSecondInfo.tTitleInfo;
		::PB::RoleTitleInfo *pPBRoleTitleInfo = tPBSecondInfo.mutable_title_info();
		pPBRoleTitleInfo->set_cur_use_title_id(tRoleTitleInfo.nCurUseTitleId);
		pPBRoleTitleInfo->set_cur_use_nickname_id(tRoleTitleInfo.nCurUseNicknameId);
		for (WORD i = 0; i < tRoleTitleInfo.wTitleNum; ++i)
		{
			TitleData &tTitleData = tRoleTitleInfo.arTitleList[i];
			::PB::TitleData *pPBTitleData = pPBRoleTitleInfo->add_title_list();
			pPBTitleData->set_id(tTitleData.nTitleId);
			pPBTitleData->set_over_time(tTitleData.nOverTime);
		}
	}
	// 无尽之地
	{
		PlayerEndlessLandData &tRoleEndlessLandInfo = tSecondInfo.tEndlessLandData;
		::PB::RoleEndlessLandInfo *pPBRoleEndlessLandInfo = tPBSecondInfo.mutable_endless_land_info();
		pPBRoleEndlessLandInfo->set_is_today_first_battle(tRoleEndlessLandInfo.wIsTodayFirstBattle);
		pPBRoleEndlessLandInfo->set_cur_wave(tRoleEndlessLandInfo.wCurWave);
		pPBRoleEndlessLandInfo->set_today_wave_record(tRoleEndlessLandInfo.wTodayWaveRecord);
		pPBRoleEndlessLandInfo->set_cur_wave_kill_num(tRoleEndlessLandInfo.wCurWaveKillNum);
		pPBRoleEndlessLandInfo->set_history_wave_record(tRoleEndlessLandInfo.wHistoryWaveRecord);
		pPBRoleEndlessLandInfo->set_cur_keep_kill_num(tRoleEndlessLandInfo.wCurContinueKillNum);
		pPBRoleEndlessLandInfo->set_keep_kill_num_record(tRoleEndlessLandInfo.wContinueKillNumRecord);
		pPBRoleEndlessLandInfo->set_today_collision_reward_times(tRoleEndlessLandInfo.wTodayCollisionRewardTimes);
		pPBRoleEndlessLandInfo->set_last_take_continue_kill_reward(tRoleEndlessLandInfo.wLastTakeContinueKillReward);
		pPBRoleEndlessLandInfo->set_is_first_battle(tRoleEndlessLandInfo.wIsFirstBattle);
		pPBRoleEndlessLandInfo->set_today_battle_times(tRoleEndlessLandInfo.wTodayBattleTimes);
		// 	for (WORD i = 0; i < tRoleEndlessLandInfo.wContinueKillRewardRecordNum; ++i)
		// 	{
		// 		pPBRoleEndlessLandInfo->add_keep_kill_reward_record(tRoleEndlessLandInfo.arContinueKillRewardRecord[i]);
		// 	}
	}
	// 玩家工会数据
	{
		PlayerGuildData &tRoleGuildData = tFullInfo.tSecondInfo.tGuildData;
		::PB::RoleGuildData *pPBRoleGuildData = tPBSecondInfo.mutable_guild_data();
		for (WORD i = 0; i < tRoleGuildData.wTakeChapterRewardRecordNum; ++i)
		{
			pPBRoleGuildData->add_take_chapter_reward_record(tRoleGuildData.arTakeChapterRewardRecord[i]);
		}
	}
	// 豪华签到数据
	{
		PlayerLuxurySignData &tRoleLuxurySignData = tFullInfo.tSecondInfo.tLuxurySignData;
		::PB::PlayerLuxurySignInData *pPBRoleLuxurySignData = tPBSecondInfo.mutable_luxury_sign_in_data();
		for (WORD i = 0; i < tRoleLuxurySignData.wSignDataNum; ++i)
		{
			LuxurySignData &luxurySignInData = tRoleLuxurySignData.arSignDataList[i];
			::PB::LuxurySignInData *pLuxurySignInData = pPBRoleLuxurySignData->add_sign_in_data_list();
			pLuxurySignInData->set_sign_in_id(luxurySignInData.wSignId);
			pLuxurySignInData->set_status(luxurySignInData.wStatus);
			pLuxurySignInData->set_rewar_id(luxurySignInData.wRewardId);
		}
	}

	// 宝石数据
	{
		PlayerGemInfo &tGemInfo = tFullInfo.tSecondInfo.tGemInfo;
		::PB::PlayerGemInfo *pPBGemInfo = tPBSecondInfo.mutable_gem_info();
		pPBGemInfo->set_hole_init_time(tGemInfo.dwHoleInitTime);
		for (WORD i = 0; i < tGemInfo.wGemNum; ++i)
		{
			Gem &tGem = tGemInfo.arGemList[i];
			::PB::Gem *pPBGem = pPBGemInfo->add_gem_list();
			pPBGem->set_id(tGem.dwId);
			pPBGem->set_num(tGem.dwNum);
		}
		for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
		{
			pPBGemInfo->add_hole_open_list(tGemInfo.arHoleOpenList[i]);
		}
	}

	// 登录奖励数据
	{
		LoginRewardData &tLoginRewardData = tFullInfo.tSecondInfo.tLoginRewardData;
		::PB::LoginRewardData *pPBLoginRewardData = tPBSecondInfo.mutable_login_reward_data();
		pPBLoginRewardData->set_login_days(tLoginRewardData.wLoginDays);
		for (WORD i = 0; i < tLoginRewardData.wRewardRecordNum; ++i)
		{
			pPBLoginRewardData->add_reward_record_list(tLoginRewardData.arRewardRecordList[i]);
		}
	}

// 王者赛数据
	{
		PlayerKingMatchData &tKingMatchData = tFullInfo.tSecondInfo.tKingMatchData;
		::PB::RoleKingMatchData *pPBKingMatchData = tPBSecondInfo.mutable_king_match_data();
		pPBKingMatchData->set_last_season(tKingMatchData.wLastSeason);
		pPBKingMatchData->set_total_battle_times(tKingMatchData.wTotalBattleTimes);
		pPBKingMatchData->set_total_win_times(tKingMatchData.wTotalWinTimes);
		pPBKingMatchData->set_today_win_times(tKingMatchData.wTodayWinTimes);
		pPBKingMatchData->set_stage_level(tKingMatchData.wStageLevel);
		pPBKingMatchData->set_context(tKingMatchData.dwContext);
		pPBKingMatchData->set_history_stage_level(tKingMatchData.wHistoryStageLevel);
		for (WORD i = 0; i < tKingMatchData.wSeasonRewardRecordNum; ++i)
		{
			pPBKingMatchData->add_season_task_reward_record(tKingMatchData.arSeasonTaskRewardRecord[i]);
		}
	}

	// 组队副本数据
	{
		const PlayerTeamDungeonData &tTeamDungeonData = tFullInfo.tSecondInfo.tTeamDungeonData;
		::PB::PlayerTeamDungeonData *pPBTeamDungeonData = tPBSecondInfo.mutable_team_dungeon_data();
		pPBTeamDungeonData->set_today_battle_times(tTeamDungeonData.wTodayBattleTimes);
		pPBTeamDungeonData->set_today_kill_reward_times(tTeamDungeonData.wTodayKillRewardTimes);
		pPBTeamDungeonData->set_today_mvp_reward_times(tTeamDungeonData.wTodayMVPRewardTimes);
		for (WORD i = 0; i < tTeamDungeonData.wPassDungeonNum; ++i)
		{
			pPBTeamDungeonData->add_pass_dungeon_list(tTeamDungeonData.arPassDungeonList[i]);
		}
	}

	// 世界等级任务数据
	{
		const PlayerWorldLevelTaskData &tRoleWorldLevelTaskData = tFullInfo.tSecondInfo.tRoleWorldLevelTaskData;
		::PB::RoleWorldLevelTaskInfo *pPBRoleWorldLevelTaskData = tPBSecondInfo.mutable_role_world_level_task_info();
		pPBRoleWorldLevelTaskData->set_is_already_take_final_reward(tRoleWorldLevelTaskData.wIsAlreadyTakeFinalReward);
		for (WORD i = 0; i < tRoleWorldLevelTaskData.wTaskEventNum; ++i)
		{
			const WorldLevelTaskEvent &tTaskEvent = tRoleWorldLevelTaskData.arTaskEventList[i];
			::PB::WorldLevelTaskData *pPBTaskData = pPBRoleWorldLevelTaskData->add_task_event_list();
			pPBTaskData->set_task_id(tTaskEvent.wTaskId);
			pPBTaskData->set_context(tTaskEvent.nContext);
			pPBTaskData->set_is_already_take(tTaskEvent.wIsAlreadyTake);
			pPBTaskData->set_task_type(tTaskEvent.wTaskType);
			pPBTaskData->set_task_level(tTaskEvent.wTaskLevel);
		}
	}

	// 挂机掉落数据
	{
		const PlayerHangUpDropData &tRoleHangUpDropData = tFullInfo.tSecondInfo.tHangUpDropData;
		::PB::RoleHangUpDropData *pPBRoleHangUpDropData = tPBSecondInfo.mutable_role_hang_up_drop_data();
		pPBRoleHangUpDropData->set_last_obtain_reward_time(tRoleHangUpDropData.dwLastObtainRewardTime);
	}

	// 奖励数据
	PB::RoleRewardData tPBRewardInfo;
	PlayerRewardData &tRewardInfo = tFullInfo.tRewardInfo;
	for (WORD i = 0; i < tRewardInfo.wRewardNum; ++i)
	{
		RewardData *pReward = &tRewardInfo.arRewardList[i];
		PB::RewardData *pPBReward = tPBRewardInfo.add_reward_list();
		pPBReward->set_uid(pReward->dwUId);
		pPBReward->set_info_id(pReward->wInfoId);
		pPBReward->set_value1(pReward->dwValue1);
		pPBReward->set_value2(pReward->dwValue2);
		pPBReward->set_time(pReward->dwTime);
		pPBReward->set_text(pReward->szText);
	}

	// 活动数据
	PB::RoleActivityData tPBActivityInfo;
	PlayerActivityData &tActivityInfo = tFullInfo.tActivityInfo;
	PB::TQTData *PBTQTData = tPBActivityInfo.mutable_tqt_data();
	PB::YCSData *PBYCSData = tPBActivityInfo.mutable_ycs_data();
	// 铜雀台
	PBTQTData->set_last_get_time(tActivityInfo.tTQTData.dwLastGetTime);
	for (WORD i = 0; i < tActivityInfo.tTQTData.wTakeRecordNum; ++i)
	{
		PBTQTData->add_take_record_list(tActivityInfo.tTQTData.arTakeRecordList[i]);
	}
	// 迎财神
	PBYCSData->set_today_get_times(tActivityInfo.tYCSData.byTodayGetTimes);
	PBYCSData->set_total_get_times(tActivityInfo.tYCSData.byTotalGetTimes);
	PBYCSData->set_total_reward_value(tActivityInfo.tYCSData.nTotalRewardValue);
	PBYCSData->set_last_get_time(tActivityInfo.tYCSData.dwLastGetTime);
	// 7天活动
	for (WORD i = 0; i < tActivityInfo.wSevenDayTargetNum; ++i)
	{
		SevenDayTargetData &tTargetData = tActivityInfo.arSevenDayTargetList[i];
		PB::SevenDayTargetData *pPBTargetData = tPBActivityInfo.add_seven_day_target_list();
		pPBTargetData->set_target_id(tTargetData.wId);
		pPBTargetData->set_value(tTargetData.dwValue);
		pPBTargetData->set_is_get_reward(tTargetData.byIsGetReward);
	}
	for (WORD i = 0; i < tActivityInfo.wCompletionRewardRecordNum; ++i)
	{
		WORD wCompletionNum = tActivityInfo.arCompletionRewardRecord[i];
		tPBActivityInfo.add_completion_reward_record_list(wCompletionNum);
	}
	// 等级商店活动
	for (WORD i = 0; i < tActivityInfo.wLevelShopActivityDataNum; ++i)
	{
		LevelShopActivityData &tActivityData = tActivityInfo.arLevelShopActivityDataList[i];
		PB::LevelShopActivityData *pPBActivityData = tPBActivityInfo.add_level_shop_activity_data_list();
		pPBActivityData->set_level(tActivityData.wLevel);
		pPBActivityData->set_activate_time(tActivityData.dwActivateTime);
	}
	// 等级礼包
	for (WORD i = 0; i < tActivityInfo.wLevelGiftTakeRecordNum; ++i)
	{
		tPBActivityInfo.add_level_gift_take_record_list(tActivityInfo.arLevelGiftTakeRecord[i]);
	}
	// VIP礼包数据
	PB::VipGiftData *pPBVipGiftData = tPBActivityInfo.mutable_vip_gift_data();
	VipGiftData &tVipGiftData= tActivityInfo.tVipGiftData;
	pPBVipGiftData->set_init_vip_level(tVipGiftData.wInitVipLevel);
	pPBVipGiftData->set_vip_level_reward_record(tVipGiftData.nVipLevelRewardRecord);
	pPBVipGiftData->set_today_recommend_recharge_reward_id(tVipGiftData.wTodayRecommendRechargeRewardId);
	pPBVipGiftData->set_today_recommend_recharge_reward_status(tVipGiftData.wTodayRecommendRechargeRewardStatus);
	for (WORD i = 0; i < tVipGiftData.wWeekGiftNum; ++i)
	{
		Gift &tGift = tVipGiftData.arWeekGiftList[i];
		::PB::Gift *pPBGift = pPBVipGiftData->add_week_gift_list();
		pPBGift->set_id(tGift.wId);
		pPBGift->set_num(tGift.wNum);
	}
	// 充值&月卡
	PB::RoleRechargeData *pPBRechargeData = tPBActivityInfo.mutable_recharge_data();
	RoleRechargeData &tRechargeData = tActivityInfo.tRechargeData;
	pPBRechargeData->set_today_recharge_times(tRechargeData.wTodayRechargetTimes);
	for (WORD i = 0; i < tRechargeData.wRechargeNum; ++i)
	{
		::PB::CommonRecharge *pPBRecharge = pPBRechargeData->add_recharge_list();
		pPBRecharge->set_id(tRechargeData.arRechargeList[i].wRechargeId);
		pPBRecharge->set_times(tRechargeData.arRechargeList[i].wRechargeTimes);
	}
	for (WORD i = 0; i < tRechargeData.wMontchCardNum; ++i)
	{
		MonthCard &tMonthCard = tRechargeData.arMonthCardList[i];
		::PB::MonthCardData *pPBMonthCard = pPBRechargeData->add_month_card_list();
		pPBMonthCard->set_card_id(tMonthCard.wCardId);
		pPBMonthCard->set_buy_time(tMonthCard.dwBuyTime);
		pPBMonthCard->set_last_take_reward_time(tMonthCard.dwLastTakeRewardTime);
	}
	for (WORD i = 0; i < tRechargeData.wCPOrderRecordNum; ++i)
	{
		pPBRechargeData->add_order_record_list(tRechargeData.arCPOrderRecordList[i]);
	}
	// 开服基金
	PB::OpenServerFundData *pPBFundData = tPBActivityInfo.mutable_open_server_fund_data();
	OpenServerFundData &tFundData = tActivityInfo.tOpenServerFundData;
	pPBFundData->set_is_already_buy_fund(tFundData.wIsAlreadyBuyFund);
	for (WORD i = 0; i < tFundData.wFundRewardTakeRecordNum; ++i)
	{
		pPBFundData->add_fund_reward_take_record(tFundData.arFundRewardTakeRecord[i]);
	}
	// 全民福利
	PB::AllPeopleWelfareData *pPBAllPeopleWelfareData = tPBActivityInfo.mutable_all_people_welfare_data();
	AllPeopleWelfareData &tAllPeopleWelfareData = tActivityInfo.tAllPoepleWelfareData;
	for (WORD i = 0; i < tAllPeopleWelfareData.wTakeWelfareRecordNum; ++i)
	{
		pPBAllPeopleWelfareData->add_welfare_take_record(tAllPeopleWelfareData.arTakeWelfareRecordList[i]);
	}
	// 限时活动
	PB::LimitActivityData *pPBLimitActivityData = tPBActivityInfo.mutable_limit_activity_data();
	LimitActivityData &tLimitActivityData = tActivityInfo.tLimitActivityData;
	pPBLimitActivityData->set_total_recharge(tLimitActivityData.dwTotleRecharge);
	for (WORD i = 0; i < tLimitActivityData.wFirstRechargeRewardRecordNum; ++i)
	{
		pPBLimitActivityData->add_first_recharge_reward_record(tLimitActivityData.arFirstRechargeRewardRecord[i]);
	}
	// 次日奖励数据
	tPBActivityInfo.set_is_take_next_day_reward(tActivityInfo.wIsTakeNextDayReward);
	// 超值折扣
	{
		PB::SuperDiscountData *pPBSuperDiscount = tPBActivityInfo.mutable_super_discount_data();
		SuperDiscountData &tSuperDiscount = tActivityInfo.tSuperDiscountData;
		pPBSuperDiscount->set_status(tSuperDiscount.wStatus);
		pPBSuperDiscount->set_take_accumulate_reward_times(tSuperDiscount.wTakeAccumulateRewardTimes);
		pPBSuperDiscount->set_cur_buy_times(tSuperDiscount.wCurBuyTimes);
		pPBSuperDiscount->set_last_activity_data_id(tSuperDiscount.wLastActivityDataId);
		for (WORD i = 0; i < tSuperDiscount.wTookAccumulateRewardNum; ++i){
			pPBSuperDiscount->add_took_accumulate_reward_list(tSuperDiscount.arTookAccumulateRewardList[i]);
		}

	}
	// 新超值折扣
	{
		PB::SuperDiscountData *pPBNewSuperDiscount = tPBActivityInfo.mutable_new_super_discount_data();
		SuperDiscountData &tNewSuperDiscount = tActivityInfo.tNewSuperDiscountData;
		pPBNewSuperDiscount->set_status(tNewSuperDiscount.wStatus);
		pPBNewSuperDiscount->set_take_accumulate_reward_times(tNewSuperDiscount.wTakeAccumulateRewardTimes);
		pPBNewSuperDiscount->set_cur_buy_times(tNewSuperDiscount.wCurBuyTimes);
		pPBNewSuperDiscount->set_last_activity_data_id(tNewSuperDiscount.wLastActivityDataId);
	}
	// 节日活动
	PB::RoleFestivalActivityInfo *pPBRoleFestivalActivityInfo = tPBActivityInfo.mutable_role_festival_activity_info();
	PlayerFestivalActivityInfo &tRoleFestivalActivityInfo = tActivityInfo.tFestivalActivityInfo;
	// 循环单充
	{
		::PB::FestivalActivity *pPBLoopOnceRecharge = pPBRoleFestivalActivityInfo->mutable_loop_once_recharge();
		const FestivalActivity &tLoopOnceRecharge = tRoleFestivalActivityInfo.tLoopOnceRecharge;
		pPBLoopOnceRecharge->set_data_reset_time(tLoopOnceRecharge.dwDataResetTime);
		for (WORD i = 0; i < tLoopOnceRecharge.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tLoopOnceRecharge.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBLoopOnceRecharge->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 循环累充
	{
		::PB::FestivalActivity *pPBLoopAccumulateRecharge = pPBRoleFestivalActivityInfo->mutable_loop_accumulate_recharge();
		const FestivalActivity &tLoopAccumulateRecharge = tRoleFestivalActivityInfo.tLoopAccumulateRecharge;
		pPBLoopAccumulateRecharge->set_data_reset_time(tLoopAccumulateRecharge.dwDataResetTime);
		for (WORD i = 0; i < tLoopAccumulateRecharge.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tLoopAccumulateRecharge.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBLoopAccumulateRecharge->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 开服限时召唤
	{
		::PB::FestivalActivity *pPBLimitRecruit = pPBRoleFestivalActivityInfo->mutable_limit_recruit();
		const FestivalActivity &tLimitRecruit = tRoleFestivalActivityInfo.tLimitRecruit;
		pPBLimitRecruit->set_data_reset_time(tLimitRecruit.dwDataResetTime);
		for (WORD i = 0; i < tLimitRecruit.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tLimitRecruit.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBLimitRecruit->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 活动单充
	{
		::PB::FestivalActivity *pPBActivityOnceRecharge = pPBRoleFestivalActivityInfo->mutable_activity_once_recharge();
		const FestivalActivity &tActivityOnceRecharge = tRoleFestivalActivityInfo.tActivityOnceRecharge;
		pPBActivityOnceRecharge->set_data_reset_time(tActivityOnceRecharge.dwDataResetTime);
		for (WORD i = 0; i < tActivityOnceRecharge.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tActivityOnceRecharge.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBActivityOnceRecharge->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 活动单充2
	{
		::PB::FestivalActivity *pPBActivityOnceRecharge2 = pPBRoleFestivalActivityInfo->mutable_activity_once_recharge_2();
		const FestivalActivity &tActivityOnceRecharge2 = tRoleFestivalActivityInfo.tActivityOnceRecharge2;
		pPBActivityOnceRecharge2->set_data_reset_time(tActivityOnceRecharge2.dwDataResetTime);
		for (WORD i = 0; i < tActivityOnceRecharge2.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tActivityOnceRecharge2.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBActivityOnceRecharge2->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 活动累充
	{
		::PB::FestivalActivity *pPBActivityAccumulateRecharge = pPBRoleFestivalActivityInfo->mutable_activity_accumulate_recharge();
		const FestivalActivity &tActivityAccumulateRecharge = tRoleFestivalActivityInfo.tActivityAccumulateRecharge;
		pPBActivityAccumulateRecharge->set_data_reset_time(tActivityAccumulateRecharge.dwDataResetTime);
		for (WORD i = 0; i < tActivityAccumulateRecharge.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tActivityAccumulateRecharge.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBActivityAccumulateRecharge->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 打折兑换（物品打折卖钻石,钻石换物品）
	{
		::PB::FestivalActivity *pPBDiamondExchange = pPBRoleFestivalActivityInfo->mutable_diamond_exchange();
		const FestivalActivity &tDiamondExchange = tRoleFestivalActivityInfo.tDiamondExchange;
		pPBDiamondExchange->set_data_reset_time(tDiamondExchange.dwDataResetTime);
		for (WORD i = 0; i < tDiamondExchange.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tDiamondExchange.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBDiamondExchange->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 活跃目标
	{
		::PB::FestivalActivity *pPBActiveTarget = pPBRoleFestivalActivityInfo->mutable_active_target();
		const FestivalActivity &tActiveTarget = tRoleFestivalActivityInfo.tActiveTarget;
		pPBActiveTarget->set_data_reset_time(tActiveTarget.dwDataResetTime);
		for (WORD i = 0; i < tActiveTarget.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tActiveTarget.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBActiveTarget->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 材料兑换（物品换物品）
	{
		::PB::FestivalActivity *pPBItemExchange = pPBRoleFestivalActivityInfo->mutable_item_exchange();
		const FestivalActivity &tItemExchange = tRoleFestivalActivityInfo.tItemExchange;
		pPBItemExchange->set_data_reset_time(tItemExchange.dwDataResetTime);
		for (WORD i = 0; i < tItemExchange.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tItemExchange.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBItemExchange->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 节日登陆
	{
		::PB::FestivalActivity *pPBFestivalLogin = pPBRoleFestivalActivityInfo->mutable_festival_login();
		const FestivalActivity &tFestivalLogin = tRoleFestivalActivityInfo.tFestivalLogin;
		pPBFestivalLogin->set_data_reset_time(tFestivalLogin.dwDataResetTime);
		for (WORD i = 0; i < tFestivalLogin.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tFestivalLogin.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBFestivalLogin->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 开服登陆
	{
		::PB::FestivalActivity *pPBOpenServerLogin = pPBRoleFestivalActivityInfo->mutable_open_server_login();
		const FestivalActivity &tOpenServerLogin = tRoleFestivalActivityInfo.tOpenServerLogin;
		pPBOpenServerLogin->set_data_reset_time(tOpenServerLogin.dwDataResetTime);
		for (WORD i = 0; i < tOpenServerLogin.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tOpenServerLogin.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBOpenServerLogin->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 开服累充
	{
		::PB::FestivalActivity *pPBOpenSvrAccumulateRecharge = pPBRoleFestivalActivityInfo->mutable_open_svr_accumulate_recharge();
		const FestivalActivity &tOpenSvrAccumulateRecharge = tRoleFestivalActivityInfo.tOpenSvrAccumulateRecharge;
		pPBOpenSvrAccumulateRecharge->set_data_reset_time(tOpenSvrAccumulateRecharge.dwDataResetTime);
		for (WORD i = 0; i < tOpenSvrAccumulateRecharge.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tOpenSvrAccumulateRecharge.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBOpenSvrAccumulateRecharge->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 节日材料兑换
	{
		::PB::FestivalActivity *pPBFestivalItemExchange = pPBRoleFestivalActivityInfo->mutable_festival_item_exchange();
		const FestivalActivity &tFestivalItemExchange = tRoleFestivalActivityInfo.tFestivalItemExchange;
		pPBFestivalItemExchange->set_data_reset_time(tFestivalItemExchange.dwDataResetTime);
		for (WORD i = 0; i < tFestivalItemExchange.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tFestivalItemExchange.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBFestivalItemExchange->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 节日打折贩售
	{
		::PB::FestivalActivity *pPBFestivalDiscountSell = pPBRoleFestivalActivityInfo->mutable_festival_discount_sell();
		const FestivalActivity &tFestivalDiscountSell = tRoleFestivalActivityInfo.tFestivalDiscountSell;
		pPBFestivalDiscountSell->set_data_reset_time(tFestivalDiscountSell.dwDataResetTime);
		for (WORD i = 0; i < tFestivalDiscountSell.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tFestivalDiscountSell.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBFestivalDiscountSell->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 节日活跃目标
	{
		::PB::FestivalActivity *pPBFestivalActiveTarget = pPBRoleFestivalActivityInfo->mutable_festival_active_target();
		const FestivalActivity &tFestivalActiveTarget = tRoleFestivalActivityInfo.tFestivalActiveTarget;
		pPBFestivalActiveTarget->set_data_reset_time(tFestivalActiveTarget.dwDataResetTime);
		for (WORD i = 0; i < tFestivalActiveTarget.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tFestivalActiveTarget.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBFestivalActiveTarget->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 开服活跃2(同7)
	{
		::PB::FestivalActivity *pPBOpenServerActiveTarget = pPBRoleFestivalActivityInfo->mutable_open_server_active_target();
		const FestivalActivity &tOpenServerActiveTarget = tRoleFestivalActivityInfo.tOpenServerActiveTarget;
		pPBOpenServerActiveTarget->set_data_reset_time(tOpenServerActiveTarget.dwDataResetTime);
		for (WORD i = 0; i < tOpenServerActiveTarget.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tOpenServerActiveTarget.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBOpenServerActiveTarget->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}
	// 节日活跃2(同14)
	{
		::PB::FestivalActivity *pPBFestivalActiveTarget2 = pPBRoleFestivalActivityInfo->mutable_festival_active_target_2();
		const FestivalActivity &tFestivalActiveTarget2 = tRoleFestivalActivityInfo.tFestivalActiveTarget2;
		pPBFestivalActiveTarget2->set_data_reset_time(tFestivalActiveTarget2.dwDataResetTime);
		for (WORD i = 0; i < tFestivalActiveTarget2.wTargetNum; ++i)
		{
			const ActivityTarget &tTarget = tFestivalActiveTarget2.arTargetList[i];
			::PB::ActivityTarget *pPBTarget = pPBFestivalActiveTarget2->add_target_list();
			pPBTarget->set_target_id(tTarget.wId);
			pPBTarget->set_context(tTarget.dwContext);
			pPBTarget->set_take_reward_times(tTarget.wTakeRewardTimes);
		}
	}

	// 精准推荐
	PB::RoleAccurateRecommendData *pPBRoleAccurateRecommendData = tPBActivityInfo.mutable_role_accurate_recommend_data();
	PlayerAccurateRecommendData &tRoleAccurateRecommendData = tActivityInfo.tAccurateRecommendData;
	for (WORD i = 0; i < tRoleAccurateRecommendData.wGoodsNum; ++i)
	{
		AccurateRecommendGoods &tGoods = tRoleAccurateRecommendData.arGoodsList[i];
		::PB::AccurateRecommendGoods *pPBGoods = pPBRoleAccurateRecommendData->add_goods_list();
		pPBGoods->set_goods_id(tGoods.wGoodsId);
		pPBGoods->set_buy_times(tGoods.wBuyTimes);
		pPBGoods->set_can_take_times(tGoods.wCanTakeTimes);
	}

	// 圣诞节数据
	const PlayerChristmasData &tChristmasData = tActivityInfo.tChristmasData;
	::PB::RoleChristmasData *pPBChristmasData = tPBActivityInfo.mutable_role_christmas_data();
	pPBChristmasData->set_total_score(tChristmasData.dwTotalScore);
	pPBChristmasData->set_cur_score(tChristmasData.dwCurScore);
	pPBChristmasData->set_already_take_lower_boss_reward_level(tChristmasData.wAlreadyTakeLowerBossRewardLevel);
	pPBChristmasData->set_already_take_high_boss_reward_level(tChristmasData.wAlreadyTakeHighBossRewardLevel);
	pPBChristmasData->set_last_reset_data_time(tChristmasData.dwLastResetDataTime);

	// facebook活动数据
	{
		const FacebookActivityData &tFacebookActivityData = tActivityInfo.tFacebookActivityData;
		::PB::FacebookActivityData *pPBFacebookActivityData = tPBActivityInfo.mutable_facebook_activity_data();
		pPBFacebookActivityData->set_is_already_take_account_bingding_reward(tFacebookActivityData.wIsAlreadyTakeAccountBindingReward);
		pPBFacebookActivityData->set_ready_to_share_time(tFacebookActivityData.dwReadyToShareTime);
		pPBFacebookActivityData->set_ready_to_commend_time(tFacebookActivityData.dwReadyToCommendTime);
		pPBFacebookActivityData->set_recall_success_friend_num(tFacebookActivityData.wSummonSuccessFriendNum);
		pPBFacebookActivityData->set_invite_success_friend_num(tFacebookActivityData.wInviteSuccessFriendNum);
		// 召回奖励记录(已领取的)
		for (WORD i = 0; i < tFacebookActivityData.wSummonRewardRecordNum; ++i)
		{
			pPBFacebookActivityData->add_recall_reward_record_list(tFacebookActivityData.arSummonRewardRecord[i]);
		}
		// 邀请奖励记录(已领取的)
		for (WORD i = 0; i < tFacebookActivityData.wInviteRewardRecordNum; ++i)
		{
			pPBFacebookActivityData->add_invite_reward_record_list(tFacebookActivityData.arInviteRewardRecord[i]);
		}
		// 已发出召回的好友记录(游戏内好友)
		for (WORD i = 0; i < tFacebookActivityData.wSummonFriendRecordNum; ++i)
		{
			const RecallFriendData &tRecallFriendData = tFacebookActivityData.arSummonFriendRecord[i];
			::PB::RecallFriendData *pPBRecallFriendData = pPBFacebookActivityData->add_send_recall_friend_record();
			pPBRecallFriendData->set_friend_id(tRecallFriendData.dwFriendId);
			pPBRecallFriendData->set_send_time(tRecallFriendData.dwTime);
		}
		// 已发出邀请的好友记录(facebook好友)
		const FacebookFriendInfo &tSendInviteFiendInfo = tFacebookActivityData.tSendInviteFriendInfo;
		for (WORD i = 0; i < tSendInviteFiendInfo.wFriendNum; ++i)
		{
			pPBFacebookActivityData->add_send_invite_friend_record(tSendInviteFiendInfo.arFriendList[i]);
		}
	}

	// 在线奖励数据
	{
		const OnlineRewardData &tOnlineRewardData = tActivityInfo.tOnlineRewardData;
		::PB::OnlineRewardData *pPBOnlineRewardData = tPBActivityInfo.mutable_online_reward_data();
		pPBOnlineRewardData->set_online_time(tOnlineRewardData.dwOnlineTime); // 在线时间
		for (int i = 0; i < MAX_ONLINE_REWARD_DAY; ++i)
		{
			::PB::OnlineRewardRecord *pPBRewardRecordList = pPBOnlineRewardData->add_reward_record_list();
			pPBRewardRecordList->set_days(i + 1); // 天数
			const WORD *pRewardRecordList = tOnlineRewardData.arRewardRewardList[i];
			for (int k = 0; k < MAX_ONLINE_REWARD_TIME_LIST; k++)
			{
				pPBRewardRecordList->add_flag_list(pRewardRecordList[k]);
			}
		}
	}
	// 7天登录数据
	{
		const SevenDayLoginData &tSevenDayLoginData = tActivityInfo.tSevenDayLoginData;
		::PB::SevenDayLoginData *pPBSevenDayLoginData = tPBActivityInfo.mutable_seven_day_login_data();
		for (int i = 0; i < MAX_SEVEN_DAY_LOGIN_DAYS; ++i)
		{
			pPBSevenDayLoginData->add_daily_data_list(tSevenDayLoginData.arDailyDataList[i]);
		}
	}
	// 充值回馈数据
	{
		const RecahrgeRewardData &tRechargeRewardData = tActivityInfo.tRechargeRewardData;
		::PB::RechargeRewardData *pPBRecahrgeRewardData = tPBActivityInfo.mutable_recharge_reward_data();
		pPBRecahrgeRewardData->set_is_took_extra_reward(tRechargeRewardData.wIsTookExtraReward);
		pPBRecahrgeRewardData->set_last_activity_data_id(tRechargeRewardData.wLastActivityDataId);
		for (int i = 0; i < MAX_RECHARGE_REWARD_DAY_NUM; ++i)
		{
			pPBRecahrgeRewardData->add_daily_data_list(tRechargeRewardData.arDailyDataList[i]);
		}
	}
	// 累充奖励
	{
		const TotalRechargeRewardData &tTotalRechargeRewardData = tActivityInfo.tTotalRechargeRewardData;
		::PB::TotalRechargeRewardData *pPBTotalRechargeRewardData = tPBActivityInfo.mutable_total_recharge_reward_data();
		pPBTotalRechargeRewardData->set_total_amount(tTotalRechargeRewardData.dwTotalAmount);
		pPBTotalRechargeRewardData->set_last_activity_data_id(tTotalRechargeRewardData.wLastActivityDataId);
		for (WORD i = 0; i < tTotalRechargeRewardData.wRewardRecordNum; ++i)
		{
			pPBTotalRechargeRewardData->add_reward_record_list(tTotalRechargeRewardData.arRewardRecordList[i]);
		}

	}
	// 热卖商品
	{
		const PlayerHotSaleGoodsInfo &tPlayerHotSaleGoodsInfo = tActivityInfo.tPlayerHotSaleGoodsInfo;
		::PB::RoleHotSaleGoodsData *tPBPlayerHotSaleGoodsInfo = tPBActivityInfo.mutable_role_hot_sale_goods();
		tPBPlayerHotSaleGoodsInfo->set_last_activity_data_id(tPlayerHotSaleGoodsInfo.wLastActivityDataId);
		for (WORD i = 0; i < tPlayerHotSaleGoodsInfo.wGoodsNum; ++i)
		{
			const HotSaleGoods &tGoods = tPlayerHotSaleGoodsInfo.arGoodsList[i];
			::PB::HotSaleGoods *pPBGoods = tPBPlayerHotSaleGoodsInfo->add_goods_list();
			pPBGoods->set_goods_id(tGoods.wGoodsId);
			pPBGoods->set_cur_data_idx(tGoods.wCurDataIdx);
			pPBGoods->set_buy_times(tGoods.wBuyTimes);
			pPBGoods->set_take_times(tGoods.wTakeTimes);
		}
	}
	// 阵营招募
	{
		const PlayerCampRecruitData &tPlayerCampRecruitData = tActivityInfo.tPlayerCampRecruitData;
		::PB::RoleCampRecruitData *tPBRoleCampRecruitData = tPBActivityInfo.mutable_role_camp_recruit_data();
		tPBRoleCampRecruitData->set_cur_data_id(tPlayerCampRecruitData.wCurDataId);
		tPBRoleCampRecruitData->set_today_already_recruit_times(tPlayerCampRecruitData.wTodayAlreadyRecruitTimes);
		for (WORD i = 0; i < CAMP_NUM; ++i)
		{
			tPBRoleCampRecruitData->add_camp_score_list(tPlayerCampRecruitData.arCampScoreList[i]);
		}
		for (WORD i = 0; i < tPlayerCampRecruitData.wScoreTaskRewardRecordNum; ++i)
		{
			tPBRoleCampRecruitData->add_score_task_reward_record_list(tPlayerCampRecruitData.arScoreTaskRewardRecordList[i]);
		}
	}
	// unlock活动
	{
		const UnlockActivityData &tRoleUnlockActivityData = tActivityInfo.tUnlockActivityData;
		::PB::UnlockActivityData *tPBRoleUnlockActivityData = tPBActivityInfo.mutable_role_unlock_activity_data();
		tPBRoleUnlockActivityData->set_is_score_for_shop(tRoleUnlockActivityData.wIsScoreForShop);
		tPBRoleUnlockActivityData->set_watching_video_times(tRoleUnlockActivityData.wWatchingVideosTimes);
	}
	// 充值与消费排行榜活动
	{
		const RechargeAndCostDiamondRankData &tRechargeAndCostDiamondRankData = tActivityInfo.tRechargeAndCostDiamondRankData;
		::PB::RechargeAndCostRankData *tPBRechargeAndCostDiamondRankData = tPBActivityInfo.mutable_role_recharge_and_cost_rank_data();
		tPBRechargeAndCostDiamondRankData->set_last_activity_data_id(tRechargeAndCostDiamondRankData.wLastActivityDataId);
		tPBRechargeAndCostDiamondRankData->set_cost_diamond(tRechargeAndCostDiamondRankData.dwCostDiamond);
		tPBRechargeAndCostDiamondRankData->set_recharge_amount(tRechargeAndCostDiamondRankData.dwRechargeAmount);
	}

	// 临时数据
	PB::RoleTempDatas PBRoleTempDatas;
	RoleTempDatas &tRoleTempDatas = tFullInfo.tTempDatas;
	for (WORD i = 0; i < tRoleTempDatas.wTempDataNum; ++i)
	{
		RoleTempData &tTempData = tRoleTempDatas.arTempDataList[i];
		PB::RoleTempData *tPBTempData = PBRoleTempDatas.add_temp_data_list();
		tPBTempData->set_type(tTempData.wType);
		tPBTempData->set_text(tTempData.szText);
		for (WORD k = 0; k < tTempData.wParamNum; ++k)
		{
			tPBTempData->add_param_list(tTempData.arParamList[k]);
		}
	}

	// 基本信息
	string strBaseInfoData("");
	if (!PBRoleBaseInfo.AppendToString(&strBaseInfoData) || strBaseInfoData.size() > MAX_ROLE_BASE_INFO_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色基本数据系列化后过大[size:%d]", __FUNCTION__, strBaseInfoData.size());
		return;
	}

	// 邮件
	string strEmailInfoData("");
	if (!PBEmailInfo.AppendToString(&strEmailInfoData) || strEmailInfoData.size() > MAX_ROLE_EMAIL_INFO_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色邮件数据系列化后过大[size:%d]", __FUNCTION__, strEmailInfoData.size());
		return;
	}

	
	// 英雄
	string strHeroInfoData("");
	if (!tPBHeroInfo.AppendToString(&strHeroInfoData) || strHeroInfoData.size() > MAX_ROLE_HERO_INFO_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色英雄数据系列化后过大[size:%d]", __FUNCTION__, strHeroInfoData.size());
		return;
	}

	// 好友
	string strFriendInfoData("");
	if (!tPBFriendInfo.AppendToString(&strFriendInfoData) || strFriendInfoData.size() > MAX_ROLE_FRIEND_INFO_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色好友数据系列化后过大[size:%d]", __FUNCTION__, strFriendInfoData.size());
		return;
	}

	// 二级信息
	string strSecondInfoData("");
	if (!tPBSecondInfo.AppendToString(&strSecondInfoData) || strSecondInfoData.size() > MAX_ROLE_SECOND_INFO_SIZE_EX)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色二级数据系列化后过大[size:%d]", __FUNCTION__, strSecondInfoData.size());
		return;
	}

	// 奖励数据
	string strRewardData("");
	if (!tPBRewardInfo.AppendToString(&strRewardData) || strRewardData.size() > MAX_ROLE_REWARD_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色奖励数据系列化后过大[size:%d]", __FUNCTION__, strRewardData.size());
		return;
	}

	// 活动数据
	string strActivityData("");
	if (!tPBActivityInfo.AppendToString(&strActivityData) || strActivityData.size() > MAX_ACTIVITY_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色活动数据系列化后过大[size:%d]", __FUNCTION__, strActivityData.size());
		return;
	}

	// 临时数据
	string strTempData("");
	if (!PBRoleTempDatas.AppendToString(&strTempData) || strTempData.size() > MAX_ROLE_TEMP_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色临时数据系列化后过大[size:%d]", __FUNCTION__, strTempData.size());
		return;
	}

	DWORD dwTotalDataSize = 0;

	// 基本信息
	char szBaseInfo[MAX_ROLE_BASE_INFO_SIZE * 2 + 1] = { 0 };
	int nReturnSize = pSqlDatabase->escape_string(szBaseInfo, strBaseInfoData.c_str(), strBaseInfoData.size());
	dwTotalDataSize += nReturnSize;

	// 二级信息
	char szSecondInfo[MAX_ROLE_SECOND_INFO_SIZE_EX * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szSecondInfo, strSecondInfoData.c_str(), strSecondInfoData.size());
	dwTotalDataSize += nReturnSize;

	// 英雄
	char szHeroInfo[MAX_ROLE_HERO_INFO_SIZE * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szHeroInfo, strHeroInfoData.c_str(), strHeroInfoData.size());
	dwTotalDataSize += nReturnSize;

	// 邮件
	char szEmailInfo[MAX_ROLE_EMAIL_INFO_SIZE * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szEmailInfo, strEmailInfoData.c_str(), strEmailInfoData.size());
	dwTotalDataSize += nReturnSize;

	// 好友
	char szFriendInfo[MAX_ROLE_FRIEND_INFO_SIZE * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szFriendInfo, strFriendInfoData.c_str(), strFriendInfoData.size());
	dwTotalDataSize += nReturnSize;

	// 奖励
	char szRewardInfo[MAX_ROLE_REWARD_DATA_SIZE * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szRewardInfo, strRewardData.c_str(), strRewardData.size());
	dwTotalDataSize += nReturnSize;

	// 活动
	char szActivityInfo[MAX_ACTIVITY_DATA_SIZE * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szActivityInfo, strActivityData.c_str(), strActivityData.size());
	dwTotalDataSize += nReturnSize;

	// 临时数据
	char szTempData[MAX_ROLE_TEMP_DATA_SIZE * 2 + 1] = { 0 };
	nReturnSize = pSqlDatabase->escape_string(szTempData, strTempData.c_str(), strTempData.size());
	dwTotalDataSize += nReturnSize;
	
	char szUpdateTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szUpdateTime);

	MainFBData &tMainFBData = tFullInfo.tFubenInfo.tMainFubenData;
	BYTE byMapIdx = tMainFBData.wChapter ? tMainFBData.arChapterList[tMainFBData.wChapter - 1].byMapIdx : 0;
	WORD wHeroFBChapter = tFullInfo.tFubenInfo.tHeroFubenData.wChapter;
	WORD wEliteFBChapter = tFullInfo.tFubenInfo.tEliteFBData.wChapter;
	WORD wTowerMapLevel = tFullInfo.tFubenInfo.tTowerFubenData.wMapLevelRecord;
	WORD wHeartOfWorldStage = tFullInfo.tSpiritInfo.tHeartOfWorld.wStage;
	WORD wArenaHistroyRank = tRoleInfo.wArenaHistroyRank;
	WORD wLandNum = tRoleInfo.wLandNum;

	// vip_level, gold, fb_chapter, fb_map, item_num, equipment_num, treasure_num, hero_num, hero_fb_chapter, elite_fb_chapter, 
	// tower_map_level, land_num, heart_of_world_stage, arena_rank

	char szCharName[2 * (MAX_ROLE_LEN + 1) + 1] = { 0 };
	pSqlDatabase->escape_string(szCharName, tRoleInfo.szRoleName, strlen(tRoleInfo.szRoleName));

 	sprintf(pSqlDatabase->GetQuery(), "Update role Set role_name='%s',head_image_id=%d,level=%d,\
		fighting_power=%d,login_times=%d,total_online_time=%d,last_update_time='%s',\
		base_info='%s',hero_info='%s',email_info='%s',friend_info='%s',reward_info='%s',activity_info='%s',second_info='%s',\
		temp_data='%s',guide_x=%d, guide_y=%d,update_times=%d,total_recharge_amount=%d,vip_level=%d, gold=%d, fb_chapter=%d, fb_map=%d, item_num=%d, \
		equipment_num=%d, treasure_num=%d, hero_num=%d,card_num=%d,hero_fb_chapter=%d, elite_fb_chapter=%d,tower_map_level=%d, land_num=%d,\
		heart_of_world_stage=%d, arena_rank=%d, diamond=%d Where role_id=%lu"
		, szCharName, tRoleInfo.dwHeadImage, tRoleInfo.wLevel
		, tRoleInfo.dwFightingPower, tRoleInfo.dwLoginTimes, tRoleInfo.dwOnlineTime, szUpdateTime
		, szBaseInfo, szHeroInfo, szEmailInfo, szFriendInfo, szRewardInfo, szActivityInfo, szSecondInfo, szTempData
		, tRoleInfo.nGuideX, tRoleInfo.nGuideY, tRoleInfo.dwUpdateTimes, tRoleInfo.dwTotalRechargeAmount, tRoleInfo.sVipData.wLevel
		, tRoleInfo.dwGold, tMainFBData.wChapter, byMapIdx, tFullInfo.tItemInfo.wListNum, tFullInfo.tEquipmentInfo.wEquipmentNum, 
		tFullInfo.tTreasureInfo.wTreasureNum, tFullInfo.tHeroInfo.wHeroNum, tFullInfo.tZhanBuInfo.wCardNum, wHeroFBChapter, wEliteFBChapter, 
		wTowerMapLevel, wLandNum,wHeartOfWorldStage, wArenaHistroyRank, tRoleInfo.dwDiamond, dwRoleId);
 	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	if(nRet == -1)
 	{
		MAPLOG(ERROR_LEVEL, "角色信息系列化后数据大小[role_id:%d,data_size:%d]", dwRoleId, dwTotalDataSize);
 		pQuery->SetQueryResultString(pSqlDatabase->error());
 		pQuery->SetQueryResultTable("update Role");
 		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
 		return;
 	}

	if (dwTotalDataSize >= 80000)
		MAPLOG(ERROR_LEVEL, "角色信息系列化后数据大小[role_id:%d,data_size:%d]", dwRoleId, dwTotalDataSize);

	// todo::加入日志方便查找数据会被写坏的情况
	if (!tRoleInfo.wLevel || !tRoleInfo.dwRoleId)
	{
		MAPLOG(ERROR_LEVEL, "[RoleId:%d,SaveType:%d]角色数据已出现异常:新[id:%d][lv:%d][name:%s] 旧[id:%d][lv:%d][name:%s][account:%s]", dwRoleId, 
			pQuery->GetSaveType(),tRoleInfo.dwRoleId, tRoleInfo.wLevel, tRoleInfo.szRoleName,  dwRoleIdBefore, wRoleLevelBefore, szRoleName, szAccount);
	}

	UINT64 lUsedTickCount = GetTickCount64() - lCurTickCount;
	if (lUsedTickCount)
	{
		MAPLOG(GUI_LEVEL, "角色信息系列化消耗[%d]", lUsedTickCount);
	}

	// 下线
	if (SAVE_DB_TYPE_LOGOUT == pQuery->GetSaveType())
	{
		DWORD dwCurTime = time(NULL);
		DWORD dwOnlineTime = dwCurTime - tRoleInfo.dwLastLoginTime;

		// 插入数据库
		RoleOfflineRecord_Save* pQuery = RoleOfflineRecord_Save::ALLOC();
		pQuery->SetIndex(DB_ROLE_OFFLINE_RECORD_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		RoleOfflineRecord tOfflineRecord;
		tOfflineRecord.dwRoleId = dwRoleId;
		tOfflineRecord.wLevel = tRoleInfo.wLevel;
		tOfflineRecord.wVipLevel = tRoleInfo.sVipData.wLevel;
		tOfflineRecord.dwDiamond = tRoleInfo.dwDiamond;
		tOfflineRecord.dwGold = tRoleInfo.dwGold;
		tOfflineRecord.wServerId = dwServerId;
		tOfflineRecord.dwOfflineTime = dwCurTime;
		tOfflineRecord.wMainFBChapter = tMainFBData.wChapter;
		tOfflineRecord.wMainFBMap = byMapIdx;
		tOfflineRecord.nGuideX = tRoleInfo.nGuideX;
		tOfflineRecord.nGuideY = tRoleInfo.nGuideY;
		tOfflineRecord.dwOnlineTime = dwOnlineTime;
		pQuery->SetRecord(tOfflineRecord);
		g_MapServer.MapDBQuery(pQuery);
	}

	//LEAVE_DUMP_FUNCTION
}

// 帐号密码修改
void DBThreadHandler::ProcessAccountPasswordModify(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	AccountPasswordModify* pQuery = (AccountPasswordModify*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "Update account Set password='%s' Where account='%s'", pQuery->GetPassword(), pQuery->GetAccID());
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("Update account set password");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 手机号修改
void DBThreadHandler::ProcessAccountMobileChange(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	AccountMobileModify* pQuery = (AccountMobileModify*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "Update account Set mobile='%s' Where account='%s'", pQuery->GetMobile(), pQuery->GetAccID());
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("Update account set mobile");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pSqlDatabase->CloseRecordset();
}

// 更新最近登录的服务器
void DBThreadHandler::DoUpateLastLoginServer(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	UpdateLastLoginServer* pQuery = (UpdateLastLoginServer*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	unsigned char szAccout[2 * (MAX_ACCOUNT_LEN + 1) + 1] = { 0 };
	unsigned int nAccountLen = pSqlDatabase->escape_string((char*)szAccout, pQuery->GetAccID(), strlen(pQuery->GetAccID()));
	szAccout[nAccountLen] = '\0';

	DWORD arLastLoginServerIdList[MAX_LOGIN_SERVER_LIST] = { 0 };

	//处理SQL查询
	sprintf(pSqlDatabase->GetQuery(), "Select last_login_zoneid,last_login_zoneid_2,last_login_zoneid_3,last_login_zoneid_4\
					From account Where account='%s' COLLATE utf8_bin and platform=%d", szAccout, pQuery->GetChannelType());
	int nRet = pSqlDatabase->OpenRecordset();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (-1 != nRet)
	{
		if (pSqlDatabase->Fetch())
		{
			int i = 0;
			pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[0]);
			pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[1]);
			pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[2]);
			pSqlDatabase->GetDWORD(i++, arLastLoginServerIdList[3]);
		}
		pQuery->SetResultRowNum(nRet);
	}
	pSqlDatabase->CloseRecordset();

	// 重新排序列表(记录列表依次往后退,最后一个会丢失,空出第一个位置给新登录的)
	int nDataSize = sizeof(DWORD)* (MAX_LOGIN_SERVER_LIST - 1);
	for (int i = 0; i < MAX_LOGIN_SERVER_LIST; ++i)
	{
		if (arLastLoginServerIdList[i] == pQuery->GetServerID())
		{
			nDataSize = sizeof(DWORD) * i;
			break;
		}
	}
	memcpy_s(&arLastLoginServerIdList[1], nDataSize, arLastLoginServerIdList, nDataSize);
	arLastLoginServerIdList[0] = pQuery->GetServerID();

	// 更新
	sprintf(pSqlDatabase->GetQuery(), "Update account Set last_login_zoneid=%d,last_login_zoneid_2=%d,\
	last_login_zoneid_3=%d,last_login_zoneid_4=%d Where account='%s' COLLATE utf8_bin and platform=%d", arLastLoginServerIdList[0], 
	arLastLoginServerIdList[1], arLastLoginServerIdList[2], arLastLoginServerIdList[3], pQuery->GetAccID(), pQuery->GetChannelType());
	nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("Update Last Login Server");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pSqlDatabase->CloseRecordset();

	// todo::测试代码
// 	DWORD dwCostTime = GetTickCount() - dwCurTime;
// 	MAPLOG(IMPORTANT_LEVEL, "DoUpateLastLoginServer[%d]", dwCostTime);
}

// 存储全局数据
void DBThreadHandler::SaveGolbalData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ENTER_DUMP_FUNCTION

	if(!pSqlDatabase || !pData)
		return;

	ServerGolbalData_Save* pQuery = (ServerGolbalData_Save*)pData;

	if(!pSqlDatabase->GetQuery())
		return;

	const ServerGolbalData *pGolbalDataList = pQuery->GetGolbalDataList();
	BYTE byGolbalDataNum = pQuery->GetGolbalDataNum();

	for (BYTE i = 0; i < byGolbalDataNum; ++i)
	{
		sprintf(pSqlDatabase->GetQuery(), "Replace Into server_golbal_data(server_id,data_key,data_value) VALUES(%u, %u, %u)", 
			pGolbalDataList[i].wServerId, pGolbalDataList[i].nKey, pGolbalDataList[i].dwValue);
		int nRet = pSqlDatabase->Execute();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
	}

	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();

	LEAVE_DUMP_FUNCTION
}

// 加载全局数据
void DBThreadHandler::LoadGolbalData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ENTER_DUMP_FUNCTION

	if(!pSqlDatabase || !pData)
		return;

	ServerGolbalData_Load* pQuery = (ServerGolbalData_Load*)pData;

	if(!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "Select server_id,data_key,data_value From server_golbal_data");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	if (nRet >= MAX_GOLBAL_DATA_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]全局数据列表数量超过上限[num:%d]!", __FUNCTION__, nRet);
	}

	WORD wServerId;
	int nKey;
	DWORD dwValue;
	while(pSqlDatabase->Fetch())
	{
		pSqlDatabase->GetWORD(0, wServerId);
		pSqlDatabase->GetInt(1, nKey);
		pSqlDatabase->GetDWORD(2, dwValue);
		pQuery->AddGolbalData(wServerId, nKey, dwValue);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();

	LEAVE_DUMP_FUNCTION
}

// 加载角色基本数据列表
void DBThreadHandler::LoadRoleBaseDataList(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleBaseDataList_Load* pQuery = (RoleBaseDataList_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 如果是第一次,则读取列表数量
	if (!pQuery->GetTotalDataNum())
	{
		sprintf(pSqlDatabase->GetQuery(), "select count(*) from role");
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
		DWORD dwCount = 0;
		while (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetDWORD(0, dwCount);
		}
		pQuery->SetTotalDataNum(dwCount);
	}

	DWORD dwMinRoleId = pQuery->GetMinRoleId();

	sprintf(pSqlDatabase->GetQuery(), "select role_id, channel_uid, server_id, base_info, hero_info,second_info from role\
									   where role_id > %d order by role_id limit 0,%d", dwMinRoleId, ROLE_BASE_DATA_LOAD_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 基本信息
	char szBaseInfo[MAX_ROLE_BASE_INFO_SIZE * 2 + 1] = { 0 };
	int nBaseInfoSize = 0;
	// 英雄
	char szHeroInfo[MAX_ROLE_HERO_INFO_SIZE * 2 + 1] = { 0 };
	int nHeroInfoSize = 0;
	// 二级信息
	char szSecondInfo[MAX_ROLE_SECOND_INFO_SIZE_EX * 2 + 1] = { 0 };
	int nSecondInfoSize = 0;


	DATETIME logoutTime;
	DBPlayerBaseData tBaseData;
	memset(&tBaseData, 0, sizeof(tBaseData));
	char szLastLogoutTime[32] = { 0 };
	//memset(tBaseData.szGuildName, 0, sizeof(tBaseData.szGuildName));
	DWORD dwRoleId = 0;
	WORD wServerId;
	DWORD dwChannelUID;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetDWORD(i++, dwRoleId);
		pSqlDatabase->GetDWORD(i++, dwChannelUID);
		pSqlDatabase->GetWORD(i++, wServerId);
		nBaseInfoSize = pSqlDatabase->GetBlob(i++, (unsigned char*)szBaseInfo);
		nHeroInfoSize = pSqlDatabase->GetBlob(i++, (unsigned char*)szHeroInfo);
		nSecondInfoSize = pSqlDatabase->GetBlob(i++, (unsigned char*)szSecondInfo);
		PB::RoleBaseInfo PBRoleBaseInfo;
		if (!PBRoleBaseInfo.ParseFromArray(szBaseInfo, nBaseInfoSize))
		{
			MAPLOG(ERROR_LEVEL, "[%s]反系列化角色基本数据失败!", __FUNCTION__);
			return;
		}
		PB::RoleHeroInfo PBRoleHeroInfo;
		if (nHeroInfoSize && !PBRoleHeroInfo.ParseFromArray(szHeroInfo, nHeroInfoSize))
		{
			MAPLOG(ERROR_LEVEL, "[%s]反系列化角色英雄数据失败!", __FUNCTION__);
			return;
		}
		PB::RoleSecondInfo PBRoleSecondInfo;
		if (nSecondInfoSize && !PBRoleSecondInfo.ParseFromArray(szSecondInfo, nSecondInfoSize))
		{
			MAPLOG(ERROR_LEVEL, "[%s]反系列化角色二级数据失败!", __FUNCTION__);
			return;
		}

		// 基本数据
		tBaseData.dwRoleId = dwRoleId;
		tBaseData.dwPlatform = dwChannelUID;
		tBaseData.wServerId = wServerId;
		tBaseData.wLevel = PBRoleBaseInfo.level();
		tBaseData.wHeadIcon = PBRoleBaseInfo.head_image_id();
		tBaseData.dwFightingPower = PBRoleBaseInfo.fighting_power();
		tBaseData.byVipLevel = PBRoleBaseInfo.vip_level();
		tBaseData.dwOfflineTime = PBRoleBaseInfo.last_logout_time_ex();
		strcpy_s(tBaseData.szAccount, sizeof(tBaseData.szAccount) - 1, PBRoleBaseInfo.account().c_str());
		strcpy_s(tBaseData.szRoleName, sizeof(tBaseData.szRoleName) - 1, PBRoleBaseInfo.role_name().c_str());
		strcpy_s(szLastLogoutTime, sizeof(szLastLogoutTime)-1, PBRoleBaseInfo.last_logout_time().c_str());
		// 下线时间(todo::旧数据兼容)
		if (!tBaseData.dwOfflineTime && 0 == logoutTime.ParseFromString(szLastLogoutTime))
			tBaseData.dwOfflineTime = (time_t)logoutTime;

		// 二级信息
		const ::PB::RoleTitleInfo &tPBTitleInfo = PBRoleSecondInfo.title_info();
		tBaseData.dwTitleId = tPBTitleInfo.cur_use_title_id();
		tBaseData.dwNicknameId = tPBTitleInfo.cur_use_nickname_id();

		// 上阵数据
		tBaseData.byToBattleHeroNum = 0;
		if (nHeroInfoSize)
		{
			for (int i = 0; i < PBRoleHeroInfo.formation_list_size(); ++i)
			{
				const PB::Formation &tPBFormation = PBRoleHeroInfo.formation_list(i);
				// 如果阵型上木有英雄则不处理
				if (!tPBFormation.hero_id())
					continue;

				// 阵型槽位上的属性列表
				const PB::FormationProperty &tPBFormationPro = PBRoleBaseInfo.formation_pro_list(i);

				for (int k = 0; k < PBRoleHeroInfo.hero_list_size(); ++k)
				{
					// 查找槽位上英雄的数据
					const ::PB::Hero &tPBHero = PBRoleHeroInfo.hero_list(k);
					if (tPBHero.uid() == tPBFormation.hero_id())
					{
						ToBattleHeroData &tToBattleHero = tBaseData.arToBattleHeroList[tBaseData.byToBattleHeroNum++];
						// 取槽位上英雄的基本属性
						tToBattleHero.byProNum = 0;
						tToBattleHero.byPos = i;
						tToBattleHero.wHeroInfoId = tPBHero.info_id();
						tToBattleHero.byQuality = tPBHero.quality();
						tToBattleHero.wLevel = tPBHero.level();
						// 取槽位上英雄的属性列表
						for (int m = 0; m < tPBFormationPro.pro_list_size(); ++m)
						{
							const PB::Property &tPBPro = tPBFormationPro.pro_list(m);
							Property &tPro = tToBattleHero.arPorList[tToBattleHero.byProNum++];
							tPro.wType = tPBPro.type();
							tPro.nValue = tPBPro.value();
						}
						break;
					}
				}
			}
		}
		pQuery->AddData(tBaseData);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 更新好友数据
void DBThreadHandler::UpdateFriendData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	FriendDataUpdate_Load* pQuery = (FriendDataUpdate_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwUpdateRoleId = pQuery->GetUpdateRoleId();

	sprintf(pSqlDatabase->GetQuery(), "select friend_info from role where role_id=%d", dwUpdateRoleId);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	PlayerFriendData sPlayerFriendData;
	memset(&sPlayerFriendData, 0, sizeof(sPlayerFriendData));
	char szFriendInfo[MAX_ROLE_FRIEND_INFO_SIZE * 2 + 1] = {0};
	int nFiendInfoSize = 0;
	if(pSqlDatabase->Fetch())
	{
		nFiendInfoSize = pSqlDatabase->GetBlob(0, (unsigned char*)&szFriendInfo);
	}
	PB::RoleFriendData PBRoleFriendInfo;
	if (nFiendInfoSize && PBRoleFriendInfo.ParseFromArray(szFriendInfo, nFiendInfoSize))
	{
		sPlayerFriendData.wFriendNum = 0;
		for (int i = 0; i < PBRoleFriendInfo.friend_list_size(); ++i)
		{
			FriendData &tFriend = sPlayerFriendData.arFriendList[sPlayerFriendData.wFriendNum++];
			const PB::FriendData &tPBFriend = PBRoleFriendInfo.friend_list(i);
			tFriend.dwFriendId = tPBFriend.role_id();
			tFriend.wIsGiveEnergy = tPBFriend.is_give_energy();
			tFriend.wTakeEnergyState = tPBFriend.get_energy_state();
		}
		sPlayerFriendData.wApplyNum = 0;
		for (int i = 0; i < PBRoleFriendInfo.apply_list_size(); ++i)
		{
			sPlayerFriendData.arApplyList[sPlayerFriendData.wApplyNum++] = PBRoleFriendInfo.apply_list(i);
		}
	}
	else if (nFiendInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色好友数据失败!", __FUNCTION__);
	}

	// 添加 
	if (FRIEND_UPDATE_ADD_FRIEND == pQuery->GetOperateType())
	{
		if (sPlayerFriendData.wFriendNum < MAX_FRIEND_NUM)
			sPlayerFriendData.arFriendList[sPlayerFriendData.wFriendNum++] = pQuery->GetFriendData();

		// 如果新的好友在申请列表里,则拿掉
		for(WORD i = 0; i < sPlayerFriendData.wApplyNum; ++i)
		{
			if (sPlayerFriendData.arApplyList[i] == pQuery->GetFriendData().dwFriendId)
			{
				sPlayerFriendData.arApplyList[i] = sPlayerFriendData.arApplyList[--sPlayerFriendData.wApplyNum];
				break;
			}
		}
	}
	// 删除
	else
	{
		for (BYTE i = 0; i < sPlayerFriendData.wFriendNum; ++i)
		{
			if (sPlayerFriendData.arFriendList[i].dwFriendId == pQuery->GetFriendData().dwFriendId)
			{
				sPlayerFriendData.arFriendList[i] = sPlayerFriendData.arFriendList[--sPlayerFriendData.wFriendNum];
				break;
			}
		}
	}

	PBRoleFriendInfo.clear_friend_list();
	for (WORD i = 0; i < sPlayerFriendData.wFriendNum; ++i)
	{
		FriendData &tFriend = sPlayerFriendData.arFriendList[i];
		PB::FriendData *tPBFriend = PBRoleFriendInfo.add_friend_list();
		tPBFriend->set_role_id(tFriend.dwFriendId);
		tPBFriend->set_is_give_energy(tFriend.wIsGiveEnergy);
		tPBFriend->set_get_energy_state(tFriend.wTakeEnergyState);
	}
	PBRoleFriendInfo.clear_apply_list();
	for (WORD i = 0; i < sPlayerFriendData.wApplyNum; ++i)
	{
		PBRoleFriendInfo.add_apply_list(sPlayerFriendData.arApplyList[i]);
	}

	string strFriendInfoData("");
	if (!PBRoleFriendInfo.AppendToString(&strFriendInfoData) || strFriendInfoData.size() > MAX_ROLE_FRIEND_INFO_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色好友数据系列化后过大[size:%d]", __FUNCTION__, strFriendInfoData.size());
		return;
	}

	// 好友
	memset(szFriendInfo, 0, sizeof(szFriendInfo));
	pSqlDatabase->escape_string(szFriendInfo, strFriendInfoData.c_str(), strFriendInfoData.size());

	// 重新更新
	sprintf(pSqlDatabase->GetQuery(), "Update role Set friend_info='%s' Where role_id='%d'", szFriendInfo, dwUpdateRoleId);
	nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("UpdateFriendData");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储竞技场数据
void DBThreadHandler::SaveArenaData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ArenaDataList_Save* pQuery = (ArenaDataList_Save*)pData;


	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwServerId = pQuery->GetServerId();

	char szBuff[MAX_ARENA_LIST_DATA_SIZE * 2 +1] = { 0 };
	int nFullInfoSize = sizeof(PlayerFriendData);
	unsigned int nBuffLen = pSqlDatabase->escape_string((char*)szBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());
	szBuff[nBuffLen] = '\0';

	sprintf(pSqlDatabase->GetQuery(), "Replace Into arena(server_id,data_size, arena_data) VALUES(%u, %u, '%s')", 
		dwServerId, pQuery->GetDataBuffSize(), szBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载竞技场数据
void DBThreadHandler::LoadArenaData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ArenaDataList_Load* pQuery = (ArenaDataList_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

 	sprintf(pSqlDatabase->GetQuery(), "select data_size,arena_data from arena where server_id=%d", pQuery->GetServerId());
 	int nRet = pSqlDatabase->OpenRecordset();
 	if (-1 == nRet)
 	{
 		pQuery->SetQueryResult(pSqlDatabase->errorno());
 		return;
 	}
 
	char szDataBuff[MAX_ARENA_LIST_DATA_SIZE] = { 0 };
	int nDataBuffSize = 0;
 	while (pSqlDatabase->Fetch())
 	{
 		int i = 0;
		pSqlDatabase->GetInt(i++, nDataBuffSize);
		pSqlDatabase->GetBlob(i++, (unsigned char*)&szDataBuff, nDataBuffSize);
 	}

	pQuery->SetDataBuff(szDataBuff, nDataBuffSize);
 
 	pQuery->SetResultRowNum(nRet);
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	pSqlDatabase->CloseRecordset();
}

// 存储排行榜数据
void DBThreadHandler::SaveRoleRankData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleRankList_Save* pQuery = (RoleRankList_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	BYTE byRankType = pQuery->GetRankType();

	char *pTempBuff = pQuery->GetTempBuff();

	//char szBuff[MAX_ROLE_RANK_LIST_DATA_SIZE * 2 + 1] = { 0 };
	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());
	//szBuff[nBuffLen] = '\0';

	sprintf(pSqlDatabase->GetQuery(), "Replace Into role_rank(rank_type,size, data) VALUES(%u, %u, '%s')",
		byRankType, pQuery->GetDataBuffSize(), pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载排行榜数据
void DBThreadHandler::LoadRoleRankData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleRankList_Load* pQuery = (RoleRankList_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "select size,data from role_rank where rank_type=%d", pQuery->GetRankType());
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	char szDataBuff[MAX_ROLE_RANK_LIST_DATA_SIZE] = { 0 };
	int nDataBuffSize = 0;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetInt(i++, nDataBuffSize);
		pSqlDatabase->GetBlob(i++, (unsigned char*)&szDataBuff, nDataBuffSize);
	}

	pQuery->SetDataBuff(szDataBuff, nDataBuffSize);

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储邮件数据
void DBThreadHandler::SaveEmailData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	InsertEmailQuery* pQuery = (InsertEmailQuery*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwRoleId = pQuery->GetKey();
	sprintf(pSqlDatabase->GetQuery(), "select email_info from role where role_id=%d", dwRoleId);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 取系列化后的数据
	PlayerEmailInfo tPlayerEmailData;
	memset(&tPlayerEmailData, 0, sizeof(tPlayerEmailData));
	char szEmailInfo[MAX_ROLE_EMAIL_INFO_SIZE * 2 + 1] = { 0 };
	int nEmailInfoSize = 0;
	if (pSqlDatabase->Fetch())
	{
		nEmailInfoSize = pSqlDatabase->GetBlob(0, (unsigned char*)&szEmailInfo);
	}

	PB::RoleEmailInfo PBRoleEmailInfo;
	if (nEmailInfoSize && PBRoleEmailInfo.ParseFromArray(szEmailInfo, nEmailInfoSize))
	{
		tPlayerEmailData.wEmailCount = 0;
		for (int i = 0; i < PBRoleEmailInfo.email_list_size(); ++i)
		{
			EmailInfo &tEmail = tPlayerEmailData.arEmailList[tPlayerEmailData.wEmailCount++];
			const PB::EmailInfo &tPBEmail = PBRoleEmailInfo.email_list(i);
			tEmail.dwID = tPBEmail.uid();
			tEmail.wInfoId = tPBEmail.info_id();
			tEmail.dwReceiveTime = tPBEmail.receive_time();
			tEmail.wContextNum = 0;
			for (int k = 0; k < tPBEmail.context_list_size(); ++k)
			{
				EmailContext &tContext = tEmail.arContextList[tEmail.wContextNum++];
				const PB::EmailContext &tPBContext = tPBEmail.context_list(k);
				tContext.wType = tPBContext.type();
				strcpy_s(tContext.szText, sizeof(tContext.szText) - 1, tPBContext.text().c_str());
			}
			strcpy_s(tEmail.szText, sizeof(tEmail.szText) - 1, tPBEmail.text().c_str());
			strcpy_s(tEmail.szTitle, sizeof(tEmail.szTitle) - 1, tPBEmail.title().c_str());
		}
	}
	else if (nEmailInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色邮件数据失败!", __FUNCTION__);
		return;
	}
	
	// todo::如果邮件已满,删除最近的10封邮件
	if (tPlayerEmailData.wEmailCount >= MAX_EMAIL_COUNT)
	{

	}

	// 邮件已满
	if (tPlayerEmailData.wEmailCount >= MAX_EMAIL_COUNT)
	{
		MAPLOG(ERROR_LEVEL, "[%s]邮件列表已满[RoleId:%d]!", __FUNCTION__,dwRoleId);
	}
	else
	{
		tPlayerEmailData.arEmailList[tPlayerEmailData.wEmailCount++] = pQuery->GetEmailInfo();
		tPlayerEmailData.wIsHaveNewEmail = true;

		PBRoleEmailInfo.set_is_have_new_email(tPlayerEmailData.wIsHaveNewEmail);
		PBRoleEmailInfo.clear_email_list();
		for (BYTE i = 0; i < tPlayerEmailData.wEmailCount; ++i)
		{
			EmailInfo &tEmail = tPlayerEmailData.arEmailList[i];
			PB::EmailInfo *tPBEmail = PBRoleEmailInfo.add_email_list();
			tPBEmail->set_uid(tEmail.dwID);
			tPBEmail->set_info_id(tEmail.wInfoId);
			tPBEmail->set_receive_time(tEmail.dwReceiveTime);
			for (WORD k = 0; k < tEmail.wContextNum; ++k)
			{
				EmailContext &tContext = tEmail.arContextList[k];
				PB::EmailContext *pPBContext = tPBEmail->add_context_list();
				pPBContext->set_text(tContext.szText);
				pPBContext->set_type(tContext.wType);
			}
			tPBEmail->set_text(tEmail.szText);
			tPBEmail->set_title(tEmail.szTitle);
		}

		string strEmailInfoData("");
		if (!PBRoleEmailInfo.AppendToString(&strEmailInfoData) || strEmailInfoData.size() > MAX_ROLE_EMAIL_INFO_SIZE)
		{
			MAPLOG(ERROR_LEVEL, "[%s]角色邮件数据系列化后过大[size:%d]", __FUNCTION__, strEmailInfoData.size());
			return;
		}

		// 邮件
		memset(szEmailInfo, 0, sizeof(szEmailInfo));
		pSqlDatabase->escape_string(szEmailInfo, strEmailInfoData.c_str(), strEmailInfoData.size());

		// 重新更新
		sprintf(pSqlDatabase->GetQuery(), "update role set email_info='%s' where role_id='%d'", szEmailInfo, dwRoleId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("UpdateEmailData");
			MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
			return;
		}
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载并删除GM事件
void  DBThreadHandler::LoadAndDeleteGMEvent(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	GMEventList_Load* pQuery = (GMEventList_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "select idx,cmd_id,param1,param2,param3,param4,param5,text,text1 from gm_event limit 0, %d", MAX_GM_EVENT_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		MAPLOG(ERROR_LEVEL, "[%s]查询GM事件出错!", __FUNCTION__);
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 事件删除列表
	vector<DWORD> vEventDeleteList;

	// 取事件列表
	GMEventData tEventData;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		memset(&tEventData, 0, sizeof(tEventData));
		pSqlDatabase->GetDWORD(i++, tEventData.dwIdx);
		pSqlDatabase->GetWORD(i++, tEventData.wEventId);
		pSqlDatabase->GetDWORD(i++, tEventData.arParamList[0]);
		pSqlDatabase->GetDWORD(i++, tEventData.arParamList[1]);
		pSqlDatabase->GetDWORD(i++, tEventData.arParamList[2]);
		pSqlDatabase->GetDWORD(i++, tEventData.arParamList[3]);
		pSqlDatabase->GetDWORD(i++, tEventData.arParamList[4]);
		pSqlDatabase->GetString(i++, tEventData.szText, sizeof(tEventData.szText) - 1);
		pSqlDatabase->GetString(i++, tEventData.szText1, sizeof(tEventData.szText1) - 1);
		pQuery->AddEventData(tEventData);

		vEventDeleteList.push_back(tEventData.dwIdx);

		//MAPLOG(ERROR_LEVEL, "获得GM事件[EventId:%d]!", tEventData.wEventId);
	}

	// 把已经获取的事件删掉
	for (size_t i = 0; i < vEventDeleteList.size(); ++i)
	{
		sprintf(pSqlDatabase->GetQuery(), "delete from gm_event where idx=%d", vEventDeleteList[i]);
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			MAPLOG(ERROR_LEVEL, "[%s]删除GM事件出错[Idx:%d]!", __FUNCTION__, vEventDeleteList[i]);
			return;
		}
		//MAPLOG(ERROR_LEVEL, "删除GM事件[EventId:%d]!", vEventDeleteList[i]);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载服务器列表
void DBThreadHandler::LoadServerList(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ServerList_Load* pQuery = (ServerList_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	sprintf(pSqlDatabase->GetQuery(), "select server_id,open_time from server_list", MAX_GM_EVENT_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		MAPLOG(ERROR_LEVEL, "[%s]查询服务器列表出错!", __FUNCTION__);
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 取事件列表
	ServerInfo tServerInfo;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetDWORD(i++, tServerInfo.dwServerId);
		pSqlDatabase->GetDWORD(i++, tServerInfo.dwOpenTime);
		pQuery->AddServerInfo(&tServerInfo);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储竞技场战斗记录数据
void DBThreadHandler::SaveArenaBattleRecordData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ArenaBattleRecord_Save* pQuery = (ArenaBattleRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const PlayerArenaBattleRecord *pPlayerArenaBattleRecord = pQuery->GetPlayerBattleRecord();
	char szBuff[sizeof(PlayerArenaBattleRecord) * 2 + 1] = { 0 };
	pSqlDatabase->escape_string((char*)szBuff, (char*)pPlayerArenaBattleRecord, sizeof(PlayerArenaBattleRecord));
	sprintf(pSqlDatabase->GetQuery(), "Replace Into arena_battle_record(role_id,battle_record) values(%u,'%s')", pQuery->GetRoleId(), szBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载竞技场战斗记录数据
void DBThreadHandler::LoadArenaBattleRecordData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ArenaBattleRecord_Load* pQuery = (ArenaBattleRecord_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 如果是第一次,则读取列表数量
	if (!pQuery->GetTotalBattleRecordNum())
	{
		sprintf(pSqlDatabase->GetQuery(), "select count(*) from arena_battle_record");
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
		int nCount = 0;
		while (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetInt(0, nCount);
		}
		pQuery->SetTotalBattleRecordNum(nCount);
	}

	DWORD dwMinRoleId = pQuery->GetMinRoleId();

	sprintf(pSqlDatabase->GetQuery(), "select role_id,battle_record from arena_battle_record\
	where role_id > %d order by role_id limit 0,%d", dwMinRoleId, ARENA_BATTLE_RECORD_LOAD_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	PlayerArenaBattleRecord tBattleRecord;
	DWORD dwRoleId = 0;
	while (pSqlDatabase->Fetch())
	{
		memset(&tBattleRecord, 0, sizeof(tBattleRecord));
		int i = 0;
		pSqlDatabase->GetDWORD(i++, dwRoleId);
		pSqlDatabase->GetBlob(i++, (unsigned char*)&tBattleRecord);
		pQuery->AddPlayerBattleRecord(&tBattleRecord);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存贷角色货币日志
void DBThreadHandler::SaveRoleCurencyLogs(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleCurrencyLogs_Save* pQuery = (RoleCurrencyLogs_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	WORD wFromId = pQuery->GetFromId();
	int nCurrencyType = pQuery->GetCurrencyType();	// 货币类型
	int nCurrencyValue = pQuery->GetCurrencyValue(); // 货币值


	sprintf(pSqlDatabase->GetQuery(), "insert into currency_logs(role_id,role_level,type,value,cur_value,from_id,time) VALUES(%u,%d,%d,%d,%d,%u,'%s')",
		pQuery->GetRoleId(), pQuery->GetRoleLevel(), nCurrencyType, nCurrencyValue, pQuery->GetCurCurrencyValue(),wFromId, szInsertTime);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();

	// 钻石
	if (nCurrencyType == ROLE_PRO_DIAMOND)
	{
		int is_add = nCurrencyValue > 0 ? true : false;
		sprintf(pSqlDatabase->GetQuery(), "select diamond from role_diamond_logs where from_id=%d and is_add=%d", wFromId, is_add);
		nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			MAPLOG(ERROR_LEVEL, "[%s]查询钻石日志出错!", __FUNCTION__);
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
		unsigned __int64 nDiamond = 0;
		while (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetUInt64(0, nDiamond);
		}

		nDiamond += abs(nCurrencyValue);
		sprintf(pSqlDatabase->GetQuery(), "Replace Into role_diamond_logs(from_id, is_add, diamond) values(%d,%d,%I64u)", wFromId, is_add, nDiamond);
		nRet = pSqlDatabase->Execute();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			pQuery->SetQueryResultString(pSqlDatabase->error());
			return;
		}

		pQuery->SetResultRowNum(nRet);
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pSqlDatabase->CloseRecordset();
	}
}

// 存储领地数据
void DBThreadHandler::SaveLandData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	LandData_Save* pQuery = (LandData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwRoleId = pQuery->GetRoleId();
	const PlayerLandInfo &tLandInfo = pQuery->GetLandInfo();
	char szBuff[sizeof(PlayerLandInfo)* 2 + 1] = { 0 };
	pSqlDatabase->escape_string((char*)szBuff, (char*)&tLandInfo, sizeof(PlayerLandInfo));

	sprintf(pSqlDatabase->GetQuery(), "Replace Into land(role_id,data) values(%u,'%s')", pQuery->GetRoleId(), szBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载领地数据
void DBThreadHandler::LoadLandData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	LandData_Load* pQuery = (LandData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 如果是第一次,则读取列表数量
	if (!pQuery->GetTotalLandNum())
	{
		sprintf(pSqlDatabase->GetQuery(), "select count(*) from land");
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
		int nCount = 0;
		while (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetInt(0, nCount);
		}
		pQuery->SetTotalLandNum(nCount);
	}

	DWORD dwMinRoleId = pQuery->GetMinRoleId();

	sprintf(pSqlDatabase->GetQuery(), "select role_id,data from land where role_id > %d order by role_id limit 0,%d", 
		dwMinRoleId, MAX_LAND_LIST_LOAD_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	PlayerLandInfo tLandInfo;
	DWORD dwRoleId = 0;
	while (pSqlDatabase->Fetch())
	{
		memset(&tLandInfo, 0, sizeof(tLandInfo));
		int i = 0;
		pSqlDatabase->GetDWORD(i++, dwRoleId);
		pSqlDatabase->GetBlob(i++, (unsigned char*)&tLandInfo);
		pQuery->AddLandInfo(dwRoleId,tLandInfo);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 竞技场排行榜查看列表更新
void DBThreadHandler::UpdateArenaRankViewList(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ArenaRankViewList_Update* pQuery = (ArenaRankViewList_Update*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwServerId = pQuery->GetServerId();

	sprintf(pSqlDatabase->GetQuery(), "select data_size,arena_data from arena where server_id=%d", dwServerId);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}
	char szDataBuff[MAX_ARENA_LIST_DATA_SIZE] = { 0 };
	int nDataBuffSize = 0;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetInt(i++, nDataBuffSize);
		pSqlDatabase->GetBlob(i++, (unsigned char*)&szDataBuff, nDataBuffSize);
	}

	// 删除旧数据
	sprintf(pSqlDatabase->GetQuery(), "delete from arena_for_view where server_id=%d", dwServerId);
	nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 解析数据
	PB::ArenaRankDataList PBRankDataList;
	if (!PBRankDataList.ParseFromArray(szDataBuff, nDataBuffSize))
	{
		MAPLOG(ERROR_LEVEL, "[%s]解析竞技场数据出错(%d)", __FUNCTION__, nDataBuffSize);
		return;
	}
	ArenaRankData tRankData;
	for (int i = 0; i < PBRankDataList.rank_list_size(); ++i)
	{
		const PB::ArenaRankData &tPBRankData = PBRankDataList.rank_list(i);
		tRankData.dwTargetId = tPBRankData.role_id();
		tRankData.wCurRank = tPBRankData.cur_rank();
		tRankData.wHistoryBestRank = tPBRankData.history_rank();

		// 插入查看列表
		sprintf(pSqlDatabase->GetQuery(), "Insert Into arena_for_view(server_id,role_id,cur_rank,history_rank) VALUE (%d, %d, %d, %d)"
			, dwServerId, tRankData.dwTargetId, tRankData.wCurRank, tRankData.wHistoryBestRank);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("Insert Into arena_for_view");
			return;
		}
	}
}

// 存储留言
void DBThreadHandler::SaveLeaveMessage(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	LeaveMessage_Save* pQuery = (LeaveMessage_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	// 联系方式
	char szContackBuff[MAX_CONTACT_LENGTH * 2 + 1] = { 0 };
	pSqlDatabase->escape_string((char*)szContackBuff, pQuery->GetContack(), strlen(pQuery->GetContack()));

	// 留言内容
	char szMessageBuff[MAX_LEAVE_MESSAGE_LENGTH * 2 + 1] = { 0 };
	pSqlDatabase->escape_string((char*)szMessageBuff, pQuery->GetMesssage(), strlen(pQuery->GetMesssage()));

	// 插入
	sprintf(pSqlDatabase->GetQuery(), "Insert Into leave_message(role_id,server_id,message,date,contact) VALUE (%d, %d, '%s', '%s', '%s')",
		pQuery->GetRoleId(), pQuery->GetServerId(), szMessageBuff, szInsertTime, szContackBuff);
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("insert into leave_message");
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储奖励
void DBThreadHandler::SaveRewardCenterData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RewardCenterData_Save* pQuery = (RewardCenterData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwRoleId = pQuery->GetRoleId();

	//DWORD dwRoleId = pQuery->GetKey();
	sprintf(pSqlDatabase->GetQuery(), "select reward_info from role where role_id=%d", dwRoleId);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 取系列化后的数据
	PlayerRewardData tPlayerRewardData;
	memset(&tPlayerRewardData, 0, sizeof(tPlayerRewardData));
	char szRewardInfo[MAX_ROLE_REWARD_DATA_SIZE * 2 + 1] = { 0 };
	int nRewardInfoSize = 0;
	if (pSqlDatabase->Fetch())
	{
		nRewardInfoSize = pSqlDatabase->GetBlob(0, (unsigned char*)&szRewardInfo);
	}

	// 反系列化数据
	PB::RoleRewardData PBRoleRewardInfo;
	if (nRewardInfoSize && PBRoleRewardInfo.ParseFromArray(szRewardInfo, nRewardInfoSize))
	{
		tPlayerRewardData.wRewardNum = 0;
		for (int i = 0; i < PBRoleRewardInfo.reward_list_size(); ++i)
		{
			RewardData &tReward = tPlayerRewardData.arRewardList[tPlayerRewardData.wRewardNum++];
			const PB::RewardData &tPBReward = PBRoleRewardInfo.reward_list(i);
			tReward.dwUId = tPBReward.uid();
			tReward.wInfoId = tPBReward.info_id();
			tReward.dwValue1 = tPBReward.value1();
			tReward.dwValue2 = tPBReward.value2();
			tReward.dwTime = tPBReward.time();
			strcpy_s(tReward.szText, sizeof(tReward.szText) - 1, tPBReward.text().c_str());
		}
	}
	else if (nRewardInfoSize)
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色奖励中心数据失败!", __FUNCTION__);
		return;
	}

	// 奖励列表已满
	if (tPlayerRewardData.wRewardNum >= MAX_REWARD_CENTER_REWARD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]奖励中心数据列表已满[RoleId:%d]!", __FUNCTION__, dwRoleId);
	}
	else
	{
		tPlayerRewardData.arRewardList[tPlayerRewardData.wRewardNum++] = pQuery->GetRewardData();

		PBRoleRewardInfo.clear_reward_list();
		for (WORD i = 0; i < tPlayerRewardData.wRewardNum; ++i)
		{
			RewardData &tReward = tPlayerRewardData.arRewardList[i];
			PB::RewardData *tPBReward = PBRoleRewardInfo.add_reward_list();
			tPBReward->set_uid(tReward.dwUId);
			tPBReward->set_info_id(tReward.wInfoId);
			tPBReward->set_value1(tReward.dwValue1);
			tPBReward->set_value2(tReward.dwValue2);
			tPBReward->set_time(tReward.dwTime);
			tPBReward->set_text(tReward.szText);
		}

		string strRewardInfoData("");
		if (!PBRoleRewardInfo.AppendToString(&strRewardInfoData) || strRewardInfoData.size() > MAX_ROLE_REWARD_DATA_SIZE)
		{
			MAPLOG(ERROR_LEVEL, "[%s]角色奖励数据系列化后过大[size:%d]", __FUNCTION__, strRewardInfoData.size());
			return;
		}

		// 邮件
		memset(szRewardInfo, 0, sizeof(szRewardInfo));
		pSqlDatabase->escape_string(szRewardInfo, strRewardInfoData.c_str(), strRewardInfoData.size());

		// 重新更新
		sprintf(pSqlDatabase->GetQuery(), "update role set reward_info='%s' where role_id='%d'", szRewardInfo, dwRoleId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("UpdateRewardData");
			MAPLOG(ERROR_LEVEL, pSqlDatabase->GetQuery());
			return;
		}
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();	
}

// 更新称号数据
void DBThreadHandler::UpdateRoleTempData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleTmpData_Update* pQuery = (RoleTmpData_Update*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

 	DWORD dwRoleId = pQuery->GetRoleId();
 	sprintf(pSqlDatabase->GetQuery(), "select temp_data from role where role_id=%d", dwRoleId);
 	int nRet = pSqlDatabase->OpenRecordset();
 	if (-1 == nRet)
 	{
 		pQuery->SetQueryResult(pSqlDatabase->errorno());
 		return;
 	}
 
 	// 取系列化后的数据
	char szBuffInfo[MAX_ROLE_TEMP_DATA_SIZE * 2 + 1] = { 0 };
 	int nBuffInfoSize = 0;
 	if (pSqlDatabase->Fetch())
 	{
		nBuffInfoSize = pSqlDatabase->GetBlob(0, (unsigned char*)&szBuffInfo);
 	}
 
 	// 反系列化数据
	RoleTempDatas tRoleTempDatas;
	memset(&tRoleTempDatas, 0, sizeof(tRoleTempDatas));
 	PB::RoleTempDatas PBRoleTempDatas;
	if (nBuffInfoSize && PBRoleTempDatas.ParseFromArray(szBuffInfo, nBuffInfoSize))
 	{
		tRoleTempDatas.wTempDataNum = 0;
		for (int i = 0; i < PBRoleTempDatas.temp_data_list_size(); ++i)
 		{
			RoleTempData &tTempData = tRoleTempDatas.arTempDataList[tRoleTempDatas.wTempDataNum++];
			const PB::RoleTempData &tPBTempData = PBRoleTempDatas.temp_data_list(i);
			tTempData.wType = tPBTempData.type();
			strcpy_s(tTempData.szText, sizeof(tTempData.szText), tPBTempData.text().c_str());
			tTempData.wParamNum = 0;
			for (int k = 0; k < tPBTempData.param_list_size(); ++k)
 			{
				tTempData.arParamList[tTempData.wParamNum++] = tPBTempData.param_list(k);
 			}
 		}
 	}
	else if (nBuffInfoSize)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]反系列化角色临时数据失败!", __FUNCTION__);
 		return;
 	}
 
 	// 列表已满
	if (tRoleTempDatas.wTempDataNum >= MAX_ROLE_TEMP_DATA_NUM)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]角色临时数据列表已满[RoleId:%d]!", __FUNCTION__, dwRoleId);
 	}
 	else
 	{
		// 重新赋值
		tRoleTempDatas.arTempDataList[tRoleTempDatas.wTempDataNum++] = pQuery->GetTempData();
		PBRoleTempDatas.clear_temp_data_list();
		for (WORD i = 0; i < tRoleTempDatas.wTempDataNum; ++i)
 		{
			RoleTempData &tTempData = tRoleTempDatas.arTempDataList[i];
			PB::RoleTempData *tPBTempData = PBRoleTempDatas.add_temp_data_list();
			tPBTempData->set_type(tTempData.wType);
			tPBTempData->set_text(tTempData.szText);
			for (WORD k = 0; k < tTempData.wParamNum; ++k)
			{
				tPBTempData->add_param_list(tTempData.arParamList[k]);
			}
 		}
 
		// 系列化
 		string strRoleTempData("");
		if (!PBRoleTempDatas.AppendToString(&strRoleTempData) || strRoleTempData.size() > MAX_ROLE_TEMP_DATA_SIZE)
 		{
			MAPLOG(ERROR_LEVEL, "[%s]角色临时数据系列化后过大[size:%d]", __FUNCTION__, strRoleTempData.size());
 			return;
 		}
 
 		// 过滤
		memset(szBuffInfo, 0, sizeof(szBuffInfo));
		pSqlDatabase->escape_string(szBuffInfo, strRoleTempData.c_str(), strRoleTempData.size());
 
 		// 重新更新
		sprintf(pSqlDatabase->GetQuery(), "update role set temp_data='%s' where role_id='%d'", szBuffInfo, dwRoleId);
 		nRet = pSqlDatabase->Execute();
 		pQuery->SetQueryResult(pSqlDatabase->errorno());
 		if (nRet == -1)
 		{
 			pQuery->SetQueryResultString(pSqlDatabase->error());
 			pQuery->SetQueryResultTable("UpdateTempData");
 			MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
 			return;
 		}
 	}
 
 	pQuery->SetResultRowNum(nRet);
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	pSqlDatabase->CloseRecordset();
}

// 存储角色下线记录
void DBThreadHandler::SaveRoleOfflineRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleOfflineRecord_Save* pQuery = (RoleOfflineRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const RoleOfflineRecord &tRecord = pQuery->GetRecord();

	// 当前时间
	char szRecordTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(tRecord.dwOfflineTime);
	CurTime.toString(szRecordTime);

	// 插入
	sprintf(pSqlDatabase->GetQuery(), 
		"Insert Into role_logout_record(role_id,level,vip_level,diamond,gold,online_time,offline_time,chapter,map,guide_x,guide_y,server_id)\
		 VALUE (%d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d)",
		 tRecord.dwRoleId, tRecord.wLevel, tRecord.wVipLevel, tRecord.dwDiamond, tRecord.dwGold, tRecord.dwOnlineTime, szRecordTime,
		 tRecord.wMainFBChapter, tRecord.wMainFBMap, tRecord.nGuideX, tRecord.nGuideY, tRecord.wServerId);
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("insert into RoleOfflineRecord");
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储角色等级记录
void DBThreadHandler::SaveRoleLevelRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleLevelRecord_Save* pQuery = (RoleLevelRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const RoleLevelRecord &tRecord = pQuery->GetRecord();

	// 当前时间
 	char szRecordTime[MAX_TIME_LEN] = { 0 };
 	DATETIME CurTime(tRecord.dwRecordTime);
 	CurTime.toString(szRecordTime);
 
 	// 插入
 	sprintf(pSqlDatabase->GetQuery(),
 		"Insert Into role_level_record(role_id,level,vip_level,diamond,gold,record_time,main_fb_chapter,main_fb_map,guide_x,guide_y,\
		fighting_power,server_id,hero_num,item_num,equipment_num,treasure_num,hero_fb_chapter,elite_fb_chapter,tower_map_level,\
		land_num,world_heart_stage,arena_history_rank)\
 		VALUE (%d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
		tRecord.dwRoleId, tRecord.wLevel, tRecord.wVipLevel, tRecord.dwDiamond, tRecord.dwGold, szRecordTime,tRecord.wMainFBChapter, 
		tRecord.wMainFBMap, tRecord.nGuideX, tRecord.nGuideY, tRecord.dwFightingPower,tRecord.wServerId, tRecord.wHeroNum, tRecord.wItemNum, 
		tRecord.wEquipmentNum, tRecord.wTreasureNum,tRecord.wHeroFBChapter, tRecord.wEliteFBChapter, tRecord.wTowerMapLevel, tRecord.wLandNum, 
		tRecord.wWorldHeartStage, tRecord.wArenaHistoryRank);
 	int nRet = pSqlDatabase->Execute();
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	if (nRet == -1)
 	{
 		pQuery->SetQueryResult(pSqlDatabase->errorno());
 		pQuery->SetQueryResultString(pSqlDatabase->error());
 		pQuery->SetQueryResultTable("insert into role_level_record");
 		return;
 	}
 
 	pQuery->SetResultRowNum(nRet);
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	pSqlDatabase->CloseRecordset();
}

// 存储角色充值记录
void DBThreadHandler::SaveRoleRechargeRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleRechargeRecord_Save* pQuery = (RoleRechargeRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const RechargeRecord &tRecord = pQuery->GetRecord();

	// 当前时间
 	char szRecordTime[MAX_TIME_LEN] = { 0 };
 	DATETIME CurTime(tRecord.dwRecordTime);
 	CurTime.toString(szRecordTime);
 
 	// 插入
 	sprintf(pSqlDatabase->GetQuery(),
 		"Insert Into recharge_record(role_id,role_level,server_id,sdk_order_id,recharge_id,recharge_type,from,amount,diamond,record_time,pay_time)\
 		VALUE (%d, %d, %d, '%s', %d, %d, %d, %d, %d, '%s', '%s')",
		tRecord.dwRoleId, tRecord.wRoleLevel, tRecord.wServerId, tRecord.szSDKOrderId, tRecord.wRechargeId, tRecord.wRechargeType, 
		tRecord.wFrom, tRecord.dwAmount, tRecord.dwDiamond, szRecordTime, tRecord.szPayTime);
 	int nRet = pSqlDatabase->Execute();
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	if (nRet == -1)
 	{
 		pQuery->SetQueryResult(pSqlDatabase->errorno());
 		pQuery->SetQueryResultString(pSqlDatabase->error());
 		pQuery->SetQueryResultTable("insert into recharge_record");
 		return;
 	}
 
 	pQuery->SetResultRowNum(nRet);
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	pSqlDatabase->CloseRecordset();
}

// 存储关卡失败记录
void DBThreadHandler::SaveRoleMapLoseRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleMapLoseRecord_Save* pQuery = (RoleMapLoseRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const MapLoseRecord &tRecord = pQuery->GetRecord();

	sprintf(pSqlDatabase->GetQuery(), "select lose_times from map_lose_record where role_id=%d and map_id=%d", tRecord.dwRoleId, tRecord.dwMapId);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}
	WORD wLoseTimes = 0;
	if (pSqlDatabase->Fetch())
	{
		pSqlDatabase->GetWORD(0, wLoseTimes);
	}

	// 插入
	sprintf(pSqlDatabase->GetQuery(),
		"Replace Into map_lose_record(role_id,map_id,map_type,chapter,map_idx,fighting_power,lose_times) VALUE (%d, %d, %d, %d, %d, %d, %d)",
		tRecord.dwRoleId, tRecord.dwMapId, tRecord.wType, tRecord.wChapter, tRecord.wMapIdx, tRecord.dwFightingPower, wLoseTimes + 1);
	nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("insert into map_lose_record");
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储首次通关记录
void DBThreadHandler::SaveRoleFirstPassMapRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	RoleFirstPassMapRecord_Save* pQuery = (RoleFirstPassMapRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const FirstPassMapRecord &tRecord = pQuery->GetRecord();

	sprintf(pSqlDatabase->GetQuery(), 
	"insert Into map_first_pass_record(role_id,map_id,map_type,chapter,map_idx,fighting_power,level) values(%d, %d, %d, %d, %d, %d, %d)", 
	tRecord.dwRoleId, tRecord.dwMapId, tRecord.wType, tRecord.wChapter, tRecord.wMapIdx, tRecord.dwFightingPower, tRecord.wLevel);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("SaveRoleFirstPassMapRecord");
		return;
	}
	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储商店购买记录
void DBThreadHandler::SaveShopBuyRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	ShopBuyRecord_Save* pQuery = (ShopBuyRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	WORD wShopType = pQuery->GetShopType();
	DWORD dwGoodsId = pQuery->GetGoodsId();
	DWORD dwBuyNum = 0;

	sprintf(pSqlDatabase->GetQuery(), "select buy_num from shop_buy_record where shop_type=%d and goods_id=%d", wShopType, dwGoodsId);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}
	if (pSqlDatabase->Fetch())
	{
		pSqlDatabase->GetDWORD(0, dwBuyNum);
	}

	dwBuyNum += pQuery->GetBuyNum();

	sprintf(pSqlDatabase->GetQuery(), "Replace Into shop_buy_record(shop_type,goods_id,buy_num) VALUE (%d, %d, %d)", wShopType, dwGoodsId, dwBuyNum);
	nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("SaveShopBuyRecord");
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储工会数据
void DBThreadHandler::SaveGuildData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	GuildData_Save* pQuery = (GuildData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwGuildId = pQuery->GetKey();

	// 删除工会
	if (pQuery->GetIsDelete())
	{
		sprintf(pSqlDatabase->GetQuery(), "delete from guild_list where guild_id=%d",dwGuildId);
	}
	// 更新工会
	else
	{
		DWORD dwServerId = pQuery->GetServerId();
		const char *pDataBuff = pQuery->GetDataBuff();
		int nDataBuffSize = pQuery->GetDataBuffSize();
		char szBuff[MAX_GUILD_DATA_SIZE * 2 + 1] = { 0 };
		pSqlDatabase->escape_string((char*)szBuff, pDataBuff, nDataBuffSize);
		sprintf(pSqlDatabase->GetQuery(), "Replace Into guild_list(guild_id,server_id,guild_name,guild_info) values(%u,%u,'%s','%s')",
			dwGuildId, dwServerId, pQuery->GetGuildName(), szBuff);
	}

	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载工会数据
void DBThreadHandler::LoadGuildData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	GuildData_Load* pQuery = (GuildData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 如果是第一次,则读取列表数量
	if (!pQuery->GetTotalGuildNum())
	{
		sprintf(pSqlDatabase->GetQuery(), "select count(*) from guild_list");
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
		int nCount = 0;
		while (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetInt(0, nCount);
		}
		pQuery->SetTotalGuildNum(nCount);
	}

	DWORD dwMinGuildId = pQuery->GetMinGuildId();

	sprintf(pSqlDatabase->GetQuery(), "select guild_id,guild_info from guild_list where guild_id > %d order by guild_id limit 0,%d", 
		dwMinGuildId, GUILD_LIST_LOAD_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	GuildDataBuff tDataBuff;
	while (pSqlDatabase->Fetch())
	{
		memset(&tDataBuff, 0, sizeof(tDataBuff));
		pSqlDatabase->GetDWORD(0, tDataBuff.dwGuildId);
		tDataBuff.nBuffSize = pSqlDatabase->GetBlob(1, (unsigned char*)&tDataBuff.szBuff);
		pQuery->AddGuildData(&tDataBuff);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储聊天记录数据
void DBThreadHandler::SaveChatRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ChatRecord_Save* pQuery = (ChatRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	char szContext[MAX_CHAT_TEXT_LEN * 2 + 1] = { 0 };
	pSqlDatabase->escape_string(szContext, pQuery->GetContext(), strlen(pQuery->GetContext()));

	char szRoleName[2 * MAX_ROLE_LEN + 1] = { 0 };
	pSqlDatabase->escape_string(szRoleName, pQuery->GetTarget(), strlen(pQuery->GetTarget()));

	sprintf(pSqlDatabase->GetQuery(), "insert Into chat_record(chat_type,server_id,sender,target,context,chat_time) values(%d,%d,'%s','%s','%s','%s')",
		pQuery->GetChatType(), pQuery->GetServerId(), pQuery->GetSender(), szRoleName, szContext, szInsertTime);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载聊天记录
void DBThreadHandler::LoadChatRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{

}

// 存储日志
void DBThreadHandler::SaveLogs(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	Log_Save* pQuery = (Log_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	char szBuff[MAX_LOG_CONTEXT_LEN * 2 + 1] = { 0 };

	g_GB2312ToUTF8(pQuery->GetContext(), szBuff);

	//pSqlDatabase->escape_string((char*)szBuff, pQuery->GetContext(), strlen(pQuery->GetContext()));
	sprintf(pSqlDatabase->GetQuery(), "insert Into log(time,log_level,context) values('%s',%d,'%s')", 
		szInsertTime, pQuery->GetLevel(),szBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储关卡评论
void DBThreadHandler::SaveMapComment(SqlDatabase* pSqlDatabase, QueryResult * pData)
{	
	MapComment_Save* pQuery = (MapComment_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	PB::MapComments PBMapComments;
	PBMapComments.set_map_id(0);
	WORD wCommentNum = pQuery->GetCommentNum();
	const MapComment *pCommentList = pQuery->GetCommentList();
	for (WORD i = 0; i < wCommentNum; ++i)
	{
		const MapComment *pComment =  &pCommentList[i];
		::PB::MapComment *pPBComment = PBMapComments.add_comment_list();
		pPBComment->set_role_id(pComment->dwRoleId);
		pPBComment->set_comment_id(pComment->wCommentId);
		pPBComment->set_time(pComment->dwTime);
		pPBComment->set_text(pComment->szText);
		pPBComment->set_like_times(pComment->wLikeTimes);
		for (WORD k = 0; k < pComment->wLikePlayerNum; ++k)
		{
			pPBComment->add_like_player_list(pComment->arLikePlayerList[k]);
		}
	}

	string strCommentData("");
	if (!PBMapComments.AppendToString(&strCommentData) || strCommentData.size() > MAX_MAP_COMMENT_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]关卡评论数据系列化后过大[size:%d]", __FUNCTION__, strCommentData.size());
		return;
	}

	char szBuff[sizeof(MapComment)*MAX_MAP_COMMENT_NUM * 2 + 1] = { 0 };

	// 评论数据
	pSqlDatabase->escape_string(szBuff, strCommentData.c_str(), strCommentData.size());
	sprintf(pSqlDatabase->GetQuery(), "Replace Into map_comment(id,comment_data) values(%u,'%s')", pQuery->GetId(), szBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载关卡评论
void DBThreadHandler::LoadMapComment(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	if (!pSqlDatabase || !pData)
		return;

	MapComment_Load* pQuery = (MapComment_Load*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 如果是第一次,则读取列表数量
	if (!pQuery->GetTotalDataNum())
	{
		sprintf(pSqlDatabase->GetQuery(), "select count(*) from map_comment");
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			return;
		}
		DWORD dwCount = 0;
		while (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetDWORD(0, dwCount);
		}
		pQuery->SetTotalDataNum(dwCount);
	}

	DWORD dwMinId = pQuery->GetMinId();

	sprintf(pSqlDatabase->GetQuery(), "select id, comment_data from map_comment where id > %d order by id limit 0,%d",
		dwMinId, MAX_MAP_COMMENT_LOAD_NUM);
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	// 评论数据
	char szCommentInfo[MAX_MAP_COMMENT_DATA_SIZE * 2 + 1] = { 0 };
	int nCommentSize = 0;
	DWORD dwFindId = 0;
	MapComments tComments;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetDWORD(i++, dwFindId);
		int nCommentInfoSize = pSqlDatabase->GetBlob(i++, (unsigned char*)szCommentInfo);
		PB::MapComments PBMapComments;
		if (!PBMapComments.ParseFromArray(szCommentInfo, nCommentInfoSize))
		{
			MAPLOG(ERROR_LEVEL, "[%s]反系列化关卡评论数据失败!", __FUNCTION__);
			return;
		}

		tComments.dwId = dwFindId;
		tComments.wCommentNum = 0;
		for (int k = 0; k < PBMapComments.comment_list_size(); ++k)
		{
			const ::PB::MapComment &tPBComment = PBMapComments.comment_list(k);
			MapComment &tComment = tComments.arCommentList[tComments.wCommentNum++];
			tComment.dwRoleId = tPBComment.role_id();
			tComment.dwTime = tPBComment.time();
			tComment.wCommentId = tPBComment.comment_id();
			tComment.wLikeTimes = tPBComment.like_times();
			strcpy_s(tComment.szText, sizeof(tComment.szText), tPBComment.text().c_str());
			tComment.wLikePlayerNum = 0;
			for (WORD m = 0; m < tPBComment.like_player_list_size(); ++m)
			{
				tComment.arLikePlayerList[tComment.wLikePlayerNum++] = tPBComment.like_player_list(m);
			}
		}

		pQuery->AddData(tComments);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储物品丢失记录
void DBThreadHandler::SaveItemLoseRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ItemLoseRecord_Save* pQuery = (ItemLoseRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const ItemLoseRecord &tRecord = pQuery->GetRecord();

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	char szBuff[MAX_LOG_CONTEXT_LEN * 2 + 1] = { 0 };

	sprintf(pSqlDatabase->GetQuery(), "insert Into item_lose_record(role_id,item_id,item_num,time) values('%d','%d','%d','%s')", 
		tRecord.dwRoleId, tRecord.dwItemId, tRecord.dwItemNum, szInsertTime);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储物品操作记录
void DBThreadHandler::SaveItemOperationRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ItemOperationRecord_Save* pQuery = (ItemOperationRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	const ItemOperationRecord &tRecord = pQuery->GetRecord();

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	char szBuff[MAX_LOG_CONTEXT_LEN * 2 + 1] = { 0 };

	sprintf(pSqlDatabase->GetQuery(), "insert Into item_operation_record(role_id,item_id,item_num,is_add,from_id,time) values(%d,%d,%d,%d,%d,'%s')",
		tRecord.dwRoleId, tRecord.dwItemId, tRecord.dwItemNum, tRecord.bIsAdd, tRecord.wFromId, szInsertTime);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 更新帐号锁
void DBThreadHandler::UpdateAccountLock(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	AccountLock_Update* pQuery = (AccountLock_Update*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 更新
	sprintf(pSqlDatabase->GetQuery(), "Update account Set is_lock=%d Where account='%s' and platform=%d", 1, pQuery->GetAccount(), pQuery->GetPlatform());
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("UpdateAccountLock");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储王者赛数据
void DBThreadHandler::SaveKingMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{	
	KingMatchData_Save* pQuery = (KingMatchData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	char *pTempBuff = pQuery->GetTempBuff();

	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());
	//szBuff[nBuffLen] = '\0';

	sprintf(pSqlDatabase->GetQuery(), 
		"Replace Into king_match(idx, cur_season, data_belong_season, battle_target_num, stage_data) values(%d, %d, %d, %d, '%s')",
		1, pQuery->GetCurSeason(), pQuery->GetDataBelongSeason(), pQuery->GetBattleTargetNum(), pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载王者赛数据
void DBThreadHandler::LoadKingMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	//MAPLOG(ERROR_LEVEL, "Enter");

	KingMatchData_Load* pQuery = (KingMatchData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null",__FUNCTION__);
		return;
	}
	
	//MAPLOG(ERROR_LEVEL, "Begin");

	sprintf(pSqlDatabase->GetQuery(), "select cur_season,data_belong_season,stage_data from king_match where idx=1");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		MAPLOG(ERROR_LEVEL, "[%s]OpenRecordset fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	//MAPLOG(ERROR_LEVEL, "End");

 	int nDataBuffSize = 0;
	//char szDataBuff[MAX_KING_MATCH_STAGE_DATA_SIZE] = { 0 };
 	//char *pTempBuff = pQuery->GetTempBuff();
 	WORD wCurSeason = 0;
 	WORD wDataBelongSeason = 0;
 	while (pSqlDatabase->Fetch())
 	{
 		int i = 0;
 		pSqlDatabase->GetWORD(i++, wCurSeason);
 		pSqlDatabase->GetWORD(i++, wDataBelongSeason);
		nDataBuffSize = pSqlDatabase->GetBlob(i++, (unsigned char*)pQuery->GetDataBuff());
 
 		pQuery->SetCurSeason(wCurSeason);
 		pQuery->SetDataBelongSeason(wDataBelongSeason);

		MAPLOG(ERROR_LEVEL, "LoadKingMatchData[CurSeason:%d, BelongSeason:%d]", wCurSeason, wDataBelongSeason);
 	}
 
	//pQuery->SetDataBuff(szDataBuff, nDataBuffSize);
	pQuery->SetDataBuffSize(nDataBuffSize);
 
 	pQuery->SetResultRowNum(nRet);
 	pQuery->SetQueryResult(pSqlDatabase->errorno());
 	pSqlDatabase->CloseRecordset();

	//MAPLOG(ERROR_LEVEL, "LoadKingMatchData is finish...");
}

// 存储积分赛数据
void DBThreadHandler::SaveScoreMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ScoreMatchData_Save* pQuery = (ScoreMatchData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	char *pTempBuff = pQuery->GetTempBuff();

	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());
	//szBuff[nBuffLen] = '\0';

	sprintf(pSqlDatabase->GetQuery(),
		"Replace Into score_match(idx,cur_activity_idx,activity_status,activity_over_time,join_role_num,score_match_data) values(%d,%d,%d,%d,%d,'%s')",
		1, pQuery->GetCurActivityIndex(), pQuery->GetActivityStatus(), pQuery->GetActivityOverTime(), pQuery->GetJoinRoleNum(), pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载积分赛数据
void DBThreadHandler::LoadScoreMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ScoreMatchData_Load* pQuery = (ScoreMatchData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	sprintf(pSqlDatabase->GetQuery(),"select cur_activity_idx,activity_status,activity_over_time,score_match_data from score_match where idx=1");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		MAPLOG(ERROR_LEVEL, "[%s]OpenRecordset fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	int nDataBuffSize = 0;
	DWORD dwCurActivityIdx = 0;
	BYTE byActivityStatus = 0;
	DWORD dwActivityOverTime = 0;
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		pSqlDatabase->GetDWORD(i++, dwCurActivityIdx);
		pSqlDatabase->GetBYTE(i++, byActivityStatus);
		pSqlDatabase->GetDWORD(i++, dwActivityOverTime);
		nDataBuffSize = pSqlDatabase->GetBlob(i++, (unsigned char*)pQuery->GetDataBuff());

		pQuery->SetCurActivityIndex(dwCurActivityIdx);
		pQuery->SetActivityStatus(byActivityStatus);
		pQuery->SetActivityOverTime(dwActivityOverTime);
	}

	//pQuery->SetDataBuff(szDataBuff, nDataBuffSize);
	pQuery->SetDataBuffSize(nDataBuffSize);

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储组队副本数据
void DBThreadHandler::SaveTeamDungeonRankData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	TeamDungeonRankData_Save* pQuery = (TeamDungeonRankData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	char *pTempBuff = pQuery->GetTempBuff();

	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());
	//szBuff[nBuffLen] = '\0';

	sprintf(pSqlDatabase->GetQuery(),"Replace Into team_dungeon_rank(idx, team_dungeon_rank_data) values(%d, '%s')",1, pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载组队副本数据
void DBThreadHandler::LoadTeamDungeonRankData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{	
	TeamDungeonRankData_Load* pQuery = (TeamDungeonRankData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	sprintf(pSqlDatabase->GetQuery(), "select team_dungeon_rank_data from team_dungeon_rank where idx=1");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		MAPLOG(ERROR_LEVEL, "[%s]OpenRecordset fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	int nDataBuffSize = 0;
	char *pDataBuff = pQuery->GetTempBuff();
	while (pSqlDatabase->Fetch())
	{
		int i = 0;
		nDataBuffSize = pSqlDatabase->GetBlob(i++, (unsigned char*)pDataBuff);
	}

	pQuery->SetDataBuff(pDataBuff, nDataBuffSize);

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();

	//MAPLOG(ERROR_LEVEL, "LoadTeamDungeonRankData is finish...");
}

// 存储在线玩家数量数据
void DBThreadHandler::SaveOfflinePlayerNumRecord(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	OnlinePlayerNumRecord_Save* pQuery = (OnlinePlayerNumRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	//DWORD dwPlayerNum = pQuery->GetPlayerNum();

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szInsertTime);

	char szBuff[MAX_LOG_CONTEXT_LEN * 2 + 1] = { 0 };

	int nRet = 0;
	for (WORD i = 0; i < pQuery->GetServerNum(); ++i)
	{
		const ServerOnlineData *pOnlineData = pQuery->GetOnlineData(i);
		if (pOnlineData)
		{
			sprintf(pSqlDatabase->GetQuery(), "insert Into online_player_num(server_id,player_num,record_time) values(%d,%d,'%s')",
				pOnlineData->wServerId, pOnlineData->wOnlinePlayerNum, szInsertTime);
			nRet = pSqlDatabase->Execute();
			if (-1 == nRet)
			{
				pQuery->SetQueryResult(pSqlDatabase->errorno());
				return;
			}
		}
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 更新合服数据 
void DBThreadHandler::UpdateServerData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	
}

// 存储转盘数据
void DBThreadHandler::SaveTurntableData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	TurntableData_Save* pQuery = (TurntableData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	char *pTempBuff = pQuery->GetTempBuff();

	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());

	sprintf(pSqlDatabase->GetQuery(), "Replace Into key_value_data(my_key, my_value) values(%d, '%s')", 1, pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载转盘数据
void DBThreadHandler::LoadTurntableData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	TurntableData_Load* pQuery = (TurntableData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	sprintf(pSqlDatabase->GetQuery(), "select my_value from key_value_data where my_key=1");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		MAPLOG(ERROR_LEVEL, "[%s]OpenRecordset fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	int nDataBuffSize = 0;
	char *pDataBuff = pQuery->GetTempBuff();
	while (pSqlDatabase->Fetch())
	{
		nDataBuffSize = pSqlDatabase->GetBlob(0, (unsigned char*)pDataBuff, pQuery->GetDataBuffMaxSize());
		pQuery->SetDataBuff(pDataBuff, nDataBuffSize);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 更新新手引导
void DBThreadHandler::UpdateNewRoleGuide(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	NewRoleGuide_Update* pQuery = (NewRoleGuide_Update*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 更新
	sprintf(pSqlDatabase->GetQuery(), "Update role Set new_role_guide=%d Where role_id=%d", pQuery->GetValue(), pQuery->GetRoleId());
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("UpdateNewRoleGuide");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储资源战数据
void DBThreadHandler::SaveResourceWarData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ResourceWarData_Save* pQuery = (ResourceWarData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	char *pTempBuff = pQuery->GetTempBuff();

	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());

	sprintf(pSqlDatabase->GetQuery(),
		"Replace Into resource_war(idx,occupy_resource_num,join_role_num,enemy_num,resource_war_data) values(%d,%d,%d,%d,'%s')",
		1, pQuery->GetOccupyResourceNum(), pQuery->GetJoinRoleNum(), pQuery->GetEnemyNum(), pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载资源战数据
void DBThreadHandler::LoadResourceWarData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ResourceWarData_Load* pQuery = (ResourceWarData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	sprintf(pSqlDatabase->GetQuery(), "select resource_war_data from resource_war where idx=1");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		MAPLOG(ERROR_LEVEL, "[%s]OpenRecordset fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	int nDataBuffSize = 0;
	if(pSqlDatabase->Fetch())
	{
		nDataBuffSize = pSqlDatabase->GetBlob(0, (unsigned char*)pQuery->GetDataBuff());
	}

	pQuery->SetDataBuffSize(nDataBuffSize);

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 存储工会战数据
void DBThreadHandler::SaveGuildWarData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	GuildWarData_Save* pQuery = (GuildWarData_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	DWORD dwJoinRoleNum = pQuery->GetJoinRoleNum();
	DWORD dwStarNum = pQuery->GetStarNum();

	char *pTempBuff = pQuery->GetTempBuff();

	unsigned int nBuffLen = pSqlDatabase->escape_string(pTempBuff, pQuery->GetDataBuff(), pQuery->GetDataBuffSize());

	sprintf(pSqlDatabase->GetQuery(),
		"Replace Into guild_war(idx,join_role_num,star_num,guild_war_data) values(%d,%d,%d,'%s')",
		1, dwJoinRoleNum, dwStarNum, pTempBuff);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 加载工会战数据 
void DBThreadHandler::LoadGuildWarData(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	GuildWarData_Load* pQuery = (GuildWarData_Load*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	sprintf(pSqlDatabase->GetQuery(), "select guild_war_data from guild_war where idx=1");
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		MAPLOG(ERROR_LEVEL, "[%s]OpenRecordset fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	int nDataBuffSize = 0;
	if (pSqlDatabase->Fetch()){
		nDataBuffSize = pSqlDatabase->GetBlob(0, (unsigned char*)pQuery->GetDataBuff());
	}

	pQuery->SetDataBuffSize(nDataBuffSize);

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 检索激活码数据
void DBThreadHandler::SelectActivityCode(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ActivationCode_Select* pQuery = (ActivationCode_Select*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	int nResult = 0; // 查询结果(0=正常,1=激活码不存在 2=奖励不存在)
	int nStatus = 1;
	int nBatch = 0;		// 没有0批次的
	DWORD dwRewardId = 0;
	DWORD dwExpireTime = 0;
	int nType = 0;

	// 取激活码数据
	sprintf(pSqlDatabase->GetQuery(), "select status,batch from activation_code_record where code='%s'", pQuery->GetActivationCode());
	int nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		MAPLOG(ERROR_LEVEL, "[%s]select activation_code_record fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}

	if (pSqlDatabase->Fetch())
	{
		pSqlDatabase->GetInt(0, nStatus);
		pSqlDatabase->GetInt(1, nBatch);
	}
	else
	{
		nResult = 1;
	}

	// 取批次数据
	if (nResult == 0)
	{
		sprintf(pSqlDatabase->GetQuery(), "select reward_id,expire_time,type from activation_code_batch where batch=%d", nBatch);
		int nRet = pSqlDatabase->OpenRecordset();
		if (-1 == nRet)
		{
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			pQuery->SetQueryResultString(pSqlDatabase->error());
			MAPLOG(ERROR_LEVEL, "[%s]select activation_code_batch fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
			return;
		}

		if (pSqlDatabase->Fetch())
		{
			pSqlDatabase->GetDWORD(0, dwRewardId);
			pSqlDatabase->GetDWORD(1, dwExpireTime);
			pSqlDatabase->GetInt(2, nType);
		}
		else
		{
			nResult = 2;
		}
	}

	// 查询是否有领取了同一批次的激活码
	sprintf(pSqlDatabase->GetQuery(), "select * from activation_code_record where batch=%d and role_id=%d", nBatch, pQuery->GetRoleID());
	nRet = pSqlDatabase->OpenRecordset();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		pQuery->SetQueryResultString(pSqlDatabase->error());
		MAPLOG(ERROR_LEVEL, "[%s]select activation_code_batch fail[%s]", __FUNCTION__, pSqlDatabase->GetQuery());
		return;
	}
	if (pSqlDatabase->Fetch())
	{
		nResult = 3;
	}

	pQuery->SetResult(nResult);
	pQuery->SetStatus(nStatus);
	pQuery->SetBatch(nBatch);
	pQuery->SetType(nType);
	pQuery->SetRewardId(dwRewardId);
	pQuery->SetExpireTime(dwExpireTime);
	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 更新激活码数据
void DBThreadHandler::UpdateActivityCode(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	ActivationCode_Update* pQuery = (ActivationCode_Update*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		MAPLOG(ERROR_LEVEL, "[%s]pSqlDatabase->GetQuery() is Null", __FUNCTION__);
		return;
	}

	CHAR szTakeTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szTakeTime);

	// 更新
	sprintf(pSqlDatabase->GetQuery(), "Update activation_code_record Set status=1,take_time='%s',role_id=%d Where code='%s'", 
		szTakeTime, pQuery->GetRoleID(), pQuery->GetActivationCode());
	int nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		pQuery->SetQueryResultTable("UpdateActivityCode");
		MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

// 游戏统计事件
void DBThreadHandler::GameStatisticsEvent(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	CGameStatistics_Event* pQuery = (CGameStatistics_Event*)pData;

	if (!pSqlDatabase->GetQuery())
	{
		printf("[%s]pSqlDatabase->GetQuery() is Null\n", __FUNCTION__);
		return;
	}

	DWORD dwTime = pQuery->GetTime();
	WORD wType = pQuery->GetType();
	int nRet = 0;
	int nDateTime = g_Time2Date(dwTime);
	WORD wServerId = pQuery->GetServerId();

	//DWORD dwCurTickCount = GetTickCount64();

	// 保证有记录
	if (GAME_STATISTICS_NEW_DAY != wType)
	{
		sprintf(pSqlDatabase->GetQuery(), "Select * from statistics_server_detail where day_datetime=%d and server_id=%d",nDateTime, wServerId);
		nRet = pSqlDatabase->OpenRecordset();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("Insert Into server_day_report");
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			//MAPLOG(ERROR_LEVEL, "(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}

		// 如果木有则插进去
		if (!pSqlDatabase->Fetch())
		{
			sprintf(pSqlDatabase->GetQuery(), "Insert Into statistics_server_detail(day_datetime, server_id) VALUE ('%d','%d')",nDateTime, wServerId);
			nRet = pSqlDatabase->Execute();
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			if (nRet == -1)
			{
				pQuery->SetQueryResultString(pSqlDatabase->error());
				pQuery->SetQueryResultTable("Insert Into server_day_report");
				printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
				//MAPLOG(ERROR_LEVEL, "(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
				return;
			}
		}
	}

	// 每日事件
	if (GAME_STATISTICS_NEW_DAY == wType)
	{
		// 单服
		WORD wServerNum = pQuery->GetServerNum();
		for (WORD i = 0; i < wServerNum; ++i)
		{
			sprintf(pSqlDatabase->GetQuery(), "Insert Into statistics_server_detail(day_datetime, server_id) VALUE ('%d','%d')",
				nDateTime, pQuery->GetParam(i));
			nRet = pSqlDatabase->Execute();
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			if (nRet == -1)
			{
				pQuery->SetQueryResultString(pSqlDatabase->error());
				pQuery->SetQueryResultTable("Insert Into server_day_report");
				printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			}
		}


		// 今日新角数量,活跃人数,充值额度,充值人数,新角色付费人数,付费额度
		int nDateKey = g_Time2Date(dwTime - g_Day2Second(1)); // 因为收到数据时已经属于新的一天
		sprintf(pSqlDatabase->GetQuery(), "SELECT sum(create_role_num) as create_role_num,\
										  sum(active_num) as active_num,\
										  sum(amount) as amount,\
										  sum(recharge_role_num) as recharge_role_num,\
										  sum(new_role_payment_num) as new_role_payment_num,\
										  sum(new_role_payment_amount) as new_role_payment_amount,\
										  sum(first_payment_amount) as first_payment_amount,\
										  sum(first_payment_role_num) as first_payment_role_num,\
										  sum(total_online_num) as total_online_num,\
										  sum(online_statistics_times) as online_statistics_times\
										  FROM statistics_server_detail where day_datetime = %d", nDateKey); 
		nRet = pSqlDatabase->OpenRecordset();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("Insert Into server_day_report");
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}

		int nCreateRoleNum = 0;			// 创角数量
		int nActiveRoleNum = 0;			// 活跃数量
		int nRechargeAmount = 0;		// 充值额度
		int nPaymentRoleNum = 0;		// 充值人数
		int nNewRolePaymentNum = 0;		// 新角充值人数
		int nNewRolePaymentAmount = 0;	// 新角充值额度
		int nFirstPaymentAmount = 0;	// 首次充值额度
		int nFirstPaymentRoleNum = 0;	// 首次充值人数
		int nTotalOnlineNum = 0;
		int nOnlineStatisticsTimes = 0;
		if (pSqlDatabase->Fetch()){
			int i = 0;
			pSqlDatabase->GetInt(i++, nCreateRoleNum);
			pSqlDatabase->GetInt(i++, nActiveRoleNum);
			pSqlDatabase->GetInt(i++, nRechargeAmount);
			pSqlDatabase->GetInt(i++, nPaymentRoleNum);
			pSqlDatabase->GetInt(i++, nNewRolePaymentNum);
			pSqlDatabase->GetInt(i++, nNewRolePaymentAmount);
			pSqlDatabase->GetInt(i++, nFirstPaymentAmount);
			pSqlDatabase->GetInt(i++, nFirstPaymentRoleNum);
			pSqlDatabase->GetInt(i++, nTotalOnlineNum);
			pSqlDatabase->GetInt(i++, nOnlineStatisticsTimes);
		}

		int nAverageOnlineRoleNum = g_DivideBy(nTotalOnlineNum, nOnlineStatisticsTimes);
		int nMaxOnlineRoleNum = pQuery->GetContext();

		// 插入记录
		sprintf(pSqlDatabase->GetQuery(),
			"Insert Into server_day_report(day_time,create_role_num,active_role_num,payment_amount,payment_role_num,new_role_payment_num,\
									new_role_payment_amount,first_payment_amount,first_payment_role_num,average_online_role_num,max_online_role_num)\
									VALUE ('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d')",
									nDateKey, nCreateRoleNum, nActiveRoleNum, nRechargeAmount, nPaymentRoleNum, nNewRolePaymentNum, 
									nNewRolePaymentAmount, nFirstPaymentAmount, nFirstPaymentRoleNum, nAverageOnlineRoleNum, nMaxOnlineRoleNum);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("Insert Into server_day_report");
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}

		// 更新次日留存 
		DBThreadHandler::UpdateRetentionRate(pSqlDatabase, pData, dwTime - g_Day2Second(2), 1, "retention_rate_1");

		// 更新三日留存
		DBThreadHandler::UpdateRetentionRate(pSqlDatabase, pData, dwTime - g_Day2Second(4), 3, "retention_rate_2");

		// 更新七日留存
		DBThreadHandler::UpdateRetentionRate(pSqlDatabase, pData, dwTime - g_Day2Second(8), 7, "retention_rate_3");
	}
	// 新建角色
	else if (GAME_STATISTICS_CREATE_ROLE == wType)
	{
		// 更新总的创建角色数量
		// var sql='UPDATE server_list SET role_num = role_num + 1 where server_id = ?';
		sprintf(pSqlDatabase->GetQuery(), "UPDATE server_list Set role_num=role_num + 1 Where server_id=%d", wServerId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("UPDATE server_list");
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			//MAPLOG(ERROR_LEVEL, "(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}

		// 更新单个服每日的角色数量 todo::
		// 'UPDATE statistics_server_detail SET create_role_num = create_role_num + 1 where day_datetime = ? and server_id = ?'
		sprintf(pSqlDatabase->GetQuery(),
			"UPDATE statistics_server_detail Set create_role_num=create_role_num + 1 Where day_datetime=%d and server_id=%d",
			nDateTime, wServerId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("UPDATE server_list Set create_role_num");
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			//MAPLOG(ERROR_LEVEL, "(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}
	}
	// 登录
	else if (GAME_STATISTICS_LOGIN == wType)
	{
		// 记录活跃用户
		// var sql = 'UPDATE statistics_server_detail SET active_num = active_num + 1 where day_datetime = ? and server_id = ?';
		sprintf(pSqlDatabase->GetQuery(),
			"UPDATE statistics_server_detail Set active_num=active_num + 1 Where day_datetime=%d and server_id=%d",
			nDateTime, wServerId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("UPDATE statistics_server_detail Set active_num=active_num + 1");
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}
	}
	// 充值
	else if (GAME_STATISTICS_RECHARGE == wType)
	{
		int nRechargeAmount = pQuery->GetParam(1);
		int nIsTodayFirstPayment = pQuery->GetParam(2);
		int nIsNewRole = pQuery->GetParam(3);
		int nIsFirstPayment = pQuery->GetParam(4);

		int nRechargeRoleNum = 0;
		// 是否今日第一次充值
		if (nIsTodayFirstPayment){
			nRechargeRoleNum = 1;
		}

		int nRewRolePaymentAmount = 0;
		int nNewRolePaymentNum = 0;
		// 今日新角色付费
		if (nIsNewRole){
			nRewRolePaymentAmount = nRechargeAmount;

			// 今日新角首次充值
			if (nIsFirstPayment){
				nNewRolePaymentNum = 1;
			}
		}

		int nFirstPaymentAmount = 0;
		int nFirstPaymentRoleNum = 0;
		// 是否首充
		if (nIsFirstPayment){
			nFirstPaymentAmount = nRechargeAmount;
			nFirstPaymentRoleNum = 1;
		}

		// 更新单服日服
		sprintf(pSqlDatabase->GetQuery(),
			"UPDATE statistics_server_detail Set amount=amount + %d,recharge_role_num=recharge_role_num+%d,\
			new_role_payment_amount=new_role_payment_amount+%d,new_role_payment_num=new_role_payment_num+%d,\
			first_payment_amount=first_payment_amount+%d,first_payment_role_num=first_payment_role_num+%d\
			Where day_datetime=%d and server_id=%d",
			nRechargeAmount, nRechargeRoleNum,
			nRewRolePaymentAmount, nNewRolePaymentNum,
			nFirstPaymentAmount, nFirstPaymentRoleNum,
			nDateTime, wServerId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}

		// 更新单服总数据
		sprintf(pSqlDatabase->GetQuery(), "UPDATE server_list Set amount=amount + %d,recharge_role_num=recharge_role_num+%d Where server_id=%d",
			nRechargeAmount, nRechargeRoleNum, wServerId);
		nRet = pSqlDatabase->Execute();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			pQuery->SetQueryResultTable("UPDATE server_list Set amount");
			MAPLOG(GUI_LEVEL, pSqlDatabase->GetQuery());
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}
	}
	// 在线
	else if (GAME_STATISTICS_ONLINE == wType)
	{
		sprintf(pSqlDatabase->GetQuery(), "Select max_online_num, min_online_num from statistics_server_detail where day_datetime=%d and server_id=%d", 
			nDateTime, wServerId);
		nRet = pSqlDatabase->OpenRecordset();
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		if (nRet == -1)
		{
			pQuery->SetQueryResultString(pSqlDatabase->error());
			printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
			return;
		}

		// 如果木有则插进去
		if (pSqlDatabase->Fetch())
		{
			int nCurOnlineNum = pQuery->GetParam(0);
			int nMaxOnlineNumInDB = 0;
			int nMinOnlineMumInDB = 0;
			pSqlDatabase->GetInt(0, nMaxOnlineNumInDB);
			pSqlDatabase->GetInt(1, nMinOnlineMumInDB);

			// 因为数据库初始的时候为0,防止永远为0
			if (nMinOnlineMumInDB == 0)
				nMinOnlineMumInDB = nCurOnlineNum;

			nMaxOnlineNumInDB = max(nMaxOnlineNumInDB, nCurOnlineNum);
			nMinOnlineMumInDB = min(nMinOnlineMumInDB, nCurOnlineNum);

			// 更新单服日服
			sprintf(pSqlDatabase->GetQuery(),
				"UPDATE statistics_server_detail Set total_online_num=total_online_num + %d,online_statistics_times=online_statistics_times+1,\
				max_online_num=%d,min_online_num=%d,cur_online_num=%d Where day_datetime=%d and server_id=%d",
				nCurOnlineNum, nMaxOnlineNumInDB, nMinOnlineMumInDB, nCurOnlineNum, nDateTime, wServerId);
			nRet = pSqlDatabase->Execute();
			pQuery->SetQueryResult(pSqlDatabase->errorno());
			if (nRet == -1)
			{
				pQuery->SetQueryResultString(pSqlDatabase->error());
				printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
				return;
			}
		}
	}
	else
	{
		printf("[%s]未处理的数据统计类型[type:%d]\n", __FUNCTION__, wType);
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();

	//DWORD dwCostTime = GetTickCount64() - dwCurTickCount;
	//printf("Do GameStatisticsEvent Cost[Type:%d,CostTime:%d]\n", wType, dwCostTime);
}

// 游戏统计事件
void DBThreadHandler::GMOperationRecordSave(SqlDatabase* pSqlDatabase, QueryResult * pData)
{
	CGMOperationRecord_Save* pQuery = (CGMOperationRecord_Save*)pData;

	if (!pSqlDatabase->GetQuery())
		return;

	// 当前时间
	char szInsertTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(pQuery->GetTime());
	CurTime.toString(szInsertTime);

	sprintf(pSqlDatabase->GetQuery(), "insert Into gm_operation_record(gm_cmd,param1,param2,param3,param4,param5,record_time)\
									   values(%d,%d,%d,%d,%d,%d,'%s')",pQuery->GetGMCmd(), pQuery->GetParam(0), pQuery->GetParam(1), pQuery->GetParam(2),
									   pQuery->GetParam(3),pQuery->GetParam(4), szInsertTime);
	int nRet = pSqlDatabase->Execute();
	if (-1 == nRet)
	{
		pQuery->SetQueryResult(pSqlDatabase->errorno());
		return;
	}

	pQuery->SetResultRowNum(nRet);
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	pSqlDatabase->CloseRecordset();
}

void DBThreadHandler::UpdateRetentionRate(SqlDatabase* pSqlDatabase, QueryResult * pQuery, DWORD dwCreateRoleStartTime, DWORD Days, const char *pszKeyName)
{
	// 总服
	DATETIME dt1(dwCreateRoleStartTime);	
	DATETIME dt2(dwCreateRoleStartTime + g_Day2Second(1));
	DATETIME dt3(dwCreateRoleStartTime + g_Day2Second(Days));
	char szCreateRoleStartTime[32] = { 0 };
	char szCreateRoleOverTime[32] = { 0 };
	char szLastUpdateTime[32] = { 0 };
	dt1.toString(szCreateRoleStartTime);
	dt2.toString(szCreateRoleOverTime);
	dt3.toString(szLastUpdateTime);

	// 查询创角数量
	sprintf(pSqlDatabase->GetQuery(), "SELECT count(*) as create_role_num FROM role where create_time >= '%s' and create_time < '%s'",
		szCreateRoleStartTime, szCreateRoleOverTime);
	int nRet = pSqlDatabase->OpenRecordset();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
		return;
	}
	int nCreateRoleNum = 0;
	if (pSqlDatabase->Fetch()){
		pSqlDatabase->GetInt(0, nCreateRoleNum);
	}

	// 查询回归数量
	sprintf(pSqlDatabase->GetQuery(),
		"SELECT count(*) as return_role_num FROM role where create_time >= '%s' and create_time < '%s' and last_update_time >= '%s'",
		szCreateRoleStartTime, szCreateRoleOverTime, szLastUpdateTime);
	nRet = pSqlDatabase->OpenRecordset();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
		return;
	}
	int nReturnRoleNum = 0;
	if (pSqlDatabase->Fetch()){
		pSqlDatabase->GetInt(0, nReturnRoleNum);
	}
	
	float fRetention = (nReturnRoleNum > 0 && nCreateRoleNum > 0) ? (float(nReturnRoleNum) / float(nCreateRoleNum))*100 : 0;

	// 更新留存
	int nDateKey = g_Time2Date(dwCreateRoleStartTime);
	sprintf(pSqlDatabase->GetQuery(), "UPDATE server_day_report Set %s=%.2f Where day_time=%d", pszKeyName, fRetention, nDateKey);
	nRet = pSqlDatabase->Execute();
	pQuery->SetQueryResult(pSqlDatabase->errorno());
	if (nRet == -1)
	{
		pQuery->SetQueryResultString(pSqlDatabase->error());
		printf("(result:%d) %s\n", nRet, pSqlDatabase->GetQuery());
	}
}