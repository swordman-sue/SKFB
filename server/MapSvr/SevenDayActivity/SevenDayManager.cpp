#include "stdafx.h"
#include "SevenDayManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\PlayerManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\PlayerBaseDataManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\RedPoint\RedPointManager.h"
#include "..\MapServer.h"
#include "..\Shop\ShopManager.h"
#include "..\Activity\ActivityManager.h"


CSevenDayManager::CSevenDayManager()
{
	
}

CSevenDayManager::~CSevenDayManager()
{
	
}

void CSevenDayManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pActivityInfo = &pPlayer->GetRoleFullInfo().tActivityInfo;
}

// 请求目标列表
void CSevenDayManager::OnNetTargetList()
{
	SendTargetList();
}

// 领取目标奖励
void CSevenDayManager::OnNetTargetAward(WORD wTargetId)
{
	WORD wFromMsgId = C2S_SEVEN_DAY_GET_REWARD_REQ;

	// 取目标配置
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	SevenDayTargetData *pTargetData = GetTargetData(wTargetId);
	if (!pTargetData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]还木有此目标,不能领取奖励![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// 升序目标
	if (pTargetCfg->byIsAsc)
	{
		if (pTargetData->dwValue < pTargetCfg->nX)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]还木有完成此目标,不能领取奖励![TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}
	// 降序目标
	else
	{
		if (pTargetData->dwValue > pTargetCfg->nX)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]还木有完成此目标,不能领取奖励![TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}

	if (pTargetData->byIsGetReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]此目标奖励已领取过![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_SEVEN_DAY_TARGET_REWARD);

	pTargetData->byIsGetReward = true;

	// 响应
	MSG_S2C_SEVEN_DAY_GET_REWARD_RESP msg;
	msg.m_wTargetId = wTargetId;
	m_pPlayer->SendPacket(&msg);
}

// 领取完成度奖励 
void CSevenDayManager::OnNetCompletionReward(WORD wCompletionNum)
{
	WORD wFromMsgId = C2S_SEVEN_DAY_COMPLETION_REWARD_REQ;

	// 检测是否合法的完成度奖励
	const SevenDayCompletionReward_Config *pCompletionRewardCfg = g_pCfgMgr->GetSevenDayCompletionReward(wCompletionNum);
	if (!pCompletionRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天完成度奖励配置![CompletionNum:%d]", __FUNCTION__, wCompletionNum);
		return;
	}

	// 检测是否可领取
	WORD wAlreadyNum = GetComplationNum();
	if (wAlreadyNum < wCompletionNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]完成度还没有达成,不能领取![CompletionNum:%d]", __FUNCTION__, wCompletionNum);
		return;
	}

	// 检测是否已领取过
	if (IsComplationRewardRecord(wCompletionNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]7天完成度奖励已领取过![CompletionNum:%d]", __FUNCTION__, wCompletionNum);
		return;
	}

	// 添加领取记录
	if (m_pActivityInfo->wCompletionRewardRecordNum < MAX_SEVEN_DAY_COMPLETION_REWARD)
		m_pActivityInfo->arCompletionRewardRecord[m_pActivityInfo->wCompletionRewardRecordNum++] = wCompletionNum;

	// 发奖励
	m_pPlayer->AddObjectList(pCompletionRewardCfg->vRewardItemList, TRUE, FROM_SEVEN_DAY_COMPLETIO_REWARD);

	// 响应
	MSG_S2C_SEVEN_DAY_COMPLETION_REWARD_RESP msg;
	msg.m_wComplecton_num = wCompletionNum;
	m_pPlayer->SendPacket(&msg);
}

