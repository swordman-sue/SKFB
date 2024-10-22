#include "stdafx.h"
#include "ScoreMatchManager.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\PlayerBaseDataManager.h"
#include "CommonLogic.h"
#include "..\Fuben\FubenManager.h"
#include "..\Rank\RankManager.h"
#include "..\MapServer.h"
#include "data_define.pb.h"
#include "..\MapSvrQuery.h"
#include "..\RewardCenter\RewardCenterManager.h"

CScoreMatchManager::CScoreMatchManager()
{
	m_byActivityStatus = 0;
	m_bIsLoadDBFinish = false;
	m_dwCurActivityIndex = 1;
}

CScoreMatchManager::~CScoreMatchManager()
{

}

void CScoreMatchManager::Init()
{
	m_byActivityStatus = 0;
	m_bIsLoadDBFinish = false;
	m_dwCurActivityIndex = 1;
}

void CScoreMatchManager::OnNewDay()
{
	std::map<DWORD, ScoreMatchData>::iterator iter = m_PlayerScoreMatchDataList.begin();
	for (; iter != m_PlayerScoreMatchDataList.end(); ++iter)
	{
		iter->second.wBattleTimes = 0;
		iter->second.wRefreshTimes = 0;
		iter->second.wBuyBattleTimes = 0;
		iter->second.wTaskRewardRecordNum = 0;
	}
}

// 下线
void CScoreMatchManager::OnLogout(Player *pPlayer)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER(pFBMgr);

	// 没有在积分赛挑战中,不处理
	if (pFBMgr->GetBattleType() != BATTLE_SCORE_MATCH)
		return;

	// 失败处理
	DoBattleOver(pPlayer, false);
}

void CScoreMatchManager::Update()
{
	if (!m_bIsLoadDBFinish)
		return;

// 	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(m_dwCurActivityIndex);
// 	if (!pStageCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季段位配置[season:%d]", __FUNCTION__, m_dwCurActivityIndex);
// 		return;
// 	}

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

// 存储数据到数据库
void CScoreMatchManager::SaveDataToDB()
{
	PB::ScoreMatchData PBScoreMatchData;

	// 角色积分赛数据
	WORD wJoinRoleNum = 0;
	map<DWORD, ScoreMatchData>::iterator iter = m_PlayerScoreMatchDataList.begin();
	for (; iter != m_PlayerScoreMatchDataList.end(); ++iter)
	{
		::PB::RoleScoreMatchData *pPBRoleScoreMatchData = PBScoreMatchData.add_role_score_match_data_list();
		pPBRoleScoreMatchData->set_role_id(iter->second.dwRoleId);
		pPBRoleScoreMatchData->set_battle_times(iter->second.wBattleTimes);
		pPBRoleScoreMatchData->set_refresh_times(iter->second.wRefreshTimes);
		pPBRoleScoreMatchData->set_buy_battle_times(iter->second.wBuyBattleTimes);
		pPBRoleScoreMatchData->set_camp(iter->second.byCamp);
		pPBRoleScoreMatchData->set_win_times(iter->second.wWinTimes);
		// 任务奖励记录
		for (WORD i = 0; i < iter->second.wTaskRewardRecordNum; ++i)
		{
			pPBRoleScoreMatchData->add_task_reward_record(iter->second.arTaskRewardRecord[i]);
		}
		// 挑战目标列表
		for (WORD i = 0; i < iter->second.wBattleTargetNum; ++i)
		{
			ScoreMatchTarget &tTarget = iter->second.arBattleTargetList[i];
			::PB::ScoreMatchTarget *pPBTarget = pPBRoleScoreMatchData->add_battle_target_list();
			pPBTarget->set_target_id(tTarget.dwTargetId);
			pPBTarget->set_status(tTarget.wStatus);
		}

		wJoinRoleNum++;
	}

	string strSaveData("");
	PBScoreMatchData.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_SCORE_MATCH_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛存储数据过大[data_size:%d,list_num:%d]",
			__FUNCTION__, strSaveData.size(), PBScoreMatchData.role_score_match_data_list_size());
		return;
	}

	ScoreMatchData_Save* pQuery = ScoreMatchData_Save::ALLOC();
	pQuery->SetIndex(DB_SCORE_MATCH_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_SCORE_MATCH);
	pQuery->SetCurActivityIndex(m_dwCurActivityIndex);
	pQuery->SetActivityStatus(m_byActivityStatus);
	pQuery->SetActivityOverTime(m_dwActivityOverTime);
	pQuery->SetJoinRoleNum(wJoinRoleNum);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);
}



