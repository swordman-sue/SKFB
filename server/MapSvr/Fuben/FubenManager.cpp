#include "stdafx.h"
#include "FubenManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Rank\RankManager.h"
#include "..\Rebel\RebelManager.h"
#include "..\Land\LandManager.h"
#include "..\Duobao\DuobaoManager.h"
#include "..\Arena\ArenaManager.h"
#include "..\ZhanBu\ZhanBuManager.h"
#include "..\ScoreMatch\ScoreMatchManager.h"
#include "..\WorldBoss\WorldBossManager.h"
#include "..\PlayerManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\MapServer.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Friend\FriendManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Guild\GuildManager.h"
#include "..\AttackCity\AttackCityManager.h"
#include "..\EndlessLand\EndlessLandManager.h"
#include "..\MapSvrQuery.h"
#include "ToolKit\MD5.h"
#include "..\KingMatch\KingMatchManager.h"
#include "..\TeamDungeon\TeamDungeonManager.h"
#include "..\GameLogic.h"
#include "..\Activity\FacebookActivity.h"
#include "..\Activity\ActivityManager.h"
#include "..\Spirit\SpiritManager.h"
#include "..\ResourceWar\ResourceWarManager.h"
#include "..\Guild\GuildWarMgr.h"

CFBBase::CFBBase()
{

}

CFBBase::~CFBBase()
{

}

void CFBBase::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	m_pPlayer = pPlayer;
	m_pFBManager = pFBMgr;
}

CMainFB::CMainFB()
{

}

CMainFB::~CMainFB()
{

}

void CMainFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	CFBBase::Init(pPlayer, pFBMgr);
	m_pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tMainFubenData;
}

// 开始战斗
bool CMainFB::OnBattleStart(WORD wChapter, BYTE byMapIdx, DWORD dwHelpBattleTargetId)
{
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 检测需要的角色等级是否足够
	if (m_pPlayer->GetLevel() < pChapterCfg->dwNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]主线关卡挑战需要的角色等级不足[Chapter:%d, CurLevel:%d, NeedLevel:%d]!", 
			__FUNCTION__, wChapter, m_pPlayer->GetLevel(), pChapterCfg->dwNeedRoleLevel);
		return false;
	}

	WORD wMapId = pChapterCfg->GetMapId(byMapIdx);

	// 取配置
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本关卡配置[chapter:%d,MapId:%d]",__FUNCTION__, wChapter, wMapId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]主线关卡挑战消耗不足!", __FUNCTION__);
		return false;
	}

	// 检测体力
// 	if (m_pPlayer->GetProperty(ROLE_PRO_STAMINA) < pMapCfg->nCostStamina)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]主线副本关卡挑战体力不足!", __FUNCTION__);
// 		return false;
// 	}

	// 取章节数据
	ChapterData *pChapterData = GetChapterData(wChapter);
	if (!pChapterData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的主线副本章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 越章节挑战
	if (wChapter > GetNextChapter())
	{
		MAPLOG(ERROR_LEVEL, "[%s]主线副本越章节挑战!", __FUNCTION__);
		return false;
	}

	// 越关卡挑战
	if (GetNextChapter() == wChapter && byMapIdx > GetNextMapIndex())
	{
		MAPLOG(ERROR_LEVEL, "[%s]主线副本越关卡挑战!", __FUNCTION__);
		return false;
	}

	// 检测当前挑战次数
	if (pChapterData->arMapList[byMapIdx - 1].byBattleTimes >= pMapCfg->byDailyBattleTimesLimit)
	{
		MAPLOG(ERROR_LEVEL, "[%s]主线副本关卡今日已挑战过!", __FUNCTION__);
		return false;
	}

	// 助战英雄ID
	DWORD dwHelpBattleHeroInfoId = 0;

	// 如果有选择助战 
	if (dwHelpBattleTargetId)
	{
		// 检测助战次数(只有好友才需要检测)
// 		FriendData *pFriend = m_pPlayer->GetFriendManager()->GetFriendData(dwHelpBattleTargetId);
// 		if (pFriend)
// 		{
// 			if (pFriend->wHelpBattleTimes >= g_GetConfigValue(122))
// 			{
// 				MAPLOG(ERROR_LEVEL, "[%s]好友的助战次数已达上限[FriendId:%d]", __FUNCTION__, dwHelpBattleTargetId);
// 				return false;
// 			}
// 			pFriend->wHelpBattleTimes++;
// 		}

		bool bIsNeedCheck = false;

		// 如果是好友
		if (m_pPlayer->GetFriendManager()->IsFriend(dwHelpBattleTargetId))
		{
			bIsNeedCheck = true;
		}

		// 如果是军团队友
		if (!bIsNeedCheck)
		{
			DWORD dwMyGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(m_pPlayer->GetRoleId());
			if (dwMyGuildId && dwMyGuildId == CGuildManager::Instance()->GetGuildIdByRoleId(dwHelpBattleTargetId))
				bIsNeedCheck = true;
		}

		// 如果是好友，或者军团里的队友,则需要检测助战次数
		if (bIsNeedCheck)
		{
			// 检测助战总次数
			if (m_pPlayer->GetProperty(ROLE_PRO_TODAY_HELP_BATTLE_TIMES) >= g_GetConfigValue(272))
			{
				MAPLOG(ERROR_LEVEL, "[%s]今日助战次数已使用完", __FUNCTION__);
				return false;
			}
			m_pPlayer->ModifyProperty(ROLE_PRO_TODAY_HELP_BATTLE_TIMES, 1);
		}


		// 取目标英雄属性
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwHelpBattleTargetId);
		if (pTargetBaseData && pTargetBaseData->vToBattleHeroList.size())
		{
			// 取第一个位置的英雄
			const ToBattleHeroData &tHeroData = pTargetBaseData->vToBattleHeroList[0];
			dwHelpBattleHeroInfoId = tHeroData.wHeroInfoId;
			for (BYTE i = 0; i < tHeroData.byProNum; ++i)
			{
				m_pFBManager->AddHelpBattleHeroPro(tHeroData.arPorList[i]);
			}
		}
	}

	// 记录战斗信息
	m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId, dwHelpBattleTargetId, dwHelpBattleHeroInfoId);

	// 热云数据统计(如果是挑战新的关卡) 
	if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
	{
		CGameLogic::Instance()->OnStageStartStatistics(m_pPlayer, wChapter, byMapIdx);
	}

	return true;
}

bool CMainFB::OnBattleOver(BYTE byIsWin, BYTE byStar)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	BYTE byMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节数据[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		// 找到主线副本关卡配置
		MAPLOG(ERROR_LEVEL, "[%s]找不到关卡数据[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	// todo::防作弊检测

	ChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的章节数据[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 热云数据统计(如果是挑战新的关卡) 
	if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
	{
		CGameLogic::Instance()->OnStageOverStatistics(m_pPlayer, wChapter, byMapIdx, byIsWin);
	}

	// 处理结算
	if (byIsWin)
	{
		// 处理奖励
		m_pFBManager->DoWinReward(BATTLE_MAIN_MAP, pMapCfg);

		// 扣除体力
		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		// 如果是挑战新的关卡
		if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
		{
			m_pFBData->wChapter = wChapter;
			m_pFBData->arChapterList[wChapter - 1].byMapIdx = byMapIdx;

			// 本章节挑战结束
			if (byMapIdx >= pChapterCfg->vMapList.size())
			{
				// 成就事件
				m_pPlayer->OnUpdateAchievement(4, m_pFBData->wChapter, false);

				// 7天活动事件(主线副本通关进度)
				m_pPlayer->OnUpdateSevenDayTarget(4, m_pFBData->wChapter, false);

				// 处理广播
				if (wChapter > g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).wChapterPassRecord)
				{
					g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).wChapterPassRecord = wChapter;
					g_MapServer.SaveGolbalData(m_pPlayer->GetServerId(),GOLBAL_DATA_CHAPTER_PASS_RECORD, wChapter);

					CSystemNotice sys_notice(8, m_pPlayer->GetServerId());
					sys_notice.AddParam(m_pPlayer->GetRoleName());
					sys_notice.AddParam(wChapter);
					sys_notice.SendMsg();
				}

				// 通关好评
				if (wChapter == 3)
					m_pPlayer->GetActivityManager()->GetFacebookActivity().SetReadyToCommendTime(time(NULL));

				// 通关分享
				if (wChapter == 2)
					m_pPlayer->GetActivityManager()->GetFacebookActivity().SetReadyToShareTime(time(NULL));
			}

			// 此关卡首次获胜
			m_pFBManager->SetIsFirstWin(true);

			// 存储首次通关记录
			RoleFirstPassMapRecord_Save* pQuery = RoleFirstPassMapRecord_Save::ALLOC();
			pQuery->SetIndex(DB_ROLE_FIRST_PASS_MAP_RECORD_SAVE);
			pQuery->SetThreadIndex(DB_THREAD_COMMON);
			FirstPassMapRecord tRecord;
			tRecord.dwRoleId = m_pPlayer->GetRoleId();
			tRecord.dwMapId = wMapId;
			tRecord.wType = BATTLE_MAIN_MAP;
			tRecord.wChapter = wChapter;
			tRecord.wMapIdx = byMapIdx;
			tRecord.dwFightingPower = m_pPlayer->GetFightingPower();
			tRecord.wLevel = m_pPlayer->GetLevel();
			pQuery->SetRecord(tRecord);
			g_MapServer.MapDBQuery(pQuery);
		}

		MapData *pMapData = &pChapter->arMapList[byMapIdx - 1];
		pMapData->byBattleTimes++;
		if (byStar > pMapData->byStar)
		{
			pMapData->byStar = byStar;

			WORD wTotalStars = GetTotalStar();

			// 成就事件
			m_pPlayer->OnUpdateAchievement(3, wTotalStars, false);

			// 排行榜
			CRankManager::Instance()->AddRoleRankData(RANK_MAIN_FB_STAR, m_pPlayer, wTotalStars);
		}
		
		// 创建随机叛军
		if (!CRebelManager::Instance()->GetSelfRebelId(m_pPlayer->GetRoleId()))
		{
			if (CRebelManager::Instance()->CreateRebel(m_pPlayer))
				m_pFBManager->SetResult(1);
		}

		// 日常任务
		m_pPlayer->OnUpdateDailyMission(1, 1, true);

		// 8-14天活动事件(主线副本通关进度)
		m_pPlayer->OnUpdateSevenDayTarget(20, 1, true);

		// 节日活动
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 6, 1, true);

		// 节日活跃目标
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 6, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 6, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 6, 1, true);

		// 世界等级任务
		//m_pPlayer->OnUpdateWorldTaskEvent(1);
	}
 	else
 	{
 		// 存储失败记录
 		RoleMapLoseRecord_Save* pQuery = RoleMapLoseRecord_Save::ALLOC();
 		pQuery->SetIndex(DB_ROLE_MAP_LOSE_RECORD_SAVE);
 		pQuery->SetThreadIndex(DB_THREAD_COMMON);
 		MapLoseRecord tRecord;
 		tRecord.dwRoleId = m_pPlayer->GetRoleId();
 		tRecord.dwMapId = wMapId;
 		tRecord.wType = BATTLE_MAIN_MAP;
 		tRecord.wChapter = wChapter;
 		tRecord.wMapIdx = byMapIdx;
		tRecord.dwFightingPower = m_pPlayer->GetFightingPower();
 		pQuery->SetRecord(tRecord);
 		g_MapServer.MapDBQuery(pQuery);
 	}

	// 如果是好友助战
	DWORD dwHelpBattleTargetId = m_pFBManager->GetHelpBattleTargetId();
	if (dwHelpBattleTargetId && m_pPlayer->GetFriendManager()->IsFriend(dwHelpBattleTargetId))
	{
		int nHelpBattleFriendshipPoint = g_GetConfigValue(125);
		// 请助者获得友情点
		if (m_pPlayer->GetProperty(ROLE_PRO_TODAY_FRIENDSHIP_PIONT) < g_GetConfigValue(123))
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_TODAY_FRIENDSHIP_PIONT, nHelpBattleFriendshipPoint);
			m_pPlayer->ModifyProperty(ROLE_PRO_FRIENDSHIP_PIONT, nHelpBattleFriendshipPoint);
		}

		// 助战目标获得友情点
		PlayerBaseData *pTargetBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseData(m_pPlayer->GetServerId(), dwHelpBattleTargetId, false);
		//const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwHelpBattleTargetId, false);
		if (pTargetBaseData)
		{
			// 如果不是同一天则重置数据
			if (!IsTheSameDay(pTargetBaseData->dwLastTakeFriendShipPointTime, time(NULL)))
			{
				pTargetBaseData->nTodayHelpBattleFriendShipPoint = 0;
			}

			// 今日获得友情点还没到达上限
			if (pTargetBaseData->nTodayHelpBattleFriendShipPoint < g_GetConfigValue(131))
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 11;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = nHelpBattleFriendshipPoint;
				strcpy_s(tRewardData.szText, sizeof(tRewardData.szText) - 1, m_pPlayer->GetRoleName());
				CRewardCenterManager::AddReward(m_pPlayer->GetServerId(), dwHelpBattleTargetId, tRewardData);

				// 更新每日获得的友情点及时间
				pTargetBaseData->nTodayHelpBattleFriendShipPoint = pTargetBaseData->nTodayHelpBattleFriendShipPoint + nHelpBattleFriendshipPoint;
				pTargetBaseData->dwLastTakeFriendShipPointTime = time(NULL);
			}
		}
	}

	// todo::暂时这么处理
	SendFBData();

	return true;
}

