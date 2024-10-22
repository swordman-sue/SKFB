#include "stdafx.h"
#include "Me_MemoryTracker.h"
#include "Me_StackWalker.h"
#include "Common.h"
#include <stdio.h>

#pragma warning( disable:4313 )

//#ifdef _DEBUG
#define _FAST_WALK
//#endif

Me_CMemoryTracker::Me_CMemoryTracker(size_t nReservoirGrowBy)
: m_pReservoir(0)
, m_pDebugInfo(0)
, m_ppReservoirBuffer(0)
, m_nReservoirBufferSize(0)
, m_nReservoirGrowBy(nReservoirGrowBy)
, m_nActiveMemory(0)
{
	m_pExternalAllocator = new Me_CExternalAllocator();
	ASSERT(m_pExternalAllocator);

	ResetSummaryStats();
}

Me_CMemoryTracker::~Me_CMemoryTracker()
{
	UnitDebugInfo_t* pDbgInfo = m_pDebugInfo;
	while ( pDbgInfo )
	{
		pDbgInfo->m_pCallStack->Clear();

		pDbgInfo = pDbgInfo->m_pPrev;
	}

	if ( m_ppReservoirBuffer )
	{
		for ( size_t i=0; i<m_nReservoirBufferSize; ++i )
		{
			m_pExternalAllocator->Deallocate(m_ppReservoirBuffer[i], -1);
		}
		m_pExternalAllocator->Deallocate(m_ppReservoirBuffer, -1);

		m_ppReservoirBuffer = NULL;
	}

	if ( m_pExternalAllocator )
	{
		delete m_pExternalAllocator;
		m_pExternalAllocator = NULL;
	}
}

void* Me_CMemoryTracker::OnAlloc(void* pMemory, size_t nNumBytes)
{
	float fTime = GetTickCount()*0.001f;

	m_nActiveMemory += nNumBytes;
	m_nAccumulatedMemory += nNumBytes;
	if ( m_nActiveMemory > m_nPeakMemory )
	{
		m_fPeakMemoryTime = fTime;
		m_nPeakMemory = m_nActiveMemory;
	}

	++m_nActiveAllocationCount;
	++m_nAccumulatedAllocationCount;
	if ( m_nActiveAllocationCount > m_nPeakAllocationCount )
	{
		m_fPeakAllocationCountTime = fTime;
		m_nPeakAllocationCount = m_nActiveAllocationCount;
	}

	UnitDebugInfo_t* pDebugInfo = AssignDebugInfo();

	pDebugInfo->m_pMemory			= pMemory;
	pDebugInfo->m_nNumByte			= nNumBytes;
	pDebugInfo->m_nAllocThreadId	= GetCurrentThreadId();
	pDebugInfo->m_fAllocTime		= fTime;
#ifdef _FAST_WALK
	pDebugInfo->m_pCallStack->FastWalk();
#else
	pDebugInfo->m_pCallStack->SafeWalk();
#endif

	InsertDebugInfo(pDebugInfo);

	return AssignAllocInfo(pDebugInfo);
}

void* Me_CMemoryTracker::OnRealloc(void* pMemory, void* pOldMemory, size_t nNumBytes, void* pInfo)
{
	float fTime = GetTickCount()*0.001f;

	UnitDebugInfo_t* pDebugInfo = FindDebugInfo(pInfo, pOldMemory);
	if ( pDebugInfo == NULL )
	{
		pDebugInfo = AssignDebugInfo();
	}

	size_t nIncreaseByte = 0;
	if( nNumBytes > pDebugInfo->m_nNumByte )
	{
		nIncreaseByte = nNumBytes-pDebugInfo->m_nNumByte;
		m_nActiveMemory += nIncreaseByte;
		m_nAccumulatedMemory += nIncreaseByte;
	}
	else
	{
		m_nActiveMemory -= (pDebugInfo->m_nNumByte-nNumBytes);
	}

	if ( m_nActiveMemory > m_nPeakMemory )
	{
		m_fPeakMemoryTime = fTime;
		m_nPeakMemory = m_nActiveMemory;
	}

	++m_nAccumulatedAllocationCount;

	pDebugInfo->m_pMemory			= pMemory;
	pDebugInfo->m_nNumByte			= nNumBytes;
	pDebugInfo->m_nAllocThreadId	= GetCurrentThreadId();
	pDebugInfo->m_fAllocTime		= fTime;
	pDebugInfo->m_pCallStack->FastWalk();

	return AssignAllocInfo(pDebugInfo);
}

void Me_CMemoryTracker::OnDealloc(void* pMemory, void* pInfo)
{
	UnitDebugInfo_t* pDebugInfo = FindDebugInfo(pInfo, pMemory);
	if ( pDebugInfo == NULL )
	{
		return;
	}

	m_nActiveMemory -= pDebugInfo->m_nNumByte;
	--m_nActiveAllocationCount;

	pDebugInfo->m_pMemory		= 0;
	pDebugInfo->m_nNumByte		= 0;
	pDebugInfo->m_nAllocThreadId= 0;
	pDebugInfo->m_fAllocTime	= 0.f;
	pDebugInfo->m_pCallStack->Clear();

	RemoveDebugInfo(pDebugInfo);
}

