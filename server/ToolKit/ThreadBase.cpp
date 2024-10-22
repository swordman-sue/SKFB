#include "StdAfx.h"
#include "ThreadBase.h"
#include <process.h>

ThreadBase::ThreadBase(void)
:m_hThread(INVALID_HANDLE_VALUE)
{
}

ThreadBase::~ThreadBase(void)
{
	if (m_hThread!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

void ThreadBase::Release()
{
	if (m_hThread!=INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_hThread,INFINITE);
		::CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

void ThreadBase::SuspendThread()
{
	::SuspendThread(m_hThread);
}

void ThreadBase::ResumeThread()
{
	::ResumeThread(m_hThread);
}

void ThreadBase::StartThread(BOOL bSuspend)
{
	if(m_hThread==INVALID_HANDLE_VALUE)
	{
		//DWORD nThread;
		//m_hThread = ::CreateThread(NULL,0,m_fnThread,this,(bSuspend ? CREATE_SUSPENDED : 0),&nThread);
		unsigned nThread;
		m_hThread = (HANDLE)_beginthreadex(NULL,0,m_fnThread,this,(bSuspend ? CREATE_SUSPENDED : 0),&nThread);
		
	}
}

unsigned WINAPI ThreadBase::m_fnThread(LPVOID lpParameter)
{
	ThreadBase* pThis = (ThreadBase*)lpParameter;

	DWORD ret = pThis->run();

	return ret;
}