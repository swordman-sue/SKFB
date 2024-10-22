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

// 首充数据请求
void CLimitActivity::OnNetFirstRechargeData()
{
	SendFirstRechargeData();
}

// 单充数据请求
// void CLimitActivity::OnNetSingleRechargeData()
// {
// 	SendSingleRechargeData();
// }
// 
// // 累充数据请求
// void CLimitActivity::OnNetAccumulateRechargeData()
// {
// 	SendAccumulateRechargeData();
// }
// 
// // 限时招募数据请求
// void CLimitActivity::OnNetRecruitData()
// {
// 	SendRecruitData();
// }

// 领取活动奖励
void CLimitActivity::OnNetTakeActivityReward(WORD wDataId)
{
	const LimitActivity_Config *pActivityCfg = g_pCfgMgr->GetLimitActivity(wDataId);
	if (!pActivityCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到限时活动配置[data_id:%d]", __FUNCTION__, wDataId);
		return;
	}

	// todo::检测活动时间

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
			MAPLOG(ERROR_LEVEL, "[%s]木有处理的限时活动类型[type:%d,data_id:%d]",
				__FUNCTION__, pActivityCfg->wType, wDataId);
		}
		break;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]领取限时活动奖励失败[data_id:%d,type:%d]", 
			__FUNCTION__, wDataId, pActivityCfg->wType);
		return;
	}

	// 响应
	MSG_S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP msg;
	msg.m_wDataId = wDataId;
	m_pPlayer->SendPacket(&msg);
}

// 处理领取首充奖励
bool CLimitActivity::DoTakeFirstRechargeReward(const LimitActivity_Config *pActivityCfg)
{
	WORD wDataId = pActivityCfg->wDataId;

	// 检测是否已充值过
	if (m_pLimitActivityData->dwTotleRecharge < pActivityCfg->dwContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]首充累积额度不够,不能领取[data_id:%d]", __FUNCTION__, wDataId);
		return false;
	}

	// 检测是否已领取过
	if (IsTakeFirstRechargeReward(wDataId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]累充的奖励已领取过[data_id:%d]", __FUNCTION__, wDataId);
		return false;
	}

	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]添加限时奖励记录失败[data_id:%d]", __FUNCTION__, wDataId);
		return false;
	}

	m_pPlayer->AddObjectList(pActivityCfg->vRewardItemList, TRUE, FROM_GET_LIMIT_FIRST_RECHARGE_REWARD);

	return true;
}

// 处理领取单充奖励
// bool CLimitActivity::DoTakeSingleRechargeReward(const LimitActivity_Config *pActivityCfg)
// {
// 	UpdateActivityData(SYSTEM_SGL_RCG);
// 
// 	WORD wDataId = pActivityCfg->wDataId;
// 
// 	// 取记录数据
// 	RecordData *pRecordData = FindSingleRechargeRewardRecord(wDataId);
// 
// 	// 已经领取的数量
// 	WORD wTakeNum = pRecordData ? pRecordData->wRecordNum : 0;
// 
// 	// 检测领取次数是否已达上限(配置上限)
// 	if (wTakeNum >= pActivityCfg->wRewardTimesMax)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]领取次数已达上限,不能领取奖励(配置上限)[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	// 检测领取次数是否已达上限(充值上限)
// 	if (wTakeNum >= GetRechargeTimes(pActivityCfg->dwContext))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]领取次数已达上限,不能领取奖励(充值上限)[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]添加限时奖励记录失败[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	m_pPlayer->AddObject(pActivityCfg->vRewardItemList);
// 
// 	return true;
// }
// 
// // 处理领取累充奖励
// bool CLimitActivity::DoTakeAccumulateRechargeReward(const LimitActivity_Config *pActivityCfg)
// {
// 	UpdateActivityData(SYSTEM_AMT_RCG);
// 
// 	WORD wDataId = pActivityCfg->wDataId;
// 
// 	// 检测额度是否足够
// 	if (m_pLimitActivityData->dwAccumulateRecharge < pActivityCfg->dwContext)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]累充的额度不足,不能领取奖励[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	// 检测是否已领取过
// 	if (IsTakeAccumulateRechargeReward(wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]累充的奖励已领取过[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]添加限时奖励记录失败[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	m_pPlayer->AddObject(pActivityCfg->vRewardItemList);
// 
// 	return true;
// }
// 
// // 处理限时招募奖励
// bool CLimitActivity::DoTakeLimitRecruitReward(const LimitActivity_Config *pActivityCfg)
// {
// 	WORD wDataId = pActivityCfg->wDataId;
// 
// 	// 检测招募次数是否足够
// 	if (m_pLimitActivityData->wRecruitTimes < pActivityCfg->dwContext)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]招募次数不足,不能领取奖励[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	// 检测是否已领取过
// 	if (IsTakeLimitRecruitReward(wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]累充的奖励已领取过[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	if (!AddRewardRecord(pActivityCfg->wType, wDataId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]添加限时奖励记录失败[data_id:%d]", __FUNCTION__, wDataId);
// 		return false;
// 	}
// 
// 	m_pPlayer->AddObject(pActivityCfg->vRewardItemList);
// 
// 	return true;
// }

