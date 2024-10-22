#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/Timer.h"

class TimeoutManager : public Singleton<TimeoutManager>
{
public:
	TimeoutManager(void);
	~TimeoutManager(void);

public:
	//��ʱ����
	VOID							Update( DWORD dwDeltaTick );
	INT								GetYear(DWORD dwTime ){ return dwTime/10000; }

	INT								GetMonth(DWORD dwTime ){ return dwTime%10000/100; }
	INT								GetDay(DWORD dwTime ){ return dwTime%100; }
	INT                             GetMounthMaxDay(DWORD dwYear,UCHAR ucMonth);

	//��ǰʱ��
	time_t							GetCurrentTime() { return m_CurrentTime; }
	struct tm*						GetLocaleTime() { return m_ptm; }

	//��ĳ��ĳ��ĳ������ucDays��֮����������������
	DWORD                           DaysLaterTime(DWORD dwBeginTime,DWORD dwDays);

	//��������֮����������(todo::Ŀǰ��BUG����ʹ��)
	DWORD                           DaysLater(DWORD dwBeginTime,DWORD dwEndTime);

	DWORD                           GetCurrentDay();//


	////�����������
	//VOID							ProcessInstanceTimes();

	////��������������
	//VOID							ProcessArenaTimes();

	////��ħ����������
	//VOID							ProcessDemonTimes();

	////ɽ�����ͼNPC�������
	//VOID							ProcessMRTimes();
	////ɽ�����ͼ��������
	//VOID							ProcessMRRest();

	////�������
	//VOID							ProcessOnlineAwardReset();

	////�ճ�����
	//VOID							ProcessDailyRemind();

	////��ʱ����
	//VOID							ProcessHotItemsReset();

	////ҡǮ��
	//VOID							ProcessMoneyTree();

	////����ÿ�����
	//VOID							ProcessYellowDay();

	////��ֵ����
	//VOID							ProcessBackBalance();

	////����aura����
	//VOID							ProcessBuyAuraNum();

private:
	VOID							ResetControlFlag();
	VOID							Reset();

private:
	time_t							m_CurrentTime;
	struct tm*						m_ptm;
	int								m_nDay;
	int								m_nHour;
	//bool							m_bInstanceTimes;
	//bool							m_bArenaTimes;
	//bool							m_bDemonTimes;
	//bool							m_bMR;
	//bool							m_bOnlineAward;
	//bool							m_bDailyRemind;
	//bool							m_bHotItems;
	//bool							m_bMoneyTree;
	//bool							m_bYellowDay;
	//bool							m_bBackBalance;
	//bool							m_bBuyAuraNum;
	bool							m_bResetFlg;
};
