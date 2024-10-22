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

// ��ӳ�ֵ���
void CRechargeAndCostRankActivity::AddRechargeAmount(DWORD dwAmount)
{
	// ����Ƿ��ڼ�
	WORD wActivityDataId = m_pActivityMgr->GetActivityDataId(159);
	if (!wActivityDataId)
		return;

	// ������µĻ,������Ҫ����
	if (wActivityDataId != m_pActivityData->wLastActivityDataId)
	{
		m_pActivityData->wLastActivityDataId = wActivityDataId;
		m_pActivityData->dwRechargeAmount = 0;
		m_pActivityData->dwCostDiamond = 0;
	}

	m_pActivityData->dwRechargeAmount += dwAmount;

	// ����Ƿ񵽴��������
	if (m_pActivityData->dwRechargeAmount < g_GetConfigValue(305))
		return;

	// �������а�
	CRankManager::Instance()->AddRoleRankData(RANK_RECHARGE, m_pPlayer, m_pActivityData->dwRechargeAmount, 0, 0, 200);
}

// ���������ʯ
void CRechargeAndCostRankActivity::AddCostDiamond(DWORD dwDiamond)
{
	// ����Ƿ��ڼ�
	WORD wActivityDataId = m_pActivityMgr->GetActivityDataId(159);
	if (!wActivityDataId)
		return;

	// ������µĻ,������Ҫ����
	if (wActivityDataId != m_pActivityData->wLastActivityDataId)
	{
		m_pActivityData->wLastActivityDataId = wActivityDataId;
		m_pActivityData->dwRechargeAmount = 0;
		m_pActivityData->dwCostDiamond = 0;
	}

	m_pActivityData->dwCostDiamond += dwDiamond;

	// ����Ƿ񵽴��������
	if (m_pActivityData->dwCostDiamond < g_GetConfigValue(306))
		return;

	// �������а�
	CRankManager::Instance()->AddRoleRankData(RANK_DIAMOND_COST, m_pPlayer, m_pActivityData->dwCostDiamond, 0, 0, 200);
}