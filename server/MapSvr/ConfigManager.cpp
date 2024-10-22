#include "StdAfx.h"
#include "ConfigManager.h"
#include "..\ToolKit\DBCFileManager.h"
#include "..\ToolKit\DBCFile.h"
#include "FileEx\TsFile.h"
#include "CommonLogic.h"

CConfigManager *g_pCfgMgr = CConfigManager::Instance();


// �½���ɫ��ʼ������
const RoleInit_Config* CConfigManager::GetRoleInit(DWORD dwDataId)
{
// 	DBCFile* pDBCFile = DBCFileManager::Instance()->GetDBC(DBC_ROLE_INIT);
// 	if (!pDBCFile)
// 		return NULL;
// 
// 	return (RoleInit_Config*)pDBCFile->GetRecord(dwDataId);

	map<DWORD, RoleInit_Config>::iterator iter = m_RoleInitList.find(dwDataId);
	return iter != m_RoleInitList.end() ? &iter->second : NULL;
}

// ȡǩ������
const SignIn_Config* CConfigManager::GetSignIn(WORD wDay)
{
	map<WORD, SignIn_Config>::iterator iter = m_SignInList.find(wDay);
	return iter != m_SignInList.end() ? &iter->second : NULL;
}


// ȡ����ǩ����������
const LuxurySignInReward_Config* CConfigManager::GetLuxurySignInReward(DWORD dwRewardId)
{
	map<DWORD, LuxurySignInReward_Config>::iterator iter = m_LuxurySignInRewardList.find(dwRewardId);
	return iter != m_LuxurySignInRewardList.end() ? &iter->second : NULL;
}

// ȡ����ǩ������
const LuxurySign_Config* CConfigManager::GetLuxurySignInData(WORD wSignInId, WORD wRoleLevel)
{
	map<WORD, map<WORD, LuxurySign_Config>>::iterator iter1 = m_LuxurySignInList.find(wSignInId);
	if (iter1 != m_LuxurySignInList.end())
	{
		map<WORD, LuxurySign_Config>::const_iterator iter2 = iter1->second.find(wRoleLevel);
		if (iter2 != iter1->second.end())
			return &iter2->second;
	}
	return NULL;
}

// ȡӭ��������
const YCS_Config* CConfigManager::GetYCS(WORD wRoleLevel)
{
	map<WORD, YCS_Config>::iterator iter = m_YCSList.find(wRoleLevel);
	return iter != m_YCSList.end() ? &iter->second : NULL;
}

// ȡ��ֵ����
const Recharge_Config* CConfigManager::GetRecharge(WORD wRechargeId, BYTE byIsMyCard)
{
	Recharge_Config *pRechargeCfg = NULL;

	if (byIsMyCard)
	{
		map<DWORD, Recharge_Config>::iterator iter = m_MyCardRechargeList.find(wRechargeId);
		if (iter != m_MyCardRechargeList.end())
		{
			pRechargeCfg = &iter->second;
		}
	}
	else
	{
		map<DWORD, Recharge_Config>::iterator iter = m_RechargeList.find(wRechargeId);
		if (iter != m_RechargeList.end())
		{
			pRechargeCfg = &iter->second;
		}
	}

	if (!pRechargeCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]ȡ������ֵ����[RechargeId:%d,IsMyCard:%d]", __FUNCTION__, wRechargeId, byIsMyCard);
	}

	return pRechargeCfg;
}

// ȡ��ֵ����
const Recharge_Config* CConfigManager::GetMyCardRecharge(WORD wRechargeId)
{
	map<DWORD, Recharge_Config>::iterator iter = m_MyCardRechargeList.find(wRechargeId);
	return iter != m_MyCardRechargeList.end() ? &iter->second : NULL;
}

// ȡ�¿�����
const MonthCard_Config* CConfigManager::GetMonthCard(WORD wCardId)
{
	map<DWORD, MonthCard_Config>::iterator iter = m_MonthCardList.find(wCardId);
	return iter != m_MonthCardList.end() ? &iter->second : NULL;
}


// ȡ��ֵ�ۿ۹�����
const SuperDiscountBuyReward_Config* CConfigManager::GetSuperDiscountBuyReward(WORD wDay)
{
	map<WORD, SuperDiscountBuyReward_Config>::iterator iter = m_SuperDiscountBuyRewardList.find(wDay);
	return iter != m_SuperDiscountBuyRewardList.end() ? &iter->second : NULL;
}

// ȡ��ֵ�ۿ��ۻ�����
const SuperDiscountAccumulateReward_Config* CConfigManager::GetSuperDiscountAccumulateReward(WORD wTimes)
{
	map<WORD, SuperDiscountAccumulateReward_Config>::iterator iter = m_SuperDiscountAccumulateRewardList.find(wTimes);
	return iter != m_SuperDiscountAccumulateRewardList.end() ? &iter->second : NULL;
}

// ȡ��ֵ�ۿ��ۻ������б�
const map<WORD, SuperDiscountAccumulateReward_Config>& CConfigManager::GetSuperDiscountAccumulateRewardList()
{
	return m_SuperDiscountAccumulateRewardList;
}

// ȡ��ֵ�ۿ۹�����
const SuperDiscountBuyReward_Config*  CConfigManager::GetNewSuperDiscountBuyReward(WORD wDataId, WORD wDay)
{
	DWORD dwKey = 10000 * wDataId + wDay;
	map<DWORD, SuperDiscountBuyReward_Config>::iterator iter = m_NewSuperDiscountBuyRewardList.find(dwKey);
	return iter != m_NewSuperDiscountBuyRewardList.end() ? &iter->second : NULL;
}

// ȡ��ֵ�ۿ��ۻ�����
const SuperDiscountAccumulateReward_Config*  CConfigManager::GetNewSuperDiscountAccumulateReward(WORD wDataId, WORD wTimes)
{
	DWORD dwKey = 10000 * wDataId + wTimes;
	map<DWORD, SuperDiscountAccumulateReward_Config>::iterator iter = m_NewSuperDiscountAccumulateRewardList.find(dwKey);
	return iter != m_NewSuperDiscountAccumulateRewardList.end() ? &iter->second : NULL;
}

// ȡ��ɫ���н���
const RoleRankReward_Config* CConfigManager::GetSevenDayRankReward(WORD wRankType, WORD wRank)
{
	DWORD dwFindId = 100000 * wRankType + wRank;
	map<DWORD, RoleRankReward_Config>::iterator iter = m_SevenDayRankRewardList.find(dwFindId);
	return iter != m_SevenDayRankRewardList.end() ? &iter->second : NULL;
}

// ȡ��׼�Ƽ��������
const AccurateRecommendGoods_Config* CConfigManager::GetAccurateRecommendGoods(WORD wGoodsId)
{
	map<DWORD, AccurateRecommendGoods_Config>::iterator iter = m_AccurateRecommendGoodsList.find(wGoodsId);
	return iter != m_AccurateRecommendGoodsList.end() ? &iter->second : NULL;
}

// ȡ��������
const Scene_Config* CConfigManager::GetScene(DWORD dwSceneId)
{
	map<DWORD, Scene_Config>::iterator iter = m_SceneList.find(dwSceneId);
	return iter != m_SceneList.end() ? &iter->second : NULL;
}

// ȡ��¼��������
const LoginReward_Config* CConfigManager::GetLoginReward(WORD wDays)
{
	map<WORD, LoginReward_Config>::iterator iter = m_LoginRewardList.find(wDays);
	return iter != m_LoginRewardList.end() ? &iter->second : NULL;
}

// ��ȡ����������
DWORD CConfigManager::GetServerCfg(DWORD dwKey, DWORD dwDefault)
{
	map<DWORD, Server_Config>::iterator iter = m_CommonList.find(dwKey);
	return iter != m_CommonList.end() ? iter->second.dwValue : dwDefault;
}

const char* CConfigManager::GetServerCfg_Str(DWORD dwKey, const char *pszDefault)
{
	map<DWORD, Server_Config>::iterator iter = m_CommonList.find(dwKey);
	return iter != m_CommonList.end() ? iter->second.strValue.c_str() : NULL;
}

// ȡͨ���б�ֵ
const Server_Config* CConfigManager::GetCommonCfg(DWORD dwKey, bool bOutLog)
{
	map<DWORD, Server_Config>::iterator iter = m_CommonList.find(dwKey);
	if (bOutLog && iter == m_CommonList.end())
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ���ͨ����������[Key:%d]!", dwKey);
	}

	return iter != m_CommonList.end() ? &iter->second : NULL;
}

// ȡ��������Ϣ
const ServerInfo_Config*  CConfigManager::GetServerInfo(DWORD dwServerId)
{
	map<DWORD, ServerInfo_Config>::iterator iter = m_ServerInfoList.find(dwServerId);
	return iter != m_ServerInfoList.end() ? &iter->second : NULL;
}

// ȡ��Ʒ���� 
const Item_Config* CConfigManager::GetItem(DWORD dwItemId)
{
	map<DWORD, Item_Config>::iterator iter = m_ItemList.find(dwItemId);
	return iter != m_ItemList.end() ? &iter->second : NULL;
}

// ȡװ������
const Equipment_Config* CConfigManager::GetEquipment(DWORD dwEquipmentId)
{
	map<DWORD, Equipment_Config>::iterator iter = m_EquipmentList.find(dwEquipmentId);
	return iter != m_EquipmentList.end() ? &iter->second : NULL;;
}

// ȡװ��ǿ���ȼ�����
const EquipmentEnhanceLevel_Config* CConfigManager::GetEquipmentEnhanceLevel(BYTE byQuality, WORD wStrengLevel)
{
	DWORD dwFindId = byQuality * 1000 + wStrengLevel;
	map<DWORD, EquipmentEnhanceLevel_Config>::iterator iter = m_EquipmentEnhanceList.find(dwFindId);
	return iter != m_EquipmentEnhanceList.end() ? &iter->second : NULL;
}

// ȡװ�������ȼ�����
const EquipmentRefineLevel_Config* CConfigManager::GetEquipmentRefineLevel(BYTE byQuality, WORD wRefineLevel)
{
	DWORD dwFindId = byQuality * 1000 + wRefineLevel;
	map<DWORD, EquipmentRefineLevel_Config>::iterator iter = m_EquipmentRefineList.find(dwFindId);
	return iter != m_EquipmentRefineList.end() ? &iter->second : NULL;;
}

// ȡװ���ϳ�����
const EquipmentCompose_Config* CConfigManager::GetEquipmentCompose(DWORD dwEquipmentId)
{
	map<DWORD, EquipmentCompose_Config>::iterator iter = m_EquipmentComposeList.find(dwEquipmentId);
	return iter != m_EquipmentComposeList.end() ? &iter->second : NULL;;
}

// ȡ��ⷵ��
const ClosedTestRebate_Config* CConfigManager::GetClosedTestRebate(const char *pszAccount)
{
	map<string, ClosedTestRebate_Config>::iterator iter = m_ClosedTestRebateList.find(pszAccount);
	return iter != m_ClosedTestRebateList.end() ? &iter->second : NULL;;
}

// ȡӢ������
const Hero_Config* CConfigManager::GetHero(DWORD dwHeroInfoId)
{
	map<DWORD, Hero_Config>::iterator iter = m_HeroList.find(dwHeroInfoId);
	return iter != m_HeroList.end() ? &iter->second : NULL;
}

// ȡӢ�۵ȼ�����
const HeroLevel_Config* CConfigManager::GetHeroLevel(WORD wQuality, WORD wLevel)
{
	DWORD dwFindId = 1000 * wQuality + wLevel;
	map<DWORD, HeroLevel_Config>::iterator iter = m_HeroLevelList.find(dwFindId);
	return iter != m_HeroLevelList.end() ? &iter->second : NULL;;
}

// ȡӢ�ۺϳ�����
const HeroCompose_Config* CConfigManager::GetHeroCompose(DWORD dwHeroId)
{
	map<DWORD, HeroCompose_Config>::iterator iter = m_HeroComposeList.find(dwHeroId);
	return iter != m_HeroComposeList.end() ? &iter->second : NULL;
}

// ȡ��ɫ�ȼ�����
const RoleLevel_Config* CConfigManager::GetRoleLevel(WORD wLevel)
{
	map<WORD, RoleLevel_Config>::iterator iter = m_RoleLevelList.find(wLevel);
	return iter != m_RoleLevelList.end() ? &iter->second : NULL;
}

