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

// 装备强化 
void CEquipmentManager::OnEnhance(BYTE byStrengType, DWORD dwEquipmentUID)
{
	// 检测是否有此装备
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_STRENG_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的装备数据[UID:%d]",__FUNCTION__,dwEquipmentUID);
		return;
	}

// 	MAPLOG(ERROR_LEVEL, "[%s]强化装备信息[UID:%d,InfoId:%d, EnhanceLevel:%d]", __FUNCTION__, 
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

	// 强化失败
	if (!bStrengSuccess)
	{
		m_pPlayer->SendErrorMsg(ERROR_EQUIPMENT_STRENGTH_FAIL);
	}
	else
	{
		// 同步金币(因为在强化的只是扣除,木有通知客户端)
		m_pPlayer->NotifyProperty(ROLE_PRO_GOLD, m_pPlayer->GetProperty(ROLE_PRO_GOLD));

		// 存储货币日志
		m_pPlayer->SaveCurrencyLog(ROLE_PRO_GOLD, -nCostGold, FROM_EQUIP_ENHANCE);

		// 更新战力
		m_pPlayer->UpdateFightingPower();

		EquipmentUpdateNotify(dwEquipmentUID);

		MSG_S2C_EQUIPMENT_STRENG_RESP msg;
		msg.m_byType = byRealEnhanceTimes;
		msg.m_dwUID = dwEquipmentUID;
		msg.m_byCritTimes = byCritTimes;
		msg.m_wStrengthenLevel = pEquipment->wEnhanceLevel;
		m_pPlayer->SendPacket(&msg);

		// 成就
		m_pPlayer->OnUpdateAchievement(17, GetEquipmentEnhanceMinLevel(), false);

		// 日常任务(装备强化)
		m_pPlayer->OnUpdateDailyMission(21);

		// 7天活动(装备强化)
		m_pPlayer->OnUpdateSevenDayTarget(5);
	}
}

// 一键强化
void CEquipmentManager::OnNetOneKeyEnhance(BYTE bySlot)
{
	// 检测一键强化开启条件(VIP等级或者角色等级)
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < g_GetConfigValue(116) && m_pPlayer->GetLevel() < g_GetConfigValue(117))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]条件不足,不能使用一键强化", __FUNCTION__);
		return;
	}

	// 检测槽位参数合法性
	if (bySlot >= MAX_BATTLE_HERO_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]阵型槽位参数越界[Slot:%d]",__FUNCTION__, bySlot);
		return;
	}

	Formation *pSlotData = &m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[bySlot];

	// 检测是否有上阵英雄
	if (!pSlotData->dwHeroUID)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]此阵型槽位上木有英雄[Slot:%d]", __FUNCTION__, bySlot);
		return;
	}

	// 强化之前等级
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
		// 选出合适的装备来强化(等级最低的优先)
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
				MAPLOG(ERROR_LEVEL, "[%s]找不到装备的配置[InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
				return;
			}

			const EquipmentEnhanceLevel_Config *pStrengLevelCfg = 
				g_pCfgMgr->GetEquipmentEnhanceLevel(pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
			if (!pStrengLevelCfg)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
				MAPLOG(ERROR_LEVEL, "[%s]找不到装备强化配置[InfoId:%d, Quality:%d, StrengthenLevel:%d]",
					__FUNCTION__, pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
				return;
			}

			// 检测装备强化等级限制(角色等级限制) 检测强化消耗
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

		// 如果木有可强化的装备,则退出
		if (!pEnhanceEquipment)
			break;

		// 处理强化
		bool bIsCrit = false;
		if (!DoEnhance(pEnhanceEquipment, bIsCrit, nCostGold))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]处理装备强化失败[UID:%d,InfoId:%d]", __FUNCTION__,
				pEnhanceEquipment->dwId, pEnhanceEquipment->dwInfoId);
			return;
		}
	}

	if (!nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]木有可强化的装备[slot:%d]", __FUNCTION__, bySlot);
		return;
	}

	// 同步金币(因为在强化的只是扣除,木有通知客户端)
	m_pPlayer->NotifyProperty(ROLE_PRO_GOLD, m_pPlayer->GetProperty(ROLE_PRO_GOLD));

	// 存储货币日志
	m_pPlayer->SaveCurrencyLog(ROLE_PRO_GOLD, -nCostGold, FROM_EQUIP_ENHANCE);

	// 记录强化之后等级
	WORD arNewEnhanceLevel[EQUIPMENT_TYPE_MAX] = { 0 };
	for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwEquipmentId = pSlotData->arEquipmentList[i];
		Equipment *pEquipment = dwEquipmentId ? FindEquipment(dwEquipmentId) : NULL;
		if (pEquipment)
			arNewEnhanceLevel[i] = pEquipment->wEnhanceLevel;
	}

	// 同步数据
	for (WORD i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		if (pSlotData->arEquipmentList[i])
			EquipmentUpdateNotify(pSlotData->arEquipmentList[i]);
	}

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 成就
	m_pPlayer->OnUpdateAchievement(17, GetEquipmentEnhanceMinLevel(), false);

	// 日常任务(装备强化)
	m_pPlayer->OnUpdateDailyMission(21);

	// 7天活动(装备强化)
	m_pPlayer->OnUpdateSevenDayTarget(5);

	// 响应
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

