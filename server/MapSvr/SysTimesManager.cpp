#include "stdafx.h"
#include "SysTimesManager.h"
#include "../ToolKit/DBCFile.h"
#include "../ToolKit/DBCFileManager.h"
#include "PlayerManager.h"
//#include "ServerRestrictions.h"

SystemTimeManager::SystemTimeManager()
{
}
SystemTimeManager::~SystemTimeManager()
{

}
bool SystemTimeManager::Init()
{
	m_mapTimes.clear();
	m_mapUpdate.clear();
	DBCFile* pFile = DBCFileManager::Instance()->GetDBC(DBC_TIMES_CONFIG);
	if (NULL == pFile)
	{
		return false;
	}
	int nNum = pFile->GetRecordNum();
	for (INT i = 1;i<nNum;i++)
	{
		BeginEndTime Info;
		Info.bIsDisposable = false;
		SystemTimes* pTime = (SystemTimes*) pFile->IterateRecord(i);
		if (NULL != pTime && (-1 != pTime->IsIng))//有开启关闭时间
		{
			SetBeginTimes(pTime, Info.dwBeginTime);
			SetEndTimes(pTime, Info.dwEndTime);
			Info.bBeginFlag = IsBegining(pTime);
			if (Info.bBeginFlag)
			{
				StartActivity((SystemIDs)pTime->ID);
			}
			m_mapTimes.insert(make_pair((SystemIDs)pTime->ID,Info));
		}
		if (NULL != pTime && (-1 != pTime->IsUpdate))//有更新时间
		{
			DWORD dwUpdateTime=0;
			SetUpdateTimes(pTime,dwUpdateTime);
			SystemIDs ID = (SystemIDs)pTime->ID;
			m_mapUpdate.insert(make_pair(ID,dwUpdateTime));
		}
	}

	//限时抢购配置加载
// 	pFile = DBCFileManager::Instance()->GetDBC(DBC_RESTRICTIONS_INFO);
// 	if (NULL == pFile)
// 	{
// 		return false;
// 	}
// 	for (INT i = 1;i<pFile->GetRecordNum();i++)
// 	{
// 		BeginEndTime Info;
// 		Info.bIsDisposable = true;
// 		RestrictionsInfo* pInfo = NULL;
// 		pInfo = (RestrictionsInfo*) pFile->IterateRecord(i);
// 		if (NULL != pInfo )
// 		{
// 			time_t nCurTime = time(NULL);
// 			if ((nCurTime>pInfo->m_dwEndTime))
// 			{
// 				continue;
// 			}
// 			if (nCurTime<pInfo->m_dwBeginTime)
// 			{
// 				Info.dwBeginTime = (DWORD)(pInfo->m_dwBeginTime - nCurTime)*1000;
// 				Info.dwEndTime   = (DWORD)(pInfo->m_dwEndTime- pInfo->m_dwBeginTime)*1000;
// 				Info.bBeginFlag  = false;
// 			}
// 			else
// 			{
// 				Info.bBeginFlag=true;
// 				Info.dwEndTime   = (DWORD)(pInfo->m_dwEndTime - nCurTime)*1000;
// 			}
// 			m_mapTimes.insert(make_pair((SystemIDs)(pInfo->m_dwID+1000),Info));
// 		}
// 	}

	return true;
}


void SystemTimeManager::SetBeginTimes( SystemTimes* pTime, DWORD& dwTimeData )
{
	if (NULL == pTime )
	{
		return;
	}
	time_t nCurTime = time(NULL);
	tm* ptm = localtime(&nCurTime);
	ptm->tm_hour	= pTime->BeginHour;
	ptm->tm_min		= pTime->BeginMinute;
	ptm->tm_sec		= 0;
	DWORD dwTime = (DWORD)mktime(ptm);

	if (nCurTime >= dwTime)
		dwTimeData = (SECOND_OF_DAY - ((DWORD)nCurTime - dwTime)) * 1000;
	else
		dwTimeData = (dwTime - (DWORD)nCurTime) * 1000;
}
void SystemTimeManager::SetEndTimes( SystemTimes* pTime, DWORD& dwTimeData )
{
	if (NULL == pTime )
	{
		return;
	}
	time_t nCurTime = time(NULL);
	tm* ptm = localtime(&nCurTime);
	ptm->tm_hour	= pTime->EndHouse;
	ptm->tm_min		= pTime->EndMinute;
	ptm->tm_sec		= 0;
	DWORD dwTime = (DWORD)mktime(ptm);

	if (nCurTime >= dwTime)
		dwTimeData = (SECOND_OF_DAY - ((DWORD)nCurTime - dwTime)) * 1000;
	else
		dwTimeData = (dwTime - (DWORD)nCurTime) * 1000;
}


void SystemTimeManager::SetUpdateTimes( SystemTimes* pTime, DWORD &dwTimeData )
{
	if (NULL == pTime )
	{
		return;
	}
	time_t nCurTime = time(NULL);
	tm* ptm = localtime(&nCurTime);
	ptm->tm_hour	= pTime->UpdateHour;
	ptm->tm_min		= pTime->UpdateMinute;
	ptm->tm_sec		= 0;
	DWORD dwTime = (DWORD)mktime(ptm);

	if (nCurTime >= dwTime)
		dwTimeData = (SECOND_OF_DAY - ((DWORD)nCurTime - dwTime)) * 1000;
	else
		dwTimeData = (dwTime - (DWORD)nCurTime) * 1000;

}

