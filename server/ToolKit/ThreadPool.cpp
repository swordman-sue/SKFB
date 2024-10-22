#include "StdAfx.h"
#include "ThreadPool.h"
#include "ThreadBase.h"

ThreadPool::ThreadPool(void)
{
}

ThreadPool::~ThreadPool(void)
{
}

void ThreadPool::AddThread(ThreadBase* pThread,BOOL bSuspend)
{
	m_ThreadList.push_back(pThread);
	pThread->StartThread(bSuspend);
}

void ThreadPool::BatchStopThread()
{
	std::list<ThreadBase*>::iterator iter;
	for (iter = m_ThreadList.begin();iter!=m_ThreadList.end();++iter)
	{
		(*iter)->EndThread();
	}
}

void ThreadPool::BatchStartThread()
{
	std::list<ThreadBase*>::iterator iter;
	for (iter = m_ThreadList.begin();iter!=m_ThreadList.end();++iter)
	{
		(*iter)->ResumeThread();
	}
}

void ThreadPool::Release()
{
	BatchStopThread();

	HANDLE* pThreads = new HANDLE[m_ThreadList.size()];

	int i = 0;
	std::list<ThreadBase*>::iterator iter;
	for (iter = m_ThreadList.begin();iter!=m_ThreadList.end();++iter)
	{
		pThreads[i++] = (*iter)->GetHandle();
	}

	::WaitForMultipleObjects((DWORD)m_ThreadList.size(),pThreads,TRUE,INFINITE);

	delete [] pThreads;

	
	for (iter = m_ThreadList.begin();iter!=m_ThreadList.end();++iter)
	{
		delete (*iter);
	}

	m_ThreadList.clear();
}
