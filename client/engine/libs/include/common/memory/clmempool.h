
#ifndef CLMEMPOOL_H
#define CLMEMPOOL_H

/*
	Custom length memory pool
	�ɶ��Ƴ��ȵ��ڴ�أ�ʹ�ö����ڴ��MemPool����ϳɿɶ��Ƴ��ȵ��ڴ��
	
	ʹ�ã�����ʹ��Init����ʼ�����Ƴ��ȣ����ǵ��ڴ��ʹ��ʱ������Ϊȫ�����ݴ��ڣ��޷�����Init�������ڴ�
	����£���̳и��࣬������Ĺ��캯���е���CLMemPool::Init����
	
	PS��1������sizeΪ0ʱҲΪ��Ч�ռ�
		2��Init����ʹ��Alloc��Free���̰߳�ȫ
*/

#define MEMPOOL_USE MemPoolEx

class MEMPOOL_USE;
class Mutex;

class CLMemPool
{
public:
	CLMemPool(const char *name);
	virtual ~CLMemPool();

	/*
		����һ�������б���ʼ���ڴ��
		@length_list��ÿ���ڴ�ض�Ӧ������Ķ����ڴ�Ϊlength_list�еĶ�ӦԪ�ء�ע�⣺length_listԪ�����ݱ�����������
		@n: length_list���鳤��
		@increase: �����ڴ�غľ�ʱ��Ԥ���ڴ�����
	*/
	void Init(unsigned int *length_list, unsigned int n, unsigned int increase=32);

	void * Alloc(unsigned int size);
	void Free(void *mem);
	void *ReAlloc(void *men, unsigned int size);


private:
	CLMemPool(const CLMemPool& _p);
	CLMemPool &operator=(const CLMemPool& _p);

private:
	unsigned int *m_length_list;
	MEMPOOL_USE **m_pool_list;
	unsigned int m_pool_num;

	Mutex *m_pool_lock;

	int m_record_index;
	const char *m_name;
};

#endif


