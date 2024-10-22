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
		// �����н�������ȡ��
		if (tGoods.wCanTakeTimes)
		{
			tGoods.wBuyTimes = 0;
			tTmpData.arGoodsList[tTmpData.wGoodsNum++] = tGoods;
		}
	}

	int nDataSize = sizeof(PlayerAccurateRecommendData);
	memcpy_s(m_pAccurateRecommendData, nDataSize, &tTmpData, nDataSize);
}

// ����׼�Ƽ�����
void CAccurateRecommend::OnNetAccurateRecommendData()
{
	SendAccurateRecommendData();
}

// ��ȡ׼�Ƽ����
void CAccurateRecommend::OnNetTakeAccurateRecommendGift(WORD wGoodsId)
{
	// ȡ��Ʒ����
	const AccurateRecommendGoods_Config *pGoodsCfg = g_pCfgMgr->GetAccurateRecommendGoods(wGoodsId);
	if (!pGoodsCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����׼�Ƽ��������[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// ȡ��Ʒ����
	AccurateRecommendGoods *pGoods = GetAccurateRecommendGoods(wGoodsId);
	if (!pGoods)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����׼�Ƽ���Ʒ����[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// ��������ȡ�Ĵ���
	if (!pGoods->wCanTakeTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��׼�Ƽ���Ʒ������ȡ[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// ���¿���ȡ����
	pGoods->wCanTakeTimes--;

	// ������
	m_pPlayer->AddObject(pGoodsCfg->dwItemId, pGoodsCfg->dwItemNum, true, FROM_RECOMMEND_GIFT);

	// ����ͬ������
	//SendAccurateRecommendData();

	// ��Ӧ
	MSG_S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP msg;
	msg.m_wGoodsId = wGoodsId;
	m_pPlayer->SendPacket(&msg);
}

// ��ֵ�ص�
void CAccurateRecommend::OnRechargeCallback(WORD wGoodsId)
{
	const AccurateRecommendGoods_Config *pGoodsCfg = g_pCfgMgr->GetAccurateRecommendGoods(wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����׼�Ƽ��������[wGoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	AccurateRecommendGoods *pGoods = GetAccurateRecommendGoods(wGoodsId);
	if (pGoods)
	{
		// �����Թ������
		if (pGoods->wBuyTimes >= pGoodsCfg->wBuyNumMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��׼�Ƽ���Ʒ�ɹ�������Ѵ�����[wGoodsId:%d]", __FUNCTION__, wGoodsId);
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

// �Ƿ��к��
bool CAccurateRecommend::IsHaveRedPoint()
{
	return true;
}

// ���;�׼�Ƽ�����
void CAccurateRecommend::SendAccurateRecommendData()
{
	MSG_S2C_ACCURATE_RECOMMEND_DATA_RESP msg;
	msg.m_Data = *m_pAccurateRecommendData;
	m_pPlayer->SendPacket(&msg);
}

// ȡ��׼�Ƽ�����
AccurateRecommendGoods* CAccurateRecommend::GetAccurateRecommendGoods(WORD wGoodsId)
{
	for (WORD i = 0; i < m_pAccurateRecommendData->wGoodsNum; ++i)
	{
		if (m_pAccurateRecommendData->arGoodsList[i].wGoodsId == wGoodsId)
			return &m_pAccurateRecommendData->arGoodsList[i];
	}
	return NULL;
}

// ��Ӿ�׼�Ƽ�����
void CAccurateRecommend::AddAccurateRecommendGoods(WORD wGoodsId)
{
	if (m_pAccurateRecommendData->wGoodsNum >= MAX_ACCURATE_RECOMMEND_GOODS_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��׼�Ƽ������б�����", __FUNCTION__);
		return;
	}

	AccurateRecommendGoods tGoods;
	tGoods.wGoodsId = wGoodsId;
	tGoods.wBuyTimes = 1;
	tGoods.wCanTakeTimes = 1;
	m_pAccurateRecommendData->arGoodsList[m_pAccurateRecommendData->wGoodsNum++] = tGoods;
}