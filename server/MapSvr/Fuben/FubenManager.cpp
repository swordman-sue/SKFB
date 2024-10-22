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

// ��ʼս��
bool CMainFB::OnBattleStart(WORD wChapter, BYTE byMapIdx, DWORD dwHelpBattleTargetId)
{
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �����Ҫ�Ľ�ɫ�ȼ��Ƿ��㹻
	if (m_pPlayer->GetLevel() < pChapterCfg->dwNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���߹ؿ���ս��Ҫ�Ľ�ɫ�ȼ�����[Chapter:%d, CurLevel:%d, NeedLevel:%d]!", 
			__FUNCTION__, wChapter, m_pPlayer->GetLevel(), pChapterCfg->dwNeedRoleLevel);
		return false;
	}

	WORD wMapId = pChapterCfg->GetMapId(byMapIdx);

	// ȡ����
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����ؿ�����[chapter:%d,MapId:%d]",__FUNCTION__, wChapter, wMapId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]���߹ؿ���ս���Ĳ���!", __FUNCTION__);
		return false;
	}

	// �������
// 	if (m_pPlayer->GetProperty(ROLE_PRO_STAMINA) < pMapCfg->nCostStamina)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]���߸����ؿ���ս��������!", __FUNCTION__);
// 		return false;
// 	}

	// ȡ�½�����
	ChapterData *pChapterData = GetChapterData(wChapter);
	if (!pChapterData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵ����߸����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// Խ�½���ս
	if (wChapter > GetNextChapter())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���߸���Խ�½���ս!", __FUNCTION__);
		return false;
	}

	// Խ�ؿ���ս
	if (GetNextChapter() == wChapter && byMapIdx > GetNextMapIndex())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���߸���Խ�ؿ���ս!", __FUNCTION__);
		return false;
	}

	// ��⵱ǰ��ս����
	if (pChapterData->arMapList[byMapIdx - 1].byBattleTimes >= pMapCfg->byDailyBattleTimesLimit)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���߸����ؿ���������ս��!", __FUNCTION__);
		return false;
	}

	// ��սӢ��ID
	DWORD dwHelpBattleHeroInfoId = 0;

	// �����ѡ����ս 
	if (dwHelpBattleTargetId)
	{
		// �����ս����(ֻ�к��Ѳ���Ҫ���)
// 		FriendData *pFriend = m_pPlayer->GetFriendManager()->GetFriendData(dwHelpBattleTargetId);
// 		if (pFriend)
// 		{
// 			if (pFriend->wHelpBattleTimes >= g_GetConfigValue(122))
// 			{
// 				MAPLOG(ERROR_LEVEL, "[%s]���ѵ���ս�����Ѵ�����[FriendId:%d]", __FUNCTION__, dwHelpBattleTargetId);
// 				return false;
// 			}
// 			pFriend->wHelpBattleTimes++;
// 		}

		bool bIsNeedCheck = false;

		// ����Ǻ���
		if (m_pPlayer->GetFriendManager()->IsFriend(dwHelpBattleTargetId))
		{
			bIsNeedCheck = true;
		}

		// ����Ǿ��Ŷ���
		if (!bIsNeedCheck)
		{
			DWORD dwMyGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(m_pPlayer->GetRoleId());
			if (dwMyGuildId && dwMyGuildId == CGuildManager::Instance()->GetGuildIdByRoleId(dwHelpBattleTargetId))
				bIsNeedCheck = true;
		}

		// ����Ǻ��ѣ����߾�����Ķ���,����Ҫ�����ս����
		if (bIsNeedCheck)
		{
			// �����ս�ܴ���
			if (m_pPlayer->GetProperty(ROLE_PRO_TODAY_HELP_BATTLE_TIMES) >= g_GetConfigValue(272))
			{
				MAPLOG(ERROR_LEVEL, "[%s]������ս������ʹ����", __FUNCTION__);
				return false;
			}
			m_pPlayer->ModifyProperty(ROLE_PRO_TODAY_HELP_BATTLE_TIMES, 1);
		}


		// ȡĿ��Ӣ������
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwHelpBattleTargetId);
		if (pTargetBaseData && pTargetBaseData->vToBattleHeroList.size())
		{
			// ȡ��һ��λ�õ�Ӣ��
			const ToBattleHeroData &tHeroData = pTargetBaseData->vToBattleHeroList[0];
			dwHelpBattleHeroInfoId = tHeroData.wHeroInfoId;
			for (BYTE i = 0; i < tHeroData.byProNum; ++i)
			{
				m_pFBManager->AddHelpBattleHeroPro(tHeroData.arPorList[i]);
			}
		}
	}

	// ��¼ս����Ϣ
	m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId, dwHelpBattleTargetId, dwHelpBattleHeroInfoId);

	// ��������ͳ��(�������ս�µĹؿ�) 
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
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½�����[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		// �ҵ����߸����ؿ�����
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ�����[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	// todo::�����׼��

	ChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��½�����[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ��������ͳ��(�������ս�µĹؿ�) 
	if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
	{
		CGameLogic::Instance()->OnStageOverStatistics(m_pPlayer, wChapter, byMapIdx, byIsWin);
	}

	// �������
	if (byIsWin)
	{
		// ������
		m_pFBManager->DoWinReward(BATTLE_MAIN_MAP, pMapCfg);

		// �۳�����
		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		// �������ս�µĹؿ�
		if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
		{
			m_pFBData->wChapter = wChapter;
			m_pFBData->arChapterList[wChapter - 1].byMapIdx = byMapIdx;

			// ���½���ս����
			if (byMapIdx >= pChapterCfg->vMapList.size())
			{
				// �ɾ��¼�
				m_pPlayer->OnUpdateAchievement(4, m_pFBData->wChapter, false);

				// 7���¼�(���߸���ͨ�ؽ���)
				m_pPlayer->OnUpdateSevenDayTarget(4, m_pFBData->wChapter, false);

				// ����㲥
				if (wChapter > g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).wChapterPassRecord)
				{
					g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).wChapterPassRecord = wChapter;
					g_MapServer.SaveGolbalData(m_pPlayer->GetServerId(),GOLBAL_DATA_CHAPTER_PASS_RECORD, wChapter);

					CSystemNotice sys_notice(8, m_pPlayer->GetServerId());
					sys_notice.AddParam(m_pPlayer->GetRoleName());
					sys_notice.AddParam(wChapter);
					sys_notice.SendMsg();
				}

				// ͨ�غ���
				if (wChapter == 3)
					m_pPlayer->GetActivityManager()->GetFacebookActivity().SetReadyToCommendTime(time(NULL));

				// ͨ�ط���
				if (wChapter == 2)
					m_pPlayer->GetActivityManager()->GetFacebookActivity().SetReadyToShareTime(time(NULL));
			}

			// �˹ؿ��״λ�ʤ
			m_pFBManager->SetIsFirstWin(true);

			// �洢�״�ͨ�ؼ�¼
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

			// �ɾ��¼�
			m_pPlayer->OnUpdateAchievement(3, wTotalStars, false);

			// ���а�
			CRankManager::Instance()->AddRoleRankData(RANK_MAIN_FB_STAR, m_pPlayer, wTotalStars);
		}
		
		// ��������Ѿ�
		if (!CRebelManager::Instance()->GetSelfRebelId(m_pPlayer->GetRoleId()))
		{
			if (CRebelManager::Instance()->CreateRebel(m_pPlayer))
				m_pFBManager->SetResult(1);
		}

		// �ճ�����
		m_pPlayer->OnUpdateDailyMission(1, 1, true);

		// 8-14���¼�(���߸���ͨ�ؽ���)
		m_pPlayer->OnUpdateSevenDayTarget(20, 1, true);

		// ���ջ
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 6, 1, true);

		// ���ջ�ԾĿ��
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 6, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 6, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 6, 1, true);

		// ����ȼ�����
		//m_pPlayer->OnUpdateWorldTaskEvent(1);
	}
 	else
 	{
 		// �洢ʧ�ܼ�¼
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

	// ����Ǻ�����ս
	DWORD dwHelpBattleTargetId = m_pFBManager->GetHelpBattleTargetId();
	if (dwHelpBattleTargetId && m_pPlayer->GetFriendManager()->IsFriend(dwHelpBattleTargetId))
	{
		int nHelpBattleFriendshipPoint = g_GetConfigValue(125);
		// �����߻�������
		if (m_pPlayer->GetProperty(ROLE_PRO_TODAY_FRIENDSHIP_PIONT) < g_GetConfigValue(123))
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_TODAY_FRIENDSHIP_PIONT, nHelpBattleFriendshipPoint);
			m_pPlayer->ModifyProperty(ROLE_PRO_FRIENDSHIP_PIONT, nHelpBattleFriendshipPoint);
		}

		// ��սĿ���������
		PlayerBaseData *pTargetBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseData(m_pPlayer->GetServerId(), dwHelpBattleTargetId, false);
		//const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwHelpBattleTargetId, false);
		if (pTargetBaseData)
		{
			// �������ͬһ������������
			if (!IsTheSameDay(pTargetBaseData->dwLastTakeFriendShipPointTime, time(NULL)))
			{
				pTargetBaseData->nTodayHelpBattleFriendShipPoint = 0;
			}

			// ���ջ������㻹û��������
			if (pTargetBaseData->nTodayHelpBattleFriendShipPoint < g_GetConfigValue(131))
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 11;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = nHelpBattleFriendshipPoint;
				strcpy_s(tRewardData.szText, sizeof(tRewardData.szText) - 1, m_pPlayer->GetRoleName());
				CRewardCenterManager::AddReward(m_pPlayer->GetServerId(), dwHelpBattleTargetId, tRewardData);

				// ����ÿ�ջ�õ�����㼰ʱ��
				pTargetBaseData->nTodayHelpBattleFriendShipPoint = pTargetBaseData->nTodayHelpBattleFriendShipPoint + nHelpBattleFriendshipPoint;
				pTargetBaseData->dwLastTakeFriendShipPointTime = time(NULL);
			}
		}
	}

	// todo::��ʱ��ô����
	SendFBData();

	return true;
}

