#include "stdafx.h"
#include "HeroManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\Property\PropertyManager.h"
#include "..\PlayerManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Activity\ActivityManager.h"
#include "..\ZhanBu\ZhanBuManager.h"
#include "DBCDefine.h"


CHeroManager::CHeroManager()
{
	m_dwSeriesId = 0;
}

CHeroManager::~CHeroManager()
{

}

void CHeroManager::Init(Player *pPlayer)
{
	m_dwSeriesId = 0;
	m_pPlayer = pPlayer;
	m_pPlayerHeroInfo = &m_pPlayer->GetRoleFullInfo().tHeroInfo;
}

// ����Ӣ��
void CHeroManager::OnHeroBattle(BYTE byPos, DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_HERO_TO_BATTLE_REQ;

	// ����Ƿ�ӵ�д�Ӣ��
	const HeroInfo *pHero = GetHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��û��ô�Ӣ��,��������![HeroUID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	if (!byPos || byPos > MAX_BATTLE_HERO_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����Ӣ�������λ�ò��Ϸ�![Pos:%d]", __FUNCTION__, byPos);
		return;
	}

	BYTE byIndexPos = byPos - 1;

	// ȡλ����������
 	const HeroToBattle_Config *pToBattleCfg = CConfigManager::Instance()->GetHeroToBattle(1,byPos);
 	if (!pToBattleCfg)
 	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������Ӣ��λ����������![Pos:%d]", __FUNCTION__, byPos);
 		return;
 	}
 
 	// ��⿪�ŵȼ�
 	if (pToBattleCfg->wNeedLevel > m_pPlayer->GetLevel())
 	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
 		MAPLOG(ERROR_LEVEL, "[%s]�ȼ����ƻ���������![Pos:%d,NeedLevel:%d]", __FUNCTION__, byPos, pToBattleCfg->wNeedLevel);
 		return;
 	}

	// ͬһ��Ӣ�۲���ͬʱ����
	if (!IsCanToBattle(pHero->dwIntoId, byPos))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ͬһ��Ӣ�۲���ͬʱ����![HeroInfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	DWORD dwOldHeroUID = m_pPlayerHeroInfo->arFormationList[byIndexPos].dwHeroUID;
	// ��¼����λ��
	m_pPlayerHeroInfo->arFormationList[byIndexPos].dwHeroUID = dwHeroUID;

	// ��һ������Ӣ��״̬ȥ��
	if (dwOldHeroUID)
		HeroBaseDataNotify(dwOldHeroUID);
	HeroBaseDataNotify(dwHeroUID);

	SendFormationList();

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_TO_BATTLE_RESP msg;
	msg.m_dwUId = dwHeroUID;
	msg.m_byPos = byPos;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(21, GetToBattleHeroNum(), false);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(9, GetFormationHeroMinLevel(), false);
}

// Ԯ������
void CHeroManager::OnReinforcementBattle(BYTE byPos, DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_REINFORCEMENT_TO_BATTLE_REQ;

	if (!byPos || byPos > MAX_REINFORCEMENT_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����Ԯ�������λ�ò��Ϸ�![Pos:%d]", __FUNCTION__,byPos);
		return;
	}

	BYTE bySlotIndex = byPos - 1;

	// ����
	if (dwHeroUID)
	{
		// ����Ƿ�ӵ�д�Ӣ��
		const HeroInfo *pHero = GetHero(dwHeroUID);
		if (!pHero)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]��û��ô�Ӣ��,��������![HeroUID:%d]", __FUNCTION__, dwHeroUID);
			return;
		}

		// ȡλ����������
		const HeroToBattle_Config *pToBattleCfg = CConfigManager::Instance()->GetHeroToBattle(2, byPos);
		if (!pToBattleCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������Ӣ��λ����������![Pos:%d]", __FUNCTION__, byPos);
			return;
		}

		// ��⿪�ŵȼ�
		if (pToBattleCfg->wNeedLevel > m_pPlayer->GetLevel())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�ȼ����ƻ���������![Pos:%d,NeedLevel:%d]", 
				__FUNCTION__, byPos, pToBattleCfg->wNeedLevel);
			return;
		}

		// ͬһ��Ӣ�۲���ͬʱ����
		if (!IsCanToBattleEx(pHero->dwIntoId, byPos))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]ͬһ��Ӣ�۲���ͬʱ����![InfoId:%d]",__FUNCTION__, pHero->dwIntoId);
			return;
		}
	}
	// ����
	else
	{ 
		// todo::��ʱ����Ҫ�����߼�
	}

	DWORD dwOldHeroUID = m_pPlayerHeroInfo->arReinforcementList[bySlotIndex];
	m_pPlayerHeroInfo->arReinforcementList[bySlotIndex] = dwHeroUID;

	// ���滻��Ӣ��
	if (dwOldHeroUID)
		HeroBaseDataNotify(dwOldHeroUID);

	// �������Ӣ��
	if (dwHeroUID)
		HeroBaseDataNotify(dwHeroUID);

	SendFormationList();

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_REINFORCEMENT_TO_BATTLE_RESP msg;
	msg.m_byPos = byPos;
	msg.m_dwUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);
}

// �ϳ�
void CHeroManager::OnCompose(WORD wHeroInfoId)
{
	WORD wFromMsgId = C2S_HERO_COMPOSE_REQ;

	const HeroCompose_Config *pComposeCfg = CConfigManager::Instance()->GetHeroCompose(wHeroInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "�Ҳ���Ӣ������[InfoId:%d]", wHeroInfoId);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// ����Ƿ����㹻�ĵ���
	if (pItemMgr->GetItemNum(pComposeCfg->dwItemId) < pComposeCfg->wItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		// ��ʾ��Ʒ��������
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�ۺϳ���Ҫ���ĵ���Ʒ����[HeroInfoId:%d,CostItemId:%d,CostItemNum:%d]", 
			__FUNCTION__, wHeroInfoId, pComposeCfg->dwItemId, pComposeCfg->wItemNum);
		return;
	}

	// ���Ӣ��
	if (!AddHero(wHeroInfoId, TRUE, FROM_HERO_COMPOSE))
	{
		// ��ʾ���Ӣ��ʧ��
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]������Ӣ��ʧ��[InfoId:%d]", __FUNCTION__, wHeroInfoId);
		return;
	}

	// ��������
	pItemMgr->RemoveItem(pComposeCfg->dwItemId, pComposeCfg->wItemNum, TRUE, FROM_HERO_COMPOSE);

	MSG_S2C_HERO_COMPOSE_RESP msg;
	msg.m_wHeroInfoId = wHeroInfoId;
	m_pPlayer->SendPacket(&msg);
}

// �ٻ�
void CHeroManager::OnSummon(BYTE byType)
{
	WORD wFromMsgId = C2S_HERO_SUMMON_REQ;

	int nSummonTimes = 1;

	vector<DWORD> vOutHeroList;
	bool bRet = false;
	switch (byType)
	{
	case NORMAL_FREE_SUMMON:
		bRet = DoNormalSummon(vOutHeroList);
		break;
	case NORMAL_ITEM_SUMMON:
		bRet = DoNormalSummon(vOutHeroList,g_GetConfigValue(CONFIG_NORMAL_SUMMON_COST_ITEM));
		break;
	case NORMAL_ITEM_ONEKEY_SUMMON:
		{
			DWORD dwCostItemId = g_GetConfigValue(CONFIG_NORMAL_SUMMON_COST_ITEM);
			BYTE byOneKeyTimes = g_GetConfigValue(CONFIG_NORMAL_ONEKEY_SUMMON_TIMES);
			for (BYTE i = 0; i < byOneKeyTimes; ++i)
			{
				bRet = DoNormalSummon(vOutHeroList,dwCostItemId);
			}
			nSummonTimes = byOneKeyTimes;
		}
		break;
	case FRIENDSHIP_POINT_SUMMON:
		bRet = DoNormalSummon(vOutHeroList, 0, g_GetConfigValue(129,1));
		break;
	case ADVANCED_FREE_SUMMON:
		bRet = DoAdvanceSummon(vOutHeroList);
		break;
	case ADVANCED_ITEM_SUMMON:
		bRet = DoAdvanceSummon(vOutHeroList,g_GetConfigValue(CONFIG_ADVANCED_SUMMON_COST_ITEM));
		break;
	case ADVANCED_GOLD_SUMMON:
		bRet = DoAdvanceSummon(vOutHeroList,0,g_GetConfigValue(CONFIG_ADVANCED_SUMMON_COST_YB));
		break;
	case ADVANCED_GOLD_ONEKEY_SUMMON:
		{
			int nCostDiamond = g_GetConfigValue(CONFIG_ADVANCED_ONEKEY_SUMMON_COST_YB);
			int nOneKeySummonTimes = g_GetConfigValue(CONFIG_ADVANCED_ONEKEY_SUMMON_TIMES);
			bRet = DoAdvanceSummon(vOutHeroList, 0, nCostDiamond, nOneKeySummonTimes);
			nSummonTimes = nOneKeySummonTimes;
		}
		break;
	case ADVANCED_ITEM_ONEKEY_SUMMON:
		{
			DWORD dwCostItemId = g_GetConfigValue(CONFIG_ADVANCED_SUMMON_COST_ITEM);
			int nOneKeySummonTimes = g_GetConfigValue(CONFIG_ADVANCED_ONEKEY_SUMMON_TIMES);
			bRet = DoAdvanceSummon(vOutHeroList, dwCostItemId, 0, nOneKeySummonTimes);
			nSummonTimes = nOneKeySummonTimes;
		}
		break;
	case FRIENDSHIP_POINT_ONE_KEY_SUMMON:
		{
			for (int i = 0; i < 10; ++i)
			{
				bRet = DoNormalSummon(vOutHeroList, 0, g_GetConfigValue(129, 1));
				if (!bRet)
					break;

				nSummonTimes = i + 1;
			}
		}
		break;
	default:
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]û������ٻ�����[type:%d]", m_pPlayer->GetUTF8RoleName(),byType);
		break;
	}

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ٻ�ʧ��[type:%d]", __FUNCTION__, byType);
		return;
	}

	SendSummonData();

	MSG_S2C_HERO_SUMMON_RESP msg;
	msg.m_byType = byType;
	msg.m_byHeroNum = 0;
	for (int i = 0; i < vOutHeroList.size(); ++i)
	{
		if (msg.m_byHeroNum >= HERO_ONE_KEY_SUMMON_TIMES)
			break;
		msg.m_HeroList[msg.m_byHeroNum++] = vOutHeroList[i];
	}
	m_pPlayer->SendPacket(&msg);

	// �ճ�����(�ͼ��ٻ�)
	m_pPlayer->OnUpdateDailyMission(8, nSummonTimes);

	if (ADVANCED_FREE_SUMMON == byType || ADVANCED_ITEM_SUMMON == byType || 
		ADVANCED_GOLD_SUMMON == byType || ADVANCED_GOLD_ONEKEY_SUMMON == byType)
	{
		// �ճ�����(�߼��ٻ�)
		m_pPlayer->OnUpdateDailyMission(9, nSummonTimes);
	}

	// ����㲥
	for (size_t i = 0; i < vOutHeroList.size(); ++i)
	{
		const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(vOutHeroList[i]);
		if (pHeroCfg && pHeroCfg->wInitQuality >= QUALITY_ORANGE)
		{
			CSystemNotice sys_notice(2, m_pPlayer->GetServerId());
			sys_notice.AddParam(m_pPlayer->GetRoleName());
			sys_notice.AddParam(vOutHeroList[i]);
			sys_notice.SendMsg();
		}
	}

}

// VIP�ٻ�
void CHeroManager::OnNetVIPSummon(WORD wCamp)
{
	// ���VIP�ȼ�
	int nNeedVipLevel = g_GetConfigValue(302);
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < nNeedVipLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]VIP�ٻ���Ҫ��VIP�ȼ�����[NeedVipLevel:%d]", __FUNCTION__, nNeedVipLevel);
		return;
	}

	// �����Ҫ����ʯ
	int nCostDiamond = g_GetConfigValue(303);
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]VIP�ٻ���Ҫ���ĵ���ʯ����[CostDiamond:%d]", __FUNCTION__, nCostDiamond);
		return;
	}

	// һ�������
	int nSummonTimes = g_GetConfigValue(304, 10);
	
	// �������ռ�
	if (GetBagEmptySize() < nSummonTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۱�������,VIP�ٻ�ʧ��!", __FUNCTION__);
		return;
	}
	
	vector<Item> vHeroList;

	// ���һ��Ӣ��
	for (int i = 1; i <= nSummonTimes; ++i)
	{
		const HeroVipSummon_Config *pSummonCfg = RandVipSummonHero(wCamp, i == nSummonTimes ? true : false);
		if (!pSummonCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ��VIP�ٻ����Ӣ��ʧ��!", __FUNCTION__);
			return;
		}

		vHeroList.push_back(Item(pSummonCfg->dwHeroId, pSummonCfg->dwHeroNum));
	}

	// �۳���ʯ
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_VIP_SUMMON);

	// ���Ӣ��
	m_pPlayer->AddObjectList(vHeroList, TRUE, FROM_VIP_SUMMON);

	// ��Ӧ
	MSG_S2C_HERO_VIP_SUMMON_RESP msg;
	msg.m_dwCamp = wCamp;
	msg.m_vHeroList = vHeroList;
	m_pPlayer->SendPacket(&msg);
}

