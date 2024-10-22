#include "stdafx.h"
#include "RechargeManager.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\Email\EmailManager.h"
#include "CommonLogic.h"
#include "..\Vip\VipManager.h"
#include "..\Activity\ActivityManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "ToolKit\MD5.h"
#include "..\AccountInfo.h"
#include "..\OpenServerSession.h"
#include "..\Shop\ShopManager.h"
#include "..\GameLogic.h"
#include "Jsoncpp\include\json\value.h"
#include "..\RewardCenter\RewardCenterManager.h"


CRechargeManager::CRechargeManager()
{

}

CRechargeManager::~CRechargeManager()
{

}

void CRechargeManager::Init(Player *pPlayer)
{
	m_dwLastRequestOrderTime = 0;
	m_dwSeriesId = 1;
	m_pPlayer = pPlayer;
	m_dwContext = 0;
	m_pRechargeData = &pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeData;
}

void CRechargeManager::OnNewDay()
{
	
}

// �����ֵ����
void CRechargeManager::OnNetRechargeData()
{
	SendRechargeData();
}

// ��ֵ����
void CRechargeManager::OnNetRecharge(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId)
{
	DATETIME dataTime(time(NULL));
	char szCurTime[32] = { 0 };
	dataTime.toString(szCurTime);

	RechargeCallbackData callbackData;
	memset(&callbackData, 0, sizeof(callbackData));
	callbackData.byIsTest = true;
	callbackData.dwChannel = 0;
	callbackData.byStatus = 0;
	callbackData.dwCPOrderId = time(NULL);
	callbackData.wRechargeId = wRechargeId;
	callbackData.wRechargeType = wRechargeType;
	callbackData.wCurrencyType = 0;
	callbackData.wGoodsId = wGoodsId;
	//strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), szCurTime);
	strcpy_s(callbackData.szPayTime, sizeof(callbackData.szPayTime), szCurTime);
	sprintf_s(callbackData.szChannelUid, "%lld", GetTickCount64());
	sprintf_s(callbackData.szSDKOrderId, "%s_%lld", szCurTime,GetTickCount64());
	sprintf_s(callbackData.szFailedDesc, "This is GM recharge!");
	callbackData.dwRoleId = m_pPlayer->GetRoleId();
	callbackData.wFrom = RECHARGE_FROM_GM;

	// ȡ��ֵ���
	callbackData.nAmount = GetRechargeAmount(wRechargeId);

	OnNetRechargeCallback(callbackData);
}

// ��ֵ����(����)
void CRechargeManager::OnNetRechargeEx(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId, DWORD dwContext)
{
	MAPLOG(GUI_LEVEL, "�����ֵ[RoleId:%d,RechargeId:%d,RechargeType:%d,GoodsId:%d,Context:%d]",
		m_pPlayer->GetRoleId(), wRechargeId, wRechargeType, wGoodsId, dwContext);

	// ����Ƿ�Խ�ϰ汾
	if (!g_MapServer.IsYGame())
	{
		MAPLOG(ERROR_LEVEL, "�˽ӿ�ֻ��YGame��Ч");
		return;
	}

	// ȡ��ֵ����
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId);
	if (!pRechargeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�Ҳ�����ֵ������Ϣ[RechargeId:%d]", wRechargeId);
		return;
	}

	// ���YCion�Ƿ��㹻
	if (m_pPlayer->GetProperty(ROLE_PRO_YCOIN) < pRechargeCfg->nYCoin)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "ycoin����,���ܹ���[RechargeId:%d,CostYCoin:%d]", wRechargeId, pRechargeCfg->nYCoin);
		return;
	}

	bool bIsFirstRecharge = IsFirstRecharge();

	bool bResult = false;

	WORD wType = wRechargeType;
	// 1=�¿�,2=��ֵ,3=��ֵ�ۿ�,4=ֱ���ػ�
	switch (wType)
	{
		// �¿�
// 	case RECHARGE_MONTH_CARD:
// 		bResult = DoMonthCard(wRechargeId);
// 		break;
		// ��ͨ��ֵ
	case RECHARGE_COMMON:
		bResult = DoCommonRecharge(pRechargeCfg);
		break;
		// �����ۿ�
	case RECHARGE_SUPER_DISCOUNT:
		bResult = DoNetSuperDiscount(pRechargeCfg);
		break;
		// �³����ۿ�
	case RECHARGE_NEW_SUPER_DISCOUNT:
		bResult = DoNetNewSuperDiscount(pRechargeCfg);
		break;
		// ֱ���ػ�
	case RECHARGE_DIRECT_BUY_DISCONT:
		bResult = DoDirectBuy(pRechargeCfg, wGoodsId);
		break;
		// �����Ƽ�
	case RECHARGE_TODAY_RECOMMEND:
		bResult = DoTodayRecommend(pRechargeCfg);
		break;
		// ��׼�Ƽ�
	case RECHARGE_ACCURATE_RECOMMEND:
		bResult = DoAccurateRecommend(pRechargeCfg, wGoodsId);
		break;
		// ������Ʒ
	case RECHARGE_HOT_SALE_GOODS:
		bResult = DoHotSaleGoods(pRechargeCfg, wGoodsId);
		break;
	default:
		MAPLOG(ERROR_LEVEL, "unknow recharge type[role_id:%d,recharge_type:%d,recharge_id:%d",m_pPlayer->GetRoleId(), wRechargeType, wRechargeId);
		return;
	}

	if (!bResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "�����ֵ�߼�ʧ��[role_id:%d, recharge_type:%d, recharge_id:%d]",m_pPlayer->GetRoleId(), wType, wRechargeId);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_YCOIN, -pRechargeCfg->nYCoin, TRUE, FROM_BUY_RECHARGE);

	//AddOrderRecord(callbackData.dwCPOrderId);

	// �ϱ���ֵ��¼(�ƶ�KeyData)
	//OnNetRechargeSuccessReport(callbackData.wRechargeId, callbackData.dwYDChannelId, callbackData.szIP, callbackData.szSDKOrderId);

	// ��ֵ�����
	m_pPlayer->GetActivityManager()->GetRechargeReward().OnRechargeCallback(wRechargeId);

	DWORD dwAmount = GetRechargeAmount(wRechargeId);

	// ��¼��ֵ���
	m_pPlayer->AddRechargeAmount(dwAmount);

	// �۳佱��
	m_pPlayer->GetActivityManager()->GetTotalRechargeReward().OnRechargeNotify(dwAmount);

	// ���Ƴ�ֵͳ��
	//RechargeCallbackData callbackData;
	//CGameLogic::Instance()->OnRechargeStatistics(callbackData, m_pPlayer);

	// ��Ϸ����ͳ��
	CGameLogic::Instance()->OnGameStatistics_Recharge(m_pPlayer, dwAmount, IsTodayFirstRecharge(), bIsFirstRecharge);

	RoleRechargeData &tRechargetData = m_pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeData;
	tRechargetData.wTodayRechargetTimes++;


	// ��ֵ�¼�֪ͨ�ͻ���
// 	MSG_S2C_RECHARGE_NOTIFY_RESP recharge_notify_msg;
// 	recharge_notify_msg.m_wRechargeType = wRechargeType;
// 	recharge_notify_msg.m_wRechargeId = wRechargeId;
// 	recharge_notify_msg.m_dwContext = wGoodsId;
// 	m_pPlayer->SendPacket(&recharge_notify_msg);
// 
// 	m_pPlayer->ModifyProperty(ROLE_PRO_YCOIN, -pRechargeCfg->nYCoin, TRUE, FROM_BUY_RECHARGE);

	DATETIME datetime(time(NULL));
	char szPayTime[32] = { 0 };
	datetime.toString(szPayTime);
	char szSDKOrderId[32] = { 0 };
	sprintf_s(szSDKOrderId, sizeof(szSDKOrderId), "%lld", g_GetCurrentTime());

	// ��ֵ��¼
	SaveRechargeRecord(wRechargeId, wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond, szPayTime, szSDKOrderId, RECHARGE_FROM_NORMAL);

	// ���Ƴ�ֵͳ��
	RechargeCallbackData callbackData;
	memset(&callbackData, 0, sizeof(RechargeCallbackData));
	strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), szSDKOrderId);
	callbackData.dwRoleId = m_pPlayer->GetRoleId();
	callbackData.wRechargeId = wRechargeId; 
	callbackData.wRechargeType = wRechargeType;
	CGameLogic::Instance()->OnRechargeStatistics(callbackData, m_pPlayer);

	// todo::��Ӧ
	MSG_S2C_RECHARGE_RESP msg;
	msg.m_wRechargeId = wRechargeId;
	msg.m_wRechargeType = wRechargeType;
	msg.m_wGoodsId = wGoodsId;
	msg.m_dwContext = dwContext;
	m_pPlayer->SendPacket(&msg);
}

// �����¿�����
void CRechargeManager::OnNetMonthCardData()
{
	SendMonthCardData();
}

// �����¿�
void CRechargeManager::OnNetMonthCardBuy(WORD wCardId)
{
// 	DeleteExpiredMonthCard();
// 
// 	// ȡ����
// 	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
// 	if (!pMonthCardCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����¿�����[CardId:%d]", __FUNCTION__, wCardId);
// 		return;
// 	}
// 
// 	// ����Ƿ��ѹ����
// 	if (GetMonthCard(wCardId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���������¿�[CardId:%d]", __FUNCTION__, wCardId);
// 		return;
// 	}
// 
// 	MSG_S2C_MONTH_CARD_BUY_RESP msg;
// 	msg.m_wCardId = wCardId;
// 	m_pPlayer->SendPacket(&msg);
// 
// 	// for test
// 	OnNetMonthCardBuyCallback(wCardId);
}

// ��ֵ����ص�
void CRechargeManager::OnNetRechargeCallbackEx(DWORD dwGuid, const RechargeCallbackData &callbackData, bool bIsNeedResp)
{
	bool bIsSuccess = true;
	bool bIsOnline = false;
	DWORD dwRoleId = callbackData.dwRoleId;
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByRoleId(dwRoleId);
	if (pPlayer)
	{
		CRechargeManager *pRechargeMgr = pPlayer->GetRechargeManager();
		bIsSuccess = pRechargeMgr->OnNetRechargeCallback(callbackData);
		bIsOnline = true;
	}
	else
	{
		Json::Value root;
		root["ip"] = callbackData.szIP;
		root["sdk_order_id"] = callbackData.szSDKOrderId;
		root["pay_time"] = callbackData.szPayTime;
		root["recharge_str_id"] = callbackData.szRechargeStrId;
		string strText = root.toStyledString();

		// �������ݿ�
		RoleTmpData_Update* pQuery = RoleTmpData_Update::ALLOC();
		pQuery->SetIndex(DB_ROLE_TEMP_DATA_UPDATE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		pQuery->SetRoleId(dwRoleId);
		pQuery->SetServerId(0);
		pQuery->AddParam(time(NULL));
		pQuery->AddParam(callbackData.byStatus);				// 1
		pQuery->AddParam(callbackData.wRechargeId);				// 2
		pQuery->AddParam(callbackData.dwCPOrderId);				// 3
		pQuery->AddParam(callbackData.nAmount);					// 4
		pQuery->AddParam(callbackData.dwChannel);				// 5
		pQuery->AddParam(callbackData.wRechargeType);			// 6
		pQuery->AddParam(callbackData.wGoodsId);				// 7
		pQuery->AddParam(callbackData.dwYDChannelId);			// 8
		pQuery->AddParam(callbackData.wCurrencyType);			// 9
		pQuery->AddParam(callbackData.byIsMyCard);				// 10
		pQuery->AddParam(callbackData.byIsCheckRechargeStrId);	// 11
		pQuery->AddParam(callbackData.byIsNotCheck);			// 12
		pQuery->AddParam(callbackData.wFrom);					// 13
		pQuery->SetText(strText.c_str());
		pQuery->SetDataType(ROLE_TEMP_RECHARGE_SEND_GOODS);
		g_MapServer.MapDBQuery(pQuery);
	}

	if (bIsSuccess)
	{
		// ��ʾ��ֵ�ɹ�
		MAPLOG(GUI_LEVEL, "�����ֵ�ɹ�[role_id:%d,amount:%d,order_id:%s,is_online:%d]",
			dwRoleId, callbackData.nAmount, callbackData.szSDKOrderId, bIsOnline);

		// ������Ӧ
		if (bIsNeedResp)
		{
			MSG_M2O_RECHARGE_SEND_GOODS_RESP msg;
			g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "�����ֵʧ��[role_id:%d,amount:%d,order_id:%s,is_online:%d]",
			dwRoleId, callbackData.nAmount, callbackData.szSDKOrderId, bIsOnline);
	}
}

// YGame��ֵ����ص�
void CRechargeManager::OnNetRechargeCallbackForYGame(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nAmount, const char *pszPaymentId)
{
	const YGamePayment_Config *pPaymentCfg = g_pCfgMgr->GetYGamePayment(pszPaymentId);
	if (!pPaymentCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ���ygame֧������[role_id:%d,payment_id:%s]",dwRoleId, pszPaymentId);
		return;
	}

	wServerId = g_MapServer.GetCurServerId(wServerId);

	// ��Ϸ��sdk��ֵ����Ҫ����, ygameƽ̨���ycoin
	if (1 != pPaymentCfg->nPaymentFromType)
	{
		RewardData tRewardData;
		tRewardData.wInfoId = 100;
		tRewardData.dwTime = time(NULL);
		tRewardData.dwValue1 = pPaymentCfg->dwAssistId;
		CRewardCenterManager::AddReward(wServerId, dwRoleId, tRewardData);
	}

	// ������Ӧ
	MSG_M2O_YGAME_RECHARGE_SEND_GOODS_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);

	MAPLOG(DEBUG_LEVEL, "Recv ygame recharge[role_id:%d,payment_id:%s,amount:%d,payment_from_type:%d]", 
		dwRoleId, pszPaymentId, nAmount, pPaymentCfg->nPaymentFromType);
}