// 重置副本挑战次数 
bool CMainFB::OnResetBattleTimes(WORD wChapter, BYTE byMapIdx)
{
	MapData *pMapData = GetMapData(wChapter, byMapIdx);
	if (!pMapData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到要重置的关卡数据[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	// 检测重置次数
	if (pMapData->byResetTimes >= m_pPlayer->GetVipValue(VIP_MAIN_FB_RESET_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]要重置的关卡次数已达上限[Chapter:%d,MapIdx:%d]", 
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(61, true);
	int nCostDiamond = pSvrCfg ? pSvrCfg->GetListValue(pMapData->byResetTimes) : 0;

	// 检测重置副本消耗
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]重置主线副本关卡消耗钻石不足[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_MAIN_FB_RESET);

	pMapData->byBattleTimes = 0;
	pMapData->byResetTimes++;

	return true;
}

// 领取副本奖励 
bool CMainFB::OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx)
{
	if (!wChapter || !byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]参数不合法[chapter:%d,index:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	BYTE bySvrMapIdx = byMapIdx - 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	ChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的章节非法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 关卡奖励
	if (byRewardType == FB_AWARD_MAP)
	{
		if (bySvrMapIdx >= pChapter->byMapIdx)
		{
			MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的关卡非法[chapter:%d,map:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterCfg->vMapList[bySvrMapIdx]);
		if (!pMapCfg || !pMapCfg->vBoxList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到关卡配置数据或者木有配置宝箱奖励[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		MapData &tMapData = pChapter->arMapList[bySvrMapIdx];

		// 检测是否已经领取过
		if (tMapData.byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]已经领取过宝箱奖励[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		// 添加奖励
		m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_LEVEL_REWARD);

		tMapData.byIsGetAward = true;
	}
	// 星数奖励
	else
	{
		if (bySvrMapIdx >= CHAPTER_STAR_AWARD_MAX || bySvrMapIdx >= pChapterCfg->vStarAwardList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的索引出错[chapter:%d,index:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[bySvrMapIdx];

		// 检测章节的数量是否足够
		if (pChapter->GetStar() < pStarAwardCfg->byStar)
		{
			MAPLOG(ERROR_LEVEL, "[%s]章节星数量不够,不能领取[chapter:%d,star:%d]",
				__FUNCTION__, wChapter, pChapter->GetStar());
			return false;
		}

		// 检测是否已经领取过
		if (pChapter->arStarAwardList[bySvrMapIdx])
		{
			MAPLOG(ERROR_LEVEL, "[%s]章节星数奖励已领取过[chapter:%d,index:%d]", __FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		// 发奖励
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_STAR_REWARD);

		pChapter->arStarAwardList[bySvrMapIdx] = true;
	}
	return true;
}

// 请求副本数据
void CMainFB::OnNetFBData()
{
	SendFBData();
}

// 发送副本数据
void CMainFB::SendFBData()
{
	MSG_S2C_FB_MAIN_DATA_RESP msg;
	msg.m_Data = *m_pFBData;
	m_pPlayer->SendPacket(&msg);
}

// 是否有红点
bool CMainFB::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		ChapterData *pChapter = &m_pFBData->arChapterList[i];

		WORD wTotalStar = pChapter->GetStar();

		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetMainFBChapter(i+1);
		if (!pChapterCfg || pChapterCfg->vStarAwardList.size() != CHAPTER_STAR_AWARD_MAX)
			continue;

		// 当主线副本内有可领取的星级宝箱未领取时
		for (BYTE k = 0; k < CHAPTER_STAR_AWARD_MAX; ++k)
		{
			// 星数到达可领取,并且还木有领取
			if (!pChapter->arStarAwardList[k] && wTotalStar >= pChapterCfg->vStarAwardList[k].byStar)
				return true;
		}

		// 主线副本内有可领取 的精英宝箱未领取时
		for (BYTE k = 0; k < pChapter->byMapIdx; ++k)
		{
			MapData *pMap = &pChapter->arMapList[k];
			const Map_Config *pMapCfg = g_pCfgMgr->GetMap(pChapterCfg->GetMapId(k + 1));
			if (pMapCfg && pMapCfg->vBoxList.size() && !pMap->byIsGetAward)
				return true;
		}
	}

	return false;
}

// 扫荡
void CMainFB::OnNetClean(WORD wChapter, BYTE byMapIdx)
{
	// 开始战斗
	if (!OnBattleStart(wChapter, byMapIdx, 0))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理开始战斗失败[chapter:%d,MapIdx:%d]", __FUNCTION__, wChapter, byMapIdx);
		return;
	}

	// 战斗结束
	if (!OnBattleOver(true, 3))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理结束战斗失败[chapter:%d,MapIdx:%d]", __FUNCTION__, wChapter, byMapIdx);
		return;
	}

	// 8-14天活动事件(主线副本通关进度)
	m_pPlayer->OnUpdateSevenDayTarget(20, 1, true);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 6, 1, true);

	// 节日活跃目标
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 6, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 6, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 6, 1, true);

	// todo::响应
}


// 一键领取奖励(关卡+星数)
bool CMainFB::OnNetOneKeyTakeReward(WORD wChapter)
{
	if (!wChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]章节参数不合法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	ChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的章节非法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 检测是否已通关
	if (pChapterCfg->vMapList.size() != pChapter->byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]此章节还木有通关,不能领取[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 处理宝箱奖励
	for (BYTE i = 0; i < pChapter->byMapIdx; ++i)
	{
		MapData &tMap = pChapter->arMapList[i];
		// 已经领取过
		if (tMap.byIsGetAward)
			continue;

		// 取关卡配置
		DWORD dwMapId = pChapterCfg->GetMapId(i + 1);
		const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
		if (!pMapCfg)
			continue;

		// 有宝箱奖励
		if (pMapCfg->vBoxList.size())
		{
			m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_ONE_KEY_REWARD);
			tMap.byIsGetAward = true;
		}
	}

	// 处理星数奖励
	WORD wTotalStar = pChapter->GetStar();
	for (BYTE i = 0; i < CHAPTER_STAR_AWARD_MAX; ++i)
	{
		// 已经领取过
		if (pChapter->arStarAwardList[i])
			continue;

		// 安全检测,防止越界(todo::不应该出现这种问题)
		if (i >= pChapterCfg->vStarAwardList.size())
			continue;

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[i];

		// 检测章节的数量是否足够
		if (wTotalStar < pStarAwardCfg->byStar)
			continue;

		// 发奖励
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_ONE_KEY_REWARD);

		pChapter->arStarAwardList[i] = true;
	}

	return true;
}

// 领取章节奖励
void CMainFB::OnNetTakeChapterReward(WORD wRewardLevel)
{
	// 取章节奖励配置
	const MainFBChapterReward_Config *pRewardCfg = g_pCfgMgr->GetMainFBChapterReward(wRewardLevel);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节奖励[RewardLevel:%d]", __FUNCTION__, wRewardLevel);
		return;
	}

	// 还木有到达当前章节
	if (m_pFBData->wChapter < pRewardCfg->wNeedChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]还木有通关指定的章节,不能领取奖励[chapter:%d]",
			__FUNCTION__, pRewardCfg->wNeedChapter);
		return;
	}
	// 已到达当前章节(检测当前章节关卡)
	if (m_pFBData->wChapter == pRewardCfg->wNeedChapter)
	{
		// 取章节配置
		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetMainFBChapter(pRewardCfg->wNeedChapter);
		if (!pChapterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节数据[chapter:%d]",
				__FUNCTION__, pRewardCfg->wNeedChapter);
			return;
		}
		// 取玩家章节数据
		ChapterData *pChapter = GetChapterData(pRewardCfg->wNeedChapter);
		if (!pChapter)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节配置数据[chapter:%d]",
				__FUNCTION__, pRewardCfg->wNeedChapter);
			return;
		}
		// 当前章节的关卡还木有完全通过
		if (pChapter->byMapIdx < pChapterCfg->vMapList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]还木有通关指定的章节,不能领取奖励[chapter:%d]",
				__FUNCTION__, pRewardCfg->wNeedChapter);
			return;
		}
	}

	// 检测是否已领取过
	if (wRewardLevel <= m_pFBData->wTakeChapterRewardRecord)
	{
		MAPLOG(ERROR_LEVEL, "[%s]主线副本章节奖励已领取过[chapter:%d]",
			__FUNCTION__, pRewardCfg->wNeedChapter);
		return;
	}

	vector<Item> vItemList;

	// 发奖励
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(pRewardCfg->dwItemId);
	if (pItemCfg && pItemCfg->byIsUseInGet)
	{
		m_pPlayer->GetItemManager()->UseItem(pRewardCfg->dwItemId, 1, &vItemList);
	}
	else
	{
		m_pPlayer->AddObject(pRewardCfg->dwItemId, 1, TRUE, FROM_MAINLINE_FB_CHAPTER_REWARD);
		vItemList.push_back(Item(pRewardCfg->dwItemId, 1));
	}

	m_pFBData->wTakeChapterRewardRecord = wRewardLevel;

	// 响应
	MSG_S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP msg;
	msg.m_wRewardLevel = wRewardLevel;
	msg.m_vItemList = vItemList;
	m_pPlayer->SendPacket(&msg);
}

// 取关卡数据
MapData* CMainFB::GetMapData(WORD wChapter, BYTE byMapIdx)
{
	ChapterData *pChapterData = GetChapterData(wChapter);
	if (pChapterData && byMapIdx <= pChapterData->byMapIdx)
	{
		return &pChapterData->arMapList[byMapIdx];
	}

	return NULL;
}

// 取章节数据
ChapterData* CMainFB::GetChapterData(WORD wChapter)
{
	if (!wChapter || wChapter > MAX_FUBEN_CHAPER_NUM)
		return NULL;

	return &m_pFBData->arChapterList[wChapter-1];
}

// 取下一个挑战的章节
WORD CMainFB::GetNextChapter()
{
	WORD wChapter = m_pFBData->wChapter;

	// 默认从第一章开始打
	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	// 本章节已本部打完
	if (m_pFBData->arChapterList[wChapter - 1].byMapIdx >= pChapterCfg->vMapList.size())
		return wChapter + 1;

	return wChapter;
}

// 取下一个挑战的关卡索引
BYTE CMainFB::GetNextMapIndex()
{
	WORD wChapter = m_pFBData->wChapter;

	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到主线副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	BYTE byMapIdx = m_pFBData->arChapterList[wChapter - 1].byMapIdx;

	// 本章节已本部打完
	if (byMapIdx >= pChapterCfg->vMapList.size())
		return 1;

	return byMapIdx + 1;
}

// 取总星数
WORD CMainFB::GetTotalStar()
{
	WORD wTotalStar = 0;
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		ChapterData &tChapter = m_pFBData->arChapterList[i];
		for (BYTE k = 0; k < tChapter.byMapIdx; ++k)
		{
			wTotalStar += tChapter.arMapList[k].byStar;
		}
	}
	return wTotalStar;
}


// 精英副本

CEliteFB::CEliteFB()
{

}

CEliteFB::~CEliteFB()
{
	
}

void CEliteFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	CFBBase::Init(pPlayer, pFBMgr);
	m_pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tEliteFBData;
}

// 开始战斗
bool CEliteFB::OnBattleStart(WORD wChapter, BYTE byMapIdx)
{
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 挑战普通关卡
	if (byMapIdx)
	{
		WORD wMapId = pChapterCfg->GetMapId(byMapIdx);

		// 取配置
		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
		if (!pMapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本关卡配置[chapter:%d,MapId:%d]", __FUNCTION__, wChapter, wMapId);
			return false;
		}

		// 检测消耗
		if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精英关卡挑战消耗不足!", __FUNCTION__);
			return false;
		}

		// 取章节数据
		EliteFBChapter *pChapterData = GetChapterData(wChapter);
		if (!pChapterData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的精英副本章节数据[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		// 越章节挑战
		if (wChapter > GetNextChapter())
		{
			MAPLOG(ERROR_LEVEL, "[%s]精英副本越章节挑战!", __FUNCTION__);
			return false;
		}

		// 越关卡挑战
		if (GetNextChapter() == wChapter && byMapIdx > GetNextMapIndex())
		{
			MAPLOG(ERROR_LEVEL, "[%s]精英副本越关卡挑战!", __FUNCTION__);
			return false;
		}

		// 检测当前挑战次数
		if (pChapterData->arMapList[byMapIdx - 1].byBattleTimes >= pMapCfg->byDailyBattleTimesLimit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]精英副本关卡今日已挑战过!", __FUNCTION__);
			return false;
		}

		// 记录战斗信息
		m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId);
	}
	// 挑战外敌关卡
	else
	{
		// 取章节数据
		EliteFBChapter *pChapterData = GetChapterData(wChapter);
		if (!pChapterData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的精英副本章节数据[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		if (!pChapterData->tEnemyData.wMapId)
		{
			m_pPlayer->SendErrorMsg(ERROR_ELITEFB_ENEMY_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]当前精英副本章节木有外敌可挑战[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		// 检测是否已死亡
		if (pChapterData->tEnemyData.wIsDead)
		{
			MAPLOG(ERROR_LEVEL, "[%s]当前精英副本章节外敌已挂掉[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		// 取配置
		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterData->tEnemyData.wMapId);
		if (!pMapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本外敌关卡配置[chapter:%d,MapId:%d]", 
				__FUNCTION__, wChapter, pChapterData->tEnemyData.wMapId);
			return false;
		}

		// 检测消耗
		if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精英外敌关卡挑战消耗不足!", __FUNCTION__);
			return false;
		}

// 		for (BYTE i = 0; i < pChapterData->tEnemyData.byMonsterNum; ++i)
// 		{
// 			m_pFBManager->AddMonsterData(pChapterData->tEnemyData.arMonsterList[i]);
// 		}

		// 记录战斗信息
		m_pFBManager->SetBattleInfo(wChapter, byMapIdx, pChapterData->tEnemyData.wMapId);
	}

	return true;
}

// 战斗结束
bool CEliteFB::OnBattleOver(BYTE byIsWin, BYTE byStar, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	BYTE byMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本章节数据[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		// 找到主线副本关卡配置
		MAPLOG(ERROR_LEVEL, "[%s]找不到精英关卡数据[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	// todo::防作弊检测

	EliteFBChapter *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的章节数据[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 处理结算
	if (byIsWin)
	{
		// 普通关卡
		if (byMapIdx)
		{
			// 处理奖励
			m_pFBManager->DoWinReward(BATTLE_ELITE_MAP, pMapCfg);

			// 扣除体力
			m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

			// 如果是挑战新的关卡
			if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
			{
				m_pFBData->wChapter = wChapter;
				m_pFBData->arChapterList[wChapter - 1].byMapIdx = byMapIdx;

				// 成就事件(通关精英副本)
				m_pPlayer->OnUpdateAchievement(6, m_pFBData->wChapter, false);

				// 此关卡首次获胜
				m_pFBManager->SetIsFirstWin(true);
			}

			MapData *pMapData = &pChapter->arMapList[byMapIdx - 1];
			pMapData->byBattleTimes++;
			if (byStar > pMapData->byStar)
			{
				pMapData->byStar = byStar;

				// 成就事件(精英副本总星级)
				m_pPlayer->OnUpdateAchievement(5, GetTotalStar(), false);
			}

			// 创建随机叛军
			if (!CRebelManager::Instance()->GetSelfRebelId(m_pPlayer->GetRoleId()))
			{
				if (CRebelManager::Instance()->CreateRebel(m_pPlayer))
					m_pFBManager->SetResult(1);
			}

			// 日常任务
			m_pPlayer->OnUpdateDailyMission(15);

			// 节日活动(每日通关精英副本次数)
			m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 13, 1, true);

			// 节日活跃目标(每日通关精英副本次数)
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 13, 1, true);

			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 13, 1, true);

			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 13, 1, true);
		}
	}

	// 外敌
	if (!byMapIdx)
	{
		if (byIsWin)
		{
			// 取配置
			const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapter->tEnemyData.wMapId);
			if (!pMapCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本外敌关卡配置[chapter:%d,MapId:%d]",
					__FUNCTION__, wChapter, pChapter->tEnemyData.wMapId);
				return false;
			}

			// 处理奖励
			m_pFBManager->DoWinReward(BATTLE_ELITE_MAP, pMapCfg);

			// 检测消耗
			m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

			pChapter->tEnemyData.wIsDead = true;

			// 成就事件(累计击杀精英副本外敌)
			m_pPlayer->OnUpdateAchievement(22);

			// 日常任务(击杀精英外敌)
			m_pPlayer->OnUpdateDailyMission(20);
		}
	}

	// todo::暂时这么处理
	SendFBData();

	return true;
}

