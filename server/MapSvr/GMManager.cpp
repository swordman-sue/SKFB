#include "stdafx.h"
#include "GMManager.h"
#include "Const.h"
#include "Player.h"
#include "Shop\ShopManager.h"
#include "EquipmentManager.h"
#include "Hero\HeroManager.h"
#include "Item\ItemManager.h"
#include "Treasure\TreasureManager.h"
#include "Rebel\RebelManager.h"
#include "Rank\RankManager.h"
#include "Fuben\FubenManager.h"
#include "ToolKit\Rand.h"
#include "Spirit\SpiritManager.h"
#include "Land\LandManager.h"
#include "Arena\ArenaManager.h"
#include "Guild\GuildManager.h"
#include "Vip\VipManager.h"
#include "DailyMission\DailyMissionManager.h"
#include "WorldBoss\WorldBossManager.h"
#include "Logic\LogicManager.h"
#include "SevenDayActivity\SevenDayManager.h"
#include "ConfigManager.h"
#include "MapSvrQuery.h"
#include "Email\EmailManager.h"
#include "RewardCenter\RewardCenterManager.h"
#include "CommonLogic.h"
#include "Title\TitleManager.h"
#include "MapServer.h"
#include "PlayerBaseDataManager.h"
#include "Activity\ActivityManager.h"
#include "SystemNotice\SystemNotice.h"
#include "Property\PropertyManager.h"
#include "RedPoint\RedPointManager.h"
#include "OpenServerSession.h"
#include "Recharge\RechargeManager.h"
#include "PVP\PVPManager.h"
#include "AttackCity\AttackCityManager.h"
#include "EndlessLand\EndlessLandManager.h"
#include "AccountInfoManager.h"
#include "Gem\GemManager.h"
#include "TeamDungeon\TeamDungeonManager.h"
#include "KingMatch\KingMatchManager.h"
#include "ZhanBu\ZhanBuManager.h"
#include "Activity\Christmas.h"
#include "ScoreMatch\ScoreMatchManager.h"
#include "ResourceWar\ResourceWarManager.h"
#include "Guild\GuildWarMgr.h"
#include "HangUpDrop\HangUpDropMgr.h"
//#define  cSplit  ","

GMManager::GMManager()
{
	DWORD dwCurTime = time(NULL);
	m_dwProcessNoticeTime = dwCurTime;
	m_dwProceesTime = dwCurTime;
	m_dwProcessEmailTime = dwCurTime;

	m_dwCPOrderId = 0;
	m_dwSendNoticeInterval = 10; // Ĭ��10��
}

GMManager::~GMManager()
{

}

BOOL GMManager::Init()
{
	DWORD dwCurTime = time(NULL);
	m_dwCPOrderId = 0;
	m_dwProcessNoticeTime = dwCurTime;
	m_dwProceesTime = dwCurTime;
	m_dwProcessEmailTime = dwCurTime;

	m_dwActivePlayerLimitTime = g_GetConfigValue(255) * 24 * 60 * 60; // ת��Ϊ�뵥λ
	m_dwSendNoticeInterval = 10; // Ĭ��10��
	return TRUE;
}

VOID GMManager::Release()
{

}

void GMManager::Update()
{
	DWORD dwCurTime = time(NULL);

	// 10�봦��һ��
	if (dwCurTime >= m_dwProceesTime + 10)
	{
		// ����
		GMEventList_Load* pQuery = GMEventList_Load::ALLOC();
		pQuery->SetIndex(DB_GM_EVENT_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_GM);
		g_MapServer.MapDBQuery(pQuery);

		m_dwProceesTime = dwCurTime;
	}

	// һ��ֻ����һ����������
	if (m_vSystemRewardList.size())
	{
		list<SystemReward>::iterator server_iter =  m_vSystemRewardList.begin();
		const PlayerBaseDataMap *pBaseDataList = PlayerBaseDataManager::Instance()->GetPlayerList(server_iter->dwServerId);
		if (!pBaseDataList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������б�����[ServerId:%d]", __FUNCTION__, server_iter->dwServerId);
			return;
		}

		DWORD dwRewardId = server_iter->tRewardData.wInfoId;
		const Reward_Config *pRewardCfg = g_pCfgMgr->GetReward(dwRewardId);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������������[reward_id:%d]", __FUNCTION__, dwRewardId);
			return;
		}

		// �޶���Ч����ʱ��
		DWORD dwLimitInvalidOfflineTime = g_GetConfigValue(255) * 24 * 60 * 60; // ת��Ϊ�뵥λ
		//DWORD dwCurTime = time(NULL);

		WORD wCurServerId = g_MapServer.GetCurServerId(server_iter->dwServerId);

		DWORD dwSendPlayerNum = 0;
		std::hash_map<DWORD, PlayerBaseData*>::const_iterator iter = pBaseDataList->begin();
		for (; iter != pBaseDataList->end(); ++iter)
		{
			// �����޶�ʱ���򲻴���
			if (iter->second->dwOfflineTime && dwCurTime >= iter->second->dwOfflineTime + dwLimitInvalidOfflineTime)
			{
				// ��λBUG����
// 				DATETIME datetime(time_t(iter->second->dwOfflineTime));
// 				char szStrTime[32] = { 0 };
// 				datetime.toString(szStrTime);
// 				MAPLOG(ERROR_LEVEL, "û���յ����������[server_id:%d, reward_id:%d, role_id:%d, server_id:%d,OfflineTime:%d,StrTime:%s]",
// 					server_iter->dwServerId, dwRewardId, iter->second->dwRoleId, iter->second->wServerId, iter->second->dwOfflineTime, szStrTime);
				continue;
			}
				

			CRewardCenterManager::AddReward(wCurServerId, iter->second->dwRoleId, server_iter->tRewardData);

			MAPLOG(ERROR_LEVEL, "[%s]����ҷ��Ž���[server_id:%d, reward_id:%d, role_id:%d, server_id:%d]", __FUNCTION__,
				server_iter->dwServerId, dwRewardId, iter->second->dwRoleId, iter->second->wServerId);

			dwSendPlayerNum++;
		}

		MAPLOG(ERROR_LEVEL, "[%s]����������ҷ��Ž���[server_id:%d, reward_id:%d, total_role_num:%d, send_role_num:%d]", __FUNCTION__,
			server_iter->dwServerId, dwRewardId, pBaseDataList->size(), dwSendPlayerNum);

		m_vSystemRewardList.pop_front();
	}

	// �����ʼ�
	ProcessSendEmail();

	// ������
	if (m_vSystemNoticeList.size())
	{
		// 10��
		if (dwCurTime >= m_dwProcessNoticeTime + m_dwSendNoticeInterval)
		{
			m_dwProcessNoticeTime = dwCurTime;
			list<SystemNoticeInfo>::iterator iter = m_vSystemNoticeList.begin();

			CSystemNotice sys_notice(iter->wNoticeId, iter->wServerId);
			sys_notice.SetText(iter->szText);
			sys_notice.SendMsg();

			iter->nNoticeTimes--;

			// �ѹ㲥��,��ɾ��
			if (iter->nNoticeTimes <= 0)
				m_vSystemNoticeList.pop_front();
		}
	}
}

