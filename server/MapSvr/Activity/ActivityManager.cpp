#include "stdafx.h"
#include "ActivityManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Guild\GuildManager.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\AccountInfo.h"
#include "..\Friend\FriendManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Shop\ShopManager.h"
#include "..\Recharge\RechargeManager.h"


CActivityManager::CActivityManager()
{

}

CActivityManager::~CActivityManager()
{

}

void CActivityManager::Init(Player *pPlayer)
{
	m_LimitActivity.Init(pPlayer, this);
	m_SuperDiscount.Init(pPlayer, this);
	m_FestivalActivity.Init(pPlayer, this);
	m_AccurateRecommend.Init(pPlayer, this);
	m_FacebookActivity.Init(pPlayer,this);
	m_SevenDayLogin.Init(pPlayer, this);
	m_RechargeReward.Init(pPlayer, this);
	m_TotalRechargeReward.Init(pPlayer, this);
	m_NewSuperDiscount.Init(pPlayer, this);
	m_HotSaleGoods.Init(pPlayer, this);
	m_CampRecruit.Init(pPlayer, this);
	m_UnlockActivity.Init(pPlayer, this);
	m_RechargeAndCostRankActivity.Init(pPlayer, this);

	m_pPlayer = pPlayer;
	m_pSignInData = &pPlayer->GetRoleFullInfo().tSignInData;
	m_pActivityInfo = &pPlayer->GetRoleFullInfo().tActivityInfo;
	m_pLuxurySignInData = &pPlayer->GetRoleFullInfo().tSecondInfo.tLuxurySignData;
	m_pLoginRewardData = &pPlayer->GetRoleFullInfo().tSecondInfo.tLoginRewardData;
}

void CActivityManager::OnNewDay()
{
	InitLuxurySignInData();
	ResetTodayRecommendRechargeData();
	m_AccurateRecommend.OnNewDay();
	m_FestivalActivity.OnNewDay();
	m_HotSaleGoods.OnNewDay();

	m_pActivityInfo->tRechargeData.wTodayRechargetTimes = 0;

	// todo::还有部分数据在角色身上处理重置，怕出问题暂时不移过来
}

// 登出
void CActivityManager::OnLogout()
{
	
}

// 签到
void CActivityManager::OnNetSignIn()
{
	// 检测是否需要重置(签满以后需要重新来过)
	if (m_pSignInData->wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay() && !IsTheSameDay(m_pSignInData->dwLastSignInTime, time(NULL)))
		m_pSignInData->wCurSignInDay = 0;

	// 检测今日是否已签过
	DATETIME lastSignTime(m_pSignInData->dwLastSignInTime);
	DATETIME curTime(time(NULL));
	if (curTime.day == lastSignTime.day && curTime.month == lastSignTime.month)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SIGN_IN_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]今日已签过", __FUNCTION__);
		return;
	}

	// 当前签到天数
	WORD wCurSignInDay = m_pSignInData->wCurSignInDay + 1;

	// 取签到配置
	const SignIn_Config *pSignInCfg = g_pCfgMgr->GetSignIn(wCurSignInDay);
	if (!pSignInCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SIGN_IN_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到签到配置数据[day:%d]", __FUNCTION__, wCurSignInDay);
		return;
	}

	WORD wItemNum = pSignInCfg->tRewardItem.dwNum;

	// 是否满足双倍奖励
	if (pSignInCfg->byDoubleRewardVipLv/* && m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= pSignInCfg->byDoubleRewardVipLv*/)
	{
		WORD wVipLevel = m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);

		// 如果是996BT版本
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
			wVipLevel = m_pPlayer->GetVipGMLevel();

		if (wVipLevel >= pSignInCfg->byDoubleRewardVipLv)
			wItemNum *= 2;
	}

	// 添加奖励
	m_pPlayer->AddObject(pSignInCfg->tRewardItem.dwId, wItemNum, true, FROM_SIGN_IN);

	// 更新签到数据
	m_pSignInData->dwLastSignInTime = time(NULL);
	m_pSignInData->wCurSignInDay = wCurSignInDay;

	// 检测是否需要重置(签满以后需要重新来过)
// 	if (wCurSignInDay > g_pCfgMgr->GetSignInTotalDay())
// 	{
// 		m_pSignInData->wCurSignInDay = 0;
// 	}

	// 主动同步数据给客户端
	SendSignInData();

	// 响应
	MSG_S2C_SIGN_IN_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 签到
void CActivityManager::OnNetSignInForTest()
{
	// 检测是否需要重置(签满以后需要重新来过)
	if (m_pSignInData->wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay() && !IsTheSameDay(m_pSignInData->dwLastSignInTime, time(NULL)))
		m_pSignInData->wCurSignInDay = 0;

	// 当前签到天数
	WORD wCurSignInDay = m_pSignInData->wCurSignInDay + 1;

	// 取签到配置
	const SignIn_Config *pSignInCfg = g_pCfgMgr->GetSignIn(wCurSignInDay);
	if (!pSignInCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到签到配置数据[day:%d]", __FUNCTION__, wCurSignInDay);
		return;
	}

	WORD wItemNum = pSignInCfg->tRewardItem.dwNum;

	// 是否满足双倍奖励
	if (pSignInCfg->byDoubleRewardVipLv && m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= pSignInCfg->byDoubleRewardVipLv)
	{
		wItemNum *= 2;
	}

	// 添加奖励
	m_pPlayer->AddObject(pSignInCfg->tRewardItem.dwId, wItemNum, true, FROM_SIGN_IN);

	// 更新签到数据
	m_pSignInData->dwLastSignInTime = time(NULL);
	m_pSignInData->wCurSignInDay = wCurSignInDay;

	// 检测是否需要重置(签满以后需要重新来过)
// 	if (wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay())
// 	{
// 		m_pSignInData->wCurSignInDay = 0;
// 	}

	// 主动同步数据给客户端
	SendSignInData();
}

// 请求签到数据
void CActivityManager::OnNetSignInDataReq()
{
	SendSignInData();
}

// 发送签到数据
void CActivityManager::SendSignInData()
{
	// 检测是否需要重置(签满以后需要重新来过)
	if (m_pSignInData->wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay() && !IsTheSameDay(m_pSignInData->dwLastSignInTime,time(NULL)))
		m_pSignInData->wCurSignInDay = 0;

	MSG_S2C_SIGN_IN_DATA_RESP msg;
	msg.m_byCurSignInDay = m_pSignInData->wCurSignInDay;
	msg.m_dwLastSignInTime = m_pSignInData->dwLastSignInTime;
	m_pPlayer->SendPacket(&msg);
}

// 领取豪华签到奖励
void CActivityManager::OnNetTakeLuxurySignReward(WORD wSignId)
{
	LuxurySignData *pSignData = NULL;
	for (WORD i = 0; i < m_pLuxurySignInData->wSignDataNum; ++i)
	{
		if (m_pLuxurySignInData->arSignDataList[i].wSignId == wSignId)
		{
			pSignData = &m_pLuxurySignInData->arSignDataList[i];
			break;
		}
	}

	if (!pSignData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家身上的签到数据[signin_id:%d]", __FUNCTION__, wSignId);
		return;
	}

	// 检测是否可以领取
	if (LUXURY_RECHARGE_CAN_TAKE != pSignData->wStatus)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]豪华签到还不能领取[signin_id:%d,status:%d]", __FUNCTION__, wSignId, pSignData->wStatus);
		return;
	}

	// 发放奖励
	const LuxurySignInReward_Config *pSignInRewardCfg =  g_pCfgMgr->CConfigManager::GetLuxurySignInReward(pSignData->wRewardId);
	if (!pSignInRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到豪华签到奖励配置[reward_id:%d]", __FUNCTION__, pSignData->wRewardId);
		return;
	}

	m_pPlayer->AddObjectList(pSignInRewardCfg->vItemList, TRUE, FROM_LUXURY_SIGN_REWARD);

	pSignData->wStatus = LUXURY_RECHARGE_ALREADY_TAKE;

	SendLuxurySignData();

	MSG_S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP msg;
	msg.m_wSignInId = wSignId;
	m_pPlayer->SendPacket(&msg);
}

// 请求豪华签到数据
void CActivityManager::OnNetLuxurySignData()
{
	SendLuxurySignData();
}