// ����
void CHeroManager::OnLevelUp(DWORD dwHeroUID, DWORD *pCostHeroList, BYTE byCostHeroListNum)
{
	// ��������б��Ƿ����ظ�ID
	for (BYTE i = 0; i < byCostHeroListNum; ++i)
	{
		for (BYTE k = i + 1; k < byCostHeroListNum; ++k)
		{
			if (pCostHeroList[i] == pCostHeroList[k])
			{
				MAPLOG(ERROR_LEVEL, "[%s]���������б����ID�ظ����[UID:%d]", __FUNCTION__, dwHeroUID);
				return;
			}
		}
	}

	// ����Ӣ������
	HeroInfo *pLevelUpHero = FindHero(dwHeroUID);
	if (!pLevelUpHero)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�����Ӣ������[UID:%d]",__FUNCTION__, dwHeroUID);
		return;
	}

	// ����Ӣ������
	const Hero_Config *pLevelUpHeroCfg = CConfigManager::Instance()->GetHero(pLevelUpHero->dwIntoId);
	if (!pLevelUpHeroCfg)
	{
		// Ҫ������Ӣ�����ò�����
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]",__FUNCTION__,pLevelUpHero->dwIntoId);
		return;
	}

	// ����Ƿ�������
	if (pLevelUpHero->wLevel >= pLevelUpHeroCfg->wLimitLevel)
	{
		// �ѵ���ȼ�����
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ѵ���ȼ�����[HeroInfoId:%d]",__FUNCTION__, pLevelUpHero->dwIntoId);
		return;
	}

	DWORD arCostHeroInfoId[5] = { 0 };
	BYTE byRealCostHeroNum = 0;

	bool bLimitLevel = false;
	int nCostGold = 0;
	for (BYTE i = 0; i < byCostHeroListNum; ++i)
	{
		// ����Ӣ������
		const HeroInfo *pCostHero = GetHero(pCostHeroList[i]);
		if (!pCostHero)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���û�д�Ӣ��[HeroUId:%d]",__FUNCTION__, pCostHeroList[i]);
			return;
		}

		const HeroLevel_Config *pCostHeroLvCfg = g_pCfgMgr->GetHeroLevel(pCostHero->wQuality, pCostHero->wLevel);
		if (!pCostHeroLvCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ��������������[Quality:%d,Lv:%d]",
				__FUNCTION__, pCostHero->wQuality, pCostHero->wLevel);
			return;
		}

		// ����Ӣ������
		const Hero_Config *pCostHeroCfg = g_pCfgMgr->GetHero(pCostHero->dwIntoId);
		if (!pCostHeroCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]",__FUNCTION__, pCostHero->dwIntoId);
			return;
		}

		DWORD dwAddExp = 0;
		dwAddExp += pCostHeroCfg->dwToExpAsCostItem;	// ��Ϊ��������Ʒ��õľ���
		dwAddExp += pCostHero->dwExp;					// ��ǰʣ��ľ���
		dwAddExp += pCostHeroLvCfg->dwTotalExp;			// ��ǰ�ȼ����ܾ���

		// �������ĵĽ��(���ĵĽ�� = ��õľ���)
		nCostGold += dwAddExp;

		// ʹ�ô�Ӣ��������Ҫ�Ľ��
		if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold)
		{
			// ��Ҳ���
			MAPLOG(ERROR_LEVEL, "[%s]����Ӣ����Ҫ�Ľ�Ҳ���[HeroInfoId:%d]",__FUNCTION__, pCostHero->dwIntoId);
			break;
		}

		pLevelUpHero->dwExp += dwAddExp;

		const HeroLevel_Config *pHeroLvCfg;
		while (true)
		{
			// ��ǰ����Ӣ�۵ȼ�����
			pHeroLvCfg = g_pCfgMgr->GetHeroLevel(pLevelUpHero->wQuality, pLevelUpHero->wLevel);
			if (!pHeroLvCfg)
			{
				// �Ҳ���Ӣ�۵ȼ������ļ�
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۵ȼ������ļ�[QualityLevel:%d,Level:%d]",
					__FUNCTION__, pLevelUpHero->wQuality, pLevelUpHero->wLevel);
				return;
			}

			// ������鲻�㹻����,���˳�
			if (!pHeroLvCfg->dwExp || pLevelUpHero->dwExp < pHeroLvCfg->dwExp)
				break;

			// ����ȼ�����
			if (pLevelUpHero->wLevel >= m_pPlayer->GetLevel())
			{
				// �������Ϊ�����������
				//pLevelUpHero->dwExp = pHeroLvCfg->dwExp - 1;
				bLimitLevel = true;
				break;
			}
			else
			{
				pLevelUpHero->dwExp -= pHeroLvCfg->dwExp;
				++pLevelUpHero->wLevel;
				//++wAddLevel;
			}
		}

		// �Ƴ�Ӣ�ۼ��۳����
		//RemoveHero(pCostHero->dwUID, false);

		arCostHeroInfoId[byRealCostHeroNum++] = pCostHero->dwIntoId;
		
		// �ȼ�����������
		if (bLimitLevel)
			break;
	}

	RemoveHero(pCostHeroList, byRealCostHeroNum, FROM_HERO_LEVEL_UP);

	// ���Ľ��
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold, TRUE, FROM_HERO_LEVEL_UP);

	// ��������ȼ��Ƿ���Ҫ����
	UpdateTrainLevel();

	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_LEVEL_UP_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(9, GetFormationHeroMinLevel(), false);
}

// ����
void CHeroManager::OnLevelUpEx(DWORD dwHeroUID, DWORD *pCostHeroList, WORD wCostHeroListNum)
{
	WORD wFromMsgId = C2S_HERO_LEVEL_UP_REQ;


	// ����Ӣ������
	HeroInfo *pLevelUpHero = FindHero(dwHeroUID);
	if (!pLevelUpHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�����Ӣ������[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// ����Ӣ������
	const Hero_Config *pLevelUpHeroCfg = g_pCfgMgr->GetHero(pLevelUpHero->dwIntoId);
	if (!pLevelUpHeroCfg)
	{
		// Ҫ������Ӣ�����ò�����
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]", __FUNCTION__, pLevelUpHero->dwIntoId);
		return;
	}

	// ����Ƿ�������
	if (pLevelUpHero->wLevel >= pLevelUpHeroCfg->wLimitLevel)
	{
		// �ѵ���ȼ�����
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ѵ���ȼ�����[HeroInfoId:%d]", __FUNCTION__, pLevelUpHero->dwIntoId);
		return;
	}

	// ��������б��Ƿ����ظ�ID
	for (WORD i = 0; i < wCostHeroListNum; ++i)
	{
		DWORD dwCheckHeroId = pCostHeroList[i];
		for (WORD k = i + 1; k < wCostHeroListNum; ++k)
		{
			if (dwCheckHeroId == pCostHeroList[k])
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]���������б����ID�ظ����[UID:%d]", __FUNCTION__, dwHeroUID);
				return;
			}
		}
	}

	// �����õ��ܾ���
	DWORD dwAddExp = 0;
	for (WORD i = 0; i < wCostHeroListNum; ++i)
	{
		// ����Ӣ������
		const HeroInfo *pCostHero = GetHero(pCostHeroList[i]);
		if (!pCostHero)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]���û�д�Ӣ��[HeroUId:%d]", __FUNCTION__, pCostHeroList[i]);
			return;
		}

		const HeroLevel_Config *pCostHeroLvCfg = g_pCfgMgr->GetHeroLevel(pCostHero->wQuality, pCostHero->wLevel);
		if (!pCostHeroLvCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ��������������[Quality:%d,Lv:%d]",__FUNCTION__, pCostHero->wQuality, pCostHero->wLevel);
			return;
		}

		// ����Ӣ������
		const Hero_Config *pCostHeroCfg = CConfigManager::Instance()->GetHero(pCostHero->dwIntoId);
		if (!pCostHeroCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]", __FUNCTION__, pCostHero->dwIntoId);
			return;
		}


		// Ӣ�������������ɵĿ��ƾ���ĳ�1, ���к��Ĳ���
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT && pCostHeroCfg->byHeroType == 1)
		{
			dwAddExp += 1;
		}
		else
		{
			dwAddExp += pCostHeroCfg->dwToExpAsCostItem;	// ��Ϊ��������Ʒ��õľ���
			dwAddExp += pCostHero->dwExp;					// ��ǰʣ��ľ���
			dwAddExp += pCostHeroLvCfg->dwTotalExp;			// ��ǰ�ȼ����ܾ���
		}

		// todo::�����־

	}

	// �������ĵĽ��(���ĵĽ�� = ��õľ���)
	int nCostGold = dwAddExp;

	// ʹ�ô�Ӣ��������Ҫ�Ľ��
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]����Ӣ����Ҫ�Ľ�Ҳ���[HeroUID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// ��������
	pLevelUpHero->dwExp += dwAddExp;
	const HeroLevel_Config *pHeroLvCfg;
	while (true)
	{
		// ��ǰ����Ӣ�۵ȼ�����
		pHeroLvCfg = g_pCfgMgr->GetHeroLevel(pLevelUpHero->wQuality, pLevelUpHero->wLevel);
		if (!pHeroLvCfg)
		{
			// �Ҳ���Ӣ�۵ȼ������ļ�
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۵ȼ������ļ�[QualityLevel:%d,Level:%d]",
				__FUNCTION__, pLevelUpHero->wQuality, pLevelUpHero->wLevel);
			break;
		}

		// ������鲻�㹻����,�˳�
		if (!pHeroLvCfg->dwExp || pLevelUpHero->dwExp < pHeroLvCfg->dwExp)
			break;

		// ����ȼ�����,�˳�
		if (pLevelUpHero->wLevel >= m_pPlayer->GetLevel())
			break;
	
		pLevelUpHero->dwExp -= pHeroLvCfg->dwExp;
		++pLevelUpHero->wLevel;
	}

	// ɾ����Ϊ�������ϵ�Ӣ��
	RemoveHero(pCostHeroList, wCostHeroListNum, FROM_HERO_LEVEL_UP);

	// ���Ľ��
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold, TRUE, FROM_HERO_LEVEL_UP);

	// ��������ȼ��Ƿ���Ҫ����
	UpdateTrainLevel();

	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_LEVEL_UP_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(9, GetFormationHeroMinLevel(), false);

	MAPLOG(ERROR_LEVEL, "[%s]����������þ���[dwAddExp:%d]", __FUNCTION__, dwAddExp);
}

