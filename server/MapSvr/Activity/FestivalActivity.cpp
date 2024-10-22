#include "stdafx.h"
#include "FestivalActivity.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\MapServer.h"
#include "..\Player.h"
#include "ActivityManager.h"

void CFestivalActivity::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pFestivalActivityInfo = &pPlayer->GetRoleFullInfo().tActivityInfo.tFestivalActivityInfo;
}

void CFestivalActivity::OnNewDay()
{
	for (WORD i = FESTIVAL_ACTIVITY_BEGIN; i < FESTIVAL_ACTIVITY_END; ++i)
	{
		FestivalActivity *pActivityData = GetFestivalActivity(i);
		if (!pActivityData)
			continue;

		for (WORD k = 0; k < pActivityData->wTargetNum; ++k)
		{
			ActivityTarget &tTarget = pActivityData->arTargetList[k];
			const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(tTarget.wId);
			if (pTargetCfg && pTargetCfg->byIsDailyReset)
			{
				tTarget.dwContext = 0;
				tTarget.wTakeRewardTimes = 0;
			}
		}
	}
}

// ������
void CFestivalActivity::OnNetActivityData(WORD wActivityType)
{
	UpdateActivityData(wActivityType);

	SendActivityData(wActivityType);
}

// ��ȡ�����
void CFestivalActivity::OnNetTakeActivityReward(WORD wActivityType, WORD wTargetId)
{
	const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
	if (!pTargetCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ������[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĽ��ջ����[ActivityType:%d,TargetId:%d]", __FUNCTION__, wActivityType, wTargetId);
		return;
	}

	ActivityTarget *pTargetData = pActivityData->GetTarget(wTargetId);

	if (!pTargetData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�˽��ջĿ�겻��ֱ����ȡ[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// ����Ƿ�����Ҫ��ȡ��Ŀ��(ͨ��������ʽ��ȡ)
	if (!pTargetCfg->byIsNeedTake)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�˽��ջĿ�겻��ֱ����ȡ[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// ����Ƿ����ȡ(����)
	if (pTargetCfg->byIsStageTarget)
	{ // �׶�Ŀ��
		if (pTargetData->dwContext < pTargetCfg->nX)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���ջĿ�����ֵ����,������ȡ[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}
	else
	{ // һ����Ŀ��
		if (pTargetData->wTakeRewardTimes >= pTargetData->dwContext)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���ջĿ�����ȡ��������,������ȡ[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}

	// ������ȡ��������
	if (pTargetData->wTakeRewardTimes >= pTargetCfg->wRewardTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���ջĿ�����ȡ�����Ѵ�����[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_GET_FESTIVAL_ACTIVITY_REWARD);

	pTargetData->wTakeRewardTimes++;

	// ��Ӧ
	MSG_S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP msg;
	msg.m_wActivityType = wActivityType;
	msg.m_wTargetId = wTargetId;
	m_pPlayer->SendPacket(&msg);
}

// ��Ʒ�һ�
void CFestivalActivity::OnNetGoodsExchange(WORD wActivityType, WORD wTargetId, WORD wExchangeNum)
{
	// todo::���ʱ��

	const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
	if (!pTargetCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����һ���ƷĿ������[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityType:%d]", __FUNCTION__, wActivityType);
		return;
	}

	ActivityTarget *pTargetData = pActivityData->GetTarget(wTargetId);

	// ��ʯ����Ʒ
	if (DIAMOND_EXCHANGE == wActivityType || FESTIVAL_ACTIVITY_13 == wActivityType)
	{
		// �����ʯ����
		if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pTargetCfg->nX * wExchangeNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�һ����ջ��Ʒ���ĵ���ʯ����[TargetId:%d, CostDiamond:%d]", 
				__FUNCTION__, wTargetId, pTargetCfg->nX);
			return;
		}

		// ���һ�����
		if (pTargetData && pTargetData->wTakeRewardTimes + wExchangeNum > pTargetCfg->wRewardTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���ջ��Ʒ�һ������Ѵ�����[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}

		// �۳���ʯ
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pTargetCfg->nX * wExchangeNum, TRUE, FROM_FESTIVAL_DIAMOND_EXCHANGE);

		// ������
		for (WORD i = 0; i < wExchangeNum; ++i)
		{
			m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_GET_FESTIVAL_DIAMOND_EXCHANGE);
		}

		// ��¼�һ�����
		if (pTargetData)
		{
			pTargetData->wTakeRewardTimes += wExchangeNum;
		}
		else
		{
			if (pActivityData->wTargetNum < MAX_ACTIVITY_TARGET_NUM)
			{
				ActivityTarget &tTarget = pActivityData->arTargetList[pActivityData->wTargetNum++];
				tTarget.wId = wTargetId;
				tTarget.dwContext = 0;
				tTarget.wTakeRewardTimes = wExchangeNum;
			}
		}
	}
	// ��Ʒ����Ʒ
	else if (ITEM_EXCHANGE == wActivityType || FESTIVAL_ITEM_EXCHANGE == wActivityType)
	{
		// ������ĵ���Ʒ
		int nCostNum = pTargetCfg->nY * wExchangeNum;
		if (m_pPlayer->GetItemNum(pTargetCfg->nX) < nCostNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�һ����ջ��Ʒ���ĵ���Ʒ����[TargetId:%d, CostNum:%d]",__FUNCTION__, wTargetId, nCostNum);
			return;
		}

		// ���һ�����
		if (pTargetData && pTargetData->wTakeRewardTimes + wExchangeNum > pTargetCfg->wRewardTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���ջ��Ʒ�һ������Ѵ�����[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}

		// �۳���Ʒ
		m_pPlayer->RemoveItem(pTargetCfg->nX, nCostNum, FROM_GET_FESTIVAL_ITEM_EXCHANGE);

		// ������
		for (WORD i = 0; i < wExchangeNum; ++i)
		{
			m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_GET_FESTIVAL_ITEM_EXCHANGE);
		}

		// ��¼�һ�����
		if (pTargetData)
		{
			pTargetData->wTakeRewardTimes += wExchangeNum;
		}
		else
		{
			if (pActivityData->wTargetNum < MAX_ACTIVITY_TARGET_NUM)
			{
				ActivityTarget &tTarget = pActivityData->arTargetList[pActivityData->wTargetNum++];
				tTarget.wId = wTargetId;
				tTarget.dwContext = 0;
				tTarget.wTakeRewardTimes = wExchangeNum;
			}
		}
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ľ��ջ����[ActivityType:%d]", __FUNCTION__, wActivityType);
		return;
	}

	 // ��Ӧ
	MSG_S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP msg;
	msg.m_wActivityType = wActivityType;
	msg.m_wTargetId = wTargetId;
	msg.m_wExchangeNum = wExchangeNum;
	m_pPlayer->SendPacket(&msg);
}

// ������
void CFestivalActivity::OnNetRedPoint()
{
	MSG_S2C_FESTIVAL_ACTIVITY_RED_POINT_RESP msg;
	msg.m_wRedPointNum = 0;
	for (WORD i = FESTIVAL_ACTIVITY_BEGIN; i < FESTIVAL_ACTIVITY_END; i++)
	{
		if (IsHaveRedPoint(i))
			msg.m_RedPointList[msg.m_wRedPointNum++] = i;
	}
	m_pPlayer->SendPacket(&msg);
}

// ����ص�
void CFestivalActivity::OnOnceRechargeCallback(WORD wRechargeId)
{
	// ѭ������
	DoOnceRechargeEvent(LOOP_ONCE_RECHARGE, wRechargeId);

	// �����
	DoOnceRechargeEvent(ACTIVITY_ONCE_RECHARGE, wRechargeId);

	// �����2
	DoOnceRechargeEvent(ACTIVITY_ONCE_RECHARGE_17, wRechargeId);
}

// �۳�ص�
void CFestivalActivity::OnAccumulateRechargeCallback(DWORD dwAmount)
{
	// ѭ���۳�
	DoAccumulateRechargeEvent(LOOP_ACCUMULATE_RECHARGE, dwAmount);

	// ��۳�
	DoAccumulateRechargeEvent(ACTIVITY_ACCUMULATE_RECHARGE, dwAmount);

	// �����۳�
	DoAccumulateRechargeEvent(OPEN_SVR_ACCUMULATE_RECHARGE, dwAmount);
}

// ��ʱ��ļ
void CFestivalActivity::OnLimitRecruit(WORD wTimes)
{
	WORD wActivityType = LIMIT_RECRUIT;

	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				AddActivityTarget(wActivityType, iter->vParamList[i], wTimes);
			}
			break;
		}
	}
}

// ���۶һ�����ʯ����Ʒ��
void CFestivalActivity::OnDiamondExchange(WORD wTargetId, DWORD dwExchangeNum)
{
	DoActiveTargetEvent(DIAMOND_EXCHANGE, wTargetId, dwExchangeNum);

	DoActiveTargetEvent(FESTIVAL_ACTIVITY_13, wTargetId, dwExchangeNum);
}

// ��ԾĿ�� 
void CFestivalActivity::OnActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd)
{
	const vector<WORD> *pTargetList = g_pCfgMgr->GetFestivalActivitySameLogicTargetList(wLogicType);
	if (!pTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ŀ��ԾĿ����������![LogicType:%d]", __FUNCTION__, wLogicType);
		return;
	}

	for (size_t i = 0; i < pTargetList->size(); ++i)
	{
		OnActiveTarget(wActivityType, pTargetList->at(i), dwX);
	}
}