// 发送豪华签到数据
void CActivityManager::SendLuxurySignData()
{
	MSG_S2C_LUXURY_SIGN_IN_DATA_RESP msg;
	msg.m_wSignInDataNum = 0;
	for (WORD i = 0; i < m_pLuxurySignInData->wSignDataNum; ++i)
	{
		msg.m_arSignInDataLList[msg.m_wSignInDataNum++] = m_pLuxurySignInData->arSignDataList[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// 初始化豪华签到数据
void CActivityManager::InitLuxurySignInData()
{
	m_pLuxurySignInData->wSignDataNum = 0;
	const map<WORD, map<WORD, LuxurySign_Config>> &luxurySignInList = g_pCfgMgr->GetLuxurySignInList();
	map<WORD, map<WORD, LuxurySign_Config>>::const_iterator iter1 = luxurySignInList.begin();
	for (; iter1 != luxurySignInList.end(); ++iter1)
	{
		LuxurySignData &signData = m_pLuxurySignInData->arSignDataList[m_pLuxurySignInData->wSignDataNum++];
		signData.wSignId = iter1->first;
		signData.wStatus = 0;
		map<WORD, LuxurySign_Config>::const_iterator iter2 = iter1->second.find(m_pPlayer->GetLevel());
		if (iter2 == iter1->second.end())
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到豪华签到配置[signin_id:%d, role_evel:%d]", __FUNCTION__, iter1->first,m_pPlayer->GetLevel());
			return;
		}

		// 计算总概率
		int nTotalRate = 0;
		for (size_t i = 0; i < iter2->second.vRewardRandList.size(); ++i)
		{
			nTotalRate += iter2->second.vRewardRandList[i].nRate;
		}
			
		// 随机概率
		int nRandValue = g_Rand(0, nTotalRate);
		int nCurRate = 0;
		for (size_t i = 0; i < iter2->second.vRewardRandList.size(); ++i)
		{
			nCurRate += iter2->second.vRewardRandList[i].nRate;
			if (nCurRate >= nRandValue)
			{
				signData.wRewardId = iter2->second.vRewardRandList[i].nValue;
				break;
			}
		}
	}
}

// 豪华签到充值
void CActivityManager::LuxurySignInRecharge(WORD wRechangeId)
{
	bool bIsNeedSync = false;
	for (WORD i = 0; i < m_pLuxurySignInData->wSignDataNum; ++i)
	{
		LuxurySignData &signInData = m_pLuxurySignInData->arSignDataList[i];
		// 还没完成的
		if (LUXURY_RECHARGE_NOT_YET == signInData.wStatus)
		{
			const LuxurySign_Config *pLuxurySignCfg = g_pCfgMgr->GetLuxurySignInData(signInData.wSignId, m_pPlayer->GetDailyInitLevel());
			if (pLuxurySignCfg && wRechangeId == pLuxurySignCfg->wRechangeId)
			{
				signInData.wStatus = LUXURY_RECHARGE_CAN_TAKE;
				bIsNeedSync = true;
			}
		}
	}

	if (bIsNeedSync)
		SendLuxurySignData();
}

// 领取奖励请求
void CActivityManager::OnNetYCSReward()
{
	// todo::检测玩家所有的服务器是否开放

	const YCS_Config *pYCSCfg = g_pCfgMgr->GetYCS(m_pPlayer->GetLevel());
	if (!pYCSCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_YCS_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到迎财神配置[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	YCSData &tYCSData = m_pActivityInfo->tYCSData;

	DWORD dwValue = 0;

	// 领取累积奖励
	if (tYCSData.byTotalGetTimes >= g_GetConfigValue(62))
	{
		m_pPlayer->ModifyProperty(pYCSCfg->byMoneyType, tYCSData.nTotalRewardValue, TRUE, FROM_YCS);

		dwValue = tYCSData.nTotalRewardValue;

		// 领取后数据清空
		tYCSData.nTotalRewardValue = 0;
		tYCSData.byTotalGetTimes = 0;
	}
	// 领取普通奖励
	else
	{
		// 检测领取的间隔时间
		if (time(NULL) < tYCSData.dwLastGetTime + g_GetConfigValue(63) * SECOND_OF_MIN)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_YCS_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]迎财神领取奖励间隔时间CD中...", __FUNCTION__);
			return;
		}

		// 今日次数是否已达上限
		if (tYCSData.byTodayGetTimes >= pYCSCfg->vMoneyValueList.size())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_YCS_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]今日的奖励次数已领完[times:%d]", __FUNCTION__, tYCSData.byTodayGetTimes);
			return;
		}

		int nAddValue = pYCSCfg->vMoneyValueList[tYCSData.byTodayGetTimes];
		m_pPlayer->ModifyProperty(pYCSCfg->byMoneyType, nAddValue, TRUE, FROM_YCS);

		// 今日领取次数
		tYCSData.byTodayGetTimes++;

		tm tNow;
		GetTMTime(&tNow);
		tm tLast;
		GetTMTime(&tLast, tYCSData.dwLastGetTime);

		if ((tNow.tm_yday == tLast.tm_yday) // 如果是同一天 
			|| (tLast.tm_yday + 1 == tNow.tm_yday && tYCSData.byTotalGetTimes == g_GetConfigValue(64))) // 如果是第二天,且前一天木有中断
		{
			tYCSData.byTotalGetTimes++;
			tYCSData.nTotalRewardValue += nAddValue;
		}
		// 领取次数中断 或 天数中断 则需要重置
		else
		{
			tYCSData.byTotalGetTimes = 1;
			tYCSData.nTotalRewardValue = nAddValue;
		}	
		// 领取次数中断 或 天数中断 则需要重置
// 		else if (m_pActivityInfo->tYCSData.byTotalGetTimes < g_GetConfigValue(64) || tLast.tm_yday + 1 < tNow.tm_yday)
// 		{
// 			m_pActivityInfo->tYCSData.byTotalGetTimes = 1;
// 			m_pActivityInfo->tYCSData.nTotalRewardValue = nAddValue;
// 		}

		// 更新领取奖励时间
		tYCSData.dwLastGetTime = time(NULL);

		dwValue = nAddValue;
	}

	// 主动同步数据
	SendYCSData();

	// 响应
	MSG_S2C_YCS_REWARD_RESP msg;
	msg.m_wType = pYCSCfg->byMoneyType;
	msg.m_dwValue = dwValue;
	m_pPlayer->SendPacket(&msg);
}

// 请求迎财神数据
void CActivityManager::OnNetYCSDataReq()
{
	SendYCSData();
}

// 发送迎财神数据
void CActivityManager::SendYCSData()
{
	MSG_S2C_YCS_DATA_RESP msg;
	msg.m_tData = m_pActivityInfo->tYCSData;
	m_pPlayer->SendPacket(&msg);
}


// 领取奖励请求
void CActivityManager::OnNetTQTReward()
{
	// todo::检测玩家所有的服务器是否开放

	// 取当前时间段奖励
	tm tNow;
	GetTMTime(&tNow);
	const TQT_Config *pTQTCfg = GetTQTCfgByTime(tNow.tm_hour * 100 + tNow.tm_min);
	if (!pTQTCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TQT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]现在不是铜雀台活动时间", __FUNCTION__);
		return;
	}

	TQTData &tTQTData = m_pActivityInfo->tTQTData;

	// 检测是否已领取过
	for (WORD i = 0; i < tTQTData.wTakeRecordNum; ++i)
	{
		if (tTQTData.arTakeRecordList[i] == pTQTCfg->wId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TQT_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]铜雀台奖励已领取过[id:%d]", __FUNCTION__, pTQTCfg->wId);
			return;
		}
	}

	// 检测列表是否已满
	if (tTQTData.wTakeRecordNum >= MAX_TQT_TAKE_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TQT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]铜雀台奖励领取列表已满", __FUNCTION__);
		return;
	}

	// 添加体力
	m_pPlayer->ModifyProperty(ROLE_PRO_STAMINA, g_GetConfigValue(65), TRUE, FROM_TQT);

	tTQTData.dwLastGetTime = time(NULL);
	tTQTData.arTakeRecordList[tTQTData.wTakeRecordNum++] = pTQTCfg->wId;

	// 主动同步数据
	SendTQTData();

	// 响应
	MSG_S2C_TQT_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 请求铜雀台数据
void CActivityManager::OnNetTQTDataReq()
{
	SendTQTData();
}

// 发送铜雀台数据
void CActivityManager::SendTQTData()
{
	MSG_S2C_TQT_DATA_RESP msg;
	msg.m_tData = m_pActivityInfo->tTQTData;
	m_pPlayer->SendPacket(&msg);
}

// 是否铜雀台活动时间
bool CActivityManager::IsTQTActivityTime(int nTime)
{
	const vector<TQT_Config> *pTQTListCfg = g_pCfgMgr->GetTQTList();
	if (!pTQTListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]铜雀台配置列表为空", __FUNCTION__);
		return false;
	}

	for (vector<TQT_Config>::const_iterator iter = pTQTListCfg->begin(); iter != pTQTListCfg->end(); ++iter)
	{
		if (nTime >= iter->nStartTime && nTime <= iter->nOverTime)
			return true;
	}

	return false;
}

// 通过时间取铜雀台活动配置
const TQT_Config * CActivityManager::GetTQTCfgByTime(int nTime)
{
	const vector<TQT_Config> *pTQTListCfg = g_pCfgMgr->GetTQTList();
	if (!pTQTListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]铜雀台配置列表为空", __FUNCTION__);
		return NULL;
	}

	for (vector<TQT_Config>::const_iterator iter = pTQTListCfg->begin(); iter != pTQTListCfg->end(); ++iter)
	{
		if (nTime >= iter->nStartTime && nTime <= iter->nOverTime)
			return &*iter;
	}

	return NULL;
}

// VIP礼包数据请求
void CActivityManager::OnNetVipGiftDataReq()
{
	SendVipGiftData();
}

