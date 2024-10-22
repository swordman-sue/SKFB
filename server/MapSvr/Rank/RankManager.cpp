#include "stdafx.h"
#include "RankManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Arena\ArenaManager.h"
#include "..\MapServer.h"
#include "..\PlayerBaseDataManager.h"
#include "..\WorldBoss\WorldBossManager.h"
#include "data_define.pb.h"
#include "..\MapSvrQuery.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\Title\TitleManager.h"
#include "..\Activity\ActivityManager.h"

void FillRoleRankData(Player *pPlayer, RoleRankData &sRankData)
{
	memset(&sRankData, 0, sizeof(sRankData));
	sRankData.dwRoleId = pPlayer->GetCharID();
	sRankData.wLevel = pPlayer->GetLevel();
	sRankData.dwServerId = pPlayer->GetServerId();
	sRankData.dwUpdateTime = time(NULL);
}

CRankManager::CRankManager()
{
	m_lProceesSaveTime = GetTickCount64();
// 	m_EndlessLandDailyRankRewardTimer.AddTriggerTime(g_GetConfigValue(194));
// 	m_EndlessLandWeekRankRewardTimer.AddTriggerTime(g_GetConfigValue(199));
// 	m_EndlessLandWeekRankRewardTimer.SetWeekDay(g_GetConfigValue(198));
// 
// 	int nRewardDay = CActivityManager::GetActivityRewardDate(SYSTEM_SEVEN_DAY_RANK,1,0);
// 	int nRewardTime = CActivityManager::GetActivityRewardTime(SYSTEM_SEVEN_DAY_RANK,1,0);
// 	m_SevenDayRankRewardTimer.SetOpenServerDay(nRewardDay);
// 	m_SevenDayRankRewardTimer.AddTriggerTime(nRewardTime);
// 
// 	m_wRechargeRankCurDataId = 0;
}

CRankManager::~CRankManager()
{

}

void CRankManager::Init()
{
	m_lProceesSaveTime = GetTickCount64();

	m_EndlessLandDailyRankRewardTimer.AddTriggerTime(g_GetConfigValue(194));
	m_EndlessLandWeekRankRewardTimer.AddTriggerTime(g_GetConfigValue(199));
	m_EndlessLandWeekRankRewardTimer.SetWeekDay(g_GetConfigValue(198));

	int nRewardDay = CActivityManager::GetActivityRewardDate(SYSTEM_SEVEN_DAY_RANK, 1, 0);
	int nRewardTime = CActivityManager::GetActivityRewardTime(SYSTEM_SEVEN_DAY_RANK, 1, 0);
	m_SevenDayRankRewardTimer.SetOpenServerDay(nRewardDay);
	m_SevenDayRankRewardTimer.AddTriggerTime(nRewardTime);

	InitRechargeRankRewardTime();
}

void CRankManager::ConfigReload()
{
	InitRechargeRankRewardTime();
}

void CRankManager::InitRechargeRankRewardTime()
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(309);
	if (!pServerCfg)
		return;

	if (pServerCfg->vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]充值排行榜奖励时间配置出错[size:%d]",
			__FUNCTION__, pServerCfg->vValueList.size());
		return;
	}

	m_RechargeRankRewardTimer.Reset();

	DWORD dwDate = pServerCfg->GetListValue(0); // 20190125
	DWORD dwTime = pServerCfg->GetListValue(1);	// 2330

	int nYear = SplitInteger(dwDate, 8, 0, 4);
	int nMonth = SplitInteger(dwDate, 8, 4, 2);
	int nDay = SplitInteger(dwDate, 8, 6, 2);

	int nHour = SplitInteger(dwTime, 4, 0, 2);
	int nMin = SplitInteger(dwTime, 4, 2, 2);

	// todo::可能有BUG
	DWORD dwTriggerTime = DATETIME(nYear, nMonth, nDay, nHour, nMin, 0);

	m_RechargeRankRewardTimer.AddTriggerTime(dwTriggerTime);
}

void CRankManager::OnNewDay()
{
	// 处理功勋奖励
	DoExploitRankReward();

	// 处理伤害奖励
	DoDamageRankReward();
}

void CRankManager::Update()
{
	// todo::测试
	//return;

	if (GetTickCount64() >= m_lProceesSaveTime + 60000)
	{
		for (BYTE i = 0; i < RANK_ROLE_MAX; ++i)
		{
			if (i == RANK_ARENA)
				continue;

			SaveDataToDB(i);
		}

		m_lProceesSaveTime = GetTickCount64();
	}

	// 无尽之地排行奖励(每日奖励)
	if (m_EndlessLandDailyRankRewardTimer.IsTrigger())
	{
		OnEndlessLandDailyRankReward();
		m_EndlessLandDailyRankRewardTimer.SetCDOverTime();
	}

	// 无尽之地排行奖励(每周)
	if (m_EndlessLandWeekRankRewardTimer.IsTrigger())
	{
		OnEndlessLandWeekRankReward();
		m_EndlessLandWeekRankRewardTimer.SetCDOverTime();
	}

	// 取服务器列表
	bool bIsSendSevenDayRankReward = false;
	const map<DWORD, ServerInfo> &mapServerList = g_MapServer.GetServerList();
	for (map<DWORD, ServerInfo>::const_iterator iter = mapServerList.begin(); iter != mapServerList.end(); ++iter)
	{
		DWORD wOpenDay = g_MapServer.GetAlreadyOpenServerDay(iter->second.dwServerId);
		if (m_SevenDayRankRewardTimer.IsTrigger(wOpenDay))
		{
			OnSevenDayRankReward(iter->second.dwServerId);
			bIsSendSevenDayRankReward = true;
		}
	}
	if (bIsSendSevenDayRankReward)
		m_SevenDayRankRewardTimer.SetCDOverTime();

	if (m_RechargeRankRewardTimer.IsTriggerEx())
	{
		DoRechargeRankReward();
		m_RechargeRankRewardTimer.SetCDOverTime();
	}
}


void CRankManager::SaveAllDataToDB()
{
	for (BYTE i = 0; i < RANK_ROLE_MAX; ++i)
	{
		if (i == RANK_ARENA)
			continue;

		SaveDataToDB(i);
	}
}