// �����ʼ�
void GMManager::ProcessSendEmail()
{
	DWORD dwCurTime = time(NULL);

	// һ��ֻ����һ����������(10�봦��һ��)
	if (m_vGMSendEmailList.size() && dwCurTime >= m_dwProcessEmailTime + 10)
	{
		list<GMSendEmailInfo>::iterator server_iter = m_vGMSendEmailList.begin();
		DWORD dwServerId = server_iter->dwServerId;
		const PlayerBaseDataMap *pBaseDataList = PlayerBaseDataManager::Instance()->GetPlayerList(dwServerId);
		if (!pBaseDataList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������б�����[ServerId:%d]", __FUNCTION__, dwServerId);
			m_vGMSendEmailList.pop_front();
			return;
		}

		EmailInfo tEmailInfo;
		tEmailInfo.CleanUp();
		tEmailInfo.dwID = EmailManager::GetEmailId();
		tEmailInfo.dwReceiveTime = time(NULL);
		strcpy_s(tEmailInfo.szText, sizeof(tEmailInfo.szText), server_iter->szText);
		strcpy_s(tEmailInfo.szTitle, sizeof(tEmailInfo.szTitle), server_iter->szTitle);

		std::hash_map<DWORD, PlayerBaseData*>::const_iterator iter = pBaseDataList->begin();
		for (; iter != pBaseDataList->end(); ++iter)
		{
			// �����޶�ʱ���򲻴���
			if (iter->second->dwOfflineTime && dwCurTime >= iter->second->dwOfflineTime + m_dwActivePlayerLimitTime)
				continue;

			EmailManager::AddEmailByGM(iter->second->dwRoleId, tEmailInfo);
		}

		m_vGMSendEmailList.pop_front();
	}
}

// ����ϵͳ����
void GMManager::ProcessSystemReward()
{
	
}

