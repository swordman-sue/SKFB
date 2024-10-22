#pragma once

// 关闭此项才能真正关闭内存泄漏检测功能
#ifdef _DEBUG
#define MEMORY_LEAK_DETECT
#endif

#ifdef MEMORY_LEAK_DETECT
#include <windows.h>
#include <dbghelp.h>
#include <hash_map>
#include "Me_MemoryTracker.h"
#include "Common.h"
#endif
#include <map>

#ifdef MEMORY_LEAK_DETECT
typedef void (*MemoryPrinter)(const char* szText);
class MemoryPoolLeakDetector
{
public:
	MemoryPoolLeakDetector();
	~MemoryPoolLeakDetector();
	void Init(MemoryPrinter printer);
	MemoryPrinter GetDetectorPrinter() const;
private:
	MemoryPrinter m_printer;
	bool m_init;
};
#endif

#ifdef MEMORY_LEAK_DETECT
#define INIT_MEMORY_LEAK_DETECT(func) extern MemoryPoolLeakDetector& GetMemoryPoolLeakDetector();\
	GetMemoryPoolLeakDetector().Init(func);
#else
#define INIT_MEMORY_LEAK_DETECT(func)
#endif

template <class DataType>
class CMemTypeBand
{
	DataType **		ppDataType;
	DataType *		pDeleteArray;
	DWORD			nMaxBandObjectNum;
	DWORD			FreeIdx;

public:
	CMemTypeBand *	pPrev;
	CMemTypeBand *	pNext;

	CMemTypeBand():pPrev(NULL),pNext(NULL){}
	virtual ~CMemTypeBand()
	{
		pPrev=NULL;pNext=NULL;
		delete [] (ppDataType);
		delete [] (pDeleteArray);
	}
	inline DWORD GetAvailableNumberOfObjects()
	{
		return FreeIdx;
	}
	inline DWORD GetMaxNumberOfObjects()
	{
		return nMaxBandObjectNum;
	}

	DataType * GetObject( DWORD dwIdx )
	{
		return &pDeleteArray[dwIdx];
	}

	static CMemTypeBand * AllocBand( CMemTypeBand<DataType> *& head, DWORD nMaxObject )
	{
		CMemTypeBand<DataType>* pp = new CMemTypeBand<DataType>;
		pp->ppDataType = new DataType*[nMaxObject];
		memset(pp->ppDataType, 0, sizeof(DataType*)*nMaxObject);
		pp->pDeleteArray = new DataType[nMaxObject];
		for(DWORD i = 0 ; i < nMaxObject ; i++)
		{
			pp->ppDataType[i] = &pp->pDeleteArray[i];
		}
		pp->nMaxBandObjectNum	= nMaxObject;
		pp->FreeIdx				= nMaxObject;
		pp->pNext				= head;
		if(head)
			head->pPrev			= pp;
		head = pp;

		return pp;
	}

	static void FreeBand( CMemTypeBand<DataType> * pDelBand )
	{
		//一旦头结点删除，所有的后继结点都被删除
		CMemTypeBand<DataType> * db = pDelBand;
		while(db)
		{
			CMemTypeBand<DataType> * tmp = db;
			db = db->pNext;
			delete tmp;
		}
	}
	DataType * AlloObject()
	{
		
		if(--FreeIdx == -1)
		{
			FreeIdx = 0;
			return NULL;
		}
		else
			return ppDataType[FreeIdx];
	}
	BOOL FreeObject(DataType * data)
	{
		
		if(FreeIdx  < nMaxBandObjectNum )
			ppDataType[FreeIdx++] = data;
		else
			return FALSE;
		return TRUE;
	}
};

template <class Type>
class CMemoryPoolFactory
{
public:
	CMemoryPoolFactory()
		:	m_pBandHead( NULL ),
		m_pFreeBand( NULL ),
		m_dwPoolExtendSize( 0 )
	{
#ifdef MEMORY_LEAK_DETECT
		m_memoryTracker = new Me_CMemoryTracker();
#endif
	}

