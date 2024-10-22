////////////////////////////////////////////////////////////////////////////////
//	Filename     :TsMemory.h
//	Created by   :Kevin
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <wtypes.h>

class TMemory
{
public:
	TMemory(void);
	~TMemory(void);

	BOOL	Alloc(DWORD dwSize);
public:
	LPVOID	m_lpData;
	DWORD	m_dwSize;
};
