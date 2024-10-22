#include "StdAfx.h"
#include "ConfigManager.h"
#include "..\ToolKit\DBCFileManager.h"
#include "..\ToolKit\DBCFile.h"
#include "FileEx\TsFile.h"
#include "CommonLogic.h"
#include "ToolKit\IniFile.h"
#include "FileEx\TsIniFile.h"
#include "MapServer.h"



void CConfigManager::Release()
{

}

// �ؼ����ļ�
void CConfigManager::Reload()
{
	// ���������ļ�
	Reset();

	// ע�ἤ�����ļ�
	//RegisterActivationCodeFile();

	// �����ļ�
	UINT64 lCurTickCount = g_GetCurrentTime();
	if (!LoadAllFile())
	{
		MAPLOG(ERROR_LEVEL, "���������ļ�ʧ��!");
		return;
	}

	g_MapServer.ReloadConfig();

	MAPLOG(GUI_LEVEL, "���¼��������ļ��ܺ�ʱ[ʹ��%d����]", g_GetCurrentTime() - lCurTickCount);
}

// ��������
void CConfigManager::Reset()
{
	UINT64 lCurTickCount = g_GetCurrentTime();

	m_CommonList.clear();						// ͨ�������б�
	m_ServerInfoList.clear();					// ��������Ϣ�����б�

	m_MonsterList.clear();						// �����б�
	m_BattleList.clear();						// ս���б�
	m_MapList.clear();							// �ؿ��б�
	m_MainFBChapterList.clear();				// ���߸����½��б�
	m_HeroToBattleList.clear();					// Ӣ�����������б�
	m_DailyChapterList.clear();					// �ճ������½��б�
	m_HeroFBChapterList.clear();				// Ӣ�۸����½��б�
	m_EliteFBChapterList.clear();				// ��Ӣ�����½��б�
	m_DailyFBDamageReward.clear();
	m_MainFBChapterRewardList.clear();
	m_vLimitDropList.clear();

	m_ItemList.clear();									// ��Ʒ����
	m_RoleLevelList.clear();						// ��ɫ�ȼ�����
	m_RoleProRecoverList.clear();				// ��ɫ���Իָ�����
	m_RoleInitList.clear();							// ��ɫ��ʼ���б�
	m_vTimeEventList.clear();							// ʱ���¼��б�

	// Ӣ��
	m_HeroList.clear();									// Ӣ������
	m_HeroLevelList.clear();						// Ӣ�۵ȼ�����
	m_HeroProList.clear();							// Ӣ����������
	m_BreakLevelUpList.clear();			// Ӣ��ͻ�����������б�
	m_HeroComposeList.clear();					// Ӣ�ۺϳ������б�
	m_HeroTrainList.clear();						// Ӣ�������б�
	m_HeroTrainRandProList.clear();			// Ӣ��������������б�
	m_HeroTrainCostList.clear();				// Ӣ�����������б�
	m_NormalSummonList.clear();				// ��ͨ�ٻ��б�
	m_AdvancedSummonList.clear();				// �߼��ٻ��б�
	m_HeroDestinyList.clear();				// Ӣ�����������б�
	m_HeroAwakenList.clear();						// Ӣ�۾��������б�
	m_HeroShopSlotList.clear();				// Ӣ���̵��λ�����б�
	m_AwakenShopSlotList.clear();				// �����̵��λ�����б�
	m_HeroBreakList.clear();						// Ӣ��ͻ������
	m_HeroPokedexList.clear();
	m_DirectBuyShopSlotList.clear();
	m_CrystalShopSlotList.clear();
	m_HeroEvoluctionList.clear();
	m_HeroQualificationsList.clear();
	m_HeroVipSummonList.clear();



	// װ��
	m_EquipmentList.clear();						// װ�������б�
	m_EquipmentComposeList.clear();					// װ���ϳ������б�
	m_EquipmentEnhanceList.clear();					// װ��ǿ�������б�
	m_EquipmentRefineList.clear();					// װ�����������б�
	m_ShopGoodshList.clear();						// �̵���Ʒ�б�
	m_EquipmentEnhanceForgeList.clear();			// װ��ǿ�����������б�
	m_EquipmentRefineForgeList.clear();				// װ���������������б�
	m_EquipmentSuitList.clear();				
	m_EuipmentTalentList.clear();
	m_EuipmentStarUpList.clear();
	m_EuipmentQualityUpList.clear();
	m_ReinforcementEncourageList.clear();

	// �������
	m_TreasureList.clear();						// ���������б�
	m_TreasureEnhanceList.clear();		// ����ǿ�������б�
	m_TreasureRefineList.clear();				// ���ﾫ�������б�
	m_TreasureComposeList.clear();			// ����ϳ������б�
	m_TreasureEnhanceForgeList.clear();			// ����ǿ�����������б�
	m_TreasureRefineForgeList.clear();				// ���ﾫ�����������б�
	m_TreasureSmeltList.clear();
	m_TreasureQualityUPCostList.clear();

	// �Ѿ�
	m_RebelExpoitList.clear();					// �Ѿ���ѫ�����б�
	m_RebelBattleList.clear();					// �Ѿ�ս�������б�
	m_RebelQualityList.clear();					// �Ѿ�Ʒ�������б�
	m_RoleRebelList.clear();						// ��ɫ�Ѿ������б�
	m_RebelRankAwardList.clear();				// �Ѿ����а��������б�

	// ����
	m_RecycleReturnList.clear();				// ���շ��������б�

	// �ᱦ
	m_DuobaoMapList.clear();						// �ᱦ�ؿ������б�
	m_DuobaoDropList.clear();						// �ᱦ���������б�
	m_DuobaoPregrossRewardList.clear();

	// ����
	m_TowerMapList.clear();						// ������ͨ�ؿ�����
	m_TowerChapterList.clear();					// ������ͨ�½�����
	m_TowerEliteMapList.clear();				// ������Ӣ�ؿ�����
	m_TowerRandProList.clear();				// ���������������
	m_TowerStarGoodsList.clear();				// ����������Ʒ����

	// ���
	m_LandCityList.clear();							// ��سǳ������б�
	m_LandPatrolEventList.clear();		// ���Ѳ���¼������б�
	m_LandPatrolList.clear();					// ���Ѳ�������б�

	// ����
	m_HeartOfWorldList.clear();								// ���������б�
	m_SpiritList.clear();
	m_SpiritEnhanceList.clear();
	m_SpiritPokedexList.clear();
	m_SpiritTrainList.clear();

	// ������
	m_ArenaRobotList.clear();						// �������������б�
	m_ArenaRankList.clear();						// ���������������б�
	m_ArenaAwardList.clear();						// ���������������б�
	m_ArenaFirstRankRewardList.clear();				// �������״����н���
	m_ArenaShopGoodsList.clear();

	// �ɾ�
	m_AchievementList.clear();					// �ɾ������б�

	// �ճ�����
	m_DailyMissionList.clear();				// �ճ����������б�
	m_DailyMissionTypeList.clear();					// �ճ��������������б�
	m_RoleLvDailyMissionList.clear();		// ��ɫ�ȼ���Ӧ���ճ�����

	// ����
	m_GuildList.clear();								// ��������
	m_GuildIconList.clear();									// ����ͼ������
	m_GuildQYList.clear();							// ����Ȫӿ����
	m_GuildQSRewardList.clear();				// ����Ȫˮ��������
	m_GuildPrivilegeList.clear();				// ����Ȩ������
	m_GuildLimitGoodsRateList.clear();						// ������ʱ��Ʒ�����б�
	m_GuildSkillList.clear();						// ���Ἴ�������б�
	m_GuildFBChapterList.clear();				// ���ḱ���½������б�
	m_GuildRewardGoodsIdList.clear();
	m_GuildLimitShopSlotList.clear();
	m_GuildBuildingList.clear();

	// Vip
	m_VipList.clear();									// Vip����
	m_VipNoticeList.clear();

	// ǩ��
	m_SignInList.clear();								// ǩ������
	m_LuxurySignInList.clear();
	m_LuxurySignInRewardList.clear();

	// ӭ����
	m_YCSList.clear();									// ӭ��������

	// ͭȸ̨
	m_TQTList.clear();						// ͭȸ̨����

	// ����BOSS
	m_WorldBossOpenTimeList.clear();		// ����BOSS����ʱ�����ñ�
	m_WorldBossList.clear();				// ����BOSS�����б�
	m_WorldBossRankRewardList.clear();		// ����BOSS���н��������б�
	m_WorldBossHonorRewardList.clear();		// ����BOSS�������������б�
	m_WorldBossDamageRewardList.clear();	// ����BOSS�˺����������б�

	// 7��ϵͳ
	m_SevenDayTargetList.clear();			// 7��Ŀ�������б�
	m_SevenDayActiviyList.clear();			// 7���б�
	m_TheSameTargetList.clear();			// ͬ��Ŀ���б�
	m_SevenDayCompletionRewardList.clear();
	m_vSevenDayShopGoodsList.clear();

	// ����
	m_CardList.clear();					// ��������
	m_CardLevelUpList.clear();				// ������������  
	m_CardBreakList.clear();				// ����ͻ������  
	m_StarSoulShopSlotList.clear();			// �����̵��λ�����б�
	m_CollectionBookList.clear();
	m_LowCardGroupList.clear();
	m_CollectionBookList.clear();
	m_CollectionGroupList.clear();
	m_HighCardGroupList.clear();
	m_CardGroupMapList.clear();
	m_ZhanBuAchievementList.clear();
	m_CardSlotList.clear();

	// ������
	m_ScoreMatchTargetServerList.clear();	// ������Ŀ��������б�
	m_ScoreMatchRobotList.clear();			// �������������б�
	m_ScoreMatchBattleTaskList.clear();		// ��������ս�����б�
	m_ScoreMatchRankRewardList.clear();		// ���������н����б�
	m_ScoreMatchActivityTimeList.clear();
	m_ScoreMatchTimesCostList.clear();
	m_ScoreMatchSearchList.clear();

	// �ƺ�
	m_TitleList.clear();

	// ϵͳ����
	m_SystemOpenList.clear();

	// ����
	m_RewardList.clear();

	// �
	//m_ActivityList.clear();
	m_LimitActivityList.clear();
	m_ActivityTimeList.clear();

	// VIP���
	m_VipDailyGiftList.clear();
	m_VipWeekGiftList.clear();
	m_VipWeekGiftRandList.clear();
	m_TodayRecommendRechargeList.clear();
	m_TodayRecommendRechargeRandList.clear();

	// �ȼ��̵�
	m_LevelShopDataList.clear();

	// �ȼ����
	m_LevelGiftList.clear();

	// ��������
	m_OpenServerFundRewardList.clear();
	m_AllPeopleWelfareList.clear();

	// ��ֵ
	m_RechargeList.clear();
	m_MonthCardList.clear();
	m_MyCardRechargeList.clear();

	// ��ʱ�Ż�
	m_LimitPreferentialShop.clear();
	m_LimitPreferentialRecharge.clear();
	m_LimitPreferentialWelfare.clear();

	m_ActivationCodeList.clear();
	m_ReyunStatisticList.clear();
	m_ActivationCodeRewardList.clear();

	// PVP
	m_PVPBattleRewardList.clear();
	m_PVPRankRewardList.clear();
	m_PVPActivityTimeList.clear();

	// ���﹥��
	m_AttackCityChapterList.clear();
	m_AttackCityBossRewardList.clear();
	m_AttackCityWeakList.clear();
	m_AttackCityEncourageList.clear();
	m_AttackCityActivityTimeList.clear();
	m_AttackCityRankRewardList.clear();

	// �޾����
	m_EndlessLandCollisionList.clear();
	m_EndlessLandWaveList.clear();
	m_EndlessLandContinueKillRewardList.clear();
	m_EndlessLandRankRewardList.clear();
	m_EndlessLandBattleCostList.clear();

	// ��λ
	m_NobilityList.clear();

	// ��ֵ�ۿ�
	m_SuperDiscountBuyRewardList.clear();
	m_SuperDiscountAccumulateRewardList.clear();

	m_NewSuperDiscountBuyRewardList.clear();
	m_NewSuperDiscountAccumulateRewardList.clear();

	// 7�����л����
	m_SevenDayRankRewardList.clear();

	// ���ջ
	m_FestivalActivityList.clear();
	m_FestivalActivtiyTargetList.clear();
	m_FestivalActivitySameLogicTargetList.clear();

	m_AccurateRecommendGoodsList.clear();

	// ��ʯ
	m_GemList.clear();
	m_GemSkillList.clear();
	m_GemHoleList.clear();
	m_GemInlayList.clear();

	// ��Ӹ���
	m_TeamDungeonLotteryList.clear();
	m_TeamDungeonList.clear();
	m_TeamDungeonTaskLinkList.clear();
	m_TeamDungeonTaskList.clear();
	m_TeamDungeonBossRewardList.clear();

	// ����
	m_SceneList.clear();

	m_LoginRewardList.clear();

	m_ClosedTestRebateList.clear();

	m_AccountWhiteList.clear();
	m_IPWhiteList.clear();

	// ������
	m_KingMatchStageList.clear();
	m_KingMatchSeasonTaskList.clear();
	m_KingMatchRankRewardList.clear();
	m_KingMatchSeasonTimeList.clear();

	// ʥ����
	m_ChristmasBossLevelList.clear();
	m_ChristmasBossList.clear();
	m_ChristmasSnowballList.clear();

	// facebook�
	m_FacebookFriendInvitList.clear();
	m_FacebookFriendSummonList.clear();
	m_FacebookAccountBindingList.clear();
	m_FacebookChareList.clear();
	m_FacebookCommendList.clear();
	

	// ���߽���
	m_OnlineRewardList.clear();

	// 7���¼
	m_SevenDayLoginList.clear();

	// ��ֵ����
	m_RechargeRewardList.clear();
	m_RechargeExtraRewardList.clear();
	m_AppleIAPList.clear();

	// �۳佱��
	m_TotalRechargeRewardList.clear();

	// Ť���齱
	m_NDLotteryList.clear();
	m_NDLotteryCostList.clear();

	// ������Ʒ
	m_HotSaleGoodsList.clear();

	// ת��
	m_TurntableList.clear();
	m_TurntableRankList.clear();
	m_TurntableLotteryList.clear();

	// ��ʱ����
	m_LimitProductList.clear();

	// ��ļ
	m_CampRecruitList.clear();
	m_CampRecruitHeroList.clear();

	// unlock����
	m_UnlockActivityRewardList.clear();
	m_UnlockWathingVideoRewardList.clear();

	// ����ȼ�����
	m_WorldTaskLevelList.clear();
	m_WorldLevelTaskIdList.clear();
	m_WorldLevelTaskIdListEx.clear();
	m_WorldLevelTaskEventList.clear();

	// ��Դս
	m_ResourceWarChapterList.clear();
	m_ResourceWarResourceList.clear();
	m_ResourceWarTechnologyList.clear();
	m_ResourceWarGuildProfitList.clear();

	// ����ս
	m_GuildWarStarList.clear();
	m_GuildWarTowerList.clear();
	m_GuildWarRankRewardList.clear();
	m_GuildWarVSRewardList.clear();
	m_GuildWarVSActivityTimeList.clear();
	m_GuildWarCommonCfgList.clear();
	m_GuildWarOrganizeDefenseList.clear();
	m_GuildWarRegionList.clear();

	// YGame֧��
	m_YGamePaymentList.clear();

	m_RechargeRankRewardList.clear();

	m_vHangUpDropList.clear();

	MAPLOG(GUI_LEVEL, "�����ļ��������[ʹ��%d����]", g_GetCurrentTime() - lCurTickCount);
}

