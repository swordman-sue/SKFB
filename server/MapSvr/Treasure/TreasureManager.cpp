#include "stdafx.h"
#include "TreasureManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"


CTreasureManager::CTreasureManager()
{
	m_dwSeriesId = 1;
}

CTreasureManager::~CTreasureManager()
{
		
}

void CTreasureManager::Init(Player *pPlayer)
{
	m_dwSeriesId = 1;
	m_pPlayer = pPlayer;
	m_pTreasureInfo = &m_pPlayer->GetRoleFullInfo().tTreasureInfo;
}

// 宝物强化 
void CTreasureManager::OnEnhance(DWORD dwTreasureUID, DWORD *pCostTreasureList, WORD wCostTreasureNum)
{
	WORD wFromMsgId = C2S_TREASURE_STRENGTHEN_REQ;

	if (!wCostTreasureNum || !pCostTreasureList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]宝物强化参数非法[UID:%d][CostTreasureNum:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID, wCostTreasureNum);
		return;
	}

	// 检测消耗物品
	for (WORD i = 0; i < wCostTreasureNum; ++i)
 	{
		for (WORD k = i+1; k < wCostTreasureNum; ++k)
		{
			if (pCostTreasureList[i] == pCostTreasureList[k])
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[name:%s]发送重复的消耗物品[UID:%d]",
					m_pPlayer->GetUTF8RoleName(), dwTreasureUID);
				return;
			}
		}
 	}

	// 检测是否有此装备
	Treasure *pTreasure = FindTreasure(dwTreasureUID);
	if (!pTreasure)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]找不到玩家的宝物数据[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID);
		return;
	}

	//MAPLOG(ERROR_LEVEL, "请求强化的宝物[UID:%d, InfoId:%d]", dwTreasureUID, pTreasure->dwInfoId);

	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物配置[InfoId:%d]",__FUNCTION__, dwTreasureUID);
		return;
	}

	// 检测是否已满级
	if (pTreasure->wEnhanceLevel >= pTreasureCfg->wStrengthenLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]宝物强化等级已达上限[InfoId:%d,MaxLevel:%d]",
			__FUNCTION__, dwTreasureUID, pTreasure->wEnhanceLevel);
		return;
	}

	WORD wOldLevel = pTreasure->wEnhanceLevel;

	// 检测消耗物品
	int nCostGold = 0;
	//WORD wRemoveTreasureNum = 0;
	for (WORD i = 0; i < wCostTreasureNum; ++i)
	{
		if (!DoStrengthen(pTreasure, pTreasureCfg, pCostTreasureList[i], nCostGold))
			break;

		//wRemoveTreasureNum++;
	}

	TreasureUpdateNotify(*pTreasure);

	WORD wNewLevel = pTreasure->wEnhanceLevel;

	// 处理消耗
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold, TRUE, FROM_TREASURE_ENHANCE);
	for (WORD i = 0; i < wCostTreasureNum; ++i)
	{
		DWORD dwCostTreasureId = pCostTreasureList[i];
		Treasure *pCostTreasure = FindTreasure(dwCostTreasureId);
		if (pCostTreasure)
			MAPLOG(GUI_LEVEL, "请求强化消耗的宝物[UID:%d, InfoId:%d]", pCostTreasure->dwId, pCostTreasure->dwInfoId);
		else
			MAPLOG(ERROR_LEVEL, "[%s]请求消耗的物不存在[UID:%d]", __FUNCTION__, dwCostTreasureId);

		RemoveTreasure(dwCostTreasureId, FROM_TREASURE_ENHANCE);
	}

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_TREASURE_STRENGTHEN_RESP msg;
	msg.m_dwTreasureUID = dwTreasureUID;
	msg.m_wOldLevel = wOldLevel;
	msg.m_wNewLevel = wNewLevel;
	msg.m_dwExp = nCostGold;
	m_pPlayer->SendPacket(&msg);

	// 日常任务事件(todo::调整为次数,不是个数)
	m_pPlayer->OnUpdateDailyMission(2);

	// 成就
	m_pPlayer->OnUpdateAchievement(19, GetMinEnhanceLevelInWear(), false);

	// 世界等级任务
	//m_pPlayer->OnUpdateWorldTaskEvent(2);
}