// ͻ��(����)
void CHeroManager::OnBreakEx(DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_HERO_BREAK_REQ;

	// ����Ƿ��д�Ӣ��
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]�Ҳ�����ɫӢ������[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwHeroUID);
		return;
	}

	const HeroBreakLevelUp_Config *pBreakLvUpCfg = g_pCfgMgr->GetHeroBreakLevelUp(pHero->wBreakLevel);
	if (!pBreakLvUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ��ͻ�Ƶȼ���Ӧ������[InfoId:%d,BreakLv:%d]", 
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// ���ͻ�Ƶȼ���Ҫ��Ʒ��
	if (pHero->wQuality < pBreakLvUpCfg->wNeedHeroQuality)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��Ʒ�����Ʋ���ͻ��[InfoId:%d,Quality:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wQuality);
		return;
	}

	// ���ͻ������(Ӣ�۵ȼ�)
	if (pHero->wLevel < pBreakLvUpCfg->wNeedHeroLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�Ƶȼ��ܵ���ǰӢ�۵ȼ�����[InfoId:%d,BreakLv:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// ���ͻ������(���)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < int(pBreakLvUpCfg->nCostGold))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵĽ�Ҳ���[InfoId:%d,CostGold:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->nCostGold);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr)
	// ���ͻ������(ͻ��ʯ)
	if (pItemMgr->GetItemNum(pBreakLvUpCfg->dwCostItemId) < pBreakLvUpCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�ͻ��ʯ����[InfoId:%d,ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum);
		return;
	}

	// ���ͻ������(ͬ��Ӣ��)
	DWORD arCanCostHeroList[MAX_HERO_BAG_NUM];
	WORD wCanCostHeroNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		const HeroInfo &sCostHero = m_pPlayerHeroInfo->arHeroList[i];
		// ͻ�Ƽ������ȼ�û������,����Ϊ����Ʒ, û������
		if (sCostHero.dwIntoId == pHero->dwIntoId && !sCostHero.wBreakLevel && !sCostHero.wDestinyLevel && !IsToBattle(sCostHero.dwUID)){
			arCanCostHeroList[wCanCostHeroNum++] = sCostHero.dwUID;
		}

		// ����㹻������,������
		if (wCanCostHeroNum >= pBreakLvUpCfg->wCostSameNameHeroNum)
			break;
	}
	if (wCanCostHeroNum < pBreakLvUpCfg->wCostSameNameHeroNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�Ӣ�۲���[InfoId:%d,CostHeroInfoId:%d,CostHeroNum:%d,HaveHeroNum:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->dwIntoId, pBreakLvUpCfg->wCostSameNameHeroNum, wCanCostHeroNum);
		return;
	}

	// ͻ�Ƶȼ�����
	pHero->wBreakLevel++;

	WORD wBreakLevel = pHero->wBreakLevel;
	DWORD dwHeroInfoId = pHero->dwIntoId;

	// ��������(���,ͻ��ʯ,Ӣ��)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pBreakLvUpCfg->nCostGold, TRUE, FROM_HERO_BREAK);
	pItemMgr->RemoveItem(pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum, TRUE, FROM_HERO_BREAK);
	RemoveHero(arCanCostHeroList, pBreakLvUpCfg->wCostSameNameHeroNum, FROM_HERO_BREAK);

	// ���¹㲥����
	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_BREAK_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(10, GetFormationHeroMinBreakLevel(), false);

	// 7���¼�(Ӣ��ͻ��)
	m_pPlayer->OnUpdateSevenDayTarget(9);

	// ����㲥
	if (wBreakLevel >= g_GetConfigValue(139))
	{
		CSystemNotice sys_notice(6, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(dwHeroInfoId);
		sys_notice.AddParam(wBreakLevel);
		sys_notice.SendMsg();
	}
}




// ͻ��(����)
void CHeroManager::OnBreak(DWORD dwHeroUID, DWORD *pCostSameNameHeroList, WORD wCostSameNameHeroNum,
	DWORD *pCostSameQualityHeroList, WORD wCostSameQualityHeroNum,
	DWORD *pCostSameProfessionHeroList, WORD wCostSameProfessionHeroNum)
{
	WORD wFromMsgId = C2S_HERO_BREAK_REQ;

	// ����Ƿ��д�Ӣ��
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]�Ҳ�����ɫӢ������[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwHeroUID);
		return;
	}

	const HeroBreakLevelUp_Config *pBreakLvUpCfg = g_pCfgMgr->GetHeroBreakLevelUp(pHero->wBreakLevel);
	if (!pBreakLvUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ��ͻ�Ƶȼ���Ӧ������[InfoId:%d,BreakLv:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// ���ͻ�Ƶȼ���Ҫ��Ʒ��
	if (pHero->wQuality < pBreakLvUpCfg->wNeedHeroQuality)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��Ʒ�����Ʋ���ͻ��[InfoId:%d,Quality:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wQuality);
		return;
	}

	// ���ͻ������(Ӣ�۵ȼ�)
	if (pHero->wLevel < pBreakLvUpCfg->wNeedHeroLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�Ƶȼ��ܵ���ǰӢ�۵ȼ�����[InfoId:%d,BreakLv:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// ���ͻ������(���)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < int(pBreakLvUpCfg->nCostGold))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵĽ�Ҳ���[InfoId:%d,CostGold:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->nCostGold);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr)
	// ���ͻ������(ͻ��ʯ)
	if (pItemMgr->GetItemNum(pBreakLvUpCfg->dwCostItemId) < pBreakLvUpCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�ͻ��ʯ����[InfoId:%d,ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum);
		return;
	}

	vector<DWORD> vCostHeroInfoIdList;
		
	// ������ĵ�Ӣ��(ͬ��Ӣ��)
	if (!CheckBreakCostHero(pHero, pCostSameNameHeroList, wCostSameNameHeroNum, 
		pBreakLvUpCfg->wCostSameNameHeroNum, NULL, SAME_NAME))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL,"[%s]���Ӣ��ͻ������ͬ��Ӣ��ʧ��[UId:%d,InfoId:%d]",__FUNCTION__,pHero->dwUID,pHero->dwIntoId);
		return;
	}

	// ������ĵ�Ӣ��(ͬƷ��Ӣ��)
	if (!CheckBreakCostHero(pHero, pCostSameQualityHeroList, wCostSameQualityHeroNum, 
		pBreakLvUpCfg->wCostSameQualityHeroNum, &vCostHeroInfoIdList, SAME_QUALITY))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Ӣ��ͻ������ͬƷ��Ӣ��ʧ��[UId:%d,InfoId:%d]", __FUNCTION__, pHero->dwUID, pHero->dwIntoId);
		return;
	}

	// ������ĵ�Ӣ��(ְͬҵӢ��)
	if (!CheckBreakCostHero(pHero, pCostSameProfessionHeroList, wCostSameProfessionHeroNum, 
		pBreakLvUpCfg->wCostSameProfessionHeroNum, &vCostHeroInfoIdList, SAME_QUALITY_AND_PROFESSION))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Ӣ��ͻ������ְͬҵӢ��ʧ��[UId:%d,InfoId:%d]", __FUNCTION__, pHero->dwUID, pHero->dwIntoId);
		return;
	}

	// ͻ�Ƶȼ�����
	pHero->wBreakLevel++;

	WORD wBreakLevel = pHero->wBreakLevel;
	DWORD dwHeroInfoId = pHero->dwIntoId;

	// ��������(���,ͻ��ʯ,Ӣ��)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pBreakLvUpCfg->nCostGold, TRUE, FROM_HERO_BREAK);
	pItemMgr->RemoveItem(pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum, TRUE, FROM_HERO_BREAK);
	RemoveHero(pCostSameNameHeroList, wCostSameNameHeroNum, FROM_HERO_BREAK);
	RemoveHero(pCostSameQualityHeroList, wCostSameQualityHeroNum, FROM_HERO_BREAK);
	RemoveHero(pCostSameProfessionHeroList, wCostSameProfessionHeroNum, FROM_HERO_BREAK);

	// ��¼����Ӣ��
	for (int i = 0; i < vCostHeroInfoIdList.size(); ++i){
		RecordBreakCostHero(pHero, vCostHeroInfoIdList[i]);
	}

	// ���¹㲥����
	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_BREAK_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(10, GetFormationHeroMinBreakLevel(), false);

	// 7���¼�(Ӣ��ͻ��)
	m_pPlayer->OnUpdateSevenDayTarget(9);

	// ����㲥
	if (wBreakLevel >= g_GetConfigValue(139))
	{
		CSystemNotice sys_notice(6, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(dwHeroInfoId);
		sys_notice.AddParam(wBreakLevel);
		sys_notice.SendMsg();
	}
}

// ����(Ʒ��)
void CHeroManager::OnNetEvolution(DWORD dwHeroUID)
{
	// ����Ƿ��д�Ӣ��
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫӢ������[UID:%d]",__FUNCTION__, dwHeroUID);
		return;
	}

	// ȡӢ������
	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ����������[InfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	// ����Ƿ��Ѵ��������
	if (pHero->wQuality == pHeroCfg->wQualityMax && pHero->wQualityStage >= pHeroCfg->wQualityStageMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���Ѿ������ٽ���[InfoId:%d,Quality:%d,Stage:%d]", 
			__FUNCTION__, pHero->dwIntoId, pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// ȡ��������
	const HeroEvolution_Config *pEvolutionCfg = g_pCfgMgr->GetHeroEvolution(pHero->wQuality, pHero->wQualityStage);
	if (!pEvolutionCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۽�������[Quality:%d,Stage:%d]",__FUNCTION__,pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// ��������Ҫ����(���)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < pEvolutionCfg->nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۽������ĵĽ�Ҳ���[Quality:%d,Stage:%d,CostGold:%d]",
			__FUNCTION__, pHero->wQuality, pHero->wQualityStage, pEvolutionCfg->nCostGold);
		return;
	}

	// ��������Ҫ����(����)
	if (!m_pPlayer->CheckMoneyCost(pEvolutionCfg->vCostCurrencyList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۽������ĵĻ��Ҳ���[Quality:%d,Stage:%d]",
			__FUNCTION__, pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// ��������Ҫ����(����)
	if (!m_pPlayer->CheckItemCost(pEvolutionCfg->vCostItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۽������ĵĵ��߲���[Quality:%d,Stage:%d]",
			__FUNCTION__, pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// ���������Ҫ����(���)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pEvolutionCfg->nCostGold, TRUE, FROM_HERO_EVOLUTION);

	// ���������Ҫ����(����)
	m_pPlayer->DoMoneyCost(pEvolutionCfg->vCostCurrencyList, 1, FROM_HERO_EVOLUTION);

	// ���������Ҫ����(����)
	for (int i = 0; i < pEvolutionCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pEvolutionCfg->vCostItemList[i], FROM_HERO_EVOLUTION);
	}

	// ��Ʒ
	if (pEvolutionCfg->isTheLastStage)
	{
		pHero->wQuality++;
		pHero->wQualityStage = 0;
	}
	// ����
	else
	{
		pHero->wQualityStage++;
	}

	// ͬ������
	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_HERO_EVOLUTION_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	msg.m_wQuality = pHero->wQuality;
	msg.m_wStage = pHero->wQualityStage;
	m_pPlayer->SendPacket(&msg);
}

// ����
void CHeroManager::OnNetTrain(DWORD dwHeroUID, BYTE byCostType, BYTE byTrainTimes)
{
	WORD wFromMsgId = C2S_HERO_TRAIN_REQ;

	// ������
	if (!byTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ����������Ϊ0", __FUNCTION__);
		return;
	}

	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ҫ������Ӣ��[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// ȡ��������
	const HeroTrainCost_Config *pTrainCostCfg = g_pCfgMgr->GetHeroTrainCost(byCostType);
	if (!pTrainCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ��������������[CostType:%d]", __FUNCTION__, byCostType);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// ���������Ʒ
	if (pItemMgr->GetItemNum(pTrainCostCfg->tCostItem.dwId) < pTrainCostCfg->tCostItem.dwNum * byTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ������������Ʒ����[CostType:%d]", __FUNCTION__, byCostType);
		return;
	}

	// ������Ļ���
	if (pTrainCostCfg->tCostMoney.nValue && 
		m_pPlayer->GetProperty(pTrainCostCfg->tCostMoney.byType) < pTrainCostCfg->tCostMoney.nValue * byTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���������Ļ��Ҳ���[CostType:%d]", __FUNCTION__, byCostType);
		return;
	}

	// ȡ��������б�
	const vector<HeroTrainRandPro_Config> *pRandProlListConfig = g_pCfgMgr->GetHeroTrainRandProList();
	if (!pRandProlListConfig)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�����������������Ϊ��", __FUNCTION__);
		return;
	}

	int arRandProList[HERO_TRAIN_PRO_NUM] = {0};

	// �������
	for (BYTE i = 0; i < byTrainTimes; ++i)
	{
		for (size_t k = 0; k < pRandProlListConfig->size(); ++k)
		{
			const HeroTrainRandPro_Config *pProCfg = &pRandProlListConfig->at(k);
			BYTE byProIdx = GetTrainProIdx(pProCfg->byProType);
			if (byProIdx >= HERO_TRAIN_PRO_NUM)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]Ӣ��������������Խ��[ProType:%d]", __FUNCTION__, pProCfg->byProType);
				return;
			}

			// todo::�����ǰ5������,���������(������������)
			if (m_pPlayerHeroInfo->wTrainTimes <= 5)
			{
				arRandProList[byProIdx] += g_Rand(1, pProCfg->nMax);
			}
			else
			{
				arRandProList[byProIdx] += g_Rand(pProCfg->nMin, pProCfg->nMax);
			}
		}

		if (m_pPlayerHeroInfo->wTrainTimes <= 5)
			m_pPlayerHeroInfo->wTrainTimes++;
	}

	// �������ֵ
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		int &nRandProValue = arRandProList[i];
		BYTE byProType = GetTrainProType(i);
		BYTE byProLevel = pHero->arTrainProLvList[i];

		const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, byProLevel);
		if (!pTrainCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ����������[ProType:%d,ProLevel:%d]",__FUNCTION__, byProType, byProLevel);
			return;
		}

		// ���޾���(���޲���Ϊ����)
		if (nRandProValue < 0)
		{
			// ����ۻ�����ֵ�Ѵﵱǰ������,�򲻻�䶯
			if (pHero->arTrainProValueList[i] >= pTrainCfg->nProMax)
			{
				nRandProValue = 0;
			}
			else
			{
				int nCurProValue = 0;
				// ��������
				if (byProLevel)
				{
					// ��֤���ܵ���ǰ�ȼ�	
					const HeroTrain_Config *pTrainCfgEx = g_pCfgMgr->GetHeroTrain(byProType, byProLevel - 1);
					if (pTrainCfgEx)
						nCurProValue = pHero->arTrainProValueList[i] - pTrainCfgEx->nProMax;
				}
				// ��û������
				else
				{
					nCurProValue = pHero->arTrainProValueList[i];
				}

				if (abs(nRandProValue) > nCurProValue)
				{
					nRandProValue = -nCurProValue;
				}
			}
		}
		
		// ���޾���(���ܳ�����ǰ������������)
		if (nRandProValue > 0)
		{
			// ȡ��ǰ�׶ε���������
			int nProMax = pTrainCfg->nProMax; // min(pTrainCfg->nProMax, GetTrainProMax(byProType, pHero->wLevel));
			if (nProMax)
			{
				if (nRandProValue + pHero->arTrainProValueList[i] > nProMax)
				{
					nRandProValue = nProMax - pHero->arTrainProValueList[i];
				}
			}
			else
			{
				nRandProValue = 0;
			}
		}
	}

	// ��������ֵ
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		pHero->arLastTrainTmpProList[i] = arRandProList[i];
	}

	// ��������
	pItemMgr->RemoveItem(pTrainCostCfg->tCostItem.dwId, pTrainCostCfg->tCostItem.dwNum * byTrainTimes, TRUE, FROM_HERO_TRAIN);
	if (pTrainCostCfg->tCostMoney.nValue)
		m_pPlayer->ModifyProperty(pTrainCostCfg->tCostMoney.byType, -pTrainCostCfg->tCostMoney.nValue * byTrainTimes, TRUE, FROM_HERO_TRAIN);

	// ��¼���ĵĲ���
	pHero->dwTrainCostItemNum += pTrainCostCfg->tCostItem.dwNum * byTrainTimes;
	if (ROLE_PRO_GOLD == pTrainCostCfg->tCostMoney.byType)
	{
		pHero->nTrainCostGold += pTrainCostCfg->tCostMoney.nValue * byTrainTimes;
	}
	else if (ROLE_PRO_DIAMOND == pTrainCostCfg->tCostMoney.byType)
	{
		pHero->nTrainCostDiamond += pTrainCostCfg->tCostMoney.nValue * byTrainTimes;
	}
	else if (pTrainCostCfg->tCostMoney.byType)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д�����������Ļ�������[ProType:%d]", __FUNCTION__, pTrainCostCfg->tCostMoney.byType);
	}

	// ��Ӧ
	MSG_S2C_HERO_TRAIN_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	msg.m_byProNum = HERO_TRAIN_PRO_NUM;       
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		msg.m_ProList[i].nValue = arRandProList[i];
		msg.m_ProList[i].wType = GetTrainProType(i);
	}
	m_pPlayer->SendPacket(&msg);

	// �ճ������¼�
	m_pPlayer->OnUpdateDailyMission(3, byTrainTimes);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 33, byTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 33, byTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 33, byTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 33, byTrainTimes, true);

	// ����ȼ�����
	//m_pPlayer->OnUpdateWorldTaskEvent(3, byTrainTimes);

	// �ɾ�
	//m_pPlayer->OnUpdateAchievement(11, GetFormationHeroMinTrainLevel(), false);

	// todo::���Դ���
