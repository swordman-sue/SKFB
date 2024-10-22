#include "stdafx.h"
#include "PropertyManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Title\TitleManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Spirit\SpiritManager.h"
#include "..\ZhanBu\ZhanBuManager.h"
#include "..\MapServer.h"



CPropertyManager::CPropertyManager()
{
	
}

CPropertyManager::~CPropertyManager()
{

}

void CPropertyManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pHeroInfo = &pPlayer->GetRoleFullInfo().tHeroInfo;
}

// 取总战力
DWORD CPropertyManager::GetAllFightingPower()
{
	CalculateProperty();

	return CalculateFightingPower();
}

// 计算属性
void CPropertyManager::CalculateProperty()
{
	UINT64 lCurTime = GetTickCount64();

	m_vTitleProList.clear();
	m_vSpriteProList.clear();
	m_vNobilityProList.clear();
	m_vHeroBreakTeamProList.clear();
	m_vZhanBuProList.clear();
	m_vReinforcementEncourageProList.clear();

	// 称号属性
	CalculateTitleProperty();

	// 精灵属性
	CalculateSpriteProperty();

	// 爵位属性
	CalculateNobilityProperty();

	// 英雄突破队伍属性
	CalculateHeroBreakTeamProperty();

	// 占卜属性(收藏,成就属性)
	CalculateZhanBuProperty();

	// 援军助威属性
	CalculateReinforcementEncourageProperty();

	memset(m_arProList, 0, sizeof(m_arProList));
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		// 阵型上木有英雄不需要处理
		if (!m_pHeroInfo->arFormationList[i].dwHeroUID)
			continue;

		// 英雄属性
		CalculateHeroProperty(i);

		// 装备属性
		CalculateEquipmentProperty(i);

		// 宝物属性
		CalculateTreasureProperty(i);

		// 宝石属性
		CalculateGemProperty(i);

		// 卡牌属性
		CalculateCardProperty(i);

		// 精灵祝福属性
		CalculateSpriteBlessProperty(i);

		// 称号属性
		if (m_vTitleProList.size())
			AddProperty(i, &m_vTitleProList, PRO_FROM_TITLE);

		// 精灵属性
		if (m_vSpriteProList.size())
			AddProperty(i, &m_vSpriteProList, PRO_FROM_SPRITE);

		// 爵位属性
		if (m_vNobilityProList.size())
			AddProperty(i, &m_vNobilityProList, PRO_FROM_NOBILITY);

		// 英雄突破队伍属性
		if (m_vHeroBreakTeamProList.size())
			AddProperty(i, &m_vHeroBreakTeamProList, PRO_FROM_TEAM_BREAK);

		// 占卜属性
		if (m_vZhanBuProList.size())
			AddProperty(i, &m_vZhanBuProList, PRO_FROM_ZHANBU);

		// 援军助威属性
		if (m_vReinforcementEncourageProList.size())
			AddProperty(i, &m_vReinforcementEncourageProList, PRO_FROM_REINFORCEMENT_ENCOURAGE);

		// 天赋技能属性转化
		if (m_vSpriteTalentSkillProList.size())
			AddProperty(i, &m_vSpriteTalentSkillProList, PRO_FROM_TALENT_SKILL_CONVERSION);

		// 属性比例加成 
		CalculateIncProperty(i);
	}

	DWORD dwUsedTime = GetTickCount64() - lCurTime;
	if (dwUsedTime)
		MAPLOG(ERROR_LEVEL, "计算战力消耗[%d ms]", dwUsedTime);
}

// 计算战力
DWORD CPropertyManager::CalculateFightingPower()
{
	// 计算战力
	DWORD dwTotalFightingPower = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pHeroInfo->arFormationList[i];

		// 阵型上木有英雄不需要处理
		if (!pFormation->dwHeroUID)
			continue;

		int *pProList = m_arProList[i];

		// 血量战力
		dwTotalFightingPower += CalculateBloodBattleValue(pProList[HERO_PRO_BLOOD], pProList[HERO_PRO_DODGE_RATE],
			pProList[HERO_PRO_BLOCK_RATE], pProList[HERO_PRO_BLOCK_RATE_CORRECT], pProList[HERO_PRO_CRIT_DEFENCE_RATE],
			pProList[HERO_PRO_CRIT_DEFENCE_RATE_CORRECT], pProList[HERO_PRO_DAMAGE_DECREASE_RATE]);

		// 攻击战力
		dwTotalFightingPower += CalculateAttackBattleValue(pProList[HERO_PRO_ATTACK], pProList[HERO_PRO_HIT_RATE],
			pProList[HERO_PRO_CRIT_RATE], pProList[HERO_PRO_CRIT_RATE_CORRECT], pProList[HERO_PRO_DAMAGE_INCREASE_RATE],
			pProList[HERO_PRO_PENETRATION_RATE], pProList[HERO_PRO_DAMAGE_INCREASE_VALUE], pProList[HERO_PRO_PENETRATION_VALUE]);

		// 防御战力
		dwTotalFightingPower += CalculateDefenceBattleValue(pProList[HERO_PRO_PHY_DEF],
			pProList[HERO_PRO_MAG_DEF], pProList[HERO_PRO_DAMAGE_INCREASE_VALUE]);
	}

	return dwTotalFightingPower;
}

// 填充阵型槽位属性
bool CPropertyManager::FillSlotProList(BYTE bySlot, Property *pProList, BYTE &byProNum)
{
	byProNum = 0;
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "阵型槽位索引越界[slot:%d]", bySlot);
		return false;
	}

	int *pList = m_arProList[bySlot];
	for (BYTE i = 0; i < HERO_PRO_MAX; ++i)
	{
		if (pList[i])
		{
			pProList[byProNum].wType = i;
			pProList[byProNum].nValue = pList[i];
			++byProNum;
		}
	}
	return true;
}

// 取阵型槽位属性列表
const int* CPropertyManager::GetSlotProList(BYTE bySlot)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "阵型槽位索引越界[slot:%d]", bySlot);
		return NULL;
	}

	return m_arProList[bySlot];
}

