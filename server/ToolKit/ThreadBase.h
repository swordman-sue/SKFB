#pragma once

class ThreadBase
{
public:
	ThreadBase(void);
	virtual ~ThreadBase(void);

	virtual void Release();

public:
	void SuspendThread();
	void ResumeThread();
	void StartThread(BOOL bSuspend = FALSE);
	virtual void EndThread() = 0;

	HANDLE GetHandle() {return m_hThread;}

protected:
	virtual DWORD run() = 0;

	static unsigned WINAPI m_fnThread(LPVOID lpParameter);

private:
	HANDLE	m_hThread;
};
