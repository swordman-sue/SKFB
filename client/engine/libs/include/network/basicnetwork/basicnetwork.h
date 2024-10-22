
#ifndef BASICNETWORK_H
#define BASICNETWORK_H

#include <vector>
#include <queue>

#include "../../common/raobjlist.h"
#include "../../common/platform/thread/mutex.h"
#include "../../common/platform/socket/pisocket.h"
#include "../../common/platform/thread/thread.h"

#include "def.h"
#include "basicnetworkhandler.h"
#include "jobqueue.h"

extern "C"
{
#include "pbc.h"
}

class Job;

#if (defined(_WIN32) || defined(__ANDROID__) || defined(__APPLE__))

#if defined(__ANDROID__) || defined (__APPLE__)
#include <sys/select.h>
#endif
	#ifdef EPOLL_NETWORK
		#undef EPOLL_NETWORK
	#endif

	#ifndef SELECT_NETWORK
		#define SELECT_NETWORK
	#endif
#endif

#ifdef __unix
	#ifndef SELECT_NETWORK
	#ifndef EPOLL_NETWORK
		#define EPOLL_NETWORK
	#endif
	#endif

#ifdef EPOLL_NETWORK
#include <sys/epoll.h>
#endif

#endif

#define MAX_HANDEL_SIZE (8)

class BasicNetwork
{
public:
#ifdef USE_PROTOBUF_FOR_NET_MSG
	//protobuf消息头协议文件数据
	static char* pb_data_for_net_msg_head;
	static unsigned int pb_data_len_for_net_msg_head;
	static pbc_env* pbc_env_for_net_msg_head;
#endif

	BasicNetwork(JobQueue *job_queue);
	~BasicNetwork();

	void Start();
	void Stop();

	NetID Add(BasicNetworkHandler *handler);
	void Remove(NetID netid);
	void Clear();

#ifdef USE_PROTOBUF_FOR_NET_MSG
	bool SendPackage(NetID netid, const char* header_data, unsigned short header_data_len, const char *data, unsigned short len);
#else
	bool SendPackage(NetID netid, unsigned short cmd, const char *data, unsigned int len);
#endif

	const char *GetPeerName(NetID netid);

protected:
	friend class ListenHandler;
	friend class TcpHandler;
	void PushJob(Job * job);
	bool RegisterWrite(NetID netid, int num);
	bool UnregisterWrite(NetID netid, int num);

private:
	struct RegisterTableItem
	{
		BasicNetworkHandler *handler;
		int write_event;

		RegisterTableItem():write_event(0){}
	};
	typedef RAObjList<RegisterTableItem> RegisterTable;
	typedef RAObjList<RegisterTableItem>::Iterator RegisterTableIter;

	typedef std::vector<NetID> DirtyQueue;
	typedef std::queue<Job *> JobTempList;

	RegisterTable m_register_table;
	Mutex m_register_table_mutex;

	DirtyQueue m_dirty_queue;			//延迟删除项
	Mutex m_dirty_queue_mutex;

	Thread	m_work_thread;				//工作线程
	bool m_is_exit;						//线程退出标志

	JobTempList m_job_to_push;
	JobQueue *m_job_queue;

	void DeleteDirtySocket();
	void PushJobToInvoke();
	static DWORD ThreadFunc(void *param); //线程函数
	DWORD WorkFunc();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// 以下为平台网络io模型相关代码 ////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	void InitSocket();
	void ReleaseSocket();
	void AddSocket(BasicNetworkHandler* handler, NetID netid);
	void RemoveSocket(SOCKET sock);
	void RegisterSocketWrite(BasicNetworkHandler *handler, NetID netid);
	void UnregisterSocketWrite(BasicNetworkHandler *handler, NetID netid);
	void PollSocket(BasicNetworkHandler *readhandler[], int max_read_size, int *out_read_size, BasicNetworkHandler *writehandler[], int max_write_size, int *out_write_size);
	
	#ifdef SELECT_NETWORK
	int GetSocketFd(fd_set &fd_read , fd_set &fd_write);
	fd_set	m_fd_read;					//读描述符
	fd_set	m_fd_write;					//写描述符 note：两个描述符与table保持一致，加入时只注册read，且与register_table共用一把锁
	int		m_max_sock;					//表中的sock最大值，linux下用

	fd_set	m_tmp_fd_read;				//在PollSocket中使用的临时读描述符
	fd_set	m_tmp_fd_write;				//在PollSocket中使用的临时写描述符
	#endif

	#ifdef EPOLL_NETWORK
	SOCKET m_epfd;						// epoll fd
	epoll_event m_tmp_event[MAX_HANDEL_SIZE];	// 在PollSocket中使用的临时event数组

	// 记录epoll_ctl总错误次数，以及最后一次操作信息和错误码
	void DealEPollCtlError(int op, SOCKET fd);
	int m_epoll_ctl_count;
	char m_last_epoll_ctl_error[256];
	unsigned long m_last_epoll_ctl_error_time;
	#endif
};

#endif