// 宝物精炼
void CTreasureManager::OnRefine(DWORD dwTreasureUID)
{
	WORD wFromMsgId = C2S_TREASURE_REFINE_REQ;

	// 检测是否有此装备
	Treasure *pTreasure = FindTreasure(dwTreasureUID);
	if (!pTreasure)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]找不到玩家的宝物数据[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID);
		return;
	}

	// 取宝物配置
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "找不到宝物的配置数据[InfoId:%d]", pTreasure->dwInfoId);
		return;
	}

	// 检测精炼等级是否到达上限
	if (pTreasure->wRefineLevel >= pTreasureCfg->wRefineLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]宝物精炼等级已达上限![UID:%d,RefineLv:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID, pTreasure->wRefineLevel);
		return;
	}

	// 取宝物精练配置
	const TreasureRefine_Config *pRefineCfg = 
		CConfigManager::Instance()->GetTreasureRefine(pTreasureCfg->wQuality, pTreasure->wRefineLevel);
	if (!pRefineCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "找不到宝物精练的配置数据[Quality:%d,level:%d]", 
			pTreasureCfg->wQuality, pTreasure->wRefineLevel);
		return;
	}

	// 检测物品
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	if (pItemMgr->GetItemNum(pRefineCfg->dwCostItemId) < pRefineCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]道具不足,精练失败[CostItemId:%d, CostItemNum:%d]!", 
			m_pPlayer->GetUTF8RoleName(), pRefineCfg->dwCostItemId, pRefineCfg->wCostItemNum);
		return;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	// 检测宝物
	DWORD arCostTreasureList[MAX_TREASURE_NUM] = { 0 };
	BYTE byCostTreasureNum = 0;
	if (pRefineCfg->byCostTreasureNum)
	{
		for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
		{
			Treasure &tmp = m_pTreasureInfo->sTreasureList[i];
			// 强化及精炼等级还没升级过，没上阵的, 则可作为消耗品
			if (dwTreasureUID != tmp.dwId && tmp.dwInfoId == pTreasure->dwInfoId && 
				tmp.wEnhanceLevel <= 1 && !tmp.wRefineLevel && !pHeroMgr->GetTreasureWearHeroId(tmp.dwId))
			{
				arCostTreasureList[byCostTreasureNum++] = tmp.dwId;
			}

			// 数量已足够
			if (byCostTreasureNum >= pRefineCfg->byCostTreasureNum)
				break;
		}
	}

	// 检测消耗的宝物
	if (byCostTreasureNum < pRefineCfg->byCostTreasureNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "宝物精炼消耗的宝物数量不足[InfoId:%d,CurNum:%d]", pTreasure->dwInfoId, byCostTreasureNum);
		return;
	}

	// 检测金币
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < pRefineCfg->nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "宝物精炼消耗的金币不足[InfoId:%d]", pTreasure->dwInfoId);
		return;
	}

	// 升级
	pTreasure->wRefineLevel++;

	TreasureUpdateNotify(*pTreasure);

	WORD wNewRefineLevel = pTreasure->wRefineLevel;

	// 扣除物品,宝物,金币
	pItemMgr->RemoveItem(pRefineCfg->dwCostItemId, pRefineCfg->wCostItemNum, TRUE, FROM_TREASURE_REFINE);
	for (BYTE i = 0; i < pRefineCfg->byCostTreasureNum; ++i)
	{
		RemoveTreasure(arCostTreasureList[i], FROM_TREASURE_REFINE);
	}
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pRefineCfg->nCostGold, TRUE, FROM_TREASURE_REFINE);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_TREASURE_REFINE_RESP msg;
	msg.m_dwTreasureUID = dwTreasureUID;
	msg.m_byRefineLevel = wNewRefineLevel;
	m_pPlayer->SendPacket(&msg);
}

