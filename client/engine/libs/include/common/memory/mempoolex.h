
#ifndef MEMPOOLEX_H
#define MEMPOOLEX_H

/*
	�����ڴ�أ�ʵ�ּ򵥵Ķ����ڴ滺�����
	�ڴ����Ϊ��ʱ��������ڴ������룬��Ϊ��������Ԥ���ռ䣬�����û��ʹ�õ�ʱ����ֱ���ͷ�

	ע�⣺���̰߳�ȫ

	PS: ���ڴ�ػ��������û��ʹ�õ��ڴ�
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
		int index_in_malloc;			// ��¼�ô���ڴ���m_malloc_record�е��±�
		int index_in_can_used;			// ��¼�ô���ڴ���m_can_use_malloc�е��±�

		int can_use_num;				// �ô���ڴ��ÿ��С���ʹ�ø���
		unsigned short *can_use_index;	// ��¼�ÿ��п�ʹ�õ�С���±�

		// �ýṹ�������m_size_for_increase��SmallMem
	};

	struct SmallMem 
	{
		BigMem *big_mem;	// ��С��Ķ�Ӧ����ַ

		// �ýṹ�������ʵ�ʸ��ⲿ�õ��ڴ棬��СΪm_size
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

