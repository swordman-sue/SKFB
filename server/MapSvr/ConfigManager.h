/**\file
    配置文件管理器
   \author Kevin
   \date 2014/1/12
   \note 
*/
#pragma once
#include <hash_map>
#include <vector>
#include "..\ToolKit\Singleton.h"
#include "..\Common\DBCDefine.h"

class CConfigManager;

typedef bool(*LoadCallbackFunc)(CConfigManager *pConfigMgr, std::vector<string> &data);

// 加载配置文件回调函数数据
struct LoadConfigCallbackData
{
	string file;			// 配置文件
	LoadCallbackFunc func;	// 回调函数
};

#define LOAD_CONFIG_FUNC(func_name) bool func_name(CConfigManager *pConfigMgr, std::vector<string> &data_list); 

// 读取Item
bool ReadItem(const char *pszSrc, const char *pszKey, Item &tItem);

// 读取uint
DWORD ReadUInt(const char *pszSrc, const char *pszKey, int nDefault = 0);

// 读取int
int ReadInt(const char *pszSrc, const char *pszKey, int nDefault = 0);

// 读取double
double ReadDouble(const char *pszSrc, const char *pszKey, double fDefault = 0);

// 读取配对数据
bool ReadPair(const char *pszSrc, const char *pszKey, DWORD &dwValue1, DWORD &dwValue2);

// 读取字符串
string ReadString(const char *pszSrc, const char *pszKey, const char *pszDefault = "");

// 读取一维数组
bool ReadArry1(const char *pszSrc, const char *pszKey, vector<DWORD> &list);

// 读取二维数组
bool ReadArry2(const char *pszSrc, const char *pszKey, vector<vector<DWORD>> &list);

// 加载通用消耗配置逻辑
bool LoadCommonCost(const char *pszSrc, vector<CostData> &cost_list, const char *pszTypeKey = "cost_type_list", const char *pszValueKey = "cost_value_list");

// 读取通用物品列表
bool ReadCommonItemList(const char *pszSrc, const char *pszIdKey, const char *pszNumKey, vector<Item> &item_list);

// 读取通用属性列表
bool ReadCommonPropertyList(const char *pszSrc, const char *pszTypeKey, const char *pszValueKey, vector<Property> &sOutProList);

// 加载通用配置
bool LoadCommon(CConfigManager *pConfigMgr, std::vector<string> &data_list);

// 添加商店商品
bool AddShopGoods(BYTE byShopType, const ShopGoodsData_Config &tGoods);


LOAD_CONFIG_FUNC(LoadScoreMatchShopGoods);
LOAD_CONFIG_FUNC(LoadScoreMatchTargetServer);
LOAD_CONFIG_FUNC(LoadScoreMatchRobot);
LOAD_CONFIG_FUNC(LoadScoreMatchBattleTask);
LOAD_CONFIG_FUNC(LoadScoreMatchRankReward);
LOAD_CONFIG_FUNC(LoadScoreMatchActivityTime);
LOAD_CONFIG_FUNC(LoadScoreMatchSearchTarget);
LOAD_CONFIG_FUNC(LoadScoreMatchTimesCost);
LOAD_CONFIG_FUNC(LoadEquipmentSuit);
LOAD_CONFIG_FUNC(LoadSystemOpen);
LOAD_CONFIG_FUNC(LoadTitle);
LOAD_CONFIG_FUNC(LoadReward);
LOAD_CONFIG_FUNC(LoadProRecover);
LOAD_CONFIG_FUNC(LoadRoleInit);
LOAD_CONFIG_FUNC(LoadActivity);
LOAD_CONFIG_FUNC(LoadLimitActivity);
LOAD_CONFIG_FUNC(LoadStarSoulShop);
LOAD_CONFIG_FUNC(LoadStarSoulShopSlot);
LOAD_CONFIG_FUNC(LoadDestinyShop);
LOAD_CONFIG_FUNC(LoadFairyMapList);
//LOAD_CONFIG_FUNC(LoadFairyHighRecruitList);
//LOAD_CONFIG_FUNC(LoadFairyNormalRecruitList);
//LOAD_CONFIG_FUNC(LoadFairyPokedexAchievement);
LOAD_CONFIG_FUNC(LoadFairyPokedex);
LOAD_CONFIG_FUNC(LoadFairyRecruit);
LOAD_CONFIG_FUNC(LoadCardLevelUp);
LOAD_CONFIG_FUNC(LoadCard);
LOAD_CONFIG_FUNC(LoadCardBreak);
LOAD_CONFIG_FUNC(LoadSevenDayShopGoods);
LOAD_CONFIG_FUNC(LoadSevenDayCompletionReward);
LOAD_CONFIG_FUNC(LoadVipDailyGift);
LOAD_CONFIG_FUNC(LoadVipWeekGift);
LOAD_CONFIG_FUNC(LoadVipWeekGiftRandList);
LOAD_CONFIG_FUNC(LoadSignIn);
LOAD_CONFIG_FUNC(LoadYCS);
LOAD_CONFIG_FUNC(LoadTQT);
LOAD_CONFIG_FUNC(LoadGuildFB);
LOAD_CONFIG_FUNC(LoadSevenDayTarget);
LOAD_CONFIG_FUNC(LoadSevenDayActivity);
LOAD_CONFIG_FUNC(LoadLevelShopGoods);
LOAD_CONFIG_FUNC(LoadLevelShopData);
LOAD_CONFIG_FUNC(LoadWorldBossOpenTime);
LOAD_CONFIG_FUNC(LoadWorldBoss);
LOAD_CONFIG_FUNC(LoadWorldBossRankReward);
LOAD_CONFIG_FUNC(LoadWorldBossHonorReward);
LOAD_CONFIG_FUNC(LoadWorldBossDamageReward);
LOAD_CONFIG_FUNC(LoadAwakeningItem);
LOAD_CONFIG_FUNC(LoadAwakenShopSlot);
LOAD_CONFIG_FUNC(LoadHeroAwaken);
LOAD_CONFIG_FUNC(LoadHeroBreak);
LOAD_CONFIG_FUNC(LoadHeroPokedex);
LOAD_CONFIG_FUNC(LoadAwakenShop);
LOAD_CONFIG_FUNC(LoadGuildSkill);
LOAD_CONFIG_FUNC(LoadOpenServerFundReward);
LOAD_CONFIG_FUNC(LoadAllThePeopleWelfare);
LOAD_CONFIG_FUNC(LoadRecharge);
LOAD_CONFIG_FUNC(LoadMycardRecharge);
LOAD_CONFIG_FUNC(LoadMonthCard);
LOAD_CONFIG_FUNC(LoadLimitPreferentialGoods);
LOAD_CONFIG_FUNC(LoadLimitPreferentialShop);
LOAD_CONFIG_FUNC(LoadLimitPreferentialRecharge);
LOAD_CONFIG_FUNC(LoadLimitPreferentialWelfare);
LOAD_CONFIG_FUNC(LoadGuildItemShop);
LOAD_CONFIG_FUNC(LoadGuildLimitShop);
LOAD_CONFIG_FUNC(LoadGuildRewardShop);
LOAD_CONFIG_FUNC(LoadGuildLimitShopSlot);
LOAD_CONFIG_FUNC(LoadLevelGiftData);
LOAD_CONFIG_FUNC(LoadDirectBuyShopSlot);
LOAD_CONFIG_FUNC(LoadLowCardGroup);
LOAD_CONFIG_FUNC(LoadHighCardGroup);
LOAD_CONFIG_FUNC(LoadCardGroupMap);
LOAD_CONFIG_FUNC(LoadCollectionBook);
LOAD_CONFIG_FUNC(LoadCollectionGroup);
LOAD_CONFIG_FUNC(LoadZhanBuAchievement);
LOAD_CONFIG_FUNC(LoadCardSlot);
LOAD_CONFIG_FUNC(LoadHideShop);
LOAD_CONFIG_FUNC(LoadHeroEvolution);
LOAD_CONFIG_FUNC(LoadHeroQualifications);
LOAD_CONFIG_FUNC(LoadActivationCode);


class CConfigManager : public Singleton<CConfigManager>
{
public:
	bool Init();
	void Release();

	// 重置配置
	void Reset();

	// 重加载文件
	void Reload();

	// 混合区
public:
	// 新建角色初始化配置
	const RoleInit_Config* GetRoleInit(DWORD dwDataId = 1);

	// 取签到配置
	const SignIn_Config* GetSignIn(WORD wDay);

	// 取签到总天数
	WORD GetSignInTotalDay() { return WORD(m_SignInList.size()); };

	// 取豪华配置列表
	const map<WORD, map<WORD, LuxurySign_Config>>& GetLuxurySignInList() { return m_LuxurySignInList; };

	// 取豪华签到奖励配置
	const LuxurySignInReward_Config* GetLuxurySignInReward(DWORD dwRewardId);

	// 取豪华签到数据
	const LuxurySign_Config* GetLuxurySignInData(WORD wSignInId, WORD wRoleLevel);