// ���ø�����ս���� 
bool CMainFB::OnResetBattleTimes(WORD wChapter, BYTE byMapIdx)
{
	MapData *pMapData = GetMapData(wChapter, byMapIdx);
	if (!pMapData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ҫ���õĹؿ�����[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	// ������ô���
	if (pMapData->byResetTimes >= m_pPlayer->GetVipValue(VIP_MAIN_FB_RESET_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ҫ���õĹؿ������Ѵ�����[Chapter:%d,MapIdx:%d]", 
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(61, true);
	int nCostDiamond = pSvrCfg ? pSvrCfg->GetListValue(pMapData->byResetTimes) : 0;

	// ������ø�������
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������߸����ؿ�������ʯ����[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_MAIN_FB_RESET);

	pMapData->byBattleTimes = 0;
	pMapData->byResetTimes++;

	return true;
}

// ��ȡ�������� 
bool CMainFB::OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx)
{
	if (!wChapter || !byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������Ϸ�[chapter:%d,index:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	BYTE bySvrMapIdx = byMapIdx - 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	ChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ȡ�������½ڷǷ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �ؿ�����
	if (byRewardType == FB_AWARD_MAP)
	{
		if (bySvrMapIdx >= pChapter->byMapIdx)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ȡ�����Ĺؿ��Ƿ�[chapter:%d,map:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterCfg->vMapList[bySvrMapIdx]);
		if (!pMapCfg || !pMapCfg->vBoxList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ��������ݻ���ľ�����ñ��佱��[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		MapData &tMapData = pChapter->arMapList[bySvrMapIdx];

		// ����Ƿ��Ѿ���ȡ��
		if (tMapData.byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ȡ�����佱��[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		// ��ӽ���
		m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_LEVEL_REWARD);

		tMapData.byIsGetAward = true;
	}
	// ��������
	else
	{
		if (bySvrMapIdx >= CHAPTER_STAR_AWARD_MAX || bySvrMapIdx >= pChapterCfg->vStarAwardList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ȡ��������������[chapter:%d,index:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[bySvrMapIdx];

		// ����½ڵ������Ƿ��㹻
		if (pChapter->GetStar() < pStarAwardCfg->byStar)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�½�����������,������ȡ[chapter:%d,star:%d]",
				__FUNCTION__, wChapter, pChapter->GetStar());
			return false;
		}

		// ����Ƿ��Ѿ���ȡ��
		if (pChapter->arStarAwardList[bySvrMapIdx])
		{
			MAPLOG(ERROR_LEVEL, "[%s]�½�������������ȡ��[chapter:%d,index:%d]", __FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		// ������
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_STAR_REWARD);

		pChapter->arStarAwardList[bySvrMapIdx] = true;
	}
	return true;
}

// ���󸱱�����
void CMainFB::OnNetFBData()
{
	SendFBData();
}

// ���͸�������
void CMainFB::SendFBData()
{
	MSG_S2C_FB_MAIN_DATA_RESP msg;
	msg.m_Data = *m_pFBData;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ��к��
bool CMainFB::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		ChapterData *pChapter = &m_pFBData->arChapterList[i];

		WORD wTotalStar = pChapter->GetStar();

		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetMainFBChapter(i+1);
		if (!pChapterCfg || pChapterCfg->vStarAwardList.size() != CHAPTER_STAR_AWARD_MAX)
			continue;

		// �����߸������п���ȡ���Ǽ�����δ��ȡʱ
		for (BYTE k = 0; k < CHAPTER_STAR_AWARD_MAX; ++k)
		{
			// �����������ȡ,���һ�ľ����ȡ
			if (!pChapter->arStarAwardList[k] && wTotalStar >= pChapterCfg->vStarAwardList[k].byStar)
				return true;
		}

		// ���߸������п���ȡ �ľ�Ӣ����δ��ȡʱ
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

// ɨ��
void CMainFB::OnNetClean(WORD wChapter, BYTE byMapIdx)
{
	// ��ʼս��
	if (!OnBattleStart(wChapter, byMapIdx, 0))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ʼս��ʧ��[chapter:%d,MapIdx:%d]", __FUNCTION__, wChapter, byMapIdx);
		return;
	}

	// ս������
	if (!OnBattleOver(true, 3))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������ս��ʧ��[chapter:%d,MapIdx:%d]", __FUNCTION__, wChapter, byMapIdx);
		return;
	}

	// 8-14���¼�(���߸���ͨ�ؽ���)
	m_pPlayer->OnUpdateSevenDayTarget(20, 1, true);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 6, 1, true);

	// ���ջ�ԾĿ��
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 6, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 6, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 6, 1, true);

	// todo::��Ӧ
}


// һ����ȡ����(�ؿ�+����)
bool CMainFB::OnNetOneKeyTakeReward(WORD wChapter)
{
	if (!wChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�½ڲ������Ϸ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	ChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ȡ�������½ڷǷ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ����Ƿ���ͨ��
	if (pChapterCfg->vMapList.size() != pChapter->byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���½ڻ�ľ��ͨ��,������ȡ[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �����佱��
	for (BYTE i = 0; i < pChapter->byMapIdx; ++i)
	{
		MapData &tMap = pChapter->arMapList[i];
		// �Ѿ���ȡ��
		if (tMap.byIsGetAward)
			continue;

		// ȡ�ؿ�����
		DWORD dwMapId = pChapterCfg->GetMapId(i + 1);
		const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
		if (!pMapCfg)
			continue;

		// �б��佱��
		if (pMapCfg->vBoxList.size())
		{
			m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_ONE_KEY_REWARD);
			tMap.byIsGetAward = true;
		}
	}

	// ������������
	WORD wTotalStar = pChapter->GetStar();
	for (BYTE i = 0; i < CHAPTER_STAR_AWARD_MAX; ++i)
	{
		// �Ѿ���ȡ��
		if (pChapter->arStarAwardList[i])
			continue;

		// ��ȫ���,��ֹԽ��(todo::��Ӧ�ó�����������)
		if (i >= pChapterCfg->vStarAwardList.size())
			continue;

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[i];

		// ����½ڵ������Ƿ��㹻
		if (wTotalStar < pStarAwardCfg->byStar)
			continue;

		// ������
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_GET_MAINLINE_FB_ONE_KEY_REWARD);

		pChapter->arStarAwardList[i] = true;
	}

	return true;
}

// ��ȡ�½ڽ���
void CMainFB::OnNetTakeChapterReward(WORD wRewardLevel)
{
	// ȡ�½ڽ�������
	const MainFBChapterReward_Config *pRewardCfg = g_pCfgMgr->GetMainFBChapterReward(wRewardLevel);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½ڽ���[RewardLevel:%d]", __FUNCTION__, wRewardLevel);
		return;
	}

	// ��ľ�е��ﵱǰ�½�
	if (m_pFBData->wChapter < pRewardCfg->wNeedChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ľ��ͨ��ָ�����½�,������ȡ����[chapter:%d]",
			__FUNCTION__, pRewardCfg->wNeedChapter);
		return;
	}
	// �ѵ��ﵱǰ�½�(��⵱ǰ�½ڹؿ�)
	if (m_pFBData->wChapter == pRewardCfg->wNeedChapter)
	{
		// ȡ�½�����
		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetMainFBChapter(pRewardCfg->wNeedChapter);
		if (!pChapterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½�����[chapter:%d]",
				__FUNCTION__, pRewardCfg->wNeedChapter);
			return;
		}
		// ȡ����½�����
		ChapterData *pChapter = GetChapterData(pRewardCfg->wNeedChapter);
		if (!pChapter)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½���������[chapter:%d]",
				__FUNCTION__, pRewardCfg->wNeedChapter);
			return;
		}
		// ��ǰ�½ڵĹؿ���ľ����ȫͨ��
		if (pChapter->byMapIdx < pChapterCfg->vMapList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ľ��ͨ��ָ�����½�,������ȡ����[chapter:%d]",
				__FUNCTION__, pRewardCfg->wNeedChapter);
			return;
		}
	}

	// ����Ƿ�����ȡ��
	if (wRewardLevel <= m_pFBData->wTakeChapterRewardRecord)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���߸����½ڽ�������ȡ��[chapter:%d]",
			__FUNCTION__, pRewardCfg->wNeedChapter);
		return;
	}

	vector<Item> vItemList;

	// ������
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

	// ��Ӧ
	MSG_S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP msg;
	msg.m_wRewardLevel = wRewardLevel;
	msg.m_vItemList = vItemList;
	m_pPlayer->SendPacket(&msg);
}

// ȡ�ؿ�����
MapData* CMainFB::GetMapData(WORD wChapter, BYTE byMapIdx)
{
	ChapterData *pChapterData = GetChapterData(wChapter);
	if (pChapterData && byMapIdx <= pChapterData->byMapIdx)
	{
		return &pChapterData->arMapList[byMapIdx];
	}

	return NULL;
}

// ȡ�½�����
ChapterData* CMainFB::GetChapterData(WORD wChapter)
{
	if (!wChapter || wChapter > MAX_FUBEN_CHAPER_NUM)
		return NULL;

	return &m_pFBData->arChapterList[wChapter-1];
}

// ȡ��һ����ս���½�
WORD CMainFB::GetNextChapter()
{
	WORD wChapter = m_pFBData->wChapter;

	// Ĭ�ϴӵ�һ�¿�ʼ��
	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	// ���½��ѱ�������
	if (m_pFBData->arChapterList[wChapter - 1].byMapIdx >= pChapterCfg->vMapList.size())
		return wChapter + 1;

	return wChapter;
}

// ȡ��һ����ս�Ĺؿ�����
BYTE CMainFB::GetNextMapIndex()
{
	WORD wChapter = m_pFBData->wChapter;

	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetMainFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������߸����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	BYTE byMapIdx = m_pFBData->arChapterList[wChapter - 1].byMapIdx;

	// ���½��ѱ�������
	if (byMapIdx >= pChapterCfg->vMapList.size())
		return 1;

	return byMapIdx + 1;
}

// ȡ������
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


// ��Ӣ����

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

// ��ʼս��
bool CEliteFB::OnBattleStart(WORD wChapter, BYTE byMapIdx)
{
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ��ս��ͨ�ؿ�
	if (byMapIdx)
	{
		WORD wMapId = pChapterCfg->GetMapId(byMapIdx);

		// ȡ����
		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
		if (!pMapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�����ؿ�����[chapter:%d,MapId:%d]", __FUNCTION__, wChapter, wMapId);
			return false;
		}

		// �������
		if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ӣ�ؿ���ս���Ĳ���!", __FUNCTION__);
			return false;
		}

		// ȡ�½�����
		EliteFBChapter *pChapterData = GetChapterData(wChapter);
		if (!pChapterData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵľ�Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		// Խ�½���ս
		if (wChapter > GetNextChapter())
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ӣ����Խ�½���ս!", __FUNCTION__);
			return false;
		}

		// Խ�ؿ���ս
		if (GetNextChapter() == wChapter && byMapIdx > GetNextMapIndex())
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ӣ����Խ�ؿ���ս!", __FUNCTION__);
			return false;
		}

		// ��⵱ǰ��ս����
		if (pChapterData->arMapList[byMapIdx - 1].byBattleTimes >= pMapCfg->byDailyBattleTimesLimit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ӣ�����ؿ���������ս��!", __FUNCTION__);
			return false;
		}

		// ��¼ս����Ϣ
		m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId);
	}
	// ��ս��йؿ�
	else
	{
		// ȡ�½�����
		EliteFBChapter *pChapterData = GetChapterData(wChapter);
		if (!pChapterData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵľ�Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		if (!pChapterData->tEnemyData.wMapId)
		{
			m_pPlayer->SendErrorMsg(ERROR_ELITEFB_ENEMY_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]��ǰ��Ӣ�����½�ľ����п���ս[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		// ����Ƿ�������
		if (pChapterData->tEnemyData.wIsDead)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ǰ��Ӣ�����½�����ѹҵ�[chapter:%d]", __FUNCTION__, wChapter);
			return false;
		}

		// ȡ����
		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterData->tEnemyData.wMapId);
		if (!pMapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ������йؿ�����[chapter:%d,MapId:%d]", 
				__FUNCTION__, wChapter, pChapterData->tEnemyData.wMapId);
			return false;
		}

		// �������
		if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ӣ��йؿ���ս���Ĳ���!", __FUNCTION__);
			return false;
		}

// 		for (BYTE i = 0; i < pChapterData->tEnemyData.byMonsterNum; ++i)
// 		{
// 			m_pFBManager->AddMonsterData(pChapterData->tEnemyData.arMonsterList[i]);
// 		}

		// ��¼ս����Ϣ
		m_pFBManager->SetBattleInfo(wChapter, byMapIdx, pChapterData->tEnemyData.wMapId);
	}

	return true;
}

// ս������
bool CEliteFB::OnBattleOver(BYTE byIsWin, BYTE byStar, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	BYTE byMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�����½�����[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		// �ҵ����߸����ؿ�����
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�ؿ�����[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	// todo::�����׼��

	EliteFBChapter *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��½�����[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �������
	if (byIsWin)
	{
		// ��ͨ�ؿ�
		if (byMapIdx)
		{
			// ������
			m_pFBManager->DoWinReward(BATTLE_ELITE_MAP, pMapCfg);

			// �۳�����
			m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

			// �������ս�µĹؿ�
			if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
			{
				m_pFBData->wChapter = wChapter;
				m_pFBData->arChapterList[wChapter - 1].byMapIdx = byMapIdx;

				// �ɾ��¼�(ͨ�ؾ�Ӣ����)
				m_pPlayer->OnUpdateAchievement(6, m_pFBData->wChapter, false);

				// �˹ؿ��״λ�ʤ
				m_pFBManager->SetIsFirstWin(true);
			}

			MapData *pMapData = &pChapter->arMapList[byMapIdx - 1];
			pMapData->byBattleTimes++;
			if (byStar > pMapData->byStar)
			{
				pMapData->byStar = byStar;

				// �ɾ��¼�(��Ӣ�������Ǽ�)
				m_pPlayer->OnUpdateAchievement(5, GetTotalStar(), false);
			}

			// ��������Ѿ�
			if (!CRebelManager::Instance()->GetSelfRebelId(m_pPlayer->GetRoleId()))
			{
				if (CRebelManager::Instance()->CreateRebel(m_pPlayer))
					m_pFBManager->SetResult(1);
			}

			// �ճ�����
			m_pPlayer->OnUpdateDailyMission(15);

			// ���ջ(ÿ��ͨ�ؾ�Ӣ��������)
			m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 13, 1, true);

			// ���ջ�ԾĿ��(ÿ��ͨ�ؾ�Ӣ��������)
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 13, 1, true);

			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 13, 1, true);

			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 13, 1, true);
		}
	}

	// ���
	if (!byMapIdx)
	{
		if (byIsWin)
		{
			// ȡ����
			const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapter->tEnemyData.wMapId);
			if (!pMapCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ������йؿ�����[chapter:%d,MapId:%d]",
					__FUNCTION__, wChapter, pChapter->tEnemyData.wMapId);
				return false;
			}

			// ������
			m_pFBManager->DoWinReward(BATTLE_ELITE_MAP, pMapCfg);

			// �������
			m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

			pChapter->tEnemyData.wIsDead = true;

			// �ɾ��¼�(�ۼƻ�ɱ��Ӣ�������)
			m_pPlayer->OnUpdateAchievement(22);

			// �ճ�����(��ɱ��Ӣ���)
			m_pPlayer->OnUpdateDailyMission(20);
		}
	}

	// todo::��ʱ��ô����
	SendFBData();

	return true;
}

