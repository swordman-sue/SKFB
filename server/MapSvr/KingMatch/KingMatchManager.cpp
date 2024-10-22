#include "stdafx.h"
#include "KingMatchManager.h"
#include "..\Player.h"
#include "..\Rank\RankManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Fuben\FubenManager.h"
#include "ToolKit\LogFile.h"
#include "..\stdafx.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Title\TitleManager.h"
#include "data_define.pb.h"
#include "..\MapSvrQuery.h"

CKingMatchManager::CKingMatchManager()
{
	m_wCurSeason = 1;
	m_wDataBelongSeason = 0;
	m_bIsLoadDBFinish = false;
	//m_bIsInActivity = false;
	m_byActivityStatus = 0;
	m_dwActivityOverTime = 0;
	m_lProceesSaveTime = 0;
}

CKingMatchManager::~CKingMatchManager()
{

}

void CKingMatchManager::Init()
{
	m_wCurSeason = 1;
	m_wDataBelongSeason = 0;
	m_bIsLoadDBFinish = false;
	m_byActivityStatus = 0;
	m_dwActivityOverTime = 0;
	m_lProceesSaveTime = 0;
}

void CKingMatchManager::Release()
{

}

void CKingMatchManager::Update()
{
	if (!m_bIsLoadDBFinish)
		return;

	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(m_wCurSeason);
	if (!pStageCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季段位配置[season:%d]", __FUNCTION__, m_wCurSeason);
		return;
	}

	// 空闲
	if (0 == m_byActivityStatus)
	{
		if (IsActivityStart())
		{
			DoActivityStart();
			m_byActivityStatus = 1;
		}

	}
	// 活动中
	else if (1 == m_byActivityStatus)
	{
		if (IsActivityOver())
		{
			DoActivityOver();
			m_byActivityStatus = 2;
			m_dwActivityOverTime = time(NULL);
		}

	}
	// 等待发奖励
	if (2 == m_byActivityStatus)
	{
		if (IsActivityReward())
		{
			DoActivityReward();
			m_byActivityStatus = 0;
		}
	}

	// 定时存储数据(1分钟存储一次)
	if (GetTickCount64() >= m_lProceesSaveTime + 60000)
	{
		SaveDataToDB();
		m_lProceesSaveTime = GetTickCount64();
	}
}

// 登出
void CKingMatchManager::OnLogout(Player *pPlayer)
{	
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER(pFBMgr);
	BYTE byBattleType = pFBMgr->GetBattleType();
	if (BATTLE_KING_MATCH == byBattleType && GetPlayerStageLevel(pPlayer->GetRoleId()))
	{
		if (!DoBattleOver(pPlayer, false))
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家下线时处理王者赛结束失败[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		}
	}
}

// 登录
void CKingMatchManager::OnLogin(Player *pPlayer)
{

}

// 活动开始
bool CKingMatchManager::DoActivityStart()
{
	int nBattleTargetNum = 0;

	WORD wOpenLevel = g_pCfgMgr->GetSystemOpenLevel(108);

	// 如果数据与当前赛季不一致 或者 段位数据为空
	if (m_wCurSeason != m_wDataBelongSeason || !m_StageDataList.size())
	{
		// 清空排行榜数据
		CRankManager::Instance()->ResetRoleRank(ALL_SERVER_ID, RANK_KINGMATCH);

		// 清空数据
		m_StageDataList.clear();

		// 清空玩家段位数据
		//m_PlayerStageLevelList.clear();

		DWORD dwPowerMin = 0;
		DWORD dwPowerMax = 0;
		const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(238);
		if (pSvrCfg)
		{
			dwPowerMin = pSvrCfg->GetListValue(0);
			dwPowerMax = pSvrCfg->GetListValue(1);
		}

		// 读入一部分数据作为挑战目标,防止先进来的用户找不到目标

		const map<DWORD, ServerInfo> &serverList = g_MapServer.GetServerList();
		map<DWORD, ServerInfo>::const_iterator svr_iter = serverList.begin();
		for (; svr_iter != serverList.end(); ++svr_iter)
		{
			const PlayerBaseDataMap *pBaseDataList = PlayerBaseDataManager::Instance()->GetPlayerList(svr_iter->second.dwServerId);
			PlayerBaseDataMap::const_iterator base_data_iter = pBaseDataList->begin();
			for (; base_data_iter != pBaseDataList->end(); ++base_data_iter)
			{
				PlayerBaseData *pBaseData = base_data_iter->second;

				// 到达指定等级,战力,还没进入挑战列表
				if (pBaseData->wLevel >= wOpenLevel
					&& pBaseData->dwFightingPower >= dwPowerMin
					&& pBaseData->dwFightingPower <= dwPowerMax
					&& !GetPlayerStageLevel(pBaseData->dwRoleId))
				{
					AddStageData(pBaseData->wServerId, pBaseData->dwRoleId);
					nBattleTargetNum++;
				}

				if (nBattleTargetNum >= 100)
					break;
			}

			if (nBattleTargetNum >= 100)
				break;
		}

		m_wDataBelongSeason = m_wCurSeason;
	}

	MAPLOG(ERROR_LEVEL, "[%s]王者赛活动已开始[season:%d,init_target_num:%d, total_target_num:%d]", 
		__FUNCTION__, m_wCurSeason, nBattleTargetNum, GetTotalBattleTargetNum());

	return true;
}