void CRankManager::SaveDataToDB(BYTE byRankType)
{
	AllRoleRankList *pTypeRankList = GetRankList(byRankType);
	if (!pTypeRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]未处理的排行榜类型[type:%d]", __FUNCTION__, byRankType);
		return;
	}

	DWORD dwRankNum = 0;

	PB::all_server_role_rank_list AllServerRankList;
	AllServerRankList.set_rank_type(byRankType);
	map<WORD, RoleRankList>::iterator iter = pTypeRankList->begin();
	for (; iter != pTypeRankList->end(); ++iter)
	{
		PB::role_rank_list *pPBRankList = AllServerRankList.add_all_rank_list();
		pPBRankList->set_server_id(iter->first);
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			RoleRankData *pRankData = &iter->second[i];
			PB::role_rank_data *pPBRankData = pPBRankList->add_rank_list();
			pPBRankData->set_role_id(pRankData->dwRoleId);
			pPBRankData->set_level(pRankData->wLevel);
			pPBRankData->set_rank(pRankData->wRank);
			pPBRankData->set_value(pRankData->dwValue);
			pPBRankData->set_param1(pRankData->dwParam1);
			pPBRankData->set_param2(pRankData->dwParam2);
			pPBRankData->set_update_time(pRankData->dwUpdateTime);
			pPBRankData->set_server_id(pRankData->dwServerId);
		}
		dwRankNum += iter->second.size();
	}

	string strSaveData("");
	AllServerRankList.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_ROLE_RANK_LIST_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]排行榜存储数据过大[rank_type:%d, size:%d]", 
			__FUNCTION__, byRankType, strSaveData.size());
		return;
	}

	if (strSaveData.size())
	{
		RoleRankList_Save* pQuery = RoleRankList_Save::ALLOC();
		pQuery->SetIndex(DB_ROLE_RANK_DATA_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_RANK);
		pQuery->SetRankType(byRankType);
		pQuery->SetRankNum(dwRankNum);
		pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
		g_MapServer.MapDBQuery(pQuery);
	}
}

void CRankManager::LoadDataFromDB(BYTE byRankType, const char *pData, int size)
{
	if (!size)
		return;

	PB::all_server_role_rank_list PBAllServerRankList;
	if (!PBAllServerRankList.ParseFromArray(pData, size))
	{
		MAPLOG(ERROR_LEVEL, "解析排行数据出错(rank_type:%d, size:%d)", byRankType, size);
		return;
	}


	// 是否世界BOSS排行
	bool bIsWorldBossRank = false;
	if (RANK_WORLD_BOSS_HONOR == byRankType || RANK_WORLD_BOSS_DAMAGE == byRankType)
		bIsWorldBossRank = true;

	AllRoleRankList *pTypeRankList = NULL;
	if (!bIsWorldBossRank)
	{
		pTypeRankList = GetRankList(byRankType);
		if (!pTypeRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]未处理的排行榜类型[type:%d]", __FUNCTION__, byRankType);
			return;
		}
		// 先清空数据
		pTypeRankList->clear();
	}

	RoleRankData tRankData;
	for (int i = 0; i < PBAllServerRankList.all_rank_list_size(); ++i)
	{
		RoleRankList vRankList;
		const PB::role_rank_list &PBAllRankList = PBAllServerRankList.all_rank_list(i);
		for (int k = 0; k < PBAllRankList.rank_list_size(); ++k)
		{
			const PB::role_rank_data &PBRankData = PBAllRankList.rank_list(k);
			tRankData.dwRoleId = PBRankData.role_id();
			tRankData.wLevel = PBRankData.level();
			tRankData.wRank = PBRankData.rank();
			tRankData.dwValue = PBRankData.value();
			tRankData.dwParam1 = PBRankData.param1();
			tRankData.dwParam2 = PBRankData.param2();
			tRankData.dwUpdateTime = PBRankData.update_time();
			tRankData.dwServerId = PBRankData.server_id();
			vRankList.push_back(tRankData);
		}

		if (bIsWorldBossRank)
			CWorldBossManager::Instance()->AddRankData(byRankType, PBAllRankList.server_id(), vRankList);
		else
			pTypeRankList->insert(make_pair(PBAllRankList.server_id(), vRankList));
	}
}