// 	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
// 	{
// 		MAPLOG(ERROR_LEVEL, "������������[ProType:%d,ProValue:%d]", GetTrainProType(i), arRandProList[i]);
// 	}
}

// �����滻
void CHeroManager::OnNetTrainReplace(DWORD dwHeroUID)
{
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_HERO_TRAIN_REPLACE_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ҫ������Ӣ��[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// ������������
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		pHero->arTrainProValueList[i] += pHero->arLastTrainTmpProList[i];
	}

	//bool bHaveLevelUp = false;

	// ��������
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		int nProValue = pHero->arTrainProValueList[i];
		BYTE byProType = GetTrainProType(i);
		const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, pHero->arTrainProLvList[i]);
		// ��������,�����滻�����б䶯(todo::Ͷ��ȡ���߼�)
		if (pTrainCfg && nProValue >= pTrainCfg->nProMax && pHero->arLastTrainTmpProList[i])
		{
			pHero->arTrainRewardFlag[i] = true;

			// ��������ȼ�����(���յȼ�����,��ǰӢ�۵ȼ�����)
// 			if (pHero->wLevel >= pTrainCfg->wNeedHeroLevel && pHero->arTrainProLvList[i] < GetTrainLevelMax(byProType))
// 			{
// 				pHero->arTrainProLvList[i]++;
// 
// 				bHaveLevelUp = true;
// 			}
		}
	}

	// ��ջ�����
	memset(pHero->arLastTrainTmpProList, 0, sizeof(pHero->arLastTrainTmpProList));

	HeroBaseDataNotify(dwHeroUID);

	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_HERO_TRAIN_REPLACE_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

// 	if (bHaveLevelUp)
// 	{
// 		// 7���¼�(Ӣ������)
// 		m_pPlayer->OnUpdateSevenDayTarget(15);
// 	}
}

// ��ȡ��������
void CHeroManager::OnNetGetTrainReward(DWORD dwHeroUID, BYTE byProType)
{
	WORD wFromMsgId = C2S_HERO_TRAIN_REWARD_GET_REQ;

	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ҫ������Ӣ��[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	BYTE byProIdx = GetTrainProIdx(byProType);

	// ����������Ͳ���
	if (byProIdx >= HERO_TRAIN_PRO_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�������Ͳ���Խ��[ProType:%d]", __FUNCTION__, byProType);
		return;
	}

	// ����Ƿ�����ȡ��
	if (!pHero->arTrainRewardFlag[byProIdx])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]������������ȡ��[ProType:%d]", __FUNCTION__, byProType);
		return;
	}

	// ��¼����ȡ��
	pHero->arTrainRewardFlag[byProIdx] = false;

	// ��������
	const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, pHero->arTrainProLvList[byProIdx]);
	// ��������ȼ�����(���յȼ�����,��ǰӢ�۵ȼ�����)
	if (pTrainCfg && pHero->wLevel >= pTrainCfg->wNeedHeroLevel && pHero->arTrainProLvList[byProIdx] < GetTrainLevelMax(byProType))
	{
		pHero->arTrainProLvList[byProIdx]++;

		// 7���¼�(Ӣ������)
		m_pPlayer->OnUpdateSevenDayTarget(15);
	}

	HeroBaseDataNotify(dwHeroUID);
	
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_TRAIN_REWARD_GET_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	msg.m_byProType = byProType;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(11, GetFormationHeroMinTrainLevel(), false);
}

// �������ֵ
void CHeroManager::OnNetDestiny(DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_HERO_DESTINY_REQ;

	// ���Ӣ���Ƿ����
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫӢ������[UID:%d]",__FUNCTION__, dwHeroUID);
		return;
	}

	// ������������
	const HeroDestiny_Config *pDestinyCfg = CConfigManager::Instance()->GetHeroDestiny(pHero->wDestinyLevel);
	if (!pDestinyCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
		return;
	}

	// ���ȼ�����
	if (!pDestinyCfg->dwNeedExp)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�����ȼ��Ѵ�����[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	// �������
	if (pDestinyCfg->dwCostItemId && pItemMgr->GetItemNum(pDestinyCfg->dwCostItemId) < pDestinyCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ����[ItemId:%d]", __FUNCTION__, pDestinyCfg->dwCostItemId);
		return;
	}

	// ��Ӿ���
	pHero->dwDestinyExp += pDestinyCfg->dwAddExp;
	
	// ��������
	if (pHero->dwDestinyExp >= pDestinyCfg->dwNeedExp)
	{
		pHero->wDestinyLevel++;
		pHero->dwDestinyExp = 0;
	}
	else
	{
		// ��������
		const RateData *pRateData = pDestinyCfg->GetRateData(pHero->dwDestinyExp);
		if (!pRateData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������Ӧ��������������[Exp:%d]",
				__FUNCTION__, pHero->dwDestinyExp);
			return;
		}

		// ����
		if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < pRateData->nRate)
		{
			pHero->wDestinyLevel++;
			pHero->dwDestinyExp = 0;
		}
	}

	// ��������
	if (pDestinyCfg->dwCostItemId)
	{
		pItemMgr->RemoveItem(pDestinyCfg->dwCostItemId, pDestinyCfg->wCostItemNum, TRUE, FROM_HERO_DESTINY);
	}

	// ��¼�ܵ���������
	pHero->dwDestinyCostItemNum += pDestinyCfg->wCostItemNum;

	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_DESTINY_RESP msg;
	msg.m_dwHeroUID = dwHeroUID;
	msg.m_byDestinyLevel = pHero->wDestinyLevel;
	msg.m_dwDestinyExp = pHero->dwDestinyExp;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(12, GetFormationHeroMinDestinyLevel(), false);

	// 7���¼�(Ǳ�������ȼ�)
	m_pPlayer->OnUpdateSevenDayTarget(11);
}

// ����(Ǳ��)
void CHeroManager::OnNetDestinyEx(DWORD dwHeroUID, WORD wDoTimes)
{
	WORD wFromMsgId = C2S_HERO_DESTINY_REQ;

	// ���Ӣ���Ƿ����
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫӢ������[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	for (WORD i = 0; i < wDoTimes; ++i)
	{
		// ������������
		const HeroDestiny_Config *pDestinyCfg = CConfigManager::Instance()->GetHeroDestiny(pHero->wDestinyLevel);
		if (!pDestinyCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
			return;
		}

		// ���ȼ�����
		if (!pDestinyCfg->dwNeedExp)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�����ȼ��Ѵ�����[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
			return;
		}

		CItemManager *pItemMgr = m_pPlayer->GetItemManager();
		CHECK_POINTER(pItemMgr);
		// �������
		if (pDestinyCfg->dwCostItemId && pItemMgr->GetItemNum(pDestinyCfg->dwCostItemId) < pDestinyCfg->wCostItemNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ����[ItemId:%d]", __FUNCTION__, pDestinyCfg->dwCostItemId);
			return;
		}

		// ��Ӿ���
		pHero->dwDestinyExp += pDestinyCfg->dwAddExp;

		// ��������
		if (pHero->dwDestinyExp >= pDestinyCfg->dwNeedExp)
		{
			pHero->wDestinyLevel++;
			pHero->dwDestinyExp = 0;
		}
		else
		{
			// ��������
			const RateData *pRateData = pDestinyCfg->GetRateData(pHero->dwDestinyExp);
			if (!pRateData)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������Ӧ��������������[Exp:%d]",
					__FUNCTION__, pHero->dwDestinyExp);
				return;
			}

			// ����
			if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < pRateData->nRate)
			{
				pHero->wDestinyLevel++;
				pHero->dwDestinyExp = 0;
			}
		}

		// ��������
		if (pDestinyCfg->dwCostItemId)
		{
			pItemMgr->RemoveItem(pDestinyCfg->dwCostItemId, pDestinyCfg->wCostItemNum, TRUE, FROM_HERO_DESTINY);
		}

		// ��¼�ܵ���������
		pHero->dwDestinyCostItemNum += pDestinyCfg->wCostItemNum;
	}

	HeroBaseDataNotify(dwHeroUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_DESTINY_RESP msg;
	msg.m_dwHeroUID = dwHeroUID;
	msg.m_byDestinyLevel = pHero->wDestinyLevel;
	msg.m_dwDestinyExp = pHero->dwDestinyExp;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(12, GetFormationHeroMinDestinyLevel(), false);

	// 7���¼�(Ǳ�������ȼ�)
	m_pPlayer->OnUpdateSevenDayTarget(11);
}