// 重置副本挑战次数
bool CEliteFB::OnResetBattleTimes(WORD wChapter, BYTE byMapIdx)
{
	MapData *pMapData = GetMapData(wChapter, byMapIdx);
	if (!pMapData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到要重置的关卡数据[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	// 检测重置次数
	if (pMapData->byResetTimes >= m_pPlayer->GetVipValue(VIP_ELITE_FB_RESET_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]要重置的关卡次数已达上限[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(97, true);
	int nCostDiamond = pSvrCfg ? pSvrCfg->GetListValue(pMapData->byResetTimes) : 0;

	// 检测重置副本消耗
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]重置精英副本关卡消耗钻石不足[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_ELITE_FB_RESET);

	pMapData->byBattleTimes = 0;
	pMapData->byResetTimes++;

	return true;
}

// 领取副本奖励
bool CEliteFB::OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx)
{
	if (!wChapter || !byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]参数不合法[chapter:%d,index:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	BYTE bySvrMapIdx = byMapIdx - 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	EliteFBChapter *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的章节非法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 关卡奖励
	if (byRewardType == FB_AWARD_MAP)
	{
		if (bySvrMapIdx >= pChapter->byMapIdx)
		{
			MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的关卡非法[chapter:%d,map:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterCfg->vMapList[bySvrMapIdx]);
		if (!pMapCfg || !pMapCfg->vBoxList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到关卡配置数据或者木有配置宝箱奖励[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		MapData &tMapData = pChapter->arMapList[bySvrMapIdx];

		// 检测是否已经领取过
		if (tMapData.byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]已经领取过宝箱奖励[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		// 添加奖励
		m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_ELITE_FB_LEVEL_REWARD);

		tMapData.byIsGetAward = true;
	}
	// 星数奖励
	else
	{
		if (bySvrMapIdx >= CHAPTER_STAR_AWARD_MAX)
		{
			MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的索引出错[chapter:%d,index:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[bySvrMapIdx];

		// 检测章节的数量是否足够
		if (pChapter->GetStar() < pStarAwardCfg->byStar)
		{
			MAPLOG(ERROR_LEVEL, "[%s]章节星数量不够,不能领取[chapter:%d,star:%d]",
				__FUNCTION__, wChapter, pChapter->GetStar());
			return false;
		}

		// 检测是否已经领取过
		if (pChapter->arStarAwardList[bySvrMapIdx])
		{
			MAPLOG(ERROR_LEVEL, "[%s]章节星数奖励已领取过[chapter:%d,index:%d]",__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		// 添加宝箱
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_ELITE_FB_STAR_REWARD);

		pChapter->arStarAwardList[bySvrMapIdx] = true;
	}
	
	return true;
}

// 请求副本数据
void CEliteFB::OnNetFBData()
{
	SendFBData();
}

// 登录
void CEliteFB::Login()
{
 	const Server_Config *pServerCfgList = g_pCfgMgr->GetCommonCfg(124);
 	if (pServerCfgList)
 	{
 		tm tmNow;
 		GetTMTime(&tmNow, time(NULL));
 		tm tmLast;
 		GetTMTime(&tmLast, m_pFBData->dwLastRefreshEnemyTime);
 		int nNowTime = tmNow.tm_hour * 100 + tmNow.tm_min;
 		int nLastTime = tmLast.tm_hour * 100 + tmLast.tm_min;
 
 		for (size_t i = 0; i < pServerCfgList->vValueList.size(); ++i)
 		{
 			int nBeginTime = pServerCfgList->vValueList[i];
 			int nEndTime = pServerCfgList->vValueList[i];
 
 			// 还没到达刷新时间
 			if (nNowTime < nBeginTime)
 				continue;
 
 			// 已刷新过
 			if (m_pFBData->dwLastRefreshEnemyTime && nLastTime > nEndTime)
 				continue;
 
 			CreateEnemy();
 		}
 	}
 	m_pFBData->dwLastRefreshEnemyTime = time(NULL);
}

// 是否有红点
bool CEliteFB::IsHaveRedPoint()
{
	// 当精英副本有未击杀的外敌怪物时
	if (GetEnemyNum())
		return true;

	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		EliteFBChapter *pChapter = &m_pFBData->arChapterList[i];

		WORD wTotalStar = pChapter->GetStar();

		// 检测章节数据的合法性
		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetElitFBeChapter(i + 1);
		if (!pChapterCfg || pChapterCfg->vStarAwardList.size() != CHAPTER_STAR_AWARD_MAX)
			continue;

		// 当精英副本有可领取的星级宝箱时
		for (BYTE k = 0; k < CHAPTER_STAR_AWARD_MAX; ++k)
		{
			// 星数到达可领取,并且还木有领取
			if (!pChapter->arStarAwardList[k] && wTotalStar >= pChapterCfg->vStarAwardList[k].byStar)
				return true;
		}

		// 当精英副本有可领取的精英宝箱时
		for (BYTE k = 0; k < pChapter->byMapIdx; ++k)
		{
			MapData *pMap = &pChapter->arMapList[k];
			const Map_Config *pMapCfg = g_pCfgMgr->GetMap(pChapterCfg->GetMapId(k + 1));
			if (pMapCfg && pMapCfg->vBoxList.size() && !pMap->byIsGetAward)
				return true;
		}
	}

	return false;
}

// 一键领取奖励(关卡+星数)
bool CEliteFB::OnNetOneKeyTakeReward(WORD wChapter)
{
	if (!wChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]章节参数不合法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	EliteFBChapter *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的章节非法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 检测是否已通关
	if (pChapterCfg->vMapList.size() != pChapter->byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]此章节还木有通关,不能领取[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 处理宝箱奖励
	for (BYTE i = 0; i < pChapter->byMapIdx; ++i)
	{
		MapData &tMap = pChapter->arMapList[i];
		// 已经领取过
		if (tMap.byIsGetAward)
			continue;

		// 取关卡配置
		DWORD dwMapId = pChapterCfg->GetMapId(i + 1);
		const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
		if (!pMapCfg)
			continue;

		// 有宝箱奖励
		if (pMapCfg->vBoxList.size())
		{
			m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_ELITE_FB_ONE_KEY_REWARD);
			tMap.byIsGetAward = true;
		}
	}

	// 处理星数奖励
	WORD wTotalStar = pChapter->GetStar();
	for (BYTE i = 0; i < CHAPTER_STAR_AWARD_MAX; ++i)
	{
		// 已经领取过
		if (pChapter->arStarAwardList[i])
			continue;

		// 安全检测,防止越界(todo::不应该出现这种问题)
		if (i >= pChapterCfg->vStarAwardList.size())
			continue;

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[i];

		// 检测章节的数量是否足够
		if (wTotalStar < pStarAwardCfg->byStar)
			continue;

		// 发奖励
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_ELITE_FB_ONE_KEY_REWARD);

		pChapter->arStarAwardList[i] = true;
	}

	return true;
}

// 创建外敌
WORD CEliteFB::CreateEnemy()
{
	// 刷新之前清除已经死掉的外敌
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		EliteMapEnemyData &tEnemyData = m_pFBData->arChapterList[i].tEnemyData;
		if (tEnemyData.wMapId && tEnemyData.wIsDead)
			memset(&tEnemyData, 0, sizeof(tEnemyData));
	}

	m_wNewEnemyNum = 0;

	const FubenChapter_Config *pCurChapterCfg = g_pCfgMgr->GetElitFBeChapter(m_pFBData->wChapter);
	if (pCurChapterCfg)
	{
		// 计算随机列表 
		vector<WORD> vRankChapterList;
		for (WORD i = pCurChapterCfg->wEnemyRandChapterMin; i <= pCurChapterCfg->wEnemyRandChapterMax; ++i)
		{
			vRankChapterList.push_back(i);
		}

		// 随机外敌数量
		WORD wCreateNum = g_Rand(pCurChapterCfg->wEnemyRandNumMin, pCurChapterCfg->wEnemyRandNumMax);
		for (WORD i = 0; i < wCreateNum; ++i)
		{
			int nListNum = vRankChapterList.size();
			if (nListNum > 0)
			{
				// 随机章节
				WORD wRandChapter = g_Rand(0, nListNum-1);
				CreateEnemy(wRandChapter);

				// 拿掉已经随到的章节
				vRankChapterList[wRandChapter] = vRankChapterList[nListNum-1];
				vRankChapterList.pop_back();
			}
		}
	}
	m_pFBData->dwLastRefreshEnemyTime = time(NULL);

	return m_wNewEnemyNum;

}

// 创建外敌
void CEliteFB::CreateEnemy(WORD wChapter)
{
	// 检测当前外敌数量(外敌数量有上限)
// 	if (GetEnemyNum() >= 4)
// 		return;

	// 检测是否已通关
	if (wChapter >= m_pFBData->wChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]当前精英副本章节还没通过,不能创建外敌[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}

	EliteFBChapter &tChapterData = m_pFBData->arChapterList[wChapter];
	
	// 检测是否已创建过
	if (tChapterData.tEnemyData.wMapId)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]当前精英副本章节已创建外敌[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}
	
	// 检测是否满星通关
	BYTE byTotalStar = 0;
	for (BYTE i = 0; i < tChapterData.byMapIdx; ++i)
	{
		byTotalStar += tChapterData.arMapList[i].byStar;
	}
	if (byTotalStar < 12/*暂时介么处理*/)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]木有满星的章节不能创建外敌[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter+1);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本章节配置数据[chapter:%d]", __FUNCTION__, wChapter+1);
		return;
	}
	
	// 随机外敌
	int nRandValue = g_Rand();
	int nRate = 0;
	for (size_t i = 0; i < pChapterCfg->vEnemyMapRateList.size(); ++i)
	{
		nRate += pChapterCfg->vEnemyMapRateList[i];
		if (nRandValue <= nRate)
		{
			tChapterData.tEnemyData.wMapId = pChapterCfg->vEnemyMapList[i];
			m_wNewEnemyNum++;
			break;
		}
	}
}

// 取当前外敌数量
BYTE CEliteFB::GetEnemyNum()
{
	BYTE byNum = 0;
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		if (m_pFBData->arChapterList[i].tEnemyData.wMapId && !m_pFBData->arChapterList[i].tEnemyData.wIsDead)
			byNum++;
	}
	return byNum;
}

// 取关卡数据
MapData* CEliteFB::GetMapData(WORD wChapter, BYTE byMapIdx)
{
	EliteFBChapter *pChapterData = GetChapterData(wChapter);
	if (pChapterData && byMapIdx <= pChapterData->byMapIdx)
	{
		return &pChapterData->arMapList[byMapIdx];
	}

	return NULL;
}

// 取章节数据
EliteFBChapter* CEliteFB::GetChapterData(WORD wChapter)
{
	if (!wChapter || wChapter > MAX_FUBEN_CHAPER_NUM)
		return NULL;

	return &m_pFBData->arChapterList[wChapter - 1];
}

// 取下一个挑战的章节
WORD CEliteFB::GetNextChapter()
{
	WORD wChapter = m_pFBData->wChapter;

	// 默认从第一章开始打
	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	// 本章节已本部打完
	if (m_pFBData->arChapterList[wChapter - 1].byMapIdx >= pChapterCfg->vMapList.size())
		return wChapter + 1;

	return wChapter;
}

// 取下一个挑战的关卡索引
BYTE CEliteFB::GetNextMapIndex()
{
	WORD wChapter = m_pFBData->wChapter;

	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精英副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	BYTE byMapIdx = m_pFBData->arChapterList[wChapter - 1].byMapIdx;

	// 本章节已本部打完
	if (byMapIdx >= pChapterCfg->vMapList.size())
		return 1;

	return byMapIdx + 1;
}

// 取总星数
WORD CEliteFB::GetTotalStar()
{
	WORD wTotalStar = 0;
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		EliteFBChapter &tChapter = m_pFBData->arChapterList[i];
		for (BYTE k = 0; k < tChapter.byMapIdx; ++k)
		{
			wTotalStar += tChapter.arMapList[k].byStar;
		}
	}
	return wTotalStar;
}

// 是否外敌挂掉
bool CEliteFB::IsEnemyDead(WORD wChapter)
{
// 	// 取章节数据
// 	EliteFBChapter *pChapterData = GetChapterData(wChapter);
// 	if (!pChapterData)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的精英副本章节数据[chapter:%d]", __FUNCTION__, wChapter);
// 		return false;
// 	}
// 
// // 	if (pChapterData->tEnemyData.wMapId)
// // 	{
// // 		MAPLOG(ERROR_LEVEL, "[%s]当前精英副本章节木有外敌可挑战[chapter:%d]", __FUNCTION__, wChapter);
// // 		return false;
// // 	}
// 
// 	// 检测是否已死亡
// 	for (BYTE i = 0; i < pChapterData->tEnemyData.byMonsterNum; ++i)
// 	{
// 		MonsterData *pMonster = &pChapterData->tEnemyData.arMonsterList[i];
// 		if (pMonster->dwDamage < pMonster->dwTotalHP)
// 			return false;
// 	}

	return true;
}

// 更新怪物伤害
void CEliteFB::UpateMonsterDamage(WORD wChapter, MonsterData *pMonsterList, BYTE byMonsterNum)
{
// 	EliteFBChapter *pChapterData = GetChapterData(wChapter);
// 	if (!pChapterData)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的精英副本章节数据[chapter:%d]", __FUNCTION__, wChapter);
// 		return;
// 	}
// 
// 	for (BYTE i = 0; i < byMonsterNum; ++i)
// 	{
// 		for (int j = 0; j < pChapterData->tEnemyData.byMonsterNum; ++j)
// 		{
// 			MonsterData &tMonster = pChapterData->tEnemyData.arMonsterList[j];
// 			if (pMonsterList[i].byIndex == tMonster.byIndex)
// 				tMonster.dwDamage += pMonsterList[i].dwDamage;
// 		}
// 	}
}


// 发送副本数据
void CEliteFB::SendFBData()
{
	MSG_S2C_FB_ELITE_DATA_RESP msg;
	msg.m_Data = *m_pFBData;
	m_pPlayer->SendPacket(&msg);
}



/////////////////////////////////////////////////////////////////////////////////////
// 英雄副本

CHeroFB::CHeroFB()
{
	
}
CHeroFB::~CHeroFB()
{

}
void CHeroFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	CFBBase::Init(pPlayer, pFBMgr);
	m_pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tHeroFubenData;
}

