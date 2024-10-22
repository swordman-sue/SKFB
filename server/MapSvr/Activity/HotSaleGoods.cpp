#include "stdafx.h"
#include "HotSaleGoods.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\Recharge\RechargeManager.h"
#include "ActivityManager.h"


// ȡ������������
WORD GetBuyNumMax(const HotSaleGoods_Config *pGoodsCfg, WORD wIdx)
{
	if (wIdx >= pGoodsCfg->vBuyNumMaxList.size())
		return 0;

	return pGoodsCfg->vBuyNumMaxList[wIdx];
}

// ȡ��ֵId
WORD GetRechargeId(const HotSaleGoods_Config *pGoodsCfg, WORD wIdx)
{
	if (wIdx >= pGoodsCfg->vRechargeIdList.size())
		return 0;

	return pGoodsCfg->vRechargeIdList[wIdx];
}








void CHotSaleGoods::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pPlayerHotSaleGoodsInfo = &pPlayer->GetRoleFullInfo().tActivityInfo.tPlayerHotSaleGoodsInfo;
	m_pActivityMgr = pActivityMgr;
}

VOID CHotSaleGoods::OnNewDay()
{
	UpdateHotSaleGoodsData();

	// ľ�д˻
	WORD wDataId = GetCurActivityDataId();
	if (!wDataId)
		return;

	WORD wDay = m_pPlayer->GetActivityManager()->GetActivityDay(SYSTEM_HOT_SALE_GOODS_ID);
	for (WORD i = 0; i < m_pPlayerHotSaleGoodsInfo->wGoodsNum; ++i)
	{
		HotSaleGoods &tGoods = m_pPlayerHotSaleGoodsInfo->arGoodsList[i];
		const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId,tGoods.wGoodsId);
		if (!pGoodsCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ʒ����[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, tGoods.wGoodsId);
			continue;
		}

		// ����Ҫ����
		if (!pGoodsCfg->vResetDayList.size())
			continue;

		// ������������
		for (int k = 0; k < pGoodsCfg->vResetDayList.size(); ++k)
		{
			if (pGoodsCfg->vResetDayList[k] == wDay)
			{
				// ������������
				tGoods.wCurDataIdx = 0;
				tGoods.wBuyTimes = 0;
				tGoods.wTakeTimes = 0;
				break;
			}
		}
	}
}

// ����������Ʒ����
void CHotSaleGoods::OnNetHotSaleGoodsData()
{
	UpdateHotSaleGoodsData();
	SendHotSaleGoodsData();
}

