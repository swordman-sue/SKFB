#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Hero\HeroManager.h"
#include "DBCDefine.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Singleton.h"
#include "ArenaManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\MapServer.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Email\EmailManager.h"
#include "..\MapSvrQuery.h"
#include "data_define.pb.h"
#include "..\SevenDayActivity\SevenDayManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Shop\ShopManager.h"
#include "..\Activity\ActivityManager.h"


CArenaManager::CArenaManager()
{
	m_lProccessTime = GetTickCount64();
	m_dwDoRewardTime = time(NULL);
	m_bRankUpdate = false;
	m_bAlreadyLoadData = false;
	m_lProceesSaveTime = GetTickCount64();
	m_RankRewardTimer.AddTriggerTime(g_GetConfigValue(193));
}

CArenaManager::~CArenaManager()
{

}

void CArenaManager::Init()
{
	m_lProccessTime = GetTickCount64();
	m_dwDoRewardTime = time(NULL);
	m_bRankUpdate = false;
	m_bAlreadyLoadData = false;
	m_lProceesSaveTime = GetTickCount64();
	m_RankListForSendReward.clear();

	// 请求数据库加载数据
	ArenaBattleRecord_Load* pQuery = ArenaBattleRecord_Load::ALLOC();
	pQuery->SetIndex(DB_ARENA_BATTLE_RECORD_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_ARENA_BATTLE_RECORD);
	g_MapServer.MapDBQuery(pQuery);
}

void CArenaManager::Update()
{
	// todo::测试暂时屏蔽
	//return;

	// todo::暂时介样处理...
	if (m_RankRewardTimer.IsTrigger())
	{
		DoRankAward();
		m_RankRewardTimer.SetCDOverTime();
	}

	// todo::测试阶段1分钟处理
	if (GetTickCount64() >= m_lProceesSaveTime + 60000)
	{
		ArenaAllRankMap::iterator iter = m_RankList.begin();
		for (; iter != m_RankList.end(); ++iter)
		{
			m_SaveDataServerList.push_back(iter->first);
		}
		m_lProceesSaveTime = GetTickCount64();
	}

	if (m_SaveDataServerList.size())
	{	 
		SaveDataToDB(m_SaveDataServerList[m_SaveDataServerList.size() - 1]);
		m_SaveDataServerList.pop_back();
	}

	if (m_RankListForSendReward.size())
	{
		DWORD dwLastTime = GetTickCount64();
		int nPlayerNum = 0;
		ArenaAllRankMap::iterator iter_list = m_RankListForSendReward.begin();
		ArenaRankMap::iterator iter_rank_data = iter_list->second.begin();
		for (; iter_rank_data != iter_list->second.end(); ++iter_rank_data)
		{
			// 如果是机器人则不处理
			if (IsRobotId(iter_rank_data->dwTargetId))
				continue;

			RewardData tRewardData;
			tRewardData.wInfoId = 1;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = iter_rank_data->wCurRank;
			CRewardCenterManager::AddReward(iter_list->first, iter_rank_data->dwTargetId, tRewardData);

			nPlayerNum++;
		}
		
		DWORD dwCostTime = GetTickCount64() - dwLastTime;
		MAPLOG(ERROR_LEVEL, "[%s]竞技场发放奖励[server_id:%d,list_num:%d,player_num:%d,cost_time:%d]", 
			__FUNCTION__, iter_list->first, iter_list->second.size(), nPlayerNum, dwCostTime);

		m_RankListForSendReward.erase(iter_list);
	}
}

// 存储数据
void CArenaManager::SaveDataToDB(DWORD dwServerId)
{
	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场的排行榜列表[ServerId:%d]", __FUNCTION__, dwServerId);
		return;
	}

	PB::ArenaRankDataList PBRankDataList;
	for (size_t i = 0; i < pRankList->size(); ++i)
	{
		const ArenaRankData &tRankData = pRankList->at(i);
		PB::ArenaRankData *pPBRankData = PBRankDataList.add_rank_list();
		pPBRankData->set_role_id(tRankData.dwTargetId);
		pPBRankData->set_cur_rank(tRankData.wCurRank);
		pPBRankData->set_history_rank(tRankData.wHistoryBestRank);
	}

	string strSaveData("");
	PBRankDataList.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_ARENA_LIST_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场存储数据过大[ListSize:%d, DataSize:%d]", 
			__FUNCTION__, pRankList->size(), strSaveData.size());
		return;
	}

	ArenaDataList_Save* pQuery = ArenaDataList_Save::ALLOC();
	pQuery->SetIndex(DB_ARENA_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_ARENA);
	pQuery->SetServerId(dwServerId);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);

	MAPLOG(GUI_LEVEL, "[%s]竞技场数据存储请求[ListSize:%d, DataSize:%d]", 
		__FUNCTION__, pRankList->size(), strSaveData.size());
}