// YGame����֪ͨ
void CRechargeManager::OnNetYGameCashbackNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nYCoin, DWORD dwDiamond)
{
	RewardData tRewardData;
	tRewardData.wInfoId = 101;
	tRewardData.dwTime = time(NULL);
	tRewardData.dwValue1 = dwDiamond;
	CRewardCenterManager::AddReward(wServerId, dwRoleId, tRewardData);

	// ������Ӧ
	MSG_M2O_YGAME_CASHBACK_NOTIFY_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);

	MAPLOG(DEBUG_LEVEL, "Recv ygame cashback[role_id:%d,ycoin:%d,diamond:%d]", dwRoleId, nYCoin, dwDiamond);
}

// YGame����֪ͨ
void CRechargeManager::OnNetYGameBonusNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, DWORD dwDiamond)
{
	RewardData tRewardData;
	tRewardData.wInfoId = 102;
	tRewardData.dwTime = time(NULL);
	tRewardData.dwValue1 = dwDiamond;
	CRewardCenterManager::AddReward(wServerId, dwRoleId, tRewardData);

	// ������Ӧ
	MSG_M2O_YGAME_BONUS_NOTIFY_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);

	MAPLOG(DEBUG_LEVEL, "Recv ygame bonus[role_id:%d,diamond:%d]", dwRoleId, dwDiamond);
}

// ��ֵ����ص�
bool CRechargeManager::OnNetRechargeCallback(const RechargeCallbackData &callbackData)
{
	// ��ֵ�ɹ�,���һ�û�����
	if (!IsOrderRecord(callbackData.dwCPOrderId))
	{
		bool bIsFirstRecharge = IsFirstRecharge();

		bool bResult = false;

		//DWORD dwOldDiamond = m_pPlayer->GetProperty(ROLE_PRO_DIAMOND);

		WORD wType = callbackData.wRechargeType;
		// 1=�¿�,2=��ֵ,3=��ֵ�ۿ�,4=ֱ���ػ�
		switch (wType)
		{
		// �¿�
		case RECHARGE_MONTH_CARD:
			bResult = OnNetMonthCardBuyCallback(callbackData);
			break;
		// ��ͨ��ֵ
		case RECHARGE_COMMON:
			bResult = OnNetCommonRechargeCallback(callbackData);
			break;
		// �����ۿ�
		case RECHARGE_SUPER_DISCOUNT:
			bResult = OnNetSuperDiscountCallback(callbackData);
			break;
		// �³����ۿ�
		case RECHARGE_NEW_SUPER_DISCOUNT:
			bResult = OnNetNewSuperDiscountCallback(callbackData);
			break;
		// ֱ���ػ�
		case RECHARGE_DIRECT_BUY_DISCONT:
			bResult = OnNetDirectBuyCallback(callbackData);
			break;
		// �����Ƽ�
		case RECHARGE_TODAY_RECOMMEND:
			bResult = OnNetTodayRecommendCallback(callbackData);
			break;
		// ��׼�Ƽ�
		case RECHARGE_ACCURATE_RECOMMEND:
			bResult = OnNetAccurateRecommendCallback(callbackData);
			break;
		// ������Ʒ
		case RECHARGE_HOT_SALE_GOODS:
			bResult = OnNetHotSaleGoodsCallback(callbackData);
			break;
		default:
			MAPLOG(ERROR_LEVEL, "unknow recharge type[role_id:%d,type:%d,order_id:%s]", 
				m_pPlayer->GetRoleId(), wType,callbackData.szSDKOrderId);
			return false;
		}

		if (!bResult)
		{
			MAPLOG(ERROR_LEVEL, "�����ֵ�߼�ʧ��[role_id:%d, recharge_type:%d, recharge_id:%d, order_id:%s]",
				m_pPlayer->GetRoleId(), wType, callbackData.wRechargeId, callbackData.szSDKOrderId);
			return false;
		}

		AddOrderRecord(callbackData.dwCPOrderId);

		// �ϱ���ֵ��¼(�ƶ�KeyData)
		//OnNetRechargeSuccessReport(callbackData.wRechargeId, callbackData.dwYDChannelId, callbackData.szIP, callbackData.szSDKOrderId);

		// ��ֵ�����
		m_pPlayer->GetActivityManager()->GetRechargeReward().OnRechargeCallback(callbackData.wRechargeId);

		DWORD dwAmount = GetRechargeAmount(callbackData.wRechargeId, callbackData.byIsMyCard);

		// ��¼��ֵ���
		m_pPlayer->AddRechargeAmount(dwAmount);

		// �۳佱��
		m_pPlayer->GetActivityManager()->GetTotalRechargeReward().OnRechargeNotify(dwAmount);

		// ���Ƴ�ֵͳ��
		CGameLogic::Instance()->OnRechargeStatistics(callbackData, m_pPlayer);

		// ��Ϸ����ͳ��
		CGameLogic::Instance()->OnGameStatistics_Recharge(m_pPlayer, dwAmount, IsTodayFirstRecharge(), bIsFirstRecharge);

		RoleRechargeData &tRechargetData = m_pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeData;
		tRechargetData.wTodayRechargetTimes++;

		// MyCara��ֵ�ϱ�
		if (callbackData.byIsMyCard)
			CGameLogic::Instance()->OnMyCardRechargeSuccessReport(m_pPlayer, dwAmount, callbackData.szSDKOrderId);

		// ���а�
		DWORD dwDiamond = GetRechargeDiamond(callbackData.wRechargeId, callbackData.byIsMyCard); 
		m_pPlayer->GetActivityManager()->GetRechargeAndCostRankActivity().AddRechargeAmount(dwDiamond);

		// ��ֵ�¼�֪ͨ�ͻ���
		MSG_S2C_RECHARGE_NOTIFY_RESP recharge_notify_msg;
		recharge_notify_msg.m_wRechargeType = callbackData.wRechargeType;
		recharge_notify_msg.m_wRechargeId = callbackData.wRechargeId;
		recharge_notify_msg.m_dwContext = m_dwContext;
		m_pPlayer->SendPacket(&recharge_notify_msg);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "�ظ�֪ͨ����[role_id:%d,amount:%d,channel:%d,order_id:%s]",
			m_pPlayer->GetRoleId(), callbackData.nAmount, callbackData.dwChannel, callbackData.szSDKOrderId);
	}

	return true;
}

