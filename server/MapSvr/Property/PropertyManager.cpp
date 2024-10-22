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

// ȡ��ս��
DWORD CPropertyManager::GetAllFightingPower()
{
	CalculateProperty();

	return CalculateFightingPower();
}

// ��������
void CPropertyManager::CalculateProperty()
{
	UINT64 lCurTime = GetTickCount64();

	m_vTitleProList.clear();
	m_vSpriteProList.clear();
	m_vNobilityProList.clear();
	m_vHeroBreakTeamProList.clear();
	m_vZhanBuProList.clear();
	m_vReinforcementEncourageProList.clear();

	// �ƺ�����
	CalculateTitleProperty();

	// ��������
	CalculateSpriteProperty();

	// ��λ����
	CalculateNobilityProperty();

	// Ӣ��ͻ�ƶ�������
	CalculateHeroBreakTeamProperty();

	// ռ������(�ղ�,�ɾ�����)
	CalculateZhanBuProperty();

	// Ԯ����������
	CalculateReinforcementEncourageProperty();

	memset(m_arProList, 0, sizeof(m_arProList));
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		// ������ľ��Ӣ�۲���Ҫ����
		if (!m_pHeroInfo->arFormationList[i].dwHeroUID)
			continue;

		// Ӣ������
		CalculateHeroProperty(i);

		// װ������
		CalculateEquipmentProperty(i);

		// ��������
		CalculateTreasureProperty(i);

		// ��ʯ����
		CalculateGemProperty(i);

		// ��������
		CalculateCardProperty(i);

		// ����ף������
		CalculateSpriteBlessProperty(i);

		// �ƺ�����
		if (m_vTitleProList.size())
			AddProperty(i, &m_vTitleProList, PRO_FROM_TITLE);

		// ��������
		if (m_vSpriteProList.size())
			AddProperty(i, &m_vSpriteProList, PRO_FROM_SPRITE);

		// ��λ����
		if (m_vNobilityProList.size())
			AddProperty(i, &m_vNobilityProList, PRO_FROM_NOBILITY);

		// Ӣ��ͻ�ƶ�������
		if (m_vHeroBreakTeamProList.size())
			AddProperty(i, &m_vHeroBreakTeamProList, PRO_FROM_TEAM_BREAK);

		// ռ������
		if (m_vZhanBuProList.size())
			AddProperty(i, &m_vZhanBuProList, PRO_FROM_ZHANBU);

		// Ԯ����������
		if (m_vReinforcementEncourageProList.size())
			AddProperty(i, &m_vReinforcementEncourageProList, PRO_FROM_REINFORCEMENT_ENCOURAGE);

		// �츳��������ת��
		if (m_vSpriteTalentSkillProList.size())
			AddProperty(i, &m_vSpriteTalentSkillProList, PRO_FROM_TALENT_SKILL_CONVERSION);

		// ���Ա����ӳ� 
		CalculateIncProperty(i);
	}

	DWORD dwUsedTime = GetTickCount64() - lCurTime;
	if (dwUsedTime)
		MAPLOG(ERROR_LEVEL, "����ս������[%d ms]", dwUsedTime);
}

// ����ս��
DWORD CPropertyManager::CalculateFightingPower()
{
	// ����ս��
	DWORD dwTotalFightingPower = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pHeroInfo->arFormationList[i];

		// ������ľ��Ӣ�۲���Ҫ����
		if (!pFormation->dwHeroUID)
			continue;

		int *pProList = m_arProList[i];

		// Ѫ��ս��
		dwTotalFightingPower += CalculateBloodBattleValue(pProList[HERO_PRO_BLOOD], pProList[HERO_PRO_DODGE_RATE],
			pProList[HERO_PRO_BLOCK_RATE], pProList[HERO_PRO_BLOCK_RATE_CORRECT], pProList[HERO_PRO_CRIT_DEFENCE_RATE],
			pProList[HERO_PRO_CRIT_DEFENCE_RATE_CORRECT], pProList[HERO_PRO_DAMAGE_DECREASE_RATE]);

		// ����ս��
		dwTotalFightingPower += CalculateAttackBattleValue(pProList[HERO_PRO_ATTACK], pProList[HERO_PRO_HIT_RATE],
			pProList[HERO_PRO_CRIT_RATE], pProList[HERO_PRO_CRIT_RATE_CORRECT], pProList[HERO_PRO_DAMAGE_INCREASE_RATE],
			pProList[HERO_PRO_PENETRATION_RATE], pProList[HERO_PRO_DAMAGE_INCREASE_VALUE], pProList[HERO_PRO_PENETRATION_VALUE]);

		// ����ս��
		dwTotalFightingPower += CalculateDefenceBattleValue(pProList[HERO_PRO_PHY_DEF],
			pProList[HERO_PRO_MAG_DEF], pProList[HERO_PRO_DAMAGE_INCREASE_VALUE]);
	}

	return dwTotalFightingPower;
}