// 每日礼包领取
void CActivityManager::OnNetVipDailyGiftTake(WORD wVipLevel)
{
	// 检测VIP等级
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < wVipLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_DAILY_GIFT_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Vip等级不足,不能领取[VipLevel:%d]", __FUNCTION__, wVipLevel);
		return;
	}

	// 检测是否已领取过
	if (m_pActivityInfo->tVipGiftData.nVipLevelRewardRecord >= wVipLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_DAILY_GIFT_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]VIP每日礼包已领取过[VipLevel:%d]", __FUNCTION__, wVipLevel);
		return;
	}

	// 取VIP等级礼包配置
	const VipDailyGift_Config *pGiftCfg = g_pCfgMgr->GetVipDailyGift(wVipLevel);
	if (!pGiftCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_DAILY_GIFT_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到VIP每日礼包配置[VipLevel:%d]", __FUNCTION__, wVipLevel);
		return;
	}

	// 发放奖励
	m_pPlayer->AddObjectList(pGiftCfg->vItemList, TRUE, FROM_DAILY_GIFT_TAKE);

	// 记录已领取
	m_pActivityInfo->tVipGiftData.nVipLevelRewardRecord = wVipLevel;

	// 响应
	MSG_S2C_VIP_DAILY_GIFT_GET_RESP msg;
	msg.m_wVipLevel = wVipLevel;
	m_pPlayer->SendPacket(&msg);
}

// 每周礼包购买
void CActivityManager::OnNetVipWeekGiftBuy(WORD wGiftId)
{
	Gift *pGift = NULL;

	VipGiftData &tGiftData = m_pActivityInfo->tVipGiftData;

	// 取礼包数据
	for (WORD i = 0; i < tGiftData.wWeekGiftNum; ++i)
	{
		if (tGiftData.arWeekGiftList[i].wId == wGiftId)
		{
			pGift = &tGiftData.arWeekGiftList[i];
			break;
		}
	}

	// 不存在的礼包
	if (!pGift)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]不存在的VIP周礼包[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// 已经购买过
	if (pGift->wNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]VIP周礼包已经购买过[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// 取礼包配置
	const VipWeekGift_Config *pGiftCfg = g_pCfgMgr->GetVipWeekGift(wGiftId);
	if (!pGiftCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到VIP周礼包配置[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// 检测消耗条件
	if (!m_pPlayer->CheckMoneyCost(pGiftCfg->vCostList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]购买VIP周礼包消耗不足[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// 更新购买次数
	++pGift->wNum;

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGiftCfg->vCostList, 1, FROM_WEEK_GIFT_BUY);

	// 添加物品
	m_pPlayer->AddObjectList(pGiftCfg->vItemList, TRUE, FROM_WEEK_GIFT_BUY);

	// 响应
	MSG_S2C_VIP_WEEK_GIFT_BUY_RESP msg;
	msg.m_wGiftId = wGiftId;
	m_pPlayer->SendPacket(&msg);
}


// 发送VIP礼包数据
void CActivityManager::SendVipGiftData()
{
	MSG_S2C_VIP_GIFT_DATA_RESP msg;
	msg.m_wDailyInitVipLevel = m_pActivityInfo->tVipGiftData.wInitVipLevel;
	msg.m_nVipLevelRewardRecord = m_pActivityInfo->tVipGiftData.nVipLevelRewardRecord;
	msg.m_wWeekGiftNum = m_pActivityInfo->tVipGiftData.wWeekGiftNum;
	msg.m_wTodayRecommendRechargeRewardId = m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardId;
	msg.m_wTodayRecommendRechargeRewardStatus = m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardStatus;
	for (WORD i = 0; i < m_pActivityInfo->tVipGiftData.wWeekGiftNum; ++i)
	{
		msg.m_WeekGiftList[i] = m_pActivityInfo->tVipGiftData.arWeekGiftList[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// 刷新周礼包
void CActivityManager::RefreshWeekGift()
{
	VipGiftData &tVipGiftData = m_pActivityInfo->tVipGiftData;
	tVipGiftData.wWeekGiftNum = 0;

	const VipWeekGiftRandList_Config *pGiftListCfg = g_pCfgMgr->GetVipWeekGiftRandList(m_pPlayer->GetLevel());
	if (!pGiftListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色VIP周礼包配置列表[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	if (pGiftListCfg->wGiftRandNum > MAX_VIP_WEEK_GIFT_NUM || 
		pGiftListCfg->wGiftRandNum > pGiftListCfg->vGiftRandList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色VIP周礼包随机数量配置出错[RandNum:%d]", 
			__FUNCTION__, pGiftListCfg->wGiftRandNum);
		return;
	}

	vector<RateDataEx> vGiftRandList = pGiftListCfg->vGiftRandList;
	for (WORD i = 0; i < pGiftListCfg->wGiftRandNum; ++i)
	{
		// 计算总概率
		int nTotalRate = 0;
		for (size_t k = 0; k < vGiftRandList.size(); ++k)
			nTotalRate += vGiftRandList[k].nRate;

		// 随机礼包
		int nRandValue = g_Rand(1, nTotalRate);
		
		// 取命中的礼包,并从随机列表里删除
		int nCurRate = 0;
		for (vector<RateDataEx>::iterator iter = vGiftRandList.begin(); iter != vGiftRandList.end(); ++iter)
		{
			nCurRate += iter->nRate;
			if (nRandValue <= nCurRate)
			{
				tVipGiftData.arWeekGiftList[tVipGiftData.wWeekGiftNum].wId = iter->nValue;
				tVipGiftData.arWeekGiftList[tVipGiftData.wWeekGiftNum].wNum = 0;
				tVipGiftData.wWeekGiftNum++;

				vGiftRandList.erase(iter);
				break;
			}
		}
	}
}

// 领取今日推荐充值的奖励
void CActivityManager::OnNetTakeTodayRecommendReward()
{
	VipGiftData &tGiftData = m_pActivityInfo->tVipGiftData;

	// 检测是否可以领取
	if (tGiftData.wTodayRecommendRechargeRewardStatus != REWARD_STATUS_DONE)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]今日推荐充值礼包还不能领取[status:%d]",
			__FUNCTION__, tGiftData.wTodayRecommendRechargeRewardStatus);
		return;
	}

	// 取奖励配置
	const TodayRecommendRecharge_Config *pRechargeCfg = g_pCfgMgr->GetTodayRecommendRecharge(tGiftData.wTodayRecommendRechargeRewardId);
	if (!pRechargeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到今日推荐充值配置[giftid:%d]",
			__FUNCTION__, tGiftData.wTodayRecommendRechargeRewardId);
		return;
	}

	m_pPlayer->AddObjectList(pRechargeCfg->vItemList, TRUE, FROM_TODAY_RECOMMEND_REWARD);

	// 标记已领取过
	tGiftData.wTodayRecommendRechargeRewardStatus = REWARD_STATUS_ALREADY_TAKE;

	MSG_S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 重置今日推荐充值数据
void CActivityManager::ResetTodayRecommendRechargeData()
{
	// 随机充值档位
	WORD wInitVipLevel = m_pActivityInfo->tVipGiftData.wInitVipLevel;
	const vector<RateDataEx> *pRandDataList = g_pCfgMgr->GetTodayRecommendRechargeRandList(wInitVipLevel);
	if (!pRandDataList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到今日推荐充值随机数据[VipLevel:%d]",__FUNCTION__, wInitVipLevel);
		return;
	}

	// 默认是1
	m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardId = 1;

	// 计算总概率
	int nTotalRate = 0;
	vector<RateDataEx>::const_iterator iter = pRandDataList->begin();
	for (; iter != pRandDataList->end(); ++iter)
	{
		nTotalRate += iter->nRate;
	}

	// 随机一个值
	int nRandValue = g_Rand(0, nTotalRate);

	// 计算当前命中的
	int nCurRate = 0;
	iter = pRandDataList->begin();
	for (; iter != pRandDataList->end(); ++iter)
	{
		nCurRate += iter->nRate;
		if (nCurRate >= nRandValue)
		{
			m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardId = iter->nValue;
			break;
		}
	}

	m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardStatus = REWARD_STATUS_UNFINISHED;
}

// 等级礼包数据请求
void CActivityManager::OnNetLevelShopActivityDataReq()
{
	MSG_S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP msg;
	msg.m_wActivityDataNum = m_pActivityInfo->wLevelShopActivityDataNum;
	int nCopyDataSize = sizeof(LevelShopActivityData)* m_pActivityInfo->wLevelShopActivityDataNum;
	memcpy_s(msg.m_ActivityDataList, nCopyDataSize, m_pActivityInfo->arLevelShopActivityDataList, nCopyDataSize);
	m_pPlayer->SendPacket(&msg);
}

// 角色等级提升
void CActivityManager::RoleLevelUp(WORD wFrom, WORD wTo)
{
	for (WORD i = wFrom; i <= wTo; ++i)
	{
		// 检测此等级是否有礼包
		const LevelShopData_Config *pGiftCfg = g_pCfgMgr->GetLevelShopData(i);
		if (!pGiftCfg)
			continue;

		// 检测列表是否已满
		if (m_pActivityInfo->wLevelShopActivityDataNum >= MAX_LEVEL_SHOP_ACTIVITY_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]等级礼包列表已满", __FUNCTION__);
			continue;
		}

		// 检测是否已经存在了
		if (GetLevelShopActivityData(i))
			continue;

		// 激活
		m_pActivityInfo->arLevelShopActivityDataList[m_pActivityInfo->wLevelShopActivityDataNum].dwActivateTime = time(NULL);
		m_pActivityInfo->arLevelShopActivityDataList[m_pActivityInfo->wLevelShopActivityDataNum].wLevel = i;
		++m_pActivityInfo->wLevelShopActivityDataNum;
	}
}

// 取等级礼包数据
const LevelShopActivityData* CActivityManager::GetLevelShopActivityData(WORD wRoleLevel)
{
	PlayerActivityData &tActivityData = m_pPlayer->GetRoleFullInfo().tActivityInfo;
	for (WORD i = 0; i < tActivityData.wLevelShopActivityDataNum; ++i)
	{
		if (tActivityData.arLevelShopActivityDataList[i].wLevel == wRoleLevel)
			return &tActivityData.arLevelShopActivityDataList[i];
	}
	return NULL;
}

// 等级礼包数据请求
void CActivityManager::OnNetLevelGiftDataReq()
{
	MSG_S2C_LEVEL_GIFT_DATA_RESP msg;
	msg.m_wLevelGiftTakeRecordNum = m_pActivityInfo->wLevelGiftTakeRecordNum;
	int nCopyDataSize = sizeof(WORD)*m_pActivityInfo->wLevelGiftTakeRecordNum;
	memcpy_s(msg.m_arLevelGiftTakeRecord, nCopyDataSize, m_pActivityInfo->arLevelGiftTakeRecord, nCopyDataSize);
	m_pPlayer->SendPacket(&msg);
}

// 等级礼包领取
void CActivityManager::OnNetTakeLevelGift(WORD wLevel)
{
	// 检测是否已经领取过
	for (WORD i = 0; i < m_pActivityInfo->wLevelGiftTakeRecordNum; ++i)
	{
		if (m_pActivityInfo->arLevelGiftTakeRecord[i] == wLevel)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LEVEL_GIFT_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]等级礼包奖励已领取过[Level:%d]", __FUNCTION__, wLevel);
			return;
		}
	}

	// 领取配置
	const LevelGift_Config *pLevelGiftCfg = g_pCfgMgr->GetLevelGift(wLevel);
	if (!pLevelGiftCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LEVEL_GIFT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到等级礼包配置[Level:%d]", __FUNCTION__, wLevel);
		return;
	}

	// 检测奖励记录列表是否已满
	if (m_pActivityInfo->wLevelGiftTakeRecordNum >= MAX_LEVEL_GIFT_TAKE_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LEVEL_GIFT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]等级礼包领取记录列表已满[Level:%d]", __FUNCTION__, wLevel);
		return;
	}

	// 添加奖励
	m_pPlayer->AddObjectList(pLevelGiftCfg->vItemList, TRUE, FROM_LEVEL_GIFT);

	// 记录领取奖励
	m_pActivityInfo->arLevelGiftTakeRecord[m_pActivityInfo->wLevelGiftTakeRecordNum++] = wLevel;

	// 响应
	MSG_S2C_TAKE_LEVEL_GIFT_REWARD_RESP msg;
	msg.m_wLevel = wLevel;
	m_pPlayer->SendPacket(&msg);
}

// 是否已领取等级礼包
bool CActivityManager::IsTakeLevelGift(WORD wLevel)
{
	for (WORD i = 0; i < m_pActivityInfo->wLevelGiftTakeRecordNum; ++i)
	{
		if (m_pActivityInfo->arLevelGiftTakeRecord[i] == wLevel)
			return true;
	}
	return false;
}

// 是否活动开放
// bool CActivityManager::IsActivityOpen(int nSystemId, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime)
// {
// 	// 取活动配置
// 	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
// 	if (!pActivityListCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, nSystemId);
// 		return false;
// 	}
// 
// 	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
// 	for (; iter != pActivityListCfg->end(); ++iter)
// 	{
// 		if (IsActivityOpen(&*iter, dwServerId, wRoleLevel, dwCreateRoleTime))
// 			return true;
// 	}
// 
// 	return false;
// }

// 是否活动开放
bool CActivityManager::IsActivityOpen(int nSystemId, Player *pPlayer)
{
	// 取活动配置
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, nSystemId);
		return false;
	}

	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		if (IsActivityOpen(&*iter, pPlayer->GetServerId(), pPlayer->GetLevel(), pPlayer->GetRoleCreateTime()))
			return true;
	}

	return false;
}