// 装备精炼
void CEquipmentManager::OnRefineEx(DWORD dwEquipmentUID, WORD wItemId, WORD wItemNum)
{
	// 检测是否有此装备
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的装备数据[UID:%d]",__FUNCTION__, dwEquipmentUID);
		return;
	}

	const Equipment_Config *pEquipCfg = CConfigManager::Instance()->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "找不到装备的配置数据[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	const Server_Config *pCommonCfg = CConfigManager::Instance()->GetCommonCfg(17);
	if (!pCommonCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "找不到通用配置[Key:%d]", 17);
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
		MAPLOG(ERROR_LEVEL, "[%s]不是装备精练的消耗物品[EquipentInfoId:%d,ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pEquipment->dwInfoId, wItemId, wItemNum);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	if (pItemMgr->GetItemNum(wItemId) < wItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ); 
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此精练物品[EquipentInfoId:%d,CostItemId:%d,CostItemNum:%d,HaveItemNum:%d]!",
			__FUNCTION__, pEquipment->dwInfoId, wItemId, wItemNum, pItemMgr->GetItemNum(wItemId));
		return;
	}

	// 检测是否有此物品
	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(wItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "找不到物品的配置数据[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// 检测精炼等级是否到达上限
	if (pEquipment->wRefineLevel >= pEquipCfg->wRefineLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]装备精炼等级已达上限![UID:%d]",__FUNCTION__, dwEquipmentUID);
		return;
	}

	// 检测角色等级限制
