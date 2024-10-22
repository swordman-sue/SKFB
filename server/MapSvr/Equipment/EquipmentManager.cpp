#include "stdafx.h"
#include "EquipmentManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\SystemNotice\SystemNotice.h"

CEquipmentManager::CEquipmentManager()
{
	m_dwSeriesId = 0;
}

CEquipmentManager::~CEquipmentManager()
{
		
}

void CEquipmentManager::Init(Player *pPlayer)
{
	m_dwSeriesId = 0;
	m_pPlayer = pPlayer;
	m_pEquipmentInfo = &m_pPlayer->GetRoleFullInfo().tEquipmentInfo;
}

// װ��ǿ�� 
void CEquipmentManager::OnEnhance(BYTE byStrengType, DWORD dwEquipmentUID)
{
	// ����Ƿ��д�װ��
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_STRENG_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�װ������[UID:%d]",__FUNCTION__,dwEquipmentUID);
		return;
	}

// 	MAPLOG(ERROR_LEVEL, "[%s]ǿ��װ����Ϣ[UID:%d,InfoId:%d, EnhanceLevel:%d]", __FUNCTION__, 
// 		dwEquipmentUID, pEquipment->dwInfoId, pEquipment->wEnhanceLevel);

	int nEnhanceTimes = byStrengType; // g_GetConfigValue(byStrengType, 1);
	BYTE byRealEnhanceTimes = 0;

	bool bStrengSuccess = false;
	bool bIsCrit;
	BYTE byCritTimes = 0;
	int nCostGold = 0;
	for (int i = 0; i < nEnhanceTimes; ++i)
	{
		if (!DoEnhance(pEquipment, bIsCrit, nCostGold))
			break;

		bStrengSuccess = true;

		if (bIsCrit)
			byCritTimes++;

		byRealEnhanceTimes++;
	}

	// ǿ��ʧ��
	if (!bStrengSuccess)
	{
		m_pPlayer->SendErrorMsg(ERROR_EQUIPMENT_STRENGTH_FAIL);
	}
	else
	{
		// ͬ�����(��Ϊ��ǿ����ֻ�ǿ۳�,ľ��֪ͨ�ͻ���)
		m_pPlayer->NotifyProperty(ROLE_PRO_GOLD, m_pPlayer->GetProperty(ROLE_PRO_GOLD));

		// �洢������־
		m_pPlayer->SaveCurrencyLog(ROLE_PRO_GOLD, -nCostGold, FROM_EQUIP_ENHANCE);

		// ����ս��
		m_pPlayer->UpdateFightingPower();

		EquipmentUpdateNotify(dwEquipmentUID);

		MSG_S2C_EQUIPMENT_STRENG_RESP msg;
		msg.m_byType = byRealEnhanceTimes;
		msg.m_dwUID = dwEquipmentUID;
		msg.m_byCritTimes = byCritTimes;
		msg.m_wStrengthenLevel = pEquipment->wEnhanceLevel;
		m_pPlayer->SendPacket(&msg);

		// �ɾ�
		m_pPlayer->OnUpdateAchievement(17, GetEquipmentEnhanceMinLevel(), false);

		// �ճ�����(װ��ǿ��)
		m_pPlayer->OnUpdateDailyMission(21);

		// 7��(װ��ǿ��)
		m_pPlayer->OnUpdateSevenDayTarget(5);
	}
}

