#pragma once
#include "DataDefine.h"
#include "../ToolKit/Singleton.h"
#include "../ToolKit/Timer.h"
#include <vector>


typedef void (*fnTimeEventCallback)();

class CTimeManager : public Singleton<CTimeManager>
{
public:
	CTimeManager(void);
	~CTimeManager(void);

	void Init();

	void Update();

	// 新的一天
	void OnNewDay();

	// 注册时间事件
	void RegisterTimeEvent(const TimeEventData &tEventData);

	// 注册回调函数
	void RegisterFunc(WORD wEventId, fnTimeEventCallback fnCallbackFunc);
public:

	// 精灵副本外敌刷新
	static void OnEliteFBEnemyRefresh();

protected:


private:
	time_t							m_CurrentTime;
	struct tm*						m_ptm;
	int								m_nDay;
	int								m_nHour;

	std::vector<TimeEventData> m_vTimeDataList;	// 时间数据列表

	std::map<DWORD, fnTimeEventCallback> m_CallbackFuncList;	// 回调函数列表
};
