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


// ����ֵ�ۿ�����
void CRechargeReward::OnNetRechargeRewardData()
{
	UpdateActivityData();
	SendRechargeRewardData();
}

// ��ȡ��ֵ����
void CRechargeReward::OnNetTakeRechargeReward(WORD wDay)
{
	// ���Ҫ��ȡ�������Ƿ�Ϸ�
	if (!wDay || wDay > MAX_RECHARGE_REWARD_DAY_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "��ȡ��ֵ�������������Ϸ�[day:%d]", wDay);
		return;
	}

	WORD wData = GetDailyData(wDay);

	// �Ƿ������ȡ
	if (REWARD_STATUS_RECHARGE != wData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "��ȡ��ֵ����������״̬����[day:%d,data:%d]", wDay, wData);
		return;
	}

	// ȡ����
	const RechargeReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeReward(m_pRechargeRewardData->wLastActivityDataId, wDay);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�Ҳ�����ֵ��������[day:%d]", wDay);
		return;
	}

	// ��¼����ȡ
	SetDailyData(wDay, REWARD_STATUS_TAKE);

	// ���Ž���
	m_pPlayer->AddObjectList(pRewardCfg->vRewardList, TRUE, FROM_GET_RECHARGE_REWARD);

	MSG_S2C_TAKE_RECHARGE_REWARD_RESP msg;
	msg.m_wDay = wDay;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ���⽱��
void CRechargeReward::OnNetTakeExtraReward()
{
	// ����Ƿ�����ȡ
	if (m_pRechargeRewardData->wIsTookExtraReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "��������ȡ��");
		return;
	}

	// ȡ���⽱������
	const RechargeExtraReward_Config *pExtarRewardCfg = g_pCfgMgr->GetRechargeExtraReward(m_pRechargeRewardData->wLastActivityDataId);
	if (!pExtarRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "ȡ������ֵ���⽱������[ActivityId:%d]", m_pRechargeRewardData->wLastActivityDataId);
		return;
	}

	// ��ȡ������
	WORD wAlreadyRechargeDay = GetAlreadyRechargeDay();
	if (wAlreadyRechargeDay < pExtarRewardCfg->wNeedRechargeDay)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "��ֵ��������,�޷���ȡ[AlreadyRechargeDay:%d]", wAlreadyRechargeDay);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(pExtarRewardCfg->vRewardList, TRUE, FROM_GET_RECHARGE_EXTRA_REWARD);

	// ��¼����ȡ
	m_pRechargeRewardData->wIsTookExtraReward = true;

	MSG_S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}


// ��ֵ�ص�
void CRechargeReward::OnRechargeCallback(WORD wRechargeId)
{
	//DWORD dwDay = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	UpdateActivityData();
	
	DWORD dwDay = m_pActivityMgr->GetActivityDay(SYSTEM_RECHARGE_EXTRA_REWARD);
	const RechargeReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeReward(m_pRechargeRewardData->wLastActivityDataId, dwDay);
	if (!pRewardCfg)		
		return;

	// ����Ƿ�ָ���ĵ�λ
	if (pRewardCfg->wRechargeId != wRechargeId)
		return;

	// ����Ƿ��ѳ��
	if (REWARD_STATUS_NOT_YET != GetDailyData(dwDay))
		return;

	// ����Ϊ�ѳ�ֵ��
	SetDailyData(dwDay, REWARD_STATUS_RECHARGE);

	// ����ͬ����
	SendRechargeRewardData();
}

// �Ƿ��к��
bool CRechargeReward::IsHaveRedPoint()
{
	for (WORD i = 0; i < MAX_RECHARGE_REWARD_DAY_NUM; ++i)
	{
		if (m_pRechargeRewardData->arDailyDataList[i] == REWARD_STATUS_RECHARGE)
			return true;
	}
	return false;
}

// ���ͳ�ֵ�ۿ�����
void CRechargeReward::SendRechargeRewardData()
{
	MSG_S2C_RECHARGE_REWARD_DATA_RESP msg;
	msg.m_Data = *m_pRechargeRewardData;
	m_pPlayer->SendPacket(&msg);
}

// ȡÿ������
WORD CRechargeReward::GetDailyData(DWORD dwDay)
{
	if (!dwDay || dwDay > MAX_RECHARGE_REWARD_DAY_NUM)
		return 0;

	return m_pRechargeRewardData->arDailyDataList[dwDay-1];
}

// ����ÿ������
void CRechargeReward::SetDailyData(DWORD dwDay, WORD wValue)
{
	if (!dwDay || dwDay > MAX_RECHARGE_REWARD_DAY_NUM)
		return;

	m_pRechargeRewardData->arDailyDataList[dwDay - 1] = wValue;
}

// ȡ��ֵ������
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

// ���»����
void CRechargeReward::UpdateActivityData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_RECHARGE_EXTRA_REWARD);
	// �µĻ�ѿ�ʼ
	if (wDataId != m_pRechargeRewardData->wLastActivityDataId)
	{
		m_pRechargeRewardData->wLastActivityDataId = wDataId;
		m_pRechargeRewardData->wIsTookExtraReward = false;
		memset(m_pRechargeRewardData->arDailyDataList, 0, sizeof(m_pRechargeRewardData->arDailyDataList));
	}
}