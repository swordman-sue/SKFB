
#include "jobrecv.h"
#include <stdlib.h>

#include "../../common/platform/thread/mutex.h"
#include "../../common/memory/mempool.h"
#include "../../common/memory/msgmem.h"

#ifdef USE_PROTOBUF_FOR_NET_MSG
JobRecv::JobRecv(NetID netid, NetMsgHead* pMsgHead, MsgMem *data, MsgLen len)
: m_data(data)
, m_length(len)
, m_netid(netid)
, m_stMsgHead(*pMsgHead)
{

}

void JobRecv::Invoke(INetworkCallback *callback)
{
	// 回调
	callback->OnRecv(m_netid, &m_stMsgHead, (const char *)m_data, (unsigned int)m_length);
}

#else
JobRecv::JobRecv(NetID netid, unsigned short cmd, MsgMem *data, MsgLen len)
: m_data(data)
, m_length(len)
, m_netid(netid)
, m_cmd(cmd)
{

}

void JobRecv::Invoke(INetworkCallback *callback)
{
	// 回调
	callback->OnRecv(m_netid, m_cmd, (const char *)m_data, (unsigned int)m_length);
}
#endif

JobRecv::~JobRecv()
{
	delete []m_data;
}

namespace jobrecvmempool
{
	MemPool g_jobrecv_mem_pool(sizeof(JobRecv), 1024, "JobRecv");
	Mutex g_jobrecv_mem_pool_lock;
}

void *JobRecv::operator new(size_t c)
{
	jobrecvmempool::g_jobrecv_mem_pool_lock.Lock();
	void *mem = jobrecvmempool::g_jobrecv_mem_pool.Alloc();
	jobrecvmempool::g_jobrecv_mem_pool_lock.Unlock();
	return mem;
}

void JobRecv::operator delete(void *m)
{
	jobrecvmempool::g_jobrecv_mem_pool_lock.Lock();
	jobrecvmempool::g_jobrecv_mem_pool.Free(m);
	jobrecvmempool::g_jobrecv_mem_pool_lock.Unlock();
}
