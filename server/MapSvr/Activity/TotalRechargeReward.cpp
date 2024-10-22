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

// �����۳�����
void CTotalRechargeReward::OnNetTotalRechargeData()
{
	UpdateActivityData();
	SendTotalRechargeData();
}

// ��ȡ�۳佱��
void CTotalRechargeReward::OnNetTakeTotalRechargeReward(WORD wRewardId)
{
	// ȡ�۳佱������
	WORD wActivityDataId = m_pTotalRechargeRewardData->wLastActivityDataId;
	const TotalRechargeReward_Config *pRewardCfg = g_pCfgMgr->GetTotalRechargeReward(wActivityDataId, wRewardId);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "ȡ�����۳佱������[DataId:%d,RewardId:%d]", wActivityDataId,wRewardId);
		return;
	}

	// ����۳����Ƿ��㹻
	if (m_pTotalRechargeRewardData->dwTotalAmount < pRewardCfg->dwTotalRchargetAmount)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�۳��Ȳ���,������ȡ[RewardId:%d]", wRewardId);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsRewardRecord(wRewardId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�۳佱���ѱ���ȡ��[RewardId:%d]", wRewardId);
		return;
	}

	// ��Ӽ�¼
	AddRewardRecord(wRewardId);

	// ������
	m_pPlayer->AddObjectList(pRewardCfg->vRewardList, TRUE, FROM_GET_TOTAL_RECHARGE_REWARD);

	// ��Ӧ
	MSG_S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP msg;
	msg.m_wRewardId = wRewardId;
	m_pPlayer->SendPacket(&msg);
}

// �����۳�����
void CTotalRechargeReward::SendTotalRechargeData()
{
	MSG_S2C_TOTAL_RECHARGE_REWARD_DATA_RESP msg;
	msg.m_Data = *m_pTotalRechargeRewardData;
	m_pPlayer->SendPacket(&msg);
}

// ����Ƿ�����¼
bool CTotalRechargeReward::IsRewardRecord(WORD wRewardId)
{
	for (WORD i = 0; i < m_pTotalRechargeRewardData->wRewardRecordNum; ++i)
	{
		if (m_pTotalRechargeRewardData->arRewardRecordList[i] == wRewardId)
			return true;
	}
	return false;
}

// ��ӽ�����¼
void CTotalRechargeReward::AddRewardRecord(WORD wRewardId)
{
	// ����б��Ƿ�����
	if (m_pTotalRechargeRewardData->wRewardRecordNum >= MAX_TOTAL_RECHARGE_REWARD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "�۳佱���б�����[Num:%d]", m_pTotalRechargeRewardData->wRewardRecordNum);
		return;
	}

	m_pTotalRechargeRewardData->arRewardRecordList[m_pTotalRechargeRewardData->wRewardRecordNum++] = wRewardId;
}

// ��ֵ֪ͨ�¼�
void CTotalRechargeReward::OnRechargeNotify(DWORD dwAmount)
{
	UpdateActivityData();

	// ����Ƿ��ڼ�
	if (!m_pActivityMgr->IsActivityOpen(SYSTEM_TOTAL_RECHARGE_REWARD, m_pPlayer))
		return;

	// �ۼ�
	m_pTotalRechargeRewardData->dwTotalAmount += dwAmount;

	// ����ͬ���ͻ���
	SendTotalRechargeData();
}

// ���»����
void CTotalRechargeReward::UpdateActivityData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_TOTAL_RECHARGE_REWARD);
	// �µĻ�ѿ�ʼ
	if (wDataId != m_pTotalRechargeRewardData->wLastActivityDataId)
	{
		m_pTotalRechargeRewardData->wLastActivityDataId = wDataId;
		m_pTotalRechargeRewardData->dwTotalAmount = 0;
		m_pTotalRechargeRewardData->wRewardRecordNum = 0;
	}
}