// 存储所有数据
void CArenaManager::SaveAllData()
{
	ArenaAllRankMap::iterator iter = m_RankList.begin();
	for (; iter != m_RankList.end(); ++iter)
	{
		SaveDataToDB(iter->first);
	}
}

// 加载数据
void CArenaManager::LoadDataFromDB(DWORD dwServerId, const char *pData, int size)
{
	ArenaRankMap vRankList;
	if (size)
	{
		PB::ArenaRankDataList PBRankDataList;
		if (!PBRankDataList.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "解析竞技场数据出错(%d)", size);
			return;
		}

		// 先清空数据
		m_RankList.erase(dwServerId);

		ArenaRankData tRankData;
		for (int i = 0; i < PBRankDataList.rank_list_size(); ++i)
		{
			const PB::ArenaRankData &tPBRankData = PBRankDataList.rank_list(i);
			tRankData.dwTargetId = tPBRankData.role_id();
			tRankData.wCurRank = tPBRankData.cur_rank();
			tRankData.wHistoryBestRank = tPBRankData.history_rank();
			vRankList.push_back(tRankData);
		}

		m_RankList.insert(make_pair(dwServerId, vRankList));
	}

	MAPLOG(GUI_LEVEL, "成功加载竞技场数据[data_size:%d,server_id:%d,list_num:%d]!", size, dwServerId, vRankList.size());
}

// 加载数据请求
void CArenaManager::LoadDataRequest()
{
	if (m_bAlreadyLoadData)
		return;

	const map<DWORD, ServerInfo> &mapServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = mapServerList.begin();
	for (; iter != mapServerList.end(); ++iter)
	{
		ArenaDataList_Load* pQuery = ArenaDataList_Load::ALLOC();
		pQuery->SetIndex(DB_ARENA_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_ARENA);
		pQuery->SetServerId(iter->second.dwServerId);
		g_MapServer.MapDBQuery(pQuery);
	}

	m_bAlreadyLoadData = true;
}

// 存储战斗记录数据
void CArenaManager::SaveBattleRecord(DWORD dwRoleId)
{
	ArenaBattleRecordMap::iterator map_iter = m_vBattleRecordList.find(dwRoleId);
	if (map_iter != m_vBattleRecordList.end())
	{
		ArenaBattleRecord_Save* pQuery = ArenaBattleRecord_Save::ALLOC();
		pQuery->SetIndex(DB_ARENA_BATTLE_RECORD_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_ARENA_BATTLE_RECORD);
		pQuery->SetRoleId(dwRoleId);

		list<ArenaBattleRecord>::iterator list_iter = map_iter->second.begin();
		for (; list_iter != map_iter->second.end(); ++list_iter)
		{
			pQuery->AddBattleRecord(&*list_iter);
		}

		g_MapServer.MapDBQuery(pQuery);
	}
}

// 请求挑战目标列表 
void CArenaManager::OnNetTargetList(Player *pPlayer)
{
	SendTargetList(pPlayer);
}

// 请求战报
void CArenaManager::OnNetBattleRecordList(Player *pPlayer)
{
	MSG_S2C_ARENA_BATTLE_RECORD_RESP msg;
	msg.m_byRecordNum = 0;

	ArenaBattleRecordMap::iterator map_iter = m_vBattleRecordList.find(pPlayer->GetCharID());
	if (map_iter != m_vBattleRecordList.end())
	{
		list<ArenaBattleRecord>::iterator list_iter = map_iter->second.begin();
		for (; list_iter != map_iter->second.end(); ++list_iter)
		{
			if (msg.m_byRecordNum >= MAX_ARENA_BATTLE_RECORD)
				break;
			msg.m_RecordList[msg.m_byRecordNum++] = *list_iter;
		}
	}

	pPlayer->SendPacket(&msg);
}