// 血量战力=血量上限*血量战力系数*闪避战力系数*格挡战力系数*抗暴战力系数*减伤战力系数
// 血量战力系数=0.1(0.5)
// 闪避战力系数=1+闪避率
// 格挡战力系数=1/MAX(1-格挡率*(0.2+格挡强度),0.1)
// 抗暴战力系数=4/(MAX(2-抗爆率*0.5,0.01)*MAX(2-抗暴击强度*0.5,0.01))
// 减伤战力系数=1+百分比减伤
float CPropertyManager::CalculateBloodBattleValue(int nBlood, int nDodgeRate, int nBlockRate, int nBlockRateCorrect,
	int nCritDefenceRate, int nCritDefenceRateCorrect, int nDamageDecreaseRate)
{
	float fBloodBattleValue = 0.5;
	float fDodgeBattleValue = 1 + nDodgeRate * PROPERTY_DECREASE_MULTIPLE;
	float fBlockBattleValue = 1 / max(1 - nBlockRate * PROPERTY_DECREASE_MULTIPLE *(0.2 + nBlockRateCorrect * PROPERTY_DECREASE_MULTIPLE), 0.1);
	float fCritDefinceBattleValue = 4 / (max(2 - nCritDefenceRate * PROPERTY_DECREASE_MULTIPLE * 0.5, 0.01) * max(2 - nCritDefenceRateCorrect * PROPERTY_DECREASE_MULTIPLE * 0.5, 0.01));
	float fDamageDecreaseBattleValue = 1 + nDamageDecreaseRate * PROPERTY_DECREASE_MULTIPLE;
	return nBlood * fBloodBattleValue * fDodgeBattleValue * fBlockBattleValue * fCritDefinceBattleValue * fDamageDecreaseBattleValue;
}

// 攻击战力=攻击*攻击战力系数*命中战力系数*暴击战力系数*伤害加成战力系数*百分比穿透战力系数+伤害附加*伤害附加战力系数+固定值穿透*固定值穿透战力系数
// 攻击战力系数=1.2(5)
// 命中战力系数=1+命中率
// 暴击战力系数=(1.05+暴击率)/1.05*(2+暴击强度)/2
// 伤害加成战力系数=1+伤害加成
// 百分比穿透战力系数=1+百分比穿透
// 伤害附加战力系数=1.8(10)
// 固定值穿透战力系数=2(5)
float CPropertyManager::CalculateAttackBattleValue(int nAttack, int nHitRate, int nCritRate, int nCritRateCorrect,
	int nDamageIncreaseRate, int nPenetrationRate, int nDamageIncreaseValue, int nPenetrationValue)
{
	float fAttackBattleValue = 5;
	float fHitBattleValue = 1 + nHitRate * PROPERTY_DECREASE_MULTIPLE;
	float fCritBattleValue = (1.05 + nCritRate * PROPERTY_DECREASE_MULTIPLE) / 1.05 * (2 + nCritRateCorrect * PROPERTY_DECREASE_MULTIPLE) / 2;
	float fDamageIncreaseBattleValue = 1 + nDamageIncreaseRate * PROPERTY_DECREASE_MULTIPLE;
	float fPenetrationRateBattleValue = 1 + nPenetrationRate * PROPERTY_DECREASE_MULTIPLE;
	float fDamageIncreaseValueBattleValue = 10;
	float fPenetrationValueBattleValue = 5;
	return nAttack * fAttackBattleValue * fHitBattleValue * fCritBattleValue * fDamageIncreaseBattleValue * fPenetrationRateBattleValue +
		nDamageIncreaseValue * fDamageIncreaseValueBattleValue + nPenetrationValue * fPenetrationValueBattleValue;
}

// 防御战力=物防*物防战力系数+法防*法防战力系数+固定值减伤*固定值减伤战力系数
// 物防战力系数=2(5)
// 法防战力系数=2(5)
// 固定值减伤战力系数=1.8(10)
float CPropertyManager::CalculateDefenceBattleValue(int nPhysicalDefence, int nMagicDefence, int nDamageDecreaseValue)
{
	float fPhysicalDefenceBattleValue = 5;
	float fMagicDefenceBattleValue = 5;
	float fDamageDecreaseValueBattleValue = 10;
	return nPhysicalDefence * fPhysicalDefenceBattleValue +
		nMagicDefence * fMagicDefenceBattleValue + nDamageDecreaseValue * fDamageDecreaseValueBattleValue;
}

void CPropertyManager::PrintHeroPro()
{
	string arProName[] =
	{
		"无名",
		"攻击",
		"物防",
		"法防",
		"生命",

		"免伤",
		"减伤",
		"伤害",
		"附伤",

		"穿透",
		"减防",

		"命中率",
		"闪避率",
		"暴击率",
		"抗暴率",
		"暴伤",
		"减暴伤",
		"格挡率",
		"格挡力",

		"击退",
		"抗击退",

		"怒气",

		"攻击加成",
		"物防加成",
		"法防加成",
		"生命加成",
		"速度",

		"攻击最终加成",
		"物防最终加成",
		"法防最终加成",
		"生命最终加成"
	};

	CalculateProperty();

	for (int i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		MAPLOG(ERROR_LEVEL, "英雄槽位[%d]属性",i);
		int *pHeroProList = m_arProList[i];
		for (int k = 0; k < HERO_PRO_MAX; ++k)
		{
			MAPLOG(ERROR_LEVEL, "属性[%s=%d] ", arProName[k].c_str(), pHeroProList[k]);
		}
	}
	//int	m_arProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX];
}

DWORD CPropertyManager::GetTotalHP()
{ 
	m_dwTotalHP = 0;
	for (int i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		m_dwTotalHP += m_arProList[i][HERO_PRO_BLOOD];
	}

	return m_dwTotalHP; 
}

// 英雄战力=血量战力+攻击战力+防御战力
// float CPropertyManager::CalculateHeroBattleValue(const HeroInfo *pHeroInfo)
// {
// 	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroInfo->dwIntoId);
// 	if (!pHeroCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "找不到英雄的配置[HeroInfoId:%d]", pHeroInfo->dwIntoId);
// 		return 0;
// 	}
// 
// 	const HeroPro_Config *pHeroProCfg = CConfigManager::Instance()->GetHeroPro(
// 		pHeroCfg->byProfession, pHeroInfo->wQuality, pHeroInfo->wLevel);
// 	if (!pHeroProCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "找不到英雄属性配置[Profession:%d,Quality:%d,level:%d]",
// 			pHeroCfg->byProfession, pHeroInfo->wQuality, pHeroInfo->wLevel);
// 		return 0;
// 	}
// 	// Calculate
// 	float fBloodBattleValue = 1; // CalculateBloodBattleValue(pHeroProCfg->nBloodMax, 0, 0, 0, 0, 0, 0);
// 	float fAttackBattleValue = 2; // CalculateAttackBattleValue(pHeroProCfg->nAttack, 0, 0, 0, 0, 0, 0, 0);
// 	float fDefanceBattleValue = 3; // CalculateDefenceBattleValue(pHeroProCfg->nPhysicalDefence, pHeroProCfg->nMagicDefence, 0);
// 
// 	return fBloodBattleValue + fAttackBattleValue + fDefanceBattleValue;
// }


