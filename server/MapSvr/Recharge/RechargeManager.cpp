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

// 请求充值数据
void CRechargeManager::OnNetRechargeData()
{
	SendRechargeData();
}

// 充值请求
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

	// 取充值额度
	callbackData.nAmount = GetRechargeAmount(wRechargeId);

	OnNetRechargeCallback(callbackData);
}

// 充值请求(购买)
void CRechargeManager::OnNetRechargeEx(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId, DWORD dwContext)
{
	MAPLOG(GUI_LEVEL, "购买充值[RoleId:%d,RechargeId:%d,RechargeType:%d,GoodsId:%d,Context:%d]",
		m_pPlayer->GetRoleId(), wRechargeId, wRechargeType, wGoodsId, dwContext);

	// 检测是否越南版本
	if (!g_MapServer.IsYGame())
	{
		MAPLOG(ERROR_LEVEL, "此接口只对YGame有效");
		return;
	}

	// 取充值配置
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId);
	if (!pRechargeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "找不到充值配置信息[RechargeId:%d]", wRechargeId);
		return;
	}

	// 检测YCion是否足够
	if (m_pPlayer->GetProperty(ROLE_PRO_YCOIN) < pRechargeCfg->nYCoin)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "ycoin不足,不能购买[RechargeId:%d,CostYCoin:%d]", wRechargeId, pRechargeCfg->nYCoin);
		return;
	}

	bool bIsFirstRecharge = IsFirstRecharge();

	bool bResult = false;

	WORD wType = wRechargeType;
	// 1=月卡,2=充值,3=超值折扣,4=直购特惠
	switch (wType)
	{
		// 月卡
// 	case RECHARGE_MONTH_CARD:
// 		bResult = DoMonthCard(wRechargeId);
// 		break;
		// 普通充值
	case RECHARGE_COMMON:
		bResult = DoCommonRecharge(pRechargeCfg);
		break;
		// 超极折扣
	case RECHARGE_SUPER_DISCOUNT:
		bResult = DoNetSuperDiscount(pRechargeCfg);
		break;
		// 新超极折扣
	case RECHARGE_NEW_SUPER_DISCOUNT:
		bResult = DoNetNewSuperDiscount(pRechargeCfg);
		break;
		// 直购特惠
	case RECHARGE_DIRECT_BUY_DISCONT:
		bResult = DoDirectBuy(pRechargeCfg, wGoodsId);
		break;
		// 今日推荐
	case RECHARGE_TODAY_RECOMMEND:
		bResult = DoTodayRecommend(pRechargeCfg);
		break;
		// 精准推荐
	case RECHARGE_ACCURATE_RECOMMEND:
		bResult = DoAccurateRecommend(pRechargeCfg, wGoodsId);
		break;
		// 热卖商品
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
		MAPLOG(ERROR_LEVEL, "处理充值逻辑失败[role_id:%d, recharge_type:%d, recharge_id:%d]",m_pPlayer->GetRoleId(), wType, wRechargeId);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_YCOIN, -pRechargeCfg->nYCoin, TRUE, FROM_BUY_RECHARGE);

	//AddOrderRecord(callbackData.dwCPOrderId);

	// 上报充值记录(云顶KeyData)
	//OnNetRechargeSuccessReport(callbackData.wRechargeId, callbackData.dwYDChannelId, callbackData.szIP, callbackData.szSDKOrderId);

	// 充值回馈活动
	m_pPlayer->GetActivityManager()->GetRechargeReward().OnRechargeCallback(wRechargeId);

	DWORD dwAmount = GetRechargeAmount(wRechargeId);

	// 记录充值额度
	m_pPlayer->AddRechargeAmount(dwAmount);

	// 累充奖励
	m_pPlayer->GetActivityManager()->GetTotalRechargeReward().OnRechargeNotify(dwAmount);

	// 热云充值统计
	//RechargeCallbackData callbackData;
	//CGameLogic::Instance()->OnRechargeStatistics(callbackData, m_pPlayer);

	// 游戏数据统计
	CGameLogic::Instance()->OnGameStatistics_Recharge(m_pPlayer, dwAmount, IsTodayFirstRecharge(), bIsFirstRecharge);

	RoleRechargeData &tRechargetData = m_pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeData;
	tRechargetData.wTodayRechargetTimes++;


	// 充值事件通知客户端
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

	// 充值记录
	SaveRechargeRecord(wRechargeId, wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond, szPayTime, szSDKOrderId, RECHARGE_FROM_NORMAL);

	// 热云充值统计
	RechargeCallbackData callbackData;
	memset(&callbackData, 0, sizeof(RechargeCallbackData));
	strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), szSDKOrderId);
	callbackData.dwRoleId = m_pPlayer->GetRoleId();
	callbackData.wRechargeId = wRechargeId; 
	callbackData.wRechargeType = wRechargeType;
	CGameLogic::Instance()->OnRechargeStatistics(callbackData, m_pPlayer);

	// todo::响应
	MSG_S2C_RECHARGE_RESP msg;
	msg.m_wRechargeId = wRechargeId;
	msg.m_wRechargeType = wRechargeType;
	msg.m_wGoodsId = wGoodsId;
	msg.m_dwContext = dwContext;
	m_pPlayer->SendPacket(&msg);
}

// 请求月卡数据
void CRechargeManager::OnNetMonthCardData()
{
	SendMonthCardData();
}