// 加载数据
void CScoreMatchManager::LoadDataFromDB(DWORD dwCurActivityIndex, BYTE byActivityStatus, DWORD dwActivityOverTime, const char *pData, int size)
{
	m_PlayerScoreMatchDataList.clear();

	if (size > 0)
	{
		PB::ScoreMatchData PBScoreMatchData;
		if (!PBScoreMatchData.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "解析积分赛数据出错(size:%d)", size);
			return;
		}

		// 角色积分赛数据
		ScoreMatchData tScoreMatchData;
		for (int i = 0; i < PBScoreMatchData.role_score_match_data_list_size(); ++i)
		{
			const PB::RoleScoreMatchData &PBRoleScoreMatchData = PBScoreMatchData.role_score_match_data_list(i);
			tScoreMatchData.dwRoleId = PBRoleScoreMatchData.role_id();
			tScoreMatchData.wBattleTimes = PBRoleScoreMatchData.battle_times();
			tScoreMatchData.wRefreshTimes = PBRoleScoreMatchData.refresh_times();
			tScoreMatchData.wBuyBattleTimes = PBRoleScoreMatchData.buy_battle_times();
			tScoreMatchData.byCamp = PBRoleScoreMatchData.camp();
			tScoreMatchData.wWinTimes = PBRoleScoreMatchData.win_times();

			// 任务奖励记录
			tScoreMatchData.wTaskRewardRecordNum = 0;	
			for (int k = 0; k < PBRoleScoreMatchData.task_reward_record_size() && k < MAX_SCORE_MATCH_TASK_NUM; ++k)
			{
				tScoreMatchData.arTaskRewardRecord[tScoreMatchData.wTaskRewardRecordNum++] = PBRoleScoreMatchData.task_reward_record(k);
			}
			// 挑战目标列表
			tScoreMatchData.wBattleTargetNum = 0;
			for (int k = 0; k < PBRoleScoreMatchData.battle_target_list_size() && k < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++k)
			{
				const ::PB::ScoreMatchTarget &tPBTarget = PBRoleScoreMatchData.battle_target_list(k);
				ScoreMatchTarget &tTarget = tScoreMatchData.arBattleTargetList[tScoreMatchData.wBattleTargetNum++];
				tTarget.dwTargetId = tPBTarget.target_id();
				tTarget.wStatus = tPBTarget.status();
			}

			m_PlayerScoreMatchDataList.insert(make_pair(tScoreMatchData.dwRoleId, tScoreMatchData));
		}
	}

	m_bIsLoadDBFinish = true;
	m_dwCurActivityIndex = dwCurActivityIndex;
	m_byActivityStatus = byActivityStatus;
	m_dwActivityOverTime = dwActivityOverTime;
	m_lProceesSaveTime = GetTickCount64();
}

// 加载数据请求
void CScoreMatchManager::LoadDataRequest()
{
	// todo::暂时不打开
	//return;

	ScoreMatchData_Load* pQuery = ScoreMatchData_Load::ALLOC();
	pQuery->SetIndex(DB_SCORE_MATCH_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_SCORE_MATCH);
	g_MapServer.MapDBQuery(pQuery);
}

// 是否活动开始
bool CScoreMatchManager::IsActivityStart()
{
	const ScoreMathActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetScoreMatchActivityTime(m_dwCurActivityIndex);
	if (!pActivityTimeCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]找不到王者赛季时间配置[season:%d]", __FUNCTION__, m_wCurSeason);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// 在日期指定的范围内
	if (nCurData > pActivityTimeCfg->dwStartDate && nCurData < pActivityTimeCfg->dwOverDate)
		return true;

	// 开始的当天
	if (nCurData == pActivityTimeCfg->dwStartDate && nCurTime >= pActivityTimeCfg->dwStartTime)
		return true;

	// 最后一天
	if (nCurData == pActivityTimeCfg->dwOverDate && nCurTime <= pActivityTimeCfg->dwOverTime)
		return true;

	return false;
}

