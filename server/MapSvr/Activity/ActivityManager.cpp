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

	// todo::���в��������ڽ�ɫ���ϴ������ã��³�������ʱ���ƹ���
}

// �ǳ�
void CActivityManager::OnLogout()
{
	
}

// ǩ��
void CActivityManager::OnNetSignIn()
{
	// ����Ƿ���Ҫ����(ǩ���Ժ���Ҫ��������)
	if (m_pSignInData->wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay() && !IsTheSameDay(m_pSignInData->dwLastSignInTime, time(NULL)))
		m_pSignInData->wCurSignInDay = 0;

	// �������Ƿ���ǩ��
	DATETIME lastSignTime(m_pSignInData->dwLastSignInTime);
	DATETIME curTime(time(NULL));
	if (curTime.day == lastSignTime.day && curTime.month == lastSignTime.month)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SIGN_IN_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]������ǩ��", __FUNCTION__);
		return;
	}

	// ��ǰǩ������
	WORD wCurSignInDay = m_pSignInData->wCurSignInDay + 1;

	// ȡǩ������
	const SignIn_Config *pSignInCfg = g_pCfgMgr->GetSignIn(wCurSignInDay);
	if (!pSignInCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SIGN_IN_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ǩ����������[day:%d]", __FUNCTION__, wCurSignInDay);
		return;
	}

	WORD wItemNum = pSignInCfg->tRewardItem.dwNum;

	// �Ƿ�����˫������
	if (pSignInCfg->byDoubleRewardVipLv/* && m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= pSignInCfg->byDoubleRewardVipLv*/)
	{
		WORD wVipLevel = m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);

		// �����996BT�汾
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
			wVipLevel = m_pPlayer->GetVipGMLevel();

		if (wVipLevel >= pSignInCfg->byDoubleRewardVipLv)
			wItemNum *= 2;
	}

	// ��ӽ���
	m_pPlayer->AddObject(pSignInCfg->tRewardItem.dwId, wItemNum, true, FROM_SIGN_IN);

	// ����ǩ������
	m_pSignInData->dwLastSignInTime = time(NULL);
	m_pSignInData->wCurSignInDay = wCurSignInDay;

	// ����Ƿ���Ҫ����(ǩ���Ժ���Ҫ��������)
// 	if (wCurSignInDay > g_pCfgMgr->GetSignInTotalDay())
// 	{
// 		m_pSignInData->wCurSignInDay = 0;
// 	}

	// ����ͬ�����ݸ��ͻ���
	SendSignInData();

	// ��Ӧ
	MSG_S2C_SIGN_IN_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ǩ��
void CActivityManager::OnNetSignInForTest()
{
	// ����Ƿ���Ҫ����(ǩ���Ժ���Ҫ��������)
	if (m_pSignInData->wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay() && !IsTheSameDay(m_pSignInData->dwLastSignInTime, time(NULL)))
		m_pSignInData->wCurSignInDay = 0;

	// ��ǰǩ������
	WORD wCurSignInDay = m_pSignInData->wCurSignInDay + 1;

	// ȡǩ������
	const SignIn_Config *pSignInCfg = g_pCfgMgr->GetSignIn(wCurSignInDay);
	if (!pSignInCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ǩ����������[day:%d]", __FUNCTION__, wCurSignInDay);
		return;
	}

	WORD wItemNum = pSignInCfg->tRewardItem.dwNum;

	// �Ƿ�����˫������
	if (pSignInCfg->byDoubleRewardVipLv && m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= pSignInCfg->byDoubleRewardVipLv)
	{
		wItemNum *= 2;
	}

	// ��ӽ���
	m_pPlayer->AddObject(pSignInCfg->tRewardItem.dwId, wItemNum, true, FROM_SIGN_IN);

	// ����ǩ������
	m_pSignInData->dwLastSignInTime = time(NULL);
	m_pSignInData->wCurSignInDay = wCurSignInDay;

	// ����Ƿ���Ҫ����(ǩ���Ժ���Ҫ��������)
// 	if (wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay())
// 	{
// 		m_pSignInData->wCurSignInDay = 0;
// 	}

	// ����ͬ�����ݸ��ͻ���
	SendSignInData();
}

// ����ǩ������
void CActivityManager::OnNetSignInDataReq()
{
	SendSignInData();
}

// ����ǩ������
void CActivityManager::SendSignInData()
{
	// ����Ƿ���Ҫ����(ǩ���Ժ���Ҫ��������)
	if (m_pSignInData->wCurSignInDay >= g_pCfgMgr->GetSignInTotalDay() && !IsTheSameDay(m_pSignInData->dwLastSignInTime,time(NULL)))
		m_pSignInData->wCurSignInDay = 0;

	MSG_S2C_SIGN_IN_DATA_RESP msg;
	msg.m_byCurSignInDay = m_pSignInData->wCurSignInDay;
	msg.m_dwLastSignInTime = m_pSignInData->dwLastSignInTime;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ����ǩ������
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
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������ϵ�ǩ������[signin_id:%d]", __FUNCTION__, wSignId);
		return;
	}

	// ����Ƿ������ȡ
	if (LUXURY_RECHARGE_CAN_TAKE != pSignData->wStatus)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����ǩ����������ȡ[signin_id:%d,status:%d]", __FUNCTION__, wSignId, pSignData->wStatus);
		return;
	}

	// ���Ž���
	const LuxurySignInReward_Config *pSignInRewardCfg =  g_pCfgMgr->CConfigManager::GetLuxurySignInReward(pSignData->wRewardId);
	if (!pSignInRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǩ����������[reward_id:%d]", __FUNCTION__, pSignData->wRewardId);
		return;
	}

	m_pPlayer->AddObjectList(pSignInRewardCfg->vItemList, TRUE, FROM_LUXURY_SIGN_REWARD);

	pSignData->wStatus = LUXURY_RECHARGE_ALREADY_TAKE;

	SendLuxurySignData();

	MSG_S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP msg;
	msg.m_wSignInId = wSignId;
	m_pPlayer->SendPacket(&msg);
}

// �������ǩ������
void CActivityManager::OnNetLuxurySignData()
{
	SendLuxurySignData();
}

// ���ͺ���ǩ������
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

// ��ʼ������ǩ������
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
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǩ������[signin_id:%d, role_evel:%d]", __FUNCTION__, iter1->first,m_pPlayer->GetLevel());
			return;
		}

		// �����ܸ���
		int nTotalRate = 0;
		for (size_t i = 0; i < iter2->second.vRewardRandList.size(); ++i)
		{
			nTotalRate += iter2->second.vRewardRandList[i].nRate;
		}
			
		// �������
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