// 加载数据请求
void CRankManager::LoadDataRequest()
{
	for (BYTE i = 0; i < RANK_ROLE_MAX; ++i)
	{
		if (i == RANK_ARENA)
			continue;

		// 请求
		RoleRankList_Load* pQuery = RoleRankList_Load::ALLOC();
		pQuery->SetIndex(DB_ROLE_RANK_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_RANK);
		pQuery->SetRankType(i);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 合服
void CRankManager::OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId)
{
	// 角色排行榜
	RoleRankData tRankData;
	for (int nRankType = RANK_BATTLE_VALUE; nRankType < RANK_ROLE_MAX; ++nRankType)
	{
		MAPLOG(ERROR_LEVEL, "[%s]正在处理排行榜合服[type:%d]", __FUNCTION__, nRankType);

		// 世界boss排行暂时不处理
		if (RANK_WORLD_BOSS_HONOR == nRankType || RANK_WORLD_BOSS_DAMAGE == nRankType)
			continue;

		AllRoleRankList *pRankList = GetRankList(nRankType);
		if (!pRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜数据[type:%d]", __FUNCTION__, nRankType);
			continue;
		}

		// 竞技场, 全服PVP不需要处理
		if (nRankType != RANK_PVP_ALL && nRankType != RANK_ARENA)
		{
			// 旧服玩家排行榜数据添加到新服
			AllRoleRankList::iterator list_iter = pRankList->find(dwFromServerId);
			if (list_iter != pRankList->end())
			{
				RoleRankList::iterator role_iter = list_iter->second.begin();
				for (; role_iter != list_iter->second.end(); ++role_iter)
				{
					//MAPLOG(ERROR_LEVEL, "[%s]正在处理排行榜玩家数据合并[type:%d,roleid:%d]", __FUNCTION__, nRankType, role_iter->dwRoleId);
					//memcpy_s(&tRankData, sizeof(RoleRankData), &*role_iter, sizeof(RoleRankData));
					tRankData = *role_iter;
					tRankData.dwServerId = dwToServerId;
					AddRoleRankDataEx(nRankType, tRankData);
				}

				// 清空旧服数据
				if (list_iter != pRankList->end())
					list_iter->second.clear();
			}
		}
	}

	// 工会排行榜
	for (int nRankType = GUILD_RANK_LEVEL; nRankType < GUILD_RANK_MAX; ++nRankType)
	{
		GuildRankList *pRankList = GetGuildRankList(dwFromServerId, nRankType);
		if (pRankList)
		{
			GuildRankList::iterator iter = pRankList->begin();
			for (; iter != pRankList->end(); ++iter)
			{
				AddGuildRankDataEx(dwToServerId, iter->dwGuildId, nRankType, iter->dwValue);
			}
			pRankList->clear();
		}
	}
}

// 请求排行榜列表 
void CRankManager::OnNetRankList(Player *pPlayer, BYTE byRankType, BYTE bySubType)
{
	//DWORD dwServerId = RANK_PVP_ALL != byRankType ? pPlayer->GetServerId() : ALL_SERVER_ID;

	DWORD dwServerId = pPlayer->GetServerId();
	if (IsAllServerRank(byRankType))
	{
		dwServerId = ALL_SERVER_ID;
	}
	// 积分赛排行榜(服务器组为作一个排行榜)
	else if (RANK_SCORE_MATCH_SUPER == byRankType || RANK_SCORE_MATCH_LEGEND == byRankType || RANK_SCORE_MATCH_WARCRAFT == byRankType)
	{
		const ScoreMatchServerGroup_Config *pServerGroupCfg = g_pCfgMgr->GetScoreMatchTargetServerList(dwServerId);
		if (!pServerGroupCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到积分赛服务器组信息[ServerId:%d,Type:%d]", __FUNCTION__, dwServerId, byRankType);
			return;
		}
		// 这个不是真正的服务器ID, 是服务器组排行ID
		dwServerId = pServerGroupCfg->wRankGroupId;
	}
	// 工会战
	else if (RANK_GUILD_WAR == byRankType)
	{
		const GuildWarRegion_Config *pRegionCfg = g_pCfgMgr->GetGuildWarRegionByServerId(dwServerId);
		if (!pRegionCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到工会战大区配置[ServerId:%d]", __FUNCTION__, dwServerId);
			return;
		}

		// 这个是区的ID
		dwServerId = pRegionCfg->wRegionId;
	}

	//MAPLOG(ERROR_LEVEL, "收到排行榜请求[type:%d]", byRankType);
	WORD wRank = 0;
	DWORD dwRankValue = 0;
	RoleRankList *pRankList = NULL;
	if (RANK_ARENA == byRankType)
	{
		RoleRankData tRankData;
		m_TempRankList.clear();
		vector<ArenaRankData> vOutRankList;
		CArenaManager::Instance()->GetRankList(dwServerId, pPlayer->GetRoleId(), &vOutRankList, 50);
		for (vector<ArenaRankData>::iterator iter = vOutRankList.begin(); iter != vOutRankList.end(); ++iter)
		{
			memset(&tRankData, 0, sizeof(tRankData));
			tRankData.dwRoleId = iter->dwTargetId;
			tRankData.wRank = iter->wCurRank;
			tRankData.dwValue = iter->wCurRank;
			tRankData.dwServerId = dwServerId;
			m_TempRankList.push_back(tRankData);
		}
		pRankList = &m_TempRankList;

		ArenaRankData *pRankData = CArenaManager::Instance()->GetPlayerRankData(dwServerId, pPlayer->GetRoleId());
		if (pRankData)
		{
			wRank = pRankData->wCurRank;
			dwRankValue = 0;
		}
	}
	// 世界BOSS排行
	else if (RANK_WORLD_BOSS_HONOR == byRankType || RANK_WORLD_BOSS_DAMAGE == byRankType)
	{
		pRankList = CWorldBossManager::Instance()->GetRankList(pPlayer, byRankType, bySubType);

		const RoleRankData *pRankData = CWorldBossManager::Instance()->GetPlayerRankData(pPlayer, byRankType);
		if (pRankData)
		{
			wRank = pRankData->wRank;
			dwRankValue = pRankData->dwValue;
		}
	}
	else
	{
		pRankList = GetServerRoleRankList(dwServerId, byRankType);

		// 请求者自己的排行榜数据
		const RoleRankData *pRankData = GetRoleRankData(dwServerId, byRankType, pPlayer->GetRoleId());
		if (pRankData)
		{
			wRank = pRankData->wRank;
			dwRankValue = pRankData->dwValue;
		}
	}
	if (!pRankList)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_RANK_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]取不到排行榜数据[ServerId:%d,Type:%d]", __FUNCTION__, dwServerId, byRankType);
		return;
	}

	// 特殊逻辑处理
	if (!wRank)
	{
		if (byRankType == RANK_LEVEL)
		{
			dwRankValue = pPlayer->GetLevel();
		}
		else if (byRankType == RANK_BATTLE_VALUE)
		{
			dwRankValue = pPlayer->GetFightingPower();
		}
		else if (byRankType == RANK_MAIN_FB_STAR)
		{
			dwRankValue = pPlayer->GetFubenManager()->GetMainFB().GetTotalStar();
		}
		else if (byRankType == RANK_DIAMOND_COST)
		{
			dwRankValue = pPlayer->GetActivityManager()->GetRechargeAndCostRankActivity().GetCostDiamond();
		}
		else if (byRankType == RANK_RECHARGE)
		{
			dwRankValue = pPlayer->GetActivityManager()->GetRechargeAndCostRankActivity().GetRechargeAmount();
		}
	}

	static MSG_S2C_RANK_LIST_RESP msg;
	msg.m_wRank = wRank;
	msg.m_dwRankValue = dwRankValue;
	msg.m_byType = byRankType;
	msg.m_bySubType = bySubType;
	msg.m_wListNum = 0;
	for (RoleRankList::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		// todo::为了兼容旧数据,暂时这样处理
		//DWORD dwRoleServerId = RANK_PVP_ALL != byRankType ? pPlayer->GetServerId() : iter->dwServerId;
// 		DWORD dwRoleServerId = pPlayer->GetServerId();
// 		if (RANK_PVP_ALL == byRankType || RANK_KINGMATCH == byRankType)
// 		{
// 			dwRoleServerId = g_MapServer.GetCurServerId(iter->dwServerId);
// 		}

		// 列表已满
		if (msg.m_wListNum >= MAX_RANK_MUM)
			break;

		// 取排行榜角色当前所在的服ID
		DWORD dwRoleServerId = g_MapServer.GetCurServerId(iter->dwServerId);

		// todo::防止部分排行出bug处理
		if (!dwRoleServerId)
			dwRoleServerId = pPlayer->GetServerId();

		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwRoleServerId, iter->dwRoleId);
		if (pBaseData)
		{
			ClientRoleRankData &tRankData = msg.m_arRankList[msg.m_wListNum++];
			tRankData = *pBaseData;
			tRankData.wRank = iter->wRank;
			tRankData.dwValue = iter->dwValue;
			tRankData.dwServerId = iter->dwServerId;
			tRankData.dwParam1 = iter->dwParam1;
			tRankData.dwParam2 = iter->dwParam2;
		}
		else
		{
			if (IsRobotId(iter->dwRoleId))
			{
				ClientRoleRankData &tRankData = msg.m_arRankList[msg.m_wListNum++];
				tRankData.dwRoleId = iter->dwRoleId;
				tRankData.wRank = iter->wRank;
				tRankData.dwValue = iter->dwValue;
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到玩家基本数据[ServerId:%d,RoleId:%d]", __FUNCTION__,
					pPlayer->GetServerId(), iter->dwRoleId);
			}
		}
	}
	pPlayer->SendPacket(&msg);
}