bool CConfigManager::RegisterCallbackFunc(const LoadConfigCallbackData &tCallbackData)
{
	if (!tCallbackData.func)
	{
		MAPLOG(ERROR_LEVEL, "ע����Ч�������ļ����ػص�����!");
		return false;
	}

	if (!tCallbackData.file.size())
	{
		MAPLOG(ERROR_LEVEL, "ע����Ч�������ļ�����!");
		return false;
	}

	// ����Ƿ��ظ�ע��
	if (m_CallbackFuncFindList.find(tCallbackData.file) != m_CallbackFuncFindList.end())
	{
		MAPLOG(ERROR_LEVEL, "�ظ�ע�������ļ�[%s]!", tCallbackData.file.c_str());
		return false;
	}

	m_CallbackFuncFindList.insert(make_pair(tCallbackData.file, tCallbackData.file));
	
	m_CallbackDataList.push_back(tCallbackData);
	
	return true;
}

bool CConfigManager::RegisterCallbackFunc(string file, LoadCallbackFunc func)
{
	//if (file.find("DBC/") < 0)
		file = "DBC/" + file;

	//if (file.find(".txt") < 0)
	//	file += ".txt";

	LoadConfigCallbackData CallbackData;
	CallbackData.file = file;
	CallbackData.func = func;
	return RegisterCallbackFunc(CallbackData);
}