void CPropertyManager::AddProperty(BYTE bySlot, const vector<Property> *pProList, WORD wFromId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "阵型槽位索引越界[slot:%d]", bySlot);
		return;
	}

	vector<Property>::const_iterator iter = pProList->begin();
	for (; iter != pProList->end(); ++iter)
	{
		AddProperty(bySlot, *iter, wFromId);
	}
}

// 添加战斗属性
void CPropertyManager::AddProperty(BYTE bySlot, const Property &tProperty, WORD wFromId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "阵型槽位索引越界[slot:%d]", bySlot);
		return;
	}

	if (tProperty.wType >= HERO_PRO_MAX)
	{
		MAPLOG(GUI_LEVEL, "添加的战斗属性越界[type:%d]", tProperty.wType);
		return;
	}

	if (0 != tProperty.nValue)
		m_arProList[bySlot][tProperty.wType] += tProperty.nValue;

	// todo:: 方便查找BUG使用
//  	if (tProperty.wType == HERO_PRO_BLOOD)
//  		MAPLOG(GUI_LEVEL, "角色血量变化[role:%d,slot:%d,add_hp:%d,cur_hp:%d,from_id:%d]", 
//  		m_pPlayer->GetRoleId(), bySlot, tProperty.nValue, m_arProList[bySlot][tProperty.wType], wFromId);
}

// 计算英雄属性
void CPropertyManager::CalculateHeroProperty(BYTE bySlot)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	DWORD dwHeroUID = m_pHeroInfo->arFormationList[bySlot].dwHeroUID;

	const HeroInfo *pHero = pHeroMgr->GetHero(dwHeroUID);
	if (!pHero)
		return;

	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
		return;


	// 英雄基础属性
	const HeroPro_Config *pHeroProCfg = g_pCfgMgr->GetHeroPro(pHeroCfg->byProfession, pHero->wQuality, pHero->wLevel);
	if (pHeroProCfg)
	{
		AddProperty(bySlot, &pHeroProCfg->vProList, PRO_FROM_HERO_BASE);
	}

	// 突破属性
	const HeroBreak_Config *pBreakCfg = g_pCfgMgr->GetHeroBreak(pHero->dwIntoId, pHero->wBreakLevel);
	if (pBreakCfg)
	{
		AddProperty(bySlot, &pBreakCfg->vSelfProList, PRO_FROM_HERO_BREAK);
	}

	// 培养属性
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		// 随机属性
		Property pro;
		pro.wType = GetTrainProType(i);
		pro.nValue = pHero->arTrainProValueList[i];
		AddProperty(bySlot, pro, PRO_FROM_HERO_TRAIN);

		// 等级额外属性
		BYTE byProLevel = pHero->arTrainProLvList[i];
		BYTE byIsTake = pHero->arTrainRewardFlag[i];
		const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(pro.wType, byProLevel);
		// 当前等级(等级及属性值都要满足)
		if (pTrainCfg && pro.nValue >= pTrainCfg->nProMax && !byIsTake)
		{
			AddProperty(bySlot, &pTrainCfg->vLevelUpProList, PRO_FROM_HERO_TRAIN);
		}
		// 上一等级
		else if (byProLevel)
		{
			pTrainCfg = g_pCfgMgr->GetHeroTrain(pro.wType, byProLevel-1);
			if (pTrainCfg)
				AddProperty(bySlot, &pTrainCfg->vLevelUpProList, PRO_FROM_HERO_TRAIN);
		}
	}

	// 天命属性
	const HeroDestiny_Config *pDestinyCfg = g_pCfgMgr->GetHeroDestiny(pHero->wDestinyLevel);
	if (pDestinyCfg)
	{
		AddProperty(bySlot, &pDestinyCfg->vProList, PRO_FROM_HERO_DESTINY);
	}
	for (WORD i = 0; i < pHero->wAwakenItemNum; ++i)
	{
		WORD wItemId = pHero->arAwakenItemList[i];
		const Item_Config *pAwakenItemCfg = g_pCfgMgr->GetItem(wItemId);
		if (pAwakenItemCfg && pAwakenItemCfg->vProList.size())
		{
			AddProperty(bySlot, &pAwakenItemCfg->vProList, PRO_FROM_HERO_DESTINY);
		}
	}

	// 觉醒属性
	const HeroAwaken_Config *pAwakenCfg = 
		g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality, pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
	if (pAwakenCfg)
	{
		AddProperty(bySlot, &pAwakenCfg->vProList, PRO_FROM_HERO_AWAKEN);
		AddProperty(bySlot, &pAwakenCfg->vItemProList, PRO_FROM_HERO_AWAKEN);
	}

	int nQualifications = pHeroCfg->nQualifications;

	// 进化属性(橙及以上有)
	if (pHero->wQuality >= QUALITY_ORANGE)
	{
		for (WORD i = pHeroCfg->wInitQuality; i <= pHero->wQuality; ++i)
		{
			// 取当前品阶上限
			WORD wStageMax = (i == pHero->wQuality) ? pHero->wQualityStage : MAX_QUALITY_STAGE;
			for (WORD k = 0; k <= wStageMax; ++k)
			{
				const HeroEvolution_Config *pEvolutionCfg = g_pCfgMgr->GetHeroEvolution(i, k);

				// 如果找不到,则当作是最后一条
				if (!pEvolutionCfg)
					break;

				AddProperty(bySlot, &pEvolutionCfg->vProList, PRO_FROM_HERO_EVOLUTION);
				nQualifications += pEvolutionCfg->wAddQualifications;
			}
		}
	}

	// 资质属性
	double fAddRate = g_pCfgMgr->GetHeroQualificationsAddRate(nQualifications) * PROPERTY_DECREASE_MULTIPLE;
	if (fAddRate > 0 && pHeroProCfg)
	{
		Property tPro;
		for (int i = 0; i < pHeroProCfg->vProList.size(); ++i)
		{
			tPro = pHeroProCfg->vProList[i];
			tPro.nValue = tPro.nValue * fAddRate;
			AddProperty(bySlot, tPro, PRO_FROM_HERO_BASE);
		}
	}

	// 关联英雄属性
	for (size_t i = 0; i < pHeroCfg->vRelationHeroList.size(); ++i)
	{
		bool bIsRelationHeroToBattle = true;
		const RelationHeroProperty &tRelationHero = pHeroCfg->vRelationHeroList[i];
		for (size_t k = 0; k < tRelationHero.vHeroList.size(); ++k)
		{
			if (!pHeroMgr->IsToBattleEx(tRelationHero.vHeroList[k]))
			{
				bIsRelationHeroToBattle = false;
				break;
			}
		}
		if (bIsRelationHeroToBattle)
		{
			AddProperty(bySlot, &tRelationHero.vProList, PRO_FROM_HERO_RELATION);
		}
	}

	const vector<RelationEquipmentProperty> *pRelationEquipmentList;
	const vector<RelationTreasureProperty> *pRelationTreasureList;

	// 如果当前品质跟初始不一样
	if (pHero->wQuality != pHeroCfg->wInitQuality)
	{
		pRelationEquipmentList = &pHeroCfg->vRelationEquipmentListEx;
		pRelationTreasureList = &pHeroCfg->vRelationTreasureListEx;
	}
	else
	{
		pRelationEquipmentList = &pHeroCfg->vRelationEquipmentList;
		pRelationTreasureList = &pHeroCfg->vRelationTreasureList;
	}

	// 关联装备属性(或者关系,只要穿戴一件就可以)
	for (vector<RelationEquipmentProperty>::const_iterator iter = pRelationEquipmentList->begin(); iter != pRelationEquipmentList->end(); ++iter)
	{
		bool bIsRelationEquipmentWear = false;
		const RelationEquipmentProperty &tRelationEquipment = *iter;
		for (size_t k = 0; k < tRelationEquipment.vEquipmentList.size(); ++k)
		{
			if (pHeroMgr->IsEquipmentWear(bySlot, tRelationEquipment.vEquipmentList[k]))
			{
				bIsRelationEquipmentWear = true;
				break;
			}
		}
		if (bIsRelationEquipmentWear)
		{
			AddProperty(bySlot, &tRelationEquipment.vProList, PRO_FROM_EQUIM_RELATION);
		}
	}

	// 关联宝物属性(或者关系,只要穿戴一件就可以)
	for (vector<RelationTreasureProperty>::const_iterator iter = pRelationTreasureList->begin(); iter != pRelationTreasureList->end(); ++iter)
	{
		bool bIsRelationTreasureWear = false;
		const RelationTreasureProperty &tRelationTreasure = *iter;
		for (size_t k = 0; k < tRelationTreasure.vTreasureList.size(); ++k)
		{
			if (pHeroMgr->IsTreasureWear(bySlot, tRelationTreasure.vTreasureList[k]))
			{
				bIsRelationTreasureWear = true;
				break;
			}
		}
		if (bIsRelationTreasureWear)
		{
			AddProperty(bySlot, &tRelationTreasure.vProList, PRO_FROM_TREASURE_RELATION);
		}
	}

	// 关联卡牌属性
	for (size_t i = 0; i < pHeroCfg->vRelationCardList.size(); ++i)
	{
		bool bIsRelationCardToBattle = true;
		const RelationCardProperty &tRelationCard = pHeroCfg->vRelationCardList[i];
		for (size_t k = 0; k < tRelationCard.vCardTypeList.size(); ++k)
		{
			if (!pHeroMgr->IsEquipCardType(bySlot, tRelationCard.vCardTypeList[k]))
			{
				bIsRelationCardToBattle = false;
				break;
			}
		}
		if (bIsRelationCardToBattle)
		{
			AddProperty(bySlot, &tRelationCard.vProList, PRO_FROM_CARD_RELATION);
		}
	}

	// 世界之心
	PlayerSpriteInfo &tSpriteData = m_pPlayer->GetRoleFullInfo().tSpiritInfo;
	for (WORD i = 1; i <= tSpriteData.tHeartOfWorld.wStage; ++i)
	{
		const HeartOfWorld_Config *pHeartOfWorldCfg = g_pCfgMgr->GetHeartOfWorld(i);
		if (!pHeartOfWorldCfg)
		{
			MAPLOG(GUI_LEVEL, "找不到精灵阶段配置[stage:%d]", i);
			continue;
		}
		BYTE byLevel = i == tSpriteData.tHeartOfWorld.wStage ? tSpriteData.tHeartOfWorld.wLevel : pHeartOfWorldCfg->vLeveDatalist.size();
		for (BYTE k = 1; k <= byLevel; ++k)
		{
			const HeartOfWorld_Config::LevelData *pLevelCfg = pHeartOfWorldCfg->GetLevelData(k);
			if (pLevelCfg)
				AddProperty(bySlot, &pLevelCfg->vAddProList, PRO_FROM_WORLD_HEART);
		}
	}
	// 精灵图鉴
	for (WORD i = 0; i < tSpriteData.wPokedexNum; ++i)
	{
		SpiritPokedex &tPokedex = tSpriteData.arPokedexList[i];
		const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(tPokedex.wPokedexId, tPokedex.wLevel);
		if (pPokedexCfg)
		{
			AddProperty(bySlot, pPokedexCfg->vAddPro, PRO_FROM_SPRITE_POKEDEX);
		}
	}
	// 工会技能
	if (CGuildManager::Instance()->GetGuildIdByRoleId(m_pPlayer->GetRoleId()))
	{	
		PlayerGuildSkillData &tGuildSkill = m_pPlayer->GetRoleFullInfo().tGuildSkillData;
		for (WORD i = 0; i < tGuildSkill.wSkillNum; ++i)
		{
			SkillData &tSkill = tGuildSkill.arSkillList[i];
			const GuildSkill_Config *pGuildSkillCfg = g_pCfgMgr->GetGuildSkill(tSkill.wSkillId, tSkill.wLevel);
			if (pGuildSkillCfg && pGuildSkillCfg->vAddProList.size())
				AddProperty(bySlot, &pGuildSkillCfg->vAddProList, PRO_FROM_GUILD_SKILL);
		}
	}
}