// 请求工会排行榜列表
void CRankManager::OnNetGuildRankList(Player *pPlayer, BYTE byRankType)
{
	GuildRankList *pRankList = GetGuildRankList(pPlayer->GetServerId(), byRankType);
	if (!pRankList)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_RANK_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]取不到工会排行榜列表[ServerId:%d,Type:%d]", __FUNCTION__,
			pPlayer->GetServerId(), byRankType);
		return;
	}

	static MSG_S2C_GUILD_RANK_LIST_RESP msg;
	msg.m_wRank = byRankType;
	msg.m_wListNum = 0;
	msg.m_wRank = 0;
	msg.m_dwRankValue = 0;

	for (GuildRankList::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildById(iter->dwGuildId);
		if (pGuild)
		{
			ClientGuildRankData &tRankData = msg.m_arRankList[msg.m_wListNum++];
			tRankData.dwGuildId = pGuild->GetGuildId();
			tRankData.wIconId = pGuild->GetGuildData().wIconId;
			tRankData.wLevel = pGuild->GetGuildLevel();
			tRankData.byMemberNum = pGuild->GetMemberNum();
			tRankData.dwValue = iter->dwValue;
			strcpy_s(tRankData.szGuildName, sizeof(tRankData.szGuildName)-1, pGuild->GetGuildName());
			strcpy_s(tRankData.szCaptainName, sizeof(tRankData.szCaptainName) - 1, pGuild->GetCaptainName());
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到工会数据[ServerId:%d,GuildId:%d]", __FUNCTION__,
				pPlayer->GetServerId(), iter->dwGuildId);
		}
	}

	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	GuildRankData *pRankData = dwGuildId ? GetGuildRankData(pPlayer->GetServerId(), byRankType, dwGuildId) : NULL;
	if (pRankData)
	{
		msg.m_wRank = pRankData->wRank;
		msg.m_dwRankValue = pRankData->dwValue;
	}

	pPlayer->SendPacket(&msg);
}

// 请求我的排名
void CRankManager::OnNetMyRank(Player *pPlayer, BYTE byRankType)
{
	MSG_S2C_MY_RANK_RESP msg;
	msg.m_wRankType = byRankType;
	msg.m_wMyRank = GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), byRankType);
	pPlayer->SendPacket(&msg);
}

// 删除某个玩家
void CRankManager::RemoveRoleRankData(BYTE byRankType, DWORD dwServerId, DWORD dwRoleId)
{
	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]", __FUNCTION__, dwServerId, byRankType);
		return;
	}


	for (size_t i = 0; i < pRankList->size(); i++)
	{
		if (pRankList->at(i).dwRoleId == dwRoleId)
		{
			pRankList->at(i) = pRankList->at(pRankList->size()-1);
			pRankList->pop_back();
			break;
		}
	}

	UpdateGuildRank(dwServerId, byRankType);
}

// 添加角色排行榜数据
void CRankManager::AddRoleRankData(BYTE byRankType, Player *pPlayer, DWORD dwValue, DWORD dwParam1, DWORD dwParam2, BYTE byMaxRankNum)
{
	// todo::暂时这样处理(王者赛积分有可能为0)
	if (RANK_KINGMATCH != byRankType && !dwValue)
		return;

	DWORD dwServerId = pPlayer->GetServerId();
	if (IsAllServerRank(byRankType))
	{
		dwServerId = ALL_SERVER_ID;
	}
	else if (RANK_SCORE_MATCH_SUPER == byRankType || RANK_SCORE_MATCH_LEGEND == byRankType || RANK_SCORE_MATCH_WARCRAFT == byRankType)
	{
		const ScoreMatchServerGroup_Config *pGroupServerCfg = g_pCfgMgr->GetScoreMatchTargetServerList(dwServerId);
		if (!pGroupServerCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到积分排行榜配置[serverid:%d, type:%d]", __FUNCTION__, dwServerId, byRankType);
			return;
		}

		// todo::配置的ID很有可能不是真正的服务器ID
		dwServerId = pGroupServerCfg->wRankGroupId;
	}
	// 工会战
	else if (RANK_GUILD_WAR == byRankType)
	{
		const GuildWarRegion_Config *pRegionCfg = g_pCfgMgr->GetGuildWarRegionByServerId(dwServerId);
		if (!pRegionCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到工会战大区配置[ServerId:%d]", __FUNCTION__, dwServerId);
			return;
		}

		// 这个是区的ID
		dwServerId = pRegionCfg->wRegionId;
	}

	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]",__FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	RoleRankData *pRankData = FindRoleRankData(pRankList, pPlayer->GetCharID());
	
	// 有则更新
	if (pRankData)
	{
		// 数据有变动才处理
		if (pRankData->dwValue != dwValue)
		{
			pRankData->wLevel = pPlayer->GetLevel();
			pRankData->dwValue = dwValue;
			pRankData->dwParam1 = dwParam1;
			pRankData->dwParam2 = dwParam2;
			pRankData->dwUpdateTime = time(NULL);
			bNeedSort = true;
		}
	}
	// 无则添加
	else
	{
		// 列表还没满，则直接添加
		if (pRankList->size() < byMaxRankNum)
		{
			RoleRankData sRankData;
			FillRoleRankData(pPlayer, sRankData);
			sRankData.dwValue = dwValue;
			sRankData.dwParam1 = dwParam1;
			sRankData.dwParam2 = dwParam2;
			sRankData.wRank = 0; // 新加入的没有排名
			pRankList->push_back(sRankData);
			bNeedSort = true;
		}
		else
		{
			// 列表已满，则需要跟后面一个对比决定能否添加
			RoleRankList::reference ref = pRankList->back();
			if (dwValue > ref.dwValue)
			{
				pRankList->pop_back();
				RoleRankData sRankData;
				FillRoleRankData(pPlayer, sRankData);
				sRankData.dwValue = dwValue;
				sRankData.dwParam1 = dwParam1;
				sRankData.dwParam2 = dwParam2;
				pRankList->push_back(sRankData);
				bNeedSort = true;
			}
		}
	}

	if (bNeedSort)
		UpdateRoleRank(dwServerId, byRankType);
}

