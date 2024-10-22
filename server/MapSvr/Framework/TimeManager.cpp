#include "stdafx.h"
#include "TimeManager.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"
#include "ToolKit\LogFile.h"
#include "..\PlayerManager.h"
#include "..\Player.h"
#include "..\Fuben\FubenManager.h"


/*
	// 时间触发类型
	1 = 间隔时间 interval (开始时间1.指定时间, 2.开服时间)
	2 = 每日固定时间 day
	3 = 每周固定时间 week
	4 = 每月固定时间 month	
	5 = 每年固定时间 year
*/

CTimeManager::CTimeManager(void)
{

}


CTimeManager::~CTimeManager(void)
{
	
}

void CTimeManager::Init()
{
	const Server_Config *pServerCfgList = g_pCfgMgr->GetCommonCfg(124);
	if (pServerCfgList && pServerCfgList->vValueList.size())
	{
		for (size_t i = 0; i < pServerCfgList->vValueList.size(); ++i)
		{
			TimeEventData tEventData;
			memset(&tEventData, 0, sizeof(tEventData));
			tEventData.wEventId = 1;
			tEventData.nBeginTime = pServerCfgList->vValueList[i];
			tEventData.nEndTime = pServerCfgList->vValueList[i];
			RegisterTimeEvent(tEventData);
		}
	}

	// 注册回调函数
	RegisterFunc(1, &CTimeManager::OnEliteFBEnemyRefresh);

}

void CTimeManager::Update()
{
	time_t t = time(NULL);
	tm TM;
	GetTMTime(&TM, t);

	int nCurTime = /*TM.tm_mon * 10000 +*/ TM.tm_hour * 100 + TM.tm_min;

	for (size_t i = 0; i < m_vTimeDataList.size(); ++i)
	{
		TimeEventData &tEventData = m_vTimeDataList[i];

		// 检测是否到触发
		if (nCurTime < tEventData.nBeginTime || nCurTime > tEventData.nEndTime)
			continue;

		// 当前已经处理过
		if (IsTheSameDay(tEventData.dwLastTriggerTime, t))
			continue;


		// 检测CD时间
// 		if (nCurTime < tEventData.dwLastTriggerTime + tEventData.nCD)
// 			continue;
// 
// 		switch (tEventData.byTimeType)
// 		{
// 		// 1.间隔时间
// 		case 1:
// 		{
// 			if (tEventData.dwLastTriggerTime < tEventData.dwLastTriggerTime + tEventData.nIntervalTime)
// 				continue;
// 		}
// 		break;
// 
// 		// 2.每日
// 		case 2:
// 		{
// 			// 检测日期
// 			if (tEventData.nTriggerDate != TM.tm_hour * 100 + TM.tm_min)
// 				continue;
// 
// 			// 检测时间
// 
// 		}
// 		break;
// 
// 		// 3.周日
// 		case 3:
// 		{
// 
// 		}
// 		break;
// 
// 		// 4.月日
// 		case 4:
// 		{
// 
// 		}
// 		break;
// 
// 		// 5.年日
// 		case 5:
// 		{
// 
// 		}
// 		break;
// 		
// 		default:
//			MAPLOG(ERROR_LEVEL, "木有处理的时间事件类型[type:%d]", tEventData.byTimeType);
//			return;
// 		}

		std::map<DWORD, fnTimeEventCallback>::iterator iter = m_CallbackFuncList.find(tEventData.wEventId);
		if (iter == m_CallbackFuncList.end() || iter->second == NULL)
		{
			MAPLOG(ERROR_LEVEL, "找不到时间事件回调函数[EventId:%d]", tEventData.wEventId);
			continue;
		}

		// 触发回调函数
		iter->second();

		tEventData.dwLastTriggerTime = t;
	}
}

void CTimeManager::OnNewDay()
{
	
}

// 注册时间事件
void CTimeManager::RegisterTimeEvent(const TimeEventData &tEventData)
{
	m_vTimeDataList.push_back(tEventData);
}

// 注册回调函数
void CTimeManager::RegisterFunc(WORD wEventId, fnTimeEventCallback fnCallbackFunc)
{
	m_CallbackFuncList.insert(make_pair(wEventId, fnCallbackFunc));
}


// 精灵副本外敌刷新
void CTimeManager::OnEliteFBEnemyRefresh()
{
	vector<Player*> vPlayerList;
	PlayerManager::Instance()->GetAllPlayer(vPlayerList);
	for (size_t i = 0; i < vPlayerList.size(); ++i)
	{
		CFubenManager *pFBMgr = vPlayerList[i]->GetFubenManager();
		if (pFBMgr && pFBMgr->GetEliteFB().CreateEnemy())
		{
			pFBMgr->GetEliteFB().SendFBData();
		}		
	}
}