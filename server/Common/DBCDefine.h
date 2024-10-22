#pragma once

#include "Const.h"
#include "DataDefine.h"

#pragma pack(push,1)

/*---------------------------------------------------------------------------------------------------*/
enum DBCFILE_KEY
{
	DBC_SERVER_CONFIG,					// 服务器配置
	DBC_LV_EXP_ENTRY,
	DBC_MONSTER_ENTRY,
	DBC_EMAILSQL_ENTRY,
	DBC_EMAIL_ENTRY,
	DBC_TIMES_CONFIG,
	DBC_CHAT_ENTRY,
	DBC_STOP_MESSAGE,

	DBC_PLAYERLEVEL_ENTRY,				// 人物升级经验配置
	DBC_ROLE_INIT,						// 新建角色初始化配置
	DBC_BROADCAST,					// 广播配置	
	DBC_MONSTER,					// 怪物配置
	DBC_HTTP_URL,
};

enum
{
	CONFIG_NORMAL_FREE_SUMMON_TIMES = 1,				// 每天可进行普通免费召唤的次数
	CONFIG_NORMAL_SUMMON_CD,							// 普通召唤CD(单位:分)
	CONFIG_ADVANCED_SUMMON_CD,							// 高级召唤CD(单位:分)
	CONFIG_NORMAL_SUMMON_COST_ITEM,						// 普通召唤消耗的物品ID
	CONFIG_ADVANCED_SUMMON_COST_ITEM,					// 高级召唤消耗的物品ID
	CONFIG_ADVANCED_SUMMON_COST_YB,						// 高级召唤消耗的元宝
	CONFIG_ADVANCED_ONEKEY_SUMMON_COST_YB,				// 高级批量召唤消耗的元宝(一键召唤)
	CONFIG_ADVANCED_ONEKEY_SUMMON_TIMES,				// 高级批量召唤的次数(一键召唤)
	CONFIG_NORMAL_ONEKEY_SUMMON_TIMES,					// 普通批量召唤的次数(一键召唤)
	CONFIG_ADVANCED_SUMMON_SPECIAL_TIMES,				// 高级召唤必出指定英雄需要的次数
	CONFIG_HERO_BREAK_LEVEL_LIMIT,						// 英雄突破等级上限
	CONFIG_HEROSOUL_SHOP_REFRESH_FREE_TIMES = 49,		// 雄魂商店每日免费刷新次数
	CONFIG_HERO_SHOP_REFRESH_RECOVER_INTERVAL = 46,		// 雄魂商店免费刷新恢复间隔时间
	CONFIG_SHOP_REFRESH_ITEM_ID = 47,					// 刷新令物品ID
	CONFIG_HERO_SHOP_REFRESH_COST_HEROSOUL = 48,		// 雄魂商店刷新消耗的雄魂值		
	CONFIG_GUILD_CREATE_COST_GOLD = 114,				// 创建工会需要的金币
	CONFIG_GUILD_DISMISS_MEMBER_NUM_LIMIT = 42,			// 工会解散成员数量上限
	CONFIG_AWAKEN_SHOP_REFRESH_COST_HEROSOUL = 60,		// 觉醒商店刷新消耗的雄魂值		
};

// 消耗数据
struct CostData
{
	WORD wType;
	int nValue;
};

// 消耗配置
struct Cost_Config
{
	vector<CostData> vCostList; // 消耗列表
};


//等级经验配置表
struct LvExpEntry
{
	DWORD		m_dwLv;								//等级
	DWORD		m_dwNextLvExp;						//下一级所需经验
	DWORD		m_dwVocationTitle;					//职业称号等级
	DWORD		m_dwHp;								//初始HP
};

 struct MonsterEntry
 {
 	DWORD		m_dwMonsterID;						//怪物ID
 	char*		m_szMonsterName;					//怪物名
 	DWORD		m_dwMonsterRace;					//怪物种族
 	DWORD		m_dwMissionMap;						//所属任务地图
 	DWORD		m_dwMonsterLv;						//怪物等级
 	DWORD		m_dwHp;								//Hp
 	DWORD		m_dwGMoney;							//奖励铜钱
 	DWORD		m_dwRMoney;							//奖励仙玉
 	DWORD		m_dwRMoneyOdds;						//奖励仙玉几率
 	DWORD		m_dwExp;							//奖励经验
 	DWORD		m_dwChaosDust;						//奖励混沌之尘
 	DWORD		m_dwCompoundToken;					//奖励天启令
 	DWORD		m_dwPrimordialLight;				//奖励鸿蒙之光
 	//RewardCard	m_tRewardCard[MAX_REWARD_CARD];		//奖励卡牌
 	char*		m_szScript;							//脚本文件
// 	DWORD		m_dwCardID[MAX_BATTLE_CARD_NUM];	//战斗卡牌ID
 };


//邮件插入数据库配置表
struct SendEmailEntry
{
	DWORD		m_dwID;                             //ID
	char*		m_szSqlQuery;						//SQL语句
	INT			m_nGender;							//角色性别
	INT			m_nVocation;						//角色职业
	INT			m_nRace;							//角色种族
	INT			m_nLv;								//角色等级
	INT			m_nAddRMoney7Day;					//7天理财
	INT			m_nAddRMoney30Day;					//30天理财
};

//邮件内容配置表
struct EmailEntry
{
	DWORD	dwID;         		// ID
	char*	pszTitle;			// 标题
	char*	pszWord;			// 内容
	char*	pszSender;			// 发件人
	DWORD	dwValidTime;		// 邮件有效时间(0=表示无时间限制) 单位:分
	DWORD 	dwExp;				// 经验
	DWORD 	dwGold;				// 铜钱
	DWORD 	dwYB;				// 元宝
	DWORD 	dwPhysicalStrength;	// 体力
	DWORD 	dwHonor;			// 荣誉
};

//各个系统开启结束时间配置
struct SystemTimes
{
	INT			ID;
	INT			IsIng;
	INT			BeginHour;
	INT			BeginMinute;
	INT			EndHouse;
	INT			EndMinute;
	INT			IsUpdate;
	INT			UpdateHour;
	INT			UpdateMinute;
	CHAR*		Name;								//行为名称 策划配置用的
};

//聊天配置结构
struct ChatEntry
{
	INT			ID;									//ID
	INT			SystemInterval;						//系统频道聊天间隔时间(秒)
	INT			WorldInterval;						//世界频道聊天间隔时间(秒)
	INT			PersonalInterval;					//私聊频道聊天间隔时间(秒)
};
struct StopMsg
{
	DWORD dwID;
	char* MsgInfo;							
};

//机器人配置表
struct RobotEntry
{
	DWORD		ID;
	char*		m_szAccID;							//AccID
	char*		m_szCharName;						//角色名
	DWORD		m_dwLevel;							//等级
};
struct RobotLoginEntry
{
	DWORD		ID;
	DWORD		m_dwBattleType;						//战斗类型
	char*		m_szFileName;						//机器人文件名
};

struct ZoneEntry
{
	DWORD				m_dwZoneID;
	DWORD				m_dwServerID;
};


struct OpenServer_Config
{
	DWORD dwKey;
	DWORD dwValue;
	char *pszValue;
	char *pszDesc;
};

// 商店商品配置
struct ShopGoodsData_Config
{
	WORD wGoodsId;			// 商品ID
	BYTE byType;			// 类型
	DWORD dwEntityId;		// 商品实体ID
	DWORD dwEntityNum;		// 商品实体数量
	WORD wBuyNumMax;		// 购买数量上限(0=表示无限制)

	WORD wNeedRoleLevel;	// 需要的角色等级
	int nNeedVipLevel;		// 需要的VIP等级
	WORD wNeedGuildLevel;	// 需要的工会等级
	
// 	int nCostGold;			// 消耗金钱
// 	int nCostDiamond;		// 消耗的钻石

	// 消耗列表
	vector<CostData> vCostList;	// 商品消耗列表(货币)

	// 消耗列表
	vector<Item> vCostItemList;	// 商品消耗列表(物品)
	
	int nContext;			// 上下文
	int nPriceListType;		// 价格列表类型
	vector<int> vPriceList;	// 价格列表
	int nVipIdx;			// 对应限购vip索引ID(0=表示木有限制)

	// 特殊商店需求
	int nSpecialCostValue;		// 特殊消耗的值
	WORD wRefreshRate;			// 刷出概率

	WORD wRefreshType;		// 刷新类型(0=日刷新(0点) 1 = 星期刷新(周一0点) 2 = 月刷新(1号0点))

	void Init()
	{
		nSpecialCostValue = 0;
		nPriceListType = 0;
		wGoodsId = 0;
		byType = 0;
		dwEntityId = 0;
		wNeedRoleLevel = 0;
		wBuyNumMax = 0;
		nNeedVipLevel = 0;
		nContext = 0;
		nVipIdx = 0;
		wRefreshType = 0;
		vPriceList.clear();
		vCostList.clear();
		vCostItemList.clear();
	}
};

// 人物升级经验配置
struct PlayerLevelExp
{
	DWORD m_dwLv;		// 等级
	DWORD m_dwNeedExp;	// 下一级所需经验
};


// 充值商品配置 
// struct RechargeItem_Config
// {
// 	DWORD dwGoodsId;			//商品ID
// 	DWORD dwCardType;			//获得福利卡类型(0=表示木有)
// 	DWORD dwGoodsPrice;			//商品价格
// 	DWORD dwGoods2YB;			//商品兑换元宝
// 	DWORD dwFirstExtraReward;	//首充额外奖励元宝
// };

// 服务器配置
struct Server_Config
{
	DWORD dwKey;
	DWORD dwValue;
	string strValue;
	vector<DWORD> vValueList;

	void Init()
	{
		vValueList.clear();
	}

	DWORD GetListValue(int nIndex) const
	{
		if (!vValueList.size())
			return 0;
		return nIndex < vValueList.size() ? vValueList[nIndex] : vValueList[vValueList.size() - 1];
	}

	bool IsInList(DWORD dwValue) const
	{
		for (size_t i = 0; i < vValueList.size(); i++)
		{
			if (vValueList[i] == dwValue)
				return true;
		}
		return false;
	}
};

// 服务器信息配置
struct ServerInfo_Config
{
	DWORD dwInitServerId; // 初始服务器ID
	DWORD dwCurServerId; // 当前服务器ID
	vector<DWORD> vServerRecordList; // 服务器合服记录列表 
};

// 新建角色初始化配置
struct RoleInit_Config
{
	DWORD dwDataId;				// 数据ID
	int nHeadImage;				// 头像
	vector<DWORD> vHeroList;	// 初始英雄列表(获得直接上阵)
	vector<Item> vItemList;		// 物品列表
	vector<Property> vProList;	// 属性列表(包括货币)

// 	DWORD dwGold;		// 铜钱
// 	DWORD dwYB;			// 元宝
// 	DWORD dwPower;		// 体力	
// 	DWORD dwExp;		// 经验
// 	DWORD dwHonor;		// 荣誉
// 	char *pszHeroList;	// 英雄列表
};

// 时间事件配置
// struct TimeEvent_Config : public TimeEventData
// {
// 
// };

typedef TimeEventData TimeEvent_Config;

// 签到配置
struct SignIn_Config
{
	WORD wDay;					// 签到天数
	Item tRewardItem;			// 奖励的物品
	BYTE byDoubleRewardVipLv;	// 双倍奖励需要的VIP等级(0=表示木有双倍奖励)
};

// 迎财神配置
struct YCS_Config
{
	WORD wRoleLevel;				// 角色等级
	BYTE byMoneyType;				// 奖励的货币类型
	vector<DWORD>	vMoneyValueList;// 领取次数奖励的货币值列表
};

// 铜雀台配置
struct TQT_Config
{
	WORD wId;
	int nStartTime;	// 开始时间
	int nOverTime;	// 结束时间
};

// 广播配置
struct Broadcast_Config
{
	DWORD dwBroadcastId;	// 广播ID
	char *pszText;			// 广播内容
};

// Http请求url配置
struct HttpUrl_Config
{
	DWORD dwUrlId;		// url对应的ID
	char *pszUrlDesc;	// URL描述
	char *pszUrl;		// URL
};

// 礼包随机物品
struct GiftRandItem
{
	DWORD dwItemId;	// 物品ID
	WORD wRate;		// 几率
	WORD wNumMin;	// 数量下限
	WORD wNumMax;	// 数量上限
};

// 物品配置
struct Item_Config
{
	DWORD dwId;				// 物品ID
	BYTE byType;			// 物品类型
	BYTE bySubType;			// 子类型
	DWORD dwPrice;			// 购买价格
	DWORD dwSellPrice;		// 出售价格
	BYTE byIsBroadcasting;	// 是否广播
	BYTE byIsUseInGet;		// 是否获得马上使用
	WORD wMaxNum;			// 堆叠数量上限
	WORD wUseLevel;			// 使用等级
	BYTE byIsCanUse;		// 能否直接使用
	DWORD dwExpiryDate;		// 有效期限(年月日 或者 开服天数  如:20171019 或 9 )
	int nParam1;			// 参数1
	int nParam2;			// 参数2
	vector<GiftRandItem> vGiftItemList;	// 礼包物品列表