// װ�����ѵ���
void CHeroManager::OnNetEquipAwakeningItem(DWORD dwHeroUID, WORD wItemId)
{
	WORD wFromMsgId = C2S_HERO_EQUIP_AWAKEN_ITEM_REQ;

	// ���Ӣ���Ƿ����
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫӢ������[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[InfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	const HeroAwaken_Config *pAwakenCfg = g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality,
		pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
	if (!pAwakenCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۾�������[Qaulity:%d,Profession:%d,Star:%d,Level:%d]",
			__FUNCTION__, pHeroCfg->wInitQuality, pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
		return;
	}

	// ����Ƿ���ѵ���
	bool bAwakenItem = false;
	for (size_t i = 0; i < pAwakenCfg->vCostItemList.size(); ++i)
	{
		if (pAwakenCfg->vCostItemList[i] == wItemId)
		{
			bAwakenItem = true;
			break;
		}
	}
	if (!bAwakenItem)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]װ���ĵ��߲��Ǹ�Ӣ�۾�����Ҫ�ĵ���[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	// ����Ƿ��ظ�װ��
	for (BYTE i = 0; i < pHero->wAwakenItemNum; ++i)
	{
		if (pHero->arAwakenItemList[i] == wItemId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�ظ�װ��ͬ������[ItemId:%d]", __FUNCTION__, wItemId);
			return;
		}
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// �������Ƿ����
	if (!pItemMgr->GetItemNum(wItemId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]װ���ľ��ѵ��߲�����[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	pItemMgr->RemoveItem(wItemId, 1, TRUE, FROM_EQUIP_AWAKENING_ITEM);

	if (pHero->wAwakenItemNum < MAX_AWAKEN_EQUIP_ITEM_NUM)
		pHero->arAwakenItemList[pHero->wAwakenItemNum++] = wItemId;

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_EQUIP_AWAKEN_ITEM_RESP msg;
	msg.m_dwUId = dwHeroUID;
	msg.m_wAwakenItemId = wItemId;
	m_pPlayer->SendPacket(&msg);
}

// ����
void CHeroManager::OnNetAwaken(DWORD dwHeroUID, const DWORD *pCostHeroUIDList, WORD wCostHeroNum)
{
	//MAPLOG(GUI_LEVEL, "[%s]�������[UID:%d,CostHeroUID:%d]", __FUNCTION__, dwHeroUID, dwCostHeroUID);

	WORD wFromMsgId = C2S_HERO_AWAKEN_REQ;

	// ���Ӣ���Ƿ����
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫӢ������[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[InfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	const HeroAwaken_Config *pAwakenCfg = g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality,
		pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
	if (!pAwakenCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۾�������[Qaulity:%d,Profession:%d,Star:%d,Level:%d]", 
			__FUNCTION__, pHeroCfg->wInitQuality, pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
		return;
	}

	// ���Ӣ�۵ȼ�����
	if (pAwakenCfg->wNeedHeroLevel > pHero->wLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾�����Ӣ�۵ȼ�����!", __FUNCTION__);
		return;
	}

	// ������Ļ���
	if (m_pPlayer->GetProperty(pAwakenCfg->nCostMoneyType) < pAwakenCfg->nCostMoneyValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾�����������Ҳ���[MoneyType:%d,MoneyValue:%d]",
			__FUNCTION__, pAwakenCfg->nCostMoneyType, pAwakenCfg->nCostMoneyValue);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// ������Ĳ�����Ʒ
	if (pItemMgr->GetItemNum(pAwakenCfg->tCostItem.dwId) < pAwakenCfg->tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾������Ĳ�����Ʒ����[ItemId:%d]",__FUNCTION__, pAwakenCfg->tCostItem.dwId);
		return;
	}

	// ����Ƿ���װ���Ͼ���������Ʒ
	for (size_t i = 0; i < pAwakenCfg->vCostItemList.size(); ++i)
	{
		DWORD dwCostItemId = pAwakenCfg->vCostItemList[i];
		bool bHave = false;
		for (BYTE k = 0; k < pHero->wAwakenItemNum; ++k)
		{
			if (pHero->arAwakenItemList[k] == dwCostItemId)
			{
				bHave = true;
				break;
			}
		}
		if (!bHave)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾�������װ����Ʒ����[ItemId:%d]", __FUNCTION__, dwCostItemId);
			return;
		}
	}

	// �������ͬ��Ӣ��
	if (pAwakenCfg->wCostHeroNum != wCostHeroNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾������ĵ�ͬ��Ӣ����������[NeedNum:%d,HaveNum]", 
			__FUNCTION__, pAwakenCfg->wCostHeroNum, wCostHeroNum);
		return;
	}
	map<DWORD, DWORD> findList;
	for (WORD i = 0; i < wCostHeroNum; ++i)
	{
		DWORD dwCostHeroUID = pCostHeroUIDList[i];
		HeroInfo *pCostHero = FindHero(dwCostHeroUID);
		if (!pCostHero)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾������ĵ�ͬ��Ӣ�۲�����[UID:%d]", __FUNCTION__, dwCostHeroUID);
			return;
		}

		// �Ƿ�ͬ��Ӣ��
		if (pCostHero->dwIntoId != pHero->dwIntoId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҷ�����,��ɱ�ƭ��,��������Ǿ���ͬ��Ӣ��![UID:%d]", __FUNCTION__, dwCostHeroUID);
			return;
		}

		// ����Ƿ������ظ�Ӣ��
		if (findList.find(dwCostHeroUID) != findList.end())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۾������ĳ����ظ�Ӣ��[UID:%d]", __FUNCTION__, dwCostHeroUID);
			return;
		}
		findList.insert(make_pair(dwCostHeroUID, dwCostHeroUID));
	}

	// ɾ������
	m_pPlayer->ModifyProperty(pAwakenCfg->nCostMoneyType, -pAwakenCfg->nCostMoneyValue, TRUE, FROM_HERO_AWAKEN);
	pItemMgr->RemoveItem(pAwakenCfg->tCostItem.dwId, pAwakenCfg->tCostItem.dwNum, TRUE, FROM_HERO_AWAKEN);

	// ����
	if (pHero->wAwakenLevel >= HERO_AWAKEN_LEVEL)
	{
		pHero->wAwakenStar++;
		pHero->wAwakenLevel = 0;
	}
	// ����
	else
	{
		pHero->wAwakenLevel++;
	}
	pHero->wAwakenItemNum = 0;

	HeroBaseDataNotify(dwHeroUID);

	// �ȼ�¼,��ֹ�����ɾ��������������λ�ø��Ķ����ݴ���
	WORD wAwakenStar = pHero->wAwakenStar;
	WORD wAwakenLevel = pHero->wAwakenLevel;

	// ɾ�����ĵ�Ӣ��
	for (WORD i = 0; i < wCostHeroNum; ++i){
		RemoveHero(pCostHeroUIDList[i], TRUE, FROM_HERO_AWAKEN);
	}

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_AWAKEN_RESP msg;
	msg.m_dwUId = dwHeroUID;
	msg.m_byAwakenStar = wAwakenStar;
	msg.m_byAwakenLevel = wAwakenLevel;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(23, GetFormationAllHeroAwakenStar(), false);
}

// ����Ӣ�ۻ��������б�
void CHeroManager::OnNetHeroBaseList()
{
	SendHeroBaseList();
}

// ����Ӣ����ϸ��Ϣ 
void CHeroManager::OnNetHeroInfo(DWORD dwUID)
{
	HeroInfo *pHeroInfo = FindHero(dwUID);
	if (!pHeroInfo)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_HERO_INFO_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]�����Ӣ�۲�����[UID:%d]",__FUNCTION__, dwUID);
		return;
	}

	MSG_S2C_HERO_INFO_RESP msg;
	memcpy_s(&msg.m_Info, sizeof(HeroInfo), pHeroInfo, sizeof(HeroInfo));
	m_pPlayer->SendPacket(&msg);
}

// �����ռ�Ӣ���б�
void CHeroManager::OnNetCollectionList()
{
	MSG_S2C_HERO_COLLECTION_LIST_RESP msg;
	msg.m_byCollectionNum = m_pPlayerHeroInfo->wCollectionNum;
	for (BYTE i = 0; i < msg.m_byCollectionNum; ++i)
	{
		msg.m_CollectionList[i] = m_pPlayerHeroInfo->arCollectionList[i];
	}

	m_pPlayer->SendPacket(&msg);
}

// �����ٻ�����
void CHeroManager::OnNetSummonData()
{
	SendSummonData();
}

// 
DWORD CHeroManager::AddHero(DWORD dwHeroInfoId, bool bNotify, WORD wFromId)
{
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(dwHeroInfoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ���Ӣ�۵���������[HeroInfoId:%d]", dwHeroInfoId);
		return 0;
	}

	HeroInfo sHero;
	sHero.Init();
	sHero.dwUID = GetNewSeriesId();
	sHero.dwIntoId = dwHeroInfoId;
	sHero.wLevel = pHeroCfg->wInitLevel;
	sHero.wQuality = pHeroCfg->wInitQuality;
	//sHero.wTrainDataIndex = 1;

	// ��¼���
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwHeroInfoId, 1, true, wFromId);

	return AddHero(sHero, bNotify) ? sHero.dwUID : 0;
}

// ���Ӣ��
bool CHeroManager::AddHero(const HeroInfo &rInfo, bool bNotify)
{
	if (GetHero(rInfo.dwUID))
	{
		MAPLOG(ERROR_LEVEL, "����ظ�Ӣ��[HeroUId:%d]", rInfo.dwUID);
		return false;
	}

	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۱����Ѵ�����[RoleId:%d,HeroListNum:%d,HeroInfoId:%d]", __FUNCTION__, 
			m_pPlayer->GetRoleId(), m_pPlayerHeroInfo->wHeroNum, rInfo.dwIntoId);

		// ��¼Ӣ�۶�ʧ��¼
		m_pPlayer->SaveItemLoseRecord(rInfo.dwIntoId);

		return false;
	}

	m_pPlayerHeroInfo->arHeroList[m_pPlayerHeroInfo->wHeroNum] = rInfo;
	++m_pPlayerHeroInfo->wHeroNum;

	// �ռ�Ӣ��
	AddCollectionHero(rInfo.dwIntoId);

	if (bNotify)
		HeroBaseDataNotify(rInfo.dwUID);

	return true;
}

// �Ƴ�Ӣ��
bool CHeroManager::RemoveHero(DWORD dwHeroUID, bool bNotify, WORD wFromId)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		HeroInfo &tFind = m_pPlayerHeroInfo->arHeroList[i];
		if (tFind.dwUID == dwHeroUID)
		{
			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tFind.dwIntoId, 1, false, wFromId);

			HeroBaseDataNotify(dwHeroUID, false, bNotify);
			tFind = m_pPlayerHeroInfo->arHeroList[--m_pPlayerHeroInfo->wHeroNum];
			return true;
		}
	}

	return false;
}

// �Ƴ�Ӣ��
bool CHeroManager::RemoveHero(DWORD *pHeroUIDList, WORD wHeroNum, WORD wFromId)
{
	for (WORD i = 0; i < wHeroNum; ++i)
	{
		for (WORD k = 0; k < m_pPlayerHeroInfo->wHeroNum; ++k)
		{
			DWORD dwHeroUID = pHeroUIDList[i];
			HeroInfo &tDeleteHero = m_pPlayerHeroInfo->arHeroList[k];
			if (tDeleteHero.dwUID == dwHeroUID)
			{
				// ��¼���
				g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tDeleteHero.dwIntoId, 1, false, wFromId);

				HeroBaseDataNotify(dwHeroUID, false, false);
				tDeleteHero = m_pPlayerHeroInfo->arHeroList[--m_pPlayerHeroInfo->wHeroNum];
				break;
			}
		}
	}

	MSG_S2C_HERO_BASE_INFO_NOTIFY msg;
	msg.m_byFlag = false;
	msg.m_wHeroNum = 0;
	for (WORD i = 0; i < wHeroNum; ++i)
	{
		ClientHeroBaseData &tBaseData = msg.m_HeroBaseDataList[msg.m_wHeroNum++];
		memset(&tBaseData, 0, sizeof(tBaseData));
		tBaseData.dwUID = pHeroUIDList[i];
	}
	m_pPlayer->SendPacket(&msg);

	return false;
}


// ȡӢ��
const HeroInfo* CHeroManager::GetHero(DWORD dwHeroUID)
{
	return FindHero(dwHeroUID);
}


// ����Ӣ��
HeroInfo* CHeroManager::FindHero(DWORD dwHeroUID)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].dwUID == dwHeroUID)
		{
			return &m_pPlayerHeroInfo->arHeroList[i];
		}
	}

	return NULL;
}