// 购买月卡
void CRechargeManager::OnNetMonthCardBuy(WORD wCardId)
{
// 	DeleteExpiredMonthCard();
// 
// 	// 取配置
// 	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
// 	if (!pMonthCardCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到月卡配置[CardId:%d]", __FUNCTION__, wCardId);
// 		return;
// 	}
// 
// 	// 检测是否已购买过
// 	if (GetMonthCard(wCardId))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]已经购买过此月卡[CardId:%d]", __FUNCTION__, wCardId);
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

// 充值请求回调
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

		// 插入数据库
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
		// 提示充值成功
		MAPLOG(GUI_LEVEL, "处理充值成功[role_id:%d,amount:%d,order_id:%s,is_online:%d]",
			dwRoleId, callbackData.nAmount, callbackData.szSDKOrderId, bIsOnline);

		// 发货响应
		if (bIsNeedResp)
		{
			MSG_M2O_RECHARGE_SEND_GOODS_RESP msg;
			g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "处理充值失败[role_id:%d,amount:%d,order_id:%s,is_online:%d]",
			dwRoleId, callbackData.nAmount, callbackData.szSDKOrderId, bIsOnline);
	}
}

// YGame充值请求回调
void CRechargeManager::OnNetRechargeCallbackForYGame(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nAmount, const char *pszPaymentId)
{
	const YGamePayment_Config *pPaymentCfg = g_pCfgMgr->GetYGamePayment(pszPaymentId);
	if (!pPaymentCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到ygame支付配置[role_id:%d,payment_id:%s]",dwRoleId, pszPaymentId);
		return;
	}

	wServerId = g_MapServer.GetCurServerId(wServerId);

	// 游戏内sdk充值不需要处理, ygame平台获得ycoin
	if (1 != pPaymentCfg->nPaymentFromType)
	{
		RewardData tRewardData;
		tRewardData.wInfoId = 100;
		tRewardData.dwTime = time(NULL);
		tRewardData.dwValue1 = pPaymentCfg->dwAssistId;
		CRewardCenterManager::AddReward(wServerId, dwRoleId, tRewardData);
	}

	// 发货响应
	MSG_M2O_YGAME_RECHARGE_SEND_GOODS_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);

	MAPLOG(DEBUG_LEVEL, "Recv ygame recharge[role_id:%d,payment_id:%s,amount:%d,payment_from_type:%d]", 
		dwRoleId, pszPaymentId, nAmount, pPaymentCfg->nPaymentFromType);
}

// YGame提现通知
void CRechargeManager::OnNetYGameCashbackNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nYCoin, DWORD dwDiamond)
{
	RewardData tRewardData;
	tRewardData.wInfoId = 101;
	tRewardData.dwTime = time(NULL);
	tRewardData.dwValue1 = dwDiamond;
	CRewardCenterManager::AddReward(wServerId, dwRoleId, tRewardData);

	// 提现响应
	MSG_M2O_YGAME_CASHBACK_NOTIFY_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);

	MAPLOG(DEBUG_LEVEL, "Recv ygame cashback[role_id:%d,ycoin:%d,diamond:%d]", dwRoleId, nYCoin, dwDiamond);
}

// YGame奖励通知
void CRechargeManager::OnNetYGameBonusNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, DWORD dwDiamond)
{
	RewardData tRewardData;
	tRewardData.wInfoId = 102;
	tRewardData.dwTime = time(NULL);
	tRewardData.dwValue1 = dwDiamond;
	CRewardCenterManager::AddReward(wServerId, dwRoleId, tRewardData);

	// 提现响应
	MSG_M2O_YGAME_BONUS_NOTIFY_RESP msg;
	g_MapServer.GetOpenSession()->SendPacket(dwGuid, msg.GetType(), &msg);

	MAPLOG(DEBUG_LEVEL, "Recv ygame bonus[role_id:%d,diamond:%d]", dwRoleId, dwDiamond);
}