// 	if (pEquipment->wRefineLevel >= m_pPlayer->GetLevel())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[name:%s]装备精炼等级受角色等级限制![UID:%d]",
// 			m_pPlayer->GetUTF8RoleName(), dwEquipmentUID);
// 		return;
// 	}

	// 添加经验
	pEquipment->dwRefineExp += (pItemCfg->nParam1 * wItemNum);

	WORD wOldRefineLv = pEquipment->wRefineLevel;

	// 处理升级逻辑
	while (true)
	{
		const EquipmentRefineLevel_Config *pRefineLevelCfg =
			CConfigManager::Instance()->GetEquipmentRefineLevel(pEquipCfg->wQuality, pEquipment->wRefineLevel);
		if (!pRefineLevelCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_REFINE_REQ);
			MAPLOG(ERROR_LEVEL, "找不到装备精练等级的配置数据[InfoId:%d, Quality:%d, RefineLv:%d]", 
				pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wRefineLevel);
			break;
		}

		// 经验不足够升级
		if (pEquipment->dwRefineExp < pRefineLevelCfg->dwExp)
			break;

		pEquipment->wRefineLevel++;
		pEquipment->dwRefineExp -= pRefineLevelCfg->dwExp;
	}

	// 扣除物品
	pItemMgr->RemoveItem(wItemId, wItemNum, TRUE, FROM_EQUIPMENT_REFINE);

	AddRefineCostItem(dwEquipmentUID, wItemId, wItemNum);

	EquipmentUpdateNotify(dwEquipmentUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_EQUIPMENT_REFINE_RESP msg;
	msg.m_dwEquipmentUID = dwEquipmentUID;
	msg.m_byOldRefineLevel = wOldRefineLv;
	msg.m_byNewRefineLevel = pEquipment->wRefineLevel;
	msg.m_dwRefineExp = pEquipment->dwRefineExp;
	msg.m_dwAddRefineExp = pItemCfg->nParam1 * wItemNum;
	m_pPlayer->SendPacket(&msg);

	// 7天活动事件(装备精炼)
	m_pPlayer->OnUpdateSevenDayTarget(14);

	// 处理广播
	if (pEquipment->wRefineLevel >= g_GetConfigValue(111) && pEquipment->wRefineLevel > wOldRefineLv)
	{
		CSystemNotice sys_notice(7, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(pEquipment->dwInfoId);
		sys_notice.AddParam(pEquipment->wRefineLevel);
		sys_notice.SendMsg();
	}
}

// 装备精炼
void CEquipmentManager::OnRefine(DWORD dwEquipmentUID, EquipmentRefineCostData *pCostList, WORD wCostListNum)
{
	WORD wFromId = C2S_EQUIPMENT_REFINE_REQ;
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 检测是否有此装备
	Equipment *pEquipment = FindEquipment(dwEquipmentUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的装备数据[UID:%d]", __FUNCTION__, dwEquipmentUID);
		return;
	}

	const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "找不到装备的配置数据[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// 检测精炼等级是否到达上限
	if (pEquipment->wRefineLevel >= pEquipCfg->wRefineLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "[%s]装备精炼等级已达上限![UID:%d]", __FUNCTION__, dwEquipmentUID);
		return;
	}

	const Server_Config *pCommonCfg = g_pCfgMgr->GetCommonCfg(17);
	if (!pCommonCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
		MAPLOG(ERROR_LEVEL, "找不到通用配置[Key:%d]", 17);
		return;
	}

	DWORD dwTotalRefineExp = 0;
	for (WORD i = 0; i < wCostListNum; ++i)
	{
		// 检测是否精炼消耗的物品
		EquipmentRefineCostData &tCostData = pCostList[i];
		if (!pCommonCfg->IsInList(tCostData.dwItemId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "[%s]不是装备精练的消耗物品[EquipentInfoId:%d,ItemId:%d,ItemNum:%d]",
				__FUNCTION__, pEquipment->dwInfoId, tCostData.dwItemId, tCostData.wItemNum);
		}

		if (pItemMgr->GetItemNum(tCostData.dwItemId) < tCostData.wItemNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此精练物品[EquipentInfoId:%d,CostItemId:%d,CostItemNum:%d,HaveItemNum:%d]!",
				__FUNCTION__, pEquipment->dwInfoId, tCostData.dwItemId, tCostData.wItemNum, pItemMgr->GetItemNum(tCostData.dwItemId));
			return;
		}

		// 检测是否有此物品
		const Item_Config *pItemCfg = g_pCfgMgr->GetItem(tCostData.dwItemId);
		if (!pItemCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "找不到物品的配置数据[InfoId:%d]", pEquipment->dwInfoId);
			return;
		}

		// 添加经验
		dwTotalRefineExp += (pItemCfg->nParam1 * tCostData.wItemNum);
	}

	pEquipment->dwRefineExp += dwTotalRefineExp;

	WORD wOldRefineLv = pEquipment->wRefineLevel;

	// 处理升级逻辑
	const EquipmentRefineLevel_Config *pRefineLevelCfg = NULL;
	while (true)
	{
		pRefineLevelCfg = g_pCfgMgr->GetEquipmentRefineLevel(pEquipCfg->wQuality, pEquipment->wRefineLevel);
		if (!pRefineLevelCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromId);
			MAPLOG(ERROR_LEVEL, "找不到装备精练等级的配置数据[InfoId:%d, Quality:%d, RefineLv:%d]",
				pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wRefineLevel);
			break;
		}

		// 经验不足够升级
		if (pEquipment->dwRefineExp < pRefineLevelCfg->dwExp)
			break;

		pEquipment->wRefineLevel++;
		pEquipment->dwRefineExp -= pRefineLevelCfg->dwExp;
	}

	for (WORD i = 0; i < wCostListNum; ++i)
	{
		EquipmentRefineCostData &tCostData = pCostList[i];

		// 扣除物品
		pItemMgr->RemoveItem(tCostData.dwItemId, tCostData.wItemNum, TRUE, FROM_EQUIPMENT_REFINE);

		// 记录消耗的物品
		AddRefineCostItem(dwEquipmentUID, tCostData.dwItemId, tCostData.wItemNum);
	}

	EquipmentUpdateNotify(dwEquipmentUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_EQUIPMENT_REFINE_RESP msg;
	msg.m_dwEquipmentUID = dwEquipmentUID;
	msg.m_byOldRefineLevel = wOldRefineLv;
	msg.m_byNewRefineLevel = pEquipment->wRefineLevel;
	msg.m_dwRefineExp = pEquipment->dwRefineExp;
	msg.m_dwAddRefineExp = dwTotalRefineExp;
	m_pPlayer->SendPacket(&msg);

	// 7天活动事件(装备精炼)
	m_pPlayer->OnUpdateSevenDayTarget(14);

	// 处理广播
	if (pEquipment->wRefineLevel >= g_GetConfigValue(111) && pEquipment->wRefineLevel > wOldRefineLv)
	{
		CSystemNotice sys_notice(7, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(pEquipment->dwInfoId);
		sys_notice.AddParam(pEquipment->wRefineLevel);
		sys_notice.SendMsg();
	}

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 35, wCostListNum, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 35, wCostListNum, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 35, wCostListNum, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 35, wCostListNum, true);
}