// 添加累充值
void CLimitActivity::AddRechargeValue(DWORD dwValue)
{
	//UpdateActivityData(SYSTEM_AMT_RCG);
	///m_pLimitActivityData->dwAccumulateRecharge += dwValue;

	m_pLimitActivityData->dwTotleRecharge += dwValue;
}

// 添加充值数据
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
// 			MAPLOG(ERROR_LEVEL, "[%s]充值记录列表已满", __FUNCTION__);
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
// // 添加招募次数
// void CLimitActivity::AddRecruitTimes(WORD wRecruitTimes)
// {
// 	m_pLimitActivityData->wRecruitTimes += wRecruitTimes;
// }

// 是否有红点
bool CLimitActivity::IsHaveRedPoint(WORD wSystemId)
{
	map<DWORD, LimitActivity_Config>::iterator iter = g_pCfgMgr->m_LimitActivityList.begin();
	if (iter == g_pCfgMgr->m_LimitActivityList.end())
		return false;

	// 74单次返利
// 	if (SYSTEM_SGL_RCG == wSystemId)
// 	{
// 		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
// 		{
// 			// 只处理单充的
// 			if (iter->second.wType != LIMIT_ACTIVITY_SINGLE)
// 				continue;
// 
// 			const LimitActivity_Config *pActivityCfg = g_pCfgMgr->GetLimitActivity(iter->second.wDataId);
// 			if (!pActivityCfg)
// 				continue;
// 
// 			// 取记录数据
// 			RecordData *pRecordData = FindSingleRechargeRewardRecord(pActivityCfg->wDataId);
// 
// 			// 已经领取的数量
// 			WORD wTakeNum = pRecordData ? pRecordData->wRecordNum : 0;
// 
// 			// 检测领取次数是否已达上限(配置上限)
// 			if (wTakeNum >= pActivityCfg->wRewardTimesMax)
// 				continue;
// 
// 			// 检测领取次数是否已达上限(充值上限)
// 			if (wTakeNum >= m_pPlayer->GetRechargeManager()->GetRechargeTimes(pActivityCfg->dwContext))
// 				continue;
// 
// 			return true;
// 		}
// 	}
// 	// 75累充豪礼
// 	else if (SYSTEM_AMT_RCG == wSystemId)
// 	{
// 		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
// 		{
// 			const LimitActivity_Config *pCfg = &iter->second;
// 
// 			// 额度满足
// 			if (m_pLimitActivityData->dwAccumulateRecharge >= pCfg->dwContext && pCfg->wType == LIMIT_ACTIVITY_ACCUMULATE)
// 			{
// 				// 是否还木有领取
// 				if (!IsTakeAccumulateRechargeReward(pCfg->wDataId))
// 					return true;
// 			}
// 		}
// 	}
	// 76首充
	if (SYSTEM_FRT_RCG == wSystemId)
	{
		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
		{
			// 额度满足
			if (m_pLimitActivityData->dwTotleRecharge >= iter->second.dwContext &&
				iter->second.wType == LIMIT_ACTIVITY_FIRST)
			{
				// 是否还木有领取
				if (!IsTakeFirstRechargeReward(iter->second.wDataId))
					return true;
			}
		}
	}
	// 77限时召唤
// 	else if (SYSTEM_LMT_RCT == wSystemId)
// 	{
// 		// 是否在活动期间
// 		// todo::是否活动时间
// 		if (!CActivityManager::IsActivityOpen(SYSTEM_LMT_RCT, m_pPlayer->GetServerId(), m_pPlayer->GetLevel()))
// 			return false;
// 
// 		for (; iter != g_pCfgMgr->m_LimitActivityList.end(); ++iter)
// 		{
// 			// 次数满足
// 			if (m_pLimitActivityData->wRecruitTimes >= iter->second.dwContext && 
// 				iter->second.wType == LIMIT_ACTIVITY_RECRUIT)
// 			{
// 				// 是否还木有领取
// 				if (!IsTakeLimitRecruitReward(iter->second.wDataId))
// 					return true;
// 			}
// 		}
// 	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的红点提示类型[SystemId:%d]", __FUNCTION__, wSystemId);
		return false;
	}

	return false;
}