// 战斗开始
bool CArenaManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId, DWORD dwIsSweep)
{
	// 检测精力
	if (pPlayer->GetProperty(ROLE_PRO_ENERGY) < 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精力不足,竞技场挑战失败!", __FUNCTION__);
		return false;
	}


// 	// 如果是第一次挑战，则当作最后一名
// 	if (!GetPlayerRankData(pPlayer->GetServerId(),pPlayer->GetCharID()))
// 	{
// 		ArenaRankMap *pRankList = GetServerRankList(pPlayer->GetServerId());
// 		if (!pRankList)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场的排行榜列表[ServerId:%d]", __FUNCTION__, pPlayer->GetServerId());
// 			return false;
// 		}
// 		ArenaRankData tRankData;
// 		tRankData.dwTargetId = pPlayer->GetRoleId();
// 		tRankData.wCurRank = pRankList->size()+1;
// 		tRankData.wHistoryBestRank = 0;
// 		pRankList->push_back(tRankData);
// 	}

	// 检测目标
	vector<ArenaRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetServerId(), pPlayer->GetCharID(), &vTargetList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]取竞技场挑战目标列表失败", __FUNCTION__);
		return false;
	}

	bool bFound = false;
	for (int i = 0; i < vTargetList.size(); ++i)
	{
		if (vTargetList[i].dwTargetId == dwTargetId)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		SendTargetList(pPlayer);
		MAPLOG(ERROR_LEVEL, "[%s]挑战的目标不合法", __FUNCTION__);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);
	//pFBMgr->SetParam(0, dwTargetId);
	pFBMgr->SetBattleInfo(0, 0, 0);
	pFBMgr->SetTargetId(dwTargetId);
	pFBMgr->SetParam(1, dwIsSweep);

	// 如果是玩家,则需要把英雄发过去
	if (!IsRobotId(dwTargetId))
	{
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId, true);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// 设置战力
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}

	pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -2, TRUE, FROM_ARENA_BATTLE);

	return true;
}

