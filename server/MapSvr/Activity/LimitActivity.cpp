#include "stdafx.h"
#include "LimitActivity.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\Recharge\RechargeManager.h"
#include "ActivityManager.h"

void CLimitActivity::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pLimitActivityData = &pPlayer->GetRoleFullInfo().tActivityInfo.tLimitActivityData;
}

// �׳���������
void CLimitActivity::OnNetFirstRechargeData()
{
	SendFirstRechargeData();
}

// ������������
// void CLimitActivity::OnNetSingleRechargeData()
// {
// 	SendSingleRechargeData();
// }
// 
// // �۳���������
// void CLimitActivity::OnNetAccumulateRechargeData()
// {
// 	SendAccumulateRechargeData();
// }
// 
// // ��ʱ��ļ��������
// void CLimitActivity::OnNetRecruitData()
// {
// 	SendRecruitData();
// }

// ��ȡ�����
void CLimitActivity::OnNetTakeActivityReward(WORD wDataId)
{
	const LimitActivity_Config *pActivityCfg = g_pCfgMgr->GetLimitActivity(wDataId);
	if (!pActivityCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ʱ�����[data_id:%d]", __FUNCTION__, wDataId);
		return;
	}

	// todo::���ʱ��

	bool bResult = false;
	switch (pActivityCfg->wType)
	{
// 	case LIMIT_ACTIVITY_SINGLE:
// 		bResult = DoTakeSingleRechargeReward(pActivityCfg);
// 		break;
// 	case LIMIT_ACTIVITY_ACCUMULATE:
// 		bResult = DoTakeAccumulateRechargeReward(pActivityCfg);
// 		break;
	case LIMIT_ACTIVITY_FIRST:
		bResult = DoTakeFirstRechargeReward(pActivityCfg);
		break;
// 	case LIMIT_ACTIVITY_RECRUIT:
// 		bResult = DoTakeLimitRecruitReward(pActivityCfg);
// 		break;
	default:
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW,C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]ľ�д������ʱ�����[type:%d,data_id:%d]",
				__FUNCTION__, pActivityCfg->wType, wDataId);
		}
		break;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ��ʱ�����ʧ��[data_id:%d,type:%d]", 
			__FUNCTION__, wDataId, pActivityCfg->wType);
		return;
	}

	// ��Ӧ
	MSG_S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP msg;
	msg.m_wDataId = wDataId;
	m_pPlayer->SendPacket(&msg);
}

// ������ȡ�׳佱��
bool CLimitActivity::DoTakeFirstRechargeReward(const LimitActivity_Config *pActivityCfg)
{
	WORD wDataId = pActivityCfg->wDataId;

	// ����Ƿ��ѳ�ֵ��
	if (m_pLimitActivityData->dwTotleRecharge < pActivityCfg->dwContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�׳��ۻ���Ȳ���,������ȡ[data_id:%d]", __FUNCTION__, wDataId);
		return false;
	}

	// ����Ƿ�����ȡ��
	if (IsTakeFirstRechargeReward(wDataId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�۳�Ľ�������ȡ��[data_id:%d]", __FUNCTION__, wDataId);
		return false;
	}

	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����ʱ������¼ʧ��[data_id:%d]", __FUNCTION__, wDataId);
		return false;
	}

	m_pPlayer->AddObjectList(pActivityCfg->vRewardItemList, TRUE, FROM_GET_LIMIT_FIRST_RECHARGE_REWARD);

	return true;
}