// 充值请求回调
bool CRechargeManager::OnNetRechargeCallback(const RechargeCallbackData &callbackData)
{
	// 充值成功,并且还没处理过
	if (!IsOrderRecord(callbackData.dwCPOrderId))
	{
		bool bIsFirstRecharge = IsFirstRecharge();

		bool bResult = false;

		//DWORD dwOldDiamond = m_pPlayer->GetProperty(ROLE_PRO_DIAMOND);

		WORD wType = callbackData.wRechargeType;
		// 1=月卡,2=充值,3=超值折扣,4=直购特惠
		switch (wType)
		{
		// 月卡
		case RECHARGE_MONTH_CARD:
			bResult = OnNetMonthCardBuyCallback(callbackData);
			break;
		// 普通充值
		case RECHARGE_COMMON:
			bResult = OnNetCommonRechargeCallback(callbackData);
			break;
		// 超极折扣
		case RECHARGE_SUPER_DISCOUNT:
			bResult = OnNetSuperDiscountCallback(callbackData);
			break;
		// 新超极折扣
		case RECHARGE_NEW_SUPER_DISCOUNT:
			bResult = OnNetNewSuperDiscountCallback(callbackData);
			break;
		// 直购特惠
		case RECHARGE_DIRECT_BUY_DISCONT:
			bResult = OnNetDirectBuyCallback(callbackData);
			break;
		// 今日推荐
		case RECHARGE_TODAY_RECOMMEND:
			bResult = OnNetTodayRecommendCallback(callbackData);
			break;
		// 精准推荐
		case RECHARGE_ACCURATE_RECOMMEND:
			bResult = OnNetAccurateRecommendCallback(callbackData);
			break;
		// 热卖商品
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
			MAPLOG(ERROR_LEVEL, "处理充值逻辑失败[role_id:%d, recharge_type:%d, recharge_id:%d, order_id:%s]",
				m_pPlayer->GetRoleId(), wType, callbackData.wRechargeId, callbackData.szSDKOrderId);
			return false;
		}

		AddOrderRecord(callbackData.dwCPOrderId);

		// 上报充值记录(云顶KeyData)
		//OnNetRechargeSuccessReport(callbackData.wRechargeId, callbackData.dwYDChannelId, callbackData.szIP, callbackData.szSDKOrderId);

		// 充值回馈活动
		m_pPlayer->GetActivityManager()->GetRechargeReward().OnRechargeCallback(callbackData.wRechargeId);

		DWORD dwAmount = GetRechargeAmount(callbackData.wRechargeId, callbackData.byIsMyCard);

		// 记录充值额度
		m_pPlayer->AddRechargeAmount(dwAmount);

		// 累充奖励
		m_pPlayer->GetActivityManager()->GetTotalRechargeReward().OnRechargeNotify(dwAmount);

		// 热云充值统计
		CGameLogic::Instance()->OnRechargeStatistics(callbackData, m_pPlayer);

		// 游戏数据统计
		CGameLogic::Instance()->OnGameStatistics_Recharge(m_pPlayer, dwAmount, IsTodayFirstRecharge(), bIsFirstRecharge);

		RoleRechargeData &tRechargetData = m_pPlayer->GetRoleFullInfo().tActivityInfo.tRechargeData;
		tRechargetData.wTodayRechargetTimes++;

		// MyCara充值上报
		if (callbackData.byIsMyCard)
			CGameLogic::Instance()->OnMyCardRechargeSuccessReport(m_pPlayer, dwAmount, callbackData.szSDKOrderId);

		// 排行榜活动
		DWORD dwDiamond = GetRechargeDiamond(callbackData.wRechargeId, callbackData.byIsMyCard); 
		m_pPlayer->GetActivityManager()->GetRechargeAndCostRankActivity().AddRechargeAmount(dwDiamond);

		// 充值事件通知客户端
		MSG_S2C_RECHARGE_NOTIFY_RESP recharge_notify_msg;
		recharge_notify_msg.m_wRechargeType = callbackData.wRechargeType;
		recharge_notify_msg.m_wRechargeId = callbackData.wRechargeId;
		recharge_notify_msg.m_dwContext = m_dwContext;
		m_pPlayer->SendPacket(&recharge_notify_msg);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "重复通知发货[role_id:%d,amount:%d,channel:%d,order_id:%s]",
			m_pPlayer->GetRoleId(), callbackData.nAmount, callbackData.dwChannel, callbackData.szSDKOrderId);
	}

	return true;
}

// 充值请求回调
bool CRechargeManager::OnNetCommonRechargeCallback(const RechargeCallbackData &callbackData)
{
	WORD wRechargeId = callbackData.wRechargeId;

	// 取配置
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, callbackData.byIsMyCard);
	if (!pRechargeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[RechargeId:%d]", __FUNCTION__, wRechargeId);
		return false;
	}

	int nAmount = callbackData.wCurrencyType ? pRechargeCfg->nUSD : pRechargeCfg->nRMB;

	// 检测充值结果
	if (!CheckRechargeResult(nAmount, callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]检测充值结果失败[RechargeId:%d,RechargeType:%d]",
			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
		return false;
	}

	// 充值奖励
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nExtraDiamond, TRUE, FROM_TOP_UP);


	MAPLOG(ERROR_LEVEL, "[%s]recharge notify[RoleId:%d, RechargeId:%d, RechargeType:%d, VipExp:%d, RechargeDiamond:%d, ExtraDiamond:%d]",
		__FUNCTION__, m_pPlayer->GetRoleId(), wRechargeId, callbackData.wRechargeType, pRechargeCfg->nVipExp,
		pRechargeCfg->nRechargeDiamond, pRechargeCfg->nExtraDiamond);

	// 首充奖励
	if (!FindCommonRecharge(wRechargeId))
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nFirstRechargeDiamond, TRUE, FROM_TOP_UP);

		// 发邮件通知(充值)
		EmailInfo tEailInfo(6);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nFirstRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}
	else
	{
		// 发邮件通知(充值)
		EmailInfo tEailInfo(8);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}

	AddRechargeData(wRechargeId);

	// 是否活动时间,已经在内部处理
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// 节日充值活动
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// 豪华签到充值
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// 7天活动事件(充值活动)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// 同步数据到客户端
	SendRechargeData();

	// 充值记录
	SaveRechargeRecord(wRechargeId, callbackData.wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
		callbackData.szPayTime, callbackData.szSDKOrderId, callbackData.wFrom);

	return true;
}

