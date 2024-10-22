#include "StdAfx.h"
#include <time.h>
#include "TimeoutManager.h"
#include "PlayerManager.h"

TimeoutManager::TimeoutManager(void)
{
	m_CurrentTime = time(NULL);
	m_ptm = localtime(&m_CurrentTime);

	m_nDay = -1;

	m_nHour = m_ptm->tm_hour;

	//m_bInstanceTimes = false;
	//m_bArenaTimes = false;
	//m_bDemonTimes = false;
	//m_bMR = false;
	//m_bOnlineAward = false;
	//m_bDailyRemind = false;
	//m_bHotItems = false;
	//m_bMoneyTree = false;
	//m_bYellowDay = false;
	//m_bBackBalance = false;
	//m_bBuyAuraNum = false;
	m_bResetFlg = false;
}

TimeoutManager::~TimeoutManager(void)
{
}

VOID TimeoutManager::ResetControlFlag()
{
	if(!m_ptm)
		return;
	
	//新的一天开始了
	if (m_ptm->tm_mday != m_nDay)
	{
		m_nDay = m_ptm->tm_mday;

		//m_bInstanceTimes = false;
		//m_bArenaTimes = false;
		//m_bDemonTimes = false;
		//m_bMR = false;
		//m_bOnlineAward = false;
		//m_bDailyRemind = false;
		//m_bHotItems = false;
		//m_bMoneyTree = false;
		//m_bYellowDay = false;
		//m_bBackBalance = false;
		//m_bBuyAuraNum = false;
		m_bResetFlg = false;
	}
}

VOID TimeoutManager::Reset()
{
	if(!m_ptm)
		return;

	if (m_ptm->tm_hour == 0 && !m_bResetFlg)
	{
		PlayerManager::Instance()->ProcessReset();
		m_bResetFlg = true;
	}
}

INT	TimeoutManager::GetMounthMaxDay(DWORD dwYear,UCHAR ucMonth)
{
	bool LeapYear = false;
	if ((dwYear%4==0&&dwYear%100!=0)||dwYear%400==0)
	{
		LeapYear = true;
	}
	switch(ucMonth)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
		break;
	case 2:
		{
			if (LeapYear)
			{
				return 29;
			}
			else
			{
				return 28;

			}
		}

	}
	return 0;

}

DWORD	TimeoutManager::DaysLaterTime(DWORD dwBeginTime,DWORD dwDays)//从某年某月某日往后dwDays天之后是哪年哪月哪日
{
	int nYear = GetYear(dwBeginTime);
	int nMonth = GetMonth(dwBeginTime);
	INT nCurrDay = GetDay(dwBeginTime);
	INT nNewDay = 0;

	bool LeapYear = false;
	if ((nYear%4==0&&nYear%100!=0)||nYear%400==0)
	{
		LeapYear = true;
	}
	while (dwDays>0)
	{
		int nMaxDay=GetMounthMaxDay(nYear,nMonth);
		if (nMaxDay-nCurrDay>=(INT)dwDays)
		{
			nNewDay = nCurrDay + dwDays;
			dwDays =0;
		}
		else
		{
			dwDays =dwDays - (nMaxDay-nCurrDay);
			nCurrDay=0;
			nNewDay=1;
			if (nMonth == 12 )
			{
				nYear++;
				nMonth=0;
			}
			++nMonth;
		}
	}
	return nYear * 10000 + nMonth * 100 + nNewDay; 
}
DWORD   TimeoutManager::GetCurrentDay()
{
	int nYear = GetYear((DWORD)m_CurrentTime);
	int nMonth = GetMonth((DWORD)m_CurrentTime);
	INT nCurrDay = GetDay((DWORD)m_CurrentTime);
	return nYear * 10000 + nMonth * 100 + nCurrDay; 
}