// 发送目标列表
void CSevenDayManager::SendTargetList()
{
	MSG_S2C_SEVEN_DAY_TARGET_LIST_RESP msg;
	msg.m_byTargetNum = 0;
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		msg.m_arTargetList[msg.m_byTargetNum++] = m_pActivityInfo->arSevenDayTargetList[i];
	}
	msg.m_byRewardRecordNum = 0;
	for (WORD i = 0; i < m_pActivityInfo->wCompletionRewardRecordNum; ++i)
	{
		msg.m_arRewardRecordList[msg.m_byRewardRecordNum++] = m_pActivityInfo->arCompletionRewardRecord[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// 发送目标数据
void CSevenDayManager::SendTargetData(const SevenDayTargetData *pData)
{
	MSG_S2C_SEVEN_DAY_TARGET_DATA_NOTICE msg;
	msg.m_tTargetData = *pData;
	m_pPlayer->SendPacket(&msg);
}

// 更新目标数据
void CSevenDayManager::OnUpdateTarget(WORD wLogicType, DWORD dwX, bool bAdd)
{
	const vector<WORD> *pTargetList = g_pCfgMgr->GetTheSameLogicTargetList(wLogicType);
	if (!pTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据![LogicType:%d]", __FUNCTION__, wLogicType);
		return;
	}

	for (size_t i = 0; i < pTargetList->size(); ++i)
	{
		DoUpdateTarget(pTargetList->at(i), dwX, bAdd);
	}
}

// 更新目标数据
void CSevenDayManager::OnUpdateTarget(WORD wLogicType)
{
	const vector<WORD> *pTargetList = g_pCfgMgr->GetTheSameLogicTargetList(wLogicType);
	if (!pTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据![LogicType:%d]", __FUNCTION__, wLogicType);
		return;
	}

	for (size_t i = 0; i < pTargetList->size(); ++i)
	{
		DoUpdateTarget(pTargetList->at(i));
	}
}

// 是否有红点
bool CSevenDayManager::IsHaveRedPoint()
{
	WORD wActivityDays = GetActivityDay();

	// todo::暂时这处理写
	wActivityDays = min(wActivityDays, 7);

	// 目标奖励
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		const SevenDayTargetData *pTargetData = &m_pActivityInfo->arSevenDayTargetList[i];
		// 已领取
		if (pTargetData->byIsGetReward)
			continue;

		// 取成就配置
		const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(pTargetData->wId);
		if (!pTargetCfg)
			continue;

		if ((pTargetCfg->byIsAsc && pTargetData->dwValue >= pTargetCfg->nX) ||	// 如果是升序,则大值有效
			(!pTargetCfg->byIsAsc && pTargetData->dwValue <= pTargetCfg->nX))	// 如果是降序,则小值有效
		{
			// 检测是否当前及之前的天数目标(即当前是否可以去领取的目标)
			for (WORD k = 1; k <= wActivityDays; ++k)
			{
				if (IsTheDayTarget(pTargetData->wId, k))
					return true;
			}
		}
	}

	// 完成度奖励
//  	WORD wComplationNum = GetComplationNum();
// 	map<WORD, SevenDayCompletionReward_Config>::iterator iter = g_pCfgMgr->m_SevenDayCompletionRewardList.begin();
// 	for (; iter != g_pCfgMgr->m_SevenDayCompletionRewardList.end(); ++iter)
// 	{
// 		// 已经完成,但木有领取
// 		if (wComplationNum >= iter->second.wCompletionNum && !IsComplationRewardRecord(iter->first))
// 			return true;
// 	}


	// 7天商店
	CShopManager *pShopMgr = m_pPlayer->GetShopManager();
	for (size_t i = 0; i < CConfigManager::Instance()->m_vSevenDayShopGoodsList.size(); ++i)
	{
		DWORD dwGoodsId = CConfigManager::Instance()->m_vSevenDayShopGoodsList[i];
		// 已经购买了
		if (pShopMgr->GetGoodsBuyRecordNum(SHOP_SEVEN_DAY, dwGoodsId))
			continue;

		// 检测是否当前及之前的天数目标(即当前是否可以去领取的目标)
		for (WORD k = 1; k <= wActivityDays; ++k)
		{
			const vector<DWORD> *pActivityList = g_pCfgMgr->GetServerDayActivity(k);
			if (!pActivityList)
				continue;

			vector<DWORD>::const_iterator iter = pActivityList->begin();
			for (; iter != pActivityList->end(); ++iter)
			{
				const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(*iter);
				if (pTargetCfg && pTargetCfg->wLogicType == 19 && pTargetCfg->nX == dwGoodsId)
					return true;
			}
		}
	}

	return false;
}

// 是否有红点
bool CSevenDayManager::IsHaveRedPointEx()
{
	WORD wActivityDays = GetActivityDay();

	if (wActivityDays <= 7)
		return false;

	// todo::暂时这处理写
	wActivityDays = min(wActivityDays, 14);

	// 目标奖励
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		const SevenDayTargetData *pTargetData = &m_pActivityInfo->arSevenDayTargetList[i];
		// 已领取
		if (pTargetData->byIsGetReward)
			continue;

		// 取成就配置
		const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(pTargetData->wId);
		if (!pTargetCfg)
			continue;

		if ((pTargetCfg->byIsAsc && pTargetData->dwValue >= pTargetCfg->nX) ||	// 如果是升序,则大值有效
			(!pTargetCfg->byIsAsc && pTargetData->dwValue <= pTargetCfg->nX))	// 如果是降序,则小值有效
		{
			// 检测是否当前及之前的天数目标(即当前是否可以去领取的目标)
			for (WORD k = 8; k <= wActivityDays; ++k)
			{
				if (IsTheDayTarget(pTargetData->wId, k))
					return true;
			}
		}
	}

	// 7天商店
	CShopManager *pShopMgr = m_pPlayer->GetShopManager();
	for (size_t i = 0; i < CConfigManager::Instance()->m_vSevenDayShopGoodsList.size(); ++i)
	{
		DWORD dwGoodsId = CConfigManager::Instance()->m_vSevenDayShopGoodsList[i];
		// 已经购买了
		if (pShopMgr->GetGoodsBuyRecordNum(SHOP_SEVEN_DAY, dwGoodsId))
			continue;

		// 检测是否当前及之前的天数目标(即当前是否可以去领取的目标)
		for (WORD k = 8; k <= wActivityDays; ++k)
		{
			const vector<DWORD> *pActivityList = g_pCfgMgr->GetServerDayActivity(k);
			if (!pActivityList)
				continue;

			vector<DWORD>::const_iterator iter = pActivityList->begin();
			for (; iter != pActivityList->end(); ++iter)
			{
				const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(*iter);
				if (pTargetCfg && pTargetCfg->wLogicType == 19 && pTargetCfg->nX == dwGoodsId)
					return true;
			}
		}
	}

	return false;
}