// 战斗结束
bool CArenaManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	// 如果是第一次挑战，则当作最后一名
	if (!GetPlayerRankData(pPlayer->GetServerId(), pPlayer->GetCharID()))
	{
		// 如果是第一次挑战逃跑
		if (!byIsWin)
			return true;

		ArenaRankMap *pRankList = GetServerRankList(pPlayer->GetServerId());
		if (!pRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场的排行榜列表[ServerId:%d]", __FUNCTION__, pPlayer->GetServerId());
			return false;
		}
		ArenaRankData tRankData;
		tRankData.dwTargetId = pPlayer->GetRoleId();
		tRankData.wCurRank = pRankList->size() + 1;
		tRankData.wHistoryBestRank = 0;
		pRankList->push_back(tRankData);
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	DWORD dwTargetId = pFBMgr->GetTargetId();
	BYTE byIsSweep = pFBMgr->GetParam(1);

	ArenaRankData *pSrc = GetPlayerRankData(pPlayer->GetServerId(), pPlayer->GetRoleId());
	if (!pSrc)
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场挑战结束时,找不到挑战方数据", __FUNCTION__);
		return false;
	}
	ArenaRankData *pTarget = GetPlayerRankData(pPlayer->GetServerId(), dwTargetId);
	if (!pTarget)
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场挑战结束时,找不到目标数据", __FUNCTION__);
		return false;
	}

	const ArenaAward_Config* pAwardCfg = CConfigManager::Instance()->GetArenaAward(pPlayer->GetLevel());
	if (!pAwardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到竞技场奖励配置[RoleLevel:%d]", __FUNCTION__, pPlayer->GetLevel());
		return false;
	}

	// 添加挑战记录
	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId);
	if (pTargetBaseData)
	{
		AddBattleRecord(pPlayer->GetRoleId(), dwTargetId, pSrc->wCurRank, pTarget->wCurRank, pPlayer->GetRoleName(),
			pTargetBaseData->szRoleName, pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE), pTargetBaseData->wHeadIcon,
			pPlayer->GetLevel(), pTargetBaseData->wLevel, byIsWin);
	}
	else
	{
		AddBattleRecord(pPlayer->GetRoleId(), dwTargetId, pSrc->wCurRank, pTarget->wCurRank, pPlayer->GetRoleName(),
			"", pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE), 0, pPlayer->GetLevel(), 0, byIsWin);
	}

	int nMultiple = CActivityManager::GetLimitProductMultiple(BATTLE_ARENA_MAP);

	if (byIsWin)
	{
		// 如果目标的排名比我高
		if (pSrc->wCurRank > pTarget->wCurRank)
		{
			// 处理广播(只广播进入前3名的)
			if (pTarget->wCurRank <= 3)
			{
				CSystemNotice sys_notice(4, pPlayer->GetServerId());
				sys_notice.AddParam(pPlayer->GetRoleName());
				const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId, false);
				if (pTargetBaseData)
					sys_notice.AddParam(pTargetBaseData->szRoleName);
				else
				{
					const ArenaRobot_Config *pRobotCfg = g_pCfgMgr->GetArenaRobot(dwTargetId);
					sys_notice.AddParam(pRobotCfg ? pRobotCfg->strRobotName.c_str() : "Robot");
				}
						
				sys_notice.AddParam(pTarget->wCurRank);
				sys_notice.SendMsg();
			}


			// 首次排行奖励
			if (!pSrc->wHistoryBestRank || pTarget->wCurRank < pSrc->wHistoryBestRank)
			{
				int nTotalDiamond = 0;
				WORD wMax = pSrc->wHistoryBestRank ? pSrc->wHistoryBestRank : CConfigManager::Instance()->m_ArenaFirstRankRewardList.size() + 1;
				for (WORD i = pTarget->wCurRank; i < wMax; ++i)
				{
					nTotalDiamond += CConfigManager::Instance()->GetArenaFirstRankReward(i);
				}
				
				if (nTotalDiamond)
					pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, nTotalDiamond, TRUE, FROM_ARENA_BATTLE);

				MAPLOG(GUI_LEVEL, "本次竞技场挑战获得钻石奖励[%d][TargetRank:%d, SrcRank:%d]", 
					nTotalDiamond, pTarget->wCurRank, pSrc->wCurRank);
			}

			// 交换位置
			swap(*pSrc, *pTarget);	// 整个数据交换
			swap(pSrc->wCurRank, pTarget->wCurRank);	// 排名交换
			swap(pSrc, pTarget);	// 交换地址

			// 更新最佳排名记录
			if (pSrc->wHistoryBestRank)
				pSrc->wHistoryBestRank = min(pSrc->wHistoryBestRank, pSrc->wCurRank);
			else
				pSrc->wHistoryBestRank = pSrc->wCurRank;

			//SendTargetList(pPlayer);

			// 竞技场防守失败(排名降低)
			if (!byIsSweep && !IsRobotId(dwTargetId))
			{
				// 发邮件通知
				EmailInfo tEailInfo(2);
				tEailInfo.AddContext(EMAIL_CONTEXT_STRING, pPlayer->GetRoleName());
				tEailInfo.AddContext(EMAIL_CONTEXT_INT, pTarget->wCurRank);
				EmailManager::AddEmail(dwTargetId, tEailInfo, true);
			}

			m_bRankUpdate = true;

			// 7天活动事件(竞技场排名)
			pPlayer->OnUpdateSevenDayTarget(6, pSrc->wHistoryBestRank, false);
		}
		else
		{
			// 竞技场防守失败(排名不变)
			if (!byIsSweep && !IsRobotId(dwTargetId))
			{
				// 发邮件通知
				EmailInfo tEailInfo(4);
				tEailInfo.AddContext(EMAIL_CONTEXT_STRING, pPlayer->GetRoleName());
				EmailManager::AddEmail(dwTargetId, tEailInfo, true);
			}
		}

		// 添加可翻牌
		vector<Item> vDropRewardItemList;
		pPlayer->GetFubenManager()->DoLimitDrop(9999, vDropRewardItemList);
		if (vDropRewardItemList.size())
		{
			// 正常不会有多个,如果出现多个,是只取第一个
			pPlayer->GetItemManager()->SetFPItem(vDropRewardItemList[0]);
		}
		else
		{
			// 如果没有限时掉落的,则取正常的
			pPlayer->GetItemManager()->SetFPItem(Item(pAwardCfg->wFPItemId,1));
		}

		pPlayer->ModifyProperty(ROLE_PRO_GOLD, pAwardCfg->nWinGold * nMultiple, TRUE, FROM_ARENA_BATTLE);
		pPlayer->ModifyProperty(ROLE_PRO_ARENA_PRESTIGE, pAwardCfg->nWinArenaPrestige * nMultiple, TRUE, FROM_ARENA_BATTLE);
	}
	else
	{
		pPlayer->ModifyProperty(ROLE_PRO_GOLD, pAwardCfg->nFailGold * nMultiple, TRUE, FROM_ARENA_BATTLE);
		pPlayer->ModifyProperty(ROLE_PRO_ARENA_PRESTIGE, pAwardCfg->nFailArenaPrestige * nMultiple, TRUE, FROM_ARENA_BATTLE);

		// 发邮件通知目标(防守成功)
		if (!byIsSweep && !IsRobotId(dwTargetId))
		{
			// 发邮件通知
			EmailInfo tEailInfo(1);
			tEailInfo.AddContext(EMAIL_CONTEXT_STRING, pPlayer->GetRoleName());
			EmailManager::AddEmail(dwTargetId, tEailInfo, true);
		}
	}

	// 经验奖励
	pPlayer->AddExp(pAwardCfg->dwExp);

	// 日常任务
	pPlayer->OnUpdateDailyMission(10);

	// 8-14天活动事件(挑战竞技场（无论胜负）)
	pPlayer->OnUpdateSevenDayTarget(23, 1, true);

	// 节日活动
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 8, 1, true);

	// 节日活跃目标
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 8, 1, true);	

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 8, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 8, 1, true);

	

	return true;
}

