#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/Timer.h"

class TimeoutManager : public Singleton<TimeoutManager>
{
public:
	TimeoutManager(void);
	~TimeoutManager(void);

public:
	//定时更新
	VOID							Update( DWORD dwDeltaTick );
	INT								GetYear(DWORD dwTime ){ return dwTime/10000; }

	INT								GetMonth(DWORD dwTime ){ return dwTime%10000/100; }
	INT								GetDay(DWORD dwTime ){ return dwTime%100; }
	INT                             GetMounthMaxDay(DWORD dwYear,UCHAR ucMonth);

	//当前时间
	time_t							GetCurrentTime() { return m_CurrentTime; }
	struct tm*						GetLocaleTime() { return m_ptm; }

	//从某年某月某日往后ucDays天之后是哪年哪月哪日
	DWORD                           DaysLaterTime(DWORD dwBeginTime,DWORD dwDays);

	//两个日期之间间隔多少天(todo::目前有BUG不能使用)
	DWORD                           DaysLater(DWORD dwBeginTime,DWORD dwEndTime);

	DWORD                           GetCurrentDay();//


	////副本次数清除
	//VOID							ProcessInstanceTimes();

	////竞技场次数重置
	//VOID							ProcessArenaTimes();

	////天魔塔数据重置
	//VOID							ProcessDemonTimes();

	////山河社稷图NPC整点产出
	//VOID							ProcessMRTimes();
	////山河社稷图数据重置
	//VOID							ProcessMRRest();

	////在线礼包
	//VOID							ProcessOnlineAwardReset();

	////日常提醒
	//VOID							ProcessDailyRemind();

	////限时抢购
	//VOID							ProcessHotItemsReset();

	////摇钱树
	//VOID							ProcessMoneyTree();

	////黄钻每日礼包
	//VOID							ProcessYellowDay();

	////充值返现
	//VOID							ProcessBackBalance();

	////购买aura次数
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