// 活动结束
bool CKingMatchManager::DoActivityOver()
{
	MAPLOG(ERROR_LEVEL, "[%s]王者赛活动已结束[season:%d]", __FUNCTION__, m_wCurSeason);

	return true;
}

// 处理活动奖励
void CKingMatchManager::DoActivityReward()
{
	DoSeasonRankReward();
	DoSeasonStageReward();

	vector<RoleKingMatchSeasonRank> vNewRankList;
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_KINGMATCH);
	if (pRankList && pRankList->size())
	{
		RoleRankList::const_iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			RoleKingMatchSeasonRank rankData;
			rankData.dwRoleId = iter->dwRoleId;
			rankData.wServerId = iter->dwServerId;
			vNewRankList.push_back(rankData);

			// 只取前3名
			if (vNewRankList.size() >= MAX_KING_MATCH_TOP_RANK_NUM)
				break;
		}
	}

	AddSeasonRank(m_wCurSeason, vNewRankList);

	// 进入下一赛季
	m_wCurSeason++;

	MAPLOG(ERROR_LEVEL, "[%s]王者赛活动已发放奖励[season:%d]", __FUNCTION__, m_wCurSeason);
}

// 发放赛季段位奖励
void CKingMatchManager::DoSeasonStageReward()
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.begin();
	for (; stage_iter != m_StageDataList.end(); ++stage_iter)
	{
		const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(stage_iter->first);
		if (!pStageCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季奖励配置[season:%d]", __FUNCTION__, stage_iter->first);
			continue;
		}

		if (pStageCfg->vSeasonReward.size())
		{
			map<DWORD, KingMatchBattleTarget>::iterator target_iter = stage_iter->second.begin();
			for (; target_iter != stage_iter->second.end(); ++target_iter)
			{
				// 发放奖励, 只要挑战过的都会发
				if (target_iter->second.wBattleTimes)
				{
					// 暂时发放其他的奖励
					RewardData tRewardData;
					tRewardData.wInfoId = 22;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = stage_iter->first;
					CRewardCenterManager::AddReward(target_iter->second.wServerId, target_iter->second.dwRoleId, tRewardData);
				}
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]王者赛发放段位奖励", __FUNCTION__);
}

// 发放赛季排行奖励
void CKingMatchManager::DoSeasonRankReward()
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_KINGMATCH);
	if (pRankList && pRankList->size())
	{
		RoleRankList::const_iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			// 广播第一名:{1}赛季王者赛已结束，{2}在王者赛中所向霹雳，荣获本赛季最强王者！
			if (1 == iter->wRank)
			{
				const PlayerBaseData *pNo1 = g_GetPlayerBaseData(iter->dwServerId, iter->dwRoleId, false);
				if (pNo1)
				{
					CSystemNotice sys_notice(21);
					sys_notice.AddParam(m_wCurSeason);
					sys_notice.AddParam(pNo1->szRoleName);
					sys_notice.SendMsg();
				}
			}

			const KingMatchRankReward_Config *pRewardCfg = g_pCfgMgr->GetKingMatchRankReward(iter->wRank);
			if (pRewardCfg)
			{
				// 物品奖励
				if (pRewardCfg->vRewardList.size())
				{
					// todo::暂时发放其他的奖励
					RewardData tRewardData;
					tRewardData.wInfoId = 21;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = iter->wRank;
					CRewardCenterManager::AddReward(iter->dwServerId, iter->dwRoleId, tRewardData);
				}


				// 称号奖励
				if (pRewardCfg->dwTitleId)
				{
					CTitleManager::AddTitleEx(iter->dwServerId, iter->dwRoleId, pRewardCfg->dwTitleId);

					MAPLOG(ERROR_LEVEL, "[%s]王者赛排行发放称号奖励[role_id:%d,rank:%d,title_id:%d]",
						__FUNCTION__, iter->dwRoleId, iter->wRank, pRewardCfg->dwTitleId);
				}
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]王者赛发放排行奖励", __FUNCTION__);
}



