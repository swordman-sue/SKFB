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

// 重加载文件
void CConfigManager::Reload()
{
	// 重置配置文件
	Reset();

	// 注册激活码文件
	//RegisterActivationCodeFile();

	// 加载文件
	UINT64 lCurTickCount = g_GetCurrentTime();
	if (!LoadAllFile())
	{
		MAPLOG(ERROR_LEVEL, "加载配置文件失败!");
		return;
	}

	g_MapServer.ReloadConfig();

	MAPLOG(GUI_LEVEL, "重新加载所有文件总耗时[使用%d毫秒]", g_GetCurrentTime() - lCurTickCount);
}

// 重置配置
void CConfigManager::Reset()
{
	UINT64 lCurTickCount = g_GetCurrentTime();

	m_CommonList.clear();						// 通用配置列表
	m_ServerInfoList.clear();					// 服务器信息配置列表

	m_MonsterList.clear();						// 怪物列表
	m_BattleList.clear();						// 战役列表
	m_MapList.clear();							// 关卡列表
	m_MainFBChapterList.clear();				// 主线副本章节列表
	m_HeroToBattleList.clear();					// 英雄上阵数据列表
	m_DailyChapterList.clear();					// 日常副本章节列表
	m_HeroFBChapterList.clear();				// 英雄副本章节列表
	m_EliteFBChapterList.clear();				// 精英副本章节列表
	m_DailyFBDamageReward.clear();
	m_MainFBChapterRewardList.clear();
	m_vLimitDropList.clear();

	m_ItemList.clear();									// 物品配置
	m_RoleLevelList.clear();						// 角色等级配置
	m_RoleProRecoverList.clear();				// 角色属性恢复配置
	m_RoleInitList.clear();							// 角色初始化列表
	m_vTimeEventList.clear();							// 时间事件列表

	// 英雄
	m_HeroList.clear();									// 英雄配置
	m_HeroLevelList.clear();						// 英雄等级配置
	m_HeroProList.clear();							// 英雄属性配置
	m_BreakLevelUpList.clear();			// 英雄突破升级配置列表
	m_HeroComposeList.clear();					// 英雄合成配置列表
	m_HeroTrainList.clear();						// 英雄培养列表
	m_HeroTrainRandProList.clear();			// 英雄培养随机属性列表
	m_HeroTrainCostList.clear();				// 英雄培养消耗列表
	m_NormalSummonList.clear();				// 普通召唤列表
	m_AdvancedSummonList.clear();				// 高级召唤列表
	m_HeroDestinyList.clear();				// 英雄天命配置列表
	m_HeroAwakenList.clear();						// 英雄觉醒配置列表
	m_HeroShopSlotList.clear();				// 英雄商店槽位配置列表
	m_AwakenShopSlotList.clear();				// 觉醒商店槽位配置列表
	m_HeroBreakList.clear();						// 英雄突破配置
	m_HeroPokedexList.clear();
	m_DirectBuyShopSlotList.clear();
	m_CrystalShopSlotList.clear();
	m_HeroEvoluctionList.clear();
	m_HeroQualificationsList.clear();
	m_HeroVipSummonList.clear();



	// 装备
	m_EquipmentList.clear();						// 装备配置列表
	m_EquipmentComposeList.clear();					// 装备合成配置列表
	m_EquipmentEnhanceList.clear();					// 装备强化配置列表
	m_EquipmentRefineList.clear();					// 装备精练配置列表
	m_ShopGoodshList.clear();						// 商店商品列表
	m_EquipmentEnhanceForgeList.clear();			// 装备强化锻造配置列表
	m_EquipmentRefineForgeList.clear();				// 装备精练锻造配置列表
	m_EquipmentSuitList.clear();				
	m_EuipmentTalentList.clear();
	m_EuipmentStarUpList.clear();
	m_EuipmentQualityUpList.clear();
	m_ReinforcementEncourageList.clear();

	// 宝物相关
	m_TreasureList.clear();						// 宝物配置列表
	m_TreasureEnhanceList.clear();		// 宝物强化配置列表
	m_TreasureRefineList.clear();				// 宝物精练配置列表
	m_TreasureComposeList.clear();			// 宝物合成配置列表
	m_TreasureEnhanceForgeList.clear();			// 宝物强化锻造配置列表
	m_TreasureRefineForgeList.clear();				// 宝物精练锻造配置列表
	m_TreasureSmeltList.clear();
	m_TreasureQualityUPCostList.clear();

	// 叛军
	m_RebelExpoitList.clear();					// 叛军功勋配置列表
	m_RebelBattleList.clear();					// 叛军战役配置列表
	m_RebelQualityList.clear();					// 叛军品质配置列表
	m_RoleRebelList.clear();						// 角色叛军配置列表
	m_RebelRankAwardList.clear();				// 叛军排行榜奖励配置列表

	// 回收
	m_RecycleReturnList.clear();				// 回收返还配置列表

	// 夺宝
	m_DuobaoMapList.clear();						// 夺宝关卡配置列表
	m_DuobaoDropList.clear();						// 夺宝掉落配置列表
	m_DuobaoPregrossRewardList.clear();

	// 爬塔
	m_TowerMapList.clear();						// 爬塔普通关卡配置
	m_TowerChapterList.clear();					// 爬塔普通章节配置
	m_TowerEliteMapList.clear();				// 爬塔精英关卡配置
	m_TowerRandProList.clear();				// 爬塔随机属性配置
	m_TowerStarGoodsList.clear();				// 爬塔星数商品配置

	// 领地
	m_LandCityList.clear();							// 领地城池配置列表
	m_LandPatrolEventList.clear();		// 领地巡逻事件配置列表
	m_LandPatrolList.clear();					// 领地巡逻配置列表

	// 精灵
	m_HeartOfWorldList.clear();								// 精灵配置列表
	m_SpiritList.clear();
	m_SpiritEnhanceList.clear();
	m_SpiritPokedexList.clear();
	m_SpiritTrainList.clear();

	// 竞技场
	m_ArenaRobotList.clear();						// 竞技场机器人列表
	m_ArenaRankList.clear();						// 竞技场排行配置列表
	m_ArenaAwardList.clear();						// 竞技场奖励配置列表
	m_ArenaFirstRankRewardList.clear();				// 竞技场首次排行奖励
	m_ArenaShopGoodsList.clear();

	// 成就
	m_AchievementList.clear();					// 成就配置列表

	// 日常任务
	m_DailyMissionList.clear();				// 日常任务配置列表
	m_DailyMissionTypeList.clear();					// 日常任务类型配置列表
	m_RoleLvDailyMissionList.clear();		// 角色等级对应的日常任务

	// 工会
	m_GuildList.clear();								// 工会配置
	m_GuildIconList.clear();									// 工会图标配置
	m_GuildQYList.clear();							// 工会泉涌配置
	m_GuildQSRewardList.clear();				// 工会泉水奖励配置
	m_GuildPrivilegeList.clear();				// 工会权限配置
	m_GuildLimitGoodsRateList.clear();						// 工会随时商品几率列表
	m_GuildSkillList.clear();						// 工会技能配置列表
	m_GuildFBChapterList.clear();				// 工会副本章节配置列表
	m_GuildRewardGoodsIdList.clear();
	m_GuildLimitShopSlotList.clear();
	m_GuildBuildingList.clear();

	// Vip
	m_VipList.clear();									// Vip配置
	m_VipNoticeList.clear();

	// 签到
	m_SignInList.clear();								// 签到配置
	m_LuxurySignInList.clear();
	m_LuxurySignInRewardList.clear();

	// 迎财神
	m_YCSList.clear();									// 迎财神配置

	// 铜雀台
	m_TQTList.clear();						// 铜雀台配置

	// 世界BOSS
	m_WorldBossOpenTimeList.clear();		// 世界BOSS开放时间配置表
	m_WorldBossList.clear();				// 世界BOSS配置列表
	m_WorldBossRankRewardList.clear();		// 世界BOSS排行奖励配置列表
	m_WorldBossHonorRewardList.clear();		// 世界BOSS荣誉奖励配置列表
	m_WorldBossDamageRewardList.clear();	// 世界BOSS伤害奖励配置列表

	// 7天系统
	m_SevenDayTargetList.clear();			// 7天目标数据列表
	m_SevenDayActiviyList.clear();			// 7天活动列表
	m_TheSameTargetList.clear();			// 同类目标列表
	m_SevenDayCompletionRewardList.clear();
	m_vSevenDayShopGoodsList.clear();

	// 仙灵
	m_CardList.clear();					// 仙灵配置
	m_CardLevelUpList.clear();				// 仙灵升级配置  
	m_CardBreakList.clear();				// 仙灵突破配置  
	m_StarSoulShopSlotList.clear();			// 仙灵商店槽位配置列表
	m_CollectionBookList.clear();
	m_LowCardGroupList.clear();
	m_CollectionBookList.clear();
	m_CollectionGroupList.clear();
	m_HighCardGroupList.clear();
	m_CardGroupMapList.clear();
	m_ZhanBuAchievementList.clear();
	m_CardSlotList.clear();

	// 积分赛
	m_ScoreMatchTargetServerList.clear();	// 积分赛目标服务器列表
	m_ScoreMatchRobotList.clear();			// 积分赛机器人列表
	m_ScoreMatchBattleTaskList.clear();		// 积分赛挑战任务列表
	m_ScoreMatchRankRewardList.clear();		// 积分赛排行奖励列表
	m_ScoreMatchActivityTimeList.clear();
	m_ScoreMatchTimesCostList.clear();
	m_ScoreMatchSearchList.clear();

	// 称号
	m_TitleList.clear();

	// 系统开放
	m_SystemOpenList.clear();

	// 奖励
	m_RewardList.clear();

	// 活动
	//m_ActivityList.clear();
	m_LimitActivityList.clear();
	m_ActivityTimeList.clear();

	// VIP礼包
	m_VipDailyGiftList.clear();
	m_VipWeekGiftList.clear();
	m_VipWeekGiftRandList.clear();
	m_TodayRecommendRechargeList.clear();
	m_TodayRecommendRechargeRandList.clear();

	// 等级商店
	m_LevelShopDataList.clear();

	// 等级礼包
	m_LevelGiftList.clear();

	// 开服基金
	m_OpenServerFundRewardList.clear();
	m_AllPeopleWelfareList.clear();

	// 充值
	m_RechargeList.clear();
	m_MonthCardList.clear();
	m_MyCardRechargeList.clear();

	// 限时优惠
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

	// 怪物攻城
	m_AttackCityChapterList.clear();
	m_AttackCityBossRewardList.clear();
	m_AttackCityWeakList.clear();
	m_AttackCityEncourageList.clear();
	m_AttackCityActivityTimeList.clear();
	m_AttackCityRankRewardList.clear();

	// 无尽这地
	m_EndlessLandCollisionList.clear();
	m_EndlessLandWaveList.clear();
	m_EndlessLandContinueKillRewardList.clear();
	m_EndlessLandRankRewardList.clear();
	m_EndlessLandBattleCostList.clear();

	// 爵位
	m_NobilityList.clear();

	// 超值折扣
	m_SuperDiscountBuyRewardList.clear();
	m_SuperDiscountAccumulateRewardList.clear();

	m_NewSuperDiscountBuyRewardList.clear();
	m_NewSuperDiscountAccumulateRewardList.clear();

	// 7天排行活动奖励
	m_SevenDayRankRewardList.clear();

	// 节日活动
	m_FestivalActivityList.clear();
	m_FestivalActivtiyTargetList.clear();
	m_FestivalActivitySameLogicTargetList.clear();

	m_AccurateRecommendGoodsList.clear();

	// 宝石
	m_GemList.clear();
	m_GemSkillList.clear();
	m_GemHoleList.clear();
	m_GemInlayList.clear();

	// 组队副本
	m_TeamDungeonLotteryList.clear();
	m_TeamDungeonList.clear();
	m_TeamDungeonTaskLinkList.clear();
	m_TeamDungeonTaskList.clear();
	m_TeamDungeonBossRewardList.clear();

	// 场景
	m_SceneList.clear();

	m_LoginRewardList.clear();

	m_ClosedTestRebateList.clear();

	m_AccountWhiteList.clear();
	m_IPWhiteList.clear();

	// 王者赛
	m_KingMatchStageList.clear();
	m_KingMatchSeasonTaskList.clear();
	m_KingMatchRankRewardList.clear();
	m_KingMatchSeasonTimeList.clear();

	// 圣诞节
	m_ChristmasBossLevelList.clear();
	m_ChristmasBossList.clear();
	m_ChristmasSnowballList.clear();

	// facebook活动
	m_FacebookFriendInvitList.clear();
	m_FacebookFriendSummonList.clear();
	m_FacebookAccountBindingList.clear();
	m_FacebookChareList.clear();
	m_FacebookCommendList.clear();
	

	// 在线奖励
	m_OnlineRewardList.clear();

	// 7天登录
	m_SevenDayLoginList.clear();

	// 充值回馈
	m_RechargeRewardList.clear();
	m_RechargeExtraRewardList.clear();
	m_AppleIAPList.clear();

	// 累充奖励
	m_TotalRechargeRewardList.clear();

	// 扭蛋抽奖
	m_NDLotteryList.clear();
	m_NDLotteryCostList.clear();

	// 热卖商品
	m_HotSaleGoodsList.clear();

	// 转盘
	m_TurntableList.clear();
	m_TurntableRankList.clear();
	m_TurntableLotteryList.clear();

	// 限时产出
	m_LimitProductList.clear();

	// 招募
	m_CampRecruitList.clear();
	m_CampRecruitHeroList.clear();

	// unlock活动相关
	m_UnlockActivityRewardList.clear();
	m_UnlockWathingVideoRewardList.clear();

	// 世界等级任务
	m_WorldTaskLevelList.clear();
	m_WorldLevelTaskIdList.clear();
	m_WorldLevelTaskIdListEx.clear();
	m_WorldLevelTaskEventList.clear();

	// 资源战
	m_ResourceWarChapterList.clear();
	m_ResourceWarResourceList.clear();
	m_ResourceWarTechnologyList.clear();
	m_ResourceWarGuildProfitList.clear();

	// 工会战
	m_GuildWarStarList.clear();
	m_GuildWarTowerList.clear();
	m_GuildWarRankRewardList.clear();
	m_GuildWarVSRewardList.clear();
	m_GuildWarVSActivityTimeList.clear();
	m_GuildWarCommonCfgList.clear();
	m_GuildWarOrganizeDefenseList.clear();
	m_GuildWarRegionList.clear();

	// YGame支付
	m_YGamePaymentList.clear();

	m_RechargeRankRewardList.clear();

	m_vHangUpDropList.clear();

	MAPLOG(GUI_LEVEL, "配置文件清除消耗[使用%d毫秒]", g_GetCurrentTime() - lCurTickCount);
}

