
#ifndef MEMPOOLEX_H
#define MEMPOOLEX_H

/*
	队列内存池：实现简单的定长内存缓存机制
	内存队列为空时则从物理内存中申请，并为继续增长预留空间，整大块没有使用的时候，则直接释放

	注意：非线程安全

	PS: 该内存池会回收整块没有使用的内存
*/

class MemPoolEx
{
public:
	MemPoolEx(unsigned int alloc_size, unsigned int _increase, const char *class_name);
	~MemPoolEx();

	void * Alloc();
	void Free(void *mem);

protected:
	void Increase();
	void CheckFree(unsigned int index);
private:
	MemPoolEx(const MemPoolEx &_m);
	MemPoolEx & operator=(const MemPoolEx &_r);

private:
	struct BigMem
	{
		int index_in_malloc;			// 记录该大块内存在m_malloc_record中的下标
		int index_in_can_used;			// 记录该大块内存在m_can_use_malloc中的下标

		int can_use_num;				// 该大块内存的每个小块的使用个数
		unsigned short *can_use_index;	// 记录该块中可使用的小块下标

		// 该结构后面跟着m_size_for_increase块SmallMem
	};

	struct SmallMem 
	{
		BigMem *big_mem;	// 该小块的对应大块地址

		// 该结构后面跟着实际给外部用的内存，大小为m_size
	};

	BigMem **m_malloc;
	unsigned int m_malloc_cur;
	unsigned int m_malloc_max;

	BigMem **m_can_use;
	unsigned int m_can_use_cur;
	unsigned int m_can_use_max;

	unsigned int m_size;
	unsigned int m_size_for_increase;

	int m_record_index;
	const char *m_name;
};

#define REGISTER_MEMPOOL_EX(PoolNameSpace, ClassName, INCREASE_NUM, ClassNameStr) \
namespace PoolNameSpace\
{\
	MemPoolEx g_##ClassName##_mem_pool(sizeof(ClassName), INCREASE_NUM, ClassNameStr);\
}\
void *ClassName::operator new(size_t c)\
{\
	void *mem = PoolNameSpace::g_##ClassName##_mem_pool.Alloc();\
	return mem;\
}\
void ClassName::operator delete(void *m)\
{\
	PoolNameSpace::g_##ClassName##_mem_pool.Free(m);\
}

#define DEFINE_MEMPOOL_EX\
void *	operator new(size_t c);\
void	operator delete(void *m);\

#endif