// ����ǩ����ֵ
void CActivityManager::LuxurySignInRecharge(WORD wRechangeId)
{
	bool bIsNeedSync = false;
	for (WORD i = 0; i < m_pLuxurySignInData->wSignDataNum; ++i)
	{
		LuxurySignData &signInData = m_pLuxurySignInData->arSignDataList[i];
		// ��û��ɵ�
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

// ��ȡ��������
void CActivityManager::OnNetYCSReward()
{
	// todo::���������еķ������Ƿ񿪷�

	const YCS_Config *pYCSCfg = g_pCfgMgr->GetYCS(m_pPlayer->GetLevel());
	if (!pYCSCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_YCS_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ӭ��������[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	YCSData &tYCSData = m_pActivityInfo->tYCSData;

	DWORD dwValue = 0;

	// ��ȡ�ۻ�����
	if (tYCSData.byTotalGetTimes >= g_GetConfigValue(62))
	{
		m_pPlayer->ModifyProperty(pYCSCfg->byMoneyType, tYCSData.nTotalRewardValue, TRUE, FROM_YCS);

		dwValue = tYCSData.nTotalRewardValue;

		// ��ȡ���������
		tYCSData.nTotalRewardValue = 0;
		tYCSData.byTotalGetTimes = 0;
	}
	// ��ȡ��ͨ����
	else
	{
		// �����ȡ�ļ��ʱ��
		if (time(NULL) < tYCSData.dwLastGetTime + g_GetConfigValue(63) * SECOND_OF_MIN)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_YCS_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]ӭ������ȡ�������ʱ��CD��...", __FUNCTION__);
			return;
		}

		// ���մ����Ƿ��Ѵ�����
		if (tYCSData.byTodayGetTimes >= pYCSCfg->vMoneyValueList.size())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_YCS_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]���յĽ�������������[times:%d]", __FUNCTION__, tYCSData.byTodayGetTimes);
			return;
		}

		int nAddValue = pYCSCfg->vMoneyValueList[tYCSData.byTodayGetTimes];
		m_pPlayer->ModifyProperty(pYCSCfg->byMoneyType, nAddValue, TRUE, FROM_YCS);

		// ������ȡ����
		tYCSData.byTodayGetTimes++;

		tm tNow;
		GetTMTime(&tNow);
		tm tLast;
		GetTMTime(&tLast, tYCSData.dwLastGetTime);

		if ((tNow.tm_yday == tLast.tm_yday) // �����ͬһ�� 
			|| (tLast.tm_yday + 1 == tNow.tm_yday && tYCSData.byTotalGetTimes == g_GetConfigValue(64))) // ����ǵڶ���,��ǰһ��ľ���ж�
		{
			tYCSData.byTotalGetTimes++;
			tYCSData.nTotalRewardValue += nAddValue;
		}
		// ��ȡ�����ж� �� �����ж� ����Ҫ����
		else
		{
			tYCSData.byTotalGetTimes = 1;
			tYCSData.nTotalRewardValue = nAddValue;
		}	
		// ��ȡ�����ж� �� �����ж� ����Ҫ����
// 		else if (m_pActivityInfo->tYCSData.byTotalGetTimes < g_GetConfigValue(64) || tLast.tm_yday + 1 < tNow.tm_yday)
// 		{
// 			m_pActivityInfo->tYCSData.byTotalGetTimes = 1;
// 			m_pActivityInfo->tYCSData.nTotalRewardValue = nAddValue;
// 		}

		// ������ȡ����ʱ��
		tYCSData.dwLastGetTime = time(NULL);

		dwValue = nAddValue;
	}

	// ����ͬ������
	SendYCSData();

	// ��Ӧ
	MSG_S2C_YCS_REWARD_RESP msg;
	msg.m_wType = pYCSCfg->byMoneyType;
	msg.m_dwValue = dwValue;
	m_pPlayer->SendPacket(&msg);
}

// ����ӭ��������
void CActivityManager::OnNetYCSDataReq()
{
	SendYCSData();
}

// ����ӭ��������
void CActivityManager::SendYCSData()
{
	MSG_S2C_YCS_DATA_RESP msg;
	msg.m_tData = m_pActivityInfo->tYCSData;
	m_pPlayer->SendPacket(&msg);
}


// ��ȡ��������
void CActivityManager::OnNetTQTReward()
{
	// todo::���������еķ������Ƿ񿪷�

	// ȡ��ǰʱ��ν���
	tm tNow;
	GetTMTime(&tNow);
	const TQT_Config *pTQTCfg = GetTQTCfgByTime(tNow.tm_hour * 100 + tNow.tm_min);
	if (!pTQTCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TQT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ڲ���ͭȸ̨�ʱ��", __FUNCTION__);
		return;
	}

	TQTData &tTQTData = m_pActivityInfo->tTQTData;

	// ����Ƿ�����ȡ��
	for (WORD i = 0; i < tTQTData.wTakeRecordNum; ++i)
	{
		if (tTQTData.arTakeRecordList[i] == pTQTCfg->wId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TQT_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]ͭȸ̨��������ȡ��[id:%d]", __FUNCTION__, pTQTCfg->wId);
			return;
		}
	}

	// ����б��Ƿ�����
	if (tTQTData.wTakeRecordNum >= MAX_TQT_TAKE_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TQT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ͭȸ̨������ȡ�б�����", __FUNCTION__);
		return;
	}

	// �������
	m_pPlayer->ModifyProperty(ROLE_PRO_STAMINA, g_GetConfigValue(65), TRUE, FROM_TQT);

	tTQTData.dwLastGetTime = time(NULL);
	tTQTData.arTakeRecordList[tTQTData.wTakeRecordNum++] = pTQTCfg->wId;

	// ����ͬ������
	SendTQTData();

	// ��Ӧ
	MSG_S2C_TQT_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ����ͭȸ̨����
void CActivityManager::OnNetTQTDataReq()
{
	SendTQTData();
}

// ����ͭȸ̨����
void CActivityManager::SendTQTData()
{
	MSG_S2C_TQT_DATA_RESP msg;
	msg.m_tData = m_pActivityInfo->tTQTData;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ�ͭȸ̨�ʱ��
bool CActivityManager::IsTQTActivityTime(int nTime)
{
	const vector<TQT_Config> *pTQTListCfg = g_pCfgMgr->GetTQTList();
	if (!pTQTListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ͭȸ̨�����б�Ϊ��", __FUNCTION__);
		return false;
	}

	for (vector<TQT_Config>::const_iterator iter = pTQTListCfg->begin(); iter != pTQTListCfg->end(); ++iter)
	{
		if (nTime >= iter->nStartTime && nTime <= iter->nOverTime)
			return true;
	}

	return false;
}

// ͨ��ʱ��ȡͭȸ̨�����
const TQT_Config * CActivityManager::GetTQTCfgByTime(int nTime)
{
	const vector<TQT_Config> *pTQTListCfg = g_pCfgMgr->GetTQTList();
	if (!pTQTListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ͭȸ̨�����б�Ϊ��", __FUNCTION__);
		return NULL;
	}

	for (vector<TQT_Config>::const_iterator iter = pTQTListCfg->begin(); iter != pTQTListCfg->end(); ++iter)
	{
		if (nTime >= iter->nStartTime && nTime <= iter->nOverTime)
			return &*iter;
	}

	return NULL;
}

// VIP�����������
void CActivityManager::OnNetVipGiftDataReq()
{
	SendVipGiftData();
}

// ÿ�������ȡ
void CActivityManager::OnNetVipDailyGiftTake(WORD wVipLevel)
{
	// ���VIP�ȼ�
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < wVipLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_DAILY_GIFT_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Vip�ȼ�����,������ȡ[VipLevel:%d]", __FUNCTION__, wVipLevel);
		return;
	}

	// ����Ƿ�����ȡ��
	if (m_pActivityInfo->tVipGiftData.nVipLevelRewardRecord >= wVipLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_DAILY_GIFT_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]VIPÿ���������ȡ��[VipLevel:%d]", __FUNCTION__, wVipLevel);
		return;
	}

	// ȡVIP�ȼ��������
	const VipDailyGift_Config *pGiftCfg = g_pCfgMgr->GetVipDailyGift(wVipLevel);
	if (!pGiftCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_DAILY_GIFT_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���VIPÿ���������[VipLevel:%d]", __FUNCTION__, wVipLevel);
		return;
	}

	// ���Ž���
	m_pPlayer->AddObjectList(pGiftCfg->vItemList, TRUE, FROM_DAILY_GIFT_TAKE);

	// ��¼����ȡ
	m_pActivityInfo->tVipGiftData.nVipLevelRewardRecord = wVipLevel;

	// ��Ӧ
	MSG_S2C_VIP_DAILY_GIFT_GET_RESP msg;
	msg.m_wVipLevel = wVipLevel;
	m_pPlayer->SendPacket(&msg);
}