bool CConfigManager::RegisterCallbackFunc(const LoadConfigCallbackData &tCallbackData)
{
	if (!tCallbackData.func)
	{
		MAPLOG(ERROR_LEVEL, "注册无效的配置文件加载回调函数!");
		return false;
	}

	if (!tCallbackData.file.size())
	{
		MAPLOG(ERROR_LEVEL, "注册无效的配置文件名称!");
		return false;
	}

	// 检测是否重复注册
	if (m_CallbackFuncFindList.find(tCallbackData.file) != m_CallbackFuncFindList.end())
	{
		MAPLOG(ERROR_LEVEL, "重复注册配置文件[%s]!", tCallbackData.file.c_str());
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

// 注册激活码文件
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

// 加载所有配置文件 
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
			MAPLOG(ERROR_LEVEL, "[%s]打开文件失败!", pCallbackData->file.c_str());
			continue;
		}

		// 如果缓存不足，则需在重新申请
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
				MAPLOG(ERROR_LEVEL, "[%s]文件配置出错,找不到配置的字符('};')!", pCallbackData->file.c_str());
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

// 加载积分赛商店商品
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
			MAPLOG(ERROR_LEVEL, "[%s]叛军商店消耗货币配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]叛军商店消耗道具配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.wGoodsId);
			return false;
		}

		AddShopGoods(SHOP_SCORE_MATCH, sConfig);
	}

	return true;
}