// ������ȡ���佱��
// bool CLimitActivity::DoTakeSingleRechargeReward(const LimitActivity_Config *pActivityCfg)
// {
// 	UpdateActivityData(SYSTEM_SGL_RCG);
// 
// 	WORD wDataId = pActivityCfg->wDataId;
// 
// 	// ȡ��¼����
// 	RecordData *pRecordData = FindSingleRechargeRewardRecord(wDataId);
// 
// 	// �Ѿ���ȡ������
// 	WORD wTakeNum = pRecordData ? pRecordData->wRecordNum : 0;
// 
// 	// �����ȡ�����Ƿ��Ѵ�����(��������)
// 	if (wTakeNum >= pActivityCfg->wRewardTimesMax)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ȡ�����Ѵ�����,������ȡ����(��������)[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	// �����ȡ�����Ƿ��Ѵ�����(��ֵ����)
// 	if (wTakeNum >= GetRechargeTimes(pActivityCfg->dwContext))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ȡ�����Ѵ�����,������ȡ����(��ֵ����)[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�����ʱ������¼ʧ��[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	m_pPlayer->AddObject(pActivityCfg->vRewardItemList);
// 
// 	return true;
// }
// 
// // ������ȡ�۳佱��
// bool CLimitActivity::DoTakeAccumulateRechargeReward(const LimitActivity_Config *pActivityCfg)
// {
// 	UpdateActivityData(SYSTEM_AMT_RCG);
// 
// 	WORD wDataId = pActivityCfg->wDataId;
// 
// 	// ������Ƿ��㹻
// 	if (m_pLimitActivityData->dwAccumulateRecharge < pActivityCfg->dwContext)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�۳�Ķ�Ȳ���,������ȡ����[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	// ����Ƿ�����ȡ��
// 	if (IsTakeAccumulateRechargeReward(wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�۳�Ľ�������ȡ��[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�����ʱ������¼ʧ��[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	m_pPlayer->AddObject(pActivityCfg->vRewardItemList);
// 
// 	return true;
// }
// 
// // ������ʱ��ļ����
// bool CLimitActivity::DoTakeLimitRecruitReward(const LimitActivity_Config *pActivityCfg)
// {
// 	WORD wDataId = pActivityCfg->wDataId;
// 
// 	// �����ļ�����Ƿ��㹻
// 	if (m_pLimitActivityData->wRecruitTimes < pActivityCfg->dwContext)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ļ��������,������ȡ����[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	// ����Ƿ�����ȡ��
// 	if (IsTakeLimitRecruitReward(wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�۳�Ľ�������ȡ��[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�����ʱ������¼ʧ��[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	m_pPlayer->AddObject(pActivityCfg->vRewardItemList);
// 
// 	return true;
// }

// ����۳�ֵ
void CLimitActivity::AddRechargeValue(DWORD dwValue)
{
	//UpdateActivityData(SYSTEM_AMT_RCG);
	///m_pLimitActivityData->dwAccumulateRecharge += dwValue;

	m_pLimitActivityData->dwTotleRecharge += dwValue;
}

// ��ӳ�ֵ����
// void CLimitActivity::AddRechargeData(WORD wRechargeId)
// {
// 	UpdateActivityData(SYSTEM_SGL_RCG);
// 
// 	CommonRecharge *pRecharge = FindCommonRecharge(wRechargeId);
// 	if (pRecharge)
// 	{
// 		pRecharge->wRechargeTimes++;
// 	}
// 	else
// 	{
// 		if (m_pLimitActivityData->wRechargeNum >= MAX_RECHARGE_NUM)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]��ֵ��¼�б�����", __FUNCTION__);
// 			return;
// 		}
// 
// 		CommonRecharge tRecharge;
// 		tRecharge.wRechargeId = wRechargeId;
// 		tRecharge.wRechargeTimes = 1;
// 		m_pLimitActivityData->arRechargeList[m_pLimitActivityData->wRechargeNum++] = tRecharge;
// 	}
// }
// 
// 
// // �����ļ����
// void CLimitActivity::AddRecruitTimes(WORD wRecruitTimes)
// {
// 	m_pLimitActivityData->wRecruitTimes += wRecruitTimes;
// }

// �Ƿ��к��
bool CLimitActivity::IsHaveRedPoint(WORD wSystemId)
{
	map<DWORD, LimitActivity_Config>::iterator iter = g_pCfgMgr->m_LimitActivityList.begin();
	if (iter == g_pCfgMgr->m_LimitActivityList.end())
		return false;

	// 74���η���
// 	if (SYSTEM_SGL_RCG == wSystemId)
// 	{
// 		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
// 		{
// 			// ֻ�������
// 			if (iter->second.wType != LIMIT_ACTIVITY_SINGLE)
// 				continue;
// 
// 			const LimitActivity_Config *pActivityCfg = g_pCfgMgr->GetLimitActivity(iter->second.wDataId);
// 			if (!pActivityCfg)
// 				continue;
// 
// 			// ȡ��¼����
// 			RecordData *pRecordData = FindSingleRechargeRewardRecord(pActivityCfg->wDataId);
// 
// 			// �Ѿ���ȡ������
// 			WORD wTakeNum = pRecordData ? pRecordData->wRecordNum : 0;
// 
// 			// �����ȡ�����Ƿ��Ѵ�����(��������)
// 			if (wTakeNum >= pActivityCfg->wRewardTimesMax)
// 				continue;
// 
// 			// �����ȡ�����Ƿ��Ѵ�����(��ֵ����)
// 			if (wTakeNum >= m_pPlayer->GetRechargeManager()->GetRechargeTimes(pActivityCfg->dwContext))
// 				continue;
// 
// 			return true;
// 		}
// 	}
// 	// 75�۳����
// 	else if (SYSTEM_AMT_RCG == wSystemId)
// 	{
// 		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
// 		{
// 			const LimitActivity_Config *pCfg = &iter->second;
// 
// 			// �������
// 			if (m_pLimitActivityData->dwAccumulateRecharge >= pCfg->dwContext && pCfg->wType == LIMIT_ACTIVITY_ACCUMULATE)
// 			{
// 				// �Ƿ�ľ����ȡ
// 				if (!IsTakeAccumulateRechargeReward(pCfg->wDataId))
// 					return true;
// 			}
// 		}
// 	}
	// 76�׳�
	if (SYSTEM_FRT_RCG == wSystemId)
	{
		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
		{
			// �������
			if (m_pLimitActivityData->dwTotleRecharge >= iter->second.dwContext &&
				iter->second.wType == LIMIT_ACTIVITY_FIRST)
			{
				// �Ƿ�ľ����ȡ
				if (!IsTakeFirstRechargeReward(iter->second.wDataId))
					return true;
			}
		}
	}
	// 77��ʱ�ٻ�
// 	else if (SYSTEM_LMT_RCT == wSystemId)
// 	{
// 		// �Ƿ��ڻ�ڼ�
// 		// todo::�Ƿ�ʱ��
// 		if (!CActivityManager::IsActivityOpen(SYSTEM_LMT_RCT, m_pPlayer->GetServerId(), m_pPlayer->GetLevel()))
// 			return false;
// 
// 		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
// 		{
// 			// ��������
// 			if (m_pLimitActivityData->wRecruitTimes >= iter->second.dwContext && 
// 				iter->second.wType == LIMIT_ACTIVITY_RECRUIT)
// 			{
// 				// �Ƿ�ľ����ȡ
// 				if (!IsTakeLimitRecruitReward(iter->second.wDataId))
// 					return true;
// 			}
// 		}
// 	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ĺ����ʾ����[SystemId:%d]", __FUNCTION__, wSystemId);
		return false;
	}

	return false;
}