// 存储数据到数据库
void CKingMatchManager::SaveDataToDB()
{
	PB::KingMatchData PBKingMatchData;

	// 段位数据
	WORD wBattleTargetNum = 0;
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.begin();
	for (; stage_iter != m_StageDataList.end(); ++stage_iter)
	{
		::PB::KingMatchStage *pPBStageList = PBKingMatchData.add_stage_list();
		pPBStageList->set_stage(stage_iter->first);
		map<DWORD, KingMatchBattleTarget>::iterator iter = stage_iter->second.begin();
		for (; iter != stage_iter->second.end(); ++iter)
		{
			KingMatchBattleTarget &tBattleTarget = iter->second;
			::PB::KingMatchBattleTarget *pPBBattleTarget = pPBStageList->add_battle_target_list();
			pPBBattleTarget->set_role_id(tBattleTarget.dwRoleId);
			pPBBattleTarget->set_server_id(tBattleTarget.wServerId);
			pPBBattleTarget->set_stage_level(tBattleTarget.wStageLevel);
			pPBBattleTarget->set_star(tBattleTarget.wStar);
			pPBBattleTarget->set_battle_times(tBattleTarget.wBattleTimes);

			wBattleTargetNum++;
		}
	}

	// 赛季排行榜记录
	LastSeasonRankList::iterator season_iter = m_LastSeasonRankList.begin();
	for (; season_iter != m_LastSeasonRankList.end(); ++season_iter)
	{
		::PB::SeasonRankData *pLastSeasonRankList = PBKingMatchData.add_last_season_rank_list();
		pLastSeasonRankList->set_season(season_iter->first);
		vector<RoleKingMatchSeasonRank>::iterator rank_iter = season_iter->second.begin();
		for (; rank_iter != season_iter->second.end(); ++rank_iter)
		{
			::PB::RoleSeasonRank *pRank = pLastSeasonRankList->add_rank_list();
			pRank->set_role_id(rank_iter->dwRoleId);
			pRank->set_server_id(rank_iter->wServerId);
		}
	}

	string strSaveData("");
	PBKingMatchData.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_KING_MATCH_STAGE_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]王者赛存储数据过大[data_size:%d,list_num:%d]",
			__FUNCTION__, strSaveData.size(), PBKingMatchData.stage_list_size());
		return;
	}

	KingMatchData_Save* pQuery = KingMatchData_Save::ALLOC();
	pQuery->SetIndex(DB_KING_MATCH_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_KING_MATCH);
	pQuery->SetDataBelongSeason(m_wDataBelongSeason);
	pQuery->SetCurSeason(m_wCurSeason);
	pQuery->SetBattleTargetNum(wBattleTargetNum);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);
}

// 从数据库加载数据
// void CKingMatchManager::LoadDataFromDB(const KingMatchBattleTarget *pTargetList, WORD wTargetListNum)
// {
// 	for (WORD i = 0; i < wTargetListNum; ++i)
// 	{
// 		const KingMatchBattleTarget &tTarget = pTargetList[i];
// 		AddStageData(tTarget.wServerId, tTarget.dwRoleId, tTarget.wStageLevel, tTarget.wStar);
// 	}
// }

// 加载数据
void CKingMatchManager::LoadDataFromDB(WORD wCurSeason, WORD wDataBelongSeason, const char *pData, int size)
{
	m_StageDataList.clear();
	m_LastSeasonRankList.clear();

	if (size > 0)
	{
		PB::KingMatchData PBKingMatchData;
		if (!PBKingMatchData.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "解析王者赛数据出错(size:%d)", size);
			return;
		}

		// 段位数据
		KingMatchBattleTarget tBattleTarget;
		for (int i = 0; i < PBKingMatchData.stage_list_size(); ++i)
		{
			map<DWORD, KingMatchBattleTarget> Battle_Target_List;
			const PB::KingMatchStage &PBStageData = PBKingMatchData.stage_list(i);
			for (int k = 0; k < PBStageData.battle_target_list_size(); ++k)
			{
				const PB::KingMatchBattleTarget &PBBattleTarget = PBStageData.battle_target_list(k);
				tBattleTarget.dwRoleId = PBBattleTarget.role_id();
				tBattleTarget.wServerId = PBBattleTarget.server_id();
				tBattleTarget.wStageLevel = PBBattleTarget.stage_level();
				tBattleTarget.wStar = PBBattleTarget.star();
				tBattleTarget.wBattleTimes = PBBattleTarget.battle_times();
				Battle_Target_List.insert(make_pair(tBattleTarget.dwRoleId, tBattleTarget));
			}
			m_StageDataList.insert(make_pair(PBStageData.stage(), Battle_Target_List));
		}

		// 近期赛季排行
		for (int i = 0; i < PBKingMatchData.last_season_rank_list_size(); ++i)
		{
			const PB::SeasonRankData &PBLastSeasonRankList = PBKingMatchData.last_season_rank_list(i);
			vector<RoleKingMatchSeasonRank> vRankList;
			for (int k = 0; k < PBLastSeasonRankList.rank_list_size(); ++k)
			{
				const ::PB::RoleSeasonRank &PBRankData = PBLastSeasonRankList.rank_list(k);
				RoleKingMatchSeasonRank tRankData;
				tRankData.dwRoleId = PBRankData.role_id();
				tRankData.wServerId = PBRankData.server_id();
				vRankList.push_back(tRankData);
			}
			AddSeasonRank(PBLastSeasonRankList.season(), vRankList);
		}
	}

	m_wCurSeason = wCurSeason;
	m_wDataBelongSeason = wDataBelongSeason;
	m_bIsLoadDBFinish = true;
	m_lProceesSaveTime = GetTickCount64();
}

// 加载数据请求
void CKingMatchManager::LoadDataRequest()
{
	KingMatchData_Load* pQuery = KingMatchData_Load::ALLOC();
	pQuery->SetIndex(DB_KING_MATCH_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_KING_MATCH);
	pQuery->SetCurSeason(m_wCurSeason);
	pQuery->SetDataBelongSeason(m_wDataBelongSeason);
	g_MapServer.MapDBQuery(pQuery);
}