// 开始战斗
bool CHeroFB::OnBattleStart(WORD wChapter, BYTE byMapIdx)
{
	// 检测挑战次数
	if (byMapIdx && m_pFBData->byBattleTimes >= m_pPlayer->GetVipValue(VIP_HERO_FB_BATTLE_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄副本今日挑战次数达上限", __FUNCTION__);
		return false;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本章节配置[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	WORD wMapId = byMapIdx ? pChapterCfg->GetMapId(byMapIdx) : pChapterCfg->wExtraMapId;

	// 取配置
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本关卡配置[chapter:%d,MapId:%d]",
			__FUNCTION__, wChapter, wMapId);
		return false;
	}
	
	// 检测体力
// 	if (m_pPlayer->GetProperty(ROLE_PRO_STAMINA) < pMapCfg->nCostStamina)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]英雄副本关卡挑战体力不足!", __FUNCTION__);
// 		return false;
// 	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄关卡挑战消耗不足!", __FUNCTION__);
		return false;
	}

	// 取章节数据
	HeroFubenChapterData *pChapterData = GetChapterData(wChapter);
	if (!pChapterData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的英雄副本章节数据[chapter:%d]",__FUNCTION__, wChapter);
		return false;
	}

	// 越章节挑战
	if (wChapter > GetNextChapter())
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄副本越章节挑战!", __FUNCTION__);
		return false;
	}

	// 越关卡挑战
	if (GetNextChapter() == wChapter && byMapIdx > GetNextMapIndex())
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄副本越关卡挑战!", __FUNCTION__);
		return false;
	}

	// 挑战等级关卡
	if (byMapIdx)
	{
		// 检测当前挑战次数
		if (pChapterData->arMapList[byMapIdx - 1].byBattleTimes >= pMapCfg->byDailyBattleTimesLimit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄副本关卡今日已挑战过!", __FUNCTION__);
			return false;
		}

		// 是否已通关
// 		if (wChapter < m_pFBData->wChapter || byMapIdx <= pChapterData->byMapIdx)
// 		{
// 			m_pFBManager->SetIsPassed(true);
// 		}
	}
	// 挑战额外关卡
	else
	{
		// 检测是否存在额外关卡
		if (!pChapterCfg->wExtraMapId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄副本本章节没额外关卡可以挑战[chapter:%d]!", __FUNCTION__, wChapter);
			return false;
		}

		// 检测是否已挑战过
		if (pChapterData->byIsExtraMapPass)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄副本本章节的额外关卡已挑战过[chapter:%d]!", __FUNCTION__, wChapter);
			return false;
		}

		// 检测是否可以挑战了
		if (pChapterData->byMapIdx < pChapterCfg->vMapList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]必需通关本章节所有关卡才能挑战额外关卡!", __FUNCTION__);
			return false;
		}
	}

	// 记录战斗信息
	m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId);

	return true;
}

// 战斗结束
bool CHeroFB::OnBattleOver(BYTE byIsWin, BYTE byStar)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	BYTE byMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本章节配置[chapter:%d]",__FUNCTION__, wChapter);
		return false;
	}

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本关卡配置[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	HeroFubenChapterData *pChapterData = GetChapterData(wChapter);
	if (!pChapterData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到玩家的英雄副本章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	if (byIsWin)
	{
		// 处理奖励
		m_pFBManager->DoWinReward(BATTLE_HERO_MAP, pMapCfg);

		// 处理消耗
		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		// 挑战等级关卡
		if (byMapIdx)
		{
			// 如果是挑战新的关卡
			if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
			{
				m_pFBData->wChapter = wChapter;
				pChapterData->arMapList[byMapIdx - 1].byBattleTimes = 1;
				pChapterData->arMapList[byMapIdx - 1].byIsGetAward = false;
				pChapterData->byMapIdx = byMapIdx;

				// 此关卡首次获胜
				m_pFBManager->SetIsFirstWin(true);
			}
			else
			{
				// 记录挑战次数
				pChapterData->arMapList[byMapIdx - 1].byBattleTimes++;
			}

			m_pFBData->byBattleTimes++;
		}
		// 挑战额外关卡
		else
		{
			pChapterData->byIsExtraMapPass = true;
		}
	}

	SendFBData();

	// 日常任务
	m_pPlayer->OnUpdateDailyMission(14);

	return true;
}

// 领取副本奖励
bool CHeroFB::OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx)
{
	if (!wChapter || !byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]参数不合法[chapter:%d,index:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	BYTE bySvrMapIdx = byMapIdx - 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到章节数据[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	HeroFubenChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的章节非法[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 关卡奖励
	if (byRewardType == FB_AWARD_MAP)
	{
		if (bySvrMapIdx >= pChapter->byMapIdx)
		{
			MAPLOG(ERROR_LEVEL, "[%s]请求领取奖励的关卡非法[chapter:%d,map:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterCfg->vMapList[bySvrMapIdx]);
		if (!pMapCfg || !pMapCfg->vBoxList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到关卡配置数据或者木有配置宝箱奖励[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		HeroMapData &tMapData = pChapter->arMapList[bySvrMapIdx];

		// 检测是否已经领取过
		if (tMapData.byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]已经领取过宝箱奖励[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		// 添加奖励
		m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_HERO_FB_REWARD);

		tMapData.byIsGetAward = true;
	}
	// 星数奖励
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄副本木有星数奖励[chapter:%d,index:%d]", __FUNCTION__, wChapter, bySvrMapIdx);
	}

	return true;
}

// 请求英雄副本数据
void CHeroFB::OnHeroFBData()
{
	SendFBData();
}

// 取章节数据
HeroFubenChapterData* CHeroFB::GetChapterData(WORD wChapter)
{
	if (!wChapter || wChapter > MAX_FUBEN_CHAPER_NUM)
		return NULL;

	return &m_pFBData->arChapterList[wChapter-1];
}

// 取下一个挑战的章节
WORD CHeroFB::GetNextChapter()
{
	if (!m_pFBData->wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(m_pFBData->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本章节配置[chapter:%d]", __FUNCTION__, m_pFBData->wChapter);
		return 1;
	}

	// 本章节已本部打完
	if (m_pFBData->arChapterList[m_pFBData->wChapter - 1].byMapIdx >= pChapterCfg->vMapList.size())
		return m_pFBData->wChapter + 1;

	return m_pFBData->wChapter;
}

// 取下一个挑战的关卡索引
BYTE CHeroFB::GetNextMapIndex()
{
	if (!m_pFBData->wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(m_pFBData->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本章节配置[chapter:%d]", __FUNCTION__, m_pFBData->wChapter);
		return 1;
	}

	BYTE byMapIdx = m_pFBData->arChapterList[m_pFBData->wChapter - 1].byMapIdx;

	// 本章节已本部打完
	if (byMapIdx >= pChapterCfg->vMapList.size())
		return 1;

	return byMapIdx + 1;
}

// 是否有红点
bool CHeroFB::IsHaveRedPoint()
{
	// 当英雄副本每日次数上限没有使用完时
	if (m_pFBData->byBattleTimes < m_pPlayer->GetVipValue(VIP_HERO_FB_BATTLE_TIMES))
		return true;

	// 当英雄副本内有精英宝箱可领取时
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		HeroFubenChapterData *pChapter = &m_pFBData->arChapterList[i];
		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetHeroFBChapter(i+1);
		if (!pChapterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本章节配置[chapter:%d]", __FUNCTION__, i + 1);
			return false;
		}
		for (BYTE k = 0; k < pChapter->byMapIdx; ++k)
		{
			HeroMapData *pMap = &pChapter->arMapList[k];
			if (pMap->byIsGetAward)
				continue;
			DWORD dwMapId = pChapterCfg->GetMapId(k+1);
			const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
			if (!pMapCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到英雄副本配置[MapId:%d]", __FUNCTION__, dwMapId);
				return false;
			}
			if (pMapCfg->vBoxList.size())
				return true;
		}
	}
	
	return false;
}


// 发送副本数据
void CHeroFB::SendFBData()
{
	MSG_S2C_FB_HERO_DATA_RESP msg;
	memcpy_s(&msg.m_tFBData, sizeof(HeroFBData), m_pFBData, sizeof(HeroFBData));
	m_pPlayer->SendPacket(&msg);
}


//////////////////////////////////////////////////////
// 日常副本

CDailyFB::CDailyFB()
{


}

CDailyFB::~CDailyFB()
{

}

void CDailyFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	CFBBase::Init(pPlayer, pFBMgr);
	m_pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tDailyFubenData;
}

// 开始战斗 
bool CDailyFB::OnBattleStart(WORD wChapter, BYTE byMapIdx)
{
	// 取副本章节配置
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetDailyChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到日常副本章节副本[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 检测是否越界
	if (!byMapIdx || byMapIdx > pChapterCfg->vMapList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]日常副本关卡索引出错[MapIdx:%d]", __FUNCTION__, byMapIdx);
		return false;
	}

	BYTE byServerMapIdx = byMapIdx - 1;

	// 检测等级
	if (m_pPlayer->GetLevel() < pChapterCfg->wOpenLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]日常副本受等级限制[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	time_t tCurTime = time(NULL);
	tm tmTime;
	localtime_s(&tmTime, &tCurTime);

	// 检测今日是否开放
	if (!pChapterCfg->IsOpenDay(tmTime.tm_wday))
	{
		MAPLOG(ERROR_LEVEL, "[%s]此日常副本今日没开放[chapter:%d,weekday:%d]", __FUNCTION__, wChapter, tmTime.tm_wday);
		return false;
	}

	WORD wMapId = pChapterCfg->vMapList[byServerMapIdx];

	// 取关卡配置
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到日常副本关卡配置[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	DailyChapterData *pChapter = GetChapterData(wChapter);

	// 检测挑战次数
	if (pChapter && pChapter->byBattleTimes >= pChapterCfg->wDailyBattleTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]日常副本挑战次数已达上限[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 检测前置关卡是否已挑战过
	if (pChapter && pChapter->nMapIdxRecord >= 0 && byMapIdx > pChapter->nMapIdxRecord + 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]日常副本的前置关卡还木有挑战[chapter:%d,MapIdx:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	// 检测体力
	if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]日常副本挑战消耗不足[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId);
	return true;
}

// 战斗结束
bool CDailyFB::OnBattleOver(BYTE byIsWin, WORD wContext, BYTE byIsPass)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	WORD wMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	//WORD wServerMapIdx = wMapIdx - 1;

	// 取章节配置
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetDailyChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到日常副本章节副本[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 取关卡配置
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到日常副本关卡配置[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	// 取副本数据
	DailyChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		DailyChapterData tChapter;
		tChapter.wChapter = wChapter;
		tChapter.nMapIdxRecord = 0;
		tChapter.byBattleTimes = 0;
		AddChapterData(tChapter);
		pChapter = GetChapterData(wChapter);
	}
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到日常副本章节数据[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// 发放浮动奖励(根据输出,时间等)
	const DailyFBDamageReward_Config *pDamageRewardCfg = g_pCfgMgr->GetDailyFBDamageReward(wChapter, wMapIdx, wContext);

	// 获胜
	if (byIsWin)
	{
		if (pDamageRewardCfg)
			m_pPlayer->AddObjectList(pDamageRewardCfg->vItemList, TRUE, FROM_DAILY_FB_BATTLE_REWARD);

		// 获胜会扣次数
		pChapter->byBattleTimes++;

		m_pFBManager->DoWinReward(BATTLE_DAILY_MAP, pMapCfg);

		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		// 如果是首次通关
		if (wMapIdx > pChapter->nMapIdxRecord && byIsPass)
		{
			// 返还挑战次数
			if (pChapter->byBattleTimes)
				pChapter->byBattleTimes--;

			// 记录挑战记录
			pChapter->nMapIdxRecord = wMapIdx;
		}
	}
	else
	{
		// 失败也有奖励
		if (pChapterCfg->byIsLoseAlsoHaveReward)
		{
			if (pDamageRewardCfg)
			{
				m_pPlayer->AddObjectList(pDamageRewardCfg->vItemList, TRUE, FROM_DAILY_FB_BATTLE_REWARD);

				// 失败时有奖励也要扣次数
				pChapter->byBattleTimes++;
			}
		}
	}

	//MAPLOG(GUI_LEVEL, "[%s]发放日常副本奖励[Chapter:%d,MapIdx:%d,Context:%d]", __FUNCTION__, wChapter, wMapIdx, wContext);

	// 日常任务
	m_pPlayer->OnUpdateDailyMission(13);

	return true;
}

// 请求日常副本数据
void CDailyFB::OnDailyDataReq()
{
	SendDailyData();
}

// 发送日常副本数据
void CDailyFB::SendDailyData()
{
	MSG_S2C_FB_DAILY_DATA_RESP msg;
	memcpy_s(&msg.m_tFBData, sizeof(DailyFBData), m_pFBData, sizeof(DailyFBData));
	m_pPlayer->SendPacket(&msg);
}

// 取章节数据
DailyChapterData* CDailyFB::GetChapterData(WORD wChapter)
{
	for (BYTE i = 0; i < m_pFBData->wChapterNum; ++i)
	{
		if (m_pFBData->arChapterList[i].wChapter == wChapter)
			return &m_pFBData->arChapterList[i];
	}
	return NULL;
}

// 添加章节数据
void CDailyFB::AddChapterData(const DailyChapterData &tChapter)
{
	if (m_pFBData->wChapterNum >= MAX_DAILY_MAP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]日常副本章节列表已满", __FUNCTION__);
		return;
	}

	m_pFBData->arChapterList[m_pFBData->wChapterNum++] = tChapter;
}




/////////////////////////////////////////////////////////
// 爬塔副本
CTowerFB::CTowerFB()
{


}

CTowerFB::~CTowerFB()
{

}

void CTowerFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	CFBBase::Init(pPlayer, pFBMgr);
	m_pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tTowerFubenData;
}

// 普通战斗开始
bool CTowerFB::OnNormalBattleStart(BYTE byType)
{
	// 检测是否需要重置
  	if (m_pFBData->byIsNeedReset)
  	{
  		MAPLOG(ERROR_LEVEL, "[%s]副本需要重置方可挑战", __FUNCTION__);
  		return false;
  	}
  
	// 准备挑战的关卡
	WORD wNextMapLevel = m_pFBData->wCurMapLevel + 1;

	// 检测关卡上限
	if (wNextMapLevel >= MAX_TOWER_MAP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔关卡已达上限[MaxLevel:%d]", __FUNCTION__, wNextMapLevel);
		return false;
	}
	
	// 取关卡配置
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(wNextMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通爬塔关卡配置[MapLevel:%d]", __FUNCTION__, wNextMapLevel);
		return false;
	}

  	// 记录战斗信息
	m_pFBManager->SetBattleInfo(byType, 0);

	return true;
}

// 普通战斗结束
bool CTowerFB::OnNormalBattleOver(BYTE byIsWin, bool bIsClean)
{ 
	// 准备挑战的关卡
	WORD wNextMapLevel = m_pFBData->wCurMapLevel + 1;

	// 取关卡配置
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(wNextMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通爬塔关卡配置[MapLevel:%d]", __FUNCTION__, wNextMapLevel);
		return false;
	}

	// 取章节配置
	const TowerChapter_Config *pTowerChapterCfg = CConfigManager::Instance()->GetTowerChapter(pTowerMapCfg->wChapter);
	if (!pTowerChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通爬塔章节配置[MapLevel:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
		return false;
	}

	BYTE byType = m_pFBManager->GetCurChapter();
	if (byType >= pTowerMapCfg->MapDataList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]选择的爬塔类型越界[type:%d]", __FUNCTION__, byType);
		return false;
	}

	const TowerMap_Config::MapData &tMapData = pTowerMapCfg->MapDataList[byType];

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(tMapData.wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到挑战关卡的配置[MapLevel:%d]", __FUNCTION__, tMapData.wMapId);
		return false;
	}

	bool bIsLose = false;

	int nMultiple = CActivityManager::GetLimitProductMultiple(BATTLE_TOWER_NORMAL_MAP);

	// 获胜
	if (byIsWin)
	{
		// 记录获得的星数
		m_pFBData->arMapStarList[m_pFBData->wCurMapLevel] = tMapData.byStar;

		// 如果是本章节最后一关
		if (pTowerMapCfg->bIsLast)
		{
			// 本章节获得的星数
			WORD wChapterStar = 0;
			for (int i = 0; i < pTowerChapterCfg->vMapList.size(); ++i)
			{
				if (pTowerChapterCfg->vMapList[i])
					wChapterStar += m_pFBData->arMapStarList[pTowerChapterCfg->vMapList[i]-1];
			}

			// 本章节奖励
			const TowerStarAward *pStarAward = pTowerChapterCfg->GetAward(wChapterStar);
			if (!pStarAward)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到爬塔章节星数奖励配置[star:%d]", __FUNCTION__, wChapterStar);
				return false;
			}
			m_pPlayer->AddObjectList(pStarAward->vAwardList, TRUE, FROM_TOWER_NORMAL_FB_BATTLE_REWARD);

			// 随机属性  todo::此处代码需要优化
			m_pFBData->byRandProNum = 0;
			m_pFBData->arRandProIdxList[m_pFBData->byRandProNum++] = RandProIdx(3);
			m_pFBData->arRandProIdxList[m_pFBData->byRandProNum++] = RandProIdx(6);
			m_pFBData->arRandProIdxList[m_pFBData->byRandProNum++] = RandProIdx(9);

			// 都是3星通关 
			if (wChapterStar >= 9 && m_pFBData->byKeepThreeStarPassedRecord + 1 == pTowerMapCfg->wChapter)
			{
				m_pFBData->byKeepThreeStarPassedRecord = pTowerMapCfg->wChapter;
			}
		}

		for (int i = 0; i < nMultiple; ++i)
		{
			m_pPlayer->ModifyPropertyList(pMapCfg->vNormalRewardList, TRUE, FROM_TOWER_FB_BATTLE);
		}

		// 往下一关
		m_pFBData->wCurMapLevel++;

		// 记录星数
		m_pFBData->wCurStar += tMapData.byStar;

		// 计算当前总星数
		WORD wCurTotalStar = 0;
		for (WORD i = 0; i < m_pFBData->wCurMapLevel; ++i)
		{
			wCurTotalStar += m_pFBData->arMapStarList[i];
		}

		// 更新历史层数
		if (m_pFBData->wCurMapLevel > m_pFBData->wMapLevelRecord)
		{
			m_pFBData->wMapLevelRecord = m_pFBData->wCurMapLevel;

			// 处理广播
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(109);
			if (pServerCfg && pServerCfg->IsInList(m_pFBData->wMapLevelRecord))
			{
				CSystemNotice sys_notice(3, m_pPlayer->GetServerId());
				sys_notice.AddParam(m_pPlayer->GetRoleName());
				sys_notice.AddParam(m_pFBData->wMapLevelRecord);
				sys_notice.SendMsg();
			}
		}

		// 如果当前星数超过历史记录
		if (wCurTotalStar > m_pFBData->wStarMaxRecord)
		{
			m_pFBData->wStarMaxRecord = wCurTotalStar;

			// 添加爬塔排行榜数据
			CRankManager::Instance()->AddRoleRankData(RANK_TOWER, m_pPlayer, m_pFBData->wStarMaxRecord, 0, 0, 20);
		}

		// 成就事件
		m_pPlayer->OnUpdateAchievement(7, m_pFBData->wStarMaxRecord, false);

		// 7天活动事件(爬塔星数累计)
		m_pPlayer->OnUpdateSevenDayTarget(12, tMapData.byStar, true);
	}
	// 失败
	else
	{
		bIsLose = true;
	}

	// 检测是否已通关所有关卡 或者 失败 
	if (m_pFBData->wCurMapLevel >= CConfigManager::Instance()->m_TowerChapterList.size() * 3 || bIsLose)
	{
		m_pFBData->byIsNeedReset = true;
		m_pFBData->byIsBuyStarGoods = false;

		const TowerStarGoods_Config *pStarGoods = CConfigManager::Instance()->GetTowerStarGoods(GetTotalStar());
		if (!pStarGoods || !pStarGoods->vGoodsList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到爬塔星数商品或列表为空[star:%d]", __FUNCTION__, GetTotalStar());
			return false;
		}

		// 随机奖励
		m_pFBData->byStarGoodsIndex = Rand::Instance()->irand(0, pStarGoods->vGoodsList.size() - 1);
	}

	if (!bIsClean)
		SendTowerData();

	// 日常任务
	m_pPlayer->OnUpdateDailyMission(12);

	return true;
}

// 精英战斗开始
bool CTowerFB::OnEliteBattleStart(WORD wMapLv)
{
	// 检测是否越关卡挑战
	if (wMapLv > m_pFBData->wEliteMapLevel + 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精英爬塔出现越关卡挑战[MapLevel:%d]", __FUNCTION__, wMapLv);
		return false;
	}

	// 取精英关卡配置
	const TowerEliteMap_Config *pEliteMapCfg = CConfigManager::Instance()->GetTowerEliteMap(wMapLv);
	if (!pEliteMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到爬塔精英关卡配置[MapLevel:%d]", __FUNCTION__, wMapLv);
		return false;
	}

	// 检测挑战次数
	if (m_pFBData->byEliteMapBattleTimes >= m_pFBData->byBuyEliteMapBattleTimes + g_GetConfigValue(32))
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔精英关卡挑战次数受限制[MapLevel:%d]", __FUNCTION__, wMapLv);
		return false;
	}

	// 如果是打下一个关卡
	if (wMapLv == m_pFBData->wEliteMapLevel + 1)
	{
		// 检测前置关卡
		if (pEliteMapCfg->wNeedNormalMapLevel > m_pFBData->wMapLevelRecord)
		{
			MAPLOG(ERROR_LEVEL, "[%s]先通关普通关卡[MapLevel:%d]才能挑战", __FUNCTION__, pEliteMapCfg->wNeedNormalMapLevel);
			return false;
		}
	}	

	m_pFBManager->SetBattleInfo(1, wMapLv, pEliteMapCfg->wMapId);

	return true;
}

// 精英战斗开始
bool CTowerFB::OnEliteBattleOver(BYTE byIsWin)
{
	BYTE byMapLevel = m_pFBManager->GetMapIdx();

	// 取精英关卡配置
	const TowerEliteMap_Config *pEliteMapCfg = CConfigManager::Instance()->GetTowerEliteMap(byMapLevel);
	if (!pEliteMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到爬塔精英关卡配置[MapLevel:%d]", __FUNCTION__, byMapLevel);
		return false;
	}

	// 取挑战关卡配置
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pEliteMapCfg->wMapId);
 	if (!pMapCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到挑战关卡的配置[MapId:%d]", __FUNCTION__, pEliteMapCfg->wMapId);
 		return false;
 	}
 
 	// todo::防作弊检测
 
 	if (byIsWin)
 	{
		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		m_pPlayer->ModifyPropertyList(pMapCfg->vNormalRewardList, TRUE, FROM_TOWER_FB_BATTLE);

		// 首胜奖励
		if (byMapLevel == m_pFBData->wEliteMapLevel + 1)
		{
			// 此关卡首次获胜
			m_pFBManager->SetIsFirstWin(true);

			// 是否需要往下走
			m_pFBData->wEliteMapLevel = byMapLevel;
		}

		m_pFBData->byEliteMapBattleTimes++;

		// 节日活动(每日战胜勇者试炼精英关卡次数)
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 14, 1, true);

		// 节日活跃目标(每日战胜勇者试炼精英关卡次数)
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 14, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 14, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 14, 1, true);
 	}

	SendTowerData();

	return true;
}

// 购买挑战次数
void CTowerFB::OnNetBattleTimesBuy()
{
	// 检测购买次数
	if (m_pFBData->byBuyEliteMapBattleTimes >= m_pPlayer->GetVipValue(VIP_BUY_TOWER_FB_BATTLE_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔关卡挑战次数购买已达上限!", __FUNCTION__);
		return;
	}

	// 取通用配置
	int nFindKey = 31;
	const Server_Config* pCfg = CConfigManager::Instance()->GetCommonCfg(nFindKey);
	if (!pCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置[key:%d]!", __FUNCTION__, nFindKey);
		return;
	}
	if (!pCfg->vValueList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔精英关卡购买次数消耗木有配置[key:%d]!", __FUNCTION__, nFindKey);
		return;
	}

	int nCostDiamond = 0;
	if (m_pFBData->byBuyEliteMapBattleTimes >= pCfg->vValueList.size())
	{
		nCostDiamond = pCfg->vValueList.back();
	}
	else
	{
		nCostDiamond = pCfg->vValueList[m_pFBData->byBuyEliteMapBattleTimes];
	}

	// 检测钻石
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买爬塔精英关卡钻石不足!", __FUNCTION__);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TOWER_BATTLE_TIMES_BUY);

	m_pFBData->byBuyEliteMapBattleTimes++;

	MSG_S2C_TOWER_BUY_TIMES_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 重置副本
bool CTowerFB::OnNetFBReset(bool bIsGM)
{
	// 检测是否需要重置
	if (!bIsGM && !m_pFBData->byIsNeedReset)
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔目前不需要重置!",__FUNCTION__);
		return false;
	}

	// 检测今日重置次数上限
	if (m_pFBData->byTodayResetTimes >= m_pPlayer->GetVipValue(VIP_TOWER_FB_RESET_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔关卡今日重置次数已达上限!", __FUNCTION__);
		return false;
	}

	// 取通用配置
	int nFindKey = 30;
	const Server_Config* pCfg = CConfigManager::Instance()->GetCommonCfg(nFindKey);
	if (!pCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置[key:%d]!", __FUNCTION__, nFindKey);
		return false;
	}
	if (!pCfg->vValueList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔精英关卡重置消耗木有配置[key:%d]!", __FUNCTION__, nFindKey);
		return false;
	}

	// 计算需要消耗的钻石
	int nCostDiamond = 0;
	if (m_pFBData->byTodayResetTimes >= pCfg->vValueList.size())
	{
		nCostDiamond = pCfg->vValueList.back();
	}
	else
	{
		nCostDiamond = pCfg->vValueList[m_pFBData->byTodayResetTimes];
	}

	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]钻石不足,重置爬塔关卡失败!",__FUNCTION__);
		return false;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TOWER_RESET);

	// 重置数据
	m_pFBData->wCurMapLevel = 0;
	m_pFBData->byStarGoodsIndex = 0;
	m_pFBData->byIsBuyStarGoods = false;
	m_pFBData->byIsNeedReset = false;
	m_pFBData->bySelectProNum = 0;
	m_pFBData->byRandProNum = 0;
	m_pFBData->wCurStar = 0;

	m_pFBData->byTodayResetTimes++;

	SendTowerData();

	MSG_S2C_TOWER_RESET_RESP msg;
	m_pPlayer->SendPacket(&msg);

	// 8-14天活动事件(爬塔重置次数)
	m_pPlayer->OnUpdateSevenDayTarget(22, 1, true);

	// 节日活动(累计勇者试炼重置次数)
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 15, 1, true);

	// 节日活跃目标(累计勇者试炼重置次数)
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 15, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 15, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 15, 1, true);

	return true;
}

// 选择属性
void CTowerFB::OnNetSelectPro(BYTE byStar, BYTE byProIndex)
{
	// 取关卡配置
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(m_pFBData->wCurMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通爬塔关卡配置[MapLevel:%d]", __FUNCTION__, m_pFBData->wCurMapLevel);
		return;
	}

	// 检测是否最后一关
	if (!pTowerMapCfg->bIsLast)
	{ 
		MAPLOG(ERROR_LEVEL, "[%s]非爬塔章节最后一关,还没能选属性[MapLevel:%d]", __FUNCTION__, m_pFBData->wCurMapLevel);
		return;
	}

	// 取星数对应的配置
	const TowerRandPro_Config *pRandProCfg = CConfigManager::Instance()->GetTowerRandPro(byStar);
	if (!pRandProCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到爬塔随机属性配置[Star:%d]", __FUNCTION__, byStar);
		return;
	}

	// 检测星数是否足够 
	if (m_pFBData->wCurStar < byStar)
	{
		MAPLOG(ERROR_LEVEL, "[%s]星数不足,不能选择此属性[Star:%d]", __FUNCTION__, byStar);
		return;
	}

	// 检测属性索引
	if (byProIndex >= pRandProCfg->vProList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]选择的属性索引越界[Star:%d, ProIndex:%d]", __FUNCTION__, byStar, byProIndex);
		return;
	}

	const Property &tPro = pRandProCfg->vProList[byProIndex];

	// 有则叠加
	bool bIsHave= false;
	for (BYTE i = 0; i < m_pFBData->bySelectProNum; ++i)
	{
		if (m_pFBData->arSelectProList[i].wType == tPro.wType)
		{
			m_pFBData->arSelectProList[i].nValue += tPro.nValue;
			bIsHave = true;
			break;
		}
	}
	// 无则添加
	if (!bIsHave && m_pFBData->bySelectProNum < HERO_PRO_MAX)
	{
		m_pFBData->arSelectProList[m_pFBData->bySelectProNum++] = tPro;
	}	

	// 扣星数
	m_pFBData->wCurStar -= byStar;

	// 清空随机数据
	m_pFBData->byRandProNum = 0;
	//memset(m_pFBData->arRandProIdxList, 0, sizeof(m_pFBData->arRandProIdxList));

	SendTowerData();

	MSG_S2C_TOWER_SELECT_PRO_RESP msg;
	msg.m_byStar = byStar;
	msg.m_byProIndex = byProIndex;
	m_pPlayer->SendPacket(&msg);
}