// 是否活动结束
bool CScoreMatchManager::IsActivityOver()
{
	const ScoreMathActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetScoreMatchActivityTime(m_dwCurActivityIndex);
	if (!pActivityTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到积分赛季时间配置[ActivityIndex:%d]", __FUNCTION__, m_dwCurActivityIndex);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// 结束的了至少一天了
	if (nCurData > pActivityTimeCfg->dwOverDate)
		return true;

	// 最后一天
	if (nCurData == pActivityTimeCfg->dwOverDate && nCurTime > pActivityTimeCfg->dwOverTime)
		return true;

	return false;
}

// 是否活动发放奖励
bool CScoreMatchManager::IsActivityReward()
{
	// 延后3分钟发放
	if (time(NULL) >= m_dwActivityOverTime + 180)
		return true;

	return false;
}

// 活动开始
bool CScoreMatchManager::DoActivityStart()
{
	// 重置数据
	m_PlayerScoreMatchDataList.clear();

	// 重置排行榜
	CRankManager::Instance()->ResetRoleRank(RANK_SCORE_MATCH_SUPER);
	CRankManager::Instance()->ResetRoleRank(RANK_SCORE_MATCH_LEGEND);
	CRankManager::Instance()->ResetRoleRank(RANK_SCORE_MATCH_WARCRAFT);

	MAPLOG(ERROR_LEVEL, "[%s]积分赛活动已开始[ActivityIndex:%d]",__FUNCTION__, m_dwCurActivityIndex);

	return true;
}

// 活动结束
bool CScoreMatchManager::DoActivityOver()
{
	m_dwActivityOverTime = time(NULL);

	MAPLOG(ERROR_LEVEL, "[%s]积分赛活动已结束[ActivityIndex:%d]", __FUNCTION__, m_dwCurActivityIndex);

	return true;
}

// 处理活动奖励
void CScoreMatchManager::DoActivityReward()
{
	// 超能排行榜
	DoActivityReward(RANK_SCORE_MATCH_SUPER);

	// 传奇(传说)排行榜
	DoActivityReward(RANK_SCORE_MATCH_LEGEND);

	// 魔兽(幻想)排行榜
	DoActivityReward(RANK_SCORE_MATCH_WARCRAFT);

	// 进入下次活动
	m_dwCurActivityIndex++;

	MAPLOG(ERROR_LEVEL, "[%s]积分赛活动已发放奖励[ActivityIndex:%d]", __FUNCTION__, m_dwCurActivityIndex);
}

// 处理活动奖励
void CScoreMatchManager::DoActivityReward(WORD wRankType)
{
	// 超能排行榜
	const AllRoleRankList *pAllRankList = CRankManager::Instance()->GetRankListEx(wRankType);
	if (!pAllRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到积分赛排行榜数据[RankType:%d]", __FUNCTION__, wRankType);
		return;
	}

	AllRoleRankList::const_iterator iter1 = pAllRankList->begin();
	for (; iter1 != pAllRankList->end(); ++iter1)
	{
		RoleRankList::const_iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			const ScoreMatchRankReward_Config *pRewardCfg = g_pCfgMgr->GetScoreMatchRankReward(iter2->wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 28;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = iter2->wRank;
				CRewardCenterManager::AddReward(iter2->dwServerId, iter2->dwRoleId, tRewardData);

				MAPLOG(GUI_LEVEL, "给玩家发放积分赛排行奖励[role_id:%d,rank:%d,rank_type:%d]", iter2->dwRoleId, iter2->wRank, wRankType);
			}
		}
	}
}

// 请求积分赛数据
void CScoreMatchManager::OnNetScoreMatchData(Player *pPlayer)
{
	SendScoreMatchData(pPlayer);
}

// 选择阵营
void CScoreMatchManager::OnNetSelectCamp(Player *pPlayer, BYTE byCamp)
{
	// 检测是否在活动中
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛活动还没开始或者已结束", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return;
	}

	// 检测参与人数(程序层面的判断,估计不会超过,万一超过就再调整)
	if (m_PlayerScoreMatchDataList.size() >= MAX_SCORE_MATCH_JOIN_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛参与人数已达上限![size:%d]", __FUNCTION__, m_PlayerScoreMatchDataList.size());
		return;
	}

	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营参数不合法![camp:%d]",__FUNCTION__, byCamp);
		return;
	}

	// 如果有数据则表现已经选过阵营
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]已经选择过阵营![camp:%d]", __FUNCTION__, byCamp);
		return;
	}

	ScoreMatchData tData;
	tData.byCamp = byCamp;
	tData.wBattleTimes = 0;
	tData.wBuyBattleTimes = 0;
	tData.wRefreshTimes = 0;
	tData.wWinTimes = 0;
	tData.wTaskRewardRecordNum = 0;
	tData.dwRoleId = pPlayer->GetRoleId();
	m_PlayerScoreMatchDataList.insert(make_pair(pPlayer->GetRoleId(), tData));
	pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家积分赛数据![roleid:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 获取挑战目标
	vector<ScoreMatchTarget> vTargetList;
	if (!GetTargetList(pPlayer, vTargetList) || vTargetList.size() != SCORE_MATCH_TARGET_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]获取积分赛挑战目标出错[num:%d]!", __FUNCTION__, vTargetList.size());
		return;
	}

	// 记录挑战目标
	pScoreMatchData->wBattleTargetNum = 0;
	for (int i = 0; i < vTargetList.size() && i < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++i)
	{
		pScoreMatchData->arBattleTargetList[pScoreMatchData->wBattleTargetNum++] = vTargetList[i];
	}

	SendScoreMatchData(pPlayer);

	// 响应
	MSG_S2C_SCORE_MATCH_SELECT_CAMP_RESP msg;
	msg.m_byCamp = byCamp;
	pPlayer->SendPacket(&msg);
}