// 宝物合成
void CTreasureManager::OnComposeEx(DWORD dwTreasureInfoId)
{
	WORD wFromMsgId = C2S_TREASURE_COMPOSE_REQ;

	// 取合成宝物配置
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(dwTreasureInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物合成配置[InfoId:%d]", __FUNCTION__, dwTreasureInfoId);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 检测需要的物品
	vector<DWORD>::const_iterator iter = pComposeCfg->cost_item_list.begin();
	for (; iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		// 道具不足
		if (!pItemMgr->GetItem(*iter))
		{
			m_pPlayer->SendErrorMsg(ERROR_DOUBAO_COMPOSE_ITEM_NOT_ENOUGH);
			MAPLOG(ERROR_LEVEL, "[%s]宝物合成需要的道具不足[InfoId:%d]", __FUNCTION__, dwTreasureInfoId);
			return;
		}
	}

	// 检测包裹空间
	if (!GetBagEmptySize())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]宝物包裹空间不足", __FUNCTION__);
		return;
	}

	// 添加宝物
	AddTreasure(dwTreasureInfoId, TRUE, FROM_TREASURE_COMPOSE);

	// 处理消耗
	for (iter = pComposeCfg->cost_item_list.begin(); iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		pItemMgr->RemoveItem(*iter, 1, TRUE, FROM_TREASURE_COMPOSE);
	}

// 	MSG_S2C_TREASURE_COMPOSE_RESP msg;
// 	msg.m_wInfoId = dwTreasureInfoId;
// 	m_pPlayer->SendPacket(&msg);

	// 日常任务(圣器合成)
	m_pPlayer->OnUpdateDailyMission(22);
}

// 宝物合成
void CTreasureManager::OnCompose(DWORD dwTreasureInfoId, WORD wTreasureNum)
{
	WORD wFromMsgId = C2S_TREASURE_COMPOSE_REQ;

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 取合成宝物配置
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(dwTreasureInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物合成配置[InfoId:%d]", __FUNCTION__, dwTreasureInfoId);
		return;
	}

	// 检测需要的物品
	vector<DWORD>::const_iterator iter = pComposeCfg->cost_item_list.begin();
	for (; iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		// 道具不足
		DWORD dwCostItemId = *iter;
		if (pItemMgr->GetItemNum(dwCostItemId) < wTreasureNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_DOUBAO_COMPOSE_ITEM_NOT_ENOUGH);
			MAPLOG(ERROR_LEVEL, "[%s]宝物合成需要的道具不足[InfoId:%d,CostItemId:%d]",
				__FUNCTION__, dwTreasureInfoId, dwCostItemId);
			return;
		}
	}

		// 检测包裹空间
	if (GetBagEmptySize() < wTreasureNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]宝物包裹空间不足", __FUNCTION__);
		return;
	}

	// 添加宝物
	for (WORD i = 0; i < wTreasureNum; ++i)
	{
		AddTreasure(dwTreasureInfoId, TRUE, FROM_TREASURE_COMPOSE);
	}

	// 处理消耗
	for (iter = pComposeCfg->cost_item_list.begin(); iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		pItemMgr->RemoveItem(*iter, wTreasureNum, TRUE, FROM_TREASURE_COMPOSE);
	}

	MSG_S2C_TREASURE_COMPOSE_RESP msg;
	msg.m_dwTreasureInfoId = dwTreasureInfoId;
	msg.m_wTreasureNum = wTreasureNum;
	m_pPlayer->SendPacket(&msg);

	// 日常任务(圣器合成)
	m_pPlayer->OnUpdateDailyMission(22);
}