	// 觉醒道具使用
	vector<Item> vComposeCostItemList;	// 合成消耗的物品列表
	int nDecomposeGodSoul;				// 分解获得的神魂
	vector<Property> vProList;			// 影响的属性列表

	void Init()
	{
		vGiftItemList.clear();
		vProList.clear();
		vComposeCostItemList.clear();
	}

	DWORD GetTotalRate() const
	{
		DWORD dwTotalRate = 0;
		for (int i = 0; i < vGiftItemList.size(); ++i)
		{
			dwTotalRate += vGiftItemList[i].wRate;
		}
		return dwTotalRate;
	}

	const GiftRandItem* GetGiftItemByRate(DWORD dwRate) const
	{
		DWORD dwCurRate = 0;
		for (int i = 0; i < vGiftItemList.size(); ++i)
		{
			dwCurRate += vGiftItemList[i].wRate;
			if (dwRate <= dwCurRate)
			{
				return &vGiftItemList[i];
			}
		}
		return NULL;
	}

	const GiftRandItem* GetGiftItemId(DWORD dwGiftItemId) const
	{
		for (int i = 0; i < vGiftItemList.size(); ++i)
		{
			if (vGiftItemList[i].dwItemId == dwGiftItemId)
			{
				return &vGiftItemList[i];
			}
		}
		return NULL;
	}
};

// 装备配置
struct Equipment_Config
{
	DWORD dwId;					// 装备ID
	BYTE byType;				// 装备类型
	WORD wQuality;				// 装备品质
	DWORD dwPrice;				// 价钱(金币)
	int nTowerPrestige;			// 爬塔声望
	WORD wStrengthLevelMax;		// 强化等级上限 
	WORD wRefineLevelMax;		// 精炼等级上限
	int nResetCostDiamond;		// 重生消耗钻石
	WORD wTalentId;				// 天赋ID(0=表示木有)
	WORD wStarMax;				// 升星上限(0表示不能升星)
	DWORD dwStarUpCostItemId;	// 升星消耗碎片ID
	WORD wNextQualityEquipmentId; // 下一品质的装备ID(0表示不可升)


	Property tStrengthenInitPro;	// 强化初始属性
	Property tStrengthenLevelPro;	// 强化等级属性(每一级的)

	vector<Property> vRefineLevelProList;	// 精炼等级属性列表
};

// 装备强化等级配置
struct EquipmentEnhanceLevel_Config
{
	int nCostGold;			// 消耗金币
	int nTotalCostGold;		// 总消耗金币
};

// 装备精炼等级配置 
struct EquipmentRefineLevel_Config
{
	BYTE byQuality;			// 品质
	WORD wLevel;			// 精练等级
	DWORD dwExp;			// 经验
	DWORD dwTotalExp;		// 总经验
};

// 装备合成配置
struct EquipmentCompose_Config
{
	DWORD dwFragmentId;		// 碎片ID
	WORD wFragmentNum;		// 碎片数量
};

// 装备锻造
struct EquipmentForge_Config
{
	WORD wLevel;				// 等级(强化,精练)
	vector<Property> vProList;	// 影响的属性列表

	void AddProperty(const Property &sPro)
	{
		vector<Property>::iterator iter = vProList.begin();
		for (; iter != vProList.end(); ++iter)
		{
			if (iter->wType == sPro.wType)
			{
				iter->nValue += sPro.nValue;
				return;
			}
		}

		vProList.push_back(sPro);
	}
};

// 套装配置
struct EquipmentSuit_Config
{
	DWORD dwSuitId;	// 套装ID
	vector<DWORD> vEquipmentList;		// 装备列表
// 	vector<Property> vProListForTwo;	// 两件属性
// 	vector<Property> vProListForThree;	// 三件属性
// 	vector<Property> vProListForFour;	// 四件属性
	map<DWORD, vector<Property>> vProList;	// 属性列表	
};

// 装备天赋
struct EquipmentTalent_Config
{
	WORD wId;	// 天赋Id
	WORD wLevel; // 等级
	WORD wNeedEnhanceLevel; // 需要的强化等级
	vector<Property> vProList;	// 获得的属性列表
};

// 装备升星
struct EquipmentStarUp_Config
{
	WORD wType;					// 装备类型
	WORD wQuality;				// 装备品质	
	WORD wStar;					// 星级	
	WORD wLevel;				// 等级	
	bool bIsStarUp;				// 是否升星	
	Item tCostItem;				// 消耗道具
	DWORD dwCostSelfNum;		// 消耗对应碎片数量	
	int nCostGold;				// 消耗金币数量	
	vector<Property> vProList;	// 属性列表
};

// 装备升品配置
struct EquipmentQualityUp_Config
{
	WORD wQuality;				// 装备品质	
	Item tCostItem;				// 消耗道具
	CostData tCostCurrency;		// 消耗货币
};

// 援军助威配置
struct ReinforcementEncourage_Config
{
	WORD wReinforcementLevel;	// 援军等级
	WORD wEncourageLevel;		// 助威等级
	vector<Property> vProList;  // 获得的属性列表
};


// 宝物锻造
struct TreasureForge_Config
{
	WORD wLevel;				// 等级(强化,精练)
	vector<Property> vProList;	// 影响的属性列表

	void AddProperty(const Property &sPro)
	{
		vector<Property>::iterator iter = vProList.begin();
		for (; iter != vProList.end(); ++iter)
		{
			if (iter->wType == sPro.wType)
			{
				iter->nValue += sPro.nValue;
				return;
			}
		}

		vProList.push_back(sPro);
	}
};

// 关联英雄属性
struct RelationHeroProperty
{
	vector<DWORD> vHeroList;	// 英雄列表
	vector<Property> vProList;	// 获得的属性
};

// 关联卡牌属性
struct RelationCardProperty
{
	vector<DWORD> vCardTypeList;	// 卡牌类型列表
	vector<Property> vProList;		// 获得的属性
};

// 关联装备属性
struct RelationEquipmentProperty
{
	vector<DWORD> vEquipmentList;	// 装备列表(或者关系,只要穿戴一件就可以)
	vector<Property> vProList;		// 获得的属性
};

// 关联宝物属性
struct RelationTreasureProperty
{
	vector<DWORD> vTreasureList;	// 宝物列表(或者关系,只要穿戴一件就可以)
	vector<Property> vProList;		// 获得的属性
};

// 英雄配置
struct Hero_Config
{
	BYTE byProfession;				// 职业
	BYTE bySex;						// 性别
	BYTE byHeroType;				// 英雄类型(1=普通英雄 2=经验英雄)
	WORD wInitLevel;				// 初始等级
	WORD wLimitLevel;				// 等级上限(暂时不起作用)
	DWORD dwToExpAsCostItem;		// 作为升级消耗品获得的经验
	DWORD dwNeedGoldAsCostItem;		// 作为升级消耗品时需要的金币	
	vector<DWORD> SkillList;		// 技能列表
	WORD wInitQuality;				// 英雄初始品质
	WORD wQualityMax;				// 英雄品质上限
	WORD wQualityStageMax;			// 英雄品阶上限
	int nHeroSoul;					// 回收获得的雄魂
	int nResetCostDiamond;			// 重生消耗钻石
	int nPrice;						// 出售价格
	int nQualifications;			// 英雄资质

	vector<RelationHeroProperty> vRelationHeroList;				// 关联英雄列表
	vector<RelationEquipmentProperty> vRelationEquipmentList;	// 关联装备列表
	vector<RelationTreasureProperty> vRelationTreasureList;		// 关联宝物列表
	vector<RelationCardProperty> vRelationCardList;				// 关联卡牌列表

	vector<RelationEquipmentProperty> vRelationEquipmentListEx;	// 关联装备列表(当前为橙,红品质的英雄)
	vector<RelationTreasureProperty> vRelationTreasureListEx;	// 关联宝物列表(当前为橙,红品质的英雄)
};

// 英雄属性配置
struct HeroPro_Config
{
	//DWORD dwHeroId;			// 英雄ID
	BYTE byProfession;		// 职业
	BYTE byQuality;			// 品质
	WORD wLevel;			// 英雄等级
	vector<Property> vProList;
// 	int nBloodMax;			// 血量上限
// 	int nAttack;			// 攻击
// 	int nPhysicalDefence;	// 物防
// 	int nMagicDefence;		// 法防
};

// 英雄等级配置
struct HeroLevel_Config
{
	WORD wLevel;
	DWORD dwExp;		// 升级需要的经验
	DWORD dwTotalExp;	// 总经验
};

// 英雄合成配置
struct HeroCompose_Config
{
	DWORD dwItemId;		// 需要的道具ID
	WORD wItemNum;		// 需要的道具数量
};

// 角色等级配置
struct RoleLevel_Config
{
	WORD wLevel;			// 等级
	DWORD dwExp;			// 需要的经验
	DWORD dwStamina;		// 获得体力值
	DWORD dwEnergy;			// 获得精力值
	DWORD dwStaminaMax;		// 体力值上限
	DWORD dwEnergyMax;		// 精力值上限
	DWORD dwWantedMax;		// 通缉令上限
	DWORD dwFriendNumMax;	// 好友数量上限
	DWORD dwBlessSpriteNumMax;	// 祝福精灵数量上限
	DWORD dwDailyChatTimesMax;	// 每日聊天次数上限
	vector<Item> vRewardItemList; // 升级奖励的物品列表
};

// 角色属性恢复
struct RoleProRecover_Config
{
	BYTE byProType;		// 属性类型
	int nIntervalTime;	// 恢复间隔时间
	int nRecoverValue;	// 每次恢复的值
};

// 英雄普通召唤
struct HeroNormalSummon_Config
{
	DWORD dwHeroId;
	DWORD dwRate;
};

// 英雄高级召唤
struct HeroAdvancedSummon_Config
{
	DWORD dwHeroId;
	DWORD dwRate;
	DWORD dwRate1;
};

// 英雄VIP召唤
struct HeroVipSummon_Config
{
	DWORD dwHeroId;
	DWORD dwHeroNum;
	DWORD dwNormalRate;
	DWORD dwSpecialRate;
};

// 英雄突破升级配置
struct HeroBreakLevelUp_Config
{
	WORD wNeedHeroLevel;				// 需要的英雄等级
	int nCostGold;						// 需要消耗的金币
	DWORD dwCostItemId;					// 需要消耗的物品ID(突破石)
	WORD wCostItemNum;					// 需要消耗的物品数量(突破石)
	DWORD dwCostHeroId;					// 需要消耗的英雄ID(todo::已不使用)
	WORD wNeedHeroQuality;				// 需要的英雄品质
	WORD wCostSameNameHeroNum;			// 需要消耗的同名英雄数量(0表示无消耗)
	WORD wCostSameQualityHeroNum;		// 需要消耗的同品质英雄数量(0表示无消耗)
	WORD wCostSameProfessionHeroNum;	// 需要消耗的同职业英雄数量(0表示无消耗)

	void Init()
	{
		memset(this, 0, sizeof(HeroBreakLevelUp_Config));
	}
};

// 英雄突破配置
struct HeroBreak_Config
{
	DWORD dwHeroId;
	WORD wBreakLevel;
	vector<Property> vSelfProList;
	vector<Property> vTeamProList;	// 队伍属性
};

// 英雄进化
struct HeroEvolution_Config
{
	WORD wQuality;					// 品质
	WORD wStage;					// 品阶
	bool isTheLastStage;			// 是否最后一阶
	vector<Item> vCostItemList;			// 消耗物品列表
	vector<CostData> vCostCurrencyList; // 消耗货币列表
	int nCostGold;						// 消耗金币
	vector<Property> vProList;			// 属性列表

	WORD wAddQualifications;			// 添加的资质
	WORD wNeedBreakLevel;				// 需要英雄进阶等级
	WORD wNeedEnhanceLevel;				// 需要英雄强化等级
};

// 英雄培养上限配置
struct HeroTrainLimit_Config
{
	vector<Property> list;
};

// 英雄图鉴
struct HeroPokedex_Config
{
	WORD wCamp;		// 阵营
	WORD wQuality;	// 品质
	vector<DWORD> vHeroIdList; // 英雄Id列表
};

// 英雄培养消耗配置
// struct HeroTrainCost_Config
// {
// 	DWORD dwItemId;		// 消耗物品ID
// 	WORD wItemNum;		// 消耗物品数量
// 	DWORD dwGold;		// 消耗金币
// 	DWORD dwYB;			// 消耗元宝
// };