// 装备合成
void CEquipmentManager::OnCompose(DWORD dwEquipmentInfoId)
{
	const EquipmentCompose_Config *pComposeCfg = CConfigManager::Instance()->GetEquipmentCompose(dwEquipmentInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_COMPOSE_REQ);
		MAPLOG(ERROR_LEVEL, "找不到装备合成配置数据[InfoId:%d]", dwEquipmentInfoId);
		return;
	}

	// 检测身上是否有足够的物品
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	if (pItemMgr->GetItemNum(pComposeCfg->dwFragmentId) < pComposeCfg->wFragmentNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_COMPOSE_REQ);
		MAPLOG(ERROR_LEVEL, "[name:%s]玩家物品数量[ItemID:%d,Num:%d]不足!",
			m_pPlayer->GetUTF8RoleName(), pComposeCfg->dwFragmentId, pItemMgr->GetItemNum(pComposeCfg->dwFragmentId));
		return;
	}

	// 检测装备背包是否已满
	if (!GetBagEmptySize())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_COMPOSE_REQ);
		MAPLOG(ERROR_LEVEL, "[name:%s]玩家装备包裹空间不足!",m_pPlayer->GetUTF8RoleName());
		return;
	}

	// 添加装备
	AddEquipent(dwEquipmentInfoId, TRUE, FROM_EQUIPMENT_COMPOSE);

	// 消耗物品
	pItemMgr->RemoveItem(pComposeCfg->dwFragmentId, pComposeCfg->wFragmentNum, TRUE, FROM_EQUIPMENT_COMPOSE);

	MSG_S2C_EQUIPMENT_COMPOSE_RESP msg;
	msg.m_dwInfoId = dwEquipmentInfoId;
	m_pPlayer->SendPacket(&msg);
}

// 请求装备列表
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

// 		MAPLOG(ERROR_LEVEL, "装备信息[uid:%d, infoid:%d, EnhanceLevel:%d]", 
// 			sSrcData.dwId, sSrcData.dwInfoId, sSrcData.wEnhanceLevel);
	}
	m_pPlayer->SendPacket(&msg);
}

// 请求装备详情
void CEquipmentManager::OnEquipmentInfo(DWORD dwUID)
{
	const Equipment *pEquipment = GetEquipment(dwUID);
	MSG_S2C_EQUIPMENT_INFO_RESP msg;
	msg.m_Equipment = *pEquipment;
	m_pPlayer->SendPacket(&msg);
}

