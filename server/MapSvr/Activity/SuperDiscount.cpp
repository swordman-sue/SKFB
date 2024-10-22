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

// ����ֵ�ۿ�����
void CSuperDiscount::OnNetSuperDiscountData()
{
	UpdateSuperDiscountData();

	SendSuperDiscountData();
}

// ��ȡ������
void CSuperDiscount::OnNetTakeBuyReward()
{
	// ����Ƿ��ڽ�����
	if (!CActivityManager::IsActivityOpen(SYSTEM_SUPER_DISCOUNT_ID, m_pPlayer))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ֵ�ۿۻû�п���", __FUNCTION__);
		return;
	}

	// ����Ƿ����ȡ
	if (1 != m_pSuperDiscountData->wStatus)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ״̬������ȡ��ֵ�ۿ۽���[status:%d]", __FUNCTION__, m_pSuperDiscountData->wStatus);
		return;
	}

	WORD wActivityDay = m_pActivityMgr->GetActivityDay(SYSTEM_SUPER_DISCOUNT_ID);

	const SuperDiscountBuyReward_Config *pBuyRewardCfg = g_pCfgMgr->GetSuperDiscountBuyReward(wActivityDay);
	if (!pBuyRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������ۿ۽�������[day:%d]", __FUNCTION__, wActivityDay);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(pBuyRewardCfg->vItemList, TRUE, FROM_GET_SUPER_DISCOUNT_BUY_REWARD);

	m_pSuperDiscountData->wStatus = REWARD_STATUS_TAKE;

	// ��Ӧ
	MSG_S2C_SUPER_DISCOUNT_BUY_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ�ۻ�����
void CSuperDiscount::OnNetTakeAccumulateReward(WORD wTakeTimes)
{
	//WORD wTakeTimes = m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1;

	const SuperDiscountAccumulateReward_Config *pAccumulateRewardCfg = g_pCfgMgr->GetSuperDiscountAccumulateReward(wTakeTimes);
	if (!pAccumulateRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������ۿ��ۻ���������[times:%d]", __FUNCTION__, wTakeTimes);
		return;
	}

	// ��⹺��Ĵ����Ƿ��㹻
	WORD wNeedBuyTimes = pAccumulateRewardCfg->wNeedBuyTimes;
	if (m_pSuperDiscountData->wCurBuyTimes < wNeedBuyTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����������,������ȡ��ֵ�ۿ��ۻ�����[need_buy_times:%d]",
			__FUNCTION__, wNeedBuyTimes);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsAccumulateRewardRecord(wTakeTimes))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ۻ���������ȡ��[TakeTimes:%d]", __FUNCTION__, wTakeTimes);
		return;
	}

	// ����б�����
	if (m_pSuperDiscountData->wTookAccumulateRewardNum >= MAX_SUPER_DISCOUNT_ACCUMULATE_REWARD)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ۻ�������¼�б��Ѵ�����[MaxTimes:%d]", __FUNCTION__, m_pSuperDiscountData->wTookAccumulateRewardNum);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(pAccumulateRewardCfg->vItemList, TRUE, FROM_GET_SUPER_DISCOUNT_ACCUMULATE_REWARD);

	// ��¼����ȡ
	m_pSuperDiscountData->arTookAccumulateRewardList[m_pSuperDiscountData->wTookAccumulateRewardNum++] = wTakeTimes;

	SendSuperDiscountData();

	// ��Ӧ
	MSG_S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��ֵ�ص�
void CSuperDiscount::OnRechargeCallback()
{
	UpdateSuperDiscountData();

	// ����Ƿ��ڼ�
	if (!CActivityManager::IsActivityOpen(SYSTEM_SUPER_DISCOUNT_ID, m_pPlayer))
		return;
	
	m_pSuperDiscountData->wCurBuyTimes++;
	m_pSuperDiscountData->wStatus = REWARD_STATUS_BUY;

	SendSuperDiscountData();
}