// ȡ��ɫ���Իָ�����
const RoleProRecover_Config* CConfigManager::GetRoleProRecover(BYTE byProType)
{
	map<DWORD, RoleProRecover_Config>::iterator iter = m_RoleProRecoverList.find(byProType);
	return iter != m_RoleProRecoverList.end() ? &iter->second : NULL;
}

// ȡӢ��ͻ����������
const HeroBreakLevelUp_Config* CConfigManager::GetHeroBreakLevelUp(WORD wLevel)
{
	map<DWORD, HeroBreakLevelUp_Config>::iterator iter = m_BreakLevelUpList.find(wLevel);
	return iter != m_BreakLevelUpList.end() ? &iter->second : NULL;
}

// ȡӢ�������������� 
const HeroDestiny_Config* CConfigManager::GetHeroDestiny(WORD wDestinyLevel)
{
	map<DWORD, HeroDestiny_Config>::iterator iter = m_HeroDestinyList.find(wDestinyLevel);
	return iter != m_HeroDestinyList.end() ? &iter->second : NULL;
}

// ȡ�������� 
const Treasure_Config* CConfigManager::GetTreasure(DWORD dwInfoId)
{
	map<DWORD, Treasure_Config>::iterator iter = m_TreasureList.find(dwInfoId);
	return iter != m_TreasureList.end() ? &iter->second : NULL;
}

// ȡ����ǿ������ 
const TreasureEnhance_Config* CConfigManager::GetTreasureStrengthen(WORD wQuality, WORD wEnhanceLevel)
{
	DWORD dwFindId = 1000 * wQuality + wEnhanceLevel;
	map<DWORD, TreasureEnhance_Config>::iterator iter = m_TreasureEnhanceList.find(dwFindId);
	return iter != m_TreasureEnhanceList.end() ? &iter->second : NULL;
}

// ȡ���ﾫ������
const TreasureRefine_Config* CConfigManager::GetTreasureRefine(WORD wQuality, WORD wRefineLevel)
{
	DWORD dwFindId = 1000 * wQuality + wRefineLevel;
	map<DWORD, TreasureRefine_Config>::iterator iter = m_TreasureRefineList.find(dwFindId);
	return iter != m_TreasureRefineList.end() ? &iter->second : NULL;
}

// ȡ����ϳ�����
const TreasureCompose_Config* CConfigManager::GetTreasureCompose(DWORD dwTreasureInfoId)
{
	map<DWORD, TreasureCompose_Config>::iterator iter = m_TreasureComposeList.find(dwTreasureInfoId);
	return iter != m_TreasureComposeList.end() ? &iter->second : NULL;
}

// ȡ��������������
const ArenaRank_Config* CConfigManager::GetArenaRank(WORD wRank)
{
	map<DWORD, ArenaRank_Config>::iterator iter = m_ArenaRankList.find(wRank);
	return iter != m_ArenaRankList.end() ? &iter->second : NULL;
}

// ȡ�������������б�
const vector<ArenaRobot_Config>* CConfigManager::GetArenaRobotList()
{
	return &m_ArenaRobotList;
}

// ȡ����������
const ArenaAward_Config* CConfigManager::GetArenaAward(WORD wRoleLevel)
{
	map<DWORD, ArenaAward_Config>::iterator iter = m_ArenaAwardList.find(wRoleLevel);
	return iter != m_ArenaAwardList.end() ? &iter->second : NULL;
}

// ȡ����������������
const ArenaRobot_Config* CConfigManager::GetArenaRobot(DWORD dwRobotId)
{
	vector<ArenaRobot_Config>::iterator iter = m_ArenaRobotList.begin();
	for (; iter != m_ArenaRobotList.end(); ++iter)
	{
		if (iter->dwRobotId == dwRobotId)
			return &*iter;
	}
	return NULL;
}

// ȡ�״�����������
int CConfigManager::GetArenaFirstRankReward(WORD wRank)
{
	map<DWORD, int>::iterator iter = m_ArenaFirstRankRewardList.find(wRank);
	return iter != m_ArenaFirstRankRewardList.end() ? iter->second : 0;
}

// ȡ�ɾ�����
const Achievement_Config* CConfigManager::GetAchievement(WORD wAchievementId)
{
	map<DWORD, Achievement_Config>::iterator iter = m_AchievementList.find(wAchievementId);
	return iter != m_AchievementList.end() ? &iter->second : NULL;
}

const DailyMission_Config* CConfigManager::GetDailyMission(WORD wMissionId)
{
	map<DWORD, DailyMission_Config>::iterator iter = m_DailyMissionList.find(wMissionId);
	return iter != m_DailyMissionList.end() ? &iter->second : NULL;
}

// ȡ�ճ�����ȼ�����
const RoleLvDailyMission_Config* CConfigManager::GetRoleLvDailyMission(WORD wRoleLevel)
{
	map<WORD, RoleLvDailyMission_Config>::iterator iter = m_RoleLvDailyMissionList.find(wRoleLevel);
	return iter != m_RoleLvDailyMissionList.end() ? &iter->second : NULL;
}

// ȡĳ�����͵��ճ������б�
const vector<WORD>* CConfigManager::GetTypeDailyMissionList(WORD wType)
{
	map<DWORD, vector<WORD>>::iterator iter = m_DailyMissionTypeList.find(wType);
	return iter != m_DailyMissionTypeList.end() ? &iter->second : NULL;
}

const SevenDayTarget_Config* CConfigManager::GetSevenDayTarget(WORD wTargetId)
{
	map<DWORD, SevenDayTarget_Config>::iterator iter = m_SevenDayTargetList.find(wTargetId);
	return iter != m_SevenDayTargetList.end() ? &iter->second : NULL;
}

// ȡ������Ŀ���б�
const vector<DWORD>* CConfigManager::GetServerDayActivity(WORD wDay)
{
	map<WORD, vector<DWORD>>::iterator iter = m_SevenDayActiviyList.find(wDay);
	return iter != m_SevenDayActiviyList.end() ? &iter->second : NULL;
}

// ȡͬ���߼����͵�Ŀ���б�
const vector<WORD>* CConfigManager::GetTheSameLogicTargetList(WORD wLogicType)
{
	map<WORD, vector<WORD>>::iterator iter = m_TheSameTargetList.find(wLogicType);
	return iter != m_TheSameTargetList.end() ? &iter->second : NULL;
}

// ȡ7����ɶȽ���
const SevenDayCompletionReward_Config* CConfigManager::GetSevenDayCompletionReward(WORD wCompletionNum)
{
	map<WORD, SevenDayCompletionReward_Config>::iterator iter = m_SevenDayCompletionRewardList.find(wCompletionNum);
	return iter != m_SevenDayCompletionRewardList.end() ? &iter->second : NULL;
}

// ȡ����BOSS����
const WorldBoss_Config* CConfigManager::GetWorldBoss(WORD wBossLv)
{
	map<DWORD, WorldBoss_Config>::iterator iter = m_WorldBossList.find(wBossLv);
	return iter != m_WorldBossList.end() ? &iter->second : NULL;
}

// ȡ����BOSS����ʱ������
const WorldBossOpenTime_Config* CConfigManager::GetWorldBossOpenTime(int nWeekDay)
{
	map<DWORD, WorldBossOpenTime_Config>::iterator iter = m_WorldBossOpenTimeList.find(nWeekDay);
	return iter != m_WorldBossOpenTimeList.end() ? &iter->second : NULL;
}

// ȡ����BOSS���н�������
const WorldBossRankReward_Config* CConfigManager::GetWorldBossRankReward(WORD wRank)
{
	map<DWORD, WorldBossRankReward_Config>::iterator iter = m_WorldBossRankRewardList.find(wRank);
	return iter != m_WorldBossRankRewardList.end() ? &iter->second : NULL;
}

// ȡ������������
const WorldBossHonorReward_Config* CConfigManager::GetWorldBossHonorReward(WORD wRewardId)
{
	map<DWORD, WorldBossHonorReward_Config>::iterator iter = m_WorldBossHonorRewardList.find(wRewardId);
	return iter != m_WorldBossHonorRewardList.end() ? &iter->second : NULL;
}

// ȡ��ɱ��������
const WorldBossKillReward_Config* CConfigManager::GetWorldBossKillReward(WORD wRewardId)
{
	map<DWORD, WorldBossKillReward_Config>::iterator iter = m_WorldBossDamageRewardList.find(wRewardId);
	return iter != m_WorldBossDamageRewardList.end() ? &iter->second : NULL;
}

// ȡ��������
const Card_Config* CConfigManager::GetCard(DWORD dwInfoId)
{
	map<DWORD, Card_Config>::iterator iter = m_CardList.find(dwInfoId);
	return iter != m_CardList.end() ? &iter->second : NULL;
}

// ȡ������������
const CardLevelUp_Config* CConfigManager::GetCardLevelUp(BYTE byQuality, WORD wLevel)
{
	DWORD dwFindId = 10000 * byQuality + wLevel;
	map<DWORD, CardLevelUp_Config>::iterator iter = m_CardLevelUpList.find(dwFindId);
	return iter != m_CardLevelUpList.end() ? &iter->second : NULL;
}

// ȡ����ͻ������
const CardBreak_Config* CConfigManager::GetCardBreak(BYTE byQuality, WORD wBreakLevel)
{
	DWORD dwFindId = 10000 * byQuality + wBreakLevel;
	map<DWORD, CardBreak_Config>::iterator iter = m_CardBreakList.find(dwFindId);
	return iter != m_CardBreakList.end() ? &iter->second : NULL;
}

// ȡ�ղ�������
const CollectionBook_Config* CConfigManager::GetCollectionBook(WORD wBookId)
{
	map<DWORD, CollectionBook_Config>::iterator iter = m_CollectionBookList.find(wBookId);
	return iter != m_CollectionBookList.end() ? &iter->second : NULL;
}

// ȡ�ղ�������
const CollectionGroup_Config* CConfigManager::GetCollectionGroup(WORD wGroupId)
{
	map<DWORD, CollectionGroup_Config>::iterator iter = m_CollectionGroupList.find(wGroupId);
	return iter != m_CollectionGroupList.end() ? &iter->second : NULL;
}

// ȡ�ͼ���������
const vector<RateDataEx>* CConfigManager::GetLowCardGroup(DWORD dwGroupId)
{
	map<DWORD, vector<RateDataEx>>::iterator iter = m_LowCardGroupList.find(dwGroupId);
	return iter != m_LowCardGroupList.end() ? &iter->second : NULL;
}

// ȡ����ؿ�����
const CardGroupMap_Config* CConfigManager::GetCardGroupMap(WORD wMapIdx)
{
	map<DWORD, CardGroupMap_Config>::iterator iter = m_CardGroupMapList.find(wMapIdx);
	return iter != m_CardGroupMapList.end() ? &iter->second : NULL;
}

// ȡ�߼���������
const vector<HighCardGroup_Config>* CConfigManager::GetHighCardGroup()
{
	return &m_HighCardGroupList;
}

// ȡռ���ɾ�����
const ZhanBuAchievement_Config* CConfigManager::GetZhanBuAchievement(WORD wLevel)
{
	map<WORD, ZhanBuAchievement_Config>::iterator iter = m_ZhanBuAchievementList.find(wLevel);
	return iter != m_ZhanBuAchievementList.end() ? &iter->second : NULL;
}

// ȡռ���ɾ�����
const CardSlot_Config* CConfigManager::GetCardSlot(WORD wSlot)
{
	map<WORD, CardSlot_Config>::iterator iter = m_CardSlotList.find(wSlot);
	return iter != m_CardSlotList.end() ? &iter->second : NULL;
}

// ȡ���߸����½�����
const FubenChapter_Config* CConfigManager::GetMainFBChapter(WORD wChapter)
{
	map<DWORD, FubenChapter_Config>::iterator iter = m_MainFBChapterList.find(wChapter);
	return iter != m_MainFBChapterList.end() ? &iter->second : NULL;
}

