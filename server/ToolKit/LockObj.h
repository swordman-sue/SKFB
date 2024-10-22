////////////////////////////////////////////////////////////////////////////////
//	Filename     :LockObj.h
//	Created by   :dcc
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////
class CLockObj
{
public:
	CLockObj()
	{
		InitializeCriticalSection(&m_lock);
	}
	~CLockObj()
	{
		DeleteCriticalSection(&m_lock); 		
	}
	void Lock()
	{
		EnterCriticalSection(&m_lock);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_lock);
	}
private:
	CRITICAL_SECTION m_lock;
};
/////////////////////////////////////////////////////////////////////
class CLock
{
public:
	CLock(CLockObj *pLockObj)
	{
		if (NULL == pLockObj)
			return;

		m_pLockObj = pLockObj;
		pLockObj->Lock();
	}	
	~CLock()
	{
		if (m_pLockObj)
			m_pLockObj->Unlock();
	}
private:
	CLockObj *m_pLockObj;
};
/////////////////////////////////////////////////////////////////////