// 取活动数据ID
WORD CActivityManager::GetActivityDataId(int nSystemId)
{
	// 取活动配置
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(GUI_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, nSystemId);
		return 0;
	}

	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		if (IsActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetLevel(), m_pPlayer->GetRoleCreateTime()))
			return iter->wDataId;
	}

	return 0;
}

// 取活动天数
WORD CActivityManager::GetActivityDay(int nSystemId)
{
	// 取活动配置
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, nSystemId);
		return 0;
	}

	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(m_pPlayer->GetServerId());
	if (!pServerInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到服务器信息[server_id:%d]", __FUNCTION__, m_pPlayer->GetServerId());
		return 0;
	}

	DWORD dwCurTime = time(NULL);
	tm tNow;
	GetTMTime(&tNow, dwCurTime);
	int nPassDays = 0;
	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		if (IsActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetLevel(), m_pPlayer->GetRoleCreateTime()))
		{
			// 检测活动时间的限制
			// 指定时间 开始日期[年:月 : 日] - 结束日期[年:月 : 日]
			if (ACTIVITY_TIME_APPOINT == iter->nTimeType)
			{
				int nYear = SplitInteger(iter->nStartDate,8, 0, 4);
				int nMonth = SplitInteger(iter->nStartDate,8, 4, 2);
				int nDay = SplitInteger(iter->nStartDate, 8, 6, 2);
				int nHour = SplitInteger(iter->nStartTime,4, 0, 2);
				int nMin = SplitInteger(iter->nStartTime, 4, 2, 2);
				// todo::可能有BUG
				DWORD dwLastTime = DATETIME(nYear, nMonth, nDay, nHour, nMin, 0);
				nPassDays = GetPassDays(dwLastTime, dwCurTime);
			}
			// 每日 开始日期[0] - 结束日期[0]
			else if (ACTIVITY_TIME_DAILY == iter->nTimeType)
			{
				nPassDays = GetPassDays(pServerInfo->dwOpenTime, dwCurTime);
			}
			// 星期 开始日期[星期几] 结束日期[星期几]
			else if (ACTIVITY_TIME_WEEK == iter->nTimeType)
			{
				int nCurWeekDay = g_CorrectWeekDay(tNow.tm_wday);
				int nStartWeekDay = g_CorrectWeekDay(iter->nStartDate);
				int nOverWeekDay = g_CorrectWeekDay(iter->nOverDate);

				// 在活动期间内
				if (nCurWeekDay >= nStartWeekDay && nCurWeekDay <= nOverWeekDay)
				{
					nPassDays = nCurWeekDay - nStartWeekDay + 1;
				}
			}
// 			// 月份 开始日期[几号] - 结束日期[几号]
// 			else if (ACTIVITY_TIME_MONTH == iter->nTimeType)
// 			{
// 
// 			}
			// 开服 开始日期[开服第几天] - 结束日期[开服第几天](todo::此逻辑应该有问题,但是防止之前的功能有问题，先保留观察)
			else if (ACTIVITY_TIME_OPEN_SERVER == iter->nTimeType)
			{
				nPassDays = GetPassDays(pServerInfo->dwOpenTime, dwCurTime);
			}
			// 开服 开始日期[开服第几天] - 结束日期[开服第几天](todo::此逻辑才是对的)
			else if (ACTIVITY_TIME_OPEN_SERVER_EX == iter->nTimeType)
			{
				// 开服当天加入指定的天数才是活动的开始时间
				time_t tLastTime = pServerInfo->dwOpenTime + g_Day2Second(iter->nStartDate-1);
				tm tmDetail;
				localtime_s(&tmDetail, &tLastTime);

				// 清除小时,分钟,秒，从当天的0点开始算
				tmDetail.tm_hour = 0;
				tmDetail.tm_min = 0;
				tmDetail.tm_sec = 0;
				tLastTime = mktime(&tmDetail);

				// 活动过去了多少天
				nPassDays = GetPassDays(tLastTime, dwCurTime);
			}
			//  创建角色 开始日期[创角第几天] - 结束日期[创角第几天]
			else if (ACTIVITY_TIME_CREATE_ROLE == iter->nTimeType)
			{
				// 创建角色当天加入指定的天数才是活动的开始时间
				time_t tLastTime = m_pPlayer->GetRoleCreateTime() + g_Day2Second(iter->nStartDate - 1);
				tm tmDetail;
				localtime_s(&tmDetail, &tLastTime);

				// 清除小时,分钟,秒，从当天的0点开始算
				tmDetail.tm_hour = 0;
				tmDetail.tm_min = 0;
				tmDetail.tm_sec = 0;
				tLastTime = mktime(&tmDetail);

				// 活动过去了多少天
				nPassDays = GetPassDays(tLastTime, dwCurTime);
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "[%s]木有处理的活动时间类型[time_type:%d]", __FUNCTION__, iter->nTimeType);
				return nPassDays;
			}

			return nPassDays;
		}
	}

	return 0;
}

