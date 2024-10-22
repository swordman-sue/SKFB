#pragma once

#include <deque>
#include "CriticalSection.h"

class ObjKeyGenerator
{
public:
	ObjKeyGenerator(void);
	~ObjKeyGenerator(void);
public:
	void	Create(DWORD dwStartKey, DWORD dwEndKey, DWORD dwStep = 1);
	DWORD	Getkey();
	void	RestoreKey(DWORD dwKey);
	size_t	GetKeyNum() const;

private:
	std::deque<DWORD>	m_dequeKey;
	CriticalSection		m_cs;
};