// 加载积分赛挑战目标服务器
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

// 加载积分赛机器人
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


// 加载积分赛挑战任务
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
			MAPLOG(ERROR_LEVEL, "[%s]积分赛奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		g_pCfgMgr->m_ScoreMatchBattleTaskList.insert(make_pair(sConfig.wTaskId, sConfig));
	}
	return true;
}

// 加载积分赛排行奖励
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
			MAPLOG(ERROR_LEVEL, "[%s]积分赛排行榜奖励物品配置不匹配!", __FUNCTION__);
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

// 加载积分赛次数消耗配置
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

// 加载积分赛查找目标配置
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

// 加载装备套装配置
bool LoadEquipmentSuit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<Property> vProList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentSuit_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwSuitId = ReadUInt(tmp.data(), "id =");
		ReadArry1(tmp.data(), "suit_list", sConfig.vEquipmentList);

		// 两件
		if (!ReadCommonPropertyList(tmp.data(), "pro1_type_list =", "pro1_value_list =", vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备套装1属性配置不匹配!", __FUNCTION__);
			return false;
		}
		sConfig.vProList.insert(make_pair(2, vProList));

		// 三件
		if (!ReadCommonPropertyList(tmp.data(), "pro2_type_list =", "pro2_value_list =", vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备套装2属性配置不匹配!", __FUNCTION__);
			return false;
		}
		sConfig.vProList.insert(make_pair(3, vProList));

		// 四件
		if (!ReadCommonPropertyList(tmp.data(), "pro3_type_list =", "pro3_value_list =", vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备套装3属性配置不匹配!", __FUNCTION__);
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

// 加载称号配置
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
			MAPLOG(ERROR_LEVEL, "[%s]称号获得的属性配置不匹配!", __FUNCTION__);
			return false;
		}
		g_pCfgMgr->m_TitleList.insert(make_pair(sConfig.nTitleId, sConfig));
	}
	return true;
}

// 加载奖励配置
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
			MAPLOG(ERROR_LEVEL, "奖励中心配置物品奖励不匹配[info_id:%d]!", sConfig.nId);
			return false;
		}
		g_pCfgMgr->m_RewardList.insert(make_pair(sConfig.nId, sConfig));
	}
	return true;
}