// �Ƿ��к��
bool CSuperDiscount::IsHaveRedPoint()
{
	// ������
	if (REWARD_STATUS_BUY == m_pSuperDiscountData->wStatus)
		return true;

	// �ۻ�����
// 	const SuperDiscountAccumulateReward_Config *pAccumulateRRewardCfg = NULL;
// 	pAccumulateRRewardCfg = g_pCfgMgr->GetSuperDiscountAccumulateReward(m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1);
// 	if (pAccumulateRRewardCfg && m_pSuperDiscountData->wCurBuyTimes >= pAccumulateRRewardCfg->wNeedBuyTimes)
// 		return true;
	const map<WORD, SuperDiscountAccumulateReward_Config> &mRewardCfgList = g_pCfgMgr->GetSuperDiscountAccumulateRewardList();
	map<WORD, SuperDiscountAccumulateReward_Config>::const_iterator iter = mRewardCfgList.begin();
	for (; iter != mRewardCfgList.end(); ++iter)
	{
		// �����������,��û��ȡ
		if (m_pSuperDiscountData->wCurBuyTimes >= iter->second.wNeedBuyTimes && !IsAccumulateRewardRecord(iter->second.wTimes))
			return true;
	}

	return false;
}

// ���ͳ�ֵ�ۿ�����
void CSuperDiscount::SendSuperDiscountData()
{
	MSG_S2C_SUPER_DISCOUNT_DATA_RESP msg;
	msg.m_Data = *m_pSuperDiscountData;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ��ۻ�������¼
bool CSuperDiscount::IsAccumulateRewardRecord(WORD wTimes)
{
	for (WORD i = 0; i < m_pSuperDiscountData->wTookAccumulateRewardNum; ++i)
	{
		if (m_pSuperDiscountData->arTookAccumulateRewardList[i] == wTimes)
			return true;
	}

	return false;
}

// ��������
void CSuperDiscount::UpdateSuperDiscountData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_SUPER_DISCOUNT_ID);
	// �µĻ�ѿ�ʼ
	if (wDataId != m_pSuperDiscountData->wLastActivityDataId)
	{
		m_pSuperDiscountData->wLastActivityDataId = wDataId;
		m_pSuperDiscountData->wStatus = REWARD_STATUS_NOT_YET;
		m_pSuperDiscountData->wTakeAccumulateRewardTimes = 0;
		m_pSuperDiscountData->wCurBuyTimes = 0;
	}

}























///////////////////////////////////////////////////////////////
// ��ģ��


void CNewSuperDiscount::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pActivityMgr = pActivityMgr;
	m_pPlayer = pPlayer;
	m_pSuperDiscountData = &pPlayer->GetRoleFullInfo().tActivityInfo.tNewSuperDiscountData;
	m_wSystemId = SYSTEM_NEW_SUPER_DISCOUNT_ID;
}

// ����ֵ�ۿ�����
void CNewSuperDiscount::OnNetSuperDiscountData()
{
	UpdateSuperDiscountData();

	SendSuperDiscountData();
}

