#include "stdafx.h"
#include "SuperDiscount.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"


void CSuperDiscount::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pActivityMgr = pActivityMgr;
	m_pPlayer = pPlayer;
	m_pSuperDiscountData = &pPlayer->GetRoleFullInfo().tActivityInfo.tSuperDiscountData;
}

// 请求超值折扣数据
void CSuperDiscount::OnNetSuperDiscountData()
{
	UpdateSuperDiscountData();

	SendSuperDiscountData();
}

// 领取购买奖励
void CSuperDiscount::OnNetTakeBuyReward()
{
	// 检测活动是否在进行中
	if (!CActivityManager::IsActivityOpen(SYSTEM_SUPER_DISCOUNT_ID, m_pPlayer))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]超值折扣活动没有开启", __FUNCTION__);
		return;
	}

	// 检测是否可领取
	if (1 != m_pSuperDiscountData->wStatus)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]当前状态不可领取超值折扣奖励[status:%d]", __FUNCTION__, m_pSuperDiscountData->wStatus);
		return;
	}

	WORD wActivityDay = m_pActivityMgr->GetActivityDay(SYSTEM_SUPER_DISCOUNT_ID);

	const SuperDiscountBuyReward_Config *pBuyRewardCfg = g_pCfgMgr->GetSuperDiscountBuyReward(wActivityDay);
	if (!pBuyRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到超级折扣奖励配置[day:%d]", __FUNCTION__, wActivityDay);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(pBuyRewardCfg->vItemList, TRUE, FROM_GET_SUPER_DISCOUNT_BUY_REWARD);

	m_pSuperDiscountData->wStatus = REWARD_STATUS_TAKE;

	// 响应
	MSG_S2C_SUPER_DISCOUNT_BUY_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 领取累积奖励
void CSuperDiscount::OnNetTakeAccumulateReward(WORD wTakeTimes)
{
	//WORD wTakeTimes = m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1;

	const SuperDiscountAccumulateReward_Config *pAccumulateRewardCfg = g_pCfgMgr->GetSuperDiscountAccumulateReward(wTakeTimes);
	if (!pAccumulateRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到超级折扣累积奖励配置[times:%d]", __FUNCTION__, wTakeTimes);
		return;
	}

	// 检测购买的次数是否足够
	WORD wNeedBuyTimes = pAccumulateRewardCfg->wNeedBuyTimes;
	if (m_pSuperDiscountData->wCurBuyTimes < wNeedBuyTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]购买次数不足,不能领取超值折扣累积奖励[need_buy_times:%d]",
			__FUNCTION__, wNeedBuyTimes);
		return;
	}

	// 检测是否已领取过
	if (IsAccumulateRewardRecord(wTakeTimes))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]累积奖励已领取过[TakeTimes:%d]", __FUNCTION__, wTakeTimes);
		return;
	}

	// 检测列表上限
	if (m_pSuperDiscountData->wTookAccumulateRewardNum >= MAX_SUPER_DISCOUNT_ACCUMULATE_REWARD)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]累积奖励记录列表已达上限[MaxTimes:%d]", __FUNCTION__, m_pSuperDiscountData->wTookAccumulateRewardNum);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(pAccumulateRewardCfg->vItemList, TRUE, FROM_GET_SUPER_DISCOUNT_ACCUMULATE_REWARD);

	// 记录已领取
	m_pSuperDiscountData->arTookAccumulateRewardList[m_pSuperDiscountData->wTookAccumulateRewardNum++] = wTakeTimes;

	SendSuperDiscountData();

	// 响应
	MSG_S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 充值回调
void CSuperDiscount::OnRechargeCallback()
{
	UpdateSuperDiscountData();

	// 检测是否活动期间
	if (!CActivityManager::IsActivityOpen(SYSTEM_SUPER_DISCOUNT_ID, m_pPlayer))
		return;
	
	m_pSuperDiscountData->wCurBuyTimes++;
	m_pSuperDiscountData->wStatus = REWARD_STATUS_BUY;

	SendSuperDiscountData();
}