	// 取迎财神配置
	const YCS_Config* GetYCS(WORD wRoleLevel);

	// 取铜雀台配置列表
	const vector<TQT_Config>* GetTQTList() { return &m_TQTList; };

	// 取充值配置
	const Recharge_Config* GetRecharge(WORD wRechargeId, BYTE byIsMyCard = false);

	// 取mycard充值配置
	const Recharge_Config* GetMyCardRecharge(WORD wRechargeId);

	// 取月卡配置
	const MonthCard_Config* GetMonthCard(WORD wCardId);

	// 取超值折扣购买奖励
	const SuperDiscountBuyReward_Config*  GetSuperDiscountBuyReward(WORD wDay);

	// 取超值折扣累积奖励
	const SuperDiscountAccumulateReward_Config*  GetSuperDiscountAccumulateReward(WORD wTimes);

	// 取超值折扣累积奖励列表
	const map<WORD, SuperDiscountAccumulateReward_Config>& GetSuperDiscountAccumulateRewardList();

	// 取超值折扣购买奖励
	const SuperDiscountBuyReward_Config*  GetNewSuperDiscountBuyReward(WORD wDataId, WORD wDay);

	// 取超值折扣累积奖励
	const SuperDiscountAccumulateReward_Config*  GetNewSuperDiscountAccumulateReward(WORD wDataId, WORD wTimes);


	// 取7天排行奖励
	const RoleRankReward_Config*  GetSevenDayRankReward(WORD wRankType, WORD wRank);

	// 取精准推荐礼包数据
	const AccurateRecommendGoods_Config* GetAccurateRecommendGoods(WORD wGoodsId);

	// 取场景配置
	const Scene_Config* GetScene(DWORD dwSceneId);

	// 取登录奖励配置
	const LoginReward_Config* GetLoginReward(WORD wDays);

	// 取登录奖励配置列表
	const map<WORD, LoginReward_Config>& GetLoginRewardList() { return m_LoginRewardList; };

	// 取封测返利
	const ClosedTestRebate_Config* GetClosedTestRebate(const char *pszAccount);


// 英雄相关
public:
	// 取英雄配置
	const Hero_Config* GetHero(DWORD dwHeroInfoId);

	// 取英雄等级配置
	const HeroLevel_Config* GetHeroLevel(WORD wQuality, WORD wLevel);

	// 取英雄合成配置
	const HeroCompose_Config* GetHeroCompose(DWORD dwHeroId);

	// 取英雄突破升级配置
	const HeroBreakLevelUp_Config* GetHeroBreakLevelUp(WORD wLevel);

	// 取英雄天命数据配置
	const HeroDestiny_Config* GetHeroDestiny(WORD wDestinyLevel);

	// 取英雄上阵配置
	const HeroToBattle_Config* GetHeroToBattle(BYTE byType, BYTE byPos);

	// 取英雄属性配置
	const HeroPro_Config* GetHeroPro(BYTE byProfession, BYTE byQuality, BYTE byLevel);

	// 取英雄培养配置
	//const HeroTrain_Config* GetHeroTrain(BYTE byProfession, WORD wDataIndex);

	// 取英雄觉醒配置
	const HeroAwaken_Config* GetHeroAwaken(BYTE byInitQaulity, BYTE byProfession, BYTE byAwakenStar, BYTE byAwakenLevel);

	// 取英雄突破配置
	const HeroBreak_Config* GetHeroBreak(WORD wHeroInfnId, WORD wBreakLv);

	// 取英雄进化配置
	const HeroEvolution_Config* GetHeroEvolution(WORD wQuality, WORD wStage);

	// 取英雄资质加成
	int GetHeroQualificationsAddRate(DWORD dwQualifications);

	// 取英雄图鉴列表
	vector<DWORD>* GetHeroPokedexList(WORD wQuality);

	// 取英雄培养配置
	const HeroTrainCost_Config* GetHeroTrainCost(BYTE byCostType);

	// 取英雄培养随机属性列表
	const vector<HeroTrainRandPro_Config>* GetHeroTrainRandProList() { return &m_HeroTrainRandProList; };

	// 取英雄培养数据
	const HeroTrain_Config* GetHeroTrain(BYTE byProType, BYTE byProLevel);

	// 取英雄培养列表
	const vector<HeroTrain_Config>* GetHeroTrainList(BYTE byProType);

// 刷新商店
public:

	// 取英雄商店槽位配置
	const RefreshShopSlot_Config* GetRefreshShopSlot(BYTE byShopType, BYTE bySlot);

	// 取英雄商店槽位配置列表
	const vector<RefreshShopSlot_Config>* GetRefreshShopSlotList(BYTE byShopType);


// 叛军相关
public:

	// 取叛军功勋配置
	const RebelExploit_Config* GetRebelExploit(DWORD dwListId, DWORD wAwardId);

	// 取叛军战役配置
	const RebelBattle_Config* GetRebelBattle(WORD wRebelLevel);

	// 取叛军品质配置
	const RebelQuality_Config* GetRebelQuality(BYTE byQuality);

	// 取角色叛军配置
	const RoleRebel_Config* GetRoleRebel(WORD wRoleLevel);

	// 叛军排行配置
	const RebelRankAward_Config* GetRebelRankAward(WORD wRank);

	// 取叛军功勋配置列表
	const map<DWORD, RebelExploit_Config>& GetRebelExpoitList() { return m_RebelExpoitList; };

// 爬塔相关
public:

	// 取爬塔关卡配置
	const TowerMap_Config* GetTowerMap(WORD wMapLevel);

	// 取爬塔章节配置
	const TowerChapter_Config* GetTowerChapter(WORD wChapter);

	// 取爬塔精英关卡
	const TowerEliteMap_Config* GetTowerEliteMap(WORD wMapLevel);

	// 取随机属性
	const TowerRandPro_Config* GetTowerRandPro(BYTE byStar);

	// 取星数商品
	const TowerStarGoods_Config* GetTowerStarGoods(WORD wStar);

// 领地
public:
	// 取领地配置
	const LandCity_Config* GetLand(WORD wLandId);

	// 取领地巡逻事件配置
	const LandPatrolEvent_Config* GetLandPatrolEvent(WORD wLandId);

	// 取领地巡逻配置
	const LandPatrol_Config* GetLandPatrol(BYTE byPatrolType);

// 工会
public:
	// 取工会泉涌配置
	const GuildQY_Config* GetGuidQY(BYTE byType);

	// 取工会泉水奖励配置
	const GuildQSAward_Config* GetGuildQSAward(DWORD dwValue, WORD wGuildLevel);

	// 取工会权限配置
	const GuildPrivilege_Config* GetGuildPrivilege(BYTE byPosition);

	// 取工会配置
	const Guild_Config* GetGuild(WORD wGuildLevel);

	// 取工会限时商品列表
	vector<RateDataEx> GetGuildLimitGoodsList() { return m_GuildLimitGoodsRateList; };

	// 取工会奖励商品ID列表
	const vector<DWORD>& GetGuildRewardGoodsIdList() { return m_GuildRewardGoodsIdList; }

	// 取工会技能
	const GuildSkill_Config* GetGuildSkill(WORD wSkillId, WORD wSkillLevel);

	// 取工会副本配置
	const GuildFBChapter_Config* GetGuildFB(WORD wChapter);

	// 取工会技能列表
	const map<DWORD, GuildSkill_Config>& GetGuildSkillList() { return m_GuildSkillList; };

	// 取工会建筑配置
	const GuildBuilding_Config* GetGuildBuilding(WORD wBuildingId);

// 宝物
public:
	// 取宝物配置
	const Treasure_Config* GetTreasure(DWORD dwInfoId);

	// 取宝物强化配置
	const TreasureEnhance_Config* GetTreasureStrengthen(WORD wQuality, WORD wEnhanceLevel);

	// 取宝物精练配置
	const TreasureRefine_Config* GetTreasureRefine(WORD wQuality, WORD wRefineLevel);

	// 取宝物合成配置
	const TreasureCompose_Config* GetTreasureCompose(DWORD dwTreasureInfoId);

// 竞技场
public:
	// 取竞技场排行配置
	const ArenaRank_Config* GetArenaRank(WORD wRank);

	// 取竞技场机器人列表
	const vector<ArenaRobot_Config>* GetArenaRobotList();

	// 取竞技场奖励
	const ArenaAward_Config* GetArenaAward(WORD wRoleLevel);

	// 取竞技场排行数量
	WORD GetAreanRankNum() { return m_ArenaRankList.size(); };

	// 取竞技场机器人配置
	const ArenaRobot_Config* GetArenaRobot(DWORD dwRobotId);

	// 取首次排行名奖励
	int GetArenaFirstRankReward(WORD wRank);

	// 取竞技场商品列表(实际只是奖励部分商品,嘿嘿...)
	const vector<DWORD>& GetArenaShopGoodsList() { return m_ArenaShopGoodsList; };

// 成就
public:
	// 取成就配置
	const Achievement_Config* GetAchievement(WORD wAchievementId);

// 日常任务
public:
	// 取日常任务配置
	const DailyMission_Config* GetDailyMission(WORD wMissionId);

