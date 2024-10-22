#include "stdafx.h"
#include "RechargeAndCostRankActivity.h"
#include "ActivityManager.h"
#include "..\Player.h"
#include "..\Rank\RankManager.h"
#include "..\ConfigManager.h"

CRechargeAndCostRankActivity::CRechargeAndCostRankActivity()
{

}

CRechargeAndCostRankActivity::~CRechargeAndCostRankActivity()
{

}

void CRechargeAndCostRankActivity::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pActivityData = &pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeAndCostDiamondRankData;
}

// 添加充值额度
void CRechargeAndCostRankActivity::AddRechargeAmount(DWORD dwAmount)
{
	// 检测是否活动期间
	WORD wActivityDataId = m_pActivityMgr->GetActivityDataId(159);
	if (!wActivityDataId)
		return;

	// 如果是新的活动,数据需要重置
	if (wActivityDataId != m_pActivityData->wLastActivityDataId)
	{
		m_pActivityData->wLastActivityDataId = wActivityDataId;
		m_pActivityData->dwRechargeAmount = 0;
		m_pActivityData->dwCostDiamond = 0;
	}

	m_pActivityData->dwRechargeAmount += dwAmount;

	// 检测是否到达入榜条件
	if (m_pActivityData->dwRechargeAmount < g_GetConfigValue(305))
		return;

	// 更新排行榜
	CRankManager::Instance()->AddRoleRankData(RANK_RECHARGE, m_pPlayer, m_pActivityData->dwRechargeAmount, 0, 0, 200);
}

// 添加消费钻石
void CRechargeAndCostRankActivity::AddCostDiamond(DWORD dwDiamond)
{
	// 检测是否活动期间
	WORD wActivityDataId = m_pActivityMgr->GetActivityDataId(159);
	if (!wActivityDataId)
		return;

	// 如果是新的活动,数据需要重置
	if (wActivityDataId != m_pActivityData->wLastActivityDataId)
	{
		m_pActivityData->wLastActivityDataId = wActivityDataId;
		m_pActivityData->dwRechargeAmount = 0;
		m_pActivityData->dwCostDiamond = 0;
	}

	m_pActivityData->dwCostDiamond += dwDiamond;

	// 检测是否到达入榜条件
	if (m_pActivityData->dwCostDiamond < g_GetConfigValue(306))
		return;

	// 更新排行榜
	CRankManager::Instance()->AddRoleRankData(RANK_DIAMOND_COST, m_pPlayer, m_pActivityData->dwCostDiamond, 0, 0, 200);
}