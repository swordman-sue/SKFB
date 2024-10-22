#include "stdafx.h"
#include "AccurateRecommend.h"
#include "..\Player.h"
#include "..\ConfigManager.h"

void CAccurateRecommend::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pAccurateRecommendData = &pPlayer->GetRoleFullInfo().tActivityInfo.tAccurateRecommendData;
}

void CAccurateRecommend::OnNewDay()
{
	if (!m_pAccurateRecommendData->wGoodsNum)
		return;

	PlayerAccurateRecommendData tTmpData;
	memset(&tTmpData, 0, sizeof(tTmpData));
	for (WORD i = 0; i < m_pAccurateRecommendData->wGoodsNum; ++i)
	{
		AccurateRecommendGoods &tGoods = m_pAccurateRecommendData->arGoodsList[i];
		// 保留有奖励可领取的
		if (tGoods.wCanTakeTimes)
		{
			tGoods.wBuyTimes = 0;
			tTmpData.arGoodsList[tTmpData.wGoodsNum++] = tGoods;
		}
	}

	int nDataSize = sizeof(PlayerAccurateRecommendData);
	memcpy_s(m_pAccurateRecommendData, nDataSize, &tTmpData, nDataSize);
}

// 请求准推荐数据
void CAccurateRecommend::OnNetAccurateRecommendData()
{
	SendAccurateRecommendData();
}

// 领取准推荐礼包
void CAccurateRecommend::OnNetTakeAccurateRecommendGift(WORD wGoodsId)
{
	// 取商品配置
	const AccurateRecommendGoods_Config *pGoodsCfg = g_pCfgMgr->GetAccurateRecommendGoods(wGoodsId);
	if (!pGoodsCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精准推荐礼包数据[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// 取商品数据
	AccurateRecommendGoods *pGoods = GetAccurateRecommendGoods(wGoodsId);
	if (!pGoods)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精准推荐商品数据[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// 检测可以领取的次数
	if (!pGoods->wCanTakeTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]精准推荐商品不能领取[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// 更新可领取次数
	pGoods->wCanTakeTimes--;

	// 发奖励
	m_pPlayer->AddObject(pGoodsCfg->dwItemId, pGoodsCfg->dwItemNum, true, FROM_RECOMMEND_GIFT);

	// 主动同步数据
	//SendAccurateRecommendData();

	// 响应
	MSG_S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP msg;
	msg.m_wGoodsId = wGoodsId;
	m_pPlayer->SendPacket(&msg);
}

// 充值回调
void CAccurateRecommend::OnRechargeCallback(WORD wGoodsId)
{
	const AccurateRecommendGoods_Config *pGoodsCfg = g_pCfgMgr->GetAccurateRecommendGoods(wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精准推荐礼包数据[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	AccurateRecommendGoods *pGoods = GetAccurateRecommendGoods(wGoodsId);
	if (pGoods)
	{
		// 检测可以购买次数
		if (pGoods->wBuyTimes >= pGoodsCfg->wBuyNumMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]精准推荐商品可购买次数已达上限[wGoodsId:%d]", __FUNCTION__, wGoodsId);
			return;
		}

		pGoods->wBuyTimes++;
		pGoods->wCanTakeTimes++;
	}
	else
	{
		AddAccurateRecommendGoods(wGoodsId);
	}

	//SendAccurateRecommendData();
}

// 是否有红点
bool CAccurateRecommend::IsHaveRedPoint()
{
	return true;
}

// 发送精准推荐数据
void CAccurateRecommend::SendAccurateRecommendData()
{
	MSG_S2C_ACCURATE_RECOMMEND_DATA_RESP msg;
	msg.m_Data = *m_pAccurateRecommendData;
	m_pPlayer->SendPacket(&msg);
}

// 取精准推荐数据
AccurateRecommendGoods* CAccurateRecommend::GetAccurateRecommendGoods(WORD wGoodsId)
{
	for (WORD i = 0; i < m_pAccurateRecommendData->wGoodsNum; ++i)
	{
		if (m_pAccurateRecommendData->arGoodsList[i].wGoodsId == wGoodsId)
			return &m_pAccurateRecommendData->arGoodsList[i];
	}
	return NULL;
}

// 添加精准推荐数据
void CAccurateRecommend::AddAccurateRecommendGoods(WORD wGoodsId)
{
	if (m_pAccurateRecommendData->wGoodsNum >= MAX_ACCURATE_RECOMMEND_GOODS_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精准推荐数据列表已满", __FUNCTION__);
		return;
	}

	AccurateRecommendGoods tGoods;
	tGoods.wGoodsId = wGoodsId;
	tGoods.wBuyTimes = 1;
	tGoods.wCanTakeTimes = 1;
	m_pAccurateRecommendData->arGoodsList[m_pAccurateRecommendData->wGoodsNum++] = tGoods;
}