// ע�ἤ�����ļ�
void CConfigManager::RegisterActivationCodeFile()
{
	char szKey[64];
	char szFileName[128];
 	IniFile inifile;
 	inifile.Init("DBC\\activation_code_file_list.ini");
	int nFileNum = inifile.GetIntVal("file_list", "file_num", 0);
 	for (int i = 1; i <= nFileNum; ++i)
 	{
 		sprintf(szKey, "file_%d", i);
		inifile.GetStrVal("file_list", szKey, szFileName, sizeof(szFileName)-1);
 		RegisterCallbackFunc(szFileName, LoadActivationCode);
 	}
}

// �������������ļ� 
bool CConfigManager::LoadAllFile()
{
	CFileOperate file_operate;
	std::vector<string> tmp_data_list;
	char *pszBuff = NULL;
	DWORD dwBuffLen = 0;

	std::vector<LoadConfigCallbackData>::iterator iter = m_CallbackDataList.begin();
	for (;iter != m_CallbackDataList.end(); ++iter)
	{
		LoadConfigCallbackData *pCallbackData = &*iter;

		tmp_data_list.clear();
		if (!file_operate.OpenA(pCallbackData->file.c_str()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ļ�ʧ��!", pCallbackData->file.c_str());
			continue;
		}

		// ������治�㣬��������������
		if (file_operate.GetSize() >= dwBuffLen)
		{
			if (pszBuff)
				delete[] pszBuff;

			dwBuffLen = file_operate.GetSize() + 1;
			pszBuff = new char[dwBuffLen];
		}

		memset(pszBuff, 0, dwBuffLen);
		file_operate.Read(pszBuff, dwBuffLen-1);

		char *pStar;
		char *pEnd;
		char *pCur = pszBuff;
		string strData;
		while (true)
		{
			pStar = strstr(pCur, "#${");
			if (!pStar) 
				break;
			pEnd = strstr(pStar, "};");
			if (!pEnd)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�ļ����ó���,�Ҳ������õ��ַ�('};')!", pCallbackData->file.c_str());
				break;
			}
			pStar += 3;
			strData.assign(pStar, pEnd - pStar);
			tmp_data_list.push_back(strData);
			pCur = pEnd;
		}

		if (!pCallbackData->func(this, tmp_data_list))
		{
			MAPLOG(ERROR_LEVEL, "Load config file[%s] error!", pCallbackData->file.c_str());
			continue;
		}
	}

	if (pszBuff)
		delete[] pszBuff;

	return true;
}

// ���ػ������̵���Ʒ
bool LoadScoreMatchShopGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ��̵����Ļ������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ��̵����ĵ������ò�ƥ��[MapId:%d]!", __FUNCTION__, sConfig.wGoodsId);
			return false;
		}

		AddShopGoods(SHOP_SCORE_MATCH, sConfig);
	}

	return true;
}

// ���ػ�������սĿ�������
bool LoadScoreMatchTargetServer(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vTargetServerList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		ScoreMatchServerGroup_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwServerId = ReadUInt(tmp.data(), "id =");
		tConfig.wRankGroupId = ReadUInt(tmp.data(), "rank_group_id =");
		ReadArry1(tmp.data(), "target_server_list =", tConfig.vGroupList);
		g_pCfgMgr->m_ScoreMatchTargetServerList.insert(make_pair(tConfig.dwServerId, tConfig));
	}

	return true;
}

// ���ػ�����������
bool LoadScoreMatchRobot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ScoreMatchRobot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwRobotId = ReadUInt(tmp.data(), "id =");
		sConfig.wServerId = ReadUInt(tmp.data(), "server_id =");
		sConfig.dwFightingPower = ReadUInt(tmp.data(), "fighting_power =");
		g_pCfgMgr->m_ScoreMatchRobotList.push_back(sConfig);
	}
	return true;
}


// ���ػ�������ս����
bool LoadScoreMatchBattleTask(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ScoreMatchBattleTask_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wTaskId = ReadUInt(tmp.data(), "id =");
		sConfig.wBattleTimes = ReadUInt(tmp.data(), "battle_times =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]������������Ʒ���ò�ƥ��!", __FUNCTION__);
			return false;
		}
		g_pCfgMgr->m_ScoreMatchBattleTaskList.insert(make_pair(sConfig.wTaskId, sConfig));
	}
	return true;
}

// ���ػ��������н���
bool LoadScoreMatchRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ScoreMatchRankReward_Config sConfig;
		string &tmp = data_list[i];
		WORD wRankMin = ReadUInt(tmp.data(), "rank_min =");
		WORD wRankMax = ReadUInt(tmp.data(), "rank_max =");
		sConfig.nTitleId = ReadUInt(tmp.data(), "reward_title_id =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���������а�����Ʒ���ò�ƥ��!", __FUNCTION__);
			return false;
		}
		for (WORD k = wRankMin; k <= wRankMax; ++k)
		{
			sConfig.wRank = k;
			g_pCfgMgr->m_ScoreMatchRankRewardList.insert(make_pair(sConfig.wRank, sConfig));
		}
	}
	return true;
}

// ���ػ�����������������
bool LoadScoreMatchTimesCost(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		WORD wType = ReadUInt(tmp.data(), "type =");
		WORD wTimes = ReadUInt(tmp.data(), "times =");
		int nCostDiamond = ReadUInt(tmp.data(), "cost_diamond =");
		DWORD dwKey = wType * 10000 + wTimes;
		g_pCfgMgr->m_ScoreMatchTimesCostList.insert(make_pair(dwKey, nCostDiamond));
	}
	return true;
}

// ���ػ���������Ŀ������
bool LoadScoreMatchSearchTarget(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ScoreMatchSearchTarget_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.wTargetNum = ReadUInt(tmp.data(), "target_num =");
		ReadArry1(tmp.data(), "battle_value_rate_min =", tConfig.vBattleValueRateMinList);
		ReadArry1(tmp.data(), "battle_value_rate_max =", tConfig.vBattleValueRateMaxList);
		CConfigManager::Instance()->m_ScoreMatchSearchList.insert(make_pair(tConfig.wType, tConfig));
	}
	return true;
}

bool LoadScoreMatchActivityTime(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ScoreMathActivityTime_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wIndex = ReadUInt(tmp.data(), "index =");
		tConfig.dwStartDate = ReadUInt(tmp.data(), "start_date =");
		tConfig.dwStartTime = ReadUInt(tmp.data(), "start_time =");
		tConfig.dwOverDate = ReadUInt(tmp.data(), "over_date =");
		tConfig.dwOverTime = ReadUInt(tmp.data(), "over_time =");
		CConfigManager::Instance()->m_ScoreMatchActivityTimeList.insert(make_pair(tConfig.wIndex, tConfig));
	}
	return true;
}

// ����װ����װ����
bool LoadEquipmentSuit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<Property> vProList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentSuit_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwSuitId = ReadUInt(tmp.data(), "id =");
		ReadArry1(tmp.data(), "suit_list", sConfig.vEquipmentList);

		// ����
		if (!ReadCommonPropertyList(tmp.data(), "pro1_type_list =", "pro1_value_list =", vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]װ����װ1�������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		sConfig.vProList.insert(make_pair(2, vProList));

		// ����
		if (!ReadCommonPropertyList(tmp.data(), "pro2_type_list =", "pro2_value_list =", vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]װ����װ2�������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		sConfig.vProList.insert(make_pair(3, vProList));

		// �ļ�
		if (!ReadCommonPropertyList(tmp.data(), "pro3_type_list =", "pro3_value_list =", vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]װ����װ3�������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		sConfig.vProList.insert(make_pair(4, vProList));

		g_pCfgMgr->m_EquipmentSuitList.push_back(sConfig);
	}
	return true;
}

bool LoadSystemOpen(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		DWORD dwSystemId = ReadUInt(tmp.data(), "id =");
		WORD wOpenLevel = ReadUInt(tmp.data(), "open_level =");
		g_pCfgMgr->m_SystemOpenList.insert(make_pair(dwSystemId, wOpenLevel));
	}
	return true;
}

// ���سƺ�����
bool LoadTitle(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Title_Config sConfig;
		string &tmp = data_list[i];
		sConfig.nTitleId = ReadUInt(tmp.data(), "id =");
		sConfig.nType = ReadUInt(tmp.data(), "type =");
		sConfig.nValidTime = ReadUInt(tmp.data(), "valid_time =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ƺŻ�õ��������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		g_pCfgMgr->m_TitleList.insert(make_pair(sConfig.nTitleId, sConfig));
	}
	return true;
}

// ���ؽ�������
bool LoadReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Reward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.nId = ReadUInt(tmp.data(), "reward_id =");
		sConfig.nType = ReadUInt(tmp.data(), "reward_type =");
		if (!ReadCommonItemList(tmp.data(), "award_item_id_list =", "award_item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "��������������Ʒ������ƥ��[info_id:%d]!", sConfig.nId);
			return false;
		}
		g_pCfgMgr->m_RewardList.insert(make_pair(sConfig.nId, sConfig));
	}
	return true;
}