// ÿ���������
void CActivityManager::OnNetVipWeekGiftBuy(WORD wGiftId)
{
	Gift *pGift = NULL;

	VipGiftData &tGiftData = m_pActivityInfo->tVipGiftData;

	// ȡ�������
	for (WORD i = 0; i < tGiftData.wWeekGiftNum; ++i)
	{
		if (tGiftData.arWeekGiftList[i].wId == wGiftId)
		{
			pGift = &tGiftData.arWeekGiftList[i];
			break;
		}
	}

	// �����ڵ����
	if (!pGift)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����ڵ�VIP�����[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// �Ѿ������
	if (pGift->wNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]VIP������Ѿ������[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// ȡ�������
	const VipWeekGift_Config *pGiftCfg = g_pCfgMgr->GetVipWeekGift(wGiftId);
	if (!pGiftCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���VIP���������[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// �����������
	if (!m_pPlayer->CheckMoneyCost(pGiftCfg->vCostList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_VIP_WEEK_GIFT_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����VIP��������Ĳ���[GiftId:%d]", __FUNCTION__, wGiftId);
		return;
	}

	// ���¹������
	++pGift->wNum;

	// ��������
	m_pPlayer->DoMoneyCost(pGiftCfg->vCostList, 1, FROM_WEEK_GIFT_BUY);

	// �����Ʒ
	m_pPlayer->AddObjectList(pGiftCfg->vItemList, TRUE, FROM_WEEK_GIFT_BUY);

	// ��Ӧ
	MSG_S2C_VIP_WEEK_GIFT_BUY_RESP msg;
	msg.m_wGiftId = wGiftId;
	m_pPlayer->SendPacket(&msg);
}


// ����VIP�������
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

// ˢ�������
void CActivityManager::RefreshWeekGift()
{
	VipGiftData &tVipGiftData = m_pActivityInfo->tVipGiftData;
	tVipGiftData.wWeekGiftNum = 0;

	const VipWeekGiftRandList_Config *pGiftListCfg = g_pCfgMgr->GetVipWeekGiftRandList(m_pPlayer->GetLevel());
	if (!pGiftListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫVIP����������б�[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	if (pGiftListCfg->wGiftRandNum > MAX_VIP_WEEK_GIFT_NUM || 
		pGiftListCfg->wGiftRandNum > pGiftListCfg->vGiftRandList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ɫVIP���������������ó���[RandNum:%d]", 
			__FUNCTION__, pGiftListCfg->wGiftRandNum);
		return;
	}

	vector<RateDataEx> vGiftRandList = pGiftListCfg->vGiftRandList;
	for (WORD i = 0; i < pGiftListCfg->wGiftRandNum; ++i)
	{
		// �����ܸ���
		int nTotalRate = 0;
		for (size_t k = 0; k < vGiftRandList.size(); ++k)
			nTotalRate += vGiftRandList[k].nRate;

		// ������
		int nRandValue = g_Rand(1, nTotalRate);
		
		// ȡ���е����,��������б���ɾ��
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

// ��ȡ�����Ƽ���ֵ�Ľ���
void CActivityManager::OnNetTakeTodayRecommendReward()
{
	VipGiftData &tGiftData = m_pActivityInfo->tVipGiftData;

	// ����Ƿ������ȡ
	if (tGiftData.wTodayRecommendRechargeRewardStatus != REWARD_STATUS_DONE)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����Ƽ���ֵ�����������ȡ[status:%d]",
			__FUNCTION__, tGiftData.wTodayRecommendRechargeRewardStatus);
		return;
	}

	// ȡ��������
	const TodayRecommendRecharge_Config *pRechargeCfg = g_pCfgMgr->GetTodayRecommendRecharge(tGiftData.wTodayRecommendRechargeRewardId);
	if (!pRechargeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������Ƽ���ֵ����[giftid:%d]",
			__FUNCTION__, tGiftData.wTodayRecommendRechargeRewardId);
		return;
	}

	m_pPlayer->AddObjectList(pRechargeCfg->vItemList, TRUE, FROM_TODAY_RECOMMEND_REWARD);

	// �������ȡ��
	tGiftData.wTodayRecommendRechargeRewardStatus = REWARD_STATUS_ALREADY_TAKE;

	MSG_S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ���ý����Ƽ���ֵ����
void CActivityManager::ResetTodayRecommendRechargeData()
{
	// �����ֵ��λ
	WORD wInitVipLevel = m_pActivityInfo->tVipGiftData.wInitVipLevel;
	const vector<RateDataEx> *pRandDataList = g_pCfgMgr->GetTodayRecommendRechargeRandList(wInitVipLevel);
	if (!pRandDataList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������Ƽ���ֵ�������[VipLevel:%d]",__FUNCTION__, wInitVipLevel);
		return;
	}

	// Ĭ����1
	m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardId = 1;

	// �����ܸ���
	int nTotalRate = 0;
	vector<RateDataEx>::const_iterator iter = pRandDataList->begin();
	for (; iter != pRandDataList->end(); ++iter)
	{
		nTotalRate += iter->nRate;
	}

	// ���һ��ֵ
	int nRandValue = g_Rand(0, nTotalRate);

	// ���㵱ǰ���е�
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

// �ȼ������������
void CActivityManager::OnNetLevelShopActivityDataReq()
{
	MSG_S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP msg;
	msg.m_wActivityDataNum = m_pActivityInfo->wLevelShopActivityDataNum;
	int nCopyDataSize = sizeof(LevelShopActivityData)* m_pActivityInfo->wLevelShopActivityDataNum;
	memcpy_s(msg.m_ActivityDataList, nCopyDataSize, m_pActivityInfo->arLevelShopActivityDataList, nCopyDataSize);
	m_pPlayer->SendPacket(&msg);
}

// ��ɫ�ȼ�����
void CActivityManager::RoleLevelUp(WORD wFrom, WORD wTo)
{
	for (WORD i = wFrom; i <= wTo; ++i)
	{
		// ���˵ȼ��Ƿ������
		const LevelShopData_Config *pGiftCfg = g_pCfgMgr->GetLevelShopData(i);
		if (!pGiftCfg)
			continue;

		// ����б��Ƿ�����
		if (m_pActivityInfo->wLevelShopActivityDataNum >= MAX_LEVEL_SHOP_ACTIVITY_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ȼ�����б�����", __FUNCTION__);
			continue;
		}

		// ����Ƿ��Ѿ�������
		if (GetLevelShopActivityData(i))
			continue;

		// ����
		m_pActivityInfo->arLevelShopActivityDataList[m_pActivityInfo->wLevelShopActivityDataNum].dwActivateTime = time(NULL);
		m_pActivityInfo->arLevelShopActivityDataList[m_pActivityInfo->wLevelShopActivityDataNum].wLevel = i;
		++m_pActivityInfo->wLevelShopActivityDataNum;
	}
}

// ȡ�ȼ��������
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

// �ȼ������������
void CActivityManager::OnNetLevelGiftDataReq()
{
	MSG_S2C_LEVEL_GIFT_DATA_RESP msg;
	msg.m_wLevelGiftTakeRecordNum = m_pActivityInfo->wLevelGiftTakeRecordNum;
	int nCopyDataSize = sizeof(WORD)*m_pActivityInfo->wLevelGiftTakeRecordNum;
	memcpy_s(msg.m_arLevelGiftTakeRecord, nCopyDataSize, m_pActivityInfo->arLevelGiftTakeRecord, nCopyDataSize);
	m_pPlayer->SendPacket(&msg);
}

// �ȼ������ȡ
void CActivityManager::OnNetTakeLevelGift(WORD wLevel)
{
	// ����Ƿ��Ѿ���ȡ��
	for (WORD i = 0; i < m_pActivityInfo->wLevelGiftTakeRecordNum; ++i)
	{
		if (m_pActivityInfo->arLevelGiftTakeRecord[i] == wLevel)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LEVEL_GIFT_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�ȼ������������ȡ��[Level:%d]", __FUNCTION__, wLevel);
			return;
		}
	}

	// ��ȡ����
	const LevelGift_Config *pLevelGiftCfg = g_pCfgMgr->GetLevelGift(wLevel);
	if (!pLevelGiftCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LEVEL_GIFT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ȼ��������[Level:%d]", __FUNCTION__, wLevel);
		return;
	}

	// ��⽱����¼�б��Ƿ�����
	if (m_pActivityInfo->wLevelGiftTakeRecordNum >= MAX_LEVEL_GIFT_TAKE_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TAKE_LEVEL_GIFT_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�ȼ������ȡ��¼�б�����[Level:%d]", __FUNCTION__, wLevel);
		return;
	}

	// ��ӽ���
	m_pPlayer->AddObjectList(pLevelGiftCfg->vItemList, TRUE, FROM_LEVEL_GIFT);

	// ��¼��ȡ����
	m_pActivityInfo->arLevelGiftTakeRecord[m_pActivityInfo->wLevelGiftTakeRecordNum++] = wLevel;

	// ��Ӧ
	MSG_S2C_TAKE_LEVEL_GIFT_REWARD_RESP msg;
	msg.m_wLevel = wLevel;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ�����ȡ�ȼ����
bool CActivityManager::IsTakeLevelGift(WORD wLevel)
{
	for (WORD i = 0; i < m_pActivityInfo->wLevelGiftTakeRecordNum; ++i)
	{
		if (m_pActivityInfo->arLevelGiftTakeRecord[i] == wLevel)
			return true;
	}
	return false;
}

// �Ƿ�����
// bool CActivityManager::IsActivityOpen(int nSystemId, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime)
// {
// 	// ȡ�����
// 	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
// 	if (!pActivityListCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, nSystemId);
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

// �Ƿ�����
bool CActivityManager::IsActivityOpen(int nSystemId, Player *pPlayer)
{
	// ȡ�����
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, nSystemId);
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

// ȡ�����ID
WORD CActivityManager::GetActivityDataId(int nSystemId)
{
	// ȡ�����
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, nSystemId);
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

// ȡ�����
WORD CActivityManager::GetActivityDay(int nSystemId)
{
	// ȡ�����
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, nSystemId);
		return 0;
	}

	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(m_pPlayer->GetServerId());
	if (!pServerInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������Ϣ[server_id:%d]", __FUNCTION__, m_pPlayer->GetServerId());
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
			// ���ʱ�������
			// ָ��ʱ�� ��ʼ����[��:�� : ��] - ��������[��:�� : ��]
			if (ACTIVITY_TIME_APPOINT == iter->nTimeType)
			{
				int nYear = SplitInteger(iter->nStartDate,8, 0, 4);
				int nMonth = SplitInteger(iter->nStartDate,8, 4, 2);
				int nDay = SplitInteger(iter->nStartDate, 8, 6, 2);
				int nHour = SplitInteger(iter->nStartTime,4, 0, 2);
				int nMin = SplitInteger(iter->nStartTime, 4, 2, 2);
				// todo::������BUG
				DWORD dwLastTime = DATETIME(nYear, nMonth, nDay, nHour, nMin, 0);
				nPassDays = GetPassDays(dwLastTime, dwCurTime);
			}
			// ÿ�� ��ʼ����[0] - ��������[0]
			else if (ACTIVITY_TIME_DAILY == iter->nTimeType)
			{
				nPassDays = GetPassDays(pServerInfo->dwOpenTime, dwCurTime);
			}
			// ���� ��ʼ����[���ڼ�] ��������[���ڼ�]
			else if (ACTIVITY_TIME_WEEK == iter->nTimeType)
			{
				int nCurWeekDay = g_CorrectWeekDay(tNow.tm_wday);
				int nStartWeekDay = g_CorrectWeekDay(iter->nStartDate);
				int nOverWeekDay = g_CorrectWeekDay(iter->nOverDate);

				// �ڻ�ڼ���
				if (nCurWeekDay >= nStartWeekDay && nCurWeekDay <= nOverWeekDay)
				{
					nPassDays = nCurWeekDay - nStartWeekDay + 1;
				}
			}
// 			// �·� ��ʼ����[����] - ��������[����]
// 			else if (ACTIVITY_TIME_MONTH == iter->nTimeType)
// 			{
// 
// 			}
			// ���� ��ʼ����[�����ڼ���] - ��������[�����ڼ���](todo::���߼�Ӧ��������,���Ƿ�ֹ֮ǰ�Ĺ��������⣬�ȱ����۲�)
			else if (ACTIVITY_TIME_OPEN_SERVER == iter->nTimeType)
			{
				nPassDays = GetPassDays(pServerInfo->dwOpenTime, dwCurTime);
			}
			// ���� ��ʼ����[�����ڼ���] - ��������[�����ڼ���](todo::���߼����ǶԵ�)
			else if (ACTIVITY_TIME_OPEN_SERVER_EX == iter->nTimeType)
			{
				// �����������ָ�����������ǻ�Ŀ�ʼʱ��
				time_t tLastTime = pServerInfo->dwOpenTime + g_Day2Second(iter->nStartDate-1);
				tm tmDetail;
				localtime_s(&tmDetail, &tLastTime);

				// ���Сʱ,����,�룬�ӵ����0�㿪ʼ��
				tmDetail.tm_hour = 0;
				tmDetail.tm_min = 0;
				tmDetail.tm_sec = 0;
				tLastTime = mktime(&tmDetail);

				// ���ȥ�˶�����
				nPassDays = GetPassDays(tLastTime, dwCurTime);
			}
			//  ������ɫ ��ʼ����[���ǵڼ���] - ��������[���ǵڼ���]
			else if (ACTIVITY_TIME_CREATE_ROLE == iter->nTimeType)
			{
				// ������ɫ�������ָ�����������ǻ�Ŀ�ʼʱ��
				time_t tLastTime = m_pPlayer->GetRoleCreateTime() + g_Day2Second(iter->nStartDate - 1);
				tm tmDetail;
				localtime_s(&tmDetail, &tLastTime);

				// ���Сʱ,����,�룬�ӵ����0�㿪ʼ��
				tmDetail.tm_hour = 0;
				tmDetail.tm_min = 0;
				tmDetail.tm_sec = 0;
				tLastTime = mktime(&tmDetail);

				// ���ȥ�˶�����
				nPassDays = GetPassDays(tLastTime, dwCurTime);
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ļʱ������[time_type:%d]", __FUNCTION__, iter->nTimeType);
				return nPassDays;
			}

			return nPassDays;
		}
	}

	return 0;
}

// �Ƿ�����
bool CActivityManager::IsActivityOpen(const Activity_Config *pActivityCfg, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime)
{
 	// ���ȼ�����
	if (wRoleLevel && wRoleLevel < pActivityCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ȼ����Ʋ��ܲ���[SytemId:%d]", __FUNCTION__, pActivityCfg->nSystemId);
		return false;
	}

	return CheckActivityTime(pActivityCfg, dwServerId, dwCreateRoleTime);
}

bool CheckDateTime(int nCurData, int nStartDate, int nEndDate, int nCurTime, int nStartTime, int nEndTime)
{
	//  �������ʱ��
	if (nCurData == nStartDate && nCurTime >= nStartTime)
		return true;

	//  ������һ��(0=��ʾ24��,��һ����)
	if (nCurData == nEndDate && (0 == nEndTime || nCurTime <= nEndTime))
		return true;

	// �������(�����������,�����е�ʱ��㶼��Ч)
	if (nCurData > nStartDate && nCurData < nEndDate)
		return true;

	return false;
};

// ���ʱ��
bool CActivityManager::CheckActivityTime(const ActivityBase_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime)
 {
 	// ���������ڷ������Ƿ񿪷Ŵ˻
 	if (!pActivityCfg->IsOpenServer(dwServerId))
 		return false;
 
 	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(dwServerId);
 	if (!pServerInfo)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������Ϣ[ServerId:%d]", __FUNCTION__, dwServerId);
 		return false;
 	}
 
 	// ��⿪��ʱ�������
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
 		// ���ʱ�������
 		// ָ��ʱ�� ��ʼ����[��:�� : ��] - ��������[��:�� : ��]
 	case ACTIVITY_TIME_APPOINT:
 		return CheckDateTime(nCurDate, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;
 
 		// ÿ�� ��ʼ����[0] - ��������[0]
 	case ACTIVITY_TIME_DAILY:
		return CheckDateTime(tNow.tm_yday, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;
 
 		// ���� ��ʼ����[���ڼ�] ��������[���ڼ�]
 	case ACTIVITY_TIME_WEEK:
 		{
 			int nCurWeekDay = g_CorrectWeekDay(tNow.tm_wday);
 			int nOverWeekDay = g_CorrectWeekDay(pActivityCfg->nOverDate);
 			int nStartWeekDay = g_CorrectWeekDay(pActivityCfg->nStartDate);
 			return CheckDateTime(nCurWeekDay, nStartWeekDay, nOverWeekDay, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		}
 		break;
 
 		// �·� ��ʼ����[����] ��������[����]
 	case ACTIVITY_TIME_MONTH:
		return CheckDateTime(tNow.tm_mday, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;
 
 		// ���� ��ʼ����[�����ڼ���] - ��������[�����ڼ���]
 	case ACTIVITY_TIME_OPEN_SERVER:
 	case ACTIVITY_TIME_OPEN_SERVER_EX:
 		return CheckDateTime(nAlreadyOpenDays, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
 		break;

	case ACTIVITY_TIME_CREATE_ROLE:
		{
			// ��⿪��ʱ�������
			int nAlreadyCreateDays = GetPassDays(dwCreateRoleTime, time(NULL));
			return CheckDateTime(nAlreadyCreateDays, pActivityCfg->nStartDate, pActivityCfg->nOverDate, nCurTime, pActivityCfg->nStartTime, pActivityCfg->nOverTime);
		}
 		break;
 
 	default:
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ļʱ������[time_type:%d]", __FUNCTION__, pActivityCfg->nTimeType);
		break;
 	}
 
 	return false;
 }


 // ���ʱ��(��ʱ����ʹ��)
//  bool CActivityManager::CheckActivityTimeEx(const ActivityBase_Config *pActivityCfg, DWORD dwServerId)
//  {
// 	 // ���������ڷ������Ƿ񿪷Ŵ˻
// 	 if (!pActivityCfg->IsOpenServer(dwServerId))
// 		 return false;
// 
// 	 const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(dwServerId);
// 	 if (!pServerInfo)
// 	 {
// 		 MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������Ϣ[ServerId:%d]", __FUNCTION__, dwServerId);
// 		 return false;
// 	 }
// 
// 	// ��⿪��ʱ�������
// 	int nAlreadyOpenDays = GetPassDays(pServerInfo->dwOpenTime, time(NULL));
// 	if (nAlreadyOpenDays < pActivityCfg->nDays)
// 	{
// 	// 		MAPLOG(ERROR_LEVEL, "[%s]��������������������[ServerId:%d, AlreayOpenDays:%d]",
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
// 	// ��⿪ʼʱ��
// 	if (pActivityCfg->nStartTime && nCurTime < pActivityCfg->nStartTime)
// 	  	return false;
// 	  
// 	// ������ʱ��
// 	if (pActivityCfg->nOverTime && nCurTime > pActivityCfg->nOverTime)
// 	  	return false;
// 	  
// 	// ���ʱ�������
// 	// ָ��ʱ�� ��ʼ����[��:�� : ��] - ��������[��:�� : ��]
// 	if (ACTIVITY_TIME_APPOINT == pActivityCfg->nTimeType)
// 	{	 
// 	  	// �������
// 	  	if (nCurDate >= pActivityCfg->nStartDate && nCurDate <= pActivityCfg->nOverDate)
// 	  		return true;
// 	}
// 	// ÿ�� ��ʼ����[0] - ��������[0]
// 	else if (ACTIVITY_TIME_DAILY == pActivityCfg->nTimeType)
// 	{
// 	  	return true;
// 	}
// 	// ���� ��ʼ����[���ڼ�] ��������[���ڼ�]
// 	else if (ACTIVITY_TIME_WEEK == pActivityCfg->nTimeType)
// 	{
// 	  	int nCurWeekDay = g_CorrectWeekDay(tNow.tm_wday);
// 	  	int nStartWeekDay = g_CorrectWeekDay(pActivityCfg->nStartDate);
// 	  	int nOverWeekDay = g_CorrectWeekDay(pActivityCfg->nOverDate);
// 	  	if (nCurWeekDay >= nStartWeekDay && nCurWeekDay <= nOverWeekDay)
// 	  		return true;
// 	}
// 	// �·� ��ʼ����[����] - ��������[����]
// 	else if (ACTIVITY_TIME_MONTH == pActivityCfg->nTimeType)
// 	{
// 	  	int nMonthDays = tNow.tm_mday;
// 	  	if (nMonthDays >= pActivityCfg->nStartDate && nMonthDays <= pActivityCfg->nOverDate)
// 	  		return true;
// 	}
// 	// ���� ��ʼ����[�����ڼ���] - ��������[�����ڼ���]
// 	else if (ACTIVITY_TIME_OPEN_SERVER == pActivityCfg->nTimeType || ACTIVITY_TIME_OPEN_SERVER_EX == pActivityCfg->nTimeType)
// 	{
// 	  	if (nAlreadyOpenDays >= pActivityCfg->nStartDate && nAlreadyOpenDays <= pActivityCfg->nOverDate)
// 	  		return true;
// 	}
// 	else
// 	{
// 	  	MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ļʱ������[time_type:%d]", __FUNCTION__, pActivityCfg->nTimeType);
// 	  	return false;
// 	}
// 	  
// 	return false;
//  }

// �Ƿ��к��
bool CActivityManager::IsHaveRedPoint(WORD wSystemId)
{
	// ǩ��
	if (SYSTEM_SIGN == wSystemId)
	{
		DATETIME lastSignTime(m_pSignInData->dwLastSignInTime);
		DATETIME curTime(time(NULL));
		// ��ͬ����,���߲�ͬ���·�
		if (curTime.day != lastSignTime.day || curTime.month != lastSignTime.month)
			return true;
	}
	// ͭȸ̨(������ȡ)
	else if (SYSTEM_TQT == wSystemId)
	{	
		tm tNow;
		GetTMTime(&tNow);
		// ����Ƿ�ʱ��
		if (!IsTQTActivityTime(tNow.tm_hour * 100 + tNow.tm_min))
			return false;

		tm tLast;
		GetTMTime(&tLast, m_pActivityInfo->tTQTData.dwLastGetTime);
		// �������Ƿ�����ȡ
		if (tLast.tm_yday == tNow.tm_yday)
		{
			const TQT_Config *pLastCfg = GetTQTCfgByTime(tLast.tm_hour * 100 + tLast.tm_min);
			const TQT_Config *pNowCfg = GetTQTCfgByTime(tNow.tm_hour * 100 + tNow.tm_min);

			// ����ʱ��λ�Ƿ�����ȡ
			if (pLastCfg && pNowCfg && pLastCfg->nStartTime == pNowCfg->nStartTime)
				return false;
		}

		return true;
	}
	// ӭ����(�����ȡ)
	else if (SYSTEM_YCS == wSystemId)
	{
		YCSData &tYCSData = m_pActivityInfo->tYCSData;

		// ����ۻ�����
		if (tYCSData.byTotalGetTimes >= g_GetConfigValue(62))
			return true;

		// �����ȡ�ļ��ʱ��
		DWORD dwCurTime = time(NULL);
		if (dwCurTime >= tYCSData.dwLastGetTime + g_GetConfigValue(63) * SECOND_OF_MIN)
		{
			const YCS_Config *pYCSCfg = g_pCfgMgr->GetYCS(m_pPlayer->GetLevel());
			if (!pYCSCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ӭ��������[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
				return false;
			}
			
			// ������ȡ������û��
			if (tYCSData.byTodayGetTimes < pYCSCfg->vMoneyValueList.size())
				return true;
		}
	}
	// �ȼ��̵�
	else if (SYSTEM_LEVEL_SHOP == wSystemId)
	{
		for (WORD i = 0; i < m_pActivityInfo->wLevelShopActivityDataNum; ++i)
		{
			LevelShopActivityData &tGiftData = m_pActivityInfo->arLevelShopActivityDataList[i];
			const LevelShopData_Config *pLevelGiftCfg = g_pCfgMgr->GetLevelShopData(tGiftData.wLevel);
			// �̵껹û����
			if (pLevelGiftCfg && time(NULL) < tGiftData.dwActivateTime + pLevelGiftCfg->nValidTime)
			{
				for (size_t k = 0; k < pLevelGiftCfg->vGoodsList.size(); ++k)
				{
					// ����л�û�й������Ʒ
					if (!m_pPlayer->GetShopManager()->GetGoodsBuyRecordNum(SHOP_LEVEL_GIFT, pLevelGiftCfg->vGoodsList[k]))
						return true;
				}
			}
		}
	}
	// �ȼ����
	else if (SYSTEM_LEVEL_GIFT == wSystemId)
	{
		map<DWORD, LevelGift_Config>::iterator iter = g_pCfgMgr->m_LevelGiftList.begin();
		for (; iter != g_pCfgMgr->m_LevelGiftList.end(); ++iter)
		{
			if (m_pPlayer->GetLevel() >= iter->second.wLevel && !IsTakeLevelGift(iter->second.wLevel))
				return true;
		}
	}
	// VIP���(VIP����п���ȡ����ʱ)
	else if (SYSTEM_VIP_GIFT == wSystemId)
	{
		VipGiftData &tGiftData = m_pActivityInfo->tVipGiftData;

		// VIP�ȼ����������ȡ
		int nVipLevel = m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);
		if (tGiftData.nVipLevelRewardRecord < nVipLevel)
			return true;

		// ������ɹ���
		for (WORD i = 0; i < tGiftData.wWeekGiftNum; ++i)
		{
			// ��vip�ȼ������Ҹ������û���������ʾ���
			Gift &tGift = tGiftData.arWeekGiftList[i];
			// ��û����
			if (!tGift.wNum)
			{
				// VIP�ȼ���������
				const VipWeekGift_Config *pGiftCfg = g_pCfgMgr->GetVipWeekGift(tGift.wId);
				if (pGiftCfg && m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) >= pGiftCfg->wNeedVipLevel)
					return true;	
			}
		}
	}
	// ���������п���ȡ����ʱ
	else if (SYSTEM_FUND == wSystemId)
	{
		OpenServerFundData &tFundData = m_pActivityInfo->tOpenServerFundData;

		// ����Ƿ��ѹ����˻���
		if (!tFundData.wIsAlreadyBuyFund)
			return false;

		map<DWORD, OpenServerFundReward_Config>::iterator iter = g_pCfgMgr->m_OpenServerFundRewardList.begin();
		for (; iter != g_pCfgMgr->m_OpenServerFundRewardList.end(); ++iter)
		{
			// ȡ��������
			const OpenServerFundReward_Config *pRewardCfg = g_pCfgMgr->GetOpenServerFundReward(iter->second.wRewardId);
			if (pRewardCfg && m_pPlayer->GetLevel() >= pRewardCfg->wNeedRoleLevel && !IsTakeFundReward(pRewardCfg->wRewardId))
				return true;
		}
	}
	// ��ȫ�����������п���ȡ����ʱ 
	else if (SYSTEM_ALL_PEOPLE_WELFARE == wSystemId)
	{
		OpenServerFundData &tFundData = m_pActivityInfo->tOpenServerFundData;

		DWORD dwBuyTimes = g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).dwOpenServerFundBuyTimes;
		map<DWORD, AllPeopleWelfare_Config>::iterator iter = g_pCfgMgr->m_AllPeopleWelfareList.begin();
		for (; iter != g_pCfgMgr->m_AllPeopleWelfareList.end(); ++iter)
		{
			// ȡ��������
			const AllPeopleWelfare_Config *pWelfareCfg = g_pCfgMgr->GetAllPeopleWelfare(iter->second.wWelfareId);
			if (pWelfareCfg && dwBuyTimes >= pWelfareCfg->wNeedBuyNum && !IsTakeAllPeopleWelfare(pWelfareCfg->wWelfareId))
				return true;
		}
	}
	// ����ǩ��
	else if (SYSTEM_LUXURY_SIGN_IN == wSystemId)
	{
		for (WORD i = 0; i < m_pLuxurySignInData->wSignDataNum; ++i)
		{
			if (m_pLuxurySignInData->arSignDataList[i].wStatus == LUXURY_RECHARGE_CAN_TAKE)
				return true;
		}
	}
	// ��¼����
	else if (SYSTEM_LOGIN_REWARD == wSystemId)
	{
		WORD wLoginDays = m_pLoginRewardData->wLoginDays; //m_pPlayer->GetRoleCreateDays();
		const map<WORD, LoginReward_Config> &loginRewardList = g_pCfgMgr->GetLoginRewardList();
		map<WORD, LoginReward_Config>::const_iterator iter =  loginRewardList.begin();
		for (; iter != loginRewardList.end(); ++iter)
		{
			// �ѵ����¼����,�һ�û��ȡ��
			if (wLoginDays >= iter->second.wDays && !IsTakeLoginReward(iter->second.wDays))
				return true;
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ĺ����ʾ����[SystemId:%d]", __FUNCTION__, wSystemId);
		return false;
	}

	return false;
}

// ���ʼʱ��
// DWORD CActivityManager::GetActivityStartTime(int nSystemId)
// {
// 	// ȡ�����
// 	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(nSystemId);
// 	if (!pActivityListCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, nSystemId);
// 		return 0;
// 	}
// 
// 	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(m_pPlayer->GetServerId());
// 	if (!pServerInfo)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������Ϣ[server_id:%d]", __FUNCTION__, m_pPlayer->GetServerId());
// 		return 0;
// 	}
// 
// 	DWORD dwStartTime = 0;
// 	vector<Activity_Config>::const_iterator iter = pActivityListCfg->begin();
// 	for (; iter != pActivityListCfg->end(); ++iter)
// 	{
// 		if (IsActivityOpen(&*iter, m_pPlayer->GetServerId(), m_pPlayer->GetLevel()))
// 		{
// 			// ���ʱ�������
// 			// ָ��ʱ�� ��ʼ����[��:�� : ��] - ��������[��:�� : ��]
// 			if (ACTIVITY_TIME_APPOINT == iter->nTimeType)
// 			{
// 				int nYear = SplitInteger(iter->nStartDate, 8, 0, 4);
// 				int nMonth = SplitInteger(iter->nStartDate, 8, 4, 2);
// 				int nDay = SplitInteger(iter->nStartDate, 8, 6, 2);
// 				int nHour = SplitInteger(iter->nStartTime, 4, 0, 2);
// 				int nMin = SplitInteger(iter->nStartTime, 4, 2, 2);
// 				// ������BUG
// 				dwStartTime = DATETIME(nYear, nMonth, nDay, nHour, nMin, 0);
// 			}
// 			// ÿ�� ��ʼ����[0] - ��������[0]
// 			else if (ACTIVITY_TIME_DAILY == iter->nTimeType)
// 			{
// 				dwStartTime = pServerInfo->dwOpenTime;
// 			}
// 			// ���� ��ʼ����[���ڼ�] ��������[���ڼ�]
// 			// 			else if (3 == iter->nTimeType)
// 			// 			{
// 			// 
// 			// 			}
// 			// 			// �·� ��ʼ����[����] - ��������[����]
// 			// 			else if (4 == iter->nTimeType)
// 			// 			{
// 			// 
// 			// 			}
// 			// ���� ��ʼ����[�����ڼ���] - ��������[�����ڼ���]
// 			else if (ACTIVITY_TIME_OPEN_SERVER == iter->nTimeType || ACTIVITY_TIME_OPEN_SERVER_EX == iter->nTimeType)
// 			{
// 				dwStartTime = pServerInfo->dwOpenTime;
// 
// 				//nPassDays = GetPassDays(pServerInfo->dwOpenTime, dwCurTime);
// 			}
// 			else
// 			{
// 				MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ļʱ������[time_type:%d]", __FUNCTION__, iter->nTimeType);
// 			}
// 			break;
// 		}
// 	}
// 
// 	return dwStartTime;
// }

// �Ƿ�����
bool CActivityManager::IsActivityOver(DWORD dwServerId, int nSystemId, WORD wDataId, DWORD dwCreateRoleTime)
{
	const Activity_Config *pActivityCfg = g_pCfgMgr->GetActivityTime(nSystemId, wDataId);
	if (!pActivityCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d,DataId:%d]", __FUNCTION__, nSystemId, wDataId);
		return false;
	}

	return IsActivityOpen(&*pActivityCfg, dwServerId, 0, dwCreateRoleTime) ? false : true;
}

// �Ƿ���ʱ���������
int CActivityManager::GetLimitProductMultiple(WORD wMapType)
{
	const LimitProduct_Config *pProductCfg = g_pCfgMgr->GetLimitProduct(wMapType);
	if (!pProductCfg || pProductCfg->vStartTimeList.size() != pProductCfg->vOverTimeList.size())
	{
		//MAPLOG(GUI_LEVEL, "[%s]�Ҳ�����ʱ���������[SystemId:%d%d]", __FUNCTION__, wSystemId);
		return 1;
	}

	tm tCurTime;
	GetTMTime(&tCurTime, time(NULL));

	int nCurDate = (tCurTime.tm_year + 1900) * 10000 + (tCurTime.tm_mon + 1) * 100 + tCurTime.tm_mday;
	int nCurTime = tCurTime.tm_hour * 100 + tCurTime.tm_min;

	// �������
	if (nCurDate < pProductCfg->nStartDate || nCurDate > pProductCfg->nOverDate)
		return 1;

	for (int i = 0; i < pProductCfg->vStartTimeList.size(); ++i)
	{
		// ���ʱ��
		if (nCurTime >= pProductCfg->vStartTimeList[i] && nCurTime <= pProductCfg->vOverTimeList[i])
			return pProductCfg->nMultiple;
	}

	return 1;
}

// ȡ�����ʱ��
int CActivityManager::GetActivityRewardDate(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(wSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, wSystemId);
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

// ȡ�����ʱ��
int CActivityManager::GetActivityRewardTime(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime)
{
	const vector<Activity_Config> *pActivityListCfg = g_pCfgMgr->GetActivityTime(wSystemId);
	if (!pActivityListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������[SystemId:%d]", __FUNCTION__, wSystemId);
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

// �����Ƽ���ֵ�ص�
void CActivityManager::OnTodayRecommendRechargeCallback()
{
	m_pActivityInfo->tVipGiftData.wTodayRecommendRechargeRewardStatus = REWARD_STATUS_DONE;
}

// ����������������
void CActivityManager::OnNetOpenServerFundDataReq()
{
	SendOpenServerFundData();
}

// ����������
void CActivityManager::OnNetOpenServerFundBuy()
{
	// �ﵽָ��VIP�ȼ���ɹ��򿪷�����
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < g_GetConfigValue(156))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]VIP�ȼ�����", __FUNCTION__);
		return;
	}

	// todo::��⹺�����ĵ���ʯ
	int nCostDiamond = g_GetConfigValue(157);
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����������ĵ���ʯ����", __FUNCTION__);
		return;
	}

	// ����Ƿ��Ѿ������
	if (m_pActivityInfo->tOpenServerFundData.wIsAlreadyBuyFund)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_BUY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���������ѹ����", __FUNCTION__);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_BUY_FUND);

	m_pActivityInfo->tOpenServerFundData.wIsAlreadyBuyFund = true;

	// ���¹�������
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId());
	tGlobalData.dwOpenServerFundBuyTimes++;

	// ��Ӧ
	MSG_S2C_OPEN_SERVER_FUND_BUY_RESP msg;
	m_pPlayer->SendPacket(&msg);

	// �洢ȫ������
	g_MapServer.SaveGolbalData(m_pPlayer->GetServerId(), GOLBAL_DATA_FUND_BUY_TIMES, tGlobalData.dwOpenServerFundBuyTimes);

	// ����㲥
	CSystemNotice sys_notice(13, m_pPlayer->GetServerId());
	sys_notice.AddParam(m_pPlayer->GetRoleName());
	sys_notice.SendMsg();
}

// ������������ȡ
void CActivityManager::OnNetOpenServerFundRewardTake(WORD wRewardId)
{
	// ȡ��������
	const OpenServerFundReward_Config *pRewardCfg = g_pCfgMgr->GetOpenServerFundReward(wRewardId);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������������[RewardId:%d]", __FUNCTION__, wRewardId);
		return;
	}

	OpenServerFundData &tFundData = m_pActivityInfo->tOpenServerFundData;

	// ����Ƿ��ѹ����˻���
	if (!tFundData.wIsAlreadyBuyFund)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ľ�й��򿪷�����,������ȡ����", __FUNCTION__);
		return;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pRewardCfg->wNeedRoleLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,������ȡ������", __FUNCTION__);
		return;
	}

	// ����Ƿ��Ѿ���ȡ��
	if (IsTakeFundReward(wRewardId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��������������ȡ��[RewardId:%d]", __FUNCTION__, wRewardId);
		return;
	}

	// ����б������(��ȫ����)
	if (tFundData.wFundRewardTakeRecordNum >= MAX_OPEN_SERVER_FUND_REWARD)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����������б�����", __FUNCTION__);
		return;
	}

	// ������
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRewardCfg->nDiamond, TRUE, FROM_FUND);

	// ��¼����ȡ
	tFundData.arFundRewardTakeRecord[tFundData.wFundRewardTakeRecordNum++] = wRewardId;

	// ��Ӧ
	MSG_S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP msg;
	msg.m_wRewardId = wRewardId;
	m_pPlayer->SendPacket(&msg);
}