// �Ƿ�����ȡ�׳佱��
bool CLimitActivity::IsTakeFirstRechargeReward(WORD wDataId)
{
	for (WORD i = 0; i < m_pLimitActivityData->wFirstRechargeRewardRecordNum; ++i)
	{
		if (m_pLimitActivityData->arFirstRechargeRewardRecord[i] == wDataId)
			return true;
	}

	return false;
}

// �Ƿ�����ȡ��ʱ��ļ����
//  bool CLimitActivity::IsTakeLimitRecruitReward(WORD wDataId)
//  {
//  	for (WORD i = 0; i < m_pLimitActivityData->wRecruitRewardRecordNum; ++i)
//  	{
//  		if (m_pLimitActivityData->arRecruitRewardRecord[i] == wDataId)
//  			return true;
//  	}
//  
//  	return false;
//  }
 
 // �Ƿ�����ȡ���۳佱��
//  bool CLimitActivity::IsTakeAccumulateRechargeReward(WORD wDataId)
//  {
//  	for (WORD i = 0; i < m_pLimitActivityData->wAccumulateRechargeRewardRecordNum; ++i)
//  	{
//  		if (m_pLimitActivityData->arAccumulateRechargeRewardRecord[i] == wDataId)
//  			return true;
//  	}
//  
//  	return false;
//  }
 
// // �Ƿ�����ȡ�˽���
// bool CLimitActivity::IsTakeReward(WORD wDataId)
// {
// 	return true;
// }

// ��ӽ�����¼
bool CLimitActivity::AddRewardRecord(WORD wType, WORD wDataId)
{
	// ����
// 	if (LIMIT_ACTIVITY_SINGLE == wType)
// 	{
// 		RecordData *pRecordData = FindSingleRechargeRewardRecord(wDataId);
// 		if (pRecordData)
// 		{
// 			pRecordData->wRecordNum++;
// 		}
// 		else
// 		{
// 			WORD &wRecordNum = m_pLimitActivityData->wSingleRechargeRewardRecordNum;
// 			RecordData *pRecordList = m_pLimitActivityData->arSingleRechargeRewardRecord;
// 
// 			// ����б��Ƿ�����
// 			if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
// 			{
// 				MAPLOG(ERROR_LEVEL, "[%s]��ʱ�������¼�б�����[type:%d]", __FUNCTION__, wType);
// 				return false;
// 			}
// 
// 			pRecordList[wRecordNum].wRecordId = wDataId;
// 			pRecordList[wRecordNum].wRecordNum = 1;
// 			wRecordNum++;
// 		}
// 	}
	// �۳�
// 	else if (LIMIT_ACTIVITY_ACCUMULATE == wType)
// 	{
// 		WORD &wRecordNum = m_pLimitActivityData->wAccumulateRechargeRewardRecordNum;
// 		WORD *pRecordList = m_pLimitActivityData->arAccumulateRechargeRewardRecord;
// 
// 		// ����б��Ƿ�����
// 		if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]��ʱ�������¼�б�����[type:%d]", __FUNCTION__, wType);
// 			return false;
// 		}
// 
// 		pRecordList[wRecordNum++] = wDataId;
// 	}
	// �׳�
	if (LIMIT_ACTIVITY_FIRST == wType)
	{
		WORD &wRecordNum = m_pLimitActivityData->wFirstRechargeRewardRecordNum;
		WORD *pRecordList = m_pLimitActivityData->arFirstRechargeRewardRecord;

		// ����б��Ƿ�����
		if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ʱ�������¼�б�����[type:%d]", __FUNCTION__, wType);
			return false;
		}

		pRecordList[wRecordNum++] = wDataId;
	}
	// ��ļ
