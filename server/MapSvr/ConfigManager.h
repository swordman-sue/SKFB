/**\file
    �����ļ�������
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

// ���������ļ��ص���������
struct LoadConfigCallbackData
{
	string file;			// �����ļ�
	LoadCallbackFunc func;	// �ص�����
};

#define LOAD_CONFIG_FUNC(func_name) bool func_name(CConfigManager *pConfigMgr, std::vector<string> &data_list); 

// ��ȡItem
bool ReadItem(const char *pszSrc, const char *pszKey, Item &tItem);

// ��ȡuint
DWORD ReadUInt(const char *pszSrc, const char *pszKey, int nDefault = 0);

// ��ȡint
int ReadInt(const char *pszSrc, const char *pszKey, int nDefault = 0);

// ��ȡdouble
double ReadDouble(const char *pszSrc, const char *pszKey, double fDefault = 0);

// ��ȡ�������
bool ReadPair(const char *pszSrc, const char *pszKey, DWORD &dwValue1, DWORD &dwValue2);

// ��ȡ�ַ���
string ReadString(const char *pszSrc, const char *pszKey, const char *pszDefault = "");

// ��ȡһά����
bool ReadArry1(const char *pszSrc, const char *pszKey, vector<DWORD> &list);

// ��ȡ��ά����
bool ReadArry2(const char *pszSrc, const char *pszKey, vector<vector<DWORD>> &list);

// ����ͨ�����������߼�
bool LoadCommonCost(const char *pszSrc, vector<CostData> &cost_list, const char *pszTypeKey = "cost_type_list", const char *pszValueKey = "cost_value_list");

// ��ȡͨ����Ʒ�б�
bool ReadCommonItemList(const char *pszSrc, const char *pszIdKey, const char *pszNumKey, vector<Item> &item_list);

// ��ȡͨ�������б�
bool ReadCommonPropertyList(const char *pszSrc, const char *pszTypeKey, const char *pszValueKey, vector<Property> &sOutProList);

// ����ͨ������
bool LoadCommon(CConfigManager *pConfigMgr, std::vector<string> &data_list);

// ����̵���Ʒ
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

	// ��������
	void Reset();

	// �ؼ����ļ�
	void Reload();

	// �����
public:
	// �½���ɫ��ʼ������
	const RoleInit_Config* GetRoleInit(DWORD dwDataId = 1);

	// ȡǩ������
	const SignIn_Config* GetSignIn(WORD wDay);

	// ȡǩ��������
	WORD GetSignInTotalDay() { return WORD(m_SignInList.size()); };

	// ȡ���������б�
	const map<WORD, map<WORD, LuxurySign_Config>>& GetLuxurySignInList() { return m_LuxurySignInList; };

	// ȡ����ǩ����������
	const LuxurySignInReward_Config* GetLuxurySignInReward(DWORD dwRewardId);

	// ȡ����ǩ������
	const LuxurySign_Config* GetLuxurySignInData(WORD wSignInId, WORD wRoleLevel);

	// ȡӭ��������
	const YCS_Config* GetYCS(WORD wRoleLevel);

	// ȡͭȸ̨�����б�
	const vector<TQT_Config>* GetTQTList() { return &m_TQTList; };

	// ȡ��ֵ����
	const Recharge_Config* GetRecharge(WORD wRechargeId, BYTE byIsMyCard = false);

	// ȡmycard��ֵ����
	const Recharge_Config* GetMyCardRecharge(WORD wRechargeId);

	// ȡ�¿�����
	const MonthCard_Config* GetMonthCard(WORD wCardId);

	// ȡ��ֵ�ۿ۹�����
	const SuperDiscountBuyReward_Config*  GetSuperDiscountBuyReward(WORD wDay);

	// ȡ��ֵ�ۿ��ۻ�����
	const SuperDiscountAccumulateReward_Config*  GetSuperDiscountAccumulateReward(WORD wTimes);

	// ȡ��ֵ�ۿ��ۻ������б�
	const map<WORD, SuperDiscountAccumulateReward_Config>& GetSuperDiscountAccumulateRewardList();

	// ȡ��ֵ�ۿ۹�����
	const SuperDiscountBuyReward_Config*  GetNewSuperDiscountBuyReward(WORD wDataId, WORD wDay);

	// ȡ��ֵ�ۿ��ۻ�����
	const SuperDiscountAccumulateReward_Config*  GetNewSuperDiscountAccumulateReward(WORD wDataId, WORD wTimes);


	// ȡ7�����н���
	const RoleRankReward_Config*  GetSevenDayRankReward(WORD wRankType, WORD wRank);

	// ȡ��׼�Ƽ��������
	const AccurateRecommendGoods_Config* GetAccurateRecommendGoods(WORD wGoodsId);

	// ȡ��������
	const Scene_Config* GetScene(DWORD dwSceneId);

	// ȡ��¼��������
	const LoginReward_Config* GetLoginReward(WORD wDays);

	// ȡ��¼���������б�
	const map<WORD, LoginReward_Config>& GetLoginRewardList() { return m_LoginRewardList; };

	// ȡ��ⷵ��
	const ClosedTestRebate_Config* GetClosedTestRebate(const char *pszAccount);


// Ӣ�����
public:
	// ȡӢ������
	const Hero_Config* GetHero(DWORD dwHeroInfoId);

	// ȡӢ�۵ȼ�����
	const HeroLevel_Config* GetHeroLevel(WORD wQuality, WORD wLevel);

	// ȡӢ�ۺϳ�����
	const HeroCompose_Config* GetHeroCompose(DWORD dwHeroId);

	// ȡӢ��ͻ����������
	const HeroBreakLevelUp_Config* GetHeroBreakLevelUp(WORD wLevel);

	// ȡӢ��������������
	const HeroDestiny_Config* GetHeroDestiny(WORD wDestinyLevel);

	// ȡӢ����������
	const HeroToBattle_Config* GetHeroToBattle(BYTE byType, BYTE byPos);

	// ȡӢ����������
	const HeroPro_Config* GetHeroPro(BYTE byProfession, BYTE byQuality, BYTE byLevel);

	// ȡӢ����������
	//const HeroTrain_Config* GetHeroTrain(BYTE byProfession, WORD wDataIndex);

	// ȡӢ�۾�������
	const HeroAwaken_Config* GetHeroAwaken(BYTE byInitQaulity, BYTE byProfession, BYTE byAwakenStar, BYTE byAwakenLevel);

	// ȡӢ��ͻ������
	const HeroBreak_Config* GetHeroBreak(WORD wHeroInfnId, WORD wBreakLv);

	// ȡӢ�۽�������
	const HeroEvolution_Config* GetHeroEvolution(WORD wQuality, WORD wStage);

	// ȡӢ�����ʼӳ�
	int GetHeroQualificationsAddRate(DWORD dwQualifications);

	// ȡӢ��ͼ���б�
	vector<DWORD>* GetHeroPokedexList(WORD wQuality);

	// ȡӢ����������
	const HeroTrainCost_Config* GetHeroTrainCost(BYTE byCostType);

	// ȡӢ��������������б�
	const vector<HeroTrainRandPro_Config>* GetHeroTrainRandProList() { return &m_HeroTrainRandProList; };

	// ȡӢ����������
	const HeroTrain_Config* GetHeroTrain(BYTE byProType, BYTE byProLevel);

	// ȡӢ�������б�
	const vector<HeroTrain_Config>* GetHeroTrainList(BYTE byProType);

// ˢ���̵�
public:

	// ȡӢ���̵��λ����
	const RefreshShopSlot_Config* GetRefreshShopSlot(BYTE byShopType, BYTE bySlot);

	// ȡӢ���̵��λ�����б�
	const vector<RefreshShopSlot_Config>* GetRefreshShopSlotList(BYTE byShopType);


// �Ѿ����
public:

	// ȡ�Ѿ���ѫ����
	const RebelExploit_Config* GetRebelExploit(DWORD dwListId, DWORD wAwardId);

	// ȡ�Ѿ�ս������
	const RebelBattle_Config* GetRebelBattle(WORD wRebelLevel);

	// ȡ�Ѿ�Ʒ������
	const RebelQuality_Config* GetRebelQuality(BYTE byQuality);

	// ȡ��ɫ�Ѿ�����
	const RoleRebel_Config* GetRoleRebel(WORD wRoleLevel);

	// �Ѿ���������
	const RebelRankAward_Config* GetRebelRankAward(WORD wRank);

	// ȡ�Ѿ���ѫ�����б�
	const map<DWORD, RebelExploit_Config>& GetRebelExpoitList() { return m_RebelExpoitList; };

// �������
public:

	// ȡ�����ؿ�����
	const TowerMap_Config* GetTowerMap(WORD wMapLevel);

	// ȡ�����½�����
	const TowerChapter_Config* GetTowerChapter(WORD wChapter);

	// ȡ������Ӣ�ؿ�
	const TowerEliteMap_Config* GetTowerEliteMap(WORD wMapLevel);

	// ȡ�������
	const TowerRandPro_Config* GetTowerRandPro(BYTE byStar);

	// ȡ������Ʒ
	const TowerStarGoods_Config* GetTowerStarGoods(WORD wStar);

// ���
public:
	// ȡ�������
	const LandCity_Config* GetLand(WORD wLandId);

	// ȡ���Ѳ���¼�����
	const LandPatrolEvent_Config* GetLandPatrolEvent(WORD wLandId);

	// ȡ���Ѳ������
	const LandPatrol_Config* GetLandPatrol(BYTE byPatrolType);

// ����
public:
	// ȡ����Ȫӿ����
	const GuildQY_Config* GetGuidQY(BYTE byType);

	// ȡ����Ȫˮ��������
	const GuildQSAward_Config* GetGuildQSAward(DWORD dwValue, WORD wGuildLevel);

	// ȡ����Ȩ������
	const GuildPrivilege_Config* GetGuildPrivilege(BYTE byPosition);

	// ȡ��������
	const Guild_Config* GetGuild(WORD wGuildLevel);

	// ȡ������ʱ��Ʒ�б�
	vector<RateDataEx> GetGuildLimitGoodsList() { return m_GuildLimitGoodsRateList; };

	// ȡ���ά����ƷID�б�
	const vector<DWORD>& GetGuildRewardGoodsIdList() { return m_GuildRewardGoodsIdList; }

	// ȡ���Ἴ��
	const GuildSkill_Config* GetGuildSkill(WORD wSkillId, WORD wSkillLevel);

	// ȡ���ḱ������
	const GuildFBChapter_Config* GetGuildFB(WORD wChapter);

	// ȡ���Ἴ���б�
	const map<DWORD, GuildSkill_Config>& GetGuildSkillList() { return m_GuildSkillList; };

	// ȡ���Ὠ������
	const GuildBuilding_Config* GetGuildBuilding(WORD wBuildingId);

// ����
public:
	// ȡ��������
	const Treasure_Config* GetTreasure(DWORD dwInfoId);

	// ȡ����ǿ������
	const TreasureEnhance_Config* GetTreasureStrengthen(WORD wQuality, WORD wEnhanceLevel);

	// ȡ���ﾫ������
	const TreasureRefine_Config* GetTreasureRefine(WORD wQuality, WORD wRefineLevel);

	// ȡ����ϳ�����
	const TreasureCompose_Config* GetTreasureCompose(DWORD dwTreasureInfoId);

// ������
public:
	// ȡ��������������
	const ArenaRank_Config* GetArenaRank(WORD wRank);

	// ȡ�������������б�
	const vector<ArenaRobot_Config>* GetArenaRobotList();

	// ȡ����������
	const ArenaAward_Config* GetArenaAward(WORD wRoleLevel);

	// ȡ��������������
	WORD GetAreanRankNum() { return m_ArenaRankList.size(); };

	// ȡ����������������
	const ArenaRobot_Config* GetArenaRobot(DWORD dwRobotId);

	// ȡ�״�����������
	int GetArenaFirstRankReward(WORD wRank);

	// ȡ��������Ʒ�б�(ʵ��ֻ�ǽ���������Ʒ,�ٺ�...)
	const vector<DWORD>& GetArenaShopGoodsList() { return m_ArenaShopGoodsList; };

// �ɾ�
public:
	// ȡ�ɾ�����
	const Achievement_Config* GetAchievement(WORD wAchievementId);

// �ճ�����
public:
	// ȡ�ճ���������
	const DailyMission_Config* GetDailyMission(WORD wMissionId);

	// ȡ�ճ�����ȼ�����
	const RoleLvDailyMission_Config* GetRoleLvDailyMission(WORD wRoleLevel);

	// ȡĳ�����͵��ճ������б�
	const vector<WORD>* GetTypeDailyMissionList(WORD wType);

// 7��
public:
	// ȡ7��Ŀ������
	const SevenDayTarget_Config* GetSevenDayTarget(WORD wTargetId);

	// ȡ7������
	const vector<DWORD>* GetServerDayActivity(WORD wDay);

	// ȡͬ���߼����͵�Ŀ���б�
	const vector<WORD>* GetTheSameLogicTargetList(WORD wLogicType);

	// ȡ7����ɶȽ���
	const SevenDayCompletionReward_Config* GetSevenDayCompletionReward(WORD wCompletionNum);

// ����BOSS
public:

	// ȡ����BOSS����
	const WorldBoss_Config* GetWorldBoss(WORD wBossLv);

	// ȡ����BOSS����ʱ������
	const WorldBossOpenTime_Config* GetWorldBossOpenTime(int nWeekDay);

	// ȡ����BOSS���н�������
	const WorldBossRankReward_Config* GetWorldBossRankReward(WORD wRank);

	// ȡ������������
	const WorldBossHonorReward_Config* GetWorldBossHonorReward(WORD wRewardId);

	// ȡ��ɱ��������
	const WorldBossKillReward_Config* GetWorldBossKillReward(WORD wRewardId);

// ����
public:
	// ȡ��������
	const Card_Config* GetCard(DWORD dwInfoId);

	// ȡ������������
	const CardLevelUp_Config* GetCardLevelUp(BYTE byQuality, WORD wLevel);

	// ȡ����ͻ������
	const CardBreak_Config* GetCardBreak(BYTE byQuality, WORD wBreakLevel);

	// ȡ�ղ�������
	const CollectionBook_Config* GetCollectionBook(WORD wBookId);

	// ȡ�ղ�������
	const CollectionGroup_Config* GetCollectionGroup(WORD wGroupId);

	// ȡ�ͼ���������
	const vector<RateDataEx>* GetLowCardGroup(DWORD dwGroupId);

	// ȡ����ؿ�����
	const CardGroupMap_Config* GetCardGroupMap(WORD wMapIdx);

	// ȡ�߼���������
	const vector<HighCardGroup_Config>* GetHighCardGroup();

	// ȡռ���ɾ�����
	const ZhanBuAchievement_Config* GetZhanBuAchievement(WORD wLevel);

	// ȡ��������
	const CardSlot_Config* GetCardSlot(WORD wSlot);
public:


	// ��ȡ���������� 
	DWORD GetServerCfg(DWORD dwKey, DWORD dwDefault = 0);

	// ȡͨ���ַ���ֵ
	const char* GetServerCfg_Str(DWORD dwKey, const char *pszDefault);

	// ȡͨ������
	const Server_Config*  GetCommonCfg(DWORD dwKey, bool bOutLog = false);

	// ȡ��������Ϣ
	const ServerInfo_Config*  GetServerInfo(DWORD dwServerId);


	// ȡ��Ʒ����
	const Item_Config* GetItem(DWORD dwItemId);

	// ȡװ������
	const Equipment_Config* GetEquipment(DWORD dwEquipmentId);

	// ȡװ��ǿ���ȼ�����
	const EquipmentEnhanceLevel_Config* GetEquipmentEnhanceLevel(BYTE byQuality, WORD wStrengLevel);

	// ȡװ�������ȼ�����
	const EquipmentRefineLevel_Config* GetEquipmentRefineLevel(BYTE byQuality, WORD wRefineLevel);

	// ȡװ���ϳ�����
	const EquipmentCompose_Config* GetEquipmentCompose(DWORD dwEquipmentId);

	// ȡװ����װ�����б�
	const vector<EquipmentSuit_Config>* GetEquipmentSuitList() { return &m_EquipmentSuitList; };


	// ȡ��ɫ�ȼ�����
	const RoleLevel_Config* GetRoleLevel(WORD wLevel);

	// ȡ��ɫ���Իָ�����
	const RoleProRecover_Config* GetRoleProRecover(BYTE byProType);

	// �������
public:

	// ȡ���߸����½�����
	const FubenChapter_Config* GetMainFBChapter(WORD wChapter);

	// ȡ���߸����ؿ�����
	const FubenMap_Config* GetMainFubenMap(WORD wChapter, BYTE byMapIdx);

	// ȡӢ�۸����½�����(��ûʵ��)
	const FubenChapter_Config* GetHeroFBChapter(WORD wChapter);

	// ȡ�ճ������½�����
	const FubenChapter_Config* GetDailyChapter(WORD wChapter);

	// ȡ��Ӣ�����½�����
	const FubenChapter_Config* GetElitFBeChapter(WORD wChapter);

	// ȡ�ճ������˺�����
	const DailyFBDamageReward_Config* GetDailyFBDamageReward(WORD wChapter, WORD wMapIdx, WORD wDamageRate);

	// ȡ���߸����½ڽ���
	const MainFBChapterReward_Config* GetMainFBChapterReward(WORD wRewardLevel);

	// ȡ��ʱ�����б�
	const vector<LimitDrop_Config>* GetLimitDropList(DWORD dwDungeonType);

	// ����
public:

	// ȡ����֮������
	const HeartOfWorld_Config* GetHeartOfWorld(BYTE byStage);

	// ȡ��������
	const Spirit_Config* GetSpirit(DWORD dwSpiritId);

	// ȡ����ǿ������
	const SpiritEnhance_Config* GetSpiritEnhance(DWORD dwSpiritId, WORD wEnhanceLevel);

	// ȡ����ͼ������
	const SpiritPokedex_Config* GetSpiritPokedex(DWORD dwPokedexId, WORD wPokedexLevel);

	// ȡ����׶�����
	BYTE GetSpiritStageMax();

	// ȡ������������
	const SpriteTrain_Config* GetSpriteTrain(DWORD dwSpiritId, WORD wTrainLevel);

	// ȡ�����츳��������
	const SpriteTalentSkill_Config* GetSpriteTalentSkill(DWORD dwSpriteId, WORD wTalentSkillLevel);

	// ȡ������������
	const SpriteStarUp_Config* GetSpriteStarUp(WORD wStar);

	// ȡ������������
	const SpriteSpecialProperty_Config* GetSpriteSpecialProperty(DWORD dwSpriteId, WORD wStar);

	// ȡ������������
	const SpriteQualifications_Config* GetSpriteQualifications(WORD wQualifications);



	// ȡ����ģ������(��ûʵ��)
	//const SpiritModel_Config* GetSpiritModel(WORD wModelId);

public:

	// ȡ�̵���Ʒ
	const ShopGoodsData_Config* GetShopGoods(BYTE byType, WORD wGoodsId);

	// ȡ�ؿ�����
	const Map_Config* GetMap(DWORD dwMapId);

	// ȡս�ؿ�������
	const Battle_Config* GetMapBattle(DWORD dwBattleId);

	// ȡ��������
	const Monster_Config* GetMonster(DWORD dwMonsterId);

	// ȡװ��ǿ����������
	const EquipmentForge_Config* GetEquipementStrengthenForge(WORD wLevel);

	// ȡװ��������������
	const EquipmentForge_Config* GetEquipementRefineForge(WORD wLevel);

	// ȡװ���츳����
	const EquipmentTalent_Config* GetEquipmentTalent(WORD wTalentId, WORD wEnhanceLevel);

	// ȡװ����������
	const EquipmentStarUp_Config* GetEquipmentStartUp(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel);

	// ȡװ����Ʒ����
	const EquipmentQualityUp_Config* GetEquipmentQualityUp(WORD wQuality);

	// ȡԮ����������
	const ReinforcementEncourage_Config* GetReinforcementEncourage(WORD wReinforcementLevel);


	// ȡ����ǿ����������
	const TreasureForge_Config* GetTreasureStrengthenForge(WORD wLevel);

	// ȡ���ﾫ����������
	const TreasureForge_Config* GetTreasureRefineForge(WORD wLevel);

	// ȡ������������
	const TreasureSmelt_Config* GetTreasureSmelt(DWORD dwTreasureInfoId);

	// ȡ���շ�������
	const RecycleReturn_Config* GetRecycleReturn(BYTE byObjType, BYTE byObjLevel);

	// ȡ�ᱦ�ؿ�����
	const DuobaoMap_Config* GetDuobaoMap(WORD wRoleLevel);

	// ȡ�ᱦ��������
	const DuobaoDrop_Config* GetDuobaoDrop(BYTE byQuality);

	// ȡ�ᱦ���Ƚ�������
	const DuobaoPregrossReward_Config* GetDuobaoPregrossReward(WORD wPregrossId);

	// ȡ������Ʒ��������
	const TreasureQualityUPCost_Config* GetTreasureQualityUPCost(WORD wRefineLevel);

public:
	// ȡӢ����ͨ�ٻ�����
	const HeroNormalSummon_Config* GetHeroNormalSummon(DWORD dwRate);

	// ȡӢ�۸߼��ٻ�����
	const HeroAdvancedSummon_Config* GetHeroAdvancedSummon(DWORD dwRate);

	// ȡӢ�۸߼��ٻ�����
	const HeroAdvancedSummon_Config* GetHeroAdvancedSummonEx(DWORD dwRate);

	// ȡӢ����ͨ�ٻ��ܵļ���
	DWORD GetHeroNormalSummonTotalRate();

	// ȡӢ�۸߼��ٻ��ܵļ���
	DWORD GetHeroAdvancedSummonTotalRate();

	// ȡӢ�۸߼��ٻ��ܵļ���
	DWORD GetHeroAdvancedSummonTotalRateEx();

	// ȡӢ��VIP��ļ�ܼ���
	DWORD GetHeroVIPSummonTotalRate(WORD wCamp, bool isSpecial);

	// ȡӢ��VIP��ļ�ܼ���
	const HeroVipSummon_Config* GetHeroVIPSummon(WORD wCamp, bool isSpecial, DWORD dwRate);

	// ȡVip����
	const Vip_Config* GetVip(WORD wVipLevel);

	// ȡVIPGM����
	const VipGM_Config* GetVipGM(WORD wGMLevel);

	// ȡVIPGM�ȼ�
	WORD GetVipGMLevel(DWORD dwRechargeAmount);

	// ȡVip����Id
	DWORD GetVipNoticeId(WORD wVipLevel);

	// ȡVIP�ȼ�����
	WORD GetMaxVipLevel();

	// ȡ�ؿ������б�
	bool GetMapMonsterList(WORD wMapId, vector<MonsterData>* pMonsterList);

	// ȡϵͳ���ŵȼ�
	WORD GetSystemOpenLevel(DWORD dwSystemId);
public:

	// ȡ������Ŀ��������б�
	const ScoreMatchServerGroup_Config* GetScoreMatchTargetServerList(DWORD dwServerId);

	// ȡ������������
	const ScoreMatchRobot_Config* GetScoreMatchRobot(DWORD dwRobotId);

	// ȡ�������������б�
	const vector<ScoreMatchRobot_Config>& GetScoreMatchRobotList(){ return m_ScoreMatchRobotList; };

	// ��������ս��������
	const ScoreMatchBattleTask_Config* GetScoreMatchBattleTask(WORD wTaskId);

	// ȡ���������н���
	const ScoreMatchRankReward_Config* GetScoreMatchRankReward(WORD wRank);

	// ȡ�������ʱ��
	const ScoreMathActivityTime_Config* GetScoreMatchActivityTime(WORD wIndex);

	// ȡ��������������
	int GetScoreMatchTimesCost(WORD wType, WORD wTimes);

	// ȡ����������Ŀ������
	const ScoreMatchSearchTarget_Config* GetScoreMatchSearchTarget(WORD wType);

	// ������
public:
	// ȡ��������������
	const WarcraftRank_Config* GetWarcraftRank(WORD wRank);

	// �ƺ�
public:
	// ȡ�ƺ�����
	const Title_Config* GetTitle(int nTitleId);

	// �
public:
	// ȡ�����
	const vector<Activity_Config>* GetActivityTime(int nActivityId);
	const Activity_Config* GetActivityTime(int nActivityId, WORD wDataId);
	const LimitActivity_Config* GetLimitActivity(WORD wDataId);

	// ȡVIPÿ�����
	const VipDailyGift_Config* GetVipDailyGift(WORD wVipLevel);

	// ȡVIP�����
	const VipWeekGift_Config* GetVipWeekGift(WORD wGiftId);

	// ȡ��ɫ�ȼ�VIP������б�
	const VipWeekGiftRandList_Config* GetVipWeekGiftRandList(WORD wRoleLevel);

	// ȡ�����Ƽ���ֵ����
	const TodayRecommendRecharge_Config* GetTodayRecommendRecharge(WORD wGiftId);

	// ȡ�����Ƽ���ֵ����б�
	const vector<RateDataEx>* GetTodayRecommendRechargeRandList(WORD wVipLevel);

	// ȡ�ȼ��̵�����
	const LevelShopData_Config* GetLevelShopData(WORD wRoleLevel);

	// ȡ�ȼ��������
	const LevelGift_Config* GetLevelGiftData(WORD wRoleLevel);

	// ȡ����������
	const OpenServerFundReward_Config* GetOpenServerFundReward(WORD wRewardId);

	// ȡȫ��������
	const AllPeopleWelfare_Config* GetAllPeopleWelfare(WORD wWelfareId);

	// ȡ��ʱ�Ż���Ʒ����
	const LimitPreferentialShop_Config* GetRoleLimitPreferentialShop(WORD wRoleLevel);

	// ��ʱ�Żݳ�ֵ����
	const LimitPreferentialRecharge_Config* GetLimitPreferentialRecharge(WORD wRechargeId);

	// ��ʱ�Żݸ���
	const LimitPreferentialWelfare_Config* GetLimitPreferentialWelfare(WORD wWelfareId);

	// ȡ�ȼ��������
	const LevelGift_Config* GetLevelGift(WORD wLevel);

	// ȡ���߽�������
	const OnlineReward_Config* GetOnlineReward(WORD wDays);

	// ȡ7���¼����
	const SevenDayLogin_Config* GetSevenDayLogin(WORD wDay);

	// ȡŤ���齱�б�
	const vector<NDLottery_Config>* GetNDLotteryList(DWORD dwDataId);

	// ȡŤ���齱�б�
	const NDLotteryCost_Config* GetNDLotteryCost(DWORD dwDataId);

	// PVP
public:
	// ȡ��ս����
	const PVPBattleReward_Config* GetPVPBattleReward(WORD wType, WORD wBattleTimes);

	// ȡ��������
	const PVPRankReward_Config* GetPVPRankReward(WORD wRank);

	// PVP�����
	const PVPActivityTime_Config* GetPVPActivityTime(WORD wWeekDay);

	// ����
public:
	// ȡ�����½�����
	const AttackCityChapter_Config* GetAttackCityChapter(WORD wChapter);

	// ȡ����boss��������
	const AttackCityBossReward_Config* GetAttackCityBossReward(WORD wChapter);

	// ȡ��������CD
	WORD GetAttackCityWeakCD(DWORD dwHPRemainRate);

	// ȡ���ǹ�������
	const AttackCityEncourage_Config* GetAttackCityEncourage(WORD wTimes);

	// ȡ���ǻʱ������
	const AttackCityActivityTime_Config* GetAttackCityActivityTime(WORD wWeekDay);

	// ȡ����������������
	const AttackCityRankReward_Config* GetAttackCityRankReward(WORD wRank);

	// �޾�֮��
public:
	// ȡ������
	const EndlessLandWave_Config* GetEndlessLandWave(WORD wWave);

	// ȡ��ײ�����б�
	const vector<EndlessLandCollision_Config>& GetEndlessLandCollisionList();

	// ȡ��ɱ����
	const EndlessLandContinueKillReward_Config* GetEndlessLandKeepKillReward(DWORD dwContinueKillNum);

	// ȡ�޾�֮�����н���
	const EndlessLandRankReward_Config*  GetEndlessLandRankReward(WORD wRank);

	// ��ս��������
	const EndlessLandBattleCost_Config* GetEndlessLandBattleCost(WORD wBattleTimes);

public:
	// ȡ��������
	const Reward_Config* GetReward(DWORD dwRewardId);

	// ȡ����������
	const ActivationCode_Config* GetActivationCode(const char *pszCode);

	// ȡ�����뽱������
	const ActivationCodeReward_Config* GetActivationCodeReward(DWORD dwRewardId);

	// ȡ����ͳ������
	const ReyunStatistic_Config* GetReyunStatistic(WORD wType);


	// ���ջ
public:
	// ȡ���ջĿ������
	const FestivalActivtiyTarget_Config* GetFestivalActivityTarget(DWORD dwTargetId);

	// ȡ���ջ����
	const vector<FestivalActivity_Config>* GetFestivalActivity(WORD wActivityType);

	// ȡ���ջ��ԾĿ���б�
	const vector<WORD>* GetFestivalActivitySameLogicTargetList(WORD wLogicType);

	// ��λ
public:
	// ȡ��λ����
	const Nobility_Config* GetBobility(WORD wNobilityLevel);

	// ��ʯ
public:
	// ȡ��ʯ����
	const Gem_Config* GetGem(DWORD dwGemId);

	// ȡ��ʯ����
	const GemSkill_Config* GetGemSkill(WORD wSkillId, WORD wSkillLevel);

	// ȡ��ʯ��λ
	const GemHole_Config* GetGemHole(WORD wHole);

	// ȡ��ʯ��Ƕ����
	const GemInlay_Config* GetGemInlay(WORD wType);

	// ��Ӹ���
public:
	const vector<TeamDungeonLottery_Config>& GetTeamDungeonLotteryList();

	// ȡ��Ӹ�������
	const TeamDungeon_Config* GetTeamDungeon(DWORD dwDungeonId);

	// ȡ��������
	const TeamDungeonTask_Config* GetTeamDungeonTask(DWORD dwTaskId);

	// ȡ����������
	const TeamDungeonTaskLink_Config* GetTeamDungeonTaskLink(DWORD dwTaskLinkId);

	// ȡBOSS��������
	const TeamDungeonBossReward_Config* GetTeamDungeonBossReward(WORD wDungeonId, int nEnergy, int nDamageRate);

	// ������
public:
	// ȡ��λ����
	const KingMatchStage_Config* GetKingMatchStage(WORD wStageLevel);

	// ȡ����������ʱ������
	const KingMatchSeasonTime_Config* GetKingMatchSeasonTime(WORD wSeason);

	// ȡ����������������
	const KingMatchSeasonTask_Config* GetKingMatchSeasonTask(WORD wWinTimes);

	// ȡ�������������н�������
	const KingMatchRankReward_Config* GetKingMatchRankReward(DWORD dwRank);

	// ʥ����
public:
	// ȡʥ����ѩ������
	const ChristmasSnowball_Config* GetChristmasSnowball(DWORD dwSnowballId);

	// ȡʥ����BOSS����
	const ChristmasBoss_Config* GetChristmasBoss(WORD wBossType);

	// ȡʥ����BOSS�ȼ�����
	const ChristmasBossLevel_Config* GetChristmasBossLevel(WORD wBossType, DWORD dwBossLevel);

	// facebook�
public:
	// ȡfacebook������������
	const FacebookFriendInvite_Config* GetFacebookFriendInvite(WORD wInviteNum);

	// ȡfacebook�����ٻ�����
	const FacebookFriendSummon_Config* GetFacebookFriendSummon(WORD wSummonNum);

	// ȡfacebook�ʺŰ�����
	const FacebookAccountBinding_Config* GetFacebookAccountBinding(DWORD dwRewardId);

	// ȡfacebook��������
	const FacebookShare_Config* GetFacebookShare(WORD wChareChapter);

	// ȡfacebook��������
	const FacebookCommend_Config* GetFacebookCommend(WORD wChapter);

	// ��ֵ����
public:
	// ȡ��ֵ��������
	const RechargeReward_Config* GetRechargeReward(WORD wActivityDataId, DWORD dwDay);

	// ȡ��ֵ������������
	const RechargeExtraReward_Config* GetRechargeExtraReward(WORD wActivityDataId);

	const AppleIap_Config* GetAppleIap(const char *pIapId);

	// �۳佱��
public:

	// ȡ�۳佱������
	const TotalRechargeReward_Config* GetTotalRechargeReward(WORD wDataId, WORD wRewardId);

	// ������Ʒ
public:
	// ȡ������Ʒ����
	const HotSaleGoods_Config* GetHotSaleGoods(DWORD dwDataId, WORD wGoodsId);

	// ת��
public:
	// ȡת������
	const Turntable_Config* GetTurntableData(WORD wDataId);

	// ȡת�̳齱����
	const vector<TurntableLottery_Config>* GetTurntableLottery(WORD wDataId);

	// ȡת�����а�����
	const TurntableRank_Config* GetTurntableRank(WORD wDataId, WORD wRank);

// ��ʱ����
public:

	// ȡ��ʱ��������
	const LimitProduct_Config* GetLimitProduct(WORD wSystemId);

// ��Ӫ��ļ
public:

	// ȡ��Ӫ��ļ����
	const CampRecruit_Config* GetCampRecruit(WORD wDataId, WORD wCamp);

	// ȡ��Ӫ��ļӢ�������б�
	const vector<CampRecruitHero_Config>* GetCampRecruitHeroList(WORD wDataId, WORD wCamp);

// unlock�
public:

	// ȡunlock�����
	const UnlockActivityReward_Config* GetUnlockActivityReward(WORD wRewardId);

	// ȡunlock�ۿ���Ƶ����
	const UnlockWatchingVideoReward_Config* GetUnlockWatchingVideoReward(WORD wWatchingTimes);

// 
public:
	// ȡ����ȼ���������
	const WorldTaskLevel_Config* GetWorldTaskLevelData(WORD wOpenServerDays);

	// ȡ����ȼ�����ID�б�
	const WorldLevelTaskIdList_Config* GetWorldLevelTaskIdList(WORD wTaskLevel);

	// ȡ����ȼ�����ID�б�
	const WorldLevelTaskIdList_Config* GetWorldLevelTaskIdListByTaskId(WORD wTaskId);

	// ȡ����ȼ������¼�
	const WorldLevelTaskEvent_Config* GetWorldLevelTaskEvent(WORD wTaskId);

// ��Դս
public:
	// ȡ��Դս�½�����
	const ResourceWarChapter_Config* GetResourceWarChapter(WORD wChapter);

	// ȡ��Դս��Դ����
	const ResourceWarResource_Config* GetResourceWarResource(WORD wResourceId);

	// ȡ��Դս�Ƽ�����
	const ResourceWarTechnology_Config* GetResourceWarTecknology(WORD wTechnologyLevel);

	// ȡ��Դս������������
	const ResourceWarGuildProfit_Config* GetResourceWarGuildProfit(WORD wMemberNum);

// ����ս
public:

	// ȡ����ս��������
	const GuildWarStar_Config* GetGuildWarStar(DWORD dwStarId);

	// ȡ����ս������
	const GuildWarTower_Config* GetGuildWarTower(DWORD dwTowerId);

	// ����ս���н���
	const GuildWarRankReward_Config* GetGuildWarRankReward(WORD wRank);

	// ����ս��ս����
	const GuildWarVSReward_Config* GetGuildWarVSReward(WORD wVSType, WORD wVSResult);

	// ����ս��ս�ʱ��
	const vector<GuildWarVSActivityTime_Config>* GetGuildWarVSActivityTime(WORD wTimeType);

	// ����սͨ������
	const Server_Config* GetGuildWarCommon(DWORD dwKey);

	// ȡ��������
	const GuildWarOrganizeDefense_Config* GetGuildWarOrganizeDefense(WORD wTowerType);

	// ȡ����ս��������
	const GuildWarRegion_Config* GetGuildWarRegion(WORD wRegionId);

	// ȡ����ս��������
	const GuildWarRegion_Config* GetGuildWarRegionByServerId(WORD wServerId);

	// ygame�������
public:

	// ȡygame֧������
	const YGamePayment_Config* GetYGamePayment(string strPaymentId);

	// ȡygame֧������
	const YGamePayment_Config* GetYGamePayment(DWORD dwAssistId);

public:
	const RechargeRankReward_Config* GetRechargeAndCostRankReward(WORD wRankType, WORD wRank);

	// �һ���� m_vHangUpDropList
public:
	const vector<HangUpDrop_Config>* GetHangUpDropList() { return &m_vHangUpDropList; };


public:

	// ����Ƿ��ʺŰ�����
	bool IsAccountWhite(const char *pszAccount);

	// ����Ƿ�IP������
	bool IsIPWhite(const char *pszIP);

protected:
	// ע����ػص�����
	bool RegisterCallbackFunc(const LoadConfigCallbackData &CallbackData);
	bool RegisterCallbackFunc(string file, LoadCallbackFunc func);

	// �������������ļ�
	bool LoadAllFile();

	// ע�ἤ�����ļ�(todo::Ŀǰ�Ѿ���ʹ��)
	void RegisterActivationCodeFile();

public:
	vector<LoadConfigCallbackData> m_CallbackDataList;	// �ص������б�
	map<string, string> m_CallbackFuncFindList;			// �ص����������б�

	// �����
	map<DWORD, Server_Config> m_CommonList;								// ͨ�������б�
	map<DWORD, ServerInfo_Config> m_ServerInfoList;						// ͨ�������б�


	// �����ļ��б�
	map<DWORD, Monster_Config> m_MonsterList;							// �����б�
	map<DWORD, Battle_Config> m_BattleList;								// ս���б�
	map<DWORD, Map_Config> m_MapList;									// �ؿ��б�
	map<DWORD, FubenChapter_Config> m_MainFBChapterList;				// ���߸����½��б�
	map<DWORD, HeroToBattle_Config> m_HeroToBattleList;					// Ӣ�����������б�
	map<DWORD, FubenChapter_Config> m_DailyChapterList;					// �ճ������½��б�
	map<DWORD, FubenChapter_Config> m_HeroFBChapterList;				// Ӣ�۸����½��б�
	map<DWORD, FubenChapter_Config> m_EliteFBChapterList;				// ��Ӣ�����½��б�
	map<DWORD, DailyFBDamageReward_Config> m_DailyFBDamageReward;		// �ճ������˺�����
	map<DWORD, MainFBChapterReward_Config> m_MainFBChapterRewardList;	// ���߸����½ڽ����б�
	map<DWORD, vector<LimitDrop_Config>> m_vLimitDropList;				// ��ʱ�����б�
	

	map<DWORD, Item_Config> m_ItemList;									// ��Ʒ����
	map<WORD,  RoleLevel_Config> m_RoleLevelList;						// ��ɫ�ȼ�����
	map<DWORD, RoleProRecover_Config> m_RoleProRecoverList;				// ��ɫ���Իָ�����
	map<DWORD, RoleInit_Config> m_RoleInitList;							// ��ɫ��ʼ���б�
	vector<TimeEvent_Config> m_vTimeEventList;							// ʱ���¼��б�

	// Ӣ��
	map<DWORD, Hero_Config> m_HeroList;									// Ӣ������
	map<DWORD, HeroLevel_Config> m_HeroLevelList;						// Ӣ�۵ȼ�����
	map<DWORD, HeroPro_Config> m_HeroProList;							// Ӣ����������
	map<DWORD, HeroBreakLevelUp_Config> m_BreakLevelUpList;				// Ӣ��ͻ�����������б�
	map<DWORD, HeroCompose_Config> m_HeroComposeList;					// Ӣ�ۺϳ������б�
	map<DWORD, vector<HeroTrain_Config>> m_HeroTrainList;						// Ӣ�������б�
	vector<HeroTrainRandPro_Config> m_HeroTrainRandProList;				// Ӣ��������������б�
	map<DWORD, HeroTrainCost_Config> m_HeroTrainCostList;				// Ӣ�����������б�
	vector<HeroNormalSummon_Config> m_NormalSummonList;					// ��ͨ�ٻ��б�
	vector<HeroAdvancedSummon_Config> m_AdvancedSummonList;				// �߼��ٻ��б�
	map<DWORD, vector<HeroVipSummon_Config>> m_HeroVipSummonList;		// Ӣ��VIP�ٻ��б�
//	vector<HeroSoulShop_Config> m_HeroSoulShopList;						// �ۻ��̵��б�
	map<DWORD, HeroDestiny_Config> m_HeroDestinyList;					// Ӣ�����������б�
	map<DWORD, HeroAwaken_Config> m_HeroAwakenList;						// Ӣ�۾��������б�
	vector<RefreshShopSlot_Config> m_HeroShopSlotList;					// Ӣ���̵��λ�����б�
	vector<RefreshShopSlot_Config> m_AwakenShopSlotList;				// �����̵��λ�����б�
	vector<RefreshShopSlot_Config> m_CrystalShopSlotList;				// ˮ���̵��λ�����б�
	map<DWORD, HeroBreak_Config> m_HeroBreakList;						// Ӣ��ͻ������
	map<DWORD, vector<DWORD>> m_HeroPokedexList;						// Ӣ��ͼ���б�
	vector<RefreshShopSlot_Config> m_DirectBuyShopSlotList;				// ֱ���̵��λ�����б�
	map<DWORD, HeroEvolution_Config> m_HeroEvoluctionList;				// Ӣ�۽��������б�
	map<DWORD, int> m_HeroQualificationsList;							// Ӣ�����������б�

	// װ��
	map<DWORD, Equipment_Config> m_EquipmentList;							// װ�������б�
	map<DWORD, EquipmentCompose_Config> m_EquipmentComposeList;				// װ���ϳ������б�
	map<DWORD, EquipmentEnhanceLevel_Config> m_EquipmentEnhanceList;		// װ��ǿ�������б�
	map<DWORD, EquipmentRefineLevel_Config> m_EquipmentRefineList;			// װ�����������б�
	map<DWORD, ShopGoodsData_Config> m_ShopGoodshList;						// �̵���Ʒ�б�
	vector<EquipmentForge_Config> m_EquipmentEnhanceForgeList;				// װ��ǿ�����������б�
	vector<EquipmentForge_Config> m_EquipmentRefineForgeList;				// װ���������������б�
	vector<EquipmentSuit_Config> m_EquipmentSuitList;						// װ����װ�����б�
	map<WORD, vector<EquipmentTalent_Config>> m_EuipmentTalentList;			// װ���츳�б�
	map<DWORD, EquipmentStarUp_Config> m_EuipmentStarUpList;				// װ�����������б�
	map<DWORD, EquipmentQualityUp_Config> m_EuipmentQualityUpList;			// װ����Ʒ�����б�
	map<DWORD, ReinforcementEncourage_Config> m_ReinforcementEncourageList;	// Ԯ�����������б�
	
	

	// �������
	map<DWORD, Treasure_Config> m_TreasureList;							// ���������б�
	map<DWORD, TreasureEnhance_Config> m_TreasureEnhanceList;		// ����ǿ�������б�
	map<DWORD, TreasureRefine_Config> m_TreasureRefineList;				// ���ﾫ�������б�
	map<DWORD, TreasureCompose_Config> m_TreasureComposeList;			// ����ϳ������б�
	vector<TreasureForge_Config> m_TreasureEnhanceForgeList;			// ����ǿ�����������б�
	vector<TreasureForge_Config> m_TreasureRefineForgeList;				// ���ﾫ�����������б�
	map<DWORD, TreasureSmelt_Config> m_TreasureSmeltList;				// �������������б�
	map<DWORD, TreasureQualityUPCost_Config> m_TreasureQualityUPCostList;// ������Ʒ���������б�

	// �Ѿ�
	map<DWORD, RebelExploit_Config> m_RebelExpoitList;					// �Ѿ���ѫ�����б�
	map<DWORD, RebelBattle_Config> m_RebelBattleList;					// �Ѿ�ս�������б�
	map<DWORD, RebelQuality_Config> m_RebelQualityList;					// �Ѿ�Ʒ�������б�
	map<DWORD, RoleRebel_Config> m_RoleRebelList;						// ��ɫ�Ѿ������б�
	map<DWORD, RebelRankAward_Config> m_RebelRankAwardList;				// �Ѿ����а��������б�

	// ����
	map<DWORD, RecycleReturn_Config> m_RecycleReturnList;				// ���շ��������б�

	// �ᱦ
	map<DWORD, DuobaoMap_Config> m_DuobaoMapList;						// �ᱦ�ؿ������б�
	map<DWORD, DuobaoDrop_Config> m_DuobaoDropList;						// �ᱦ���������б�
	map<DWORD, DuobaoPregrossReward_Config> m_DuobaoPregrossRewardList;	// �ᱦ���Ƚ��������б�
	

	// ����
	map<DWORD, TowerMap_Config> m_TowerMapList;							// ������ͨ�ؿ�����
	map<DWORD, TowerChapter_Config> m_TowerChapterList;					// ������ͨ�½�����
	map<DWORD, TowerEliteMap_Config> m_TowerEliteMapList;				// ������Ӣ�ؿ�����
	map<DWORD, TowerRandPro_Config> m_TowerRandProList;					// ���������������
	map<DWORD, TowerStarGoods_Config> m_TowerStarGoodsList;				// ����������Ʒ����

	// ���
	map<DWORD, LandCity_Config> m_LandCityList;							// ��سǳ������б�
	map<DWORD, LandPatrolEvent_Config> m_LandPatrolEventList;			// ���Ѳ���¼������б�
	map<DWORD, LandPatrol_Config> m_LandPatrolList;						// ���Ѳ�������б�
	
	// ����
	map<DWORD, HeartOfWorld_Config> m_HeartOfWorldList;					// ����֮�������б�
	map<DWORD, Spirit_Config> m_SpiritList;								// ���������б�
	map<DWORD, SpiritEnhance_Config> m_SpiritEnhanceList;				// ����ǿ�������б�
	map<DWORD, SpiritPokedex_Config> m_SpiritPokedexList;				// ����ͼ���б�
	map<DWORD, SpriteTrain_Config> m_SpiritTrainList;					// �������������б�
	map<DWORD, SpriteTalentSkill_Config> m_SpriteTalentSkillList;		// �����츳���������б�
	map<DWORD, SpriteStarUp_Config> m_SpriteStarUpList;					// �������������б�	
	map<DWORD, SpriteSpecialProperty_Config> m_SpriteSpecialPropertyList;	// �������������б�	
	map<DWORD, SpriteQualifications_Config> m_SpriteQualificationsList;	// ���������б�	


	// ������
	vector<ArenaRobot_Config> m_ArenaRobotList;							// �������������б�
	map<DWORD, ArenaRank_Config> m_ArenaRankList;						// ���������������б�
	map<DWORD, ArenaAward_Config> m_ArenaAwardList;						// ���������������б�
	map<DWORD, int> m_ArenaFirstRankRewardList;							// �������״����������б�
	vector<DWORD> m_ArenaShopGoodsList;									// �������̵���Ʒ�б�(�������ֵ�,��Ϊ�����ʾʹ��)

	// �ɾ�
	map<DWORD, Achievement_Config> m_AchievementList;					// �ɾ������б�

	// �ճ�����
	map<DWORD, DailyMission_Config> m_DailyMissionList;					// �ճ����������б�
	map<DWORD, vector<WORD>> m_DailyMissionTypeList;					// �ճ��������������б�
	map<WORD, RoleLvDailyMission_Config> m_RoleLvDailyMissionList;		// ��ɫ�ȼ���Ӧ���ճ�����
	

	// ����
	map<DWORD, Guild_Config> m_GuildList;								// ��������
	map<WORD, WORD> m_GuildIconList;									// ����ͼ������
	map<DWORD, GuildQY_Config> m_GuildQYList;							// ����Ȫӿ����
	map<DWORD, GuildQSAward_Config> m_GuildQSRewardList;				// ����Ȫˮ��������
	map<DWORD, GuildPrivilege_Config> m_GuildPrivilegeList;				// ����Ȩ������
	vector<RateDataEx> m_GuildLimitGoodsRateList;						// ������ʱ��Ʒ�����б�
	map<DWORD, GuildSkill_Config> m_GuildSkillList;						// ���Ἴ�������б�
	map<WORD, GuildFBChapter_Config> m_GuildFBChapterList;				// ���ḱ���½������б�
	vector<DWORD> m_GuildRewardGoodsIdList;								// ���ά����ƷID�б�
	vector<RefreshShopSlot_Config> m_GuildLimitShopSlotList;			// ������ʱ�̵��λ�����б�
	map<WORD, GuildBuilding_Config> m_GuildBuildingList;				// ���Ὠ������
	

	// Vip
	map<WORD, Vip_Config> m_VipList;									// Vip����
	map<WORD, DWORD> m_VipNoticeList;									// Vip��������
	map<WORD, VipGM_Config> m_VipGMList;								// VipGM����

	// ǩ��
	map<WORD, SignIn_Config> m_SignInList;							// ǩ������
	map<WORD, map<WORD, LuxurySign_Config>> m_LuxurySignInList;		// ����ǩ���б�[sign_in_id, role_level, data]
	map<DWORD, LuxurySignInReward_Config> m_LuxurySignInRewardList;	// ����ǩ�������б�
	
	

	// ӭ����
	map<WORD, YCS_Config> m_YCSList;									// ӭ��������

	// ͭȸ̨
	vector<TQT_Config> m_TQTList;										// ͭȸ̨����

	// ����BOSS
	map<DWORD, WorldBossOpenTime_Config> m_WorldBossOpenTimeList;		// ����BOSS����ʱ�����ñ�
	map<DWORD, WorldBoss_Config> m_WorldBossList;						// ����BOSS�����б�
	map<DWORD, WorldBossRankReward_Config> m_WorldBossRankRewardList;	// ����BOSS���н��������б�
	map<DWORD, WorldBossHonorReward_Config> m_WorldBossHonorRewardList;	// ����BOSS�������������б�
	map<DWORD, WorldBossKillReward_Config> m_WorldBossDamageRewardList;	// ����BOSS�˺����������б�
	//map<DWORD, WorldBossKillReward_Config> m_WorldBossDamageRewardList;	// ����BOSSӰ��ˢ��ʱ�������б�

	// 7��ϵͳ
	map<DWORD, SevenDayTarget_Config> m_SevenDayTargetList;			// 7��Ŀ�������б�
	map<WORD, vector<DWORD>> m_SevenDayActiviyList;					// 7���б�
	map<WORD, vector<WORD>> m_TheSameTargetList;					// ͬ��Ŀ���б�
	map<WORD, SevenDayCompletionReward_Config> m_SevenDayCompletionRewardList;	// 7����ɶȽ����б�
	vector<WORD> m_vSevenDayShopGoodsList;							// 7���̵���Ʒ�б�


	// ռ��
	map<DWORD, Card_Config> m_CardList;							// ��������
	map<DWORD, CardLevelUp_Config> m_CardLevelUpList;			// ������������  
	map<DWORD, CardBreak_Config> m_CardBreakList;				// ����ͻ������  
	vector<RefreshShopSlot_Config> m_StarSoulShopSlotList;		// �����̵��λ�����б�
	map<DWORD, vector<RateDataEx>> m_LowCardGroupList;			// �ͼ������б� group_id, card_id_list
	map<DWORD, CollectionBook_Config> m_CollectionBookList;		// �ղ�������
	map<DWORD, CollectionGroup_Config> m_CollectionGroupList;	// �ղ�������
	vector<HighCardGroup_Config> m_HighCardGroupList;			// �߼���������
	map<DWORD, CardGroupMap_Config> m_CardGroupMapList;			// ����ؿ�����
	map<WORD, ZhanBuAchievement_Config> m_ZhanBuAchievementList;// ռ���ɾ�����
	map<WORD, CardSlot_Config> m_CardSlotList;// ��������
	
	
	// ������
	map<DWORD, ScoreMatchServerGroup_Config> m_ScoreMatchTargetServerList;		// ������Ŀ��������б�
	vector<ScoreMatchRobot_Config> m_ScoreMatchRobotList;						// �������������б�
	map<DWORD, ScoreMatchBattleTask_Config> m_ScoreMatchBattleTaskList;			// ��������ս�����б�
	map<DWORD, ScoreMatchRankReward_Config> m_ScoreMatchRankRewardList;			// ���������н����б�
	map<DWORD, ScoreMathActivityTime_Config> m_ScoreMatchActivityTimeList;		// �������ʱ���б�
	map<DWORD, int> m_ScoreMatchTimesCostList;									// ���������������б�
	map<DWORD, ScoreMatchSearchTarget_Config> m_ScoreMatchSearchList;			// ����������Ŀ���б�
	

	// �ƺ�
	map<DWORD, Title_Config> m_TitleList;	// �ƺ������б�

	// ϵͳ����
	map<DWORD, WORD> m_SystemOpenList;	// ϵͳ���������б�

	// ����
	map<DWORD, Reward_Config> m_RewardList;	// �����б�

	// �
	map<DWORD, LimitActivity_Config> m_LimitActivityList;	// ��ʱ������б�
	map<DWORD, vector<Activity_Config>> m_ActivityTimeList;	// ������б�
	

	// VIP���
	map<DWORD, VipDailyGift_Config> m_VipDailyGiftList;	// VIPÿ������б�
	map<DWORD, VipWeekGift_Config> m_VipWeekGiftList;	// VIP����������б�
	map<DWORD, VipWeekGiftRandList_Config> m_VipWeekGiftRandList;	// VIPÿ����������
	map<WORD, TodayRecommendRecharge_Config> m_TodayRecommendRechargeList;	// �����Ƽ���ֵ�б�
	map<WORD, vector<RateDataEx>> m_TodayRecommendRechargeRandList;	// �����Ƽ���ֵ����б�

	// �ȼ��̵�
	map<DWORD, LevelShopData_Config> m_LevelShopDataList;

	// �ȼ����
	map<DWORD, LevelGift_Config> m_LevelGiftList;

	// ��������
	map<DWORD, OpenServerFundReward_Config> m_OpenServerFundRewardList;
	map<DWORD, AllPeopleWelfare_Config> m_AllPeopleWelfareList;

	// ��ֵ
	map<DWORD, Recharge_Config> m_RechargeList;		// ��ֵ�����б�
	map<DWORD, MonthCard_Config> m_MonthCardList;	// �¿������б�
	map<DWORD, Recharge_Config> m_MyCardRechargeList;	// mycard��ֵ�����б�

	// ��ʱ�Ż�
	map<DWORD, LimitPreferentialShop_Config> m_LimitPreferentialShop;			// ��ʱ�Ż��̵�
	map<DWORD, LimitPreferentialRecharge_Config> m_LimitPreferentialRecharge;	// ��ʱ�Żݳ�ֵ
	map<DWORD, LimitPreferentialWelfare_Config> m_LimitPreferentialWelfare;		// ��ʱ�Żݸ���

	map<string, ActivationCode_Config> m_ActivationCodeList; // ����������(todo::Ŀǰ�Ѿ���ʹ��)
	map<DWORD, ActivationCodeReward_Config> m_ActivationCodeRewardList; // �����뽱������
	map<WORD, ReyunStatistic_Config> m_ReyunStatisticList;	// ����ͳ�������б�

	// PVP
	map<DWORD, PVPBattleReward_Config> m_PVPBattleRewardList;	// PVPս�������б�
	map<DWORD, PVPRankReward_Config> m_PVPRankRewardList;	// PVP���������б�
	map<DWORD, PVPActivityTime_Config> m_PVPActivityTimeList;	// PVP�ʱ���б�

	// ���﹥��
	map<WORD, AttackCityChapter_Config> m_AttackCityChapterList;	// �����½��б� 
	map<WORD, AttackCityBossReward_Config> m_AttackCityBossRewardList;	// ����BOSS�����б� 
	vector<AttackCityWeak_Config> m_AttackCityWeakList; // ������������
	map<WORD, AttackCityEncourage_Config> m_AttackCityEncourageList;	// ���ǹ��������б� 
	map<WORD, AttackCityActivityTime_Config> m_AttackCityActivityTimeList; // ���ǻʱ�������б�
	map<WORD, AttackCityRankReward_Config> m_AttackCityRankRewardList; // �����������������б�
	
	// �޾�֮��
	vector<EndlessLandCollision_Config> m_EndlessLandCollisionList; // �޾�֮����ײ�����б�
	map<WORD, EndlessLandWave_Config> m_EndlessLandWaveList;	// �޾�֮�ز������б�
	map<DWORD, EndlessLandContinueKillReward_Config> m_EndlessLandContinueKillRewardList; // �޾�֮����ɱ�����б�
	map<WORD, EndlessLandRankReward_Config> m_EndlessLandRankRewardList; // �޾�֮�����н����б�
	map<WORD, EndlessLandBattleCost_Config> m_EndlessLandBattleCostList; // �޾�֮����ս�����б�

	

	// ��λ
	map<WORD, Nobility_Config> m_NobilityList;	// ��λ�����б�

	// ��ֵ�ۿ�
	map<WORD, SuperDiscountBuyReward_Config> m_SuperDiscountBuyRewardList;
	map<WORD, SuperDiscountAccumulateReward_Config> m_SuperDiscountAccumulateRewardList;

	// ��ֵ�ۿ�(��)
	map<DWORD, SuperDiscountBuyReward_Config> m_NewSuperDiscountBuyRewardList;
	map<DWORD, SuperDiscountAccumulateReward_Config> m_NewSuperDiscountAccumulateRewardList;

	// 7�����л����
	map<DWORD, RoleRankReward_Config> m_SevenDayRankRewardList;
	
	// ���ջ
	map<DWORD, vector<FestivalActivity_Config>> m_FestivalActivityList;			// ���ջ�б�
	map<DWORD, FestivalActivtiyTarget_Config> m_FestivalActivtiyTargetList;		// ���ջĿ���б�
	map<WORD, vector<WORD>> m_FestivalActivitySameLogicTargetList;				// ���ջͬ����߼�Ŀ���б�

	map<DWORD, AccurateRecommendGoods_Config> m_AccurateRecommendGoodsList;		// ��׼�Ƽ���Ʒ�б�

	// ��ʯ
	map<DWORD, Gem_Config> m_GemList; // ��ʯ�б�
	map<DWORD, GemSkill_Config> m_GemSkillList; // ��ʯ�����б�
	map<DWORD, GemHole_Config> m_GemHoleList; // ��ʯ��λ�б�
	map<DWORD, GemInlay_Config> m_GemInlayList; // ��ʯ��Ƕ�б�

	// ��Ӹ���
	vector<TeamDungeonLottery_Config> m_TeamDungeonLotteryList;				// �齱�б�
	map<DWORD, TeamDungeon_Config> m_TeamDungeonList;						// ��Ӹ����б�
	map<DWORD, TeamDungeonTaskLink_Config> m_TeamDungeonTaskLinkList;		// ��Ӹ����������б�
	map<DWORD, TeamDungeonTask_Config> m_TeamDungeonTaskList;				// ��Ӹ��������б�
	map<DWORD, vector<TeamDungeonBossReward_Config>> m_TeamDungeonBossRewardList;	// ��Ӹ���Boss�����б�

	// ����
	map<DWORD, Scene_Config> m_SceneList; // �����б�

	map<WORD, LoginReward_Config> m_LoginRewardList; // ��¼���������б�

	map<string, ClosedTestRebate_Config> m_ClosedTestRebateList; // ����ֵ�����б�

	map<string, string> m_AccountWhiteList; // �ʺŰ������б�
	map<string, string> m_IPWhiteList;		// IP�������б�

	// ������
	map<WORD, KingMatchStage_Config> m_KingMatchStageList; // ��������λ�����б�
	map<WORD, KingMatchSeasonTask_Config> m_KingMatchSeasonTaskList; // �����������������б�
	vector<KingMatchRankReward_Config> m_KingMatchRankRewardList; // ���������н��������б�
	map<WORD, KingMatchSeasonTime_Config> m_KingMatchSeasonTimeList; // ��������ʱ�������б�

	// ʥ����
	map<DWORD, ChristmasBossLevel_Config> m_ChristmasBossLevelList;	// ʥ����Boss�ȼ������б�
	map<DWORD, ChristmasBoss_Config> m_ChristmasBossList;			// ʥ����Boss�����б�
	map<DWORD, ChristmasSnowball_Config> m_ChristmasSnowballList;	// ʥ����ѩ�������б�

	// facebook�
	map<DWORD, FacebookFriendInvite_Config> m_FacebookFriendInvitList;		// facebook���������б�
	map<DWORD, FacebookFriendSummon_Config> m_FacebookFriendSummonList;		// facebook�����ٻ��б�
	map<DWORD, FacebookAccountBinding_Config> m_FacebookAccountBindingList;	// facebook�˺Ű��б�
	map<DWORD, FacebookShare_Config> m_FacebookChareList;					// facebook�����б�
	map<DWORD, FacebookCommend_Config> m_FacebookCommendList;				// facebook�����б�

	// ���߽���
	map<DWORD, OnlineReward_Config> m_OnlineRewardList;	// ���߽����б�

	// 7���¼
	map<DWORD, SevenDayLogin_Config> m_SevenDayLoginList;	// 7���¼�����б�

	// ��ֵ����
	map<DWORD, RechargeReward_Config> m_RechargeRewardList;				// ��ֵ���������б�
	map<DWORD, RechargeExtraReward_Config> m_RechargeExtraRewardList;	// ��ֵ�������⽱�������б�
	map<string, AppleIap_Config> m_AppleIAPList;						// AppleIap��Ϣ�б�
	

	// �۳佱��
	map<DWORD, TotalRechargeReward_Config> m_TotalRechargeRewardList;	// �۳佱���б�

	// Ť���齱
	map<DWORD, vector<NDLottery_Config>> m_NDLotteryList;				// Ť���齱�б�
	map<DWORD, NDLotteryCost_Config> m_NDLotteryCostList;				// Ť���齱�����б�

	// ������Ʒ
	map<DWORD, HotSaleGoods_Config> m_HotSaleGoodsList;	// ������Ʒ�б�

	// ת��
	map<DWORD, Turntable_Config> m_TurntableList;		// ת���б�
	map<DWORD, TurntableRank_Config> m_TurntableRankList;	// ת�����а��б�
	map<DWORD, vector<TurntableLottery_Config>> m_TurntableLotteryList;	// ת�̳齱�б�

	// ��ʱ����
	map<DWORD, LimitProduct_Config> m_LimitProductList;	// ��ʱ�����б�

	// ��Ӫ��ļ
	map<DWORD, CampRecruit_Config> m_CampRecruitList;			// ��Ӫ��ļ�����б� data_id + camp, data_list
	map<DWORD, vector<CampRecruitHero_Config>> m_CampRecruitHeroList;	// ��Ӫ��ļӢ���б� data_id + camp, data_list

	// unlock�
	map<DWORD, UnlockActivityReward_Config> m_UnlockActivityRewardList;
	map<DWORD, UnlockWatchingVideoReward_Config> m_UnlockWathingVideoRewardList;
	
	// ����ȼ�����
	map<DWORD, WorldTaskLevel_Config> m_WorldTaskLevelList;				// ��������ȼ��б�
	map<DWORD, WorldLevelTaskIdList_Config> m_WorldLevelTaskIdList;		// ����ȼ�����ID�б�[TaskLevel,List]
	map<DWORD, WorldLevelTaskIdList_Config> m_WorldLevelTaskIdListEx;	// ����ȼ�����ID�б�[TaskId, List]
	map<DWORD, WorldLevelTaskEvent_Config> m_WorldLevelTaskEventList;	// ����ȼ������¼��б�

	// ��Դս����
	map<DWORD, ResourceWarChapter_Config> m_ResourceWarChapterList;			// ��Դս�½������б�
	map<DWORD, ResourceWarResource_Config> m_ResourceWarResourceList;		// ��Դս��Դ�����б�
	map<WORD, ResourceWarTechnology_Config> m_ResourceWarTechnologyList;	// ��Դս�Ƽ������б�
	map<WORD, ResourceWarGuildProfit_Config> m_ResourceWarGuildProfitList;	// ��Դս�������������б�

	// ����ս
	map<DWORD, GuildWarStar_Config> m_GuildWarStarList;								// ����ս��������
	map<DWORD, GuildWarTower_Config> m_GuildWarTowerList;							// ����ս������
	map<DWORD, GuildWarRankReward_Config> m_GuildWarRankRewardList;					// ����ս���н���
	map<DWORD, GuildWarVSReward_Config> m_GuildWarVSRewardList;						// ����ս��ս����
	map<DWORD, vector<GuildWarVSActivityTime_Config>> m_GuildWarVSActivityTimeList;	// ����ս��ս�ʱ������
	map<DWORD, Server_Config> m_GuildWarCommonCfgList;								// ����սͨ������
	map<DWORD, GuildWarOrganizeDefense_Config> m_GuildWarOrganizeDefenseList;		// ����ս��������
	map<DWORD, GuildWarRegion_Config> m_GuildWarRegionList;							// ����ս��������

	// YGame֧��
	map<string, YGamePayment_Config> m_YGamePaymentList;							// YGame֧�������б�

	// ��ֵ���а���
	map<DWORD, RechargeRankReward_Config> m_RechargeRankRewardList;		// ��ֵ���а���[key=rank_type+rank, data]

	// �һ�
	vector<HangUpDrop_Config> m_vHangUpDropList; // �һ���������
};

 
DWORD g_GetConfigValue(DWORD dwKey, DWORD dwDefault = 0);

extern CConfigManager *g_pCfgMgr;