// �������Իָ�����
bool LoadProRecover(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RoleProRecover_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byProType = ReadUInt(tmp.data(), "pro_type =");
		sConfig.nIntervalTime = ReadUInt(tmp.data(), "recover_interval_time =");
		sConfig.nRecoverValue = ReadUInt(tmp.data(), "recover_value =");
		CConfigManager::Instance()->m_RoleProRecoverList.insert(make_pair(sConfig.byProType, sConfig));
	}

	return true;
}


// ���ؽ�ɫ��ʼ����
bool LoadRoleInit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RoleInit_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwDataId = ReadUInt(tmp.data(), "id =");
		sConfig.nHeadImage = ReadUInt(tmp.data(), "head_image =");
		ReadArry1(tmp.data(), "hero_list =", sConfig.vHeroList);
		ReadArry1(tmp.data(), "hero_list =", sConfig.vHeroList);
		if (!ReadCommonItemList(tmp.data(), "object_id_list =", "object_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ɫ��ʼ������Ʒ��ƥ��!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ɫ��ʼ�������Բ�ƥ��!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_RoleInitList.insert(make_pair(sConfig.dwDataId, sConfig));
	}

	return true;
}

bool LoadActivity(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Activity_Config sConfig; 
		string &tmp = data_list[i];
		sConfig.nSystemId = ReadUInt(tmp.data(), "id =");
		sConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		sConfig.nTimeType = ReadUInt(tmp.data(), "time_type =");
		sConfig.nDays = ReadUInt(tmp.data(), "days ="); 
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		ReadArry1(tmp.data(), "server_list =", sConfig.vOpenServerList);
		sConfig.nStartDate = ReadUInt(tmp.data(), "star_date =");
		sConfig.nStartTime = ReadUInt(tmp.data(), "star_time =");
		sConfig.nOverDate = ReadUInt(tmp.data(), "over_date =");
		sConfig.nOverTime = ReadUInt(tmp.data(), "over_time =");
		sConfig.nRewardOverDate = ReadUInt(tmp.data(), "reward_over_date =");
		sConfig.nRewardOverTime = ReadUInt(tmp.data(), "reward_over_time =");

		map<DWORD, vector<Activity_Config>> *pActivityList = &CConfigManager::Instance()->m_ActivityTimeList;
		map<DWORD, vector<Activity_Config>>::iterator iter = pActivityList->find(sConfig.nSystemId);
		if (iter != pActivityList->end())
		{
			// ���ó�����
			for (vector<Activity_Config>::iterator it = iter->second.begin(); it != iter->second.end(); ++it)
			{
				if (it->wDataId == sConfig.wDataId)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�����ID�����ظ����:%d!", __FUNCTION__, sConfig.wDataId);
					continue;
				}
			}

			iter->second.push_back(sConfig);
		}
		else
		{
			vector<Activity_Config> vList;
			vList.push_back(sConfig);
			pActivityList->insert(make_pair(sConfig.nSystemId,vList));
		}
	}

	return true;
}

bool LoadLimitActivity(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LimitActivity_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wDataId = ReadUInt(tmp.data(), "id =");
		sConfig.wType = ReadUInt(tmp.data(), "type =");
		sConfig.dwContext = ReadUInt(tmp.data(), "context =");
		sConfig.wRewardTimesMax = ReadUInt(tmp.data(), "reward_times_max =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ʱ�������Ʒ��ƥ��!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_LimitActivityList.insert(make_pair(sConfig.wDataId, sConfig));
	}

	return true;
}

// �����ǻ��̵���Ʒ����
bool LoadStarSoulShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ǻ��̵��������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_STAR_SOUL, sConfig);
	}


	return true;
}

// �����ǻ��̵��λ����
bool LoadStarSoulShopSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	vector<DWORD> goods_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RefreshShopSlot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.bySlot = ReadUInt(tmp.data(), "slot =");
		ReadArry1(tmp.c_str(), "goods_list", goods_list);
		for (size_t k = 0; k < goods_list.size(); ++k)
		{
			DWORD dwFindId = 10000 * SHOP_STAR_SOUL + goods_list[k];
			map<DWORD, ShopGoodsData_Config>::iterator iter = pShopGoodsList->find(dwFindId);
			if (iter == pShopGoodsList->end())
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ǻ��̵���Ʒ[GoodsId:%d]!", __FUNCTION__, goods_list[k]);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_StarSoulShopSlotList.push_back(sConfig);
	}

	return true;
}


// ���ؿ�������
bool LoadCard(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Card_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwInfoId = ReadUInt(tmp.data(), "id =");
		sConfig.byType = ReadUInt(tmp.data(), "type =");
		sConfig.byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.dwToExp = ReadUInt(tmp.data(), "to_exp =");
		sConfig.byType = ReadUInt(tmp.data(), "type =");
		sConfig.dwDecomposeReturnStarSoul = ReadUInt(tmp.data(), "decompose_return_star_soul =");
		sConfig.nResetCostDiamond = ReadUInt(tmp.data(), "reset_cost_diamond =");
		if (!ReadCommonPropertyList(tmp.data(), "base_pro_type_list =", "base_pro_value_list =", sConfig.vBaseProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ƻ����������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		if (!ReadCommonPropertyList(tmp.data(), "level_up_pro_type_list =", "level_up_pro_value_list =", sConfig.vLevelProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���������������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		if (!ReadCommonPropertyList(tmp.data(), "break_pro_type_list =", "break_pro_value_list =", sConfig.vBreakProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ͻ���������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_CardList.insert(make_pair(sConfig.dwInfoId, sConfig));
	}

	return true;
}

// ���ؿ�����������
bool LoadCardLevelUp(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CardLevelUp_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		sConfig.dwExp = ReadUInt(tmp.data(), "exp =");
		sConfig.dwTotalExp = ReadUInt(tmp.data(), "total_exp =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		DWORD dwFindId = 10000 * sConfig.byQuality + sConfig.wLevel;
		CConfigManager::Instance()->m_CardLevelUpList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// ���ؿ���ͻ������
bool LoadCardBreak(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CardBreak_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wBreakLevel = ReadUInt(tmp.data(), "break_level =");
		sConfig.wNeedCardLevel = ReadUInt(tmp.data(), "need_card_level =");
		sConfig.wCostSameCardNum = ReadUInt(tmp.data(), "cost_same_card_num =");
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ͻ�����ĵ���Ʒ���ò�ƥ��!", __FUNCTION__);
			continue;
		}
		if (!LoadCommonCost(tmp.data(), sConfig.vCostMoneyList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ͻ�����ĵĻ������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		DWORD dwFindId = 10000 * sConfig.byQuality + sConfig.wBreakLevel;
		CConfigManager::Instance()->m_CardBreakList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// ���صͼ���������
bool LoadLowCardGroup(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<RateDataEx>> *pCardGroupList = &CConfigManager::Instance()->m_LowCardGroupList;

	for (int i = 0; i < data_list.size(); ++i)
	{
		RateDataEx tRateData;
		string &tmp = data_list[i];
		DWORD wGruopId = ReadUInt(tmp.data(), "group_id =");
		tRateData.nValue = ReadUInt(tmp.data(), "card_id =");
		tRateData.nRate = ReadUInt(tmp.data(), "weight =");
		map<DWORD, vector<RateDataEx>>::iterator iter =  pCardGroupList->find(wGruopId);
		if (iter != pCardGroupList->end())
		{
			iter->second.push_back(tRateData);
		}
		else
		{
			vector<RateDataEx> vList;
			vList.push_back(tRateData);
			pCardGroupList->insert(make_pair(wGruopId, vList));
		}
	}

	return true;
}

// ���ظ߼���������
bool LoadHighCardGroup(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HighCardGroup_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwCardId = ReadUInt(tmp.data(), "card_id =");
		tConfig.nNormalWeight = ReadUInt(tmp.data(), "normal_weight =");
		tConfig.nMustWeight = ReadUInt(tmp.data(), "must_weight =");
		CConfigManager::Instance()->m_HighCardGroupList.push_back(tConfig);
	}

	return true;
}

// ���ؿ���ؿ�����
bool LoadCardGroupMap(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CardGroupMap_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wMapIdx = ReadUInt(tmp.data(), "map_idx =");
		tConfig.dwMapId = ReadUInt(tmp.data(), "map_id =");
		tConfig.wCardGroupId = ReadUInt(tmp.data(), "card_group_id =");
		tConfig.byIsTheLastMap = ReadUInt(tmp.data(), "is_the_last =");
		CConfigManager::Instance()->m_CardGroupMapList.insert(make_pair(tConfig.wMapIdx, tConfig));
	}

	return true;
}

// �����ղ�������
bool LoadCollectionBook(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CollectionBook_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wBookId = ReadUInt(tmp.data(), "id =");
		tConfig.nNeedCollectionValue = ReadUInt(tmp.data(), "collection_value =");
		ReadArry1(tmp.data(), "collection_group_list =", tConfig.vCollectionGroupList);
		CConfigManager::Instance()->m_CollectionBookList.insert(make_pair(tConfig.wBookId, tConfig));
	}

	return true;
}

// �����ղ�������
bool LoadCollectionGroup(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CollectionGroup_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wGroupId = ReadUInt(tmp.data(), "id =");
		tConfig.nActivateCollectionValue = ReadUInt(tmp.data(), "activate_collection_value =");
		tConfig.nUpgradeCollectionValue = ReadUInt(tmp.data(), "upgrade_collection_value =");
		ReadArry1(tmp.data(), "cost_card_list =", tConfig.vCostCardList);
		if (!ReadCommonPropertyList(tmp.data(), "activate_pro_type_list =", "activate_pro_value_list =", tConfig.vActivateProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���Ƽ����������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		if (!ReadCommonPropertyList(tmp.data(), "activate_pro_type_list =", "upgrade_pro_value_list =", tConfig.vUpgradeProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���������������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_CollectionGroupList.insert(make_pair(tConfig.wGroupId, tConfig));
	}

	return true;
}

// ����ռ���ɾ�����
bool LoadZhanBuAchievement(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ZhanBuAchievement_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wLevel = ReadUInt(tmp.data(), "level =");
		tConfig.dwNeedCollectionValue = ReadUInt(tmp.data(), "need_collection_value =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", tConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���Ƽ����������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_ZhanBuAchievementList.insert(make_pair(tConfig.wLevel, tConfig));
	}

	return true;
}

// ���ؿ�������
bool LoadCardSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CardSlot_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wCardSlot = ReadUInt(tmp.data(), "slot =");
		tConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "role_level =");
		CConfigManager::Instance()->m_CardSlotList.insert(make_pair(tConfig.wCardSlot, tConfig));
	}

	return true;
}



// ���������̵�
bool LoadDestinyShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "object_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "object_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		//sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����̵��������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_DESTINY, sConfig);
	}

	return true;
}

// ���������̵�
bool LoadHideShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����̵��������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����̵����ĵ���Ʒ���ò�ƥ��!", __FUNCTION__);
			continue;
		}
		AddShopGoods(SHOP_HIDE, sConfig);
	}

	return true;
}

// ����7���̵���Ʒ
bool LoadSevenDayShopGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]7���̵��������ò�ƥ��!", __FUNCTION__);
			return false;
		}

		AddShopGoods(SHOP_SEVEN_DAY, sConfig);

		CConfigManager::Instance()->m_vSevenDayShopGoodsList.push_back(sConfig.wGoodsId);
	}

	return true;
}