// ���ø�����ս����
bool CEliteFB::OnResetBattleTimes(WORD wChapter, BYTE byMapIdx)
{
	MapData *pMapData = GetMapData(wChapter, byMapIdx);
	if (!pMapData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ҫ���õĹؿ�����[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	// ������ô���
	if (pMapData->byResetTimes >= m_pPlayer->GetVipValue(VIP_ELITE_FB_RESET_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ҫ���õĹؿ������Ѵ�����[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(97, true);
	int nCostDiamond = pSvrCfg ? pSvrCfg->GetListValue(pMapData->byResetTimes) : 0;

	// ������ø�������
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���þ�Ӣ�����ؿ�������ʯ����[Chapter:%d,MapIdx:%d]",
			__FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_ELITE_FB_RESET);

	pMapData->byBattleTimes = 0;
	pMapData->byResetTimes++;

	return true;
}

// ��ȡ��������
bool CEliteFB::OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx)
{
	if (!wChapter || !byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������Ϸ�[chapter:%d,index:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	BYTE bySvrMapIdx = byMapIdx - 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	EliteFBChapter *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ȡ�������½ڷǷ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �ؿ�����
	if (byRewardType == FB_AWARD_MAP)
	{
		if (bySvrMapIdx >= pChapter->byMapIdx)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ȡ�����Ĺؿ��Ƿ�[chapter:%d,map:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterCfg->vMapList[bySvrMapIdx]);
		if (!pMapCfg || !pMapCfg->vBoxList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ��������ݻ���ľ�����ñ��佱��[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		MapData &tMapData = pChapter->arMapList[bySvrMapIdx];

		// ����Ƿ��Ѿ���ȡ��
		if (tMapData.byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ȡ�����佱��[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		// ��ӽ���
		m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_ELITE_FB_LEVEL_REWARD);

		tMapData.byIsGetAward = true;
	}
	// ��������
	else
	{
		if (bySvrMapIdx >= CHAPTER_STAR_AWARD_MAX)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ȡ��������������[chapter:%d,index:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[bySvrMapIdx];

		// ����½ڵ������Ƿ��㹻
		if (pChapter->GetStar() < pStarAwardCfg->byStar)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�½�����������,������ȡ[chapter:%d,star:%d]",
				__FUNCTION__, wChapter, pChapter->GetStar());
			return false;
		}

		// ����Ƿ��Ѿ���ȡ��
		if (pChapter->arStarAwardList[bySvrMapIdx])
		{
			MAPLOG(ERROR_LEVEL, "[%s]�½�������������ȡ��[chapter:%d,index:%d]",__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		// ��ӱ���
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_ELITE_FB_STAR_REWARD);

		pChapter->arStarAwardList[bySvrMapIdx] = true;
	}
	
	return true;
}

// ���󸱱�����
void CEliteFB::OnNetFBData()
{
	SendFBData();
}

// ��¼
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
 
 			// ��û����ˢ��ʱ��
 			if (nNowTime < nBeginTime)
 				continue;
 
 			// ��ˢ�¹�
 			if (m_pFBData->dwLastRefreshEnemyTime && nLastTime > nEndTime)
 				continue;
 
 			CreateEnemy();
 		}
 	}
 	m_pFBData->dwLastRefreshEnemyTime = time(NULL);
}

// �Ƿ��к��
bool CEliteFB::IsHaveRedPoint()
{
	// ����Ӣ������δ��ɱ����й���ʱ
	if (GetEnemyNum())
		return true;

	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		EliteFBChapter *pChapter = &m_pFBData->arChapterList[i];

		WORD wTotalStar = pChapter->GetStar();

		// ����½����ݵĺϷ���
		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetElitFBeChapter(i + 1);
		if (!pChapterCfg || pChapterCfg->vStarAwardList.size() != CHAPTER_STAR_AWARD_MAX)
			continue;

		// ����Ӣ�����п���ȡ���Ǽ�����ʱ
		for (BYTE k = 0; k < CHAPTER_STAR_AWARD_MAX; ++k)
		{
			// �����������ȡ,���һ�ľ����ȡ
			if (!pChapter->arStarAwardList[k] && wTotalStar >= pChapterCfg->vStarAwardList[k].byStar)
				return true;
		}

		// ����Ӣ�����п���ȡ�ľ�Ӣ����ʱ
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

// һ����ȡ����(�ؿ�+����)
bool CEliteFB::OnNetOneKeyTakeReward(WORD wChapter)
{
	if (!wChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�½ڲ������Ϸ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	EliteFBChapter *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ȡ�������½ڷǷ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ����Ƿ���ͨ��
	if (pChapterCfg->vMapList.size() != pChapter->byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���½ڻ�ľ��ͨ��,������ȡ[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �����佱��
	for (BYTE i = 0; i < pChapter->byMapIdx; ++i)
	{
		MapData &tMap = pChapter->arMapList[i];
		// �Ѿ���ȡ��
		if (tMap.byIsGetAward)
			continue;

		// ȡ�ؿ�����
		DWORD dwMapId = pChapterCfg->GetMapId(i + 1);
		const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
		if (!pMapCfg)
			continue;

		// �б��佱��
		if (pMapCfg->vBoxList.size())
		{
			m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_ELITE_FB_ONE_KEY_REWARD);
			tMap.byIsGetAward = true;
		}
	}

	// ������������
	WORD wTotalStar = pChapter->GetStar();
	for (BYTE i = 0; i < CHAPTER_STAR_AWARD_MAX; ++i)
	{
		// �Ѿ���ȡ��
		if (pChapter->arStarAwardList[i])
			continue;

		// ��ȫ���,��ֹԽ��(todo::��Ӧ�ó�����������)
		if (i >= pChapterCfg->vStarAwardList.size())
			continue;

		const StarAwardData *pStarAwardCfg = &pChapterCfg->vStarAwardList[i];

		// ����½ڵ������Ƿ��㹻
		if (wTotalStar < pStarAwardCfg->byStar)
			continue;

		// ������
		m_pPlayer->AddObjectList(pStarAwardCfg->vBoxList, TRUE, FROM_ELITE_FB_ONE_KEY_REWARD);

		pChapter->arStarAwardList[i] = true;
	}

	return true;
}

// �������
WORD CEliteFB::CreateEnemy()
{
	// ˢ��֮ǰ����Ѿ����������
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
		// ��������б� 
		vector<WORD> vRankChapterList;
		for (WORD i = pCurChapterCfg->wEnemyRandChapterMin; i <= pCurChapterCfg->wEnemyRandChapterMax; ++i)
		{
			vRankChapterList.push_back(i);
		}

		// ����������
		WORD wCreateNum = g_Rand(pCurChapterCfg->wEnemyRandNumMin, pCurChapterCfg->wEnemyRandNumMax);
		for (WORD i = 0; i < wCreateNum; ++i)
		{
			int nListNum = vRankChapterList.size();
			if (nListNum > 0)
			{
				// ����½�
				WORD wRandChapter = g_Rand(0, nListNum-1);
				CreateEnemy(wRandChapter);

				// �õ��Ѿ��浽���½�
				vRankChapterList[wRandChapter] = vRankChapterList[nListNum-1];
				vRankChapterList.pop_back();
			}
		}
	}
	m_pFBData->dwLastRefreshEnemyTime = time(NULL);

	return m_wNewEnemyNum;

}

// �������
void CEliteFB::CreateEnemy(WORD wChapter)
{
	// ��⵱ǰ�������(�������������)
// 	if (GetEnemyNum() >= 4)
// 		return;

	// ����Ƿ���ͨ��
	if (wChapter >= m_pFBData->wChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ��Ӣ�����½ڻ�ûͨ��,���ܴ������[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}

	EliteFBChapter &tChapterData = m_pFBData->arChapterList[wChapter];
	
	// ����Ƿ��Ѵ�����
	if (tChapterData.tEnemyData.wMapId)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]��ǰ��Ӣ�����½��Ѵ������[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}
	
	// ����Ƿ�����ͨ��
	BYTE byTotalStar = 0;
	for (BYTE i = 0; i < tChapterData.byMapIdx; ++i)
	{
		byTotalStar += tChapterData.arMapList[i].byStar;
	}
	if (byTotalStar < 12/*��ʱ��ô����*/)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]ľ�����ǵ��½ڲ��ܴ������[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter+1);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�����½���������[chapter:%d]", __FUNCTION__, wChapter+1);
		return;
	}
	
	// ������
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

// ȡ��ǰ�������
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

// ȡ�ؿ�����
MapData* CEliteFB::GetMapData(WORD wChapter, BYTE byMapIdx)
{
	EliteFBChapter *pChapterData = GetChapterData(wChapter);
	if (pChapterData && byMapIdx <= pChapterData->byMapIdx)
	{
		return &pChapterData->arMapList[byMapIdx];
	}

	return NULL;
}

// ȡ�½�����
EliteFBChapter* CEliteFB::GetChapterData(WORD wChapter)
{
	if (!wChapter || wChapter > MAX_FUBEN_CHAPER_NUM)
		return NULL;

	return &m_pFBData->arChapterList[wChapter - 1];
}

// ȡ��һ����ս���½�
WORD CEliteFB::GetNextChapter()
{
	WORD wChapter = m_pFBData->wChapter;

	// Ĭ�ϴӵ�һ�¿�ʼ��
	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	// ���½��ѱ�������
	if (m_pFBData->arChapterList[wChapter - 1].byMapIdx >= pChapterCfg->vMapList.size())
		return wChapter + 1;

	return wChapter;
}

// ȡ��һ����ս�Ĺؿ�����
BYTE CEliteFB::GetNextMapIndex()
{
	WORD wChapter = m_pFBData->wChapter;

	if (!wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetElitFBeChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return 1;
	}

	BYTE byMapIdx = m_pFBData->arChapterList[wChapter - 1].byMapIdx;

	// ���½��ѱ�������
	if (byMapIdx >= pChapterCfg->vMapList.size())
		return 1;

	return byMapIdx + 1;
}

// ȡ������
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

// �Ƿ���йҵ�
bool CEliteFB::IsEnemyDead(WORD wChapter)
{
// 	// ȡ�½�����
// 	EliteFBChapter *pChapterData = GetChapterData(wChapter);
// 	if (!pChapterData)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵľ�Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
// 		return false;
// 	}
// 
// // 	if (pChapterData->tEnemyData.wMapId)
// // 	{
// // 		MAPLOG(ERROR_LEVEL, "[%s]��ǰ��Ӣ�����½�ľ����п���ս[chapter:%d]", __FUNCTION__, wChapter);
// // 		return false;
// // 	}
// 
// 	// ����Ƿ�������
// 	for (BYTE i = 0; i < pChapterData->tEnemyData.byMonsterNum; ++i)
// 	{
// 		MonsterData *pMonster = &pChapterData->tEnemyData.arMonsterList[i];
// 		if (pMonster->dwDamage < pMonster->dwTotalHP)
// 			return false;
// 	}

	return true;
}

// ���¹����˺�
void CEliteFB::UpateMonsterDamage(WORD wChapter, MonsterData *pMonsterList, BYTE byMonsterNum)
{
// 	EliteFBChapter *pChapterData = GetChapterData(wChapter);
// 	if (!pChapterData)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵľ�Ӣ�����½�����[chapter:%d]", __FUNCTION__, wChapter);
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


// ���͸�������
void CEliteFB::SendFBData()
{
	MSG_S2C_FB_ELITE_DATA_RESP msg;
	msg.m_Data = *m_pFBData;
	m_pPlayer->SendPacket(&msg);
}



/////////////////////////////////////////////////////////////////////////////////////
// Ӣ�۸���

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

// ��ʼս��
bool CHeroFB::OnBattleStart(WORD wChapter, BYTE byMapIdx)
{
	// �����ս����
	if (byMapIdx && m_pFBData->byBattleTimes >= m_pPlayer->GetVipValue(VIP_HERO_FB_BATTLE_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸���������ս����������", __FUNCTION__);
		return false;
	}

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	WORD wMapId = byMapIdx ? pChapterCfg->GetMapId(byMapIdx) : pChapterCfg->wExtraMapId;

	// ȡ����
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����ؿ�����[chapter:%d,MapId:%d]",
			__FUNCTION__, wChapter, wMapId);
		return false;
	}
	
	// �������
// 	if (m_pPlayer->GetProperty(ROLE_PRO_STAMINA) < pMapCfg->nCostStamina)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸����ؿ���ս��������!", __FUNCTION__);
// 		return false;
// 	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۹ؿ���ս���Ĳ���!", __FUNCTION__);
		return false;
	}

	// ȡ�½�����
	HeroFubenChapterData *pChapterData = GetChapterData(wChapter);
	if (!pChapterData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵ�Ӣ�۸����½�����[chapter:%d]",__FUNCTION__, wChapter);
		return false;
	}

	// Խ�½���ս
	if (wChapter > GetNextChapter())
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸���Խ�½���ս!", __FUNCTION__);
		return false;
	}

	// Խ�ؿ���ս
	if (GetNextChapter() == wChapter && byMapIdx > GetNextMapIndex())
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸���Խ�ؿ���ս!", __FUNCTION__);
		return false;
	}

	// ��ս�ȼ��ؿ�
	if (byMapIdx)
	{
		// ��⵱ǰ��ս����
		if (pChapterData->arMapList[byMapIdx - 1].byBattleTimes >= pMapCfg->byDailyBattleTimesLimit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸����ؿ���������ս��!", __FUNCTION__);
			return false;
		}

		// �Ƿ���ͨ��
// 		if (wChapter < m_pFBData->wChapter || byMapIdx <= pChapterData->byMapIdx)
// 		{
// 			m_pFBManager->SetIsPassed(true);
// 		}
	}
	// ��ս����ؿ�
	else
	{
		// ����Ƿ���ڶ���ؿ�
		if (!pChapterCfg->wExtraMapId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸������½�û����ؿ�������ս[chapter:%d]!", __FUNCTION__, wChapter);
			return false;
		}

		// ����Ƿ�����ս��
		if (pChapterData->byIsExtraMapPass)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸������½ڵĶ���ؿ�����ս��[chapter:%d]!", __FUNCTION__, wChapter);
			return false;
		}

		// ����Ƿ������ս��
		if (pChapterData->byMapIdx < pChapterCfg->vMapList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ͨ�ر��½����йؿ�������ս����ؿ�!", __FUNCTION__);
			return false;
		}
	}

	// ��¼ս����Ϣ
	m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId);

	return true;
}

// ս������
bool CHeroFB::OnBattleOver(BYTE byIsWin, BYTE byStar)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	BYTE byMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����½�����[chapter:%d]",__FUNCTION__, wChapter);
		return false;
	}

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����ؿ�����[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	HeroFubenChapterData *pChapterData = GetChapterData(wChapter);
	if (!pChapterData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������ҵ�Ӣ�۸����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	if (byIsWin)
	{
		// ������
		m_pFBManager->DoWinReward(BATTLE_HERO_MAP, pMapCfg);

		// ��������
		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		// ��ս�ȼ��ؿ�
		if (byMapIdx)
		{
			// �������ս�µĹؿ�
			if (GetNextChapter() == wChapter && GetNextMapIndex() == byMapIdx)
			{
				m_pFBData->wChapter = wChapter;
				pChapterData->arMapList[byMapIdx - 1].byBattleTimes = 1;
				pChapterData->arMapList[byMapIdx - 1].byIsGetAward = false;
				pChapterData->byMapIdx = byMapIdx;

				// �˹ؿ��״λ�ʤ
				m_pFBManager->SetIsFirstWin(true);
			}
			else
			{
				// ��¼��ս����
				pChapterData->arMapList[byMapIdx - 1].byBattleTimes++;
			}

			m_pFBData->byBattleTimes++;
		}
		// ��ս����ؿ�
		else
		{
			pChapterData->byIsExtraMapPass = true;
		}
	}

	SendFBData();

	// �ճ�����
	m_pPlayer->OnUpdateDailyMission(14);

	return true;
}

// ��ȡ��������
bool CHeroFB::OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx)
{
	if (!wChapter || !byMapIdx)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������Ϸ�[chapter:%d,index:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	BYTE bySvrMapIdx = byMapIdx - 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����½�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	HeroFubenChapterData *pChapter = GetChapterData(wChapter);
	if (!pChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ȡ�������½ڷǷ�[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// �ؿ�����
	if (byRewardType == FB_AWARD_MAP)
	{
		if (bySvrMapIdx >= pChapter->byMapIdx)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ȡ�����Ĺؿ��Ƿ�[chapter:%d,map:%d]",
				__FUNCTION__, wChapter, bySvrMapIdx);
			return false;
		}

		const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pChapterCfg->vMapList[bySvrMapIdx]);
		if (!pMapCfg || !pMapCfg->vBoxList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ��������ݻ���ľ�����ñ��佱��[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		HeroMapData &tMapData = pChapter->arMapList[bySvrMapIdx];

		// ����Ƿ��Ѿ���ȡ��
		if (tMapData.byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ȡ�����佱��[map:%d]", __FUNCTION__, bySvrMapIdx);
			return false;
		}

		// ��ӽ���
		m_pPlayer->AddObjectList(pMapCfg->vBoxList, TRUE, FROM_HERO_FB_REWARD);

		tMapData.byIsGetAward = true;
	}
	// ��������
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۸���ľ����������[chapter:%d,index:%d]", __FUNCTION__, wChapter, bySvrMapIdx);
	}

	return true;
}

// ����Ӣ�۸�������
void CHeroFB::OnHeroFBData()
{
	SendFBData();
}

// ȡ�½�����
HeroFubenChapterData* CHeroFB::GetChapterData(WORD wChapter)
{
	if (!wChapter || wChapter > MAX_FUBEN_CHAPER_NUM)
		return NULL;

	return &m_pFBData->arChapterList[wChapter-1];
}

// ȡ��һ����ս���½�
WORD CHeroFB::GetNextChapter()
{
	if (!m_pFBData->wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(m_pFBData->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����½�����[chapter:%d]", __FUNCTION__, m_pFBData->wChapter);
		return 1;
	}

	// ���½��ѱ�������
	if (m_pFBData->arChapterList[m_pFBData->wChapter - 1].byMapIdx >= pChapterCfg->vMapList.size())
		return m_pFBData->wChapter + 1;

	return m_pFBData->wChapter;
}

// ȡ��һ����ս�Ĺؿ�����
BYTE CHeroFB::GetNextMapIndex()
{
	if (!m_pFBData->wChapter)
		return 1;

	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetHeroFBChapter(m_pFBData->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����½�����[chapter:%d]", __FUNCTION__, m_pFBData->wChapter);
		return 1;
	}

	BYTE byMapIdx = m_pFBData->arChapterList[m_pFBData->wChapter - 1].byMapIdx;

	// ���½��ѱ�������
	if (byMapIdx >= pChapterCfg->vMapList.size())
		return 1;

	return byMapIdx + 1;
}

// �Ƿ��к��
bool CHeroFB::IsHaveRedPoint()
{
	// ��Ӣ�۸���ÿ�մ�������û��ʹ����ʱ
	if (m_pFBData->byBattleTimes < m_pPlayer->GetVipValue(VIP_HERO_FB_BATTLE_TIMES))
		return true;

	// ��Ӣ�۸������о�Ӣ�������ȡʱ
	for (WORD i = 0; i < m_pFBData->wChapter; ++i)
	{
		HeroFubenChapterData *pChapter = &m_pFBData->arChapterList[i];
		const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetHeroFBChapter(i+1);
		if (!pChapterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸����½�����[chapter:%d]", __FUNCTION__, i + 1);
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
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۸�������[MapId:%d]", __FUNCTION__, dwMapId);
				return false;
			}
			if (pMapCfg->vBoxList.size())
				return true;
		}
	}
	
	return false;
}


// ���͸�������
void CHeroFB::SendFBData()
{
	MSG_S2C_FB_HERO_DATA_RESP msg;
	memcpy_s(&msg.m_tFBData, sizeof(HeroFBData), m_pFBData, sizeof(HeroFBData));
	m_pPlayer->SendPacket(&msg);
}


//////////////////////////////////////////////////////
// �ճ�����

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

// ��ʼս�� 
bool CDailyFB::OnBattleStart(WORD wChapter, BYTE byMapIdx)
{
	// ȡ�����½�����
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetDailyChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ������½ڸ���[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ����Ƿ�Խ��
	if (!byMapIdx || byMapIdx > pChapterCfg->vMapList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ճ������ؿ���������[MapIdx:%d]", __FUNCTION__, byMapIdx);
		return false;
	}

	BYTE byServerMapIdx = byMapIdx - 1;

	// ���ȼ�
	if (m_pPlayer->GetLevel() < pChapterCfg->wOpenLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ճ������ܵȼ�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	time_t tCurTime = time(NULL);
	tm tmTime;
	localtime_s(&tmTime, &tCurTime);

	// �������Ƿ񿪷�
	if (!pChapterCfg->IsOpenDay(tmTime.tm_wday))
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ճ���������û����[chapter:%d,weekday:%d]", __FUNCTION__, wChapter, tmTime.tm_wday);
		return false;
	}

	WORD wMapId = pChapterCfg->vMapList[byServerMapIdx];

	// ȡ�ؿ�����
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ������ؿ�����[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	DailyChapterData *pChapter = GetChapterData(wChapter);

	// �����ս����
	if (pChapter && pChapter->byBattleTimes >= pChapterCfg->wDailyBattleTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ճ�������ս�����Ѵ�����[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ���ǰ�ùؿ��Ƿ�����ս��
	if (pChapter && pChapter->nMapIdxRecord >= 0 && byMapIdx > pChapter->nMapIdxRecord + 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ճ�������ǰ�ùؿ���ľ����ս[chapter:%d,MapIdx:%d]", __FUNCTION__, wChapter, byMapIdx);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pMapCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ճ�������ս���Ĳ���[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	m_pFBManager->SetBattleInfo(wChapter, byMapIdx, wMapId);
	return true;
}

// ս������
bool CDailyFB::OnBattleOver(BYTE byIsWin, WORD wContext, BYTE byIsPass)
{
	WORD wChapter = m_pFBManager->GetCurChapter();
	WORD wMapIdx = m_pFBManager->GetMapIdx();
	WORD wMapId = m_pFBManager->GetMapId();

	//WORD wServerMapIdx = wMapIdx - 1;

	// ȡ�½�����
	const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetDailyChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ������½ڸ���[chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ȡ�ؿ�����
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ������ؿ�����[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	// ȡ��������
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
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ������½�����[Chapter:%d]", __FUNCTION__, wChapter);
		return false;
	}

	// ���Ÿ�������(�������,ʱ���)
	const DailyFBDamageReward_Config *pDamageRewardCfg = g_pCfgMgr->GetDailyFBDamageReward(wChapter, wMapIdx, wContext);

	// ��ʤ
	if (byIsWin)
	{
		if (pDamageRewardCfg)
			m_pPlayer->AddObjectList(pDamageRewardCfg->vItemList, TRUE, FROM_DAILY_FB_BATTLE_REWARD);

		// ��ʤ��۴���
		pChapter->byBattleTimes++;

		m_pFBManager->DoWinReward(BATTLE_DAILY_MAP, pMapCfg);

		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		// ������״�ͨ��
		if (wMapIdx > pChapter->nMapIdxRecord && byIsPass)
		{
			// ������ս����
			if (pChapter->byBattleTimes)
				pChapter->byBattleTimes--;

			// ��¼��ս��¼
			pChapter->nMapIdxRecord = wMapIdx;
		}
	}
	else
	{
		// ʧ��Ҳ�н���
		if (pChapterCfg->byIsLoseAlsoHaveReward)
		{
			if (pDamageRewardCfg)
			{
				m_pPlayer->AddObjectList(pDamageRewardCfg->vItemList, TRUE, FROM_DAILY_FB_BATTLE_REWARD);

				// ʧ��ʱ�н���ҲҪ�۴���
				pChapter->byBattleTimes++;
			}
		}
	}

	//MAPLOG(GUI_LEVEL, "[%s]�����ճ���������[Chapter:%d,MapIdx:%d,Context:%d]", __FUNCTION__, wChapter, wMapIdx, wContext);

	// �ճ�����
	m_pPlayer->OnUpdateDailyMission(13);

	return true;
}

// �����ճ���������
void CDailyFB::OnDailyDataReq()
{
	SendDailyData();
}

// �����ճ���������
void CDailyFB::SendDailyData()
{
	MSG_S2C_FB_DAILY_DATA_RESP msg;
	memcpy_s(&msg.m_tFBData, sizeof(DailyFBData), m_pFBData, sizeof(DailyFBData));
	m_pPlayer->SendPacket(&msg);
}

// ȡ�½�����
DailyChapterData* CDailyFB::GetChapterData(WORD wChapter)
{
	for (BYTE i = 0; i < m_pFBData->wChapterNum; ++i)
	{
		if (m_pFBData->arChapterList[i].wChapter == wChapter)
			return &m_pFBData->arChapterList[i];
	}
	return NULL;
}

// ����½�����
void CDailyFB::AddChapterData(const DailyChapterData &tChapter)
{
	if (m_pFBData->wChapterNum >= MAX_DAILY_MAP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ճ������½��б�����", __FUNCTION__);
		return;
	}

	m_pFBData->arChapterList[m_pFBData->wChapterNum++] = tChapter;
}




/////////////////////////////////////////////////////////
// ��������
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

// ��ͨս����ʼ
bool CTowerFB::OnNormalBattleStart(BYTE byType)
{
	// ����Ƿ���Ҫ����
  	if (m_pFBData->byIsNeedReset)
  	{
  		MAPLOG(ERROR_LEVEL, "[%s]������Ҫ���÷�����ս", __FUNCTION__);
  		return false;
  	}
  
	// ׼����ս�Ĺؿ�
	WORD wNextMapLevel = m_pFBData->wCurMapLevel + 1;

	// ���ؿ�����
	if (wNextMapLevel >= MAX_TOWER_MAP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����ؿ��Ѵ�����[MaxLevel:%d]", __FUNCTION__, wNextMapLevel);
		return false;
	}
	
	// ȡ�ؿ�����
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(wNextMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�����ؿ�����[MapLevel:%d]", __FUNCTION__, wNextMapLevel);
		return false;
	}

  	// ��¼ս����Ϣ
	m_pFBManager->SetBattleInfo(byType, 0);

	return true;
}

// ��ͨս������
bool CTowerFB::OnNormalBattleOver(BYTE byIsWin, bool bIsClean)
{ 
	// ׼����ս�Ĺؿ�
	WORD wNextMapLevel = m_pFBData->wCurMapLevel + 1;

	// ȡ�ؿ�����
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(wNextMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�����ؿ�����[MapLevel:%d]", __FUNCTION__, wNextMapLevel);
		return false;
	}

	// ȡ�½�����
	const TowerChapter_Config *pTowerChapterCfg = CConfigManager::Instance()->GetTowerChapter(pTowerMapCfg->wChapter);
	if (!pTowerChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�����½�����[MapLevel:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
		return false;
	}

	BYTE byType = m_pFBManager->GetCurChapter();
	if (byType >= pTowerMapCfg->MapDataList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]ѡ�����������Խ��[type:%d]", __FUNCTION__, byType);
		return false;
	}

	const TowerMap_Config::MapData &tMapData = pTowerMapCfg->MapDataList[byType];

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(tMapData.wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ս�ؿ�������[MapLevel:%d]", __FUNCTION__, tMapData.wMapId);
		return false;
	}

	bool bIsLose = false;

	int nMultiple = CActivityManager::GetLimitProductMultiple(BATTLE_TOWER_NORMAL_MAP);

	// ��ʤ
	if (byIsWin)
	{
		// ��¼��õ�����
		m_pFBData->arMapStarList[m_pFBData->wCurMapLevel] = tMapData.byStar;

		// ����Ǳ��½����һ��
		if (pTowerMapCfg->bIsLast)
		{
			// ���½ڻ�õ�����
			WORD wChapterStar = 0;
			for (int i = 0; i < pTowerChapterCfg->vMapList.size(); ++i)
			{
				if (pTowerChapterCfg->vMapList[i])
					wChapterStar += m_pFBData->arMapStarList[pTowerChapterCfg->vMapList[i]-1];
			}

			// ���½ڽ���
			const TowerStarAward *pStarAward = pTowerChapterCfg->GetAward(wChapterStar);
			if (!pStarAward)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������½�������������[star:%d]", __FUNCTION__, wChapterStar);
				return false;
			}
			m_pPlayer->AddObjectList(pStarAward->vAwardList, TRUE, FROM_TOWER_NORMAL_FB_BATTLE_REWARD);

			// �������  todo::�˴�������Ҫ�Ż�
			m_pFBData->byRandProNum = 0;
			m_pFBData->arRandProIdxList[m_pFBData->byRandProNum++] = RandProIdx(3);
			m_pFBData->arRandProIdxList[m_pFBData->byRandProNum++] = RandProIdx(6);
			m_pFBData->arRandProIdxList[m_pFBData->byRandProNum++] = RandProIdx(9);

			// ����3��ͨ�� 
			if (wChapterStar >= 9 && m_pFBData->byKeepThreeStarPassedRecord + 1 == pTowerMapCfg->wChapter)
			{
				m_pFBData->byKeepThreeStarPassedRecord = pTowerMapCfg->wChapter;
			}
		}

		for (int i = 0; i < nMultiple; ++i)
		{
			m_pPlayer->ModifyPropertyList(pMapCfg->vNormalRewardList, TRUE, FROM_TOWER_FB_BATTLE);
		}

		// ����һ��
		m_pFBData->wCurMapLevel++;

		// ��¼����
		m_pFBData->wCurStar += tMapData.byStar;

		// ���㵱ǰ������
		WORD wCurTotalStar = 0;
		for (WORD i = 0; i < m_pFBData->wCurMapLevel; ++i)
		{
			wCurTotalStar += m_pFBData->arMapStarList[i];
		}

		// ������ʷ����
		if (m_pFBData->wCurMapLevel > m_pFBData->wMapLevelRecord)
		{
			m_pFBData->wMapLevelRecord = m_pFBData->wCurMapLevel;

			// ����㲥
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(109);
			if (pServerCfg && pServerCfg->IsInList(m_pFBData->wMapLevelRecord))
			{
				CSystemNotice sys_notice(3, m_pPlayer->GetServerId());
				sys_notice.AddParam(m_pPlayer->GetRoleName());
				sys_notice.AddParam(m_pFBData->wMapLevelRecord);
				sys_notice.SendMsg();
			}
		}

		// �����ǰ����������ʷ��¼
		if (wCurTotalStar > m_pFBData->wStarMaxRecord)
		{
			m_pFBData->wStarMaxRecord = wCurTotalStar;

			// ����������а�����
			CRankManager::Instance()->AddRoleRankData(RANK_TOWER, m_pPlayer, m_pFBData->wStarMaxRecord, 0, 0, 20);
		}

		// �ɾ��¼�
		m_pPlayer->OnUpdateAchievement(7, m_pFBData->wStarMaxRecord, false);

		// 7���¼�(���������ۼ�)
		m_pPlayer->OnUpdateSevenDayTarget(12, tMapData.byStar, true);
	}
	// ʧ��
	else
	{
		bIsLose = true;
	}

	// ����Ƿ���ͨ�����йؿ� ���� ʧ�� 
	if (m_pFBData->wCurMapLevel >= CConfigManager::Instance()->m_TowerChapterList.size() * 3 || bIsLose)
	{
		m_pFBData->byIsNeedReset = true;
		m_pFBData->byIsBuyStarGoods = false;

		const TowerStarGoods_Config *pStarGoods = CConfigManager::Instance()->GetTowerStarGoods(GetTotalStar());
		if (!pStarGoods || !pStarGoods->vGoodsList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������Ʒ���б�Ϊ��[star:%d]", __FUNCTION__, GetTotalStar());
			return false;
		}

		// �������
		m_pFBData->byStarGoodsIndex = Rand::Instance()->irand(0, pStarGoods->vGoodsList.size() - 1);
	}

	if (!bIsClean)
		SendTowerData();

	// �ճ�����
	m_pPlayer->OnUpdateDailyMission(12);

	return true;
}

// ��Ӣս����ʼ
bool CTowerFB::OnEliteBattleStart(WORD wMapLv)
{
	// ����Ƿ�Խ�ؿ���ս
	if (wMapLv > m_pFBData->wEliteMapLevel + 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӣ��������Խ�ؿ���ս[MapLevel:%d]", __FUNCTION__, wMapLv);
		return false;
	}

	// ȡ��Ӣ�ؿ�����
	const TowerEliteMap_Config *pEliteMapCfg = CConfigManager::Instance()->GetTowerEliteMap(wMapLv);
	if (!pEliteMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ӣ�ؿ�����[MapLevel:%d]", __FUNCTION__, wMapLv);
		return false;
	}

	// �����ս����
	if (m_pFBData->byEliteMapBattleTimes >= m_pFBData->byBuyEliteMapBattleTimes + g_GetConfigValue(32))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ӣ�ؿ���ս����������[MapLevel:%d]", __FUNCTION__, wMapLv);
		return false;
	}

	// ����Ǵ���һ���ؿ�
	if (wMapLv == m_pFBData->wEliteMapLevel + 1)
	{
		// ���ǰ�ùؿ�
		if (pEliteMapCfg->wNeedNormalMapLevel > m_pFBData->wMapLevelRecord)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ͨ����ͨ�ؿ�[MapLevel:%d]������ս", __FUNCTION__, pEliteMapCfg->wNeedNormalMapLevel);
			return false;
		}
	}	

	m_pFBManager->SetBattleInfo(1, wMapLv, pEliteMapCfg->wMapId);

	return true;
}

// ��Ӣս����ʼ
bool CTowerFB::OnEliteBattleOver(BYTE byIsWin)
{
	BYTE byMapLevel = m_pFBManager->GetMapIdx();

	// ȡ��Ӣ�ؿ�����
	const TowerEliteMap_Config *pEliteMapCfg = CConfigManager::Instance()->GetTowerEliteMap(byMapLevel);
	if (!pEliteMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ӣ�ؿ�����[MapLevel:%d]", __FUNCTION__, byMapLevel);
		return false;
	}

	// ȡ��ս�ؿ�����
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(pEliteMapCfg->wMapId);
 	if (!pMapCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ս�ؿ�������[MapId:%d]", __FUNCTION__, pEliteMapCfg->wMapId);
 		return false;
 	}
 
 	// todo::�����׼��
 
 	if (byIsWin)
 	{
		m_pPlayer->DoMoneyCost(pMapCfg->vCostList);

		m_pPlayer->ModifyPropertyList(pMapCfg->vNormalRewardList, TRUE, FROM_TOWER_FB_BATTLE);

		// ��ʤ����
		if (byMapLevel == m_pFBData->wEliteMapLevel + 1)
		{
			// �˹ؿ��״λ�ʤ
			m_pFBManager->SetIsFirstWin(true);

			// �Ƿ���Ҫ������
			m_pFBData->wEliteMapLevel = byMapLevel;
		}

		m_pFBData->byEliteMapBattleTimes++;

		// ���ջ(ÿ��սʤ����������Ӣ�ؿ�����)
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 14, 1, true);

		// ���ջ�ԾĿ��(ÿ��սʤ����������Ӣ�ؿ�����)
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 14, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 14, 1, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 14, 1, true);
 	}

	SendTowerData();

	return true;
}

// ������ս����
void CTowerFB::OnNetBattleTimesBuy()
{
	// ��⹺�����
	if (m_pFBData->byBuyEliteMapBattleTimes >= m_pPlayer->GetVipValue(VIP_BUY_TOWER_FB_BATTLE_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����ؿ���ս���������Ѵ�����!", __FUNCTION__);
		return;
	}

	// ȡͨ������
	int nFindKey = 31;
	const Server_Config* pCfg = CConfigManager::Instance()->GetCommonCfg(nFindKey);
	if (!pCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ������[key:%d]!", __FUNCTION__, nFindKey);
		return;
	}
	if (!pCfg->vValueList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ӣ�ؿ������������ľ������[key:%d]!", __FUNCTION__, nFindKey);
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

	// �����ʯ
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ӣ�ؿ���ʯ����!", __FUNCTION__);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TOWER_BATTLE_TIMES_BUY);

	m_pFBData->byBuyEliteMapBattleTimes++;

	MSG_S2C_TOWER_BUY_TIMES_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ���ø���
bool CTowerFB::OnNetFBReset(bool bIsGM)
{
	// ����Ƿ���Ҫ����
	if (!bIsGM && !m_pFBData->byIsNeedReset)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����Ŀǰ����Ҫ����!",__FUNCTION__);
		return false;
	}

	// ���������ô�������
	if (m_pFBData->byTodayResetTimes >= m_pPlayer->GetVipValue(VIP_TOWER_FB_RESET_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����ؿ��������ô����Ѵ�����!", __FUNCTION__);
		return false;
	}

	// ȡͨ������
	int nFindKey = 30;
	const Server_Config* pCfg = CConfigManager::Instance()->GetCommonCfg(nFindKey);
	if (!pCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ������[key:%d]!", __FUNCTION__, nFindKey);
		return false;
	}
	if (!pCfg->vValueList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ӣ�ؿ���������ľ������[key:%d]!", __FUNCTION__, nFindKey);
		return false;
	}

	// ������Ҫ���ĵ���ʯ
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
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ����,���������ؿ�ʧ��!",__FUNCTION__);
		return false;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TOWER_RESET);

	// ��������
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

	// 8-14���¼�(�������ô���)
	m_pPlayer->OnUpdateSevenDayTarget(22, 1, true);

	// ���ջ(�ۼ������������ô���)
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 15, 1, true);

	// ���ջ�ԾĿ��(�ۼ������������ô���)
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 15, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 15, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 15, 1, true);

	return true;
}

// ѡ������
void CTowerFB::OnNetSelectPro(BYTE byStar, BYTE byProIndex)
{
	// ȡ�ؿ�����
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(m_pFBData->wCurMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�����ؿ�����[MapLevel:%d]", __FUNCTION__, m_pFBData->wCurMapLevel);
		return;
	}

	// ����Ƿ����һ��
	if (!pTowerMapCfg->bIsLast)
	{ 
		MAPLOG(ERROR_LEVEL, "[%s]�������½����һ��,��û��ѡ����[MapLevel:%d]", __FUNCTION__, m_pFBData->wCurMapLevel);
		return;
	}

	// ȡ������Ӧ������
	const TowerRandPro_Config *pRandProCfg = CConfigManager::Instance()->GetTowerRandPro(byStar);
	if (!pRandProCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������������[Star:%d]", __FUNCTION__, byStar);
		return;
	}

	// ��������Ƿ��㹻 
	if (m_pFBData->wCurStar < byStar)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������,����ѡ�������[Star:%d]", __FUNCTION__, byStar);
		return;
	}

	// �����������
	if (byProIndex >= pRandProCfg->vProList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]ѡ�����������Խ��[Star:%d, ProIndex:%d]", __FUNCTION__, byStar, byProIndex);
		return;
	}

	const Property &tPro = pRandProCfg->vProList[byProIndex];

	// �������
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
	// �������
	if (!bIsHave && m_pFBData->bySelectProNum < HERO_PRO_MAX)
	{
		m_pFBData->arSelectProList[m_pFBData->bySelectProNum++] = tPro;
	}	

	// ������
	m_pFBData->wCurStar -= byStar;

	// ����������
	m_pFBData->byRandProNum = 0;
	//memset(m_pFBData->arRandProIdxList, 0, sizeof(m_pFBData->arRandProIdxList));

	SendTowerData();

	MSG_S2C_TOWER_SELECT_PRO_RESP msg;
	msg.m_byStar = byStar;
	msg.m_byProIndex = byProIndex;
	m_pPlayer->SendPacket(&msg);
}

