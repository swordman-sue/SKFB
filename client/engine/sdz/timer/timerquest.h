#ifndef TIMERQUEST_H
#define TIMERQUEST_H

#include <map>
#include <set>
#include "zqcommon/typedef.h"

class TimerQuest;

class TimerQuestItem
{
public:
	TimerQuestItem(unsigned int id, TimerQuest* agent, ZQ::UINT32 quest_func);
	~TimerQuestItem();

	/*
		执行对应Quest一次
		@Return 为true表示该TimerQuest将在该次运行结束后被删除
		@author deadline
		@create 3/2/2012
	*/
	bool Run();

	void Pause();
	void Resume();

public:
	unsigned int m_id;

	bool m_is_always_run;

	unsigned int m_run_count;

	//任务对应的定时函数
	ZQ::UINT32 m_timer_func;

	double m_run_period;
	double m_last_time;
	double m_next_run_time;

	//暂停开始的时间
	double m_pause_time;

	TimerQuest* m_parent;

private:
	bool m_is_pause;
	int m_pause_ref_count;
};

class TimerQuest
{
public: 
	TimerQuest();
	~TimerQuest();

	void Update(double now_time, double elapse_time);

	void Stop();

	bool _IsQuestAlive(unsigned int id);

	bool _IsQuestPause(unsigned int id);

	bool _IsQuestResume(unsigned int id);

	/*
		延时执行一个任务
		@para1 quest_func 任务对应的脚本函数 in
		@para2 delay_time 延时时间 in
		@Return 返回生命周期持有ID
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddDelayQuest(
		ZQ::UINT32 quest_func,
		double delay_time); 

	/*
		周期性执行一个任务
		@para1 quest_func 任务对应的脚本函数 in
		@para2 period 执行周期 in
		@para3 last_time 任务持续时间(为负表示任务永不过期) in
		@Return 返回生命周期持有ID
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddPeriodQuest(
		ZQ::UINT32 quest_func,
		double period, 
		double last_time);
	
	
	/*
		周期性执行任务指定次数
		@para1 quest_func 任务对应的脚本函数 in
		@para2 period 执行周期 in
		@para3 run_count 任务执行的次数 in
		@Return 返回生命周期持有ID
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddRunTimesQuest(
		ZQ::UINT32 quest_func,
		double period,
		unsigned int run_count);
	
	/*
		取消一个任务
		@para1 quest_id 任务ID in
		@author deadline
		@create 4/5/2012
	*/
	void CancelQuest(unsigned int quest_id);

	/*
		暂停一个任务
		@para1 quest_id 任务ID in
		@author swordman sue
	*/
	void PauseQuest(unsigned int quest_id);

	/*
		恢复一个任务
		@para1 quest_id 任务ID in
		@author swordman sue
	*/
	void ResumeQuest(unsigned int quest_id);

	// 暂停所有任务
	void Pause();

	// 恢复所有任务
	void Resume();

protected:
	typedef std::multimap<double, TimerQuestItem*> TimerQuestItemMap;
	TimerQuestItemMap m_timer_quest_map;
	unsigned int m_id_count;
	double m_now_time;

	typedef std::set<unsigned int> KeepIdSet;
	KeepIdSet m_keep_id_set;

	bool m_is_pause;
};

#endif 