// 	else if (LIMIT_ACTIVITY_RECRUIT == wType)
// 	{
// 		WORD &wRecordNum = m_pLimitActivityData->wRecruitRewardRecordNum;
// 		WORD *pRecordList = m_pLimitActivityData->arRecruitRewardRecord;
// 
// 		// ����б��Ƿ�����
// 		if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]��ʱ�������¼�б�����[type:%d]", __FUNCTION__, wType);
// 			return false;
// 		}
// 
// 		pRecordList[wRecordNum++] = wDataId;
// 	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д������ʱ�����[type:%d]", __FUNCTION__, wType);
		return false;
	}

	return true;
}

// ���»����
void CLimitActivity::UpdateActivityData(int nSystemId)
{
// 	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(nSystemId);
// 	if (wDataId)
// 	{
// 		// ����
// 		if (SYSTEM_SGL_RCG == nSystemId && m_pLimitActivityData->wLastSingleRechargeDataId != wDataId)
// 		{
// 			m_pLimitActivityData->wLastSingleRechargeDataId = wDataId;
// 			m_pLimitActivityData->wSingleRechargeRewardRecordNum = 0;
// 			m_pLimitActivityData->wRechargeNum = 0;
// 		}
// 		// �۳�
// 		else if (SYSTEM_AMT_RCG == nSystemId && m_pLimitActivityData->wLastAccumulateRechargeDataId != wDataId)
// 		{
// 			m_pLimitActivityData->wLastAccumulateRechargeDataId = wDataId;
// 			m_pLimitActivityData->wAccumulateRechargeRewardRecordNum = 0;
// 			m_pLimitActivityData->dwAccumulateRecharge = 0;
// 		}
// 	}
}

// ȡ��ֵ����
// WORD CLimitActivity::GetRechargeTimes(WORD wRechargeId)
// {
// 	CommonRecharge *pRecharge = FindCommonRecharge(wRechargeId);
// 	return pRecharge ? pRecharge->wRechargeTimes : 0;
// }

// ������ͨ��ֵ����
// CommonRecharge* CLimitActivity::FindCommonRecharge(WORD wRechargeId)
// {
// 	for (WORD i = 0; i < m_pLimitActivityData->wRechargeNum; ++i)
// 	{
// 		if (m_pLimitActivityData->arRechargeList[i].wRechargeId == wRechargeId)
// 			return &m_pLimitActivityData->arRechargeList[i];
// 	}
// 	return NULL;
// }

void CLimitActivity::SendFirstRechargeData()
{
	MSG_S2C_LIMIT_FIRST_RECHARGE_DATA_RESP msg;
	msg.m_pActivityData = m_pLimitActivityData;
	m_pPlayer->SendPacket(&msg);
}

// void CLimitActivity::SendSingleRechargeData()
// {
// 	UpdateActivityData(SYSTEM_SGL_RCG);
// 
// 	MSG_S2C_LIMIT_SINGLE_RECHARGE_DATA_RESP msg;
// 	msg.m_pActivityData = m_pLimitActivityData;
// 	m_pPlayer->SendPacket(&msg);
// }
// 
// void CLimitActivity::SendAccumulateRechargeData()
// {
// 	UpdateActivityData(SYSTEM_AMT_RCG);
// 
// 	MSG_S2C_LIMIT_ACCUMULATE_RECHARGE_DATA_RESP msg;
// 	msg.m_pActivityData = m_pLimitActivityData;
// 	m_pPlayer->SendPacket(&msg);
// }
// 
// void CLimitActivity::SendRecruitData()
// {
// 	MSG_S2C_LIMIT_RECRUIT_DATA_RESP msg;
// 	msg.m_pActivityData = m_pLimitActivityData;
// 	m_pPlayer->SendPacket(&msg);
// }

// ȡ���佱����¼
// RecordData* CLimitActivity::FindSingleRechargeRewardRecord(WORD wDataId)
// {
// 	for (WORD i = 0; i < m_pLimitActivityData->wSingleRechargeRewardRecordNum; ++i)
// 	{
// 		if (m_pLimitActivityData->arSingleRechargeRewardRecord[i].wRecordId == wDataId)
// 			return &m_pLimitActivityData->arSingleRechargeRewardRecord[i];
// 	}
// 	return NULL;
// }