// ȫ������ȡ
void CActivityManager::OnNetAllPeopleWelfareTake(WORD wWelfareId)
{
	// ȡ��������
	const AllPeopleWelfare_Config *pWelfareCfg = g_pCfgMgr->GetAllPeopleWelfare(wWelfareId);
	if (!pWelfareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ȫ��������[WelfareId:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	AllPeopleWelfareData &tWelfareData = m_pActivityInfo->tAllPoepleWelfareData;

	// ����Ƿ��Ѿ���ȡ��
	if (IsTakeAllPeopleWelfare(wWelfareId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ȫ��������ȡ��[RewardId:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	// ����б��Ƿ�����
	if (tWelfareData.wTakeWelfareRecordNum >= MAX_ALL_THE_PEOPLE_WELFARE_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ȫ�����б�����", __FUNCTION__);
		return;
	}

	// ����ѹ�������
	if (g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).dwOpenServerFundBuyTimes < pWelfareCfg->wNeedBuyNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��������������!", __FUNCTION__);
		return;
	}
	
	// ������
	m_pPlayer->AddObjectList(pWelfareCfg->vItemList, TRUE, FROM_ALL_PEOPLE_WELFARE);

	// ��¼����ȡ����
	tWelfareData.arTakeWelfareRecordList[tWelfareData.wTakeWelfareRecordNum++] = wWelfareId;

	// ��Ӧ
	MSG_S2C_ALL_PEOPLE_WELFARE_TAKE_RESP msg;
	msg.m_wWelfareId = wWelfareId;
	m_pPlayer->SendPacket(&msg);
}

// ���Ϳ�����������
void CActivityManager::SendOpenServerFundData()
{
	MSG_S2C_OPEN_SERVER_FUND_DATA_RESP msg;
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId());
	msg.m_dwOpenServerFundBuyTimes = tGlobalData.dwOpenServerFundBuyTimes;
	msg.m_tFundData = m_pActivityInfo->tOpenServerFundData;
	msg.m_tWelfareData = m_pActivityInfo->tAllPoepleWelfareData;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ�����ȡ������
bool CActivityManager::IsTakeFundReward(WORD wRewardId)
{
	for (WORD i = 0; i < m_pActivityInfo->tOpenServerFundData.wFundRewardTakeRecordNum; ++i)
	{
		if (m_pActivityInfo->tOpenServerFundData.arFundRewardTakeRecord[i] == wRewardId)
			return true;
	}
	return false;
}

// �Ƿ�����ȡȫ����
bool CActivityManager::IsTakeAllPeopleWelfare(WORD wWelfareId)
{
	for (WORD i = 0; i < m_pActivityInfo->tAllPoepleWelfareData.wTakeWelfareRecordNum; ++i)
	{
		if (m_pActivityInfo->tAllPoepleWelfareData.arTakeWelfareRecordList[i] == wWelfareId)
			return true;
	}
	return false;
}

// ������ʱ�Żݻ����
void CActivityManager::OnNetLimitPreferentialReq()
{
	SendLimitPreferentialData();
}

// ��ȡ��ʱ�Żݸ���
void CActivityManager::OnNetTakeLimitPreferentialWelfare(WORD wWelfareId)
{
	// ȡ��������
	const LimitPreferentialWelfare_Config *pWelfareCfg = g_pCfgMgr->GetLimitPreferentialWelfare(wWelfareId);
	if (!pWelfareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ʱ�Żݸ�������[welfare_id:%d]", __FUNCTION__, wWelfareId);
		return;
	}
	
	// �����ȡ����(��������Ƿ�����)
	if (pWelfareCfg->wNeedBuyTimes > g_MapServer.GetServerGlobalData(m_pPlayer->GetServerId()).dwLimitActivityGoodsBuyTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ʱ�Ż���Ʒ�����������,������ȡ����[welfare_id:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	LimitPreferentialData &tLimitPreferentialData = m_pActivityInfo->tLimitPreferentialData;

	// ����Ƿ�����ȡ��
	for (WORD i = 0; i < tLimitPreferentialData.wTakeWelfareRecordNum; ++i)
	{
		if (tLimitPreferentialData.arTakeWelfareRecordList[i] == wWelfareId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]��ʱ�Żݸ�������ȡ��[welfare_id:%d]", __FUNCTION__, wWelfareId);
			return;
		}
	}

	// ����¼�б��Ƿ�����
	if (tLimitPreferentialData.wTakeWelfareRecordNum >= MAX_LIMIT_PREFERENTIAL_WELFARE_NUM)
	{
		// I will always rember this spical day...for you and me....
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]��ʱ�Żݸ�����¼�б�����[welfare_id:%d]", __FUNCTION__, wWelfareId);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(pWelfareCfg->vItemList, TRUE, FROM_LIMIT_PREFERENTIAL_WELFARE);

	// ��Ӽ�¼
	tLimitPreferentialData.arTakeWelfareRecordList[tLimitPreferentialData.wTakeWelfareRecordNum++] = wWelfareId;

	SendLimitPreferentialData();

	MSG_S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP msg;
	msg.m_wWelfareId = wWelfareId;
	m_pPlayer->SendPacket(&msg);
}

// ������ʱ�Żݻ����
void CActivityManager::SendLimitPreferentialData()
{
	MSG_S2C_LIMIT_PREFERENTIAL_DATA_RESP msg;
	msg.m_Data = m_pActivityInfo->tLimitPreferentialData;
	m_pPlayer->SendPacket(&msg);
}

// ���ս�����������
void CActivityManager::OnNetNextDayRewardData()
{
	MSG_S2C_NEXT_DAY_REWARD_DATA_RESP msg;
	msg.m_wIsTakeReward = m_pActivityInfo->wIsTakeNextDayReward;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ���ս���
void CActivityManager::OnNetTakeNextDayReward()
{
	// ����Ƿ�����ȡ��
	if (m_pActivityInfo->wIsTakeNextDayReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_NEXT_DAY_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ս�������ȡ", __FUNCTION__);
		return;
	}

	// ����Ƿ�ͬһ��
	if (IsTheSameDay(m_pPlayer->GetRoleFullInfo().tRoleBaseInfo.dwRoleCreateTime, time(NULL)))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_NEXT_DAY_REWARD_TAKE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���첻����ȡ����", __FUNCTION__);
		return;
	}
		
	m_pActivityInfo->wIsTakeNextDayReward = true;

	DWORD dwItemId = g_GetConfigValue(188);
	m_pPlayer->AddObject(dwItemId, 1, TRUE, FROM_NEXT_DAY_REWARD);

	MSG_S2C_NEXT_DAY_REWARD_TAKE_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��¼������������
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

// ��ȡ��¼��������
void CActivityManager::OnNetTakeLoginReward(WORD wDays)
{
	WORD wFromMsgId = C2S_TAKE_LOGIN_REWARD_REQ;

	// ȡ����
	const LoginReward_Config *pRewardCfg = g_pCfgMgr->GetLoginReward(wDays);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������¼��������[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsTakeLoginReward(wDays))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��¼��������ȡ��[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// ��������Ƿ�������
	WORD wLoginDays = m_pLoginRewardData->wLoginDays;
	if (wLoginDays < wDays)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��¼��������,������ȡ[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// ����¼�б��Ƿ�����(��ȫ�Լ��)
	if (m_pLoginRewardData->wRewardRecordNum >= MAX_LOGIN_REWARD_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��¼������¼�б�����", __FUNCTION__);
		return;
	}

	// ��Ӽ�¼
	m_pLoginRewardData->arRewardRecordList[m_pLoginRewardData->wRewardRecordNum++] = wDays;

	// ������
	m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, FROM_GET_LOGIN_REWARD);

	MSG_S2C_TAKE_LOGIN_REWARD_RESP msg;
	msg.m_wDays = wDays;
	m_pPlayer->SendPacket(&msg);
}

// ����Ƿ�����ȡ��¼����
bool CActivityManager::IsTakeLoginReward(WORD wDays)
{
	for (WORD i = 0; i < m_pLoginRewardData->wRewardRecordNum; ++i)
	{
		if (m_pLoginRewardData->arRewardRecordList[i] == wDays)
			return true;
	}
	return false;
}

// ���߽�����������
void CActivityManager::OnNetOnlineRewardData()
{
	SendOnlineRewardData();
}

// ��ȡ���߽�������
void CActivityManager::OnNetTakeOnlineReward()
{
	WORD wDays = m_pPlayer->GetRoleCreateDays();

	// ȡ���߽�������
	const OnlineReward_Config *pOnlineRewardCfg = g_pCfgMgr->GetOnlineReward(wDays);
	if (!pOnlineRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�������߽�������[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// ��������Ƿ�Խ��
	if (!wDays || wDays > MAX_ONLINE_REWARD_DAY)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���߽���������Խ��[days:%d]", __FUNCTION__, wDays);
		return;
	}

	OnlineRewardData &tOnlineRewardData = m_pActivityInfo->tOnlineRewardData;

	// ��ʱ��ε�����ʱ��
	DWORD dwOnlienTime = GetOnlineTime() + tOnlineRewardData.dwOnlineTime;

	WORD *pRewardList = tOnlineRewardData.arRewardRewardList[wDays-1];

	// ȡ��ǰҪ��ȡ�Ľ���ʱ���±�
	int nTimeIdx = -1;
	for (int i = 0; i < MAX_ONLINE_REWARD_TIME_LIST; ++i)
	{
		if (!pRewardList[i])
		{
			nTimeIdx = i;
			break;
		}
	}

	// ������Ľ����Ƿ���ȫ����ȡ��
	if (-1 == nTimeIdx)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�������߽�����ȫ����ȡ��[days:%d]", __FUNCTION__, wDays);
		return;
	}

	// ����±��Ƿ�Խ��
	if (nTimeIdx >= pOnlineRewardCfg->vTimeDataList.size())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�������߽���ʱ�������[days:%d,index:%d]", __FUNCTION__, wDays, nTimeIdx);
		return;
	}

	const OnlineRewardTime_Config *pTimeCfg = &pOnlineRewardCfg->vTimeDataList[nTimeIdx];

	// �������ʱ���Ƿ��㹻
	if (pTimeCfg->dwTime > dwOnlienTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����ʱ�䲻��,������ȡ���߽���[days:%d,online_time:%d]", __FUNCTION__, wDays, dwOnlienTime);
		return;
	}

	// ���Ž���
	m_pPlayer->AddObjectList(pTimeCfg->vRewardList, TRUE, FROM_GET_ONLINE_REWARD);

	// ��¼Ϊ����ȡ
	pRewardList[nTimeIdx] = true;

	// ��ȡ��ǰ�Ľ��������¼�¼����ʱ��
	tOnlineRewardData.dwOnlineTime = 0;
	tOnlineRewardData.dwStartTime = time(NULL);

	// ��Ӧ
	MSG_S2C_TAKE_ONLINE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// �������߽�������
void CActivityManager::SendOnlineRewardData()
{
	MSG_S2C_ONLINE_REWARD_DATA_RESP msg;
	msg.m_OnlineRewardData = m_pActivityInfo->tOnlineRewardData;
	msg.m_OnlineRewardData.dwOnlineTime += GetOnlineTime(); // ��Ҫ���ϱ��ε�¼���ߵ�ʱ��
	m_pPlayer->SendPacket(&msg);
}

// ȡ����ʱ��
DWORD CActivityManager::GetOnlineTime()
{
	return time(NULL) - m_pActivityInfo->tOnlineRewardData.dwStartTime;
}