// 购买星数商品
void CTowerFB::OnNetBuyStarGoods()
{
	// 是否已经领取过了
	if (m_pFBData->byIsBuyStarGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]已经领取过星数商品!", __FUNCTION__);
		return;
	}

	// 取星数商品配置
	const TowerStarGoods_Config *pStarGoods = CConfigManager::Instance()->GetTowerStarGoods(GetTotalStar());
	if (!pStarGoods || !pStarGoods->vGoodsList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到爬塔星数商品或列表为空[star:%d]", __FUNCTION__, GetTotalStar());
		return;
	}

	// 检测索引合法性
	if (m_pFBData->byStarGoodsIndex >= pStarGoods->vGoodsList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔星数商品索引越界[GoodsIndex:%d]", __FUNCTION__, m_pFBData->byStarGoodsIndex);
		return;
	}

	const TowerStarGoods_Config::StarGoods &tGoods = pStarGoods->vGoodsList[m_pFBData->byStarGoodsIndex];

	// 检测消耗
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < tGoods.nPrice)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买爬塔星数商品钻石不足", __FUNCTION__);
		return;
	}

	// 消耗
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -tGoods.nPrice, TRUE, FROM_TOWER_BUY_STAR_GOODS);

	// 发放商品
	m_pPlayer->AddObject(tGoods.nId, tGoods.wNum, TRUE, FROM_TOWER_BUY_STAR_GOODS);

	// 修改标志(已领取)
	m_pFBData->byIsBuyStarGoods = true;

	MSG_S2C_TOWER_BUY_STAR_GOODS_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 请求爬塔普通副本数据
void CTowerFB::OnTowerFBData()
{
	SendTowerData();
}

