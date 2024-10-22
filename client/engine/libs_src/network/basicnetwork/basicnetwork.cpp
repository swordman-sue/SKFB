
#include "basicnetwork.h"
#include "tcphandler.h"
#include "job.h"

#include "../../common/platform/system.h"

#if defined(ANDROID)
#include <android/log.h>
#endif

#ifdef USE_PROTOBUF_FOR_NET_MSG
char* BasicNetwork::pb_data_for_net_msg_head = nullptr;
unsigned int BasicNetwork::pb_data_len_for_net_msg_head = 0;
pbc_env* BasicNetwork::pbc_env_for_net_msg_head = nullptr;
#endif

BasicNetwork::BasicNetwork(JobQueue *job_queue):m_is_exit(true),m_job_queue(job_queue)
{
	InitSocket();
}

BasicNetwork::~BasicNetwork()
{
	Clear();

#ifdef USE_PROTOBUF_FOR_NET_MSG
	if (nullptr != pb_data_for_net_msg_head)
	{
		delete[] pb_data_for_net_msg_head;
		pb_data_for_net_msg_head = nullptr;
	}

	if (nullptr != pbc_env_for_net_msg_head)
	{
		pbc_delete(pbc_env_for_net_msg_head);
		pbc_env_for_net_msg_head = nullptr;
	}
#endif
}

void BasicNetwork::Start()
{
	if (m_is_exit)
	{
		m_is_exit = false;
		m_work_thread.Run(ThreadFunc, this, 0);
	}
}

void BasicNetwork::Stop()
{
	if (!m_is_exit)
	{
		m_is_exit = true;
		m_work_thread.Join();
	}
}

NetID BasicNetwork::Add(BasicNetworkHandler *handler)
{
	m_register_table_mutex.Lock();

	RegisterTableItem item;
	item.handler = handler;
	item.write_event = 0;

	NetID netid = m_register_table.Insert(item);
	handler->SetBasicNetwork(this);
	handler->SetNetid(netid);

	AddSocket(handler, netid);

	m_register_table_mutex.Unlock();
	return netid;
}

void BasicNetwork::Remove(NetID netid)
{
	m_dirty_queue_mutex.Lock();
	m_dirty_queue.push_back(netid);
	m_dirty_queue_mutex.Unlock();
}