// 刷新目标
void CScoreMatchManager::OnNetRefreshTarget(Player *pPlayer)
{
	if (!DoRefreshTarget(pPlayer, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理刷新挑战目标失败[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	SendScoreMatchData(pPlayer);

	// 响应
	MSG_S2C_SCORE_MATCH_REFRESH_TARGET_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 购买挑战次数
void CScoreMatchManager::OnNetBuyBattleTimes(Player *pPlayer)
{
	// 检测是否在活动中
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛活动还没开始或者已结束", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return;
	}

	// 取玩家积分赛数据
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]还木有选择阵营!", __FUNCTION__);
		return;
	}

	// 检测购买次数
	if (pScoreMatchData->wBuyBattleTimes >= pPlayer->GetVipValue(VIP_SCORE_MATCH_BATTLE_TIMES_BUY))
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛挑战次数购买已达上限!", __FUNCTION__);
		return;
	}

	int nCostDiamond = g_pCfgMgr->GetScoreMatchTimesCost(1, pScoreMatchData->wBuyBattleTimes + 1);

	// 检测消耗
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买积分赛挑战次数消耗的钻石不足[Cost:%d]!", __FUNCTION__, nCostDiamond);
		return;
	}
	
	// 添加购买次数
	pScoreMatchData->wBuyBattleTimes++;

	// 消耗钻石
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_SCORE_MATCH_BATTLE_TIMES_BUY);

	SendScoreMatchData(pPlayer);

	// 响应
	MSG_S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 领取挑战任务奖励
void CScoreMatchManager::OnNetTakeBattleTaskReward(Player *pPlayer, WORD *pTaskList, WORD wTaskNum)
{
	// 检测参数
	if (!pTaskList || !wTaskNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]领取奖励数量参数为空!", __FUNCTION__);
		return;
	}

	// 取玩家积分赛数据
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]还木有选择阵营[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 处理奖励
	for (WORD i = 0; i < wTaskNum; ++i)
	{
		// 取奖励配置
		WORD wTaskId = pTaskList[i];

		// 检测是否已领取过
		if (IsTaskRewardRecord(pPlayer->GetRoleId(), wTaskId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]积分赛挑战任务已领取过[RoleId:%d,TaskId:%d]!", __FUNCTION__, pPlayer->GetRoleId(), wTaskId);
			continue;
		}

		// 领取任务配置数据
		const ScoreMatchBattleTask_Config *pRewardCfg = g_pCfgMgr->GetScoreMatchBattleTask(wTaskId);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到积分赛挑战次数奖励配置[TaskId:%d]!", __FUNCTION__, wTaskId);
			return;
		}
		
		// 检测获胜次数
		if (pRewardCfg->wBattleTimes > pScoreMatchData->wBattleTimes)
		{
			MAPLOG(ERROR_LEVEL, "[%s]积分赛奖励需要的挑战次数不足[RewardId:%d]!", __FUNCTION__, wTaskId);
			return;
		}

		// 添加奖励
		pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_SCORE_MATCH_BATTLE_TIMES_REWRAD);

		// 添加任务记录
		AddTaskRewardRecord(pPlayer->GetRoleId(), wTaskId);
	}

	// 响应
	MSG_S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP msg;
	msg.m_wTaskNum = wTaskNum;
	memcpy_s(msg.m_TaskList, wTaskNum*sizeof(WORD), pTaskList, wTaskNum*sizeof(WORD));
	pPlayer->SendPacket(&msg);
}

// 开始战斗
bool CScoreMatchManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// 检测是否在活动中
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛活动还没开始或者已结束", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// 取玩家积分赛数据
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]还木有选择阵营!", __FUNCTION__);
		return false;
	}

	// 检测挑战次数
	if (pScoreMatchData->wBattleTimes >= g_GetConfigValue(95) + pScoreMatchData->wBuyBattleTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛挑战次数不足!", __FUNCTION__);
		return false;
	}

	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
	if (!pTargetBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛找不到挑战目标信息[TargetId:%d]!", __FUNCTION__, dwTargetId);
		return false;
	}

	pScoreMatchData->wBattleTimes++;

	pFBMgr->SetTargetId(dwTargetId);

	// 设置战力
	pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);

	pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

	return true;
}

// 结束战斗
bool CScoreMatchManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	return DoBattleOver(pPlayer, byIsWin);
}

