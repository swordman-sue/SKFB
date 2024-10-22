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

// 活动活动请求
void CFestivalActivity::OnNetActivityData(WORD wActivityType)
{
	UpdateActivityData(wActivityType);

	SendActivityData(wActivityType);
}

// 领取活动奖励
void CFestivalActivity::OnNetTakeActivityReward(WORD wActivityType, WORD wTargetId)
{
	const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
	if (!pTargetCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标数据[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的节日活动数据[ActivityType:%d,TargetId:%d]", __FUNCTION__, wActivityType, wTargetId);
		return;
	}

	ActivityTarget *pTargetData = pActivityData->GetTarget(wTargetId);

	if (!pTargetData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]此节日活动目标不可直接领取[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// 检测是否是需要领取的目标(通过其他方式领取)
	if (!pTargetCfg->byIsNeedTake)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]此节日活动目标不可直接领取[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// 检测是否可领取(条件)
	if (pTargetCfg->byIsStageTarget)
	{ // 阶段目标
		if (pTargetData->dwContext < pTargetCfg->nX)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]节日活动目标进度值不足,不能领取[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}
	else
	{ // 一次性目标
		if (pTargetData->wTakeRewardTimes >= pTargetData->dwContext)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]节日活动目标可领取次数不足,不能领取[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}

	// 检测可领取次数上限
	if (pTargetData->wTakeRewardTimes >= pTargetCfg->wRewardTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]节日活动目标可领取次数已达上限[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_GET_FESTIVAL_ACTIVITY_REWARD);

	pTargetData->wTakeRewardTimes++;

	// 响应
	MSG_S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP msg;
	msg.m_wActivityType = wActivityType;
	msg.m_wTargetId = wTargetId;
	m_pPlayer->SendPacket(&msg);
}

// 商品兑换
void CFestivalActivity::OnNetGoodsExchange(WORD wActivityType, WORD wTargetId, WORD wExchangeNum)
{
	// todo::检测活动时间

	const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
	if (!pTargetCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到兑换商品目标数据[TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动数据[ActivityType:%d]", __FUNCTION__, wActivityType);
		return;
	}

	ActivityTarget *pTargetData = pActivityData->GetTarget(wTargetId);

	// 钻石换物品
	if (DIAMOND_EXCHANGE == wActivityType || FESTIVAL_ACTIVITY_13 == wActivityType)
	{
		// 检测钻石消耗
		if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pTargetCfg->nX * wExchangeNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]兑换节日活动商品消耗的钻石不足[TargetId:%d, CostDiamond:%d]", 
				__FUNCTION__, wTargetId, pTargetCfg->nX);
			return;
		}

		// 检测兑换次数
		if (pTargetData && pTargetData->wTakeRewardTimes + wExchangeNum > pTargetCfg->wRewardTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]节日活动商品兑换次数已达上限[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}

		// 扣除钻石
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pTargetCfg->nX * wExchangeNum, TRUE, FROM_FESTIVAL_DIAMOND_EXCHANGE);

		// 发奖励
		for (WORD i = 0; i < wExchangeNum; ++i)
		{
			m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_GET_FESTIVAL_DIAMOND_EXCHANGE);
		}

		// 记录兑换次数
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
	// 物品换物品
	else if (ITEM_EXCHANGE == wActivityType || FESTIVAL_ITEM_EXCHANGE == wActivityType)
	{
		// 检测消耗的物品
		int nCostNum = pTargetCfg->nY * wExchangeNum;
		if (m_pPlayer->GetItemNum(pTargetCfg->nX) < nCostNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]兑换节日活动商品消耗的物品不足[TargetId:%d, CostNum:%d]",__FUNCTION__, wTargetId, nCostNum);
			return;
		}

		// 检测兑换次数
		if (pTargetData && pTargetData->wTakeRewardTimes + wExchangeNum > pTargetCfg->wRewardTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]节日活动商品兑换次数已达上限[TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}

		// 扣除物品
		m_pPlayer->RemoveItem(pTargetCfg->nX, nCostNum, FROM_GET_FESTIVAL_ITEM_EXCHANGE);

		// 发奖励
		for (WORD i = 0; i < wExchangeNum; ++i)
		{
			m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_GET_FESTIVAL_ITEM_EXCHANGE);
		}

		// 记录兑换次数
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
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的节日活动类型[ActivityType:%d]", __FUNCTION__, wActivityType);
		return;
	}

	 // 响应
	MSG_S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP msg;
	msg.m_wActivityType = wActivityType;
	msg.m_wTargetId = wTargetId;
	msg.m_wExchangeNum = wExchangeNum;
	m_pPlayer->SendPacket(&msg);
}