// ��ֵ����ص�
bool CRechargeManager::OnNetCommonRechargeCallback(const RechargeCallbackData &callbackData)
{
	WORD wRechargeId = callbackData.wRechargeId;

	// ȡ����
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, callbackData.byIsMyCard);
	if (!pRechargeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[RechargeId:%d]", __FUNCTION__, wRechargeId);
		return false;
	}

	int nAmount = callbackData.wCurrencyType ? pRechargeCfg->nUSD : pRechargeCfg->nRMB;

	// ����ֵ���
	if (!CheckRechargeResult(nAmount, callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ���ʧ��[RechargeId:%d,RechargeType:%d]",
			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
		return false;
	}

	// ��ֵ����
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nExtraDiamond, TRUE, FROM_TOP_UP);


	MAPLOG(ERROR_LEVEL, "[%s]recharge notify[RoleId:%d, RechargeId:%d, RechargeType:%d, VipExp:%d, RechargeDiamond:%d, ExtraDiamond:%d]",
		__FUNCTION__, m_pPlayer->GetRoleId(), wRechargeId, callbackData.wRechargeType, pRechargeCfg->nVipExp,
		pRechargeCfg->nRechargeDiamond, pRechargeCfg->nExtraDiamond);

	// �׳佱��
	if (!FindCommonRecharge(wRechargeId))
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nFirstRechargeDiamond, TRUE, FROM_TOP_UP);

		// ���ʼ�֪ͨ(��ֵ)
		EmailInfo tEailInfo(6);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nFirstRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}
	else
	{
		// ���ʼ�֪ͨ(��ֵ)
		EmailInfo tEailInfo(8);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}

	AddRechargeData(wRechargeId);

	// �Ƿ�ʱ��,�Ѿ����ڲ�����
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// ���ճ�ֵ�
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// ����ǩ����ֵ
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// 7���¼�(��ֵ�)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// ͬ�����ݵ��ͻ���
	SendRechargeData();

	// ��ֵ��¼
	SaveRechargeRecord(wRechargeId, callbackData.wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
		callbackData.szPayTime, callbackData.szSDKOrderId, callbackData.wFrom);

	return true;
}

// �¿�����ص�
bool CRechargeManager::OnNetMonthCardBuyCallback(const RechargeCallbackData &callbackData)
{
	WORD wCardId = callbackData.wRechargeId;

	// ȡ����
	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
	if (!pMonthCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[CardId:%d]", __FUNCTION__, wCardId);
		return false;
	}

	int nAmount = callbackData.wCurrencyType ? pMonthCardCfg->nSellPrice_USD : pMonthCardCfg->nSellPrice;

	// ����ֵ���
	if (!CheckRechargeResult(nAmount, callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ���ʧ��[RechargeId:%d,RechargeType:%d]",
			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
		return false;
	}


	// ��ֵ����
	m_pPlayer->GetVipManager()->AddExp(pMonthCardCfg->nVipExp);

	// ��������������ʯ
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pMonthCardCfg->nBuyRewardDiamond, TRUE, FROM_TOP_UP);

	// �����Ʒ
	if (pMonthCardCfg->tRewardItem.dwId && pMonthCardCfg->tRewardItem.dwNum)
		m_pPlayer->AddObjectData(&pMonthCardCfg->tRewardItem, TRUE, FROM_RECHARGE_MONTH_CARD);

	// ��ӹ����¿�����
	MonthCard tCard;
	tCard.wCardId = wCardId;
	tCard.dwLastTakeRewardTime = 0;
	tCard.dwBuyTime = time(NULL);
	AddMonthCardData(&tCard);

	// ���ʼ�֪ͨ(��ֵ)
	WORD wEmailInfoId = wCardId == 1002 ? 10 : 7;
	EmailInfo tEailInfo(wEmailInfoId);
	//tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nSellPrice);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nBuyRewardDiamond);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nDailyRewardDiamond);
	EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);

	// todo::������۳�Ļʱ����
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pMonthCardCfg->nSellPrice);

	// ���ճ�ֵ�
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pMonthCardCfg->nSellPrice);

	// ����ǩ����ֵ
	//m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pMonthCardCfg->nSellPrice);

	// 7���¼�(��ֵ�)
	m_pPlayer->OnUpdateSevenDayTarget(3, pMonthCardCfg->nSellPrice, true);

	// ֪ͨ�ͻ���
	SendMonthCardData();

	// ��ֵ��¼
	SaveRechargeRecord(wCardId, callbackData.wRechargeType, pMonthCardCfg->nSellPrice, pMonthCardCfg->nBuyRewardDiamond,
		callbackData.szPayTime, callbackData.szSDKOrderId, callbackData.wFrom);

	return true;
}

// ��ֵ�ۿ۳��λص�
bool CRechargeManager::OnNetSuperDiscountCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ�ۿ۳�ֵ�ص�ʧ��[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// ��ֵ�ۿۻص�
	m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// ����ͬ������
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().SendSuperDiscountData();

	return true;
}

// �³�ֵ�ۿ۳��λص�
bool CRechargeManager::OnNetNewSuperDiscountCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ�ۿ۳�ֵ�ص�ʧ��[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// ��ֵ�ۿۻص�(��)
	m_pPlayer->GetActivityManager()->GetNewSuperDiscount().OnRechargeCallback();

	return true;
}

// ֱ���Żݻص�
bool CRechargeManager::OnNetDirectBuyCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֱ���Żݻص�ʧ��[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// ͨ����ֵ������Ʒ
	m_pPlayer->GetShopManager()->DirectBuyShopBuyByRecharge(callbackData.wGoodsId);

	// ����ͬ������
	m_pPlayer->GetShopManager()->SendDirectBuyShopData();

	return true;
}

// �����Ƽ��ص�
bool CRechargeManager::OnNetTodayRecommendCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������Ƽ���ֵ�ص�ʧ��[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	m_pPlayer->GetActivityManager()->OnTodayRecommendRechargeCallback();

	// ����ͬ������
	m_pPlayer->GetActivityManager()->SendVipGiftData();

	return true;
}

// ��׼�Ƽ��ص�
bool CRechargeManager::OnNetAccurateRecommendCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����׼�Ƽ���ֵ�ص�ʧ��[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// ��ֵ�ۿۻص�
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().OnRechargeCallback(callbackData.wGoodsId);

	// ����ͬ������
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().SendAccurateRecommendData();

	return true;
}

// ������Ʒ
bool CRechargeManager::OnNetHotSaleGoodsCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ�ص�ʧ��[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	m_pPlayer->GetActivityManager()->GetHotSaleGoods().OnRechargeNotify(callbackData.wGoodsId, callbackData.wRechargeId);

	return true;
}

// �����ֵ�ص�
bool CRechargeManager::DoRechargeCallback(const RechargeCallbackData &callbackData, bool bAddDiamond)
{
	WORD wRechargeId = callbackData.wRechargeId;

	// ȡ����
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, callbackData.byIsMyCard);
	if (!pRechargeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ֵ����[RechargeId:%d]", __FUNCTION__, wRechargeId);
		return false;
	}

	int nAmount = callbackData.wCurrencyType ? pRechargeCfg->nUSD : pRechargeCfg->nRMB;

	// ����ֵ���
	if (!CheckRechargeResult(nAmount, callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ���ʧ��[RechargeId:%d,RechargeType:%d]",
			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
		return false;
	}

	// ��ֵ����
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);

	if (bAddDiamond)
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);

	// todo::��ȷ���Ƿ��������
	// AddRechargeData(wRechargeId);

	// �Ƿ�ʱ��,�Ѿ����ڲ�����
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// ���ճ�ֵ�
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// ����ǩ����ֵ
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// ��ֵ�ۿۻص�
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// 7���¼�(��ֵ�)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// ͬ�����ݵ��ͻ���
	SendRechargeData();

	// ��ֵ��¼
	SaveRechargeRecord(wRechargeId, callbackData.wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
		callbackData.szPayTime, callbackData.szSDKOrderId, callbackData.wFrom);

	return true;
}