// 添加角色排行榜数据(合服使用)
void CRankManager::AddRoleRankDataEx(BYTE byRankType, const RoleRankData &tRankData, WORD wMaxRankNum)
{
	DWORD dwServerId = tRankData.dwServerId;
	DWORD dwRoleId = tRankData.dwRoleId;
	DWORD dwValue = tRankData.dwValue;

	// todo::暂时这样处理(王者赛积分有可能为0)
	if (RANK_KINGMATCH != byRankType && !dwValue)
		return;

	if (IsAllServerRank(byRankType))
	{
		dwServerId = ALL_SERVER_ID;
	}

	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]", __FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	RoleRankData *pRankData = FindRoleRankData(pRankList, dwRoleId);
	// 有则更新
	if (pRankData)
	{
		// 数据有变动才处理
		if (pRankData->dwValue != dwValue)
		{
			pRankData->wLevel = tRankData.wLevel;
			pRankData->dwValue = tRankData.dwValue;
			pRankData->dwParam1 = tRankData.dwParam1;
			pRankData->dwParam2 = tRankData.dwParam2;
			pRankData->dwUpdateTime = time(NULL);
			bNeedSort = true;
		}
	}
	// 无则添加
	else
	{
		// 列表还没满，则直接添加
		if (pRankList->size() < wMaxRankNum)
		{
			pRankList->push_back(tRankData);
			bNeedSort = true;
		}
		else
		{
			// 列表已满，则需要跟后面一个对比决定能否添加
			RoleRankList::reference ref = pRankList->back();
			if (dwValue > ref.dwValue)
			{
				pRankList->pop_back();
				pRankList->push_back(tRankData);
				bNeedSort = true;
			}
		}
	}

	if (bNeedSort)
		UpdateRoleRank(dwServerId, byRankType);
}

// 取排行名
WORD CRankManager::GetRoleRank(DWORD dwRoleId, DWORD dwServerId, BYTE byRankType)
{
	WORD wRank = 0;
	if (byRankType == RANK_ARENA)
	{
		wRank = CArenaManager::Instance()->GetPlayerRank(dwServerId, dwRoleId);
	}
	else
	{
		RoleRankData *pRankData = GetRoleRankData(dwServerId, byRankType, dwRoleId);
		if (pRankData)
			wRank = pRankData->wRank;
	}
	return wRank;
}

// 取排行值
DWORD CRankManager::GetRoleRankValue(DWORD dwRoleId, DWORD dwServerId, BYTE byRankType)
{
	DWORD dwValue = 0;
	if (byRankType == RANK_ARENA)
	{
		//dwValue = CArenaManager::Instance()->GetPlayerRank(dwServerId, dwRoleId);
	}
	else
	{
		RoleRankData *pRankData = GetRoleRankData(dwServerId, byRankType, dwRoleId);
		if (pRankData)
			dwValue = pRankData->dwValue;
	}
	return dwValue;
}

// 添加工会排行榜数据
void CRankManager::AddGuildRankData(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum)
{
	if (!dwValue)
		return;

	//DWORD dwServerId = dwGuildId * 0.000001;

	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]",
			__FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	GuildRankData *pRankData = FindGuildRankData(pRankList, dwGuildId);

	// 有则更新
	if (pRankData)
	{
		// 数据有变动才处理
		if (pRankData->dwValue != dwValue)
		{
			pRankData->dwValue = dwValue;
			pRankData->dwUpdateTime = time(NULL);
			bNeedSort = true;
		}
	}
	// 无则添加
	else
	{
		// 列表还没满，则直接添加
		if (pRankList->size() < byMaxRankNum)
		{
			GuildRankData tRankData;
			tRankData.dwGuildId = dwGuildId;
			tRankData.dwUpdateTime = time(NULL);
			tRankData.dwValue = dwValue;
			tRankData.wRank = pRankList->size()+1;
			pRankList->push_back(tRankData);
			bNeedSort = true;
		}
		else
		{
			// 列表已满，则需要跟后面一个对比决定能否添加
			GuildRankList::reference ref = pRankList->back();
			if (dwValue > ref.dwValue)
			{
				pRankList->pop_back();
				GuildRankData tRankData;
				tRankData.dwGuildId = dwGuildId;
				tRankData.dwUpdateTime = time(NULL);
				tRankData.dwValue = dwValue;
				tRankData.wRank = pRankList->size()+1;
				pRankList->push_back(tRankData);
				bNeedSort = true;
			}
		}
	}

	if (bNeedSort)
		UpdateGuildRank(dwServerId, byRankType);
}

// 添加工会排行榜数据(合服使用)
void CRankManager::AddGuildRankDataEx(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum)
{
	if (!dwValue)
		return;

	//DWORD dwServerId = dwGuildId * 0.000001;

	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]",
			__FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	GuildRankData *pRankData = FindGuildRankData(pRankList, dwGuildId);

	// 有则更新
	if (pRankData)
	{
		// 数据有变动才处理
		if (pRankData->dwValue != dwValue)
		{
			pRankData->dwValue = dwValue;
			pRankData->dwUpdateTime = time(NULL);
			bNeedSort = true;
		}
	}
	// 无则添加
	else
	{
		// 列表还没满，则直接添加
		if (pRankList->size() < byMaxRankNum)
		{
			GuildRankData tRankData;
			tRankData.dwGuildId = dwGuildId;
			tRankData.dwUpdateTime = time(NULL);
			tRankData.dwValue = dwValue;
			tRankData.wRank = pRankList->size() + 1;
			pRankList->push_back(tRankData);
			bNeedSort = true;
		}
		else
		{
			// 列表已满，则需要跟后面一个对比决定能否添加
			GuildRankList::reference ref = pRankList->back();
			if (dwValue > ref.dwValue)
			{
				pRankList->pop_back();
				GuildRankData tRankData;
				tRankData.dwGuildId = dwGuildId;
				tRankData.dwUpdateTime = time(NULL);
				tRankData.dwValue = dwValue;
				tRankData.wRank = pRankList->size() + 1;
				pRankList->push_back(tRankData);
				bNeedSort = true;
			}
		}
	}

	if (bNeedSort)
		UpdateGuildRank(dwServerId, byRankType);
}

