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
		ִ�ж�ӦQuestһ��
		@Return Ϊtrue��ʾ��TimerQuest���ڸô����н�����ɾ��
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

	//�����Ӧ�Ķ�ʱ����
	ZQ::UINT32 m_timer_func;

	double m_run_period;
	double m_last_time;
	double m_next_run_time;

	//��ͣ��ʼ��ʱ��
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
		��ʱִ��һ������
		@para1 quest_func �����Ӧ�Ľű����� in
		@para2 delay_time ��ʱʱ�� in
		@Return �����������ڳ���ID
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddDelayQuest(
		ZQ::UINT32 quest_func,
		double delay_time); 

	/*
		������ִ��һ������
		@para1 quest_func �����Ӧ�Ľű����� in
		@para2 period ִ������ in
		@para3 last_time �������ʱ��(Ϊ����ʾ������������) in
		@Return �����������ڳ���ID
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddPeriodQuest(
		ZQ::UINT32 quest_func,
		double period, 
		double last_time);
	
	
	/*
		������ִ������ָ������
		@para1 quest_func �����Ӧ�Ľű����� in
		@para2 period ִ������ in
		@para3 run_count ����ִ�еĴ��� in
		@Return �����������ڳ���ID
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddRunTimesQuest(
		ZQ::UINT32 quest_func,
		double period,
		unsigned int run_count);
	
	/*
		ȡ��һ������
		@para1 quest_id ����ID in
		@author deadline
		@create 4/5/2012
	*/
	void CancelQuest(unsigned int quest_id);

	/*
		��ͣһ������
		@para1 quest_id ����ID in
		@author swordman sue
	*/
	void PauseQuest(unsigned int quest_id);

	/*
		�ָ�һ������
		@para1 quest_id ����ID in
		@author swordman sue
	*/
	void ResumeQuest(unsigned int quest_id);

	// ��ͣ��������
	void Pause();

	// �ָ���������
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