// 加载属性恢复配置
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


// 加载角色初始配置
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
			MAPLOG(ERROR_LEVEL, "[%s]角色初始配置物品不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]角色初始配置属性不匹配!", __FUNCTION__);
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
			// 配置出错检测
			for (vector<Activity_Config>::iterator it = iter->second.begin(); it != iter->second.end(); ++it)
			{
				if (it->wDataId == sConfig.wDataId)
				{
					MAPLOG(ERROR_LEVEL, "[%s]活动数据ID出现重复情况:%d!", __FUNCTION__, sConfig.wDataId);
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
			MAPLOG(ERROR_LEVEL, "[%s]限时活动配置物品不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_LimitActivityList.insert(make_pair(sConfig.wDataId, sConfig));
	}

	return true;
}

// 加载星魂商店商品数据
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
			MAPLOG(ERROR_LEVEL, "[%s]星魂商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_STAR_SOUL, sConfig);
	}


	return true;
}

// 加载星魂商店槽位数据
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
				MAPLOG(ERROR_LEVEL, "[%s]找不到星魂商店商品[GoodsId:%d]!", __FUNCTION__, goods_list[k]);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_StarSoulShopSlotList.push_back(sConfig);
	}

	return true;
}


// 加载卡牌配置
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
			MAPLOG(ERROR_LEVEL, "[%s]卡牌基础属性配置不匹配!", __FUNCTION__);
			continue;
		}
		if (!ReadCommonPropertyList(tmp.data(), "level_up_pro_type_list =", "level_up_pro_value_list =", sConfig.vLevelProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]卡牌升级属性配置不匹配!", __FUNCTION__);
			continue;
		}
		if (!ReadCommonPropertyList(tmp.data(), "break_pro_type_list =", "break_pro_value_list =", sConfig.vBreakProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]卡牌突破属性配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_CardList.insert(make_pair(sConfig.dwInfoId, sConfig));
	}

	return true;
}