// ���ͳ�ֵ����
void CRechargeManager::SendRechargeData()
{
	DeleteExpiredMonthCard();

	MSG_S2C_RECHARGE_DATA_RESP msg;
	msg.m_pRechargetData = m_pRechargeData;
	m_pPlayer->SendPacket(&msg);
}

// �����¿�����
void CRechargeManager::SendMonthCardData()
{
	DeleteExpiredMonthCard();

	MSG_S2C_MONTH_CARD_DATA_RESP msg;
	msg.m_pRechargetData = m_pRechargeData;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ�¿�����
void CRechargeManager::OnNetTakeMonthCardReward(WORD wCardId)
{
	// ȡ����
	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
	if (!pMonthCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����¿�����[CardId:%d]", __FUNCTION__, wCardId);
		return;
	}

	// ɾ�����ڵ��¿�
	DeleteExpiredMonthCard();

	// ����Ƿ��ѹ����
	MonthCard *pMonthCard = GetMonthCard(wCardId);
	if (!pMonthCard)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���¿���û����,������ȡ����[CardId:%d]", __FUNCTION__, wCardId);
		return;
	}

	DWORD dwNowTime = time(NULL);

	// �������Ƿ�����ȡ��
	if (IsTheSameDay(pMonthCard->dwLastTakeRewardTime, dwNowTime))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�¿���������ȡ��[CardId:%d]", __FUNCTION__, wCardId);
		return;
	}

	// ������
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pMonthCardCfg->nDailyRewardDiamond, TRUE, FROM_GET_MONTH_CARD_REWARD);

	// ������ȡ����ʱ��
	pMonthCard->dwLastTakeRewardTime = dwNowTime;

	// ����ͬ������
	SendMonthCardData();

	// ��Ӧ
	MSG_S2C_TAKE_MONTH_CARD_REWARD_RESP msg;
	msg.m_wCardId = wCardId;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ��ʱ��ֵ����
void CRechargeManager::OnNetTakeLimitRechargeReward(WORD wDataId)
{
	
}

// ���󶩵���
void CRechargeManager::OnNetOrderId(BYTE byRechargeType, WORD wRechargeId, DWORD dwContext)
{
	WORD wFromMsgId = C2S_SDK_ORDER_ID_REQ;

	WORD wChannelType = m_pPlayer->GetAccountInfo()->GetChannelUID();

	DWORD dwCurTime = time(NULL);

	// ���ʱ����(1��)
	if (dwCurTime <= m_dwLastRequestOrderTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]���󶩵���̫��Ƶ��[id:%d]", __FUNCTION__, wRechargeId);
		return;
	}


	// ��ֵ����(1=�¿�,2=��ͨ��ֵ,3=��ֵ�ۿ�)
	if (RECHARGE_MONTH_CARD == byRechargeType)
	{
		const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wRechargeId);
		if (!pMonthCardCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����¿�����[id:%d]", __FUNCTION__, wRechargeId);
			return;
		}
	}
	else
	{
		const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId);
		if (!pRechargeCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ֵ����[id:%d]", __FUNCTION__, wRechargeId);
			return;
		}
	}

	MSG_S2C_SDK_ORDER_ID_RESP msg;
	memset(msg.m_szOrderId, 0, sizeof(msg.m_szOrderId));
	memset(msg.m_szSign, 0, sizeof(msg.m_szSign));
	msg.m_byRechargeType = byRechargeType;
	msg.m_wRechargeId = wRechargeId;
	msg.m_dwContext = dwContext;
	memset(m_szOrderId, 0, sizeof(m_szOrderId));
	// cp��������Ϲ��� role_id + time + series_id
	sprintf_s(m_szOrderId, sizeof(m_szOrderId), "%d", dwCurTime);

	m_dwLastRequestOrderTime = dwCurTime;
	m_dwContext = dwContext;
	
	// ��Ӧ
	strcpy_s(msg.m_szOrderId, sizeof(msg.m_szOrderId), m_szOrderId);
	m_pPlayer->SendPacket(&msg);

	MAPLOG(GUI_LEVEL, "[%s]���󶩵���[role_id:%d,recharge_id:%d,recharge_type:%d]",
		__FUNCTION__, m_pPlayer->GetRoleId(), wRechargeId, byRechargeType);
}

// ��ֵ�ɹ��ϱ�
void CRechargeManager::OnNetRechargeSuccessReport(WORD wRechargeId, DWORD dwChannel, const char *pszIP, const char *pszOrderId)
{
	//bool bReport = g_GetConfigValue(202);
	if (!g_MapServer.IsReportRecharge())
	{
		MAPLOG(GUI_LEVEL, "����Ҫ�ϱ���ֵ�ɹ�����");
		return;
	}

	MAPLOG(GUI_LEVEL, "��ֵ�ɹ��ϱ�����[RechargeId:%d,Channel:%d,IP:%s,OrderId:%s]", wRechargeId, dwChannel, pszIP, pszOrderId);

	// ��Ӧ
// 	MSG_S2C_RECHARGE_SUCCESS_REPORT_RESP resp_msg;
// 	m_pPlayer->SendPacket(&resp_msg);


	// ͨ��openserver�ϱ�
	MSG_M2O_RECHARGE_SUCCESS_REPORT_NOTIFY msg;
	memset(&msg.m_ReportData, 0, sizeof(msg.m_ReportData));
	msg.m_ReportData.dwRoleId = m_pPlayer->GetRoleId();
	strcpy_s(msg.m_ReportData.szRoleName, sizeof(msg.m_ReportData.szRoleName), m_pPlayer->GetUTF8RoleName());
	strcpy_s(msg.m_ReportData.szAccountId, sizeof(msg.m_ReportData.szAccountId), m_pPlayer->GetAccID());
	msg.m_ReportData.wRoleLevel = m_pPlayer->GetLevel();
	msg.m_ReportData.wVipLevel = m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);
	msg.m_ReportData.dwVipExp = m_pPlayer->GetProperty(ROLE_PRO_VIP_EXP);
	strcpy_s(msg.m_ReportData.szOrderId, sizeof(msg.m_ReportData.szOrderId), pszOrderId);
	msg.m_ReportData.dwIapId = wRechargeId;
	msg.m_ReportData.dwChannel = dwChannel;
	msg.m_ReportData.dwServerId = m_pPlayer->GetServerId();
	//msg.m_ReportData.dwVirtualCurrencyAmount = m_pPlayer->GetProperty(ROLE_PRO_DIAMOND);
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId);
	const MonthCard_Config *pCardCfg = g_pCfgMgr->GetMonthCard(wRechargeId);
	if (pRechargeCfg)
	{
		msg.m_ReportData.dwCurrencyAmount = pRechargeCfg->nRMB;
		msg.m_ReportData.dwVirtualCurrencyAmount = pRechargeCfg->nRechargeDiamond;
		if (!GetRechargeTimes(wRechargeId))
			msg.m_ReportData.dwVirtualCurrencyAmountEx = pRechargeCfg->nFirstRechargeDiamond;
	}
	if (pCardCfg)
	{
		msg.m_ReportData.dwCurrencyAmount = pCardCfg->nSellPrice;
		msg.m_ReportData.dwVirtualCurrencyAmount = pCardCfg->nBuyRewardDiamond;
	}
	strcpy_s(msg.m_ReportData.szIP, sizeof(msg.m_ReportData.szIP), pszIP);

	g_MapServer.GetOpenSession()->SendPacket(m_pPlayer->GetGuid(), msg.GetType(), &msg);
}

