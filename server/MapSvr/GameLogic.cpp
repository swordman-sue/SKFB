#include "stdafx.h"
#include "GameLogic.h"
#include "DBCDefine.h"
#include "ConfigManager.h"
#include "MsgDefine.h"
#include "OpenServerSession.h"
#include "CommonLogic.h"
#include "Player.h"
#include "Recharge\RechargeManager.h"
#include "Jsoncpp\src\lib_json\json_batchallocator.h"
#include "Jsoncpp\include\json\value.h"
#include "MapSvrQuery.h"

CGameLogic::CGameLogic()
{
	
}

CGameLogic::~CGameLogic()
{
	
}

void CGameLogic::Init(IniFile *pInifile)
{
	// 热云相关
	pInifile->GetStrVal("MapSvr", "ReYunAppId", m_szReyunAppId, sizeof(m_szReyunAppId)-1);
	m_bIsOpenReyunStatistics = pInifile->GetIntVal("MapSvr", "IsOpenReyunStatistics");


}

void CGameLogic::ReloadInitFile(IniFile *pInifile)
{
	// 热云相关
	pInifile->GetStrVal("MapSvr", "ReYunAppId", m_szReyunAppId, sizeof(m_szReyunAppId)-1);
	m_bIsOpenReyunStatistics = pInifile->GetIntVal("MapSvr", "IsOpenReyunStatistics");


}

// 热云统计
void CGameLogic::OnReyunStatistics(WORD wStatisticsType, DWORD dwChannelType, const char *pszAccount, WORD wServerId)
{
	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);

	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (pStatisticCfg)
	{
		char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
		sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(),
			GetReyunAppId(), pszAccount, pClientInfo ? pClientInfo->szDeviceId : "unknow", wServerId);

		OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
	}
}

// 热云统计
void CGameLogic::OnReyunStatistics(const char *pszPath, const char *pszContext)
{
	// 检测是否开启热云统计
	OpenServerSession *pSession = g_MapServer.GetOpenSession();
	if (pSession && IsOpenReyunStatistics())
	{
		MSG_M2O_REYUN_STATISTIC_NOTIFY msg;
		strcpy_s(msg.m_szPath, sizeof(msg.m_szPath) - 1, pszPath);
		strcpy_s(msg.m_szContext, sizeof(msg.m_szContext) - 1, pszContext);
		pSession->SendPacket(0, msg.GetType(), &msg);
	}
}

// 充值统计
void CGameLogic::OnRechargeStatistics(const RechargeCallbackData &callbackData, Player *pPlayer)
{
	// const char *pszAccount, DWORD dwChannelType, WORD wServerId
	DWORD dwChannelType = pPlayer->GetChannelType();
	const char *pszAccount = pPlayer->GetAccID();

	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);
	// 找不到客户端信息数据
	if (!pClientInfo)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到客户端信息数据[type:%d,account:%s]",__FUNCTION__, dwChannelType, pszAccount);
		return;
	}

	WORD wStatisticsType = 8;
	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (!pStatisticCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
		return;
	}

	DWORD dwCurrencyAmount = 0;
	DWORD dwVirtualCurrencyAmount = 0;
	const Recharge_Config *pRechargeCfg = g_pCfgMgr->GetRecharge(callbackData.wRechargeId, callbackData.byIsMyCard);
	const MonthCard_Config *pCardCfg = g_pCfgMgr->GetMonthCard(callbackData.wRechargeId);
	if (pRechargeCfg)
	{
		dwCurrencyAmount = pRechargeCfg->nRMB;
		dwVirtualCurrencyAmount = pRechargeCfg->nRechargeDiamond;

		Player *pPlayer = PlayerManager::Instance()->FindPlayerByRoleId(callbackData.dwRoleId);
		if (pPlayer && !pPlayer->GetRechargeManager()->GetRechargeTimes(callbackData.wRechargeId))
			dwVirtualCurrencyAmount = pRechargeCfg->nFirstRechargeDiamond;
	}
	if (pCardCfg)
	{
		dwCurrencyAmount = pCardCfg->nSellPrice;
		dwVirtualCurrencyAmount = pCardCfg->nBuyRewardDiamond;
	}

	// { "appid" : %s,"who" : %s,"context" : { "currencyAmount" : %d,"currencytype" : %s,"deviceid" : %s,"iapamount" :%d,
	// "iapname" : %s,"paymenttype" : %d,"transactionid" : %s,"virtualcoinamount" : %d,"level":"%d","Channelid":"%d","serverid":"%d"}}
	char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
	sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(), GetReyunAppId(), pszAccount, dwCurrencyAmount,
		"CNY", pClientInfo->szDeviceId, 1, "unknow", 1, callbackData.szSDKOrderId, dwVirtualCurrencyAmount, pPlayer->GetLevel(),
		dwChannelType, pPlayer->GetServerId());

	OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
}