// ��ԾĿ�� 
void CFestivalActivity::OnActiveTarget(WORD wActivityType, WORD wTargetId, DWORD dwContext)
{
	DoActiveTargetEvent(wActivityType, wTargetId, dwContext);
}

// ���϶һ�����Ʒ����Ʒ��
void CFestivalActivity::OnItemExchange(WORD wTargetId, DWORD dwExchangeNum)
{
	DoActiveTargetEvent(ITEM_EXCHANGE, wTargetId, dwExchangeNum);

	DoActiveTargetEvent(FESTIVAL_ITEM_EXCHANGE, wTargetId, dwExchangeNum);
}

// ���յ�½
void CFestivalActivity::OnNetFestivalLogin()
{
	DoLoginEvent(LOGIN_GIFT);
	DoLoginEventEx(OPEN_SERVER_LOGIN);
}

// �����ֵ�¼�
void CFestivalActivity::DoOnceRechargeEvent(WORD wActivityType, WORD wRechargeId)
{
	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ����������[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				// �����ֵ�ĵ�λһ��
				if (pTargetCfg->nX == wRechargeId)
					AddActivityTarget(wActivityType, wLocalTargetId, 1);
			}
			break;
		}
	}
}

// �����۳��¼�
void CFestivalActivity::DoAccumulateRechargeEvent(WORD wActivityType, DWORD dwAmount)
{
	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				AddActivityTarget(wActivityType, iter->vParamList[i], dwAmount);
			}
			break;
		}
	}
}