// ���صȼ�����̵���Ʒ
bool LoadLevelShopGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ȼ�����̵��������ò�ƥ��!", __FUNCTION__);
			return false;
		}

		AddShopGoods(SHOP_LEVEL_GIFT, sConfig);
	}

	return true;
}

bool LoadLevelShopData(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LevelShopData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRoleLevel = ReadUInt(tmp.data(), "level =");
		sConfig.nValidTime = ReadUInt(tmp.data(), "valid_time =") * 60;
		ReadArry1(tmp.data(), "goods_id_list =", sConfig.vGoodsList);
		CConfigManager::Instance()->m_LevelShopDataList.insert(make_pair(sConfig.wRoleLevel, sConfig));
	}

	return true;
}

bool LoadLevelGiftData(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LevelGift_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ȼ�����������ó���[Level:%d]!", __FUNCTION__, sConfig.wLevel);
			continue;
		}
		CConfigManager::Instance()->m_LevelGiftList.insert(make_pair(sConfig.wLevel, sConfig));
	}

	return true;
}



bool LoadSevenDayCompletionReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SevenDayCompletionReward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wCompletionNum = ReadUInt(tmp.data(), "target_num =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]7��Ŀ����ɶȽ������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_SevenDayCompletionRewardList.insert(make_pair(sConfig.wCompletionNum, sConfig));
	}
	return true;
}

// VIPÿ�����
bool LoadVipDailyGift(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		VipDailyGift_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wVipLevel = ReadUInt(tmp.data(), "level =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]VIPÿ��������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_VipDailyGiftList.insert(make_pair(sConfig.wVipLevel, sConfig));
	}
	return true;
}

// VIPÿ�����
bool LoadVipWeekGift(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		VipWeekGift_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGiftId = ReadUInt(tmp.data(), "id =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.wNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]VIPÿ�������Ʒ���ò�ƥ��!", __FUNCTION__);
			continue;
		}
		if (!LoadCommonCost(tmp.data(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]VIPÿ����������������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_VipWeekGiftList.insert(make_pair(sConfig.wGiftId, sConfig));
	}
	return true;
}

// ��ɫVIPÿ������б�
bool LoadVipWeekGiftRandList(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vRateList;
	vector<DWORD> vIdList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		VipWeekGiftRandList_Config sConfig;
		string &tmp = data_list[i];
		int nLevelMin = ReadUInt(tmp.data(), "min_level =");
		int nLevelMax = ReadUInt(tmp.data(), "max_level =");
		sConfig.wGiftRandNum = ReadUInt(tmp.data(), "gift_rand_num =");
		ReadArry1(tmp.data(), "gift_id_list =", vIdList);
		ReadArry1(tmp.data(), "gift_rate_list =", vRateList);
		if (vIdList.size() != vRateList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s] ��ɫVIPÿ�����ID�б���������ò�ƥ��[RoleLv:%d]!", __FUNCTION__, i);
			return false;
		}
		for (size_t k = 0; k < vIdList.size(); ++k)
		{
			RateDataEx tRateData;
			tRateData.nRate = vRateList[k];
			tRateData.nValue = vIdList[k];
			sConfig.vGiftRandList.push_back(tRateData);
		}
		for (int k = nLevelMin; k <= nLevelMax; ++k)
		{
			CConfigManager::Instance()->m_VipWeekGiftRandList.insert(make_pair(k, sConfig));
		}		
	}
	return true;
}

bool LoadSignIn(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SignIn_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wDay = ReadUInt(tmp.data(), "day =");
		sConfig.tRewardItem.dwId = ReadUInt(tmp.data(), "reward_item_id =");
		sConfig.tRewardItem.dwNum = ReadUInt(tmp.data(), "reward_item_num =");
		sConfig.byDoubleRewardVipLv = ReadUInt(tmp.data(), "double_reward_need_vip_level =");
		CConfigManager::Instance()->m_SignInList.insert(make_pair(sConfig.wDay, sConfig));
	}

	return true;
}




bool LoadYCS(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		YCS_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRoleLevel = ReadUInt(tmp.data(), "level =");
		sConfig.byMoneyType = ReadUInt(tmp.data(), "reward_type =");
		ReadArry1(tmp.c_str(), "reward_value_list", sConfig.vMoneyValueList);
		CConfigManager::Instance()->m_YCSList.insert(make_pair(sConfig.wRoleLevel, sConfig));
	}

	return true;
}

bool LoadTQT(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TQT_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wId = ReadUInt(tmp.data(), "id =");
		sConfig.nStartTime = ReadUInt(tmp.data(), "start_time =");
		sConfig.nOverTime = ReadUInt(tmp.data(), "over_time =");
		CConfigManager::Instance()->m_TQTList.push_back(sConfig);
	}

	return true;
}

bool LoadGuildFB(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<vector<DWORD>> chapter_reward_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildFBChapter_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wChapter = ReadUInt(tmp.data(), "id =");
		sConfig.nKillContribution = ReadUInt(tmp.data(), "kill_contribution =");
		sConfig.nBattleContribution = ReadUInt(tmp.data(), "battle_contribution =");
		sConfig.nKillExp = ReadUInt(tmp.data(), "kill_exp =");
		ReadArry1(tmp.data(), "map_list =", sConfig.vMapList);
		if (!ReadCommonItemList(tmp.data(), "map_reward_id_list =", "map_reward_num_list =", sConfig.vMapRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���Ÿ����ؿ��������ò�ƥ��!", __FUNCTION__);
			continue;
		}
		ReadArry2(tmp.c_str(), "chapter_reward_list =", chapter_reward_list);
		for (size_t k = 0; k < chapter_reward_list.size(); ++k)
		{
			if (chapter_reward_list[k].size() != 2)
			{
				MAPLOG(ERROR_LEVEL, "[%s]���Ÿ����½ڽ�������ƷID���������ò�ƥ��!", __FUNCTION__);
				continue;
			}
			sConfig.vChapterRewardList.push_back(Item(chapter_reward_list[k][0], chapter_reward_list[k][1]));
		}
		CConfigManager::Instance()->m_GuildFBChapterList.insert(make_pair(sConfig.wChapter, sConfig));
	}

	return true;
}