// ������Ͳ�λ����
bool CPropertyManager::FillSlotProList(BYTE bySlot, Property *pProList, BYTE &byProNum)
{
	byProNum = 0;
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "���Ͳ�λ����Խ��[slot:%d]", bySlot);
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

// ȡ���Ͳ�λ�����б�
const int* CPropertyManager::GetSlotProList(BYTE bySlot)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "���Ͳ�λ����Խ��[slot:%d]", bySlot);
		return NULL;
	}

	return m_arProList[bySlot];
}

// Ѫ��ս��=Ѫ������*Ѫ��ս��ϵ��*����ս��ϵ��*��ս��ϵ��*����ս��ϵ��*����ս��ϵ��
// Ѫ��ս��ϵ��=0.1(0.5)
// ����ս��ϵ��=1+������
// ��ս��ϵ��=1/MAX(1-����*(0.2+��ǿ��),0.1)
// ����ս��ϵ��=4/(MAX(2-������*0.5,0.01)*MAX(2-������ǿ��*0.5,0.01))
// ����ս��ϵ��=1+�ٷֱȼ���
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

// ����ս��=����*����ս��ϵ��*����ս��ϵ��*����ս��ϵ��*�˺��ӳ�ս��ϵ��*�ٷֱȴ�͸ս��ϵ��+�˺�����*�˺�����ս��ϵ��+�̶�ֵ��͸*�̶�ֵ��͸ս��ϵ��
// ����ս��ϵ��=1.2(5)
// ����ս��ϵ��=1+������
// ����ս��ϵ��=(1.05+������)/1.05*(2+����ǿ��)/2
// �˺��ӳ�ս��ϵ��=1+�˺��ӳ�
// �ٷֱȴ�͸ս��ϵ��=1+�ٷֱȴ�͸
// �˺�����ս��ϵ��=1.8(10)
// �̶�ֵ��͸ս��ϵ��=2(5)
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

// ����ս��=���*���ս��ϵ��+����*����ս��ϵ��+�̶�ֵ����*�̶�ֵ����ս��ϵ��
// ���ս��ϵ��=2(5)
// ����ս��ϵ��=2(5)
// �̶�ֵ����ս��ϵ��=1.8(10)
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
		"����",
		"����",
		"���",
		"����",
		"����",

		"����",
		"����",
		"�˺�",
		"����",

		"��͸",
		"����",

		"������",
		"������",
		"������",
		"������",
		"����",
		"������",
		"����",
		"����",

		"����",
		"������",

		"ŭ��",

		"�����ӳ�",
		"����ӳ�",
		"�����ӳ�",
		"�����ӳ�",
		"�ٶ�",

		"�������ռӳ�",
		"������ռӳ�",
		"�������ռӳ�",
		"�������ռӳ�"
	};

	CalculateProperty();

	for (int i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		MAPLOG(ERROR_LEVEL, "Ӣ�۲�λ[%d]����",i);
		int *pHeroProList = m_arProList[i];
		for (int k = 0; k < HERO_PRO_MAX; ++k)
		{
			MAPLOG(ERROR_LEVEL, "����[%s=%d] ", arProName[k].c_str(), pHeroProList[k]);
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

// Ӣ��ս��=Ѫ��ս��+����ս��+����ս��
// float CPropertyManager::CalculateHeroBattleValue(const HeroInfo *pHeroInfo)
// {
// 	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroInfo->dwIntoId);
// 	if (!pHeroCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "�Ҳ���Ӣ�۵�����[HeroInfoId:%d]", pHeroInfo->dwIntoId);
// 		return 0;
// 	}
// 
// 	const HeroPro_Config *pHeroProCfg = CConfigManager::Instance()->GetHeroPro(
// 		pHeroCfg->byProfession, pHeroInfo->wQuality, pHeroInfo->wLevel);
// 	if (!pHeroProCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "�Ҳ���Ӣ����������[Profession:%d,Quality:%d,level:%d]",
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
		MAPLOG(GUI_LEVEL, "���Ͳ�λ����Խ��[slot:%d]", bySlot);
		return;
	}

	vector<Property>::const_iterator iter = pProList->begin();
	for (; iter != pProList->end(); ++iter)
	{
		AddProperty(bySlot, *iter, wFromId);
	}
}