	// 取日常任务等级配置
	const RoleLvDailyMission_Config* GetRoleLvDailyMission(WORD wRoleLevel);

	// 取某种类型的日常任务列表
	const vector<WORD>* GetTypeDailyMissionList(WORD wType);

// 7天
public:
	// 取7天目标数据
	const SevenDayTarget_Config* GetSevenDayTarget(WORD wTargetId);

	// 取7天活动配置
	const vector<DWORD>* GetServerDayActivity(WORD wDay);

	// 取同种逻辑类型的目标列表
	const vector<WORD>* GetTheSameLogicTargetList(WORD wLogicType);

	// 取7天完成度奖励
	const SevenDayCompletionReward_Config* GetSevenDayCompletionReward(WORD wCompletionNum);

// 世界BOSS
public:

	// 取世界BOSS配置
	const WorldBoss_Config* GetWorldBoss(WORD wBossLv);

	// 取世界BOSS开放时间配置
	const WorldBossOpenTime_Config* GetWorldBossOpenTime(int nWeekDay);

	// 取世界BOSS排行奖励配置
	const WorldBossRankReward_Config* GetWorldBossRankReward(WORD wRank);

	// 取荣誉奖励配置
	const WorldBossHonorReward_Config* GetWorldBossHonorReward(WORD wRewardId);

	// 取击杀奖励配置
	const WorldBossKillReward_Config* GetWorldBossKillReward(WORD wRewardId);

// 卡牌
public:
	// 取卡牌配置
	const Card_Config* GetCard(DWORD dwInfoId);

	// 取卡牌升级配置
	const CardLevelUp_Config* GetCardLevelUp(BYTE byQuality, WORD wLevel);

	// 取卡牌突破配置
	const CardBreak_Config* GetCardBreak(BYTE byQuality, WORD wBreakLevel);

	// 取收藏书配置
	const CollectionBook_Config* GetCollectionBook(WORD wBookId);

	// 取收藏组配置
	const CollectionGroup_Config* GetCollectionGroup(WORD wGroupId);

	// 取低级卡组配置
	const vector<RateDataEx>* GetLowCardGroup(DWORD dwGroupId);

	// 取卡组关卡配置
	const CardGroupMap_Config* GetCardGroupMap(WORD wMapIdx);

	// 取高级卡组配置
	const vector<HighCardGroup_Config>* GetHighCardGroup();

	// 取占卜成就配置
	const ZhanBuAchievement_Config* GetZhanBuAchievement(WORD wLevel);

	// 取卡槽配置
	const CardSlot_Config* GetCardSlot(WORD wSlot);
public:


	// 获取服务器配置 
	DWORD GetServerCfg(DWORD dwKey, DWORD dwDefault = 0);

	// 取通用字符串值
	const char* GetServerCfg_Str(DWORD dwKey, const char *pszDefault);

	// 取通用配置
	const Server_Config*  GetCommonCfg(DWORD dwKey, bool bOutLog = false);

	// 取服务器信息
	const ServerInfo_Config*  GetServerInfo(DWORD dwServerId);


	// 取物品配置
	const Item_Config* GetItem(DWORD dwItemId);

	// 取装备配置
	const Equipment_Config* GetEquipment(DWORD dwEquipmentId);

	// 取装备强化等级配置
	const EquipmentEnhanceLevel_Config* GetEquipmentEnhanceLevel(BYTE byQuality, WORD wStrengLevel);

	// 取装备精炼等级配置
	const EquipmentRefineLevel_Config* GetEquipmentRefineLevel(BYTE byQuality, WORD wRefineLevel);

	// 取装备合成配置
	const EquipmentCompose_Config* GetEquipmentCompose(DWORD dwEquipmentId);

	// 取装备套装配置列表
	const vector<EquipmentSuit_Config>* GetEquipmentSuitList() { return &m_EquipmentSuitList; };


	// 取角色等级配置
	const RoleLevel_Config* GetRoleLevel(WORD wLevel);

	// 取角色属性恢复配置
	const RoleProRecover_Config* GetRoleProRecover(BYTE byProType);

	// 副本相关
public:

	// 取主线副本章节配置
	const FubenChapter_Config* GetMainFBChapter(WORD wChapter);

	// 取主线副本关卡配置
	const FubenMap_Config* GetMainFubenMap(WORD wChapter, BYTE byMapIdx);

	// 取英雄副本章节配置(还没实现)
	const FubenChapter_Config* GetHeroFBChapter(WORD wChapter);

	// 取日常副本章节配置
	const FubenChapter_Config* GetDailyChapter(WORD wChapter);

	// 取精英副本章节配置
	const FubenChapter_Config* GetElitFBeChapter(WORD wChapter);

	// 取日常副本伤害奖励
	const DailyFBDamageReward_Config* GetDailyFBDamageReward(WORD wChapter, WORD wMapIdx, WORD wDamageRate);

	// 取主线副本章节奖励
	const MainFBChapterReward_Config* GetMainFBChapterReward(WORD wRewardLevel);

	// 取限时掉落列表
	const vector<LimitDrop_Config>* GetLimitDropList(DWORD dwDungeonType);

	// 精灵
public:

	// 取世界之心配置
	const HeartOfWorld_Config* GetHeartOfWorld(BYTE byStage);

	// 取精灵配置
	const Spirit_Config* GetSpirit(DWORD dwSpiritId);

	// 取精灵强化配置
	const SpiritEnhance_Config* GetSpiritEnhance(DWORD dwSpiritId, WORD wEnhanceLevel);

	// 取精灵图鉴配置
	const SpiritPokedex_Config* GetSpiritPokedex(DWORD dwPokedexId, WORD wPokedexLevel);

	// 取精灵阶段上限
	BYTE GetSpiritStageMax();

	// 取精灵培养配置
	const SpriteTrain_Config* GetSpriteTrain(DWORD dwSpiritId, WORD wTrainLevel);

	// 取精灵天赋技能配置
	const SpriteTalentSkill_Config* GetSpriteTalentSkill(DWORD dwSpriteId, WORD wTalentSkillLevel);

	// 取精灵升星配置
	const SpriteStarUp_Config* GetSpriteStarUp(WORD wStar);

	// 取精灵特性配置
	const SpriteSpecialProperty_Config* GetSpriteSpecialProperty(DWORD dwSpriteId, WORD wStar);

	// 取精灵资质配置
	const SpriteQualifications_Config* GetSpriteQualifications(WORD wQualifications);



	// 取精灵模型配置(还没实现)
	//const SpiritModel_Config* GetSpiritModel(WORD wModelId);

public:

	// 取商店商品
	const ShopGoodsData_Config* GetShopGoods(BYTE byType, WORD wGoodsId);

	// 取关卡配置
	const Map_Config* GetMap(DWORD dwMapId);

	// 取战关卡役配置
	const Battle_Config* GetMapBattle(DWORD dwBattleId);

	// 取怪物配置
	const Monster_Config* GetMonster(DWORD dwMonsterId);

	// 取装备强化锻造配置
	const EquipmentForge_Config* GetEquipementStrengthenForge(WORD wLevel);

	// 取装备精练锻造配置
	const EquipmentForge_Config* GetEquipementRefineForge(WORD wLevel);

	// 取装备天赋配置
	const EquipmentTalent_Config* GetEquipmentTalent(WORD wTalentId, WORD wEnhanceLevel);

	// 取装备升星配置
	const EquipmentStarUp_Config* GetEquipmentStartUp(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel);

	// 取装备升品配置
	const EquipmentQualityUp_Config* GetEquipmentQualityUp(WORD wQuality);

	// 取援军助威配置
	const ReinforcementEncourage_Config* GetReinforcementEncourage(WORD wReinforcementLevel);


	// 取宝物强化锻造配置
	const TreasureForge_Config* GetTreasureStrengthenForge(WORD wLevel);

	// 取宝物精练锻造配置
	const TreasureForge_Config* GetTreasureRefineForge(WORD wLevel);

	// 取宝物熔炼配置
	const TreasureSmelt_Config* GetTreasureSmelt(DWORD dwTreasureInfoId);

	// 取回收返还配置
	const RecycleReturn_Config* GetRecycleReturn(BYTE byObjType, BYTE byObjLevel);

	// 取夺宝关卡配置
	const DuobaoMap_Config* GetDuobaoMap(WORD wRoleLevel);

	// 取夺宝掉落配置
	const DuobaoDrop_Config* GetDuobaoDrop(BYTE byQuality);

	// 取夺宝进度奖励配置
	const DuobaoPregrossReward_Config* GetDuobaoPregrossReward(WORD wPregrossId);

	// 取宝物升品消耗配置
	const TreasureQualityUPCost_Config* GetTreasureQualityUPCost(WORD wRefineLevel);

public:
	// 取英雄普通召唤配置
	const HeroNormalSummon_Config* GetHeroNormalSummon(DWORD dwRate);