// ����7��Ŀ������
bool LoadSevenDayTarget(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<WORD, vector<WORD>> *pTheSameTargetList = &CConfigManager::Instance()->m_TheSameTargetList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		SevenDayTarget_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wTargetId = ReadUInt(tmp.data(), "id =");
		sConfig.wLogicType = ReadUInt(tmp.data(), "logic_type =");
		sConfig.byIsAsc = ReadUInt(tmp.data(), "is_asc =");
		sConfig.nX = ReadUInt(tmp.data(), "x =");
		sConfig.nY = ReadUInt(tmp.data(), "y =");
		sConfig.byIsCompletionTarget = ReadUInt(tmp.data(), "is_completion_target =");
		sConfig.byIsTheSameDayValid = ReadUInt(tmp.data(), "is_the_same_day_valid =");
		sConfig.byIsFromTheSameDayValid = ReadUInt(tmp.data(), "is_from_the_same_day_valid =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]7��Ŀ�꽱�����ò�ƥ��!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_SevenDayTargetList.insert(make_pair(sConfig.wTargetId, sConfig));

		map<WORD, vector<WORD>>::iterator iter = pTheSameTargetList->find(sConfig.wLogicType);
		if (iter != pTheSameTargetList->end())
		{
			iter->second.push_back(sConfig.wTargetId);
		}
		else
		{
			vector<WORD> vIdList;
			vIdList.push_back(sConfig.wTargetId);
			pTheSameTargetList->insert(make_pair(sConfig.wLogicType, vIdList));
		}
	}

	return true;
}

// ����7������
bool LoadSevenDayActivity(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, SevenDayTarget_Config> *pSevenDayTargetList = &CConfigManager::Instance()->m_SevenDayTargetList;

	vector<DWORD> target_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		WORD wDay = ReadUInt(tmp.data(), "day =");
		ReadArry1(tmp.data(), "target_list =", target_list);
		CConfigManager::Instance()->m_SevenDayActiviyList.insert(make_pair(wDay, target_list));

		for (int k = 0; k < target_list.size(); ++k)
		{
			DWORD dwTargetId = target_list[k];
			map<DWORD, SevenDayTarget_Config>::iterator iter = pSevenDayTargetList->find(dwTargetId);
			if (iter != pSevenDayTargetList->end())
			{
				iter->second.wBelongActivityDay = wDay;		
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������[targetid:%d]!", __FUNCTION__, dwTargetId);
			}
		}
	}

	return true;
}

// ��������BOSS����ʱ������
bool LoadWorldBossOpenTime(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldBossOpenTime_Config sConfig;
		string &tmp = data_list[i];
		sConfig.nWeekDay = ReadUInt(tmp.data(), "week_day =");
		sConfig.nStartTime = ReadUInt(tmp.data(), "start_time =");
		sConfig.nOverTime = ReadUInt(tmp.data(), "over_time =");
		sConfig.nRewardTime = ReadUInt(tmp.data(), "reward_time =");
		CConfigManager::Instance()->m_WorldBossOpenTimeList.insert(make_pair(sConfig.nWeekDay, sConfig));
	}
	return true;
}

// ��������BOSS����
bool LoadWorldBoss(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldBoss_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wBossLevel = ReadUInt(tmp.data(), "level =");
		sConfig.wMapId = ReadUInt(tmp.data(), "map_id =");
		sConfig.nLuckRewardRate = ReadUInt(tmp.data(), "luck_reward_rate =");
		sConfig.tLuckRewardItem.dwId = ReadUInt(tmp.data(), "luck_reward_item_id =");
		sConfig.tLuckRewardItem.dwNum = ReadUInt(tmp.data(), "luck_reward_item_num =");
		sConfig.tKillRewardItem.dwId = ReadUInt(tmp.data(), "kill_reward_item_id =");
		sConfig.tKillRewardItem.dwNum = ReadUInt(tmp.data(), "kill_reward_item_num =");
		CConfigManager::Instance()->m_WorldBossList.insert(make_pair(sConfig.wBossLevel, sConfig));
	}
	return true;
}

// ��������BOSS���н�������
bool LoadWorldBossRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldBossRankReward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRank = ReadUInt(tmp.data(), "rank =");
		if (!ReadCommonItemList(tmp.data(), "nonor_reward_id_list =", "nonor_reward_num_list =", sConfig.vHonorRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����BOSS��������������Ʒ���ò�ƥ��[Rank:%d]!", __FUNCTION__, sConfig.wRank);
			return false;
		}

		if (!ReadCommonItemList(tmp.data(), "damage_reward_id_list =", "damage_reward_num_list =", sConfig.vDamageRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����BOSS�������˺�����Ʒ���ò�ƥ��[Rank:%d]!", __FUNCTION__, sConfig.wRank);
			return false;
		}
		CConfigManager::Instance()->m_WorldBossRankRewardList.insert(make_pair(sConfig.wRank, sConfig));
	}
	return true;
}

// ��������BOSS������������
bool LoadWorldBossHonorReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldBossHonorReward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRewardId = ReadUInt(tmp.data(), "id =");
		sConfig.nNeedHonor = ReadUInt(tmp.data(), "need_honor =");
		sConfig.tRewardItem.dwId = ReadUInt(tmp.data(), "reward_item_id =");
		sConfig.tRewardItem.dwNum = ReadUInt(tmp.data(), "reward_item_num =");
		CConfigManager::Instance()->m_WorldBossHonorRewardList.insert(make_pair(sConfig.wRewardId, sConfig));
	}
	return true;
}

// ��������BOSS��ɱ��������
bool LoadWorldBossDamageReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldBossKillReward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRewardId = ReadUInt(tmp.data(), "id =");
		sConfig.nNeedBossLv = ReadUInt(tmp.data(), "need_boss_lv =");
		sConfig.tRewardItem.dwId = ReadUInt(tmp.data(), "reward_item_id =");
		sConfig.tRewardItem.dwNum = ReadUInt(tmp.data(), "reward_item_num =");
		CConfigManager::Instance()->m_WorldBossDamageRewardList.insert(make_pair(sConfig.wRewardId, sConfig));
	}
	return true;
}

// ���ؾ��ѵ�������
bool LoadAwakeningItem(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Item_Config sConfig;
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.dwId = ReadUInt(tmp.data(), "id =");
		sConfig.byType = OBJECT_ITEM;
		sConfig.bySubType = 111;
		sConfig.nDecomposeGodSoul = ReadUInt(tmp.data(), "god_soul =");
		sConfig.byIsUseInGet = false;
		if (!ReadCommonItemList(tmp.c_str(), "compose_item_id_list", "compose_item_num_list", sConfig.vComposeCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ѵ��ߺϳ���Ʒ���ò�ƥ��[ItemId:%d]!", __FUNCTION__, sConfig.dwId);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "pro_type_list", "pro_value_list", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ѵ��߸����������ò�ƥ��[ItemId:%d]!", __FUNCTION__, sConfig.dwId);
			return false;
		}
		CConfigManager::Instance()->m_ItemList.insert(make_pair(sConfig.dwId, sConfig));
	}
	return true;
}

// ���ؾ����̵��λ����
bool LoadAwakenShopSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	vector<DWORD> goods_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RefreshShopSlot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.bySlot = ReadUInt(tmp.data(), "slot =");
		//sConfig.byIsCanRepeat = ReadUInt(tmp.data(), "is_can_repeat =");
		ReadArry1(tmp.c_str(), "goods_list", goods_list);
		for (size_t k = 0; k < goods_list.size(); ++k)
		{
			DWORD dwFindId = 10000 * SHOP_AWAKEN + goods_list[k];
			map<DWORD, ShopGoodsData_Config>::iterator iter = pShopGoodsList->find(dwFindId);
			if (iter == pShopGoodsList->end())
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������̵���Ʒ[GoodsId:%d]!", __FUNCTION__, dwFindId);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_AwakenShopSlotList.push_back(sConfig);
	}

	return true;
}