// 是否有红点
bool CKingMatchManager::IsHaveRedPoint(Player *pPlayer)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;
	map<WORD, KingMatchSeasonTask_Config>::iterator iter = g_pCfgMgr->m_KingMatchSeasonTaskList.begin();
	for (; iter != g_pCfgMgr->m_KingMatchSeasonTaskList.end(); ++iter)
	{
		// 获胜次数满足
		if (tKingMatchData.wTotalWinTimes >= iter->second.wWinTimes)
		{
			// 还没领取
			if (!IsTakeSeasonTaskReward(tKingMatchData, iter->second.wWinTimes))
				return true;
		}
	}

	return false;
}

// fot gm
void CKingMatchManager::SetStageForTest(Player *pPlayer, WORD wStage)
{
	// 检测设置的赛季合法性
	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(wStage);
	if (!pStageCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛段位配置[Stage:%d]", __FUNCTION__, wStage);
		return;
	}

	OnNetPlayerKingMatchData(pPlayer);

	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	RemoveStageData(tKingMatchData.wStageLevel, pPlayer->GetRoleId());
	AddStageData(pPlayer->GetServerId(), pPlayer->GetRoleId(), wStage, tKingMatchData.dwContext, tKingMatchData.wTotalBattleTimes);
	tKingMatchData.wStageLevel = wStage;
	if (tKingMatchData.wStageLevel > tKingMatchData.wHistoryStageLevel)
	{
		tKingMatchData.wHistoryStageLevel = tKingMatchData.wStageLevel;
	}
}

// 开始战斗
bool CKingMatchManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// 检测是否活动中
	if (m_byActivityStatus != 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]王者赛活动还没开始[RoleId:%d]!",__FUNCTION__,pPlayer->GetRoleId());
		return false;
	}

	// 检测所在的服务器能否参加
// 	if (!IsHaveActivity(pPlayer->GetRoleId()))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]玩家所在的服务器不能参加[RoleId:%d,ServerId:%d]!",
// 			__FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetServerId());
// 		return false;
// 	}

	// 检测目标是否合法
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	if (pFBMgr->GetTargetId() != dwTargetId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战目标出现异常[ClientTargetId:%d, ServerTargetId:%d]!",
			__FUNCTION__, dwTargetId, pFBMgr->GetTargetId());
		return false;
	}

	WORD wServerId = pFBMgr->GetTargetServerId();

	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(wServerId, dwTargetId, true);
	if (pTargetBaseData)
	{
		pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

		// 设置战力
		pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
	}

	return true;
}

// 结束战斗
bool CKingMatchManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	return DoBattleOver(pPlayer, byIsWin);
}

// 请求玩家的王者赛数据
void CKingMatchManager::OnNetPlayerKingMatchData(Player *pPlayer)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	KingMatchBattleTarget *pBattleTarget = GetStageData(pPlayer->GetRoleId());
	if (pBattleTarget)
	{
		// 数据需要重置
		if (!pBattleTarget->wBattleTimes)
		{
			memset(&tKingMatchData, 0, sizeof(tKingMatchData));
			tKingMatchData.wLastSeason = m_wCurSeason;
			tKingMatchData.wStageLevel = 1;
		}

		// 做数据纠正(防止数据存储出问题)
		if (pBattleTarget->wStageLevel != tKingMatchData.wStageLevel)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家段位出现异常,要进行纠正[RoleId:%d,RoleStage:%d,ListStage:%d]!",
				__FUNCTION__, pPlayer->GetRoleId(), tKingMatchData.wStageLevel, pBattleTarget->wStageLevel);

			tKingMatchData.wStageLevel = pBattleTarget->wStageLevel;
		}
	}
	else
	{
		// 还没进入段位
		memset(&tKingMatchData, 0, sizeof(tKingMatchData));
		tKingMatchData.wLastSeason = m_wCurSeason;
		tKingMatchData.wStageLevel = 1;
		AddStageData(pPlayer->GetServerId(), pPlayer->GetRoleId(), tKingMatchData.wStageLevel);
	}

	SendKingMatchData(pPlayer);
}