// ȡ���߸����ؿ�����
const FubenMap_Config* CConfigManager::GetMainFubenMap(WORD wChapter, BYTE byMapIdx)
{
	const FubenChapter_Config *pChapterCfg = GetMainFBChapter(wChapter);
	if (!pChapterCfg)
		return NULL;

	if (byMapIdx > pChapterCfg->vMapList.size())
		return NULL;

	DWORD dwMapId = pChapterCfg->vMapList[byMapIdx - 1];

	map<DWORD, Map_Config>::iterator iter = m_MapList.find(dwMapId);

	return iter != m_MapList.end() ? &iter->second : NULL;
}

// ȡӢ�۸����½�����
const FubenChapter_Config* CConfigManager::GetHeroFBChapter(WORD wChapter)
{
	map<DWORD, FubenChapter_Config>::iterator iter = m_HeroFBChapterList.find(wChapter);
	return iter != m_HeroFBChapterList.end() ? &iter->second : NULL;
}


// ȡ�����ؿ�����
const TowerMap_Config* CConfigManager::GetTowerMap(WORD wMapLevel)
{
	map<DWORD, TowerMap_Config>::iterator iter = m_TowerMapList.find(wMapLevel);
	return iter != m_TowerMapList.end() ? &iter->second : NULL;
}

// ȡ�����½�����
const TowerChapter_Config* CConfigManager::GetTowerChapter(WORD wChapter)
{
	map<DWORD, TowerChapter_Config>::iterator iter = m_TowerChapterList.find(wChapter);
	return iter != m_TowerChapterList.end() ? &iter->second : NULL;
}

// ȡ������Ӣ�ؿ�
const TowerEliteMap_Config* CConfigManager::GetTowerEliteMap(WORD wMapLevel)
{
	map<DWORD, TowerEliteMap_Config>::iterator iter = m_TowerEliteMapList.find(wMapLevel);
	return iter != m_TowerEliteMapList.end() ? &iter->second : NULL;
}

// ȡ�������
const TowerRandPro_Config* CConfigManager::GetTowerRandPro(BYTE byStar)
{
	map<DWORD, TowerRandPro_Config>::iterator iter = m_TowerRandProList.find(byStar);
	return iter != m_TowerRandProList.end() ? &iter->second : NULL;
}

// ȡ������Ʒ
const TowerStarGoods_Config* CConfigManager::GetTowerStarGoods(WORD wStar)
{
	map<DWORD, TowerStarGoods_Config>::iterator iter = m_TowerStarGoodsList.find(wStar);
	return iter != m_TowerStarGoodsList.end() ? &iter->second : NULL;
}

const LandCity_Config* CConfigManager::GetLand(WORD wLandId)
{
	map<DWORD, LandCity_Config>::iterator iter = m_LandCityList.find(wLandId);
	return iter != m_LandCityList.end() ? &iter->second : NULL;
}

// ȡ��ؽ�������
const LandPatrolEvent_Config* CConfigManager::GetLandPatrolEvent(WORD wLandId)
{
	map<DWORD, LandPatrolEvent_Config>::iterator iter = m_LandPatrolEventList.find(wLandId);
	return iter != m_LandPatrolEventList.end() ? &iter->second : NULL;
}

// ȡ���Ѳ������
const LandPatrol_Config* CConfigManager::GetLandPatrol(BYTE byPatrolType)
{
	map<DWORD, LandPatrol_Config>::iterator iter = m_LandPatrolList.find(byPatrolType);
	return iter != m_LandPatrolList.end() ? &iter->second : NULL;
}

// ȡ�ճ������½�����
const FubenChapter_Config* CConfigManager::GetDailyChapter(WORD wChapter)
{
	map<DWORD, FubenChapter_Config>::iterator iter = m_DailyChapterList.find(wChapter);
	return iter != m_DailyChapterList.end() ? &iter->second : NULL;
}

// ȡ��Ӣ�����½�����
const FubenChapter_Config* CConfigManager::GetElitFBeChapter(WORD wChapter)
{
	map<DWORD, FubenChapter_Config>::iterator iter = m_EliteFBChapterList.find(wChapter);
	return iter != m_EliteFBChapterList.end() ? &iter->second : NULL;
}

// ȡ�ճ������˺�����
const DailyFBDamageReward_Config* CConfigManager::GetDailyFBDamageReward(WORD wChapter, WORD wMapIdx, WORD wContext)
{
	DWORD dwFindId = 1000000 * wChapter + 10000 * wMapIdx + wContext;
	map<DWORD, DailyFBDamageReward_Config>::iterator iter = m_DailyFBDamageReward.find(dwFindId);
	return iter != m_DailyFBDamageReward.end() ? &iter->second : NULL;
}

// ȡ���߸����½ڽ���
const MainFBChapterReward_Config* CConfigManager::GetMainFBChapterReward(WORD wRewardLevel)
{
	map<DWORD, MainFBChapterReward_Config>::iterator iter = m_MainFBChapterRewardList.find(wRewardLevel);
	return iter != m_MainFBChapterRewardList.end() ? &iter->second : NULL;
}

// ȡ��ʱ�����б�
const vector<LimitDrop_Config>* CConfigManager::GetLimitDropList(DWORD dwDungeonType)
{
	map<DWORD, vector<LimitDrop_Config>>::const_iterator iter = m_vLimitDropList.find(dwDungeonType);
	return iter != m_vLimitDropList.end() ? &iter->second : NULL;
}

// ȡ�ۻ��̵�����
const vector<RefreshShopSlot_Config>* CConfigManager::GetRefreshShopSlotList(BYTE byShopType)
{
	if (SHOP_AWAKEN == byShopType)
	{
		return &m_AwakenShopSlotList;
	}
	else if (SHOP_HERO == byShopType)
	{
		return &m_HeroShopSlotList;
	}
	else if (SHOP_DIRECT_BUY == byShopType)
	{
		return &m_DirectBuyShopSlotList;
	}
	else if (SHOP_GUILD_LIMIT == byShopType)
	{
		return &m_GuildLimitShopSlotList;
	}
	else if (SHOP_STAR_SOUL == byShopType)
	{
		return &m_StarSoulShopSlotList;
	}
	else if (SHOP_CRYSTAL == byShopType)
	{
		return &m_CrystalShopSlotList;
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д����ˢ���̵���Ʒ��λ����[ShopType:%d]", __FUNCTION__, byShopType);
	}

	return NULL;
}

// ȡӢ�۾�������
const HeroAwaken_Config* CConfigManager::GetHeroAwaken(BYTE byInitQaulity, BYTE byProfession, BYTE byAwakenStar, BYTE byAwakenLevel)
{
	DWORD dwFindId = byInitQaulity * 1000000 + byProfession * 10000 + byAwakenStar * 100 + byAwakenLevel;
	map<DWORD, HeroAwaken_Config>::iterator iter = m_HeroAwakenList.find(dwFindId);
	return iter != m_HeroAwakenList.end() ? &iter->second : NULL;
}

// ȡӢ��ͻ������
const HeroBreak_Config* CConfigManager::GetHeroBreak(WORD wHeroInfnId, WORD wBreakLv)
{
	DWORD dwFindId = wHeroInfnId * 1000 + wBreakLv;
	map<DWORD, HeroBreak_Config>::iterator iter = m_HeroBreakList.find(dwFindId);
	return iter != m_HeroBreakList.end() ? &iter->second : NULL;
}

// ȡӢ�۽�������
const HeroEvolution_Config* CConfigManager::GetHeroEvolution(WORD wQuality, WORD wStage)
{
	DWORD dwKey = 10000 * wQuality + wStage;
	map<DWORD, HeroEvolution_Config>::iterator iter = m_HeroEvoluctionList.find(dwKey);
	return iter != m_HeroEvoluctionList.end() ? &iter->second : NULL;
}

// ȡӢ�����ʼӳ�
int CConfigManager::GetHeroQualificationsAddRate(DWORD dwQualifications)
{
	map<DWORD, int>::iterator iter = m_HeroQualificationsList.find(dwQualifications);
	return iter != m_HeroQualificationsList.end() ? iter->second : 0;
}

// ȡӢ��ͻ������
vector<DWORD>* CConfigManager::GetHeroPokedexList(WORD wQuality)
{
	map<DWORD, vector<DWORD>>::iterator iter = m_HeroPokedexList.find(wQuality);
	return iter != m_HeroPokedexList.end() ? &iter->second : NULL;
}

// ȡӢ����������
const HeroTrainCost_Config* CConfigManager::GetHeroTrainCost(BYTE byCostType)
{
	map<DWORD, HeroTrainCost_Config>::iterator iter = m_HeroTrainCostList.find(byCostType);
	return iter != m_HeroTrainCostList.end() ? &iter->second : NULL;
}

// ȡӢ����������
const HeroTrain_Config* CConfigManager::GetHeroTrain(BYTE byProType, BYTE byProLevel)
{
	map<DWORD, vector<HeroTrain_Config>>::iterator iter = m_HeroTrainList.find(byProType);
	if (iter != m_HeroTrainList.end())
	{
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].byProLevel == byProLevel)
				return &iter->second[i];
		}
	}
	return NULL;
}

// ȡӢ�������б�
const vector<HeroTrain_Config>* CConfigManager::GetHeroTrainList(BYTE byProType)
{
	map<DWORD, vector<HeroTrain_Config>>::iterator iter = m_HeroTrainList.find(byProType);
	return iter != m_HeroTrainList.end() ? &iter->second : NULL;
}

// ȡ��������
const HeartOfWorld_Config* CConfigManager::GetHeartOfWorld(BYTE byStage)
{
	map<DWORD, HeartOfWorld_Config>::iterator iter = m_HeartOfWorldList.find(byStage);
	return iter != m_HeartOfWorldList.end() ? &iter->second : NULL;
}

// ȡ��������
const Spirit_Config* CConfigManager::GetSpirit(DWORD dwSpiritId)
{
	map<DWORD, Spirit_Config>::iterator iter = m_SpiritList.find(dwSpiritId);
	return iter != m_SpiritList.end() ? &iter->second : NULL;
}

// ȡ����ǿ������
const SpiritEnhance_Config* CConfigManager::GetSpiritEnhance(DWORD dwSpiritId, WORD wEnhanceLevel)
{
	DWORD dwFindId = dwSpiritId * 1000 + wEnhanceLevel;
	map<DWORD, SpiritEnhance_Config>::iterator iter = m_SpiritEnhanceList.find(dwFindId);
	return iter != m_SpiritEnhanceList.end() ? &iter->second : NULL;
}

// ȡ����ͼ������
const SpiritPokedex_Config* CConfigManager::GetSpiritPokedex(DWORD dwPokedexId, WORD wPokedexLevel)
{
	DWORD dwFindId = dwPokedexId * 100 + wPokedexLevel;
	map<DWORD, SpiritPokedex_Config>::iterator iter = m_SpiritPokedexList.find(dwFindId);
	return iter != m_SpiritPokedexList.end() ? &iter->second : NULL;
}

// ȡ����׶�����
BYTE CConfigManager::GetSpiritStageMax()
{
	return m_HeartOfWorldList.size();
}

// ȡ������������
const SpriteTrain_Config* CConfigManager::GetSpriteTrain(DWORD dwSpiritId, WORD wTrainLevel)
{
	DWORD dwFindId = dwSpiritId * 100 + wTrainLevel;
	map<DWORD, SpriteTrain_Config>::iterator iter = m_SpiritTrainList.find(dwFindId);
	return iter != m_SpiritTrainList.end() ? &iter->second : NULL;
}

// ȡ�����츳��������
const SpriteTalentSkill_Config* CConfigManager::GetSpriteTalentSkill(DWORD dwSpriteId, WORD wTalentSkillLevel)
{
	DWORD dwKey = dwSpriteId * 100 + wTalentSkillLevel;
	map<DWORD, SpriteTalentSkill_Config>::iterator iter = m_SpriteTalentSkillList.find(dwKey);
	return iter != m_SpriteTalentSkillList.end() ? &iter->second : NULL;
}

// ȡ������������
const SpriteStarUp_Config* CConfigManager::GetSpriteStarUp(WORD wStar)
{
	//DWORD dwKey = dwSpriteId * 100 + wStar;
	map<DWORD, SpriteStarUp_Config>::iterator iter = m_SpriteStarUpList.find(wStar);
	return iter != m_SpriteStarUpList.end() ? &iter->second : NULL;
}