void Me_CMemoryTracker::InsertDebugInfo(UnitDebugInfo_t* pDebugInfo)
{
	pDebugInfo->m_pNext = 0;

	if (!m_pDebugInfo)
	{
		m_pDebugInfo = pDebugInfo;

		pDebugInfo->m_pPrev = 0;
	}
	else
	{
		m_pDebugInfo->m_pNext = pDebugInfo;

		pDebugInfo->m_pPrev	= m_pDebugInfo;

		m_pDebugInfo = pDebugInfo;
	}
}

void Me_CMemoryTracker::RemoveDebugInfo(UnitDebugInfo_t* pDebugInfo)
{
	if (pDebugInfo->m_pNext)
	{
		pDebugInfo->m_pNext->m_pPrev = pDebugInfo->m_pPrev;
	}

	if (pDebugInfo->m_pPrev)
	{
		pDebugInfo->m_pPrev->m_pNext = pDebugInfo->m_pNext;
	}

	if (pDebugInfo == m_pDebugInfo)
	{
		m_pDebugInfo = m_pDebugInfo->m_pPrev;
	}

	pDebugInfo->m_pNext = m_pReservoir;
	pDebugInfo->m_pPrev = 0;

	m_pReservoir = pDebugInfo;
}

void Me_CMemoryTracker::GrowReservoir()
{
	ASSERT( NULL == m_pReservoir );

	size_t nDebugInfoSize = m_nReservoirGrowBy * sizeof(UnitDebugInfo_t);
	size_t nCallStackSize = m_nReservoirGrowBy * sizeof(Me_CStringLineWalker);

	m_pReservoir = (UnitDebugInfo_t*)m_pExternalAllocator->Allocate(nDebugInfoSize + nCallStackSize);
	ASSERT( NULL != m_pReservoir );

	for (size_t i = 0; i < m_nReservoirGrowBy-1; i++)
	{
		m_pReservoir[i].m_nAllocationID = MAKELONG(i, m_nReservoirBufferSize);
		m_pReservoir[i].m_pNext = &m_pReservoir[i+1];
	}
	m_pReservoir[m_nReservoirGrowBy-1].m_nAllocationID = MAKELONG(m_nReservoirGrowBy-1, m_nReservoirBufferSize);
	m_pReservoir[m_nReservoirGrowBy-1].m_pNext = NULL;

	Me_CStackWalker* pStackBuffer = (Me_CStackWalker*)(m_pReservoir+m_nReservoirGrowBy);
	for (size_t i = 0; i < m_nReservoirGrowBy; i++)
	{
		// 使用上面申请的内存构造调用堆栈类
		m_pReservoir[i].m_pCallStack = new (pStackBuffer+i)Me_CStringLineWalker();
	}

	UnitDebugInfo_t** pTempDebugBuffer = (UnitDebugInfo_t**)m_pExternalAllocator->Reallocate(
		m_ppReservoirBuffer, (m_nReservoirBufferSize + 1)*sizeof(UnitDebugInfo_t*));
	
	ASSERT( NULL != pTempDebugBuffer );
	if ( pTempDebugBuffer )
	{
		m_ppReservoirBuffer = pTempDebugBuffer;
		m_ppReservoirBuffer[m_nReservoirBufferSize] = m_pReservoir;
		++m_nReservoirBufferSize;
	}

	m_nDebugInfoCount += m_nReservoirGrowBy;
	m_nDebugInfoSize += (nDebugInfoSize+nCallStackSize);
}

void* Me_CMemoryTracker::AssignAllocInfo(UnitDebugInfo_t* pDebugInfo)
{
	//return pDebugInfo;
	return (void*)pDebugInfo->m_nAllocationID;
}

UnitDebugInfo_t* Me_CMemoryTracker::AssignDebugInfo()
{
	if ( m_pReservoir == NULL )
	{
		GrowReservoir();
	}
	ASSERT( NULL != m_pReservoir );

	UnitDebugInfo_t* pDebugInfo = m_pReservoir;
	m_pReservoir = pDebugInfo->m_pNext;

	ASSERT(pDebugInfo);
	return pDebugInfo;
}