// 关卡结果统计
void CGameLogic::OnStageStartStatistics(Player *pPlayer, int nX, int nY)
{
	DWORD dwChannelType = pPlayer->GetChannelType();
	const char *pszAccount = pPlayer->GetAccID();

	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);
	// 找不到客户端信息数据
	if (!pClientInfo)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到客户端信息数据[type:%d,account:%s]",__FUNCTION__, dwChannelType, pszAccount);
		return;
	}

	WORD wStatisticsType = 5;
	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (!pStatisticCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
		return;
	}

	char szQuestId[16] = { 0 };
	sprintf_s(szQuestId, sizeof(szQuestId)-1, "%d_%d", nX, nY);

	char szQuestStatus[8] = { 0 };
	sprintf_s(szQuestStatus, sizeof(szQuestStatus)-1, "%s", "a");

	// {"appid":"%s","who":"%s","context":{"deviceid":"%s","questid":"%s","queststatus":"s",
	// "questtype":"%s","serverid":"%d","level":"%d","Channelid":"%d"}}
	char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
	sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(), GetReyunAppId(), pszAccount, pClientInfo->szDeviceId,
		szQuestId, szQuestStatus, "main", pPlayer->GetServerId(), pPlayer->GetLevel(), pPlayer->GetChannelType());

	OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
}

// 关卡结果统计
void CGameLogic::OnStageOverStatistics(Player *pPlayer, int nX, int nY, bool bIsPass)
{
	DWORD dwChannelType = pPlayer->GetChannelType();
	const char *pszAccount = pPlayer->GetAccID();

	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);
	// 找不到客户端信息数据
	if (!pClientInfo)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到客户端信息数据[type:%d,account:%s]", __FUNCTION__,dwChannelType, pszAccount);
		return;
	}

	WORD wStatisticsType = 5;
	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (!pStatisticCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
		return;
	}

	char szQuestId[16] = { 0 };
	sprintf_s(szQuestId, sizeof(szQuestId)-1, "%d_%d", nX, nY);

	char szQuestStatus[8] = { 0 };
	if (bIsPass)
		sprintf_s(szQuestStatus, sizeof(szQuestStatus)-1, "%s", "c");
	else
		sprintf_s(szQuestStatus, sizeof(szQuestStatus)-1, "%s", "f");

	// {"appid":"%s","who":"%s","context":{"deviceid":"%s","questid":"%d","queststatus":"a",
	// "questtype":"%s","serverid":"%d","level":"%d","Channelid":"%d"}}
	char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
	sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(), GetReyunAppId(), pszAccount, pClientInfo->szDeviceId, 
		szQuestId, szQuestStatus, "main", pPlayer->GetServerId(), pPlayer->GetLevel(), pPlayer->GetChannelType());

	OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
}

// 登录统计
void CGameLogic::OnLoginStatistics(Player *pPlayer)
{
	DWORD dwChannelType = pPlayer->GetChannelType();
	const char *pszAccount = pPlayer->GetAccID();

	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);
	// 找不到客户端信息数据
	if (!pClientInfo)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到客户端信息数据[type:%d,account:%s]", __FUNCTION__,dwChannelType, pszAccount);
		return;
	}

	WORD wStatisticsType = REYUN_STATISTICS_LOGGEDIN;
	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (!pStatisticCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
		return;
	}

	// {"appid":"%s","who":"%s","context":{"deviceid":"%s","serverid":"%d","level":"%d","Channelid":"%d"}}
	char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
	sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(),GetReyunAppId(), pszAccount, 
		pClientInfo->szDeviceId, pPlayer->GetServerId(), pPlayer->GetLevel(), pPlayer->GetChannelType());

	OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
}