void BasicNetwork::Clear()
{
	Stop();

	m_register_table_mutex.Lock();
	ReleaseSocket();
	for (RegisterTableIter iter = m_register_table.Beg() ; iter != m_register_table.End(); ++iter)
	{
		iter->handler->OnClose();
		delete iter->handler;
	}
	m_register_table_mutex.Unlock();

	m_dirty_queue_mutex.Lock();
	m_dirty_queue.clear();
	m_dirty_queue_mutex.Unlock();

	while (!m_job_to_push.empty())
	{
		Job *job = m_job_to_push.front();
		m_job_to_push.pop();
		delete job;
	}

	m_is_exit = false;
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
bool BasicNetwork::SendPackage(NetID netid, const char* header_data, unsigned short header_data_len, const char *buffer, unsigned short len)
{
	m_register_table_mutex.Lock();

	RegisterTableIter iter = m_register_table.Find(netid);
	if (iter == m_register_table.End())
	{
		m_register_table_mutex.Unlock();
		return false;
	}

	bool ret = false;
	TcpHandler *tcphandler = (TcpHandler*)(iter->handler);
	if (tcphandler != 0 && tcphandler->GetType() == BasicNetworkHandler::HT_TCP)
	{
		tcphandler->SendPackage(header_data, header_data_len, buffer, len);
		if (iter->write_event == 0)
		{
			RegisterSocketWrite(tcphandler, netid);
		}
		iter->write_event += len + sizeof(NetMsgBasic);
		ret = true;
	}

	m_register_table_mutex.Unlock();

	return ret;
}
#else
bool BasicNetwork::SendPackage(NetID netid, unsigned short cmd, const char *buffer, unsigned int len)
{
	m_register_table_mutex.Lock();

	RegisterTableIter iter = m_register_table.Find(netid);
	if (iter == m_register_table.End())
	{
		m_register_table_mutex.Unlock();
		return false;
	}

	bool ret = false;
	TcpHandler *tcphandler = (TcpHandler*)(iter->handler);
	if (tcphandler != 0 && tcphandler->GetType() == BasicNetworkHandler::HT_TCP)
	{
		tcphandler->SendPackage(cmd, buffer, len);
		if (iter->write_event == 0)
		{
			RegisterSocketWrite(tcphandler, netid);
		}
		iter->write_event += len + sizeof(NetMsgBasic);
		ret = true;
	}

	m_register_table_mutex.Unlock();

	return ret;
}
#endif

const char *BasicNetwork::GetPeerName(NetID netid)
{
	char *ip_str = 0;

	m_register_table_mutex.Lock();

	RegisterTableIter iter = m_register_table.Find(netid);
	if (iter != m_register_table.End())
	{
		TcpHandler *tcphandler = (TcpHandler*)(iter->handler);
		if (tcphandler != 0 && tcphandler->GetType() == BasicNetworkHandler::HT_TCP)
		{
			sockaddr_in addr;
			if (PISocket::PeerName(tcphandler->GetSocket(), &addr) != SOCKET_ERROR)
			{
				//IP ip = ntohl(addr.sin_addr.s_addr);
				//Port port = ntohs(addr.sin_port);
				ip_str = inet_ntoa(addr.sin_addr);
			}
		}
	}
	m_register_table_mutex.Unlock();

	return ip_str;
}

void BasicNetwork::DeleteDirtySocket()
{
	//清理dirty socket
	DirtyQueue temp_queue;
	m_dirty_queue_mutex.Lock();
	m_dirty_queue.swap(temp_queue);
	m_dirty_queue_mutex.Unlock();

	if (temp_queue.size() != 0)
	{
		m_register_table_mutex.Lock();
		for (DirtyQueue::iterator iter = temp_queue.begin(); iter != temp_queue.end(); ++ iter)
		{
			RegisterTableIter item_erase = m_register_table.Find(*iter);
			if (item_erase == m_register_table.End())
			{
				continue;
			}

			//先删除BasicNetwork中的信息，再调用OnClose
			BasicNetworkHandler *handler = item_erase->handler;
			SOCKET sock = handler->GetSocket();
			m_register_table.Erase(*iter);
			handler->OnClose();				// 这里有点问题，这里限制了OnClose里面不能调用BasicNetwork中加锁的接口!*****************************
											// 因为在Linux下pthread_mutex_t是非递归锁
			RemoveSocket(sock);
			delete handler;
		}
		m_register_table_mutex.Unlock();
	}
}

void BasicNetwork::PushJobToInvoke()
{
	while (!m_job_to_push.empty())
	{
		Job *job = m_job_to_push.front();

		if (m_job_queue->TryPush(job))
		{
			m_job_to_push.pop();
		}
		else
		{
			break;
		}
	}
}

void BasicNetwork::PushJob(Job * job)
{
	m_job_to_push.push(job);
}

bool BasicNetwork::RegisterWrite(NetID netid, int num)
{
	m_register_table_mutex.Lock();

	BasicNetwork::RegisterTableIter iter = m_register_table.Find(netid);
	if (iter == m_register_table.End())
	{
		m_register_table_mutex.Unlock();
		return false;
	}

	if (iter->write_event == 0)
	{
		RegisterSocketWrite(iter->handler, netid);
	}
	iter->write_event += num;

	m_register_table_mutex.Unlock();

	return true;
}

bool BasicNetwork::UnregisterWrite(NetID netid, int num)
{
	m_register_table_mutex.Lock();

	BasicNetwork::RegisterTableIter iter = m_register_table.Find(netid);
	if (iter == m_register_table.End())
	{
		m_register_table_mutex.Unlock();
		return false;
	}

	iter->write_event -= num;
	if (iter->write_event == 0)
	{
		UnregisterSocketWrite(iter->handler, netid);
	}

	m_register_table_mutex.Unlock();

	return true;
}

//工作线程的主循环函数
DWORD BasicNetwork::ThreadFunc(void *param)
{
	BasicNetwork *pthis = (BasicNetwork*) param;
	return pthis->WorkFunc();
}

DWORD BasicNetwork::WorkFunc()
{
	BasicNetworkHandler *vector_can_read[MAX_HANDEL_SIZE];
	BasicNetworkHandler *vector_can_write[MAX_HANDEL_SIZE];

	for (;;)
	{
		if (m_is_exit)
		{
			break;
		}

		DeleteDirtySocket();
		PushJobToInvoke();

		m_register_table_mutex.Lock();
		if (m_register_table.Size() == 0)
		{
			m_register_table_mutex.Unlock();
			PISleep(10);
			continue;
		}
		m_register_table_mutex.Unlock();

		int read_size = 0;
		int write_size = 0;
		PollSocket(vector_can_read, MAX_HANDEL_SIZE, &read_size, vector_can_write, MAX_HANDEL_SIZE, &write_size);

		//处理事件
		for (int i = 0; i < read_size; ++i)
		{
			vector_can_read[i]->OnCanRead();
		}

		for (int i = 0; i < write_size; ++i)
		{
			vector_can_write[i]->OnCanWrite();
		}
	}

	DeleteDirtySocket();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// 以下为平台网络io模型相关代码 ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SELECT_NETWORK
//由构造函数调用
void BasicNetwork::InitSocket()
{
	ReleaseSocket();
}

//由Clear调用
void BasicNetwork::ReleaseSocket()
{	
	m_max_sock = SOCKET_ERROR;
	FD_ZERO(&m_fd_read);
	FD_ZERO(&m_fd_write);
}

//由Add调用
void BasicNetwork::AddSocket(BasicNetworkHandler* handler, NetID netid)
{
	SOCKET sock_fd = handler->GetSocket();
	FD_SET(sock_fd, &m_fd_read);
	m_max_sock = ((m_max_sock) > (int)(sock_fd)) ? (m_max_sock) : (int)(sock_fd);
}

//由DeleteDirtySocket调用
void BasicNetwork::RemoveSocket(SOCKET sock_remove)
{
	FD_CLR(sock_remove, &m_fd_read);
	FD_CLR(sock_remove, &m_fd_write);
	if (sock_remove == m_max_sock)
	{
		//需要重新找出最大的sock
		m_max_sock = -1;
		for (RegisterTableIter iter = m_register_table.Beg() ; iter != m_register_table.End(); ++iter)
		{
			int sock_iter = (int)iter->handler->GetSocket();
			m_max_sock = ((m_max_sock) > sock_iter ? (m_max_sock) : sock_iter);
		}
	}
}

int BasicNetwork::GetSocketFd(fd_set &fdread , fd_set &fdwrite)
{
	fdread = m_fd_read;
	fdwrite = m_fd_write;
	return m_max_sock;
}

//由SendPackage调用
void BasicNetwork::RegisterSocketWrite(BasicNetworkHandler* handler, NetID netid)
{
	FD_SET(handler->GetSocket(), &m_fd_write);
}

//由UnRegisterWrite调用
void BasicNetwork::UnregisterSocketWrite(BasicNetworkHandler* handler, NetID netid)
{
	FD_CLR(handler->GetSocket(), &m_fd_write);
}

void BasicNetwork::PollSocket(BasicNetworkHandler *readhandler[], int max_read_size, int *out_read_size, BasicNetworkHandler *writehandler[], int max_write_size, int *out_write_size)
{
	struct timeval tv;	//超时参数
	tv.tv_sec = 0;		//秒
	tv.tv_usec = 10000;	//微秒,10毫秒

	m_register_table_mutex.Lock();
	int max_sock =  GetSocketFd(m_tmp_fd_read, m_tmp_fd_write);
	m_register_table_mutex.Unlock();

	int ret = select(max_sock + 1, &m_tmp_fd_read,  &m_tmp_fd_write, 0, &tv);

	int read_size = 0;
	int write_size = 0;

	if (ret > 0)
	{
		m_register_table_mutex.Lock();
		for (BasicNetwork::RegisterTableIter iter = m_register_table.Beg() ; iter != m_register_table.End(); ++iter)
		{
			SOCKET sock_iter = iter->handler->GetSocket();
			if (FD_ISSET(sock_iter, &m_tmp_fd_read) && read_size < max_read_size)
			{
				//检查断开在OnCanRead中完成
				readhandler[read_size++] = iter->handler;
			}

			if (FD_ISSET(sock_iter, &m_tmp_fd_write) && write_size < max_write_size)
			{
				writehandler[write_size++] = iter->handler;
			}
		}
		m_register_table_mutex.Unlock();
	}

	*out_read_size = read_size;
	*out_write_size = write_size;
}
#endif

#ifdef EPOLL_NETWORK
void BasicNetwork::InitSocket()
{
	m_epfd = epoll_create(MAX_HANDEL_SIZE);

	memset(m_last_epoll_ctl_error, 0, sizeof(m_last_epoll_ctl_error));
	m_last_epoll_ctl_error_time = 0;
	m_epoll_ctl_count = 0;
}

void BasicNetwork::ReleaseSocket()
{	
	PISocket::Close(m_epfd);
}

void BasicNetwork::AddSocket(BasicNetworkHandler* handler, NetID netid)
{
	SOCKET sock_fd = handler->GetSocket();
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.u32 = netid;
	if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
	{
		// 添加失败
		DealEPollCtlError(EPOLL_CTL_ADD, sock_fd);
	}
}

void BasicNetwork::RemoveSocket(SOCKET sock_remove)
{
	struct epoll_event ev;
	if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, sock_remove, &ev) == -1)
	{
		// 删除失败
		DealEPollCtlError(EPOLL_CTL_DEL, sock_remove);
	}
}