// 一键扫荡
void CTowerFB::OnNetOneKeyClean(bool bIsToHistoryMap)
{
	// 检测VIP及角色等级
	if (!m_pPlayer->GetVipValue(VIP_TOWER_MAP_CLEAN) && m_pPlayer->GetLevel() < g_GetConfigValue(90))
	{
		MAPLOG(ERROR_LEVEL, "[%s]VIP及角色等级都不足,不能扫荡", __FUNCTION__);
		return;
	}

	// 准备挑战的关卡
	WORD wNextMapLevel = m_pFBData->wCurMapLevel + 1;

	// 取关卡配置
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(wNextMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通爬塔关卡配置[MapLevel:%d]", __FUNCTION__, wNextMapLevel);
		return;
	}

	// 当前章节不能扫荡
	if (pTowerMapCfg->wChapter > m_pFBData->byKeepThreeStarPassedRecord)
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔当前章节不能扫荡[MapLevel:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
		return;
	}

	const TowerChapter_Config *pChapterCfg = g_pCfgMgr->GetTowerChapter(pTowerMapCfg->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通爬塔章节配置[ChapterIdx:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
		return;
	}

	BYTE byBegin = m_pFBData->wCurMapLevel;
	BYTE byEnd = m_pFBData->wCurMapLevel;
	for (WORD i = m_pFBData->wCurMapLevel; i < pChapterCfg->vMapList[pChapterCfg->vMapList.size() - 1]; ++i)
	{
		if (!OnNormalBattleStart(2))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔一键扫荡处理开始战斗失败[ChapterIdx:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
			return;
		}
		if (!OnNormalBattleOver(true, true))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔一键扫荡处理结束战斗失败[ChapterIdx:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
			return;
		}

		byEnd++;
	}

	SendTowerData();

	MSG_S2C_TOWER_ONE_KEY_CLEAN_RESP msg;
	msg.m_byIsToHistoryMap = bIsToHistoryMap;
	msg.m_byBegin = byBegin;
	msg.m_byEnd = byEnd;
	m_pPlayer->SendPacket(&msg);
}


void CTowerFB::SendTowerData()
{
	MSG_S2C_TOWER_DATA_RESP msg;
	msg.m_FBData = *m_pFBData;
	m_pPlayer->SendPacket(&msg);
}

// 是否有红点
bool CTowerFB::IsHaveRedPoint()
{
	// 当前爬塔进行中，没有失败时
	if (!m_pFBData->byIsNeedReset)
		return true;
	
	// 当前爬塔有可有可免费重置次数时
	if (!m_pFBData->byTodayResetTimes)
		return true;

	return false;
}

// 取历史排行
WORD CTowerFB::GetMaxStarRecord()
{
	return m_pFBData->wStarMaxRecord;
}

// 取爬塔总星数
WORD CTowerFB::GetTotalStar()
{
	WORD wTotalStar = 0;
	for (WORD i = 0; i < m_pFBData->wCurMapLevel; ++i)
	{
		wTotalStar += m_pFBData->arMapStarList[i];
	}
	return wTotalStar;
}

// 随机属性
BYTE CTowerFB::RandProIdx(BYTE byStar)
{
	const TowerRandPro_Config *pProCfg = CConfigManager::Instance()->GetTowerRandPro(byStar);
	if (!pProCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到[%d]星随机属性配置", __FUNCTION__, byStar);
		return 0;
	}
	if (!pProCfg->vProList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到[%d]星随机属性配置为空", __FUNCTION__, byStar);
		return 0;
	}

	return Rand::Instance()->irand(0, pProCfg->vProList.size() - 1);
}

void CRebelFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	m_pFBManager = pFBMgr;
	m_pPlayer = pPlayer;
}

// 开始战斗
bool CRebelFB::OnBattleStart(DWORD dwRebelId, WORD wType)
{
	return CRebelManager::Instance()->OnBattleStart(m_pPlayer, dwRebelId, wType);
}

// 战斗结束
bool CRebelFB::OnBattleOver(BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	//DWORD dwRebelId = m_pFBManager->GetCurChapter();
	DWORD dwRebelId = m_pFBManager->GetTargetId();
	WORD wType = m_pFBManager->GetMapIdx();
	return CRebelManager::Instance()->OnBattleOver(m_pPlayer, dwRebelId, wType, pMonsterList, byMonsterNum);
}

// 取额外奖励的掉落物品数量
DWORD GetExtraRewardDropItemNum(const map<DWORD, int> &mItemInfoList, DWORD dwItemId, DWORD dwItemNum)
{
	DWORD dwExtraRewardNum = 0;
	map<DWORD, int>::const_iterator iter = mItemInfoList.find(dwItemId);
	if (iter != mItemInfoList.end())
	{
		dwExtraRewardNum = (dwItemNum * iter->second * PROPERTY_DECREASE_MULTIPLE);
	}
	return dwExtraRewardNum;
}

CFubenManager::CFubenManager()
{

}

CFubenManager::~CFubenManager()
{

}

void CFubenManager::Init(Player *pPlayer)
{
	m_byIsFirstWin = false;
	m_lStartBattleTime = 0;
	m_byFBType = BATTLE_TYPE_INVALID;
	m_pPlayer = pPlayer;
	m_pFubenInfo = &m_pPlayer->GetRoleFullInfo().tFubenInfo;
	m_MainFB.Init(pPlayer,this);
	m_HeroFB.Init(pPlayer, this);
	m_DailyFB.Init(pPlayer, this);
	m_RebelFB.Init(pPlayer, this);
	m_TowerFB.Init(pPlayer, this);
	m_EliteFB.Init(pPlayer, this);
}

void CFubenManager::OnNewDay()
{
	// 清空精灵副本外敌
	for (WORD i = 0; i < m_pFubenInfo->tEliteFBData.wChapter; ++i)
	{
		EliteFBChapter &tChapter = m_pFubenInfo->tEliteFBData.arChapterList[i];
		memset(&tChapter.tEnemyData, 0, sizeof(tChapter.tEnemyData));
	}
}

// 开始战斗 
void CFubenManager::OnBattleStart(BYTE byFBType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, DWORD dwClientPower, const char *pSign)
{
	MAPLOG(DEBUG_LEVEL, "[%s]请求开始战斗[RoleId:%d,FBType:%d,dwParam1:%d,dwParam2:%d,dwParam3:%d,dwParam4:%d,ClientPower:%u]", 
		__FUNCTION__, m_pPlayer->GetRoleId(), byFBType, dwParam1, dwParam2, dwParam3, dwParam4, dwClientPower);

	// mapidx 开始说明
	/*
	主线: 1
	精英 : 1
	英雄 : 1
	日常 : 0
	叛军 : 无
	普通爬塔 : 无
	精英爬塔 :
	领地 : 无
	夺宝 : 无
	竞技场 : 无
	将灵 : 无
	*/

	/*
	使用体力的模块：主线副本、精英副本、英雄副本
	使用精力的模块：夺宝、竞技场

	体力：获得经验=队伍等级*2*消耗体力
	获得金币=队伍等级*5*消耗体力+100

	精力：获得经验=队伍等级*2*消耗精力
	获得金币=队伍等级*10*消耗精力+200
	*/

	// 防止连续快速点击
	if (BATTLE_TYPE_INVALID != m_byFBType && GetTickCount64() <= m_lStartBattleTime + 2000)
	{
		m_pPlayer->SendErrorMsg(ERROR_IN_BATTLE, C2S_BATTLE_START_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家正在战斗中...[last_type:%d,cur_type:%d]",
			__FUNCTION__, m_byFBType, byFBType);
		return;
	}

	// 检测战力是否有问题 power + role_id + "HelloWorld"
	if (pSign)
	{
		char szBuff[128] = { 0 };
		sprintf_s(szBuff, sizeof(szBuff), "%d%d%s", dwClientPower, m_pPlayer->GetRoleId(), "HelloWorld");
		string strMd5Sign = MD5::Encode(szBuff).HexString();

		// 签名不对,有可能被改包
		if (strcmp(strMd5Sign.c_str(), pSign) != 0)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "战斗开始请求的签名异常[role_id:%d,server_sign:%s,client_sign:%s]",
				m_pPlayer->GetRoleId(), strMd5Sign.c_str(), pSign);
			return;
		}

		// 检测战力
		DWORD dwServerPower = m_pPlayer->GetFightingPower();
		if (!g_MapServer.CheckPowerIsValid(dwServerPower, dwClientPower))
		{
			// 封号处理
			m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "战斗开始请求的战力异常[role_id:%d,server_power:%d,client_power:%d]",
				m_pPlayer->GetRoleId(), dwServerPower, dwClientPower);
			return;
		}
	}

	m_byIsPassed = false;
	m_byIsFirstWin = false;
	bool bRet = false;
	m_MonsterList.clear();
	m_TargetHeroList.clear();
	m_vTargetSpriteList.clear();
	ResetBattleInfo();
	m_vHelpBattleHeroProList.clear();

	switch (byFBType)
	{
	case BATTLE_MAIN_MAP:
		bRet = m_MainFB.OnBattleStart(dwParam1, dwParam2, dwParam3);
		break;

	case BATTLE_ELITE_MAP:
		bRet = m_EliteFB.OnBattleStart(dwParam1, dwParam2);
		break;

	case BATTLE_HERO_MAP:
		bRet = m_HeroFB.OnBattleStart(dwParam1, dwParam2);
		break;

	case BATTLE_DAILY_MAP:
		bRet = m_DailyFB.OnBattleStart(dwParam1, dwParam2);
		break;

	case BATTLE_REBEL_MAP:
		bRet = m_RebelFB.OnBattleStart(dwParam1, dwParam2);
		break;

	case BATTLE_TOWER_NORMAL_MAP:
		bRet = m_TowerFB.OnNormalBattleStart(dwParam1);
		break;

	case BATTLE_TOWER_ELITE_MAP:
		bRet = m_TowerFB.OnEliteBattleStart(dwParam1);
		break;

	case BATTLE_LAND_MAP:
		bRet = CLandManager::Instance()->OnBattleStar(m_pPlayer, dwParam1);
		break;

	case BATTLE_DOUBAO_MAP:
		bRet = m_pPlayer->GetDuobaoManager()->BattleStar(dwParam1, dwParam2, dwParam3, dwParam4);
		break;

	case BATTLE_ARENA_MAP:
		bRet = CArenaManager::Instance()->OnBattleStart(m_pPlayer, dwParam1, dwParam2);
		break;

	case BATTLE_FAIRY_MAP:
		bRet = m_pPlayer->GetZhanBuManager()->OnBattleStart();
		break;

	case BATTLE_SCORE_MATCH:
		bRet = CScoreMatchManager::Instance()->OnBattleStart(m_pPlayer, dwParam1);
		break;

	case BATTLE_GUIDE_MAP:
		bRet = true;
		break;

	case BATTLE_WORLD_BOSS_MAP:
		bRet = CWorldBossManager::Instance()->OnBattleStart(m_pPlayer, dwParam1);
		break;

	case BATTLE_GUILD_MAP:
		bRet = CGuildManager::Instance()->OnBattleStart(m_pPlayer, dwParam1);
		break;

	case BATTLE_ATTACK_CITY:
		bRet = CAttackCityManager::Instance()->OnBattleStart(m_pPlayer, dwParam1);
		break;

	case BATTLE_ENDLESS_LAND:
		bRet = m_pPlayer->GetEndlessLandManager()->OnNetBattleStart();
		break;

	case BATTLE_TEAM_DUNGEON_MONSTER:
		bRet = CTeamDungeonManager::Instance()->OnMonsterBattleStart(m_pPlayer, dwParam1);
		break;

	case BATTLE_TEAM_DUNGEON_BOSS:
		bRet = CTeamDungeonManager::Instance()->OnBossBattleStart(m_pPlayer);
		break;

 	case BATTLE_KING_MATCH:
 		bRet = CKingMatchManager::Instance()->OnBattleStart(m_pPlayer, dwParam1);
 		break;

	case BATTLE_CARD_GROUP_MAP:
		bRet = m_pPlayer->GetZhanBuManager()->OnBattleStart();
		break;

	case BATTLE_RESOURCE_WAR:
		bRet = CResourceWarManager::Instance()->OnNetBattleStart(m_pPlayer, dwParam1, dwParam2);
		break;

	case BATTLE_GUILD_WAR_LOW_STAR:
		bRet = CGuildWarMgr::Instance()->OnNetLowStarBattleStart(m_pPlayer, dwParam1);
		break;

	case BATTLE_GUILD_WAR_HIGH_STAR:
		bRet = CGuildWarMgr::Instance()->OnNetHighStarBattleStart(m_pPlayer, dwParam1, dwParam2, dwParam3);
		break;

	default:
		MAPLOG(ERROR_LEVEL, "[%s]未处理的副本类型[type:%d]", __FUNCTION__, byFBType);
		break;
	}

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_START_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]请求开始战斗处理失败[type:%d,param1:%d,param2:%d]", 
			__FUNCTION__, byFBType, dwParam1, dwParam2);
		return;
	}

	// 计算好掉落的物品先缓存
	m_DropItemList.clear();
	if (m_dwMapId)
	{
		if (!MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_START_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]创建掉落物品失败[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}
	}

	SetBattleType(byFBType);
	m_lStartBattleTime = GetTickCount64();	// 开始战斗时间
	
	// 响应客户端进入战斗
	MSG_S2C_BATTLE_START_RESP msg;
	msg.m_byType = byFBType;
	msg.m_dwTargetFightingPower = GetTargetFightingPower();
	msg.m_dwParam1 = dwParam1;
	msg.m_dwParam2 = dwParam2;
	msg.m_dwParam3 = dwParam3;
	msg.m_dwParam4 = BATTLE_REBEL_MAP == byFBType ? m_dwMapId : dwParam4;
	msg.m_byMonsterDropNum = 0;
	msg.m_byMonsterNum = 0;
	msg.m_wTargetToBattleHeroNum = 0;
	msg.m_wHelpBattleHeroProNum = 0;
	msg.m_dwHelpBattleHeroInfoId = m_dwHelpBattleHeroInfoId;
	for (size_t i = 0; i < m_DropItemList.size() && i < MAX_MAP_DROP_NUM; ++i)
	{
		msg.m_DropItemList[msg.m_byMonsterDropNum++] = m_DropItemList[i];
	}
	for (size_t i = 0; i < m_MonsterList.size() && i < MAX_BATTLE_MONSTER_NUM; ++i)
	{
		msg.m_MonsterList[msg.m_byMonsterNum++] = m_MonsterList[i];
	}
	for (size_t i = 0; i < m_TargetHeroList.size() && i < MAX_BATTLE_HERO_NUM; ++i)
	{
		msg.m_TargetToBattleHeroList[msg.m_wTargetToBattleHeroNum++] = m_TargetHeroList[i];
	}
	for (size_t i = 0; i < m_vHelpBattleHeroProList.size(); ++i)
	{
		msg.m_vHelpBattleHeroProList[msg.m_wHelpBattleHeroProNum++] = m_vHelpBattleHeroProList[i];
	}

	m_pPlayer->SendPacket(&msg);
}