// 更新目标数据
void CSevenDayManager::DoUpdateTarget(WORD wTargetId, DWORD dwValue, bool bAdd)
{
	// 取目标配置
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据![TargetId:%d]", __FUNCTION__, wTargetId);
 		return;
 	}

	// 检测是否当前活动目标
	if (!IsCurActivityTarget(wTargetId))
		return;
 
 	//  玩家成就数据
	SevenDayTargetData *pTargetData = GetTargetData(wTargetId);
	if (!pTargetData)
 	{
		SevenDayTargetData tData;
 		memset(&tData, 0, sizeof(tData));
		tData.wId = wTargetId;
 		AddTargetData(tData);
		pTargetData = GetTargetData(wTargetId);
 	}

	if (!pTargetData)
		return;

	// 升序目标
	if (pTargetCfg->byIsAsc)
	{
		// 成就已完成
		if (pTargetData->dwValue >= pTargetCfg->nX)
			return;
	}
	// 降序目标
	else
	{
		// 成就已完成
		if (pTargetData->dwValue && pTargetData->dwValue <= pTargetCfg->nX)
			return;
	}

	// 添加值
	if (bAdd)
	{
		pTargetData->dwValue += dwValue;
	}
	// 设置值
	else
	{		
		// 升序目标
		if (pTargetCfg->byIsAsc)
		{
			// 使用最佳记录
			if (dwValue <= pTargetData->dwValue)
				return;
		}
		// 降序目标
		else
		{
			// 使用最佳记录
			if (pTargetData->dwValue && dwValue >= pTargetData->dwValue)
				return;
		}

		pTargetData->dwValue = dwValue;

		// 升序目标
		if (pTargetCfg->byIsAsc)
		{
			// 纠正目标值
			pTargetData->dwValue = min(pTargetData->dwValue, pTargetCfg->nX);
		}
		// 降序目标
		else
		{
			// 纠正目标值
			pTargetData->dwValue = max(pTargetData->dwValue, pTargetCfg->nX);
		}
	}

	SendTargetData(pTargetData);
}

// 更新目标数据
void CSevenDayManager::DoUpdateTarget(WORD wTargetId)
{
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// 检测是否当前活动目标
	if (!IsCurActivityTarget(wTargetId))
		return;

	SevenDayTargetData *pTargetData = GetTargetData(wTargetId);

	// 已完成
	if (pTargetData && pTargetData->dwValue >= pTargetCfg->nX)
		return;

	WORD wNum = 0;

	switch (pTargetCfg->wLogicType)
	{
	// 装备强化
	case 5:
	{
		CEquipmentManager *pEquipMgr = m_pPlayer->GetEquipmentManager();
		CHECK_POINTER(pEquipMgr);
		wNum = pEquipMgr->GetEquipmentNumByStrengthenLevel(pTargetCfg->nY);
	}
	break;
	// 圣器收集
	case 7:
	{
		CTreasureManager *pTreasureMgr = m_pPlayer->GetTreasureManager();
		CHECK_POINTER(pTreasureMgr);
		wNum = pTreasureMgr->GetTreasureNumByQuality(pTargetCfg->nY);
	}
	break;
	// 英雄突破
	case 9:
	{
		CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
		CHECK_POINTER(pHeroMgr);
		wNum = pHeroMgr->GetHeroNumByBreakLevel(pTargetCfg->nY);
	}
	break;
	// 英雄潜能提升等级(天命)
	case 11:
	{
		CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
		CHECK_POINTER(pHeroMgr);
		wNum = pHeroMgr->GetHeroNumByDestinyLevel(pTargetCfg->nY);
	}
	break;
	// 英雄培养
	case 15:
	{
		CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
		CHECK_POINTER(pHeroMgr);
		wNum = pHeroMgr->GetHeroNumByTrainLevel(pTargetCfg->nY);
	}
	break;
	// 装备品质
	case 13:
	{
		CEquipmentManager *pEquipMgr = m_pPlayer->GetEquipmentManager();
		CHECK_POINTER(pEquipMgr);
		wNum = pEquipMgr->GetEquipmentNumByQuality(pTargetCfg->nY);
	}
	break;
	// 装备精炼
	case 14:
	{
		CEquipmentManager *pEquipMgr = m_pPlayer->GetEquipmentManager();
		CHECK_POINTER(pEquipMgr);
		wNum = pEquipMgr->GetEquipmentNumByRefineLevel(pTargetCfg->nY);
	}
	break;
	default:
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的7天目标逻辑类型![TargetI:%d,LogicType:%d]",
			__FUNCTION__, wTargetId, pTargetCfg->wLogicType);
		return;
	}

	// 一个都木有，还敢进来。。。。。
	if (!wNum)
		return;

	if (pTargetData)
	{
		pTargetData->dwValue = wNum;

		SendTargetData(pTargetData);
	}
	else
	{
		SevenDayTargetData tTargetData;
		memset(&tTargetData, 0, sizeof(tTargetData));
		tTargetData.wId = wTargetId;
		tTargetData.dwValue = wNum;
		AddTargetData(tTargetData);

		SendTargetData(&tTargetData);
	}
}