// 英雄培养配置
struct HeroTrain_Config
{
	BYTE byProType;		// 属性类型
	BYTE byProLevel;	// 属性等级
	WORD wNeedHeroLevel;// 需要的英雄等级
	int nProMax;		// 属性上限
	vector<Property> vLevelUpProList; // 升级属性列表(累积属性)
};

// 英雄培养配置
// struct HeroTrain_Config
// {
// 	BYTE byProfession;			// 职业
// 	BYTE byType;				// 培养类型
// 	BYTE byLevel;				// 培养等级
// 	WORD wNeedHeroLevel;		// 需要的英雄等级
// 	WORD wCostItemId;			// 消耗的物品ID
// 	WORD wCostItemNum1;			// 消耗的物品数量1
// 	WORD wCostItemNum2;			// 消耗的物品数量2
// 	int nCostDiamond;			// 消耗钻石
// 	int nDefaultRate;			// 默认成功率
// 	int nRateFromFail;			// 失败获得的成功几率
// 	int nRateMax;				// 几率上限
// 	vector<Property> vProList;	// 影响的属性列表
// 
// 	void Init()
// 	{
// 		vProList.clear();
// 	}
// };

// 英雄培养几率数据
struct HeroTrainRateData
{
	WORD wMinRate;
	WORD wMaxRate;
	int nProp;
};

// 英雄培养随机属性数据
struct HeroTrainRandPro_Config
{
	BYTE byProType;	// 属性类型
	int nMin;		// 随机下限
	int nMax;		// 随机上限
};

// 英雄培养消耗配置
struct HeroTrainCost_Config
{
	BYTE byCostType;		// 消耗类型
	Item tCostItem;			// 消耗物品
	MoneyData tCostMoney;	// 消耗货币
};

// 英雄培养随机属性配置
struct HeroTrainRandProp_Config
{
	//map<DWORD, vector<HeroTrainRateData>> list;
	vector<HeroTrainRateData> list;
};

// 英雄天命配置
struct HeroDestiny_Config
{
	DWORD dwNeedExp;					// 升级所需经验(0表示不能升级)
	DWORD dwAddExp;						// 每次注入的经验
	DWORD dwCostItemId;					// 消耗的物品ID
	WORD wCostItemNum;					// 消耗的物品数量
	DWORD dwSkillId;					// 关联的技能ID
	vector<RateData> vLevelUpRateList; 	// 直接升级的概率数据
	vector<Property> vProList;			// 影响的属性列表

	const RateData* GetRateData(DWORD dwValue) const
	{
		const RateData *pTemp = NULL;
		vector<RateData>::const_iterator iter = vLevelUpRateList.begin();
		for (; iter != vLevelUpRateList.end(); ++iter)
		{
			if (dwValue >= iter->dwMin && dwValue <= iter->dwMax)
			{
				pTemp = &*iter;
				break;
			}
		}
		return pTemp;
	}
};

// 英雄觉醒配置
struct HeroAwaken_Config
{
	BYTE byInitQuality;			// 初始化品质
	BYTE byProfession;			// 职业
	BYTE byStar;				// 星
	BYTE byLevel;				// 等级
	WORD wNeedHeroLevel;		// 需要英雄等级
	int nCostMoneyType;			// 消耗货币类型
	int nCostMoneyValue;		// 消耗货币值
	Item tCostItem;				// 消耗的物品(觉醒丹) 觉醒时消耗
	WORD wCostHeroNum;			// 消耗同名英雄数量(0=表示无消耗)
	vector<DWORD> vCostItemList;// 消耗物品列表(觉醒物品) 装备消耗
	vector<Property> vProList;	// 影响的属性列表(到当前阶段的总属性,包括额外属性)
	vector<Property> vItemProList;	// 材料加的属性列表
};

// 宝物配置
struct Treasure_Config
{
	WORD wInfoId;				// 信息ID
	WORD wQuality;				// 品质
	BYTE byType;				// 类型
	WORD wStrengthenLevelMax;	// 强化等级上限
	WORD wRefineLevelMax;		// 精炼等级上限
	DWORD dwAsExp;				// 作为经验
	int nPrice;					// 价格
	int nResetCostDiamond;		// 重生消耗钻石

	DWORD dwNextQualityTreasureId;	// 下一级品质宝物ID(0表示不能升级)
	DWORD dwQualityUPCostItemId;	// 升品消耗的道具ID

	vector<Property> vStrengthenInitProList;	// 强化初始属性列表
	vector<Property> vStrengthenLevelProList;	// 强化等级属性列表(每一级的)

	vector<Property> vRefineLevelProList;	// 精炼等级属性列表
};

// 宝物强化配置
struct TreasureEnhance_Config
{
	DWORD dwNeedExp;		// 需要的经验(0表示不能再升级)
	int wCostGold;		// 强化消耗金币
	DWORD dwTotalExp;	// 总的累积经验
};

// 宝物精练配置
struct TreasureRefine_Config
{
	DWORD dwCostItemId;			// 消耗的物品ID
	WORD wCostItemNum;			// 消耗的物品数量
	//DWORD dwCostTreasureId;		// 消耗的宝物ID
	BYTE byCostTreasureNum;		// 消耗的宝物数量
	int nCostGold;			// 消耗的金币
};

// 宝物合成配置
struct TreasureCompose_Config
{
	vector<DWORD> cost_item_list;

	void Init()
	{
		cost_item_list.clear();
	}

	// 是否消耗物品
	bool IsCostItem(WORD wItemId) const
	{
		for (int i = 0; i < cost_item_list.size(); ++i)
		{
			if (cost_item_list[i] == wItemId)
			{
				return true;
			}
		}
		return false;
	}
};

// 宝物熔炼
struct TreasureSmelt_Config
{
	DWORD dwTreasureInfoId;
	DWORD dwCostTreasureInfoId;	// 消耗的宝物ID
	DWORD dwItemId;				// 熔炼获得的物品ID
};

// 宝物升品消耗配置
struct TreasureQualityUPCost_Config
{
	WORD wRefineLevel; // 精炼等级
	DWORD dwCostItemNum; // 消耗的道具数量
	vector<CostData> vCostCurrencyList; // 消耗货币类型列表
};

// 竞技场排行配置
struct ArenaRank_Config
{
	WORD wRank;					// 排名
	WORD wTargetRankInterval;	// 挑战目标排名间隔
	int nAwardPrestige;			// 奖励竞技场声望
	int nAwardGold;				// 奖励金币
	int nFirstRankDiamond;		// 首次排行奖励
};

// 竞技场奖励配置
struct ArenaAward_Config
{
	WORD wRoleLevel;
	DWORD dwExp;			// 经验奖励
	int nWinGold;			// 获胜金币奖励
	int nFailGold;			// 失败金币奖励
	int nWinArenaPrestige;	// 获胜竞技场声望奖励
	int nFailArenaPrestige;	// 失败竞技场声望奖励
	WORD wFPItemId;			// 翻牌物品ID
};

// 竞技场机器人配置
struct ArenaRobot_Config
{
	DWORD dwRobotId; // 机器人Id
	WORD wRank;	// 排名
	string strRobotName; // 机器人名称
};

// 成就配置
struct Achievement_Config
{
	// 等级数据
	struct LvData
	{
		WORD wLv;					// 等级
		WORD wNeedRoleLv;			// 需要的角色等级
		DWORD wValue;				// 需要的上下文值
		Item tRewardItem;			// 奖励物品
		//vector<Item> vAwardList;	// 奖励列表
	};

	WORD wAchievementId;			// 成就ID
	vector<LvData> vLvDataList;		// 成就等级数据列表

	const LvData* GetLvData(WORD wLv) const
	{
		for (size_t i = 0; i < vLvDataList.size(); ++i)
		{
			if (vLvDataList[i].wLv == wLv)
			{
				return &vLvDataList[i];
			}
		}
		return NULL;
	}
};

// 7天目标数据配置
struct SevenDayTarget_Config
{
	WORD wTargetId;					// 目标ID
	WORD wLogicType;				// 逻辑类型
	int nX;							// 参数X
	int nY;							// 参数Y
	BYTE byIsTheSameDayValid;		// 是否当天有效
	BYTE byIsFromTheSameDayValid;	// 是否当天开始有效
	BYTE byIsCompletionTarget;		// 是否完成度目标
	BYTE byIsAsc;					// 是否升序
	WORD wBelongActivityDay;		// 所属活动天数
	vector<Item> vRewardItemList;	// 奖励物品列表
};

// 7天完成度奖励
struct SevenDayCompletionReward_Config
{
	WORD wCompletionNum;		
	vector<Item> vRewardItemList;	// 奖励物品列表
};

// 日常任务配置
struct DailyMission_Config
{
	WORD wMissionId;	// 任务Id
	WORD wNeedRoleLv;	// 需要的角色等级
	DWORD dwValue;		// 任务值
	WORD wPoint;		// 获得的积分
	DWORD dwBaseExp;	// 获得的基础经验值
	//Item tAwardItem;	// 奖励的物品
	vector<Item> vRewardItemList; // 奖励物品列表
};


// 角色等级日常任务配置
struct RoleLvDailyMission_Config
{
	// 积分数据
	struct PointData
	{
		WORD wPoint;		// 积分
		WORD wRewardItemId;	// 奖励的物品Id
	};

	WORD wLevel;
	vector<DWORD> vMissionList;		// 任务列表
	vector<PointData> vPointList;	// 积分列表

	const PointData* GetPointData(WORD wPoint) const
	{
		for (size_t i = 0; i < vPointList.size(); ++i)
		{
			if (vPointList[i].wPoint == wPoint)
				return &vPointList[i];
		}
		return NULL;
	}

	// 是否任务
	bool IsMission(WORD wMissionId) const
	{
		for (size_t i = 0; i < vMissionList.size(); ++i)
		{
			if (vMissionList[i] == wMissionId)
				return true;
		}
		return false;
	}
};

// 星数奖励数据
struct StarAwardData
{
	BYTE byStar;
	//DWORD dwBoxId;
	vector<Item> vBoxList;
};

// 副本章节配置
struct FubenChapter_Config
{
	DWORD dwChapter;						// 章节
	vector<DWORD> vMapList;					// 关卡列表
	vector<DWORD> vLevelLimitList;			// 关卡等级限制列表
	vector<StarAwardData> vStarAwardList;	// 星数奖励列表
	vector<DWORD> vOpenWeekdayList;			// 开放星期列表
	WORD wOpenLevel;						// 开放等级
	WORD wExtraMapId;						// 额外关卡
	DWORD dwNeedRoleLevel;					// 需要的角色等级

	// 日常副本使用
	WORD wDailyBattleTimes;		// 每日可挑战次数(0表示无限制)
	BYTE byIsDamageReward;		// 是否伤害奖励
	BYTE byWinCondition;		// 获胜条件 1=我方剩余血量不低于x % 2=我方死亡人数不超过x 3=x秒内获胜 4=杀死对方所有单位
	BYTE byIsLoseAlsoHaveReward;// 是否失败也有奖励
	DWORD dwSweepRewardContext;	// 扫荡奖励上下文


	// 精灵副本使用
	vector<DWORD> vEnemyMapList;				// 外敌关卡列表
	vector<DWORD> vEnemyMapRateList;			// 外敌关卡几率列表
	WORD wEnemyRandNumMin;		// 外敌随机数量下限
	WORD wEnemyRandNumMax;		// 外敌随机数量上限
	WORD wEnemyRandChapterMin;	// 外敌随机章节下限
	WORD wEnemyRandChapterMax;	// 外敌随机章节上限


	DWORD GetMapId(BYTE byMapIndex) const
	{
		if (!byMapIndex || byMapIndex > vMapList.size())
			return 0;

		return vMapList[byMapIndex - 1];
	}

	// 是否开放的日
	bool IsOpenDay(BYTE byDay) const
	{
		for (int i = 0; i < vOpenWeekdayList.size(); ++i)
		{
			if (vOpenWeekdayList[i] == byDay)
				return true;
		}
		return false;
	}

	void Init()
	{
		vMapList.clear();
		vStarAwardList.clear();
		vOpenWeekdayList.clear();
		wDailyBattleTimes = 0;
		byIsDamageReward = false;
	}
};

// 日常副本伤害奖励
struct DailyFBDamageReward_Config
{
	WORD wChapter;		// 章节(副本类型)
	WORD wMapIdx;		// 关卡索引(难度类型)
	//WORD wDamageRate;	// 伤害比例(百分比)
	WORD wContext;		// 上下文件(伤害比例,消耗时间)
	//BYTE byIsSweepReward;	// 是否扫荡奖励

	vector<Item> vItemList;	// 奖励物品列表
};

// 主线副本章节奖励
struct MainFBChapterReward_Config
{
	WORD wLevel;				// 奖励等级
	WORD wNeedChapter;		// 需要的章节
	DWORD dwItemId;			// 物品ID
	//vector<Item> vItemList;	// 奖励物品列表
};