// 取玩家的排行
WORD CArenaManager::GetPlayerRank(DWORD dwServerId, DWORD dwRoleId)
{
	ArenaRankData *pRankData = GetPlayerRankData(dwServerId, dwRoleId);
	return pRankData ? pRankData->wCurRank : 0;
}

// 取玩家的历史排行
WORD CArenaManager::GetPlayerHistoryRank(DWORD dwServerId, DWORD dwRoleId)
{
	ArenaRankData *pRankData = GetPlayerRankData(dwServerId, dwRoleId);
	return pRankData ? pRankData->wHistoryBestRank : 0;
}

// 取挑战目标列表
bool CArenaManager::GetTargetList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutTargetList)
{
	pOutTargetList->clear();

	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场的排行榜列表[ServerId:%d]", __FUNCTION__, dwServerId);
		return false;
	}

	// 检测排行榜数量 todo::日后调整
	if (pRankList->size() < 5000)
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场排行榜数量少于指定数量[ServerId:%d,size:%d]", 
			__FUNCTION__, dwServerId, pRankList->size());
		return false;
	}

	WORD wShowRank = 10;

	// 取前10名
	for (ArenaRankMap::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		if (pOutTargetList->size() >= wShowRank)
			break;
		pOutTargetList->push_back(*iter);
	}

	WORD wRank = GetPlayerRank(dwServerId, dwRoleId);

	bool bFirstTimes = false;

	// 如果还没入榜，则当作最后一名
	if (!wRank)
	{
		wRank = pRankList->size() + 1;
		bFirstTimes = true;
	}

	const ArenaRank_Config *pRankCfg = g_pCfgMgr->GetArenaRank(wRank);
 	if (!pRankCfg)
 	{
		// 如果找不到则使用最一个排名的配置
		pRankCfg = g_pCfgMgr->GetArenaRank(g_pCfgMgr->GetAreanRankNum());
		if (!pRankCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场排名配置[Rank:%d]", __FUNCTION__, g_pCfgMgr->GetAreanRankNum());
			return false;
		}
 	}

	// 防止为0的情况
	WORD wTargetRankInterval = bFirstTimes ? 1 : max(pRankCfg->wTargetRankInterval, 1);

	// 排名在10名之外
	vector<ArenaRankData> vTmpRankList;
	if (wRank > wShowRank)
	{
		// 上面目标(只取9名)
		int nTargetRank = wRank - wTargetRankInterval * 9;
		nTargetRank = max(nTargetRank, 0);
		while (wTargetRankInterval && nTargetRank < wRank)
		{
			//pOutTargetList->push_back((*pRankList)[nTargetRank-1]);
			vTmpRankList.push_back((*pRankList)[nTargetRank - 1]);
			nTargetRank += wTargetRankInterval;
		}

		for (size_t i = 0; i < vTmpRankList.size(); ++i)
		{
			bool bHave = false;
			for (size_t k = 0; k < pOutTargetList->size(); ++k)
			{
				if (pOutTargetList->at(k).dwTargetId == vTmpRankList[i].dwTargetId)
				{
					bHave = true;
					break;
				}
			}
			if (!bHave)
			{
				pOutTargetList->push_back(vTmpRankList[i]);
			}
		}

		// 下面目标(只取两名)
		nTargetRank = wRank;
		for (int i = 0; i < 2/*todo::暂时介么处理*/; ++i)
		{
			nTargetRank += wTargetRankInterval;
			if (pRankList->size() > nTargetRank)
				pOutTargetList->push_back((*pRankList)[nTargetRank-1]);
		}
	}
	// 排名在10名之内
	else
	{
		pOutTargetList->push_back((*pRankList)[wShowRank]);
		pOutTargetList->push_back((*pRankList)[wShowRank+1]);
	}	

	return true;
}

