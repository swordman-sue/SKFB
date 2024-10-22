
#include "memory/clmempool.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "memory/memdef.h"
#include "memory/mempoolex.h"
#include "memory/mempool.h"
#include "platform/thread/mutex.h"

#ifdef MEMORY_MONITOR
void *AllocFromMem(int record_index, size_t size);
void FreeToMem(int record_index, void *mem);
#endif

CLMemPool::CLMemPool(const char *name):m_length_list(0), m_pool_list(0), m_pool_num(0), m_pool_lock(0), m_record_index(-1), m_name(name)
{
}

CLMemPool::~CLMemPool()
{
	for (unsigned int i = 0; i < m_pool_num; ++i)
	{
		if (m_pool_list[i] != 0)
		{
			delete m_pool_list[i];
		}
	}

	if (m_pool_list != 0)
	{
		::free(m_pool_list);
	}
	if (m_length_list != 0)
	{
		::free(m_length_list);
	}
	if (m_pool_lock != 0)
	{
		delete[] m_pool_lock;
	}
}

void CLMemPool::Init(unsigned int *length_list, unsigned int n, unsigned int increase)
{
	assert(n < 256);

	m_pool_num = n;
	m_length_list = (unsigned int*)malloc(sizeof(unsigned int) * m_pool_num);
	memcpy(m_length_list, length_list, sizeof(unsigned int) * m_pool_num);

	m_pool_list = (MEMPOOL_USE**)malloc(sizeof(MEMPOOL_USE*) * m_pool_num);
	memset(m_pool_list, 0, sizeof(MEMPOOL_USE*) * m_pool_num);

	for (unsigned int i = 0; i < m_pool_num; ++i)
	{
		assert(m_length_list[i] != 0);

		// 每个内存池为实际使用内存长度加上sizeof(unsigned int)，前面的sizeof(unsigned int)字节用于存放所申
		// 请内存在该内存池列表中对应的下标
		m_pool_list[i] = new MEMPOOL_USE(m_length_list[i] + sizeof(unsigned int), increase, m_name);
	}

	m_pool_lock = new Mutex[n];
}

void * CLMemPool::Alloc(unsigned int size)
{
	unsigned int i = 0;
	while(i < m_pool_num && m_length_list[i] < size) ++i;
	
	char *mem = 0;
	if (i < m_pool_num)
	{
		m_pool_lock[i].Lock();
		mem = (char *)m_pool_list[i]->Alloc();
		m_pool_lock[i].Unlock();
	}
	else
	{
		// size过大, 直接从物理内存申请,同样多申请sizeof(unsigned int)个字节
		size_t alloc_size = size + sizeof(unsigned int);

		#ifdef MEMORY_MONITOR
		if (m_record_index == -1)
		{
			m_record_index = memmonitor::RegisterMemMoniter(m_name);
		}
		mem = (char *)AllocFromMem(m_record_index, alloc_size);
		#else
		mem = (char *)::malloc(alloc_size);
		#endif

		if (mem == 0) return 0;
	}

	// 前面sizeof(unsigned int)个字节存放下标
	unsigned int *index = (unsigned int *)mem;
	*index = i;

	return (mem + sizeof(unsigned int));
}

void CLMemPool::Free(void *mem) 
{
	if (mem == 0) return ;

	// 取出前面sizeof(unsigned int)个字节转换为下标
	char *raw_mem = ((char *)mem) - sizeof(unsigned int);
	unsigned int index = *(unsigned int*)raw_mem;

	if (index == m_pool_num)
	{
		#ifdef MEMORY_MONITOR
		assert(m_record_index != -1);
		FreeToMem(m_record_index, raw_mem);
		#else
		::free(raw_mem);
		#endif
	}
	else
	{
		m_pool_lock[index].Lock();
		m_pool_list[index]->Free(raw_mem);
		m_pool_lock[index].Unlock();
	}
}

void *CLMemPool::ReAlloc(void *mem, unsigned int size)
{
	// 如果需要size为0则返回0，直接把该if解注释即可
	//if (size == 0)
	//{
	//	if (mem != 0) Free(mem);
	//	return 0;
	//}

	unsigned int old_index = -1;
	unsigned int old_length = 0;

	if (mem != 0)
	{
		char *raw_mem = ((char *)mem) - sizeof(unsigned int);
		old_index = *(unsigned int*)raw_mem;
		old_length = m_length_list[old_index];
	}

	if (size <= old_length)
	{
		if (mem != 0)
		{
			// 如果新申请的比原来的小, 则直接 return 原来的
			return mem;
		}
		return Alloc(size);
	}

	// 新申请的size比原来的大

	// 判断新申请的size所在的区间
	unsigned int new_index = 0;
	while(new_index < m_pool_num && m_length_list[new_index] < size) ++new_index;

	// 区间相同则直接return原来的
	if (old_index == new_index) return mem;
	// 否则重新申请

	void *new_mem = Alloc(size);
	if (new_mem == 0)
	{
		// 申请不到，原mem保留，return 0
		return 0;
	}

	if (mem != 0)
	{
		memcpy(new_mem, mem, old_length);
		Free(mem);
	}

	return new_mem;
}

#ifdef MEMORY_MONITOR
void *AllocFromMem(int record_index, size_t size)
{
	// 直接从物理内存多申请sizeof(unsigned int)个字节放申请大小，用于DEBUG内存统计
	size_t alloc_size = size + sizeof(unsigned int);
	char *mem = (char*)::malloc(alloc_size);
	memmonitor::AllocStat(record_index, alloc_size, 1);
	memmonitor::UseStat(record_index, alloc_size, 1);

	unsigned int *index = (unsigned int *)mem;
	*index = (unsigned int)alloc_size;

	return (mem + sizeof(unsigned int));
}

void FreeToMem(int record_index, void *mem)
{
	// 从内存前sizeof(unsigned int)个字节拿申请大小，用于DEBUG内存统计
	char *raw_mem = ((char *)mem) - sizeof(unsigned int);
	long long alloc_size = (long long)*(unsigned int*)raw_mem;
	memmonitor::AllocStat(record_index, -alloc_size, -1);
	memmonitor::UseStat(record_index, -alloc_size, -1);
	::free(raw_mem);
}
#endif
