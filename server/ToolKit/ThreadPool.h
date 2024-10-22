#pragma once

#include <list>

class ThreadBase;

class ThreadPool
{
public:
	ThreadPool(void);
	~ThreadPool(void);

public:
	void AddThread(ThreadBase* pThread,BOOL bSuspend = FALSE);
	void BatchStopThread();
	void BatchStartThread();
	void Release();

private:
	std::list<ThreadBase*>	m_ThreadList;
};