// 是否活动开放
bool CActivityManager::IsActivityOpen(const Activity_Config *pActivityCfg, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime)
{
 	// 检测等级限制
	if (wRoleLevel && wRoleLevel < pActivityCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]等级限制不能参与活动[SytemId:%d]", __FUNCTION__, pActivityCfg->nSystemId);
		return false;
	}

	return CheckActivityTime(pActivityCfg, dwServerId, dwCreateRoleTime);
}

bool CheckDateTime(int nCurData, int nStartDate, int nEndDate, int nCurTime, int nStartTime, int nEndTime)
{
	//  检测首日时间
	if (nCurData == nStartDate && nCurTime >= nStartTime)
		return true;

	//  检测最后一日(0=表示24点,即一整天)
	if (nCurData == nEndDate && (0 == nEndTime || nCurTime <= nEndTime))
		return true;

	// 检测日期(如果是有日内,则所有的时间点都生效)
	if (nCurData > nStartDate && nCurData < nEndDate)
		return true;

	return false;
};

// 检测活动时间
bool CActivityManager::CheckActivityTime(const ActivityBase_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime)
 {
 	// 检测玩家所在服务器是否开放此活动
 	if (!pActivityCfg->IsOpenServer(dwServerId))
 		return false;
 
 	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(dwServerId);
 	if (!pServerInfo)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到服务器信息[ServerId:%d]", __FUNCTION__, dwServerId);
 		return false;
 	}
 
 	// 检测开服时间的限制
 	int nAlreadyOpenDays = GetPassDays(pServerInfo->dwOpenTime, time(NULL));
 	if (nAlreadyOpenDays < pActivityCfg->nDays)
 		return false;
 
 	tm tNow;
 	GetTMTime(&tNow, time(NULL));
 	tm tServerOpenTime;
 	GetTMTime(&tServerOpenTime, pServerInfo->dwOpenTime);
 
 	int nCurDate = (tNow.tm_year + 1900) * 10000 + (tNow.tm_mon + 1) * 100 + tNow.tm_mday;
 	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
 
 	switch (pActivityCfg->nTimeType)
 	{
 		// 检测活动时间的限制
 		// 指定时间 开始日期[年:月 : 日] - 结束日期[年:月 : 日]
 	case ACTIVITY_TIME_APPOINT:
 		return CheckDateTime(nCurDate, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;
 
 		// 每日 开始日期[0] - 结束日期[0]
 	case ACTIVITY_TIME_DAILY:
		return CheckDateTime(tNow.tm_yday, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;
 
 		// 星期 开始日期[星期几] 结束日期[星期几]
 	case ACTIVITY_TIME_WEEK:
 		{
 			int nCurWeekDay = g_CorrectWeekDay(tNow.tm_wday);
 			int nOverWeekDay = g_CorrectWeekDay(pActivityCfg->nOverDate);
 			int nStartWeekDay = g_CorrectWeekDay(pActivityCfg->nStartDate);
 			return CheckDateTime(nCurWeekDay, nStartWeekDay, nOverWeekDay, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		}
 		break;
 
 		// 月份 开始日期[几号] 结束日期[几号]
 	case ACTIVITY_TIME_MONTH:
		return CheckDateTime(tNow.tm_mday, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;
 
 		// 开服 开始日期[开服第几天] - 结束日期[开服第几天]
 	case ACTIVITY_TIME_OPEN_SERVER:
 	case ACTIVITY_TIME_OPEN_SERVER_EX:
 		return CheckDateTime(nAlreadyOpenDays, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;

	case ACTIVITY_TIME_CREATE_ROLE:
		{
			// 检测开服时间的限制
			int nAlreadyCreateDays = GetPassDays(dwCreateRoleTime, time(NULL));
			return CheckDateTime(nAlreadyCreateDays, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
		}
 		break;
 
 	default:
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的活动时间类型[time_type:%d]", __FUNCTION__, pActivityCfg->nTimeType);
		break;
 	}
 
 	return false;
 }


 // 检测活动时间(临时功能使用)
//  bool CActivityManager::CheckActivityTimeEx(const ActivityBase_Config *pActivityCfg, DWORD dwServerId)
//  {
// 	 // 检测玩家所在服务器是否开放此活动
// 	 if (!pActivityCfg->IsOpenServer(dwServerId))
// 		 return false;
// 
// 	 const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(dwServerId);
// 	 if (!pServerInfo)
// 	 {
// 		 MAPLOG(ERROR_LEVEL, "[%s]找不到服务器信息[ServerId:%d]", __FUNCTION__, dwServerId);
// 		 return false;
// 	 }
// 
// 	// 检测开服时间的限制
// 	int nAlreadyOpenDays = GetPassDays(pServerInfo->dwOpenTime, time(NULL));
// 	if (nAlreadyOpenDays < pActivityCfg->nDays)
// 	{
// 	// 		MAPLOG(ERROR_LEVEL, "[%s]服务器开服的天数不足[ServerId:%d, AlreayOpenDays:%d]",
// 	// 			__FUNCTION__, m_pPlayer->GetServerId(), nAlreadyOpenDays);
// 	  	return false;
// 	}
// 	  
// 	tm tNow;
// 	GetTMTime(&tNow, time(NULL));
// 	tm tServerOpenTime;
// 	GetTMTime(&tServerOpenTime, pServerInfo->dwOpenTime);
// 	  
// 	int nCurDate = (tNow.tm_year + 1900) * 10000 + (tNow.tm_mon + 1) * 100 + tNow.tm_mday;
// 	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
// 	  
// 	// 检测开始时间
// 	if (pActivityCfg->nStartTime && nCurTime < pActivityCfg->nStartTime)
// 	  	return false;
// 	  
// 	// 检测结束时间
// 	if (pActivityCfg->nOverTime && nCurTime > pActivityCfg->nOverTime)
// 	  	return false;
// 	  
// 	// 检测活动时间的限制
// 	// 指定时间 开始日期[年:月 : 日] - 结束日期[年:月 : 日]
// 	if (ACTIVITY_TIME_APPOINT == pActivityCfg->nTimeType)
// 	{	 
// 	  	// 检测日期
// 	  	if (nCurDate >= pActivityCfg->nStartDate && nCurDate <= pActivityCfg->nOverDate)
// 	  		return true;
// 	}
// 	// 每日 开始日期[0] - 结束日期[0]
// 	else if (ACTIVITY_TIME_DAILY == pActivityCfg->nTimeType)
// 	{
// 	  	return true;
// 	}
// 	// 星期 开始日期[星期几] 结束日期[星期几]
// 	else if (ACTIVITY_TIME_WEEK == pActivityCfg->nTimeType)
// 	{
// 	  	int nCurWeekDay = g_CorrectWeekDay(tNow.tm_wday);
// 	  	int nStartWeekDay = g_CorrectWeekDay(pActivityCfg->nStartDate);
// 	  	int nOverWeekDay = g_CorrectWeekDay(pActivityCfg->nOverDate);
// 	  	if (nCurWeekDay >= nStartWeekDay && nCurWeekDay <= nOverWeekDay)
// 	  		return true;
// 	}
// 	// 月份 开始日期[几号] - 结束日期[几号]
// 	else if (ACTIVITY_TIME_MONTH == pActivityCfg->nTimeType)
// 	{
// 	  	int nMonthDays = tNow.tm_mday;
// 	  	if (nMonthDays >= pActivityCfg->nStartDate && nMonthDays <= pActivityCfg->nOverDate)
// 	  		return true;
// 	}
// 	// 开服 开始日期[开服第几天] - 结束日期[开服第几天]
// 	else if (ACTIVITY_TIME_OPEN_SERVER == pActivityCfg->nTimeType || ACTIVITY_TIME_OPEN_SERVER_EX == pActivityCfg->nTimeType)
// 	{
// 	  	if (nAlreadyOpenDays >= pActivityCfg->nStartDate && nAlreadyOpenDays <= pActivityCfg->nOverDate)
// 	  		return true;
// 	}
// 	else
// 	{
// 	  	MAPLOG(ERROR_LEVEL, "[%s]木有处理的活动时间类型[time_type:%d]", __FUNCTION__, pActivityCfg->nTimeType);
// 	  	return false;
// 	}
// 	  
// 	return false;
//  }

// 是否有红点
bool CActivityManager::IsHaveRedPoint(WORD wSystemId)
{
	// 签到
	if (SYSTEM_SIGN == wSystemId)
	{
		DATETIME lastSignTime(m_pSignInData->dwLastSignInTime);
		DATETIME curTime(time(NULL));
		// 不同的天,或者不同的月份
		if (curTime.day != lastSignTime.day || curTime.month != lastSignTime.month)
			return true;
	}
	// 铜雀台(体力领取)
	else if (SYSTEM_TQT == wSystemId)
	{	
		tm tNow;
		GetTMTime(&tNow);
		// 检测是否活动时间
		if (!IsTQTActivityTime(tNow.tm_hour * 100 + tNow.tm_min))
			return false;

		tm tLast;
		GetTMTime(&tLast, m_pActivityInfo->tTQTData.dwLastGetTime);
		// 检测今日是否已领取
		if (tLast.tm_yday == tNow.tm_yday)
		{
			const TQT_Config *pLastCfg = GetTQTCfgByTime(tLast.tm_hour * 100 + tLast.tm_min);
			const TQT_Config *pNowCfg = GetTQTCfgByTime(tNow.tm_hour * 100 + tNow.tm_min);

			// 检测此时间段活动是否已领取
			if (pLastCfg && pNowCfg && pLastCfg->nStartTime == pNowCfg->nStartTime)
				return false;
		}

		return true;
	}
	// 迎财神(金币领取)
	else if (SYSTEM_YCS == wSystemId)
	{
		YCSData &tYCSData = m_pActivityInfo->tYCSData;

		// 检测累积奖励
		if (tYCSData.byTotalGetTimes >= g_GetConfigValue(62))
			return true;

		// 检测领取的间隔时间
		DWORD dwCurTime = time(NULL);
		if (dwCurTime >= tYCSData.dwLastGetTime + g_GetConfigValue(63) * SECOND_OF_MIN)
		{
			const YCS_Config *pYCSCfg = g_pCfgMgr->GetYCS(m_pPlayer->GetLevel());
			if (!pYCSCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到迎财神配置[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
				return false;
			}
			
			// 今日领取次数还没满
			if (tYCSData.byTodayGetTimes < pYCSCfg->vMoneyValueList.size())
				return true;
		}
	}
	// 等级商店
	else if (SYSTEM_LEVEL_SHOP == wSystemId)
	{
		for (WORD i = 0; i < m_pActivityInfo->wLevelShopActivityDataNum; ++i)
		{
			LevelShopActivityData &tGiftData = m_pActivityInfo->arLevelShopActivityDataList[i];
			const LevelShopData_Config *pLevelGiftCfg = g_pCfgMgr->GetLevelShopData(tGiftData.wLevel);
			// 商店还没过期
			if (pLevelGiftCfg && time(NULL) < tGiftData.dwActivateTime + pLevelGiftCfg->nValidTime)
			{
				for (size_t k = 0; k < pLevelGiftCfg->vGoodsList.size(); ++k)
				{
					// 如果有还没有购买的商品
					if (!m_pPlayer->GetShopManager()->GetGoodsBuyRecordNum(SHOP_LEVEL_GIFT, pLevelGiftCfg->vGoodsList[k]))
						return true;
				}
			}
		}
	}
	// 等级礼包
	else if (SYSTEM_LEVEL_GIFT == wSystemId)
	{
		map<DWORD, LevelGift_Config>::iterator iter = g_pCfgMgr->m_LevelGiftList.begin();
		for (; iter != g_pCfgMgr->m_LevelGiftList.end(); ++iter)
		{
			if (m_pPlayer->GetLevel() >= iter->second.wLevel && !IsTakeLevelGift(iter->second.wLevel))
				return true;
		}
	}
	// VIP礼包(VIP礼包有可领取奖励时)
	else if (SYSTEM_VIP_GIFT == wSystemId)
	{
		VipGiftData &tGiftData = m_pActivityInfo->tVipGiftData;

		// VIP等级礼包可以领取
		int nVipLevel = m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);
		if (tGiftData.nVipLevelRewardRecord < nVipLevel)
			return true;

		// 周礼包可购买
		for (WORD i = 0; i < tGiftData.wWeekGiftNum; ++i)
		{
			// 当vip等级满足且该礼包还没被购买就显示红点
			Gift &tGift = tGiftData.arWeekGiftList[i];
			// 还没购买
			if (!tGift.wNum)
			{
				// VIP等级条件满足
				const VipWeekGift_Config *pGiftCfg = g_pCfgMgr->GetVipWeekGift(tGift.wId);
				if (pGiftCfg && m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= pGiftCfg->wNeedVipLevel)
					return true;	
			}
		}
	}
	// 当基金下有可领取奖励时
	else if (SYSTEM_FUND == wSystemId)
	{
		OpenServerFundData &tFundData = m_pActivityInfo->tOpenServerFundData;

		// 检测是否已购买了基金
		if (!tFundData.wIsAlreadyBuyFund)
			return false;

		map<DWORD, OpenServerFundReward_Config>::iterator iter = g_pCfgMgr->m_OpenServerFundRewardList.begin();
		for (; iter != g_pCfgMgr->m_OpenServerFundRewardList.end(); ++iter)
		{
			// 取奖励配置
			const OpenServerFundReward_Config *pRewardCfg = g_pCfgMgr->GetOpenServerFundReward(iter->second.wRewardId);
			if (pRewardCfg && m_pPlayer->GetLevel() >= pRewardCfg->wNeedRoleLevel && !IsTakeFundReward(pRewardCfg->wRewardId))
				return true;
		}
	}
	// 当全民福利界面中有可领取奖励时 
	else if (SYSTEM_ALL_PEOPLE_WELFARE == wSystemId)
	{
		OpenServerFundData &tFundData = m_pActivityInfo->tOpenServerFundData;

		DWORD dwBuyTimes = g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).dwOpenServerFundBuyTimes;
		map<DWORD, AllPeopleWelfare_Config>::iterator iter = g_pCfgMgr->m_AllPeopleWelfareList.begin();
		for (; iter != g_pCfgMgr->m_AllPeopleWelfareList.end(); ++iter)
		{
			// 取奖励配置
			const AllPeopleWelfare_Config *pWelfareCfg = g_pCfgMgr->GetAllPeopleWelfare(iter->second.wWelfareId);
			if (pWelfareCfg && dwBuyTimes >= pWelfareCfg->wNeedBuyNum && !IsTakeAllPeopleWelfare(pWelfareCfg->wWelfareId))
				return true;
		}
	}
	// 豪华签到
	else if (SYSTEM_LUXURY_SIGN_IN == wSystemId)
	{
		for (WORD i = 0; i < m_pLuxurySignInData->wSignDataNum; ++i)
		{
			if (m_pLuxurySignInData->arSignDataList[i].wStatus == LUXURY_RECHARGE_CAN_TAKE)
				return true;
		}
	}
	// 登录奖励
	else if (SYSTEM_LOGIN_REWARD == wSystemId)
	{
		WORD wLoginDays = m_pLoginRewardData->wLoginDays; //m_pPlayer->GetRoleCreateDays();
		const map<WORD, LoginReward_Config> &loginRewardList = g_pCfgMgr->GetLoginRewardList();
		map<WORD, LoginReward_Config>::const_iterator iter =  loginRewardList.begin();
		for (; iter != loginRewardList.end(); ++iter)
		{
			// 已到达登录天数,且还没领取过
			if (wLoginDays >= iter->second.wDays && !IsTakeLoginReward(iter->second.wDays))
				return true;
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的红点提示类型[SystemId:%d]", __FUNCTION__, wSystemId);
		return false;
	}

	return false;
}

// 活动开始时间
// DWORD CActivityManager::GetActivityStartTime(int nSystemId)
// {
// 	// 取活动配置
// 	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
// 	if (!pActivityListCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, nSystemId);
// 		return 0;
// 	}
// 
// 	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(m_pPlayer->GetServerId());
// 	if (!pServerInfo)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到服务器信息[server_id:%d]", __FUNCTION__, m_pPlayer->GetServerId());
// 		return 0;
// 	}
// 
// 	DWORD dwStartTime = 0;
// 	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
// 	for (; iter != pActivityListCfg->end(); ++iter)
// 	{
// 		if (IsActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetLevel()))
// 		{
// 			// 检测活动时间的限制
// 			// 指定时间 开始日期[年:月 : 日] - 结束日期[年:月 : 日]
// 			if (ACTIVITY_TIME_APPOINT == iter->nTimeType)
// 			{
// 				int nYear = SplitInteger(iter->nStartDate, 8, 0, 4);
// 				int nMonth = SplitInteger(iter->nStartDate, 8, 4, 2);
// 				int nDay = SplitInteger(iter->nStartDate, 8, 6, 2);
// 				int nHour = SplitInteger(iter->nStartTime, 4, 0, 2);
// 				int nMin = SplitInteger(iter->nStartTime, 4, 2, 2);
// 				// 可能有BUG
// 				dwStartTime = DATETIME(nYear, nMonth, nDay, nHour, nMin, 0);
// 			}
// 			// 每日 开始日期[0] - 结束日期[0]
// 			else if (ACTIVITY_TIME_DAILY == iter->nTimeType)
// 			{
// 				dwStartTime = pServerInfo->dwOpenTime;
// 			}
// 			// 星期 开始日期[星期几] 结束日期[星期几]
// 			// 			else if (3 == iter->nTimeType)
// 			// 			{
// 			// 
// 			// 			}
// 			// 			// 月份 开始日期[几号] - 结束日期[几号]
// 			// 			else if (4 == iter->nTimeType)
// 			// 			{
// 			// 
// 			// 			}
// 			// 开服 开始日期[开服第几天] - 结束日期[开服第几天]
// 			else if (ACTIVITY_TIME_OPEN_SERVER == iter->nTimeType || ACTIVITY_TIME_OPEN_SERVER_EX == iter->nTimeType)
// 			{
// 				dwStartTime = pServerInfo->dwOpenTime;
// 
// 				//nPassDays = GetPassDays(pServerInfo->dwOpenTime, dwCurTime);
// 			}
// 			else
// 			{
// 				MAPLOG(ERROR_LEVEL, "[%s]木有处理的活动时间类型[time_type:%d]", __FUNCTION__, iter->nTimeType);
// 			}
// 			break;
// 		}
// 	}
// 
// 	return dwStartTime;
// }

// 是否活动结束
bool CActivityManager::IsActivityOver(DWORD dwServerId, int nSystemId, WORD wDataId, DWORD dwCreateRoleTime)
{
	const Activity_Config *pActivityCfg = g_pCfgMgr->GetActivityTime(nSystemId, wDataId);
	if (!pActivityCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d,DataId:%d]", __FUNCTION__, nSystemId, wDataId);
		return false;
	}

	return IsActivityOpen(&*pActivityCfg, dwServerId, 0, dwCreateRoleTime) ? false : true;
}

// 是否限时产出活动开启
int CActivityManager::GetLimitProductMultiple(WORD wMapType)
{
	const LimitProduct_Config *pProductCfg = g_pCfgMgr->GetLimitProduct(wMapType);
	if (!pProductCfg || pProductCfg->vStartTimeList.size() != pProductCfg->vOverTimeList.size())
	{
		//MAPLOG(GUI_LEVEL, "[%s]找不到限时产出活动配置[SystemId:%d%d]", __FUNCTION__, wSystemId);
		return 1;
	}

	tm tCurTime;
	GetTMTime(&tCurTime, time(NULL));

	int nCurDate = (tCurTime.tm_year + 1900) * 10000 + (tCurTime.tm_mon + 1) * 100 + tCurTime.tm_mday;
	int nCurTime = tCurTime.tm_hour * 100 + tCurTime.tm_min;

	// 检测日期
	if (nCurDate < pProductCfg->nStartDate || nCurDate > pProductCfg->nOverDate)
		return 1;

	for (int i = 0; i < pProductCfg->vStartTimeList.size(); ++i)
	{
		// 检测时间
		if (nCurTime >= pProductCfg->vStartTimeList[i] && nCurTime <= pProductCfg->vOverTimeList[i])
			return pProductCfg->nMultiple;
	}

	return 1;
}

// 取活动奖励时间
int CActivityManager::GetActivityRewardDate(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(wSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, wSystemId);
		return 0;
	}

	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		if (IsActivityOpen(&*iter, dwServerId, 0, dwCreateRoleTime))
 			return iter->nRewardOverDate;
	}

	return 0;
}

// 取活动奖励时间
int CActivityManager::GetActivityRewardTime(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(wSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到活动配置[SystemId:%d]", __FUNCTION__, wSystemId);
		return 0;
	}

	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
	for (; iter != pActivityListCfg->end(); ++iter)
	{
		if (IsActivityOpen(&*iter, dwServerId, 0, dwCreateRoleTime))
 			return iter->nRewardOverTime;
	}

	return 0;
}

// 今日推荐充值回调
void CActivityManager::OnTodayRecommendRechargeCallback()
{
	m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardStatus = REWARD_STATUS_DONE;
}

// 开服基金数据请求
void CActivityManager::OnNetOpenServerFundDataReq()
{
	SendOpenServerFundData();
}

// 开服基金购买
void CActivityManager::OnNetOpenServerFundBuy()
{
	// 达到指定VIP等级后可购买开服基金
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < g_GetConfigValue(156))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]VIP等级限制", __FUNCTION__);
		return;
	}

	// todo::检测购买消耗的钻石
	int nCostDiamond = g_GetConfigValue(157);
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]购买基金消耗的钻石不足", __FUNCTION__);
		return;
	}

	// 检测是否已经购买过
	if (m_pActivityInfo->tOpenServerFundData.wIsAlreadyBuyFund)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]开服基金已购买过", __FUNCTION__);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_BUY_FUND);

	m_pActivityInfo->tOpenServerFundData.wIsAlreadyBuyFund = true;

	// 更新购买人数
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId());
	tGlobalData.dwOpenServerFundBuyTimes++;

	// 响应
	MSG_S2C_OPEN_SERVER_FUND_BUY_RESP msg;
	m_pPlayer->SendPacket(&msg);

	// 存储全局数据
	g_MapServer.SaveGolbalData(m_pPlayer->GetServerId(), GOLBAL_DATA_FUND_BUY_TIMES, tGlobalData.dwOpenServerFundBuyTimes);

	// 处理广播
	CSystemNotice sys_notice(13, m_pPlayer->GetServerId());
	sys_notice.AddParam(m_pPlayer->GetRoleName());
	sys_notice.SendMsg();
}