// 请求装备
void CEquipmentManager::OnEquipmentPutOn(BYTE byFormationPos, BYTE byEquipmentPos, DWORD dwUID)
{
	// 检测参数 (阵型位置, 装备位置)
	if (!byFormationPos || byFormationPos > MAX_BATTLE_HERO_NUM || byEquipmentPos >= EQUIPMENT_TYPE_MAX)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_PUT_ON_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]请求装备穿戴参数出错[formation_pos:%d, equipment_pos:%d]", 
			__FUNCTION__, byFormationPos, byEquipmentPos);
		return;
	}

	BYTE bySvrFormationPos = byFormationPos - 1;
	BYTE bySvrEquipmentPos = byEquipmentPos;


	// 检测是否有此装备
	if (dwUID)
	{
		const Equipment *pEquipment = GetEquipment(dwUID);
		if (!pEquipment)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_EQUIPMENT_PUT_ON_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此装备[UID:%d]", __FUNCTION__, dwUID);
			return;
		}
	}

	// todo::检测此装备是否可以穿戴到此位置

	Formation &sFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[bySvrFormationPos];
	DWORD dwOldEquipmentUID = sFormation.arEquipmentList[bySvrEquipmentPos];
	sFormation.arEquipmentList[bySvrEquipmentPos] = dwUID;

	if (dwOldEquipmentUID)
		EquipmentUpdateNotify(dwOldEquipmentUID);
	EquipmentUpdateNotify(dwUID);

	// 从新同步阵型列表
	m_pPlayer->GetHeroManager()->SendFormationList();

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_EQUIPMENT_PUT_ON_RESP msg;
	msg.m_byFormationPos = byFormationPos;
	msg.m_byEquipmentPos = byEquipmentPos;
	msg.m_dwUID = dwUID;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(18, GetEquipmentLowQuality(), false);

	// 7天活动事件(装备精炼)
	m_pPlayer->OnUpdateSevenDayTarget(13);
}

// 升星
void CEquipmentManager::OnNetStarUP(DWORD dwUID)
{
	// 取装备数据
	Equipment *pEquipment = FindEquipment(dwUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的装备数据[UID:%d]", __FUNCTION__, dwUID);
		return;
	}

	// 取装备配置
	const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "找不到装备的配置数据[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// 检测此装备是否可升星
	if (!pEquipCfg->wStarMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "此装备不能升星[InfoId:%d]", pEquipment->dwInfoId);
		return;
	}

	// 检测装备是否已达升星上限
	if (pEquipment->wStar >= pEquipCfg->wStarMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "装备已在达升星上限[InfoId:%d,Star:%d]", pEquipment->dwInfoId, pEquipment->wStar);
		return;
	}

	// 取升星数配置
	const EquipmentStarUp_Config *pStarUPCfg = g_pCfgMgr->GetEquipmentStartUp(pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel);
	if (!pStarUPCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "找不到装备升星配置[type:%d,quality:%d,Star:%d,level:%d]", 
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel);
		return;
	}

	// 检测消耗(金币)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < pStarUPCfg->nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "装备升星消耗金币不足[type:%d,quality:%d,Star:%d,level:%d,CostGold:%d]",
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel, pStarUPCfg->nCostGold);
		return;
	}

	// 检测消耗(道具)
	if (m_pPlayer->GetItemNum(pStarUPCfg->tCostItem.dwId) < pStarUPCfg->tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "装备升星消耗道具不足[type:%d,quality:%d,Star:%d,level:%d,CostItemId:%d,CostItemNum:%d]",
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel, pStarUPCfg->tCostItem.dwId, pStarUPCfg->tCostItem.dwNum);
	}

	// 检测消耗(碎片)
	if (m_pPlayer->GetItemNum(pEquipCfg->dwStarUpCostItemId) < pStarUPCfg->dwCostSelfNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "装备升星消耗碎片不足[type:%d,quality:%d,Star:%d,level:%d,CostSelfId:%d,CostSelfNum:%d]",
			pEquipCfg->byType, pEquipCfg->wQuality, pEquipment->wStar, pEquipment->wStarLevel, pEquipCfg->dwStarUpCostItemId, pStarUPCfg->dwCostSelfNum);
	}

	// 处理消耗(金币)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pStarUPCfg->nCostGold, TRUE, FROM_EQUIPMENT_STAR_UP);

	// 处理消耗(道具)
	m_pPlayer->RemoveItem(pStarUPCfg->tCostItem.dwId, pStarUPCfg->tCostItem.dwNum, FROM_EQUIPMENT_STAR_UP);

	// 处理消耗(碎片)
	m_pPlayer->RemoveItem(pEquipCfg->dwStarUpCostItemId, pStarUPCfg->dwCostSelfNum, FROM_EQUIPMENT_STAR_UP);

	// 检测是否升星
	if (pStarUPCfg->bIsStarUp)
	{
		pEquipment->wStar++;
		pEquipment->wStarLevel = 0;
	}
	else
	{
		pEquipment->wStarLevel++;
	}
	
	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 同步装备数据
	EquipmentUpdateNotify(dwUID);

	MSG_S2C_EQUIPMENT_STAR_UP_RESP msg;
	msg.m_byCurStar = pEquipment->wStar;
	msg.m_byCurStarLevel = pEquipment->wStarLevel;
	msg.m_dwUID = dwUID;
	m_pPlayer->SendPacket(&msg);
}

