#include "stdafx.h"
#include "TotalRechargeReward.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"
#include "..\MapServer.h"


void CTotalRechargeReward::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pTotalRechargeRewardData = &pPlayer->GetRoleFullInfo().tActivityInfo.tTotalRechargeRewardData;
}

// 请求累充数据
void CTotalRechargeReward::OnNetTotalRechargeData()
{
	UpdateActivityData();
	SendTotalRechargeData();
}

// 领取累充奖励
void CTotalRechargeReward::OnNetTakeTotalRechargeReward(WORD wRewardId)
{
	// 取累充奖励配置
	WORD wActivityDataId = m_pTotalRechargeRewardData->wLastActivityDataId;
	const TotalRechargeReward_Config *pRewardCfg = g_pCfgMgr->GetTotalRechargeReward(wActivityDataId, wRewardId);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "取不到累充奖励配置[DataId:%d,RewardId:%d]", wActivityDataId,wRewardId);
		return;
	}

	// 检测累充额度是否足够
	if (m_pTotalRechargeRewardData->dwTotalAmount < pRewardCfg->dwTotalRchargetAmount)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "累充额度不足,不能领取[RewardId:%d]", wRewardId);
		return;
	}

	// 检测是否已领取过
	if (IsRewardRecord(wRewardId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "累充奖励已被领取过[RewardId:%d]", wRewardId);
		return;
	}

	// 添加记录
	AddRewardRecord(wRewardId);

	// 发奖励
	m_pPlayer->AddObjectList(pRewardCfg->vRewardList, TRUE, FROM_GET_TOTAL_RECHARGE_REWARD);

	// 响应
	MSG_S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP msg;
	msg.m_wRewardId = wRewardId;
	m_pPlayer->SendPacket(&msg);
}

// 发送累充数据
void CTotalRechargeReward::SendTotalRechargeData()
{
	MSG_S2C_TOTAL_RECHARGE_REWARD_DATA_RESP msg;
	msg.m_Data = *m_pTotalRechargeRewardData;
	m_pPlayer->SendPacket(&msg);
}

// 检测是否奖励记录
bool CTotalRechargeReward::IsRewardRecord(WORD wRewardId)
{
	for (WORD i = 0; i < m_pTotalRechargeRewardData->wRewardRecordNum; ++i)
	{
		if (m_pTotalRechargeRewardData->arRewardRecordList[i] == wRewardId)
			return true;
	}
	return false;
}

// 添加奖励记录
void CTotalRechargeReward::AddRewardRecord(WORD wRewardId)
{
	// 检测列表是否已满
	if (m_pTotalRechargeRewardData->wRewardRecordNum >= MAX_TOTAL_RECHARGE_REWARD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "累充奖励列表已满[Num:%d]", m_pTotalRechargeRewardData->wRewardRecordNum);
		return;
	}

	m_pTotalRechargeRewardData->arRewardRecordList[m_pTotalRechargeRewardData->wRewardRecordNum++] = wRewardId;
}

// 充值通知事件
void CTotalRechargeReward::OnRechargeNotify(DWORD dwAmount)
{
	UpdateActivityData();

	// 检测是否活动期间
	if (!m_pActivityMgr->IsActivityOpen(SYSTEM_TOTAL_RECHARGE_REWARD, m_pPlayer))
		return;

	// 累加
	m_pTotalRechargeRewardData->dwTotalAmount += dwAmount;

	// 主动同步客户端
	SendTotalRechargeData();
}

// 更新活动数据
void CTotalRechargeReward::UpdateActivityData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_TOTAL_RECHARGE_REWARD);
	// 新的活动已开始
	if (wDataId != m_pTotalRechargeRewardData->wLastActivityDataId)
	{
		m_pTotalRechargeRewardData->wLastActivityDataId = wDataId;
		m_pTotalRechargeRewardData->dwTotalAmount = 0;
		m_pTotalRechargeRewardData->wRewardRecordNum = 0;
	}
}