// һ��ǿ��
void CEquipmentManager::OnNetOneKeyEnhance(BYTE bySlot)
{
	// ���һ��ǿ����������(VIP�ȼ����߽�ɫ�ȼ�)
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < g_GetConfigValue(116) && m_pPlayer->GetLevel() < g_GetConfigValue(117))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��������,����ʹ��һ��ǿ��", __FUNCTION__);
		return;
	}

	// ����λ�����Ϸ���
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���Ͳ�λ����Խ��[Slot:%d]",__FUNCTION__, bySlot);
		return;
	}

	Formation *pSlotData = &m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[bySlot];

	// ����Ƿ�������Ӣ��
	if (!pSlotData->dwHeroUID)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����Ͳ�λ��ľ��Ӣ��[Slot:%d]", __FUNCTION__, bySlot);
		return;
	}

	// ǿ��֮ǰ�ȼ�
	WORD arOldEnhanceLevel[EQUIPMENT_TYPE_MAX] = {0};
	for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwEquipmentId = pSlotData->arEquipmentList[i];
		Equipment *pEquipment = dwEquipmentId ? FindEquipment(dwEquipmentId) : NULL;
		if (pEquipment)
			arOldEnhanceLevel[i] = pEquipment->wEnhanceLevel;
	}

	int nCostGold = 0;
	while (true)
	{
		// ѡ�����ʵ�װ����ǿ��(�ȼ���͵�����)
		Equipment *pEnhanceEquipment = NULL;
		//int nPreCostCold = 0;
		WORD wEnhanceLevel = 0;
		for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
		{
			DWORD dwEquipmentId = pSlotData->arEquipmentList[i];
			Equipment *pEquipment = dwEquipmentId ? FindEquipment(dwEquipmentId) : NULL;
			if (!pEquipment)
				continue;

			const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
			if (!pEquipCfg)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ��������[InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
				return;
			}

			const EquipmentEnhanceLevel_Config *pStrengLevelCfg = 
				g_pCfgMgr->GetEquipmentEnhanceLevel(pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
			if (!pStrengLevelCfg)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ��ǿ������[InfoId:%d, Quality:%d, StrengthenLevel:%d]",
					__FUNCTION__, pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
				return;
			}

			// ���װ��ǿ���ȼ�����(��ɫ�ȼ�����) ���ǿ������
			if (pEquipment->wEnhanceLevel < m_pPlayer->GetLevel() * 2 
				&& m_pPlayer->GetProperty(ROLE_PRO_GOLD) >= pStrengLevelCfg->nCostGold
				&& (pEquipment->wEnhanceLevel < wEnhanceLevel || !wEnhanceLevel))
				//&& (pStrengLevelCfg->nCostGold < nPreCostCold || !nPreCostCold))
			{
				pEnhanceEquipment = pEquipment;
				//nPreCostCold = pStrengLevelCfg->nCostGold;
				wEnhanceLevel = pEquipment->wEnhanceLevel;
			}
		}

		// ���ľ�п�ǿ����װ��,���˳�
		if (!pEnhanceEquipment)
			break;

		// ����ǿ��
		bool bIsCrit = false;
		if (!DoEnhance(pEnhanceEquipment, bIsCrit, nCostGold))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]����װ��ǿ��ʧ��[UID:%d,InfoId:%d]", __FUNCTION__,
				pEnhanceEquipment->dwId, pEnhanceEquipment->dwInfoId);
			return;
		}
	}

	if (!nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�п�ǿ����װ��[slot:%d]", __FUNCTION__, bySlot);
		return;
	}

	// ͬ�����(��Ϊ��ǿ����ֻ�ǿ۳�,ľ��֪ͨ�ͻ���)
	m_pPlayer->NotifyProperty(ROLE_PRO_GOLD, m_pPlayer->GetProperty(ROLE_PRO_GOLD));

	// �洢������־
	m_pPlayer->SaveCurrencyLog(ROLE_PRO_GOLD, -nCostGold, FROM_EQUIP_ENHANCE);

	// ��¼ǿ��֮��ȼ�
	WORD arNewEnhanceLevel[EQUIPMENT_TYPE_MAX] = { 0 };
	for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwEquipmentId = pSlotData->arEquipmentList[i];
		Equipment *pEquipment = dwEquipmentId ? FindEquipment(dwEquipmentId) : NULL;
		if (pEquipment)
			arNewEnhanceLevel[i] = pEquipment->wEnhanceLevel;
	}

	// ͬ������
	for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		if (pSlotData->arEquipmentList[i])
			EquipmentUpdateNotify(pSlotData->arEquipmentList[i]);
	}

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(17, GetEquipmentEnhanceMinLevel(), false);

	// �ճ�����(װ��ǿ��)
	m_pPlayer->OnUpdateDailyMission(21);

	// 7��(װ��ǿ��)
	m_pPlayer->OnUpdateSevenDayTarget(5);

	// ��Ӧ
	MSG_S2C_EQUIPMENT_ONE_KEY_ENHANCE_RESP msg;
	msg.m_bySlot = bySlot;
	msg.m_wEquipmentNum = EQUIPMENT_TYPE_MAX;
	for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		msg.m_arOldEnhanceLevel[i] = arOldEnhanceLevel[i];
		msg.m_arNewEnhanceLevel[i] = arNewEnhanceLevel[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// װ������
void CEquipmentManager::OnRefineEx(DWORD dwEquipmentUID, WORD wItemId, WORD wItemNum)
{
	// ����Ƿ��д�װ��
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�װ������[UID:%d]",__FUNCTION__, dwEquipmentUID);
		return;
	}

	const Equipment_Config *pEquipCfg = CConfigManager::Instance()->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "�Ҳ���װ������������[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	const Server_Config *pCommonCfg = CConfigManager::Instance()->GetCommonCfg(17);
	if (!pCommonCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "�Ҳ���ͨ������[Key:%d]", 17);
		return;
	}
	bool bFound = false;
	for (int i = 0; i < pCommonCfg->vValueList.size(); ++i)
	{
		if (pCommonCfg->vValueList[i] == wItemId)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����װ��������������Ʒ[EquipentInfoId:%d,ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pEquipment->dwInfoId, wItemId, wItemNum);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	if (pItemMgr->GetItemNum(wItemId) < wItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ); 
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˾�����Ʒ[EquipentInfoId:%d,CostItemId:%d,CostItemNum:%d,HaveItemNum:%d]!",
			__FUNCTION__, pEquipment->dwInfoId, wItemId, wItemNum, pItemMgr->GetItemNum(wItemId));
		return;
	}

	// ����Ƿ��д���Ʒ
	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(wItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "�Ҳ�����Ʒ����������[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// ��⾫���ȼ��Ƿ񵽴�����
	if (pEquipment->wRefineLevel >= pEquipCfg->wRefineLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]װ�������ȼ��Ѵ�����![UID:%d]",__FUNCTION__, dwEquipmentUID);
		return;
	}

	// ����ɫ�ȼ�����
// 	if (pEquipment->wRefineLevel >= m_pPlayer->GetLevel())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[name:%s]װ�������ȼ��ܽ�ɫ�ȼ�����![UID:%d]",
// 			m_pPlayer->GetUTF8RoleName(), dwEquipmentUID);
// 		return;
// 	}

	// ��Ӿ���
	pEquipment->dwRefineExp += (pItemCfg->nParam1 * wItemNum);

	WORD wOldRefineLv = pEquipment->wRefineLevel;

	// ���������߼�
	while (true)
	{
		const EquipmentRefineLevel_Config *pRefineLevelCfg =
			CConfigManager::Instance()->GetEquipmentRefineLevel(pEquipCfg->wQuality, pEquipment->wRefineLevel);
		if (!pRefineLevelCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
			MAPLOG(ERROR_LEVEL, "�Ҳ���װ�������ȼ�����������[InfoId:%d, Quality:%d, RefineLv:%d]", 
				pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wRefineLevel);
			break;
		}

		// ���鲻�㹻����
		if (pEquipment->dwRefineExp < pRefineLevelCfg->dwExp)
			break;

		pEquipment->wRefineLevel++;
		pEquipment->dwRefineExp -= pRefineLevelCfg->dwExp;
	}

	// �۳���Ʒ
	pItemMgr->RemoveItem(wItemId, wItemNum, TRUE, FROM_EQUIPMENT_REFINE);

	AddRefineCostItem(dwEquipmentUID, wItemId, wItemNum);

	EquipmentUpdateNotify(dwEquipmentUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_EQUIPMENT_REFINE_RESP msg;
	msg.m_dwEquipmentUID = dwEquipmentUID;
	msg.m_byOldRefineLevel = wOldRefineLv;
	msg.m_byNewRefineLevel = pEquipment->wRefineLevel;
	msg.m_dwRefineExp = pEquipment->dwRefineExp;
	msg.m_dwAddRefineExp = pItemCfg->nParam1 * wItemNum;
	m_pPlayer->SendPacket(&msg);

	// 7���¼�(װ������)
	m_pPlayer->OnUpdateSevenDayTarget(14);

	// ����㲥
	if (pEquipment->wRefineLevel >= g_GetConfigValue(111) && pEquipment->wRefineLevel > wOldRefineLv)
	{
		CSystemNotice sys_notice(7, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(pEquipment->dwInfoId);
		sys_notice.AddParam(pEquipment->wRefineLevel);
		sys_notice.SendMsg();
	}
}

// װ������
void CEquipmentManager::OnRefine(DWORD dwEquipmentUID, EquipmentRefineCostData *pCostList, WORD wCostListNum)
{
	WORD wFromId = C2S_EQUIPMENT_REFINE_REQ;
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// ����Ƿ��д�װ��
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�װ������[UID:%d]", __FUNCTION__, dwEquipmentUID);
		return;
	}

	const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "�Ҳ���װ������������[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// ��⾫���ȼ��Ƿ񵽴�����
	if (pEquipment->wRefineLevel >= pEquipCfg->wRefineLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "[%s]װ�������ȼ��Ѵ�����![UID:%d]", __FUNCTION__, dwEquipmentUID);
		return;
	}

	const Server_Config *pCommonCfg = g_pCfgMgr->GetCommonCfg(17);
	if (!pCommonCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "�Ҳ���ͨ������[Key:%d]", 17);
		return;
	}

	DWORD dwTotalRefineExp = 0;
	for (WORD i = 0; i < wCostListNum; ++i)
	{
		// ����Ƿ������ĵ���Ʒ
		EquipmentRefineCostData &tCostData = pCostList[i];
		if (!pCommonCfg->IsInList(tCostData.dwItemId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "[%s]����װ��������������Ʒ[EquipentInfoId:%d,ItemId:%d,ItemNum:%d]",
				__FUNCTION__, pEquipment->dwInfoId, tCostData.dwItemId, tCostData.wItemNum);
		}

		if (pItemMgr->GetItemNum(tCostData.dwItemId) < tCostData.wItemNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "[%s]���û�д˾�����Ʒ[EquipentInfoId:%d,CostItemId:%d,CostItemNum:%d,HaveItemNum:%d]!",
				__FUNCTION__, pEquipment->dwInfoId, tCostData.dwItemId, tCostData.wItemNum, pItemMgr->GetItemNum(tCostData.dwItemId));
			return;
		}

		// ����Ƿ��д���Ʒ
		const Item_Config *pItemCfg = g_pCfgMgr->GetItem(tCostData.dwItemId);
		if (!pItemCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "�Ҳ�����Ʒ����������[InfoId:%d]", pEquipment->dwInfoId);
			return;
		}

		// ��Ӿ���
		dwTotalRefineExp += (pItemCfg->nParam1 * tCostData.wItemNum);
	}

	pEquipment->dwRefineExp += dwTotalRefineExp;

	WORD wOldRefineLv = pEquipment->wRefineLevel;

	// ���������߼�
	const EquipmentRefineLevel_Config *pRefineLevelCfg = NULL;
	while (true)
	{
		pRefineLevelCfg = g_pCfgMgr->GetEquipmentRefineLevel(pEquipCfg->wQuality, pEquipment->wRefineLevel);
		if (!pRefineLevelCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "�Ҳ���װ�������ȼ�����������[InfoId:%d, Quality:%d, RefineLv:%d]",
				pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wRefineLevel);
			break;
		}

		// ���鲻�㹻����
		if (pEquipment->dwRefineExp < pRefineLevelCfg->dwExp)
			break;

		pEquipment->wRefineLevel++;
		pEquipment->dwRefineExp -= pRefineLevelCfg->dwExp;
	}

	for (WORD i = 0; i < wCostListNum; ++i)
	{
		EquipmentRefineCostData &tCostData = pCostList[i];

		// �۳���Ʒ
		pItemMgr->RemoveItem(tCostData.dwItemId, tCostData.wItemNum, TRUE, FROM_EQUIPMENT_REFINE);

		// ��¼���ĵ���Ʒ
		AddRefineCostItem(dwEquipmentUID, tCostData.dwItemId, tCostData.wItemNum);
	}

	EquipmentUpdateNotify(dwEquipmentUID);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_EQUIPMENT_REFINE_RESP msg;
	msg.m_dwEquipmentUID = dwEquipmentUID;
	msg.m_byOldRefineLevel = wOldRefineLv;
	msg.m_byNewRefineLevel = pEquipment->wRefineLevel;
	msg.m_dwRefineExp = pEquipment->dwRefineExp;
	msg.m_dwAddRefineExp = dwTotalRefineExp;
	m_pPlayer->SendPacket(&msg);

	// 7���¼�(װ������)
	m_pPlayer->OnUpdateSevenDayTarget(14);

	// ����㲥
	if (pEquipment->wRefineLevel >= g_GetConfigValue(111) && pEquipment->wRefineLevel > wOldRefineLv)
	{
		CSystemNotice sys_notice(7, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(pEquipment->dwInfoId);
		sys_notice.AddParam(pEquipment->wRefineLevel);
		sys_notice.SendMsg();
	}

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 35, wCostListNum, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 35, wCostListNum, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 35, wCostListNum, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 35, wCostListNum, true);
}