// 升品
void CEquipmentManager::OnNetQualityUP(DWORD dwUID)
{
	// 取装备数据
	Equipment *pEquipment = FindEquipment(dwUID);
	if (!pEquipment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的装备数据[UID:%d]", __FUNCTION__, dwUID);
		return;
	}

	// 取装备配置
	const Equipment_Config *pEquipCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备的配置数据[InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
		return;
	}

	// 检测是否可以升品
	if (!pEquipCfg->wNextQualityEquipmentId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]此装备不能升品[InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
		return;
	}

	// 取升级品质
	const EquipmentQualityUp_Config *pQualityUpCfg = g_pCfgMgr->GetEquipmentQualityUp(pEquipCfg->wQuality);
	if (!pQualityUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备的升品配置数据[Quality:%d]", __FUNCTION__, pEquipCfg->wQuality);
		return;
	}

	// 检测消耗(道具)
	if (m_pPlayer->GetItemNum(pQualityUpCfg->tCostItem.dwId) < pQualityUpCfg->tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]装备升品消耗道具不足[Quality:%d,CostItemId:%d,CostItemNum:%d]", 
			__FUNCTION__, pEquipCfg->wQuality, pQualityUpCfg->tCostItem.dwId, pQualityUpCfg->tCostItem.dwNum);
		return;
	}

	// 检测消耗(货币)
	const CostData &tCostCurrency = pQualityUpCfg->tCostCurrency;
	if (m_pPlayer->GetProperty(tCostCurrency.wType) < tCostCurrency.nValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]装备升品消耗货币不足[Quality:%d,CostCurrencyType:%d,CostCurrencyValue:%d]",
			__FUNCTION__, pEquipCfg->wQuality, tCostCurrency.wType, tCostCurrency.nValue);
		return;
	}

	// 处理消耗(道具)
	m_pPlayer->RemoveItem(pQualityUpCfg->tCostItem, FROM_EQUIPMENT_QUALITY_UP);

	// 处理消耗(货币)
	m_pPlayer->ModifyProperty(tCostCurrency.wType, -tCostCurrency.nValue, TRUE, FROM_EQUIPMENT_QUALITY_UP);

	// 变为另一个装备
	pEquipment->dwInfoId = pEquipCfg->wNextQualityEquipmentId;

	// 星数及星级重置(todo::目前是这样的逻辑,策划大大说日后有可能会调整)
	pEquipment->wStar = 0;
	pEquipment->wStarLevel = 0;

	// 装备强化等级转化：按照当前装备经验重新计算目标红装的经验
	pEquipment->wEnhanceLevel = GetNextQualityEnhanceLevel(pEquipCfg->wQuality, pEquipment->wEnhanceLevel);

	// 装备精炼等级转化：按照当前装备精炼等级的精炼值和身上的精炼值重新计算目标红装的精炼等级
	pEquipment->wRefineLevel = GetNextQualityRefineLevel(pEquipCfg->wQuality, pEquipment->wRefineLevel, pEquipment->dwRefineExp);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 重新同步更新后的装备数据
	EquipmentUpdateNotify(dwUID);

	MSG_S2C_EQUIPMENT_QUALITY_UP_RESP msg;
	msg.m_dwUID = dwUID;
	m_pPlayer->SendPacket(&msg);
}

