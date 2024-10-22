#include "stdafx.h"
#include "RechargeReward.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"
#include "..\MapServer.h"


void CRechargeReward::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pRechargeRewardData = &pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeRewardData;
}


// 请求超值折扣数据
void CRechargeReward::OnNetRechargeRewardData()
{
	UpdateActivityData();
	SendRechargeRewardData();
}

// 领取充值奖励
void CRechargeReward::OnNetTakeRechargeReward(WORD wDay)
{
	// 检测要领取的天数是否合法
	if (!wDay || wDay > MAX_RECHARGE_REWARD_DAY_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "领取充值奖励的天数不合法[day:%d]", wDay);
		return;
	}

	WORD wData = GetDailyData(wDay);

	// 是否可以领取
	if (REWARD_STATUS_RECHARGE != wData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "领取充值奖励的天数状态不对[day:%d,data:%d]", wDay, wData);
		return;
	}

	// 取配置
	const RechargeReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeReward(m_pRechargeRewardData->wLastActivityDataId, wDay);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "找不到充值奖励配置[day:%d]", wDay);
		return;
	}

	// 记录已领取
	SetDailyData(wDay, REWARD_STATUS_TAKE);

	// 发放奖励
	m_pPlayer->AddObjectList(pRewardCfg->vRewardList, TRUE, FROM_GET_RECHARGE_REWARD);

	MSG_S2C_TAKE_RECHARGE_REWARD_RESP msg;
	msg.m_wDay = wDay;
	m_pPlayer->SendPacket(&msg);
}

// 领取额外奖励
void CRechargeReward::OnNetTakeExtraReward()
{
	// 检测是否已领取
	if (m_pRechargeRewardData->wIsTookExtraReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "奖励已领取过");
		return;
	}

	// 取额外奖励配置
	const RechargeExtraReward_Config *pExtarRewardCfg = g_pCfgMgr->GetRechargeExtraReward(m_pRechargeRewardData->wLastActivityDataId);
	if (!pExtarRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "取不到充值额外奖励配置[ActivityId:%d]", m_pRechargeRewardData->wLastActivityDataId);
		return;
	}

	// 领取的天数
	WORD wAlreadyRechargeDay = GetAlreadyRechargeDay();
	if (wAlreadyRechargeDay < pExtarRewardCfg->wNeedRechargeDay)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "充值天数不足,无法领取[AlreadyRechargeDay:%d]", wAlreadyRechargeDay);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(pExtarRewardCfg->vRewardList, TRUE, FROM_GET_RECHARGE_EXTRA_REWARD);

	// 记录已领取
	m_pRechargeRewardData->wIsTookExtraReward = true;

	MSG_S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}


// 充值回调
void CRechargeReward::OnRechargeCallback(WORD wRechargeId)
{
	//DWORD dwDay = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	UpdateActivityData();
	
	DWORD dwDay = m_pActivityMgr->GetActivityDay(SYSTEM_RECHARGE_EXTRA_REWARD);
	const RechargeReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeReward(m_pRechargeRewardData->wLastActivityDataId, dwDay);
	if (!pRewardCfg)		
		return;

	// 检测是否指定的档位
	if (pRewardCfg->wRechargeId != wRechargeId)
		return;

	// 检测是否已充过
	if (REWARD_STATUS_NOT_YET != GetDailyData(dwDay))
		return;

	// 设置为已充值过
	SetDailyData(dwDay, REWARD_STATUS_RECHARGE);

	// 主动同步数
	SendRechargeRewardData();
}

// 是否有红点
bool CRechargeReward::IsHaveRedPoint()
{
	for (WORD i = 0; i < MAX_RECHARGE_REWARD_DAY_NUM; ++i)
	{
		if (m_pRechargeRewardData->arDailyDataList[i] == REWARD_STATUS_RECHARGE)
			return true;
	}
	return false;
}

// 发送超值折扣数据
void CRechargeReward::SendRechargeRewardData()
{
	MSG_S2C_RECHARGE_REWARD_DATA_RESP msg;
	msg.m_Data = *m_pRechargeRewardData;
	m_pPlayer->SendPacket(&msg);
}

// 取每日数据
WORD CRechargeReward::GetDailyData(DWORD dwDay)
{
	if (!dwDay || dwDay > MAX_RECHARGE_REWARD_DAY_NUM)
		return 0;

	return m_pRechargeRewardData->arDailyDataList[dwDay-1];
}

// 设置每日数据
void CRechargeReward::SetDailyData(DWORD dwDay, WORD wValue)
{
	if (!dwDay || dwDay > MAX_RECHARGE_REWARD_DAY_NUM)
		return;

	m_pRechargeRewardData->arDailyDataList[dwDay - 1] = wValue;
}

// 取充值的天数
WORD CRechargeReward::GetAlreadyRechargeDay()
{
	WORD wDay = 0;
	for (WORD i = 0; i < MAX_RECHARGE_REWARD_DAY_NUM; ++i)
	{
		if (m_pRechargeRewardData->arDailyDataList[i])
			wDay++;
	}
	return wDay;
}

// 更新活动数据
void CRechargeReward::UpdateActivityData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_RECHARGE_EXTRA_REWARD);
	// 新的活动已开始
	if (wDataId != m_pRechargeRewardData->wLastActivityDataId)
	{
		m_pRechargeRewardData->wLastActivityDataId = wDataId;
		m_pRechargeRewardData->wIsTookExtraReward = false;
		memset(m_pRechargeRewardData->arDailyDataList, 0, sizeof(m_pRechargeRewardData->arDailyDataList));
	}
}