// ���ս������
void CPropertyManager::AddProperty(BYTE bySlot, const Property &tProperty, WORD wFromId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(GUI_LEVEL, "���Ͳ�λ����Խ��[slot:%d]", bySlot);
		return;
	}

	if (tProperty.wType >= HERO_PRO_MAX)
	{
		MAPLOG(GUI_LEVEL, "��ӵ�ս������Խ��[type:%d]", tProperty.wType);
		return;
	}

	if (0 != tProperty.nValue)
		m_arProList[bySlot][tProperty.wType] += tProperty.nValue;

	// todo:: �������BUGʹ��
//  	if (tProperty.wType == HERO_PRO_BLOOD)
//  		MAPLOG(GUI_LEVEL, "��ɫѪ���仯[role:%d,slot:%d,add_hp:%d,cur_hp:%d,from_id:%d]", 
//  		m_pPlayer->GetRoleId(), bySlot, tProperty.nValue, m_arProList[bySlot][tProperty.wType], wFromId);
}

// ����Ӣ������
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


	// Ӣ�ۻ�������
	const HeroPro_Config *pHeroProCfg = g_pCfgMgr->GetHeroPro(pHeroCfg->byProfession, pHero->wQuality, pHero->wLevel);
	if (pHeroProCfg)
	{
		AddProperty(bySlot, &pHeroProCfg->vProList, PRO_FROM_HERO_BASE);
	}

	// ͻ������
	const HeroBreak_Config *pBreakCfg = g_pCfgMgr->GetHeroBreak(pHero->dwIntoId, pHero->wBreakLevel);
	if (pBreakCfg)
	{
		AddProperty(bySlot, &pBreakCfg->vSelfProList, PRO_FROM_HERO_BREAK);
	}

	// ��������
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		// �������
		Property pro;
		pro.wType = GetTrainProType(i);
		pro.nValue = pHero->arTrainProValueList[i];
		AddProperty(bySlot, pro, PRO_FROM_HERO_TRAIN);

		// �ȼ���������
		BYTE byProLevel = pHero->arTrainProLvList[i];
		BYTE byIsTake = pHero->arTrainRewardFlag[i];
		const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(pro.wType, byProLevel);
		// ��ǰ�ȼ�(�ȼ�������ֵ��Ҫ����)
		if (pTrainCfg && pro.nValue >= pTrainCfg->nProMax && !byIsTake)
		{
			AddProperty(bySlot, &pTrainCfg->vLevelUpProList, PRO_FROM_HERO_TRAIN);
		}
		// ��һ�ȼ�
		else if (byProLevel)
		{
			pTrainCfg = g_pCfgMgr->GetHeroTrain(pro.wType, byProLevel-1);
			if (pTrainCfg)
				AddProperty(bySlot, &pTrainCfg->vLevelUpProList, PRO_FROM_HERO_TRAIN);
		}
	}

	// ��������
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

	// ��������
	const HeroAwaken_Config *pAwakenCfg = 
		g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality, pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
	if (pAwakenCfg)
	{
		AddProperty(bySlot, &pAwakenCfg->vProList, PRO_FROM_HERO_AWAKEN);
		AddProperty(bySlot, &pAwakenCfg->vItemProList, PRO_FROM_HERO_AWAKEN);
	}

	int nQualifications = pHeroCfg->nQualifications;

	// ��������(�ȼ�������)
	if (pHero->wQuality >= QUALITY_ORANGE)
	{
		for (WORD i = pHeroCfg->wInitQuality; i <= pHero->wQuality; ++i)
		{
			// ȡ��ǰƷ������
			WORD wStageMax = (i == pHero->wQuality) ? pHero->wQualityStage : MAX_QUALITY_STAGE;
			for (WORD k = 0; k <= wStageMax; ++k)
			{
				const HeroEvolution_Config *pEvolutionCfg = g_pCfgMgr->GetHeroEvolution(i, k);

				// ����Ҳ���,���������һ��
				if (!pEvolutionCfg)
					break;

				AddProperty(bySlot, &pEvolutionCfg->vProList, PRO_FROM_HERO_EVOLUTION);
				nQualifications += pEvolutionCfg->wAddQualifications;
			}
		}
	}

	// ��������
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

	// ����Ӣ������
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

	// �����ǰƷ�ʸ���ʼ��һ��
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

	// ����װ������(���߹�ϵ,ֻҪ����һ���Ϳ���)
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

	// ������������(���߹�ϵ,ֻҪ����һ���Ϳ���)
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

	// ������������
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

	// ����֮��
	PlayerSpriteInfo &tSpriteData = m_pPlayer->GetRoleFullInfo().tSpiritInfo;
	for (WORD i = 1; i <= tSpriteData.tHeartOfWorld.wStage; ++i)
	{
		const HeartOfWorld_Config *pHeartOfWorldCfg = g_pCfgMgr->GetHeartOfWorld(i);
		if (!pHeartOfWorldCfg)
		{
			MAPLOG(GUI_LEVEL, "�Ҳ�������׶�����[stage:%d]", i);
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
	// ����ͼ��
	for (WORD i = 0; i < tSpriteData.wPokedexNum; ++i)
	{
		SpiritPokedex &tPokedex = tSpriteData.arPokedexList[i];
		const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(tPokedex.wPokedexId, tPokedex.wLevel);
		if (pPokedexCfg)
		{
			AddProperty(bySlot, pPokedexCfg->vAddPro, PRO_FROM_SPRITE_POKEDEX);
		}
	}
	// ���Ἴ��
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

// ����װ������
void CPropertyManager::CalculateEquipmentProperty(BYTE bySlot)
{
	CEquipmentManager *pEquipmentMgr = m_pPlayer->GetEquipmentManager();

	// װ������
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

		// ǿ������
		AddProperty(bySlot, pEquipmentCfg->tStrengthenInitPro, PRO_FROM_EQUIP_ENHANCE);
		for (WORD k = 1; k < pEqupment->wEnhanceLevel; ++k)
		{
			AddProperty(bySlot, pEquipmentCfg->tStrengthenLevelPro, PRO_FROM_EQUIP_ENHANCE);
		}

		// ��������
		for (WORD k = 0; k < pEqupment->wRefineLevel; ++k)
		{
			AddProperty(bySlot, &pEquipmentCfg->vRefineLevelProList, PRO_FROM_EQUIP_REFINE);
		}

		// �츳����
		if (pEquipmentCfg->wTalentId)
		{
			const EquipmentTalent_Config *pTalentCfg = g_pCfgMgr->GetEquipmentTalent(pEquipmentCfg->wTalentId, pEqupment->wEnhanceLevel);
			if (pTalentCfg)
			{
				AddProperty(bySlot, &pTalentCfg->vProList, PRO_FROM_EQUIP_TALENT);
			}
		}

		// ��������
		if (pEquipmentCfg->wStarMax)
		{
			const EquipmentStarUp_Config *pStarUpCfg = g_pCfgMgr->GetEquipmentStartUp(pEquipmentCfg->byType, pEquipmentCfg->wQuality, pEqupment->wStar, pEqupment->wStarLevel);
			if (pStarUpCfg)
			{
				AddProperty(bySlot, &pStarUpCfg->vProList, PRO_FROM_EQUIP_STARUP);
			}
		}
	}


	// ��������
	WORD wEnhanceForgeLevel = 0;
	WORD wRefineForgeLevel = 0;
	bool bEnhanceInit = false;
	bool bRefineInit = false;
	for (BYTE i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwEquipUId = m_pHeroInfo->arFormationList[bySlot].arEquipmentList[i];

		// ĳ����λ�ϵ�װ��Ϊ��,�����ȼ���Ϊ0
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

	// ǿ����������
	if (wEnhanceForgeLevel)
	{
		const EquipmentForge_Config *pForgeCfg = g_pCfgMgr->GetEquipementStrengthenForge(wEnhanceForgeLevel);
		if (pForgeCfg)
		{
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_EQUIP_ENHANCE_FORGE);
		}
	}

	// ������������
	if (wRefineForgeLevel)
	{
		const EquipmentForge_Config *pForgeCfg = g_pCfgMgr->GetEquipementRefineForge(wRefineForgeLevel);
		if (pForgeCfg)
		{
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_EQUIP_REFINE_FORGE);
		}
	}

	// ��װ���� todo::��Щ����,�պ���������û�и��õİ취
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	const vector<EquipmentSuit_Config> *pSuitCfgList = g_pCfgMgr->GetEquipmentSuitList();
	if (pSuitCfgList)
	{
		for (size_t i = 0; i < pSuitCfgList->size(); ++i)
		{
			// ���㴩����װ�ļ���
			int nNum = 0;
			const EquipmentSuit_Config &tSuitCfg = pSuitCfgList->at(i);
			for (size_t k = 0; k < tSuitCfg.vEquipmentList.size(); ++k)
			{
				DWORD dwInfoId = tSuitCfg.vEquipmentList[k];
				if (pHeroMgr->IsEquipmentWear(bySlot, dwInfoId))
					nNum++;
			}

			// ��Ӽ�����Ӧ������
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

// ���㱦������
void CPropertyManager::CalculateTreasureProperty(BYTE bySlot)
{
	CTreasureManager *pTreasureMgr = m_pPlayer->GetTreasureManager();

	// ��������
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

		// ǿ������
		AddProperty(bySlot, &pTreasureCfg->vStrengthenInitProList, PRO_FROM_TREASURE_ENHANCE);	// ��ʼ����
		for (WORD k = 1; k < pTreasure->wEnhanceLevel; ++k)
		{
			AddProperty(bySlot, &pTreasureCfg->vStrengthenLevelProList, PRO_FROM_TREASURE_ENHANCE);	// �ȼ�����
		}

		// ��������
		for (WORD k = 0; k < pTreasure->wRefineLevel; ++k)
		{
			AddProperty(bySlot, &pTreasureCfg->vRefineLevelProList, PRO_FROM_TREASURE_REFINE);	// �ȼ�����
		}
	}

	// ��������
	WORD wEnhanceForgeLevel = 0;
	WORD wRefineForgeLevel = 0;
	bool bEnhanceInit = false;
	bool bRefineInit = false;
	for (BYTE i = 0; i < TREASURE_TYPE_MAX; ++i)
	{
		DWORD dwTreasureUId = m_pHeroInfo->arFormationList[bySlot].arTreasureList[i];

		// ĳ����λ�ϵ�װ��Ϊ��,�����ȼ���Ϊ0
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

	// ǿ����������
	if (wEnhanceForgeLevel)
	{
		const TreasureForge_Config *pForgeCfg = g_pCfgMgr->GetTreasureStrengthenForge(wEnhanceForgeLevel);
		if (pForgeCfg)
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_TREASURE_ENHANCE_FORGE);
	}

	// ��������
	if (wRefineForgeLevel)
	{
		const TreasureForge_Config *pForgeCfg = g_pCfgMgr->GetTreasureRefineForge(wRefineForgeLevel);
		if (pForgeCfg)
			AddProperty(bySlot, &pForgeCfg->vProList, PRO_FROM_TREASURE_REFINE_FORGE);
	}
}

// ���㱦ʯ����
void CPropertyManager::CalculateGemProperty(BYTE bySlot)
{
	Formation *pFormation = &m_pHeroInfo->arFormationList[bySlot];

	m_GemSkillList.clear();

	// ��ʯ����
	for (BYTE i = 0; i < MAX_GEM_HOLE_NUM; ++i)
	{
		DWORD dwGemId = pFormation->arGemList[i];
		if (!dwGemId)
			continue;

		const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
		if (!pGemCfg)
			continue;

		// ��ʯ��ʼ����
		AddProperty(bySlot, &pGemCfg->vInitProList, PRO_FROM_GEM_INIT);

		// ���㼼�ܵȼ�
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

	// ��ʯ��������
	map<WORD, WORD>::iterator iter = m_GemSkillList.begin();
	for (; iter != m_GemSkillList.end(); ++iter)
	{
		const GemSkill_Config *pSkillCfg = g_pCfgMgr->GetGemSkill(iter->first, iter->second);
		if (pSkillCfg)
			AddProperty(bySlot, &pSkillCfg->vProList, PRO_FROM_GEM_SKILL);
	}
}

// ���㿨������
void CPropertyManager::CalculateCardProperty(BYTE bySlot)
{
	Formation *pFormation = &m_pHeroInfo->arFormationList[bySlot];
	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();

	// ��������
	for (BYTE i = 0; i < MAX_CARD_SLOT_NUM; ++i)
	{
		DWORD dwCardId = pFormation->arCardList[i];
		if (!dwCardId)
			continue;

		const Card *pCard = pZhanBuMgr->GetCard(dwCardId);
		if (!pCard)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[CardUId:%d]", __FUNCTION__, dwCardId);
			continue;
		}

		const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
		if (!pCardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[CardInfoId:%d]", __FUNCTION__, pCard->dwInfoId);
			continue;
		}

		// ���������б�
		AddProperty(bySlot, &pCardCfg->vBaseProList, PRO_FROM_CARD_BASE);

		// �ȼ������б�
		for (WORD k = 1; k < pCard->wLevel; ++k)
		{
			AddProperty(bySlot, &pCardCfg->vLevelProList, PRO_FROM_CARD_LEVEL);
		}

		// ͻ�������б�
		for (WORD k = 0; k < pCard->wBreakLevel; ++k)
		{
			AddProperty(bySlot, &pCardCfg->vBreakProList, PRO_FROM_CARD_BREAK);
		}
	}
}

// ����ӳ�����
void CPropertyManager::CalculateIncProperty(BYTE bySlot)
{
	int *pProList = m_arProList[bySlot];
	pProList[HERO_PRO_ATTACK] *= (1 + pProList[HERO_PRO_ATTACK_RATE] * 0.0001);			// �����ӳ�
	pProList[HERO_PRO_ATTACK] *= (1 + pProList[HERO_PRO_ATTACK_INC_RATE] * 0.0001);		// �������ռӳ�


	pProList[HERO_PRO_PHY_DEF] *= (1 + pProList[HERO_PRO_PHY_DEF_RATE] * 0.0001);		// ����ӳ�
	pProList[HERO_PRO_PHY_DEF] *= (1 + pProList[HERO_PRO_PHY_DEF_INC_RATE] * 0.0001);	// ������ռӳ�

	pProList[HERO_PRO_MAG_DEF] *= (1 + pProList[HERO_PRO_MAG_DEF_RATE] * 0.0001);		// �����ӳ�
	pProList[HERO_PRO_MAG_DEF] *= (1 + pProList[HERO_PRO_MAG_DEF_INC_RATE] * 0.0001);	// �������ռӳ�

	pProList[HERO_PRO_BLOOD] *= (1 + pProList[HERO_PRO_BLOOD_RATE] * 0.0001);			// �����ӳ�
	pProList[HERO_PRO_BLOOD] *= (1 + pProList[HERO_PRO_BLOOD_INC_RATE] * 0.0001);		// �������ռӳ�

	//pProList[HERO_PRO_ATTACK] += (pProList[HERO_PRO_ATTACK] * pProList[HERO_PRO_ATTACK_INC_RATE] * 0.0001);
	//pProList[HERO_PRO_PHY_DEF] += (pProList[HERO_PRO_PHY_DEF] * pProList[HERO_PRO_PHY_DEF_INC_RATE] * 0.0001);
	//pProList[HERO_PRO_MAG_DEF] += (pProList[HERO_PRO_MAG_DEF] * pProList[HERO_PRO_MAG_DEF_INC_RATE] * 0.0001);
	//pProList[HERO_PRO_BLOOD] += (pProList[HERO_PRO_BLOOD] * pProList[HERO_PRO_BLOOD_INC_RATE] * 0.0001);
}

// ����ƺ�����
void CPropertyManager::CalculateTitleProperty()
{
	PlayerTitleInfo &tRoleTitleInfo = m_pPlayer->GetRoleFullInfo().tSecondInfo.tTitleInfo;
	for (WORD i = 0; i < tRoleTitleInfo.wTitleNum; ++i)
	{
		TitleData &tTitleData = tRoleTitleInfo.arTitleList[i];;

		// ����Ƿ����
		if (tTitleData.nOverTime && time(NULL) >= tTitleData.nOverTime)
		{
			// todo::�Ƿ������ﴦ��ɾ������
		}
		else
		{
			const Title_Config *pTitleCfg = g_pCfgMgr->GetTitle(tTitleData.nTitleId);
			if (!pTitleCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ƺ�����[TitleId:%d]", __FUNCTION__, tTitleData.nTitleId);
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

// ���㾫������
void CPropertyManager::CalculateSpriteProperty()
{
	m_vSpriteProList.clear();
	m_vSpriteTalentSkillProList.clear();
	CSpriteManager *pSpriteMgr = m_pPlayer->GetSpiritManager();
	if (!pSpriteMgr)
		return;

	DWORD dwToBattleSpriteId = pSpriteMgr->GetToBattleSpiritId();

	// ��ǰû������ľ���
	if (!dwToBattleSpriteId)
		return;

	// ȡ����ľ�������
	const SpiritData *pToBattleSprite = pSpriteMgr->GetSpiritEx(dwToBattleSpriteId);
	if (!pToBattleSprite)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ľ�������[spirit_id:%d]", __FUNCTION__, dwToBattleSpriteId);
		return;
	}

	// ȡ������������
	const SpriteTrain_Config *pToBattleTrainCfg = g_pCfgMgr->GetSpriteTrain(pToBattleSprite->dwId, pToBattleSprite->wTrainStage);
	if (!pToBattleTrainCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[SpriteId:%d,TrainStage:%d]",
			__FUNCTION__, pToBattleSprite->dwId, pToBattleSprite->wTrainStage);
 		return;
 	}

	float fProConversionRate = double(pToBattleTrainCfg->dwProConversionRate) * PROPERTY_DECREASE_MULTIPLE;

	// ��BUG�ģ�����Ϊ��֮ǰ��ս�����䣬��ʱʹ��ԭ���
	if (g_MapServer.IsUseOldPower())
		fProConversionRate = ceil(pToBattleTrainCfg->dwProConversionRate * PROPERTY_DECREASE_MULTIPLE);


	// ���㾫���������(ǿ��,����)
	CalculateSpriteBaseProperty(m_vSpriteProList, pToBattleSprite);

	// ǿ��, ����, ���ʼӳ�����ת��
	for (size_t i = 0; i < m_vSpriteProList.size(); i++)
	{
		Property &tPro = m_vSpriteProList[i];
		tPro.nValue = tPro.nValue * fProConversionRate;
	}

	// �츳��������
	CalculateSpriteTalentSkillProperty();
}

// ���㾫���������(ǿ��,����)
void CPropertyManager::CalculateSpriteBaseProperty(vector<Property> &vProList, const SpiritData *pSpriteData)
{
	// ǿ������
	const SpiritEnhance_Config *pEnhanceCfg = g_pCfgMgr->GetSpiritEnhance(pSpriteData->dwId, pSpriteData->wEnhanceLevel);
	if (pEnhanceCfg)
	{
		// ��������Ӱ������Լӳ�ϵ��
		int nAddRate = 0;
		const Spirit_Config *pSpriteCfg = g_pCfgMgr->GetSpirit(pSpriteData->dwId);
		if (pSpriteCfg)
		{
			WORD wQualifications = pSpriteCfg->GetQualifications(pSpriteData->wStarLevel);
			const SpriteQualifications_Config *pQualificationsCfg = g_pCfgMgr->GetSpriteQualifications(wQualifications);
			if (pQualificationsCfg)
				nAddRate = pQualificationsCfg->nAddRate;
		}

		// ǿ������ + ���ʼӳ�����
		Property pro;
		vector<Property>::const_iterator iter = pEnhanceCfg->vAddProList.begin();
		for (; iter != pEnhanceCfg->vAddProList.end(); ++iter)
		{
			pro = *iter;
			pro.nValue += (pro.nValue * nAddRate * PROPERTY_DECREASE_MULTIPLE);
			vProList.push_back(pro);
		}
	}

	// ��������
	const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(pSpriteData->dwId, pSpriteData->wTrainStage);
	if (pTrainCfg && pTrainCfg->vTrainProList.size())
	{
		// ǰ�������
		for (size_t i = 0; i < pTrainCfg->vTrainTotalProList.size(); ++i)
		{
			Property tPro = pTrainCfg->vTrainTotalProList[i];

			// �������
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

			// �������
			if (bAdd)
				vProList.push_back(tPro);
		}

		// ��ǰ������ (ֻ���������Ĳ��д�����)
		if (pSpriteData->wTrainTimes)
		{
			for (size_t i = 0; i < pTrainCfg->vTrainProList.size(); ++i)
			{
				Property tPro = pTrainCfg->vTrainProList[i];

				// ����ת��
				//if (pSpriteData->wTrainTimes)
				tPro.nValue *= pSpriteData->wTrainTimes;

				// �������
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

				// �������
				if (bAdd)
					vProList.push_back(tPro);
			}
		}
	}
}

// ���㾫��ף������
void CPropertyManager::CalculateSpriteBlessProperty(BYTE bySlot)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	DWORD dwBlessSpriteId = m_pHeroInfo->arFormationList[bySlot].dwBlessSpriteId;

	// û��ף������
	if (!dwBlessSpriteId)
		return;

	// ȡ��������
	const SpiritData *pSpriteData = m_pPlayer->GetSpiritManager()->GetSpiritEx(dwBlessSpriteId);
	if (!pSpriteData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[SpriteId:%d]",__FUNCTION__, dwBlessSpriteId);
		return;
	}

	// ȡ��������
	const SpriteSpecialProperty_Config *pSpecialProCfg = g_pCfgMgr->GetSpriteSpecialProperty(pSpriteData->dwId, pSpriteData->wStarLevel);
	if (!pSpecialProCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������������[SpriteId:%d,Star:%d]", 
			__FUNCTION__, pSpriteData->dwId, pSpriteData->wStarLevel);
		return;
	}

	// ȡ������������
	const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(pSpriteData->dwId, pSpriteData->wTrainStage);
	if (!pTrainCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������������[SpriteId:%d,TrainStage:%d]",
			__FUNCTION__, pSpriteData->dwId, pSpriteData->wTrainStage);
		return;
	}

	// ף����������
	AddProperty(bySlot, &pSpecialProCfg->vBlessProList, PRO_FROM_SPRITE_BLESS_BASE);

	float fProConversionRate = double(pTrainCfg->dwProConversionRate) * PROPERTY_DECREASE_MULTIPLE;

	// ��BUG�ģ�����Ϊ��֮ǰ��ս�����䣬��ʱʹ��ԭ���
	if (g_MapServer.IsUseOldPower())
		fProConversionRate = ceil(pTrainCfg->dwProConversionRate * PROPERTY_DECREASE_MULTIPLE);

	// ����ף��ת������
	vector<Property> vProList;
	CalculateSpriteBaseProperty(vProList, pSpriteData);

	// ǿ��, ����, ���ʼӳ�����ת��
	for (size_t i = 0; i < vProList.size(); i++)
	{
		Property &tPro = vProList[i];
		tPro.nValue = tPro.nValue * fProConversionRate;
	}
	AddProperty(bySlot, &vProList, PRO_FROM_SPRITE_BLESS_CONVERSION);

	// �츳��������ת�� todo::����λ��
	// AddProperty(bySlot, &m_vSpriteTalentSkillProList, PRO_FROM_TALENT_SKILL_CONVERSION);
}

// ���㾫���츳��������
void CPropertyManager::CalculateSpriteTalentSkillProperty()
{
	CSpriteManager *pSpriteMgr = m_pPlayer->GetSpiritManager();
	if (!pSpriteMgr)
		return;

	// �츳��������
	vector<SpiritData> vBlessSpriteList;
	pSpriteMgr->GetBlessSpriteList(vBlessSpriteList);
	for (size_t i = 0; i < vBlessSpriteList.size(); ++i)
	{
		const SpiritData *pSprite = &vBlessSpriteList[i];
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(pSprite->dwId, pSprite->wTrainStage);
		if (!pTrainCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[spirit_id:%d,TrainStage:%d]",
				__FUNCTION__, pSprite->dwId, pSprite->wTrainStage);
			continue;
		}
		const SpriteTalentSkill_Config *pTalentSkillCfg = g_pCfgMgr->GetSpriteTalentSkill(pSprite->dwId, pTrainCfg->wTalentLevel);
		if (!pTalentSkillCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������츳��������[SpriteId:%d, TalentSkillLevel:%d]",
				__FUNCTION__, pSprite->dwId, pTrainCfg->wTalentLevel);
			continue;
		}
		// ��¼Ӱ��Ӣ�۵������б�
		vector<Property>::const_iterator hero_pro_iter = pTalentSkillCfg->vHeroProList.begin();
		for (; hero_pro_iter != pTalentSkillCfg->vHeroProList.end(); ++hero_pro_iter)
		{
			//m_vSpriteProList.push_back(*hero_pro_iter);
			m_vSpriteTalentSkillProList.push_back(*hero_pro_iter);
		}

		// ��¼Ӱ�쾫��������б�
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

// �����λ����
void CPropertyManager::CalculateNobilityProperty()
{
	WORD wNobilityLevel = m_pPlayer->GetProperty(ROLE_PRO_NOBILITY_LEVEL);
	const Nobility_Config *pNobilityCfg = g_pCfgMgr->GetBobility(wNobilityLevel);
	if (pNobilityCfg)
		m_vNobilityProList = pNobilityCfg->vProList;
}

// �����Ŷ�����
void CPropertyManager::CalculateHeroBreakTeamProperty()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwHeroUID = m_pHeroInfo->arFormationList[i].dwHeroUID;

		// ������ľ��Ӣ�۲���Ҫ����
		if (!dwHeroUID)
			continue;

		const HeroInfo *pHero = pHeroMgr->GetHero(dwHeroUID);
		if (!pHero)
			return;

		const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
		if (!pHeroCfg)
			return;

		// ͻ������
		const HeroBreak_Config *pBreakCfg = g_pCfgMgr->GetHeroBreak(pHero->dwIntoId, pHero->wBreakLevel);
		if (!pBreakCfg)
			continue;

		// ����
		vector<Property>::const_iterator iter = pBreakCfg->vTeamProList.begin();
		for (; iter != pBreakCfg->vTeamProList.end(); ++iter)
		{
			m_vHeroBreakTeamProList.push_back(*iter);
		}
	}
}