// 匹配目标
void CKingMatchManager::OnNetMatchTarget(Player *pPlayer)
{
	WORD wFromMsgId = C2S_KING_MATCH_SEARCH_TARGET_REQ;

	// 检测是否活动中
	if (m_byActivityStatus != 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]王者赛活动还没开始[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	// 取段位配置
	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
	if (!pStageCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛段位配置[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
		return;
	}

	// 先随机段位
	int nTotalRate = 0;
	for (size_t i = 0; i < pStageCfg->vMatchingStageList.size(); ++i)
	{
		nTotalRate += pStageCfg->vMatchingStageList[i].nRate;
	}
	int nRandValue = g_Rand(0, nTotalRate);
	WORD wRandStageLevel = 0;
	int nCurRate = 0;
	for (size_t i = 0; i < pStageCfg->vMatchingStageList.size(); ++i)
	{
		nCurRate += pStageCfg->vMatchingStageList[i].nRate;
		if (nRandValue <= nCurRate)
		{
			wRandStageLevel = pStageCfg->vMatchingStageList[i].nValue;
			break;
		}
	}

	// 后随机目标
	const map<DWORD, KingMatchBattleTarget> *pTargetList = NULL;
	while (wRandStageLevel)
	{
		// 取随机到的段位目标列表
		pTargetList = GetStageLevelList(wRandStageLevel);
		if (!pTargetList)
			break;

		// 目标列表有人且保证至少两个人(因为如果只有一个人而那个人肛好是自己那就悲剧了...)
		if (pTargetList->size() > 1)
			break;

		wRandStageLevel--;
	}
	if (!pTargetList || !pTargetList->size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]王者赛匹配不到目标[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
		return;
	}
	KingMatchBattleTarget tTarget;
	memset(&tTarget, 0, sizeof(tTarget));

	WORD wDoTimes = 0;
	while (true)
	{
		int nRandIdx = g_Rand(0, pTargetList->size() - 1);
		int nCurIdx = 0;
		for (map<DWORD, KingMatchBattleTarget>::const_iterator iter = pTargetList->begin(); iter != pTargetList->end(); ++iter)
		{
			// 暂时这样处理
			if (wDoTimes++ >= 1000)
				break;

			// 去掉自己
			if (iter->second.dwRoleId == pPlayer->GetRoleId())
				continue;

			// 使用 ">=" 的原因主要是当使用到自己时,会取下一个目标
			if (nCurIdx++ >= nRandIdx)
			{
				tTarget = iter->second;
				break;
			}
		}

		// 随机到目标了
		if (tTarget.dwRoleId)
			break;

		// 暂时这样处理
		if (wDoTimes++ >= 1000)
			break;
	}

	MSG_S2C_KING_MATCH_SEARCH_TARGET_RESP msg;
	msg.m_wStageLevel = wRandStageLevel;
	msg.m_byStar = tTarget.wStar;
	msg.m_wRank = 0;
	msg.m_dwRankValue = 0;
	msg.m_TargetData.dwRoleId = tTarget.dwRoleId;
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(g_MapServer.GetCurServerId(tTarget.wServerId), tTarget.dwRoleId);
	if (pBaseData)
		memcpy_s(&msg.m_TargetData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(ALL_SERVER_ID, RANK_KINGMATCH, tTarget.dwRoleId);
	if (pRankData)
	{
		msg.m_wRank = pRankData->wRank;
		msg.m_dwRankValue = pRankData->dwValue;
	}
	pPlayer->SendPacket(&msg);

	// 记录匹配到的挑战目标
	pPlayer->GetFubenManager()->SetTargetId(tTarget.dwRoleId);
	pPlayer->GetFubenManager()->SetTargetServerId(tTarget.wServerId);

	MAPLOG(ERROR_LEVEL, "[%s]王者赛匹配目标信息[wRandStageLevel:%d, TargetId:%d]!", __FUNCTION__, wRandStageLevel, tTarget.dwRoleId);
}

// 领取赛季任务奖励
void CKingMatchManager::OnNetTakeSesionTaskReward(Player *pPlayer, WORD wWinTimes)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	if (tKingMatchData.wTotalWinTimes < wWinTimes)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]获胜次数不足,不能领取[CurWinTimes:%d,ReqWinTimes:%d]!",
			__FUNCTION__, tKingMatchData.wTotalWinTimes, wWinTimes);
		return;
	}

	if (tKingMatchData.wSeasonRewardRecordNum >= MAX_SEASON_TASK_REWARD_RECORD)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]赛季任务奖励记录列表已满[num:%d]!", __FUNCTION__, tKingMatchData.wSeasonRewardRecordNum);
		return;
	}

	const KingMatchSeasonTask_Config *pTaskCfg = g_pCfgMgr->GetKingMatchSeasonTask(wWinTimes);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到赛季任务配置[WinTimes:%d]!", __FUNCTION__, wWinTimes);
		return;
	}

	// 发奖励
	pPlayer->AddObjectList(pTaskCfg->vRewardList, TRUE, FROM_KING_MATCH_SEASON_TASK_REWARD);

	tKingMatchData.arSeasonTaskRewardRecord[tKingMatchData.wSeasonRewardRecordNum++] = wWinTimes;

	MSG_S2C_TAKE_KING_MATCH_TASK_REWARD_RESP msg;
	msg.m_wWinTimes = wWinTimes;
	pPlayer->SendPacket(&msg);
}