// 开服基金奖励领取
void CActivityManager::OnNetOpenServerFundRewardTake(WORD wRewardId)
{
	// 取奖励配置
	const OpenServerFundReward_Config *pRewardCfg = g_pCfgMgr->GetOpenServerFundReward(wRewardId);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到开服基金奖励配置[RewardId:%d]", __FUNCTION__, wRewardId);
		return;
	}

	OpenServerFundData &tFundData = m_pActivityInfo->tOpenServerFundData;

	// 检测是否已购买了基金
	if (!tFundData.wIsAlreadyBuyFund)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]还木有购买开服基金,不能领取福利", __FUNCTION__);
		return;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pRewardCfg->wNeedRoleLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]角色等级限制,不能领取基金奖励", __FUNCTION__);
		return;
	}

	// 检测是否已经领取过
	if (IsTakeFundReward(wRewardId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]开服基金奖励已领取过[RewardId:%d]", __FUNCTION__, wRewardId);
		return;
	}

	// 检测列表否已满(安全考虑)
	if (tFundData.wFundRewardTakeRecordNum >= MAX_OPEN_SERVER_FUND_REWARD)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]开服基金奖励列表已满", __FUNCTION__);
		return;
	}

	// 发奖励
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRewardCfg->nDiamond, TRUE, FROM_FUND);

	// 记录已领取
	tFundData.arFundRewardTakeRecord[tFundData.wFundRewardTakeRecordNum++] = wRewardId;

	// 响应
	MSG_S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP msg;
	msg.m_wRewardId = wRewardId;
	m_pPlayer->SendPacket(&msg);
}