// װ���ϳ�
void CEquipmentManager::OnCompose(DWORD dwEquipmentInfoId)
{
	const EquipmentCompose_Config *pComposeCfg = CConfigManager::Instance()->GetEquipmentCompose(dwEquipmentInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_COMPOSE_REQ);
		MAPLOG(ERROR_LEVEL, "�Ҳ���װ���ϳ���������[InfoId:%d]", dwEquipmentInfoId);
		return;
	}

	// ��������Ƿ����㹻����Ʒ
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	if (pItemMgr->GetItemNum(pComposeCfg->dwFragmentId) < pComposeCfg->wFragmentNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_COMPOSE_REQ);
		MAPLOG(ERROR_LEVEL, "[name:%s]�����Ʒ����[ItemID:%d,Num:%d]����!",
			m_pPlayer->GetUTF8RoleName(), pComposeCfg->dwFragmentId, pItemMgr->GetItemNum(pComposeCfg->dwFragmentId));
		return;
	}

	// ���װ�������Ƿ�����
	if (!GetBagEmptySize())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_COMPOSE_REQ);
		MAPLOG(ERROR_LEVEL, "[name:%s]���װ�������ռ䲻��!",m_pPlayer->GetUTF8RoleName());
		return;
	}

	// ���װ��
	AddEquipent(dwEquipmentInfoId, TRUE, FROM_EQUIPMENT_COMPOSE);

	// ������Ʒ
	pItemMgr->RemoveItem(pComposeCfg->dwFragmentId, pComposeCfg->wFragmentNum, TRUE, FROM_EQUIPMENT_COMPOSE);

	MSG_S2C_EQUIPMENT_COMPOSE_RESP msg;
	msg.m_dwInfoId = dwEquipmentInfoId;
	m_pPlayer->SendPacket(&msg);
}

// ����װ���б�
void CEquipmentManager::OnEquipmentList()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_EQUIPMENT_LIST_RESP msg;
	msg.m_wEquipmentNum = 0; 
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		EquipmentClientData &sDestData = msg.m_EquipmentList[msg.m_wEquipmentNum++];
		Equipment &sSrcData = m_pEquipmentInfo->sEquipmentList[i];
		sDestData.dwUID = sSrcData.dwId;
		sDestData.wInfoId = sSrcData.dwInfoId;
		sDestData.wEnhanceLevel = sSrcData.wEnhanceLevel;
		sDestData.byRefineLevel = sSrcData.wRefineLevel;
		sDestData.dwRefineExp = sSrcData.dwRefineExp;
		sDestData.wWearHeroId = pHeroMgr->GetEquipmentWearHeroId(sSrcData.dwId);
		sDestData.nEnhanceCostGold = sSrcData.nEnhanceCostGold;
		sDestData.byStar = sSrcData.wStar;
		sDestData.byStarLevel = sSrcData.wStarLevel;

// 		MAPLOG(ERROR_LEVEL, "װ����Ϣ[uid:%d, infoid:%d, EnhanceLevel:%d]", 
// 			sSrcData.dwId, sSrcData.dwInfoId, sSrcData.wEnhanceLevel);
	}
	m_pPlayer->SendPacket(&msg);
}

// ����װ������
void CEquipmentManager::OnEquipmentInfo(DWORD dwUID)
{
	const Equipment *pEquipment = GetEquipment(dwUID);
	MSG_S2C_EQUIPMENT_INFO_RESP msg;
	msg.m_Equipment = *pEquipment;
	m_pPlayer->SendPacket(&msg);
}