// 计算装备属性
void CPropertyManager::CalculateEquipmentProperty(BYTE bySlot)
{
	CEquipmentManager *pEquipmentMgr = m_pPlayer->GetEquipmentManager();

	// 装备属性
	for (BYTE i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwEquipUId = m_pHeroInfo->arFormationList[bySlot].arEquipmentList[i];
		if (!dwEquipUId)
			continue;

		const Equipment *pEqupment = pEquipmentMgr->GetEquipment(dwEquipUId);
		if (!pEqupment)
			continue;

		const Equipment_Config *pEquipmentCfg = g_pCfgMgr->GetEquipment(pEqupment->dwInfoId);
		if (!pEquipmentCfg)
			continue;

		// 强化属性
		AddProperty(bySlot, pEquipmentCfg->tStrengthenInitPro, PRO_FROM_EQUIP_ENHANCE);
		for (WORD k = 1; k < pEqupment->wEnhanceLevel; ++k)
		{
			AddProperty(bySlot, pEquipmentCfg->tStrengthenLevelPro, PRO_FROM_EQUIP_ENHANCE);
		}

		// 精炼属性
		for (WORD k = 0; k < pEqupment->wRefineLevel; ++k)
		{
			AddProperty(bySlot, &pEquipmentCfg->vRefineLevelProList, PRO_FROM_EQUIP_REFINE);
		}

		// 天赋属性
		if (pEquipmentCfg->wTalentId)
		{
			const EquipmentTalent_Config *pTalentCfg = g_pCfgMgr->GetEquipmentTalent(pEquipmentCfg->wTalentId, pEqupment->wEnhanceLevel);
			if (pTalentCfg)
			{
				AddProperty(bySlot, &pTalentCfg->vProList, PRO_FROM_EQUIP_TALENT);
			}
		}

		// 升星属性
		if (pEquipmentCfg->wStarMax)
		{
			const EquipmentStarUp_Config *pStarUpCfg = g_pCfgMgr->GetEquipmentStartUp(pEquipmentCfg->byType, pEquipmentCfg->wQuality, pEqupment->wStar, pEqupment->wStarLevel);
			if (pStarUpCfg)
			{
				AddProperty(bySlot, &pStarUpCfg->vProList, PRO_FROM_EQUIP_STARUP);
			}
		}
	}


	// 锻造属性
	WORD wEnhanceForgeLevel = 0;
	WORD wRefineForgeLevel = 0;
	bool bEnhanceInit = false;
	bool bRefineInit = false;
	for (BYTE i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwEquipUId = m_pHeroInfo->arFormationList[bySlot].arEquipmentList[i];

		// 某个槽位上的装备为空,则锻造等级都为0
		if (!dwEquipUId)
		{
			wEnhanceForgeLevel = 0;
			wRefineForgeLevel = 0;
			break;
		}

		const Equipment *pEqupment = pEquipmentMgr->GetEquipment(dwEquipUId);
		if (pEqupment)
		{
			if (!bEnhanceInit)
			{
				wEnhanceForgeLevel = pEqupment->wEnhanceLevel;
				bEnhanceInit = true;
			}
			else
			{
				wEnhanceForgeLevel = min(wEnhanceForgeLevel, pEqupment->wEnhanceLevel);
			}

			if (!bRefineInit)
			{
				wRefineForgeLevel = pEqupment->wRefineLevel;
				bRefineInit = true;
			}
			else
			{
				wRefineForgeLevel = min(wRefineForgeLevel, pEqupment->wRefineLevel);
			}
		}
	}

	// 强化锻造属性
	if (wEnhanceForgeLevel)
	{
		const EquipmentForge_Config *pForgeCfg = g_pCfgMgr->GetEquipementStrengthenForge(wEnhanceForgeLevel);
		if (pForgeCfg)
		{
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_EQUIP_ENHANCE_FORGE);
		}
	}

	// 精炼锻造属性
	if (wRefineForgeLevel)
	{
		const EquipmentForge_Config *pForgeCfg = g_pCfgMgr->GetEquipementRefineForge(wRefineForgeLevel);
		if (pForgeCfg)
		{
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_EQUIP_REFINE_FORGE);
		}
	}

	// 套装属性 todo::有些消耗,日后再想想有没有更好的办法
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	const vector<EquipmentSuit_Config> *pSuitCfgList = g_pCfgMgr->GetEquipmentSuitList();
	if (pSuitCfgList)
	{
		for (size_t i = 0; i < pSuitCfgList->size(); ++i)
		{
			// 计算穿戴套装的件数
			int nNum = 0;
			const EquipmentSuit_Config &tSuitCfg = pSuitCfgList->at(i);
			for (size_t k = 0; k < tSuitCfg.vEquipmentList.size(); ++k)
			{
				DWORD dwInfoId = tSuitCfg.vEquipmentList[k];
				if (pHeroMgr->IsEquipmentWear(bySlot, dwInfoId))
					nNum++;
			}

			// 添加件数对应的属性
			for(int k = 2; k <= nNum; ++k)
			{
				map<DWORD, vector<Property>>::const_iterator iter = tSuitCfg.vProList.find(k);
				if (iter != tSuitCfg.vProList.end())
				{
					AddProperty(bySlot, &iter->second, PRO_FROM_EQUIP_SUIT);
				}
			}
		}
	}
}