// 加载卡牌升级配置
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

// 加载卡牌突破配置
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
			MAPLOG(ERROR_LEVEL, "[%s]仙灵突破消耗的物品配置不匹配!", __FUNCTION__);
			continue;
		}
		if (!LoadCommonCost(tmp.data(), sConfig.vCostMoneyList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]仙灵突破消耗的货币配置不匹配!", __FUNCTION__);
			continue;
		}
		DWORD dwFindId = 10000 * sConfig.byQuality + sConfig.wBreakLevel;
		CConfigManager::Instance()->m_CardBreakList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载低级卡组配置
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

// 加载高级卡组配置
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

// 加载卡组关卡配置
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

// 加载收藏书配置
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

// 加载收藏组配置
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
			MAPLOG(ERROR_LEVEL, "[%s]卡牌激活属性配置不匹配!", __FUNCTION__);
			continue;
		}
		if (!ReadCommonPropertyList(tmp.data(), "activate_pro_type_list =", "upgrade_pro_value_list =", tConfig.vUpgradeProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]卡牌升级属性配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_CollectionGroupList.insert(make_pair(tConfig.wGroupId, tConfig));
	}

	return true;
}

// 加载占卜成就配置
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
			MAPLOG(ERROR_LEVEL, "[%s]卡牌激活属性配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_ZhanBuAchievementList.insert(make_pair(tConfig.wLevel, tConfig));
	}

	return true;
}