// 发送目标列表
void CArenaManager::SendTargetList(Player *pPlayer)
{

	vector<ArenaRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetServerId(), pPlayer->GetCharID(), &vTargetList))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ARENA_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]取竞技场挑战目标列表失败", __FUNCTION__);
		return;
	}

	MSG_S2C_ARENA_TARGET_LIST_RESP msg;
	msg.m_wRank = 0; // GetPlayerRank(pPlayer->GetServerId(), pPlayer->GetCharID());
	msg.m_wHistoryRank = 0;
	msg.m_byTargetNum = 0;
	ArenaRankData *pRankData = GetPlayerRankData(pPlayer->GetServerId(), pPlayer->GetCharID());
	if (pRankData)
	{
		msg.m_wRank = pRankData->wCurRank;
		msg.m_wHistoryRank = pRankData->wHistoryBestRank;
	}
	for (vector<ArenaRankData>::iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter)
	{
		if (msg.m_byTargetNum >= MAX_ARENA_TARGET_NUM)
			break;

		ArenaTargetData &tTargetData = msg.m_TargetList[msg.m_byTargetNum++];
		memset(&tTargetData, 0, sizeof(tTargetData));
		tTargetData.dwTargetId = iter->dwTargetId;
		tTargetData.wRank = iter->wCurRank;
		if (!IsRobotId(iter->dwTargetId))
		{
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), iter->dwTargetId);
			if (pBaseData)
			{
				strcpy_s(tTargetData.szTargetName, sizeof(tTargetData.szTargetName) - 1, pBaseData->szRoleName);
				tTargetData.wHeadIcon = pBaseData->wHeadIcon;
				tTargetData.dwFightingPower = pBaseData->dwFightingPower;
				tTargetData.dwTitleId = pBaseData->dwTitleId;
				tTargetData.dwNicknameId = pBaseData->dwNicknameId;
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// 取排行榜列表
void CArenaManager::GetRankList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutRankList, WORD wMaxNum)
{
	pOutRankList->clear();
	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场的排行榜列表[ServerId:%d]", __FUNCTION__, dwServerId);
		return;
	}

	for (vector<ArenaRankData>::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		if (pOutRankList->size() >= wMaxNum)
			break;
		pOutRankList->push_back(*iter);
	}
}

// 处理排行奖励
void CArenaManager::DoRankAward()
{	// todo::如果后期人数比较多的话，这个逻辑会比较在压力，需要调整
	
	//WORD wRankArenaNum = g_pCfgMgr->GetAreanRankNum();
// 	ArenaAllRankMap::iterator map_iter = m_RankList.begin();
// 	for (; map_iter != m_RankList.end(); ++map_iter)
// 	{
// 		ArenaRankMap::iterator vector_iter = map_iter->second.begin();
// 		for (; vector_iter != map_iter->second.end(); ++vector_iter)
// 		{
// 			// 处理奖励,前x排名才有奖励
// 			//if (vector_iter->wCurRank <= wRankArenaNum)
// 			//{
// 			RewardData tRewardData;
// 			tRewardData.wInfoId = 1;
// 			tRewardData.dwTime = time(NULL);
// 			tRewardData.dwValue1 = vector_iter->wCurRank;
// 			CRewardCenterManager::AddReward(map_iter->first, vector_iter->dwTargetId, tRewardData);
// 
// 			//MAPLOG(ERROR_LEVEL, "[%s]竞技场发放奖励[role_id:%d,Rank:%d]", __FUNCTION__, vector_iter->dwTargetId, vector_iter->wCurRank);
// 			//}
// 		}
// 
// 		MAPLOG(ERROR_LEVEL, "[%s]竞技场发放奖励[server_id:%d]", __FUNCTION__, map_iter->first);
// 	}

	m_RankListForSendReward = m_RankList;

	MAPLOG(ERROR_LEVEL, "[%s]竞技场排行榜已发放奖励[server_num:%d]", __FUNCTION__, m_RankListForSendReward.size());
}

