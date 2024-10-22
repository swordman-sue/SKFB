
#include "memory/mempoolex.h"
#include <stdlib.h>
#include <assert.h>
#include "memory/memdef.h"

MemPoolEx::MemPoolEx(unsigned int alloc_size, unsigned int _increase, const char *class_name)
:m_malloc(0), m_malloc_cur(0), m_malloc_max(0), m_can_use(0), m_can_use_cur(0), m_can_use_max(0), 
m_size(alloc_size), m_size_for_increase(_increase), m_record_index(-1), m_name(class_name)
{

}

MemPoolEx::~MemPoolEx()
{
	for (unsigned int i = 0; i < m_malloc_cur; ++i)
	{
		::free(m_malloc[i]->can_use_index);
		::free(m_malloc[i]);

		// 这里无法统计释放，不过由于内存池一般都是全局性的，所以，此处的统计也就变得无关紧要了

//#ifdef MEMORY_MONITOR
//		size_t size_smallmem = m_size + sizeof(SmallMem);
//		size_t register_size = size_smallmem * m_size_for_increase;
//		memmonitor::AllocStat(-register_size);
//#endif

	}
	if (m_malloc != 0)
	{
		::free(m_malloc);
	}
	if (m_can_use != 0)
	{
		::free(m_can_use);
	}
}

void * MemPoolEx::Alloc()
{
	BigMem *b_mem = 0;

	if (m_can_use_cur == 0)
	{
		Increase();
		if (m_can_use_cur == 0)
		{
			return 0;
		}
	}

	b_mem = m_can_use[m_can_use_cur - 1];
	--(b_mem->can_use_num);
	unsigned short use_index = b_mem->can_use_index[b_mem->can_use_num];
	

	// 如果can_use_num用完了，则从m_can_use_malloc中去掉
	if (b_mem->can_use_num == 0)
	{
		--m_can_use_cur;
		if (b_mem->index_in_can_used != (int)m_can_use_cur)
		{
			m_can_use[b_mem->index_in_can_used] = m_can_use[m_can_use_cur];
		}
		b_mem->index_in_can_used = -1;
	}

#ifdef MEMORY_MONITOR
	if (m_record_index == -1)
	{
		m_record_index = memmonitor::RegisterMemMoniter(m_name);
	}
	memmonitor::UseStat(m_record_index, m_size + sizeof(SmallMem), 1);
#endif

	SmallMem *s_begin = (SmallMem*)((char*)b_mem + sizeof(BigMem));
	SmallMem *s_mem = (SmallMem*)((char*)s_begin + use_index * (sizeof(SmallMem) + m_size));

	char *p = (char*)s_mem;
	p += sizeof(SmallMem);
	return (void*)p;
}

void MemPoolEx::Free(void *mem)
{
	SmallMem *s_mem = (SmallMem*)((char*)mem - sizeof(SmallMem));

	BigMem *b_mem = s_mem->big_mem;
	SmallMem *s_begin = (SmallMem*)((char*)b_mem + sizeof(BigMem));
	unsigned short s_index = (unsigned short)(((char*)s_mem - (char*)s_begin) / (sizeof(SmallMem) + m_size));

	b_mem->can_use_index[b_mem->can_use_num] = s_index;
	++(b_mem->can_use_num);

	if (b_mem->can_use_num == 1)
	{
		if (m_can_use_cur >= m_can_use_max)
		{
			unsigned int alloc_max = (m_can_use_max == 0) ? 4 : (m_can_use_max * 2);
			m_can_use = (BigMem **)realloc(m_can_use, sizeof(BigMem *) * alloc_max);
			m_can_use_max = alloc_max;
		}
		m_can_use[m_can_use_cur++] = b_mem;
		b_mem->index_in_can_used = m_can_use_cur - 1;
	}

	if (b_mem->can_use_num == (int)m_size_for_increase)
	{
		CheckFree(s_mem->big_mem->index_in_malloc);
	}

#ifdef MEMORY_MONITOR
	assert(m_record_index != -1);
	long long un_size = m_size + sizeof(SmallMem);
	memmonitor::UseStat(m_record_index, -un_size, -1);
#endif
}

void MemPoolEx::Increase()
{
	size_t size_smallmem = m_size + sizeof(SmallMem);
	size_t register_size = size_smallmem * m_size_for_increase;
	size_t increase_size = register_size + sizeof(BigMem);
	BigMem *b_mem = (BigMem*)::malloc(increase_size);
	if (b_mem == 0) return ;

#ifdef MEMORY_MONITOR
	if (m_record_index == -1)
	{
		m_record_index = memmonitor::RegisterMemMoniter(m_name);
	}
	memmonitor::AllocStat(m_record_index, register_size, m_size_for_increase);
#endif

	if (m_malloc_cur >= m_malloc_max)
	{
		unsigned int alloc_max = (m_malloc_max == 0) ? 4 : (m_malloc_max * 2);
		m_malloc = (BigMem **)realloc(m_malloc, sizeof(BigMem *) * alloc_max);
		m_malloc_max = alloc_max;
	}
	m_malloc[m_malloc_cur++] = b_mem;

	if (m_can_use_cur >= m_can_use_max)
	{
		unsigned int alloc_max = (m_can_use_max == 0) ? 4 : (m_can_use_max * 2);
		m_can_use = (BigMem **)realloc(m_can_use, sizeof(BigMem *) * alloc_max);
		m_can_use_max = alloc_max;
	}
	m_can_use[m_can_use_cur++] = b_mem;

	b_mem->can_use_num = m_size_for_increase;
	b_mem->index_in_malloc = m_malloc_cur - 1;
	b_mem->index_in_can_used = m_can_use_cur - 1;
	b_mem->can_use_index = (unsigned short *)::malloc(sizeof(unsigned short) * m_size_for_increase);

	char *p = (char *)((char *)b_mem + sizeof(BigMem));
	for (unsigned int i = 0; i < m_size_for_increase; ++i, p += size_smallmem)
	{
		SmallMem *s_p = (SmallMem *)p;
		s_p->big_mem = b_mem;
		b_mem->can_use_index[i] = i;
	}
}

void MemPoolEx::CheckFree(unsigned int index)
{
	if (index >= m_malloc_cur) return;

	BigMem *b_men = m_malloc[index];
	if (b_men->can_use_num != (int)m_size_for_increase) return;

	--m_can_use_cur;
	if (b_men->index_in_can_used != (int)m_can_use_cur)
	{
		m_can_use[b_men->index_in_can_used] = m_can_use[m_can_use_cur];
		m_can_use[b_men->index_in_can_used]->index_in_can_used = b_men->index_in_can_used;
	}

	--m_malloc_cur;
	if (index != m_malloc_cur)
	{
		m_malloc[index] = m_malloc[m_malloc_cur];
		m_malloc[index]->index_in_malloc = index;
	}

#ifdef MEMORY_MONITOR
	assert(m_record_index != -1);
	long long size_smallmem = m_size + sizeof(SmallMem);
	long long register_size = size_smallmem * m_size_for_increase;
	int un_reg_block = (int)m_size_for_increase;
	memmonitor::AllocStat(m_record_index, -register_size, -un_reg_block);
#endif
	
	::free(b_men->can_use_index);
	::free(b_men);
}