// 请求最近赛季排行榜
void CKingMatchManager::OnNetRecentlySesionRank(Player *pPlayer)
{
	MSG_S2C_KING_MATCH_SEASON_RANK_RESP msg;
	msg.m_wSeasonRankNum = 0;

	LastSeasonRankList::iterator season_iter = m_LastSeasonRankList.begin();
	for (; season_iter != m_LastSeasonRankList.end(); ++season_iter)
	{
		if (msg.m_wSeasonRankNum >= RECENTLY_KING_MATCH_SEASON_RANK_NUM)
			break;

		KingMatchRankRecord &season_record = msg.m_SeasonRankList[msg.m_wSeasonRankNum++];
		season_record.wRoleRankNum = 0;
		season_record.wSeason = season_iter->first;
		vector<RoleKingMatchSeasonRank>::iterator role_iter = season_iter->second.begin();
		for (; role_iter != season_iter->second.end(); ++role_iter)
		{
			if (season_record.wRoleRankNum >= MAX_KING_MATCH_TOP_RANK_NUM)
				break;

			DWORD dwCurServerId = g_MapServer.GetCurServerId(role_iter->wServerId, role_iter->dwRoleId);
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwCurServerId, role_iter->dwRoleId, true);
			if (pBaseData)
			{
				PlayerCommonData &role_record = season_record.arRoleRankList[season_record.wRoleRankNum++];
				memcpy_s(&role_record, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
			}
			else
			{
				MAPLOG(IMPORTANT_LEVEL, "[%s]找不到玩家数据[ServerId:%d,RoleId:%d]!",
					__FUNCTION__, role_iter->wServerId, role_iter->dwRoleId);
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// 设置当前赛季
void CKingMatchManager::SetCurSeason(WORD wSeason)
{
	// 如果非空闲状态不能设置,防止活动正在进行中,会出错
	if (m_byActivityStatus)
		return;

	// 检测设置的赛季合法性
	const KingMatchSeasonTime_Config *pSeasonTimeCfg = g_pCfgMgr->GetKingMatchSeasonTime(wSeason);
	if (!pSeasonTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季时间配置[season:%d]", __FUNCTION__, wSeason);
		return;
	}

	m_wCurSeason = wSeason;
}

// 是否有此活动
bool CKingMatchManager::IsHaveActivity(WORD wServerId)
{
	DWORD dwOpenDay = g_MapServer.GetAlreadyOpenServerDay(wServerId);

	// 开服到达指定的天数才开放
	if (dwOpenDay >= g_GetConfigValue(225))
		return true;

	return false;
}

// 处理结束结束
bool CKingMatchManager::DoBattleOver(Player *pPlayer, BYTE byIsWin)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	// 取段位配置
	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
	if (!pStageCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛段位配置[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
		return false;
	}

	WORD wOldStageLevel = tKingMatchData.wStageLevel;

	if (byIsWin)
	{
		// 首胜奖励
		if (!tKingMatchData.wTodayWinTimes)
		{
			// 物品奖励
			pPlayer->AddObjectList(pStageCfg->vDailyFirstWinReward, TRUE, FROM_KING_MATCH_BATTLE_REWARD);

			// 积分,星数奖励
			tKingMatchData.dwContext++;

			// 设置为首胜
			pFBMgr->SetIsFirstWin(true);
		}

		// 积分,星数奖励
		tKingMatchData.dwContext++;

		// 加总获胜次数及今日获胜次数
		tKingMatchData.wTodayWinTimes++;
		tKingMatchData.wTotalWinTimes++;

		// 如果不是最后的段位,则检测是否会升级
		if (!pStageCfg->byIsLastStage && tKingMatchData.dwContext >= pStageCfg->wLevelUpNeedStar)
		{
			// 升级
			tKingMatchData.wStageLevel++;

			// 星数重置
			tKingMatchData.dwContext = tKingMatchData.dwContext - pStageCfg->wLevelUpNeedStar;

			// 升级奖励
			if (tKingMatchData.wStageLevel > tKingMatchData.wHistoryStageLevel)
			{
				pPlayer->AddObjectList(pStageCfg->vLevelUpReward, TRUE, FROM_KING_MATCH_BATTLE_REWARD);
				tKingMatchData.wHistoryStageLevel = tKingMatchData.wStageLevel;

				// 广播:{1}在王者赛中手起刀落斩杀对手{1}，段位晋级为{2}！
				if (pStageCfg->byIsBroadcastLeveUp)
				{
					const PlayerBaseData *pTargetData = g_GetPlayerBaseData(pFBMgr->GetTargetServerId(), pFBMgr->GetTargetId(), false);
					if (pTargetData)
					{
						CSystemNotice sys_notice(19, pPlayer->GetServerId());
						sys_notice.AddParam(pPlayer->GetRoleName());
						sys_notice.AddParam(pTargetData->szRoleName);
						sys_notice.AddParam(tKingMatchData.wStageLevel);
						sys_notice.SendMsg();
					}
				}
			}
		}
	}
	else
	{
		// 如果掉段
		if (pStageCfg->byIsDropStage)
		{
			// 减星数,积分
			if (tKingMatchData.dwContext)
			{
				tKingMatchData.dwContext--;
			}
			// 降级
			else
			{
				tKingMatchData.wStageLevel--;
				const KingMatchStage_Config *pPreStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
				if (!pPreStageCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛段位配置[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
					return false;
				}
				tKingMatchData.dwContext = pPreStageCfg->wLevelUpNeedStar - 1;
			}
		}
		// 不会降级
		else
		{
			// 减星数,积分
			if (pStageCfg->byIsDropStar && tKingMatchData.dwContext)
				tKingMatchData.dwContext--;
		}
	}

	// 增加总的挑战次数
	tKingMatchData.wTotalBattleTimes++;

	// 如果是最后一段位,则处理排行
	const KingMatchStage_Config *pCurStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
	if (pCurStageCfg && pCurStageCfg->byIsLastStage)
	{
		int nWinRate = 0;
		if (tKingMatchData.wTotalBattleTimes && tKingMatchData.wTotalWinTimes)
			nWinRate = ceil((double)tKingMatchData.wTotalWinTimes * 100 / tKingMatchData.wTotalBattleTimes);
		CRankManager::Instance()->AddRoleRankData(RANK_KINGMATCH, pPlayer, tKingMatchData.dwContext, nWinRate, tKingMatchData.wTotalBattleTimes, 200);

		WORD wRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), ALL_SERVER_ID, RANK_KINGMATCH);

		pFBMgr->SetParam(0, wRank);
	}


	// 如果段位有变化
	if (wOldStageLevel != tKingMatchData.wStageLevel)
	{
		RemoveStageData(wOldStageLevel, pPlayer->GetRoleId());
		AddStageData(pPlayer->GetServerId(), pPlayer->GetRoleId(), tKingMatchData.wStageLevel,
			tKingMatchData.dwContext, tKingMatchData.wTotalBattleTimes);
	}
	else
	{
		// 更新挑战次数,星数
		KingMatchBattleTarget *pStageData = GetStageData(tKingMatchData.wStageLevel, pPlayer->GetRoleId());
		if (pStageData)
		{
			pStageData->wBattleTimes++;

			// 如果不是最后一个段位,则需要更新星数
			if (pCurStageCfg && pCurStageCfg->byIsLastStage)
				pStageData->wStar = tKingMatchData.dwContext;
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到玩家段位数据[StageLevel:%d,RoleId:%d]!",
				__FUNCTION__, tKingMatchData.wStageLevel, pPlayer->GetRoleId());
		}
	}

	// 节日活动(累计挑战王者赛次数)
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 18, 1, true);

	// 节日活跃目标(累计挑战王者赛次数)
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 18, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 18, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 18, 1, true);

	return true;
}