// 掉落数据
struct DropData
{
	//BYTE byType;			// 类型
	WORD wRate;				// 掉落概率
	DWORD dwDropObjectId;	// 掉落对象ID
	WORD wDropObjectNum;	// 掉落对象数量
};

// 限时掉落配置
struct LimitDrop_Config
{
	DWORD dwDropItemId;	// 掉落物品ID
	DWORD dwStartDate;	// 开始日期
	DWORD dwOverDate;	// 结束日期
	DWORD dwDropRate;	// 掉落几率
	DWORD dwDropNumMin;	// 掉落数量下限
	DWORD dwDropNumMax;	// 掉落数量上限 
};

// 副本关卡配置
struct Map_Config
{
	DWORD dwMapId;
	BYTE byDailyBattleTimesLimit;	// 每日可以挑战限制
	vector<DWORD> vBattleList;		// 战役列表
	BYTE byIsCanComment;			// 是否可评论
	DWORD dwNeedMinPower;			// 挑战需要最小的战力

	// 消耗
	vector<CostData> vCostList;				// 消耗列表

	// 首胜奖励
	vector<Item> vFirstWinItemList;			// 首胜奖励物品列表
	vector<Property> vFirstWinCurrencyList;	// 首胜奖励货币列表

	// 普通奖励
	vector<Property> vNormalRewardList;		// 普通奖励货币列表
	vector<RewardItem> vRandRewardList;		// 普通奖励物品列表(部分关卡物品数量会随机)

	// 宝箱,掉落
	vector<Item> vBoxList;					// 宝箱奖励物品列表
	vector<DropData> vDropList;				// 掉落奖励物品列表
	
	int GetCostValue(WORD wCostType) const
	{
		for (size_t i = 0; i < vCostList.size(); ++i)
		{
			if (vCostList[i].wType == wCostType)
				return vCostList[i].nValue;
		}
		return 0;
	}
};


// 爬塔星数奖励
struct TowerStarAward
{
	BYTE byStar;				// 星数
	vector<Item> vAwardList;	// 奖励列表
};

// 爬塔章节
struct TowerChapter_Config
{
	BYTE byChapter;								// 章节
	vector<WORD> vMapList;						// 包含的关卡列表
	vector<TowerStarAward> vAwardList;			// 奖励列表

	const TowerStarAward* GetAward(BYTE byStar) const
	{
		for (size_t i = vAwardList.size() - 1; i >= 0; --i)
		{
			if (byStar >= vAwardList[i].byStar)
			{
				return &vAwardList[i];
			}
		}
		return NULL;
	}
};

// 爬塔普通关卡
struct TowerMap_Config
{
	struct MapData
	{
		WORD wMapId;	// 关卡ID
		BYTE byStar;		// 奖励星数
	};

	WORD wMapLevel;					// 关卡等级
	WORD wChapter;					// 所属章节
	bool bIsLast;					// 是否最后章节一关
	vector<MapData> MapDataList;	// 关卡数据列表
};

// 爬塔精英关卡
struct TowerEliteMap_Config
{
	WORD wMapLevel;				// 关卡等级
	WORD wMapId;				// 关卡ID
	WORD wNeedNormalMapLevel;	// 需要普通副本的关卡等级
};

// 爬塔随机属性配置
struct TowerRandPro_Config
{
	BYTE byStar;
	vector<Property> vProList;
};

// 爬塔星数商品
struct TowerStarGoods_Config
{
	struct StarGoods
	{
		WORD nId;
		WORD wNum;
		int nPrice;
	};

	WORD wStarMin;				// 星数下限
	WORD wStarMax;				// 星数上限
	//vector<Item> vGoodsList;	// 商品列表
	//vector<int> vPriceList;		// 价格列表

	vector<StarGoods> vGoodsList; // 商品列表
};

// 领地技能
struct LandSkill
{
	WORD wLevel;			// 等级
	WORD wDoubleAwardRate;	// 双倍奖励几率
	WORD wNeedPatrolTime;	// 需要的巡逻时间
	int nCostDiamond;		// 消耗的钻石
};

// 领地城池配置  
struct LandCity_Config
{
	WORD wCityId;					// 城池ID
	WORD wPreCityId;				// 前置城池ID
	WORD wMapId;					// 关卡ID
	int nSuppressEnergy;			// 镇压奖励精力
	int nHelpSuppressDiamond;		// 帮助镇压奖励钻石
	vector<LandSkill> vSkillList;	// 技能列表
};

// 领地巡逻事件配置
struct LandPatrolEvent_Config
{
	// 事件数据
	struct EventData
	{
		WORD wEventId;		// 事件ID
		WORD wEventRate;	// 事件几率
		DWORD dwObjId;		// 奖励对象Id
		DWORD dwOjbNum;		// 奖励对象数量
	};

	WORD wCityId;
	vector<EventData> vEventList; // 事件列表

	const EventData* GetEventData(WORD wEventId) const
	{
		for (int i = 0; i < vEventList.size(); ++i)
		{
			if (vEventList[i].wEventId == wEventId)
			{
				return &vEventList[i];
			}
		}

		return NULL;
	}
};

// 领地巡逻配置
struct LandPatrol_Config
{
	// 时间数据
	struct TimeData
	{
		BYTE byTimeType;		// 时间类型
		DWORD dwTotalTime;		// 总时间(秒)
		WORD wItemNumMin;		// 碎片数量下限Fragment
		WORD wItemNumMax;		// 碎片数量上限
		int nCostGold;			// 消耗金币
		int nCostEnergy;		// 消耗精力
	};

	BYTE byType;					// 巡逻方式	
	WORD wIntervalTime;				// 收益时间间隔(分钟)
	BYTE byVipIdx;					// 受限的VIP索引(0=表示木有受限)
	vector<TimeData> vTimeDataList; // 时间数据列表

	const TimeData* GetTimeData(BYTE byTimeType) const
	{
		for (int i = 0; i < vTimeDataList.size(); ++i)
		{
			if (vTimeDataList[i].byTimeType == byTimeType)
			{
				return &vTimeDataList[i];
			}
		}
		return NULL;
	}
};


// 雄魂商店随机商品
 struct HeroSoulShopRandGoods
 {
 	WORD wGoodsId;		// 商品ID
 	WORD wLevelLimit;	// 等级限制
 	WORD wRate;			// 概率
 };

// 雄魂商店商品配置
struct HeroSoulShopGoods_Config
{
	WORD wGoodsId;		// 商品ID
	BYTE byType;		// 商品类型(1=物品,2=英雄,3=碎片)
	DWORD dwEntityId;	// 商品实体ID
	WORD wEntityNum;	// 商品实体数量
	WORD wLevelLimit;	// 等级限制
	WORD wRate;			// 概率
	int nYB;			// 商品售价(元宝)
	int nHeroSoul;		// 商品售价(雄魂)
	BYTE byIsRecommend;	// 是否推荐
};

// 雄魂商店配置
struct HeroSoulShop_Config
{
	BYTE byPos;										// 位置
	BYTE byIsRepeat;								// 是否可重复
	vector<HeroSoulShopGoods_Config*> GoodsList;	// 商品随机列表
};

// 世界之心配置
struct HeartOfWorld_Config
{
	struct LevelData
	{
		BYTE byLevel;			// 等级
		WORD wRoleQuality;		// 影响的主角品质
		//Property sAddPro;		// 增加的属性(属性类型|属性值)
		vector<Property> vAddProList; // 添加的属性列表
		Item sAwardItem;		// 奖励的物品
		Item sCostItem;			// 升级消耗物品(物品ID|物品数量)
	};

	BYTE byStage;
	vector<LevelData> vLeveDatalist;

	const LevelData* GetLevelData(BYTE byLevel) const
	{
		for (int i = 0; i < vLeveDatalist.size(); ++i)
		{
			if (vLeveDatalist[i].byLevel == byLevel)
				return &vLeveDatalist[i];
		}
		return NULL;
	}
};

// 精灵配置
struct Spirit_Config
{
	DWORD dwId;			// 精灵ID
	WORD wPokedexId;	// 所属的图鉴ID

	WORD wInitStar;			// 初始星数
	WORD wStarMax;			// 星数上限
	vector<DWORD> vQualificationsList;	// 精灵资质
	DWORD dwTtarUpCostItemId; // 升星消耗的物品ID(碎片)
	DWORD dwFragmentNum;	// 碎片

	WORD GetQualifications(WORD wStar) const
	{
		if (!vQualificationsList.size())
			return 0;

		// 旧数据兼容
		if (wStar < wInitStar)
			wStar = wInitStar;

		// 防止为0的情况
		if (!wStar)
			return vQualificationsList[wStar];

		// 防止越界
		if (wStar >= vQualificationsList.size())
			return vQualificationsList[vQualificationsList.size()-1];

		return vQualificationsList[wStar];
	}
};

// 精灵强化配置
struct SpiritEnhance_Config
{
	DWORD dwId;	// 精灵ID
	WORD wEnhanceLevel;	// 强化等级
	vector<Item> vCostItemList;	// 消耗道具列表
	vector<Property> vAddProList;// 获得属性列表	
};

// 精灵培养
struct  SpriteTrain_Config
{
	DWORD dwId;				// 精灵ID
	WORD wStage;			// 阶
	WORD wNeedEnhanceLevel;	// 需要的强化等级
	WORD wNeedTrainTimes;	// 升阶需要的培养次数(0=表示已达上限)
	WORD wSkillLevel;		// 技能等级
	WORD wTalentLevel;		// 天赋等级
	DWORD dwModulId;		// 精灵模型Id
	DWORD dwProConversionRate;		// 属性转化率
	Item tStageUpCostItem;			// 升阶消耗的物品[id|num]
	vector<Item> vTrainCostItemList;// 培养消耗的物品列表
	vector<Property> vTrainProList; // 培养属性列表
	vector<Property> vTrainTotalProList; // 培养总属性列表
};

// 精灵图鉴
struct SpiritPokedex_Config
{
	DWORD dwPokedexId;	// 图鉴ID
	WORD wPokedexLevel; // 图鉴等级
	vector<DWORD> vNeedSpiritList;	// 需要的精灵列表
	WORD wNeedEnhanceLevel;		// 需要的强化等级
	Property vAddPro;			// 获得的属性
	
	//vector<WORD> vLevelUpConditionList; // 图鉴升级条件列表
	//int nLevelUpAddProValue;	// 升级添加的属性值(每一级)
};

// 精灵天赋技能
struct SpriteTalentSkill_Config
{
	DWORD dwSpriteId;							// 精灵ID	
	WORD wTalentSkillId;						// 天赋技能等级	
	vector<Property> vHeroProList;				// 全体出战英雄属性类型	全体出战英雄属性值	
	vector<Property> vSpriteProList;			// 全体精灵属性类型	全体精灵属性值	
	vector<DWORD> m_vFBTypeList;				// 副本类型列表	
	vector<DWORD> vExtraRewardDropItemId;		// 获得额外奖励的掉落物品ID	
	vector<DWORD> vExtraRewardDropItemRate;		// 获得额外奖励的物品比例	
	vector<DWORD> vExtraRewardCurrencyType;		// 获得额外奖励的货币类型	
	vector<DWORD> vExtraRewardCurrencyRate;		// 获得额外奖励的货币万分比

	// 是否额外奖励的物品
	bool IsExtraRewardFB(WORD wFBType) const
	{
		for (size_t i = 0; i < m_vFBTypeList.size(); ++i)
		{
			if (m_vFBTypeList[i] == wFBType)
				return true;
		}
		return false;
	}
};

// 精灵升星
struct SpriteStarUp_Config
{
	WORD wStar;					// 星数
	DWORD dwStarUpCostItemNum;	// 升到下级所需消耗碎片数量
};

// 精灵特性
struct SpriteSpecialProperty_Config
{
	DWORD dwSpriteId;	// 精灵ID
	WORD wStar;			// 星级
	vector<Property> vBlessProList;			// 祝福提升的属性列表
};

// 精灵资质
struct SpriteQualifications_Config
{
	WORD wQualifications;	// 精灵资质
	int nAddRate;			// 加成系数
};

// 精灵模型配置
struct SpiritModel_Config
{
	int nPrice;			// 价格
	BYTE byNeedStage;	// 需要的阶段
	BYTE byNeedLevel;	// 需要的等级
};

// 工会配置
struct Guild_Config
{
	WORD wLevel;		// 工会等级
	DWORD dwNeedExp;	// 升级需要的经验
	WORD wMaxMember;	// 成员数量上限
};


// 工会权限配置
struct GuildPrivilege_Config
{
	bool bDismiss;		// 解散
	bool bApplyCheck;	// 入会审核
	bool bLeave;		// 退出工会
	bool bAppoint;		// 任命副会长
						// 转移会长
	bool bKickOut;		// 踢人
						// 发布内部消息
						// 发布宣言
};