// 计算宝物属性
void CPropertyManager::CalculateTreasureProperty(BYTE bySlot)
{
	CTreasureManager *pTreasureMgr = m_pPlayer->GetTreasureManager();

	// 宝物属性
	for (BYTE i = 0; i < TREASURE_TYPE_MAX; ++i)
	{
		DWORD dwTreasureUId = m_pHeroInfo->arFormationList[bySlot].arTreasureList[i];
		if (!dwTreasureUId)
			continue;

		const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwTreasureUId);
		if (!pTreasure)
			continue;

		const Treasure_Config *pTreasureCfg = g_pCfgMgr->GetTreasure(pTreasure->dwInfoId);
		if (!pTreasureCfg)
			continue;

		// 强化属性
		AddProperty(bySlot, &pTreasureCfg->vStrengthenInitProList, PRO_FROM_TREASURE_ENHANCE);	// 初始属性
		for (WORD k = 1; k < pTreasure->wEnhanceLevel; ++k)
		{
			AddProperty(bySlot, &pTreasureCfg->vStrengthenLevelProList, PRO_FROM_TREASURE_ENHANCE);	// 等级属性
		}

		// 精炼属性
		for (WORD k = 0; k < pTreasure->wRefineLevel; ++k)
		{
			AddProperty(bySlot, &pTreasureCfg->vRefineLevelProList, PRO_FROM_TREASURE_REFINE);	// 等级属性
		}
	}

	// 锻造属性
	WORD wEnhanceForgeLevel = 0;
	WORD wRefineForgeLevel = 0;
	bool bEnhanceInit = false;
	bool bRefineInit = false;
	for (BYTE i = 0; i < TREASURE_TYPE_MAX; ++i)
	{
		DWORD dwTreasureUId = m_pHeroInfo->arFormationList[bySlot].arTreasureList[i];

		// 某个槽位上的装备为空,则锻造等级都为0
		if (!dwTreasureUId)
		{
			wEnhanceForgeLevel = 0;
			wRefineForgeLevel = 0;
			break;
		}

		const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwTreasureUId);
		if (pTreasure)
		{
			if (!bEnhanceInit)
			{
				wEnhanceForgeLevel = pTreasure->wEnhanceLevel;
				bEnhanceInit = true;
			}
			else
			{
				wEnhanceForgeLevel = min(wEnhanceForgeLevel, pTreasure->wEnhanceLevel);
			}

			if (!bRefineInit)
			{
				wRefineForgeLevel = pTreasure->wRefineLevel;
				bRefineInit = true;
			}
			else
			{
				wRefineForgeLevel = min(wRefineForgeLevel, pTreasure->wRefineLevel);
			}
		}
	}

	// 强化锻造属性
	if (wEnhanceForgeLevel)
	{
		const TreasureForge_Config *pForgeCfg = g_pCfgMgr->GetTreasureStrengthenForge(wEnhanceForgeLevel);
		if (pForgeCfg)
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_TREASURE_ENHANCE_FORGE);
	}

	// 精炼锻造
	if (wRefineForgeLevel)
	{
		const TreasureForge_Config *pForgeCfg = g_pCfgMgr->GetTreasureRefineForge(wRefineForgeLevel);
		if (pForgeCfg)
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_TREASURE_REFINE_FORGE);
	}
}