// 发送玩家王者数据
void CKingMatchManager::SendKingMatchData(Player *pPlayer)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	MSG_S2C_KING_MATCH_DATA_RESP msg;
	msg.m_wCurSeason = m_wCurSeason;
	msg.m_KingMatchData = tKingMatchData;
	msg.m_wMyRank = 0;
	msg.m_dwMyRankValue = 0;
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(ALL_SERVER_ID, RANK_KINGMATCH, pPlayer->GetRoleId());
	if (pRankData)
	{
		msg.m_wMyRank = pRankData->wRank;
		msg.m_dwMyRankValue = pRankData->dwValue;
	}

	pPlayer->SendPacket(&msg);
}

// 是否活动中
bool CKingMatchManager::IsActivityIng()
{
	if (!IsActivityStart())
		return false;

	// 223 开始时间
	DWORD dwStartTime = g_GetConfigValue(223);

	// 224 结束时间
	DWORD dwOverTime = g_GetConfigValue(224);

	tm tmDetailTime;
	GetTMTime(&tmDetailTime, time(NULL));

	DWORD dwCurTime = tmDetailTime.tm_hour * 100 + tmDetailTime.tm_min;

	if (dwCurTime >= dwStartTime && dwCurTime <= dwOverTime)
		return true;

	return false;
}

// 是否活动开始
bool CKingMatchManager::IsActivityStart()
{
	const KingMatchSeasonTime_Config *pSeasonTimeCfg = g_pCfgMgr->GetKingMatchSeasonTime(m_wCurSeason);
	if (!pSeasonTimeCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季时间配置[season:%d]", __FUNCTION__, m_wCurSeason);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// 在日期指定的范围内
	if (nCurData > pSeasonTimeCfg->dwStartDate && nCurData < pSeasonTimeCfg->dwOverDate)
		return true;

	// 开始的当天
	if (nCurData == pSeasonTimeCfg->dwStartDate && nCurTime >= pSeasonTimeCfg->dwStartTime)
		return true;

	// 最后一天
	if (nCurData == pSeasonTimeCfg->dwOverDate && nCurTime <= pSeasonTimeCfg->dwOverTime)
		return true;

	return false;
}


// 是否活动结束
bool CKingMatchManager::IsActivityOver()
{
	const KingMatchSeasonTime_Config *pSeasonTimeCfg = g_pCfgMgr->GetKingMatchSeasonTime(m_wCurSeason);
	if (!pSeasonTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季时间配置[season:%d]", __FUNCTION__, m_wCurSeason);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// 结束的了至少一天了
	if (nCurData > pSeasonTimeCfg->dwOverDate)
		return true;

	// 最后一天
	if (nCurData == pSeasonTimeCfg->dwOverDate && nCurTime > pSeasonTimeCfg->dwOverTime)
		return true;

	return false;
}

// 是否活动发放奖励
bool CKingMatchManager::IsActivityReward()
{
	// 延后3分钟发放
	if (time(NULL) >= m_dwActivityOverTime + 180)
		return true;

	return false;
}

// 是否在挑战列表中
bool CKingMatchManager::IsInStageList(Player *pPlayer)
{
	WORD wStageLevel = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData.wStageLevel;
	DWORD dwRoleId = pPlayer->GetRoleId();

	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.find(wStageLevel);
	// 找到段位数据
	if (iter != m_StageDataList.end())
	{
		// 找到玩家数据
		if (iter->second.find(dwRoleId) != iter->second.end())
			return true;
	}
	return false;
}

// 添加挑战目标数据
void CKingMatchManager::AddStageData(WORD wServerId, DWORD dwRoleId, WORD wStageLevel, WORD wStar, WORD wBattleTimes)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.find(wStageLevel);
	// 找到段位数据
	if (iter == m_StageDataList.end())
	{
		map<DWORD, KingMatchBattleTarget> list;
		m_StageDataList.insert(make_pair(wStageLevel, list));
		iter = m_StageDataList.find(wStageLevel);
	}

	KingMatchBattleTarget tTarget;
	tTarget.wServerId = wServerId;
	tTarget.dwRoleId = dwRoleId;
	tTarget.wStageLevel = wStageLevel;
	tTarget.wStar = wStar;
	tTarget.wBattleTimes = wBattleTimes;
	iter->second.insert(make_pair(dwRoleId, tTarget));

	//AddPlayerStageLevelData(dwRoleId, wStageLevel);
}

// 删除段位数据
void CKingMatchManager::RemoveStageData(WORD wStageLevel, DWORD dwRoleId)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.find(wStageLevel);
	// 找到段位数据
	if (stage_iter != m_StageDataList.end())
	{
		stage_iter->second.erase(dwRoleId);
	}
}