	virtual ~CMemoryPoolFactory()
	{
#ifdef MEMORY_LEAK_DETECT
		extern MemoryPoolLeakDetector& GetMemoryPoolLeakDetector();
		MemoryPrinter printer = GetMemoryPoolLeakDetector().GetDetectorPrinter();
		if (printer)
		{
			std::string memoryInfo = "";
			m_memoryTracker->Dump(memoryInfo);
			if (memoryInfo != "")
				printer(memoryInfo.c_str());
		}

		delete m_memoryTracker;
		m_memoryTracker = NULL;
#endif
		Release();
	}

	BOOL Initialize( DWORD dwPoolBasicSize, DWORD dwPoolExtendSize = 0 )
	{
		if( 0 == dwPoolExtendSize )
			m_dwPoolExtendSize = dwPoolBasicSize/2+1;
		else
			m_dwPoolExtendSize = dwPoolExtendSize;

		if( NULL == m_pBandHead )
		{
			CMemTypeBand<Type>::AllocBand( m_pBandHead, dwPoolBasicSize );
			m_pFreeBand = m_pBandHead;
			if( !m_pFreeBand ) 
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	void Release()
	{
		if( m_pBandHead )
		{
			CMemTypeBand<Type>::FreeBand( m_pBandHead );
			
			m_pBandHead = NULL;
			m_pFreeBand = NULL;
		}
	}

	inline Type * Alloc()
	{
		Type * pn = m_pFreeBand->AlloObject();
		if(pn == NULL)
		{
			if( !m_pFreeBand->pPrev )
			{
				CMemTypeBand<Type>::AllocBand( m_pBandHead, m_dwPoolExtendSize );
				m_pFreeBand = m_pBandHead;
				pn = m_pFreeBand->AlloObject();
			}
			else
			{
				m_pFreeBand = m_pFreeBand->pPrev;
				pn = m_pFreeBand->AlloObject();
			}
		}
		
#ifdef MEMORY_LEAK_DETECT
		m_debugInfo[pn] = m_memoryTracker->OnAlloc(pn, 0);
#endif

		return pn;
	}

	inline void Free(Type * pNode)
	{
#ifdef MEMORY_LEAK_DETECT
		DEBUG_INFO_MAP_ITER it = m_debugInfo.find(pNode);
		IF_OK (it != m_debugInfo.end())
			m_memoryTracker->OnDealloc(pNode, it->second);
#endif

		if (!m_pFreeBand->FreeObject(pNode))
		{
			if( !m_pFreeBand->pNext )
				return;

			m_pFreeBand = m_pFreeBand->pNext;
			if(!m_pFreeBand->FreeObject(pNode))	{ }
		}
	}

	inline DWORD GetPoolBasicSize()
	{
		return m_pBandHead->GetMaxNumberOfObjects();
	}

	inline DWORD GetPoolExtendSize()
	{
		return m_dwPoolExtendSize;
	}

	inline DWORD GetNumberOfBands()
	{
		DWORD dwTotalNum = 0;
		CMemTypeBand<Type> *	pHead = m_pBandHead;
		while( pHead )
		{
			++dwTotalNum;
			pHead = pHead->pNext;
		}
		return dwTotalNum;
	}

	inline DWORD GetAvailableNumberOfTypes()
	{
		DWORD dwTotalNum = 0;
		CMemTypeBand<Type> * pHead = m_pBandHead;
		while(pHead)
		{
			dwTotalNum += pHead->GetAvailableNumberOfObjects();
			pHead = pHead->pNext;
		}
		return dwTotalNum;
	}
protected:

	CMemTypeBand<Type> *	m_pBandHead;
	CMemTypeBand<Type> *	m_pFreeBand;
	DWORD					m_dwPoolExtendSize;

#ifdef MEMORY_LEAK_DETECT
	Me_CMemoryTracker*		m_memoryTracker;
#ifdef __SGI_STL_PORT
	//typedef std::hash_map<void* /* pointer */, void* /* debug info */> DEBUG_INFO_MAP;
	typedef std::map<void* /* pointer */, void* /* debug info */> DEBUG_INFO_MAP;
#else
	typedef stdext::hash_map<void* /* pointer */, void* /* debug info */> DEBUG_INFO_MAP;
#endif
	typedef DEBUG_INFO_MAP::iterator DEBUG_INFO_MAP_ITER;
	DEBUG_INFO_MAP m_debugInfo;
#endif
};
