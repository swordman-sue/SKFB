#include "stdafx.h"
#include "HotSaleGoods.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\Recharge\RechargeManager.h"
#include "ActivityManager.h"


// 取购买数量上限
WORD GetBuyNumMax(const HotSaleGoods_Config *pGoodsCfg, WORD wIdx)
{
	if (wIdx >= pGoodsCfg->vBuyNumMaxList.size())
		return 0;

	return pGoodsCfg->vBuyNumMaxList[wIdx];
}

// 取充值Id
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

	// 木有此活动
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
			MAPLOG(ERROR_LEVEL, "[%s]找不到热卖商品配置[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, tGoods.wGoodsId);
			continue;
		}

		// 不需要处理
		if (!pGoodsCfg->vResetDayList.size())
			continue;

		// 处理数据重置
		for (int k = 0; k < pGoodsCfg->vResetDayList.size(); ++k)
		{
			if (pGoodsCfg->vResetDayList[k] == wDay)
			{
				// 数据索引重置
				tGoods.wCurDataIdx = 0;
				tGoods.wBuyTimes = 0;
				tGoods.wTakeTimes = 0;
				break;
			}
		}
	}
}

// 请求热卖商品数据
void CHotSaleGoods::OnNetHotSaleGoodsData()
{
	UpdateHotSaleGoodsData();
	SendHotSaleGoodsData();
}