// 宝物穿戴
void CTreasureManager::OnPutOn(BYTE byFormationPos, BYTE byTreasurePos, DWORD dwTreasureUID)
{
	WORD wFromMsgId = C2S_TREASURE_PUT_ON_REQ;

	// 检测参数(阵型槽位, 宝物槽位)
	if (!byFormationPos || byFormationPos > MAX_BATTLE_HERO_NUM || byTreasurePos >= TREASURE_TYPE_MAX)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]请求宝物穿戴参数出错[formation_pos:%d, treasure_pos:%d]",
			__FUNCTION__, byFormationPos, byTreasurePos);
		return;
	}

	BYTE bySvrFormationPos = byFormationPos - 1;
	BYTE bySvrTreasurePos = byTreasurePos;

	// 检测是否有此装备
	if (dwTreasureUID)
	{
		const Treasure *pTreasure = GetTreasure(dwTreasureUID);
		if (!pTreasure)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此宝物[UID:%d]", __FUNCTION__, dwTreasureUID);
			return;
		}
	}

	// todo::检测此装备是否可以穿戴到此位置

	Formation &sFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[bySvrFormationPos];
	DWORD dwOldTreasureUID = sFormation.arTreasureList[bySvrTreasurePos];
	sFormation.arTreasureList[bySvrTreasurePos] = dwTreasureUID;

	if (dwOldTreasureUID)
		TreasureUpdateNotify(dwOldTreasureUID);
	TreasureUpdateNotify(dwTreasureUID);
 
 	// 从新同步阵型列表
 	m_pPlayer->GetHeroManager()->SendFormationList();

	// 更新战力
	m_pPlayer->UpdateFightingPower();
 
	MSG_S2C_TREASURE_PUT_ON_RESP msg;
	msg.m_byFormationPos = byFormationPos;
	msg.m_byTreasurePos = byTreasurePos;
	msg.m_dwUID = dwTreasureUID;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(20, GetLowQualityInWear(), false);
}

// 熔炼
void CTreasureManager::OnNetSmelt(DWORD dwTreasureInfoId)
{
	WORD wFromMsgId = C2S_TREASURE_SMELT_REQ;

	// 取熔炼配置
	const TreasureSmelt_Config *pSmeltCfg = g_pCfgMgr->GetTreasureSmelt(dwTreasureInfoId);
	if (!pSmeltCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "找不到宝物熔炼配置数据[InfoId:%d]", dwTreasureInfoId);
		return;
	}

	// 检测钻石消耗
	int nCostDiamond = g_GetConfigValue(103);
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "宝物熔炼消耗钻石不足[InfoId:%d]", dwTreasureInfoId);
		return;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	// 检测是否有要消耗的宝物
	int nIndex = -1;
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure *pTreasure = &m_pTreasureInfo->sTreasureList[i];
		// 1.未强化过 2.未精炼过 3.品质为橙色（配置表里面策划直接填被熔炼宝物ID即可） 4.还没穿戴的
		if (pTreasure->dwInfoId == pSmeltCfg->dwCostTreasureInfoId && 
			1 == pTreasure->wEnhanceLevel && !pTreasure->wRefineLevel && !pHeroMgr->GetTreasureWearHeroId(pTreasure->dwId))
		{
			nIndex = i;
			break;
		}
	}
	if (-1 == nIndex)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "宝物熔炼消耗的宝物不足[CostTreasureId:%d]", pSmeltCfg->dwCostTreasureInfoId);
		return;
	}

	// 删除宝物
	m_pTreasureInfo->sTreasureList[nIndex] = m_pTreasureInfo->sTreasureList[--m_pTreasureInfo->wTreasureNum];

	// 消耗钻石
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TREASURE_SMELT);

	// 获得熔炼物品
	m_pPlayer->AddObject(pSmeltCfg->dwItemId, 1, TRUE, FROM_TREASURE_SMELT);

	// 响应
	MSG_S2C_TREASURE_SMELT_RESP msg;
	msg.m_dwTreasureInfoId = dwTreasureInfoId;
	m_pPlayer->SendPacket(&msg);
}