// ����������Ʒ
void CTowerFB::OnNetBuyStarGoods()
{
	// �Ƿ��Ѿ���ȡ����
	if (m_pFBData->byIsBuyStarGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ȡ��������Ʒ!", __FUNCTION__);
		return;
	}

	// ȡ������Ʒ����
	const TowerStarGoods_Config *pStarGoods = CConfigManager::Instance()->GetTowerStarGoods(GetTotalStar());
	if (!pStarGoods || !pStarGoods->vGoodsList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������Ʒ���б�Ϊ��[star:%d]", __FUNCTION__, GetTotalStar());
		return;
	}

	// ��������Ϸ���
	if (m_pFBData->byStarGoodsIndex >= pStarGoods->vGoodsList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ����Խ��[GoodsIndex:%d]", __FUNCTION__, m_pFBData->byStarGoodsIndex);
		return;
	}

	const TowerStarGoods_Config::StarGoods &tGoods = pStarGoods->vGoodsList[m_pFBData->byStarGoodsIndex];

	// �������
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < tGoods.nPrice)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������������Ʒ��ʯ����", __FUNCTION__);
		return;
	}

	// ����
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -tGoods.nPrice, TRUE, FROM_TOWER_BUY_STAR_GOODS);

	// ������Ʒ
	m_pPlayer->AddObject(tGoods.nId, tGoods.wNum, TRUE, FROM_TOWER_BUY_STAR_GOODS);

	// �޸ı�־(����ȡ)
	m_pFBData->byIsBuyStarGoods = true;

	MSG_S2C_TOWER_BUY_STAR_GOODS_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ����������ͨ��������
