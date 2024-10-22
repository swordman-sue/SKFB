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
		MAPLOG(ERROR_LEVEL, "[%s]��ֵ���а���ʱ�����ó���[size:%d]",
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

	// todo::������BUG
	DWORD dwTriggerTime = DATETIME(nYear, nMonth, nDay, nHour, nMin, 0);

	m_RechargeRankRewardTimer.AddTriggerTime(dwTriggerTime);
}

void CRankManager::OnNewDay()
{
	// ����ѫ����
	DoExploitRankReward();

	// �����˺�����
	DoDamageRankReward();
}

void CRankManager::Update()
{
	// todo::����
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

	// �޾�֮�����н���(ÿ�ս���)
	if (m_EndlessLandDailyRankRewardTimer.IsTrigger())
	{
		OnEndlessLandDailyRankReward();
		m_EndlessLandDailyRankRewardTimer.SetCDOverTime();
	}

	// �޾�֮�����н���(ÿ��)
	if (m_EndlessLandWeekRankRewardTimer.IsTrigger())
	{
		OnEndlessLandWeekRankReward();
		m_EndlessLandWeekRankRewardTimer.SetCDOverTime();
	}

	// ȡ�������б�
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
		MAPLOG(ERROR_LEVEL, "[%s]δ��������а�����[type:%d]", __FUNCTION__, byRankType);
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
		MAPLOG(ERROR_LEVEL, "[%s]���а�洢���ݹ���[rank_type:%d, size:%d]", 
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
		MAPLOG(ERROR_LEVEL, "�����������ݳ���(rank_type:%d, size:%d)", byRankType, size);
		return;
	}


	// �Ƿ�����BOSS����
	bool bIsWorldBossRank = false;
	if (RANK_WORLD_BOSS_HONOR == byRankType || RANK_WORLD_BOSS_DAMAGE == byRankType)
		bIsWorldBossRank = true;

	AllRoleRankList *pTypeRankList = NULL;
	if (!bIsWorldBossRank)
	{
		pTypeRankList = GetRankList(byRankType);
		if (!pTypeRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]δ��������а�����[type:%d]", __FUNCTION__, byRankType);
			return;
		}
		// ���������
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

// ������������
void CRankManager::LoadDataRequest()
{
	for (BYTE i = 0; i < RANK_ROLE_MAX; ++i)
	{
		if (i == RANK_ARENA)
			continue;

		// ����
		RoleRankList_Load* pQuery = RoleRankList_Load::ALLOC();
		pQuery->SetIndex(DB_ROLE_RANK_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_RANK);
		pQuery->SetRankType(i);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// �Ϸ�
void CRankManager::OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId)
{
	// ��ɫ���а�
	RoleRankData tRankData;
	for (int nRankType = RANK_BATTLE_VALUE; nRankType < RANK_ROLE_MAX; ++nRankType)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ڴ������а�Ϸ�[type:%d]", __FUNCTION__, nRankType);

		// ����boss������ʱ������
		if (RANK_WORLD_BOSS_HONOR == nRankType || RANK_WORLD_BOSS_DAMAGE == nRankType)
			continue;

		AllRoleRankList *pRankList = GetRankList(nRankType);
		if (!pRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а�����[type:%d]", __FUNCTION__, nRankType);
			continue;
		}

		// ������, ȫ��PVP����Ҫ����
		if (nRankType != RANK_PVP_ALL && nRankType != RANK_ARENA)
		{
			// �ɷ�������а�������ӵ��·�
			AllRoleRankList::iterator list_iter = pRankList->find(dwFromServerId);
			if (list_iter != pRankList->end())
			{
				RoleRankList::iterator role_iter = list_iter->second.begin();
				for (; role_iter != list_iter->second.end(); ++role_iter)
				{
					//MAPLOG(ERROR_LEVEL, "[%s]���ڴ������а�������ݺϲ�[type:%d,roleid:%d]", __FUNCTION__, nRankType, role_iter->dwRoleId);
					//memcpy_s(&tRankData, sizeof(RoleRankData), &*role_iter, sizeof(RoleRankData));
					tRankData = *role_iter;
					tRankData.dwServerId = dwToServerId;
					AddRoleRankDataEx(nRankType, tRankData);
				}

				// ��վɷ�����
				if (list_iter != pRankList->end())
					list_iter->second.clear();
			}
		}
	}

	// �������а�
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

// �������а��б� 
void CRankManager::OnNetRankList(Player *pPlayer, BYTE byRankType, BYTE bySubType)
{
	//DWORD dwServerId = RANK_PVP_ALL != byRankType ? pPlayer->GetServerId() : ALL_SERVER_ID;

	DWORD dwServerId = pPlayer->GetServerId();
	if (IsAllServerRank(byRankType))
	{
		dwServerId = ALL_SERVER_ID;
	}
	// ���������а�(��������Ϊ��һ�����а�)
	else if (RANK_SCORE_MATCH_SUPER == byRankType || RANK_SCORE_MATCH_LEGEND == byRankType || RANK_SCORE_MATCH_WARCRAFT == byRankType)
	{
		const ScoreMatchServerGroup_Config *pServerGroupCfg = g_pCfgMgr->GetScoreMatchTargetServerList(dwServerId);
		if (!pServerGroupCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������������Ϣ[ServerId:%d,Type:%d]", __FUNCTION__, dwServerId, byRankType);
			return;
		}
		// ������������ķ�����ID, �Ƿ�����������ID
		dwServerId = pServerGroupCfg->wRankGroupId;
	}
	// ����ս
	else if (RANK_GUILD_WAR == byRankType)
	{
		const GuildWarRegion_Config *pRegionCfg = g_pCfgMgr->GetGuildWarRegionByServerId(dwServerId);
		if (!pRegionCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ս��������[ServerId:%d]", __FUNCTION__, dwServerId);
			return;
		}

		// ���������ID
		dwServerId = pRegionCfg->wRegionId;
	}

	//MAPLOG(ERROR_LEVEL, "�յ����а�����[type:%d]", byRankType);
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
	// ����BOSS����
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

		// �������Լ������а�����
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
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�������а�����[ServerId:%d,Type:%d]", __FUNCTION__, dwServerId, byRankType);
		return;
	}

	// �����߼�����
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
		// todo::Ϊ�˼��ݾ�����,��ʱ��������
		//DWORD dwRoleServerId = RANK_PVP_ALL != byRankType ? pPlayer->GetServerId() : iter->dwServerId;
// 		DWORD dwRoleServerId = pPlayer->GetServerId();
// 		if (RANK_PVP_ALL == byRankType || RANK_KINGMATCH == byRankType)
// 		{
// 			dwRoleServerId = g_MapServer.GetCurServerId(iter->dwServerId);
// 		}

		// �б�����
		if (msg.m_wListNum >= MAX_RANK_MUM)
			break;

		// ȡ���а��ɫ��ǰ���ڵķ�ID
		DWORD dwRoleServerId = g_MapServer.GetCurServerId(iter->dwServerId);

		// todo::��ֹ�������г�bug����
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
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����һ�������[ServerId:%d,RoleId:%d]", __FUNCTION__,
					pPlayer->GetServerId(), iter->dwRoleId);
			}
		}
	}
	pPlayer->SendPacket(&msg);
}