// ����װ��
void CEquipmentManager::OnEquipmentPutOn(BYTE byFormationPos, BYTE byEquipmentPos, DWORD dwUID)
{
	// ������ (����λ��, װ��λ��)
	if (!byFormationPos || byFormationPos > MAX_BATTLE_HERO_NUM || byEquipmentPos >= EQUIPMENT_TYPE_MAX)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_PUT_ON_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����װ��������������[formation_pos:%d, equipment_pos:%d]", 
			__FUNCTION__, byFormationPos, byEquipmentPos);
		return;
	}

	BYTE bySvrFormationPos = byFormationPos - 1;
	BYTE bySvrEquipmentPos = byEquipmentPos;


	// ����Ƿ��д�װ��
	if (dwUID)
	{
		const Equipment *pEquipment = GetEquipment(dwUID);
		if (!pEquipment)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_PUT_ON_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]���û�д�װ��[UID:%d]", __FUNCTION__, dwUID);
			return;
		}
	}

	// todo::����װ���Ƿ���Դ�������λ��

	Formation &sFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[bySvrFormationPos];
	DWORD dwOldEquipmentUID = sFormation.arEquipmentList[bySvrEquipmentPos];
	sFormation.arEquipmentList[bySvrEquipmentPos] = dwUID;

	if (dwOldEquipmentUID)
		EquipmentUpdateNotify(dwOldEquipmentUID);
	EquipmentUpdateNotify(dwUID);

	// ����ͬ�������б�
	m_pPlayer->GetHeroManager()->SendFormationList();

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_EQUIPMENT_PUT_ON_RESP msg;
	msg.m_byFormationPos = byFormationPos;
	msg.m_byEquipmentPos = byEquipmentPos;
	msg.m_dwUID = dwUID;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(18, GetEquipmentLowQuality(), false);

	// 7���¼�(װ������)
	m_pPlayer->OnUpdateSevenDayTarget(13);
}

// ����
void CEquipmentManager::OnNetStarUP(DWORD dwUID)
{
	// ȡװ������
	Equipment *pEquipment = FindEquipment(dwUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�װ������[UID:%d]", __FUNCTION__, dwUID);
		return;
	}

	// ȡװ������
	const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�Ҳ���װ������������[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// ����װ���Ƿ������
	if (!pEquipCfg->wStarMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "��װ����������[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// ���װ���Ƿ��Ѵ���������
	if (pEquipment->wStar >= pEquipCfg->wStarMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "װ�����ڴ���������[InfoId:%d,Star:%d]", pEquipment->dwInfoId, pEquipment->wStar);
		return;
	}

	// ȡ����������
	const EquipmentStarUp_Config *pStarUPCfg = g_pCfgMgr->GetEquipmentStartUp(pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel);
	if (!pStarUPCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�Ҳ���װ����������[type:%d,quality:%d,Star:%d,level:%d]", 
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel);
		return;
	}

	// �������(���)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < pStarUPCfg->nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "װ���������Ľ�Ҳ���[type:%d,quality:%d,Star:%d,level:%d,CostGold:%d]",
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel, pStarUPCfg->nCostGold);
		return;
	}

	// �������(����)
	if (m_pPlayer->GetItemNum(pStarUPCfg->tCostItem.dwId) < pStarUPCfg->tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "װ���������ĵ��߲���[type:%d,quality:%d,Star:%d,level:%d,CostItemId:%d,CostItemNum:%d]",
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel, pStarUPCfg->tCostItem.dwId, pStarUPCfg->tCostItem.dwNum);
	}

	// �������(��Ƭ)
	if (m_pPlayer->GetItemNum(pEquipCfg->dwStarUpCostItemId) < pStarUPCfg->dwCostSelfNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "װ������������Ƭ����[type:%d,quality:%d,Star:%d,level:%d,CostSelfId:%d,CostSelfNum:%d]",
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel, pEquipCfg->dwStarUpCostItemId, pStarUPCfg->dwCostSelfNum);
	}

	// ��������(���)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pStarUPCfg->nCostGold, TRUE, FROM_EQUIPMENT_STAR_UP);

	// ��������(����)
	m_pPlayer->RemoveItem(pStarUPCfg->tCostItem.dwId, pStarUPCfg->tCostItem.dwNum, FROM_EQUIPMENT_STAR_UP);

	// ��������(��Ƭ)
	m_pPlayer->RemoveItem(pEquipCfg->dwStarUpCostItemId, pStarUPCfg->dwCostSelfNum, FROM_EQUIPMENT_STAR_UP);

	// ����Ƿ�����
	if (pStarUPCfg->bIsStarUp)
	{
		pEquipment->wStar++;
		pEquipment->wStarLevel = 0;
	}
	else
	{
		pEquipment->wStarLevel++;
	}
	
	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// ͬ��װ������
	EquipmentUpdateNotify(dwUID);

	MSG_S2C_EQUIPMENT_STAR_UP_RESP msg;
	msg.m_byCurStar = pEquipment->wStar;
	msg.m_byCurStarLevel = pEquipment->wStarLevel;
	msg.m_dwUID = dwUID;
	m_pPlayer->SendPacket(&msg);
}