// 请求红点
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

// 单充回调
void CFestivalActivity::OnOnceRechargeCallback(WORD wRechargeId)
{
	// 循环单充
	DoOnceRechargeEvent(LOOP_ONCE_RECHARGE, wRechargeId);

	// 活动单充
	DoOnceRechargeEvent(ACTIVITY_ONCE_RECHARGE, wRechargeId);

	// 活动单充2
	DoOnceRechargeEvent(ACTIVITY_ONCE_RECHARGE_17, wRechargeId);
}

// 累充回调
void CFestivalActivity::OnAccumulateRechargeCallback(DWORD dwAmount)
{
	// 循环累充
	DoAccumulateRechargeEvent(LOOP_ACCUMULATE_RECHARGE, dwAmount);

	// 活动累充
	DoAccumulateRechargeEvent(ACTIVITY_ACCUMULATE_RECHARGE, dwAmount);

	// 开服累充
	DoAccumulateRechargeEvent(OPEN_SVR_ACCUMULATE_RECHARGE, dwAmount);
}

// 限时招募
void CFestivalActivity::OnLimitRecruit(WORD wTimes)
{
	WORD wActivityType = LIMIT_RECRUIT;

	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
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

// 打折兑换（钻石换物品）
void CFestivalActivity::OnDiamondExchange(WORD wTargetId, DWORD dwExchangeNum)
{
	DoActiveTargetEvent(DIAMOND_EXCHANGE, wTargetId, dwExchangeNum);

	DoActiveTargetEvent(FESTIVAL_ACTIVITY_13, wTargetId, dwExchangeNum);
}

// 活跃目标 
void CFestivalActivity::OnActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd)
{
	const vector<WORD> *pTargetList = g_pCfgMgr->GetFestivalActivitySameLogicTargetList(wLogicType);
	if (!pTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节目活跃目标配置数据![LogicType:%d]", __FUNCTION__, wLogicType);
		return;
	}

	for (size_t i = 0; i < pTargetList->size(); ++i)
	{
		OnActiveTarget(wActivityType, pTargetList->at(i), dwX);
	}
}

// 活跃目标 
void CFestivalActivity::OnActiveTarget(WORD wActivityType, WORD wTargetId, DWORD dwContext)
{
	DoActiveTargetEvent(wActivityType, wTargetId, dwContext);
}

// 材料兑换（物品换物品）
void CFestivalActivity::OnItemExchange(WORD wTargetId, DWORD dwExchangeNum)
{
	DoActiveTargetEvent(ITEM_EXCHANGE, wTargetId, dwExchangeNum);

	DoActiveTargetEvent(FESTIVAL_ITEM_EXCHANGE, wTargetId, dwExchangeNum);
}

// 节日登陆
void CFestivalActivity::OnNetFestivalLogin()
{
	DoLoginEvent(LOGIN_GIFT);
	DoLoginEventEx(OPEN_SERVER_LOGIN);
}

// 处理充值事件
void CFestivalActivity::DoOnceRechargeEvent(WORD wActivityType, WORD wRechargeId)
{
	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标数据配置[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				// 如果充值的档位一样
				if (pTargetCfg->nX == wRechargeId)
					AddActivityTarget(wActivityType, wLocalTargetId, 1);
			}
			break;
		}
	}
}