void CScoreMatchManager::SendScoreMatchData(Player *pPlayer)
{
	MSG_S2C_SCORE_MATCH_DATA_RESP msg;
	msg.m_wBattleTargetNum = 0;
	msg.m_wCurActivityIdx = m_dwCurActivityIndex;

	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (pScoreMatchData)
	{
		msg.m_MatchData = *pScoreMatchData;
		for (WORD i = 0; i < pScoreMatchData->wBattleTargetNum && i < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++i)
		{
			DWORD dwTargetId = pScoreMatchData->arBattleTargetList[i].dwTargetId;
			const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
			if (pTargetBaseData)
				msg.m_BattleTargetList[msg.m_wBattleTargetNum++] = *pTargetBaseData;

			// todo:: 查找BUG日志
			//MAPLOG(ERROR_LEVEL, "挑战目标数据![TargetId:%d,IsDead:%d]", dwTargetId, pScoreMatchData->arBattleTargetList[i].wStatus);
		}
	}
	else
	{
		msg.m_MatchData.wBattleTimes = 0;
		msg.m_MatchData.wRefreshTimes = 0;
		msg.m_MatchData.wBuyBattleTimes = 0;
		msg.m_MatchData.byCamp = CAMP_INVALID;
		msg.m_MatchData.wWinTimes = 0;
		msg.m_MatchData.wTaskRewardRecordNum = 0;
	}

	//MAPLOG(ERROR_LEVEL, "当前活动索引![m_dwCurActivityIndex:%d]", msg.m_wCurActivityIdx);

	pPlayer->SendPacket(&msg);
}

// 取挑战目标列表
bool CScoreMatchManager::GetTargetList(Player *pPlayer, vector<ScoreMatchTarget> &vTargetList)
{
	// 取挑战目标服务器列表
	const ScoreMatchServerGroup_Config *pGroupServerCfg = g_pCfgMgr->GetScoreMatchTargetServerList(pPlayer->GetServerId());
	if (!pGroupServerCfg || !pGroupServerCfg->vGroupList.size())
	{
	 	MAPLOG(ERROR_LEVEL, "[%s]找不到积分赛挑战目标服务器列表或者列表为空![ServerId:%d]",__FUNCTION__, pPlayer->GetServerId());
	 	return false;
	}

	// 随机打乱原服务器列表
	map<DWORD,DWORD> vFindList;
	vector<DWORD> vTargetServerList;
	for (int i = 0; i < pGroupServerCfg->vGroupList.size(); ++i)
	{
		DWORD dwServerId = g_MapServer.GetCurServerId(pGroupServerCfg->vGroupList.at(i));

		// 防止合服的情况，出现一个服添加多次
		if (vFindList.find(dwServerId) == vFindList.end())
		{
			vTargetServerList.push_back(dwServerId);
			vFindList.insert(make_pair(dwServerId, dwServerId));
		}
	}
	int nTargetServerListSize = vTargetServerList.size();
	for (int i = 0; i < nTargetServerListSize; ++i)
	{
		int nIndex = g_Rand(0, nTargetServerListSize-1);
		swap(vTargetServerList[nIndex], vTargetServerList[nTargetServerListSize - 1]);
	}

	// 取查找随机目标配置
	const ScoreMatchSearchTarget_Config *pRandTargetCfg = g_pCfgMgr->GetScoreMatchSearchTarget(2);
	if (!pRandTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到查找随机目标配置!", __FUNCTION__);
		return false;
	}
	// 战力比例列表配置出错
	if (!pRandTargetCfg->vBattleValueRateMinList.size() ||
		pRandTargetCfg->vBattleValueRateMinList.size() != pRandTargetCfg->vBattleValueRateMaxList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛查找目标战力比例列表配置出错[rand]!", __FUNCTION__);
		return false;
	}
	// 取低战目标配置
	const ScoreMatchSearchTarget_Config *pLowTargetCfg = g_pCfgMgr->GetScoreMatchSearchTarget(1);
	if (!pLowTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到查找低战力目标配置!", __FUNCTION__);
		return false;
	}
	// 战力比例列表配置出错
	if (!pLowTargetCfg->vBattleValueRateMinList.size() ||
		pLowTargetCfg->vBattleValueRateMinList.size() != pLowTargetCfg->vBattleValueRateMaxList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛查找目标战力比例列表配置出错[low]!", __FUNCTION__);
		return false;
	}

	DWORD dwBattleValue = pPlayer->GetFightingPower();

	vector<ScoreMatchTarget> vRandTargetList;
	vector<ScoreMatchTarget> vLowTargetList;

	// 取随机战力玩家
	for (int i = 0; i < pRandTargetCfg->vBattleValueRateMinList.size(); ++i)
	{
		// 目标战力上下限
		DWORD dwTargetBattleValueMin = dwBattleValue - double(pRandTargetCfg->vBattleValueRateMinList[i] * 0.0001) * dwBattleValue;
		DWORD dwTargetBattleValueMax = dwBattleValue + double(pRandTargetCfg->vBattleValueRateMaxList[i] * 0.0001) * dwBattleValue;

		// 查找问题日志
		//MAPLOG(ERROR_LEVEL, "随机战力目标[PowerMin:%d,PowerMax:%d,idx:%d]!", dwTargetBattleValueMin, dwTargetBattleValueMax, i);

		// 已查找到指定的人数
		if (SearchTargetList(&vTargetServerList, pPlayer, vRandTargetList, dwTargetBattleValueMin, 
			dwTargetBattleValueMax, 20, pRandTargetCfg->wTargetNum, vTargetList))
			break;
	}

	// 检测候选目标列表人数是否足够
	if (pRandTargetCfg->wTargetNum > vRandTargetList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]随机战力目标列表人数不足[size:%d,role_id:%d,power:%d]!", 
			__FUNCTION__, vRandTargetList.size(), pPlayer->GetRoleId(), dwBattleValue);
		return false;
	}