// ��Ʒ
void CEquipmentManager::OnNetQualityUP(DWORD dwUID)
{
	// ȡװ������
	Equipment *pEquipment = FindEquipment(dwUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�װ������[UID:%d]", __FUNCTION__, dwUID);
		return;
	}

	// ȡװ������
	const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ������������[InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
		return;
	}

	// ����Ƿ������Ʒ
	if (!pEquipCfg->wNextQualityEquipmentId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��װ��������Ʒ[InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
		return;
	}

	// ȡ����Ʒ��
	const EquipmentQualityUp_Config *pQualityUpCfg = g_pCfgMgr->GetEquipmentQualityUp(pEquipCfg->wQuality);
	if (!pQualityUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ������Ʒ��������[Quality:%d]", __FUNCTION__, pEquipCfg->wQuality);
		return;
	}

	// �������(����)
	if (m_pPlayer->GetItemNum(pQualityUpCfg->tCostItem.dwId) < pQualityUpCfg->tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]װ����Ʒ���ĵ��߲���[Quality:%d,CostItemId:%d,CostItemNum:%d]", 
			__FUNCTION__, pEquipCfg->wQuality, pQualityUpCfg->tCostItem.dwId, pQualityUpCfg->tCostItem.dwNum);
		return;
	}

	// �������(����)
	const CostData &tCostCurrency = pQualityUpCfg->tCostCurrency;
	if (m_pPlayer->GetProperty(tCostCurrency.wType) < tCostCurrency.nValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]װ����Ʒ���Ļ��Ҳ���[Quality:%d,CostCurrencyType:%d,CostCurrencyValue:%d]",
			__FUNCTION__, pEquipCfg->wQuality, tCostCurrency.wType, tCostCurrency.nValue);
		return;
	}

	// ��������(����)
	m_pPlayer->RemoveItem(pQualityUpCfg->tCostItem, FROM_EQUIPMENT_QUALITY_UP);

	// ��������(����)
	m_pPlayer->ModifyProperty(tCostCurrency.wType, -tCostCurrency.nValue, TRUE, FROM_EQUIPMENT_QUALITY_UP);

	// ��Ϊ��һ��װ��
	pEquipment->dwInfoId = pEquipCfg->wNextQualityEquipmentId;

	// �������Ǽ�����(todo::Ŀǰ���������߼�,�߻����˵�պ��п��ܻ����)
	pEquipment->wStar = 0;
	pEquipment->wStarLevel = 0;

	// װ��ǿ���ȼ�ת�������յ�ǰװ���������¼���Ŀ���װ�ľ���
	pEquipment->wEnhanceLevel = GetNextQualityEnhanceLevel(pEquipCfg->wQuality, pEquipment->wEnhanceLevel);

	// װ�������ȼ�ת�������յ�ǰװ�������ȼ��ľ���ֵ�����ϵľ���ֵ���¼���Ŀ���װ�ľ����ȼ�
	pEquipment->wRefineLevel = GetNextQualityRefineLevel(pEquipCfg->wQuality, pEquipment->wRefineLevel, pEquipment->dwRefineExp);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// ����ͬ�����º��װ������
	EquipmentUpdateNotify(dwUID);

	MSG_S2C_EQUIPMENT_QUALITY_UP_RESP msg;
	msg.m_dwUID = dwUID;
	m_pPlayer->SendPacket(&msg);
}

// ���װ��
bool CEquipmentManager::AddEquipent(DWORD dwEquipmentInfoId, bool bNotify, WORD wFromId)
{
	const Equipment_Config *pEquipCfg = CConfigManager::Instance()->GetEquipment(dwEquipmentInfoId);
	if (!pEquipCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ���װ����������[InfoId:%d]", dwEquipmentInfoId);
		return false;
	}

	Equipment sEquipment;
	memset(&sEquipment, 0, sizeof(sEquipment));
	sEquipment.dwId = GetNewSeriesId();
	sEquipment.dwInfoId = dwEquipmentInfoId;
	sEquipment.wEnhanceLevel = 1;
	return AddEquipment(sEquipment, bNotify, wFromId);
}

// ���װ��
bool CEquipmentManager::AddEquipment(const Equipment &sEquipment, bool bNotify, WORD wFromId)
{
	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]װ����������[num:%d]", __FUNCTION__, m_pEquipmentInfo->wEquipmentNum);

		// ��¼װ����ʧ��¼
		m_pPlayer->SaveItemLoseRecord(sEquipment.dwInfoId);

		return false;
	}

	m_pEquipmentInfo->sEquipmentList[m_pEquipmentInfo->wEquipmentNum++] = sEquipment;

	if (bNotify)
	{
		EquipmentUpdateNotify(sEquipment.dwId);
	}

	// ��¼���
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), sEquipment.dwInfoId, 1, true, wFromId);

	return true;
}

// ɾ��װ��
bool CEquipmentManager::RemoveEquipment(DWORD dwEquipmentUID, bool bNotify, WORD wFromId)
{
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		Equipment &tFind = m_pEquipmentInfo->sEquipmentList[i];
		if (tFind.dwId == dwEquipmentUID)
		{
			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tFind.dwInfoId, 1, false, wFromId);

			if (bNotify)
				EquipmentUpdateNotify(dwEquipmentUID, false);
			tFind = m_pEquipmentInfo->sEquipmentList[--m_pEquipmentInfo->wEquipmentNum];
			return true;
		}
	}

	return false;
}

// ȡװ��������������
WORD CEquipmentManager::GetBagEmptySize()
{
	//return MAX_EQUIPMENT_NUM - m_pEquipmentInfo->wEquipmentNum;

	WORD wBagSize = min(MAX_EQUIPMENT_NUM, m_pPlayer->GetVipValue(VIP_EQUIPMENT_BAG_NUM));
	return wBagSize > m_pEquipmentInfo->wEquipmentNum ? wBagSize - m_pEquipmentInfo->wEquipmentNum : 0;
}

// ��Ӿ���������Ʒ 
bool CEquipmentManager::AddRefineCostItem(DWORD dwEquipmentUID, DWORD dwItemId, DWORD dwItemNum)
{
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
		return false;

	for (WORD i = 0; i < pEquipment->wRefineCostItemNum; ++i)
	{
		if (pEquipment->arRefineCostItem[i].dwId == dwItemId)
		{
			pEquipment->arRefineCostItem[i].dwNum += dwItemNum;
			return true;
		}
	}

	if (pEquipment->wRefineCostItemNum >= MAX_EQUIPMENT_REFINE_COST_ITEM)
		return false;

	pEquipment->arRefineCostItem[pEquipment->wRefineCostItemNum].dwId = dwItemId;
	pEquipment->arRefineCostItem[pEquipment->wRefineCostItemNum].dwNum = dwItemNum;
	pEquipment->wRefineCostItemNum++;

	return true;
}