// 取成就数据
SevenDayTargetData* CSevenDayManager::GetTargetData(WORD wTargetId)
{
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		if (m_pActivityInfo->arSevenDayTargetList[i].wId == wTargetId)
		{
			return &m_pActivityInfo->arSevenDayTargetList[i];
		}
	}
	return NULL;
}

// 添加成就数据
void CSevenDayManager::AddTargetData(const SevenDayTargetData &tTargetData)
{
	if (m_pActivityInfo->wSevenDayTargetNum >= MAX_SEVEN_DAY_TARGET_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]7天目标列表已满![ListNum:%d]", __FUNCTION__, m_pActivityInfo->wSevenDayTargetNum);
		return;
	}

	m_pActivityInfo->arSevenDayTargetList[m_pActivityInfo->wSevenDayTargetNum++] = tTargetData;
}

// 取活动天数
WORD CSevenDayManager::GetActivityDay()
{
	return g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());
}

// 是否今日的目标
bool CSevenDayManager::IsTheDayTarget(WORD wTargetId, WORD wActivityDays)
{
	const vector<DWORD> *pDayTargetList = g_pCfgMgr->GetServerDayActivity(wActivityDays);
	if (!pDayTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天活动的配置数据![Day:%d]", __FUNCTION__, wActivityDays);
		return false;
	}

	for (size_t i = 0; i < pDayTargetList->size(); ++i)
	{
		if (pDayTargetList->at(i) == wTargetId)
			return true;
	}

	return false;
}

// 取当前完成度
WORD CSevenDayManager::GetComplationNum()
{
	WORD wComplationNum = 0;
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		SevenDayTargetData *pTargetData = &m_pActivityInfo->arSevenDayTargetList[i];
		const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(pTargetData->wId);
		if (pTargetCfg && pTargetCfg->byIsCompletionTarget && pTargetData->byIsGetReward)
			wComplationNum++;
	}
	return wComplationNum;
}

// 是否完成度奖励记录
bool CSevenDayManager::IsComplationRewardRecord(WORD wComplationValue)
{
	for (WORD i = 0; i < m_pActivityInfo->wCompletionRewardRecordNum; ++i)
	{
		if (m_pActivityInfo->arCompletionRewardRecord[i] == wComplationValue)
			return true;
	}
	return false;
}

// 是否当前活动目标
bool CSevenDayManager::IsCurActivityTarget(WORD wTargetId)
{
	// 取目标配置
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到7天目标配置数据![TargetId:%d]", __FUNCTION__, wTargetId);
		return false;
	}

	WORD wActivityDay = GetActivityDay();

	// 检测活动是否已结束 todo::暂时这样处理 
	if (pTargetCfg->wBelongActivityDay > 7)
	{ // 8-14天活动
		// 检测是否活动期
		if (!CActivityManager::IsActivityOpen(SYSTEM_SEVEN_DAY_2, m_pPlayer))
			return false;
	}
	else
	{ // 7天活动
		// 检测是否活动期
		if (!CActivityManager::IsActivityOpen(SYSTEM_SEVEN_DAY_1, m_pPlayer))
			return false;
	}

	// 不是今日目标数据
	if (pTargetCfg->byIsTheSameDayValid && !IsTheDayTarget(wTargetId, wActivityDay))
		return false;

	// 是否从当天开始有效, 如果是,则必需从那一天开始才会成效
	if (pTargetCfg->byIsFromTheSameDayValid && wActivityDay < pTargetCfg->wBelongActivityDay)
		return false;

	return true;
}