// 月卡购买回调
bool CRechargeManager::OnNetMonthCardBuyCallback(const RechargeCallbackData &callbackData)
{
	WORD wCardId = callbackData.wRechargeId;

	// 取配置
	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
	if (!pMonthCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[CardId:%d]", __FUNCTION__, wCardId);
		return false;
	}

	int nAmount = callbackData.wCurrencyType ? pMonthCardCfg->nSellPrice_USD : pMonthCardCfg->nSellPrice;

	// 检测充值结果
	if (!CheckRechargeResult(nAmount, callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]检测充值结果失败[RechargeId:%d,RechargeType:%d]",
			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
		return false;
	}


	// 充值奖励
	m_pPlayer->GetVipManager()->AddExp(pMonthCardCfg->nVipExp);

	// 购买后立即获得钻石
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pMonthCardCfg->nBuyRewardDiamond, TRUE, FROM_TOP_UP);

	// 获得物品
	if (pMonthCardCfg->tRewardItem.dwId && pMonthCardCfg->tRewardItem.dwNum)
		m_pPlayer->AddObjectData(&pMonthCardCfg->tRewardItem, TRUE, FROM_RECHARGE_MONTH_CARD);

	// 添加购买月卡数据
	MonthCard tCard;
	tCard.wCardId = wCardId;
	tCard.dwLastTakeRewardTime = 0;
	tCard.dwBuyTime = time(NULL);
	AddMonthCardData(&tCard);

	// 发邮件通知(充值)
	WORD wEmailInfoId = wCardId == 1002 ? 10 : 7;
	EmailInfo tEailInfo(wEmailInfoId);
	//tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nSellPrice);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nBuyRewardDiamond);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nDailyRewardDiamond);
	EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);

	// todo::如果是累充的活动时间则
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pMonthCardCfg->nSellPrice);

	// 节日充值活动
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pMonthCardCfg->nSellPrice);

	// 豪华签到充值
	//m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pMonthCardCfg->nSellPrice);

	// 7天活动事件(充值活动)
	m_pPlayer->OnUpdateSevenDayTarget(3, pMonthCardCfg->nSellPrice, true);

	// 通知客户端
	SendMonthCardData();

	// 充值记录
	SaveRechargeRecord(wCardId, callbackData.wRechargeType, pMonthCardCfg->nSellPrice, pMonthCardCfg->nBuyRewardDiamond,
		callbackData.szPayTime, callbackData.szSDKOrderId, callbackData.wFrom);

	return true;
}

// 超值折扣充任回调
bool CRechargeManager::OnNetSuperDiscountCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理超值折扣充值回调失败[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// 超值折扣回调
	m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// 主动同步数据
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().SendSuperDiscountData();

	return true;
}

// 新超值折扣充任回调
bool CRechargeManager::OnNetNewSuperDiscountCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理超值折扣充值回调失败[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// 超值折扣回调(新)
	m_pPlayer->GetActivityManager()->GetNewSuperDiscount().OnRechargeCallback();

	return true;
}

// 直购优惠回调
bool CRechargeManager::OnNetDirectBuyCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理直购优惠回调失败[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// 通过充值购买商品
	m_pPlayer->GetShopManager()->DirectBuyShopBuyByRecharge(callbackData.wGoodsId);

	// 主动同步数据
	m_pPlayer->GetShopManager()->SendDirectBuyShopData();

	return true;
}

// 今日推荐回调
bool CRechargeManager::OnNetTodayRecommendCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理今日推荐充值回调失败[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	m_pPlayer->GetActivityManager()->OnTodayRecommendRechargeCallback();

	// 主动同步数据
	m_pPlayer->GetActivityManager()->SendVipGiftData();

	return true;
}

// 精准推荐回调
bool CRechargeManager::OnNetAccurateRecommendCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理精准推荐充值回调失败[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	// 超值折扣回调
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().OnRechargeCallback(callbackData.wGoodsId);

	// 主动同步数据
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().SendAccurateRecommendData();

	return true;
}

// 热卖商品
bool CRechargeManager::OnNetHotSaleGoodsCallback(const RechargeCallbackData &callbackData)
{
	if (!DoRechargeCallback(callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理热卖商品回调失败[SDKOrderId:%s]", __FUNCTION__, callbackData.szSDKOrderId);
		return false;
	}

	m_pPlayer->GetActivityManager()->GetHotSaleGoods().OnRechargeNotify(callbackData.wGoodsId, callbackData.wRechargeId);

	return true;
}

// 处理充值回调
bool CRechargeManager::DoRechargeCallback(const RechargeCallbackData &callbackData, bool bAddDiamond)
{
	WORD wRechargeId = callbackData.wRechargeId;

	// 取配置
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, callbackData.byIsMyCard);
	if (!pRechargeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到充值配置[RechargeId:%d]", __FUNCTION__, wRechargeId);
		return false;
	}

	int nAmount = callbackData.wCurrencyType ? pRechargeCfg->nUSD : pRechargeCfg->nRMB;

	// 检测充值结果
	if (!CheckRechargeResult(nAmount, callbackData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]检测充值结果失败[RechargeId:%d,RechargeType:%d]",
			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
		return false;
	}

	// 充值奖励
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);

	if (bAddDiamond)
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);

	// todo::不确定是否会有问题
	// AddRechargeData(wRechargeId);

	// 是否活动时间,已经在内部处理
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// 节日充值活动
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// 豪华签到充值
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// 超值折扣回调
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// 7天活动事件(充值活动)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// 同步数据到客户端
	SendRechargeData();

	// 充值记录
	SaveRechargeRecord(wRechargeId, callbackData.wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
		callbackData.szPayTime, callbackData.szSDKOrderId, callbackData.wFrom);

	return true;
}

// 发送充值数据
void CRechargeManager::SendRechargeData()
{
	DeleteExpiredMonthCard();

	MSG_S2C_RECHARGE_DATA_RESP msg;
	msg.m_pRechargetData = m_pRechargeData;
	m_pPlayer->SendPacket(&msg);
}

// 发送月卡数据
void CRechargeManager::SendMonthCardData()
{
	DeleteExpiredMonthCard();

	MSG_S2C_MONTH_CARD_DATA_RESP msg;
	msg.m_pRechargetData = m_pRechargeData;
	m_pPlayer->SendPacket(&msg);
}