void CTowerFB::OnTowerFBData()
{
	SendTowerData();
}

// һ��ɨ��
void CTowerFB::OnNetOneKeyClean(bool bIsToHistoryMap)
{
	// ���VIP����ɫ�ȼ�
	if (!m_pPlayer->GetVipValue(VIP_TOWER_MAP_CLEAN) && m_pPlayer->GetLevel() < g_GetConfigValue(90))
	{
		MAPLOG(ERROR_LEVEL, "[%s]VIP����ɫ�ȼ�������,����ɨ��", __FUNCTION__);
		return;
	}

	// ׼����ս�Ĺؿ�
	WORD wNextMapLevel = m_pFBData->wCurMapLevel + 1;

	// ȡ�ؿ�����
	const TowerMap_Config *pTowerMapCfg = CConfigManager::Instance()->GetTowerMap(wNextMapLevel);
	if (!pTowerMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�����ؿ�����[MapLevel:%d]", __FUNCTION__, wNextMapLevel);
		return;
	}

	// ��ǰ�½ڲ���ɨ��
	if (pTowerMapCfg->wChapter > m_pFBData->byKeepThreeStarPassedRecord)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ǰ�½ڲ���ɨ��[MapLevel:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
		return;
	}

	const TowerChapter_Config *pChapterCfg = g_pCfgMgr->GetTowerChapter(pTowerMapCfg->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�����½�����[ChapterIdx:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
		return;
	}

	BYTE byBegin = m_pFBData->wCurMapLevel;
	BYTE byEnd = m_pFBData->wCurMapLevel;
	for (WORD i = m_pFBData->wCurMapLevel; i < pChapterCfg->vMapList[pChapterCfg->vMapList.size() - 1]; ++i)
	{
		if (!OnNormalBattleStart(2))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����һ��ɨ������ʼս��ʧ��[ChapterIdx:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
			return;
		}
		if (!OnNormalBattleOver(true, true))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����һ��ɨ���������ս��ʧ��[ChapterIdx:%d]", __FUNCTION__, pTowerMapCfg->wChapter);
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

// �Ƿ��к��
bool CTowerFB::IsHaveRedPoint()
{
	// ��ǰ���������У�û��ʧ��ʱ
	if (!m_pFBData->byIsNeedReset)
		return true;
	
	// ��ǰ�����п��п�������ô���ʱ
	if (!m_pFBData->byTodayResetTimes)
		return true;

	return false;
}

// ȡ��ʷ����
WORD CTowerFB::GetMaxStarRecord()
{
	return m_pFBData->wStarMaxRecord;
}

// ȡ����������
WORD CTowerFB::GetTotalStar()
{
	WORD wTotalStar = 0;
	for (WORD i = 0; i < m_pFBData->wCurMapLevel; ++i)
	{
		wTotalStar += m_pFBData->arMapStarList[i];
	}
	return wTotalStar;
}

// �������
BYTE CTowerFB::RandProIdx(BYTE byStar)
{
	const TowerRandPro_Config *pProCfg = CConfigManager::Instance()->GetTowerRandPro(byStar);
	if (!pProCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���[%d]�������������", __FUNCTION__, byStar);
		return 0;
	}
	if (!pProCfg->vProList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���[%d]�������������Ϊ��", __FUNCTION__, byStar);
		return 0;
	}

	return Rand::Instance()->irand(0, pProCfg->vProList.size() - 1);
}

void CRebelFB::Init(Player *pPlayer, CFubenManager *pFBMgr)
{
	m_pFBManager = pFBMgr;
	m_pPlayer = pPlayer;
}

// ��ʼս��
bool CRebelFB::OnBattleStart(DWORD dwRebelId, WORD wType)
{
	return CRebelManager::Instance()->OnBattleStart(m_pPlayer, dwRebelId, wType);
}

// ս������
bool CRebelFB::OnBattleOver(BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	//DWORD dwRebelId = m_pFBManager->GetCurChapter();
	DWORD dwRebelId = m_pFBManager->GetTargetId();
	WORD wType = m_pFBManager->GetMapIdx();
	return CRebelManager::Instance()->OnBattleOver(m_pPlayer, dwRebelId, wType, pMonsterList, byMonsterNum);
}

// ȡ���⽱���ĵ�����Ʒ����
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
	// ��վ��鸱�����
	for (WORD i = 0; i < m_pFubenInfo->tEliteFBData.wChapter; ++i)
	{
		EliteFBChapter &tChapter = m_pFubenInfo->tEliteFBData.arChapterList[i];
		memset(&tChapter.tEnemyData, 0, sizeof(tChapter.tEnemyData));
	}
}

// ��ʼս�� 
void CFubenManager::OnBattleStart(BYTE byFBType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, DWORD dwClientPower, const char *pSign)
{
	MAPLOG(DEBUG_LEVEL, "[%s]����ʼս��[RoleId:%d,FBType:%d,dwParam1:%d,dwParam2:%d,dwParam3:%d,dwParam4:%d,ClientPower:%u]", 
		__FUNCTION__, m_pPlayer->GetRoleId(), byFBType, dwParam1, dwParam2, dwParam3, dwParam4, dwClientPower);

	// mapidx ��ʼ˵��
	/*
	����: 1
	��Ӣ : 1
	Ӣ�� : 1
	�ճ� : 0
	�Ѿ� : ��
	��ͨ���� : ��
	��Ӣ���� :
	��� : ��
	�ᱦ : ��
	������ : ��
	���� : ��
	*/

	/*
	ʹ��������ģ�飺���߸�������Ӣ������Ӣ�۸���
	ʹ�þ�����ģ�飺�ᱦ��������

	��������þ���=����ȼ�*2*��������
	��ý��=����ȼ�*5*��������+100

	��������þ���=����ȼ�*2*���ľ���
	��ý��=����ȼ�*10*���ľ���+200
	*/

	// ��ֹ�������ٵ��
	if (BATTLE_TYPE_INVALID != m_byFBType && GetTickCount64() <= m_lStartBattleTime + 2000)
	{
		m_pPlayer->SendErrorMsg(ERROR_IN_BATTLE, C2S_BATTLE_START_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�������ս����...[last_type:%d,cur_type:%d]",
			__FUNCTION__, m_byFBType, byFBType);
		return;
	}

	// ���ս���Ƿ������� power + role_id + "HelloWorld"
	if (pSign)
	{
		char szBuff[128] = { 0 };
		sprintf_s(szBuff, sizeof(szBuff), "%d%d%s", dwClientPower, m_pPlayer->GetRoleId(), "HelloWorld");
		string strMd5Sign = MD5::Encode(szBuff).HexString();

		// ǩ������,�п��ܱ��İ�
		if (strcmp(strMd5Sign.c_str(), pSign) != 0)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "ս����ʼ�����ǩ���쳣[role_id:%d,server_sign:%s,client_sign:%s]",
				m_pPlayer->GetRoleId(), strMd5Sign.c_str(), pSign);
			return;
		}

		// ���ս��
		DWORD dwServerPower = m_pPlayer->GetFightingPower();
		if (!g_MapServer.CheckPowerIsValid(dwServerPower, dwClientPower))
		{
			// ��Ŵ���
			m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "ս����ʼ�����ս���쳣[role_id:%d,server_power:%d,client_power:%d]",
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
		MAPLOG(ERROR_LEVEL, "[%s]δ����ĸ�������[type:%d]", __FUNCTION__, byFBType);
		break;
	}

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_START_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����ʼս������ʧ��[type:%d,param1:%d,param2:%d]", 
			__FUNCTION__, byFBType, dwParam1, dwParam2);
		return;
	}

	// ����õ������Ʒ�Ȼ���
	m_DropItemList.clear();
	if (m_dwMapId)
	{
		if (!MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_START_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]����������Ʒʧ��[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}
	}

	SetBattleType(byFBType);
	m_lStartBattleTime = GetTickCount64();	// ��ʼս��ʱ��
	
	// ��Ӧ�ͻ��˽���ս��
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