// �����¼�¼�
void CFestivalActivity::DoLoginEvent(WORD wActivityType)
{
	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	tm tNow;
	GetTMTime(&tNow, time(NULL));
	int nCurDate = (tNow.tm_year + 1900) * 10000 + (tNow.tm_mon + 1) * 100 + tNow.tm_mday;

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ����������[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				if (nCurDate == pTargetCfg->nX)
					AddActivityTarget(wActivityType, wLocalTargetId);
			}
			break;
		}
	}
}

// �����¼�¼�
void CFestivalActivity::DoLoginEventEx(WORD wActivityType)
{
	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	int nOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ����������[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				// ֻ�е�ǰ��Ч
				if (nOpenServerDays == pTargetCfg->nX)
					AddActivityTarget(wActivityType, wLocalTargetId);
			}
			break;
		}
	}
}

// �����ԾĿ���¼�
void CFestivalActivity::DoActiveTargetEvent(WORD wActivityType, WORD wTargetId, DWORD dwContext)
{
	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ����������[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				if (wLocalTargetId == wTargetId)
					AddActivityTarget(wActivityType, wLocalTargetId, dwContext);
			}
			break;// ͬ��ֻ����һ�����ڽ�����
		}
	}
}

// ����һ��¼�
void CFestivalActivity::DoExchangeEvent(WORD wActivityType, WORD wTargetId, DWORD dwExchangeNum)
{
	// ����Ƿ��ڼ�
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// ��⵱ǰ���ڽ��еĻ
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ����������[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				if (wTargetId == wLocalTargetId)
					AddActivityTarget(wActivityType, wLocalTargetId, dwExchangeNum);
			}
			break;
		}
	}
}