// 是否有红点
bool CSuperDiscount::IsHaveRedPoint()
{
	// 购买奖励
	if (REWARD_STATUS_BUY == m_pSuperDiscountData->wStatus)
		return true;

	// 累积奖励
// 	const SuperDiscountAccumulateReward_Config *pAccumulateRRewardCfg = NULL;
// 	pAccumulateRRewardCfg = g_pCfgMgr->GetSuperDiscountAccumulateReward(m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1);
// 	if (pAccumulateRRewardCfg && m_pSuperDiscountData->wCurBuyTimes >= pAccumulateRRewardCfg->wNeedBuyTimes)
// 		return true;
	const map<WORD, SuperDiscountAccumulateReward_Config> &mRewardCfgList = g_pCfgMgr->GetSuperDiscountAccumulateRewardList();
	map<WORD, SuperDiscountAccumulateReward_Config>::const_iterator iter = mRewardCfgList.begin();
	for (; iter != mRewardCfgList.end(); ++iter)
	{
		// 购买次数满足,还没领取
		if (m_pSuperDiscountData->wCurBuyTimes >= iter->second.wNeedBuyTimes && !IsAccumulateRewardRecord(iter->second.wTimes))
			return true;
	}

	return false;
}

// 发送超值折扣数据
void CSuperDiscount::SendSuperDiscountData()
{
	MSG_S2C_SUPER_DISCOUNT_DATA_RESP msg;
	msg.m_Data = *m_pSuperDiscountData;
	m_pPlayer->SendPacket(&msg);
}

// 是否累积奖励记录
bool CSuperDiscount::IsAccumulateRewardRecord(WORD wTimes)
{
	for (WORD i = 0; i < m_pSuperDiscountData->wTookAccumulateRewardNum; ++i)
	{
		if (m_pSuperDiscountData->arTookAccumulateRewardList[i] == wTimes)
			return true;
	}

	return false;
}

// 更新数据
void CSuperDiscount::UpdateSuperDiscountData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_SUPER_DISCOUNT_ID);
	// 新的活动已开始
	if (wDataId != m_pSuperDiscountData->wLastActivityDataId)
	{
		m_pSuperDiscountData->wLastActivityDataId = wDataId;
		m_pSuperDiscountData->wStatus = REWARD_STATUS_NOT_YET;
		m_pSuperDiscountData->wTakeAccumulateRewardTimes = 0;
		m_pSuperDiscountData->wCurBuyTimes = 0;
	}

}























///////////////////////////////////////////////////////////////
// 新模块


void CNewSuperDiscount::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pActivityMgr = pActivityMgr;
	m_pPlayer = pPlayer;
	m_pSuperDiscountData = &pPlayer->GetRoleFullInfo().tActivityInfo.tNewSuperDiscountData;
	m_wSystemId = SYSTEM_NEW_SUPER_DISCOUNT_ID;
}

// 请求超值折扣数据
void CNewSuperDiscount::OnNetSuperDiscountData()
{
	UpdateSuperDiscountData();

	SendSuperDiscountData();
}

