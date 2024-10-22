// CopyRight(c) 2010, Trans Studio. All rights reserved.

// ----------------------------------------------------------------------------
// Purpose : 内存跟踪
// Create  : 2010.06.20   11:07  徐志辉
// ----------------------------------------------------------------------------
#ifndef __Me_MemoryTracker_h__
#define __Me_MemoryTracker_h__

#include <stdlib.h>
#include <string>

struct UnitDebugInfo_t
{
	size_t						m_nAllocationID;

	void*						m_pMemory;
	size_t						m_nNumByte;

	unsigned long				m_nAllocThreadId;
	float						m_fAllocTime;

	class Me_CStringLineWalker*	m_pCallStack;

	UnitDebugInfo_t*			m_pPrev;
	UnitDebugInfo_t*			m_pNext;
};

class Me_CExternalAllocator
{
public:
	virtual void* Allocate(size_t nNumBytes) { return malloc(nNumBytes); }
	virtual void* Reallocate(void* pMemory, size_t nNumBytes) { return realloc(pMemory, nNumBytes); }
	virtual void Deallocate(void* pMemory, size_t nNumBytes) { return free(pMemory); }
};

class Me_CMemoryTracker
{
public:
	Me_CMemoryTracker(size_t nReservoirGrowBy = 1024);
	~Me_CMemoryTracker();

	void* OnAlloc(void* pMemory, size_t nNumBytes);
	// 这个方法有点臭
	void* OnRealloc(void* pMemory, void* pOldMemory, size_t nNumBytes, void* pInfo);
	void OnDealloc(void* pMemory, void* pDebugInfo = 0);

	void Dump(std::string& str);

private:
	Me_CExternalAllocator* m_pExternalAllocator;

private:
	void InsertDebugInfo(UnitDebugInfo_t* pDebugInfo);
	void RemoveDebugInfo(UnitDebugInfo_t* pDebugInfo);
	void GrowReservoir();

	void* AssignAllocInfo(UnitDebugInfo_t* pDebugInfo);

	UnitDebugInfo_t* AssignDebugInfo();
	UnitDebugInfo_t* FindDebugInfo(void* pInfo, void* pMemory);

private:
	UnitDebugInfo_t*	m_pReservoir;
	UnitDebugInfo_t*	m_pDebugInfo;

	UnitDebugInfo_t**	m_ppReservoirBuffer;
	size_t				m_nReservoirBufferSize;

	size_t				m_nReservoirGrowBy;

private:
	void ResetSummaryStats();

public:
	float m_fPeakMemoryTime;
	float m_fPeakAllocationCountTime;

	size_t m_nActiveMemory;
	size_t m_nPeakMemory;
	size_t m_nAccumulatedMemory;

	size_t m_nActiveAllocationCount;
	size_t m_nPeakAllocationCount;
	size_t m_nAccumulatedAllocationCount;

	size_t m_nDebugInfoSize;
	size_t m_nDebugInfoCount;
};

#endif // __Me_MemoryTracker_h__