void GMManager::OnNetGMMsg(Player *pPlayer, WORD wCmdId, DWORD dwValue, DWORD dwValue1, DWORD dwValue2)
{
	if (!g_MapServer.IsOpenGM())
	{
		MAPLOG(GUI_LEVEL, "GM��ǰľ�п���[role_id:%d,cmd_id:%d]", pPlayer->GetRoleId(), wCmdId);
		return;
	}

	switch (wCmdId)
	{
	case GM_SET_LEVEL:						//1.���õȼ�
	{
		if (dwValue)
			pPlayer->SetProperty(ROLE_PRO_LEVEL, min(MAX_ROLE_LEVEL,dwValue));
	}
	break;
	case GM_ADD_EXP:						//2.��Ӿ���
	{
		pPlayer->AddExp(dwValue);
	}
	break;
	case GM_SET_VIP:						//3.����VIP�ȼ�
	{
		int nVipLevel = pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);
		if (dwValue + nVipLevel >= MAX_VIP_LEVEL)
		{
			dwValue = MAX_VIP_LEVEL - nVipLevel;
		}
		pPlayer->ModifyProperty(ROLE_PRO_VIP_LEVEL, dwValue);
	}
	break;
	case GM_SET_GOLD:						//4.���ý��
	{
		pPlayer->SetProperty(ROLE_PRO_GOLD, dwValue);
	}
	break;
	case GM_SET_STAMINA:					//5 ��������
	{
		pPlayer->SetProperty(ROLE_PRO_STAMINA, dwValue);
	}
	break;
	case GM_SET_DIAMOND:					//6 ������ʯ
	{
		pPlayer->SetProperty(ROLE_PRO_DIAMOND, dwValue);
	}
	break;
	case GM_SET_ENERGY:						//7 ���þ��� 
	{
		pPlayer->SetProperty(ROLE_PRO_ENERGY, dwValue);
	}
	break;
	case GM_ADD_HERO:						//8 ���Ӣ�� 
	{
		for (DWORD i = 0; i < dwValue1; ++i)
		{
			pPlayer->GetHeroManager()->AddHero(dwValue, TRUE, FROM_GM);
		}
	}
	break;
	case GM_ADD_EQUIPMENT:					//9 ���װ�� 
	{
		for (DWORD i = 0; i < dwValue1; ++i)
		{
			pPlayer->GetEquipmentManager()->AddEquipent(dwValue, TRUE, FROM_GM);
		}	
	}
	break;
	case GM_ADD_ITEM:					//9 �����Ʒ 
	{
		dwValue1 = min(dwValue1, 999);
		dwValue1 = max(dwValue1, 1);
		pPlayer->AddObject(dwValue, dwValue1, TRUE, 9999);
	}
	break;
	case  GM_ADD_TREASURE:
	{
		for (DWORD i = 0; i < dwValue1; ++i)
		{
			pPlayer->GetTreasureManager()->AddTreasure(dwValue, true, FROM_GM);
		}	
	}
	break;
	case  GM_ADD_REBEL_VALUE:
	{
		dwValue1 = min(dwValue1, 65535);
		pPlayer->ModifyProperty(ROLE_PRO_REBEL_VALUE, dwValue);
	}
	break;
	case  GM_ADD_REBEL_DAMAGE:
	{
		pPlayer->GetRoleFullInfo().tRebelInfo.dwTodayDamage += dwValue;
		CRebelManager::Instance()->OnRebelDataReq(pPlayer);
		CRankManager::Instance()->AddRoleRankData(RANK_REBEL_DAMAGE, pPlayer,
			pPlayer->GetRoleFullInfo().tRebelInfo.dwTodayDamage, 0,0, 200);
	}
	break;
	case  GM_ADD_REBEL_EXPLOIT:
	{
		pPlayer->GetRoleFullInfo().tRebelInfo.dwTodayExploit += dwValue;
		CRebelManager::Instance()->OnRebelDataReq(pPlayer);	
		CRankManager::Instance()->AddRoleRankData(RANK_REBEL_EXPLOIT, pPlayer, 
			pPlayer->GetRoleFullInfo().tRebelInfo.dwTodayExploit, 0,0,200);
	}
	break;
	case  GM_ADD_TOWER_PRESTIGE:
	{
		pPlayer->ModifyProperty(ROLE_PRO_TOWER_PRESTIGE, dwValue);
	}
	break;
	case  GM_ADD_TOWER_STAR:
	{
		TowerFBData &tFBData = pPlayer->GetRoleFullInfo().tFubenInfo.tTowerFubenData;// .wStarMaxRecord += dwValue;
		tFBData.wStarMaxRecord += dwValue;
		pPlayer->GetFubenManager()->GetTower().SendTowerData();
		CRankManager::Instance()->AddRoleRankData(RANK_TOWER, pPlayer,tFBData.wStarMaxRecord, 0, 0, 20);
	}
	break;
	case GM_ADD_REBEL:
	{
		CRebelManager::Instance()->CreateRebel(pPlayer);
// 		if (CRebelManager::Instance()->GetSelfRebelId(pPlayer->GetCharID()))
// 		{
// 			return;
// 		}
// 
// 		while (true)
// 		{
// 			if (CRebelManager::Instance()->CreateRebel(pPlayer))
// 				break;
// 		}
	}
	break;
	case GM_ADD_LAND_PATROL_TIME:
	{
		PlayerLandInfo *pLandInfo = CLandManager::Instance()->GetPlayerLandInfoEx(pPlayer->GetCharID());
		if (pLandInfo)
		{
			pLandInfo->dwPatrolTotalTime += dwValue;
			CLandManager::Instance()->OnNetPlayerLandData(pPlayer, pPlayer->GetCharID());
		}
	}
	break;
	case GM_ADD_ARENA_PRESTIGE:
	{
		pPlayer->ModifyProperty(ROLE_PRO_ARENA_PRESTIGE, dwValue);
	}
	break;
	case GM_ADD_GUILD_QS:
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
		if (pGuild)
		{
			pGuild->AddQSValue(dwValue);
			pGuild->SendGuildData(pPlayer);
		}
	}
	break;
	case GM_ADD_GUILD_CONTRIBUTION:
	{
		pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, dwValue);
	}
	break;
	case GM_ADD_VIP_EXP:
	{
		pPlayer->GetVipManager()->AddExp(dwValue);
	}
	break;
	case GM_ADD_DAILY_MISSION_POINT:
	{
		pPlayer->GetRoleFullInfo().tDailyMissionInfo.wPoint += dwValue;
		pPlayer->GetDailyMissionManager()->SendMissionList();
	}
	break;
	case GM_ADD_ACHIEVEMENT_VALUE:
	{
		for (DWORD i = 0; i < dwValue1; ++i)
		{
			pPlayer->OnUpdateAchievement(dwValue);
		}
	}
	break;
	case GM_ADD_HERO_SOUL:
	{
		pPlayer->ModifyProperty(ROLE_PRO_HERO_SOUL, dwValue);
	}
	break;
	case GM_WORLD_BOSS_START:
	{
		if (!CWorldBossManager::Instance()->IsActivity())
		{
			CWorldBossManager::Instance()->ActivityStart();
			CWorldBossManager::Instance()->SetGM(true);
		}
	}
	break;
	case GM_WORLD_BOSS_OVER:
	{
		if (CWorldBossManager::Instance()->IsActivity())
		{
			CWorldBossManager::Instance()->ActivityOver();
		}
	}
	break;
	case GM_ADD_GUILD_EXP:
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
		if (pGuild)
		{
			pGuild->AddExp(dwValue);
		}
	}
	break;
	//29 ����BOSS�ȼ�
	case GM_SET_BOSS_LEVEL:						
	{
		CWorldBossManager::Instance()->SetBossLevel(pPlayer->GetServerId(), dwValue);
		CWorldBossManager::Instance()->OnNetWorldBossData(pPlayer);
	}
	break;	
	//30 ����BOSS����
	case GM_ADD_BOSS_HONOR:						
	{
		CWorldBossManager::Instance()->AddRankDataEX(pPlayer, RANK_WORLD_BOSS_HONOR, dwValue);
		CWorldBossManager::Instance()->OnNetWorldBossData(pPlayer);
	}
	break;	
	//31 ����BOSS�˺�
	case GM_ADD_BOSS_DAMAGE:						
	{
		CWorldBossManager::Instance()->AddRankDataEX(pPlayer, RANK_WORLD_BOSS_DAMAGE, dwValue);
		CWorldBossManager::Instance()->OnNetWorldBossData(pPlayer);
	}
	break;
	//31 ͨ���������߸����ؿ�
	case GM_PASS_ALL_MAIN_MAP:
	{
		if (!dwValue)
			dwValue = g_pCfgMgr->m_MainFBChapterList.size();
		dwValue = min(dwValue, g_pCfgMgr->m_MainFBChapterList.size());
		MainFBData *pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tMainFubenData;
		memset(pFBData, 0, sizeof(MainFBData));
		pFBData->wChapter = 0;
		for (DWORD i = 0; i <= dwValue; ++i)
		{
			const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetMainFBChapter(i);
			if (!pChapterCfg)
				continue;

			ChapterData *pChapter = &pFBData->arChapterList[pFBData->wChapter++];
			pChapter->byMapIdx = 0;
			memset(pChapter->arStarAwardList, 0, sizeof(pChapter->arStarAwardList));
			for (size_t i = 0; i < pChapterCfg->vMapList.size(); ++i)
			{
				MapData *pMapData = &pChapter->arMapList[pChapter->byMapIdx++];
				memset(pMapData, 0, sizeof(MapData));
				pMapData->byStar = 3;
			}
		}
// 		map<DWORD, FubenChapter_Config>::iterator iter = g_pCfgMgr->m_MainFBChapterList.begin();
// 		for (; iter != g_pCfgMgr->m_MainFBChapterList.end(); ++iter)
// 		{
// 			ChapterData *pChapter = &pFBData->arChapterList[pFBData->wChapter++];
// 			pChapter->byMapIdx = 0;
// 			memset(pChapter->arStarAwardList, 1, sizeof(pChapter->arStarAwardList));
// 			for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
// 			{
// 				MapData *pMapData = &pChapter->arMapList[pChapter->byMapIdx++];
// 				memset(pMapData, 0, sizeof(MapData));
// 				pMapData->byStar = 3;
// 			}
// 		}
		pPlayer->GetFubenManager()->GetMainFB().SendFBData();
	}
	break;
	//33 ����������ͨ�ؿ�ͨ����
	case GM_SET_PASS_TOWER_NORMAL_MAP_IDX:
	{
		if (!dwValue)
			return;
		dwValue = min(g_pCfgMgr->m_TowerChapterList.size()*3, dwValue);
		TowerFBData *pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tTowerFubenData;
		pFBData->wCurMapLevel = 0;
		for (int i = 0; i < dwValue; ++i)
		{
			pFBData->arMapStarList[pFBData->wCurMapLevel++] = 3;
		}
		pFBData->wMapLevelRecord = max(pFBData->wCurMapLevel, pFBData->wMapLevelRecord);
		BYTE byKeepThreeStarPassedRecord = pFBData->wCurMapLevel / 3;
		pFBData->byKeepThreeStarPassedRecord = max(byKeepThreeStarPassedRecord, pFBData->byKeepThreeStarPassedRecord);
		WORD wTotalStar = pFBData->wCurMapLevel * 3;
		pFBData->wStarMaxRecord = max(wTotalStar, pFBData->wStarMaxRecord);
		pPlayer->GetFubenManager()->GetTower().SendTowerData();
	}
	break;
	//34 ����������Ӣ�ؿ�ͨ����
	case GM_SET_PASS_TOWER_ELITE_MAP_IDX:
	{
		if (!dwValue)
			return;
		dwValue = min(g_pCfgMgr->m_TowerEliteMapList.size(), dwValue);
		TowerFBData *pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tTowerFubenData;
		pFBData->wEliteMapLevel = dwValue;
		pPlayer->GetFubenManager()->GetTower().SendTowerData();
	}
	break;
	case GM_ADD_TITLE:
	{
		pPlayer->GetTitleManager()->AddTitle(dwValue,time(NULL));
		pPlayer->GetTitleManager()->SendRoleTitleInfo();
	}
	break;
	case GM_USE_TITLE:
	{
		pPlayer->GetTitleManager()->OnNetUseTitle(dwValue);
	}
	break;
	case GM_ADD_GOD_SOUL:
	pPlayer->ModifyProperty(ROLE_PRO_GOD_SOUL, dwValue);
	break;
	case GM_SET_TOWER_RESET_TIMES:
	{
		pPlayer->GetRoleFullInfo().tFubenInfo.tTowerFubenData.byTodayResetTimes = 0;
		pPlayer->GetFubenManager()->GetTower().SendTowerData();
	}
	break;
	case GM_SET_HERO_FB_CHAPTER:
	{
		if (!dwValue)
			dwValue = g_pCfgMgr->m_HeroFBChapterList.size();
		dwValue = min(dwValue, g_pCfgMgr->m_HeroFBChapterList.size());
		HeroFBData *pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tHeroFubenData;
		memset(pFBData, 0, sizeof(HeroFBData));
		pFBData->wChapter = 0;
		for (DWORD i = 0; i <= dwValue; ++i)
		{
			const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetHeroFBChapter(i);
			if (!pChapterCfg)
				continue;

			HeroFubenChapterData *pChapter = &pFBData->arChapterList[pFBData->wChapter++];
			pChapter->byMapIdx = 0;
			pChapter->byIsExtraMapPass = false;
			for (size_t i = 0; i < pChapterCfg->vMapList.size(); ++i)
			{
				HeroMapData *pMapData = &pChapter->arMapList[pChapter->byMapIdx++];
				memset(pMapData, 0, sizeof(MapData));
				pMapData->byBattleTimes = 1;
			}
		}
// 		HeroFBData &tFBData = pPlayer->GetRoleFullInfo().tFubenInfo.tHeroFubenData;
// 		memset(&tFBData, 0, sizeof(tFBData));
		pPlayer->GetFubenManager()->GetHeroFB().SendFBData();
	}
	break;
	case GM_RESET_HERO_FB_BATTLE_TIMES:
	{
		pPlayer->GetRoleFullInfo().tFubenInfo.tHeroFubenData.byBattleTimes = 0;
		pPlayer->GetFubenManager()->GetHeroFB().SendFBData();
	}
	break;
	case GM_PRINT_HERO_PRO:
		pPlayer->GetPropertyManager()->PrintHeroPro();
	break;
	case GM_SET_DAILY_FB_BATTLE_TIMES:
	{
		DailyFBData *pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tDailyFubenData;
		for (WORD i = 0; i < pFBData->wChapterNum; ++i)
		{
			pFBData->arChapterList[i].byBattleTimes = 0;
		}
	}
	break;
	case GM_PRINT_RED_PIONT_STATUS:
	{
		pPlayer->GetRedPointManager()->PrintRedPointStatus();
	}
	break;
	case GM_ELITE_FB_ENEMY_REFRESH:
	{
		pPlayer->GetFubenManager()->GetEliteFB().CreateEnemy();
	}
	break;
	case GM_SET_PASS_ELITE_MAP:
	{
		if (!dwValue)
		dwValue = min(dwValue, g_pCfgMgr->m_DailyChapterList.size());
		EliteFBData *pFBData = &pPlayer->GetRoleFullInfo().tFubenInfo.tEliteFBData;
		memset(pFBData, 0, sizeof(EliteFBData));
		pFBData->wChapter = 0;
		for (DWORD i = 0; i <= dwValue; ++i)
		{
			const FubenChapter_Config *pChapterCfg = g_pCfgMgr->GetElitFBeChapter(i);
			if (!pChapterCfg)
				continue;

			EliteFBChapter *pChapter = &pFBData->arChapterList[pFBData->wChapter++];
			pChapter->byMapIdx = 0;
			memset(pChapter->arStarAwardList, 0, sizeof(pChapter->arStarAwardList));
			for (size_t i = 0; i < pChapterCfg->vMapList.size(); ++i)
			{
				MapData *pMapData = &pChapter->arMapList[pChapter->byMapIdx++];
				memset(pMapData, 0, sizeof(MapData));
				pMapData->byStar = 3;
			}
			pChapter->tEnemyData.wMapId = 0;
			pChapter->tEnemyData.wIsDead = 0;
		}		
	}
	break;
	case GM_ADD_SPIRIT:
		if (pPlayer->GetSpiritManager()->AddSpirit(dwValue, FROM_GM))
			pPlayer->GetRoleFullInfo().tSpiritInfo.dwCurUseSpiritId = dwValue;
		break;
	case GM_SET_FRIENDSHIP_POINT:
		pPlayer->ModifyProperty(ROLE_PRO_FRIENDSHIP_PIONT, min(dwValue, MAX_MONEY));
		break;
	case GM_RESET_GUILD_FB_BATTLE_TIMES:
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
		if (pGuild)
			pGuild->ResetBattleTimes(pPlayer);
	}
	break;
	case GM_RESET_GUILD_QY_TIMES:
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
		if (pGuild)
			pGuild->ResetQYTimes(pPlayer);
	}
	break;
	case GM_RESET_KICK_OUT:
	{
		MSG_M2G_CLIENT_KICKOUT_CMD msg;
		msg.m_dwErrorCode = 0;
		pPlayer->SendPacket(&msg);
	}
	break;
	case GM_REFRESH_WEEK_GIFT:
	{
		pPlayer->GetActivityManager()->RefreshWeekGift();
	}
	break;
	case GM_MONTH_CARD_BUY:
	{
		pPlayer->GetRechargeManager()->OnNetMonthCardBuy(dwValue);
	}
	break;
	case GM_RECHARGE:
	{
		pPlayer->GetRechargeManager()->OnNetRecharge(dwValue, dwValue1, dwValue2);
	}
	break;
	case GM_RESET_ELITE_FB_BATTLE_TIMES:
	{
		EliteFBData &tFBData = pPlayer->GetRoleFullInfo().tFubenInfo.tEliteFBData;
		for (WORD i = 0; i < tFBData.wChapter; ++i)
		{
			EliteFBChapter &tChapter = tFBData.arChapterList[i];
			for (WORD k = 0; k < tChapter.byMapIdx; ++k)
			{
				tChapter.arMapList[k].byResetTimes = 0;
				tChapter.arMapList[k].byBattleTimes = 0;
			}
		}
		pPlayer->GetFubenManager()->GetEliteFB().SendFBData();
	}
	break;
	case GM_SET_ENDLESS_STONE:
	pPlayer->ModifyProperty(ROLE_PRO_ENDLESS_STONE, dwValue);
	break;
	case GM_RESET_ENDLESS_DATA:
		{
			PlayerEndlessLandData &tEndlessLandData = pPlayer->GetRoleFullInfo().tSecondInfo.tEndlessLandData;
			memset(&tEndlessLandData, 0, sizeof(tEndlessLandData));
			tEndlessLandData.wCurWave = 1;
			tEndlessLandData.wTodayBattleTimes = true;
			pPlayer->GetEndlessLandManager()->SendEndlessLandData();
			CRankManager::Instance()->ResetRoleRank(pPlayer->GetServerId(), RANK_ENDLESS_KILL);
		}
		break;
	case GM_SUPER_DISCOUNT_RECHARGE:
		pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();
		break;
	case GM_NEW_SUPER_DISCOUNT_RECHARGE:
		pPlayer->GetActivityManager()->GetNewSuperDiscount().OnRechargeCallback();
		break;
	case GM_SIGNIN:
		pPlayer->GetActivityManager()->OnNetSignInForTest();
		break;
	case GM_REMOVE_ROLE_RANK:
		{
			DWORD dwRankType = dwValue;
			DWORD dwServerId = pPlayer->GetServerId();
			DWORD dwRoleId = dwValue1;
			CRankManager::Instance()->RemoveRoleRankData(dwRankType, dwServerId, dwRoleId);
		}
		break;
	case GM_UPDATE_ROLE_RANK:
		CRankManager::Instance()->UpdateRoleRankEx(dwValue, dwValue1);
		break;
	case GM_ADD_GEM:
		pPlayer->GetGemManager()->AddGem(dwValue, min(max(dwValue1,1),99999999));
		break;
	case GM_KILL_ATTACK_CITY_MONSTER:
		CAttackCityManager::Instance()->OnKillCurAllMonster(pPlayer->GetServerId());
		break;
	case GM_SET_KING_MATCH_SEASON:
		CKingMatchManager::Instance()->SetCurSeason(dwValue);
		break;
	case GM_SET_KING_MATCH_STAGE:
		CKingMatchManager::Instance()->SetStageForTest(pPlayer, dwValue);
		break;
	case GM_SET_GEM_CION:
		pPlayer->ModifyProperty(ROLE_PRO_GEM_COIN, dwValue);
		break;
	case GM_PRINT_LAND_REWARD:
		CLandManager::Instance()->PrintPatrolReward(pPlayer, dwValue);
		break;
	case GM_ADD_CARD:
		pPlayer->GetZhanBuManager()->AddCard(dwValue, TRUE, FROM_GM);
		break;
	case GM_ADD_DESTINY_COIN:
		pPlayer->ModifyProperty(ROLE_PRO_DESTINY_COIN, dwValue);
		break;
	case GM_ADD_STAR_SOUL:
		pPlayer->ModifyProperty(ROLE_PRO_STAR_SOUL, dwValue);
		break;
	case GM_ADD_COLLETION_VALUE:
		{
			pPlayer->GetRoleFullInfo().tZhanBuInfo.dwHistoryCollectionValue += dwValue;
			pPlayer->GetZhanBuManager()->OnNetPlayerZhanBuData();
		}
		break;
	case GM_ADD_CHRISTMAS_SCORE:
		{
			PlayerChristmasData &tChristmasData = pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;
			tChristmasData.dwTotalScore += dwValue;
			tChristmasData.dwCurScore += dwValue;
			CChristmas::Instance()->SendChristmasData(pPlayer);
		}
		break;
	case GM_SET_HEARTOFWORLD:
		{
			DWORD dwMaxStage = CConfigManager::Instance()->GetSpiritStageMax();
			dwValue = min(dwMaxStage, dwValue);
			// ȡ��ǰ����������
			const HeartOfWorld_Config *pSpiritCfg = CConfigManager::Instance()->GetHeartOfWorld(dwValue);
			if (!pSpiritCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[stage:%d]", __FUNCTION__, dwValue);
				return;
			}
			dwValue1 = min(dwValue1, pSpiritCfg->vLeveDatalist.size());
			HeartOfWorld &tHeartOfWorld = pPlayer->GetRoleFullInfo().tSpiritInfo.tHeartOfWorld;
			tHeartOfWorld.wStage = dwValue;
			tHeartOfWorld.wLevel = dwValue1;
			pPlayer->UpdateFightingPower();
			pPlayer->GetSpiritManager()->SendHeartOfWorldData();
		}
		break;

	case GM_ADD_OCCUPY_TIME:
		CResourceWarManager::Instance()->AddOccupyTimeForGM(pPlayer, dwValue);
		break;

	case GM_RESET_TOWER:
		pPlayer->GetFubenManager()->GetTower().OnNetFBReset(true);
		break;

	case GM_BUY_RECHARGE:
		pPlayer->GetRechargeManager()->OnNetRechargeEx(dwValue, dwValue1, dwValue2, 0);
		break;

	case GM_ADD_YCOIN:
		pPlayer->ModifyProperty(ROLE_PRO_YCOIN, dwValue);
		break;

	case GM_DELETE_ITEM:
		pPlayer->RemoveItem(dwValue, dwValue1, FROM_GM);
		break;

	case GM_NEW_DAY:
		pPlayer->OnNewDay();
		break;

	case GM_NEW_WEEK:
		pPlayer->OnNewWeek();
		break;

	case GM_SET_PVP_SCORE:
		CPVPManager::Instance()->ModifyScoreForGM(pPlayer, dwValue, dwValue1);
		break;

	case GM_MY_TEST:
		pPlayer->GetHangUpDropManager()->OnNetObtainReward();
		break;

	case GM_ACCUPY_STAR:
		CGuildWarMgr::Instance()->OnOccupyStarForGM(pPlayer, dwValue);
		break;

	case GM_ADD_RECHARGE_AMOUNT:
		pPlayer->SetProperty(ROLE_PRO_RECHARGE_AMOUNT, pPlayer->GetProperty(ROLE_PRO_RECHARGE_AMOUNT) + dwValue);
		break;

	default:
		MAPLOG(ERROR_LEVEL, "û�����GM����[cmd:%d]", wCmdId);
		return;
	}

	MAPLOG(GUI_LEVEL, "GM����ִ�гɹ�[cmd_id:%d,value1:%d, value2:%d]", wCmdId, dwValue, dwValue1);
}



