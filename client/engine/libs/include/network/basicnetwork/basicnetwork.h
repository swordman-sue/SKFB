
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
	//protobuf��ϢͷЭ���ļ�����
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

	DirtyQueue m_dirty_queue;			//�ӳ�ɾ����
	Mutex m_dirty_queue_mutex;

	Thread	m_work_thread;				//�����߳�
	bool m_is_exit;						//�߳��˳���־

	JobTempList m_job_to_push;
	JobQueue *m_job_queue;

	void DeleteDirtySocket();
	void PushJobToInvoke();
	static DWORD ThreadFunc(void *param); //�̺߳���
	DWORD WorkFunc();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// ����Ϊƽ̨����ioģ����ش��� ////////////////////////////////////////////
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
	fd_set	m_fd_read;					//��������
	fd_set	m_fd_write;					//д������ note��������������table����һ�£�����ʱֻע��read������register_table����һ����
	int		m_max_sock;					//���е�sock���ֵ��linux����

	fd_set	m_tmp_fd_read;				//��PollSocket��ʹ�õ���ʱ��������
	fd_set	m_tmp_fd_write;				//��PollSocket��ʹ�õ���ʱд������
	#endif

	#ifdef EPOLL_NETWORK
	SOCKET m_epfd;						// epoll fd
	epoll_event m_tmp_event[MAX_HANDEL_SIZE];	// ��PollSocket��ʹ�õ���ʱevent����

	// ��¼epoll_ctl�ܴ���������Լ����һ�β�����Ϣ�ʹ�����
	void DealEPollCtlError(int op, SOCKET fd);
	int m_epoll_ctl_count;
	char m_last_epoll_ctl_error[256];
	unsigned long m_last_epoll_ctl_error_time;
	#endif
};

#endif