// 领取购买奖励
void CNewSuperDiscount::OnNetTakeBuyReward()
{
	WORD wDataId = m_pSuperDiscountData->wLastActivityDataId;

	// 检测活动是否在进行中
	if (!CActivityManager::IsActivityOpen(m_wSystemId, m_pPlayer))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]超值折扣活动没有开启", __FUNCTION__);
		return;
	}

	// 检测是否可领取
	if (REWARD_STATUS_BUY != m_pSuperDiscountData->wStatus)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]当前状态不可领取超值折扣奖励[status:%d]", __FUNCTION__, m_pSuperDiscountData->wStatus);
		return;
	}

	WORD wActivityDay = m_pActivityMgr->GetActivityDay(m_wSystemId);

	const SuperDiscountBuyReward_Config *pBuyRewardCfg = g_pCfgMgr->GetNewSuperDiscountBuyReward(wDataId, wActivityDay);
	if (!pBuyRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到超级折扣奖励配置[day:%d]", __FUNCTION__, wActivityDay);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(pBuyRewardCfg->vItemList, TRUE, FROM_GET_NEW_SUPER_DISCOUNT_BUY_REWARD);

	m_pSuperDiscountData->wStatus = REWARD_STATUS_TAKE;

	// 响应
	MSG_S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 领取累积奖励
void CNewSuperDiscount::OnNetTakeAccumulateReward()
{
	WORD wDataId = m_pSuperDiscountData->wLastActivityDataId;

	WORD wTakeTimes = m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1;

	const SuperDiscountAccumulateReward_Config *pAccumulateRewardCfg = g_pCfgMgr->GetNewSuperDiscountAccumulateReward(wDataId, wTakeTimes);
	if (!pAccumulateRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到超级折扣累积奖励配置[times:%d]", __FUNCTION__, wTakeTimes);
		return;
	}

	// 检测购买的次数是否足够
	WORD wNeedBuyTimes = pAccumulateRewardCfg->wNeedBuyTimes;
	if (m_pSuperDiscountData->wCurBuyTimes < wNeedBuyTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]购买次数不足,不能领取超值折扣累积奖励[need_buy_times:%d]",
			__FUNCTION__, wNeedBuyTimes);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(pAccumulateRewardCfg->vItemList, TRUE, FROM_GET_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD);

	m_pSuperDiscountData->wCurBuyTimes = 0;
	m_pSuperDiscountData->wTakeAccumulateRewardTimes++;

	SendSuperDiscountData();

	// 响应
	MSG_S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 充值回调
void CNewSuperDiscount::OnRechargeCallback()
{
	UpdateSuperDiscountData();

	// 检测是否活动期间
	if (!CActivityManager::IsActivityOpen(m_wSystemId, m_pPlayer))
		return;

	m_pSuperDiscountData->wCurBuyTimes++;
	m_pSuperDiscountData->wStatus = REWARD_STATUS_BUY;

	SendSuperDiscountData();
}

// 是否有红点
bool CNewSuperDiscount::IsHaveRedPoint()
{
	WORD wDataId = m_pSuperDiscountData->wLastActivityDataId;

	// 购买奖励
	if (REWARD_STATUS_BUY == m_pSuperDiscountData->wStatus)
		return true;

	// 累积奖励
	const SuperDiscountAccumulateReward_Config *pAccumulateRRewardCfg = NULL;
	pAccumulateRRewardCfg = g_pCfgMgr->GetNewSuperDiscountAccumulateReward(wDataId, m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1);
	if (pAccumulateRRewardCfg && m_pSuperDiscountData->wCurBuyTimes >= pAccumulateRRewardCfg->wNeedBuyTimes)
		return true;

	return false;
}

// 发送超值折扣数据
void CNewSuperDiscount::SendSuperDiscountData()
{
	MSG_S2C_NEW_SUPER_DISCOUNT_DATA_RESP msg;
	msg.m_Data = *m_pSuperDiscountData;
	m_pPlayer->SendPacket(&msg);
}

// 更新数据
void CNewSuperDiscount::UpdateSuperDiscountData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(m_wSystemId);
	// 新的活动已开始
	if (wDataId != m_pSuperDiscountData->wLastActivityDataId)
	{
		m_pSuperDiscountData->wLastActivityDataId = wDataId;
		m_pSuperDiscountData->wStatus = REWARD_STATUS_NOT_YET;
		m_pSuperDiscountData->wTakeAccumulateRewardTimes = 0;
		m_pSuperDiscountData->wCurBuyTimes = 0;
	}
}