void BasicNetwork::RegisterSocketWrite(BasicNetworkHandler* handler, NetID netid)
{
	SOCKET sock = handler->GetSocket();
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.u32 = netid;
	if (epoll_ctl(m_epfd, EPOLL_CTL_MOD, sock, &ev) == -1)
	{
		// 注册写失败
		DealEPollCtlError(EPOLL_CTL_MOD, sock);
	}
}

void BasicNetwork::UnregisterSocketWrite(BasicNetworkHandler* handler, NetID netid)
{
	SOCKET sock = handler->GetSocket();
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.u32 = netid;
	if (epoll_ctl(m_epfd, EPOLL_CTL_MOD, sock, &ev) == -1)
	{
		// 反注册写失败
		DealEPollCtlError(EPOLL_CTL_MOD, sock);
	}
}

void BasicNetwork::DealEPollCtlError(int op, SOCKET fd)
{
	/*
	EBADF  epfd or fd is not a valid file descriptor.

	EEXIST op was EPOLL_CTL_ADD, and the supplied file descriptor fd is already
	registered with this epoll instance.

	EINVAL epfd is not an epoll file descriptor, or fd is the same as epfd, or the
	requested operation op is not supported by this interface.

	ENOENT op was EPOLL_CTL_MOD or EPOLL_CTL_DEL, and fd is not registered with
	this epoll instance.

	ENOMEM There was insufficient memory to handle the requested op control
	operation.

	ENOSPC The limit imposed by /proc/sys/fs/epoll/max_user_watches was
	encountered while trying to register (EPOLL_CTL_ADD) a new file
	descriptor on an epoll instance.  See epoll(7) for further details.

	EPERM  The target file fd does not support epoll.
	*/

	const char *opstr = "op_unknow";
	switch (op)
	{
	case EPOLL_CTL_ADD:	{ opstr = "EPOLL_CTL_ADD"; break; }
	case EPOLL_CTL_DEL:	{ opstr = "EPOLL_CTL_DEL"; break;}
	case EPOLL_CTL_MOD:	{ opstr = "EPOLL_CTL_MOD"; break;}
	default:			{ opstr = "op_unknow"; break;}
	}

	const char *errstr = "err_unknow";
	switch (PISocket::Errno())
	{
	case EBADF:		{ errstr = "EBADF"; break; }
	case EEXIST:	{ errstr = "EEXIST"; break;}
	case EINVAL:	{ errstr = "EINVAL"; break;}
	case ENOENT:	{ errstr = "ENOENT"; break;}
	case ENOMEM:	{ errstr = "ENOMEM"; break;}
	case ENOSPC:	{ errstr = "ENOSPC"; break;}
	case EPERM:		{ errstr = "EPERM"; break; }
	default:		{ errstr = "err_unknow"; break;}
	}

	sprintf(m_last_epoll_ctl_error, "FD:%d %s %s", fd, opstr, errstr);
	m_last_epoll_ctl_error_time = PITime();
	++m_epoll_ctl_count;
}

void BasicNetwork::PollSocket(BasicNetworkHandler *readhandler[], int max_read_size, int *out_read_size, BasicNetworkHandler *writehandler[], int max_write_size, int *out_write_size)
{
	int eret = epoll_wait(m_epfd, m_tmp_event, MAX_HANDEL_SIZE, 1);

	int read_size = 0;
	int write_size = 0;

	if (eret > 0)
	{
		m_register_table_mutex.Lock();
		for (int i = 0; i < eret; ++i)
		{
			if ((m_tmp_event[i].events & EPOLLIN) && read_size < max_read_size)
			{
				NetID netid = m_tmp_event[i].data.u32;
				if (m_register_table.Exist(netid))
				{
					readhandler[read_size++] = m_register_table[netid].handler;
				}
			}
			if ((m_tmp_event[i].events & EPOLLOUT) && write_size < max_write_size)
			{
				NetID netid = m_tmp_event[i].data.u32;
				if (m_register_table.Exist(netid))
				{
					writehandler[write_size++] = m_register_table[netid].handler;
				}
			}
		}
		m_register_table_mutex.Unlock();
	}

	*out_read_size = read_size;
	*out_write_size = write_size;
}

#endif