// ���󶩵���
void CRechargeManager::OnNetOrderIdFromYD(DWORD dwGuid)
{
	DWORD dwCurTime = time(NULL);

	MSG_M2O_CP_ORDER_ID_RESP resp;
	resp.m_dwRoleId = m_pPlayer->GetRoleId();
	resp.m_dwOrderId = 0;

	// ���ʱ����(1��)
	if (dwCurTime <= m_dwLastRequestOrderTime)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���󶩵���̫��Ƶ��[RoleId:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
	}
	else
	{
		m_dwLastRequestOrderTime = dwCurTime;
		resp.m_dwOrderId = dwCurTime;
	}

	g_MapServer.GetOpenSession()->SendPacket(dwGuid, resp.GetType(), &resp);
}

// ��ӳ�ֵ����
void CRechargeManager::AddRechargeData(WORD wRechargeId)
{
	CommonRecharge *pRecharge = FindCommonRecharge(wRechargeId);
	if (pRecharge)
	{
		pRecharge->wRechargeTimes++;
	}
	else
	{
		if (m_pRechargeData->wRechargeNum >= MAX_RECHARGE_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ֵ��¼�б�����", __FUNCTION__);
			return;
		}

		CommonRecharge tRecharge;
		tRecharge.wRechargeId = wRechargeId;
		tRecharge.wRechargeTimes = 1;
		m_pRechargeData->arRechargeList[m_pRechargeData->wRechargeNum++] = tRecharge;
	}
}

// ����¿�����
void CRechargeManager::AddMonthCardData(const MonthCard *pMonthCard)
{
	MonthCard *pCard = GetMonthCard(pMonthCard->wCardId);
	if (pCard)
	{
		*pCard = *pMonthCard;
	}
	else 
	{
		if (m_pRechargeData->wMontchCardNum >= MAX_MONTH_CAR_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�¿��б�����", __FUNCTION__);
			return;
		}
		m_pRechargeData->arMonthCardList[m_pRechargeData->wMontchCardNum++] = *pMonthCard;
	}
}

// ȡ��ֵ����
WORD CRechargeManager::GetRechargeTimes(WORD wRechargeId)
{
	CommonRecharge *pRecharge = FindCommonRecharge(wRechargeId);
	return pRecharge ? pRecharge->wRechargeTimes : 0;
}

// �Ƿ��к��
bool CRechargeManager::IsHaveRedPoint(WORD wSystemId)
{
	DWORD dwNowTime = time(NULL);

	// ���¿��п���ȡ����ʱ
	if (SYSTEM_MONTH_CARD == wSystemId)
	{
		for (WORD i = 0; i < m_pRechargeData->wMontchCardNum; ++i)
		{
			MonthCard &tCard = m_pRechargeData->arMonthCardList[i];

			// �������Ƿ�����ȡ��
			if (!IsTheSameDay(tCard.dwLastTakeRewardTime, dwNowTime))
			{
				// ����¿��Ƿ��ѹ���
				const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(tCard.wCardId);
				if (pMonthCardCfg)
				{
					// ���ÿ�
					if (!pMonthCardCfg->nValidTime)
						return true;

					// �¿�
					if (GetPassDays(tCard.dwBuyTime, dwNowTime) <= pMonthCardCfg->nValidTime)
						return true;
				}
			}
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ĺ����ʾ����[SystemId:%d]", __FUNCTION__, wSystemId);
		return false;
	}

	return false;
}

// ɾ�������¿�
void CRechargeManager::DeleteExpiredMonthCard()
{
	MonthCard arMonthCardList[MAX_MONTH_CAR_NUM];
	WORD wMontchCardNum = 0;

	// ������ȡ��û���ڵĿ�
	DWORD dwNowTime = time(NULL);
	for (WORD i = 0; i < m_pRechargeData->wMontchCardNum; ++i)
	{
		MonthCard &tMonthCard = m_pRechargeData->arMonthCardList[i];
		const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(tMonthCard.wCardId);
		if (!pMonthCardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����¿�����[CardId:%d]", __FUNCTION__, tMonthCard.wCardId);
			return;
		}

		// ������ �� ��û����
		if (!pMonthCardCfg->nValidTime || (pMonthCardCfg->nValidTime && GetPassDays(tMonthCard.dwBuyTime, dwNowTime) <= pMonthCardCfg->nValidTime))
		{
			arMonthCardList[wMontchCardNum++] = tMonthCard;
		}
	}

	// ���¸�ֵ
	m_pRechargeData->wMontchCardNum = wMontchCardNum;
	int nCopySize = wMontchCardNum * sizeof(MonthCard);
	memcpy_s(m_pRechargeData->arMonthCardList, nCopySize, arMonthCardList, nCopySize);
}

// �Ƿ��ֵ������¼
bool CRechargeManager::IsOrderRecord(DWORD dwOrderId)
{
	for (WORD i = 0; i < m_pRechargeData->wCPOrderRecordNum; ++i)
	{
		if (m_pRechargeData->arCPOrderRecordList[i] == dwOrderId)
			return true;
	}
	return false;
}

// ��Ӷ�����¼
void CRechargeManager::AddOrderRecord(DWORD dwOrderId)
{
	if (m_pRechargeData->wCPOrderRecordNum >= MAX_ORDER_RECORD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������¼�б�����[role_id:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return;
	}

	m_pRechargeData->arCPOrderRecordList[m_pRechargeData->wCPOrderRecordNum++] = dwOrderId;
}

// �洢��ֵ��¼
void CRechargeManager::SaveRechargeRecord(WORD wRechargeId, WORD wRechargeType, int nAmount, int nRechargeDiamond, const char *pszPayTime, const char *pszSDKOrderId, WORD wFrom)
{
	// ��ֵ��¼
	RoleRechargeRecord_Save* pQuery = RoleRechargeRecord_Save::ALLOC();
	pQuery->SetIndex(DB_ROLE_RECHARGE_RECORD_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	RechargeRecord tRechargeRecord;
	tRechargeRecord.dwRoleId = m_pPlayer->GetRoleId();
	tRechargeRecord.wRoleLevel = m_pPlayer->GetLevel();
	tRechargeRecord.wServerId = m_pPlayer->GetServerId();
	tRechargeRecord.wRechargeId = wRechargeId;
	tRechargeRecord.wRechargeType = wRechargeType;
	//tRechargeRecord.wCardId = 0;
	tRechargeRecord.wFrom = wFrom;
	tRechargeRecord.dwAmount = nAmount;
	tRechargeRecord.dwDiamond = nRechargeDiamond;
	tRechargeRecord.dwRecordTime = time(NULL);
	strcpy_s(tRechargeRecord.szPayTime, sizeof(tRechargeRecord.szPayTime), pszPayTime);
	strcpy_s(tRechargeRecord.szSDKOrderId, sizeof(tRechargeRecord.szSDKOrderId), pszSDKOrderId);
	pQuery->SetRecord(tRechargeRecord);
	g_MapServer.MapDBQuery(pQuery);
}

// ���AppleIapID
bool CRechargeManager::CheckAppleIapId(const RechargeCallbackData &callbackData)
{
	const AppleIap_Config *pIapCfg = g_pCfgMgr->GetAppleIap(callbackData.szRechargeStrId);
	if (!pIapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���AppleIap����[RechargeId:%d, AppleIapId:%s]", 
			__FUNCTION__, callbackData.wRechargeId, callbackData.szRechargeStrId);
		return false;
	}

	// ����ֵID�Ƿ�һ��
	if (pIapCfg->dwRechargeId != callbackData.wRechargeId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ֵ�ִ�ID�쳣[role_id:%d,order_id:%s,config_recharge_id:%d,apple_iap_id:%s,is_mycard:%d,recharge_id:%d,currency_type:%d]",
			__FUNCTION__, m_pPlayer->GetRoleId(), callbackData.szSDKOrderId, pIapCfg->dwRechargeId, callbackData.szRechargeStrId,
			callbackData.byIsMyCard, callbackData.wRechargeId, callbackData.wCurrencyType);
		return false;
	}

	return true;
}

// ����ֵ���
bool CRechargeManager::CheckRechargeResult(int nCfgAmount, const RechargeCallbackData &callbackData)
{
	// ����Ʒ
	if (callbackData.byIsNotCheck)
	{
		// todo::������־����
		MAPLOG(DEBUG_LEVEL, "[%s]����Ʒ��ֵ[RoleId:%d, RechargeId:%d, Amount:%d,SdkOrderId:%s]",
			__FUNCTION__, callbackData.dwRoleId, callbackData.wRechargeId, callbackData.nAmount, callbackData.szSDKOrderId);
	}
	// ����ֵ�ִ�ID
	else if (callbackData.byIsCheckRechargeStrId)
	{
		const AppleIap_Config *pIapCfg = g_pCfgMgr->GetAppleIap(callbackData.szRechargeStrId);
		if (!pIapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]cannt found AppleIap config[RechargeId:%d, AppleIapId:%s]",
				__FUNCTION__, callbackData.wRechargeId, callbackData.szRechargeStrId);
			return false;
		}

		// ����ֵID�Ƿ�һ��
		if (pIapCfg->dwRechargeId != callbackData.wRechargeId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]recharge string id erorr[role_id:%d,order_id:%s,config_recharge_id:%d,apple_iap_id:%s,is_mycard:%d,recharge_id:%d,currency_type:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), callbackData.szSDKOrderId, pIapCfg->dwRechargeId, callbackData.szRechargeStrId,
				callbackData.byIsMyCard, callbackData.wRechargeId, callbackData.wCurrencyType);
			return false;
		}
	}
	// ����ֵ���
	else
	{
		// ��֤Ҫ�ӵĽ��
		if (nCfgAmount != callbackData.nAmount)
		{
			MAPLOG(ERROR_LEVEL, "[%s]recharge amount erorr[order_id:%s,config_amount:%d,sdk_amount:%d,role_id:%d]",
				__FUNCTION__, callbackData.szSDKOrderId, nCfgAmount, callbackData.nAmount, m_pPlayer->GetRoleId());
			return false;
		}
	}

	return true;
}