// 领取月卡奖励
void CRechargeManager::OnNetTakeMonthCardReward(WORD wCardId)
{
	// 取配置
	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
	if (!pMonthCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到月卡配置[CardId:%d]", __FUNCTION__, wCardId);
		return;
	}

	// 删除过期的月卡
	DeleteExpiredMonthCard();

	// 检测是否已购买过
	MonthCard *pMonthCard = GetMonthCard(wCardId);
	if (!pMonthCard)
	{
		MAPLOG(ERROR_LEVEL, "[%s]此月卡还没购买,不能领取奖励[CardId:%d]", __FUNCTION__, wCardId);
		return;
	}

	DWORD dwNowTime = time(NULL);

	// 检测今日是否已领取过
	if (IsTheSameDay(pMonthCard->dwLastTakeRewardTime, dwNowTime))
	{
		MAPLOG(ERROR_LEVEL, "[%s]月卡奖励已领取过[CardId:%d]", __FUNCTION__, wCardId);
		return;
	}

	// 发奖励
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pMonthCardCfg->nDailyRewardDiamond, TRUE, FROM_GET_MONTH_CARD_REWARD);

	// 更新领取奖励时间
	pMonthCard->dwLastTakeRewardTime = dwNowTime;

	// 主动同步数据
	SendMonthCardData();

	// 响应
	MSG_S2C_TAKE_MONTH_CARD_REWARD_RESP msg;
	msg.m_wCardId = wCardId;
	m_pPlayer->SendPacket(&msg);
}

// 领取限时充值奖励
void CRechargeManager::OnNetTakeLimitRechargeReward(WORD wDataId)
{
	
}

// 请求订单号
void CRechargeManager::OnNetOrderId(BYTE byRechargeType, WORD wRechargeId, DWORD dwContext)
{
	WORD wFromMsgId = C2S_SDK_ORDER_ID_REQ;

	WORD wChannelType = m_pPlayer->GetAccountInfo()->GetChannelUID();

	DWORD dwCurTime = time(NULL);

	// 检测时间间隔(1秒)
	if (dwCurTime <= m_dwLastRequestOrderTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]请求订单号太过频繁[id:%d]", __FUNCTION__, wRechargeId);
		return;
	}


	// 充值类型(1=月卡,2=普通充值,3=超值折扣)
	if (RECHARGE_MONTH_CARD == byRechargeType)
	{
		const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wRechargeId);
		if (!pMonthCardCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到月卡配置[id:%d]", __FUNCTION__, wRechargeId);
			return;
		}
	}
	else
	{
		const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId);
		if (!pRechargeCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到充值配置[id:%d]", __FUNCTION__, wRechargeId);
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
	// cp订单号组合规则 role_id + time + series_id
	sprintf_s(m_szOrderId, sizeof(m_szOrderId), "%d", dwCurTime);

	m_dwLastRequestOrderTime = dwCurTime;
	m_dwContext = dwContext;
	
	// 响应
	strcpy_s(msg.m_szOrderId, sizeof(msg.m_szOrderId), m_szOrderId);
	m_pPlayer->SendPacket(&msg);

	MAPLOG(GUI_LEVEL, "[%s]请求订单号[role_id:%d,recharge_id:%d,recharge_type:%d]",
		__FUNCTION__, m_pPlayer->GetRoleId(), wRechargeId, byRechargeType);
}

// 充值成功上报
void CRechargeManager::OnNetRechargeSuccessReport(WORD wRechargeId, DWORD dwChannel, const char *pszIP, const char *pszOrderId)
{
	//bool bReport = g_GetConfigValue(202);
	if (!g_MapServer.IsReportRecharge())
	{
		MAPLOG(GUI_LEVEL, "不需要上报充值成功数据");
		return;
	}

	MAPLOG(GUI_LEVEL, "充值成功上报数据[RechargeId:%d,Channel:%d,IP:%s,OrderId:%s]", wRechargeId, dwChannel, pszIP, pszOrderId);

	// 响应
// 	MSG_S2C_RECHARGE_SUCCESS_REPORT_RESP resp_msg;
// 	m_pPlayer->SendPacket(&resp_msg);


	// 通过openserver上报
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

// 请求订单号
void CRechargeManager::OnNetOrderIdFromYD(DWORD dwGuid)
{
	DWORD dwCurTime = time(NULL);

	MSG_M2O_CP_ORDER_ID_RESP resp;
	resp.m_dwRoleId = m_pPlayer->GetRoleId();
	resp.m_dwOrderId = 0;

	// 检测时间间隔(1秒)
	if (dwCurTime <= m_dwLastRequestOrderTime)
	{
		MAPLOG(ERROR_LEVEL, "[%s]请求订单号太过频繁[RoleId:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
	}
	else
	{
		m_dwLastRequestOrderTime = dwCurTime;
		resp.m_dwOrderId = dwCurTime;
	}

	g_MapServer.GetOpenSession()->SendPacket(dwGuid, resp.GetType(), &resp);
}

// 添加充值数据
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
			MAPLOG(ERROR_LEVEL, "[%s]充值记录列表已满", __FUNCTION__);
			return;
		}

		CommonRecharge tRecharge;
		tRecharge.wRechargeId = wRechargeId;
		tRecharge.wRechargeTimes = 1;
		m_pRechargeData->arRechargeList[m_pRechargeData->wRechargeNum++] = tRecharge;
	}
}

// 添加月卡数据
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
			MAPLOG(ERROR_LEVEL, "[%s]月卡列表已满", __FUNCTION__);
			return;
		}
		m_pRechargeData->arMonthCardList[m_pRechargeData->wMontchCardNum++] = *pMonthCard;
	}
}

// 取充值次数
WORD CRechargeManager::GetRechargeTimes(WORD wRechargeId)
{
	CommonRecharge *pRecharge = FindCommonRecharge(wRechargeId);
	return pRecharge ? pRecharge->wRechargeTimes : 0;
}