	// 取英雄高级召唤配置
	const HeroAdvancedSummon_Config* GetHeroAdvancedSummon(DWORD dwRate);

	// 取英雄高级召唤配置
	const HeroAdvancedSummon_Config* GetHeroAdvancedSummonEx(DWORD dwRate);

	// 取英雄普通召唤总的几率
	DWORD GetHeroNormalSummonTotalRate();

	// 取英雄高级召唤总的几率
	DWORD GetHeroAdvancedSummonTotalRate();

	// 取英雄高级召唤总的几率
	DWORD GetHeroAdvancedSummonTotalRateEx();

	// 取英雄VIP招募总几率
	DWORD GetHeroVIPSummonTotalRate(WORD wCamp, bool isSpecial);

	// 取英雄VIP招募总几率
	const HeroVipSummon_Config* GetHeroVIPSummon(WORD wCamp, bool isSpecial, DWORD dwRate);

	// 取Vip配置
	const Vip_Config* GetVip(WORD wVipLevel);

	// 取VIPGM配置
	const VipGM_Config* GetVipGM(WORD wGMLevel);

	// 取VIPGM等级
	WORD GetVipGMLevel(DWORD dwRechargeAmount);

	// 取Vip公告Id
	DWORD GetVipNoticeId(WORD wVipLevel);

	// 取VIP等级上限
	WORD GetMaxVipLevel();

	// 取关卡怪物列表
	bool GetMapMonsterList(WORD wMapId, vector<MonsterData>* pMonsterList);

	// 取系统开放等级
	WORD GetSystemOpenLevel(DWORD dwSystemId);
public:

	// 取积分赛目标服务器列表
	const ScoreMatchServerGroup_Config* GetScoreMatchTargetServerList(DWORD dwServerId);

	// 取积分赛机器人
	const ScoreMatchRobot_Config* GetScoreMatchRobot(DWORD dwRobotId);

	// 取积分赛机器人列表
	const vector<ScoreMatchRobot_Config>& GetScoreMatchRobotList(){ return m_ScoreMatchRobotList; };

	// 积分赛挑战任务配置
	const ScoreMatchBattleTask_Config* GetScoreMatchBattleTask(WORD wTaskId);

	// 取积分赛排行奖励
	const ScoreMatchRankReward_Config* GetScoreMatchRankReward(WORD wRank);

	// 取积分赛活动时间
	const ScoreMathActivityTime_Config* GetScoreMatchActivityTime(WORD wIndex);

	// 取积分赛次数消耗
	int GetScoreMatchTimesCost(WORD wType, WORD wTimes);

	// 取积分赛查找目标配置
	const ScoreMatchSearchTarget_Config* GetScoreMatchSearchTarget(WORD wType);

	// 争霸赛
public:
	// 取争霸赛排行配置
	const WarcraftRank_Config* GetWarcraftRank(WORD wRank);

	// 称号
public:
	// 取称号配置
	const Title_Config* GetTitle(int nTitleId);

	// 活动
public:
	// 取活动配置
	const vector<Activity_Config>* GetActivityTime(int nActivityId);
	const Activity_Config* GetActivityTime(int nActivityId, WORD wDataId);
	const LimitActivity_Config* GetLimitActivity(WORD wDataId);

	// 取VIP每日礼包
	const VipDailyGift_Config* GetVipDailyGift(WORD wVipLevel);

	// 取VIP周礼包
	const VipWeekGift_Config* GetVipWeekGift(WORD wGiftId);

	// 取角色等级VIP周礼包列表
	const VipWeekGiftRandList_Config* GetVipWeekGiftRandList(WORD wRoleLevel);

	// 取今天推荐充值配置
	const TodayRecommendRecharge_Config* GetTodayRecommendRecharge(WORD wGiftId);

	// 取今天推荐充值随机列表
	const vector<RateDataEx>* GetTodayRecommendRechargeRandList(WORD wVipLevel);

	// 取等级商店数据
	const LevelShopData_Config* GetLevelShopData(WORD wRoleLevel);

	// 取等级礼包数据
	const LevelGift_Config* GetLevelGiftData(WORD wRoleLevel);

	// 取开服基金奖励
	const OpenServerFundReward_Config* GetOpenServerFundReward(WORD wRewardId);

	// 取全民福利配置
	const AllPeopleWelfare_Config* GetAllPeopleWelfare(WORD wWelfareId);

	// 取限时优惠商品配置
	const LimitPreferentialShop_Config* GetRoleLimitPreferentialShop(WORD wRoleLevel);

	// 限时优惠充值配置
	const LimitPreferentialRecharge_Config* GetLimitPreferentialRecharge(WORD wRechargeId);

	// 限时优惠福利
	const LimitPreferentialWelfare_Config* GetLimitPreferentialWelfare(WORD wWelfareId);

	// 取等级礼包数据
	const LevelGift_Config* GetLevelGift(WORD wLevel);

	// 取在线奖励配置
	const OnlineReward_Config* GetOnlineReward(WORD wDays);

	// 取7天登录配置
	const SevenDayLogin_Config* GetSevenDayLogin(WORD wDay);

	// 取扭蛋抽奖列表
	const vector<NDLottery_Config>* GetNDLotteryList(DWORD dwDataId);

	// 取扭蛋抽奖列表
	const NDLotteryCost_Config* GetNDLotteryCost(DWORD dwDataId);

	// PVP
public:
	// 取挑战奖励
	const PVPBattleReward_Config* GetPVPBattleReward(WORD wType, WORD wBattleTimes);

	// 取排名奖励
	const PVPRankReward_Config* GetPVPRankReward(WORD wRank);

	// PVP活动配置
	const PVPActivityTime_Config* GetPVPActivityTime(WORD wWeekDay);

	// 攻城
public:
	// 取攻城章节配置
	const AttackCityChapter_Config* GetAttackCityChapter(WORD wChapter);

	// 取攻城boss奖励配置
	const AttackCityBossReward_Config* GetAttackCityBossReward(WORD wChapter);

	// 取攻城虚弱CD
	WORD GetAttackCityWeakCD(DWORD dwHPRemainRate);

	// 取攻城鼓舞配置
	const AttackCityEncourage_Config* GetAttackCityEncourage(WORD wTimes);

	// 取攻城活动时间配置
	const AttackCityActivityTime_Config* GetAttackCityActivityTime(WORD wWeekDay);

	// 取攻城排名奖励配置
	const AttackCityRankReward_Config* GetAttackCityRankReward(WORD wRank);

	// 无尽之地
public:
	// 取波配置
	const EndlessLandWave_Config* GetEndlessLandWave(WORD wWave);

	// 取碰撞配置列表
	const vector<EndlessLandCollision_Config>& GetEndlessLandCollisionList();

	// 取连杀奖励
	const EndlessLandContinueKillReward_Config* GetEndlessLandKeepKillReward(DWORD dwContinueKillNum);

	// 取无尽之地排行奖励
	const EndlessLandRankReward_Config*  GetEndlessLandRankReward(WORD wRank);

	// 挑战消耗配置
	const EndlessLandBattleCost_Config* GetEndlessLandBattleCost(WORD wBattleTimes);

public:
	// 取奖励配置
	const Reward_Config* GetReward(DWORD dwRewardId);

	// 取激活码配置
	const ActivationCode_Config* GetActivationCode(const char *pszCode);

	// 取激活码奖励配置
	const ActivationCodeReward_Config* GetActivationCodeReward(DWORD dwRewardId);

	// 取热云统计配置
	const ReyunStatistic_Config* GetReyunStatistic(WORD wType);


	// 节日活动
public:
	// 取节日活动目标配置
	const FestivalActivtiyTarget_Config* GetFestivalActivityTarget(DWORD dwTargetId);

	// 取节日活动配置
	const vector<FestivalActivity_Config>* GetFestivalActivity(WORD wActivityType);

	// 取节日活动活跃目标列表
	const vector<WORD>* GetFestivalActivitySameLogicTargetList(WORD wLogicType);

	// 爵位
public:
	// 取爵位配置
	const Nobility_Config* GetBobility(WORD wNobilityLevel);

	// 宝石
public:
	// 取宝石配置
	const Gem_Config* GetGem(DWORD dwGemId);

	// 取宝石技能
	const GemSkill_Config* GetGemSkill(WORD wSkillId, WORD wSkillLevel);

	// 取宝石孔位
	const GemHole_Config* GetGemHole(WORD wHole);

	// 取宝石镶嵌配置
	const GemInlay_Config* GetGemInlay(WORD wType);

	// 组队副本
public:
	const vector<TeamDungeonLottery_Config>& GetTeamDungeonLotteryList();

	// 取组队副本配置
	const TeamDungeon_Config* GetTeamDungeon(DWORD dwDungeonId);

	// 取任务配置
	const TeamDungeonTask_Config* GetTeamDungeonTask(DWORD dwTaskId);

	// 取任务链配置
	const TeamDungeonTaskLink_Config* GetTeamDungeonTaskLink(DWORD dwTaskLinkId);