// ���»����
void CFestivalActivity::UpdateActivityData(WORD wActivityType)
{
	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
		return;

	// �Ѿ���ʼ��������
	if (pActivityData->dwDataResetTime)
	{
		DWORD dwCurTime = time(NULL);
		// ����Ƿ���Ҫ��������
		if (dwCurTime >= pActivityData->dwDataResetTime)
		{
			MAPLOG(GUI_LEVEL, "���ջ��������[RoleId:%d,ActivityType:%d,DataResetTime:%d]",
				m_pPlayer->GetRoleId(), wActivityType, pActivityData->dwDataResetTime);

			// �����������
			pActivityData->wTargetNum = 0;
			pActivityData->dwDataResetTime = 0;
		}
	}
	else
	{
		// ����ݳ�ʼ��
		pActivityData->dwDataResetTime = GetActivityDataResetTime(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime());
		pActivityData->wTargetNum = 0;

		MAPLOG(GUI_LEVEL, "���ջ��ʼ��ʼ��[RoleId:%d,ActivityType:%d,DataResetTime:%d]",
			m_pPlayer->GetRoleId(), wActivityType, pActivityData->dwDataResetTime);
	}
}

// �Ƿ���ջ����
bool CFestivalActivity::IsFestivalActivityOpen(int wActivityType, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	// ȡ�����
	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
		return false;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		if (IsFestivalActivityOpen(&*iter, dwServerId, dwCreateRoleTime))
			return true;
	}

	return false;
}


// �Ƿ���ջ����
bool CFestivalActivity::IsFestivalActivityOpen(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	return CActivityManager::CheckActivityTime(pActivityCfg, dwServerId, dwCreateRoleTime);
}

// ȡ�����������ʱ��(ȡ���ڻ�е�)
DWORD CFestivalActivity::GetActivityDataResetTime(WORD wActivityType, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityType:%d]!", __FUNCTION__, wActivityType);
		return 0;
	}

	DWORD dwCurTime = time(NULL);
	DWORD dwDataResetTime = 0;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		dwDataResetTime = GetActivityDataResetTime(&*iter, dwServerId, dwCreateRoleTime);
		// ȡ��ǰ���ڻ�е�
		if (dwDataResetTime > dwCurTime)
			break;
	}

	return dwDataResetTime;
}