// ȡ������������
const SpriteSpecialProperty_Config* CConfigManager::GetSpriteSpecialProperty(DWORD dwSpriteId, WORD wStar)
{
	DWORD dwKey = dwSpriteId * 100 + wStar;
	map<DWORD, SpriteSpecialProperty_Config>::iterator iter = m_SpriteSpecialPropertyList.find(dwKey);
	return iter != m_SpriteSpecialPropertyList.end() ? &iter->second : NULL;
}

// ȡ������������
const SpriteQualifications_Config* CConfigManager::GetSpriteQualifications(WORD wQualifications)
{
	map<DWORD, SpriteQualifications_Config>::iterator iter = m_SpriteQualificationsList.find(wQualifications);
	return iter != m_SpriteQualificationsList.end() ? &iter->second : NULL;
}

// ȡ����ģ������
// const SpiritModel_Config* CConfigManager::GetSpiritModel(WORD wModelId)
// {
// 	return NULL;
// }

// ȡ��Ȫ����
const GuildQY_Config* CConfigManager::GetGuidQY(BYTE byType)
{
	map<DWORD, GuildQY_Config>::iterator iter = m_GuildQYList.find(byType);
	return iter != m_GuildQYList.end() ? &iter->second : NULL;
}

// ȡ��Ȫ��������
const GuildQSAward_Config* CConfigManager::GetGuildQSAward(DWORD dwValue, WORD wGuildLevel)
{
	DWORD dwFindId = dwValue * 100 + wGuildLevel;
	map<DWORD, GuildQSAward_Config>::iterator iter = m_GuildQSRewardList.find(dwFindId);
	return iter != m_GuildQSRewardList.end() ? &iter->second : NULL;
}

// ȡ����Ȩ������
const GuildPrivilege_Config* CConfigManager::GetGuildPrivilege(BYTE byPosition)
{
	map<DWORD, GuildPrivilege_Config>::iterator iter = m_GuildPrivilegeList.find(byPosition);
	return iter != m_GuildPrivilegeList.end() ? &iter->second : NULL;
}

// ȡ��������
const Guild_Config* CConfigManager::GetGuild(WORD wGuildLevel)
{
	map<DWORD, Guild_Config>::iterator iter = m_GuildList.find(wGuildLevel);
	return iter != m_GuildList.end() ? &iter->second : NULL;
}

// ȡ���Ἴ��
const GuildSkill_Config* CConfigManager::GetGuildSkill(WORD wSkillId, WORD wSkillLevel)
{
	DWORD dwFindId = 1000 * wSkillId + wSkillLevel;
	map<DWORD, GuildSkill_Config>::iterator iter = m_GuildSkillList.find(dwFindId);
	return iter != m_GuildSkillList.end() ? &iter->second : NULL;
}

// ȡ���Ὠ������
const GuildBuilding_Config* CConfigManager::GetGuildBuilding(WORD wBuildingId)
{
	map<WORD, GuildBuilding_Config>::iterator iter = m_GuildBuildingList.find(wBuildingId);
	return iter != m_GuildBuildingList.end() ? &iter->second : NULL;
}


// ȡ���ḱ������
const GuildFBChapter_Config* CConfigManager::GetGuildFB(WORD wChapter)
{
	map<WORD, GuildFBChapter_Config>::iterator iter = m_GuildFBChapterList.find(wChapter);
	return iter != m_GuildFBChapterList.end() ? &iter->second : NULL;
}

	// ȡ�̳���Ʒ
const ShopGoodsData_Config* CConfigManager::GetShopGoods(BYTE byType, WORD wGoodsId)
{
	DWORD dwFindId = byType * 10000 + wGoodsId;
	map<DWORD, ShopGoodsData_Config>::iterator iter = m_ShopGoodshList.find(dwFindId);	// �̵���Ʒ�б�
	return iter != m_ShopGoodshList.end() ? &iter->second : NULL;;
}

// ȡ�ؿ�����
const Map_Config* CConfigManager::GetMap(DWORD dwMapId)
{
	map<DWORD, Map_Config>::iterator iter = m_MapList.find(dwMapId);
	return iter != m_MapList.end() ? &iter->second : NULL;
}

// ȡս������
const Battle_Config* CConfigManager::GetMapBattle(DWORD dwBattleId)
{
	map<DWORD, Battle_Config>::iterator iter = m_BattleList.find(dwBattleId);
	return iter != m_BattleList.end() ? &iter->second : NULL;
}

// ȡ��������
const Monster_Config* CConfigManager::GetMonster(DWORD dwMonsterId)
{
	map<DWORD, Monster_Config>::iterator iter = m_MonsterList.find(dwMonsterId);
	return iter != m_MonsterList.end() ? &iter->second : NULL;
}

// ȡӢ����������
const HeroToBattle_Config* CConfigManager::GetHeroToBattle(BYTE byType, BYTE byPos)
{
	DWORD dwFindId = byType * 100 + byPos;
	map<DWORD, HeroToBattle_Config>::iterator iter = m_HeroToBattleList.find(dwFindId);
	return iter != m_HeroToBattleList.end() ? &iter->second : NULL;
}

// ȡӢ����������
const HeroPro_Config* CConfigManager::GetHeroPro(BYTE byProfession, BYTE byQuality, BYTE byLevel)
{
	DWORD dwFindId = 100000*byProfession + 1000*byQuality + byLevel;
	map<DWORD, HeroPro_Config>::iterator iter = m_HeroProList.find(dwFindId);
	return iter != m_HeroProList.end() ? &iter->second : NULL;
}

// ȡӢ���̵��λ����
const RefreshShopSlot_Config* CConfigManager::GetRefreshShopSlot(BYTE byShopType, BYTE bySlot)
{
	if (SHOP_AWAKEN == byShopType)
	{
		for (size_t i = 0; i < m_AwakenShopSlotList.size(); ++i)
		{
			if (m_AwakenShopSlotList[i].bySlot == bySlot)
				return &m_AwakenShopSlotList[i];
		}
	}
	else if (SHOP_HERO == byShopType)
	{
		for (size_t i = 0; i < m_HeroShopSlotList.size(); ++i)
		{
			if (m_HeroShopSlotList[i].bySlot == bySlot)
				return &m_HeroShopSlotList[i];
		}
	}
	else if (SHOP_STAR_SOUL == byShopType)
	{
		for (size_t i = 0; i < m_StarSoulShopSlotList.size(); ++i)
		{
			if (m_StarSoulShopSlotList[i].bySlot == bySlot)
				return &m_StarSoulShopSlotList[i];
		}
	}
	else if (SHOP_CRYSTAL == byShopType)
	{
		for (size_t i = 0; i < m_CrystalShopSlotList.size(); ++i)
		{
			if (m_CrystalShopSlotList[i].bySlot == bySlot)
				return &m_CrystalShopSlotList[i];
		}
	}
	return NULL;
}

// ȡ�Ѿ���ѫ����
const RebelExploit_Config* CConfigManager::GetRebelExploit(DWORD dwListId, DWORD wAwardId)
{
	DWORD dwFindId = 10000 * dwListId + wAwardId;
	map<DWORD, RebelExploit_Config>::iterator iter = m_RebelExpoitList.find(dwFindId);
	return iter != m_RebelExpoitList.end() ? &iter->second : NULL;
}

// ȡ�Ѿ�ս������
const RebelBattle_Config* CConfigManager::GetRebelBattle(WORD wRebelLevel)
{
	map<DWORD, RebelBattle_Config>::iterator iter = m_RebelBattleList.find(wRebelLevel);
	return iter != m_RebelBattleList.end() ? &iter->second : NULL;
}

// ȡ�Ѿ�Ʒ������
const RebelQuality_Config* CConfigManager::GetRebelQuality(BYTE byQuality)
{
	map<DWORD, RebelQuality_Config>::iterator iter = m_RebelQualityList.find(byQuality);
	return iter != m_RebelQualityList.end() ? &iter->second : NULL;
}

// ȡ��ɫ�Ѿ�����
const RoleRebel_Config* CConfigManager::GetRoleRebel(WORD wRoleLevel)
{
	map<DWORD, RoleRebel_Config>::iterator iter = m_RoleRebelList.find(wRoleLevel);
	return iter != m_RoleRebelList.end() ? &iter->second : NULL;
}


// ȡ�Ѿ�������������
const RebelRankAward_Config* CConfigManager::GetRebelRankAward(WORD wRank)
{
	map<DWORD, RebelRankAward_Config>::iterator iter = m_RebelRankAwardList.find(wRank);
	return iter != m_RebelRankAwardList.end() ? &iter->second : NULL;
}

// ȡװ����������
const EquipmentForge_Config* CConfigManager::GetEquipementStrengthenForge(WORD wLevel)
{
	vector<EquipmentForge_Config>::reverse_iterator riter = m_EquipmentEnhanceForgeList.rbegin();
	for (; riter != m_EquipmentEnhanceForgeList.rend(); ++riter)
	{
		if (wLevel >= riter->wLevel)
			return &*riter;
	}

	return NULL;
}



// ȡװ��������������
const EquipmentForge_Config* CConfigManager::GetEquipementRefineForge(WORD wLevel)
{
	vector<EquipmentForge_Config>::reverse_iterator riter = m_EquipmentRefineForgeList.rbegin();
	for (; riter != m_EquipmentRefineForgeList.rend(); ++riter)
	{
		if (wLevel >= riter->wLevel)
			return &*riter;
	}

	return NULL;
}

// ȡװ���츳����
const EquipmentTalent_Config* CConfigManager::GetEquipmentTalent(WORD wTalentId, WORD wEnhanceLevel)
{
	map<WORD, vector<EquipmentTalent_Config>>::iterator iter = m_EuipmentTalentList.find(wTalentId);
	if (iter == m_EuipmentTalentList.end())
		return NULL;

	EquipmentTalent_Config *pTanlentCfg = NULL;
	vector<EquipmentTalent_Config>::iterator iter2 = iter->second.begin();
	for (; iter2 != iter->second.end(); ++iter2)
	{
		// ����������,������
		if (wEnhanceLevel < iter2->wNeedEnhanceLevel)
			break;

		// ��¼����������
		pTanlentCfg = &*iter2;
	}

	return pTanlentCfg;
}

// ȡװ����������
const EquipmentStarUp_Config* CConfigManager::GetEquipmentStartUp(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel)
{
	// 00     00          00      00
	// type   quality     star    level 
	DWORD dwKey = wStarLevel + wStar * 100 + wQuality * 10000 + wType * 1000000;
	map<DWORD, EquipmentStarUp_Config>::iterator iter = m_EuipmentStarUpList.find(dwKey);
	return iter != m_EuipmentStarUpList.end() ? &iter->second : NULL;
}

// ȡװ����Ʒ����
const EquipmentQualityUp_Config* CConfigManager::GetEquipmentQualityUp(WORD wQuality)
{
	map<DWORD, EquipmentQualityUp_Config>::iterator iter = m_EuipmentQualityUpList.find(wQuality);
	return iter != m_EuipmentQualityUpList.end() ? &iter->second : NULL;
}

// ȡԮ����������
const ReinforcementEncourage_Config* CConfigManager::GetReinforcementEncourage(WORD wReinforcementLevel)
{
	map<DWORD, ReinforcementEncourage_Config>::iterator iter = m_ReinforcementEncourageList.find(wReinforcementLevel);
	return iter != m_ReinforcementEncourageList.end() ? &iter->second : NULL;
}



// ȡװ����������
const TreasureForge_Config* CConfigManager::GetTreasureStrengthenForge(WORD wLevel)
{
	vector<TreasureForge_Config>::reverse_iterator riter = m_TreasureEnhanceForgeList.rbegin();
	for (; riter != m_TreasureEnhanceForgeList.rend(); ++riter)
	{
		if (wLevel >= riter->wLevel)
			return &*riter;
	}

	return NULL;
}

// ȡ���ﾫ����������
const TreasureForge_Config* CConfigManager::GetTreasureRefineForge(WORD wLevel)
{
	vector<TreasureForge_Config>::reverse_iterator riter = m_TreasureRefineForgeList.rbegin();
	for (; riter != m_TreasureRefineForgeList.rend(); ++riter)
	{
		if (wLevel >= riter->wLevel)
			return &*riter;
	}

	return NULL;
}