DWORD TimeoutManager::DaysLater(DWORD dwBeginTime,DWORD dwEndTime)//两个日期之间间隔多少天
{
	if (dwBeginTime>dwEndTime)
		return 0;

	int nBeginYear = GetYear(dwBeginTime);
	int nBeginMonth = GetMonth(dwBeginTime);
	INT nBeginDay = GetDay(dwBeginTime);
	int nEndYear = GetYear(dwEndTime);
	int nEndMonth = GetMonth(dwEndTime);
	INT nEndDay = GetDay(dwEndTime);

	if ((nBeginYear==nEndYear) && (nBeginMonth==nEndMonth) )
	{
		return nEndDay - nBeginDay;  
	}

	int nDays = GetMounthMaxDay(nBeginYear, nBeginMonth);
	
	nBeginMonth++;

	while ((nBeginYear!=nEndYear) && (nBeginMonth!=nEndMonth) )
	{
		nDays += GetMounthMaxDay(nBeginYear,nBeginMonth++);
	}

	return nDays;
}
VOID TimeoutManager::Update( DWORD dwDeltaTick )
{
	m_CurrentTime = time(NULL);
	m_ptm = localtime(&m_CurrentTime);
	if(!m_ptm)
		return;

	//重置标志
	ResetControlFlag();

	//ProcessInstanceTimes();

	//ProcessArenaTimes();

	//ProcessDemonTimes();

	//ProcessMRTimes();

	//ProcessMRRest();

	//ProcessOnlineAwardReset();

	//ProcessDailyRemind();

	////取消商城限时抢购
	////ProcessHotItemsReset();

	//ProcessMoneyTree();

	//ProcessYellowDay();

	//ProcessBackBalance();

	//ProcessBuyAuraNum();

	//重置系统功能
	Reset();
}

//VOID TimeoutManager::ProcessInstanceTimes()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bInstanceTimes)
//	{
//		PlayerManager::Instance()->ProcessInstanceTimes();
//
//		m_bInstanceTimes = true;
//	}
//}
//
//VOID TimeoutManager::ProcessArenaTimes()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bArenaTimes)
//	{
//		PlayerManager::Instance()->ProcessValidArenaTimes();
//
//		m_bArenaTimes = true;
//	}
//}
//
//VOID TimeoutManager::ProcessDemonTimes()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bDemonTimes)
//	{
//		PlayerManager::Instance()->ProcessDemonTimes();
//
//		m_bDemonTimes = true;
//	}
//}
//
//VOID TimeoutManager::ProcessMRTimes()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour != m_nHour)
//	{
//		m_nHour = m_ptm->tm_hour;
//
//		//到整点了
//		PlayerManager::Instance()->ProcessMRTimes(m_nHour);
//	}
//}
//
//VOID TimeoutManager::ProcessMRRest()
//{
//	if (m_ptm->tm_hour == 0 && !m_bMR)
//	{
//		PlayerManager::Instance()->ProcessMRRest();
//
//		m_bMR = true;
//	}
//}
//
//VOID TimeoutManager::ProcessOnlineAwardReset()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bOnlineAward)
//	{
//		PlayerManager::Instance()->ProcessOnlineAwardReset();
//
//		m_bOnlineAward = true;
//	}
//}
//
////日常提醒
//VOID TimeoutManager::ProcessDailyRemind()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bDailyRemind)
//	{
//		PlayerManager::Instance()->ProcessDailyRemindReset();
//
//		m_bDailyRemind = true;
//	}
//}
//
//VOID TimeoutManager::ProcessHotItemsReset()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bHotItems)
//	{
//		ShopMall::UpdateHotItem();
//
//		PlayerManager::Instance()->ProcessHotItemsReset();
//
//		m_bHotItems = true;
//	}
//}
//
//VOID TimeoutManager::ProcessMoneyTree()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bMoneyTree)
//	{
//		PlayerManager::Instance()->ProcessMoneyTree();
//
//		m_bMoneyTree = true;
//	}
//}
//
//VOID TimeoutManager::ProcessYellowDay()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bYellowDay)
//	{
//		PlayerManager::Instance()->ProcessYellowDay();
//
//		m_bYellowDay = true;
//	}
//}
//
//VOID TimeoutManager::ProcessBackBalance()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bBackBalance)
//	{
//		PlayerManager::Instance()->ProcessBackBalance();
//
//		m_bBackBalance = true;
//	}
//}
//
//VOID TimeoutManager::ProcessBuyAuraNum()
//{
//	if(!m_ptm)
//		return;
//
//	if (m_ptm->tm_hour == 0 && !m_bBuyAuraNum)
//	{
//		PlayerManager::Instance()->ProcessBuyAuraNum();
//
//		m_bBuyAuraNum = true;
//	}
//}