// 处理累充事件
void CFestivalActivity::DoAccumulateRechargeEvent(WORD wActivityType, DWORD dwAmount)
{
	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
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

// 处理登录事件
void CFestivalActivity::DoLoginEvent(WORD wActivityType)
{
	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	tm tNow;
	GetTMTime(&tNow, time(NULL));
	int nCurDate = (tNow.tm_year + 1900) * 10000 + (tNow.tm_mon + 1) * 100 + tNow.tm_mday;

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标数据配置[ActivityType:%d,TargetId:%d]",
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

// 处理登录事件
void CFestivalActivity::DoLoginEventEx(WORD wActivityType)
{
	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	int nOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标数据配置[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				// 只有当前有效
				if (nOpenServerDays == pTargetCfg->nX)
					AddActivityTarget(wActivityType, wLocalTargetId);
			}
			break;
		}
	}
}

// 处理活跃目标事件
void CFestivalActivity::DoActiveTargetEvent(WORD wActivityType, WORD wTargetId, DWORD dwContext)
{
	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标数据配置[ActivityType:%d,TargetId:%d]",
						__FUNCTION__, wActivityType, wLocalTargetId);
					continue;
				}

				if (wLocalTargetId == wTargetId)
					AddActivityTarget(wActivityType, wLocalTargetId, dwContext);
			}
			break;// 同类活动只会有一个正在进行中
		}
	}
}

