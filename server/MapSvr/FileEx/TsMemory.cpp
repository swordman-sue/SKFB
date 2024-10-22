#include "StdAfx.h"
#include "tsmemory.h"

#if defined(_DEBUG) && defined(_TESTMEMORY) && defined(_MSC_VER) 
#	if defined(_MFC_VER) 
#		define new DEBUG_NEW
#	else
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include "crtdbg.h"
#		define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	endif
#endif

//-----------------------------------------------------------------
// Function name   : TMemory::TMemory
// Return type     : 
// Argument        : void
// Description   : 
//-----------------------------------------------------------------
TMemory::TMemory(void)
{
	m_lpData = NULL;
	m_dwSize = 0;
}

//-----------------------------------------------------------------
// Function name   : TMemory::~TMemory
// Return type     : 
// Argument        : void
// Description   : 
//-----------------------------------------------------------------
TMemory::~TMemory(void)
{
	if (m_lpData)
	{
		delete [] m_lpData;
		m_lpData = NULL;
	}
	m_dwSize = 0;
}

//-----------------------------------------------------------------
// Function name   : TMemory::Alloc
// Return type     : BOOL 
// Argument        : DWORD dwSize
// Description   : 
//-----------------------------------------------------------------
BOOL TMemory::Alloc(DWORD dwSize)
{
	if (m_lpData && m_dwSize < dwSize)
	{
		delete [] m_lpData;
		m_lpData = NULL;
	}

	if (m_lpData == NULL)
	{
		m_dwSize = dwSize;
		try
		{
			m_lpData = new BYTE [m_dwSize];
		}
		catch(...)
		{
			return FALSE;
		}
		ZeroMemory(m_lpData,m_dwSize);
	}
	return TRUE;
}