// 删除工会排行数据
void CRankManager::RemoveGuildRandData(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType)
{
	//DWORD dwServerId = dwGuildId * 0.000001;

	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]",__FUNCTION__, dwServerId, byRankType);
		return;
	}

	GuildRankList::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		if (iter->dwGuildId == dwGuildId)
		{
			pRankList->erase(iter);
			break;
		}
	}
}

// 取工会排名
WORD CRankManager::GetGuildRank(DWORD dwGuildId, DWORD dwServerId, BYTE byType)
{
	GuildRankData *pRankData = GetGuildRankData(dwServerId, byType, dwGuildId);
	return pRankData ? pRankData->wRank : 0;
}

// 取服务器角色排行榜列表
const RoleRankList* CRankManager::GetServerRoleRankListEx(WORD wServerId, BYTE byRankType)
{
	return GetServerRoleRankList(wServerId, byRankType);
}

// 取玩家排行榜数据
const RoleRankData* CRankManager::GetRoleRankDataEx(WORD dwServerId, BYTE byRankType, DWORD dwRoleId)
{
	return GetRoleRankData(dwServerId, byRankType, dwRoleId);
}

// 清空排行榜数据
void CRankManager::ResetRoleRank(WORD wServerId, BYTE byRankType)
{
	RoleRankList *pRankList = GetServerRoleRankList(wServerId, byRankType);
	if (pRankList)
		pRankList->clear();
}

// 清空排行榜数据
void CRankManager::ResetRoleRank(BYTE byRankType)
{
	AllRoleRankList *pRankList = GetRankList(byRankType);
	if (!pRankList)
		return;

	AllRoleRankList::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		iter->second.clear();
	}
}

// 清空服务器角色排行数据
void CRankManager::ResetServerRoleRankData(WORD wServerId)
{
	// 角色排行榜
	for (int i = RANK_BATTLE_VALUE; i < RANK_ROLE_MAX; ++i)
	{
		AllRoleRankList &tRankList = m_AllRoleRankList[i];
		AllRoleRankList::iterator iter = tRankList.find(wServerId);
		if (iter != tRankList.end())
		{
			iter->second.clear();
		}
	}

	// 工会排行榜
	for (int i = GUILD_RANK_LEVEL; i < GUILD_RANK_MAX; ++i)
	{
		AllGuildRankList &tRankList = m_AllGuildRankList[i];
		AllGuildRankList::iterator iter = tRankList.find(wServerId);
		if (iter != tRankList.end())
		{
			iter->second.clear();
		}
	}
}

// 更新角色排行榜
void CRankManager::UpdateRoleRankEx(WORD wServerId, BYTE byRankType)
{
	UpdateRoleRank(wServerId, byRankType);
}

// 
bool RoleRankDataPred(RoleRankData &data1, RoleRankData &data2)
{
	// 第一排序值不相等
	if (data1.dwValue > data2.dwValue)
		return true;

	// 第一排序值相等
	if (data1.dwValue == data2.dwValue && data1.dwParam1 > data2.dwParam1)
		return true;

	// 第一,二排序值不相等
	if (data1.dwValue == data2.dwValue && data1.dwParam1 == data2.dwParam1 && data1.dwUpdateTime < data2.dwUpdateTime)
		return true;

	return false;
}

bool GuildRankDataPred(GuildRankData &data1, GuildRankData &data2)
{
	if (data1.dwValue > data2.dwValue)
		return true;

	if (data1.dwValue == data2.dwValue && data1.dwUpdateTime < data2.dwUpdateTime)
		return true;

	return false;
}

// 更新排行榜
void CRankManager::UpdateRoleRank(WORD wServerId, BYTE byRankType)
{
	// 排序
	RoleRankList *pRankList = GetServerRoleRankList(wServerId, byRankType);
	if (pRankList)
	{
		std::sort(pRankList->begin(), pRankList->end(), RoleRankDataPred);
	}

	// 更新排名
	pRankList = GetServerRoleRankList(wServerId, byRankType);
	if (pRankList)
	{
		WORD wRank = 0;
		vector<RoleRankData>::iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			iter->wRank = ++wRank;
		}
	}
}


// 取玩家排行榜数据
RoleRankData* CRankManager::GetRoleRankData(WORD dwServerId, BYTE byRankType, DWORD dwRoleId)
{
	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[serverid:%d, type:%d]",__FUNCTION__, dwServerId,byRankType);
		return NULL;
	}

	return FindRoleRankData(pRankList, dwRoleId);
}

// 取服务器爬塔排行榜列表 
RoleRankList* CRankManager::GetServerRoleRankList(WORD wServerId, BYTE byRankType)
{
	AllRoleRankList *pTypeRankList = GetRankList(byRankType);
	if (!pTypeRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]未处理的排行榜类型[type:%d]", __FUNCTION__, byRankType);
		return NULL;
	}

	// 全服排行榜的服务器ID为0
	if (IsAllServerRank(byRankType))
		wServerId = ALL_SERVER_ID;

	AllRoleRankList::iterator iter = pTypeRankList->find(wServerId);
	if (iter == pTypeRankList->end())
	{
		pTypeRankList->insert(make_pair(wServerId, RoleRankList()));
		iter = pTypeRankList->find(wServerId);
	}

	return &iter->second;
}

// 查找玩家排行榜数据
RoleRankData* CRankManager::FindRoleRankData(RoleRankList *pRankList, DWORD dwRoleId)
{
	vector<RoleRankData>::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); iter++)
	{
		if (iter->dwRoleId == dwRoleId)
		{
			return &*iter;
		}
	}
	return NULL;
}