// 是否有红点
bool CRechargeManager::IsHaveRedPoint(WORD wSystemId)
{
	DWORD dwNowTime = time(NULL);

	// 当月卡有可领取奖励时
	if (SYSTEM_MONTH_CARD == wSystemId)
	{
		for (WORD i = 0; i < m_pRechargeData->wMontchCardNum; ++i)
		{
			MonthCard &tCard = m_pRechargeData->arMonthCardList[i];

			// 检测今日是否已领取过
			if (!IsTheSameDay(tCard.dwLastTakeRewardTime, dwNowTime))
			{
				// 检测月卡是否已过期
				const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(tCard.wCardId);
				if (pMonthCardCfg)
				{
					// 永久卡
					if (!pMonthCardCfg->nValidTime)
						return true;

					// 月卡
					if (GetPassDays(tCard.dwBuyTime, dwNowTime) <= pMonthCardCfg->nValidTime)
						return true;
				}
			}
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的红点提示类型[SystemId:%d]", __FUNCTION__, wSystemId);
		return false;
	}

	return false;
}

// 删除过期月卡
void CRechargeManager::DeleteExpiredMonthCard()
{
	MonthCard arMonthCardList[MAX_MONTH_CAR_NUM];
	WORD wMontchCardNum = 0;

	// 遍历并取还没过期的卡
	DWORD dwNowTime = time(NULL);
	for (WORD i = 0; i < m_pRechargeData->wMontchCardNum; ++i)
	{
		MonthCard &tMonthCard = m_pRechargeData->arMonthCardList[i];
		const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(tMonthCard.wCardId);
		if (!pMonthCardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到月卡配置[CardId:%d]", __FUNCTION__, tMonthCard.wCardId);
			return;
		}

		// 终生卡 或 还没过期
		if (!pMonthCardCfg->nValidTime || (pMonthCardCfg->nValidTime && GetPassDays(tMonthCard.dwBuyTime, dwNowTime) <= pMonthCardCfg->nValidTime))
		{
			arMonthCardList[wMontchCardNum++] = tMonthCard;
		}
	}

	// 重新赋值
	m_pRechargeData->wMontchCardNum = wMontchCardNum;
	int nCopySize = wMontchCardNum * sizeof(MonthCard);
	memcpy_s(m_pRechargeData->arMonthCardList, nCopySize, arMonthCardList, nCopySize);
}

// 是否充值订单记录
bool CRechargeManager::IsOrderRecord(DWORD dwOrderId)
{
	for (WORD i = 0; i < m_pRechargeData->wCPOrderRecordNum; ++i)
	{
		if (m_pRechargeData->arCPOrderRecordList[i] == dwOrderId)
			return true;
	}
	return false;
}

// 添加订单记录
void CRechargeManager::AddOrderRecord(DWORD dwOrderId)
{
	if (m_pRechargeData->wCPOrderRecordNum >= MAX_ORDER_RECORD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]订单记录列表已满[role_id:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return;
	}

	m_pRechargeData->arCPOrderRecordList[m_pRechargeData->wCPOrderRecordNum++] = dwOrderId;
}

// 存储充值记录
void CRechargeManager::SaveRechargeRecord(WORD wRechargeId, WORD wRechargeType, int nAmount, int nRechargeDiamond, const char *pszPayTime, const char *pszSDKOrderId, WORD wFrom)
{
	// 充值记录
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

// 检测AppleIapID
bool CRechargeManager::CheckAppleIapId(const RechargeCallbackData &callbackData)
{
	const AppleIap_Config *pIapCfg = g_pCfgMgr->GetAppleIap(callbackData.szRechargeStrId);
	if (!pIapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到AppleIap配置[RechargeId:%d, AppleIapId:%s]", 
			__FUNCTION__, callbackData.wRechargeId, callbackData.szRechargeStrId);
		return false;
	}

	// 检测充值ID是否一样
	if (pIapCfg->dwRechargeId != callbackData.wRechargeId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]充值字串ID异常[role_id:%d,order_id:%s,config_recharge_id:%d,apple_iap_id:%s,is_mycard:%d,recharge_id:%d,currency_type:%d]",
			__FUNCTION__, m_pPlayer->GetRoleId(), callbackData.szSDKOrderId, pIapCfg->dwRechargeId, callbackData.szRechargeStrId,
			callbackData.byIsMyCard, callbackData.wRechargeId, callbackData.wCurrencyType);
		return false;
	}

	return true;
}

// 检测充值结果
bool CRechargeManager::CheckRechargeResult(int nCfgAmount, const RechargeCallbackData &callbackData)
{
	// 免检产品
	if (callbackData.byIsNotCheck)
	{
		// todo::做好日志防护
		MAPLOG(DEBUG_LEVEL, "[%s]免检产品充值[RoleId:%d, RechargeId:%d, Amount:%d,SdkOrderId:%s]",
			__FUNCTION__, callbackData.dwRoleId, callbackData.wRechargeId, callbackData.nAmount, callbackData.szSDKOrderId);
	}
	// 检测充值字串ID
	else if (callbackData.byIsCheckRechargeStrId)
	{
		const AppleIap_Config *pIapCfg = g_pCfgMgr->GetAppleIap(callbackData.szRechargeStrId);
		if (!pIapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]cannt found AppleIap config[RechargeId:%d, AppleIapId:%s]",
				__FUNCTION__, callbackData.wRechargeId, callbackData.szRechargeStrId);
			return false;
		}

		// 检测充值ID是否一样
		if (pIapCfg->dwRechargeId != callbackData.wRechargeId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]recharge string id erorr[role_id:%d,order_id:%s,config_recharge_id:%d,apple_iap_id:%s,is_mycard:%d,recharge_id:%d,currency_type:%d]",
				__FUNCTION__, m_pPlayer->GetRoleId(), callbackData.szSDKOrderId, pIapCfg->dwRechargeId, callbackData.szRechargeStrId,
				callbackData.byIsMyCard, callbackData.wRechargeId, callbackData.wCurrencyType);
			return false;
		}
	}
	// 检测充值金额
	else
	{
		// 验证要加的金额
		if (nCfgAmount != callbackData.nAmount)
		{
			MAPLOG(ERROR_LEVEL, "[%s]recharge amount erorr[order_id:%s,config_amount:%d,sdk_amount:%d,role_id:%d]",
				__FUNCTION__, callbackData.szSDKOrderId, nCfgAmount, callbackData.nAmount, m_pPlayer->GetRoleId());
			return false;
		}
	}

	return true;
}

