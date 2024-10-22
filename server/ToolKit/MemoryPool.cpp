#include "StdAfx.h"
#include "MemoryPool.h"

#ifdef MEMORY_LEAK_DETECT
MemoryPoolLeakDetector::MemoryPoolLeakDetector()
{
	m_printer = NULL;
	m_init = false;
}

MemoryPoolLeakDetector::~MemoryPoolLeakDetector()
{
	if (m_init)
		::SymCleanup(::GetCurrentProcess());
}

void MemoryPoolLeakDetector::Init(MemoryPrinter printer)
{
	IF_NOT (!m_init)
		return;
	m_init = true;
	m_printer = printer;
	::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
}

MemoryPrinter MemoryPoolLeakDetector::GetDetectorPrinter() const
{
	return m_printer;
}

static MemoryPoolLeakDetector g_memoryPoolLeakDetector;
MemoryPoolLeakDetector& GetMemoryPoolLeakDetector() { return g_memoryPoolLeakDetector; }
#endif