// 工会泉涌配置
struct GuildQY_Config : public Cost_Config
{
	WORD wType;						// 泉涌类型
	WORD wCritRate;					// 暴击几率
	DWORD dwExp;					// 获得的经验
	DWORD dwCritExp;				// 暴击经验值
	DWORD dwContribution;			// 获得的贡献值
	DWORD dwCritContribution;		// 暴击贡献值
	DWORD dwQuanShui;				// 获得的泉水
	WORD wNeedVipLevel;				// 需要的VIP等级
};

// 工会泉水奖励配置
struct GuildQSAward_Config
{
	WORD wQS;			// 泉水
	WORD wGuildLevel;	// 工会等级
	vector<Item> vItemList; // 奖励的物品列表
};

// 工会技能配置
struct GuildSkill_Config
{
	WORD wSkillId;					// 技能Id
	WORD wLevel;					// 技能等级
	WORD wNeedGuildLevel;			// 需要工会等级
	int nCostGuildExp;				// 工会提升消耗的工会经验
	//CostData tCostData;				// 个人学习消耗
	vector<CostData> vCostList;		// 个人学习消耗列表
	vector<Property> vAddProList;	// 添加的属性列表

	int GetCostType(WORD wType) const
	{
		for (size_t i = 0; i < vCostList.size(); ++i)
		{
			if (vCostList[i].wType == wType)
				return vCostList[i].nValue;
		}
		return 0;
	}
};

// 工会副本配置
struct GuildFBChapter_Config
{
	WORD wChapter;						// 章节
	vector<DWORD> vMapList;				// 地图列表
	vector<Item> vMapRewardList;		// 关卡奖励列表(所有的阵营都使用同一个奖励列表)
	vector<Item> vChapterRewardList;	// 章节奖励列表
	int nKillExp;						// 击杀奖励工会经验
	int nKillContribution;				// 击杀奖励贡献值
	int nBattleContribution;			// 挑战奖励贡献值
};

// 怪物配置
struct Monster_Config
{
	DWORD dwMonsterId;
	DWORD dwBlood;				// 血量	
	string strName;				// 怪物名称
	vector<DropData> vDropList;	// 掉落列表
	void Init()
	{
		vDropList.clear();
	}
};

// 战役配置
struct Battle_Config
{
	DWORD dwBattleId;			// 战役ID
	//vector<DWORD> vMonsterList;	// 怪物列表
	DWORD arMonsterList[MAX_BATTLE_MONSTER_NUM];
	void Inti()
	{
		memset(arMonsterList, 0, sizeof(arMonsterList));
		//vMonsterList.clear();
	}

// 	void AddMonster(BYTE byIndex, WORD wMapId)
// 	{
// 		if (byIndex >= MAX_BATTLE_MONSTER_NUM)
// 			return;
// 
// 		arMonsterList[byIndex] = wMapId;
// 	}
};

// 英雄上阵配置
struct HeroToBattle_Config
{
	BYTE byType;
	BYTE byPos;
	WORD wNeedLevel;
};

// 关卡配置
typedef Map_Config FubenMap_Config;



// 宝箱配置
// struct Box_Config
// {
// 	int nGold;		// 金币
// 	int nExp;		// 经验
// 	int nDiamond;	// 钻石
// 	vector<ObjectData> vOjbList; // 对象列表 
// };

// 叛军功勋
struct RebelExploit_Config
{
	DWORD dwListId;		// 奖励列表ID
	DWORD dwAwardId;	// 奖励Id
	int nNeedExploit;	// 需要的功勋
	WORD wAwardItemId;	// 奖励的物品Id
	WORD wAwardItemNum;	// 奖励的物品数量
	int nAwardGold;		// 奖励的金币
};

// 叛军战役
struct RebelBattle_Config
{
	WORD wLevel;	// 叛军等级	
	//WORD wMapId;	// 关卡ID
	vector<DWORD> vRandMapList;		// 随机关卡列表
	vector<Property> vAddProList;	// 添加的属性列表
};

// 叛军品质
struct RebelQuality_Config
{
	BYTE byQuality;				// 品质
	vector<Property> vProList;	// 属性列表
	int nDiscoverAwardDiamond;	// 发现者奖励钻石
	int nKillerAwardDiamond;	// 击杀者奖励钻石	
	DWORD dwLiveTime;			// 存活时间

	void Init()
	{
		vProList.clear();
	}
};

// 角色叛军
struct RoleRebel_Config
{
	WORD wRoleLevel;						// 角色等级
	WORD wAwardListId;						// 奖励的列表ID
	vector<RateDataEx> vRandList;			// 品质随机列表

	void Init()
	{
		vRandList.clear();
	}

	int GetTotalRate() const
	{
		int nTotalRate = 0;
		for (int i = 0; i < vRandList.size(); ++i)
		{
			nTotalRate += vRandList[i].nRate;
		}
		return nTotalRate;
	}

	BYTE GetQuality(WORD wRate) const
	{
		int nMax = 0;
		for (int i = 0; i < vRandList.size(); ++i)
		{
			nMax += vRandList[i].nRate;
			if (nMax && wRate <= nMax)
			{
				return vRandList[i].nValue;
			}
		}
		return QUALITY_INVALID;
	}
};

// 叛军攻击配置
struct RebelAttack_Config
{
	BYTE byAttackType;		// 进攻类型
	int nStartTime;			// 开始时间
	int nOverTime;			// 结束时间
	int nActivityCostWanted;// 活动期间消耗通缉令(征讨令)
	int nActivityAwardRate;	// 活动奖励倍数
	int nAttackRate;		// 进攻倍数
	int nCostWanted;		// 消耗征讨令通缉令(征讨令)

	// 是否在活动中
	bool IsActivity(int nTime) const
	{
		return nTime >= nStartTime && nTime <= nOverTime ? true : false;
	}
};

// 叛军排行榜奖励配置
struct RebelRankAward_Config
{
	WORD wRank;			// 排名
	int nExploitAward;	// 功勋排行奖励
	int nDamageAward;	// 伤害排行奖励
};

// 回收返还配置
struct RecycleReturn_Config
{
	BYTE byObjectType;		// 回收对象类型
	BYTE byObjectLevel;		// 回收对象级别
	DWORD dwObjectId;		// 回收返还的对象Id
	DWORD dwNeedExp;		// 需要的经验
};

// 夺宝关卡配置
struct DuobaoMap_Config
{
	WORD wRoleLevel;
	int nWinGold;			// 胜利金币
	int nFailGold;			// 失败金币
	DWORD dwExp;			// 经验奖励
	WORD wPFGitfId;			// 翻牌随机礼包ID
	vector<DWORD> vMapList;	// 关卡列表

	void Init()
	{
		vMapList.clear();
	}
};

// 夺宝掉落配置
struct DuobaoDrop_Config
{
	BYTE byQuality;		// 品质
	WORD wUserRate;		// 用户概率
	WORD wRobotRate;	// 机器人概率
	WORD wTimesOfSureDrop; // 必掉次数(夺宝到一定次数时,一定会掉落)
};

// 夺宝进度奖励配置
struct DuobaoPregrossReward_Config
{
	WORD wId;	// 进度ID	
	WORD wTimes; // 夺宝次数
	vector<Item> vRewardItemList; // 奖励物品列表
};


// Vip配置
struct Vip_Config
{
	WORD wLevel;			// 等级
	DWORD dwNeedExp;		// 升级需要的经验
	vector<int>	vVipList;	// Vip特权值列表
};

// VIPGM配置
struct VipGM_Config
{
	WORD wGMLevel;	// VipGm等级
	DWORD dwNeedRechargeAmount;	// 需要的充值额度
};

// 刷新商店槽位配置
struct RefreshShopSlot_Config
{
	BYTE bySlot;								// 槽位
	//BYTE byIsCanRepeat;							// 是否可以重复
	vector<ShopGoodsData_Config*> vGoodsList;	// 商品列表
};

// 工会建筑配置
struct GuildBuilding_Config
{
	WORD wBuildingId; // 建筑ID
	WORD wNeedGuildLevel; // 需要的工会等级
};

// 世界BOSS
struct WorldBoss_Config
{
	WORD wBossLevel;		// BOSS等级
	WORD wMapId;			// 对应关卡ID
	int nLuckRewardRate;	// 幸运奖励几率
	Item tLuckRewardItem;	// 幸运奖励物品(钻石)
	Item tKillRewardItem;	// 幸运奖励物品(钻石)
	//WORD wKillRewardId;		// 最终一击奖励(物品) 
};

// 世界BOSS开放时间
struct WorldBossOpenTime_Config
{
	int nWeekDay;		// 星期几
	int nStartTime;		// 开始时间[小时:分钟]
	int nOverTime;		// 结束时间
	int nRewardTime;	// 奖励时间
};

// 世界BOSS排行奖励配置
struct WorldBossRankReward_Config
{
	WORD wRank; // 排名
	vector<Item> vHonorRewardList; // 荣誉奖励列表
	vector<Item> vDamageRewardList; // 伤害奖励列表
};

// 世界BOSS荣誉奖励配置
struct WorldBossHonorReward_Config
{
	WORD wRewardId;		// 奖励ID
	int nNeedHonor;		// 需要荣誉
	Item tRewardItem;	// 奖励物品
};

// 世界BOSS击杀奖励配置
struct WorldBossKillReward_Config
{
	WORD wRewardId;		// 奖励ID
	WORD nNeedBossLv;	// 需要BOSS等级
	Item tRewardItem;	// 奖励物品
};

// 世界BOSS刷新时间配置
struct WorldBossRefreshTime_Config
{
	
};

// 卡牌配置
struct Card_Config
{
	DWORD dwInfoId;	// 信息ID
	BYTE byType;	// 类型
	BYTE byQuality;	// 品质
	DWORD dwToExp;	// 作为升级材料获得的经验
	DWORD dwDecomposeReturnStarSoul; // 分解返还星魂
	int nResetCostDiamond;	// 重生消耗钻石
	vector<Property> vBaseProList;	// 基础属性列表
	vector<Property> vLevelProList;	// 等级属性列表
	vector<Property> vBreakProList;	// 突破属性列表
};

// 卡组关卡配置
struct CardGroupMap_Config
{
	WORD wMapIdx;		// 关卡索引
	DWORD dwMapId;		// 关卡ID
	WORD wCardGroupId;	// 卡组ID
	BYTE byIsTheLastMap;// 是否最后的关卡
};

// 高级卡组配置
struct HighCardGroup_Config
{
	DWORD dwCardId;		// 卡牌ID
	int nNormalWeight;	// 正常权值
	int nMustWeight;	// 保底权值
};

// 收藏组配置
struct CollectionGroup_Config
{
	WORD wGroupId; // 组ID
	vector<DWORD> vCostCardList;	// 消耗的卡牌列表
	vector<Property> vActivateProList;	// 激活属性列表
	vector<Property> vUpgradeProList;	// 升级属性列表
	int nActivateCollectionValue; // 激活获得的收藏值
	int nUpgradeCollectionValue;	// 升级获得的收藏值
};

// 收藏书配置
struct CollectionBook_Config
{
	WORD wBookId; // 书本ID
	int nNeedCollectionValue; // 需要的收藏值
	vector<DWORD> vCollectionGroupList; // 收藏组列表
};

// 占卜成就
struct ZhanBuAchievement_Config
{
	WORD wLevel; // 等级
	DWORD dwNeedCollectionValue; 
	vector<Property> vProList; // 获得的属性列表
};

// 卡牌升级配置
struct CardLevelUp_Config
{
	BYTE byQuality;		// 品质
	WORD wLevel;		// 等级
	DWORD dwExp;		// 升级经验
	DWORD dwTotalExp;	// 升级总经验
	WORD wNeedRoleLevel;// 需要角色等级
};

// 卡牌突破配置
struct CardBreak_Config
{
	BYTE byQuality;		// 品质
	WORD wBreakLevel;	// 突破等级
	WORD wNeedCardLevel;	// 需要卡牌等级
	WORD wCostSameCardNum;	// 消耗同样(同名)卡牌数量
	vector<CostData> vCostMoneyList; // 消耗的货币列表
	vector<Item> vCostItemList;  // 消耗的物品列表
};

// 卡槽配置
struct CardSlot_Config
{
	WORD wCardSlot; // 卡槽
	WORD wNeedRoleLevel; // 需要的角色等级
};


// 积分赛机器人
struct ScoreMatchRobot_Config
{
	DWORD dwRobotId;		// 机器人Id
	DWORD dwFightingPower;	// 战力
	WORD wServerId;			// 假装所在的服务器
};

// 积分赛挑战任务配置
struct ScoreMatchBattleTask_Config
{
	WORD wTaskId;			// 任务ID
	WORD wBattleTimes;		// 需要的挑战次数
	vector<Item> vRewardItemList; // 奖励物品列表
};