// 加载卡槽配置
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



// 加载命运商店
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
			MAPLOG(ERROR_LEVEL, "[%s]仙灵商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_DESTINY, sConfig);
	}

	return true;
}

// 加载隐藏商店
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
			MAPLOG(ERROR_LEVEL, "[%s]隐藏商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]隐藏商店消耗的物品配置不匹配!", __FUNCTION__);
			continue;
		}
		AddShopGoods(SHOP_HIDE, sConfig);
	}

	return true;
}

// 加载7天商店商品
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
			MAPLOG(ERROR_LEVEL, "[%s]7天商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}

		AddShopGoods(SHOP_SEVEN_DAY, sConfig);

		CConfigManager::Instance()->m_vSevenDayShopGoodsList.push_back(sConfig.wGoodsId);
	}

	return true;
}

// 加载等级礼包商店商品
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
			MAPLOG(ERROR_LEVEL, "[%s]等级礼包商店消耗配置不匹配!", __FUNCTION__);
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
			MAPLOG(ERROR_LEVEL, "[%s]等级礼包奖励配置出错[Level:%d]!", __FUNCTION__, sConfig.wLevel);
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
			MAPLOG(ERROR_LEVEL, "[%s]7天目标完成度奖励配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_SevenDayCompletionRewardList.insert(make_pair(sConfig.wCompletionNum, sConfig));
	}
	return true;
}

// VIP每日礼包
bool LoadVipDailyGift(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		VipDailyGift_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wVipLevel = ReadUInt(tmp.data(), "level =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]VIP每日礼包配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_VipDailyGiftList.insert(make_pair(sConfig.wVipLevel, sConfig));
	}
	return true;
}

// VIP每周礼包
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
			MAPLOG(ERROR_LEVEL, "[%s]VIP每周礼包物品配置不匹配!", __FUNCTION__);
			continue;
		}
		if (!LoadCommonCost(tmp.data(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]VIP每周礼包购买消耗配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_VipWeekGiftList.insert(make_pair(sConfig.wGiftId, sConfig));
	}
	return true;
}

// 角色VIP每周礼包列表
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
			MAPLOG(ERROR_LEVEL, "[%s] 角色VIP每周礼包ID列表与概率配置不匹配[RoleLv:%d]!", __FUNCTION__, i);
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
			MAPLOG(ERROR_LEVEL, "[%s]军团副本关卡奖励配置不匹配!", __FUNCTION__);
			continue;
		}
		ReadArry2(tmp.c_str(), "chapter_reward_list =", chapter_reward_list);
		for (size_t k = 0; k < chapter_reward_list.size(); ++k)
		{
			if (chapter_reward_list[k].size() != 2)
			{
				MAPLOG(ERROR_LEVEL, "[%s]军团副本章节奖励的物品ID与数量配置不匹配!", __FUNCTION__);
				continue;
			}
			sConfig.vChapterRewardList.push_back(Item(chapter_reward_list[k][0], chapter_reward_list[k][1]));
		}
		CConfigManager::Instance()->m_GuildFBChapterList.insert(make_pair(sConfig.wChapter, sConfig));
	}

	return true;
}

// 加载7天目标数据
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
			MAPLOG(ERROR_LEVEL, "[%s]7天目标奖励配置不匹配!", __FUNCTION__);
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

// 加载7天活动配置
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
				MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据[targetid:%d]!", __FUNCTION__, dwTargetId);
			}
		}
	}

	return true;
}

// 加载世界BOSS开放时间配置
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

// 加载世界BOSS配置
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