void GMManager::OnCmdFromDB(WORD wCmdId, const DWORD *pParamList, int nParamNum, const char *pszText, const char *pszText1)
{
	DWORD dwParam1 = pParamList[0];
	DWORD dwParam2 = pParamList[1];
	DWORD dwParam3 = pParamList[2];
	DWORD dwParam4 = pParamList[3];
	DWORD dwParam5 = pParamList[4];

	MAPLOG(ERROR_LEVEL, "�յ�GM�¼�[cmd_id:%d,param1:%d,param2:%d,param3:%d,param4:%d,param5:%d]", 
		wCmdId, dwParam1, dwParam2, dwParam3, dwParam4, dwParam5);

	switch (wCmdId)
	{
	// ϵͳ�ʼ�
	case DB_GM_SYETM_MAIL:
	{
		Player* arPlayerList[100];
		int nPlayerNum = 0;
		PlayerManager::Instance()->GetAllPlayer(arPlayerList, nPlayerNum, 100);
		for (int i = 0; i < nPlayerNum; ++i)
		{
			Player *pPlayer = arPlayerList[i];
			pPlayer->GetEmailManager()->AddEmail(pPlayer->GetRoleId(), pParamList[0], NULL, 0, true);
		}
	}
	break;
	// ϵͳ�ʼ�
	case DB_GM_SYETM_MAIL_EX:
	{
		const char *pszEmailText = pszText;
		const char *pszEmailTital = pszText1;
		DWORD dwTargetId = dwParam1;
		DWORD dwType = dwParam3;	// ����(0=����,1=ָ��������,2=���з�)

		// ������ݳ��ȼ��
		if (strlen(pszEmailText) >= MAX_EMAIL_TEXT)
		{
			MAPLOG(ERROR_LEVEL, "�ʼ����ݳ���[TextSize:%d]", strlen(pszEmailText));
			return;
		}
		// �����ⳤ�ȼ��
		if (strlen(pszEmailTital) >= MAX_EMAIL_TITLE_LEN)
		{
			MAPLOG(ERROR_LEVEL, "�ʼ����ⳬ��[TextSize:%d]", strlen(pszEmailTital));
			return;
		}

		EmailInfo tEmailInfo;
		tEmailInfo.CleanUp();
		tEmailInfo.dwID = EmailManager::GetEmailId();
		tEmailInfo.dwReceiveTime = time(NULL);
		strcpy_s(tEmailInfo.szText, sizeof(tEmailInfo.szText), pszEmailText);
		strcpy_s(tEmailInfo.szTitle, sizeof(tEmailInfo.szTitle), pszEmailTital);

		vector<DWORD> vTargetServerList;
		// ��������
		if (dwType == 0)
		{
			EmailManager::AddEmailByGM(dwTargetId, tEmailInfo);
		}
		// ����
		else if (dwType == 1)
		{
			vTargetServerList.push_back(g_MapServer.GetCurServerId(dwTargetId));
		}
		// ȫ��
		else if (dwType == 2)
		{
			map<DWORD, DWORD> tmp_list;
			const map<DWORD, ServerInfo> &vServerList = g_MapServer.GetServerList();
			for (map<DWORD, ServerInfo>::const_iterator iter = vServerList.begin(); iter != vServerList.end(); iter++)
			{
				DWORD dwCurServerId = g_MapServer.GetCurServerId(iter->second.dwServerId);
				if (tmp_list.find(dwCurServerId) == tmp_list.end())
				{
					vTargetServerList.push_back(dwCurServerId);
					tmp_list.insert(make_pair(dwCurServerId, dwCurServerId));
				}
			}
		}

		for (size_t i = 0; i < vTargetServerList.size(); ++i)
		{
			GMSendEmailInfo tSystemReward;
			tSystemReward.dwServerId = vTargetServerList[i];
			strcpy_s(tSystemReward.szText, sizeof(tEmailInfo.szText), pszEmailText);
			strcpy_s(tSystemReward.szTitle, sizeof(tEmailInfo.szTitle), pszEmailTital);
			m_vGMSendEmailList.push_back(tSystemReward);
		}
	}
	break;
	// �������ķ�����(ָ��������)
	case DB_GM_REWARD_CENTER:
	{
		
	}
	break;
	// ���¼��������ļ�
	case DB_GM_RELOAD_SERVER_CONFIG:
	CConfigManager::Instance()->Reload();
	break;
	// ���¼��ط������б�
	case DB_GM_RELOAD_SERVER_LIST:
	g_MapServer.LoadServerListRequest();
	break;
	// ϵͳ����
	case DB_GM_SYSTEM_NOTICE:
	{
		WORD wServerId			= dwParam1;		// ������ID
		WORD wNoticeId			= dwParam2;		// �㲥ID
		DWORD dwNoticeTimes		= dwParam3;		// �㲥����
		m_dwSendNoticeInterval	= dwParam4;		// �㲥Ƶ��

		// ��֤���ٹ㲥һ��
		dwNoticeTimes = max(dwNoticeTimes, 1);

		// ��ֹ����Ϊ0�����
		if (m_dwSendNoticeInterval == 0)
			m_dwSendNoticeInterval = 10;

		// ��ֹ����Ϊ̫������(30����)
		if (m_dwSendNoticeInterval > 1800)
			m_dwSendNoticeInterval = 1800;

		// ����㲥
// 		CSystemNotice sys_notice(wNoticeId, wServerId);
// 		sys_notice.SetText(pszText);
// 		sys_notice.SendMsg();

		if (dwNoticeTimes)
		{
			SystemNoticeInfo tNoticeInfo;
			tNoticeInfo.nNoticeTimes = dwNoticeTimes;
			tNoticeInfo.wServerId = wServerId;
			tNoticeInfo.wNoticeId = wNoticeId;
			strcpy_s(tNoticeInfo.szText, sizeof(tNoticeInfo.szText) - 1, pszText);

			// ���
			m_vSystemNoticeList.push_back(tNoticeInfo);
		}
	}
	break;
	// ���������а�鿴�б�ˢ��
	case DB_GM_RANK_VIEW_REFRESH:
	{
		const map<DWORD, ServerInfo> &vServerList = g_MapServer.GetServerList();
		map<DWORD, ServerInfo>::const_iterator iter = vServerList.begin();
		for (; iter != vServerList.end(); iter++)
		{
			ArenaRankViewList_Update* pQuery = ArenaRankViewList_Update::ALLOC();
			pQuery->SetIndex(DB_ARENA_RANK_VIEW_LIST_UPDATE);
			pQuery->SetThreadIndex(DB_THREAD_GM);
			pQuery->SetServerId(iter->second.dwServerId);
			g_MapServer.MapDBQuery(pQuery);
		}
	}
	break;
	// ���Ž���(����)
	case DB_GM_SEND_REWARD_TO_ONE:
	{
		DWORD dwTargetId = pParamList[0];	// ����Ŀ��ID
		//DWORD dwServerId = pParamList[1];	// ����ָ��������
		DWORD dwItemId = pParamList[2];		// ������ƷID
		DWORD dwItemNum = pParamList[3];	// ������Ʒ����
		DWORD dwRewardInfoId = pParamList[4];	// ������Ʒ����
		RewardData tReward;
		tReward.dwValue1 = dwItemId;
		tReward.dwValue2 = dwItemNum;
		tReward.wInfoId = dwRewardInfoId;
		tReward.dwTime = time(NULL);
		const Reward_Config *pRewardCfg = g_pCfgMgr->GetReward(dwRewardInfoId);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������������[reward_id:%d]", __FUNCTION__, dwRewardInfoId);
			return;
		}

		DWORD dwServerId = PlayerBaseDataManager::Instance()->GetServerId(dwTargetId);
		if (!dwServerId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����һ�������[RoleId:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		CRewardCenterManager::AddReward(dwServerId, dwTargetId, tReward);
	}
	break;
	// ���Ž���(������)
	case DB_GM_SEND_REWARD_TO_ALL:
	{									 
		DWORD dwServerId = pParamList[1];	// ����ָ��������
		DWORD dwItemId = pParamList[2];		// ������ƷID
		DWORD dwItemNum = pParamList[3];	// ������Ʒ����
		DWORD dwRewardId = pParamList[4];	// ������Ʒ����

		vector<DWORD> vTargetServerList;
		if (dwServerId)
		{
			vTargetServerList.push_back(dwServerId);
		}
		else
		{
			const map<DWORD, ServerInfo> &vServerList = g_MapServer.GetServerList();
			for (map<DWORD, ServerInfo>::const_iterator iter = vServerList.begin(); iter != vServerList.end(); iter++)
			{
				vTargetServerList.push_back(iter->second.dwServerId);
			}
		}

		for (size_t i = 0; i < vTargetServerList.size(); ++i)
		{
			SystemReward tSystemReward;
			tSystemReward.dwServerId = vTargetServerList[i];
			tSystemReward.wMinLevel = 1;
			tSystemReward.wMaxLevel = 200;
			tSystemReward.tRewardData.dwValue1 = dwItemId;
			tSystemReward.tRewardData.wInfoId = dwRewardId;
			tSystemReward.tRewardData.dwValue2 = dwItemNum;
			tSystemReward.tRewardData.dwTime = time(NULL);
			m_vSystemRewardList.push_back(tSystemReward);
		}
	}
	break;
	// ��������
	case DB_GM_KICK_OUT:
	{
		DWORD dwTargetId = pParamList[0];	// �����ߵ�Ŀ��ID
		DWORD dwReasonId = pParamList[1];	// ԭ��ID
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (pPlayer)
		{
			pPlayer->SendErrorMsg(dwReasonId);
			AccountInfo *pAccount = pPlayer->GetAccountInfo();
			AccountInfoManager::Instance()->KickoutAccount(pAccount);
		}
	}
	break;
	// ����
	case DB_GM_SILENCE:
	{
		DWORD dwTargetId = pParamList[0];	// ���Ե�Ŀ��ID
		DWORD dwForbidToTalkOverTime = time(NULL) + pParamList[1] * 60; // ���Խ���ʱ��
		DWORD dwIsForbidToTalk = pParamList[2]; // �Ƿ����
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (pPlayer)
		{
			pPlayer->SetProperty(ROLE_PRO_FORBID_TO_TALK_OVER_TIME, dwIsForbidToTalk ? dwForbidToTalkOverTime : 0);
		}
		else
		{
			// �������ݿ�
			RoleTmpData_Update* pQuery = RoleTmpData_Update::ALLOC();
			pQuery->SetIndex(DB_ROLE_TEMP_DATA_UPDATE);
			pQuery->SetThreadIndex(DB_THREAD_COMMON);
			pQuery->SetRoleId(dwTargetId);
			pQuery->SetServerId(0);
			pQuery->AddParam(dwForbidToTalkOverTime);
			pQuery->AddParam(dwIsForbidToTalk);
			pQuery->SetDataType(ROLE_TEMP_FORBID_TO_TALK_SET);
			g_MapServer.MapDBQuery(pQuery);
		}
	}
	break;
	// ��������������
	case DB_GM_KICK_OUT_ALL:
	{
		DWORD dwReasonId = pParamList[1];	// ԭ��ID
		vector<Player*> vPlayerList;
		PlayerManager::Instance()->GetAllPlayer(vPlayerList);
		for (size_t i = 0; i < vPlayerList.size(); i++)
		{
			Player *pPlayer = vPlayerList[i];
			if (pPlayer)
			{
				pPlayer->SendErrorMsg(dwReasonId);
				AccountInfo *pAccount = pPlayer->GetAccountInfo();
				AccountInfoManager::Instance()->KickoutAccount(pAccount);
			}
		}
	}
	break;
	// ɾ�����а��û�����
	case DB_GM_DEL_RANK_DATA:
	{
		DWORD dwRankType = pParamList[0];	
		DWORD dwServerId = pParamList[1];	
		DWORD dwRoleId = pParamList[2];	
		CRankManager::Instance()->RemoveRoleRankData(dwRankType, dwServerId, dwRoleId);
	}
	break;
	// ���а���������
	case DB_GM_RANK_REFRESH:
	{
		DWORD dwRankType = pParamList[0];	
		DWORD dwServerId = pParamList[1];	
		CRankManager::Instance()->UpdateRoleRankEx(dwServerId, dwRankType);
	}
	break;
	// ��������
	case DB_GM_RESET_GUIDE:
	{
		DWORD dwServerId = pParamList[0];	
		DWORD dwRoleId = pParamList[1];	
		DWORD dwGuideKey = pParamList[2];
		DWORD dwGuideValue = pParamList[3];

		Player *pPlayer = PlayerManager::Instance()->FindPlayerByRoleId(dwRoleId);
		if (pPlayer)
		{
			pPlayer->GetLogicManager()->SetGuide(dwGuideKey, dwGuideValue);
		}
		else
		{
			// �������ݿ�
			RoleTmpData_Update* pQuery = RoleTmpData_Update::ALLOC();
			pQuery->SetIndex(DB_ROLE_TEMP_DATA_UPDATE);
			pQuery->SetThreadIndex(DB_THREAD_COMMON);
			pQuery->SetRoleId(dwRoleId);
			pQuery->SetServerId(dwServerId);
			pQuery->AddParam(time(NULL));
			pQuery->AddParam(dwGuideKey);
			pQuery->AddParam(dwGuideValue);
			pQuery->SetDataType(ROLE_TEMP_SET_GUIDE);
			g_MapServer.MapDBQuery(pQuery);
		}
	}
	break;
	// �㲥�ͻ��˸���
	case DB_GM_CLIENT_UPDATE_NOTIFY:
	{
		MSG_S2C_CLIENT_UPDATE_NOTIFY msg;
		PlayerManager::Instance()->SendWorldChat(&msg);
		MAPLOG(ERROR_LEVEL, "[%s]ʹ�ù㲥�ͻ��˸���", __FUNCTION__);
	}
	break;
	// ���ž���������
	case DB_GM_SEND_ARENA_REWARD:
	{
		CArenaManager::Instance()->DoRankAward();
		MAPLOG(ERROR_LEVEL, "[%s]ʹ��GM���ž���������", __FUNCTION__);
	}
	break;
	// ��������BOSS���� 
	case DB_GM_SEND_WORLD_BOSS_REWARD:
	{
		CArenaManager::Instance()->DoRankAward();
		MAPLOG(ERROR_LEVEL, "[%s]ʹ��GM��������BOSS����", __FUNCTION__);
	}
	break;
	// ����7�����н��� 
	case DB_GM_SEND_SEVEN_DAY_RANK_REWARD:
	{
		DWORD dwServerId = pParamList[0];	
		CRankManager::Instance()->OnSevenDayRankReward(dwServerId);
		MAPLOG(ERROR_LEVEL, "[%s]ʹ��GM����7�����н���", __FUNCTION__);
	}
	break;
	// ���¼���init�ļ�  
	case DB_GM_RELOAD_INIT_FILE:
	{
		g_MapServer.ReloadInitFile();
		MAPLOG(ERROR_LEVEL, "[%s]���¼�����Init�ļ�", __FUNCTION__);
	}
	break;
	// �������ݴ洢 
	case DB_GM_SAVE_ALL_DATA:
	{
		CArenaManager::Instance()->SaveAllData();
		CRankManager::Instance()->SaveAllDataToDB();
		CLandManager::Instance()->SaveAllDataToDB();
		CResourceWarManager::Instance()->SaveDataToDB();
		PlayerManager::Instance()->SaveAllDataToDB();
	}
	break;
	// ��ֵ 
	case DB_GM_RECHARGE:
	{		   
		DWORD dwRoleId = dwParam1;
		DWORD dwRechargeId = dwParam2;
		if (!dwRoleId || !dwRechargeId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]ͨ����̨��ֵ�����������[RoleId:%d,RechargeId:%d]", __FUNCTION__, dwRoleId, dwRechargeId);
			return;
		}

		DWORD dwCurTime = time(NULL);

		// ��ֹID�ظ������
		if (m_dwCPOrderId == dwCurTime)
			m_dwCPOrderId++;
		else
			m_dwCPOrderId = dwCurTime;

		bool isMonthCard = IsMonthCard(dwRechargeId);
		RechargeCallbackData tRechargeData;
		memset(&tRechargeData, 0, sizeof(tRechargeData));
		tRechargeData.dwRoleId = dwRoleId;
		tRechargeData.wRechargeId = dwRechargeId;
		tRechargeData.dwCPOrderId = m_dwCPOrderId;
		tRechargeData.wRechargeType = isMonthCard ? RECHARGE_MONTH_CARD : RECHARGE_COMMON;
		tRechargeData.wFrom = RECHARGE_FROM_GM;
		if (isMonthCard)
		{
			const MonthCard_Config *pCardCfg = g_pCfgMgr->GetMonthCard(dwRechargeId);
			if (!pCardCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����¿�����[RechargeId:%d]",__FUNCTION__, dwRechargeId);
				return;
			}
			tRechargeData.nAmount = pCardCfg->nSellPrice;
		}
		else
		{
			const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(dwRechargeId);
			if (!pRechargeCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ��ֵ����[RechargeId:%d]", __FUNCTION__, dwRechargeId);
				return;
			}
			tRechargeData.nAmount = pRechargeCfg->nRMB;
		}

		CRechargeManager::OnNetRechargeCallbackEx(0, tRechargeData, false);

		// ��ʾ��ֵ�ɹ�
		MAPLOG(ERROR_LEVEL, "[%s]ͨ����̨��ֵ�ɹ�[role_id:%d,amount:%d,cp_order_id:%d]",
			__FUNCTION__, dwRoleId, tRechargeData.nAmount, m_dwCPOrderId);
	}
	break;
	// ����ʥ���ڻ
	case DB_GM_RESET_CHRISTMAS:
	CChristmas::Instance()->ResetData();
	break;
	// �Ϸ��¼�
	case DB_GM_SERVER:
	{
		DWORD dwFromServerId = pParamList[0];	// �ɷ�
		DWORD dwToServerId = pParamList[1];	// �·�

		g_MapServer.OnMergeServer(dwFromServerId, dwToServerId);
	}
	break;
	default:
		MAPLOG(ERROR_LEVEL, "ľ�д����GM�¼�[cmd_id:%d]", wCmdId);
		break;
	}

	// �洢GM������¼
	CGMOperationRecord_Save* pQuery = CGMOperationRecord_Save::ALLOC();
	pQuery->SetTime(time(NULL));
	pQuery->SetIndex(DB_GM_OPERATION_RECORD_SAVE);
	pQuery->SetGMCmd(wCmdId);
	pQuery->AddParam(dwParam1);
	pQuery->AddParam(dwParam2);
	pQuery->AddParam(dwParam3);
	pQuery->AddParam(dwParam4);
	pQuery->AddParam(dwParam5);
	g_MapServer.MapDBQuery(pQuery);
}