// 	MAPLOG(ERROR_LEVEL, "随机战力目标列表数量[size:%d", vRandTargetList.size());
// 	for (int i = 0; i < vRandTargetList.size(); ++i)
// 	{
// 		MAPLOG(ERROR_LEVEL, "目标ID[%d]", vRandTargetList[i].dwTargetId);
// 	}

	// 随机目标
	while (vRandTargetList.size())
	{
		// 目标已达成
		if (vTargetList.size() >= pRandTargetCfg->wTargetNum)
			break;

		int nMax = vRandTargetList.size() - 1;
		int nIndex = g_Rand(0, nMax);
		//MAPLOG(ERROR_LEVEL, "size:%d,index:%d", vRandTargetList.size(), nIndex);
		vTargetList.push_back(vRandTargetList[nIndex]);	

		// 删除已经选中的
		vRandTargetList[nIndex] = vRandTargetList[nMax];
		vRandTargetList.pop_back();
	}

	// 取低战力玩家
	for (int i = 0; i < pLowTargetCfg->vBattleValueRateMinList.size(); ++i)
	{
		// 目标战力上下限
		DWORD dwTargetBattleValueMin = dwBattleValue - double(pLowTargetCfg->vBattleValueRateMinList[i] * 0.0001) * dwBattleValue;
		DWORD dwTargetBattleValueMax = dwBattleValue + double(pLowTargetCfg->vBattleValueRateMaxList[i] * 0.0001) * dwBattleValue;

		// 查找问题日志
		MAPLOG(ERROR_LEVEL, "低战战力目标[PowerMin:%d,PowerMax:%d,idx:%d]!", dwTargetBattleValueMin, dwTargetBattleValueMax, i);

		// 已查找到指定的人数
		if (SearchTargetList(&vTargetServerList, pPlayer, vLowTargetList, dwTargetBattleValueMin, 
			dwTargetBattleValueMax, 10, pLowTargetCfg->wTargetNum, vTargetList))
			break;
	}

	// 检测候选目标列表人数是否足够
	if (pLowTargetCfg->wTargetNum > vLowTargetList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]低战力目标列表人数不足[size:%d,role_id:%d,power:%d]!", 
			__FUNCTION__, vLowTargetList.size(), pPlayer->GetRoleId(), dwBattleValue);
		return false; 
	}

	//MAPLOG(ERROR_LEVEL, "低战力目标列表数量[size:%d", vRandTargetList.size());

	// 随机目标
	while (vLowTargetList.size())
	{
		// 目标已达成
		if (vTargetList.size() >= pRandTargetCfg->wTargetNum + pLowTargetCfg->wTargetNum)
			break;

		int nMax = vLowTargetList.size() - 1;
		int nIndex = g_Rand(0, nMax);
		vTargetList.push_back(vLowTargetList[nIndex]);

		// 删除已经选中的
		vLowTargetList[nIndex] = vLowTargetList[nMax];
		vLowTargetList.pop_back();
	}

	return true;
}

