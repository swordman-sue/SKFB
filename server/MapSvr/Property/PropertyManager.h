/**\file
属性管理器
\author Kevin
\date 2016/11/14
\note
*/

#pragma once
#include "DataDefine.h"
class Player;


// 属性来源ID(不是逻辑使用，只是作为方便定位问题)
enum PRO_FORM_ID
{
	PRO_FROM_TITLE = 1,					// 称号属性
	PRO_FROM_SPRITE,					// 精灵属性
	PRO_FROM_NOBILITY,					// 爵位属性
	PRO_FROM_TEAM_BREAK,				// 英雄突破队伍属性
	PRO_FROM_ZHANBU,					// 占卜属性
	PRO_FROM_HERO_BASE,					// 英雄基础属性
	PRO_FROM_HERO_BREAK,				// 英雄突破属性
	PRO_FROM_HERO_TRAIN,				// 培养属性
	PRO_FROM_HERO_DESTINY,				// 天命属性
	PRO_FROM_HERO_AWAKEN,				// 觉醒属性
	PRO_FROM_HERO_EVOLUTION,			// 进化属性	
	PRO_FROM_HERO_RELATION,				// 关联英雄属性
	PRO_FROM_EQUIM_RELATION,			// 关联装备属性
	PRO_FROM_TREASURE_RELATION,			// 关联宝物属性
	PRO_FROM_CARD_RELATION,				// 关联卡牌属性
	PRO_FROM_WORLD_HEART,				// 世界之心
	PRO_FROM_SPRITE_POKEDEX,			// 精灵图鉴
	PRO_FROM_GUILD_SKILL,				// 工会技能
	PRO_FROM_EQUIP_ENHANCE,				// 装备强化属性
	PRO_FROM_EQUIP_REFINE,				// 装备精炼属性	
	PRO_FROM_EQUIP_TALENT,				// 装备天赋属性
	PRO_FROM_EQUIP_ENHANCE_FORGE,		// 装备强化锻造属性
	PRO_FROM_EQUIP_REFINE_FORGE,		// 装备精炼锻造属性
	PRO_FROM_EQUIP_SUIT,				// 装备套装属性
	PRO_FROM_EQUIP_STARUP,				// 装备升星属性
	PRO_FROM_TREASURE_ENHANCE,			// 宝物强化属性	
	PRO_FROM_TREASURE_REFINE,			// 宝物精炼属性
	PRO_FROM_TREASURE_ENHANCE_FORGE,	// 宝物强化锻造属性	
	PRO_FROM_TREASURE_REFINE_FORGE,		// 宝物精炼锻造属性
	PRO_FROM_GEM_INIT,					// 宝石初始属性
	PRO_FROM_GEM_SKILL,					// 宝石技能属性
	PRO_FROM_CARD_BASE,					// 卡牌基础属性
	PRO_FROM_CARD_LEVEL,				// 卡牌等级属性
	PRO_FROM_CARD_BREAK,				// 卡牌突破属性
	PRO_FROM_SPRITE_BLESS_BASE,			// 精灵祝福基本属性
	PRO_FROM_SPRITE_BLESS_CONVERSION,	// 精灵祝福转化属性	
	PRO_FROM_TALENT_SKILL_CONVERSION,	// 天赋技能属性精灵祝福转化	
	PRO_FROM_REINFORCEMENT_ENCOURAGE,	// 援军助威属性
};

class CPropertyManager
{
public:
	CPropertyManager();
	~CPropertyManager();

	void Init(Player *pPlayer);

	// 取总战力
	DWORD GetAllFightingPower();

	// 计算属性
	void  CalculateProperty();

	// 计算战力
	DWORD CalculateFightingPower();

	// 填充阵型槽位属性列表
	bool FillSlotProList(BYTE bySlot, Property *pProList, BYTE &byProNum);

	// 取阵型槽位属性列表
	const int* GetSlotProList(BYTE bySlot);