// 处理兑换事件
void CFestivalActivity::DoExchangeEvent(WORD wActivityType, WORD wTargetId, DWORD dwExchangeNum)
{
	// 检测是否活动期间
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return;

	UpdateActivityData(wActivityType);

	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
		return;
	}

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		// 检测当前正在进行的活动
		if (IsFestivalActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		{
			for (size_t i = 0; i < iter->vParamList.size(); ++i)
			{
				WORD wLocalTargetId = iter->vParamList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wLocalTargetId);
				if (!pTargetCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标数据配置[ActivityType:%d,TargetId:%d]",
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

// 更新活动数据
void CFestivalActivity::UpdateActivityData(WORD wActivityType)
{
	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
		return;

	// 已经初始化过数据
	if (pActivityData->dwDataResetTime)
	{
		DWORD dwCurTime = time(NULL);
		// 检测是否需要重置数据
		if (dwCurTime >= pActivityData->dwDataResetTime)
		{
			MAPLOG(GUI_LEVEL, "节日活动结束重置[RoleId:%d,ActivityType:%d,DataResetTime:%d]",
				m_pPlayer->GetRoleId(), wActivityType, pActivityData->dwDataResetTime);

			// 活动能数据重置
			pActivityData->wTargetNum = 0;
			pActivityData->dwDataResetTime = 0;
		}
	}
	else
	{
		// 活动数据初始化
		pActivityData->dwDataResetTime = GetActivityDataResetTime(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime());
		pActivityData->wTargetNum = 0;

		MAPLOG(GUI_LEVEL, "节日活动开始初始化[RoleId:%d,ActivityType:%d,DataResetTime:%d]",
			m_pPlayer->GetRoleId(), wActivityType, pActivityData->dwDataResetTime);
	}
}

// 是否节日活动开放
bool CFestivalActivity::IsFestivalActivityOpen(int wActivityType, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	// 取活动配置
	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
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


// 是否节日活动开放
bool CFestivalActivity::IsFestivalActivityOpen(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	return CActivityManager::CheckActivityTime(pActivityCfg, dwServerId, dwCreateRoleTime);
}

// 取活动的数据重置时间(取正在活动中的)
DWORD CFestivalActivity::GetActivityDataResetTime(WORD wActivityType, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityType:%d]!", __FUNCTION__, wActivityType);
		return 0;
	}

	DWORD dwCurTime = time(NULL);
	DWORD dwDataResetTime = 0;
	vector<FestivalActivity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		dwDataResetTime = GetActivityDataResetTime(&*iter, dwServerId, dwCreateRoleTime);
		// 取当前正在活动中的
		if (dwDataResetTime > dwCurTime)
			break;
	}

	return dwDataResetTime;
}

// 取活动的数据重置时间(取正在活动中的)
DWORD CFestivalActivity::GetActivityDataResetTime(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	// 检测是否在活动中
	if (!IsFestivalActivityOpen(pActivityCfg, dwServerId, dwCreateRoleTime))
		return 0;

	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(dwServerId);
	if (!pServerInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到服务器数据[ServerId:%d]!", __FUNCTION__, dwServerId);
		return 0;
	}


	DWORD dwCurTime = time(NULL);

	tm tmResetTime;
	GetTMTime(&tmResetTime, dwCurTime);

	switch (pActivityCfg->nTimeType)
	{
		// 1.指定时间 开始日期[年:月 : 日] 20170912  结束日期[年:月 : 日] 2359
	case ACTIVITY_TIME_APPOINT:
		{
			tmResetTime.tm_year = SplitInteger(pActivityCfg->nRewardOverDate, 8, 0, 4);
			tmResetTime.tm_mon = SplitInteger(pActivityCfg->nRewardOverDate, 8, 4, 2);
			tmResetTime.tm_mday = SplitInteger(pActivityCfg->nRewardOverDate, 8, 6, 2);
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
		}
		break;

		// 2.每日 开始日期[0] 结束日期[0]
	case ACTIVITY_TIME_DAILY:
		{
			tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
			tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;

		//  3.星期 开始日期[星期几] 结束日期[星期几]
	case ACTIVITY_TIME_WEEK:
		{
			int nOverWeekDay = g_CorrectWeekDay(pActivityCfg->nRewardOverDate);
			int nCurWeekDay = g_CorrectWeekDay(tmResetTime.tm_wday);
			int nRemainDay = nOverWeekDay - nCurWeekDay;
			// 明天及后结束
			if (nRemainDay > 0)
			{
				dwCurTime += g_Day2Second(nRemainDay);
				GetTMTime(&tmResetTime, dwCurTime);
				tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
				tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			}
			// 当天
			else if (nRemainDay == 0)
			{
				tmResetTime.tm_hour = SplitInteger(pActivityCfg->nRewardOverTime, 4, 0, 2);
				tmResetTime.tm_min = SplitInteger(pActivityCfg->nRewardOverTime, 4, 2, 2);
			}
			tmResetTime.tm_mon += 1;
			tmResetTime.tm_year += 1900;
		}
		break;

		//	4.月份 开始日期[几号] 结束日期[几号]
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

		//  5.开服 开始日期[开服第几天] 结束日期[开服第几天]
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
			int nOverDay = pActivityCfg->nRewardOverDate - 1; // todo::为啥要减1说明: 因为时间是按开服当天算的, 开服当天已经算一天了
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
			int nOverDay = pActivityCfg->nRewardOverDate - 1; // todo::为啥要减1说明: 因为时间是按创建当天算的, 创建当天已经算一天了
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
			MAPLOG(ERROR_LEVEL, "[%s]没有处理的活动时间类型[ActivityType:%d]!",__FUNCTION__,  pActivityCfg->nTimeType);
			return 0;	
		}
		break;
	}

	// todo::可能有BUG
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

// 是否有红点
bool CFestivalActivity::IsHaveRedPoint(WORD wActivityType)
{
	// 检测是否在活动中
	if (!IsFestivalActivityOpen(wActivityType, m_pPlayer->GetServerId(), m_pPlayer->GetRoleCreateTime()))
		return false;

	UpdateActivityData(wActivityType);

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityType:%d]!", __FUNCTION__, wActivityType);
		return false;
	}

	const FestivalActivtiyTarget_Config *pTargetCfg = NULL;

	switch (wActivityType)
	{
	case LOOP_ONCE_RECHARGE:		// 循环单充
	case ACTIVITY_ONCE_RECHARGE:	// 活动单充
	case ACTIVITY_ONCE_RECHARGE_17: // 活动单充2
	case LOGIN_GIFT:				// 登陆有礼
	case OPEN_SERVER_LOGIN:			// 开服登录
		{
			for (WORD i = 0; i < pActivityData->wTargetNum; ++i)
			{
				ActivityTarget &tTarget = pActivityData->arTargetList[i];
				// 有可领取次数
				if (tTarget.dwContext > tTarget.wTakeRewardTimes)
					return true;
			}
		}
		break;

	case LOOP_ACCUMULATE_RECHARGE:		// 循环累充
	case ACTIVITY_ACCUMULATE_RECHARGE:	// 活动累充
	case LIMIT_RECRUIT:					// 开服限时召唤
	case ACTIVATE_TARGET:				// 活跃目标
	case OPEN_SVR_ACCUMULATE_RECHARGE:
	case FESTIVAL_ACTIVITY_14:
	case FESTIVAL_ACTIVITY_15:
	case FESTIVAL_ACTIVITY_16:
		{
			for (WORD i = 0; i < pActivityData->wTargetNum; ++i)
			{
				ActivityTarget &tTarget = pActivityData->arTargetList[i];
				pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(tTarget.wId);
				// 进度已完成,且还没有领取
				if (pTargetCfg && tTarget.dwContext >= pTargetCfg->nX && tTarget.wTakeRewardTimes < pTargetCfg->wRewardTimes)
					return true;
			}
		}
		break;
	case DIAMOND_EXCHANGE:
	case FESTIVAL_ACTIVITY_13:
		{
			// 取活动配置
			const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
			if (!pActivityListCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
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

					// 钻石足够,且兑换次数还有
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
			// 取活动配置
			const vector<FestivalActivity_Config> *pActivityListCfg = g_pCfgMgr->GetFestivalActivity(wActivityType);
			if (!pActivityListCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动配置[ActivityId:%d]", __FUNCTION__, wActivityType);
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

					// 兑换物品数量足够,兑换次数足够
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
			MAPLOG(ERROR_LEVEL, "[%s]没有处理的节目活动类型[ActivityType:%d]!", __FUNCTION__, wActivityType);
			return false;
		}
		break;
	}

	return false;
}

// 添加目标数据
void CFestivalActivity::AddActivityTarget(WORD wActivityType, WORD wTargetId, DWORD dwValue)
{
	const FestivalActivtiyTarget_Config *pTargetCfg = g_pCfgMgr->GetFestivalActivityTarget(wTargetId);
	if (!pTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动目标配置[TargetId:%d]!", __FUNCTION__, wTargetId);
		return;
	}

	FestivalActivity *pActivityData = GetFestivalActivity(wActivityType);
	if (!pActivityData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到节日活动数据[ActivityType:%d]!", __FUNCTION__, wActivityType);
		return;
	}

	ActivityTarget *pTargetData = pActivityData->GetTarget(wTargetId);
	if (pTargetData)
	{
		pTargetData->dwContext += dwValue;
	}
	else
	{
		// 活动目标列表已满
		if (pActivityData->wTargetNum >= MAX_ACTIVITY_TARGET_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]节日活动目标列表已满[ActivityType:%d]!", __FUNCTION__, wActivityType);
			return;
		}

		pTargetData = &pActivityData->arTargetList[pActivityData->wTargetNum++];
		pTargetData->wId = wTargetId;
		pTargetData->dwContext = dwValue;
		pTargetData->wTakeRewardTimes = 0;
	}

	// 做数据纠正
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

	// todo::同步数据
}

// 取节日活动数据
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
		MAPLOG(ERROR_LEVEL, "[%s]没有处理的节日活动类型[ActivityId:%d]!", __FUNCTION__, wActivityId);
		break;
	}

	return NULL;
}


// 发送活动数据
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