// 取挑战目标列表
bool CScoreMatchManager::SearchTargetList(const vector<DWORD> *pServerList, Player *pPlayer, vector<ScoreMatchTarget> &vTargetList,
	DWORD dwBattleValueMin, DWORD dwBattleValueMax, int nMaxTargetNum, int nMinTargetNum, const vector<ScoreMatchTarget> &vExceptList)
{
	// 清空目标列表
	vTargetList.clear();

	ScoreMatchTarget tTarget;
	for (vector<DWORD>::const_iterator iter = pServerList->begin(); iter != pServerList->end(); ++iter)
	{
		DWORD dwServerId = *iter;
		const PlayerBaseDataMap *pPlayerList = PlayerBaseDataManager::Instance()->GetPlayerList(dwServerId);
		if (!pPlayerList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到服务器玩家列表![ServerId:%d]", __FUNCTION__, dwServerId);
			continue;
		}

		PlayerBaseDataMap::const_iterator iter1 = pPlayerList->begin();
		for (; iter1 != pPlayerList->end(); ++iter1)
		{
			PlayerBaseData *pBaseData = iter1->second;

			// 去掉自己
			if (pPlayer->GetRoleId() == pBaseData->dwRoleId)
				continue;

			bool bIsExist = false;
			for (int i = 0; i < vExceptList.size(); ++i)
			{
				if (vExceptList[i].dwTargetId == pBaseData->dwRoleId)
				{
					bIsExist = true;
					break;
				}
			}

			// 检测是否存在
			if (bIsExist)
				continue;

			// 战力在指定的范围内
			if (pBaseData->dwFightingPower >= dwBattleValueMin && pBaseData->dwFightingPower <= dwBattleValueMax)
			{
				tTarget.wStatus = 0;
				tTarget.dwTargetId = pBaseData->dwRoleId;
				vTargetList.push_back(tTarget);

				//MAPLOG(ERROR_LEVEL, "目标数据[ServerId:%d,TargetId:%d]", dwServerId, pBaseData->dwRoleId);
			}

			// 目标列表已达标
			if (vTargetList.size() >= nMaxTargetNum)
				return true;
		}
	}

	// 已达到最小目标数量也算成功
	if (vTargetList.size() >= nMinTargetNum)
		return true;
	
	return false;
}


// 取玩家积分赛数据
ScoreMatchData* CScoreMatchManager::GetPlayerScoreMatchData(DWORD dwRoleId)
{
	std::map<DWORD, ScoreMatchData>::iterator iter = m_PlayerScoreMatchDataList.find(dwRoleId);
	return iter != m_PlayerScoreMatchDataList.end() ? &iter->second : NULL;
}

// 通过阵营取排行榜类型
WORD CScoreMatchManager::GetRankTypeByCamp(WORD wCamp)
{
	WORD wRankType;

	// 超能
	if (CAMP_SUPER == wCamp)
	{
		wRankType = RANK_SCORE_MATCH_SUPER;
	}
	// 传奇(传说)
	else if (CAMP_LEGEND == wCamp)
	{
		wRankType = RANK_SCORE_MATCH_LEGEND;
	}
	// 魔兽(幻想)
	else
	{
		wRankType = RANK_SCORE_MATCH_WARCRAFT;
	}

	return wRankType;
}

// 是否任务任务奖励记录
bool CScoreMatchManager::IsTaskRewardRecord(DWORD dwRoleId, WORD wTaskId)
{
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(dwRoleId);
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家积分赛数据![dwRoleId:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	
	for (WORD i = 0; i < pScoreMatchData->wTaskRewardRecordNum; ++i)
	{
		if (pScoreMatchData->arTaskRewardRecord[i] == wTaskId)
			return true;
	}
	
	return false;
}

// 添加任务奖励记录
void CScoreMatchManager::AddTaskRewardRecord(DWORD dwRoleId, WORD wTaskId)
{
	// 领数据
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(dwRoleId);
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家积分赛数据![dwRoleId:%d]", __FUNCTION__, dwRoleId);
		return;
	}

	// 检测是否已满
	if (pScoreMatchData->wTaskRewardRecordNum >= MAX_SCORE_MATCH_TASK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家积分赛任务奖励列表已满[num:%d]!", 
			__FUNCTION__, pScoreMatchData->wTaskRewardRecordNum);
		return;
	}

	pScoreMatchData->arTaskRewardRecord[pScoreMatchData->wTaskRewardRecordNum++] = wTaskId;
}