// ȡ�����������ʱ��(ȡ���ڻ�е�)
DWORD CFestivalActivity::GetActivityDataResetTime(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	// ����Ƿ��ڻ��
	if (!IsFestivalActivityOpen(pActivityCfg, dwServerId, dwCreateRoleTime))
		return 0;

	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(dwServerId);
	if (!pServerInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[ServerId:%d]!", __FUNCTION__, dwServerId);
		return 0;
	}


	DWORD dwCurTime = time(NULL);

	tm tmResetTime;
	GetTMTime(&tmResetTime, dwCurTime);

	switch (pActivityCfg->nTimeType)
	{
		// 1.ָ��ʱ�� ��ʼ����[��:�� : ��] 20170912  ��������[��:�� : ��] 2359
	case ACTIVITY_TIME_APPOINT:
		{
			tmResetTime.tm_year = SplitInteger(pActivityCfg->nRewardOverDate, 8, 0, 4);
			tmResetTime.tm_mon = SplitInteger(pActivityCfg->nRewardOverDate, 8, 4, 2);
			tmResetTime.tm_mday = SplitInteger(pActivityCfg->nRewardOverDate, 8, 6, 2);
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
		}
		break;

		// 2.ÿ�� ��ʼ����[0] ��������[0]
	case ACTIVITY_TIME_DAILY:
		{
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;

		//  3.���� ��ʼ����[���ڼ�] ��������[���ڼ�]
	case ACTIVITY_TIME_WEEK:
		{
			int nOverWeekDay = g_CorrectWeekDay(pActivityCfg->nRewardOverDate);
			int nCurWeekDay = g_CorrectWeekDay(tmResetTime.tm_wday);
			int nRemainDay = nOverWeekDay - nCurWeekDay;
			// ���켰�����
			if (nRemainDay > 0)
			{
				dwCurTime += g_Day2Second(nRemainDay);
				GetTMTime(&tmResetTime, dwCurTime);
				tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
				tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			}
			// ����
			else if (nRemainDay == 0)
			{
				tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
				tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			}
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;

		//	4.�·� ��ʼ����[����] ��������[����]
	case ACTIVITY_TIME_MONTH:
		{
			int nOverMonthDay = pActivityCfg->nRewardOverDate;
			int nCurMonthDay = tmResetTime.tm_wday;
			int nRemainDay = nOverMonthDay - nCurMonthDay;
			if (nRemainDay > 0)
			{
				dwCurTime += g_Day2Second(nRemainDay);
				GetTMTime(&tmResetTime, dwCurTime);
				tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
				tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			}
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;

		//  5.���� ��ʼ����[�����ڼ���] ��������[�����ڼ���]
	case ACTIVITY_TIME_OPEN_SERVER:
		{
			GetTMTime(&tmResetTime, pServerInfo->dwOpenTime + g_Day2Second(pActivityCfg->nRewardOverDate));
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;
	case ACTIVITY_TIME_OPEN_SERVER_EX:
		{
			int nOverDay = pActivityCfg->nRewardOverDate - 1; // todo::ΪɶҪ��1˵��: ��Ϊʱ���ǰ������������, ���������Ѿ���һ����
			nOverDay = max(nOverDay, 0);
			GetTMTime(&tmResetTime, pServerInfo->dwOpenTime + g_Day2Second(nOverDay));
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;
	case ACTIVITY_TIME_CREATE_ROLE:
		{
			int nOverDay = pActivityCfg->nRewardOverDate - 1; // todo::ΪɶҪ��1˵��: ��Ϊʱ���ǰ������������, ���������Ѿ���һ����
			nOverDay = max(nOverDay, 0);
			GetTMTime(&tmResetTime, dwCreateRoleTime + g_Day2Second(nOverDay));
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;

	default:
		{
			MAPLOG(ERROR_LEVEL, "[%s]û�д���Ļʱ������[ActivityType:%d]!",__FUNCTION__,  pActivityCfg->nTimeType);
			return 0;	
		}
		break;
	}

	// todo::������BUG
	DWORD dwTime = DATETIME(tmResetTime.tm_year, tmResetTime.tm_mon, tmResetTime.tm_mday, tmResetTime.tm_hour, tmResetTime.tm_min, 0);
	return dwTime;
}

bool CFestivalActivity::IsHaveRedPoint()
{
	for (WORD i = FESTIVAL_ACTIVITY_BEGIN; i < FESTIVAL_ACTIVITY_END; i++)
	{
		if (IsHaveRedPoint(i))
			return true;
	}
	return false;
}

// �Ƿ��к��
bool CFestivalActivity::IsHaveRedPoint(WORD wActivityType)
{
	// ����Ƿ��ڻ��
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return false;

	UpdateActivityData(wActivityType);

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityType:%d]!", __FUNCTION__, wActivityType);
		return false;
	}

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;

	switch (wActivityType)
	{
	case LOOP_ONCE_RECHARGE:		// ѭ������
	case ACTIVITY_ONCE_RECHARGE:	// �����
	case ACTIVITY_ONCE_RECHARGE_17: // �����2
	case LOGIN_GIFT:				// ��½����
	case OPEN_SERVER_LOGIN:			// ������¼
		{
			for (WORD i = 0; i < pActivityData->wTargetNum; ++i)
			{
				ActivityTarget &tTarget = pActivityData->arTargetList[i];
				// �п���ȡ����
				if (tTarget.dwContext > tTarget.wTakeRewardTimes)
					return true;
			}
		}
		break;

	case LOOP_ACCUMULATE_RECHARGE:		// ѭ���۳�
	case ACTIVITY_ACCUMULATE_RECHARGE:	// ��۳�
	case LIMIT_RECRUIT:					// ������ʱ�ٻ�
	case ACTIVATE_TARGET:				// ��ԾĿ��
	case OPEN_SVR_ACCUMULATE_RECHARGE:
	case FESTIVAL_ACTIVITY_14:
	case FESTIVAL_ACTIVITY_15:
	case FESTIVAL_ACTIVITY_16:
		{
			for (WORD i = 0; i < pActivityData->wTargetNum; ++i)
			{
				ActivityTarget &tTarget = pActivityData->arTargetList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(tTarget.wId);
				// ���������,�һ�û����ȡ
				if (pTargetCfg && tTarget.dwContext >= pTargetCfg->nX && tTarget.wTakeRewardTimes < pTargetCfg->wRewardTimes)
					return true;
			}
		}
		break;
	case DIAMOND_EXCHANGE:
	case FESTIVAL_ACTIVITY_13:
		{
			// ȡ�����
			const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
			if (!pActivityListCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
				return false;
			}

			vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
			for (; iter != pActivityListCfg->end(); ++iter)
			{
				if (!IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
					continue;

				for (size_t i = 0; i < iter->vParamList.size(); ++i)
				{
					DWORD wTargetId = iter->vParamList[i];
					pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
					ActivityTarget *pTarget = pActivityData->GetTarget(wTargetId);
					if (!pTargetCfg || !pTarget)
						continue;

					// ��ʯ�㹻,�Ҷһ���������
					if (pTarget->wTakeRewardTimes < pTargetCfg->wRewardTimes && m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) >= pTargetCfg->nX)
						return true;
				}
				break;
			}
		}
		break;
	case ITEM_EXCHANGE:
	case FESTIVAL_ITEM_EXCHANGE:
		{
			// ȡ�����
			const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
			if (!pActivityListCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityId:%d]", __FUNCTION__, wActivityType);
				return false;
			}

			vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
			for (; iter != pActivityListCfg->end(); ++iter)
			{
				if (!IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
					continue;

				for (size_t i = 0; i < iter->vParamList.size(); ++i)
				{
					DWORD wTargetId = iter->vParamList[i];
					pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
					if (!pTargetCfg)
						continue;

					ActivityTarget *pTarget = pActivityData->GetTarget(wTargetId);

					// �һ���Ʒ�����㹻,�һ������㹻
					if ((m_pPlayer->GetItemNum(pTargetCfg->nX) >= pTargetCfg->nY) && 
						(!pTarget || pTarget->wTakeRewardTimes < pTargetCfg->wRewardTimes))
						return true;
				}
				break;
			}
		}
		break;
	default:
		{
			MAPLOG(ERROR_LEVEL, "[%s]û�д���Ľ�Ŀ�����[ActivityType:%d]!", __FUNCTION__, wActivityType);
			return false;
		}
		break;
	}

	return false;
}

// ���Ŀ������
void CFestivalActivity::AddActivityTarget(WORD wActivityType, WORD wTargetId, DWORD dwValue)
{
	const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
	if (!pTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջĿ������[TargetId:%d]!", __FUNCTION__, wTargetId);
		return;
	}

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ջ����[ActivityType:%d]!", __FUNCTION__, wActivityType);
		return;
	}

	ActivityTarget *pTargetData = pActivityData->GetTarget(wTargetId);
	if (pTargetData)
	{
		pTargetData->dwContext += dwValue;
	}
	else
	{
		// �Ŀ���б�����
		if (pActivityData->wTargetNum >= MAX_ACTIVITY_TARGET_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ջĿ���б�����[ActivityType:%d]!", __FUNCTION__, wActivityType);
			return;
		}

		pTargetData = &pActivityData->arTargetList[pActivityData->wTargetNum++];
		pTargetData->wId = wTargetId;
		pTargetData->dwContext = dwValue;
		pTargetData->wTakeRewardTimes = 0;
	}

	// �����ݾ���
	if (pTargetCfg->byIsStageTarget)
	{
		if (pTargetData->dwContext > pTargetCfg->nX)
			pTargetData->dwContext = pTargetCfg->nX;
	}
	else
	{
		if (pTargetData->dwContext > pTargetCfg->wRewardTimes)
			pTargetData->dwContext = pTargetCfg->wRewardTimes;
	}

	// todo::ͬ������
}

// ȡ���ջ����
FestivalActivity* CFestivalActivity::GetFestivalActivity(WORD wActivityId)
{
	switch (wActivityId)
	{
	case LOOP_ONCE_RECHARGE:
		return &m_pFestivalActivityInfo->tLoopOnceRecharge;
		break;

	case LOOP_ACCUMULATE_RECHARGE:
		return &m_pFestivalActivityInfo->tLoopAccumulateRecharge;
		break;

	case LIMIT_RECRUIT:
		return &m_pFestivalActivityInfo->tLimitRecruit;
		break;

	case ACTIVITY_ONCE_RECHARGE:
		return &m_pFestivalActivityInfo->tActivityOnceRecharge;
		break;

	case ACTIVITY_ACCUMULATE_RECHARGE:
		return &m_pFestivalActivityInfo->tActivityAccumulateRecharge;
		break;

	case DIAMOND_EXCHANGE:
		return &m_pFestivalActivityInfo->tDiamondExchange;
		break;

	case ACTIVATE_TARGET:
		return &m_pFestivalActivityInfo->tActiveTarget;
		break;

	case ITEM_EXCHANGE:
		return &m_pFestivalActivityInfo->tItemExchange;
		break;

	case LOGIN_GIFT:
		return &m_pFestivalActivityInfo->tFestivalLogin;
		break;

	case OPEN_SERVER_LOGIN:
		return &m_pFestivalActivityInfo->tOpenServerLogin;
		break;

	case OPEN_SVR_ACCUMULATE_RECHARGE:
		return &m_pFestivalActivityInfo->tOpenSvrAccumulateRecharge;
		break;

	case FESTIVAL_ITEM_EXCHANGE:
		return &m_pFestivalActivityInfo->tFestivalItemExchange;
		break;

	case FESTIVAL_ACTIVITY_13:
		return &m_pFestivalActivityInfo->tFestivalDiscountSell;
		break;

	case FESTIVAL_ACTIVITY_14:
		return &m_pFestivalActivityInfo->tFestivalActiveTarget;
		break;

	case FESTIVAL_ACTIVITY_15:
		return &m_pFestivalActivityInfo->tOpenServerActiveTarget;
		break;

	case FESTIVAL_ACTIVITY_16:
		return &m_pFestivalActivityInfo->tFestivalActiveTarget2;
		break;

	case ACTIVITY_ONCE_RECHARGE_17:
		return &m_pFestivalActivityInfo->tActivityOnceRecharge2;
		break;

	default:
		MAPLOG(ERROR_LEVEL, "[%s]û�д���Ľ��ջ����[ActivityId:%d]!", __FUNCTION__, wActivityId);
		break;
	}

	return NULL;
}


// ���ͻ����
void CFestivalActivity::SendActivityData(WORD wActivityType)
{
	MSG_S2C_FESTIVAL_ACTIVITY_DATA_RESP msg;
	msg.m_wActivityType = wActivityType;
	memset(&msg.m_tActivityData, 0, sizeof(msg.m_tActivityData));
	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (pActivityData)
		msg.m_tActivityData = *pActivityData;
	m_pPlayer->SendPacket(&msg);
}