// ���ؾ����̵��λ����
bool LoadDirectBuyShopSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	vector<DWORD> goods_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RefreshShopSlot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.bySlot = ReadUInt(tmp.data(), "slot =");
		//sConfig.byIsCanRepeat = ReadUInt(tmp.data(), "is_can_repeat =");
		ReadArry1(tmp.c_str(), "goods_list", goods_list);
		for (size_t k = 0; k < goods_list.size(); ++k)
		{
			DWORD dwFindId = MAKE_GOODS_FIND_ID(SHOP_DIRECT_BUY, goods_list[k]);
			map<DWORD, ShopGoodsData_Config>::iterator iter = pShopGoodsList->find(dwFindId);
			if (iter == pShopGoodsList->end())
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ���̵���Ʒ[GoodsId:%d]!", __FUNCTION__, dwFindId);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_DirectBuyShopSlotList.push_back(sConfig);
	}

	return true;
}

// ����Ӣ�۾�������
bool LoadHeroAwaken(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> cost_money;
	vector<DWORD> cost_item;
	vector<DWORD> cost_item_list;
	vector<Property> extra_pro_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroAwaken_Config sConfig;
		string &tmp = data_list[i];

		int nId = ReadUInt(tmp.data(), "id =");
		sConfig.byInitQuality = nId * 0.1;	//ReadUInt(tmp.data(), "init_quality =");
		sConfig.byProfession = nId % 10;	//ReadUInt(tmp.data(), "profession =");
		sConfig.byStar = ReadUInt(tmp.data(), "star =");
		sConfig.byLevel = ReadUInt(tmp.data(), "level =");
		sConfig.wNeedHeroLevel = ReadUInt(tmp.data(), "need_hero_level =");
		sConfig.wCostHeroNum = ReadUInt(tmp.data(), "cost_hero_num =");
		ReadArry1(tmp.c_str(), "cost_money", cost_money);
		if (cost_money.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾������ĵĻ������ò�ƥ��[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		sConfig.nCostMoneyType = cost_money[0];
		sConfig.nCostMoneyValue = cost_money[1];
		ReadArry1(tmp.c_str(), "cost_item", cost_item);
		if (cost_item.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾������ĵ���Ʒ���ò�ƥ��[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		sConfig.tCostItem = Item(cost_item[0], cost_item[1]);
		ReadArry1(tmp.c_str(), "cost_item_list", sConfig.vCostItemList);
		if (!ReadCommonPropertyList(tmp.c_str(), ",pro_type_list", ",pro_value_list", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾�����ӵ��������ò�ƥ��[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "extra_pro_type_list", "extra_pro_value_list", extra_pro_list))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾�����ӵĶ����������ò�ƥ��[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		for (size_t k = 0; k < extra_pro_list.size(); ++k)
		{
			sConfig.vProList.push_back(extra_pro_list[k]);
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "item_pro_type_list", "item_pro_value_list", sConfig.vItemProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾�����ӵĲ����������ò�ƥ��[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}

		DWORD dwFindId = sConfig.byInitQuality * 1000000 + sConfig.byProfession * 10000 + sConfig.byStar * 100 + sConfig.byLevel;
		CConfigManager::Instance()->m_HeroAwakenList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// ����Ӣ��ͻ������
bool LoadHeroBreak(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroBreak_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwHeroId = ReadUInt(tmp.data(), "id =");
		sConfig.wBreakLevel = ReadUInt(tmp.data(), "level =");
		if (!ReadCommonPropertyList(tmp.c_str(), "self_pro_type_list", "to_self_pro_value_list", sConfig.vSelfProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ò�ƥ��!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "team_pro_type_list", "team_pro_value_list", sConfig.vTeamProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ò�ƥ��!", __FUNCTION__);
			return false;
		}
		DWORD dwFindId = sConfig.dwHeroId * 1000 + sConfig.wBreakLevel;
		CConfigManager::Instance()->m_HeroBreakList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// ����Ӣ�۽�������
bool LoadHeroEvolution(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroEvolution_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wStage = ReadUInt(tmp.data(), "stage =");
		sConfig.isTheLastStage = ReadUInt(tmp.data(), "is_the_last_stage =");
		sConfig.nCostGold = ReadUInt(tmp.data(), "cost_gold =");
		sConfig.wAddQualifications = ReadUInt(tmp.data(), "add_qualifications =");
		sConfig.wNeedBreakLevel = ReadUInt(tmp.data(), "need_level =");
		sConfig.wNeedEnhanceLevel = ReadUInt(tmp.data(), "need_break_level =");
		if (!ReadCommonPropertyList(tmp.c_str(), "pro_type_list", "pro_value_list", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۽����������ò�ƥ��[Quality:%d,Stage:%d]!",
				__FUNCTION__, sConfig.wQuality, sConfig.wStage);
			return false;
		}
		if (!ReadCommonItemList(tmp.c_str(), "cost_item_id_list", "cost_item_num_list", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۽������ĵ��߲�ƥ��[Quality:%d,Stage:%d]!",
				__FUNCTION__, sConfig.wQuality, sConfig.wStage);
			return false;
		}
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostCurrencyList,  "cost_currency_type_list", "cost_currency_value_list"))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۽������Ļ��Ҳ�ƥ��[Quality:%d,Stage:%d]!",
				__FUNCTION__, sConfig.wQuality, sConfig.wStage);
			return false;
		}
		DWORD dwKey = sConfig.wQuality * 10000 + sConfig.wStage;
		CConfigManager::Instance()->m_HeroEvoluctionList.insert(make_pair(dwKey, sConfig));
	}

	return true;
}

// ����Ӣ������
bool LoadHeroQualifications(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		DWORD dwQualifications = ReadUInt(tmp.data(), "qualifications =");
		int nAddRate = ReadUInt(tmp.data(), "add_rate =");
		CConfigManager::Instance()->m_HeroQualificationsList.insert(make_pair(dwQualifications, nAddRate));
	}

	return true;
}


// ����Ӣ��ͼ������
bool LoadHeroPokedex(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<DWORD>> *pPokedexList = &CConfigManager::Instance()->m_HeroPokedexList;
	vector<DWORD> vHeroList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		//HeroPokedex_Config tConfig;
		string &tmp = data_list[i];
		WORD wQuality = ReadUInt(tmp.data(), "quality =");
		ReadArry1(tmp.data(), "hero_list =", vHeroList);
		map<DWORD, vector<DWORD>>::iterator iter = pPokedexList->find(wQuality);
		if (iter != pPokedexList->end())
		{
			iter->second.insert(iter->second.end(), vHeroList.begin(), vHeroList.end());
		}
		else
		{
			pPokedexList->insert(make_pair(wQuality, vHeroList));
		}
	}

	return true;
}



// ���ؾ����̵�
bool LoadAwakenShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����̵��������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_AWAKEN, sConfig);
	}

	return true;
}

// ���ع��Ἴ��
bool LoadGuildSkill(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildSkill_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wSkillId = ReadUInt(tmp.data(), "id =");
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		sConfig.wNeedGuildLevel = ReadUInt(tmp.data(), "need_guild_level =");
		sConfig.nCostGuildExp = ReadUInt(tmp.data(), "cost_guild_exp =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�������������б����ò�ƥ��!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "add_pro_type", "add_pro_value", sConfig.vAddProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�������������б����ò�ƥ��!", __FUNCTION__);
			return false;
		}
		DWORD dwFindId = 1000 * sConfig.wSkillId + sConfig.wLevel;
		CConfigManager::Instance()->m_GuildSkillList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}


// ���ؿ�������������
bool LoadOpenServerFundReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		OpenServerFundReward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRewardId = ReadUInt(tmp.data(), "id =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nDiamond = ReadUInt(tmp.data(), "diamond =");
		CConfigManager::Instance()->m_OpenServerFundRewardList.insert(make_pair(sConfig.wRewardId, sConfig));
	}
	return true;
}

// ����ȫ��������
bool LoadAllThePeopleWelfare(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AllPeopleWelfare_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wWelfareId = ReadUInt(tmp.data(), "id =");
		sConfig.wNeedBuyNum = ReadUInt(tmp.data(), "need_buy_num =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȫ������Ʒ�б����ò�ƥ��[WelfareId:%d]!", __FUNCTION__, sConfig.wWelfareId);
			continue;
		}
		CConfigManager::Instance()->m_AllPeopleWelfareList.insert(make_pair(sConfig.wWelfareId, sConfig));
	}
	return true;
}