// 处理战斗结束逻辑
bool CScoreMatchManager::DoBattleOver(Player *pPlayer, BYTE byIsWin)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// 取玩家积分赛数据
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]还木有选择阵营!", __FUNCTION__);
		return false;
	}

	DWORD dwTargetId = pFBMgr->GetTargetId();


	// 检测挑战目标
	ScoreMatchTarget *pTarget = NULL;
	for (WORD i = 0; i < pScoreMatchData->wBattleTargetNum; ++i)
	{
		if (pScoreMatchData->arBattleTargetList[i].dwTargetId == dwTargetId)
		{
			pTarget = &pScoreMatchData->arBattleTargetList[i];
			break;
		}
	}
	if (!pTarget)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到挑战目标数据[TargetId:%d]!", __FUNCTION__, dwTargetId);
		return false;
	}

	// 检测目标是否已挂掉
	if (pTarget->wStatus)
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战目标已挑战过[TargetId:%d,Status:%d]!", __FUNCTION__, dwTargetId, pTarget->wStatus);
		return false;
	}

	// 取挑战目标基本数据
	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
	if (!pTargetBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到挑战目标基本数据[TargetId:%d]!", __FUNCTION__, dwTargetId);
		return false;
	}

	double fRate = g_GetConfigValue(290) * 0.0001;

	DWORD dwScore, dwScoreMatchHonor;


	// 积分 = 50+对手战力/5万
	//dwScore = 50 + pTargetBaseData->dwFightingPower * 0.0005;

	// 胜利
	if (byIsWin)
	{

		// 战胜积分=math.floor(math.pow(敌人战力,0.3354)+50)
		dwScore = floor(pow(pTargetBaseData->dwFightingPower, fRate) + 50);

		// 战胜荣誉=math.floor(math.pow(敌人战力,0.3354)+50)
		dwScoreMatchHonor = dwScore;

		// 更新获胜次数
		pScoreMatchData->wWinTimes++;

		pTarget->wStatus = 1;
	}
	// 失败
	else
	{
		// 战败积分=math.floor(math.pow(敌人战力,0.3354)/2+25)
		dwScore = floor(pow(pTargetBaseData->dwFightingPower, fRate) * 0.5 + 25);

		// 战败荣誉=math.floor(math.pow(敌人战力,0.3354)/2+25)
		dwScoreMatchHonor = dwScore;

		pTarget->wStatus = 2;
	}

	// 检测是否需要重新刷新
	bool bIsNeedRefresh = true;
	for (WORD i = 0; i < pScoreMatchData->wBattleTargetNum; ++i)
	{
		if (!pScoreMatchData->arBattleTargetList[i].wStatus)
		{
			bIsNeedRefresh = false;
			break;
		}
	}
	if (bIsNeedRefresh)
		DoRefreshTarget(pPlayer, false);

	pPlayer->ModifyProperty(ROLE_PRO_SCORE_MATCH_HONOR, dwScoreMatchHonor, TRUE);

	WORD wRankType = GetRankTypeByCamp(pScoreMatchData->byCamp);

	// 通知客户端获得的积分及荣誉
	pFBMgr->SetParam(0, dwScore);
	pFBMgr->SetParam(1, dwScoreMatchHonor);


	// 处理排行榜
	const ScoreMatchServerGroup_Config *pGroupServerCfg = g_pCfgMgr->GetScoreMatchTargetServerList(pPlayer->GetServerId());
	if (pGroupServerCfg)
	{
		DWORD dwTotalScore = CRankManager::Instance()->GetRoleRankValue(pPlayer->GetRoleId(), pGroupServerCfg->wRankGroupId, wRankType) + dwScore;
		CRankManager::Instance()->AddRoleRankData(wRankType, pPlayer, dwTotalScore, pScoreMatchData->wWinTimes, 0, 200);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到积分排行榜配置[serverid:%d, type:%d]", __FUNCTION__, pPlayer->GetServerId(), wRankType);
	}

	return true;
}

// 处理刷新目标逻辑
bool CScoreMatchManager::DoRefreshTarget(Player *pPlayer, bool bIsNeedCost)
{
	// 检测是否在活动中
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛活动还没开始或者已结束", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return false;
	}

	// 取玩家的积分赛数据
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]还木有选择阵营!", __FUNCTION__);
		return false;
	}

	int nCostDiamond = g_pCfgMgr->GetScoreMatchTimesCost(2, pScoreMatchData->wRefreshTimes + 1);

	// 消耗钻石不足(todo::前面几次可能会配置为免费)
	if (bIsNeedCost && pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]积分赛刷新目标消耗钻石不足[Cost:%d]!", __FUNCTION__, nCostDiamond);
		return false;
	}

	pScoreMatchData->wBattleTargetNum = 0;

	// 获取挑战目标
	vector<ScoreMatchTarget> vTargetList;
	if (!GetTargetList(pPlayer, vTargetList) || vTargetList.size() != SCORE_MATCH_TARGET_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]获取积分赛挑战目标出错[num:%d]!", __FUNCTION__, vTargetList.size());
		return false;
	}

	// 需要消耗
	if (bIsNeedCost)
	{
		// 处理消耗
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_SCORE_MATCH_REFRESH_TARGET);

		// 添加刷新次数
		pScoreMatchData->wRefreshTimes++;
	}

	// 记录挑战目标
	// 记录挑战目标
	//pScoreMatchData->wBattleTargetNum = 0;
	for (int i = 0; i < vTargetList.size() && i < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++i)
	{
		pScoreMatchData->arBattleTargetList[pScoreMatchData->wBattleTargetNum++] = vTargetList[i];
	}

	return true;
}