// ����ռ������
void CPropertyManager::CalculateZhanBuProperty()
{
	PlayerZhanBuInfo &tZhanBuInfo = m_pPlayer->GetRoleFullInfo().tZhanBuInfo;

	// �ɾ�����
	const ZhanBuAchievement_Config *pAchievementCfg = g_pCfgMgr->GetZhanBuAchievement(tZhanBuInfo.wAchievementLevel);
	if (pAchievementCfg)
	{
		m_vZhanBuProList = pAchievementCfg->vProList;
	}

	// �ղ�����
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
				// ��������
				m_vZhanBuProList.insert(m_vZhanBuProList.end(), pGroupCfg->vActivateProList.begin(), pGroupCfg->vActivateProList.end());

				// ��������
				for (WORD m = 1; m < tGroup.wStar; ++m)
				{
					m_vZhanBuProList.insert(m_vZhanBuProList.end(), pGroupCfg->vUpgradeProList.begin(), pGroupCfg->vUpgradeProList.end());
				}
			}
		}
	}
}

// ����Ԯ����������
void CPropertyManager::CalculateReinforcementEncourageProperty()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	
	WORD wLevel = pHeroMgr->GetReinforcementMinLevel();

	// 0��������
	if (!wLevel)
		return;

	const ReinforcementEncourage_Config *pCfg = g_pCfgMgr->GetReinforcementEncourage(wLevel);
	if (!pCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ԯ����������[ReinforcementLevel:%d]",__FUNCTION__, wLevel);
		return;
	}

	m_vReinforcementEncourageProList = pCfg->vProList;
}