// �Ƿ�����׳�
bool CRechargeManager::IsTodayFirstRecharge()
{
	return !m_pRechargeData->wTodayRechargetTimes ? true : false;
}

// �Ƿ��׳�
bool CRechargeManager::IsFirstRecharge()
{
	return (!m_pRechargeData->wRechargeNum && !m_pRechargeData->wMontchCardNum) ? true : false;
}

// �¿�����ص�
bool CRechargeManager::DoMonthCard(WORD wCardId)
{
	// ȡ����
	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
	if (!pMonthCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[CardId:%d]", __FUNCTION__, wCardId);
		return false;
	}

	int nAmount = pMonthCardCfg->nSellPrice;

	// ����ֵ���
// 	if (!CheckRechargeResult(nAmount, callbackData))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]����ֵ���ʧ��[RechargeId:%d,RechargeType:%d]",
// 			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
// 		return false;
// 	}


	// ��ֵ����
	m_pPlayer->GetVipManager()->AddExp(pMonthCardCfg->nVipExp);

	// ��������������ʯ
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pMonthCardCfg->nBuyRewardDiamond, TRUE, FROM_TOP_UP);

	// �����Ʒ
	if (pMonthCardCfg->tRewardItem.dwId && pMonthCardCfg->tRewardItem.dwNum)
		m_pPlayer->AddObjectData(&pMonthCardCfg->tRewardItem, TRUE, FROM_RECHARGE_MONTH_CARD);

	// ��ӹ����¿�����
	MonthCard tCard;
	tCard.wCardId = wCardId;
	tCard.dwLastTakeRewardTime = 0;
	tCard.dwBuyTime = time(NULL);
	AddMonthCardData(&tCard);

	// ���ʼ�֪ͨ(��ֵ)
	WORD wEmailInfoId = wCardId == 1002 ? 10 : 7;
	EmailInfo tEailInfo(wEmailInfoId);
	//tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nSellPrice);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nBuyRewardDiamond);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nDailyRewardDiamond);
	EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);

	// todo::������۳�Ļʱ����
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pMonthCardCfg->nSellPrice);

	// ���ճ�ֵ�
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pMonthCardCfg->nSellPrice);

	// ����ǩ����ֵ
	//m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pMonthCardCfg->nSellPrice);

	// 7���¼�(��ֵ�)
	m_pPlayer->OnUpdateSevenDayTarget(3, pMonthCardCfg->nSellPrice, true);

	// ֪ͨ�ͻ���
	SendMonthCardData();

	// ��ֵ��¼
// 	SaveRechargeRecord(wCardId, callbackData.wRechargeType, pMonthCardCfg->nSellPrice, pMonthCardCfg->nBuyRewardDiamond,
// 		callbackData.szPayTime, callbackData.szSDKOrderId);

	return true;
}

// ��ͨ��ֵ
bool CRechargeManager::DoCommonRecharge(const Recharge_Config *pRechargeCfg)
{
// 	WORD wRechargeId = callbackData.wRechargeId;
// 
// 	// ȡ����
// 	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, callbackData.byIsMyCard);
// 	if (!pRechargeCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[RechargeId:%d]", __FUNCTION__, wRechargeId);
// 		return false;
// 	}

	int nAmount = pRechargeCfg->nRMB;
	WORD wRechargeId = pRechargeCfg->wId;

	// ����ֵ���
// 	if (!CheckRechargeResult(nAmount, callbackData))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]����ֵ���ʧ��[RechargeId:%d,RechargeType:%d]",
// 			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
// 		return false;
// 	}

	// ��ֵ����
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nExtraDiamond, TRUE, FROM_TOP_UP);