	// 取BOSS奖励配置
	const TeamDungeonBossReward_Config* GetTeamDungeonBossReward(WORD wDungeonId, int nEnergy, int nDamageRate);

	// 王者赛
public:
	// 取段位配置
	const KingMatchStage_Config* GetKingMatchStage(WORD wStageLevel);

	// 取王者赛赛季时间配置
	const KingMatchSeasonTime_Config* GetKingMatchSeasonTime(WORD wSeason);

	// 取王者赛季任务配置
	const KingMatchSeasonTask_Config* GetKingMatchSeasonTask(WORD wWinTimes);

	// 取奖励王者赛排行奖励配置
	const KingMatchRankReward_Config* GetKingMatchRankReward(DWORD dwRank);

	// 圣诞节
public:
	// 取圣诞节雪球配置
	const ChristmasSnowball_Config* GetChristmasSnowball(DWORD dwSnowballId);

	// 取圣诞节BOSS配置
	const ChristmasBoss_Config* GetChristmasBoss(WORD wBossType);

	// 取圣诞节BOSS等级配置
	const ChristmasBossLevel_Config* GetChristmasBossLevel(WORD wBossType, DWORD dwBossLevel);

	// facebook活动
public:
	// 取facebook好友邀请配置
	const FacebookFriendInvite_Config* GetFacebookFriendInvite(WORD wInviteNum);

	// 取facebook好友召唤配置
	const FacebookFriendSummon_Config* GetFacebookFriendSummon(WORD wSummonNum);

	// 取facebook帐号绑定配置
	const FacebookAccountBinding_Config* GetFacebookAccountBinding(DWORD dwRewardId);

	// 取facebook分享配置
	const FacebookShare_Config* GetFacebookShare(WORD wChareChapter);

	// 取facebook评论配置
	const FacebookCommend_Config* GetFacebookCommend(WORD wChapter);

	// 充值回馈
public:
	// 取充值回馈配置
	const RechargeReward_Config* GetRechargeReward(WORD wActivityDataId, DWORD dwDay);

	// 取充值回馈额外配置
	const RechargeExtraReward_Config* GetRechargeExtraReward(WORD wActivityDataId);

	const AppleIap_Config* GetAppleIap(const char *pIapId);

	// 累充奖励
public:

	// 取累充奖励配置
	const TotalRechargeReward_Config* GetTotalRechargeReward(WORD wDataId, WORD wRewardId);

	// 热卖商品
public:
	// 取热卖商品配置
	const HotSaleGoods_Config* GetHotSaleGoods(DWORD dwDataId, WORD wGoodsId);

	// 转盘
public:
	// 取转盘配置
	const Turntable_Config* GetTurntableData(WORD wDataId);

	// 取转盘抽奖配置
	const vector<TurntableLottery_Config>* GetTurntableLottery(WORD wDataId);

	// 取转盘排行榜配置
	const TurntableRank_Config* GetTurntableRank(WORD wDataId, WORD wRank);

// 限时产出
public:

	// 取限时产出配置
	const LimitProduct_Config* GetLimitProduct(WORD wSystemId);

// 阵营招募
public:

	// 取阵营招募配置
	const CampRecruit_Config* GetCampRecruit(WORD wDataId, WORD wCamp);

	// 取阵营招募英雄配置列表
	const vector<CampRecruitHero_Config>* GetCampRecruitHeroList(WORD wDataId, WORD wCamp);

// unlock活动
public:

	// 取unlock活动奖励
	const UnlockActivityReward_Config* GetUnlockActivityReward(WORD wRewardId);

	// 取unlock观看视频奖励
	const UnlockWatchingVideoReward_Config* GetUnlockWatchingVideoReward(WORD wWatchingTimes);

// 
public:
	// 取世界等级任务数据
	const WorldTaskLevel_Config* GetWorldTaskLevelData(WORD wOpenServerDays);

	// 取世界等级任务ID列表
	const WorldLevelTaskIdList_Config* GetWorldLevelTaskIdList(WORD wTaskLevel);

	// 取世界等级任务ID列表
	const WorldLevelTaskIdList_Config* GetWorldLevelTaskIdListByTaskId(WORD wTaskId);

	// 取世界等级任务事件
	const WorldLevelTaskEvent_Config* GetWorldLevelTaskEvent(WORD wTaskId);

// 资源战
public:
	// 取资源战章节配置
	const ResourceWarChapter_Config* GetResourceWarChapter(WORD wChapter);

	// 取资源战资源配置
	const ResourceWarResource_Config* GetResourceWarResource(WORD wResourceId);

	// 取资源战科技配置
	const ResourceWarTechnology_Config* GetResourceWarTecknology(WORD wTechnologyLevel);

	// 取资源战工会收益配置
	const ResourceWarGuildProfit_Config* GetResourceWarGuildProfit(WORD wMemberNum);

// 工会战
public:

	// 取工会战星球配置
	const GuildWarStar_Config* GetGuildWarStar(DWORD dwStarId);

	// 取工会战塔配置
	const GuildWarTower_Config* GetGuildWarTower(DWORD dwTowerId);

	// 工会战排行奖励
	const GuildWarRankReward_Config* GetGuildWarRankReward(WORD wRank);

	// 工会战对战奖励
	const GuildWarVSReward_Config* GetGuildWarVSReward(WORD wVSType, WORD wVSResult);

	// 工会战对战活动时间
	const vector<GuildWarVSActivityTime_Config>* GetGuildWarVSActivityTime(WORD wTimeType);

	// 工会战通用配置
	const Server_Config* GetGuildWarCommon(DWORD dwKey);

	// 取布防配置
	const GuildWarOrganizeDefense_Config* GetGuildWarOrganizeDefense(WORD wTowerType);

	// 取工会战大区配置
	const GuildWarRegion_Config* GetGuildWarRegion(WORD wRegionId);

	// 取工会战大区配置
	const GuildWarRegion_Config* GetGuildWarRegionByServerId(WORD wServerId);

	// ygame相关配置
public:

	// 取ygame支付配置
	const YGamePayment_Config* GetYGamePayment(string strPaymentId);

	// 取ygame支付配置
	const YGamePayment_Config* GetYGamePayment(DWORD dwAssistId);

public:
	const RechargeRankReward_Config* GetRechargeAndCostRankReward(WORD wRankType, WORD wRank);

	// 挂机相关 m_vHangUpDropList
public:
	const vector<HangUpDrop_Config>* GetHangUpDropList() { return &m_vHangUpDropList; };


public:

	// 检测是否帐号白名单
	bool IsAccountWhite(const char *pszAccount);

	// 检测是否IP白名单
	bool IsIPWhite(const char *pszIP);

protected:
	// 注册加载回调函数
	bool RegisterCallbackFunc(const LoadConfigCallbackData &CallbackData);
	bool RegisterCallbackFunc(string file, LoadCallbackFunc func);

	// 加载所有配置文件
	bool LoadAllFile();

	// 注册激活码文件(todo::目前已经不使用)
	void RegisterActivationCodeFile();

public:
	vector<LoadConfigCallbackData> m_CallbackDataList;	// 回调数据列表
	map<string, string> m_CallbackFuncFindList;			// 回调函数查找列表

	// 混合区
	map<DWORD, Server_Config> m_CommonList;								// 通用配置列表
	map<DWORD, ServerInfo_Config> m_ServerInfoList;						// 通用配置列表


	// 配置文件列表
	map<DWORD, Monster_Config> m_MonsterList;							// 怪物列表
	map<DWORD, Battle_Config> m_BattleList;								// 战役列表
	map<DWORD, Map_Config> m_MapList;									// 关卡列表
	map<DWORD, FubenChapter_Config> m_MainFBChapterList;				// 主线副本章节列表
	map<DWORD, HeroToBattle_Config> m_HeroToBattleList;					// 英雄上阵数据列表
	map<DWORD, FubenChapter_Config> m_DailyChapterList;					// 日常副本章节列表
	map<DWORD, FubenChapter_Config> m_HeroFBChapterList;				// 英雄副本章节列表
	map<DWORD, FubenChapter_Config> m_EliteFBChapterList;				// 精英副本章节列表
	map<DWORD, DailyFBDamageReward_Config> m_DailyFBDamageReward;		// 日常副本伤害奖励
	map<DWORD, MainFBChapterReward_Config> m_MainFBChapterRewardList;	// 主线副本章节奖励列表
	map<DWORD, vector<LimitDrop_Config>> m_vLimitDropList;				// 限时掉落列表
	

	map<DWORD, Item_Config> m_ItemList;									// 物品配置
	map<WORD,  RoleLevel_Config> m_RoleLevelList;						// 角色等级配置
	map<DWORD, RoleProRecover_Config> m_RoleProRecoverList;				// 角色属性恢复配置
	map<DWORD, RoleInit_Config> m_RoleInitList;							// 角色初始化列表
	vector<TimeEvent_Config> m_vTimeEventList;							// 时间事件列表