// ������ͨ�ٻ�
bool CHeroManager::DoNormalSummon(vector<DWORD> &OutHeroList, DWORD dwCostItemId, int nFriendshipPoint)
{
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER_RET(pItemMgr, false);

	// ������Ʒ
	if (dwCostItemId)
	{
		if (!pItemMgr->GetItemNum(dwCostItemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ٻ�������Ʒ��������[CostItemId:%d]", __FUNCTION__, dwCostItemId);
			return false;
		}
	}
	// ��Ϣ�����
	else if (nFriendshipPoint > 0)
	{
		if (m_pPlayer->GetProperty(ROLE_PRO_FRIENDSHIP_PIONT) < nFriendshipPoint)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ٻ���������㲻��[cost_point:%d]", __FUNCTION__, nFriendshipPoint);
			return false;
		}
	}
	// ������Ѵ���
	else
	{
		// �����Ѵ���
		if (m_pPlayerHeroInfo->wNormalFreeSummonTimes >= g_GetConfigValue(CONFIG_NORMAL_FREE_SUMMON_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ٻ���Ѵ�������[used_times:%d]", 
				__FUNCTION__, m_pPlayerHeroInfo->wNormalFreeSummonTimes);
			return false;
		}

		// ���CD(����10������)
		if (time(NULL) + 10000 < m_pPlayerHeroInfo->dwNormalFreeSummonTime + g_GetConfigValue(CONFIG_NORMAL_SUMMON_CD) * SECOND_OF_MIN)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ٻ����CD��...",__FUNCTION__);
			return false;
		}
	}

	// ���Ӣ�۰����Ƿ�����
	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۱�������,�ٻ�ʧ��!", __FUNCTION__);
		return false;
	}

	// ���Ӣ��
	DWORD dwRandHeroId = RandNormalSummonHero();
	if (!dwRandHeroId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���ٻ����Ӣ��ʧ��!", __FUNCTION__);
		return false;
	}

	// ���Ӣ��
	AddHero(dwRandHeroId, true, FROM_HERO_SUMMON);

	// ��������
	if (dwCostItemId)
	{
		pItemMgr->RemoveItem(dwCostItemId, 1, TRUE, FROM_HERO_SUMMON);
	}
	else if (nFriendshipPoint > 0 )
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_FRIENDSHIP_PIONT, -nFriendshipPoint);
	}
	else
	{
		// ��������,����CD
		++m_pPlayerHeroInfo->wNormalFreeSummonTimes;
		m_pPlayerHeroInfo->dwNormalFreeSummonTime = (DWORD)time(NULL);
	}

	OutHeroList.push_back(dwRandHeroId);

	return true;
}

// ����߼��ٻ�
bool CHeroManager::DoAdvanceSummon(vector<DWORD> &vOutHeroList, DWORD dwCostItemId, int nCostDiamond, int nSummonTimes)
{
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER_RET(pItemMgr, false);

	// ������Ʒ
	if (dwCostItemId)
	{
		if (pItemMgr->GetItemNum(dwCostItemId) < nSummonTimes)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�߼�Ӣ���ٻ�������Ʒ��������[CostItemId:%d,CostItemNum:%d]",__FUNCTION__, dwCostItemId, nSummonTimes);
			return false;
		}
	}
	// ������ʯ
	else if (nCostDiamond > 0)
	{
		if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�߼�Ӣ���ٻ�������ʯ����[CostDiamond:%d]", __FUNCTION__, nCostDiamond);
			return false;
		}
	}
	// ������Ѵ���
	else
	{
		// ���CD(����10������)
		if (time(NULL) + 10000 < m_pPlayerHeroInfo->dwHighFreeSummonTime + g_GetConfigValue(CONFIG_ADVANCED_SUMMON_CD) * SECOND_OF_MIN)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�߼�Ӣ���ٻ����CD��...", __FUNCTION__);
			return false;
		}
	}

	// �������ռ�
	if (GetBagEmptySize() < nSummonTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۱�������,�ٻ�ʧ��!", __FUNCTION__);
		return false;
	}

	// ���һ��Ӣ��
	for (int i = 0; i < nSummonTimes; ++i)
	{
		DWORD dwRandHeroId = RandAdvancedSummonHero();
		if (!dwRandHeroId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�߼�Ӣ���ٻ����Ӣ��ʧ��!", __FUNCTION__);
			return false;
		}

		// ��¼�ٻ�����
		m_pPlayerHeroInfo->wHighSummonTimes++;

		// �ٻ���һ���Ĵ���ʱ���ô���
		if (m_pPlayerHeroInfo->wHighSummonTimes >= g_GetConfigValue(CONFIG_ADVANCED_SUMMON_SPECIAL_TIMES))
		{
			m_pPlayerHeroInfo->wHighSummonTimes = 0;
		}

		vOutHeroList.push_back(dwRandHeroId);

		// ��ʱ��ļ�
		//m_pPlayer->GetActivityManager()->GetLimitActivity().AddRecruitTimes();
	}

	// ������Ʒ
	if (dwCostItemId)
	{
		pItemMgr->RemoveItem(dwCostItemId, nSummonTimes, TRUE, FROM_HERO_SUMMON);
	}
	// ������ʯ
	else if (nCostDiamond > 0)
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_HERO_HIGH_SUMMON);
	}
	// ������Ѵ���
	else
	{
		// ����CD
		m_pPlayerHeroInfo->dwHighFreeSummonTime = time(NULL);
	}

	// ���Ӣ��
	for (size_t i = 0; i < vOutHeroList.size(); ++i)
	{
		AddHero(vOutHeroList[i], TRUE, FROM_HERO_SUMMON);
	}

	//vOutHeroList.push_back(dwRandHeroId);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(LIMIT_RECRUIT, 10, nSummonTimes, true);

	return true;
}

// ���һ����ͨ�ٻ�Ӣ��
DWORD CHeroManager::RandNormalSummonHero()
{
	// todo::����ǵ�һ���ٻ�,��س�ָ��Ӣ��
	if (!m_pPlayer->CheckOnceFlag(once_normal_summon))
	{
		m_pPlayer->SetOnceFlag(once_normal_summon);
		return g_GetConfigValue(210, 32341);
	}

	// ���Ӣ��
	DWORD dwTotalRate = CConfigManager::Instance()->GetHeroNormalSummonTotalRate();
	if (!dwTotalRate)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ͨ�ٻ��ܵĸ���Ϊ0!", __FUNCTION__);
		return 0;
	}
	DWORD dwRandRate = Rand::Instance()->urand(0, dwTotalRate - 1);
	const HeroNormalSummon_Config *pNormalSummonCfg = CConfigManager::Instance()->GetHeroNormalSummon(dwRandRate);
	if (!pNormalSummonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ͨ�ٻ����ʶ�Ӧ������[rate:%d]!", __FUNCTION__, dwRandRate);
		return 0;
	}

	return pNormalSummonCfg->dwHeroId;
}

// ���һ���߼��ٻ�Ӣ��
DWORD CHeroManager::RandAdvancedSummonHero()
{
	DWORD dwRandHeroId = 0;
	if (m_pPlayerHeroInfo->wHighSummonTimes + 1 >= g_GetConfigValue(CONFIG_ADVANCED_SUMMON_SPECIAL_TIMES))
	{
		// ���Ӣ��
		DWORD dwTotalRate = CConfigManager::Instance()->GetHeroAdvancedSummonTotalRateEx();
		if (!dwTotalRate)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�߼��ٻ��ܵļ���Ϊ0!", __FUNCTION__);
			return dwRandHeroId;
		}
		DWORD dwRandRate = Rand::Instance()->urand(0, dwTotalRate - 1);
		const HeroAdvancedSummon_Config *pAdvancedSummonCfg = CConfigManager::Instance()->GetHeroAdvancedSummonEx(dwRandRate);
		if (!pAdvancedSummonCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����߼��ٻ����ʶ�Ӧ������[rate:%d]!", __FUNCTION__, dwRandRate);
			return dwRandHeroId;
		}

		dwRandHeroId = pAdvancedSummonCfg->dwHeroId;
	}
	else
	{
		// ���Ӣ��
		DWORD dwTotalRate = CConfigManager::Instance()->GetHeroAdvancedSummonTotalRate();
		if (!dwTotalRate)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�߼��ٻ��ܵļ���Ϊ0!", __FUNCTION__);
			return dwRandHeroId;
		}
		DWORD dwRandRate = Rand::Instance()->urand(0, dwTotalRate - 1);
		const HeroAdvancedSummon_Config *pAdvancedSummonCfg = CConfigManager::Instance()->GetHeroAdvancedSummon(dwRandRate);
		if (!pAdvancedSummonCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����߼��ٻ����ʶ�Ӧ������[rate:%d]!", __FUNCTION__, dwRandRate);
			return dwRandHeroId;
		}

		dwRandHeroId = pAdvancedSummonCfg->dwHeroId;
	}

	return dwRandHeroId;
}

// ���һ��VIP�ٻ�Ӣ��
const HeroVipSummon_Config * CHeroManager::RandVipSummonHero(WORD wCamp, bool isSpecial)
{
	// ���Ӣ��
	DWORD dwTotalRate = g_pCfgMgr->GetHeroVIPSummonTotalRate(wCamp, isSpecial);
	if (!dwTotalRate)
	{
		MAPLOG(ERROR_LEVEL, "[%s]VIP�ٻ��ܵļ���Ϊ0!", __FUNCTION__);
		return NULL;
	}

	DWORD dwRandRate = g_Rand(1, dwTotalRate);
	const HeroVipSummon_Config *pVipSummonCfg = g_pCfgMgr->GetHeroVIPSummon(wCamp, isSpecial, dwRandRate);
	if (!pVipSummonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���VIP�ٻ����ʶ�Ӧ������[rate:%d]!", __FUNCTION__, dwRandRate);
		return NULL;
	}

	return pVipSummonCfg;
	//return ObjectData(pVipSummonCfg->dwHeroId, pVipSummonCfg->dwHeroNum);
}

// ȡ������������
WORD CHeroManager::GetBagEmptySize()
{
	WORD wBagSize = min(MAX_HERO_BAG_NUM, m_pPlayer->GetVipValue(VIP_HERO_BAG_NUM));
	return wBagSize > m_pPlayerHeroInfo->wHeroNum ? wBagSize - m_pPlayerHeroInfo->wHeroNum : 0;
}

// ȡӢ������ 
WORD CHeroManager::GetHeroNumByInfoId(DWORD dwHeroInfoId)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].dwIntoId == dwHeroInfoId)
		{
			++wNum;
		}
	}
	return wNum;
}

// �Ƿ�����
bool CHeroManager::IsToBattle(DWORD dwUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arFormationList[i].dwHeroUID == dwUID)
			return true;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arReinforcementList[i] == dwUID)
			return true;
	}

	return false;
}

// �Ƿ�����(ʹ��infoid���)
bool CHeroManager::IsToBattleEx(DWORD dwInfoId, BYTE bySlot)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId && bySlot != i)
			return true;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId)
			return true;
	}

	return false;
}

// �Ƿ��������
bool CHeroManager::IsCanToBattle(DWORD dwInfoId, BYTE bySlot)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		// ͬһ��Ӣ�۲���ͬʱ����,�������滻
		if (pHero && pHero->dwIntoId == dwInfoId && i + 1 != bySlot)
			return false;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId)
			return false;
	}

	return true;
}

// �Ƿ������Ԯ��
bool CHeroManager::IsCanToBattleEx(DWORD dwInfoId, BYTE bySlot)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId)
			return false;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId && i + 1 != bySlot)
			return false;
	}

	return true;
}