// ���󹤻����а��б�
void CRankManager::OnNetGuildRankList(Player *pPlayer, BYTE byRankType)
{
	GuildRankList *pRankList = GetGuildRankList(pPlayer->GetServerId(), byRankType);
	if (!pRankList)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_RANK_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�����������а��б�[ServerId:%d,Type:%d]", __FUNCTION__,
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
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[ServerId:%d,GuildId:%d]", __FUNCTION__,
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

// �����ҵ�����
void CRankManager::OnNetMyRank(Player *pPlayer, BYTE byRankType)
{
	MSG_S2C_MY_RANK_RESP msg;
	msg.m_wRankType = byRankType;
	msg.m_wMyRank = GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), byRankType);
	pPlayer->SendPacket(&msg);
}

// ɾ��ĳ�����
void CRankManager::RemoveRoleRankData(BYTE byRankType, DWORD dwServerId, DWORD dwRoleId)
{
	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]", __FUNCTION__, dwServerId, byRankType);
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

// ��ӽ�ɫ���а�����
void CRankManager::AddRoleRankData(BYTE byRankType, Player *pPlayer, DWORD dwValue, DWORD dwParam1, DWORD dwParam2, BYTE byMaxRankNum)
{
	// todo::��ʱ��������(�����������п���Ϊ0)
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
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������а�����[serverid:%d, type:%d]", __FUNCTION__, dwServerId, byRankType);
			return;
		}

		// todo::���õ�ID���п��ܲ��������ķ�����ID
		dwServerId = pGroupServerCfg->wRankGroupId;
	}
	// ����ս
	else if (RANK_GUILD_WAR == byRankType)
	{
		const GuildWarRegion_Config *pRegionCfg = g_pCfgMgr->GetGuildWarRegionByServerId(dwServerId);
		if (!pRegionCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ս��������[ServerId:%d]", __FUNCTION__, dwServerId);
			return;
		}

		// ���������ID
		dwServerId = pRegionCfg->wRegionId;
	}

	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]",__FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	RoleRankData *pRankData = FindRoleRankData(pRankList, pPlayer->GetCharID());
	
	// �������
	if (pRankData)
	{
		// �����б䶯�Ŵ���
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
	// �������
	else
	{
		// �б�û������ֱ�����
		if (pRankList->size() < byMaxRankNum)
		{
			RoleRankData sRankData;
			FillRoleRankData(pPlayer, sRankData);
			sRankData.dwValue = dwValue;
			sRankData.dwParam1 = dwParam1;
			sRankData.dwParam2 = dwParam2;
			sRankData.wRank = 0; // �¼����û������
			pRankList->push_back(sRankData);
			bNeedSort = true;
		}
		else
		{
			// �б�����������Ҫ������һ���ԱȾ����ܷ����
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

// ��ӽ�ɫ���а�����(�Ϸ�ʹ��)
void CRankManager::AddRoleRankDataEx(BYTE byRankType, const RoleRankData &tRankData, WORD wMaxRankNum)
{
	DWORD dwServerId = tRankData.dwServerId;
	DWORD dwRoleId = tRankData.dwRoleId;
	DWORD dwValue = tRankData.dwValue;

	// todo::��ʱ��������(�����������п���Ϊ0)
	if (RANK_KINGMATCH != byRankType && !dwValue)
		return;

	if (IsAllServerRank(byRankType))
	{
		dwServerId = ALL_SERVER_ID;
	}

	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]", __FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	RoleRankData *pRankData = FindRoleRankData(pRankList, dwRoleId);
	// �������
	if (pRankData)
	{
		// �����б䶯�Ŵ���
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
	// �������
	else
	{
		// �б�û������ֱ�����
		if (pRankList->size() < wMaxRankNum)
		{
			pRankList->push_back(tRankData);
			bNeedSort = true;
		}
		else
		{
			// �б�����������Ҫ������һ���ԱȾ����ܷ����
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

// ȡ������
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

// ȡ����ֵ
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

// ��ӹ������а�����
void CRankManager::AddGuildRankData(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum)
{
	if (!dwValue)
		return;

	//DWORD dwServerId = dwGuildId * 0.000001;

	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]",
			__FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	GuildRankData *pRankData = FindGuildRankData(pRankList, dwGuildId);

	// �������
	if (pRankData)
	{
		// �����б䶯�Ŵ���
		if (pRankData->dwValue != dwValue)
		{
			pRankData->dwValue = dwValue;
			pRankData->dwUpdateTime = time(NULL);
			bNeedSort = true;
		}
	}
	// �������
	else
	{
		// �б�û������ֱ�����
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
			// �б�����������Ҫ������һ���ԱȾ����ܷ����
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

// ��ӹ������а�����(�Ϸ�ʹ��)
void CRankManager::AddGuildRankDataEx(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType, DWORD dwValue, BYTE byMaxRankNum)
{
	if (!dwValue)
		return;

	//DWORD dwServerId = dwGuildId * 0.000001;

	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]",
			__FUNCTION__, dwServerId, byRankType);
		return;
	}

	bool bNeedSort = false;

	GuildRankData *pRankData = FindGuildRankData(pRankList, dwGuildId);

	// �������
	if (pRankData)
	{
		// �����б䶯�Ŵ���
		if (pRankData->dwValue != dwValue)
		{
			pRankData->dwValue = dwValue;
			pRankData->dwUpdateTime = time(NULL);
			bNeedSort = true;
		}
	}
	// �������
	else
	{
		// �б�û������ֱ�����
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
			// �б�����������Ҫ������һ���ԱȾ����ܷ����
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

// ɾ��������������
void CRankManager::RemoveGuildRandData(DWORD dwServerId, DWORD dwGuildId, BYTE byRankType)
{
	//DWORD dwServerId = dwGuildId * 0.000001;

	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]",__FUNCTION__, dwServerId, byRankType);
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

// ȡ��������
WORD CRankManager::GetGuildRank(DWORD dwGuildId, DWORD dwServerId, BYTE byType)
{
	GuildRankData *pRankData = GetGuildRankData(dwServerId, byType, dwGuildId);
	return pRankData ? pRankData->wRank : 0;
}

// ȡ��������ɫ���а��б�
const RoleRankList* CRankManager::GetServerRoleRankListEx(WORD wServerId, BYTE byRankType)
{
	return GetServerRoleRankList(wServerId, byRankType);
}

// ȡ������а�����
const RoleRankData* CRankManager::GetRoleRankDataEx(WORD dwServerId, BYTE byRankType, DWORD dwRoleId)
{
	return GetRoleRankData(dwServerId, byRankType, dwRoleId);
}

// ������а�����
void CRankManager::ResetRoleRank(WORD wServerId, BYTE byRankType)
{
	RoleRankList *pRankList = GetServerRoleRankList(wServerId, byRankType);
	if (pRankList)
		pRankList->clear();
}

// ������а�����
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

// ��շ�������ɫ��������
void CRankManager::ResetServerRoleRankData(WORD wServerId)
{
	// ��ɫ���а�
	for (int i = RANK_BATTLE_VALUE; i < RANK_ROLE_MAX; ++i)
	{
		AllRoleRankList &tRankList = m_AllRoleRankList[i];
		AllRoleRankList::iterator iter = tRankList.find(wServerId);
		if (iter != tRankList.end())
		{
			iter->second.clear();
		}
	}

	// �������а�
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

// ���½�ɫ���а�
void CRankManager::UpdateRoleRankEx(WORD wServerId, BYTE byRankType)
{
	UpdateRoleRank(wServerId, byRankType);
}

// 
bool RoleRankDataPred(RoleRankData &data1, RoleRankData &data2)
{
	// ��һ����ֵ�����
	if (data1.dwValue > data2.dwValue)
		return true;

	// ��һ����ֵ���
	if (data1.dwValue == data2.dwValue && data1.dwParam1 > data2.dwParam1)
		return true;

	// ��һ,������ֵ�����
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

// �������а�
void CRankManager::UpdateRoleRank(WORD wServerId, BYTE byRankType)
{
	// ����
	RoleRankList *pRankList = GetServerRoleRankList(wServerId, byRankType);
	if (pRankList)
	{
		std::sort(pRankList->begin(), pRankList->end(), RoleRankDataPred);
	}

	// ��������
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


// ȡ������а�����
RoleRankData* CRankManager::GetRoleRankData(WORD dwServerId, BYTE byRankType, DWORD dwRoleId)
{
	RoleRankList *pRankList = GetServerRoleRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[serverid:%d, type:%d]",__FUNCTION__, dwServerId,byRankType);
		return NULL;
	}

	return FindRoleRankData(pRankList, dwRoleId);
}

// ȡ�������������а��б� 
RoleRankList* CRankManager::GetServerRoleRankList(WORD wServerId, BYTE byRankType)
{
	AllRoleRankList *pTypeRankList = GetRankList(byRankType);
	if (!pTypeRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]δ��������а�����[type:%d]", __FUNCTION__, byRankType);
		return NULL;
	}

	// ȫ�����а�ķ�����IDΪ0
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

// ����������а�����
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

// ȡ���а��б�
AllRoleRankList* CRankManager::GetRankList(BYTE byRankType)
{
	if (byRankType >= RANK_ROLE_MAX)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���а�����Խ��[type:%d]", __FUNCTION__, byRankType);
		return NULL;
	}

	if (RANK_WORLD_BOSS_HONOR == byRankType || RANK_WORLD_BOSS_DAMAGE == byRankType)
		return CWorldBossManager::Instance()->GetTypeRankList(byRankType);

	return &m_AllRoleRankList[byRankType];
}

// ����Ƿ��ڻ��
bool CRankManager::IsInActivity(WORD wServerId)
{
	const ScoreMatchServerGroup_Config *pServerList = g_pCfgMgr->GetScoreMatchTargetServerList(wServerId);
	return pServerList ? true : false;
}

// �Ƿ�ȫ������
bool CRankManager::IsAllServerRank(WORD wRankType)
{
	if (RANK_PVP_ALL == wRankType || RANK_KINGMATCH == wRankType || RANK_DIAMOND_COST == wRankType || RANK_RECHARGE == wRankType)
		return true;
		
	return false; 
}

// ���½�ɫ���а�
void CRankManager::UpdateGuildRank(WORD wServerId, BYTE byRankType)
{
	// ����
	GuildRankList *pRankList = GetGuildRankList(wServerId, byRankType);
	if (pRankList)
	{
		std::sort(pRankList->begin(), pRankList->end(), GuildRankDataPred);
	}

	// ��������
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

// ȡ�������������а��б�
GuildRankList* CRankManager::GetGuildRankList(WORD wServerId, BYTE byRankType)
{
	if (byRankType >= GUILD_RANK_MAX)
	{
		MAPLOG(ERROR_LEVEL, "[%s]δ����Ĺ������а�����[type:%d]", __FUNCTION__, byRankType);
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

// ȡ�������а�����
GuildRankData* CRankManager::GetGuildRankData(WORD dwServerId, BYTE byRankType, DWORD dwGuildId)
{
	GuildRankList *pRankList = GetGuildRankList(dwServerId, byRankType);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������а��б�[serverid:%d, type:%d]",
			__FUNCTION__, dwServerId, byRankType);
		return NULL;
	}

	return FindGuildRankData(pRankList, dwGuildId);
}

// ����������а�����
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

// �����޾�֮�����н���
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
				
				MAPLOG(ERROR_LEVEL, "[%s]�޾�֮��ÿ�����з��Ž���[role_id:%d,rank:%d]", 
					__FUNCTION__, tRankData.dwRoleId, tRankData.wRank);
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]�޾�֮��ÿ�����а��ѷ��Ž���", __FUNCTION__);
}

// �����޾�֮��ÿ�����н���
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

				MAPLOG(ERROR_LEVEL, "[%s]�޾�֮��ÿ�����з��Ž���[role_id:%d,rank:%d,title_id:%d]",
					__FUNCTION__, tRankData.dwRoleId, tRankData.wRank, pRewardCfg->dwTitleId);
			}
				
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]�޾�֮��ÿ�����а��ѷ��Ž���", __FUNCTION__);
}

// ������н���(����,ս��,���������а���)
void CRankManager::OnSevenDayRankReward(DWORD dwServerId)
{
	const RoleRankReward_Config *pRewardCfg;

	// ���߸�������
	RoleRankList *pMainFBRankList = GetServerRoleRankList(dwServerId, RANK_MAIN_FB_STAR);
	if (pMainFBRankList)
	{
		MAPLOG(GUI_LEVEL, "[%s]��ʼ����7�����а���(���߸���)[serverid:%d]", __FUNCTION__, dwServerId);
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

				MAPLOG(ERROR_LEVEL, "����7�����߸������а���(server_id:%d,role_id:%d,rank:%d)",
					dwServerId, tRankData.dwRoleId, tRankData.wRank);
			}
		}
	}

	// ս������
	RoleRankList *pPowerRankList = GetServerRoleRankList(dwServerId, RANK_BATTLE_VALUE);
	if (pPowerRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʼ����7�����а���(ս������)[serverid:%d]", __FUNCTION__, dwServerId);
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

				MAPLOG(ERROR_LEVEL, "����7��ս������(server_id:%d,role_id:%d,rank:%d)",
					dwServerId, tRankData.dwRoleId, tRankData.wRank);
			}
		}
	}

	// ����������
	CArenaManager::Instance()->DoSevenDayRankAward(dwServerId);

	MAPLOG(ERROR_LEVEL, "[%s]7�����а��ѷ�����[serverid:%d]", __FUNCTION__, dwServerId);
}

