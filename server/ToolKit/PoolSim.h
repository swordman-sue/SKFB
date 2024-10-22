////////////////////////////////////////////////////////////////////////////////
//	Filename     :PoolSim.h
//	Created by   :dcc
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////

#pragma once

#if (!defined(_MT))
#   error Please define _MT
#endif

#pragma warning( push )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4511 )
#pragma warning( disable : 4512 )
#pragma warning( disable : 4311 )

#include <hash_map>
#include <set>

///////////////////////////////////////////////////////////////////////////////////////
template<class T>
class PoolSim
{
	typedef T       Element,*ElementPtr;
public:
	typedef struct _PoolItem
	{
		T			_Data;
		int			_nRef;
		_PoolItem*	_pNext;

		_PoolItem()
		{
			_nRef = 0;
			_pNext = NULL;
		}
		
	}POOLITEM,*LPPOOLITEM;
	
	//---------------------------------------
public:
	explicit PoolSim();
	virtual ~PoolSim();

	void	Initialize(unsigned int nStart, unsigned int nIncrement , unsigned int nMaximum = 0xffffffff);
	bool	Select(ElementPtr &pElement);
	void	Release(ElementPtr pElement);
	void	ReleaseAll();

	LONG	GetCurrentSize()
	{
		CLock l(&m_csTotolLock);
		return m_nCurrent;
	};
	void GetResource(int &nMax, int &nCurrent, int &nFree)
	{
		nMax = this->m_nMaxCount;
		nCurrent = this->m_nCurrent;
		nFree = this->m_nFreeCount;
		if (nFree < 0)
			nFree = 0;
	}

protected:
	UINT	AddNewItem(UINT uCount);
	void	PushItemToFreeList(LPPOOLITEM pItem);
	LPPOOLITEM PopNewItem()
	{
		LPPOOLITEM pItem;

		pItem = MYNEW POOLITEM;

		OnCreate(pItem);

		m_csTotolLock.Lock();
		m_TotolMap.insert(pItem);

		m_nCurrent ++;
		m_csTotolLock.Unlock();

		return pItem;
	}
	LPPOOLITEM PopItemFromFreeList()
	{
		LPPOOLITEM pItem;

		m_csFreeLock.Lock();
		if (NULL == m_pFreeList)
		{
			m_csFreeLock.Unlock();

			pItem = PopNewItem();
		}
		else
		{
			pItem = m_pFreeList;

			m_pFreeList = m_pFreeList->_pNext;
			pItem->_pNext = NULL;

			m_nFreeCount --;
			m_csFreeLock.Unlock();
		}	

		return pItem;
	}
	virtual void OnDestroy(LPPOOLITEM){}
	virtual void OnCreate(LPPOOLITEM){}
	virtual void OnRelease(LPPOOLITEM){}
	virtual void OnSelect(LPPOOLITEM){}

protected:
	UINT	m_nIncStep;		// 每次添加的数量
	UINT	m_nMaxCount;	// 最大装载的数量,cache的数量,超过的,使用完立即删除
	UINT	m_nCurrent;		// 池中当前分配的资源数量	
	UINT	m_nFreeCount;	// 空闲表内的数据数量

	CLockObj	    m_csFreeLock;	// 空闲表
	LPPOOLITEM		m_pFreeList;
	
	CLockObj	    m_csUsedLock;
	stdext::hash_map<UINT32,LPPOOLITEM> m_UsedMap; // 正在使用的表

	CLockObj		m_csTotolLock;
	std::set<LPPOOLITEM> m_TotolMap;		// 总表
};
///////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------- 
// Function name   : PoolSim<T>::PoolSim
// Return type     : 
// Description     : 
// ----------------------------------------------- 
template<class T>
PoolSim<T>::PoolSim()
{
	m_nCurrent = 0;
	m_nFreeCount = 0;
	m_pFreeList = NULL;

	m_nIncStep = 1;
	m_nMaxCount = 0xffffffff;
}

// ----------------------------------------------- 
// Function name   : PoolSim<T>::~PoolSim
// Return type     : 
// Description     : 
// ----------------------------------------------- 
template<class T>
PoolSim<T>::~PoolSim()
{
	ReleaseAll();
}

// ----------------------------------------------- 
// Function name   : PoolSim<T>::ReleaseAll
// Return type     : void 
// Description     : 
// ----------------------------------------------- 
template<class T>
void PoolSim<T>::ReleaseAll()
{
	LPPOOLITEM lpItem;

	m_nMaxCount = 0;
	m_nCurrent = 0;
	m_nFreeCount = 0;

	m_csFreeLock.Lock();
	m_pFreeList = NULL;
	m_csFreeLock.Unlock();

	m_csUsedLock.Lock();
	m_UsedMap.clear();
	m_csUsedLock.Unlock();

	std::set<LPPOOLITEM>::iterator it;

	m_csTotolLock.Lock();

	for (it = m_TotolMap.begin();it != m_TotolMap.end();++ it)	
	{
		lpItem = *it;
		if (lpItem->_nRef > 0)
		{
			DebugMsgOutA(CON_ERROR, "[%s]数据还有%d在使用!\n",__FUNCTION__,lpItem->_nRef);
		}
		OnDestroy(lpItem);		
		delete lpItem;
	}
	m_TotolMap.clear();

	m_csTotolLock.Unlock();	
}