// 取排行榜列表
AllRoleRankList* CRankManager::GetRankList(BYTE byRankType)
{
	if (byRankType >= RANK_ROLE_MAX)
	{
		MAPLOG(ERROR_LEVEL, "[%s]排行榜类型越界[type:%d]", __FUNCTION__, byRankType);
		return NULL;
	}

	if (RANK_WORLD_BOSS_HONOR == byRankType || RANK_WORLD_BOSS_DAMAGE == byRankType)
		return CWorldBossManager::Instance()->GetTypeRankList(byRankType);

	return &m_AllRoleRankList[byRankType];
}

// 检测是否在活动中
bool CRankManager::IsInActivity(WORD wServerId)
{
	const ScoreMatchServerGroup_Config *pServerList = g_pCfgMgr->GetScoreMatchTargetServerList(wServerId);
	return pServerList ? true : false;
}

// 是否全服排行
bool CRankManager::IsAllServerRank(WORD wRankType)
{
	if (RANK_PVP_ALL == wRankType || RANK_KINGMATCH == wRankType || RANK_DIAMOND_COST == wRankType || RANK_RECHARGE == wRankType)
		return true;
		
	return false; 
}

// 更新角色排行榜
void CRankManager::UpdateGuildRank(WORD wServerId, BYTE byRankType)
{
	// 排序
	GuildRankList *pRankList = GetGuildRankList(wServerId, byRankType);
	if (pRankList)
	{
		std::sort(pRankList->begin(), pRankList->end(), GuildRankDataPred);
	}

	// 更新排名
	pRankList = GetGuildRankList(wServerId, byRankType);
	if (pRankList)
	{
		WORD wRank = 0;
		vector<GuildRankData>::iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			iter->wRank = ++wRank;
		}
	}
}

// 取服务器工会排行榜列表
GuildRankList* CRankManager::GetGuildRankList(WORD wServerId, BYTE byRankType)
{
	if (byRankType >= GUILD_RANK_MAX)
	{
		MAPLOG(ERROR_LEVEL, "[%s]未处理的工会排行榜类型[type:%d]", __FUNCTION__, byRankType);
		return NULL;
	}

	AllGuildRankList *pAllRankList = &m_AllGuildRankList[byRankType];
	AllGuildRankList::iterator iter = pAllRankList->find(wServerId);
	if (iter == pAllRankList->end())
	{
		pAllRankList->insert(make_pair(wServerId, GuildRankList()));
		iter = pAllRankList->find(wServerId);
	}

	return &iter->second;
}

// 取工会排行榜数据
GuildRankData* CRankManager::GetGuildRankData(WORD dwServerId, BYTE byRankType, DWORD dwGuildId)
{
	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会排行榜列表[serverid:%d, type:%d]",
			__FUNCTION__, dwServerId, byRankType);
		return NULL;
	}

	return FindGuildRankData(pRankList, dwGuildId);
}

// 查找玩家排行榜数据
GuildRankData* CRankManager::FindGuildRankData(GuildRankList *pRankList, DWORD dwGuildId)
{
	vector<GuildRankData>::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); iter++)
	{
		if (iter->dwGuildId == dwGuildId)
		{
			return &*iter;
		}
	}
	return NULL;
}

// 处理无尽之地排行奖励
void CRankManager::OnEndlessLandDailyRankReward()
{
	AllRoleRankList *pRankList = GetRankList(RANK_ENDLESS_KILL);
	if (!pRankList)
		return;

	AllRoleRankList::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			const RoleRankData &tRankData = iter->second[i];
			const EndlessLandRankReward_Config *pRewardCfg = g_pCfgMgr->GetEndlessLandRankReward(tRankData.wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 14;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = tRankData.wRank;
				CRewardCenterManager::AddReward(iter->first, tRankData.dwRoleId, tRewardData);
				
				MAPLOG(ERROR_LEVEL, "[%s]无尽之地每日排行发放奖励[role_id:%d,rank:%d]", 
					__FUNCTION__, tRankData.dwRoleId, tRankData.wRank);
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]无尽之地每日排行榜已发放奖励", __FUNCTION__);
}

// 处理无尽之地每周排行奖励
void CRankManager::OnEndlessLandWeekRankReward()
{
	AllRoleRankList *pRankList = GetRankList(RANK_ENDLESS_KILL);
	if (!pRankList)
		return;

	AllRoleRankList::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			const RoleRankData &tRankData = iter->second[i];
			const EndlessLandRankReward_Config *pRewardCfg = g_pCfgMgr->GetEndlessLandRankReward(tRankData.wRank);
			if (pRewardCfg && pRewardCfg->dwTitleId)
			{
				CTitleManager::AddTitleEx(iter->first, tRankData.dwRoleId, pRewardCfg->dwTitleId);

				MAPLOG(ERROR_LEVEL, "[%s]无尽之地每周排行发放奖励[role_id:%d,rank:%d,title_id:%d]",
					__FUNCTION__, tRankData.dwRoleId, tRankData.wRank, pRewardCfg->dwTitleId);
			}
				
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]无尽之地每周排行榜已发放奖励", __FUNCTION__);
}