// 是否已领取首充奖励
bool CLimitActivity::IsTakeFirstRechargeReward(WORD wDataId)
{
	for (WORD i = 0; i < m_pLimitActivityData->wFirstRechargeRewardRecordNum; ++i)
	{
		if (m_pLimitActivityData->arFirstRechargeRewardRecord[i] == wDataId)
			return true;
	}

	return false;
}

// 是否已领取限时招募奖励
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
 
 // 是否已领取了累充奖励
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
 
// // 是否已领取了奖励
// bool CLimitActivity::IsTakeReward(WORD wDataId)
// {
// 	return true;
// }

// 添加奖励记录
bool CLimitActivity::AddRewardRecord(WORD wType, WORD wDataId)
{
	// 单充
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
// 			// 检测列表是否已满
// 			if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
// 			{
// 				MAPLOG(ERROR_LEVEL, "[%s]限时活动奖励记录列表已满[type:%d]", __FUNCTION__, wType);
// 				return false;
// 			}
// 
// 			pRecordList[wRecordNum].wRecordId = wDataId;
// 			pRecordList[wRecordNum].wRecordNum = 1;
// 			wRecordNum++;
// 		}
// 	}
	// 累充
// 	else if (LIMIT_ACTIVITY_ACCUMULATE == wType)
// 	{
// 		WORD &wRecordNum = m_pLimitActivityData->wAccumulateRechargeRewardRecordNum;
// 		WORD *pRecordList = m_pLimitActivityData->arAccumulateRechargeRewardRecord;
// 
// 		// 检测列表是否已满
// 		if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]限时活动奖励记录列表已满[type:%d]", __FUNCTION__, wType);
// 			return false;
// 		}
// 
// 		pRecordList[wRecordNum++] = wDataId;
// 	}
	// 首充
	if (LIMIT_ACTIVITY_FIRST == wType)
	{
		WORD &wRecordNum = m_pLimitActivityData->wFirstRechargeRewardRecordNum;
		WORD *pRecordList = m_pLimitActivityData->arFirstRechargeRewardRecord;

		// 检测列表是否已满
		if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]限时活动奖励记录列表已满[type:%d]", __FUNCTION__, wType);
			return false;
		}

		pRecordList[wRecordNum++] = wDataId;
	}
	// 招募
// 	else if (LIMIT_ACTIVITY_RECRUIT == wType)
// 	{
// 		WORD &wRecordNum = m_pLimitActivityData->wRecruitRewardRecordNum;
// 		WORD *pRecordList = m_pLimitActivityData->arRecruitRewardRecord;
// 
// 		// 检测列表是否已满
// 		if (wRecordNum >= MAX_LIMIT_ACTIVITY_REWARD_NUM)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]限时活动奖励记录列表已满[type:%d]", __FUNCTION__, wType);
// 			return false;
// 		}
// 
// 		pRecordList[wRecordNum++] = wDataId;
// 	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的限时活动类型[type:%d]", __FUNCTION__, wType);
		return false;
	}

	return true;
}

// 更新活动数据
void CLimitActivity::UpdateActivityData(int nSystemId)
{
// 	WORD wDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(nSystemId);
// 	if (wDataId)
// 	{
// 		// 单充
// 		if (SYSTEM_SGL_RCG == nSystemId && m_pLimitActivityData->wLastSingleRechargeDataId != wDataId)
// 		{
// 			m_pLimitActivityData->wLastSingleRechargeDataId = wDataId;
// 			m_pLimitActivityData->wSingleRechargeRewardRecordNum = 0;
// 			m_pLimitActivityData->wRechargeNum = 0;
// 		}
// 		// 累充
// 		else if (SYSTEM_AMT_RCG == nSystemId && m_pLimitActivityData->wLastAccumulateRechargeDataId != wDataId)
// 		{
// 			m_pLimitActivityData->wLastAccumulateRechargeDataId = wDataId;
// 			m_pLimitActivityData->wAccumulateRechargeRewardRecordNum = 0;
// 			m_pLimitActivityData->dwAccumulateRecharge = 0;
// 		}
// 	}
}

// 取充值次数
// WORD CLimitActivity::GetRechargeTimes(WORD wRechargeId)
// {
// 	CommonRecharge *pRecharge = FindCommonRecharge(wRechargeId);
// 	return pRecharge ? pRecharge->wRechargeTimes : 0;
// }

// 查找普通充值数据
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

// 取单充奖励记录
// RecordData* CLimitActivity::FindSingleRechargeRewardRecord(WORD wDataId)
// {
// 	for (WORD i = 0; i < m_pLimitActivityData->wSingleRechargeRewardRecordNum; ++i)
// 	{
// 		if (m_pLimitActivityData->arSingleRechargeRewardRecord[i].wRecordId == wDataId)
// 			return &m_pLimitActivityData->arSingleRechargeRewardRecord[i];
// 	}
// 	return NULL;
// }