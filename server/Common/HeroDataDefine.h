#ifndef _HERO_DATA_DEFINE_H_
#define _HERO_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 阵型数据
struct Formation
{
	DWORD dwHeroUID;	// 阵型上的英雄ID
	DWORD arEquipmentList[EQUIPMENT_TYPE_MAX];	// 装备列表
	DWORD arTreasureList[TREASURE_TYPE_MAX];	// 宝物列表	
	DWORD arGemList[MAX_GEM_HOLE_NUM];			// 宝石列表
	DWORD arCardList[MAX_CARD_SLOT_NUM];		// 卡牌列表
	DWORD dwBlessSpriteId;						// 祝福精灵ID(0=表示木有)
};

// 阵型装备数据
struct FormationEquipment
{
	WORD wInfoId;			// 装备信息ID
	WORD wStrengthenLevel;	// 强化等级
	BYTE byRefineLevel;		// 精炼等级
};

// 阵型宝物数据
struct FormationTreasure
{
	WORD wInfoId;			// 信息ID
	WORD wStrengthenLevel;	// 强化等级
	BYTE byRefineLevel;		// 精炼等级
};

// 客户端阵型数据
struct ClientFormation
{
	BYTE bySlot;		// 槽位
	WORD wHeroInfoId;	// 英雄信息ID
	BYTE byAwakenStar;	// 觉醒星级
	WORD wBreakLevel;	// 突破等级
	BYTE byQaulity;		// 品质
	FormationEquipment arEquipmentList[EQUIPMENT_TYPE_MAX];
	BYTE byEquipmentNum;
	FormationTreasure arTreasureList[TREASURE_TYPE_MAX];
	BYTE byTreasureNum;
};

// 客户端英雄基础数据
struct ClientHeroBaseData
{
	DWORD dwUID;		// ID	
	WORD wInfoId;		// 配置ID
	BYTE byLevel;		// 等级
	BYTE byQuality;		// 品质等级
	BYTE byQualityStage;// 品质阶段
	BYTE byDestinyLevel;// 天命等级
	BYTE byAwakenStar;	// 觉醒星级
	BYTE byAwakenLevel;	// 觉醒等级
	BYTE byBreakLevel;	// 突破等级
	BYTE byIsToBattle;	// 就否已上阵
	DWORD dwExp;		// 当前经验
// 	BYTE byTrainType;	// 培养类型
// 	BYTE byTrainLevel;	// 培养等级
	//int arTrainProValueList[HERO_TRAIN_PRO_NUM];	// 培养累积属性值列表
	Property arTrainProList[HERO_TRAIN_PRO_NUM];
	BYTE arTrainProLvList[HERO_TRAIN_PRO_NUM];		// 培养属性的当前等级列表
	// 觉醒装备的道具列表
	WORD arAwakenItemList[MAX_AWAKEN_EQUIP_ITEM_NUM];
	WORD wAwakenItemNum;
};

// 英雄培养属性
struct HeroTrainPro
{
	BYTE byProType;		// 属性类型
	BYTE byProLevel;	// 属性等级
	int nProValue;		// 属性值
	int nTmpProValue;	// 属性临时值(最近一次培养的结果,非0表示还木有替换)
};


// 英雄信息
struct HeroInfo
{
	DWORD dwUID;			// 唯一ID
	DWORD dwIntoId;			// 配置ID
	WORD wLevel;			// 等级
	DWORD dwExp;			// 经验
	WORD wQuality;			// 品质
	WORD wQualityStage;		// 品阶
	WORD wBreakLevel;		// 突破等级
	WORD wDestinyLevel;		// 天命等级(潜能)
	DWORD dwDestinyExp;		// 天命经验(潜能)
	WORD wAwakenStar;		// 觉醒星级
	WORD wAwakenLevel;		// 觉醒等级

	// 觉醒装备的道具列表
	WORD arAwakenItemList[MAX_AWAKEN_EQUIP_ITEM_NUM];
	WORD wAwakenItemNum;

	// 培养相关
	BYTE arTrainProLvList[HERO_TRAIN_PRO_NUM];		// 培养属性的当前等级列表
	int arTrainProValueList[HERO_TRAIN_PRO_NUM];	// 培养累积属性值列表
	int arLastTrainTmpProList[HERO_TRAIN_PRO_NUM];	// 上次培养缓存的属性列表
	BYTE arTrainRewardFlag[HERO_TRAIN_PRO_NUM];		// 培养奖励标志列表(1=表示可领取,0=已领取或冒得领取)

	// 作为回收使用
	DWORD dwTrainCostItemNum;	// 培养消耗的物品数量
	DWORD dwDestinyCostItemNum;	// 天命消耗的物品数量
	int nTrainCostDiamond;		// 培养消耗钻石
	int nTrainCostGold;			// 培养消耗金币
 	WORD arBreakCostHeroList[MAX_HERO_BREAK_COST_HERO_NUM];// toto::突破消耗的英雄列表 目前英雄的信息Id不会超过两字节
 	WORD wBreakCostHeroNum;

	void Init()
	{
		memset(this, 0, sizeof(HeroInfo));
	}
};

// 英雄
struct ClientHero : public HeroInfo
{
	BYTE byTrainType;	// 培养类型
	BYTE byTrainLv;		// 培养等级
};

// 玩家英雄信息
struct PlayerHeroInfo
{
	WORD wNormalFreeSummonTimes;	// 普通免费召唤次数
	DWORD dwNormalFreeSummonTime;	// 普通免费召唤时间
	DWORD dwHighFreeSummonTime;	// 高级免费召唤时间
	WORD wHighSummonTimes;		// 高级召唤次数
	WORD wTrainTimes;			// 培养次数

	HeroInfo arHeroList[MAX_HERO_BAG_NUM];	// 英雄列表
	WORD wHeroNum;							// 英雄列表数量

	// 阵型数据列表
	Formation arFormationList[MAX_BATTLE_HERO_NUM];

	// 援军列表
	DWORD arReinforcementList[MAX_REINFORCEMENT_NUM];

	// 英雄收集列表
	DWORD arCollectionList[MAX_HERO_NUM];
	WORD wCollectionNum;
};



#pragma pack(pop)
#endif