// 加载世界BOSS排行奖励配置
bool LoadWorldBossRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldBossRankReward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRank = ReadUInt(tmp.data(), "rank =");
		if (!ReadCommonItemList(tmp.data(), "nonor_reward_id_list =", "nonor_reward_num_list =", sConfig.vHonorRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]世界BOSS排行荣誉奖励物品配置不匹配[Rank:%d]!", __FUNCTION__, sConfig.wRank);
			return false;
		}

		if (!ReadCommonItemList(tmp.data(), "damage_reward_id_list =", "damage_reward_num_list =", sConfig.vDamageRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]世界BOSS排行荣伤害励物品配置不匹配[Rank:%d]!", __FUNCTION__, sConfig.wRank);
			return false;
		}
		CConfigManager::Instance()->m_WorldBossRankRewardList.insert(make_pair(sConfig.wRank, sConfig));
	}
	return true;
}

// 加载世界BOSS荣誉奖励配置
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

// 加载世界BOSS击杀奖励配置
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

// 加载觉醒道具配置
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
			MAPLOG(ERROR_LEVEL, "[%s]觉醒道具合成物品配置不匹配[ItemId:%d]!", __FUNCTION__, sConfig.dwId);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "pro_type_list", "pro_value_list", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]觉醒道具附加属性配置不匹配[ItemId:%d]!", __FUNCTION__, sConfig.dwId);
			return false;
		}
		CConfigManager::Instance()->m_ItemList.insert(make_pair(sConfig.dwId, sConfig));
	}
	return true;
}

// 加载觉醒商店槽位数据
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
				MAPLOG(ERROR_LEVEL, "[%s]找不到觉醒商店商品[GoodsId:%d]!", __FUNCTION__, dwFindId);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_AwakenShopSlotList.push_back(sConfig);
	}

	return true;
}

// 加载觉醒商店槽位数据
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
				MAPLOG(ERROR_LEVEL, "[%s]找不到直购商店商品[GoodsId:%d]!", __FUNCTION__, dwFindId);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_DirectBuyShopSlotList.push_back(sConfig);
	}

	return true;
}

// 加载英雄觉醒配置
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
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗的货币配置不匹配[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		sConfig.nCostMoneyType = cost_money[0];
		sConfig.nCostMoneyValue = cost_money[1];
		ReadArry1(tmp.c_str(), "cost_item", cost_item);
		if (cost_item.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗的物品配置不匹配[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		sConfig.tCostItem = Item(cost_item[0], cost_item[1]);
		ReadArry1(tmp.c_str(), "cost_item_list", sConfig.vCostItemList);
		if (!ReadCommonPropertyList(tmp.c_str(), ",pro_type_list", ",pro_value_list", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒添加的属性配置不匹配[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "extra_pro_type_list", "extra_pro_value_list", extra_pro_list))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒添加的额外属性配置不匹配[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}
		for (size_t k = 0; k < extra_pro_list.size(); ++k)
		{
			sConfig.vProList.push_back(extra_pro_list[k]);
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "item_pro_type_list", "item_pro_value_list", sConfig.vItemProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒添加的材料属性配置不匹配[Quality:%d]!", __FUNCTION__, sConfig.byInitQuality);
			return false;
		}

		DWORD dwFindId = sConfig.byInitQuality * 1000000 + sConfig.byProfession * 10000 + sConfig.byStar * 100 + sConfig.byLevel;
		CConfigManager::Instance()->m_HeroAwakenList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载英雄突破配置
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
			MAPLOG(ERROR_LEVEL, "[%s]英雄突破配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "team_pro_type_list", "team_pro_value_list", sConfig.vTeamProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄突破配置不匹配!", __FUNCTION__);
			return false;
		}
		DWORD dwFindId = sConfig.dwHeroId * 1000 + sConfig.wBreakLevel;
		CConfigManager::Instance()->m_HeroBreakList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载英雄进化配置
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
			MAPLOG(ERROR_LEVEL, "[%s]英雄进化属性配置不匹配[Quality:%d,Stage:%d]!",
				__FUNCTION__, sConfig.wQuality, sConfig.wStage);
			return false;
		}
		if (!ReadCommonItemList(tmp.c_str(), "cost_item_id_list", "cost_item_num_list", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄进化消耗道具不匹配[Quality:%d,Stage:%d]!",
				__FUNCTION__, sConfig.wQuality, sConfig.wStage);
			return false;
		}
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostCurrencyList,  "cost_currency_type_list", "cost_currency_value_list"))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄进化消耗货币不匹配[Quality:%d,Stage:%d]!",
				__FUNCTION__, sConfig.wQuality, sConfig.wStage);
			return false;
		}
		DWORD dwKey = sConfig.wQuality * 10000 + sConfig.wStage;
		CConfigManager::Instance()->m_HeroEvoluctionList.insert(make_pair(dwKey, sConfig));
	}

	return true;
}