bool SystemTimeManager::GetSystemTimes(SystemIDs dwKey,BeginEndTime& Info)
{
	BETimeMap_ITR itr = m_mapTimes.find(dwKey);
	if (itr == m_mapTimes.end() )
	{
		return false;
	}
	Info = itr->second;
	return true;
}
bool SystemTimeManager::IsBegin(SystemIDs dwKey)
{
	BETimeMap_ITR itr = m_mapTimes.find(dwKey);
	if (itr == m_mapTimes.end() )
	{
		return false;//不在容器中的默认是结束的
	}
	return itr->second.bBeginFlag;

}
bool  SystemTimeManager::IsHave(SystemIDs dwKey)
{
	BETimeMap_ITR itr = m_mapTimes.find(dwKey);
	if (itr == m_mapTimes.end() )
	{
		return false;//不在容器中的默认是结束的
	}
	return true;
}
void SystemTimeManager::StartActivity(SystemIDs dwKey,DWORD dwSecond)
{
	BETimeMap_ITR itr = m_mapTimes.find(dwKey);
	if (itr == m_mapTimes.end() )
	{
		return ;//不在容器中的默认是开始的
	}
	itr->second.dwBeginTime = 0;
	itr->second.dwEndTime = dwSecond*1000;
	itr->second.bBeginFlag = true;
}
bool SystemTimeManager::IsEnd(SystemIDs dwKey)
{
	BETimeMap_ITR itr = m_mapTimes.find(dwKey);
	if (itr == m_mapTimes.end() )
	{
		return true;//不在容器中的默认是结束的
	}
	return !itr->second.bBeginFlag;
}

void SystemTimeManager::DeleteTimes(SystemIDs dwKey)
{
	BETimeMap_ITR itr = m_mapTimes.find(dwKey);
	if (itr==m_mapTimes.end())
	{
		return;
	}
	m_mapTimes.erase(itr);

}
void SystemTimeManager::CheckInsert()
{
	BETimeMap_ITR itr = m_mapTimes.begin();
	for (itr;itr!=m_mapTimes.end();itr++)
	{
		if (itr->second.bIsDisposable && itr->second.bBeginFlag )
		{
			//ServerRestrictions::Instance()->InsertSyn(itr->first-1000,0);

		}

	}

}
void SystemTimeManager::Update(DWORD dwTick)
{
	BETimeMap_ITR itr = m_mapTimes.begin();
	for (itr;itr!=m_mapTimes.end();itr++)
	{
		if (itr->second.dwBeginTime>dwTick )
		{
			itr->second.dwBeginTime -= dwTick;
		}
		else
		{
			itr->second.dwBeginTime = SECOND_OF_DAY*1000; 
			itr->second.bBeginFlag = true;
			StartActivity(itr->first);
			
			
		}
		if (itr->second.dwEndTime>dwTick )
		{
			itr->second.dwEndTime -= dwTick;
		}
		else
		{
			itr->second.dwEndTime = SECOND_OF_DAY*1000;
			itr->second.bBeginFlag = false;
			if (!itr->second.bIsDisposable)
			{
				EndActivity(itr->first);
			}
			else
			{
				DeleteTimes(itr->first);
				//ServerRestrictions::Instance()->CheckTimeOut();
				//PlayerManager::Instance()->CheckTimeOutRestrictions();
			}
			
		}
	}
	UpdateMap_ITR itrUpdate = m_mapUpdate.begin();
	for (itrUpdate;itrUpdate!= m_mapUpdate.end();itrUpdate++)
	{
		if (itrUpdate->second>dwTick )
		{
			itrUpdate->second -= dwTick;
		}
		else
		{
			itrUpdate->second = SECOND_OF_DAY*1000; 
			UpdateActivity(itrUpdate->first);
		}

	}
}
void SystemTimeManager::StartActivity(SystemIDs dwKey)
{
	if (dwKey>1000)
	{
		//ServerRestrictions::Instance()->InsertSyn(dwKey-1000,0);
	}
	
}
void SystemTimeManager::EndActivity(SystemIDs dwKey)
{
	//PlayerManager::Instance()->UpdateSys(dwKey,false);
}
void SystemTimeManager::UpdateActivity(SystemIDs dwKey)
{
	PlayerManager::Instance()->UpdateSys(dwKey,ActivityOpe_Update);
}

bool SystemTimeManager::IsBegining( SystemTimes* pTime )
{
	time_t nCurTime = time(NULL);
	tm* ptm = localtime(&nCurTime);
	ptm->tm_hour	= pTime->BeginHour;
	ptm->tm_min		= pTime->BeginMinute;
	ptm->tm_sec		= 0;
	DWORD dwBeginTime = (DWORD)mktime(ptm);

	ptm->tm_hour	= pTime->EndHouse;
	ptm->tm_min		= pTime->EndMinute;
	ptm->tm_sec		= 0;
	DWORD dwEndTime = (DWORD)mktime(ptm);
	DWORD dwTime1 = 0;
	DWORD dwTime2 = 0;

	if (dwEndTime>dwBeginTime)
	{
		dwTime1 = (dwEndTime -dwBeginTime)*1000;
	}
	else
	{
		dwTime1 = (SECOND_OF_DAY - dwBeginTime + dwEndTime)*1000;
	}
	if (dwEndTime>nCurTime)
	{
		dwTime2 = (DWORD)(dwEndTime -nCurTime)*1000;
	}
	else
	{
		dwTime2 = (DWORD)(SECOND_OF_DAY - nCurTime + dwEndTime)*1000;
	}
	
	if (dwTime2 >= dwTime1  )
	{
		return false;
	}
	return true;
}