// 	MAPLOG(ERROR_LEVEL, "[%s]recharge notify[RoleId:%d, RechargeId:%d, RechargeType:%d, VipExp:%d, RechargeDiamond:%d, ExtraDiamond:%d]",
// 		__FUNCTION__, m_pPlayer->GetRoleId(), wRechargeId, callbackData.wRechargeType, pRechargeCfg->nVipExp,
// 		pRechargeCfg->nRechargeDiamond, pRechargeCfg->nExtraDiamond);

	// �׳佱��
	if (!FindCommonRecharge(wRechargeId))
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nFirstRechargeDiamond, TRUE, FROM_TOP_UP);

		// ���ʼ�֪ͨ(��ֵ)
		EmailInfo tEailInfo(6);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nFirstRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}
	else
	{
		// ���ʼ�֪ͨ(��ֵ)
		EmailInfo tEailInfo(8);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}

	AddRechargeData(wRechargeId);

	// �Ƿ�ʱ��,�Ѿ����ڲ�����
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
	// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// ���ճ�ֵ�
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// ����ǩ����ֵ
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// 7���¼�(��ֵ�)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// ͬ�����ݵ��ͻ���
	SendRechargeData();

	// ��ֵ��¼
// 	SaveRechargeRecord(wRechargeId, callbackData.wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
// 		callbackData.szPayTime, callbackData.szSDKOrderId);

	return true;
}

// ��ֵ�ۿ۳��λص�(��ͨ���߼�)
bool CRechargeManager::DoNetSuperDiscount(const Recharge_Config *pRechargeCfg)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ�ۿ۳�ֵ�ص�ʧ��", __FUNCTION__);
		return false;
	}

	// ��ֵ�ۿۻص�
	m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// ����ͬ������
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().SendSuperDiscountData();

	return true;
}

// �³�ֵ�ۿ۳��λص�
bool CRechargeManager::DoNetNewSuperDiscount(const Recharge_Config *pRechargeCfg)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֵ�ۿ۳�ֵ�ص�ʧ��", __FUNCTION__);
		return false;
	}

	// ��ֵ�ۿۻص�(��)
	m_pPlayer->GetActivityManager()->GetNewSuperDiscount().OnRechargeCallback();

	return true;
}

// ֱ���Żݻص�
bool CRechargeManager::DoDirectBuy(const Recharge_Config *pRechargeCfg, WORD wGoodsId)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֱ���Żݻص�ʧ��", __FUNCTION__);
		return false;
	}

	// ͨ����ֵ������Ʒ
	m_pPlayer->GetShopManager()->DirectBuyShopBuyByRecharge(wGoodsId);

	// ����ͬ������
	m_pPlayer->GetShopManager()->SendDirectBuyShopData();

	return true;
}

// �����Ƽ��ص�
bool CRechargeManager::DoTodayRecommend(const Recharge_Config *pRechargeCfg)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������Ƽ���ֵ�ص�ʧ��", __FUNCTION__);
		return false;
	}

	m_pPlayer->GetActivityManager()->OnTodayRecommendRechargeCallback();

	// ����ͬ������
	m_pPlayer->GetActivityManager()->SendVipGiftData();

	return true;
}

// ��׼�Ƽ��ص�
bool CRechargeManager::DoAccurateRecommend(const Recharge_Config *pRechargeCfg, WORD wGoodsId)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����׼�Ƽ���ֵ�ص�ʧ��", __FUNCTION__);
		return false;
	}

	// ��ֵ�ۿۻص�
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().OnRechargeCallback(wGoodsId);

	// ����ͬ������
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().SendAccurateRecommendData();

	return true;
}

// ������Ʒ
bool CRechargeManager::DoHotSaleGoods(const Recharge_Config *pRechargeCfg, WORD wGoodsId)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ�ص�ʧ��", __FUNCTION__);
		return false;
	}

	m_pPlayer->GetActivityManager()->GetHotSaleGoods().OnRechargeNotify(wGoodsId, pRechargeCfg->wId);

	return true;
}

// �����ֵ�ص�
bool CRechargeManager::DoRechargeCallbackEx(const Recharge_Config *pRechargeCfg)
{
	WORD wRechargeId = pRechargeCfg->wId;

	int nAmount = pRechargeCfg->nRMB;

	// ����ֵ���
// 	if (!CheckRechargeResult(nAmount, callbackData))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]����ֵ���ʧ��[RechargeId:%d,RechargeType:%d]",
// 			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
// 		return false;
// 	}

	// ��ֵ����
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);

	//if (bAddDiamond)
	//	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);

	// todo::��ȷ���Ƿ��������
	// AddRechargeData(wRechargeId);

	// �Ƿ�ʱ��,�Ѿ����ڲ�����
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
	// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// ���ճ�ֵ�
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// ����ǩ����ֵ
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// ��ֵ�ۿۻص�
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// 7���¼�(��ֵ�)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// ͬ�����ݵ��ͻ���
	SendRechargeData();

	// ��ֵ��¼
// 	SaveRechargeRecord(wRechargeId, pRechargeCfg->wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
// 		callbackData.szPayTime, callbackData.szSDKOrderId);

	return true;
}

// ȡ�¿�����
MonthCard* CRechargeManager::GetMonthCard(WORD wMonthCardId)
{
	for (WORD i = 0; i < m_pRechargeData->wMontchCardNum; ++i)
	{
		if (m_pRechargeData->arMonthCardList[i].wCardId == wMonthCardId)
			return &m_pRechargeData->arMonthCardList[i];
	}
	return NULL;
}

// ȡ��ͨ��ֵ����
CommonRecharge* CRechargeManager::FindCommonRecharge(WORD wRechargeId)
{
	for (WORD i = 0; i < m_pRechargeData->wRechargeNum; ++i)
	{
		if (m_pRechargeData->arRechargeList[i].wRechargeId == wRechargeId)
			return &m_pRechargeData->arRechargeList[i];
	}
	return NULL;
}

// ��ȡ��ֵ���
DWORD CRechargeManager::GetRechargeAmount(WORD wRechargeId, BYTE byIsMyCard)
{
	DWORD dwAmount = 0;
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, byIsMyCard);
	if (pRechargeCfg)
	{
		dwAmount = pRechargeCfg->nRMB;
	}
	else
	{
		const MonthCard_Config *pCardCfg = g_pCfgMgr->GetMonthCard(wRechargeId);
		if (pCardCfg)
			dwAmount = pCardCfg->nSellPrice;
	}
	return dwAmount;
}

// ��ȡ��ֵ���
DWORD CRechargeManager::GetRechargeDiamond(WORD wRechargeId, BYTE byIsMyCard)
{
	DWORD dwDiamond = 0;
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, byIsMyCard);
	if (pRechargeCfg)
	{
		dwDiamond = pRechargeCfg->nRechargeDiamond;
	}
	else
	{
		const MonthCard_Config *pCardCfg = g_pCfgMgr->GetMonthCard(wRechargeId);
		if (pCardCfg)
			dwDiamond = pCardCfg->nBuyRewardDiamond;
	}
	return dwDiamond;
}