// ȡ������������
const TreasureSmelt_Config* CConfigManager::GetTreasureSmelt(DWORD dwTreasureInfoId)
{
	map<DWORD, TreasureSmelt_Config>::iterator iter = m_TreasureSmeltList.find(dwTreasureInfoId);
	return iter != m_TreasureSmeltList.end() ? &iter->second : NULL;
}

// ȡ������Ʒ��������
const TreasureQualityUPCost_Config* CConfigManager::GetTreasureQualityUPCost(WORD wRefineLevel)
{
	map<DWORD, TreasureQualityUPCost_Config>::iterator iter = m_TreasureQualityUPCostList.find(wRefineLevel);
	return iter != m_TreasureQualityUPCostList.end() ? &iter->second : NULL;
}

// ȡ���շ�������
const RecycleReturn_Config* CConfigManager::GetRecycleReturn(BYTE byObjType, BYTE byObjLevel)
{
	DWORD dwFindId = 10000 * byObjType + byObjLevel;
	map<DWORD, RecycleReturn_Config>::iterator iter = m_RecycleReturnList.find(dwFindId);
	return iter != m_RecycleReturnList.end() ? &iter->second : NULL;
}

// ȡ�ᱦ�ؿ�����
const DuobaoMap_Config* CConfigManager::GetDuobaoMap(WORD wRoleLevel)
{
	map<DWORD, DuobaoMap_Config>::iterator iter = m_DuobaoMapList.find(wRoleLevel);
	return iter != m_DuobaoMapList.end() ? &iter->second : NULL;
}

// ȡ�ᱦ��������
const DuobaoDrop_Config* CConfigManager::GetDuobaoDrop(BYTE byQuality)
{
	map<DWORD, DuobaoDrop_Config>::iterator iter = m_DuobaoDropList.find(byQuality);
	return iter != m_DuobaoDropList.end() ? &iter->second : NULL;
}

// ȡ�ᱦ���Ƚ�������
const DuobaoPregrossReward_Config* CConfigManager::GetDuobaoPregrossReward(WORD wPregrossId)
{
	map<DWORD, DuobaoPregrossReward_Config>::iterator iter = m_DuobaoPregrossRewardList.find(wPregrossId);
	return iter != m_DuobaoPregrossRewardList.end() ? &iter->second : NULL;
}

const HeroNormalSummon_Config* CConfigManager::GetHeroNormalSummon(DWORD dwRate)
{
	int nCurRate = 0;
	std::vector<HeroNormalSummon_Config>::iterator iter;
	for (iter = m_NormalSummonList.begin(); iter != m_NormalSummonList.end(); ++iter)
	{
		nCurRate += iter->dwRate;
		if (iter->dwRate && dwRate <= nCurRate)
			return &(*iter);
	}

	return NULL;
}

// ȡӢ�۸߼��ٻ�����
const HeroAdvancedSummon_Config* CConfigManager::GetHeroAdvancedSummon(DWORD dwRate)
{
	int nCurRate = 0;
	std::vector<HeroAdvancedSummon_Config>::iterator iter;
	for (iter = m_AdvancedSummonList.begin(); iter != m_AdvancedSummonList.end(); ++iter)
	{
		nCurRate += iter->dwRate;
		if (iter->dwRate && dwRate <= nCurRate)
			return &(*iter);
	}

	return NULL;
}

// ȡӢ�۸߼��ٻ�����
const HeroAdvancedSummon_Config* CConfigManager::GetHeroAdvancedSummonEx(DWORD dwRate)
{
	int nCurRate = 0;
	std::vector<HeroAdvancedSummon_Config>::iterator iter;
	for (iter = m_AdvancedSummonList.begin(); iter != m_AdvancedSummonList.end(); ++iter)
	{
		nCurRate += iter->dwRate1;
		if (iter->dwRate1 && dwRate <= nCurRate)
			return &(*iter);
	}

	return NULL;
}

// ȡӢ����ͨ�ٻ��ܵļ��� 
DWORD CConfigManager::GetHeroNormalSummonTotalRate()
{
	DWORD dwTotalRate = 0;
	std::vector<HeroNormalSummon_Config>::iterator iter;
	for (iter = m_NormalSummonList.begin(); iter != m_NormalSummonList.end(); ++iter)
	{
		dwTotalRate += iter->dwRate;
	}

	return dwTotalRate;
}

// ȡӢ�۸߼��ٻ��ܵļ��� 
DWORD CConfigManager::GetHeroAdvancedSummonTotalRate()
{
	DWORD dwTotalRate = 0;
	std::vector<HeroAdvancedSummon_Config>::iterator iter;
	for (iter = m_AdvancedSummonList.begin(); iter != m_AdvancedSummonList.end(); ++iter)
	{
		dwTotalRate += iter->dwRate;
	}

	return dwTotalRate;
}

// ȡӢ�۸߼��ٻ��ܵļ���
DWORD CConfigManager::GetHeroAdvancedSummonTotalRateEx()
{
	DWORD dwTotalRate = 0;
	std::vector<HeroAdvancedSummon_Config>::iterator iter;
	for (iter = m_AdvancedSummonList.begin(); iter != m_AdvancedSummonList.end(); ++iter)
	{
		dwTotalRate += iter->dwRate1;
	}

	return dwTotalRate;
}

// ȡӢ��VIP��ļ�ܼ���
DWORD CConfigManager::GetHeroVIPSummonTotalRate(WORD wCamp, bool isSpecial)
{
	DWORD dwTotalRate = 0;
	map<DWORD, vector<HeroVipSummon_Config>>::iterator iter = m_HeroVipSummonList.find(wCamp);
	if (iter == m_HeroVipSummonList.end())
	{
		return dwTotalRate;
	}

	std::vector<HeroVipSummon_Config>::iterator iter1;
	for (iter1 = iter->second.begin(); iter1 != iter->second.end(); ++iter1)
	{
		dwTotalRate += (isSpecial ? iter1->dwSpecialRate : iter1->dwNormalRate);
	}

	return dwTotalRate;
}

// ȡӢ��VIP��ļ�ܼ���
const HeroVipSummon_Config* CConfigManager::GetHeroVIPSummon(WORD wCamp, bool isSpecial, DWORD dwRate)
{
	map<DWORD, vector<HeroVipSummon_Config>>::iterator list_iter = m_HeroVipSummonList.find(wCamp);
	if (list_iter == m_HeroVipSummonList.end())
		return NULL;

	DWORD dwCurRate = 0;
	std::vector<HeroVipSummon_Config>::iterator iter;
	for (iter = list_iter->second.begin(); iter != list_iter->second.end(); ++iter)
	{
		if (isSpecial)
		{
			dwCurRate += iter->dwSpecialRate;

			if (iter->dwSpecialRate && dwRate <= dwCurRate)
				return &(*iter);
		}
		else
		{
			dwCurRate += iter->dwNormalRate;

			if (iter->dwNormalRate && dwRate <= dwCurRate)
				return &(*iter);
		}
	}

	return NULL;
}

// ȡVip����
const Vip_Config* CConfigManager::GetVip(WORD wVipLevel)
{
	map<WORD, Vip_Config>::iterator iter = m_VipList.find(wVipLevel);
	return iter != m_VipList.end() ? &iter->second : NULL;
}

const VipGM_Config* CConfigManager::GetVipGM(WORD wGMLevel)
{
	map<WORD, VipGM_Config>::iterator iter = m_VipGMList.find(wGMLevel);
	return iter != m_VipGMList.end() ? &iter->second : NULL;
}

// ȡVIPGM�ȼ�����
WORD CConfigManager::GetVipGMLevel(DWORD dwRechargeAmount)
{
	WORD wGMLevel = 0;
	map<WORD, VipGM_Config>::iterator iter = m_VipGMList.begin();
	for (; iter != m_VipGMList.end(); ++iter)
	{
		// 0��ʾ����,����������������һ��
		if (iter->second.dwNeedRechargeAmount && dwRechargeAmount >= iter->second.dwNeedRechargeAmount)
		{
			if (iter->second.wGMLevel >= wGMLevel)
			{
				wGMLevel = iter->second.wGMLevel + 1;
			}
			//wGMLevel = max(wGMLevel, iter->second.wGMLevel);
		}
	}
	return wGMLevel;
}

// ȡVip����Id
DWORD CConfigManager::GetVipNoticeId(WORD wVipLevel)
{
	map<WORD, DWORD>::iterator iter = m_VipNoticeList.find(wVipLevel);
	return iter != m_VipNoticeList.end() ? iter->second : 0;
}

// ȡVIP�ȼ�����
WORD CConfigManager::GetMaxVipLevel()
{
	WORD wMaxLevel = 0;
	map<WORD, Vip_Config>::iterator iter = m_VipList.begin();
	for (; iter != m_VipList.end(); ++iter)
	{
		wMaxLevel = max(wMaxLevel, iter->first);
	}

	return wMaxLevel;
	//return m_VipList.size();
}

// ȡ�ؿ������б�
bool CConfigManager::GetMapMonsterList(WORD wMapId, vector<MonsterData>* pMonsterList)
{
	const Map_Config *pMapCfg = CConfigManager::Instance()->GetMap(wMapId);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ���������[MapId:%d]", __FUNCTION__, wMapId);
		return  false;
	}

	if (!pMapCfg->vBattleList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ؿ�ս���б�����Ϊ��[MapId:%d]", __FUNCTION__, wMapId);
		return false;
	}

	DWORD dwBattleId = pMapCfg->vBattleList[0];
	const Battle_Config *pBattleCfg = CConfigManager::Instance()->GetMapBattle(dwBattleId);
	if (!pBattleCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ս������[BattleId:%d]", __FUNCTION__, dwBattleId);
		return false;
	}

	MonsterData tMonster;
	for (int i = 0; i < MAX_BATTLE_MONSTER_NUM; ++i)
	{
		DWORD dwMonsterId = pBattleCfg->arMonsterList[i];
		if (!dwMonsterId)
			continue;

		const Monster_Config *pMonsterCfg = CConfigManager::Instance()->GetMonster(dwMonsterId);
		if (!pMonsterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[MonsterId:%d]", __FUNCTION__, dwMonsterId);
			return false;
		}

		memset(&tMonster, 0, sizeof(tMonster));
		tMonster.byIndex = i;
		tMonster.dwTotalHP = pMonsterCfg->dwBlood;
		pMonsterList->push_back(tMonster);
	}

	return true;
}

// ȡϵͳ���ŵȼ�
WORD CConfigManager::GetSystemOpenLevel(DWORD dwSystemId)
{
	map<DWORD, WORD>::iterator iter = m_SystemOpenList.find(dwSystemId);
	return iter != m_SystemOpenList.end() ? iter->second : NULL;
}

// ȡ������Ŀ��������б�
const ScoreMatchServerGroup_Config* CConfigManager::GetScoreMatchTargetServerList(DWORD dwServerId)
{
	map<DWORD, ScoreMatchServerGroup_Config>::iterator iter = m_ScoreMatchTargetServerList.find(dwServerId);
	return iter != m_ScoreMatchTargetServerList.end() ? &iter->second : NULL;
}

// ȡ������������
const ScoreMatchRobot_Config* CConfigManager::GetScoreMatchRobot(DWORD dwRobotId)
{
	for (size_t i = 0; i < m_ScoreMatchRobotList.size(); ++i)
	{
		if (m_ScoreMatchRobotList[i].dwRobotId == dwRobotId)
			return &m_ScoreMatchRobotList[i];
	}
	return NULL;
}

// ��������ս��������
const ScoreMatchBattleTask_Config* CConfigManager::GetScoreMatchBattleTask(WORD wTaskId)
{
	map<DWORD, ScoreMatchBattleTask_Config>::iterator iter = m_ScoreMatchBattleTaskList.find(wTaskId);
	return iter != m_ScoreMatchBattleTaskList.end() ? &iter->second : NULL;
}

// ȡ���������н���
const ScoreMatchRankReward_Config* CConfigManager::GetScoreMatchRankReward(WORD wRank)
{
	map<DWORD, ScoreMatchRankReward_Config>::iterator iter = m_ScoreMatchRankRewardList.find(wRank);
	return iter != m_ScoreMatchRankRewardList.end() ? &iter->second : NULL;
}