// 添加装备
bool CEquipmentManager::AddEquipent(DWORD dwEquipmentInfoId, bool bNotify, WORD wFromId)
{
	const Equipment_Config *pEquipCfg = CConfigManager::Instance()->GetEquipment(dwEquipmentInfoId);
	if (!pEquipCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到装备配置数据[InfoId:%d]", dwEquipmentInfoId);
		return false;
	}

	Equipment sEquipment;
	memset(&sEquipment, 0, sizeof(sEquipment));
	sEquipment.dwId = GetNewSeriesId();
	sEquipment.dwInfoId = dwEquipmentInfoId;
	sEquipment.wEnhanceLevel = 1;
	return AddEquipment(sEquipment, bNotify, wFromId);
}

// 添加装备
bool CEquipmentManager::AddEquipment(const Equipment &sEquipment, bool bNotify, WORD wFromId)
{
	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]装备背包已满[num:%d]", __FUNCTION__, m_pEquipmentInfo->wEquipmentNum);

		// 记录装备丢失记录
		m_pPlayer->SaveItemLoseRecord(sEquipment.dwInfoId);

		return false;
	}

	m_pEquipmentInfo->sEquipmentList[m_pEquipmentInfo->wEquipmentNum++] = sEquipment;

	if (bNotify)
	{
		EquipmentUpdateNotify(sEquipment.dwId);
	}

	// 记录获得
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), sEquipment.dwInfoId, 1, true, wFromId);

	return true;
}

// 删除装备
bool CEquipmentManager::RemoveEquipment(DWORD dwEquipmentUID, bool bNotify, WORD wFromId)
{
	for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
	{
		Equipment &tFind = m_pEquipmentInfo->sEquipmentList[i];
		if (tFind.dwId == dwEquipmentUID)
		{
			// 记录获得
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tFind.dwInfoId, 1, false, wFromId);

			if (bNotify)
				EquipmentUpdateNotify(dwEquipmentUID, false);
			tFind = m_pEquipmentInfo->sEquipmentList[--m_pEquipmentInfo->wEquipmentNum];
			return true;
		}
	}

	return false;
}

// 取装备背包空闲数量
WORD CEquipmentManager::GetBagEmptySize()
{
	//return MAX_EQUIPMENT_NUM - m_pEquipmentInfo->wEquipmentNum;

	WORD wBagSize = min(MAX_EQUIPMENT_NUM, m_pPlayer->GetVipValue(VIP_EQUIPMENT_BAG_NUM));
	return wBagSize > m_pEquipmentInfo->wEquipmentNum ? wBagSize - m_pEquipmentInfo->wEquipmentNum : 0;
}

// 添加精炼消耗物品 
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

// 取装备数据
const Equipment* CEquipmentManager::GetEquipment(DWORD dwUID)
{
	return FindEquipment(dwUID);
}

// 装备更新通知
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

// 取到达指定强化等级的装备数量
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

// 取到达指定精炼等级的装备数量
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

// 取到达指定品质的装备数量
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

// 取强化最小等级(已装备的)
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

// 取装备最低品质(已装备的) 
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