// ����Ӣ���б�
void CHeroManager::SendHeroBaseList()
{
	static MSG_S2C_HERO_BASE_LIST_RESP msg;
	msg.m_wHeroBaseNum = m_pPlayerHeroInfo->wHeroNum;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		ClientHeroBaseData &sDest = msg.m_HeroBaseList[i];
		HeroInfo &sSrc = m_pPlayerHeroInfo->arHeroList[i];
		sDest.dwUID = sSrc.dwUID;
		sDest.wInfoId = sSrc.dwIntoId;
		sDest.byLevel = sSrc.wLevel;
		sDest.byBreakLevel = sSrc.wBreakLevel;
		sDest.byQuality = sSrc.wQuality;
		sDest.byQualityStage = sSrc.wQualityStage;
		sDest.dwExp = sSrc.dwExp;
		sDest.byDestinyLevel = sSrc.wDestinyLevel;
		sDest.byIsToBattle = IsToBattle(sSrc.dwUID);
		sDest.byAwakenStar = sSrc.wAwakenStar;
		sDest.byAwakenLevel = sSrc.wAwakenLevel;
		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			sDest.arTrainProList[k].wType = GetTrainProType(k);
			sDest.arTrainProList[k].nValue = sSrc.arTrainProValueList[k];
			sDest.arTrainProLvList[k] = sSrc.arTrainProLvList[k];
		}
		sDest.wAwakenItemNum = sSrc.wAwakenItemNum;
		for (WORD k = 0; k < sSrc.wAwakenItemNum; ++k)
		{
			sDest.arAwakenItemList[k] = sSrc.arAwakenItemList[k];
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// ���������б�
void CHeroManager::SendFormationList()
{
	MSG_S2C_FORMATION_LIST_NOTIFY msg;	
	msg.m_byFormationNum = MAX_BATTLE_HERO_NUM;
	int nCopySize = sizeof(m_pPlayerHeroInfo->arFormationList);
	memcpy_s(msg.m_FormactionList, nCopySize, m_pPlayerHeroInfo->arFormationList, nCopySize);
	msg.m_byReinforcementNum = MAX_REINFORCEMENT_NUM;
	nCopySize = sizeof(msg.m_arReinforcementList);
	memcpy_s(msg.m_arReinforcementList, nCopySize, m_pPlayerHeroInfo->arReinforcementList, nCopySize);
	m_pPlayer->SendPacket(&msg);
}

// �����ٻ�����
void CHeroManager::SendSummonData()
{
	MSG_S2C_HERO_SUMMON_DATA_RESP msg;
	msg.m_byNormalFreeTimes = m_pPlayerHeroInfo->wNormalFreeSummonTimes;
	msg.m_dwNormalFreeTime = m_pPlayerHeroInfo->dwNormalFreeSummonTime;
	msg.m_dwAdvancedFreeTime = m_pPlayerHeroInfo->dwHighFreeSummonTime;
	msg.m_byAdvancedTimes = m_pPlayerHeroInfo->wHighSummonTimes;
	m_pPlayer->SendPacket(&msg);
}

// ��������б� 
void CHeroManager::FillFormationList(ClientFormation *pFormationList, BYTE &byFormationNum)
{
	byFormationNum = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			continue;

		ClientFormation *pClientFormation = &pFormationList[byFormationNum++];

		// ���Ӣ����Ϣ
		pClientFormation->bySlot = i;
		pClientFormation->wHeroInfoId = pHero->dwIntoId;
		pClientFormation->byAwakenStar = pHero->wAwakenStar;
		pClientFormation->wBreakLevel = pHero->wBreakLevel;
		pClientFormation->byQaulity = pHero->wQuality;

		// ���װ����Ϣ
		pClientFormation->byEquipmentNum = 0;
		for (BYTE k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			DWORD dwEquipmentId = pFormation->arEquipmentList[k];
			const Equipment *pEquipment = dwEquipmentId ? m_pPlayer->GetEquipmentManager()->GetEquipment(dwEquipmentId) : NULL;
			if (pEquipment)
			{
				FormationEquipment *pTmp = &pClientFormation->arEquipmentList[pClientFormation->byEquipmentNum++];
				pTmp->wInfoId = pEquipment->dwInfoId;
				pTmp->wStrengthenLevel = pEquipment->wEnhanceLevel;
				pTmp->byRefineLevel = pEquipment->wRefineLevel;
			}
		}

		// ��䱦����Ϣ
		pClientFormation->byTreasureNum = 0;
		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			DWORD dwTreasureId = pFormation->arTreasureList[k];
			const Treasure *pTreasure = dwTreasureId ? m_pPlayer->GetTreasureManager()->GetTreasure(dwTreasureId) : NULL;
			if (pTreasure)
			{
				FormationTreasure *pTmp = &pClientFormation->arTreasureList[pClientFormation->byTreasureNum++];
				pTmp->wInfoId = pTreasure->dwInfoId;
				pTmp->wStrengthenLevel = pTreasure->wEnhanceLevel;
				pTmp->byRefineLevel = pTreasure->wRefineLevel;
			}
		}
	}
}

// �Ƿ����ռ���Ӣ��
bool CHeroManager::IsAlreadyCollection(DWORD dwHeroInfoId)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wCollectionNum; ++i)
	{
		if (m_pPlayerHeroInfo->arCollectionList[i] == dwHeroInfoId)
			return true;
	}
	return false;
}

// ���������ȼ�
void CHeroManager::UpdateTrainLevel()
{
	bool bHaveLevelUp = false;

	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwHeroUID = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		if (!dwHeroUID)
			continue;

		HeroInfo *pHero = FindHero(dwHeroUID);
		if (!pHero)
			continue;

		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			BYTE byProType = GetTrainProType(k);
			int nProValue = pHero->arTrainProValueList[k];
			BYTE byProLevel = pHero->arTrainProLvList[k];
			BYTE byIsTake = pHero->arTrainRewardFlag[k];
			const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, byProLevel);
			// ���Ӣ��������,�ȼ�����,�����������ȼ�
			if (pTrainCfg && nProValue >= pTrainCfg->nProMax && pHero->wLevel >= pTrainCfg->wNeedHeroLevel && !byIsTake)
			{
				pHero->arTrainProLvList[k]++;
				bHaveLevelUp = true;
			}
		}
	}

	if (bHaveLevelUp)
	{
	 	// 7���¼�(Ӣ������)
	 	m_pPlayer->OnUpdateSevenDayTarget(15);
	}		
}

// ȡ��ף����Ӣ������
WORD CHeroManager::GetBlessHeroNum()
{
	WORD wNum = 0;
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arFormationList[i].dwBlessSpriteId)
			wNum++;
	}
	return wNum;
}

// �Ƿ�ף������
bool CHeroManager::IsBlessSprite(DWORD dwSpriteId)
{
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arFormationList[i].dwBlessSpriteId == dwSpriteId)
			return true;
	}
	return false;
}

// ���þ���ף��
void CHeroManager::SetSpriteBless(WORD wFormactionPos, DWORD dwSpriteId)
{
	if (wFormactionPos >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���þ���ף����ӪԽ��[pos:%d]", __FUNCTION__, wFormactionPos);
		return;
	}

	m_pPlayerHeroInfo->arFormationList[wFormactionPos].dwBlessSpriteId = dwSpriteId;
}

// ȡ��λ�ϵ�ף������
DWORD CHeroManager::GetBlessSpriteId(WORD wFormactionPos)
{
	if (wFormactionPos >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӪԽ��[pos:%d]", __FUNCTION__, wFormactionPos);
		return 0;
	}
	return m_pPlayerHeroInfo->arFormationList[wFormactionPos].dwBlessSpriteId;
}

// ȡף���������ڵ���λ
short CHeroManager::GetBlessSpriteFormactionPos(DWORD dwSpriteId)
{
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormaction = &m_pPlayerHeroInfo->arFormationList[i];
		if (pFormaction->dwBlessSpriteId && pFormaction->dwBlessSpriteId == dwSpriteId)
			return i;
	}
	return -1;
}

// ȡ����Ӣ�۵�ս��
// DWORD CHeroManager::GetAllHeroBattleValue()
// {
// 	DWORD dwTotalBattleValue = 0;
// 	for (int i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
// 	{
// 		DWORD dwHeroId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
// 		if (!dwHeroId)
// 			continue;
// 
// 		const HeroInfo *pHeroInfo = GetHero(dwHeroId);
// 		if (pHeroInfo)
// 		{
// 			dwTotalBattleValue += CPropertyManager::CalculateHeroBattleValue(pHeroInfo);
// 		}	
// 	}
// 
// 	return dwTotalBattleValue;
// }

// ����ռ���Ӣ��
void CHeroManager::AddCollectionHero(DWORD dwHeroInfoId)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wCollectionNum; ++i)
	{
		if (m_pPlayerHeroInfo->arCollectionList[i] == dwHeroInfoId)
			return;
	}

	if (m_pPlayerHeroInfo->wCollectionNum >= MAX_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "Ӣ���ռ��б�����,���ʧ��[%d]", m_pPlayerHeroInfo->wCollectionNum);
		return;
	}

	m_pPlayerHeroInfo->arCollectionList[m_pPlayerHeroInfo->wCollectionNum++] = dwHeroInfoId;
}

// ȡװ����������Ӣ��ID
WORD CHeroManager::GetEquipmentWearHeroId(DWORD dwEquipemntUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &sFormation = m_pPlayerHeroInfo->arFormationList[i];
		for (BYTE k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			if (sFormation.arEquipmentList[k] == dwEquipemntUID)
			{				
				const HeroInfo *pHero = GetHero(sFormation.dwHeroUID);
				return pHero ? pHero->dwIntoId : 0;
			}
		}
	}
	return 0;
}

// ȡ���ﱻ������Ӣ��ID 
WORD CHeroManager::GetTreasureWearHeroId(DWORD dwTreasureUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &sFormation = m_pPlayerHeroInfo->arFormationList[i];
		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			if (sFormation.arTreasureList[k] == dwTreasureUID)
			{
				const HeroInfo *pHero = GetHero(sFormation.dwHeroUID);
				return pHero ? pHero->dwIntoId : 0;
			}
		}
	}
	return 0;
}

// ȡ���Ʊ�������Ӣ��ID
DWORD CHeroManager::GetCardWearHeroId(DWORD dwCardUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &sFormation = m_pPlayerHeroInfo->arFormationList[i];
		for (BYTE k = 0; k < MAX_CARD_SLOT_NUM; ++k)
		{
			if (sFormation.arCardList[k] == dwCardUID)
			{
				const HeroInfo *pHero = GetHero(sFormation.dwHeroUID);
				return pHero ? pHero->dwIntoId : 0;
			}
		}
	}
	return 0;
}

// Ӣ�۸���֪ͨ
void CHeroManager::HeroBaseDataNotify(DWORD dwUID, BYTE byFlag, bool bNotice)
{
	const HeroInfo *pHero = GetHero(dwUID);
	if (pHero && bNotice)
	{
		MSG_S2C_HERO_BASE_INFO_NOTIFY msg;
		msg.m_byFlag = byFlag;
		msg.m_wHeroNum = 0;
		ClientHeroBaseData &tBaseData = msg.m_HeroBaseDataList[msg.m_wHeroNum++];
		tBaseData.dwUID = pHero->dwUID;
		tBaseData.wInfoId = pHero->dwIntoId;
		tBaseData.byLevel = pHero->wLevel;
		tBaseData.byBreakLevel = pHero->wBreakLevel;
		tBaseData.byIsToBattle = IsToBattle(pHero->dwUID);
		tBaseData.dwExp = pHero->dwExp;
		tBaseData.byQuality = pHero->wQuality;
		tBaseData.byQualityStage = pHero->wQualityStage;
		tBaseData.byAwakenStar = pHero->wAwakenStar;
		tBaseData.byAwakenLevel = pHero->wAwakenLevel;
		tBaseData.byDestinyLevel = pHero->wDestinyLevel;
		for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
		{
			tBaseData.arTrainProList[i].wType = GetTrainProType(i);
			tBaseData.arTrainProList[i].nValue = pHero->arTrainProValueList[i];
			tBaseData.arTrainProLvList[i] = pHero->arTrainProLvList[i];
		}
		tBaseData.wAwakenItemNum = pHero->wAwakenItemNum;
		memcpy_s(tBaseData.arAwakenItemList, sizeof(tBaseData.arAwakenItemList), pHero->arAwakenItemList, pHero->wAwakenItemNum*sizeof(WORD));

		m_pPlayer->SendPacket(&msg);
	}
}

// ͨ������ȡ��������
const Formation* CHeroManager::GetFormationByIndex(BYTE bySlot)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return NULL;

	return &m_pPlayerHeroInfo->arFormationList[bySlot];
}

// �Ƿ񴩴�
bool CHeroManager::IsEquipmentWear(BYTE bySlot, DWORD dwEquipmentInfoId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return false;

	Formation &tFormation = m_pPlayerHeroInfo->arFormationList[bySlot];
	for (BYTE i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwUID = tFormation.arEquipmentList[i];

		if (!dwUID)
			continue;

		const Equipment *pEquipment = m_pPlayer->GetEquipmentManager()->GetEquipment(dwUID);
		if (pEquipment && pEquipment->dwInfoId == dwEquipmentInfoId)
			return true;
	}

	return false;
}

// �Ƿ��ﱻ����
bool CHeroManager::IsTreasureWear(BYTE bySlot, DWORD dwTreasureInfoId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return false;

	Formation &tFormation = m_pPlayerHeroInfo->arFormationList[bySlot];
	for (BYTE i = 0; i < TREASURE_TYPE_MAX; ++i)
	{
		DWORD dwUID = tFormation.arTreasureList[i];

		if (!dwUID)
			continue;

		const Treasure *pTreasure = m_pPlayer->GetTreasureManager()->GetTreasure(dwUID);
		if (pTreasure && pTreasure->dwInfoId == dwTreasureInfoId)
			return true;
	}

	return false;
}