// 计算宝石属性
void CPropertyManager::CalculateGemProperty(BYTE bySlot)
{
	Formation *pFormation = &m_pHeroInfo->arFormationList[bySlot];

	m_GemSkillList.clear();

	// 宝石属性
	for (BYTE i = 0; i < MAX_GEM_HOLE_NUM; ++i)
	{
		DWORD dwGemId = pFormation->arGemList[i];
		if (!dwGemId)
			continue;

		const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
		if (!pGemCfg)
			continue;

		// 宝石初始属性
		AddProperty(bySlot, &pGemCfg->vInitProList, PRO_FROM_GEM_INIT);

		// 计算技能等级
		map<WORD, WORD>::iterator iter = m_GemSkillList.find(pGemCfg->wSkillId);
		if (iter != m_GemSkillList.end())
		{
			iter->second += pGemCfg->wSkillLevel;
		}
		else
		{
			m_GemSkillList.insert(std::make_pair(pGemCfg->wSkillId, pGemCfg->wSkillLevel));
		}
	}

	// 宝石技能属性
	map<WORD, WORD>::iterator iter = m_GemSkillList.begin();
	for (; iter != m_GemSkillList.end(); ++iter)
	{
		const GemSkill_Config *pSkillCfg = g_pCfgMgr->GetGemSkill(iter->first, iter->second);
		if (pSkillCfg)
			AddProperty(bySlot, &pSkillCfg->vProList, PRO_FROM_GEM_SKILL);
	}
}

// 计算卡牌属性
void CPropertyManager::CalculateCardProperty(BYTE bySlot)
{
	Formation *pFormation = &m_pHeroInfo->arFormationList[bySlot];
	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();

	// 卡牌属性
	for (BYTE i = 0; i < MAX_CARD_SLOT_NUM; ++i)
	{
		DWORD dwCardId = pFormation->arCardList[i];
		if (!dwCardId)
			continue;

		const Card *pCard = pZhanBuMgr->GetCard(dwCardId);
		if (!pCard)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌数据[CardUId:%d]", __FUNCTION__, dwCardId);
			continue;
		}

		const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
		if (!pCardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌配置数据[CardInfoId:%d]", __FUNCTION__, pCard->dwInfoId);
			continue;
		}

		// 基础属性列表
		AddProperty(bySlot, &pCardCfg->vBaseProList, PRO_FROM_CARD_BASE);

		// 等级属性列表
		for (WORD k = 1; k < pCard->wLevel; ++k)
		{
			AddProperty(bySlot, &pCardCfg->vLevelProList, PRO_FROM_CARD_LEVEL);
		}

		// 突破属性列表
		for (WORD k = 0; k < pCard->wBreakLevel; ++k)
		{
			AddProperty(bySlot, &pCardCfg->vBreakProList, PRO_FROM_CARD_BREAK);
		}
	}
}

// 计算加成属性
void CPropertyManager::CalculateIncProperty(BYTE bySlot)
{
	int *pProList = m_arProList[bySlot];
	pProList[HERO_PRO_ATTACK] *= (1 + pProList[HERO_PRO_ATTACK_RATE] * 0.0001);			// 攻击加成
	pProList[HERO_PRO_ATTACK] *= (1 + pProList[HERO_PRO_ATTACK_INC_RATE] * 0.0001);		// 攻击最终加成


	pProList[HERO_PRO_PHY_DEF] *= (1 + pProList[HERO_PRO_PHY_DEF_RATE] * 0.0001);		// 物防加成
	pProList[HERO_PRO_PHY_DEF] *= (1 + pProList[HERO_PRO_PHY_DEF_INC_RATE] * 0.0001);	// 物防最终加成

	pProList[HERO_PRO_MAG_DEF] *= (1 + pProList[HERO_PRO_MAG_DEF_RATE] * 0.0001);		// 法防加成
	pProList[HERO_PRO_MAG_DEF] *= (1 + pProList[HERO_PRO_MAG_DEF_INC_RATE] * 0.0001);	// 法防最终加成

	pProList[HERO_PRO_BLOOD] *= (1 + pProList[HERO_PRO_BLOOD_RATE] * 0.0001);			// 法防加成
	pProList[HERO_PRO_BLOOD] *= (1 + pProList[HERO_PRO_BLOOD_INC_RATE] * 0.0001);		// 法防最终加成

	//pProList[HERO_PRO_ATTACK] += (pProList[HERO_PRO_ATTACK] * pProList[HERO_PRO_ATTACK_INC_RATE] * 0.0001);
	//pProList[HERO_PRO_PHY_DEF] += (pProList[HERO_PRO_PHY_DEF] * pProList[HERO_PRO_PHY_DEF_INC_RATE] * 0.0001);
	//pProList[HERO_PRO_MAG_DEF] += (pProList[HERO_PRO_MAG_DEF] * pProList[HERO_PRO_MAG_DEF_INC_RATE] * 0.0001);
	//pProList[HERO_PRO_BLOOD] += (pProList[HERO_PRO_BLOOD] * pProList[HERO_PRO_BLOOD_INC_RATE] * 0.0001);
}

// 计算称号属性
void CPropertyManager::CalculateTitleProperty()
{
	PlayerTitleInfo &tRoleTitleInfo = m_pPlayer->GetRoleFullInfo().tSecondInfo.tTitleInfo;
	for (WORD i = 0; i < tRoleTitleInfo.wTitleNum; ++i)
	{
		TitleData &tTitleData = tRoleTitleInfo.arTitleList[i];;

		// 检测是否过期
		if (tTitleData.nOverTime && time(NULL) >= tTitleData.nOverTime)
		{
			// todo::是否在这里处理删除操作
		}
		else
		{
			const Title_Config *pTitleCfg = g_pCfgMgr->GetTitle(tTitleData.nTitleId);
			if (!pTitleCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到称号配置[TitleId:%d]", __FUNCTION__, tTitleData.nTitleId);
				return;
			}
			//AddProperty(bySlot, &pTitleCfg->vProList);
			for (size_t k = 0; k < pTitleCfg->vProList.size(); ++k)
			{
				m_vTitleProList.push_back(pTitleCfg->vProList[k]);
			}
		}
	}
}