	// 英雄
	map<DWORD, Hero_Config> m_HeroList;									// 英雄配置
	map<DWORD, HeroLevel_Config> m_HeroLevelList;						// 英雄等级配置
	map<DWORD, HeroPro_Config> m_HeroProList;							// 英雄属性配置
	map<DWORD, HeroBreakLevelUp_Config> m_BreakLevelUpList;				// 英雄突破升级配置列表
	map<DWORD, HeroCompose_Config> m_HeroComposeList;					// 英雄合成配置列表
	map<DWORD, vector<HeroTrain_Config>> m_HeroTrainList;						// 英雄培养列表
	vector<HeroTrainRandPro_Config> m_HeroTrainRandProList;				// 英雄培养随机属性列表
	map<DWORD, HeroTrainCost_Config> m_HeroTrainCostList;				// 英雄培养消耗列表
	vector<HeroNormalSummon_Config> m_NormalSummonList;					// 普通召唤列表
	vector<HeroAdvancedSummon_Config> m_AdvancedSummonList;				// 高级召唤列表
	map<DWORD, vector<HeroVipSummon_Config>> m_HeroVipSummonList;		// 英雄VIP召唤列表
//	vector<HeroSoulShop_Config> m_HeroSoulShopList;						// 雄魂商店列表
	map<DWORD, HeroDestiny_Config> m_HeroDestinyList;					// 英雄天命配置列表
	map<DWORD, HeroAwaken_Config> m_HeroAwakenList;						// 英雄觉醒配置列表
	vector<RefreshShopSlot_Config> m_HeroShopSlotList;					// 英雄商店槽位配置列表
	vector<RefreshShopSlot_Config> m_AwakenShopSlotList;				// 觉醒商店槽位配置列表
	vector<RefreshShopSlot_Config> m_CrystalShopSlotList;				// 水晶商店槽位配置列表
	map<DWORD, HeroBreak_Config> m_HeroBreakList;						// 英雄突破配置
	map<DWORD, vector<DWORD>> m_HeroPokedexList;						// 英雄图鉴列表
	vector<RefreshShopSlot_Config> m_DirectBuyShopSlotList;				// 直购商店槽位配置列表
	map<DWORD, HeroEvolution_Config> m_HeroEvoluctionList;				// 英雄进化配置列表
	map<DWORD, int> m_HeroQualificationsList;							// 英雄资质配置列表

	// 装备
	map<DWORD, Equipment_Config> m_EquipmentList;							// 装备配置列表
	map<DWORD, EquipmentCompose_Config> m_EquipmentComposeList;				// 装备合成配置列表
	map<DWORD, EquipmentEnhanceLevel_Config> m_EquipmentEnhanceList;		// 装备强化配置列表
	map<DWORD, EquipmentRefineLevel_Config> m_EquipmentRefineList;			// 装备精练配置列表
	map<DWORD, ShopGoodsData_Config> m_ShopGoodshList;						// 商店商品列表
	vector<EquipmentForge_Config> m_EquipmentEnhanceForgeList;				// 装备强化锻造配置列表
	vector<EquipmentForge_Config> m_EquipmentRefineForgeList;				// 装备精练锻造配置列表
	vector<EquipmentSuit_Config> m_EquipmentSuitList;						// 装备套装配置列表
	map<WORD, vector<EquipmentTalent_Config>> m_EuipmentTalentList;			// 装备天赋列表
	map<DWORD, EquipmentStarUp_Config> m_EuipmentStarUpList;				// 装备升星配置列表
	map<DWORD, EquipmentQualityUp_Config> m_EuipmentQualityUpList;			// 装备升品配置列表
	map<DWORD, ReinforcementEncourage_Config> m_ReinforcementEncourageList;	// 援军助威配置列表
	
	

	// 宝物相关
	map<DWORD, Treasure_Config> m_TreasureList;							// 宝物配置列表
	map<DWORD, TreasureEnhance_Config> m_TreasureEnhanceList;		// 宝物强化配置列表
	map<DWORD, TreasureRefine_Config> m_TreasureRefineList;				// 宝物精练配置列表
	map<DWORD, TreasureCompose_Config> m_TreasureComposeList;			// 宝物合成配置列表
	vector<TreasureForge_Config> m_TreasureEnhanceForgeList;			// 宝物强化锻造配置列表
	vector<TreasureForge_Config> m_TreasureRefineForgeList;				// 宝物精练锻造配置列表
	map<DWORD, TreasureSmelt_Config> m_TreasureSmeltList;				// 宝物熔炼配置列表
	map<DWORD, TreasureQualityUPCost_Config> m_TreasureQualityUPCostList;// 宝物升品消耗配置列表

	// 叛军
	map<DWORD, RebelExploit_Config> m_RebelExpoitList;					// 叛军功勋配置列表
	map<DWORD, RebelBattle_Config> m_RebelBattleList;					// 叛军战役配置列表
	map<DWORD, RebelQuality_Config> m_RebelQualityList;					// 叛军品质配置列表
	map<DWORD, RoleRebel_Config> m_RoleRebelList;						// 角色叛军配置列表
	map<DWORD, RebelRankAward_Config> m_RebelRankAwardList;				// 叛军排行榜奖励配置列表

	// 回收
	map<DWORD, RecycleReturn_Config> m_RecycleReturnList;				// 回收返还配置列表

	// 夺宝
	map<DWORD, DuobaoMap_Config> m_DuobaoMapList;						// 夺宝关卡配置列表
	map<DWORD, DuobaoDrop_Config> m_DuobaoDropList;						// 夺宝掉落配置列表
	map<DWORD, DuobaoPregrossReward_Config> m_DuobaoPregrossRewardList;	// 夺宝进度奖励配置列表
	

	// 爬塔
	map<DWORD, TowerMap_Config> m_TowerMapList;							// 爬塔普通关卡配置
	map<DWORD, TowerChapter_Config> m_TowerChapterList;					// 爬塔普通章节配置
	map<DWORD, TowerEliteMap_Config> m_TowerEliteMapList;				// 爬塔精英关卡配置
	map<DWORD, TowerRandPro_Config> m_TowerRandProList;					// 爬塔随机属性配置
	map<DWORD, TowerStarGoods_Config> m_TowerStarGoodsList;				// 爬塔星数商品配置

	// 领地
	map<DWORD, LandCity_Config> m_LandCityList;							// 领地城池配置列表
	map<DWORD, LandPatrolEvent_Config> m_LandPatrolEventList;			// 领地巡逻事件配置列表
	map<DWORD, LandPatrol_Config> m_LandPatrolList;						// 领地巡逻配置列表
	
	// 精灵
	map<DWORD, HeartOfWorld_Config> m_HeartOfWorldList;					// 世界之心配置列表
	map<DWORD, Spirit_Config> m_SpiritList;								// 精灵配置列表
	map<DWORD, SpiritEnhance_Config> m_SpiritEnhanceList;				// 精灵强化配置列表
	map<DWORD, SpiritPokedex_Config> m_SpiritPokedexList;				// 精灵图鉴列表
	map<DWORD, SpriteTrain_Config> m_SpiritTrainList;					// 精灵培养配置列表
	map<DWORD, SpriteTalentSkill_Config> m_SpriteTalentSkillList;		// 精灵天赋技术配置列表
	map<DWORD, SpriteStarUp_Config> m_SpriteStarUpList;					// 精灵升星配置列表	
	map<DWORD, SpriteSpecialProperty_Config> m_SpriteSpecialPropertyList;	// 精灵特性配置列表	
	map<DWORD, SpriteQualifications_Config> m_SpriteQualificationsList;	// 精灵资质列表	


	// 竞技场
	vector<ArenaRobot_Config> m_ArenaRobotList;							// 竞技场机器人列表
	map<DWORD, ArenaRank_Config> m_ArenaRankList;						// 竞技场排行配置列表
	map<DWORD, ArenaAward_Config> m_ArenaAwardList;						// 竞技场奖励配置列表
	map<DWORD, int> m_ArenaFirstRankRewardList;							// 竞技场首次排名奖励列表
	vector<DWORD> m_ArenaShopGoodsList;									// 竞技场商店商品列表(奖励部分的,作为红点提示使用)

	// 成就
	map<DWORD, Achievement_Config> m_AchievementList;					// 成就配置列表

	// 日常任务
	map<DWORD, DailyMission_Config> m_DailyMissionList;					// 日常任务配置列表
	map<DWORD, vector<WORD>> m_DailyMissionTypeList;					// 日常任务类型配置列表
	map<WORD, RoleLvDailyMission_Config> m_RoleLvDailyMissionList;		// 角色等级对应的日常任务
	