// 处理7天排行奖励
void CArenaManager::DoSevenDayRankAward(DWORD dwServerId)
{
	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场排行榜列表[ServerId:%d]", __FUNCTION__, dwServerId);
		return;
	}

	MAPLOG(ERROR_LEVEL, "[%s]开始发放7天排行榜奖励(竞技榜奖励)[serverid:%d]", __FUNCTION__, dwServerId);
	const RoleRankReward_Config *pRewardCfg;
	ArenaRankMap::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		// 如果是机器人则不处理
		if (IsRobotId(iter->dwTargetId))
			continue;

		pRewardCfg = g_pCfgMgr->GetSevenDayRankReward(RANK_ARENA, iter->wCurRank);
		if (pRewardCfg)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 18;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = iter->wCurRank;
			tRewardData.dwValue2 = RANK_ARENA; 
			CRewardCenterManager::AddReward(dwServerId, iter->dwTargetId, tRewardData);

			MAPLOG(ERROR_LEVEL, "发放7天战力榜奖励(server_id:%d,role_id:%d,rank:%d)",
				dwServerId, iter->dwTargetId, iter->wCurRank);
		}
	}
}

// 添加玩家战斗记录
void CArenaManager::AddPlayerBattleRecord(const PlayerArenaBattleRecord *pBattleRecord)
{
	ArenaBattleRecordMap::iterator map_iter = m_vBattleRecordList.find(pBattleRecord->dwRoleId);
	if (map_iter != m_vBattleRecordList.end())
	{
		MAPLOG(ERROR_LEVEL, "[%s]重复添加玩家竞技场战斗记录[roleid:%d]", __FUNCTION__,pBattleRecord->dwRoleId);
		return;
	}

	list<ArenaBattleRecord> lBattleRecordList;
	for (WORD i = 0; i < pBattleRecord->wBattleRecordNum; ++i)
	{
		lBattleRecordList.push_back(pBattleRecord->arBattleRecordList[i]);
	}
	m_vBattleRecordList.insert(make_pair(pBattleRecord->dwRoleId, lBattleRecordList));
}

// 是否有红点
bool CArenaManager::IsHaveRedPoint(Player *pPlayer)
{
	WORD wRank = GetPlayerRank(pPlayer->GetServerId(), pPlayer->GetRoleId());
// 	if (!wRank)
// 		return false;

	const vector<DWORD> &vGoodsList = g_pCfgMgr->GetArenaShopGoodsList();
	for (size_t i = 0; i < vGoodsList.size(); ++i)
	{
		const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_ARENA, vGoodsList[i]);
		if (!pGoodsCfg)
			continue;

		// 到达指定排名(0表示木有排名要求)
		if (!pGoodsCfg->nContext || (wRank && wRank <= pGoodsCfg->nContext))
		{
			// 还可以购买(包括购买消耗的条件)
			if (!pPlayer->GetShopManager()->GetGoodsBuyRecordNum(SHOP_ARENA, vGoodsList[i]) && 
				pPlayer->CheckMoneyCost(pGoodsCfg->vCostList))
				return true;
		}
	}

	return false;
}

// 重置排行榜数据
void CArenaManager::ResetRankData(WORD wServerId)
{
	ArenaAllRankMap::iterator rank_list_iter = m_RankList.find(wServerId);
	if (rank_list_iter != m_RankList.end())
	{
		// 清战报数据
		ArenaRankMap::iterator rank_data_iter = rank_list_iter->second.begin();
		for (; rank_data_iter != rank_list_iter->second.end(); ++rank_data_iter)
		{
			m_vBattleRecordList.erase(rank_data_iter->dwTargetId);
		}

		// 清排行榜数据
		rank_list_iter->second.clear();
	}
}

// 取排行榜列表
ArenaRankMap* CArenaManager::GetServerRankList(DWORD dwServerId)
{
	ArenaAllRankMap::iterator iter = m_RankList.find(dwServerId);
	if (iter == m_RankList.end() || !iter->second.size())
	{
		ArenaRankMap vRankList;
		const vector<ArenaRobot_Config> *pRobotListCfg = CConfigManager::Instance()->GetArenaRobotList();
		if (!pRobotListCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场机器人配置列表", __FUNCTION__);
			return NULL;
		}

		ArenaRankData tRankData;
		vector<ArenaRobot_Config>::const_iterator robot_iter = pRobotListCfg->begin();
		for (; robot_iter != pRobotListCfg->end(); ++robot_iter)
		{
			tRankData.dwTargetId = robot_iter->dwRobotId;
			tRankData.wCurRank = robot_iter->wRank;
			tRankData.wHistoryBestRank = 0;
			vRankList.push_back(tRankData);
		}

		m_RankList.insert(make_pair(dwServerId, vRankList));
		iter = m_RankList.find(dwServerId);
	}
	return &iter->second;
}

