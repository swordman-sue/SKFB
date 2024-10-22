#pragma once

class CriticalSection
{
public:
	CriticalSection(void);
	~CriticalSection(void);

public:
	void Lock();
	void Unlock(void);

private:
	CRITICAL_SECTION m_cs;
};