// �Ƿ�װ���Ŀ�������
bool CHeroManager::IsEquipCardType(BYTE bySlot, WORD wCardType)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return false;

	Formation &tFormation = m_pPlayerHeroInfo->arFormationList[bySlot];
	for (BYTE i = 0; i < MAX_CARD_SLOT_NUM; ++i)
	{
		DWORD dwUID = tFormation.arCardList[i];

		if (!dwUID)
			continue;

		const Card *pCard = m_pPlayer->GetZhanBuManager()->GetCard(dwUID);
		if (!pCard)
			continue;

		const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);

		if (pCardCfg && pCardCfg->byType == wCardType)
			return true;
	}

	return false;
}

// ȡ����ָ��ͻ�Ƶȼ���Ӣ������
WORD CHeroManager::GetHeroNumByBreakLevel(WORD wBreakLevel)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].wBreakLevel >= wBreakLevel)
			wNum++;
	}
	return wNum;
}

// ȡ����ָ��ͻ�Ƶȼ���Ӣ������
WORD CHeroManager::GetHeroNumByDestinyLevel(WORD wDestinyLevel)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].wDestinyLevel >= wDestinyLevel)
			wNum++;
	}
	return wNum;
}

// ȡ����ָ�������ȼ���Ӣ������
WORD CHeroManager::GetHeroNumByTrainLevel(WORD wTrainLevel)
{
// todo::�˴��߼�Ϊ��������
 	WORD wNum = 0;
 	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
 	{
 		HeroInfo *pHero = &m_pPlayerHeroInfo->arHeroList[i];
 		BYTE byMinLevel = pHero->arTrainProLvList[0];
 		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
 		{
 			byMinLevel = min(byMinLevel, pHero->arTrainProLvList[k]);
 		}
 		if (byMinLevel >= wTrainLevel)
 			++wNum;
 	}
 	return wNum;

// �˴��߼�����Ϊ����Ӣ��
// 	WORD wNum = 0;
// 	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
// 	{
// 		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
// 		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
// 		if (pHero)
// 		{
// 			BYTE byMinLevel = pHero->arTrainProLvList[0];
// 			for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
// 			{
// 				byMinLevel = min(byMinLevel, pHero->arTrainProLvList[k]);
// 			}
// 			if (byMinLevel >= wTrainLevel)
// 				++wNum;
// 		}
// 	}
// 
// 	return wNum;
}

// ȡ������Ӣ�۵���͵ȼ�
WORD CHeroManager::GetFormationHeroMinLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		if (!wMinLevel)
			wMinLevel = pHero->wLevel;
		
		wMinLevel = min(wMinLevel, pHero->wLevel);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// ȡ������Ӣ�۵���ͽ��׵ȼ���ͻ�Ƶȼ���
WORD CHeroManager::GetFormationHeroMinBreakLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		if (!wMinLevel)
			wMinLevel = pHero->wBreakLevel;

		wMinLevel = min(wMinLevel, pHero->wBreakLevel);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// ȡ������Ӣ�۵���������ȼ�
WORD CHeroManager::GetFormationHeroMinTrainLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		WORD wProLv = pHero->arTrainProLvList[0];
		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			wProLv = min(wProLv, pHero->arTrainProLvList[k]);
		}

		if (!wMinLevel)
			wMinLevel = wProLv;

		wMinLevel = min(wMinLevel, wProLv);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// ȡ������Ӣ�۵���������ȼ�
WORD CHeroManager::GetFormationHeroMinDestinyLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		if (!wMinLevel)
			wMinLevel = pHero->wDestinyLevel;

		wMinLevel = min(wMinLevel, pHero->wDestinyLevel);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// ȡ����������Ӣ�۵�����
WORD CHeroManager::GetFormationAllHeroAwakenStar()
{
	WORD wStar = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		wStar += pHero->wAwakenStar;
	}

	return wStar;
}

// ȡ����Ӣ������
BYTE CHeroManager::GetToBattleHeroNum()
{
	BYTE byNum = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		pHero ? byNum++ : NULL;
	}
	return byNum;
}

// �Ƿ��к��
bool CHeroManager::IsHaveRedPoint()
{
	// ��������ļ�������ļ����������ʹ��ʱ
	if (m_pPlayerHeroInfo->wNormalFreeSummonTimes < g_GetConfigValue(CONFIG_NORMAL_FREE_SUMMON_TIMES) &&
		time(NULL) >= m_pPlayerHeroInfo->dwNormalFreeSummonTime + g_GetConfigValue(CONFIG_NORMAL_SUMMON_CD) * SECOND_OF_MIN)
		return true;

	// ���߼���ļ�������ļ����������ʹ��ʱ
	if (time(NULL) > m_pPlayerHeroInfo->dwHighFreeSummonTime + g_GetConfigValue(CONFIG_ADVANCED_SUMMON_CD) * SECOND_OF_MIN)
		return true;

	// ������ļ����ļ���߿���ʱ
	if (m_pPlayer->GetItemManager()->GetItem(g_GetConfigValue(CONFIG_NORMAL_SUMMON_COST_ITEM)))
		return true;

	// �������ļ
	if (m_pPlayer->GetProperty(ROLE_PRO_FRIENDSHIP_PIONT) >= g_GetConfigValue(129))
		return true;

	return false;
}

// ���ñ�ʯ��λ
void CHeroManager::SetGemHole(WORD wFormationSlot, WORD wHole, DWORD dwGemId)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ͳ�λԽ��[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return;
	}

	if (wHole >= MAX_GEM_HOLE_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λԽ��[Hole:%d]", __FUNCTION__, wHole);
		return;
	}

	m_pPlayerHeroInfo->arFormationList[wFormationSlot].arGemList[wHole] = dwGemId;
}

// ȡԮ������С�ȼ�
WORD CHeroManager::GetReinforcementMinLevel()
{
	WORD wLevel = INVALIDID16;
	for (WORD i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUID = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUID ? GetHero(dwUID) : NULL;
		// ���п���λ,����0��
		if (!pHero)
		{
			wLevel = 0;
			break;
		}
		wLevel = min(wLevel, pHero->wLevel);
	}
	return wLevel;
}

// ȡһ���µ�ϵ��id
DWORD CHeroManager::GetNewSeriesId()
{
	if (!m_dwSeriesId)
	{
		// ȡĿǰ����ID
		for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
		{
			if (m_pPlayerHeroInfo->arHeroList[i].dwUID > m_dwSeriesId)
				m_dwSeriesId = m_pPlayerHeroInfo->arHeroList[i].dwUID;
		}
		++m_dwSeriesId;
	}

	return m_dwSeriesId++;
}

// ȡӢ��������������
int CHeroManager::GetTrainProMax(BYTE byProType, WORD wHeroLevel)
{
	int nProMax = 0;
	const vector<HeroTrain_Config> *pTrainList = g_pCfgMgr->GetHeroTrainList(byProType);
	if (pTrainList)
	{
		for (size_t i = 0; i < pTrainList->size(); ++i)
		{
			if (pTrainList->at(i).wNeedHeroLevel && wHeroLevel >= pTrainList->at(i).wNeedHeroLevel)
				nProMax = pTrainList->at(i).nProMax;
		}
	}
	return nProMax;
}

// ȡӢ��������������
int CHeroManager::GetTrainProMaxEx(BYTE byProType, WORD wProLevel)
{
	int nProMax = 0;
	const vector<HeroTrain_Config> *pTrainList = g_pCfgMgr->GetHeroTrainList(byProType);
	if (pTrainList)
	{
		for (size_t i = 0; i < pTrainList->size(); ++i)
		{
			if (wProLevel >= pTrainList->at(i).byProLevel)
				nProMax = max(nProMax, pTrainList->at(i).nProMax);
		}
	}
	return nProMax;
}

// ȡ�����ȼ�����
BYTE CHeroManager::GetTrainLevelMax(BYTE byProType)
{
	const vector<HeroTrain_Config> *pTrainList = g_pCfgMgr->GetHeroTrainList(byProType);
	return pTrainList ? pTrainList->size() : 0;
}

// ���ͻ�����ĵ�Ӣ��
bool CHeroManager::CheckBreakCostHero(HeroInfo *pBreakHero, DWORD *pForCostHeroList, WORD wForCostHeroNum,
	WORD wNeedToCostHeroNum, vector<DWORD> *pForCostHeroInfoIdList, WORD wCostType)
{
	// 0��ʾ����Ҫ����
	if (!wNeedToCostHeroNum)
		return true;

	// ������ĵ�����
	if (wForCostHeroNum != wNeedToCostHeroNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�Ӣ�������쳣[UId:%d, InfoId:%d, ForCostHeroNum:%d, NeedToCostHeroNum:%d]",
			__FUNCTION__, pBreakHero->dwUID, pBreakHero->dwIntoId, wForCostHeroNum, wNeedToCostHeroNum);
		return false;
	}

	map<DWORD, DWORD> tmp_find_list;

	// �������Ӣ���Ƿ����
	for (WORD i = 0; i < wForCostHeroNum; ++i)
	{
		DWORD dwCostHeroUId = pForCostHeroList[i];
		const HeroInfo *pCostHero = GetHero(dwCostHeroUId);
		if (!pCostHero)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�Ӣ�۲�����[InfoId:%d,CostHeroUId:%d]",
				__FUNCTION__, pBreakHero->dwIntoId, dwCostHeroUId);
			return false;
		}

		// ���ͬ��Ӣ��
		if (wCostType == SAME_NAME)
		{
			if (pBreakHero->dwIntoId != pCostHero->dwIntoId)
			{
				MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�Ӣ�۲���ͬ��Ӣ��[BreakHeroInfoId:%d,CostHeroInfoId:%d]",
					__FUNCTION__, pBreakHero->dwIntoId, pCostHero->dwIntoId);
				return false;
			}
		}
		// ͬƷ��Ӣ��
		else if (wCostType == SAME_QUALITY)
		{
			if (pBreakHero->wQuality != pCostHero->wQuality)
			{
				MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ�Ӣ�۲���ͬƷ��Ӣ��[BreakHeroInfoId:%d,CostHeroInfoId:%d,BreakHeroQuality:%d,CostHeroQuality:%d]",
					__FUNCTION__, pBreakHero->dwIntoId, pCostHero->dwIntoId,pBreakHero->wQuality, pCostHero->wQuality);
				return false;
			}
		}
		// ͬƷ��ְͬҵӢ��
		else
		{
			const Hero_Config *pBreakHeroCfg = g_pCfgMgr->GetHero(pBreakHero->dwIntoId);
			if (!pBreakHeroCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]",__FUNCTION__, pBreakHero->dwIntoId);
				return false;
			}
			const Hero_Config *pCostHeroCfg = g_pCfgMgr->GetHero(pCostHero->dwIntoId);
			if (!pBreakHeroCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]", __FUNCTION__, pCostHero->dwIntoId);
				return false;
			}

			if ((pBreakHero->wQuality != pCostHero->wQuality) || (pBreakHeroCfg->byProfession != pCostHeroCfg->byProfession))
			{
				MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ������ͬƷ��ְͬҵӢ�۳���[BreakHeroInfoId:%d,Quality:%d,Profession:%d][CostHeroInfoId:%d,Quality:%d,Profession:%d]",
					__FUNCTION__, pBreakHero->dwIntoId, pBreakHero->wQuality, pBreakHeroCfg->byProfession,
					pCostHero->dwIntoId, pCostHero->wQuality, pCostHeroCfg->byProfession);
				return false;
			}
		}

		// �Ƿ����ظ�Ӣ��
		if (tmp_find_list.find(dwCostHeroUId) != tmp_find_list.end())
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ��ͻ�����ĵ��ظ�Ӣ��[InfoId:%d,CostHeroUId:%d]",
				__FUNCTION__, pBreakHero->dwIntoId, dwCostHeroUId);
			return false;
		}

		tmp_find_list.insert(make_pair(dwCostHeroUId, dwCostHeroUId));

		if (pForCostHeroInfoIdList)
			pForCostHeroInfoIdList->push_back(dwCostHeroUId);
	}

	return true;
}

// ��¼Ӣ��ͻ�����ĵ�Ӣ��
void CHeroManager::RecordBreakCostHero(HeroInfo *pBreakHero, DWORD dwCostHeroInfoId)
{
 	if (pBreakHero->wBreakCostHeroNum >= MAX_HERO_BREAK_COST_HERO_NUM)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]��¼Ӣ��ͻ�����ĵ�Ӣ��ʧ��[RoleId:%d,BreakInfoId:%d,CostHeroUId:%d]",
 			__FUNCTION__, m_pPlayer->GetRoleId(), pBreakHero->dwIntoId, dwCostHeroInfoId);
 		return;
 	}

	pBreakHero->arBreakCostHeroList[pBreakHero->wBreakCostHeroNum++] = dwCostHeroInfoId;
}