// 冲榜排行奖励(主线,战力,竞技场排行榜奖励)
void CRankManager::OnSevenDayRankReward(DWORD dwServerId)
{
	const RoleRankReward_Config *pRewardCfg;

	// 主线副本奖励
	RoleRankList *pMainFBRankList = GetServerRoleRankList(dwServerId, RANK_MAIN_FB_STAR);
	if (pMainFBRankList)
	{
		MAPLOG(GUI_LEVEL, "[%s]开始发放7天排行榜奖励(主线副本)[serverid:%d]", __FUNCTION__, dwServerId);
		RoleRankList::iterator iter = pMainFBRankList->begin();
		for (; iter != pMainFBRankList->end(); ++iter)
		{
			const RoleRankData &tRankData = *iter;
			pRewardCfg = g_pCfgMgr->GetSevenDayRankReward(RANK_MAIN_FB_STAR, tRankData.wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 17;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = tRankData.wRank;
				tRewardData.dwValue2 = RANK_MAIN_FB_STAR;
				CRewardCenterManager::AddReward(tRankData.dwServerId, tRankData.dwRoleId, tRewardData);

				MAPLOG(ERROR_LEVEL, "发放7天主线副本排行榜奖励(server_id:%d,role_id:%d,rank:%d)",
					dwServerId, tRankData.dwRoleId, tRankData.wRank);
			}
		}
	}

	// 战力榜奖励
	RoleRankList *pPowerRankList = GetServerRoleRankList(dwServerId, RANK_BATTLE_VALUE);
	if (pPowerRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]开始发放7天排行榜奖励(战力榜奖励)[serverid:%d]", __FUNCTION__, dwServerId);
		RoleRankList::iterator iter = pPowerRankList->begin();
		for (; iter != pPowerRankList->end(); ++iter)
		{
			const RoleRankData &tRankData = *iter;
			pRewardCfg = g_pCfgMgr->GetSevenDayRankReward(RANK_BATTLE_VALUE, tRankData.wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 16;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = tRankData.wRank;
				tRewardData.dwValue2 = RANK_BATTLE_VALUE;
				CRewardCenterManager::AddReward(tRankData.dwServerId, tRankData.dwRoleId, tRewardData);

				MAPLOG(ERROR_LEVEL, "发放7天战力榜奖励(server_id:%d,role_id:%d,rank:%d)",
					dwServerId, tRankData.dwRoleId, tRankData.wRank);
			}
		}
	}

	// 竞技场奖励
	CArenaManager::Instance()->DoSevenDayRankAward(dwServerId);

	MAPLOG(ERROR_LEVEL, "[%s]7天排行榜已发奖励[serverid:%d]", __FUNCTION__, dwServerId);
}

// 取排行榜列表
const AllRoleRankList* CRankManager::GetRankListEx(BYTE byRankType)
{
	return GetRankList(byRankType);
}

// 处理功勋排行榜奖励
void CRankManager::DoExploitRankReward()
{
	AllRoleRankList *pRankList = GetRankList(RANK_REBEL_EXPLOIT);
	if (!pRankList)
		return;

	DWORD dwSendRewardNum;

	AllRoleRankList::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		dwSendRewardNum = 0;
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			const RoleRankData &tRankData = iter->second[i];
			const RebelRankAward_Config *pRewardCfg = g_pCfgMgr->GetRebelRankAward(tRankData.wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 8;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = tRankData.wRank;
				CRewardCenterManager::AddReward(iter->first, tRankData.dwRoleId, tRewardData);
				dwSendRewardNum++;
			}
		}
		MAPLOG(GUI_LEVEL, "[%s]已发放叛军功勋排行奖励[server_id:%d, send_num:%d]", __FUNCTION__, iter->first, dwSendRewardNum);
		ResetRoleRank(iter->first, RANK_REBEL_EXPLOIT);
	}
}

// 处理伤害排行榜奖励
void CRankManager::DoDamageRankReward()
{
	AllRoleRankList *pRankList = GetRankList(RANK_REBEL_DAMAGE);
	if (!pRankList)
		return;

	DWORD dwSendRewardNum;
	AllRoleRankList::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		dwSendRewardNum = 0;
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			const RoleRankData &tRankData = iter->second[i];
			const RebelRankAward_Config *pRewardCfg = g_pCfgMgr->GetRebelRankAward(tRankData.wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 7;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = tRankData.wRank;
				CRewardCenterManager::AddReward(iter->first, tRankData.dwRoleId, tRewardData);
				dwSendRewardNum++;
			}
		}
		MAPLOG(GUI_LEVEL, "[%s]已发放叛军伤害排行奖励[server_id:%d, send_num:%d]", __FUNCTION__, iter->first, dwSendRewardNum);
		ResetRoleRank(iter->first, RANK_REBEL_DAMAGE);
	}
}

// 充值排行榜奖励
void CRankManager::DoRechargeRankReward()
{
	// 充值榜
	AllRoleRankList *pRechargeRankList = GetRankList(RANK_RECHARGE);
	if (pRechargeRankList)
	{
		AllRoleRankList::iterator iter = pRechargeRankList->begin();
		for (; iter != pRechargeRankList->end(); ++iter)
		{
			for (size_t i = 0; i < iter->second.size(); ++i)
			{
				const RoleRankData &tRankData = iter->second[i];
				const RechargeRankReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeAndCostRankReward(tRankData.wRank, 1);
				if (pRewardCfg)
				{
					RewardData tRewardData;
					tRewardData.wInfoId = 103;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = tRankData.wRank;
					CRewardCenterManager::AddReward(tRankData.dwServerId, tRankData.dwRoleId, tRewardData);
					MAPLOG(SPECAIL_LEVEL, "[%s]给玩家发放充值排行榜奖励(充值榜)[server_id:%d,role_id:%d,rank:%d]",
						__FUNCTION__, tRankData.dwServerId, tRankData.dwRoleId, tRankData.wRank);
				}
				else
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到充值榜配置[role_id:%d,rank:%d]", __FUNCTION__, tRankData.dwRoleId, tRankData.wRank);
				}
			}
		}
	}

	// 消费榜
	AllRoleRankList *pCostRankList = GetRankList(RANK_DIAMOND_COST);
	if (pCostRankList)
	{
		AllRoleRankList::iterator iter = pCostRankList->begin();
		for (; iter != pCostRankList->end(); ++iter)
		{
			for (size_t i = 0; i < iter->second.size(); ++i)
			{
				const RoleRankData &tRankData = iter->second[i];
				const RechargeRankReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeAndCostRankReward(tRankData.wRank, 2);
				if (pRewardCfg)
				{
					RewardData tRewardData;
					tRewardData.wInfoId = 104;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = tRankData.wRank;
					CRewardCenterManager::AddReward(tRankData.dwServerId, tRankData.dwRoleId, tRewardData);
					MAPLOG(SPECAIL_LEVEL, "[%s]给玩家发放充值排行榜奖励(消费榜)[server_id:%d,role_id:%d,rank:%d]",
						__FUNCTION__, tRankData.dwServerId, tRankData.dwRoleId, tRankData.wRank);
				}
				else
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到消费榜配置[role_id:%d,rank:%d]",__FUNCTION__,  tRankData.dwRoleId, tRankData.wRank);
				}
			}
		}
	}


	MAPLOG(GUI_LEVEL, "[%s]已发放充值排行奖励", __FUNCTION__);
	ResetRoleRank(RANK_DIAMOND_COST);
	ResetRoleRank(RANK_RECHARGE);
}