	// 工会
	map<DWORD, Guild_Config> m_GuildList;								// 工会配置
	map<WORD, WORD> m_GuildIconList;									// 工会图标配置
	map<DWORD, GuildQY_Config> m_GuildQYList;							// 工会泉涌配置
	map<DWORD, GuildQSAward_Config> m_GuildQSRewardList;				// 工会泉水奖励配置
	map<DWORD, GuildPrivilege_Config> m_GuildPrivilegeList;				// 工会权限配置
	vector<RateDataEx> m_GuildLimitGoodsRateList;						// 工会随时商品几率列表
	map<DWORD, GuildSkill_Config> m_GuildSkillList;						// 工会技能配置列表
	map<WORD, GuildFBChapter_Config> m_GuildFBChapterList;				// 工会副本章节配置列表
	vector<DWORD> m_GuildRewardGoodsIdList;								// 工会奖励商品ID列表
	vector<RefreshShopSlot_Config> m_GuildLimitShopSlotList;			// 工会限时商店槽位配置列表
	map<WORD, GuildBuilding_Config> m_GuildBuildingList;				// 工会建筑配置
	

	// Vip
	map<WORD, Vip_Config> m_VipList;									// Vip配置
	map<WORD, DWORD> m_VipNoticeList;									// Vip公告配置
	map<WORD, VipGM_Config> m_VipGMList;								// VipGM配置

	// 签到
	map<WORD, SignIn_Config> m_SignInList;							// 签到配置
	map<WORD, map<WORD, LuxurySign_Config>> m_LuxurySignInList;		// 豪华签到列表[sign_in_id, role_level, data]
	map<DWORD, LuxurySignInReward_Config> m_LuxurySignInRewardList;	// 豪华签到奖励列表
	
	

	// 迎财神
	map<WORD, YCS_Config> m_YCSList;									// 迎财神配置

	// 铜雀台
	vector<TQT_Config> m_TQTList;										// 铜雀台配置

	// 世界BOSS
	map<DWORD, WorldBossOpenTime_Config> m_WorldBossOpenTimeList;		// 世界BOSS开放时间配置表
	map<DWORD, WorldBoss_Config> m_WorldBossList;						// 世界BOSS配置列表
	map<DWORD, WorldBossRankReward_Config> m_WorldBossRankRewardList;	// 世界BOSS排行奖励配置列表
	map<DWORD, WorldBossHonorReward_Config> m_WorldBossHonorRewardList;	// 世界BOSS荣誉奖励配置列表
	map<DWORD, WorldBossKillReward_Config> m_WorldBossDamageRewardList;	// 世界BOSS伤害奖励配置列表
	//map<DWORD, WorldBossKillReward_Config> m_WorldBossDamageRewardList;	// 世界BOSS影响刷新时间配置列表

	// 7天系统
	map<DWORD, SevenDayTarget_Config> m_SevenDayTargetList;			// 7天目标数据列表
	map<WORD, vector<DWORD>> m_SevenDayActiviyList;					// 7天活动列表
	map<WORD, vector<WORD>> m_TheSameTargetList;					// 同类目标列表
	map<WORD, SevenDayCompletionReward_Config> m_SevenDayCompletionRewardList;	// 7天完成度奖励列表
	vector<WORD> m_vSevenDayShopGoodsList;							// 7天商店商品列表


	// 占卜
	map<DWORD, Card_Config> m_CardList;							// 仙灵配置
	map<DWORD, CardLevelUp_Config> m_CardLevelUpList;			// 仙灵升级配置  
	map<DWORD, CardBreak_Config> m_CardBreakList;				// 仙灵突破配置  
	vector<RefreshShopSlot_Config> m_StarSoulShopSlotList;		// 仙灵商店槽位配置列表
	map<DWORD, vector<RateDataEx>> m_LowCardGroupList;			// 低级卡组列表 group_id, card_id_list
	map<DWORD, CollectionBook_Config> m_CollectionBookList;		// 收藏书配置
	map<DWORD, CollectionGroup_Config> m_CollectionGroupList;	// 收藏组配置
	vector<HighCardGroup_Config> m_HighCardGroupList;			// 高级卡组配置
	map<DWORD, CardGroupMap_Config> m_CardGroupMapList;			// 卡组关卡配置
	map<WORD, ZhanBuAchievement_Config> m_ZhanBuAchievementList;// 占卜成就配置
	map<WORD, CardSlot_Config> m_CardSlotList;// 卡槽配置
	
	
	// 积分赛
	map<DWORD, ScoreMatchServerGroup_Config> m_ScoreMatchTargetServerList;		// 积分赛目标服务器列表
	vector<ScoreMatchRobot_Config> m_ScoreMatchRobotList;						// 积分赛机器人列表
	map<DWORD, ScoreMatchBattleTask_Config> m_ScoreMatchBattleTaskList;			// 积分赛挑战任务列表
	map<DWORD, ScoreMatchRankReward_Config> m_ScoreMatchRankRewardList;			// 积分赛排行奖励列表
	map<DWORD, ScoreMathActivityTime_Config> m_ScoreMatchActivityTimeList;		// 积分赛活动时间列表
	map<DWORD, int> m_ScoreMatchTimesCostList;									// 积分赛次数消耗列表
	map<DWORD, ScoreMatchSearchTarget_Config> m_ScoreMatchSearchList;			// 积分赛查找目标列表
	

	// 称号
	map<DWORD, Title_Config> m_TitleList;	// 称号配置列表

	// 系统开放
	map<DWORD, WORD> m_SystemOpenList;	// 系统开放配置列表

	// 奖励
	map<DWORD, Reward_Config> m_RewardList;	// 奖励列表

	// 活动
	map<DWORD, LimitActivity_Config> m_LimitActivityList;	// 限时活动配置列表
	map<DWORD, vector<Activity_Config>> m_ActivityTimeList;	// 活动配置列表
	

	// VIP礼包
	map<DWORD, VipDailyGift_Config> m_VipDailyGiftList;	// VIP每日礼包列表
	map<DWORD, VipWeekGift_Config> m_VipWeekGiftList;	// VIP周礼包配置列表
	map<DWORD, VipWeekGiftRandList_Config> m_VipWeekGiftRandList;	// VIP每周礼包随机库
	map<WORD, TodayRecommendRecharge_Config> m_TodayRecommendRechargeList;	// 今日推荐充值列表
	map<WORD, vector<RateDataEx>> m_TodayRecommendRechargeRandList;	// 今日推荐充值随机列表

	// 等级商店
	map<DWORD, LevelShopData_Config> m_LevelShopDataList;

	// 等级礼包
	map<DWORD, LevelGift_Config> m_LevelGiftList;

	// 开服基金
	map<DWORD, OpenServerFundReward_Config> m_OpenServerFundRewardList;
	map<DWORD, AllPeopleWelfare_Config> m_AllPeopleWelfareList;

	// 充值
	map<DWORD, Recharge_Config> m_RechargeList;		// 充值配置列表
	map<DWORD, MonthCard_Config> m_MonthCardList;	// 月卡配置列表
	map<DWORD, Recharge_Config> m_MyCardRechargeList;	// mycard充值配置列表

	// 限时优惠
	map<DWORD, LimitPreferentialShop_Config> m_LimitPreferentialShop;			// 限时优惠商店
	map<DWORD, LimitPreferentialRecharge_Config> m_LimitPreferentialRecharge;	// 限时优惠充值
	map<DWORD, LimitPreferentialWelfare_Config> m_LimitPreferentialWelfare;		// 限时优惠福利

	map<string, ActivationCode_Config> m_ActivationCodeList; // 激活码配置(todo::目前已经不使用)
	map<DWORD, ActivationCodeReward_Config> m_ActivationCodeRewardList; // 激活码奖励配置
	map<WORD, ReyunStatistic_Config> m_ReyunStatisticList;	// 热云统计配置列表

	// PVP
	map<DWORD, PVPBattleReward_Config> m_PVPBattleRewardList;	// PVP战斗奖励列表
	map<DWORD, PVPRankReward_Config> m_PVPRankRewardList;	// PVP排名奖励列表
	map<DWORD, PVPActivityTime_Config> m_PVPActivityTimeList;	// PVP活动时间列表

	// 怪物攻城
	map<WORD, AttackCityChapter_Config> m_AttackCityChapterList;	// 攻城章节列表 
	map<WORD, AttackCityBossReward_Config> m_AttackCityBossRewardList;	// 攻城BOSS奖励列表 
	vector<AttackCityWeak_Config> m_AttackCityWeakList; // 攻城虚弱配置
	map<WORD, AttackCityEncourage_Config> m_AttackCityEncourageList;	// 攻城鼓舞配置列表 
	map<WORD, AttackCityActivityTime_Config> m_AttackCityActivityTimeList; // 攻城活动时间配置列表
	map<WORD, AttackCityRankReward_Config> m_AttackCityRankRewardList; // 攻城排名奖励配置列表
	
	// 无尽之地
	vector<EndlessLandCollision_Config> m_EndlessLandCollisionList; // 无尽之地碰撞配置列表
	map<WORD, EndlessLandWave_Config> m_EndlessLandWaveList;	// 无尽之地波配置列表
	map<DWORD, EndlessLandContinueKillReward_Config> m_EndlessLandContinueKillRewardList; // 无尽之地连杀奖励列表
	map<WORD, EndlessLandRankReward_Config> m_EndlessLandRankRewardList; // 无尽之地排行奖励列表
	map<WORD, EndlessLandBattleCost_Config> m_EndlessLandBattleCostList; // 无尽之地挑战消耗列表

	