// 积分赛排行奖励
struct ScoreMatchRankReward_Config
{
	WORD wRank;	// 排行
	vector<Item> vRewardItemList;	// 奖励物品列表
	int nTitleId;	// 奖励的称号ID
};

// 积分赛活动时间
struct ScoreMathActivityTime_Config
{
	WORD wIndex;		// 活动顺序索引
	DWORD dwStartDate;	// 开始日期
	DWORD dwStartTime;	// 开始时间
	DWORD dwOverDate;	// 结束日期
	DWORD dwOverTime;	// 结束时间
};

// 积分赛查找目标配置
struct ScoreMatchSearchTarget_Config
{
	WORD wType;			// 类型
	WORD wTargetNum;	// 查找目标数量
	vector<DWORD> vBattleValueRateMinList;	// 战力区域下限比例列表
	vector<DWORD> vBattleValueRateMaxList;	// 战力区域上限比例列表
};

// 积分赛服务器组配置
struct ScoreMatchServerGroup_Config
{
	DWORD dwServerId;
	WORD wRankGroupId;			// 排行榜组ID(大区)
	vector<DWORD> vGroupList;	// 服务器组列表
}; 

// 争霸赛排行配置
struct WarcraftRank_Config
{
	WORD wRank;					// 排名
	WORD wTargetRankInterval;	// 挑战目标排名间隔
	int nAwardPrestige;			// 奖励竞技场声望
	int nAwardGold;				// 奖励金币
};

// 称号配置
struct Title_Config
{
	int nTitleId;		// 称号ID
	int nType;			// 类型(1=称号,2=昵称)
	int nValidTime;		// 有效时间(分钟,0=永久有效)
	vector<Property> vProList; // 获得的属性列表
};


// 奖励配置
struct Reward_Config
{
	int nId;	// 奖励信息ID
	int nType;	// 奖励类型(作为处理奖励内容逻辑,可以通用)
	vector<Item> vItemList;	// 奖励物品列表
};

// 活动配置基类
struct ActivityBase_Config
{
	int nTimeType;	// 时间类型
	int nDays;		// 开服多少天后有效(天)

	vector<DWORD> vOpenServerList; // 开放的服务器列表(0=表示所有服务器开放)

	int nStartDate;			// 开始日期(年/月/日)
	int nStartTime;			// 开始时间(小时/分钟)
	int nOverDate;			// 结束日期(年/月/日)
	int nOverTime;			// 结束时间(小时/分钟)

	int nRewardOverDate;	// 奖励结束日期(年/月/日)
	int nRewardOverTime;	// 奖励结束时间(小时/分钟)

	bool IsOpenServer(DWORD dwServerId) const
	{
		if (!vOpenServerList.size())
			return true;

		for (int i = 0; i < vOpenServerList.size(); ++i)
		{
			if (vOpenServerList[i] == dwServerId)
				return true;
		}
		return false;
	}
};

// 活动配置
struct Activity_Config : public ActivityBase_Config
{
	int nSystemId; // 系统ID
	WORD wDataId;	// 数据Id
// 	int nTimeType;	// 时间类型
// 	int nDays;		// 开服多少天后有效(天)
	WORD wNeedRoleLevel;	// 需要角色等级
//	vector<DWORD> vOpenServerList; // 开放的服务器列表(0=表示所有服务器开放)
// 	int nStartDate;			// 开始日期(年/月/日)
// 	int nStartTime;			// 开始时间(小时/分钟)
// 	int nOverDate;			// 结束日期(年/月/日)
// 	int nOverTime;			// 结束时间(小时/分钟)
// 	int nRewardOverDate;	// 奖励结束日期(年/月/日)
// 	int nRewardOverTime;	// 奖励结束时间(小时/分钟)
};

// 节日活动配置
struct FestivalActivity_Config : public ActivityBase_Config
{
	int nActivityType;// 活动类型
// 	int nTimeType;	// 时间类型
// 	int nDays;		// 开服多少天后有效(天)
//	vector<DWORD> vOpenServerList; // 开放的服务器列表(0=表示所有服务器开放)
// 	int nStartDate;			// 开始日期(年/月/日)
// 	int nStartTime;			// 开始时间(小时/分钟)
// 	int nOverDate;			// 结束日期(年/月/日)
// 	int nOverTime;			// 结束时间(小时/分钟)
// 	int nRewardOverDate;	// 奖励结束日期(年/月/日)
// 	int nRewardOverTime;	// 奖励结束时间(小时/分钟)	
	vector<DWORD> vParamList;// 活动参数列表
};

// 限时活动配置
struct LimitActivity_Config
{
	WORD wDataId;					// 数据ID
	WORD wType;						// 限时活动类型
	DWORD dwContext;				// 上下文
	WORD wRewardTimesMax;			// 奖励次数上限
	vector<Item> vRewardItemList;	// 奖励物品列表
};

// 节日活动目标
struct FestivalActivtiyTarget_Config
{
	WORD wTargetId;					// 目标ID
	WORD wLogicType;				// 逻辑类型
	int nX;							// 参数X
	int nY;							// 参数Y
	BYTE byIsAsc;					// 是否升序(暂时不配置)
	WORD wRewardTimes;				// 奖励次数
	BYTE byIsDailyReset;			// 是否每日重置
	BYTE byIsStageTarget;			// 是否阶段目标
	BYTE byIsNeedTake;				// 是否需要领取
	vector<Item> vRewardItemList;	// 奖励物品列表	

	// 	BYTE byIsTheSameDayValid;		// 是否当天有效
	// 	BYTE byIsFromTheSameDayValid;	// 是否当天开始有效
	//	BYTE byIsCompletionTarget;		// 是否完成度目标
	//	WORD wBelongActivityDay;		// 所属活动天数
};

// VIP每日礼包 
struct VipDailyGift_Config
{
	WORD wVipLevel;		
	vector<Item> vItemList;	// 奖励的物品列表
};

// VIP周礼包
struct VipWeekGift_Config
{
	WORD wGiftId;
	WORD wBuyNumMax;			// 购买数量上限
	WORD wNeedVipLevel;			// 需要VIP等级
	vector<CostData> vCostList; // 消耗列表
	vector<Item> vItemList;		// 物品列表
};

// VIP周礼包随机库
struct VipWeekGiftRandList_Config
{
	WORD wGiftRandNum;					// 礼包随机数量
	vector<RateDataEx> vGiftRandList;	// 礼包随机列表
};

// 今天推荐充值配置
struct TodayRecommendRecharge_Config
{
	WORD wGiftId;	// 礼包ID
	WORD wVipLevel;	// VIP等级
	int nRate;	// 随机几率
	WORD wRechargeId;	// 充值ID
	vector<Item> vItemList;	// 物品列表
};


// 等级商店配置
struct LevelShopData_Config
{
	WORD wRoleLevel;
	int nValidTime;				// 有效时间
	vector<DWORD> vGoodsList;	// 商品列表
};

// 开服基金奖励配置
struct OpenServerFundReward_Config
{
	WORD wRewardId;			// 奖励ID
	WORD wNeedRoleLevel;	// 需要的角色等级
	int nDiamond;			// 奖励的钻石
};

// 全民福利配置
struct AllPeopleWelfare_Config
{
	WORD wWelfareId;		// 福利ID
	WORD wNeedBuyNum;		// 需要购买的人数
	vector<Item> vItemList; // 福袋的物品列表
};

// 充值
struct Recharge_Config
{
	WORD wId;			// 充值ID
	//int nAmount;		// 需要的人民币(元)
	int nRMB;			// 需要的人民币(元)
	int nUSD;			// 需要的美元(分)
	int nYCoin;			// 越南版本使用的货币
	string strName;	// 商品名称
	//vector<Item> vRewardItemList;	// 充值奖励列表(已不使用)
	int nFirstRechargeDiamond;		// 首充奖励钻石
	int nRechargeDiamond;		// 获得的钻石
	int nVipExp;		// 获得的VIP经验
	int nExtraDiamond;	// 额外奖励的钻石
	string strRechargeStrId; // 充值字串ID
};

// 月卡配置
struct MonthCard_Config
{
	WORD wCardId;			// 卡ID
	int nSellPrice;			// 出售价格(人民币)
	int nSellPrice_USD;		// 出售价格(美分)
	int nBuyRewardDiamond;	// 购买后立即获得钻石
	int nDailyRewardDiamond;// 每天赠送钻石
	int nValidTime;			// 有效时间(天数,0=表示永久)
	int nVipExp;			// 获得的VIP经验
	Item tRewardItem;		// 奖励物品
	string strName;			// 月卡名称
	string strRechargeStrId; // 充值字串ID
};

// 限时优惠商店
struct LimitPreferentialShop_Config
{
	WORD wRoleLevelMin;			// 等级下限
	WORD wRoleLevelMax;			// 角色等级上限
	WORD wRandGoodsNum;			// 随机商品数量
	vector<DWORD> vGoodsList;	// 商品列表
};

// 限时优惠充值
struct LimitPreferentialRecharge_Config
{
	WORD wRechargeId;	// 充值ID(对应充值配置表ID)
	int nRate;			// 随机概率
	int nRewardDiamond; // 奖励钻石
	int nValidTime;		// 有效时间(秒)
};

// 限时优惠福利
struct LimitPreferentialWelfare_Config
{
	WORD wWelfareId;		// 奖励ID
	WORD wNeedBuyTimes;		// 需要购买次数
	vector<Item> vItemList; // 奖励的物品列表
};

// 等级礼包配置
struct LevelGift_Config
{
	WORD wLevel;
	vector<Item> vItemList; // 奖励的物品列表
};

// 在线奖励时间数据配置
struct OnlineRewardTime_Config
{
	DWORD dwTime; // 时间(秒)
	vector<Item> vRewardList; // 奖励列表
};

// 在线奖励配置
struct OnlineReward_Config
{
	WORD wDays; // 天数
	vector<OnlineRewardTime_Config> vTimeDataList; // 时间数据列表
};

// 7天登录配置
struct SevenDayLogin_Config
{
	WORD wDays; // 天数
	vector<Item> vRewardList; // 奖励列表
};


// 激活码配置
struct ActivationCode_Config
{
	DWORD dwId;	// ID
	string strCode;	// 激活码
	DWORD dwRewardId; // 奖励ID
	//DWORD dwItemId; // 奖励的礼包物品ID
};

// 激活码奖励配置
struct ActivationCodeReward_Config
{
	WORD wRewardId;
	DWORD dwItemId;	// 奖励的礼包物品ID
	int nValidDate;	// 有效日期[年:月:日]
	int nValidTime; // 有效时间[小时:分钟]
};

// 热云统计配置
struct ReyunStatistic_Config
{
	WORD wType;	// 行为类型
	string strPath;	// 路径
	string strContext; // 内容
};

// PVP挑战奖励
struct PVPBattleReward_Config
{
	WORD wBattleTimes;	// 战斗次数
	WORD wType;			// 类型(1=每日,2=每周)
	vector<Item> vItemList;	// 奖励物品列表
};

// PVP排名奖励
struct PVPRankReward_Config
{
	WORD wMinRank;	// 排名下限
	WORD wMaxRank;	// 排名上限
	vector<Item> vItemList;
	DWORD dwTitleId; // 称号Id
};

// PVP活动时间配置
struct PVPActivityTime_Config
{
	DWORD dwWeekDay; // 周几
	vector<TimeData> vTimeList; // 时间列表
// 	int nStartTime;	// 开始时间
// 	int nOverTime;	// 结束时间
};

// 攻城章节(波)配置
struct AttackCityChapter_Config
{
	WORD wChapter; // 章节
	BYTE byIsBoss; // 是否boss
	WORD wResetInitChapter; // 重置时初始波数
	WORD wRefreshMonsterNum; // 刷怪数量
	vector<DWORD> vMapList;	// 关卡列表
	vector<DWORD> vMapRateList;	// 关卡概率列表
	
	vector<Property> vMonsterProList; // 怪物属性列表
	WORD wRefreshItemNumMin;	// 刷宝箱数量下限
	WORD wRefreshItemNumMax;	// 刷宝箱数量上限
	DWORD dwRefreshItemId;	// 刷出的宝箱Id(物品Id)

	WORD wBossItemNumMin;	// Boss箱数量下限
	WORD wBossItemNumMax;	// Boss宝箱数量上限
	DWORD dwBossItemId;	// Boss的宝箱Id(物品Id)

	DWORD dwBattleScore;	// 挑战积分
	DWORD dwKillScore;	// 击杀积分
	vector<Property> vBattleMoneyList; // 挑战奖励货币列表
	vector<Item> vKillRewardItemList;	// 击杀奖励物品列表
};

// 攻城boss奖励配置
struct AttackCityBossReward_Config
{
	WORD wChapter;	// 章节
	vector<Item> vRewardItemList; // 奖励物品列表
};