// 取下个品质的强化等级
WORD CEquipmentManager::GetNextQualityEnhanceLevel(WORD wCurQuality, WORD wCurEnhanceLevel)
{
	// 取强化配置
	const EquipmentEnhanceLevel_Config *pEnhanceCfg = g_pCfgMgr->GetEquipmentEnhanceLevel(wCurQuality, wCurEnhanceLevel);
	if (!pEnhanceCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到装备强化配置[Quality:%d,EnhanceLevel:%d]", __FUNCTION__, wCurQuality, wCurEnhanceLevel);
		return 0;
	}

	// 装备强化获得的经验跟消耗的金币相同
	int nTotalExp = pEnhanceCfg->nTotalCostGold;

	WORD wNewEquipmentQuality = wCurQuality + 1;
	WORD wNewEquipmentEnhanceLevel = 1;

	const EquipmentEnhanceLevel_Config *pNewEquipmentEnhanceCfg;
	while (true)
	{
		pNewEquipmentEnhanceCfg = g_pCfgMgr->GetEquipmentEnhanceLevel(wNewEquipmentQuality, wNewEquipmentEnhanceLevel);
		if (!pNewEquipmentEnhanceCfg)
			break;

		// 如果剩余的经验不足够升级,则向上取整,升一级
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

// 取下个品质的精炼等级
WORD CEquipmentManager::GetNextQualityRefineLevel(WORD wCurQuality, WORD wCurRefineLevel, DWORD &dwCurExp)
{
	// 取精炼配置
	const EquipmentRefineLevel_Config *pRefineCfg = g_pCfgMgr->GetEquipmentRefineLevel(wCurQuality, wCurRefineLevel);
	if (!pRefineCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到装备精炼配置[Quality:%d,EnhanceLevel:%d]", __FUNCTION__, wCurQuality, wCurRefineLevel);
		return 0;
	}

	// 把当前剩余经验跟当前等级对应的总经验相加
	dwCurExp += pRefineCfg->dwTotalExp;

	WORD wNewEquipmentQuality = wCurQuality + 1;
	WORD wNewEquipmentRefineLevel = 0;

	const EquipmentRefineLevel_Config *pNewEquipmentRefineCfg;
	while (true)
	{
		pNewEquipmentRefineCfg = g_pCfgMgr->GetEquipmentRefineLevel(wNewEquipmentQuality, wNewEquipmentRefineLevel);
		if (!pNewEquipmentRefineCfg)
			break;

		// 如果剩余的经验不足够升级
		if (dwCurExp < pNewEquipmentRefineCfg->dwExp)
			break;

		wNewEquipmentRefineLevel++;
		dwCurExp -= pNewEquipmentRefineCfg->dwExp;
	}

	return wNewEquipmentRefineLevel;
}

// 处理强化
bool CEquipmentManager::DoEnhance(Equipment *pEquipment, bool &bIsCrit, int &nCostGold)
{
	const Equipment_Config *pEquipCfg = CConfigManager::Instance()->GetEquipment(pEquipment->dwInfoId);
	if (!pEquipCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备的配置[InfoId:%d]",__FUNCTION__,pEquipment->dwInfoId);
		return false;
	}

	// 检测装备强化等级限制(装备等级上限限制)
// 	if (pEquipment->wStrengthLevel >= pEquipCfg->wStrengthLevelMax)
// 	{
// 		return false;
// 	}

	// 检测装备强化等级限制(角色等级限制)
	if (pEquipment->wEnhanceLevel >= m_pPlayer->GetLevel() * 2)
	{
		MAPLOG(GUI_LEVEL, "[%s]角色等级不足，装备强化失败![uid:%d,InfoId:%d]", __FUNCTION__, pEquipment->dwId, pEquipment->dwInfoId);
		return false;
	}

	const EquipmentEnhanceLevel_Config *pStrengLevelCfg =
		CConfigManager::Instance()->GetEquipmentEnhanceLevel(pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
	if (!pStrengLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备强化配置[InfoId:%d, Quality:%d, StrengthenLevel:%d]",
			__FUNCTION__, pEquipment->dwInfoId, pEquipCfg->wQuality, pEquipment->wEnhanceLevel);
		return false;
	}

	// 检测强化消耗
	if (pStrengLevelCfg->nCostGold > m_pPlayer->GetProperty(ROLE_PRO_GOLD))
	{
		MAPLOG(ERROR_LEVEL, "[%s]金钱不足够，装备强化失败![InfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
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

	// 处理消耗
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pStrengLevelCfg->nCostGold, FALSE, 0, FALSE);

	// 记录强化消耗
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

// 取一个新的系列id
DWORD CEquipmentManager::GetNewSeriesId()
{
	if (!m_dwSeriesId)
	{
		// 取目前最大的ID
		for (WORD i = 0; i < m_pEquipmentInfo->wEquipmentNum; ++i)
		{
			if (m_pEquipmentInfo->sEquipmentList[i].dwId > m_dwSeriesId)
				m_dwSeriesId = m_pEquipmentInfo->sEquipmentList[i].dwId;
		}
		++m_dwSeriesId;
	}

	return m_dwSeriesId++;
}