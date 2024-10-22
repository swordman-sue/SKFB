#include "stdafx.h"
#include "TimeManager.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"
#include "ToolKit\LogFile.h"
#include "..\PlayerManager.h"
#include "..\Player.h"
#include "..\Fuben\FubenManager.h"


/*
	// ʱ�䴥������
	1 = ���ʱ�� interval (��ʼʱ��1.ָ��ʱ��, 2.����ʱ��)
	2 = ÿ�չ̶�ʱ�� day
	3 = ÿ�̶ܹ�ʱ�� week
	4 = ÿ�¹̶�ʱ�� month	
	5 = ÿ��̶�ʱ�� year
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

	// ע��ص�����
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

		// ����Ƿ񵽴���
		if (nCurTime < tEventData.nBeginTime || nCurTime > tEventData.nEndTime)
			continue;

		// ��ǰ�Ѿ������
		if (IsTheSameDay(tEventData.dwLastTriggerTime, t))
			continue;


		// ���CDʱ��
// 		if (nCurTime < tEventData.dwLastTriggerTime + tEventData.nCD)
// 			continue;
// 
// 		switch (tEventData.byTimeType)
// 		{
// 		// 1.���ʱ��
// 		case 1:
// 		{
// 			if (tEventData.dwLastTriggerTime < tEventData.dwLastTriggerTime + tEventData.nIntervalTime)
// 				continue;
// 		}
// 		break;
// 
// 		// 2.ÿ��
// 		case 2:
// 		{
// 			// �������
// 			if (tEventData.nTriggerDate != TM.tm_hour * 100 + TM.tm_min)
// 				continue;
// 
// 			// ���ʱ��
// 
// 		}
// 		break;
// 
// 		// 3.����
// 		case 3:
// 		{
// 
// 		}
// 		break;
// 
// 		// 4.����
// 		case 4:
// 		{
// 
// 		}
// 		break;
// 
// 		// 5.����
// 		case 5:
// 		{
// 
// 		}
// 		break;
// 		
// 		default:
//			MAPLOG(ERROR_LEVEL, "ľ�д����ʱ���¼�����[type:%d]", tEventData.byTimeType);
//			return;
// 		}

		std::map<DWORD, fnTimeEventCallback>::iterator iter = m_CallbackFuncList.find(tEventData.wEventId);
		if (iter == m_CallbackFuncList.end() || iter->second == NULL)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ���ʱ���¼��ص�����[EventId:%d]", tEventData.wEventId);
			continue;
		}

		// �����ص�����
		iter->second();

		tEventData.dwLastTriggerTime = t;
	}
}

void CTimeManager::OnNewDay()
{
	
}

// ע��ʱ���¼�
void CTimeManager::RegisterTimeEvent(const TimeEventData &tEventData)
{
	m_vTimeDataList.push_back(tEventData);
}

// ע��ص�����
void CTimeManager::RegisterFunc(WORD wEventId, fnTimeEventCallback fnCallbackFunc)
{
	m_CallbackFuncList.insert(make_pair(wEventId, fnCallbackFunc));
}


// ���鸱�����ˢ��
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