// ս������
void CFubenManager::OnBattleOver(BYTE byFBType, int nWinValue, BYTE byStar, DWORD dwContext, DWORD dwContext1, MonsterData *pMonsterList,
	BYTE byMonsterNum, DWORD dwDamage, DWORD dwClientPower, const char *pSign)
{
	MAPLOG(DEBUG_LEVEL, "[%s]����ս������[RoleId:%d,FBType:%d,nWinValue:%d,byStar:%d,dwContext:%d,dwContext1:%d,Damage%d,ClientPower:%d]",
		__FUNCTION__, m_pPlayer->GetRoleId(), byFBType, nWinValue, byStar, dwContext, dwContext1, dwDamage, dwClientPower);

	/*
	ʹ��������ģ�飺���߸�������Ӣ������Ӣ�۸���
	ʹ�þ�����ģ�飺�ᱦ��������

	��������þ���=����ȼ�*2*��������
	��ý��=����ȼ�*5*��������+100

	��������þ���=����ȼ�*2*���ľ���
	��ý��=����ȼ�*10*���ľ���+200
	*/

	DWORD dwSignDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i){
		dwSignDamage += pMonsterList[i].dwDamage;
	}

	int nMin = 16978;
	int nMax = 78786969;

	BYTE byIsWin = false;

	// ʤ��
	if (nWinValue >= nMin && nWinValue <= nMax)
	{
		byIsWin = true;
	}
	// ʧ��
	else
	{
		byIsWin = false;
	}

	// ����ǻ�ʤ������֤  WinValue + role_id + power + damage + "HelloWorld"
	if (byIsWin)
	{	
		char szBuff[128] = { 0 };
		sprintf_s(szBuff, sizeof(szBuff), "%d%d%d%d%s", nWinValue, m_pPlayer->GetRoleId(), dwClientPower, dwDamage, "HelloWorld");
		string strMd5Sign = MD5::Encode(szBuff).HexString();

		// ǩ������,�п��ܱ��İ�
		if (strcmp(strMd5Sign.c_str(), pSign) != 0)
		{
			// ��Ŵ���
			//m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]ս�����������ǩ���쳣[role_id:%d,server_sign:%s,client_sign:%s]", 
				__FUNCTION__, m_pPlayer->GetRoleId(), strMd5Sign.c_str(), pSign);
			return;
		}

		// ���ս��
		DWORD dwServerPower = m_pPlayer->GetFightingPower();
		if (!g_MapServer.CheckPowerIsValid(dwServerPower, dwClientPower))
		{
			// ��Ŵ���
			m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]ս�����������ս���쳣[role_id:%d,server_power:%d,client_power:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), dwServerPower, dwClientPower);
			return;
		}

		// ����˺�
		if (g_MapServer.IsCheckPower() && dwDamage != dwSignDamage)
		{
			// ��Ŵ���
			m_pPlayer->LockAccount();

			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]ս������������˺��쳣[role_id:%d,src_damage:%d,sign_damage:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), dwDamage, dwSignDamage);
			return;
		}

		// ���ս��
		const Map_Config *pMapCfg = m_dwMapId ? g_pCfgMgr->GetMap(m_dwMapId) : NULL;
		if (g_MapServer.IsCheckPower() && pMapCfg && dwServerPower < pMapCfg->dwNeedMinPower)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
			MAPLOG(ERROR_LEVEL, "[%s]ս������������˺��쳣[role_id:%d,server_power:%d,need_min_power:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), dwServerPower, pMapCfg->dwNeedMinPower);
			return;
		}


		// ��⹥������
	}

	// ���ս�������Ƿ�ƥ��
	if (m_byFBType != byFBType)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ս����ʼ����������Ͳ�ƥ��[start_type:%d,over_type:%d]", 
			__FUNCTION__, m_byFBType, byFBType);
		return;
	}

	// ����Ƿ�����ս״̬
	if (!CheckIsInBattle())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ľ����ս����...[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	// todo::�п���ʹ�������
	if (GetTickCount64() < m_lStartBattleTime + g_GetConfigValue(140))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ս������ϱ�̫����![type:%d,Interval:%d]", __FUNCTION__,
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

	// (���⽱��)
	case BATTLE_REBEL_MAP:
		bRet = m_RebelFB.OnBattleOver(byIsWin, pMonsterList, byMonsterNum);
		break;

	// (���⽱��)
	case BATTLE_TOWER_NORMAL_MAP:
		bRet = m_TowerFB.OnNormalBattleOver(byIsWin);
		break;

	// (���⽱��)
	case BATTLE_TOWER_ELITE_MAP:
		bRet = m_TowerFB.OnEliteBattleOver(byIsWin);
		break;

	// ľ�н���
	case BATTLE_LAND_MAP:
		bRet = CLandManager::Instance()->OnBattleOver(m_pPlayer, m_wCurChapter, byIsWin);
		break;

	// (���⽱��)
	case BATTLE_DOUBAO_MAP:
		bRet = m_pPlayer->GetDuobaoManager()->BattleOver(byIsWin);
		break;

	// (���⽱��)
	case BATTLE_ARENA_MAP:
		bRet = CArenaManager::Instance()->OnBattleOver(m_pPlayer, byIsWin);
		break;

	// (���⽱��)
	case BATTLE_FAIRY_MAP:
		bRet = m_pPlayer->GetZhanBuManager()->OnBattleOver(byIsWin);
		break;

	// (���⽱��)
	case BATTLE_SCORE_MATCH:
		bRet = CScoreMatchManager::Instance()->OnBattleOver(m_pPlayer, byIsWin);
		break;

	case BATTLE_GUIDE_MAP:
		bRet = true;
		break;

	// (���⽱��)
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
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]δ����ĸ�������[type:%d]", byFBType);
		}
		break;
	}

	m_pPlayer->SetIsHaveSpriteTalentSkillReward(false);

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BATTLE_END_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�������ս������ʧ��[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	// ��Ӧ�ͻ���ս������
	MSG_S2C_BATTLE_END_RESP msg;
	msg.m_wRewardItemNum = 0;

	// ����ͳһ����
	if (byIsWin)
	{
		const Map_Config *pMapCfg = m_dwMapId ? g_pCfgMgr->GetMap(m_dwMapId) : NULL;

		// ������
		if (pMapCfg)
		{
			map<DWORD, int> mItemInfoList;
			m_pPlayer->GetSpiritManager()->GetExtraRewardDropItemInfo(byFBType, mItemInfoList);

			// ��������Ʒ�б�(����)
			for (size_t i = 0; i < pMapCfg->vRandRewardList.size(); ++i)
			{
				const RewardItem &tRewardItem = pMapCfg->vRandRewardList[i];
				Item item(tRewardItem.dwItemId, g_Rand(tRewardItem.wNumMin, tRewardItem.wNumMax));

				// �����츳����Ӱ��Ķ��⽱��
				item.dwNum += GetExtraRewardDropItemNum(mItemInfoList, item.dwId, item.dwNum);

				m_pPlayer->AddObjectData(&item, TRUE, FROM_FB_NORMAL_REWARD);

				if (msg.m_wRewardItemNum < MAX_BATTLE_REWARD_ITEM_NUM)
					msg.m_arRewardItemList[msg.m_wRewardItemNum++] = item;
			}
		}

		// ��ʤ����
		if (m_byIsFirstWin && pMapCfg)
		{
			// ��ʤ����Ԫ��
			if (pMapCfg->vFirstWinCurrencyList.size())
				m_pPlayer->ModifyPropertyList(pMapCfg->vFirstWinCurrencyList, TRUE, FROM_HERO_FB_BATTLE);

			// ��ʤ������Ʒ
			if (pMapCfg->vFirstWinItemList.size())
				m_pPlayer->AddObjectList(pMapCfg->vFirstWinItemList, TRUE, FROM_FB_FIRST_WIN_REWARD);
		}

		// ��ʱ����
		{
			// �������
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);

			// ��ӽ�����Ʒ
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);

			// ��ӽ���
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
// 			// �洢ʧ�ܼ�¼
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
	for (BYTE i = 0; i < 5/*Ŀǰ��ʱֻ��Ҫ5������*/; ++i)
	{
		msg.m_arParamList[msg.m_byParamNum++] = GetParam(i);
		//MAPLOG(ERROR_LEVEL, "arParamList[index:%d,value:%d]", i, GetParam(i));
	}
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ��������
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
		MAPLOG(ERROR_LEVEL, "[%s]û�������ȡ������������[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_AWARD_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ��������ʧ��[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	MSG_S2C_FB_AWARD_GET_RESP msg;
	msg.m_byAwardType = byAwardType;
	msg.m_byFBType = byFBType;
	msg.m_wChapter = wChapter;
	msg.m_byIndex = byMapIdx;
	m_pPlayer->SendPacket(&msg);
}

// ɨ��
void CFubenManager::OnNetSweep(BYTE byFBType, WORD wChapter, BYTE byMapIdx)
{
	MSG_S2C_FB_CLEAN_RESP msg;
	msg.m_dwParam1 = wChapter;
	msg.m_dwParam2 = byMapIdx;
	msg.m_byResult = 0;
	msg.m_byFBType = byFBType;
	m_byResult = 0;

	// ���߸���
	if (BATTLE_MAIN_MAP == byFBType)
	{
		// ���VIP�ȼ�
		if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < m_pPlayer->GetVipValue(VIP_MAIN_MAP_CLEAN))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]���߸���ɨ����VIP�ȼ�����", __FUNCTION__);
			return;
		}

		// ��ʼս���߼�
		m_DropItemList.clear();
		if (!m_MainFB.OnBattleStart(wChapter, byMapIdx, 0))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]������ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// ���������Ʒ
		if (m_dwMapId && !MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]����������Ʒʧ��[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}
		
		// ��������Ʒ
		vector<MonsterDropData>::iterator iter = m_DropItemList.begin();
		for (; iter != m_DropItemList.end(); ++iter)
		{
			msg.m_vDropItemList.push_back(Item(iter->dwObjId, iter->wObjNum));
		}

		// ������ʱ������Ʒ
		{
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);
			// ��ӽ�����Ʒ
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);
			// �����ʱ������Ʒ
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				msg.m_vDropItemList.push_back(vLimitRewardItemList[i]);
			}
		}


		// ����ս���߼�
		if (!m_MainFB.OnBattleOver(true, 3))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]��������ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		msg.m_byResult = m_byResult;

		// ��ʱ��ô����
		m_MainFB.SendFBData();
	}
	// ��Ӣ����
	else if (BATTLE_ELITE_MAP == byFBType)
	{
		// ���ؿ��±�
		if (!byMapIdx)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]��Ӣ�����ؿ��±����,ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// ��ʼս���߼�
		m_DropItemList.clear();
		if (!m_EliteFB.OnBattleStart(wChapter, byMapIdx))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]����Ӣ����ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// ���������Ʒ
		if (m_dwMapId && !MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]������Ӣ����������Ʒʧ��[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}

		// ��������Ʒ
		vector<MonsterDropData>::iterator iter = m_DropItemList.begin();
		for (; iter != m_DropItemList.end(); ++iter)
		{
			msg.m_vDropItemList.push_back(Item(iter->dwObjId, iter->wObjNum));
		}

		// ������ʱ������Ʒ
		{
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);
			// ��ӽ�����Ʒ
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);
			// �����ʱ������Ʒ
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				msg.m_vDropItemList.push_back(vLimitRewardItemList[i]);
			}
		}

		// ����ս���߼�
		if (!m_EliteFB.OnBattleOver(true, 3, NULL, 0))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]������Ӣ����ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		msg.m_byResult = m_byResult;

		// ��ʱ��ô����
		m_EliteFB.SendFBData();
	}
	// �ճ��ؿ�
	else if (BATTLE_DAILY_MAP == byFBType)
	{
		// ���ؿ��±�
		if (!byMapIdx)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�ճ������ؿ��±����,ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// ��ʼս���߼�
		m_DropItemList.clear();
		if (!m_DailyFB.OnBattleStart(wChapter, byMapIdx))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�����ճ�����ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		// ���������Ʒ
		if (m_dwMapId && !MonsterDropItem(m_dwMapId, byFBType))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�����ճ�����������Ʒʧ��[MapId:%d]", __FUNCTION__, m_dwMapId);
			return;
		}

		// ��������Ʒ
		vector<MonsterDropData>::iterator iter = m_DropItemList.begin();
		for (; iter != m_DropItemList.end(); ++iter)
		{
			msg.m_vDropItemList.push_back(Item(iter->dwObjId, iter->wObjNum));
		}

		// ������ʱ������Ʒ
		{
			vector<Item> vLimitRewardItemList;
			DoLimitDrop(byFBType, vLimitRewardItemList);
			// ��ӽ�����Ʒ
			if (vLimitRewardItemList.size())
				m_pPlayer->AddObjectList(vLimitRewardItemList, TRUE, FROM_FB_LIMIT_REWARD);
			// �����ʱ������Ʒ
			for (size_t i = 0; i < vLimitRewardItemList.size(); ++i)
			{
				msg.m_vDropItemList.push_back(vLimitRewardItemList[i]);
			}
		}

		// ȡ�½�����
		const FubenChapter_Config *pChapterCfg = CConfigManager::Instance()->GetDailyChapter(wChapter);
		if (!pChapterCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ������½ڸ���[chapter:%d]", __FUNCTION__, wChapter);
			return;
		}

		// ����ս���߼�
		if (!m_DailyFB.OnBattleOver(true, pChapterCfg->dwSweepRewardContext, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�����ճ�����ɨ��ʧ��[type:%d]", __FUNCTION__, byFBType);
			return;
		}

		msg.m_byResult = m_byResult;

		// ��ʱ��ô����
		m_DailyFB.SendDailyData();
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_CLEAN_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޷�ʶ��ĸ���ɨ������[type:%d]", __FUNCTION__, byFBType);
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

// ��������
void CFubenManager::OnNetFBReset(BYTE byFBType, DWORD dwParam1, DWORD dwParam2)
{
	bool bResult;
	switch (byFBType)
	{
	// ���߹ؿ�
	case BATTLE_MAIN_MAP:
		bResult = m_MainFB.OnResetBattleTimes(dwParam1, dwParam2);
		break;

	// ������ͨ�ؿ�
	case BATTLE_TOWER_NORMAL_MAP:
		bResult = m_TowerFB.OnNetFBReset(false);
		break;

	// ��Ӣ�ؿ�
	case BATTLE_ELITE_MAP:
		bResult = m_EliteFB.OnResetBattleTimes(dwParam1, dwParam2);
		break;

	default:
		{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW,C2S_FB_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ĸ�����������[type:%d]", __FUNCTION__, byFBType);
		}
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�������ô���ʧ��[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	MSG_S2C_FB_RESET_RESP msg;
	msg.m_byMapType = byFBType;
	msg.m_dwParam1 = dwParam1;
	msg.m_dwParam2 = dwParam2;
	m_pPlayer->SendPacket(&msg);
}

// һ����ȡ
void CFubenManager::OnNetOneKeyTakeReward(BYTE byFBType, WORD wChapter)
{
	bool bResult;
	switch (byFBType)
	{
		// ���߹ؿ�
	case BATTLE_MAIN_MAP:
		bResult = m_MainFB.OnNetOneKeyTakeReward(wChapter);
		break;

		// ��Ӣ�ؿ�
	case BATTLE_ELITE_MAP:
		bResult = m_EliteFB.OnNetOneKeyTakeReward(wChapter);
		break;

	default:
		{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW,C2S_FB_ONE_KEY_TAKE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ĸ���һ����ȡ����[type:%d]", __FUNCTION__, byFBType);
		}
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FB_ONE_KEY_TAKE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����һ����ȡ����ʧ��[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	// ��Ӧ
	MSG_S2C_FB_ONE_KEY_TAKE_REWARD_RESP msg;
	msg.m_byFBType = byFBType;
	msg.m_wChapter = wChapter;
	m_pPlayer->SendPacket(&msg);
}

// ��ӵ������
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

// ��ӵ����������
bool CFubenManager::AddDropOjectToPlayer(WORD wMapType)
{
	int nMultiple = CActivityManager::GetLimitProductMultiple(wMapType);

	for (int i = 0; i < m_DropItemList.size(); ++i)
	{
		MonsterDropData *pDropData = &m_DropItemList[i];
		m_pPlayer->AddObject(pDropData->dwObjId, pDropData->wObjNum * nMultiple, TRUE, FROM_FB_DROP_ITEM);
	}

	MAPLOG(GUI_LEVEL, "[%s]�ؿ�������Ʒ����[map_id:%d,num:%d]", __FUNCTION__,m_dwMapId, m_DropItemList.size());

	return true;
}



// ���������Ʒ
bool CFubenManager::MonsterDropItem(WORD wMapId, WORD wMapType)
{
	map<DWORD, int> mItemInfoList;
	m_pPlayer->GetSpiritManager()->GetExtraRewardDropItemInfo(wMapType, mItemInfoList);

	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		// �ҵ����߸����ؿ�����
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ�����[MapId:%d]",__FUNCTION__, wMapId);
		return false;
	}
	m_DropItemList.clear();

	// �ؿ�����
	for (size_t i = 0; i < pMapCfg->vDropList.size(); ++i)
	{
		const DropData *pDropItem = &pMapCfg->vDropList[i];
		if (g_Rand() < pDropItem->wRate)
		{
			MonsterDropData tDropData;
			tDropData.dwObjId = pDropItem->dwDropObjectId;
			tDropData.wObjNum = pDropItem->wDropObjectNum;
			tDropData.dwMonsterId = 0;

			// �����츳����Ӱ��Ķ��⽱��
			tDropData.wObjNum += GetExtraRewardDropItemNum(mItemInfoList, tDropData.dwObjId, tDropData.wObjNum);

			m_DropItemList.push_back(tDropData);
		}
	}

	// �������ϵ���
	for (int i = 0; i < pMapCfg->vBattleList.size(); ++i)
	{
		DWORD dwBattleId = pMapCfg->vBattleList[i];
		const Battle_Config *pBattleCfg = CConfigManager::Instance()->GetMapBattle(dwBattleId);
		if (!pBattleCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ս����������[BattleId:%d]",__FUNCTION__, dwBattleId);
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
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[MonsterId:%d]",__FUNCTION__, dwMonsterId);
				return false;
			}

			for (int k = 0; k < pMonsterCfg->vDropList.size(); ++k)
			{
				const DropData &tDropData = pMonsterCfg->vDropList[k];
				if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < tDropData.wRate)
				{
					int nDropNum = tDropData.wDropObjectNum;

					// �����츳����Ӱ��Ķ��⽱��
					nDropNum += GetExtraRewardDropItemNum(mItemInfoList, tDropData.dwDropObjectId, nDropNum);

					AddDropObject(dwMonsterId, tDropData.dwDropObjectId, nDropNum);
				}
			}
		}
	}

	return true;
}

// ������ʱ����
void CFubenManager::DoLimitDrop(DWORD dwFBType, vector<Item> &vRewardItemList)
{
	// ��ʱ����
	const vector<LimitDrop_Config> *pLimitDropList = g_pCfgMgr->GetLimitDropList(dwFBType);
	if (!pLimitDropList)
		return;

	DWORD dwCurDate = g_Time2Date(time(NULL));

	vector<LimitDrop_Config>::const_iterator iter = pLimitDropList->begin();
	for (; iter != pLimitDropList->end(); ++iter)
	{
		// ����Ƿ���ָ��ʱ����
		if (dwCurDate >= iter->dwStartDate && dwCurDate <= iter->dwOverDate)
		{
			// ����Ƿ�����
			if (g_Rand() > iter->dwDropRate)
				continue;

			// �������
			DWORD dwItemNum = g_Rand(iter->dwDropNumMin, iter->dwDropNumMax);

			CItemManager::UseRandItem(iter->dwDropItemId, dwItemNum, vRewardItemList);
		}
	}
}

// ��ӹ�������
void CFubenManager::AddMonsterData(const MonsterData &tMonster)
{
	m_MonsterList.push_back(tMonster);
}

// ���Ӣ������
void CFubenManager::SetBattleTargetData(const vector<ToBattleHeroData> &vTargetHeroList, const vector<SpiritData> &vTargetSpriteList)
{
	m_TargetHeroList = vTargetHeroList;
	m_vTargetSpriteList = vTargetSpriteList;
}

// ����Ƿ���ս����
bool CFubenManager::CheckIsInBattle()
{
	// ����Ƿ�����ս״̬
	if (!m_lStartBattleTime || GetTickCount64() > m_lStartBattleTime + 1000 * 60 * 30)
	{
		return false;
	}
	return true;
}

// ����ս����Ϣ
void CFubenManager::SetBattleInfo(WORD wChapter, BYTE byMapLv, DWORD dwMapId, DWORD dwHelpBattleTargetId, DWORD dwHelpBattleHeroInfoId)
{
	m_byFBType = BATTLE_TYPE_INVALID;	// ��ǰ��ս�ĸ�������
	m_wCurChapter = wChapter;			// ��ǰ��ս���½�
	m_byCurMapIdx = byMapLv;				// ��ǰ��ս�ĵȼ�
	m_dwMapId = dwMapId;
	m_lStartBattleTime = GetTickCount64();	// ��ʼս��ʱ��
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

// ����ͨ�ý���
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

	// ���߸�������Ӣ������Ӣ�۸���
	if (BATTLE_MAIN_MAP == nMapType || BATTLE_ELITE_MAP == nMapType || BATTLE_HERO_MAP == nMapType)
	{
		// ��þ��� = ����ȼ�*2*��������
		// ��ý�� = ����ȼ� * 5 * �������� + 100
		int nCostStamina = pMapCfg->GetCostValue(ROLE_PRO_STAMINA);
		DWORD dwExp = wLevel * 2 * nCostStamina;
		int nGold = wLevel * 5 * nCostStamina + 100;

		// todo::����ͨ�ؽ����Ľ��*10 996bt
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
			nGold *= 10;

		m_pPlayer->AddExp(dwExp);
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, wFromId);
	}
	// �ᱦ��������
	else if (BATTLE_DOUBAO_MAP == nMapType || BATTLE_ARENA_MAP == nMapType)
	{
		// ��þ��� = ����ȼ�*2*���ľ���
		// ��ý�� = ����ȼ� * 10 * ���ľ��� + 200
		int nCostEnergy = pMapCfg->GetCostValue(ROLE_PRO_ENERGY);
		DWORD dwExp = wLevel * 2 * nCostEnergy;
		INT nGold = wLevel * 10 * nCostEnergy + 200;

		// todo::����ͨ�ؽ����Ľ��*10 996bt
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
			nGold *= 10;

		m_pPlayer->AddExp(dwExp);
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, wFromId);
	}
	// �ճ�����
	else if (BATTLE_DAILY_MAP == nMapType)
	{
		
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ĺؿ���ý���[MapType:%d]", __FUNCTION__, nMapType);
		return;
	}

	// ��ͨ���ҽ���
	m_pPlayer->ModifyPropertyList(pMapCfg->vNormalRewardList, TRUE, wFromId);

	// ��ӵ�����Ʒ
	AddDropOjectToPlayer(nMapType);
}

// �������Ӣ������
void CFubenManager::AddHelpBattleHeroPro(const Property &tPro)
{
	m_vHelpBattleHeroProList.push_back(tPro);
}