// 战斗结束
void CFubenManager::OnBattleOver(BYTE byFBType, int nWinValue, BYTE byStar, DWORD dwContext, DWORD dwContext1, MonsterData *pMonsterList,
	BYTE byMonsterNum, DWORD dwDamage, DWORD dwClientPower, const char *pSign)
{
	MAPLOG(DEBUG_LEVEL, "[%s]请求战斗结束[RoleId:%d,FBType:%d,nWinValue:%d,byStar:%d,dwContext:%d,dwContext1:%d,Damage%d,ClientPower:%d]",
		__FUNCTION__, m_pPlayer->GetRoleId(), byFBType, nWinValue, byStar, dwContext, dwContext1, dwDamage, dwClientPower);

	/*
	使用体力的模块：主线副本、精英副本、英雄副本
	使用精力的模块：夺宝、竞技场

	体力：获得经验=队伍等级*2*消耗体力
	获得金币=队伍等级*5*消耗体力+100

	精力：获得经验=队伍等级*2*消耗精力
	获得金币=队伍等级*10*消耗精力+200
	*/

	DWORD dwSignDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i){
		dwSignDamage += pMonsterList[i].dwDamage;
	}

	int nMin = 16978;
	int nMax = 78786969;

	BYTE byIsWin = false;

	// 胜利
	if (nWinValue >= nMin && nWinValue <= nMax)
	{
		byIsWin = true;
	}
	// 失败
	else
	{
		byIsWin = false;
	}

	// 如果是获胜则做验证  WinValue + role_id + power + damage + "HelloWorld"
	if (byIsWin)
	{	
		char szBuff[128] = { 0 };
		sprintf_s(szBuff, sizeof(szBuff), "%d%d%d%d%s", nWinValue, m_pPlayer->GetRoleId(), dwClientPower, dwDamage, "HelloWorld");
		string strMd5Sign = MD5::Encode(szBuff).HexString();

		// 签名不对,有可能被改包
		if (strcmp(strMd5Sign.c_str(), pSign) != 0)
		{
			// 封号处理
			//m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]战斗结束请求的签名异常[role_id:%d,server_sign:%s,client_sign:%s]", 
				__FUNCTION__, m_pPlayer->GetRoleId(), strMd5Sign.c_str(), pSign);
			return;
		}

		// 检测战力
		DWORD dwServerPower = m_pPlayer->GetFightingPower();
		if (!g_MapServer.CheckPowerIsValid(dwServerPower, dwClientPower))
		{
			// 封号处理
			m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]战斗结束请求的战力异常[role_id:%d,server_power:%d,client_power:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), dwServerPower, dwClientPower);
			return;
		}

		// 检测伤害
		if (g_MapServer.IsCheckPower() && dwDamage != dwSignDamage)
		{
			// 封号处理
			m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]战斗结束请求的伤害异常[role_id:%d,src_damage:%d,sign_damage:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), dwDamage, dwSignDamage);
			return;
		}

		// 检测战力
		const Map_Config *pMapCfg = m_dwMapId ? g_pCfgMgr->GetMap(m_dwMapId) : NULL;
		if (g_MapServer.IsCheckPower() && pMapCfg && dwServerPower < pMapCfg->dwNeedMinPower)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]战斗结束请求的伤害异常[role_id:%d,server_power:%d,need_min_power:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), dwServerPower, pMapCfg->dwNeedMinPower);
			return;
		}


		// 检测攻击次数
	}

	// 检测战斗类型是否匹配
	if (m_byFBType != byFBType)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]战斗开始与结束的类型不匹配[start_type:%d,over_type:%d]", 
			__FUNCTION__, m_byFBType, byFBType);
		return;
	}

	// 检测是否在挑战状态
	if (!CheckIsInBattle())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]木有在战斗中...[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	// todo::有可能使用了外挂
	if (GetTickCount64() < m_lStartBattleTime + g_GetConfigValue(140))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]战斗结果上报太快啦![type:%d,Interval:%d]", __FUNCTION__,
			byFBType, GetTickCount64() - m_lStartBattleTime);
		return;
	}

	m_byIsFirstWin = false;
	m_byIsMeKill = false;

	m_pPlayer->SetIsHaveSpriteTalentSkillReward(true);

	bool bRet = false;
	switch (byFBType)
	{
	case BATTLE_MAIN_MAP:
		bRet = m_MainFB.OnBattleOver(byIsWin, byStar);
		break;

	case BATTLE_ELITE_MAP:
		bRet = m_EliteFB.OnBattleOver(byIsWin, byStar, pMonsterList, byMonsterNum);
		break;

	case BATTLE_HERO_MAP:
		bRet = m_HeroFB.OnBattleOver(byIsWin, byStar);
		break;

	case BATTLE_DAILY_MAP:
		bRet = m_DailyFB.OnBattleOver(byIsWin, dwContext, dwContext1);
		break;

	// (特殊奖励)
	case BATTLE_REBEL_MAP:
		bRet = m_RebelFB.OnBattleOver(byIsWin, pMonsterList, byMonsterNum);
		break;

	// (特殊奖励)
	case BATTLE_TOWER_NORMAL_MAP:
		bRet = m_TowerFB.OnNormalBattleOver(byIsWin);
		break;

	// (特殊奖励)
	case BATTLE_TOWER_ELITE_MAP:
		bRet = m_TowerFB.OnEliteBattleOver(byIsWin);
		break;

	// 木有奖励
	case BATTLE_LAND_MAP:
		bRet = CLandManager::Instance()->OnBattleOver(m_pPlayer, m_wCurChapter, byIsWin);
		break;

	// (特殊奖励)
	case BATTLE_DOUBAO_MAP:
		bRet = m_pPlayer->GetDuobaoManager()->BattleOver(byIsWin);
		break;

	// (特殊奖励)
	case BATTLE_ARENA_MAP:
		bRet = CArenaManager::Instance()->OnBattleOver(m_pPlayer, byIsWin);
		break;

	// (特殊奖励)
	case BATTLE_FAIRY_MAP:
		bRet = m_pPlayer->GetZhanBuManager()->OnBattleOver(byIsWin);
		break;

	// (特殊奖励)
	case BATTLE_SCORE_MATCH:
		bRet = CScoreMatchManager::Instance()->OnBattleOver(m_pPlayer, byIsWin);
		break;

	case BATTLE_GUIDE_MAP:
		bRet = true;
		break;

	// (特殊奖励)
	case BATTLE_WORLD_BOSS_MAP:
		bRet = CWorldBossManager::Instance()->OnBattleOver(m_pPlayer, byIsWin, pMonsterList, byMonsterNum);
		break;

	case BATTLE_GUILD_MAP:
		bRet = CGuildManager::Instance()->OnBattleOver(m_pPlayer, pMonsterList, byMonsterNum);
		break;

	case BATTLE_ATTACK_CITY:
		bRet = CAttackCityManager::Instance()->OnBattleOver(m_pPlayer, pMonsterList, byMonsterNum, dwContext);
		break;

	case BATTLE_ENDLESS_LAND:
		bRet = m_pPlayer->GetEndlessLandManager()->OnNetBattleOver(byIsWin,dwContext);
		break;

	case BATTLE_TEAM_DUNGEON_MONSTER:
		bRet = CTeamDungeonManager::Instance()->OnMonsterBattleOver(m_pPlayer, byIsWin, pMonsterList, byMonsterNum);
		break;

	case BATTLE_TEAM_DUNGEON_BOSS:
		bRet = CTeamDungeonManager::Instance()->OnBossBattleOver(m_pPlayer, byIsWin, pMonsterList, byMonsterNum);
		break;

	case BATTLE_KING_MATCH:
		bRet = CKingMatchManager::Instance()->OnBattleOver(m_pPlayer, byIsWin);
		break;

	case BATTLE_CARD_GROUP_MAP:
		bRet = m_pPlayer->GetZhanBuManager()->OnBattleOver(byIsWin);
		break;

	case BATTLE_RESOURCE_WAR:
		bRet = CResourceWarManager::Instance()->OnNetBattleOver(m_pPlayer, byIsWin);
		break;

	case BATTLE_GUILD_WAR_LOW_STAR:
		bRet = CGuildWarMgr::Instance()->OnNetLowStarBattleOver(m_pPlayer, byIsWin, dwContext);
		break;

	case BATTLE_GUILD_WAR_HIGH_STAR:
		bRet = CGuildWarMgr::Instance()->OnNetHighStarBattleOver(m_pPlayer, byIsWin, dwContext);
		break;

	default:
		{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW,C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]未处理的副本类型[type:%d]", byFBType);
		}
		break;
	}

	m_pPlayer->SetIsHaveSpriteTalentSkillReward(false);

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]请求结束战斗处理失败[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	// 响应客户端战斗结束
	MSG_S2C_BATTLE_END_RESP msg;
	msg.m_wRewardItemNum = 0;

	// 处理统一奖励
	if (byIsWin)
	{
		const Map_Config *pMapCfg = m_dwMapId ? g_pCfgMgr->GetMap(m_dwMapId) : NULL;

		// 处理奖励
		if (pMapCfg)
		{
			map<DWORD, int> mItemInfoList;
			m_pPlayer->GetSpiritManager()->GetExtraRewardDropItemInfo(byFBType, mItemInfoList);

			// 奖励的物品列表(道具)
			for (size_t i = 0; i < pMapCfg->vRandRewardList.size(); ++i)
			{
				const RewardItem &tRewardItem = pMapCfg->vRandRewardList[i];
				Item item(tRewardItem.dwItemId, g_Rand(tRewardItem.wNumMin, tRewardItem.wNumMax));

				// 精灵天赋技能影响的额外奖励
				item.dwNum += GetExtraRewardDropItemNum(mItemInfoList, item.dwId, item.dwNum);

				m_pPlayer->AddObjectData(&item, TRUE, FROM_FB_NORMAL_REWARD);

				if (msg.m_wRewardItemNum < MAX_BATTLE_REWARD_ITEM_NUM)
					msg.m_arRewardItemList[msg.m_wRewardItemNum++] = item;
			}
		}

		// 首胜奖励
		if (m_byIsFirstWin && pMapCfg)
		{
			// 首胜奖励元宝
			if (pMapCfg->vFirstWinCurrencyList.size())
				m_pPlayer->ModifyPropertyList(pMapCfg->vFirstWinCurrencyList, TRUE, FROM_HERO_FB_BATTLE);

			// 首胜奖励物品
			if (pMapCfg->vFirstWinItemList.size())
				m_pPlayer->AddObjectList(pMapCfg->vFirstWinItemList, TRUE, FROM_FB_FIRST_WIN_REWARD);
		}

		// 限时奖励
		{
			// 随机奖励
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);

			// 添加奖励物品
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);

			// 添加奖励
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				if (msg.m_wRewardItemNum >= MAX_BATTLE_REWARD_ITEM_NUM)
					break;
				msg.m_arRewardItemList[msg.m_wRewardItemNum++] = vLimitRewardItemList[i];
			}
		}
	}
	else
	{
		// 
// 		if (BATTLE_MAIN_MAP != byFBType && m_dwMapId)
// 		{
// 			// 存储失败记录
// 			RoleMapLoseRecord_Save* pQuery = RoleMapLoseRecord_Save::ALLOC();
// 			pQuery->SetIndex(DB_ROLE_MAP_LOSE_RECORD_SAVE);
// 			pQuery->SetThreadIndex(DB_THREAD_COMMON);
// 			MapLoseRecord tRecord;
// 			tRecord.dwRoleId = m_pPlayer->GetRoleId();
// 			tRecord.dwMapId = m_dwMapId;
// 			tRecord.wType = byFBType;
// 			tRecord.wChapter = 0;
// 			tRecord.wMapIdx = 0;
// 			tRecord.dwFightingPower = m_pPlayer->GetFightingPower();
// 			pQuery->SetRecord(tRecord);
// 			g_MapServer.MapDBQuery(pQuery);
// 		}
	}

	ResetBattleInfo();


	msg.m_byIsWin = byIsWin;
	msg.m_byStar = byStar;
	msg.m_byIsFirstWin = m_byIsFirstWin;
	msg.m_byIsMeKill = m_byIsMeKill;
	msg.m_byParamNum = 0;
	for (BYTE i = 0; i < 5/*目前暂时只需要5个参数*/; ++i)
	{
		msg.m_arParamList[msg.m_byParamNum++] = GetParam(i);
		//MAPLOG(ERROR_LEVEL, "arParamList[index:%d,value:%d]", i, GetParam(i));
	}
	m_pPlayer->SendPacket(&msg);
}