// ----------------------------------------------- 
// Function name   : PoolSim<T>::Initialize
// Return type     : void 
// Argument        : unsigned int nStart 开始拥有的空闲数据
// Argument        : unsigned int nIncrement 每次增量
// Argument        : unsigned int nMaximum 最大量
// Description     : 初始化
// ----------------------------------------------- 
template<class T>
void PoolSim<T>::Initialize(unsigned int nStart, unsigned int nIncrement , unsigned int nMaximum)
{
	m_nIncStep = nIncrement;
	m_nMaxCount = nMaximum;

	AddNewItem(nStart);
}
// ----------------------------------------------- 
// Function name   : PoolSim<T>::AddItemToFreeList
// Return type     : void 
// Argument        : PoolSim<T>::LPPOOLITEM pItem
// Description     : 
// ----------------------------------------------- 
template<class T>
void PoolSim<T>::PushItemToFreeList(LPPOOLITEM pItem)
{
	m_csFreeLock.Lock();
	if (NULL == m_pFreeList)
	{
		m_pFreeList = pItem;
	}
	else
	{
		pItem->_pNext = m_pFreeList;
		m_pFreeList = pItem;
	}
	m_nFreeCount ++;
	m_csFreeLock.Unlock();
}

// ----------------------------------------------- 
// Function name   : PoolSim<T>::AddFreeItem
// Return type     : UINT 
// Argument        : UINT uCount
// Description     : 添加一定数量的空闲数据
// ----------------------------------------------- 
template<class T>
UINT PoolSim<T>::AddNewItem(UINT uCount)
{
	UINT i; 
	LPPOOLITEM pItem;

	if (0xffffffff != m_nMaxCount && m_nCurrent >= m_nMaxCount)
		return 0;

	if (0xffffffff != m_nMaxCount)
	{
		if (m_nCurrent + uCount >= m_nMaxCount)
		{
			uCount = m_nMaxCount - m_nCurrent;
		}
	}

	for (i = 0;i < uCount;i ++)
	{
		pItem = PopNewItem();

		PushItemToFreeList(pItem);
	}

	return uCount;
}
// ----------------------------------------------- 
// Function name   : PoolSim<T>::Select
// Return type     : bool 
// Argument        : ElementPtr &pElement
// Description     : 获得一个数据 
// ----------------------------------------------- 
template<class T>
bool PoolSim<T>::Select(ElementPtr &pElement)
{
	// 从池中选择一个
	LPPOOLITEM pItem;

	do
	{
		pItem = PopItemFromFreeList();

		if (pItem->_nRef >= 1)
		{
			DebugMsgOutA(CON_ERROR, "[%s] 数据重复用%d!\n",__FUNCTION__,pItem->_nRef);
			
			ASSERT(0);
			continue;
		}
		else
			break;
	}while(TRUE);

	OnSelect(pItem);
	pItem->_nRef ++;

	std::pair<stdext::hash_map<UINT32,LPPOOLITEM>::iterator,bool> ret;

	m_csUsedLock.Lock();
	ret = m_UsedMap.insert(std::make_pair((UINT32)&pItem->_Data,pItem));
	if (!ret.second)
	{
		DebugMsgOutA(CON_ERROR, "[%s] 数据重复用!\n",__FUNCTION__);
	}
	m_csUsedLock.Unlock();

	pElement = &pItem->_Data;

	return true;
}

// ----------------------------------------------- 
// Function name   : PoolSim<T>::Release
// Return type     : void 
// Argument        : ElementPtr pElement
// Description     : 
// ----------------------------------------------- 
template<class T>
void PoolSim<T>::Release(ElementPtr pElement)
{
	stdext::hash_map<UINT32,LPPOOLITEM>::iterator it;
	LPPOOLITEM pItem;

	m_csUsedLock.Lock();
	it = m_UsedMap.find((UINT32)pElement);
	if (it == m_UsedMap.end())
	{
		m_csUsedLock.Unlock();
		return;
	}
	pItem = it->second;

	m_UsedMap.erase(it);
	m_csUsedLock.Unlock();

	//pItem->_Data.OnRelease();
	OnRelease(pItem);
	pItem->_nRef --;

	if (pItem->_nRef > 0)
		DebugMsgOutA(CON_ERROR, "[%s] 数据还有%d在使用!\n",__FUNCTION__,pItem->_nRef);

	if (pItem->_nRef <= 0 && 0xffffffff != m_nMaxCount &&  m_nCurrent > m_nMaxCount)
	{
		OnDestroy(pItem);
		
		m_csTotolLock.Lock();
		m_TotolMap.erase(pItem);
		m_nCurrent --;
		m_csTotolLock.Unlock();

		delete pItem;
	}
	else
		PushItemToFreeList(pItem);
}
#pragma warning( pop )
///////////////////////////////////////////////////////////////////////////////////////