// 升品
void CTreasureManager::OnNetQualityUP(DWORD dwTreasureUID)
{
	// 检测是否有此宝物
	Treasure *pTreasure = FindTreasure(dwTreasureUID);
	if (!pTreasure)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的宝物数据[UID:%d]",__FUNCTION__, dwTreasureUID);
		return;
	}

	// 取宝物配置
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物的配置数据[InfoId:%d]",__FUNCTION__, pTreasure->dwInfoId);
		return;
	}

	// 检测是否可以升品
	if (!pTreasureCfg->dwNextQualityTreasureId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]此宝物不能升品[InfoId:%d,Quality:%d]", __FUNCTION__, pTreasure->dwInfoId, pTreasureCfg->wQuality);
		return;
	}

	// 取升品消耗配置
	const TreasureQualityUPCost_Config *pQualityUPCostCfg = g_pCfgMgr->GetTreasureQualityUPCost(pTreasure->wRefineLevel);
	if (!pQualityUPCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物升品消耗配置[RefineLevel:%d]", __FUNCTION__, pTreasure->wRefineLevel);
		return;
	}

	// 检测消耗条件(道具)
	if (m_pPlayer->GetItemNum(pTreasureCfg->dwQualityUPCostItemId) < pQualityUPCostCfg->dwCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝物升品消耗道具不足[RefineLevel:%d,CostItemId:%d,CostItemNum:%d]", 
			__FUNCTION__, pTreasure->wRefineLevel, pTreasureCfg->dwQualityUPCostItemId, pQualityUPCostCfg->dwCostItemNum);
		return;
	}

	// 检测消耗条件(货币)
	if (!m_pPlayer->CheckMoneyCost(pQualityUPCostCfg->vCostCurrencyList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝物升品消耗货币不足[RefineLevel:%d]",__FUNCTION__, pTreasure->wRefineLevel);
		return;
	}

	// 强化配置
	const TreasureEnhance_Config *pTreasureStrengthenCfg =
		g_pCfgMgr->GetTreasureStrengthen(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
	if (!pTreasureStrengthenCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物强化配置[Quality:%d,EnhanceLevel:%d]",
			__FUNCTION__, pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		return;
	}

	// 处理消耗(道具)
	m_pPlayer->RemoveItem(pTreasureCfg->dwQualityUPCostItemId, pQualityUPCostCfg->dwCostItemNum, FROM_TREASURE_QUALITY_UP);

	// 处理消耗(货币)
	m_pPlayer->DoMoneyCost(pQualityUPCostCfg->vCostCurrencyList, 1, FROM_TREASURE_QUALITY_UP);

	// 升品规则(1.新精炼等级=旧精炼等级 2.强化等级重新处理)
	pTreasure->dwInfoId = pTreasureCfg->dwNextQualityTreasureId;
	pTreasure->wEnhanceLevel = 1;
	pTreasure->dwEnhanceExp += pTreasureStrengthenCfg->dwTotalExp;

	// 取新宝物配置
	const Treasure_Config *pNewTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物的配置数据[InfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return;
	}

	// 处理升级
	const TreasureEnhance_Config *pStrengthenCfg;
	while (true)
	{
		pStrengthenCfg = g_pCfgMgr->GetTreasureStrengthen(pNewTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		if (!pStrengthenCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到宝物强化配置[Quality:%d,StrengthenLevel:%d]",
				__FUNCTION__, pNewTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
			break;
		}

		// 可升级
		if (pStrengthenCfg->dwNeedExp && pTreasure->dwEnhanceExp >= pStrengthenCfg->dwNeedExp)
		{
			pTreasure->dwEnhanceExp -= pStrengthenCfg->dwNeedExp;
			pTreasure->wEnhanceLevel++;
		}
		else
		{
			break;
		}

		// 检测是否已满级
		if (pTreasure->wEnhanceLevel >= pNewTreasureCfg->wStrengthenLevelMax)
			break;
	}

	// 重新同步数据给客户端
	TreasureUpdateNotify(*pTreasure, true);

	// 响应
	MSG_S2C_TREASURE_QUALITY_UP_RESP msg;
	msg.m_tTreasure = *pTreasure;
	m_pPlayer->SendPacket(&msg);
}

// 请求宝物列表
void CTreasureManager::OnTreasureList()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_TREASURE_LIST_RESP msg;
	msg.m_wTreasureNum = m_pTreasureInfo->wTreasureNum;
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		TreasureBaseData &sDest = msg.m_TreasureList[i];
		Treasure &sSrc = m_pTreasureInfo->sTreasureList[i];
		sDest.dwId = sSrc.dwId;
		sDest.wInfoId = sSrc.dwInfoId;
		sDest.byEnhanceLevel = sSrc.wEnhanceLevel;
		sDest.dwEnhanceExp = sSrc.dwEnhanceExp;
		sDest.byRefineLevel = sSrc.wRefineLevel;
		sDest.wWearHeroInfoId = pHeroMgr->GetTreasureWearHeroId(sSrc.dwId);
	}
	m_pPlayer->SendPacket(&msg);
}