// ȡװ������
const Equipment* CEquipmentManager::GetEquipment(DWORD dwUID)
{
	return FindEquipment(dwUID);
}

// װ������֪ͨ
void CEquipmentManager::EquipmentUpdateNotify(DWORD dwUID, BYTE byFlag)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	const Equipment *pEquipment = GetEquipment(dwUID);
	if (pEquipment)
	{
		MSG_S2C_EQUIPMENT_INFO_NOTIFY msg;
		msg.m_sEquipment.dwUID = pEquipment->dwId;
		msg.m_sEquipment.wInfoId = pEquipment->dwInfoId;
		msg.m_sEquipment.wEnhanceLevel = pEquipment->wEnhanceLevel;
		msg.m_sEquipment.byRefineLevel = pEquipment->wRefineLevel;
		msg.m_sEquipment.dwRefineExp = pEquipment->dwRefineExp;
		msg.m_sEquipment.wWearHeroId = pHeroMgr->GetEquipmentWearHeroId(dwUID);
		msg.m_sEquipment.nEnhanceCostGold = pEquipment->nEnhanceCostGold;
		msg.m_sEquipment.byStar = pEquipment->wStar;
		msg.m_sEquipment.byStarLevel = pEquipment->wStarLevel;
		msg.m_byFlag = byFlag;
		m_pPlayer->SendPacket(&msg);
	}
}

// ȡ����ָ��ǿ���ȼ���װ������
WORD CEquipmentManager::GetEquipmentNumByStrengthenLevel(WORD wStrenthenLevel)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		if (m_pEquipmentInfo->sEquipmentList[i].wEnhanceLevel >= wStrenthenLevel)
			wNum++;
	}
	return wNum;
}

// ȡ����ָ�������ȼ���װ������
WORD CEquipmentManager::GetEquipmentNumByRefineLevel(WORD wRefineLevel)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		if (m_pEquipmentInfo->sEquipmentList[i].wRefineLevel >= wRefineLevel)
			wNum++;
	}
	return wNum;
}

// ȡ����ָ��Ʒ�ʵ�װ������
WORD CEquipmentManager::GetEquipmentNumByQuality(WORD wQuality)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	WORD wNum = 0;
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		Equipment *pEquipment = &m_pEquipmentInfo->sEquipmentList[i];

		const Equipment_Config *pEquipmentCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);

		if (pEquipmentCfg && pEquipmentCfg->wQuality >= wQuality && pHeroMgr->GetEquipmentWearHeroId(pEquipment->dwId))
			wNum++;
	}
	return wNum;
}

// ȡǿ����С�ȼ�(��װ����)
WORD CEquipmentManager::GetEquipmentEnhanceMinLevel()
{
	WORD wMinLevel = 0;
	const Formation *pFormationList = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList;
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation &tFormation = pFormationList[i];
		if (!tFormation.dwHeroUID)
			return 0;

		for (WORD k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			DWORD dwEquipmentId = tFormation.arEquipmentList[k];
			const Equipment *pEquipment = dwEquipmentId ? GetEquipment(dwEquipmentId) : NULL;
			if (!pEquipment)
				return 0;

			if (!wMinLevel)
				wMinLevel = pEquipment->wEnhanceLevel;

			wMinLevel = min(wMinLevel, pEquipment->wEnhanceLevel);
		}
	}
	return wMinLevel;
}

// ȡװ�����Ʒ��(��װ����) 
WORD CEquipmentManager::GetEquipmentLowQuality()
{
	WORD wLowQuality = 0;
	const Formation *pFormationList = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList;
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation &tFormation = pFormationList[i];
		if (!tFormation.dwHeroUID)
			return 0;

		for (WORD k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			DWORD dwEquipmentId = tFormation.arEquipmentList[k];
			const Equipment *pEquipment = dwEquipmentId ? GetEquipment(dwEquipmentId) : NULL;
			if (!pEquipment)
				return 0;

			const Equipment_Config *pEquipmentCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
			if (!pEquipmentCfg)
				return 0;

			if (!wLowQuality)
				wLowQuality = pEquipmentCfg->wQuality;

			wLowQuality = min(wLowQuality, pEquipmentCfg->wQuality);
		}
	}

	return wLowQuality;
}