// ȡ�������ʱ��
const ScoreMathActivityTime_Config* CConfigManager::GetScoreMatchActivityTime(WORD wIndex)
{
	map<DWORD, ScoreMathActivityTime_Config>::iterator iter = m_ScoreMatchActivityTimeList.find(wIndex);
	return iter != m_ScoreMatchActivityTimeList.end() ? &iter->second : NULL;
}

// ȡ��������������s
int CConfigManager::GetScoreMatchTimesCost(WORD wType, WORD wTimes)
{
	DWORD dwKey = wType * 10000 + wTimes;
	map<DWORD, int>::iterator iter = m_ScoreMatchTimesCostList.find(dwKey);
	return iter != m_ScoreMatchTimesCostList.end() ? iter->second : 0;
}

// ȡ����������Ŀ������
const ScoreMatchSearchTarget_Config* CConfigManager::GetScoreMatchSearchTarget(WORD wType)
{
	map<DWORD, ScoreMatchSearchTarget_Config>::iterator iter = m_ScoreMatchSearchList.find(wType);
	return iter != m_ScoreMatchSearchList.end() ? &iter->second : NULL;
}

// ȡ��������������
const WarcraftRank_Config* CConfigManager::GetWarcraftRank(WORD wRank)
{
	return NULL;
}

// ȡ�ƺ�����
const Title_Config* CConfigManager::GetTitle(int nTitleId)
{
	map<DWORD, Title_Config>::iterator iter = m_TitleList.find(nTitleId);
	return iter != m_TitleList.end() ? &iter->second : NULL;
}

// ȡ�����
const vector<Activity_Config>* CConfigManager::GetActivityTime(int nActivityId)
{
	map<DWORD, vector<Activity_Config>>::iterator iter = m_ActivityTimeList.find(nActivityId);
	return iter != m_ActivityTimeList.end() ? &iter->second : NULL;
}

const Activity_Config* CConfigManager::GetActivityTime(int nActivityId, WORD wDataId)
{
	const vector<Activity_Config> *pActivityListCfg = CConfigManager::GetActivityTime(nActivityId);
	if (pActivityListCfg)
	{
		vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
		for (; iter != pActivityListCfg->end(); ++iter)
		{
			if (iter->wDataId == wDataId)
				return &*iter;
		}
	}
	return NULL;
}

// ȡ�����
const LimitActivity_Config* CConfigManager::GetLimitActivity(WORD wDataId)
{
	map<DWORD, LimitActivity_Config>::iterator iter = m_LimitActivityList.find(wDataId);
	return iter != m_LimitActivityList.end() ? &iter->second : NULL;
}


// ȡ��������
const Reward_Config* CConfigManager::GetReward(DWORD dwRewardId)
{
	map<DWORD, Reward_Config>::iterator iter = m_RewardList.find(dwRewardId);
	return iter != m_RewardList.end() ? &iter->second : NULL;
}

// ȡ����������
const ActivationCode_Config* CConfigManager::GetActivationCode(const char *pszCode)
{
	map<string, ActivationCode_Config>::iterator iter = m_ActivationCodeList.find(pszCode);
	return iter != m_ActivationCodeList.end() ? &iter->second : NULL;
}

// ȡ�����뽱������
const ActivationCodeReward_Config* CConfigManager::GetActivationCodeReward(DWORD dwRewardId)
{
	map<DWORD, ActivationCodeReward_Config>::iterator iter = m_ActivationCodeRewardList.find(dwRewardId);
	return iter != m_ActivationCodeRewardList.end() ? &iter->second : NULL;
}

// ȡ����ͳ������
const ReyunStatistic_Config* CConfigManager::GetReyunStatistic(WORD wType)
{
	map<WORD, ReyunStatistic_Config>::iterator iter = m_ReyunStatisticList.find(wType);
	return iter != m_ReyunStatisticList.end() ? &iter->second : NULL;
}

const FestivalActivtiyTarget_Config* CConfigManager::GetFestivalActivityTarget(DWORD dwTargetId)
{
	map<DWORD, FestivalActivtiyTarget_Config>::iterator iter = m_FestivalActivtiyTargetList.find(dwTargetId);
	return iter != m_FestivalActivtiyTargetList.end() ? &iter->second : NULL;
}

// ȡ���ջ����
const vector<FestivalActivity_Config>* CConfigManager::GetFestivalActivity(WORD wActivityType)
{
	map<DWORD, vector<FestivalActivity_Config>>::iterator iter = m_FestivalActivityList.find(wActivityType);
	return iter != m_FestivalActivityList.end() ? &iter->second : NULL;
}

// ȡ���ջ��ԾĿ���б�
const vector<WORD>* CConfigManager::GetFestivalActivitySameLogicTargetList(WORD wLogicType)
{
	map<WORD, vector<WORD>>::iterator iter = m_FestivalActivitySameLogicTargetList.find(wLogicType);
	return iter != m_FestivalActivitySameLogicTargetList.end() ? &iter->second : NULL;
}

// ȡ��λ����
const Nobility_Config* CConfigManager::GetBobility(WORD wNobilityLevel)
{
	map<WORD, Nobility_Config>::iterator iter = m_NobilityList.find(wNobilityLevel);
	return iter != m_NobilityList.end() ? &iter->second : NULL;
}

// ȡ��ʯ����
const Gem_Config* CConfigManager::GetGem(DWORD dwGemId)
{	
	map<DWORD, Gem_Config>::iterator iter = m_GemList.find(dwGemId);
	return iter != m_GemList.end() ? &iter->second : NULL;
}

// ȡ��ʯ����
const GemSkill_Config* CConfigManager::GetGemSkill(WORD wSkillId, WORD wSkillLevel)
{
	DWORD dwKey = wSkillId * 100 + wSkillLevel;
	map<DWORD, GemSkill_Config>::iterator iter = m_GemSkillList.find(dwKey);
	return iter != m_GemSkillList.end() ? &iter->second : NULL;
}

// ȡ��ʯ��λ
const GemHole_Config* CConfigManager::GetGemHole(WORD wHole)
{
	map<DWORD, GemHole_Config>::iterator iter = m_GemHoleList.find(wHole);
	return iter != m_GemHoleList.end() ? &iter->second : NULL;
}

// ȡ��ʯ��Ƕ����
const GemInlay_Config* CConfigManager::GetGemInlay(WORD wType)
{
	map<DWORD, GemInlay_Config>::iterator iter = m_GemInlayList.find(wType);
	return iter != m_GemInlayList.end() ? &iter->second : NULL;
}

const vector<TeamDungeonLottery_Config>& CConfigManager::GetTeamDungeonLotteryList()
{ 
	return m_TeamDungeonLotteryList; 
};

// ȡ��Ӹ�������
const TeamDungeon_Config* CConfigManager::GetTeamDungeon(DWORD dwDungeonId)
{
	map<DWORD, TeamDungeon_Config>::iterator iter = m_TeamDungeonList.find(dwDungeonId);
	return iter != m_TeamDungeonList.end() ? &iter->second : NULL;
}

// ȡ��������
const TeamDungeonTask_Config* CConfigManager::GetTeamDungeonTask(DWORD dwTaskId)
{
	map<DWORD, TeamDungeonTask_Config>::iterator iter = m_TeamDungeonTaskList.find(dwTaskId);
	return iter != m_TeamDungeonTaskList.end() ? &iter->second : NULL;
}

// ȡ����������
const TeamDungeonTaskLink_Config* CConfigManager::GetTeamDungeonTaskLink(DWORD dwTaskLinkId)
{
	map<DWORD, TeamDungeonTaskLink_Config>::iterator iter = m_TeamDungeonTaskLinkList.find(dwTaskLinkId);
	return iter != m_TeamDungeonTaskLinkList.end() ? &iter->second : NULL;
}

// ȡBOSS��������
const TeamDungeonBossReward_Config* CConfigManager::GetTeamDungeonBossReward(WORD wDungeonId, int nEnergy, int nDamageRate)
{
	map<DWORD, vector<TeamDungeonBossReward_Config>>::iterator iter = m_TeamDungeonBossRewardList.find(wDungeonId);
	if (iter != m_TeamDungeonBossRewardList.end())
	{
		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			TeamDungeonBossReward_Config *pRewardCfg = &iter->second.at(i);
			if (nEnergy >= pRewardCfg->nEnergyMin && nEnergy <= pRewardCfg->nEnergyMax &&
				nDamageRate >= pRewardCfg->nDamageRateMin && nDamageRate <= pRewardCfg->nDamageRateMax)
			{
				return pRewardCfg;
			}
		}
	}

	return NULL;
}

// ȡ��λ����
const KingMatchStage_Config* CConfigManager::GetKingMatchStage(WORD wStageLevel)
{
	map<WORD, KingMatchStage_Config>::iterator iter = m_KingMatchStageList.find(wStageLevel);
	return iter != m_KingMatchStageList.end() ? &iter->second : NULL;
}

// ȡ����������ʱ��
const KingMatchSeasonTime_Config* CConfigManager::GetKingMatchSeasonTime(WORD wSeason)
{
	map<WORD, KingMatchSeasonTime_Config>::iterator iter = m_KingMatchSeasonTimeList.find(wSeason);
	return iter != m_KingMatchSeasonTimeList.end() ? &iter->second : NULL;
}

// ȡ����������������
const KingMatchSeasonTask_Config* CConfigManager::GetKingMatchSeasonTask(WORD wWinTimes)
{
	map<WORD, KingMatchSeasonTask_Config>::iterator iter = m_KingMatchSeasonTaskList.find(wWinTimes);
	return iter != m_KingMatchSeasonTaskList.end() ? &iter->second : NULL;
}

// ȡ�������������н�������
const KingMatchRankReward_Config* CConfigManager::GetKingMatchRankReward(DWORD dwRank)
{
	vector<KingMatchRankReward_Config>::iterator iter = m_KingMatchRankRewardList.begin();
	for (; iter != m_KingMatchRankRewardList.end(); ++iter)
	{
		if (dwRank >= iter->wRankMin && (!iter->wRankMax || dwRank <= iter->wRankMax))
			return &*iter;
	}
	return NULL;
}

// ȡʥ����ѩ������
const ChristmasSnowball_Config* CConfigManager::GetChristmasSnowball(DWORD dwSnowballId)
{
	map<DWORD, ChristmasSnowball_Config>::iterator iter = m_ChristmasSnowballList.find(dwSnowballId);
	return iter != m_ChristmasSnowballList.end() ? &iter->second : NULL;
}

// ȡʥ����BOSS����
const ChristmasBoss_Config* CConfigManager::GetChristmasBoss(WORD wBossType)
{
	map<DWORD, ChristmasBoss_Config>::iterator iter = m_ChristmasBossList.find(wBossType);
	return iter != m_ChristmasBossList.end() ? &iter->second : NULL;
}

// ȡʥ����BOSS�ȼ�����
const ChristmasBossLevel_Config* CConfigManager::GetChristmasBossLevel(WORD wBossType, DWORD dwBossLevel)
{
	DWORD dwFindId = wBossType * 100000 + dwBossLevel;
	map<DWORD, ChristmasBossLevel_Config>::iterator iter = m_ChristmasBossLevelList.find(dwFindId);
	return iter != m_ChristmasBossLevelList.end() ? &iter->second : NULL;
}

// ȡfacebook������������
const FacebookFriendInvite_Config* CConfigManager::GetFacebookFriendInvite(WORD wInviteNum)
{
	map<DWORD, FacebookFriendInvite_Config>::iterator iter = m_FacebookFriendInvitList.find(wInviteNum);
	return iter != m_FacebookFriendInvitList.end() ? &iter->second : NULL;
}

// ȡfacebook�����ٻ�����
const FacebookFriendSummon_Config* CConfigManager::GetFacebookFriendSummon(WORD wSummonNum)
{
	map<DWORD, FacebookFriendSummon_Config>::iterator iter = m_FacebookFriendSummonList.find(wSummonNum);
	return iter != m_FacebookFriendSummonList.end() ? &iter->second : NULL;
}

// ȡfacebook�ʺŰ�����
const FacebookAccountBinding_Config* CConfigManager::GetFacebookAccountBinding(DWORD dwRewardId)
{
	map<DWORD, FacebookAccountBinding_Config>::iterator iter = m_FacebookAccountBindingList.find(dwRewardId);
	return iter != m_FacebookAccountBindingList.end() ? &iter->second : NULL;
}

