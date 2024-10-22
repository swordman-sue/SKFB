#pragma once

class Timers
{
public: 
	Timers():m_dwExpireTime(0),m_dwIntervalTime(0),m_bCheckTime(FALSE) {}
	~Timers(){}

public: 

	inline void SetTimer( DWORD dwIntervalTime )
	{
		m_dwIntervalTime	= dwIntervalTime;
		Reset();
	}

	inline DWORD GetIntervalTime() { return m_dwIntervalTime; }
	
	inline void Reset()
	{
		m_dwExpireTime		= GetTickCount();
		EnableCheckTime();
	}

	inline BOOL IsEnabled() const
	{
		return m_bCheckTime;
	}
	
	inline VOID EnableCheckTime()
	{
		m_bCheckTime = TRUE;
	}
	
	inline VOID DisableCheckTime()
	{
		m_bCheckTime = FALSE;
	}

	// DWORD表示的毫秒时间是一个有限的区间(0 ~ 0xFFFFFFFF)可能发生回溯
	inline DWORD GetTimeDiff(DWORD dwOldTime, DWORD dwNewTime)
	{
		if (dwNewTime>=dwOldTime)
		{
			return dwNewTime - dwOldTime;
		}
		else
		{
			return (0xFFFFFFFF - dwOldTime + dwNewTime + 1);
		}

	}
	
	inline BOOL IsExpired( BOOL bReset=TRUE )
	{
		DWORD dwCurTime;
		if( m_bCheckTime ) 
		{
			dwCurTime = GetTickCount();
			if(GetTimeDiff(m_dwExpireTime,dwCurTime)>=m_dwIntervalTime)
			{
				if( bReset )
				{
					m_dwExpireTime = dwCurTime;
				}
				return TRUE;
			}
			else
				return FALSE;
		}
		else 
			return FALSE;
	}

	inline DWORD GetExpireTime()	{ return m_dwExpireTime; }

private:
	BOOL		m_bCheckTime;
	DWORD		m_dwExpireTime;
	DWORD		m_dwIntervalTime;
};


// 日期计时器
class CDateTimers
{
public:
	CDateTimers() :m_dwCDOverTime(0), m_nWeekDay(-1), m_nOpenServerDay(-1){}
	~CDateTimers(){}

	void Reset()
	{
		m_nWeekDay = 0;
		m_nOpenServerDay = 0;
		m_dwCDOverTime = 0;
		m_TriggerTimeList.clear();
	}

public:

	inline void SetCDOverTime(DWORD dwCD = 61/*秒*/) { m_dwCDOverTime = time(NULL) + dwCD; }
	inline void SetWeekDay(int nWeekDay) { m_nWeekDay = nWeekDay; };
	inline void SetOpenServerDay(int nOpenServerDay) { m_nOpenServerDay = nOpenServerDay; };
	inline void AddTriggerTime(DWORD dwTriggerTime) { m_TriggerTimeList.push_back(dwTriggerTime); };
	inline BOOL IsTrigger()
	{
		time_t t = time(NULL);

		// CD中...
		if (t <= m_dwCDOverTime)
			return FALSE;

		tm tmNow;
		localtime_s(&tmNow, &t);

		if (-1 != m_nWeekDay && tmNow.tm_wday != m_nWeekDay)
			return FALSE;

		DWORD dwCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;

		for (size_t i = 0; i < m_TriggerTimeList.size(); ++i)
		{
			if (dwCurTime == m_TriggerTimeList[i])
				return TRUE;
		}

		return FALSE;
	}
	inline BOOL IsTrigger(int nOpenServerDay)
	{
		time_t t = time(NULL);

		// CD中...
		if (t <= m_dwCDOverTime)
			return FALSE;

		tm tmNow;
		localtime_s(&tmNow, &t);

		// 到达指定的开服天数
		if (nOpenServerDay != m_nOpenServerDay)
			return FALSE;

		DWORD dwCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;

		for (size_t i = 0; i < m_TriggerTimeList.size(); ++i)
		{
			if (dwCurTime == m_TriggerTimeList[i])
				return TRUE;
		}

		return FALSE;
	}

	inline BOOL IsTriggerEx()
	{
		time_t t = time(NULL);

		// CD中...
		if (t <= m_dwCDOverTime)
			return FALSE;

		tm tmNow;
		localtime_s(&tmNow, &t);

		DWORD dwCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;

		for (size_t i = 0; i < m_TriggerTimeList.size(); ++i)
		{
			tm tmTrigger;
			time_t tmp = m_TriggerTimeList[i];
			localtime_s(&tmTrigger, &tmp);

			// 秒级别
			if (tmTrigger.tm_year == tmNow.tm_year 
				&& tmTrigger.tm_mon == tmNow.tm_mon 
				&& tmTrigger.tm_hour == tmNow.tm_hour
				&& tmTrigger.tm_min == tmNow.tm_min)
				return TRUE;
		}

		return FALSE;
	}

private:
	int				m_nWeekDay;			// -1 表示木有日期限制（days since Sunday - [0,6]）
	int				m_nOpenServerDay;	// 开服第几天
	DWORD			m_dwCDOverTime;		// CD结束时间(秒)(这段时间检测无效)
	vector<DWORD>	m_TriggerTimeList;	// 触发时间段列表[小时分钟]
};