// ȡ�¸�Ʒ�ʵ�ǿ���ȼ�
WORD CEquipmentManager::GetNextQualityEnhanceLevel(WORD wCurQuality, WORD wCurEnhanceLevel)
{
	// ȡǿ������
	const EquipmentEnhanceLevel_Config *pEnhanceCfg = g_pCfgMgr->GetEquipmentEnhanceLevel(wCurQuality, wCurEnhanceLevel);
	if (!pEnhanceCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ����װ��ǿ������[Quality:%d,EnhanceLevel:%d]", __FUNCTION__, wCurQuality, wCurEnhanceLevel);
		return 0;
	}

	// װ��ǿ����õľ�������ĵĽ����ͬ
	int nTotalExp = pEnhanceCfg->nTotalCostGold;

	WORD wNewEquipmentQuality = wCurQuality + 1;
	WORD wNewEquipmentEnhanceLevel = 1;

	const EquipmentEnhanceLevel_Config *pNewEquipmentEnhanceCfg;
	while (true)
	{
		pNewEquipmentEnhanceCfg = g_pCfgMgr->GetEquipmentEnhanceLevel(wNewEquipmentQuality, wNewEquipmentEnhanceLevel);
		if (!pNewEquipmentEnhanceCfg)
			break;

		// ���ʣ��ľ��鲻�㹻����,������ȡ��,��һ��
		if (nTotalExp < pNewEquipmentEnhanceCfg->nCostGold)
		{
			if (nTotalExp)
				wNewEquipmentEnhanceLevel++;

			break;
		}

		wNewEquipmentEnhanceLevel++;
		nTotalExp -= pNewEquipmentEnhanceCfg->nCostGold;
	}

	return wNewEquipmentEnhanceLevel;
}

// ȡ�¸�Ʒ�ʵľ����ȼ�
WORD CEquipmentManager::GetNextQualityRefineLevel(WORD wCurQuality, WORD wCurRefineLevel, DWORD &dwCurExp)
{
	// ȡ��������
	const EquipmentRefineLevel_Config *pRefineCfg = g_pCfgMgr->GetEquipmentRefineLevel(wCurQuality, wCurRefineLevel);
	if (!pRefineCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ����װ����������[Quality:%d,EnhanceLevel:%d]", __FUNCTION__, wCurQuality, wCurRefineLevel);
		return 0;
	}

	// �ѵ�ǰʣ�ྭ�����ǰ�ȼ���Ӧ���ܾ������
	dwCurExp += pRefineCfg->dwTotalExp;

	WORD wNewEquipmentQuality = wCurQuality + 1;
	WORD wNewEquipmentRefineLevel = 0;

	const EquipmentRefineLevel_Config *pNewEquipmentRefineCfg;
	while (true)
	{
		pNewEquipmentRefineCfg = g_pCfgMgr->GetEquipmentRefineLevel(wNewEquipmentQuality, wNewEquipmentRefineLevel);
		if (!pNewEquipmentRefineCfg)
			break;

		// ���ʣ��ľ��鲻�㹻����
		if (dwCurExp < pNewEquipmentRefineCfg->dwExp)
			break;

		wNewEquipmentRefineLevel++;
		dwCurExp -= pNewEquipmentRefineCfg->dwExp;
	}

	return wNewEquipmentRefineLevel;
}

// ����ǿ��
bool CEquipmentManager::DoEnhance(Equipment *pEquipment, bool &bIsCrit, int &nCostGold)
{
	const Equipment_Config *pEquipCfg = CConfigManager::Instance()->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ��������[InfoId:%d]",__FUNCTION__,pEquipment->dwInfoId);
		return false;
	}

	// ���װ��ǿ���ȼ�����(װ���ȼ���������)
// 	if (pEquipment->wStrengthLevel >= pEquipCfg->wStrengthLevelMax)
// 	{
// 		return false;
// 	}

	// ���װ��ǿ���ȼ�����(��ɫ�ȼ�����)
	if (pEquipment->wEnhanceLevel >= m_pPlayer->GetLevel() * 2)
	{
		MAPLOG(GUI_LEVEL, "[%s]��ɫ�ȼ����㣬װ��ǿ��ʧ��![uid:%d,InfoId:%d]", __FUNCTION__, pEquipment->dwId, pEquipment->dwInfoId);
		return false;
	}

	const EquipmentEnhanceLevel_Config *pStrengLevelCfg =
		CConfigManager::Instance()->GetEquipmentEnhanceLevel(pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
	if (!pStrengLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ��ǿ������[InfoId:%d, Quality:%d, StrengthenLevel:%d]",
			__FUNCTION__, pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
		return false;
	}

	// ���ǿ������
	if (pStrengLevelCfg->nCostGold > m_pPlayer->GetProperty(ROLE_PRO_GOLD))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ǯ���㹻��װ��ǿ��ʧ��![InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
		return false;
	}

	if (g_Rand() < m_pPlayer->GetVipValue(VIP_EQUIP_STRENGTHEN_CRIT_RATE))
	{
		pEquipment->wEnhanceLevel += 2;
		bIsCrit = true;
	}
	else
	{
		pEquipment->wEnhanceLevel += 1;
		bIsCrit = false;
	}

	// ��������
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pStrengLevelCfg->nCostGold, FALSE, 0, FALSE);

	// ��¼ǿ������
	pEquipment->nEnhanceCostGold += pStrengLevelCfg->nCostGold;

	nCostGold += pStrengLevelCfg->nCostGold;

	return true;
}

Equipment* CEquipmentManager::FindEquipment(DWORD dwUID)
{
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		if (m_pEquipmentInfo->sEquipmentList[i].dwId == dwUID)
		{
			return &m_pEquipmentInfo->sEquipmentList[i];
		}
	}

	return NULL;
}

// ȡһ���µ�ϵ��id
DWORD CEquipmentManager::GetNewSeriesId()
{
	if (!m_dwSeriesId)
	{
		// ȡĿǰ����ID
		for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
		{
			if (m_pEquipmentInfo->sEquipmentList[i].dwId > m_dwSeriesId)
				m_dwSeriesId = m_pEquipmentInfo->sEquipmentList[i].dwId;
		}
		++m_dwSeriesId;
	}

	return m_dwSeriesId++;
}