// 取段位数据
KingMatchBattleTarget* CKingMatchManager::GetStageData(WORD wStageLevel, DWORD dwRoleId)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.find(wStageLevel);
	// 找到段位数据
	if (stage_iter != m_StageDataList.end())
	{
		map<DWORD, KingMatchBattleTarget>::iterator target_iter = stage_iter->second.find(dwRoleId);
		if (target_iter != stage_iter->second.end())
			return &target_iter->second;
	}
	return NULL;
}

// 取段位数据
KingMatchBattleTarget* CKingMatchManager::GetStageData(DWORD dwRoleId)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.begin();
	for (; stage_iter != m_StageDataList.end(); ++stage_iter)
	{
		map<DWORD, KingMatchBattleTarget>::iterator battle_target_iter = stage_iter->second.find(dwRoleId);
		if (battle_target_iter != stage_iter->second.end())
			return &battle_target_iter->second;
	}
	return NULL;
}

// 取段位等级列表
const map<DWORD, KingMatchBattleTarget>* CKingMatchManager::GetStageLevelList(WORD wStageLevel)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.find(wStageLevel);
	if (iter == m_StageDataList.end())
	{
		m_StageDataList.insert(make_pair(wStageLevel, map<DWORD, KingMatchBattleTarget>()));
		iter = m_StageDataList.find(wStageLevel);
	}
	return iter != m_StageDataList.end() ? &iter->second : NULL;
}

// 取玩家段位等级
WORD CKingMatchManager::GetPlayerStageLevel(DWORD dwRoleId)
{
	KingMatchBattleTarget *pBattleTarget = GetStageData(dwRoleId);
	return pBattleTarget ? pBattleTarget->wStageLevel : 0;
}

// 更新玩家段位等级
// void CKingMatchManager::UpdatePlayerStageLevel(DWORD dwRoleId, WORD wStageLevel)
// {
// 	map<DWORD, WORD>::iterator iter = m_PlayerStageLevelList.find(dwRoleId);
// 	if (iter != m_PlayerStageLevelList.end())
// 	{
// 		iter->second = wStageLevel;
// 	}
// }

// 添加玩家段位等级数据
// void CKingMatchManager::AddPlayerStageLevelData(DWORD dwRoleId, WORD wStageLevel)
// {
// 	m_PlayerStageLevelList.insert(make_pair(dwRoleId, wStageLevel));
// }

// 添加赛季排行榜
void CKingMatchManager::AddSeasonRank(WORD wSeason, const vector<RoleKingMatchSeasonRank> &vRankList)
{
	m_LastSeasonRankList.insert(make_pair(wSeason, vRankList));

	// 只取指定的几个赛季
	if (m_LastSeasonRankList.size() > RECENTLY_KING_MATCH_SEASON_RANK_NUM)
	{
		// 查找最小的赛季(也是最久的赛季)
		WORD wRemoveSeason = 0;
		LastSeasonRankList::iterator iter = m_LastSeasonRankList.begin();
		for (; iter != m_LastSeasonRankList.end(); ++iter)
		{
			if (!wRemoveSeason || iter->first < wRemoveSeason)
			{
				wRemoveSeason = iter->first;
			}
		}

		if (wRemoveSeason)
			m_LastSeasonRankList.erase(wRemoveSeason);
	}

	// todo::存储到数据库
}

// 取总的挑战目标人数
DWORD CKingMatchManager::GetTotalBattleTargetNum()
{
	DWORD dwTotalBattleTargetNum = 0;
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.begin();
	for (; iter != m_StageDataList.end(); ++iter)
	{
		dwTotalBattleTargetNum += iter->second.size();
	}
	return dwTotalBattleTargetNum;
}

// 检测是否已经赛季领取过
bool CKingMatchManager::IsTakeSeasonTaskReward(const PlayerKingMatchData &tKingMatchData, WORD wWinTimes)
{
	for (WORD i = 0; i < tKingMatchData.wSeasonRewardRecordNum; i++)
	{
		if (tKingMatchData.arSeasonTaskRewardRecord[i] == wWinTimes)
			return true;
	}

	return false;
}