	// 爵位
	map<WORD, Nobility_Config> m_NobilityList;	// 爵位配置列表

	// 超值折扣
	map<WORD, SuperDiscountBuyReward_Config> m_SuperDiscountBuyRewardList;
	map<WORD, SuperDiscountAccumulateReward_Config> m_SuperDiscountAccumulateRewardList;

	// 超值折扣(新)
	map<DWORD, SuperDiscountBuyReward_Config> m_NewSuperDiscountBuyRewardList;
	map<DWORD, SuperDiscountAccumulateReward_Config> m_NewSuperDiscountAccumulateRewardList;

	// 7天排行活动奖励
	map<DWORD, RoleRankReward_Config> m_SevenDayRankRewardList;
	
	// 节日活动
	map<DWORD, vector<FestivalActivity_Config>> m_FestivalActivityList;			// 节日活动列表
	map<DWORD, FestivalActivtiyTarget_Config> m_FestivalActivtiyTargetList;		// 节日活动目标列表
	map<WORD, vector<WORD>> m_FestivalActivitySameLogicTargetList;				// 节日活动同类的逻辑目标列表

	map<DWORD, AccurateRecommendGoods_Config> m_AccurateRecommendGoodsList;		// 精准推荐商品列表

	// 宝石
	map<DWORD, Gem_Config> m_GemList; // 宝石列表
	map<DWORD, GemSkill_Config> m_GemSkillList; // 宝石技能列表
	map<DWORD, GemHole_Config> m_GemHoleList; // 宝石孔位列表
	map<DWORD, GemInlay_Config> m_GemInlayList; // 宝石镶嵌列表

	// 组队副本
	vector<TeamDungeonLottery_Config> m_TeamDungeonLotteryList;				// 抽奖列表
	map<DWORD, TeamDungeon_Config> m_TeamDungeonList;						// 组队副本列表
	map<DWORD, TeamDungeonTaskLink_Config> m_TeamDungeonTaskLinkList;		// 组队副本任务链列表
	map<DWORD, TeamDungeonTask_Config> m_TeamDungeonTaskList;				// 组队副本任务列表
	map<DWORD, vector<TeamDungeonBossReward_Config>> m_TeamDungeonBossRewardList;	// 组队副本Boss奖励列表

	// 场景
	map<DWORD, Scene_Config> m_SceneList; // 场景列表

	map<WORD, LoginReward_Config> m_LoginRewardList; // 登录奖励配置列表

	map<string, ClosedTestRebate_Config> m_ClosedTestRebateList; // 封测充值返利列表

	map<string, string> m_AccountWhiteList; // 帐号白名单列表
	map<string, string> m_IPWhiteList;		// IP白名单列表

	// 王者赛
	map<WORD, KingMatchStage_Config> m_KingMatchStageList; // 王者赛段位配置列表
	map<WORD, KingMatchSeasonTask_Config> m_KingMatchSeasonTaskList; // 王者赛季任务配置列表
	vector<KingMatchRankReward_Config> m_KingMatchRankRewardList; // 王者赛排行奖励配置列表
	map<WORD, KingMatchSeasonTime_Config> m_KingMatchSeasonTimeList; // 王者赛季时间配置列表

	// 圣诞节
	map<DWORD, ChristmasBossLevel_Config> m_ChristmasBossLevelList;	// 圣诞节Boss等级配置列表
	map<DWORD, ChristmasBoss_Config> m_ChristmasBossList;			// 圣诞节Boss配置列表
	map<DWORD, ChristmasSnowball_Config> m_ChristmasSnowballList;	// 圣诞节雪球配置列表

	// facebook活动
	map<DWORD, FacebookFriendInvite_Config> m_FacebookFriendInvitList;		// facebook好友邀请列表
	map<DWORD, FacebookFriendSummon_Config> m_FacebookFriendSummonList;		// facebook好友召唤列表
	map<DWORD, FacebookAccountBinding_Config> m_FacebookAccountBindingList;	// facebook账号绑定列表
	map<DWORD, FacebookShare_Config> m_FacebookChareList;					// facebook分享列表
	map<DWORD, FacebookCommend_Config> m_FacebookCommendList;				// facebook评论列表

	// 在线奖励
	map<DWORD, OnlineReward_Config> m_OnlineRewardList;	// 在线奖励列表

	// 7天登录
	map<DWORD, SevenDayLogin_Config> m_SevenDayLoginList;	// 7天登录配置列表

	// 充值回馈
	map<DWORD, RechargeReward_Config> m_RechargeRewardList;				// 充值回馈配置列表
	map<DWORD, RechargeExtraReward_Config> m_RechargeExtraRewardList;	// 充值回馈额外奖励配置列表
	map<string, AppleIap_Config> m_AppleIAPList;						// AppleIap信息列表
	

	// 累充奖励
	map<DWORD, TotalRechargeReward_Config> m_TotalRechargeRewardList;	// 累充奖励列表

	// 扭蛋抽奖
	map<DWORD, vector<NDLottery_Config>> m_NDLotteryList;				// 扭蛋抽奖列表
	map<DWORD, NDLotteryCost_Config> m_NDLotteryCostList;				// 扭蛋抽奖消耗列表

	// 热卖商品
	map<DWORD, HotSaleGoods_Config> m_HotSaleGoodsList;	// 热卖商品列表

	// 转盘
	map<DWORD, Turntable_Config> m_TurntableList;		// 转盘列表
	map<DWORD, TurntableRank_Config> m_TurntableRankList;	// 转盘排行榜列表
	map<DWORD, vector<TurntableLottery_Config>> m_TurntableLotteryList;	// 转盘抽奖列表

	// 限时产出
	map<DWORD, LimitProduct_Config> m_LimitProductList;	// 限时产出列表

	// 阵营招募
	map<DWORD, CampRecruit_Config> m_CampRecruitList;			// 阵营招募配置列表 data_id + camp, data_list
	map<DWORD, vector<CampRecruitHero_Config>> m_CampRecruitHeroList;	// 阵营招募英雄列表 data_id + camp, data_list

	// unlock活动
	map<DWORD, UnlockActivityReward_Config> m_UnlockActivityRewardList;
	map<DWORD, UnlockWatchingVideoReward_Config> m_UnlockWathingVideoRewardList;
	
	// 世界等级任务
	map<DWORD, WorldTaskLevel_Config> m_WorldTaskLevelList;				// 世界任务等级列表
	map<DWORD, WorldLevelTaskIdList_Config> m_WorldLevelTaskIdList;		// 世界等级任务ID列表[TaskLevel,List]
	map<DWORD, WorldLevelTaskIdList_Config> m_WorldLevelTaskIdListEx;	// 世界等级任务ID列表[TaskId, List]
	map<DWORD, WorldLevelTaskEvent_Config> m_WorldLevelTaskEventList;	// 世界等级任务事件列表

	// 资源战配置
	map<DWORD, ResourceWarChapter_Config> m_ResourceWarChapterList;			// 资源战章节配置列表
	map<DWORD, ResourceWarResource_Config> m_ResourceWarResourceList;		// 资源战资源配置列表
	map<WORD, ResourceWarTechnology_Config> m_ResourceWarTechnologyList;	// 资源战科技配置列表
	map<WORD, ResourceWarGuildProfit_Config> m_ResourceWarGuildProfitList;	// 资源战工会收益配置列表

	// 工会战
	map<DWORD, GuildWarStar_Config> m_GuildWarStarList;								// 工会战星球配置
	map<DWORD, GuildWarTower_Config> m_GuildWarTowerList;							// 工会战塔配置
	map<DWORD, GuildWarRankReward_Config> m_GuildWarRankRewardList;					// 工会战排行奖励
	map<DWORD, GuildWarVSReward_Config> m_GuildWarVSRewardList;						// 工会战对战奖励
	map<DWORD, vector<GuildWarVSActivityTime_Config>> m_GuildWarVSActivityTimeList;	// 工会战对战活动时间配置
	map<DWORD, Server_Config> m_GuildWarCommonCfgList;								// 工会战通用配置
	map<DWORD, GuildWarOrganizeDefense_Config> m_GuildWarOrganizeDefenseList;		// 工会战布防配置
	map<DWORD, GuildWarRegion_Config> m_GuildWarRegionList;							// 工会战大区配置

	// YGame支付
	map<string, YGamePayment_Config> m_YGamePaymentList;							// YGame支付配置列表

	// 充值排行榜奖励
	map<DWORD, RechargeRankReward_Config> m_RechargeRankRewardList;		// 充值排行榜奖励[key=rank_type+rank, data]

	// 挂机
	vector<HangUpDrop_Config> m_vHangUpDropList; // 挂机掉落配置
};

 
DWORD g_GetConfigValue(DWORD dwKey, DWORD dwDefault = 0);

extern CConfigManager *g_pCfgMgr;