// 取玩家排行数据
ArenaRankData* CArenaManager::GetPlayerRankData(DWORD dwServerId, DWORD dwRoleId)
{
	ArenaRankMap* pRankList = GetServerRankList(dwServerId);
	if (pRankList)
	{
		ArenaRankMap::iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			if (iter->dwTargetId == dwRoleId)
			{
				return &*iter;
			}
		}
	}
	return NULL;
}

// 添加战报
void CArenaManager::AddBattleRecord(DWORD dwRoleId, const ArenaBattleRecord &tRecord)
{
	ArenaBattleRecordMap::iterator iter = m_vBattleRecordList.find(dwRoleId);
	if (iter == m_vBattleRecordList.end())
	{
		list<ArenaBattleRecord> vRecordList;
		vRecordList.push_back(tRecord);
		m_vBattleRecordList.insert(make_pair(dwRoleId, vRecordList));
	}
	else
	{
		iter->second.push_back(tRecord);
		if (iter->second.size() > MAX_ARENA_BATTLE_RECORD)
		{
			iter->second.pop_front();
		}
	}

	// 存储到数据库 todo::可能需要调整为跟竞技场数据同时触发存储
	SaveBattleRecord(dwRoleId);
}

// 添加战报
void CArenaManager::AddBattleRecord(DWORD dwSrcRoleId, DWORD dwTargetId, WORD wSrcRank, WORD wTargetRank, 
	const char *pszSrcName, const char *pszTargetName, WORD wScrHeadIcon, WORD wTargetHeadIcon, WORD wScrLevel, WORD wTargetLevel, BYTE byIsWin)
{
	ArenaBattleRecord tSrcBattleRecord;
	ArenaBattleRecord tTargetBattleRecord;

	// Id
	tSrcBattleRecord.dwTargetId = dwTargetId;
	tTargetBattleRecord.dwTargetId = dwSrcRoleId;

	// 名称
	strcpy_s(tSrcBattleRecord.szTargetName, sizeof(tSrcBattleRecord.szTargetName) - 1, pszTargetName);
	strcpy_s(tTargetBattleRecord.szTargetName, sizeof(tTargetBattleRecord.szTargetName) - 1, pszSrcName);

	// 战前排名
	tSrcBattleRecord.wMyOldRank = wSrcRank;
	tTargetBattleRecord.wMyOldRank = wTargetRank;

	// 战后排名
	if (byIsWin && wSrcRank > wTargetRank)
	{
		tSrcBattleRecord.wMyNewRank = wTargetRank;
		tTargetBattleRecord.wMyNewRank = wSrcRank;
	}
	else
	{
		tSrcBattleRecord.wMyNewRank = wSrcRank;
		tTargetBattleRecord.wMyNewRank = wTargetRank;
	}

	// 是否获胜
	tSrcBattleRecord.byIsWin = byIsWin ? true : false;
	tTargetBattleRecord.byIsWin = byIsWin ? false : true;

	// 是否攻击方
	tSrcBattleRecord.byIsAttack = true;
	tTargetBattleRecord.byIsAttack = false;

	// 挑战时间
	tSrcBattleRecord.dwTime = time(NULL);
	tTargetBattleRecord.dwTime = time(NULL);

	// 头像
	tSrcBattleRecord.wTargetHeadIcon = wTargetHeadIcon;
	tTargetBattleRecord.wTargetHeadIcon = wScrHeadIcon;

	// 等级
	tSrcBattleRecord.wTargetLevel = wTargetLevel;
	tTargetBattleRecord.wTargetLevel = wScrLevel;

	// 攻方记录
	AddBattleRecord(dwSrcRoleId, tSrcBattleRecord);

	// 目标方记录(机器人的话不需要记录)
	if (!IsRobotId(dwTargetId))
		AddBattleRecord(dwTargetId, tTargetBattleRecord);
}

// 更新排行榜
void CArenaManager::UpdateRank(DWORD dwServerId)
{
	
}