// 全民福利领取
void CActivityManager::OnNetAllPeopleWelfareTake(WORD wWelfareId)
{
	// 取奖励配置
	const AllPeopleWelfare_Config *pWelfareCfg = g_pCfgMgr->GetAllPeopleWelfare(wWelfareId);
	if (!pWelfareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到全民福利配置[WelfareId:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	AllPeopleWelfareData &tWelfareData = m_pActivityInfo->tAllPoepleWelfareData;

	// 检测是否已经领取过
	if (IsTakeAllPeopleWelfare(wWelfareId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]全民福利已领取过[RewardId:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	// 检测列表是否已满
	if (tWelfareData.wTakeWelfareRecordNum >= MAX_ALL_THE_PEOPLE_WELFARE_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]全民福利列表已满", __FUNCTION__);
		return;
	}

	// 检测已购买人数
	if (g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).dwOpenServerFundBuyTimes < pWelfareCfg->wNeedBuyNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]基金购买人数不足!", __FUNCTION__);
		return;
	}
	
	// 发奖励
	m_pPlayer->AddObjectList(pWelfareCfg->vItemList, TRUE, FROM_ALL_PEOPLE_WELFARE);

	// 记录已领取福利
	tWelfareData.arTakeWelfareRecordList[tWelfareData.wTakeWelfareRecordNum++] = wWelfareId;

	// 响应
	MSG_S2C_ALL_PEOPLE_WELFARE_TAKE_RESP msg;
	msg.m_wWelfareId = wWelfareId;
	m_pPlayer->SendPacket(&msg);
}

// 发送开服基金数据
void CActivityManager::SendOpenServerFundData()
{
	MSG_S2C_OPEN_SERVER_FUND_DATA_RESP msg;
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId());
	msg.m_dwOpenServerFundBuyTimes = tGlobalData.dwOpenServerFundBuyTimes;
	msg.m_tFundData = m_pActivityInfo->tOpenServerFundData;
	msg.m_tWelfareData = m_pActivityInfo->tAllPoepleWelfareData;
	m_pPlayer->SendPacket(&msg);
}

// 是否已领取基金奖励
bool CActivityManager::IsTakeFundReward(WORD wRewardId)
{
	for (WORD i = 0; i < m_pActivityInfo->tOpenServerFundData.wFundRewardTakeRecordNum; ++i)
	{
		if (m_pActivityInfo->tOpenServerFundData.arFundRewardTakeRecord[i] == wRewardId)
			return true;
	}
	return false;
}

