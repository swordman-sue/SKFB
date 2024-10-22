#include "StdAfx.h"
#include "CriticalSection.h"

CriticalSection::CriticalSection(void)
{
	::InitializeCriticalSection( &m_cs );
}

CriticalSection::~CriticalSection(void)
{
	::DeleteCriticalSection( &m_cs );
}

void CriticalSection::Lock()
{
	::EnterCriticalSection( &m_cs );
}

void CriticalSection::Unlock()
{
	::LeaveCriticalSection( &m_cs );
}