// ��ȡ������
void CNewSuperDiscount::OnNetTakeBuyReward()
{
	WORD wDataId = m_pSuperDiscountData->wLastActivityDataId;

	// ����Ƿ��ڽ�����
	if (!CActivityManager::IsActivityOpen(m_wSystemId, m_pPlayer))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ֵ�ۿۻû�п���", __FUNCTION__);
		return;
	}

	// ����Ƿ����ȡ
	if (REWARD_STATUS_BUY != m_pSuperDiscountData->wStatus)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ״̬������ȡ��ֵ�ۿ۽���[status:%d]", __FUNCTION__, m_pSuperDiscountData->wStatus);
		return;
	}

	WORD wActivityDay = m_pActivityMgr->GetActivityDay(m_wSystemId);

	const SuperDiscountBuyReward_Config *pBuyRewardCfg = g_pCfgMgr->GetNewSuperDiscountBuyReward(wDataId, wActivityDay);
	if (!pBuyRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������ۿ۽�������[day:%d]", __FUNCTION__, wActivityDay);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(pBuyRewardCfg->vItemList, TRUE, FROM_GET_NEW_SUPER_DISCOUNT_BUY_REWARD);

	m_pSuperDiscountData->wStatus = REWARD_STATUS_TAKE;

	// ��Ӧ
	MSG_S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ�ۻ�����
void CNewSuperDiscount::OnNetTakeAccumulateReward()
{
	WORD wDataId = m_pSuperDiscountData->wLastActivityDataId;

	WORD wTakeTimes = m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1;

	const SuperDiscountAccumulateReward_Config *pAccumulateRewardCfg = g_pCfgMgr->GetNewSuperDiscountAccumulateReward(wDataId, wTakeTimes);
	if (!pAccumulateRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������ۿ��ۻ���������[times:%d]", __FUNCTION__, wTakeTimes);
		return;
	}

	// ��⹺��Ĵ����Ƿ��㹻
	WORD wNeedBuyTimes = pAccumulateRewardCfg->wNeedBuyTimes;
	if (m_pSuperDiscountData->wCurBuyTimes < wNeedBuyTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����������,������ȡ��ֵ�ۿ��ۻ�����[need_buy_times:%d]",
			__FUNCTION__, wNeedBuyTimes);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(pAccumulateRewardCfg->vItemList, TRUE, FROM_GET_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD);

	m_pSuperDiscountData->wCurBuyTimes = 0;
	m_pSuperDiscountData->wTakeAccumulateRewardTimes++;

	SendSuperDiscountData();

	// ��Ӧ
	MSG_S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��ֵ�ص�
void CNewSuperDiscount::OnRechargeCallback()
{
	UpdateSuperDiscountData();

	// ����Ƿ��ڼ�
	if (!CActivityManager::IsActivityOpen(m_wSystemId, m_pPlayer))
		return;

	m_pSuperDiscountData->wCurBuyTimes++;
	m_pSuperDiscountData->wStatus = REWARD_STATUS_BUY;

	SendSuperDiscountData();
}

// �Ƿ��к��
bool CNewSuperDiscount::IsHaveRedPoint()
{
	WORD wDataId = m_pSuperDiscountData->wLastActivityDataId;

	// ������
	if (REWARD_STATUS_BUY == m_pSuperDiscountData->wStatus)
		return true;

	// �ۻ�����
	const SuperDiscountAccumulateReward_Config *pAccumulateRRewardCfg = NULL;
	pAccumulateRRewardCfg = g_pCfgMgr->GetNewSuperDiscountAccumulateReward(wDataId, m_pSuperDiscountData->wTakeAccumulateRewardTimes + 1);
	if (pAccumulateRRewardCfg && m_pSuperDiscountData->wCurBuyTimes >= pAccumulateRRewardCfg->wNeedBuyTimes)
		return true;

	return false;
}

// ���ͳ�ֵ�ۿ�����
void CNewSuperDiscount::SendSuperDiscountData()
{
	MSG_S2C_NEW_SUPER_DISCOUNT_DATA_RESP msg;
	msg.m_Data = *m_pSuperDiscountData;
	m_pPlayer->SendPacket(&msg);
}

// ��������
void CNewSuperDiscount::UpdateSuperDiscountData()
{
	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(m_wSystemId);
	// �µĻ�ѿ�ʼ
	if (wDataId != m_pSuperDiscountData->wLastActivityDataId)
	{
		m_pSuperDiscountData->wLastActivityDataId = wDataId;
		m_pSuperDiscountData->wStatus = REWARD_STATUS_NOT_YET;
		m_pSuperDiscountData->wTakeAccumulateRewardTimes = 0;
		m_pSuperDiscountData->wCurBuyTimes = 0;
	}
}