// 领取副本奖励
void CFubenManager::OnNetTakeReward(BYTE byAwardType, BYTE byFBType, WORD wChapter, BYTE byMapIdx)
{
	bool bResult = false;
	if (BATTLE_MAIN_MAP == byFBType)
	{
		bResult = m_MainFB.OnTakeReward(byAwardType, wChapter, byMapIdx);
	}
	else if (BATTLE_ELITE_MAP == byFBType)
	{
		bResult = m_EliteFB.OnTakeReward(byAwardType, wChapter, byMapIdx);
	}
	else if (BATTLE_HERO_MAP == byFBType)
	{
		bResult = m_HeroFB.OnTakeReward(byAwardType, wChapter, byMapIdx);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_AWARD_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]没处理的领取副本奖励类型[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_AWARD_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]领取副本奖励失败[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	MSG_S2C_FB_AWARD_GET_RESP msg;
	msg.m_byAwardType = byAwardType;
	msg.m_byFBType = byFBType;
	msg.m_wChapter = wChapter;
	msg.m_byIndex = byMapIdx;
	m_pPlayer->SendPacket(&msg);
}

// 扫荡
void CFubenManager::OnNetSweep(BYTE byFBType, WORD wChapter, BYTE byMapIdx)
{
	MSG_S2C_FB_CLEAN_RESP msg;
	msg.m_dwParam1 = wChapter;
	msg.m_dwParam2 = byMapIdx;
	msg.m_byResult = 0;
	msg.m_byFBType = byFBType;
	m_byResult = 0;

	// 主线副本
	if (BATTLE_MAIN_MAP == byFBType)
	{
		// 检测VIP等级
		if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < m_pPlayer->GetVipValue(VIP_MAIN_MAP_CLEAN))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]主线副本扫荡受VIP等级限制", __FUNCTION__);
			return;
		}

		// 开始战斗逻辑
		m_DropItemList.clear();
		if (!m_MainFB.OnBattleStart(wChapter, byMapIdx, 0))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]处理副本扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// 处理掉落物品
		if (m_dwMapId && !MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]创建掉落物品失败[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}
		
		// 填充掉落物品
		vector<MonsterDropData>::iterator iter = m_DropItemList.begin();
		for (; iter != m_DropItemList.end(); ++iter)
		{
			msg.m_vDropItemList.push_back(Item(iter->dwObjId, iter->wObjNum));
		}

		// 处理限时掉落物品
		{
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);
			// 添加奖励物品
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);
			// 填充限时掉落物品
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				msg.m_vDropItemList.push_back(vLimitRewardItemList[i]);
			}
		}


		// 结束战斗逻辑
		if (!m_MainFB.OnBattleOver(true, 3))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]结束副本扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		msg.m_byResult = m_byResult;

		// 暂时这么处理
		m_MainFB.SendFBData();
	}
	// 精英副本
	else if (BATTLE_ELITE_MAP == byFBType)
	{
		// 检测关卡下标
		if (!byMapIdx)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]精英副本关卡下标错误,扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// 开始战斗逻辑
		m_DropItemList.clear();
		if (!m_EliteFB.OnBattleStart(wChapter, byMapIdx))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]处理精英副本扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// 处理掉落物品
		if (m_dwMapId && !MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]创建精英副本掉落物品失败[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}

		// 填充掉落物品
		vector<MonsterDropData>::iterator iter = m_DropItemList.begin();
		for (; iter != m_DropItemList.end(); ++iter)
		{
			msg.m_vDropItemList.push_back(Item(iter->dwObjId, iter->wObjNum));
		}

		// 处理限时掉落物品
		{
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);
			// 添加奖励物品
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);
			// 填充限时掉落物品
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				msg.m_vDropItemList.push_back(vLimitRewardItemList[i]);
			}
		}

		// 结束战斗逻辑
		if (!m_EliteFB.OnBattleOver(true, 3, NULL, 0))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]结束精英副本扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		msg.m_byResult = m_byResult;

		// 暂时这么处理
		m_EliteFB.SendFBData();
	}
	// 日常关卡
	else if (BATTLE_DAILY_MAP == byFBType)
	{
		// 检测关卡下标
		if (!byMapIdx)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]日常副本关卡下标错误,扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// 开始战斗逻辑
		m_DropItemList.clear();
		if (!m_DailyFB.OnBattleStart(wChapter, byMapIdx))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]处理日常副本扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// 处理掉落物品
		if (m_dwMapId && !MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]创建日常副本掉落物品失败[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}

		// 填充掉落物品
		vector<MonsterDropData>::iterator iter = m_DropItemList.begin();
		for (; iter != m_DropItemList.end(); ++iter)
		{
			msg.m_vDropItemList.push_back(Item(iter->dwObjId, iter->wObjNum));
		}

		// 处理限时掉落物品
		{
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);
			// 添加奖励物品
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);
			// 填充限时掉落物品
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				msg.m_vDropItemList.push_back(vLimitRewardItemList[i]);
			}
		}

		// 取章节配置
		const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetDailyChapter(wChapter);
		if (!pChapterCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到日常副本章节副本[chapter:%d]", __FUNCTION__, wChapter);
			return;
		}

		// 结束战斗逻辑
		if (!m_DailyFB.OnBattleOver(true, pChapterCfg->dwSweepRewardContext, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]结束日常副本扫荡失败[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		msg.m_byResult = m_byResult;

		// 暂时这么处理
		m_DailyFB.SendDailyData();
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无法识别的副本扫荡类型[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	m_pPlayer->SendPacket(&msg);
}

// 1. on your connection string, make sure that you're using the SERVER'S correct IP.On the other hand, you can use the domain name, and this is what I do.
// 2. The rest of the parameters are the same.I mean they're what you want or have set. 
// 3. NOW the critical thing.First you've got to tell your web server that you need to accept remote connections. 
// You can easily do this using the cpanel. When you're asked for the remote host name, type in % (the percentage sign) to mean ANY HOST.
// 4. Finally, you may have to tell the web server's firewall that you're making a remote connection from your machine.To do this, 
// cpanel allows you to add your own IP into a 'white list'.In most cases, cpanel shows you a list of class C IPs and your one should be there.Select it.Save the settings.

// 副本重置
void CFubenManager::OnNetFBReset(BYTE byFBType, DWORD dwParam1, DWORD dwParam2)
{
	bool bResult;
	switch (byFBType)
	{
	// 主线关卡
	case BATTLE_MAIN_MAP:
		bResult = m_MainFB.OnResetBattleTimes(dwParam1, dwParam2);
		break;

	// 爬塔普通关卡
	case BATTLE_TOWER_NORMAL_MAP:
		bResult = m_TowerFB.OnNetFBReset(false);
		break;

	// 精英关卡
	case BATTLE_ELITE_MAP:
		bResult = m_EliteFB.OnResetBattleTimes(dwParam1, dwParam2);
		break;

	default:
		{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW,C2S_FB_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的副本重置类型[type:%d]", __FUNCTION__, byFBType);
		}
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]副本重置处理失败[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	MSG_S2C_FB_RESET_RESP msg;
	msg.m_byMapType = byFBType;
	msg.m_dwParam1 = dwParam1;
	msg.m_dwParam2 = dwParam2;
	m_pPlayer->SendPacket(&msg);
}

// 一键领取
void CFubenManager::OnNetOneKeyTakeReward(BYTE byFBType, WORD wChapter)
{
	bool bResult;
	switch (byFBType)
	{
		// 主线关卡
	case BATTLE_MAIN_MAP:
		bResult = m_MainFB.OnNetOneKeyTakeReward(wChapter);
		break;

		// 精英关卡
	case BATTLE_ELITE_MAP:
		bResult = m_EliteFB.OnNetOneKeyTakeReward(wChapter);
		break;

	default:
		{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW,C2S_FB_ONE_KEY_TAKE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的副本一键领取类型[type:%d]", __FUNCTION__, byFBType);
		}
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_ONE_KEY_TAKE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]副本一键领取处理失败[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	// 响应
	MSG_S2C_FB_ONE_KEY_TAKE_REWARD_RESP msg;
	msg.m_byFBType = byFBType;
	msg.m_wChapter = wChapter;
	m_pPlayer->SendPacket(&msg);
}

// 添加掉落对象
bool CFubenManager::AddDropObject(DWORD dwMonsterId, DWORD dwDropObjectId, WORD wDropObjectNum)
{
	for (int i = 0; i < m_DropItemList.size(); ++i)
	{
		if (m_DropItemList[i].dwObjId == dwDropObjectId)
		{
			m_DropItemList[i].wObjNum += wDropObjectNum;
			return true;
		}
	}

	MonsterDropData sDropData;
	sDropData.dwMonsterId = dwMonsterId;
	sDropData.byType = 0;
	sDropData.dwObjId = dwDropObjectId;
	sDropData.wObjNum = wDropObjectNum;
	m_DropItemList.push_back(sDropData);
	return true;
}

// 添加掉落对象给玩家
bool CFubenManager::AddDropOjectToPlayer(WORD wMapType)
{
	int nMultiple = CActivityManager::GetLimitProductMultiple(wMapType);

	for (int i = 0; i < m_DropItemList.size(); ++i)
	{
		MonsterDropData *pDropData = &m_DropItemList[i];
		m_pPlayer->AddObject(pDropData->dwObjId, pDropData->wObjNum * nMultiple, TRUE, FROM_FB_DROP_ITEM);
	}

	MAPLOG(GUI_LEVEL, "[%s]关卡掉落物品数量[map_id:%d,num:%d]", __FUNCTION__,m_dwMapId, m_DropItemList.size());

	return true;
}



// 处理掉落物品
bool CFubenManager::MonsterDropItem(WORD wMapId, WORD wMapType)
{
	map<DWORD, int> mItemInfoList;
	m_pPlayer->GetSpiritManager()->GetExtraRewardDropItemInfo(wMapType, mItemInfoList);

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		// 找到主线副本关卡配置
		MAPLOG(ERROR_LEVEL, "[%s]找不到关卡配置[MapId:%d]",__FUNCTION__, wMapId);
		return false;
	}
	m_DropItemList.clear();

	// 关卡掉落
	for (size_t i = 0; i < pMapCfg->vDropList.size(); ++i)
	{
		const DropData *pDropItem = &pMapCfg->vDropList[i];
		if (g_Rand() < pDropItem->wRate)
		{
			MonsterDropData tDropData;
			tDropData.dwObjId = pDropItem->dwDropObjectId;
			tDropData.wObjNum = pDropItem->wDropObjectNum;
			tDropData.dwMonsterId = 0;

			// 精灵天赋技能影响的额外奖励
			tDropData.wObjNum += GetExtraRewardDropItemNum(mItemInfoList, tDropData.dwObjId, tDropData.wObjNum);

			m_DropItemList.push_back(tDropData);
		}
	}

	// 怪物身上掉落
	for (int i = 0; i < pMapCfg->vBattleList.size(); ++i)
	{
		DWORD dwBattleId = pMapCfg->vBattleList[i];
		const Battle_Config *pBattleCfg = CConfigManager::Instance()->GetMapBattle(dwBattleId);
		if (!pBattleCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到战役配置数据[BattleId:%d]",__FUNCTION__, dwBattleId);
			return false;
		}

		for (int m = 0; m < MAX_BATTLE_MONSTER_NUM; ++m)
		{
			DWORD dwMonsterId = pBattleCfg->arMonsterList[m];
			if (!dwMonsterId)
				continue;

			const Monster_Config *pMonsterCfg = CConfigManager::Instance()->GetMonster(dwMonsterId);
			if (!pMonsterCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到怪物配置[MonsterId:%d]",__FUNCTION__, dwMonsterId);
				return false;
			}

			for (int k = 0; k < pMonsterCfg->vDropList.size(); ++k)
			{
				const DropData &tDropData = pMonsterCfg->vDropList[k];
				if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < tDropData.wRate)
				{
					int nDropNum = tDropData.wDropObjectNum;

					// 精灵天赋技能影响的额外奖励
					nDropNum += GetExtraRewardDropItemNum(mItemInfoList, tDropData.dwDropObjectId, nDropNum);

					AddDropObject(dwMonsterId, tDropData.dwDropObjectId, nDropNum);
				}
			}
		}
	}

	return true;
}

// 处理限时掉落
void CFubenManager::DoLimitDrop(DWORD dwFBType, vector<Item> &vRewardItemList)
{
	// 限时奖励
	const vector<LimitDrop_Config> *pLimitDropList = g_pCfgMgr->GetLimitDropList(dwFBType);
	if (!pLimitDropList)
		return;

	DWORD dwCurDate = g_Time2Date(time(NULL));

	vector<LimitDrop_Config>::const_iterator iter = pLimitDropList->begin();
	for (; iter != pLimitDropList->end(); ++iter)
	{
		// 检测是否在指定时间内
		if (dwCurDate >= iter->dwStartDate && dwCurDate <= iter->dwOverDate)
		{
			// 随机是否会掉落
			if (g_Rand() > iter->dwDropRate)
				continue;

			// 随机数量
			DWORD dwItemNum = g_Rand(iter->dwDropNumMin, iter->dwDropNumMax);

			CItemManager::UseRandItem(iter->dwDropItemId, dwItemNum, vRewardItemList);
		}
	}
}

// 添加怪物数据
void CFubenManager::AddMonsterData(const MonsterData &tMonster)
{
	m_MonsterList.push_back(tMonster);
}

// 添加英雄数据
void CFubenManager::SetBattleTargetData(const vector<ToBattleHeroData> &vTargetHeroList, const vector<SpiritData> &vTargetSpriteList)
{
	m_TargetHeroList = vTargetHeroList;
	m_vTargetSpriteList = vTargetSpriteList;
}

// 检测是否在战斗中
bool CFubenManager::CheckIsInBattle()
{
	// 检测是否在挑战状态
	if (!m_lStartBattleTime || GetTickCount64() > m_lStartBattleTime + 1000 * 60 * 30)
	{
		return false;
	}
	return true;
}

// 设置战斗信息
void CFubenManager::SetBattleInfo(WORD wChapter, BYTE byMapLv, DWORD dwMapId, DWORD dwHelpBattleTargetId, DWORD dwHelpBattleHeroInfoId)
{
	m_byFBType = BATTLE_TYPE_INVALID;	// 当前挑战的副本类型
	m_wCurChapter = wChapter;			// 当前挑战的章节
	m_byCurMapIdx = byMapLv;				// 当前挑战的等级
	m_dwMapId = dwMapId;
	m_lStartBattleTime = GetTickCount64();	// 开始战斗时间
	m_dwHelpBattleTargetId = dwHelpBattleTargetId;
	m_dwHelpBattleHeroInfoId = dwHelpBattleHeroInfoId;
}

void CFubenManager::ResetBattleInfo()
{
	m_byFBType = 0;
	m_wCurChapter = 0;
	m_byCurMapIdx = 0;
	m_lStartBattleTime = 0;
	m_dwMapId = 0;
	m_DropItemList.clear();
	m_MonsterList.clear();
	m_TargetHeroList.clear();
	m_vTargetSpriteList.clear();
	m_dwHelpBattleTargetId = 0;
	m_dwHelpBattleHeroInfoId = 0;
}

void CFubenManager::SetParam(BYTE byIndex, DWORD dwParam)
{ 
	if (byIndex >= MAX_BATTLE_PARAM_NUM)
		return;

	m_arParamList[byIndex] = dwParam; 
}

DWORD CFubenManager::GetParam(BYTE byIndex)
{ 
	if (byIndex >= MAX_BATTLE_PARAM_NUM)
		return 0;

	return m_arParamList[byIndex];
}

// 处理通用奖励
void CFubenManager::DoWinReward(int nMapType, const Map_Config *pMapCfg)
{
	WORD wFromId = 0;
	if (BATTLE_MAIN_MAP == nMapType)
		wFromId = FROM_MAIN_FB_BATTLE;
	else if (BATTLE_ELITE_MAP == nMapType)
		wFromId = FROM_ELITE_FB_BATTLE;
	else if (BATTLE_HERO_MAP == nMapType)
		wFromId = FROM_HERO_FB_BATTLE;
	else if (BATTLE_DOUBAO_MAP == nMapType)
		wFromId = FROM_DOUBAO;
	else if (BATTLE_ARENA_MAP == nMapType)
		wFromId = FROM_ARENA_BATTLE;
	else if (BATTLE_DAILY_MAP == nMapType)
		wFromId = FROM_DAILY_FB_BATTLE;

	WORD wLevel = m_pPlayer->GetLevel();

	// 主线副本、精英副本、英雄副本
	if (BATTLE_MAIN_MAP == nMapType || BATTLE_ELITE_MAP == nMapType || BATTLE_HERO_MAP == nMapType)
	{
		// 获得经验 = 队伍等级*2*消耗体力
		// 获得金币 = 队伍等级 * 5 * 消耗体力 + 100
		int nCostStamina = pMapCfg->GetCostValue(ROLE_PRO_STAMINA);
		DWORD dwExp = wLevel * 2 * nCostStamina;
		int nGold = wLevel * 5 * nCostStamina + 100;

		// todo::副本通关奖励的金币*10 996bt
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
			nGold *= 10;

		m_pPlayer->AddExp(dwExp);
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, wFromId);
	}
	// 夺宝、竞技场
	else if (BATTLE_DOUBAO_MAP == nMapType || BATTLE_ARENA_MAP == nMapType)
	{
		// 获得经验 = 队伍等级*2*消耗精力
		// 获得金币 = 队伍等级 * 10 * 消耗精力 + 200
		int nCostEnergy = pMapCfg->GetCostValue(ROLE_PRO_ENERGY);
		DWORD dwExp = wLevel * 2 * nCostEnergy;
		INT nGold = wLevel * 10 * nCostEnergy + 200;

		// todo::副本通关奖励的金币*10 996bt
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
			nGold *= 10;

		m_pPlayer->AddExp(dwExp);
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, wFromId);
	}
	// 日常副本
	else if (BATTLE_DAILY_MAP == nMapType)
	{
		
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的关卡获得奖励[MapType:%d]", __FUNCTION__, nMapType);
		return;
	}

	// 普通货币奖励
	m_pPlayer->ModifyPropertyList(pMapCfg->vNormalRewardList, TRUE, wFromId);

	// 添加掉落物品
	AddDropOjectToPlayer(nMapType);
}

// 添加助阵英雄属性
void CFubenManager::AddHelpBattleHeroPro(const Property &tPro)
{
	m_vHelpBattleHeroProList.push_back(tPro);
}