// ��ȡ��ֵ����
void CHotSaleGoods::OnNetTakeRechargeReward(WORD wGoodsId)
{
	// ȡ��Ʒ����
	WORD wDataId = GetCurActivityDataId();
	const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ʒ����[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, wGoodsId);
		return; 
	}

	// ȡ��Ʒ����
	HotSaleGoods *pGoods = GetGoods(wGoodsId);
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ������������Ʒ����[RoleId:%d,GoodsId:%d]",m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// ����Ƿ������ȡ����
	if (pGoods->wTakeTimes >= pGoods->wBuyTimes)
	{
		MAPLOG(ERROR_LEVEL, "��ҵ�ǰû�п���ȡ�Ľ���[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// ���Ҫ��ȡ����Ʒ
	if (pGoods->wCurDataIdx >= pGoodsCfg->vItemList.size())
	{
		MAPLOG(IMPORTANT_LEVEL, "��Ʒ���������쳣[RoleId:%d,GoodsId:%d,CurItemIdx:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, pGoods->wCurDataIdx);
		return;
	}

	// ���Ž���
	m_pPlayer->AddObjectData(&pGoodsCfg->vItemList[pGoods->wCurDataIdx], TRUE, FROM_GET_HOT_SALE_RECHARGE_REWARD);

	// ��������ȡ����
	pGoods->wTakeTimes++;

	// ��ǰ��Ʒ�Ĺ�������Ѵ�����
	WORD wBuyNumMax = GetBuyNumMax(pGoodsCfg, pGoods->wCurDataIdx);
	if (wBuyNumMax && pGoods->wTakeTimes >= wBuyNumMax)
	{
		// ���滹����Ʒ�ɹ���
		if (pGoods->wCurDataIdx + 1 < pGoodsCfg->vItemList.size())
		{
			// �����µ���Ʒ
			pGoods->wCurDataIdx++;
			pGoods->wBuyTimes = 0;
			pGoods->wTakeTimes = 0;
		}
	}

	// ����ͬ������
	SendHotSaleGoodsData();
	
	// ��Ӧ
	MSG_S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP msg;
	msg.m_wGoodsId = wGoodsId;
	m_pPlayer->SendPacket(&msg);
}

// ������Ʒ
void CHotSaleGoods::OnNetBuyGoods(WORD wGoodsId)
{
	UpdateHotSaleGoodsData();

	// ȡ��Ʒ����
	WORD wDataId = GetCurActivityDataId();
	const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ���������Ʒ����[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, wGoodsId);
		return;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ���Ĳ���[DataId:%d,GoodsId:%d]", __FUNCTION__, wDataId, wGoodsId);
		return;
	}

	// todo::���ȼ�����

	// ȡ��Ʒ����
	HotSaleGoods *pGoods = GetGoods(wGoodsId);
	if (!pGoods)
	{
		HotSaleGoods tGoods;
		memset(&tGoods, 0, sizeof(tGoods));
		tGoods.wGoodsId = wGoodsId;
		AddGoods(tGoods);
		pGoods = GetGoods(wGoodsId);
	}
	if (!pGoods)
	{
		MAPLOG(IMPORTANT_LEVEL, "�Ҳ������������Ʒ����[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// �����Ʒ����
	if (pGoods->wCurDataIdx >= pGoodsCfg->vItemList.size())
	{
		MAPLOG(IMPORTANT_LEVEL, "������Ʒ��Ʒ�����쳣[RoleId:%d,GoodsId:%d,Idx:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, pGoods->wCurDataIdx);
		return;
	}

	// ����Ƿ��Ѵ��޹�������
	WORD wBuyNumMax = GetBuyNumMax(pGoodsCfg, pGoods->wCurDataIdx);
	if (wBuyNumMax && pGoods->wBuyTimes >= wBuyNumMax)
	{
		MAPLOG(IMPORTANT_LEVEL, "������Ʒ�Ѵﹺ������[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList);

	// ��ӹ������
	pGoods->wBuyTimes++;

	// ����Ƿ������ȡ����
	if (pGoods->wTakeTimes >= pGoods->wBuyTimes)
	{
		MAPLOG(ERROR_LEVEL, "��ҵ�ǰû�п���ȡ�Ľ���[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// ���Ž���
	m_pPlayer->AddObjectData(&pGoodsCfg->vItemList[pGoods->wCurDataIdx], TRUE, FROM_HOT_SALE_BUY_REWARD);

	// ��������ȡ����
	pGoods->wTakeTimes++;

	// ��ǰ��Ʒ�Ĺ�������Ѵ�����
	if (wBuyNumMax && pGoods->wTakeTimes >= wBuyNumMax)
	{
		// ���滹����Ʒ�ɹ���
		if (pGoods->wCurDataIdx + 1 < pGoodsCfg->vItemList.size())
		{
			// �����µ���Ʒ
			pGoods->wCurDataIdx++;
			pGoods->wBuyTimes = 0;
			pGoods->wTakeTimes = 0;
		}
	}

	// ����ͬ�����ݸ��ͻ���
	SendHotSaleGoodsData();
}

// ��ֵ֪ͨ�¼�
void CHotSaleGoods::OnRechargeNotify(WORD wGoodsId, DWORD dwRechargeId)
{
	UpdateHotSaleGoodsData();

	// ȡ��Ʒ����
	WORD wDataId = GetCurActivityDataId();
	const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ���������Ʒ����[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, wGoodsId);
		return;
	}

	// todo::���ȼ�����

	// ȡ��Ʒ����
	HotSaleGoods *pGoods = GetGoods(wGoodsId);
	if (!pGoods)
	{
		HotSaleGoods tGoods;
		memset(&tGoods, 0, sizeof(tGoods));
		tGoods.wGoodsId = wGoodsId;
		AddGoods(tGoods);
		pGoods = GetGoods(wGoodsId);
	}
	if (!pGoods)
	{
		MAPLOG(IMPORTANT_LEVEL, "�Ҳ������������Ʒ����[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// �����Ʒ����
	if (pGoods->wCurDataIdx >= pGoodsCfg->vItemList.size())
	{
		MAPLOG(IMPORTANT_LEVEL, "������Ʒ��Ʒ�����쳣[RoleId:%d,GoodsId:%d,Idx:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, pGoods->wCurDataIdx);
		return;
	}

	// ����Ƿ��Ѵ��޹�������
	WORD wBuyNumMax = GetBuyNumMax(pGoodsCfg, pGoods->wCurDataIdx);
	if (wBuyNumMax && pGoods->wBuyTimes >= wBuyNumMax)
	{
		MAPLOG(IMPORTANT_LEVEL, "������Ʒ�Ѵﹺ������[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// ����Ƿ��Ӧ�ĳ�ֵ��λ
	if (GetRechargeId(pGoodsCfg, pGoods->wCurDataIdx) != dwRechargeId)
	{
		MAPLOG(IMPORTANT_LEVEL, "������Ʒ��ֵ��λ�쳣[RoleId:%d,GoodsId:%d,RechargeId:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, dwRechargeId);
		return;
	}

	// ��ӹ������
	pGoods->wBuyTimes++;

	// ����ͬ�����ݸ��ͻ���
	SendHotSaleGoodsData();
}

bool CHotSaleGoods::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_pPlayerHotSaleGoodsInfo->wGoodsNum; ++i)
	{
		HotSaleGoods &tGoods = m_pPlayerHotSaleGoodsInfo->arGoodsList[i];
		if (tGoods.wBuyTimes > tGoods.wTakeTimes)
			return true;
	}

	return false;
}


// ����������Ʒ����
void CHotSaleGoods::SendHotSaleGoodsData()
{
	MSG_S2C_HOT_SALE_GOODS_DATA_RESP msg;
	msg.m_Data = *m_pPlayerHotSaleGoodsInfo;
	m_pPlayer->SendPacket(&msg);
}

// ��������
void CHotSaleGoods::UpdateHotSaleGoodsData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_HOT_SALE_GOODS_ID);
	// �µĻ�ѿ�ʼ
	if (wDataId != m_pPlayerHotSaleGoodsInfo->wLastActivityDataId)
	{
		m_pPlayerHotSaleGoodsInfo->wLastActivityDataId = wDataId;
		m_pPlayerHotSaleGoodsInfo->wGoodsNum = 0;
	}
}

// ȡ��Ʒ����
HotSaleGoods* CHotSaleGoods::GetGoods(WORD wGoodsId)
{
	for (WORD i = 0; i < m_pPlayerHotSaleGoodsInfo->wGoodsNum; ++i)
	{
		if (m_pPlayerHotSaleGoodsInfo->arGoodsList[i].wGoodsId == wGoodsId)
			return &m_pPlayerHotSaleGoodsInfo->arGoodsList[i];
	}
	return NULL;
}

// �����Ʒ
void CHotSaleGoods::AddGoods(const HotSaleGoods &tGoods)
{
	// ����б��Ƿ�����
	if (m_pPlayerHotSaleGoodsInfo->wGoodsNum >= MAX_HOT_SALE_GOODS_NUM)
	{
		MAPLOG(ERROR_LEVEL, "������Ʒ�б�����[RoleId:%d,GoodsId:%d]",m_pPlayer->GetRoleId(), tGoods.wGoodsId);
		return;
	}

	m_pPlayerHotSaleGoodsInfo->arGoodsList[m_pPlayerHotSaleGoodsInfo->wGoodsNum++] = tGoods;
}

// ȡ��ǰ�����ID
WORD CHotSaleGoods::GetCurActivityDataId()
{
	return m_pPlayerHotSaleGoodsInfo->wLastActivityDataId;
}