// 加载英雄资质
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


// 加载英雄图鉴配置
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



// 加载觉醒商店
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
			MAPLOG(ERROR_LEVEL, "[%s]觉醒商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_AWAKEN, sConfig);
	}

	return true;
}

// 加载工会技能
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
			MAPLOG(ERROR_LEVEL, "[%s]工会升级消耗列表配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "add_pro_type", "add_pro_value", sConfig.vAddProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会升级消耗列表配置不匹配!", __FUNCTION__);
			return false;
		}
		DWORD dwFindId = 1000 * sConfig.wSkillId + sConfig.wLevel;
		CConfigManager::Instance()->m_GuildSkillList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}


// 加载开服基金奖励配置
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

// 加载全民福利配置
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
			MAPLOG(ERROR_LEVEL, "[%s]全民福利物品列表配置不匹配[WelfareId:%d]!", __FUNCTION__, sConfig.wWelfareId);
			continue;
		}
		CConfigManager::Instance()->m_AllPeopleWelfareList.insert(make_pair(sConfig.wWelfareId, sConfig));
	}
	return true;
}

// 加载充值配置
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
// 			MAPLOG(ERROR_LEVEL, "[%s]充值列表配置不匹配[RechargeId:%d]!", __FUNCTION__, sConfig.wId);
// 			continue;
// 		}
		CConfigManager::Instance()->m_RechargeList.insert(make_pair(sConfig.wId, sConfig));

		//MAPLOG(ERROR_LEVEL, "[%s]充值数据[RechargeId:%d,USD:%d]!", __FUNCTION__, sConfig.wId, sConfig.nUSD);
	}
	return true;
}

// 加载mycard充值配置
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

		//MAPLOG(ERROR_LEVEL, "[%s]充值数据[RechargeId:%d,USD:%d]!", __FUNCTION__, sConfig.wId, sConfig.nUSD);
	}
	return true;
}

// 加载月卡配置
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

		//MAPLOG(ERROR_LEVEL, "[%s]充值数据[RechargeId:%d,USD:%d]!", __FUNCTION__, sConfig.wCardId, sConfig.nSellPrice_USD);
	}
	return true;
}


// 加载限时优惠商品
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

// 加载限时优惠商店
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

// 加载限时优惠充值
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

// 加载限时优惠福利
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
			MAPLOG(ERROR_LEVEL, "[%s]限时优惠福利配置不匹配[RechargeId:%d]!", __FUNCTION__, sConfig.wWelfareId);
			continue;
		}
		CConfigManager::Instance()->m_LimitPreferentialWelfare.insert(make_pair(sConfig.wWelfareId, sConfig));
	}
	return true;
}

// 加载工会道具商店
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
			MAPLOG(ERROR_LEVEL, "[%s]工会道具商品消耗配置不匹配!", __FUNCTION__);
			return false;
		}

		AddShopGoods(SHOP_GUILD_ITEM, sConfig);
	}

	return true;
}

// 加载工会限时商店
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
		sConfig.nContext = ReadUInt(tmp.data(), "refresh_rate ="); // todo::可能已经失效,不是很确定
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "refresh_rate =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会限时商品消耗配置不匹配!", __FUNCTION__);
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

// 加载工会奖励商店
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
			MAPLOG(ERROR_LEVEL, "[%s]工会道具奖励消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_GUILD_AWARD, sConfig);

		CConfigManager::Instance()->m_GuildRewardGoodsIdList.push_back(sConfig.wGoodsId);
	}

	return true;
}

// 加载工会限时商店槽位数据
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
				MAPLOG(ERROR_LEVEL, "[%s]找不到工会限时商店商品[GoodsKey(type+id):%d]!", __FUNCTION__, dwKey);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_GuildLimitShopSlotList.push_back(sConfig);
	}

	return true;
}