UnitDebugInfo_t* Me_CMemoryTracker::FindDebugInfo(void* pInfo, void* pMemory)
{
	//return static_cast<UnitDebugInfo_t*>(pInfo);

	size_t nAllocationID = (size_t)pInfo;

	size_t nReservoirIndex = HIWORD(nAllocationID);
	size_t nDebugIndex = LOWORD(nAllocationID);

	UnitDebugInfo_t* pDebugInfo = NULL;
	if ( ( nReservoirIndex < m_nReservoirBufferSize ) && ( nDebugIndex < m_nReservoirGrowBy ) )
	{
		pDebugInfo = m_ppReservoirBuffer[nReservoirIndex] + nDebugIndex;

		// 判断一下调试信息是不是记录着当前要删除的内存
		// 如果不是,那通常是外面传进来的内存有问题
		if ( pDebugInfo->m_pMemory != pMemory )
		{
			pDebugInfo = NULL;
		}
	}

	// 没有找到相关的调试信息.
	if ( NULL == pDebugInfo )
	{
		// 到现有调试列表找.(这一步会比较耗时,不过如果走到这一步,那说明内存操作上已经出现问题了.)
		pDebugInfo = m_pDebugInfo;
		while ( pDebugInfo )
		{
			if ( pDebugInfo->m_pMemory == pMemory )
			{
				break;
			}
			pDebugInfo = pDebugInfo->m_pPrev;
		}
	}

	// 还是没找到,那只有以下情况了:
	//   1,内存已经释放过了
	//   2,传进来的地址是错的.
	if ( NULL == pDebugInfo )
	{
		ASSERT ( !"Try free memory twice!" );
	}

	return pDebugInfo;
}

void Me_CMemoryTracker::ResetSummaryStats()
{
	m_fPeakMemoryTime = 0.0f;
	m_fPeakAllocationCountTime = 0.0f;

	m_nActiveMemory = 0;
	m_nPeakMemory = 0;
	m_nAccumulatedMemory = 0;

	m_nActiveAllocationCount = 0;
	m_nPeakAllocationCount = 0;
	m_nAccumulatedAllocationCount = 0;

	m_nDebugInfoSize = 0;
	m_nDebugInfoCount = 0;
}

#define TO_KB(v) v / 1024
#define TO_MB(v) v / (1024 * 1024)
#define TO_GB(v) v / (1024 * 1024 * 1024)

void Me_CMemoryTracker::Dump(std::string& str)
{
	char buff[4096];

	//str += "\n";
	//str += "  *--------------------------------BEGIN MEMORY SUMMARY--------------------------------*\n";
	//sprintf_s(buff, sizeof(buff),
	//	"    -- See if there is any unfreed memory block.\n"
	//	"    TotalActiveSize          = %-10d byte, %-7d KB, %-4d MB\n"
	//	"    ActiveAllocCount         = %d\n"
	//	"\n"
	//	"    -- Maximal memory bytes.\n"
	//	"    PeakActiveSize           = %-10d byte, %-7d KB, %-4d MB\n"
	//	"    AccumulatedSize          = %-10d byte, %-7d KB, %-4d MB\n"
	//	"\n"
	//	"    -- Maximal alloc count.\n"
	//	"    PeakActiveAllocCount     = %d\n"
	//	"    TotalAllocCount          = %d\n"
	//	"\n"
	//	"    -- Maximal debug info.\n"
	//	"    DebugInfoSize            = %-10d byte, %-7d KB, %-4d MB\n"
	//	"    DebugInfoCount           = %d\n"
	//	"",
	//	m_nActiveMemory, TO_KB(m_nActiveMemory), TO_MB(m_nActiveMemory),
	//	m_nActiveAllocationCount,

	//	m_nPeakMemory, TO_KB(m_nPeakMemory), TO_MB(m_nPeakMemory),
	//	m_nAccumulatedMemory, TO_KB(m_nAccumulatedMemory), TO_MB(m_nAccumulatedMemory),

	//	m_nPeakAllocationCount,
	//	m_nAccumulatedAllocationCount,

	//	m_nDebugInfoSize, TO_KB(m_nDebugInfoSize), TO_MB(m_nDebugInfoSize),
	//	m_nDebugInfoCount
	//	);

	//str += buff;

	//str += "  *---------------------------------END MEMORY SUMMARY---------------------------------*\n";
	//str += "\n\n";

	UnitDebugInfo_t* pDbgInfo = m_pDebugInfo;
	if (pDbgInfo == NULL)
		return;

	str += "\n  *------------------------------BEGIN MEMORY POOL LEAK REPORT------------------------------*\n";
	
	while ( pDbgInfo )
	{
		sprintf_s(buff, sizeof(buff), 
			//"  Address : 0x%x  size : %d  time : %f  thread : %d \n", 
			"  Address : 0x%p  time : %f  thread : %d \n", 
			pDbgInfo->m_pMemory, pDbgInfo->m_fAllocTime, pDbgInfo->m_nAllocThreadId);

		str += buff;
		pDbgInfo->m_pCallStack->Dump(str, 3);
		str += "\n";

		pDbgInfo = pDbgInfo->m_pPrev;
	}

	str += "  *-------------------------------END MEMORY POOL LEAK REPORT-------------------------------*\n";
	str += "\n";
}