// 计算精灵属性
void CPropertyManager::CalculateSpriteProperty()
{
	m_vSpriteProList.clear();
	m_vSpriteTalentSkillProList.clear();
	CSpriteManager *pSpriteMgr = m_pPlayer->GetSpiritManager();
	if (!pSpriteMgr)
		return;

	DWORD dwToBattleSpriteId = pSpriteMgr->GetToBattleSpiritId();

	// 当前没有上阵的精灵
	if (!dwToBattleSpriteId)
		return;

	// 取上阵的精灵数据
	const SpiritData *pToBattleSprite = pSpriteMgr->GetSpiritEx(dwToBattleSpriteId);
	if (!pToBattleSprite)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到上阵的精灵数据[spirit_id:%d]", __FUNCTION__, dwToBattleSpriteId);
		return;
	}

	// 取精灵培养配置
	const SpriteTrain_Config *pToBattleTrainCfg = g_pCfgMgr->GetSpriteTrain(pToBattleSprite->dwId, pToBattleSprite->wTrainStage);
	if (!pToBattleTrainCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置[SpriteId:%d,TrainStage:%d]",
			__FUNCTION__, pToBattleSprite->dwId, pToBattleSprite->wTrainStage);
 		return;
 	}

	float fProConversionRate = double(pToBattleTrainCfg->dwProConversionRate) * PROPERTY_DECREASE_MULTIPLE;

	// 有BUG的，但是为了之前的战力不变，暂时使用原因的
	if (g_MapServer.IsUseOldPower())
		fProConversionRate = ceil(pToBattleTrainCfg->dwProConversionRate * PROPERTY_DECREASE_MULTIPLE);


	// 计算精灵基本属性(强化,培养)
	CalculateSpriteBaseProperty(m_vSpriteProList, pToBattleSprite);

	// 强化, 培养, 资质加成属性转化
	for (size_t i = 0; i < m_vSpriteProList.size(); i++)
	{
		Property &tPro = m_vSpriteProList[i];
		tPro.nValue = tPro.nValue * fProConversionRate;
	}

	// 天赋技能属性
	CalculateSpriteTalentSkillProperty();
}

// 计算精灵基本属性(强化,培养)
void CPropertyManager::CalculateSpriteBaseProperty(vector<Property> &vProList, const SpiritData *pSpriteData)
{
	// 强化属性
	const SpiritEnhance_Config *pEnhanceCfg = g_pCfgMgr->GetSpiritEnhance(pSpriteData->dwId, pSpriteData->wEnhanceLevel);
	if (pEnhanceCfg)
	{
		// 计算资质影响的属性加成系数
		int nAddRate = 0;
		const Spirit_Config *pSpriteCfg = g_pCfgMgr->GetSpirit(pSpriteData->dwId);
		if (pSpriteCfg)
		{
			WORD wQualifications = pSpriteCfg->GetQualifications(pSpriteData->wStarLevel);
			const SpriteQualifications_Config *pQualificationsCfg = g_pCfgMgr->GetSpriteQualifications(wQualifications);
			if (pQualificationsCfg)
				nAddRate = pQualificationsCfg->nAddRate;
		}

		// 强化属性 + 资质加成属性
		Property pro;
		vector<Property>::const_iterator iter = pEnhanceCfg->vAddProList.begin();
		for (; iter != pEnhanceCfg->vAddProList.end(); ++iter)
		{
			pro = *iter;
			pro.nValue += (pro.nValue * nAddRate * PROPERTY_DECREASE_MULTIPLE);
			vProList.push_back(pro);
		}
	}

	// 培养属性
	const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(pSpriteData->dwId, pSpriteData->wTrainStage);
	if (pTrainCfg && pTrainCfg->vTrainProList.size())
	{
		// 前面阶属性
		for (size_t i = 0; i < pTrainCfg->vTrainTotalProList.size(); ++i)
		{
			Property tPro = pTrainCfg->vTrainTotalProList[i];

			// 有则更新
			bool bAdd = true;
			for (size_t k = 0; k < vProList.size(); ++k)
			{
				if (tPro.wType == vProList[k].wType)
				{
					vProList[k].nValue += tPro.nValue;
					bAdd = false;
					break;
				}
			}

			// 无则添加
			if (bAdd)
				vProList.push_back(tPro);
		}

		// 当前阶属性 (只有培养过的才有此属性)
		if (pSpriteData->wTrainTimes)
		{
			for (size_t i = 0; i < pTrainCfg->vTrainProList.size(); ++i)
			{
				Property tPro = pTrainCfg->vTrainProList[i];

				// 属性转化
				//if (pSpriteData->wTrainTimes)
				tPro.nValue *= pSpriteData->wTrainTimes;

				// 有则更新
				bool bAdd = true;
				for (size_t k = 0; k < vProList.size(); ++k)
				{
					if (tPro.wType == vProList[k].wType)
					{
						vProList[k].nValue += tPro.nValue;
						bAdd = false;
						break;
					}
				}

				// 无则添加
				if (bAdd)
					vProList.push_back(tPro);
			}
		}
	}
}

// 计算精灵祝福属性
void CPropertyManager::CalculateSpriteBlessProperty(BYTE bySlot)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	DWORD dwBlessSpriteId = m_pHeroInfo->arFormationList[bySlot].dwBlessSpriteId;

	// 没有祝福精灵
	if (!dwBlessSpriteId)
		return;

	// 取精灵数据
	const SpiritData *pSpriteData = m_pPlayer->GetSpiritManager()->GetSpiritEx(dwBlessSpriteId);
	if (!pSpriteData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵数据[SpriteId:%d]",__FUNCTION__, dwBlessSpriteId);
		return;
	}

	// 取精灵特性
	const SpriteSpecialProperty_Config *pSpecialProCfg = g_pCfgMgr->GetSpriteSpecialProperty(pSpriteData->dwId, pSpriteData->wStarLevel);
	if (!pSpecialProCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵特性配置数据[SpriteId:%d,Star:%d]", 
			__FUNCTION__, pSpriteData->dwId, pSpriteData->wStarLevel);
		return;
	}

	// 取精灵培养配置
	const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(pSpriteData->dwId, pSpriteData->wTrainStage);
	if (!pTrainCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置数据[SpriteId:%d,TrainStage:%d]",
			__FUNCTION__, pSpriteData->dwId, pSpriteData->wTrainStage);
		return;
	}

	// 祝福基本属性
	AddProperty(bySlot, &pSpecialProCfg->vBlessProList, PRO_FROM_SPRITE_BLESS_BASE);

	float fProConversionRate = double(pTrainCfg->dwProConversionRate) * PROPERTY_DECREASE_MULTIPLE;

	// 有BUG的，但是为了之前的战力不变，暂时使用原因的
	if (g_MapServer.IsUseOldPower())
		fProConversionRate = ceil(pTrainCfg->dwProConversionRate * PROPERTY_DECREASE_MULTIPLE);

	// 处理祝福转化属性
	vector<Property> vProList;
	CalculateSpriteBaseProperty(vProList, pSpriteData);

	// 强化, 培养, 资质加成属性转化
	for (size_t i = 0; i < vProList.size(); i++)
	{
		Property &tPro = vProList[i];
		tPro.nValue = tPro.nValue * fProConversionRate;
	}
	AddProperty(bySlot, &vProList, PRO_FROM_SPRITE_BLESS_CONVERSION);

	// 天赋技能属性转化 todo::调整位置
	// AddProperty(bySlot, &m_vSpriteTalentSkillProList, PRO_FROM_TALENT_SKILL_CONVERSION);
}