// ȡfacebook��������
const FacebookShare_Config* CConfigManager::GetFacebookShare(WORD wChareChapter)
{
	map<DWORD, FacebookShare_Config>::iterator iter = m_FacebookChareList.find(wChareChapter);
	return iter != m_FacebookChareList.end() ? &iter->second : NULL;
}

// ȡfacebook��������
const FacebookCommend_Config* CConfigManager::GetFacebookCommend(WORD wChapter)
{
	map<DWORD, FacebookCommend_Config>::iterator iter = m_FacebookCommendList.find(wChapter);
	return iter != m_FacebookCommendList.end() ? &iter->second : NULL;
}

// ȡ��ֵ��������
const RechargeReward_Config* CConfigManager::GetRechargeReward(WORD wActivityDataId, DWORD dwDay)
{
	DWORD dwKey = wActivityDataId * 1000 + dwDay;
	map<DWORD, RechargeReward_Config>::iterator iter = m_RechargeRewardList.find(dwKey);
	return iter != m_RechargeRewardList.end() ? &iter->second : NULL;
}

// ȡ��ֵ������������
const RechargeExtraReward_Config* CConfigManager::GetRechargeExtraReward(WORD wActivityDataId)
{
	map<DWORD, RechargeExtraReward_Config>::iterator iter = m_RechargeExtraRewardList.find(wActivityDataId);
	return iter != m_RechargeExtraRewardList.end() ? &iter->second : NULL;
}

const AppleIap_Config* CConfigManager::GetAppleIap(const char *pIapId)
{
	map<string, AppleIap_Config>::iterator iter = m_AppleIAPList.find(pIapId);
	return iter != m_AppleIAPList.end() ? &iter->second : NULL;
}


// ȡ�۳佱������
const TotalRechargeReward_Config* CConfigManager::GetTotalRechargeReward(WORD wDataId, WORD wRewardId)
{
	DWORD dwFindId = 100000 * wDataId + wRewardId;
	map<DWORD, TotalRechargeReward_Config>::iterator iter = m_TotalRechargeRewardList.find(dwFindId);
	return iter != m_TotalRechargeRewardList.end() ? &iter->second : NULL;
}

// ȡ������Ʒ����
const HotSaleGoods_Config* CConfigManager::GetHotSaleGoods(DWORD dwDataId, WORD wGoodsId)
{
	DWORD dwKey = dwDataId * 100000 + wGoodsId;
	map<DWORD, HotSaleGoods_Config>::iterator iter = m_HotSaleGoodsList.find(dwKey);
	return iter != m_HotSaleGoodsList.end() ? &iter->second : NULL;
}

// ȡת������
const Turntable_Config* CConfigManager::GetTurntableData(WORD wDataId)
{
	map<DWORD, Turntable_Config>::iterator iter = m_TurntableList.find(wDataId);
	return iter != m_TurntableList.end() ? &iter->second : NULL;
}

// ȡת�̳齱����
const vector<TurntableLottery_Config>* CConfigManager::GetTurntableLottery(WORD wDataId)
{
	map<DWORD, vector<TurntableLottery_Config>>::iterator iter = m_TurntableLotteryList.find(wDataId);
	return iter != m_TurntableLotteryList.end() ? &iter->second : NULL;
}

// ȡת�����а�����
const TurntableRank_Config* CConfigManager::GetTurntableRank(WORD wDataId, WORD wRank)
{
	DWORD dwKey = 10000 * wDataId + wRank;
	map<DWORD, TurntableRank_Config>::iterator iter = m_TurntableRankList.find(dwKey);
	return iter != m_TurntableRankList.end() ? &iter->second : NULL;
}

// ȡ��ʱ��������
const LimitProduct_Config* CConfigManager::GetLimitProduct(WORD wSystemId)
{
	map<DWORD, LimitProduct_Config>::iterator iter = m_LimitProductList.find(wSystemId);
	return iter != m_LimitProductList.end() ? &iter->second : NULL;
}


// ȡ��ʱ��������
const CampRecruit_Config* CConfigManager::GetCampRecruit(WORD wDataId, WORD wCamp)
{
	DWORD dwKey = 100 * wDataId + wCamp;
	map<DWORD, CampRecruit_Config>::iterator iter = m_CampRecruitList.find(dwKey);
	return iter != m_CampRecruitList.end() ? &iter->second : NULL;
}

// ȡ��Ӫ��ļӢ�������б�
const vector<CampRecruitHero_Config>* CConfigManager::GetCampRecruitHeroList(WORD wDataId, WORD wCamp)
{
	DWORD dwKey = 100 * wDataId + wCamp;
	map<DWORD, vector<CampRecruitHero_Config>>::iterator iter = m_CampRecruitHeroList.find(dwKey);
	return iter != m_CampRecruitHeroList.end() ? &iter->second : NULL;
}

// ȡunlock�����
const UnlockActivityReward_Config* CConfigManager::GetUnlockActivityReward(WORD wRewardId)
{
	map<DWORD, UnlockActivityReward_Config>::iterator iter = m_UnlockActivityRewardList.find(wRewardId);
	return iter != m_UnlockActivityRewardList.end() ? &iter->second : NULL;
}

// ȡunlock�ۿ���Ƶ����
const UnlockWatchingVideoReward_Config* CConfigManager::GetUnlockWatchingVideoReward(WORD wWatchingTimes)
{
	map<DWORD, UnlockWatchingVideoReward_Config>::iterator iter = m_UnlockWathingVideoRewardList.find(wWatchingTimes);
	return iter != m_UnlockWathingVideoRewardList.end() ? &iter->second : NULL;
}

// ȡ����ȼ���������
const WorldTaskLevel_Config* CConfigManager::GetWorldTaskLevelData(WORD wOpenServerDays)
{
	map<DWORD, WorldTaskLevel_Config>::iterator iter = m_WorldTaskLevelList.find(wOpenServerDays);
	return iter != m_WorldTaskLevelList.end() ? &iter->second : NULL;
}

// ȡ����ȼ�����ID�б�
const WorldLevelTaskIdList_Config* CConfigManager::GetWorldLevelTaskIdList(WORD wTaskLevel)
{
	map<DWORD, WorldLevelTaskIdList_Config>::iterator iter = m_WorldLevelTaskIdList.find(wTaskLevel);
	return iter != m_WorldLevelTaskIdList.end() ? &iter->second : NULL;
}

// ȡ����ȼ�����ID�б�
const WorldLevelTaskIdList_Config* CConfigManager::GetWorldLevelTaskIdListByTaskId(WORD wTaskId)
{
	map<DWORD, WorldLevelTaskIdList_Config>::iterator iter = m_WorldLevelTaskIdListEx.find(wTaskId);
	return iter != m_WorldLevelTaskIdListEx.end() ? &iter->second : NULL;
}

// ȡ����ȼ������¼�
const WorldLevelTaskEvent_Config* CConfigManager::GetWorldLevelTaskEvent(WORD wTaskId)
{
	map<DWORD, WorldLevelTaskEvent_Config>::iterator iter = m_WorldLevelTaskEventList.find(wTaskId);
	return iter != m_WorldLevelTaskEventList.end() ? &iter->second : NULL;
}

// ȡ��Դս�½�����
const ResourceWarChapter_Config* CConfigManager::GetResourceWarChapter(WORD wChapter)
{
	map<DWORD, ResourceWarChapter_Config>::iterator iter = m_ResourceWarChapterList.find(wChapter);
	return iter != m_ResourceWarChapterList.end() ? &iter->second : NULL;
}

// ȡ��Դս��Դ����
const ResourceWarResource_Config* CConfigManager::GetResourceWarResource(WORD wResourceId)
{
	map<DWORD, ResourceWarResource_Config>::iterator iter = m_ResourceWarResourceList.find(wResourceId);
	return iter != m_ResourceWarResourceList.end() ? &iter->second : NULL;
}

// ȡ��Դս�Ƽ�����
const ResourceWarTechnology_Config* CConfigManager::GetResourceWarTecknology(WORD wTechnologyLevel)
{
	map<WORD, ResourceWarTechnology_Config>::iterator iter = m_ResourceWarTechnologyList.find(wTechnologyLevel);
	return iter != m_ResourceWarTechnologyList.end() ? &iter->second : NULL;
}

// ȡ��Դս����ռ������
const ResourceWarGuildProfit_Config* CConfigManager::GetResourceWarGuildProfit(WORD wMemberNum)
{
	map<WORD, ResourceWarGuildProfit_Config>::iterator iter = m_ResourceWarGuildProfitList.find(wMemberNum);
	return iter != m_ResourceWarGuildProfitList.end() ? &iter->second : NULL;
}

// ȡ����ս������
const GuildWarStar_Config* CConfigManager::GetGuildWarStar(DWORD dwStarId)
{
	map<DWORD, GuildWarStar_Config>::iterator iter = m_GuildWarStarList.find(dwStarId);
	return iter != m_GuildWarStarList.end() ? &iter->second : NULL;
}

// ȡ����ս������
const GuildWarTower_Config* CConfigManager::GetGuildWarTower(DWORD dwTowerId)
{
	map<DWORD, GuildWarTower_Config>::iterator iter = m_GuildWarTowerList.find(dwTowerId);
	return iter != m_GuildWarTowerList.end() ? &iter->second : NULL;
}

// ����ս���н���
const GuildWarRankReward_Config* CConfigManager::GetGuildWarRankReward(WORD wRank)
{
	map<DWORD, GuildWarRankReward_Config>::iterator iter = m_GuildWarRankRewardList.find(wRank);
	return iter != m_GuildWarRankRewardList.end() ? &iter->second : NULL;
}

// ����ս��ս����
const GuildWarVSReward_Config* CConfigManager::GetGuildWarVSReward(WORD wVSType, WORD wVSResult)
{
	DWORD dwKey = 100 * wVSType + wVSResult;
	map<DWORD, GuildWarVSReward_Config>::iterator iter = m_GuildWarVSRewardList.find(dwKey);
	return iter != m_GuildWarVSRewardList.end() ? &iter->second : NULL;
}

// ����ս��ս����
const vector<GuildWarVSActivityTime_Config>* CConfigManager::GetGuildWarVSActivityTime(WORD wTimeType)
{
	map<DWORD, vector<GuildWarVSActivityTime_Config>>::iterator iter = m_GuildWarVSActivityTimeList.find(wTimeType);
	return iter != m_GuildWarVSActivityTimeList.end() ? &iter->second : NULL;
}

// ����սͨ������
const Server_Config* CConfigManager::GetGuildWarCommon(DWORD dwKey)
{
	map<DWORD, Server_Config>::iterator iter = m_GuildWarCommonCfgList.find(dwKey);
	return iter != m_GuildWarCommonCfgList.end() ? &iter->second : NULL;
}

// ȡ��������
const GuildWarOrganizeDefense_Config* CConfigManager::GetGuildWarOrganizeDefense(WORD wTowerType)
{
	map<DWORD, GuildWarOrganizeDefense_Config>::iterator iter = m_GuildWarOrganizeDefenseList.find(wTowerType);
	return iter != m_GuildWarOrganizeDefenseList.end() ? &iter->second : NULL;
}

const GuildWarRegion_Config* CConfigManager::GetGuildWarRegion(WORD wRegionId)
{
	map<DWORD, GuildWarRegion_Config>::iterator iter = m_GuildWarRegionList.find(wRegionId);
	return iter != m_GuildWarRegionList.end() ? &iter->second : NULL;
}

// ȡ����ս��������
const GuildWarRegion_Config* CConfigManager::GetGuildWarRegionByServerId(WORD wServerId)
{
	map<DWORD, GuildWarRegion_Config>::iterator iter = m_GuildWarRegionList.begin();
	for (; iter != m_GuildWarRegionList.end(); ++iter)
	{
		if (iter->second.mServerList.find(wServerId) != iter->second.mServerList.end())
			return &iter->second;
	}
	return NULL;
}

// ȡygame֧������
const YGamePayment_Config* CConfigManager::GetYGamePayment(string strPaymentId)
{
	map<string, YGamePayment_Config>::iterator iter = m_YGamePaymentList.find(strPaymentId);
	return iter != m_YGamePaymentList.end() ? &iter->second : NULL;
}