// ȡ���а��б�
const AllRoleRankList* CRankManager::GetRankListEx(BYTE byRankType)
{
	return GetRankList(byRankType);
}

// ����ѫ���а���
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
		MAPLOG(GUI_LEVEL, "[%s]�ѷ����Ѿ���ѫ���н���[server_id:%d, send_num:%d]", __FUNCTION__, iter->first, dwSendRewardNum);
		ResetRoleRank(iter->first, RANK_REBEL_EXPLOIT);
	}
}

// �����˺����а���
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
		MAPLOG(GUI_LEVEL, "[%s]�ѷ����Ѿ��˺����н���[server_id:%d, send_num:%d]", __FUNCTION__, iter->first, dwSendRewardNum);
		ResetRoleRank(iter->first, RANK_REBEL_DAMAGE);
	}
}

// ��ֵ���а���
void CRankManager::DoRechargeRankReward()
{
	// ��ֵ��
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
					MAPLOG(SPECAIL_LEVEL, "[%s]����ҷ��ų�ֵ���а���(��ֵ��)[server_id:%d,role_id:%d,rank:%d]",
						__FUNCTION__, tRankData.dwServerId, tRankData.dwRoleId, tRankData.wRank);
				}
				else
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ֵ������[role_id:%d,rank:%d]", __FUNCTION__, tRankData.dwRoleId, tRankData.wRank);
				}
			}
		}
	}

	// ���Ѱ�
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
					MAPLOG(SPECAIL_LEVEL, "[%s]����ҷ��ų�ֵ���а���(���Ѱ�)[server_id:%d,role_id:%d,rank:%d]",
						__FUNCTION__, tRankData.dwServerId, tRankData.dwRoleId, tRankData.wRank);
				}
				else
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ѱ�����[role_id:%d,rank:%d]",__FUNCTION__,  tRankData.dwRoleId, tRankData.wRank);
				}
			}
		}
	}


	MAPLOG(GUI_LEVEL, "[%s]�ѷ��ų�ֵ���н���", __FUNCTION__);
	ResetRoleRank(RANK_DIAMOND_COST);
	ResetRoleRank(RANK_RECHARGE);
}