// 添加装备
bool CTreasureManager::AddTreasure(DWORD dwTreasureInfoId, BOOL bNotice, WORD wFromId)
{
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(dwTreasureInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到宝物配置数据[InfoId:%d]", dwTreasureInfoId);
		return false;
	}

	// 记录获得
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwTreasureInfoId, 1, true, wFromId);

	// 7天活动事件(圣器收集)
	m_pPlayer->OnUpdateSevenDayTarget(7);

	Treasure tTreasure;
	memset(&tTreasure, 0, sizeof(tTreasure));
	tTreasure.dwId = GetNewSeriesId();
	tTreasure.dwInfoId = dwTreasureInfoId;
	tTreasure.wEnhanceLevel = 1;
	return AddTreasure(tTreasure);
}

// 添加装备
bool CTreasureManager::AddTreasure(const Treasure &sTreasure, BOOL bNotice)
{
	if (!GetBagEmptySize())
	{
		//MAPLOG(ERROR_LEVEL, "[%s]宝物背包已满[num:%d]", __FUNCTION__, m_pTreasureInfo->wTreasureNum);

		MAPLOG(ERROR_LEVEL, "[%s]宝物背包已满[RoleId:%d,ListNum:%d,InfoId:%d]", __FUNCTION__,
			m_pPlayer->GetRoleId(), m_pTreasureInfo->wTreasureNum, sTreasure.dwInfoId);

		// 记录装备丢失记录
		m_pPlayer->SaveItemLoseRecord(sTreasure.dwInfoId);

		return false;
	}

	m_pTreasureInfo->sTreasureList[m_pTreasureInfo->wTreasureNum++] = sTreasure;

	if (bNotice)
		TreasureUpdateNotify(sTreasure);

	return true;
}

// 删除宝物
bool CTreasureManager::RemoveTreasure(DWORD dwUID, WORD wFromId)
{
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure &sDel = m_pTreasureInfo->sTreasureList[i];
		if (sDel.dwId == dwUID)
		{
			// 记录获得
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), sDel.dwInfoId, 1, false, wFromId);

			TreasureUpdateNotify(sDel, false);
			sDel = m_pTreasureInfo->sTreasureList[--m_pTreasureInfo->wTreasureNum];
			return true;
		}
	}
	return true;
}

// 删除宝物(只删除没有强化及精练的)
bool CTreasureManager::RemoveTreasureByInfoId(DWORD dwInfoId, WORD wFromId)
{
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure *pDel = &m_pTreasureInfo->sTreasureList[i];
		if (pDel->dwInfoId == dwInfoId && !pDel->wEnhanceLevel && !pDel->wRefineLevel)
		{
			// 记录获得
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), pDel->dwInfoId, 1, false, wFromId);

			*pDel = m_pTreasureInfo->sTreasureList[--m_pTreasureInfo->wTreasureNum];
			return true;
		}
	}

	return false;
}

// 取装备背包空闲数量
WORD CTreasureManager::GetBagEmptySize()
{
	WORD wBagSize = min(MAX_TREASURE_NUM, m_pPlayer->GetVipValue(VIP_TREASURE_BAG_NUM));
	return wBagSize > m_pTreasureInfo->wTreasureNum ? wBagSize - m_pTreasureInfo->wTreasureNum : 0;
}