// 帐号注册
void CGameLogic::OnRegisterStatistics(DWORD dwChannelType, const char *pszAccount, WORD wServerId)
{
	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);
	// 找不到客户端信息数据
	if (!pClientInfo)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到客户端信息数据[type:%d,account:%s]",__FUNCTION__, dwChannelType, pszAccount);
		return;
	}

	WORD wStatisticsType = REYUN_STATISTICS_REGISTER;
	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (!pStatisticCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
		return;
	}

	// { "appid":"%s", "who" : "%s", "context" : {"deviceid":"%s", "serverid" : "%d", "Channelid" : "%d"} }
	char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
	sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(), GetReyunAppId(), pszAccount,
		pClientInfo->szDeviceId, wServerId, dwChannelType);

	OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
}

// 商品购买统计
void CGameLogic::OnGoodsBuyStatistics(Player *pPlayer, WORD wShopType, WORD wGoodsId, WORD wBuyNum)
{
	DWORD dwChannelType = pPlayer->GetChannelType();
	const char *pszAccount = pPlayer->GetAccID();

	const ClientInfo *pClientInfo = GetClientInfo(dwChannelType, pszAccount);
	// 找不到客户端信息数据
	if (!pClientInfo)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到客户端信息数据[type:%d,account:%s]",__FUNCTION__, dwChannelType, pszAccount);
		return;
	}

	WORD wStatisticsType = 9;
	const ReyunStatistic_Config *pStatisticCfg = g_pCfgMgr->GetReyunStatistic(wStatisticsType);
	if (!pStatisticCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到热云统计配置[type:%d]", wStatisticsType);
		return;
	}

	char szItemId[32] = { 0 };
	sprintf_s(szItemId, sizeof(szItemId)-1, "ShopType:%d|GoodsId:%d", wShopType, wGoodsId);

	// {"appid":"%s","who":"%s","context":{"deviceid":"%s","itemamount":"%d","itemname":"%s",
	// "itemtotalprice":"%s","serverid":"%d","level":"%d","Channelid":"%d"}}
	char szContext[MAX_REYUN_HTTP_CONTEXT_LEN] = { 0 };
	sprintf_s(szContext, sizeof(szContext)-1, pStatisticCfg->strContext.c_str(), GetReyunAppId(), pszAccount,
		pClientInfo->szDeviceId, wBuyNum, szItemId,"0", pPlayer->GetServerId(), pPlayer->GetLevel(), pPlayer->GetChannelType());

	OnReyunStatistics(pStatisticCfg->strPath.c_str(), szContext);
}