// 攻城虚弱配置
struct AttackCityWeak_Config
{
	DWORD dwHPRemainRateMin; // hp剩余比例下限
	DWORD dwHPRemainRateMax; // hp剩余比例下限
	WORD wWeakCD; // 虚弱CD(秒)
};

// 攻城鼓舞配置
struct AttackCityEncourage_Config
{
	WORD wTimes; // 鼓舞次数
	int nScoreAddRate; // 积分加成比例
	CostData tCostData; // 消耗数据
	Property tAddPro;	// 影响的英雄属性
};

// 攻城活动时间
struct AttackCityActivityTime_Config
{
	WORD wWeekDay; // 周几
	int nStartTime;	// 开始时间
	int nOverTime;	// 结束时间
};

// 攻城排名奖励配置
struct AttackCityRankReward_Config
{
	WORD wRankMin;
	WORD wRankMax;
	vector<Item> vItemList;
};

// 无尽之地波配置
struct EndlessLandWave_Config
{
	WORD wWave; // 波数
	WORD wInitWave; // 开始时初始的波
	WORD wMonsterNum; // 本波怪物数量
	int nEndlessStone; // 通关获得的无尽石
	vector<Item> vRewardItemList;  // 通关奖励物品ID列表
	vector<DWORD> vMonsterList; // 怪物列表
	vector<DropData> vDropDataList; // 掉落数据列表
};

// 无尽之地碰撞配置
struct EndlessLandCollision_Config
{
	WORD wRewardId; // 奖励ID
	DWORD dwRate; // 概率
	vector<Item> vRewardItemList; // 奖励的物品列表
};

// 无尽之地连杀奖励
struct EndlessLandContinueKillReward_Config
{
	DWORD dwContinueKillNum; // 连杀数量
	int nDiamond; // 奖励的钻石
};

// 无尽之地排行奖励配置
struct EndlessLandRankReward_Config
{
	WORD wRankMin;	// 排行下限
	WORD wRankMax;	// 排行上限
	vector<Item> vItemList; // 奖励物品列表
	DWORD dwTitleId; // 奖励的称号ID
};

// 无尽之地挑战消耗配置
struct EndlessLandBattleCost_Config
{
	WORD wBattleTimes;		// 挑战次数
	CostData tCostCurrency;	// 消耗货币
	Item tCostItem;			// 消耗物品
};


// 爵位配置
struct Nobility_Config
{
	WORD wLevel;
	DWORD dwNeedPower;	// 需要的战力
	vector<Property> vProList; // 获得的属性列表
};

// 宝石配置
struct Gem_Config
{
	DWORD dwGemId;					// 宝石ID
	WORD wType;						// 宝石类型(1、普通 2、珍贵)
	WORD wComposeCostNum;			// 合成消耗的数量
	DWORD dwNewGemFromCompose;		// 合成后获得的新宝石
	int nGemCoin;					// 分解获得的货币值(宝石逼)
	WORD wSkillId;					// 技能ID
	WORD wSkillLevel;				// 技能等级
	vector<Property> vInitProList;	// 初始属性列表
};

// 宝石技能
struct GemSkill_Config
{
	WORD wSkillId;			// 技能ID
	WORD wSkillLevel;		// 技能等级
	vector<Property> vProList;	// 属性列表
};

// 宝石孔位
struct GemHole_Config
{
	WORD wHole; // 孔位
	WORD wNeedRoleLevel;	// 需要的角色等级		
	WORD wIsDefaultOpen; // 是否默认开启
	Item vCostItem; // 消耗的道具
};

// 宝石镶嵌配置
struct GemInlay_Config
{
	WORD wType; // 宝石类型	
	WORD wInlayNumLimit; // 同一个阵位镶嵌数量上限
};

// 豪华签到配置
struct LuxurySign_Config
{
	WORD wSignInId;		// 签到ID
	WORD wLevelMin;		// 等级下限
	WORD wLevelMax;		// 等级上限
	//int nAmount;		// 充值额度
	WORD wRechangeId;	// 充值Id
	vector<RateDataEx> vRewardRandList; // 奖励随机列表
};


// 豪华签到奖励数据
struct LuxurySignInReward_Config
{
	DWORD dwRewardId; 
	vector<Item> vItemList;
};

// 超值折扣购买奖励配置
struct SuperDiscountBuyReward_Config
{
	WORD wDay;	// 天数
	vector<Item> vItemList; // 奖励物品列表
};

// 超值折扣累积奖励配置
struct SuperDiscountAccumulateReward_Config
{
	WORD wTimes;	// 累积次数
	WORD wNeedBuyTimes; // 需要的购买次数
	vector<Item> vItemList; // 奖励物品列表	
};

// 角色排行榜奖励
struct RoleRankReward_Config
{
	WORD wRankType;	// 排行榜类型
	WORD wRankMin;	// 排行下限
	WORD wRankMax;	// 排行上限
	vector<Item> vItemList; // 奖励物品列表
	DWORD dwTitleId; // 奖励的称号ID
};

// 精准推荐商品数据
struct AccurateRecommendGoods_Config
{
	WORD wGoodsId;		// 商品Id
	DWORD dwItemId;		// 物品Id
	DWORD dwItemNum;	// 物品数量
	WORD wBuyNumMax;	// 购买数量上限(0=表示无限制)
	WORD wRechargeId;	// 充值Id
};

// 场景配置
struct Scene_Config
{
	DWORD dwSceneId;  // 场景ID
	DWORD dwMapId; // 地图
	BYTE byIsSyncMove;
	vector<DWORD> vMoveArea;		 // 可行走区域
	vector<DWORD> vHeroBirthArea;	 // 英雄出生区域
	vector<DWORD> vHeroReliveArea;	 // 英雄复活区域
	vector<DWORD> vMonsterRefrshArea;// 怪物刷新区域
	vector<DWORD> vItemRefrshArea;	 // 场景物品刷新区域
};

// 组队副本配置
struct TeamDungeon_Config
{
	DWORD dwDungeonId;			// 副本ID
	WORD wRecommendLevelMin;	// 推荐等级段下限
	WORD wRecommendLevelMax;	// 推荐等级段上限
	DWORD dwWorldMapId;			// 世界地图ID
	vector<DWORD> vTaskList;	// 任务链ID列表
};

// 组队副本抽奖
struct TeamDungeonLottery_Config
{
	DWORD dwItemId; // 物品ID
	DWORD dwItemNum;	// 物品数量
	int nWeight;	// 权值
	BYTE byIsNeedRecord; // 是否需要记录
};

// 组队副本任务链
struct TeamDungeonTaskLink_Config
{
	DWORD dwTaskListId;				// 任务列表Id		
	DWORD dwBossMapId;				// BOSS关卡
// 	DWORD dwBossKillRewardId;		// Boss击杀奖励
// 	DWORD dwMVPRewardId;			// MVP奖励

	vector<DWORD> vMVPRewardList;		// MVP奖励列表
	vector<DWORD> vBossKillRewardList;	// Boss击杀奖励列表
	
	MapPos tBossScenePos;			// BOSS在场景的坐标
	WORD wTaskNum;					// 任务数量
	DWORD dwTaskLimitTime;			// 整个任务限制时间(秒)
	vector<vector<DWORD>> vTaskList;// 任务列表

	DWORD GetMVPReward(WORD wTimes) const
	{
		if (!vMVPRewardList.size())
			return 0;

		if (wTimes >= vMVPRewardList.size())
			return vMVPRewardList[vMVPRewardList.size()-1];

		return vMVPRewardList[wTimes];
	}

	DWORD GetKillReward(WORD wTimes) const
	{
		if (!vBossKillRewardList.size())
			return 0;

		if (wTimes >= vBossKillRewardList.size())
			return vBossKillRewardList[vBossKillRewardList.size() - 1];

		return vBossKillRewardList[wTimes];
	}
};

// 组队任务数据
struct TeamDungeonTask_Config
{
	DWORD dwTaskId; // 任务ID
	WORD wTaskType;	// 任务类型
	DWORD dwSceneId; // 场景地图ID(0=表示不需要场景)
	WORD wTaskNpc; // 任务NPC
	DWORD dwExtraRewardId; // 额外奖励ID
	vector<DWORD> vTaskParam; // 任务参数列表

	vector<DWORD> vScoreList;		// 评分列表
	vector<DWORD> vEnergyList;		// 法阵能量列表
	vector<DWORD> vRewardItemList;	// 任务奖励物品列表

	DWORD GetTaskParam(int nIndex) const
	{
		if (nIndex >= vTaskParam.size())
			return 0;
		return vTaskParam[nIndex];
	}

	int GetScoreIndex(DWORD dwValue) const
	{
		for (size_t i = 0; i < vScoreList.size(); ++i)
		{
			if (dwValue <= vScoreList[i])
				return i;
		}
		return 0;
	}

	DWORD GetEnergy(int nIndex) const
	{
		if (nIndex >= vEnergyList.size())
			return 0;
		return vEnergyList[nIndex];
	}

	DWORD GetRewardItem(int nIndex) const
	{
		if (nIndex >= vRewardItemList.size())
			return 0;
		return vRewardItemList[nIndex];
	}
};

// 组队BOSS奖励
struct TeamDungeonBossReward_Config
{
	DWORD dwDungeonId;		 // 副本ID
	int nEnergyMin;			 // 能量下限
	int nEnergyMax;			 // 能量上限
	int nDamageRateMin;		 // Boss伤害比例下限
	int nDamageRateMax;		 // Boss伤害比例上限
	vector<Item> vRewardList;// 奖励列表
};

// 登录奖励配置
struct LoginReward_Config
{
	WORD wDays; // 登录天数
	vector<Item> vRewardItemList; // 奖励物品列表
};

// 封测返利 
struct ClosedTestRebate_Config
{
	WORD wId;
	DWORD dwChannel;
	string strAcount;
	int nDiamond;
};

// 王者赛段位配置
struct KingMatchStage_Config
{
	WORD wStageLevel; // 段位等级
	WORD wLevelUpNeedStar; // 升级需要的星数
	BYTE byIsDropStage;	// 是否会降段
	BYTE byIsDropStar;	// 是否会掉星
	BYTE byIsLastStage;	// 是否最后的段位
	BYTE byIsBroadcastLeveUp;	// 是否要广播升级
	vector<RateDataEx>	vMatchingStageList; // 可匹配的段位列表
	vector<Item> vDailyFirstWinReward; // 每日首胜奖励
	vector<Item> vLevelUpReward; // 升级奖励
	vector<Item> vSeasonReward;	// 赛季奖励
};

// 王者赛赛季时间
struct KingMatchSeasonTime_Config
{
	WORD wSeason;		// 赛季
	DWORD dwStartDate;	// 开始日期
	DWORD dwStartTime;	// 开始时间
	DWORD dwOverDate;	// 结束日期
	DWORD dwOverTime;	// 结束时间
};

// 王者赛季任务
struct KingMatchSeasonTask_Config
{
	WORD wWinTimes; // 获胜次数
	vector<Item> vRewardList; // 奖励列表
};

// 王者赛排行奖励
struct KingMatchRankReward_Config
{
	WORD wRankMin;	// 排行下限
	WORD wRankMax;	// 排行上限

	DWORD dwTitleId; // 称号ID
	vector<Item> vRewardList; // 奖励列表
};

// 圣诞节BOSS等级配置
struct ChristmasBossLevel_Config
{
	WORD wType;	// boss类型
	DWORD dwLevel; // boss等级
	DWORD dwBossHP;	// bossHP
	vector<Item> vKillRewardList; // 击杀奖励列表
	vector<Item> vBossRewardList; // Boss奖励列表
};

// 圣诞节BOSS配置
struct ChristmasBoss_Config
{
	WORD wType;							// boss类型
	DWORD dwReviveTime;					// 复活时间
	vector<DWORD> vLimitSnowBallList;	// 可对Boss攻击的雪球列表
};

// 圣诞节雪球配置
struct ChristmasSnowball_Config
{
	DWORD dwSnowballId; // 雪球ID
	DWORD dwDamageMin;	// 伤害下限
	DWORD dwDamageMax;	// 伤害上限
	vector<DWORD> vRewardItemList;	// 奖励物品列表
	vector<DWORD> vRewardNumList;	// 奖励数量列表
	vector<DWORD> vRewardRateList;	// 奖励几率列表
};

// facebook好友邀请配置
struct FacebookFriendInvite_Config
{
	DWORD dwInviteNum;			// 邀请数量
	vector<Item> vRewardList;	// 奖励列表
};

// facebook好友召唤配置
struct FacebookFriendSummon_Config
{
	DWORD dwSummonNum;			// 召唤数量
	vector<Item> vRewardList;	// 奖励列表
};


// facebook帐号绑定配置
struct FacebookAccountBinding_Config
{
	vector<Item> vRewardList;	// 奖励列表
};

// facebook分享配置
struct FacebookShare_Config
{
	vector<Item> vRewardList;	// 奖励列表	
};