	// 影响英雄战力: 英雄属性 + 装备 + 套装 + 英雄组合 + 工会

public:
	// 血量战力=血量上限*血量战力系数*闪避战力系数*格挡战力系数*抗暴战力系数*减伤战力系数
	static float CalculateBloodBattleValue(int nBlood, int nDodgeRate, int nBlockRate, int nBlockRateCorrect,
		int nCritDefenceRate, int nCritDefenceRateCorrect, int nDamageDecreaseRate);

	// 攻击战力=攻击*攻击战力系数*命中战力系数*暴击战力系数*伤害加成战力系数*百分比穿透战力系数+
	// 伤害附加*伤害附加战力系数+固定值穿透*固定值穿透战力系数
	static float CalculateAttackBattleValue(int nAttack, int nHitRate, int nCritRate, int nCritRateCorrect,
		int nDamageIncreaseRate, int nPenetrationRate, int nDamageIncreaseValue, int nPenetrationValue);

	// 防御战力=物防*物防战力系数+法防*法防战力系数+固定值减伤*固定值减伤战力系数
	static float CalculateDefenceBattleValue(int nPhysicalDefence, int nMagicDefence, int nDamageDecreaseValue);

	// 英雄战力=血量战力+攻击战力+防御战力
	//static float CalculateHeroBattleValue(const HeroInfo *pHeroInfo);

	// 队伍战力=上阵角色战力总和

	// for test 
	void PrintHeroPro();
	DWORD GetTotalHP();
protected:
	// 添加属性
	void AddProperty(BYTE bySlot, const vector<Property> *pProList, WORD wFromId);

	// 添加属性
	void AddProperty(BYTE bySlot, const Property &tProperty, WORD wFromId);

	// 计算英雄属性
	void CalculateHeroProperty(BYTE bySlot);

	// 计算装备属性
	void CalculateEquipmentProperty(BYTE bySlot);

	// 计算宝物属性
	void CalculateTreasureProperty(BYTE bySlot);

	// 计算宝石属性
	void CalculateGemProperty(BYTE bySlot);

	// 计算卡牌属性
	void CalculateCardProperty(BYTE bySlot);

	// 计算加成属性
	void CalculateIncProperty(BYTE bySlot);

	// 计算称号属性
	void CalculateTitleProperty();

	// 计算爵位属性
	void CalculateNobilityProperty();

	// 英雄突破队伍属性
	void CalculateHeroBreakTeamProperty();

	// 计算占卜数据
	void CalculateZhanBuProperty();

	// 计算援军助威属性
	void CalculateReinforcementEncourageProperty();

protected:

	// 计算精灵属性(对所有上阵的英雄有影响)
	void CalculateSpriteProperty();

	// 计算精灵基本属性(强化,培养)
	void CalculateSpriteBaseProperty(vector<Property> &vProList, const SpiritData *pSpriteData);

	// 计算精灵天赋技能属性
	void CalculateSpriteTalentSkillProperty();

	// 计算精灵祝福属性(只对被祝福的英雄有影响)
	void CalculateSpriteBlessProperty(BYTE bySlot);


private:
	Player *m_pPlayer;
	PlayerHeroInfo *m_pHeroInfo;

	int	m_arProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX];			// 阵型属性列表
	
	// 对s英雄有效的
	vector<Property> m_vTitleProList;					// 称号属性
	vector<Property> m_vSpriteProList;					// 精灵属性
	vector<Property> m_vNobilityProList;				// 爵位属性
	vector<Property> m_vHeroBreakTeamProList;			// 英雄突破队伍属性
	vector<Property> m_vZhanBuProList;					// 占卜属性
	vector<Property> m_vReinforcementEncourageProList;	// 援军助威属性

	map<WORD, WORD> m_GemSkillList;	// 宝石技能列表


	vector<Property> m_vSpriteTalentSkillProList;	// 精灵天赋技能属性

	DWORD m_dwTotalHP;
};