// ���س�ֵ����
bool LoadRecharge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Recharge_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wId = ReadUInt(tmp.data(), "id =");
		sConfig.nRMB = ReadUInt(tmp.data(), "money =");
		double fMoneyUSD = 100 * ReadDouble(tmp.data(), "money_USD =");
		sConfig.nUSD = round(fMoneyUSD);
		sConfig.nFirstRechargeDiamond = ReadUInt(tmp.data(), "first_recharge_diamond =");
		sConfig.nYCoin = ReadUInt(tmp.data(), "ycoin =");
		sConfig.nRechargeDiamond = ReadUInt(tmp.data(), "recharge_diamond =");
		sConfig.nExtraDiamond = ReadUInt(tmp.data(), "extra_diamond =");
		sConfig.nVipExp = ReadUInt(tmp.data(), "vip_exp =");
		sConfig.strName = ReadString(tmp.data(), "name =");
		sConfig.strRechargeStrId = ReadString(tmp.data(), "recharge_str_id = ");
// 		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vRewardItemList))
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]��ֵ�б����ò�ƥ��[RechargeId:%d]!", __FUNCTION__, sConfig.wId);
// 			continue;
// 		}
		CConfigManager::Instance()->m_RechargeList.insert(make_pair(sConfig.wId, sConfig));

		//MAPLOG(ERROR_LEVEL, "[%s]��ֵ����[RechargeId:%d,USD:%d]!", __FUNCTION__, sConfig.wId, sConfig.nUSD);
	}
	return true;
}

// ����mycard��ֵ����
bool LoadMycardRecharge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Recharge_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wId = ReadUInt(tmp.data(), "id =");
		sConfig.nRMB = ReadUInt(tmp.data(), "money =");
		double fMoneyUSD = 100 * ReadDouble(tmp.data(), "money_USD =");
		sConfig.nUSD = round(fMoneyUSD);
		sConfig.nFirstRechargeDiamond = ReadUInt(tmp.data(), "first_recharge_diamond =");
		sConfig.nRechargeDiamond = ReadUInt(tmp.data(), "recharge_diamond =");
		sConfig.nExtraDiamond = ReadUInt(tmp.data(), "extra_diamond =");
		sConfig.nVipExp = ReadUInt(tmp.data(), "vip_exp =");
		sConfig.strName = ReadString(tmp.data(), "name =");
		sConfig.strRechargeStrId = ReadString(tmp.data(), "recharge_str_id = ");
		CConfigManager::Instance()->m_MyCardRechargeList.insert(make_pair(sConfig.wId, sConfig));

		//MAPLOG(ERROR_LEVEL, "[%s]��ֵ����[RechargeId:%d,USD:%d]!", __FUNCTION__, sConfig.wId, sConfig.nUSD);
	}
	return true;
}

// �����¿�����
bool LoadMonthCard(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		MonthCard_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wCardId = ReadUInt(tmp.data(), "id =");
		sConfig.nSellPrice = ReadUInt(tmp.data(), "sell_price =");
		double fMoneyUSD = 100 * ReadDouble(tmp.data(), "sell_price_USD =");
		sConfig.nSellPrice_USD = round(fMoneyUSD);
		sConfig.nVipExp = ReadUInt(tmp.data(), "vip_exp =");
		sConfig.nBuyRewardDiamond = ReadUInt(tmp.data(), "buy_reward_diamond =");
		sConfig.nDailyRewardDiamond = ReadUInt(tmp.data(), "daily_reward_diamond =");
		sConfig.tRewardItem.dwId = ReadUInt(tmp.data(), "reward_item_id =");
		sConfig.tRewardItem.dwNum = ReadUInt(tmp.data(), "reward_item_num =");
		sConfig.nValidTime = ReadUInt(tmp.data(), "valid_time =");
		sConfig.strName = ReadString(tmp.data(), "name =");
		sConfig.strRechargeStrId = ReadString(tmp.data(), "recharge_str_id = ");
		CConfigManager::Instance()->m_MonthCardList.insert(make_pair(sConfig.wCardId, sConfig));

		//MAPLOG(ERROR_LEVEL, "[%s]��ֵ����[RechargeId:%d,USD:%d]!", __FUNCTION__, sConfig.wCardId, sConfig.nSellPrice_USD);
	}
	return true;
}


// ������ʱ�Ż���Ʒ
bool LoadLimitPreferentialGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.nContext = ReadUInt(tmp.data(), "schedule =");
		sConfig.wNeedRoleLevel = 0;
		CostData tCost;
		tCost.wType = ReadUInt(tmp.data(), "cost_type =");
		tCost.nValue = ReadUInt(tmp.data(), "cost_value =");
		sConfig.vCostList.push_back(tCost);
		AddShopGoods(SHOP_LIMIT_PREFERENTIAL_SHOP, sConfig);
	}
	return true;
}

// ������ʱ�Ż��̵�
bool LoadLimitPreferentialShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LimitPreferentialShop_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRoleLevelMin = ReadUInt(tmp.data(), "level_min =");
		sConfig.wRoleLevelMax = ReadUInt(tmp.data(), "level_max =");
		sConfig.wRandGoodsNum = ReadUInt(tmp.data(), "rand_goods_num =");
		ReadArry1(tmp.c_str(), "goods_list", sConfig.vGoodsList);
		for (WORD k = sConfig.wRoleLevelMin; k <= sConfig.wRoleLevelMax; ++k)
		{
			CConfigManager::Instance()->m_LimitPreferentialShop.insert(make_pair(k, sConfig));
		}
	}
	return true;
}

// ������ʱ�Żݳ�ֵ
bool LoadLimitPreferentialRecharge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LimitPreferentialRecharge_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRechargeId = ReadUInt(tmp.data(), "recharge_id =");
		sConfig.nRate = ReadUInt(tmp.data(), "rate =");
		sConfig.nRewardDiamond = ReadUInt(tmp.data(), "reward_diamond =");
		sConfig.nValidTime = ReadUInt(tmp.data(), "valid_time =") * 60;
		CConfigManager::Instance()->m_LimitPreferentialRecharge.insert(make_pair(sConfig.wRechargeId, sConfig));
	}
	return true;
}

// ������ʱ�Żݸ���
bool LoadLimitPreferentialWelfare(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LimitPreferentialWelfare_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wWelfareId = ReadUInt(tmp.data(), "id =");
		sConfig.wNeedBuyTimes = ReadUInt(tmp.data(), "need_buy_times =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ʱ�Żݸ������ò�ƥ��[RechargeId:%d]!", __FUNCTION__, sConfig.wWelfareId);
			continue;
		}
		CConfigManager::Instance()->m_LimitPreferentialWelfare.insert(make_pair(sConfig.wWelfareId, sConfig));
	}
	return true;
}

// ���ع�������̵�
bool LoadGuildItemShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "entity_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "entity_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wNeedGuildLevel = ReadUInt(tmp.data(), "need_guild_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���������Ʒ�������ò�ƥ��!", __FUNCTION__);
			return false;
		}

		AddShopGoods(SHOP_GUILD_ITEM, sConfig);
	}

	return true;
}

// ���ع�����ʱ�̵�
bool LoadGuildLimitShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "entity_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "entity_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wNeedGuildLevel = ReadUInt(tmp.data(), "need_guild_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "refresh_rate ="); // todo::�����Ѿ�ʧЧ,���Ǻ�ȷ��
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "refresh_rate =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ʱ��Ʒ�������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_GUILD_LIMIT, sConfig);

		RateDataEx tRateData;
		tRateData.nRate = sConfig.nContext;
		tRateData.nValue = sConfig.wGoodsId;
		CConfigManager::Instance()->m_GuildLimitGoodsRateList.push_back(tRateData);
	}

	return true;
}

// ���ع��ά���̵�
bool LoadGuildRewardShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "entity_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "entity_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wNeedGuildLevel = ReadUInt(tmp.data(), "need_guild_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]������߽����������ò�ƥ��!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_GUILD_AWARD, sConfig);

		CConfigManager::Instance()->m_GuildRewardGoodsIdList.push_back(sConfig.wGoodsId);
	}

	return true;
}

// ���ع�����ʱ�̵��λ����
bool LoadGuildLimitShopSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	vector<DWORD> goods_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RefreshShopSlot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.bySlot = ReadUInt(tmp.data(), "slot =");
		ReadArry1(tmp.c_str(), "goods_list", goods_list);
		for (size_t k = 0; k < goods_list.size(); ++k)
		{
			DWORD dwKey = MAKE_GOODS_FIND_ID(SHOP_GUILD_LIMIT, goods_list[k]);
			map<DWORD, ShopGoodsData_Config>::iterator iter = pShopGoodsList->find(dwKey);
			if (iter == pShopGoodsList->end())
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������ʱ�̵���Ʒ[GoodsKey(type+id):%d]!", __FUNCTION__, dwKey);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_GuildLimitShopSlotList.push_back(sConfig);
	}

	return true;
}