// 创建角色统计
void CGameLogic::OnGameStatistics_CreateRole(DWORD dwRoleId, WORD wServerId)
{
	if (g_MapServer.IsUse3rdStatisticsData())
	{
		Json::Value root;
		root["role_id"] = int(dwRoleId);
		root["server_id"] = wServerId;
		root["datetime"] = int(g_Time2Date(time(NULL)));
		string strText = root.toStyledString();

		MSG_M2O_STATISTICS_NOTIFY msg;
		msg.m_wType = 10;
		strcpy_s(msg.m_szContext, sizeof(msg.m_szContext), strText.c_str());
		g_MapServer.GetOpenSession()->SendPacket(0, msg.GetType(), &msg);
	}
	else
	{
		CGameStatistics_Event* pQuery = CGameStatistics_Event::ALLOC();
		pQuery->SetTime(time(NULL));
		pQuery->SetType(GAME_STATISTICS_CREATE_ROLE);
		pQuery->SetIndex(DB_GAME_STATISTICS_EVENT);
		pQuery->SetThreadIndex(DB_THREAD_LOG);
		pQuery->SetServerId(g_MapServer.GetCurServerId(wServerId));
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 登录统计
void CGameLogic::OnGameStatistics_Login(Player *pPlayer, bool bIsNewDay)
{
	if (g_MapServer.IsUse3rdStatisticsData())
	{
		Json::Value root;
		root["role_id"] = int(pPlayer->GetRoleId());
		root["server_id"] = pPlayer->GetServerId();
		root["is_new_day"] = bIsNewDay;
		root["datetime"] = int(g_Time2Date(time(NULL)));
		string strText = root.toStyledString();

		MSG_M2O_STATISTICS_NOTIFY msg;
		msg.m_wType = 11;
		strcpy_s(msg.m_szContext, sizeof(msg.m_szContext), strText.c_str());
		g_MapServer.GetOpenSession()->SendPacket(0, msg.GetType(), &msg);
	}
	else
	{
		if (!bIsNewDay)
			return;

		CGameStatistics_Event* pQuery = CGameStatistics_Event::ALLOC();
		pQuery->SetTime(time(NULL));
		pQuery->SetType(GAME_STATISTICS_LOGIN);
		pQuery->SetIndex(DB_GAME_STATISTICS_EVENT);
		pQuery->SetThreadIndex(DB_THREAD_LOG);
		pQuery->SetServerId(pPlayer->GetServerId());
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 充值统计
void CGameLogic::OnGameStatistics_Recharge(Player *pPlayer, int nAmount, bool bIsTodayFirstPayment, bool bIsFristPayment)
{
	if (g_MapServer.IsUse3rdStatisticsData())
	{
		Json::Value root;
		root["role_id"] = int(pPlayer->GetRoleId());
		root["server_id"] = pPlayer->GetServerId();
		root["is_today_first_payment"] = bIsTodayFirstPayment;
		root["amount"] = nAmount;
		root["is_new_role"] = pPlayer->IsTodayCreateRole();
		root["datetime"] = int(g_Time2Date(time(NULL)));
		string strText = root.toStyledString();

		MSG_M2O_STATISTICS_NOTIFY msg;
		msg.m_wType = 12;
		strcpy_s(msg.m_szContext, sizeof(msg.m_szContext), strText.c_str());
		g_MapServer.GetOpenSession()->SendPacket(0, msg.GetType(), &msg);
	}
	else
	{
		CGameStatistics_Event* pQuery = CGameStatistics_Event::ALLOC();
		pQuery->SetTime(time(NULL));
		pQuery->SetType(GAME_STATISTICS_RECHARGE);
		pQuery->SetIndex(DB_GAME_STATISTICS_EVENT);
		pQuery->SetThreadIndex(DB_THREAD_LOG);
		pQuery->SetParam(0, pPlayer->GetServerId());
		pQuery->SetParam(1, nAmount);
		pQuery->SetParam(2, bIsTodayFirstPayment);
		pQuery->SetParam(3, pPlayer->IsTodayCreateRole());
		pQuery->SetParam(4, bIsFristPayment);
		pQuery->SetServerId(pPlayer->GetServerId());
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 新的一天
void CGameLogic::OnGameStatistics_NewDay()
{
 	if (g_MapServer.IsUse3rdStatisticsData())
 	{
		Json::Value root;
		root["datetime"] = int(g_Time2Date(time(NULL)));
		string strText = root.toStyledString();

		MSG_M2O_STATISTICS_NOTIFY msg;
		msg.m_wType = 13;
		strcpy_s(msg.m_szContext, sizeof(msg.m_szContext), strText.c_str());
		g_MapServer.GetOpenSession()->SendPacket(0, msg.GetType(), &msg);
 	}
  	else
  	{
  		DWORD dwCurTime = time(NULL);
  
  		// 存储数据库
 		CGameStatistics_Event* pQuery = CGameStatistics_Event::ALLOC();
  		pQuery->SetTime(dwCurTime);
 		pQuery->SetType(GAME_STATISTICS_NEW_DAY);
 		pQuery->SetIndex(DB_GAME_STATISTICS_EVENT);
  		pQuery->SetThreadIndex(DB_THREAD_LOG);
		pQuery->SetContext(g_MapServer.GetCurMaxOnlineRoleNum());
  
  		const map<DWORD, ServerInfo> &serverList = g_MapServer.GetServerList();
  		map<DWORD, ServerInfo>::const_iterator iter = serverList.begin();
  		for (; iter != serverList.end(); ++iter)
  		{
  			DWORD dwCurServerId = g_MapServer.GetCurServerId(iter->second.dwServerId);
  
  			// 已经被合服, 不处理
  			if (dwCurServerId != iter->second.dwServerId)
  				continue;
  
  			// 还没开服，且不是今天开服
  			if (iter->second.dwOpenTime > dwCurTime && !IsTheSameDay(dwCurTime, iter->second.dwOpenTime))
  				continue;
  
  			pQuery->AddParam(dwCurServerId);
  		}
  
  		g_MapServer.MapDBQuery(pQuery);
  	}
}

// 在线数据统计
void CGameLogic::OnGameStatistics_Online(WORD wServerId, WORD wOnlineNum)
{
	if (g_MapServer.IsUse3rdStatisticsData())
	{
		// todo::
	}
	else
	{
		// 存储数据库
		CGameStatistics_Event* pQuery = CGameStatistics_Event::ALLOC();
		pQuery->SetTime(time(NULL));
		pQuery->SetType(GAME_STATISTICS_ONLINE);
		pQuery->SetIndex(DB_GAME_STATISTICS_EVENT);
		pQuery->SetThreadIndex(DB_THREAD_LOG);
		pQuery->SetServerId(wServerId);
		pQuery->SetParam(0, wOnlineNum);
		g_MapServer.MapDBQuery(pQuery);
	}
}

void CGameLogic::OnMyCardRechargeSuccessReport(Player *pPlayer, int nAmount, const char *pszOrderNo)
{
	/*
	Vid	是	int	万家用户唯一id，登陆后才有
	Ip	是	string	用户的IP信息
	imsi	否	string	imsi
	imei	否	string	手机串号
	vimei	否	string	vimei
	role_id	否	string	角色id
	role_name	否	string	角色名
	server_id	否	string	服务器id
	server_name	否	string	服务器名
	order_time	是	string	订单时间，格式：2018 - 05 - 30 10:05 : 23
	order_no	是	string	订单号
	pay	是	int	订单成功金额，单位：美分
	*/

	MSG_M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY msg;
	memset(&msg.m_ReportData, 0, sizeof(msg.m_ReportData));
	msg.m_ReportData.dwRoleId = pPlayer->GetRoleId();
	msg.m_ReportData.dwServerId = pPlayer->GetServerId();
	msg.m_ReportData.nPay = nAmount;
	string strAccount = pPlayer->GetAccID();
	msg.m_ReportData.dwVid = atoi(strAccount.c_str());
	DWORD dwChannelType = pPlayer->GetChannelType();
	const ClientInfo *pClientInfo = CGameLogic::Instance()->GetClientInfo(dwChannelType, pPlayer->GetAccID());
	if (pClientInfo)
		strcpy_s(msg.m_ReportData.szIP, sizeof(msg.m_ReportData.szIP), pClientInfo->szIP);
	strcpy_s(msg.m_ReportData.szOrderNo, sizeof(msg.m_ReportData.szOrderNo), pszOrderNo);
	strcpy_s(msg.m_ReportData.szRoleName, sizeof(msg.m_ReportData.szRoleName), pPlayer->GetRoleName());
	DATETIME datatime(time(NULL));
	datatime.toString(msg.m_ReportData.szOrderTime);
	g_MapServer.GetOpenSession()->SendPacket(0, msg.GetType(), &msg);
}

void CGameLogic::AddClientInfo(DWORD dwChannelType, const char *pszAccount, ClientInfo &tClientInfo)
{
	string strKey = MakeAccountKey(dwChannelType, pszAccount);
	map<string, ClientInfo>::iterator iter = m_ClientInfoList.find(strKey);
	// 有则更新
	if (iter != m_ClientInfoList.end())
	{
		iter->second = tClientInfo;
	}
	// 无则插入
	else
	{
		m_ClientInfoList.insert(std::make_pair(strKey, tClientInfo));
	}
}

// 取客户端信息
const ClientInfo* CGameLogic::GetClientInfo(DWORD dwChannelType, const char *pszAccount)
{
	string strKey = MakeAccountKey(dwChannelType, pszAccount);
	map<string, ClientInfo>::iterator iter = m_ClientInfoList.find(strKey);
	return iter != m_ClientInfoList.end() ? &iter->second : NULL;
}

// 添加阵营招募记录数据
void CGameLogic::AddCampRecruitRecord(DWORD dwServerId, WORD wCamp, const CampRecruitRecord &tRecord)
{
	DWORD dwKey = dwServerId * 100 + wCamp;
	map<DWORD, deque<CampRecruitRecord>>::iterator iter = m_CampRecruitRecordList.find(dwKey);
	if (iter == m_CampRecruitRecordList.end())
	{
		m_CampRecruitRecordList.insert(make_pair(dwKey, deque<CampRecruitRecord>()));
		iter = m_CampRecruitRecordList.find(dwKey);
	}

	iter->second.push_back(tRecord);

	// 只取最近5条记录 todo::需要调整为配置
	if (iter->second.size() > 5)
	{
		iter->second.pop_front();
	}
}

// 取阵营招募记录列表
const deque<CampRecruitRecord>* CGameLogic::GetCampRecruitRecordList(DWORD dwServerId, WORD wCamp)
{
	DWORD dwKey = dwServerId * 100 + wCamp;
	map<DWORD, deque<CampRecruitRecord>>::iterator iter = m_CampRecruitRecordList.find(dwKey);
	return iter != m_CampRecruitRecordList.end() ? &iter->second : NULL;
}