// 是否今日首充
bool CRechargeManager::IsTodayFirstRecharge()
{
	return !m_pRechargeData->wTodayRechargetTimes ? true : false;
}

// 是否首充
bool CRechargeManager::IsFirstRecharge()
{
	return (!m_pRechargeData->wRechargeNum && !m_pRechargeData->wMontchCardNum) ? true : false;
}

// 月卡购买回调
bool CRechargeManager::DoMonthCard(WORD wCardId)
{
	// 取配置
	const MonthCard_Config *pMonthCardCfg = g_pCfgMgr->GetMonthCard(wCardId);
	if (!pMonthCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[CardId:%d]", __FUNCTION__, wCardId);
		return false;
	}

	int nAmount = pMonthCardCfg->nSellPrice;

	// 检测充值结果
// 	if (!CheckRechargeResult(nAmount, callbackData))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]检测充值结果失败[RechargeId:%d,RechargeType:%d]",
// 			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
// 		return false;
// 	}


	// 充值奖励
	m_pPlayer->GetVipManager()->AddExp(pMonthCardCfg->nVipExp);

	// 购买后立即获得钻石
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pMonthCardCfg->nBuyRewardDiamond, TRUE, FROM_TOP_UP);

	// 获得物品
	if (pMonthCardCfg->tRewardItem.dwId && pMonthCardCfg->tRewardItem.dwNum)
		m_pPlayer->AddObjectData(&pMonthCardCfg->tRewardItem, TRUE, FROM_RECHARGE_MONTH_CARD);

	// 添加购买月卡数据
	MonthCard tCard;
	tCard.wCardId = wCardId;
	tCard.dwLastTakeRewardTime = 0;
	tCard.dwBuyTime = time(NULL);
	AddMonthCardData(&tCard);

	// 发邮件通知(充值)
	WORD wEmailInfoId = wCardId == 1002 ? 10 : 7;
	EmailInfo tEailInfo(wEmailInfoId);
	//tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nSellPrice);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nBuyRewardDiamond);
	tEailInfo.AddContext(EMAIL_CONTEXT_INT, pMonthCardCfg->nDailyRewardDiamond);
	EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);

	// todo::如果是累充的活动时间则
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pMonthCardCfg->nSellPrice);

	// 节日充值活动
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pMonthCardCfg->nSellPrice);

	// 豪华签到充值
	//m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pMonthCardCfg->nSellPrice);

	// 7天活动事件(充值活动)
	m_pPlayer->OnUpdateSevenDayTarget(3, pMonthCardCfg->nSellPrice, true);

	// 通知客户端
	SendMonthCardData();

	// 充值记录
// 	SaveRechargeRecord(wCardId, callbackData.wRechargeType, pMonthCardCfg->nSellPrice, pMonthCardCfg->nBuyRewardDiamond,
// 		callbackData.szPayTime, callbackData.szSDKOrderId);

	return true;
}

// 普通充值
bool CRechargeManager::DoCommonRecharge(const Recharge_Config *pRechargeCfg)
{
// 	WORD wRechargeId = callbackData.wRechargeId;
// 
// 	// 取配置
// 	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(wRechargeId, callbackData.byIsMyCard);
// 	if (!pRechargeCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]cannt found recharge config[RechargeId:%d]", __FUNCTION__, wRechargeId);
// 		return false;
// 	}

	int nAmount = pRechargeCfg->nRMB;
	WORD wRechargeId = pRechargeCfg->wId;

	// 检测充值结果
// 	if (!CheckRechargeResult(nAmount, callbackData))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]检测充值结果失败[RechargeId:%d,RechargeType:%d]",
// 			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
// 		return false;
// 	}

	// 充值奖励
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nExtraDiamond, TRUE, FROM_TOP_UP);


// 	MAPLOG(ERROR_LEVEL, "[%s]recharge notify[RoleId:%d, RechargeId:%d, RechargeType:%d, VipExp:%d, RechargeDiamond:%d, ExtraDiamond:%d]",
// 		__FUNCTION__, m_pPlayer->GetRoleId(), wRechargeId, callbackData.wRechargeType, pRechargeCfg->nVipExp,
// 		pRechargeCfg->nRechargeDiamond, pRechargeCfg->nExtraDiamond);

	// 首充奖励
	if (!FindCommonRecharge(wRechargeId))
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nFirstRechargeDiamond, TRUE, FROM_TOP_UP);

		// 发邮件通知(充值)
		EmailInfo tEailInfo(6);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nFirstRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}
	else
	{
		// 发邮件通知(充值)
		EmailInfo tEailInfo(8);
		tEailInfo.AddContext(EMAIL_CONTEXT_INT, pRechargeCfg->nRechargeDiamond);
		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
	}

	AddRechargeData(wRechargeId);

	// 是否活动时间,已经在内部处理
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
	// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// 节日充值活动
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// 豪华签到充值
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// 7天活动事件(充值活动)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// 同步数据到客户端
	SendRechargeData();

	// 充值记录