// 计算精灵天赋技能属性
void CPropertyManager::CalculateSpriteTalentSkillProperty()
{
	CSpriteManager *pSpriteMgr = m_pPlayer->GetSpiritManager();
	if (!pSpriteMgr)
		return;

	// 天赋技能属性
	vector<SpiritData> vBlessSpriteList;
	pSpriteMgr->GetBlessSpriteList(vBlessSpriteList);
	for (size_t i = 0; i < vBlessSpriteList.size(); ++i)
	{
		const SpiritData *pSprite = &vBlessSpriteList[i];
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(pSprite->dwId, pSprite->wTrainStage);
		if (!pTrainCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置[spirit_id:%d,TrainStage:%d]",
				__FUNCTION__, pSprite->dwId, pSprite->wTrainStage);
			continue;
		}
		const SpriteTalentSkill_Config *pTalentSkillCfg = g_pCfgMgr->GetSpriteTalentSkill(pSprite->dwId, pTrainCfg->wTalentLevel);
		if (!pTalentSkillCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵天赋技能配置[SpriteId:%d, TalentSkillLevel:%d]",
				__FUNCTION__, pSprite->dwId, pTrainCfg->wTalentLevel);
			continue;
		}
		// 记录影响英雄的属性列表
		vector<Property>::const_iterator hero_pro_iter = pTalentSkillCfg->vHeroProList.begin();
		for (; hero_pro_iter != pTalentSkillCfg->vHeroProList.end(); ++hero_pro_iter)
		{
			//m_vSpriteProList.push_back(*hero_pro_iter);
			m_vSpriteTalentSkillProList.push_back(*hero_pro_iter);
		}

		// 记录影响精灵的属性列表
		//float fProConversionRate = ceil(pTrainCfg->dwProConversionRate * PROPERTY_DECREASE_MULTIPLE);
		vector<Property>::const_iterator sprite_pro_iter = pTalentSkillCfg->vSpriteProList.begin();
		for (; sprite_pro_iter != pTalentSkillCfg->vSpriteProList.end(); ++sprite_pro_iter)
		{
// 			Property tmp = *sprite_pro_iter;
// 			tmp.nValue = tmp.nValue * fProConversionRate;
// 			m_vSpriteTalentSkillProList.push_back(tmp);
			m_vSpriteProList.push_back(*sprite_pro_iter);
		}
	}
}

// 计算爵位属性
void CPropertyManager::CalculateNobilityProperty()
{
	WORD wNobilityLevel = m_pPlayer->GetProperty(ROLE_PRO_NOBILITY_LEVEL);
	const Nobility_Config *pNobilityCfg = g_pCfgMgr->GetBobility(wNobilityLevel);
	if (pNobilityCfg)
		m_vNobilityProList = pNobilityCfg->vProList;
}

// 计算团队属性
void CPropertyManager::CalculateHeroBreakTeamProperty()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwHeroUID = m_pHeroInfo->arFormationList[i].dwHeroUID;

		// 阵型上木有英雄不需要处理
		if (!dwHeroUID)
			continue;

		const HeroInfo *pHero = pHeroMgr->GetHero(dwHeroUID);
		if (!pHero)
			return;

		const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
		if (!pHeroCfg)
			return;

		// 突破属性
		const HeroBreak_Config *pBreakCfg = g_pCfgMgr->GetHeroBreak(pHero->dwIntoId, pHero->wBreakLevel);
		if (!pBreakCfg)
			continue;

		// 属性
		vector<Property>::const_iterator iter = pBreakCfg->vTeamProList.begin();
		for (; iter != pBreakCfg->vTeamProList.end(); ++iter)
		{
			m_vHeroBreakTeamProList.push_back(*iter);
		}
	}
}

// 计算占卜数据
void CPropertyManager::CalculateZhanBuProperty()
{
	PlayerZhanBuInfo &tZhanBuInfo = m_pPlayer->GetRoleFullInfo().tZhanBuInfo;

	// 成就属性
	const ZhanBuAchievement_Config *pAchievementCfg = g_pCfgMgr->GetZhanBuAchievement(tZhanBuInfo.wAchievementLevel);
	if (pAchievementCfg)
	{
		m_vZhanBuProList = pAchievementCfg->vProList;
	}

	// 收藏属性
	const CollectionGroup_Config *pGroupCfg = NULL;
	for (WORD i = 0; i < tZhanBuInfo.wCollectionBookNum; ++i)
	{
		CollectionBook &tBook = tZhanBuInfo.arCollectionBookList[i];
		for (WORD k = 0; k < tBook.wCollectionGroupNum; ++k)
		{
			CollectionGroup &tGroup = tBook.arCollectionGroupList[k];
			pGroupCfg = g_pCfgMgr->GetCollectionGroup(tGroup.wId);
			if (pGroupCfg)
			{
				// 激活属性
				m_vZhanBuProList.insert(m_vZhanBuProList.end(), pGroupCfg->vActivateProList.begin(), pGroupCfg->vActivateProList.end());

				// 升级属性
				for (WORD m = 1; m < tGroup.wStar; ++m)
				{
					m_vZhanBuProList.insert(m_vZhanBuProList.end(), pGroupCfg->vUpgradeProList.begin(), pGroupCfg->vUpgradeProList.end());
				}
			}
		}
	}
}

// 计算援军助威属性
void CPropertyManager::CalculateReinforcementEncourageProperty()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	
	WORD wLevel = pHeroMgr->GetReinforcementMinLevel();

	// 0级不处理
	if (!wLevel)
		return;

	const ReinforcementEncourage_Config *pCfg = g_pCfgMgr->GetReinforcementEncourage(wLevel);
	if (!pCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]找不到援军助威配置[ReinforcementLevel:%d]",__FUNCTION__, wLevel);
		return;
	}

	m_vReinforcementEncourageProList = pCfg->vProList;
}