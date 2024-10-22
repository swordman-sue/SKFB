#ifndef QUERYPOOLDEFINE_H_
#define QUERYPOOLDEFINE_H_

#include "../ToolKit/MemoryPool.h"
template<class Type>
class QUERY_POOL
{
public:
	enum { eDEFUALT_POOL_SIZE = 100, };

	QUERY_POOL( int size = eDEFUALT_POOL_SIZE )
	{
		m_pool.Initialize( size );
	}

	~QUERY_POOL()
	{
		m_pool.Release();
	}

	Type * Alloc()
	{
		return m_pool.Alloc();
	}

	VOID Free( Type * pType )
	{
		m_pool.Free( pType );
	}
private:
	CMemoryPoolFactory<Type> m_pool;
};

#define _DECLARE_QUERY_POOL( CLASSNAME )			\
	private:										\
	static QUERY_POOL<CLASSNAME> m_Pool;			\
	public:											\
	static CLASSNAME * ALLOC()						\
{													\
	CLASSNAME* p = m_Pool.Alloc();					\
	p->InitEx();									\
	return p;										\
}													\
	static VOID FREE( CLASSNAME * pObject )			\
{													\
	m_Pool.Free(pObject);							\
}													\
	VOID Release()									\
{													\
	m_Pool.Free(this);								\
}

#define _IMPL_QUERY_POOL( CLASSNAME )				\
	QUERY_POOL<CLASSNAME> CLASSNAME::m_Pool;

#endif