// 	SaveRechargeRecord(wRechargeId, callbackData.wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
// 		callbackData.szPayTime, callbackData.szSDKOrderId);

	return true;
}

// 超值折扣充任回调(走通用逻辑)
bool CRechargeManager::DoNetSuperDiscount(const Recharge_Config *pRechargeCfg)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理超值折扣充值回调失败", __FUNCTION__);
		return false;
	}

	// 超值折扣回调
	m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// 主动同步数据
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().SendSuperDiscountData();

	return true;
}

// 新超值折扣充任回调
bool CRechargeManager::DoNetNewSuperDiscount(const Recharge_Config *pRechargeCfg)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理超值折扣充值回调失败", __FUNCTION__);
		return false;
	}

	// 超值折扣回调(新)
	m_pPlayer->GetActivityManager()->GetNewSuperDiscount().OnRechargeCallback();

	return true;
}

// 直购优惠回调
bool CRechargeManager::DoDirectBuy(const Recharge_Config *pRechargeCfg, WORD wGoodsId)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理直购优惠回调失败", __FUNCTION__);
		return false;
	}

	// 通过充值购买商品
	m_pPlayer->GetShopManager()->DirectBuyShopBuyByRecharge(wGoodsId);

	// 主动同步数据
	m_pPlayer->GetShopManager()->SendDirectBuyShopData();

	return true;
}

// 今日推荐回调
bool CRechargeManager::DoTodayRecommend(const Recharge_Config *pRechargeCfg)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理今日推荐充值回调失败", __FUNCTION__);
		return false;
	}

	m_pPlayer->GetActivityManager()->OnTodayRecommendRechargeCallback();

	// 主动同步数据
	m_pPlayer->GetActivityManager()->SendVipGiftData();

	return true;
}

// 精准推荐回调
bool CRechargeManager::DoAccurateRecommend(const Recharge_Config *pRechargeCfg, WORD wGoodsId)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理精准推荐充值回调失败", __FUNCTION__);
		return false;
	}

	// 超值折扣回调
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().OnRechargeCallback(wGoodsId);

	// 主动同步数据
	m_pPlayer->GetActivityManager()->GetAccurateRecommend().SendAccurateRecommendData();

	return true;
}

// 热卖商品
bool CRechargeManager::DoHotSaleGoods(const Recharge_Config *pRechargeCfg, WORD wGoodsId)
{
	if (!DoRechargeCallbackEx(pRechargeCfg))
	{
		MAPLOG(ERROR_LEVEL, "[%s]处理热卖商品回调失败", __FUNCTION__);
		return false;
	}

	m_pPlayer->GetActivityManager()->GetHotSaleGoods().OnRechargeNotify(wGoodsId, pRechargeCfg->wId);

	return true;
}

// 处理充值回调
bool CRechargeManager::DoRechargeCallbackEx(const Recharge_Config *pRechargeCfg)
{
	WORD wRechargeId = pRechargeCfg->wId;

	int nAmount = pRechargeCfg->nRMB;

	// 检测充值结果
// 	if (!CheckRechargeResult(nAmount, callbackData))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]检测充值结果失败[RechargeId:%d,RechargeType:%d]",
// 			__FUNCTION__, callbackData.wRechargeId, callbackData.wRechargeType);
// 		return false;
// 	}

	// 充值奖励
	m_pPlayer->GetVipManager()->AddExp(pRechargeCfg->nVipExp);

	//if (bAddDiamond)
	//	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRechargeCfg->nRechargeDiamond, TRUE, FROM_TOP_UP);

	// todo::不确定是否会有问题
	// AddRechargeData(wRechargeId);

	// 是否活动时间,已经在内部处理
	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeValue(pRechargeCfg->nRMB);
	// 	m_pPlayer->GetActivityManager()->GetLimitActivity().AddRechargeData(wRechargeId);

	// 节日充值活动
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnAccumulateRechargeCallback(pRechargeCfg->nRMB);
	m_pPlayer->GetActivityManager()->GetFestivalActivity().OnOnceRechargeCallback(wRechargeId);

	// 豪华签到充值
	m_pPlayer->GetActivityManager()->LuxurySignInRecharge(pRechargeCfg->wId);

	// 超值折扣回调
	//m_pPlayer->GetActivityManager()->GetSuperDiscount().OnRechargeCallback();

	// 7天活动事件(充值活动)
	m_pPlayer->OnUpdateSevenDayTarget(3, pRechargeCfg->nRMB, true);

	// 同步数据到客户端
	SendRechargeData();

	// 充值记录
// 	SaveRechargeRecord(wRechargeId, pRechargeCfg->wRechargeType, pRechargeCfg->nRMB, pRechargeCfg->nRechargeDiamond,
// 		callbackData.szPayTime, callbackData.szSDKOrderId);

	return true;
}

// 取月卡数据
MonthCard* CRechargeManager::GetMonthCard(WORD wMonthCardId)
{
	for (WORD i = 0; i < m_pRechargeData->wMontchCardNum; ++i)
	{
		if (m_pRechargeData->arMonthCardList[i].wCardId == wMonthCardId)
			return &m_pRechargeData->arMonthCardList[i];
	}
	return NULL;
}

// 取普通充值数据
CommonRecharge* CRechargeManager::FindCommonRecharge(WORD wRechargeId)
{
	for (WORD i = 0; i < m_pRechargeData->wRechargeNum; ++i)
	{
		if (m_pRechargeData->arRechargeList[i].wRechargeId == wRechargeId)
			return &m_pRechargeData->arRechargeList[i];
	}
	return NULL;
}

// 领取充值额度
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

// 领取充值额度
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