// 是否已领取全民福利
bool CActivityManager::IsTakeAllPeopleWelfare(WORD wWelfareId)
{
	for (WORD i = 0; i < m_pActivityInfo->tAllPoepleWelfareData.wTakeWelfareRecordNum; ++i)
	{
		if (m_pActivityInfo->tAllPoepleWelfareData.arTakeWelfareRecordList[i] == wWelfareId)
			return true;
	}
	return false;
}

// 请求限时优惠活动数据
void CActivityManager::OnNetLimitPreferentialReq()
{
	SendLimitPreferentialData();
}

// 领取限时优惠福利
void CActivityManager::OnNetTakeLimitPreferentialWelfare(WORD wWelfareId)
{
	// 取福利配置
	const LimitPreferentialWelfare_Config *pWelfareCfg = g_pCfgMgr->GetLimitPreferentialWelfare(wWelfareId);
	if (!pWelfareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]限时优惠福利配置[welfare_id:%d]", __FUNCTION__, wWelfareId);
		return;
	}
	
	// 检测领取条件(购买次数是否满足)
	if (pWelfareCfg->wNeedBuyTimes > g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).dwLimitActivityGoodsBuyTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]限时优惠商品购买次数不足,不能领取福利[welfare_id:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	LimitPreferentialData &tLimitPreferentialData = m_pActivityInfo->tLimitPreferentialData;

	// 检测是否已领取过
	for (WORD i = 0; i < tLimitPreferentialData.wTakeWelfareRecordNum; ++i)
	{
		if (tLimitPreferentialData.arTakeWelfareRecordList[i] == wWelfareId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]限时优惠福利已领取过[welfare_id:%d]", __FUNCTION__, wWelfareId);
			return;
		}
	}

	// 检测记录列表是否已满
	if (tLimitPreferentialData.wTakeWelfareRecordNum >= MAX_LIMIT_PREFERENTIAL_WELFARE_NUM)
	{
		// I will always rember this spical day...for you and me....
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]限时优惠福利记录列表已满[welfare_id:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(pWelfareCfg->vItemList, TRUE, FROM_LIMIT_PREFERENTIAL_WELFARE);

	// 添加记录
	tLimitPreferentialData.arTakeWelfareRecordList[tLimitPreferentialData.wTakeWelfareRecordNum++] = wWelfareId;

	SendLimitPreferentialData();

	MSG_S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP msg;
	msg.m_wWelfareId = wWelfareId;
	m_pPlayer->SendPacket(&msg);
}

// 发送限时优惠活动数据
void CActivityManager::SendLimitPreferentialData()
{
	MSG_S2C_LIMIT_PREFERENTIAL_DATA_RESP msg;
	msg.m_Data = m_pActivityInfo->tLimitPreferentialData;
	m_pPlayer->SendPacket(&msg);
}

// 次日奖励数据请求
void CActivityManager::OnNetNextDayRewardData()
{
	MSG_S2C_NEXT_DAY_REWARD_DATA_RESP msg;
	msg.m_wIsTakeReward = m_pActivityInfo->wIsTakeNextDayReward;
	m_pPlayer->SendPacket(&msg);
}

// 领取次日奖励
void CActivityManager::OnNetTakeNextDayReward()
{
	// 检测是否已领取过
	if (m_pActivityInfo->wIsTakeNextDayReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_NEXT_DAY_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]次日奖励已领取", __FUNCTION__);
		return;
	}

	// 检测是否同一天
	if (IsTheSameDay(m_pPlayer->GetRoleFullInfo().tRoleBaseInfo.dwRoleCreateTime, time(NULL)))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_NEXT_DAY_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]当天不能领取奖励", __FUNCTION__);
		return;
	}
		
	m_pActivityInfo->wIsTakeNextDayReward = true;

	DWORD dwItemId = g_GetConfigValue(188);
	m_pPlayer->AddObject(dwItemId, 1, TRUE, FROM_NEXT_DAY_REWARD);

	MSG_S2C_NEXT_DAY_REWARD_TAKE_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 登录奖励数据请求
void CActivityManager::OnNetLoginRewardData()
{
	MSG_S2C_LOGIN_REWARD_DATA_RESP msg;
	msg.m_wLoginDays = m_pLoginRewardData->wLoginDays;
	msg.m_wRewardRecordNum = 0;
	for (WORD i = 0; i < m_pLoginRewardData->wRewardRecordNum; ++i)
	{
		msg.m_RewardRecordList[msg.m_wRewardRecordNum++] = m_pLoginRewardData->arRewardRecordList[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// 领取登录奖励请求
void CActivityManager::OnNetTakeLoginReward(WORD wDays)
{
	WORD wFromMsgId = C2S_TAKE_LOGIN_REWARD_REQ;

	// 取配置
	const LoginReward_Config *pRewardCfg = g_pCfgMgr->GetLoginReward(wDays);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]取不到登录奖励配置[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// 检测是否已领取过
	if (IsTakeLoginReward(wDays))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]登录奖励已领取过[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// 检测天数是否已满足
	WORD wLoginDays = m_pLoginRewardData->wLoginDays;
	if (wLoginDays < wDays)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]登录天数不足,不能领取[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// 检测记录列表是否已满(安全性检测)
	if (m_pLoginRewardData->wRewardRecordNum >= MAX_LOGIN_REWARD_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]登录奖励记录列表已满", __FUNCTION__);
		return;
	}

	// 添加记录
	m_pLoginRewardData->arRewardRecordList[m_pLoginRewardData->wRewardRecordNum++] = wDays;

	// 发奖励
	m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, FROM_GET_LOGIN_REWARD);

	MSG_S2C_TAKE_LOGIN_REWARD_RESP msg;
	msg.m_wDays = wDays;
	m_pPlayer->SendPacket(&msg);
}

// 检测是否已领取登录奖励
bool CActivityManager::IsTakeLoginReward(WORD wDays)
{
	for (WORD i = 0; i < m_pLoginRewardData->wRewardRecordNum; ++i)
	{
		if (m_pLoginRewardData->arRewardRecordList[i] == wDays)
			return true;
	}
	return false;
}

// 在线奖励数据请求
void CActivityManager::OnNetOnlineRewardData()
{
	SendOnlineRewardData();
}

// 领取在线奖励请求
void CActivityManager::OnNetTakeOnlineReward()
{
	WORD wDays = m_pPlayer->GetRoleCreateDays();

	// 取在线奖励配置
	const OnlineReward_Config *pOnlineRewardCfg = g_pCfgMgr->GetOnlineReward(wDays);
	if (!pOnlineRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到在线奖励配置[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// 检测天数是否越界
	if (!wDays || wDays > MAX_ONLINE_REWARD_DAY)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]在线奖励天数已越界[days:%d]", __FUNCTION__, wDays);
		return;
	}

	OnlineRewardData &tOnlineRewardData = m_pActivityInfo->tOnlineRewardData;

	// 本时间段的在线时间
	DWORD dwOnlienTime = GetOnlineTime() + tOnlineRewardData.dwOnlineTime;

	WORD *pRewardList = tOnlineRewardData.arRewardRewardList[wDays-1];

	// 取当前要领取的奖励时间下标
	int nTimeIdx = -1;
	for (int i = 0; i < MAX_ONLINE_REWARD_TIME_LIST; ++i)
	{
		if (!pRewardList[i])
		{
			nTimeIdx = i;
			break;
		}
	}

	// 检测今天的奖励是否已全部领取完
	if (-1 == nTimeIdx)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]今日在线奖励已全部领取完[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// 检测下标是否越界
	if (nTimeIdx >= pOnlineRewardCfg->vTimeDataList.size())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到在线奖励时间段配置[days:%d,index:%d]", __FUNCTION__, wDays, nTimeIdx);
		return;
	}

	const OnlineRewardTime_Config *pTimeCfg = &pOnlineRewardCfg->vTimeDataList[nTimeIdx];

	// 检测在线时间是否足够
	if (pTimeCfg->dwTime > dwOnlienTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]在线时间不足,不能领取在线奖励[days:%d,online_time:%d]", __FUNCTION__, wDays, dwOnlienTime);
		return;
	}

	// 发放奖励
	m_pPlayer->AddObjectList(pTimeCfg->vRewardList, TRUE, FROM_GET_ONLINE_REWARD);

	// 记录为已领取
	pRewardList[nTimeIdx] = true;

	// 领取当前的奖励后重新记录在线时间
	tOnlineRewardData.dwOnlineTime = 0;
	tOnlineRewardData.dwStartTime = time(NULL);

	// 响应
	MSG_S2C_TAKE_ONLINE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 发送在线奖励数据
void CActivityManager::SendOnlineRewardData()
{
	MSG_S2C_ONLINE_REWARD_DATA_RESP msg;
	msg.m_OnlineRewardData = m_pActivityInfo->tOnlineRewardData;
	msg.m_OnlineRewardData.dwOnlineTime += GetOnlineTime(); // 需要加上本次登录在线的时间
	m_pPlayer->SendPacket(&msg);
}

// 取在线时间
DWORD CActivityManager::GetOnlineTime()
{
	return time(NULL) - m_pActivityInfo->tOnlineRewardData.dwStartTime;
}