// ȡygame֧������
const YGamePayment_Config* CConfigManager::GetYGamePayment(DWORD dwAssistId)
{
	map<string, YGamePayment_Config>::iterator iter = m_YGamePaymentList.begin();
	for (; iter != m_YGamePaymentList.end(); ++iter){
		if (iter->second.dwAssistId == dwAssistId)
			return &iter->second;
	}
	return NULL;
}

const RechargeRankReward_Config* CConfigManager::GetRechargeAndCostRankReward(WORD wRankType, WORD wRank)
{
	DWORD dwKey = wRankType * 10000 + wRank;
	map<DWORD, RechargeRankReward_Config>::iterator iter = m_RechargeRankRewardList.find(dwKey);
	return iter != m_RechargeRankRewardList.end() ? &iter->second : NULL;
}



// ����Ƿ��ʺŰ�����
bool CConfigManager::IsAccountWhite(const char *pszAccount)
{
	map<string, string>::iterator iter = m_AccountWhiteList.find(pszAccount);
	return iter != m_AccountWhiteList.end() ? true : false;
}

// ����Ƿ�IP������
bool CConfigManager::IsIPWhite(const char *pszIP)
{
	map<string, string>::iterator iter = m_IPWhiteList.find(pszIP);
	return iter != m_IPWhiteList.end() ? true : false;
}

// ȡVIPÿ�����
const VipDailyGift_Config* CConfigManager::GetVipDailyGift(WORD wVipLevel)
{
	map<DWORD, VipDailyGift_Config>::iterator iter = m_VipDailyGiftList.find(wVipLevel);
	return iter != m_VipDailyGiftList.end() ? &iter->second : NULL;
}

// ȡVIP�����
const VipWeekGift_Config* CConfigManager::GetVipWeekGift(WORD wGiftId)
{
	map<DWORD, VipWeekGift_Config>::iterator iter = m_VipWeekGiftList.find(wGiftId);
	return iter != m_VipWeekGiftList.end() ? &iter->second : NULL;
}

// ȡ��ɫ�ȼ�VIP������б�
const VipWeekGiftRandList_Config* CConfigManager::GetVipWeekGiftRandList(WORD wRoleLevel)
{
	map<DWORD, VipWeekGiftRandList_Config>::iterator iter = m_VipWeekGiftRandList.find(wRoleLevel);
	return iter != m_VipWeekGiftRandList.end() ? &iter->second : NULL;
}

// ȡ�����Ƽ��������
const TodayRecommendRecharge_Config* CConfigManager::GetTodayRecommendRecharge(WORD wGiftId)
{
	map<WORD, TodayRecommendRecharge_Config>::iterator iter = m_TodayRecommendRechargeList.find(wGiftId);
	return iter != m_TodayRecommendRechargeList.end() ? &iter->second : NULL;
}

// ȡ�����Ƽ���ֵ����б�
const vector<RateDataEx>* CConfigManager::GetTodayRecommendRechargeRandList(WORD wVipLevel)
{
	map<WORD, vector<RateDataEx>>::iterator iter = m_TodayRecommendRechargeRandList.find(wVipLevel);
	return iter != m_TodayRecommendRechargeRandList.end() ? &iter->second : NULL;
}



// ȡ�ȼ��������
const LevelShopData_Config* CConfigManager::GetLevelShopData(WORD wRoleLevel)
{
	map<DWORD, LevelShopData_Config>::iterator iter = m_LevelShopDataList.find(wRoleLevel);
	return iter != m_LevelShopDataList.end() ? &iter->second : NULL;
}

// ȡ�ȼ��������
const LevelGift_Config* CConfigManager::GetLevelGiftData(WORD wRoleLevel)
{
	map<DWORD, LevelGift_Config>::iterator iter = m_LevelGiftList.find(wRoleLevel);
	return iter != m_LevelGiftList.end() ? &iter->second : NULL;
}

// ȡ����������
const OpenServerFundReward_Config* CConfigManager::GetOpenServerFundReward(WORD wRewardId)
{
	map<DWORD, OpenServerFundReward_Config>::iterator iter = m_OpenServerFundRewardList.find(wRewardId);
	return iter != m_OpenServerFundRewardList.end() ? &iter->second : NULL;
}

// ȡȫ��������
const AllPeopleWelfare_Config* CConfigManager::GetAllPeopleWelfare(WORD wWelfareId)
{
	map<DWORD, AllPeopleWelfare_Config>::iterator iter = m_AllPeopleWelfareList.find(wWelfareId);
	return iter != m_AllPeopleWelfareList.end() ? &iter->second : NULL;
}

// ȡ��ʱ�Ż���Ʒ����
const LimitPreferentialShop_Config* CConfigManager::GetRoleLimitPreferentialShop(WORD wRoleLevel)
{
	map<DWORD, LimitPreferentialShop_Config>::iterator iter = m_LimitPreferentialShop.find(wRoleLevel);
	return iter != m_LimitPreferentialShop.end() ? &iter->second : NULL;
}

// ��ʱ�Żݳ�ֵ����
const LimitPreferentialRecharge_Config* CConfigManager::GetLimitPreferentialRecharge(WORD wRechargeId)
{
	map<DWORD, LimitPreferentialRecharge_Config>::iterator iter = m_LimitPreferentialRecharge.find(wRechargeId);
	return iter != m_LimitPreferentialRecharge.end() ? &iter->second : NULL;
}

// ��ʱ�Żݸ���
const LimitPreferentialWelfare_Config* CConfigManager::GetLimitPreferentialWelfare(WORD wWelfareId)
{
	map<DWORD, LimitPreferentialWelfare_Config>::iterator iter = m_LimitPreferentialWelfare.find(wWelfareId);
	return iter != m_LimitPreferentialWelfare.end() ? &iter->second : NULL;
}

// ȡ�ȼ��������
const LevelGift_Config* CConfigManager::GetLevelGift(WORD wLevel)
{
	map<DWORD, LevelGift_Config>::iterator iter = m_LevelGiftList.find(wLevel);
	return iter != m_LevelGiftList.end() ? &iter->second : NULL;
}

// ȡ���߽�������
const OnlineReward_Config* CConfigManager::GetOnlineReward(WORD wDays)
{
	map<DWORD, OnlineReward_Config>::iterator iter = m_OnlineRewardList.find(wDays);
	return iter != m_OnlineRewardList.end() ? &iter->second : NULL;
}

// ȡ7���¼����
const SevenDayLogin_Config* CConfigManager::GetSevenDayLogin(WORD wDay)
{
	map<DWORD, SevenDayLogin_Config>::iterator iter = m_SevenDayLoginList.find(wDay);
	return iter != m_SevenDayLoginList.end() ? &iter->second : NULL;
}

// ȡŤ���齱�б�
const vector<NDLottery_Config>* CConfigManager::GetNDLotteryList(DWORD dwDataId)
{
	map<DWORD, vector<NDLottery_Config>>::iterator iter = m_NDLotteryList.find(dwDataId);
	return iter != m_NDLotteryList.end() ? &iter->second : NULL;
}

// ȡŤ���齱�б�
const NDLotteryCost_Config* CConfigManager::GetNDLotteryCost(DWORD dwDataId)
{
	map<DWORD, NDLotteryCost_Config>::iterator iter = m_NDLotteryCostList.find(dwDataId);
	return iter != m_NDLotteryCostList.end() ? &iter->second : NULL;
}

// ȡ��ս����
const PVPBattleReward_Config* CConfigManager::GetPVPBattleReward(WORD wType, WORD wBattleTimes)
{
	DWORD dwFindId = wBattleTimes * 10 + wType;
	map<DWORD, PVPBattleReward_Config>::iterator iter = m_PVPBattleRewardList.find(dwFindId);
	return iter != m_PVPBattleRewardList.end() ? &iter->second : NULL;
}

// ȡ��������
const PVPRankReward_Config* CConfigManager::GetPVPRankReward(WORD wRank)
{
	map<DWORD, PVPRankReward_Config>::iterator iter = m_PVPRankRewardList.find(wRank);
	return iter != m_PVPRankRewardList.end() ? &iter->second : NULL;
}

// PVP�����
const PVPActivityTime_Config* CConfigManager::GetPVPActivityTime(WORD wWeekDay)
{
	map<DWORD, PVPActivityTime_Config>::iterator iter = m_PVPActivityTimeList.find(wWeekDay);
	return iter != m_PVPActivityTimeList.end() ? &iter->second : NULL;
}

// ȡ�����½�����
const AttackCityChapter_Config* CConfigManager::GetAttackCityChapter(WORD wChapter)
{
	map<WORD, AttackCityChapter_Config>::iterator iter = m_AttackCityChapterList.find(wChapter);
	return iter != m_AttackCityChapterList.end() ? &iter->second : NULL;
}

// ȡ����boss��������
const AttackCityBossReward_Config* CConfigManager::GetAttackCityBossReward(WORD wChapter)
{
	map<WORD, AttackCityBossReward_Config>::iterator iter = m_AttackCityBossRewardList.find(wChapter);
	return iter != m_AttackCityBossRewardList.end() ? &iter->second : NULL;
}

// ȡ��������CD
WORD CConfigManager::GetAttackCityWeakCD(DWORD dwHPRemainRate)
{
	dwHPRemainRate *= 100;
	for (size_t i = 0; i < m_AttackCityWeakList.size(); ++i)
	{
		AttackCityWeak_Config &tWeekCfg = m_AttackCityWeakList[i];
		if (dwHPRemainRate >= tWeekCfg.dwHPRemainRateMin && dwHPRemainRate <= tWeekCfg.dwHPRemainRateMax)
			return tWeekCfg.wWeakCD;
	}

	return 0;
}

// ȡ���ǹ�������
const AttackCityEncourage_Config* CConfigManager::GetAttackCityEncourage(WORD wTimes)
{
	map<WORD, AttackCityEncourage_Config>::iterator iter = m_AttackCityEncourageList.find(wTimes);
	return iter != m_AttackCityEncourageList.end() ? &iter->second : NULL;
}

// ȡ���ǻʱ������
const AttackCityActivityTime_Config* CConfigManager::GetAttackCityActivityTime(WORD wWeekDay)
{
	map<WORD, AttackCityActivityTime_Config>::iterator iter = m_AttackCityActivityTimeList.find(wWeekDay);
	return iter != m_AttackCityActivityTimeList.end() ? &iter->second : NULL;
}


// ȡ����������������
const AttackCityRankReward_Config* CConfigManager::GetAttackCityRankReward(WORD wRank)
{
	map<WORD, AttackCityRankReward_Config>::iterator iter = m_AttackCityRankRewardList.find(wRank);
	return iter != m_AttackCityRankRewardList.end() ? &iter->second : NULL;
}

const EndlessLandWave_Config* CConfigManager::GetEndlessLandWave(WORD wWave)
{
	map<WORD, EndlessLandWave_Config>::iterator iter = m_EndlessLandWaveList.find(wWave);
	return iter != m_EndlessLandWaveList.end() ? &iter->second : NULL;
}

// ȡ��ײ�����б�
const vector<EndlessLandCollision_Config>& CConfigManager::GetEndlessLandCollisionList()
{
	return m_EndlessLandCollisionList;
}

// ȡ��ɱ����
const EndlessLandContinueKillReward_Config* CConfigManager::GetEndlessLandKeepKillReward(DWORD dwContinueKillNum)
{
	map<DWORD, EndlessLandContinueKillReward_Config>::iterator iter = m_EndlessLandContinueKillRewardList.find(dwContinueKillNum);
	return iter != m_EndlessLandContinueKillRewardList.end() ? &iter->second : NULL;
}

// ȡ�޾�֮�����н���
const EndlessLandRankReward_Config*  CConfigManager::GetEndlessLandRankReward(WORD wRank)
{
	map<WORD, EndlessLandRankReward_Config>::iterator iter = m_EndlessLandRankRewardList.find(wRank);
	return iter != m_EndlessLandRankRewardList.end() ? &iter->second : NULL;
}

// ��ս��������
const EndlessLandBattleCost_Config* CConfigManager::GetEndlessLandBattleCost(WORD wBattleTimes)
{
	map<WORD, EndlessLandBattleCost_Config>::iterator iter = m_EndlessLandBattleCostList.find(wBattleTimes);
	return iter != m_EndlessLandBattleCostList.end() ? &iter->second : NULL;
}