// facebook评论配置(强行加入fb活动)
struct FacebookCommend_Config
{
	vector<Item> vRewardList;	// 奖励列表	
};

// 充值回馈配置
struct RechargeReward_Config
{
	WORD wDataId;
	WORD wRechargeId; // 充值ID
	vector<Item> vRewardList;	// 奖励列表	
};

// 充值额外奖励
struct RechargeExtraReward_Config
{
	WORD wDataId;			// 活动数据ID
	WORD wFindId;				// 查找ID
	WORD wNeedRechargeDay;		// 充值ID
	vector<Item> vRewardList;	// 奖励列表	
};

// apple_iap 配置
struct AppleIap_Config
{
	string strIapId;
	DWORD dwRechargeId;	// 充值ID
};


// 累充奖励配置
struct TotalRechargeReward_Config
{
	WORD wDataId;	// 数据ID
	WORD wRewardId;	// 奖励Id
	DWORD dwTotalRchargetAmount; // 需要的总充值额度
	vector<Item> vRewardList; // 奖励列表
};

// 抽奖配置
struct NDLottery_Config
{
	DWORD dwItemId;		// 物品ID
	DWORD dwItemNum;	// 物品数量
	int nWeight;		// 权值
	BYTE byIsNeedRecord;// 是否需要记录
};

// 扭蛋抽奖消耗
struct NDLotteryCost_Config
{
	DWORD dwDataId;			// 数据ID
	DWORD dwCostItemId;		// 消耗物品Id

	int	nSingleCostDiamond;	// 普通抽消耗钻石
	int nOenKeyCostDiamond;	// 一键抽消耗钻石	
};

// 热卖商品
struct HotSaleGoods_Config
{
	WORD wGoodsId;					// 商品Id
	vector<Item> vItemList;			// 物品列表
	vector<DWORD> vBuyNumMaxList;	// 购买数量上限列表(0 = 表示无上限)
	vector<CostData> vCostList;		// 消耗列表
	vector<DWORD> vRechargeIdList;	// 充值ID列表
	vector<DWORD> vNeedLevelList;	// 需要的等级

	vector<DWORD> vResetDayList;	// 重置天列表（不填表示不重置）
};

// 转盘数据
struct Turntable_Config
{
	WORD wDataId;				// 活动数据ID	
	int nInitDiamond;			// 奖池初始钻石	
	DWORD dwCostItemId;			// 抽奖消耗的物品id	
	DWORD dwCostItemNum;		// 抽奖消耗的物品数量
	int nNormalLotteryScore;	// 普通抽奖获得积分
	int nOneKeyLotteryScore;	// 一键抽奖获得积分
	int nNormalLotteryCostDiamond; // 普通抽奖消耗的钻石
	int nOneKeyLotteryCostDiamond; // 一键抽奖消耗的钻石
	int nRankNeedScore;			// 入榜需要的积分
	int nNormalLotteryDiamond;	// 普通抽奖添加的奖池钻石
	int nOneKeyLotteryDiamond;	// 一键抽奖添加的奖池钻石
	int nOneKeyLotteryTimes;	// 一键抽奖次数
};

// 转盘抽奖数据
struct TurntableLottery_Config
{
	WORD wDataId;					// 活动数据ID
	WORD wIndex;					// 数据索引
	DWORD dwRewardItemId;			// 奖励物品ID
	DWORD dwRewardItemNumMin;		// 奖励物品数量下限
	DWORD dwRewardItemNumMax;		// 奖励物品数量上限
	int nRewardDiamondRate;			// 奖励钻石比例
	int nWeight;					// 权值
	bool bIsNeedRecord;				// 是否需要记录
};

// 转盘排行榜配置
struct TurntableRank_Config
{
	WORD wRank;
	vector<Item> vRewardItemList; // 奖励物品列表
};

// 限时产出
struct LimitProduct_Config
{
	WORD wMapType;	// 战斗副本类型
	int nStartDate; // 开始日期(指定日期 如:20180323 开服天数库 如 1 )
	int nOverDate;	// 结束日期
	vector<DWORD> vStartTimeList; // 开始时间列表
	vector<DWORD> vOverTimeList; // 开始时间列表
	//int nStartTime; // 开始时间(指定时间 如2230)
	//int nOverTime;	// 结束时间
	int nMultiple;	// 添加的倍数
};

// 阵营招募配置
struct CampRecruit_Config
{
	WORD wDataId;						// 数据id
	WORD wCamp;							// 阵营
	DWORD dwNormalRecruitScore;			// 普通招募积分
	DWORD dwOneKeyRecruitScore;			// 一键招募积分
	WORD wDailyFreeRecruitTimes;		// 每日免费招募次数
	WORD wOneKeyRecruitTimes;			// 一键招募次数
	DWORD dwNormalRecruitCostDiamond;	// 普通招募消耗钻石
	DWORD dwOneKeyRecruitCostDiamond;	// 一键招募消耗钻石
	vector<DWORD> vScoreTaskList;		// 积分任务列表
	vector<Item> vScoreTaskRewardList;	// 积分任务奖励列表

	// 取任务积分
	DWORD GetTaskScore(WORD wIndex) const
	{
		return wIndex >= vScoreTaskList.size() ? 9999999 : vScoreTaskList[wIndex];
	}

	// 取任务奖励物品
	const Item* GetTaskRewardItem(WORD wIndex) const
	{
		return wIndex >= vScoreTaskRewardList.size() ? NULL : &vScoreTaskRewardList[wIndex];
	}
};

// 阵营招募英雄配置
struct CampRecruitHero_Config
{
	WORD wDataId;		// 活动数据ID
	WORD wCamp;			// 阵营
	DWORD dwWeight;		// 权值
	Item tRewardItem;	// 奖励物品
	BYTE byIsNeedRecord;// 是否需要记录
};

// unlock活动奖励
struct UnlockActivityReward_Config
{
	WORD wRewardId;		// 奖励ID
	vector<Item> vRewardItemList; // 奖励物品列表
};

// 观看视频奖励
struct UnlockWatchingVideoReward_Config
{
	WORD wWatchingTimes;		// 观看次数
	vector<Item> vRewardItemList; // 奖励物品列表	
};

// 世界任务等级配置
struct WorldTaskLevel_Config
{
	WORD wDays;							// 开服天数
	WORD wWorldLevel;					// 世界等级
	map<WORD, vector<Item>> vRewardList;// 奖励列表[RoleLevel,RewardData]
};

// 世界等级任务ID列表
struct WorldLevelTaskIdList_Config
{
	WORD wTaskLevel;			// 任务等级
	int nExp;					// 经验
	vector<DWORD> vTaskIdList;	// 任务ID列表
};

// 世界等级任务事件
struct WorldLevelTaskEvent_Config
{
	WORD wTaskId;				// 任务ID
	WORD wTaskType;				// 任务类型
	int nX;						// 任务参数X
	int nY;						// 任务参数Y	
	vector<Item> vRewardList;	// 奖励列表
};

// 资源战额外奖励数据
struct ResourceWarExtraReward
{
	int nCostDiamond;				// 延长占领时间消耗的钻石
	WORD vRewardNeedTime;			// 奖励需要的时间
	vector<Item> tRewardItemList;	// 奖励的物品列表
};

// 资源战章节配置
struct ResourceWarChapter_Config
{
	WORD wChapter; // 章节
	//vector<DWORD> vResourceList;	// 资源列表(vector)
	map<DWORD, DWORD> resourceList;	// 资源列表(map)
};

// 资源战资源配置
struct ResourceWarResource_Config
{
	WORD wResourceId;	// 资源ID
	WORD wResourceType;	// 资源类型
	DWORD dwMapId;		// 关卡id
	int nCostDiamond;	// 挑战消耗的钻石
	int nBaseRewardResourceWarSocre;	// 基础奖励的资源战积分
	WORD wBaseProfitIntervalTime;		// 基础收益间隔时间
	//vector<Property> vBaseRewardCurrencyList;	// 基础奖励货币列表
	vector<ResourceWarExtraReward> vExtraRewardDataList;	// 额外奖励数据列表
};

// 资源战科技配置
struct ResourceWarTechnology_Config
{
	WORD wLevel;			// 等级
	DWORD dwNeedAccupyTime;	// 需要的占领时间(秒)
	int nBaseRewardAddRate; // 基础奖励加成比例(万分比)
};

// 资源战军团收益资源配置
struct ResourceWarGuildProfit_Config
{
	WORD wMemberNum;		// 需要占领成员的数量
	int nBaseRewardAddRate; // 基础奖励加成比例(万分比)
};

// 工会战星球配置
struct GuildWarStar_Config
{
	DWORD dwStarId; // 星球ID
	WORD wStarType;	// 星球类型(1=绿品 2=蓝品 3=紫品 4=橙品)
	vector<DWORD> vTowerList; // 星球塔列表
	DWORD dwMapId; // 关卡地图ID(积分模式)	
	vector<Item> vLocalProductList; // 特产列表
	vector<Item> vDailyRewardList;	// 每日奖励列表(对战模式)
	vector<Item> vLoserRewardList;	// 挑战失败奖励列表(积分模式)
	vector<Item> vWinnerRewardList;	// 挑战胜利奖励列表(积分模式)

	bool IsTower(DWORD dwTowerId) const
	{
		for (int i = 0; i < vTowerList.size(); ++i)
		{
			if (vTowerList[i] == dwTowerId)
				return true;
		}
		return false;
	}
};

// 工会战塔配置
struct GuildWarTower_Config
{
	DWORD dwTowerId; // 塔ID
	WORD wTowerType; // 塔类型
	DWORD dwMapId; // 关卡地图ID
	WORD wMonsterDefenseTimes; // 怪物防守次数
	WORD wDefenderNumMax;	// 驻守人数上限
	WORD wRoleDefenseTimes; // 角色防守次数
	DWORD dwDurability; // 耐久度
	vector<Item> vBreakTowerRewardList; // 攻破塔奖励列表
	vector<Item> vLoserRewardList;	// 挑战失败奖励列表
	vector<Item> vWinnerRewardList;	// 挑战胜利奖励列表
};

// 工会战排行奖励
struct GuildWarRankReward_Config
{
	WORD wRank; // 排名
	vector<Item> vRewardList; // 奖励列表
};

// 工会战对战奖励
struct GuildWarVSReward_Config
{
	WORD wVSType; // 对战类型
	WORD wVSResult; // 对战结果
	vector<DWORD> vBreakTowerNumList; // 攻破的塔数量
	vector<vector<Item>> vRewardList; // 奖励列表	
};

// 工会战对战活动时间
struct GuildWarVSActivityTime_Config
{
	WORD wTimeType; // 时间类型 (1 = 报名时间 2 = 对战时间 3 = 决定进攻者时间)
	DWORD dwDecideAttackerTime; // 决定攻击者时间[星期小时分钟], 如 11030 表示周一10点30分
	DWORD dwBeginTime; // 开始时间[星期小时分钟], 如 11030 表示周一10点30分
	DWORD dwEndTime; // 开始结束[星期小时分钟], 如 32030 表示周三20点30分 7=表示星期天
	vector<DWORD> vTakeRewardTime; // 领取奖励时间[星期几|星期几] 如: 1|3|5 表示星期一, 星期三, 星期五
};

// 工会战通用配置
// struct GuildWarCommon_Config
// {
// 	DWORD dwKey;
// 	DWORD dwValue;
// 	vector<DWORD> vList;
// };


// 工会战布阵
struct GuildWarOrganizeDefense_Config
{
	WORD wTowerType;				// 塔类型	
	vector<DWORD> vPowerIdxList;	// 战力索引列表
};

// 工会战大区配置
struct GuildWarRegion_Config
{
	WORD wRegionId; // 大区ID
	map<DWORD, DWORD> mServerList; // 服务器列表
};

// YGame支付配置
struct YGamePayment_Config
{
	string strPaymentId;	// 支付ID
	DWORD dwAssistId;		// 辅助Id
	DWORD dwRechargeId;		// 充值ID(todo::目前只有月卡用到)
	int nPaymentFromType;	// 支付来源类型(1=内购, 2=月卡, 3=网页普充, 4=网页首充 5=游戏内兑换(ycoin兑换gem))	
	int nYCoin;				// ycoin	
	int nVipExp;			// vip经验	
	int nDiamond;			// 获得的钻石	
	int nBonus;				// 奖励的钻石
	vector<Item> vItemList;	// 奖励物品列表
};

// 充值排行奖励 1.充值榜 2.消费榜
struct RechargeRankReward_Config
{
	WORD wRank; // 排名
	vector<Item> vRewardList; // 奖励列表
};

// 挂机掉落配置
struct HangUpDrop_Config
{
	DWORD dwDropItemId;
	DWORD dwDropItemNum;
	int nDropRate;
};

/*---------------------------------------------------------------------------------------------------*/
#pragma pack(pop)