// 取宝物强化等级对应的总经验
DWORD CTreasureManager::GetStrengthLevelTotalExp(WORD wQuality, WORD wLevel)
{
	const TreasureEnhance_Config *pStrengthenCf = CConfigManager::Instance()->GetTreasureStrengthen(wQuality, wLevel);
	if (!pStrengthenCf)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物强化等级对应的配置[Quality:%d,Level:%d]", __FUNCTION__, wQuality, wLevel);
		return 0;
	}
	return pStrengthenCf->dwTotalExp;
}

// 取宝物数据
const Treasure* CTreasureManager::GetTreasure(DWORD dwTreasureUID)
{
	return FindTreasure(dwTreasureUID);
}

// 宝物更新通知
void CTreasureManager::TreasureUpdateNotify(DWORD dwTreasureUID, BYTE byFlag)
{
	const Treasure *pTreasure = GetTreasure(dwTreasureUID);
	if (pTreasure)
	{
		TreasureUpdateNotify(*pTreasure);
	}
}

// 宝物更新通知
void CTreasureManager::TreasureUpdateNotify(const Treasure &sTreasure, BYTE byFlag)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_TREASURE_INFO_NOTIFY msg;
	msg.m_byFlag = byFlag;
	msg.m_sBaseData.dwId = sTreasure.dwId;
	msg.m_sBaseData.wInfoId = sTreasure.dwInfoId;
	msg.m_sBaseData.byEnhanceLevel = sTreasure.wEnhanceLevel;
	msg.m_sBaseData.byRefineLevel = sTreasure.wRefineLevel;
	msg.m_sBaseData.dwEnhanceExp = sTreasure.dwEnhanceExp;
	msg.m_sBaseData.wWearHeroInfoId = pHeroMgr->GetTreasureWearHeroId(sTreasure.dwId);
	m_pPlayer->SendPacket(&msg);
}

// 处理强化
bool CTreasureManager::DoStrengthen(Treasure *pTreasure, const Treasure_Config *pTreasureCfg, DWORD dwCostTreasureUId, int &nCostGold)
{
	// 检测是否已满级
	if (pTreasure->wEnhanceLevel >= pTreasureCfg->wStrengthenLevelMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]宝物强化等级已达上限[uId:%d,MaxLevel:%d]",
			__FUNCTION__, pTreasure->dwId, pTreasure->wEnhanceLevel);
		return false;
	}

	Treasure *pCostTreasure = FindTreasure(dwCostTreasureUId);
	if (!pCostTreasure)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有存在此宝物[uid:%d]", __FUNCTION__, dwCostTreasureUId);
		return false;
	}

	// 已经精练过的不能作为材料
	if (pCostTreasure->wRefineLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]已精炼过的宝物不能作为消耗品[InfoId:%d]", __FUNCTION__, pCostTreasure->dwInfoId);
		return false;
	}

	const Treasure_Config *pCostTreasureCfg = g_pCfgMgr->GetTreasure(pCostTreasure->dwInfoId);
	if (!pCostTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物配置[InfoId:%d]", __FUNCTION__, pCostTreasure->dwInfoId);
		return false;
	}

	const TreasureEnhance_Config *pCostTreasureStrengthenCfg = 
		g_pCfgMgr->GetTreasureStrengthen(pCostTreasureCfg->wQuality, pCostTreasure->wEnhanceLevel);
	if (!pCostTreasureStrengthenCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物强化配置[Quality:%d,StrengthenLevel:%d]",
			__FUNCTION__, pCostTreasureCfg->wQuality, pCostTreasure->wEnhanceLevel);
		return false;
	}

	DWORD dwTotalExp = pCostTreasureStrengthenCfg->dwTotalExp + pCostTreasure->dwEnhanceExp + pCostTreasureCfg->dwAsExp;

	// 英雄升级，把吞噬的卡牌经验改成1, 圣器升级也和英雄升级处理方式一样
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT && pCostTreasureCfg->byType == 2){
		dwTotalExp = 1;
	}

	// 检测金币
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold + dwTotalExp)
	{
		MAPLOG(ERROR_LEVEL, "[%s]宝物强化金币不足[InfoId:%d,NeedGold:%d]", __FUNCTION__, nCostGold + dwTotalExp);
		return false;
	}

	pTreasure->dwEnhanceExp += dwTotalExp;

	// 处理升级
	bool bMaxLevel = false;
	const TreasureEnhance_Config *pStrengthenCfg;
	while (true)
	{
		pStrengthenCfg = g_pCfgMgr->GetTreasureStrengthen(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		if (!pStrengthenCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到宝物强化配置[Quality:%d,StrengthenLevel:%d]",
				__FUNCTION__, pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
			return false;
		}

		if (pStrengthenCfg->dwNeedExp && pTreasure->dwEnhanceExp >= pStrengthenCfg->dwNeedExp)
		{
			pTreasure->dwEnhanceExp -= pStrengthenCfg->dwNeedExp;
			pTreasure->wEnhanceLevel++;
		}
		else
		{
			break;
		}

		// 检测是否已满级
		if (pTreasure->wEnhanceLevel >= pTreasureCfg->wStrengthenLevelMax)
			break;
	}

	nCostGold += dwTotalExp;
	return true;
}