// 领取充值奖励
void CHotSaleGoods::OnNetTakeRechargeReward(WORD wGoodsId)
{
	// 取商品配置
	WORD wDataId = GetCurActivityDataId();
	const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到热卖商品配置[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, wGoodsId);
		return; 
	}

	// 取商品数据
	HotSaleGoods *pGoods = GetGoods(wGoodsId);
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "找不到玩家热卖商品数据[RoleId:%d,GoodsId:%d]",m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 检测是否可以领取奖励
	if (pGoods->wTakeTimes >= pGoods->wBuyTimes)
	{
		MAPLOG(ERROR_LEVEL, "玩家当前没有可领取的奖励[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 检测要领取的物品
	if (pGoods->wCurDataIdx >= pGoodsCfg->vItemList.size())
	{
		MAPLOG(IMPORTANT_LEVEL, "物品索引出现异常[RoleId:%d,GoodsId:%d,CurItemIdx:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, pGoods->wCurDataIdx);
		return;
	}

	// 发放奖励
	m_pPlayer->AddObjectData(&pGoodsCfg->vItemList[pGoods->wCurDataIdx], TRUE, FROM_GET_HOT_SALE_RECHARGE_REWARD);

	// 更新已领取次数
	pGoods->wTakeTimes++;

	// 当前商品的购买次数已达上限
	WORD wBuyNumMax = GetBuyNumMax(pGoodsCfg, pGoods->wCurDataIdx);
	if (wBuyNumMax && pGoods->wTakeTimes >= wBuyNumMax)
	{
		// 后面还有商品可购买
		if (pGoods->wCurDataIdx + 1 < pGoodsCfg->vItemList.size())
		{
			// 产生新的商品
			pGoods->wCurDataIdx++;
			pGoods->wBuyTimes = 0;
			pGoods->wTakeTimes = 0;
		}
	}

	// 主动同步数据
	SendHotSaleGoodsData();
	
	// 响应
	MSG_S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP msg;
	msg.m_wGoodsId = wGoodsId;
	m_pPlayer->SendPacket(&msg);
}

// 购买商品
void CHotSaleGoods::OnNetBuyGoods(WORD wGoodsId)
{
	UpdateHotSaleGoodsData();

	// 取商品配置
	WORD wDataId = GetCurActivityDataId();
	const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到热卖商品配置[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, wGoodsId);
		return;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]购买热卖商品消耗不足[DataId:%d,GoodsId:%d]", __FUNCTION__, wDataId, wGoodsId);
		return;
	}

	// todo::检测等级限制

	// 取商品数据
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
		MAPLOG(IMPORTANT_LEVEL, "找不到玩家热卖商品数据[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 检测物品索引
	if (pGoods->wCurDataIdx >= pGoodsCfg->vItemList.size())
	{
		MAPLOG(IMPORTANT_LEVEL, "热卖商品物品索引异常[RoleId:%d,GoodsId:%d,Idx:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, pGoods->wCurDataIdx);
		return;
	}

	// 检测是否已达限购买上限
	WORD wBuyNumMax = GetBuyNumMax(pGoodsCfg, pGoods->wCurDataIdx);
	if (wBuyNumMax && pGoods->wBuyTimes >= wBuyNumMax)
	{
		MAPLOG(IMPORTANT_LEVEL, "热卖商品已达购买上限[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList);

	// 添加购买次数
	pGoods->wBuyTimes++;

	// 检测是否可以领取奖励
	if (pGoods->wTakeTimes >= pGoods->wBuyTimes)
	{
		MAPLOG(ERROR_LEVEL, "玩家当前没有可领取的奖励[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 发放奖励
	m_pPlayer->AddObjectData(&pGoodsCfg->vItemList[pGoods->wCurDataIdx], TRUE, FROM_HOT_SALE_BUY_REWARD);

	// 更新已领取次数
	pGoods->wTakeTimes++;

	// 当前商品的购买次数已达上限
	if (wBuyNumMax && pGoods->wTakeTimes >= wBuyNumMax)
	{
		// 后面还有商品可购买
		if (pGoods->wCurDataIdx + 1 < pGoodsCfg->vItemList.size())
		{
			// 产生新的商品
			pGoods->wCurDataIdx++;
			pGoods->wBuyTimes = 0;
			pGoods->wTakeTimes = 0;
		}
	}

	// 主动同步数据给客户端
	SendHotSaleGoodsData();
}

// 充值通知事件
void CHotSaleGoods::OnRechargeNotify(WORD wGoodsId, DWORD dwRechargeId)
{
	UpdateHotSaleGoodsData();

	// 取商品配置
	WORD wDataId = GetCurActivityDataId();
	const HotSaleGoods_Config *pGoodsCfg = g_pCfgMgr->GetHotSaleGoods(wDataId, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到热卖商品配置[DataId:%d,GoodsId:%d]",__FUNCTION__, wDataId, wGoodsId);
		return;
	}

	// todo::检测等级限制

	// 取商品数据
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
		MAPLOG(IMPORTANT_LEVEL, "找不到玩家热卖商品数据[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 检测物品索引
	if (pGoods->wCurDataIdx >= pGoodsCfg->vItemList.size())
	{
		MAPLOG(IMPORTANT_LEVEL, "热卖商品物品索引异常[RoleId:%d,GoodsId:%d,Idx:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, pGoods->wCurDataIdx);
		return;
	}

	// 检测是否已达限购买上限
	WORD wBuyNumMax = GetBuyNumMax(pGoodsCfg, pGoods->wCurDataIdx);
	if (wBuyNumMax && pGoods->wBuyTimes >= wBuyNumMax)
	{
		MAPLOG(IMPORTANT_LEVEL, "热卖商品已达购买上限[RoleId:%d,GoodsId:%d]", m_pPlayer->GetRoleId(), wGoodsId);
		return;
	}

	// 检测是否对应的充值档位
	if (GetRechargeId(pGoodsCfg, pGoods->wCurDataIdx) != dwRechargeId)
	{
		MAPLOG(IMPORTANT_LEVEL, "热卖商品充值档位异常[RoleId:%d,GoodsId:%d,RechargeId:%d]",
			m_pPlayer->GetRoleId(), wGoodsId, dwRechargeId);
		return;
	}

	// 添加购买次数
	pGoods->wBuyTimes++;

	// 主动同步数据给客户端
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


// 发送热卖商品数据
void CHotSaleGoods::SendHotSaleGoodsData()
{
	MSG_S2C_HOT_SALE_GOODS_DATA_RESP msg;
	msg.m_Data = *m_pPlayerHotSaleGoodsInfo;
	m_pPlayer->SendPacket(&msg);
}

// 更新数据
void CHotSaleGoods::UpdateHotSaleGoodsData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_HOT_SALE_GOODS_ID);
	// 新的活动已开始
	if (wDataId != m_pPlayerHotSaleGoodsInfo->wLastActivityDataId)
	{
		m_pPlayerHotSaleGoodsInfo->wLastActivityDataId = wDataId;
		m_pPlayerHotSaleGoodsInfo->wGoodsNum = 0;
	}
}

// 取商品数据
HotSaleGoods* CHotSaleGoods::GetGoods(WORD wGoodsId)
{
	for (WORD i = 0; i < m_pPlayerHotSaleGoodsInfo->wGoodsNum; ++i)
	{
		if (m_pPlayerHotSaleGoodsInfo->arGoodsList[i].wGoodsId == wGoodsId)
			return &m_pPlayerHotSaleGoodsInfo->arGoodsList[i];
	}
	return NULL;
}

// 添加商品
void CHotSaleGoods::AddGoods(const HotSaleGoods &tGoods)
{
	// 检测列表是否已满
	if (m_pPlayerHotSaleGoodsInfo->wGoodsNum >= MAX_HOT_SALE_GOODS_NUM)
	{
		MAPLOG(ERROR_LEVEL, "热卖商品列表已满[RoleId:%d,GoodsId:%d]",m_pPlayer->GetRoleId(), tGoods.wGoodsId);
		return;
	}

	m_pPlayerHotSaleGoodsInfo->arGoodsList[m_pPlayerHotSaleGoodsInfo->wGoodsNum++] = tGoods;
}

// 取当前活动数据ID
WORD CHotSaleGoods::GetCurActivityDataId()
{
	return m_pPlayerHotSaleGoodsInfo->wLastActivityDataId;
}

