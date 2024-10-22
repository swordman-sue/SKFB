
#include "memory/mempool.h"
#include <stdlib.h>
#include <assert.h>
#include "memory/memdef.h"

MemPool::MemPool(unsigned int alloc_size, unsigned int _increase, const char *class_name)
:m_block_pool(0), m_block_use(0), m_block_max(0), m_malloc_record(0), m_malloc_use(0), m_malloc_max(0),
m_size(alloc_size), m_size_for_increase(_increase), m_record_index(-1), m_name(class_name)
{

}

MemPool::~MemPool()
{
	for (unsigned int i = 0; i < m_malloc_use; ++i)
	{
		::free(m_malloc_record[i]);

		// 这里无法统计释放，不过由于内存池一般都是全局性的，所以，此处的统计也就变得无关紧要了

//#ifdef MEMORY_MONITOR
//		long long increase_size = m_size * m_size_for_increase;
//		memmonitor::AllocStat(-increase_size);
//#endif

	}
	if (m_malloc_record != 0)
	{
		::free(m_malloc_record);
	}
	if (m_block_pool != 0)
	{
		::free(m_block_pool);
	}
}

void * MemPool::Alloc()
{
	void *mem = 0;

	if (m_block_use == 0)
	{
		Increase();
		if (m_block_use == 0)
		{
			return 0;
		}
	}

	mem = m_block_pool[--m_block_use];

#ifdef MEMORY_MONITOR
	if (m_record_index == -1)
	{
		m_record_index = memmonitor::RegisterMemMoniter(m_name);
	}
	memmonitor::UseStat(m_record_index, m_size, 1);
#endif

	return mem;
}

void MemPool::Free(void *mem)
{
	if (m_block_use >= m_block_max)
	{
		unsigned int alloc_max = (m_block_max == 0) ? 4 : (m_block_max * 2);
		m_block_pool = (void **)realloc(m_block_pool, sizeof(void *) * alloc_max);
		m_block_max = alloc_max;
	}
	m_block_pool[m_block_use++] = mem;

#ifdef MEMORY_MONITOR
	assert(m_record_index != -1);
	long long un_size = m_size;
	memmonitor::UseStat(m_record_index, -un_size, -1);
#endif
}

void MemPool::Increase()
{
	size_t increase_size = m_size * m_size_for_increase;
	void *mem = ::malloc(increase_size);
	if (mem == 0) return ;

#ifdef MEMORY_MONITOR
	if (m_record_index == -1)
	{
		m_record_index = memmonitor::RegisterMemMoniter(m_name);
	}
	memmonitor::AllocStat(m_record_index, increase_size, m_size_for_increase);
#endif

	if (m_malloc_use >= m_malloc_max)
	{
		unsigned int alloc_max = (m_malloc_max == 0) ? 4 : (m_malloc_max * 2);
		m_malloc_record = (void **)realloc(m_malloc_record, sizeof(void *) * alloc_max);
		m_malloc_max = alloc_max;
	}
	m_malloc_record[m_malloc_use++] = mem;

	char *p = (char *)mem;
	for (unsigned int i = 0; i < m_size_for_increase; ++i, p += m_size)
	{
		if (m_block_use >= m_block_max)
		{
			unsigned int alloc_max = (m_block_max == 0) ? 4 : (m_block_max * 2);
			m_block_pool = (void **)realloc(m_block_pool, sizeof(void *) * alloc_max);
			m_block_max = alloc_max;
		}
		m_block_pool[m_block_use++] = (void*)p;
	}
}