// 取到达指定品质的宝物数量
WORD CTreasureManager::GetTreasureNumByQuality(WORD wQuality)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure *pTreasure = &m_pTreasureInfo->sTreasureList[i];
		const Treasure_Config *pTreasureCfg = g_pCfgMgr->GetTreasure(pTreasure->dwInfoId);

		if (pTreasureCfg && pTreasureCfg->wQuality >= wQuality)
			wNum++;
	}
	return wNum;
}

// 取英雄宝物最小等级
WORD CTreasureManager::GetMinEnhanceLevelInWear()
{
	WORD wMinLevel = 0;
	const Formation *pFormationList = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation &tFormation = pFormationList[i];
		if (!tFormation.dwHeroUID)
			return 0;

		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			DWORD dwTreasureId = tFormation.arTreasureList[k];
			const Treasure *pTreasure = dwTreasureId ? GetTreasure(dwTreasureId) : NULL;
			if (!pTreasure)
				return 0;

			if (!wMinLevel)
				wMinLevel = pTreasure->wEnhanceLevel;

			wMinLevel = min(wMinLevel, pTreasure->wEnhanceLevel);
		}
	}
	return wMinLevel;
}

// 取英雄宝物最低品质
WORD CTreasureManager::GetLowQualityInWear()
{
	WORD wLowQuality = 0;
	const Formation *pFormationList = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation &tFormation = pFormationList[i];
		if (!tFormation.dwHeroUID)
			return 0;

		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			DWORD dwTreasureId = tFormation.arTreasureList[k];
			const Treasure *pTreasure = dwTreasureId ? GetTreasure(dwTreasureId) : NULL;
			if (!pTreasure)
				return 0;

			const Treasure_Config *pTreasureCfg = g_pCfgMgr->GetTreasure(pTreasure->dwInfoId);
			if (!pTreasureCfg)
				return 0;

			if (!wLowQuality)
				wLowQuality = pTreasureCfg->wQuality;

			wLowQuality = min(wLowQuality, pTreasureCfg->wQuality);
		}
	}

	return wLowQuality;
}

Treasure* CTreasureManager::FindTreasure(DWORD dwUID)
{
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		if (m_pTreasureInfo->sTreasureList[i].dwId == dwUID)
		{
			return &m_pTreasureInfo->sTreasureList[i];
		}
	}

	return NULL;
}

// 取一个新的系列id
DWORD CTreasureManager::GetNewSeriesId()
{
	// 不应该出现这种情况
	if (m_dwSeriesId >= 9999)
	{
		m_dwSeriesId = 1;
		MAPLOG(ERROR_LEVEL, "[%s]宝物的系列ID出现异常,使用超过9999次!!!", m_pPlayer->GetUTF8RoleName());
	}

	return 100000 * m_pPlayer->GetLoginTimes() + m_dwSeriesId++;
}
