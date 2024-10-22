
#include "network.h"
#include <string.h>
#include "job.h"
#include "listenhandler.h"
#include "tcphandler.h"
#include "../../common/platform/socket/pisocketex.h"

#if defined(ANDROID)
#include <android/log.h>
#endif

class DefaultCallback:public INetworkCallback
{
public:
	virtual ~DefaultCallback(){}
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port){}

#ifdef USE_PROTOBUF_FOR_NET_MSG
	virtual void OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length){}
#else
	virtual void OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length){}
#endif

	virtual void OnDisconnect(NetID netid){}
	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port){}
};

Network::Network(NetworkConfig config):m_config(config), m_job_queue(config.job_queue_length), m_basicnetwork(&m_job_queue), 
m_cur_handle_id(0), m_connect_req_queue(256), m_connect_result_queue(256), m_connect_thread_run(false)
{
	PISocket::Startup();
	m_default_callback = new DefaultCallback();
	m_callback = m_default_callback;
}

Network::~Network()
{
	delete m_default_callback;
	PISocket::Cleanup();
}

void Network::RegisterCallback(INetworkCallback * callback)
{
	if (callback == 0)
	{
		m_callback = m_default_callback;
	}
	else
	{
		m_callback = callback;
	}
}

void Network::Start()
{
	m_basicnetwork.Start();
	m_connect_thread_run = true;
	for (int i = 0; i < CONNECT_THREAD_NUM; ++i)
	{
		m_connect_thread[i].Run(ConnectThread, this);
	}
}

void Network::Stop()
{
	m_connect_thread_run = false;
	for (int i = 0; i < CONNECT_THREAD_NUM; ++i)
	{
		m_connect_event.Signal();
	}
	for (int i = 0; i < CONNECT_THREAD_NUM; ++i)
	{
		m_connect_thread[i].Join();
	}
	m_basicnetwork.Stop();
	
	Job *job;
	while (m_job_queue.TryPop(&job, 0))
	{
		delete job;
	}
}

void Network::Update()
{
	Job *job;
	while (m_job_queue.TryPop(&job, 0))
	{
		job->Invoke(m_callback);
		delete job;
	}

	ConnectResult cr;
	while (m_connect_result_queue.TryPop(&cr))
	{
		m_callback->OnConnect(cr.result, cr.handle, cr.netid, cr.ip, cr.port);
	}
}

bool Network::Listen(Port port, int backlog, NetID *netid_out, const char *ip_bind)
{
	ListenHandler *listenhandler = new ListenHandler(m_config.max_package_size);
	SOCKET sock = listenhandler->Listen(port, backlog, ip_bind);
	if (sock == SOCKET_ERROR)
	{
		return false;
	}

	NetID netid = m_basicnetwork.Add(listenhandler);

	if (netid_out != 0)
	{
		*netid_out = netid;
	}
	
	return true;
}

#if defined(__unix) || defined(__APPLE__) || defined(__ANDROID__)
bool Network::Connect(const char *domain, Port port, NetID *netid_out, unsigned long time_out)
{
	//连接ip  
	char ip[128];
	memset(ip, 0, sizeof(ip));
	strcpy(ip, domain);

	void* svraddr = nullptr;
	int error = -1, svraddr_len;
	bool ret = true;
	struct sockaddr_in svraddr_4;
	struct sockaddr_in6 svraddr_6;

	//获取网络协议  
	struct addrinfo *result;
	error = getaddrinfo(ip, NULL, NULL, &result);
    if (NULL == result)
    {
        return false;
    }
	const struct sockaddr *sa = result->ai_addr;
	socklen_t maxlen = 128;
	switch (sa->sa_family)
	{
	case AF_INET://ipv4  
		PISocket::nFamily = AF_INET;

		printf("socket created ipv4/n");

		if (inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), ip, maxlen) == 0)
		{
			perror(ip);
			ret = false;
			break;
		}
        memset(&svraddr_4, 0, sizeof(sockaddr_in));
		svraddr_4.sin_family = AF_INET;
		svraddr_4.sin_addr.s_addr = inet_addr(ip);
		svraddr_4.sin_port = htons(port);
		svraddr_len = sizeof(svraddr_4);
		svraddr = &svraddr_4;
		break;

	case AF_INET6://ipv6  
		PISocket::nFamily = AF_INET6;

		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), ip, maxlen);

		printf("socket created ipv6/n");

		bzero(&svraddr_6, sizeof(svraddr_6));
		svraddr_6.sin6_family = AF_INET6;
		svraddr_6.sin6_port = htons(port);
		if (inet_pton(AF_INET6, ip, &svraddr_6.sin6_addr) == 0)
        {
			perror(ip);
			ret = false;
			break;
		}
		svraddr_len = sizeof(svraddr_6);
		svraddr = &svraddr_6;
		break;

	default:
		printf("Unknown AF\ns");
		ret = false;
	}
	freeaddrinfo(result);

	if (!ret)
	{
		fprintf(stderr, "Cannot Connect the server!n");
		return false;
	}

	SOCKET connect_sock = PISocket::Connect((const sockaddr*)svraddr, svraddr_len, time_out);
	if (connect_sock != SOCKET_ERROR)
	{
		TcpHandler *tcphandler = new TcpHandler(connect_sock, m_config.max_package_size);
		NetID netid = m_basicnetwork.Add(tcphandler);
		//__android_log_print(ANDROID_LOG_DEBUG, "Network::Connect debug info", "xxxxxxxxxxxxxxxxx %d", (unsigned int)netid);

		if (netid_out != 0)
		{
			*netid_out = netid;
		}
		return true;
	}
	
	return false;
}

#elif defined(_WIN32)
static bool GetConnectIP(const char *ip_name, IP &ip)
{
	unsigned long ip_n = inet_addr(ip_name);

	if (ip_n != INADDR_NONE)
	{
		ip = ntohl(ip_n);
		return true;
	}

	const char *ip_name_real = NULL;
	hostent *pHostEnt = gethostbyname(ip_name);

	if (pHostEnt && pHostEnt->h_addrtype == AF_INET)
	{
		for (int i = 0; i < pHostEnt->h_length; ++i)
		{
			in_addr *pAddr = (in_addr*)pHostEnt->h_addr_list[i];
			if (pAddr && NULL != (ip_name_real = inet_ntoa(*pAddr)))
			{
				break;
			}
		}
	}

	if (NULL == ip_name_real)
	{
		return false;
	}

	ip_n = inet_addr(ip_name_real);

	if (ip_n == INADDR_NONE)
		return false;

	ip = ntohl(ip_n);

	return true;
}

bool Network::Connect(const char *domain, Port port, NetID *netid_out, unsigned long time_out)
{
	PISocket::nFamily = AF_INET;

	IP ip;
	if (!GetConnectIP(domain, ip))
	{
		return false;
	}

	sockaddr_in server_adr;
	memset(&server_adr, 0, sizeof(sockaddr_in));
	server_adr.sin_family = AF_INET;
	server_adr.sin_addr.s_addr = htonl(ip);
	server_adr.sin_port = htons(port);

	SOCKET connect_sock = PISocket::Connect((const sockaddr*)&server_adr, sizeof(server_adr), time_out);
	if (connect_sock != SOCKET_ERROR)
	{
		TcpHandler *tcphandler = new TcpHandler(connect_sock, m_config.max_package_size);
		NetID netid = m_basicnetwork.Add(tcphandler);
		if (netid_out != 0)
		{
			*netid_out = netid;
		}
		return true;
	}

	return false;
}
#endif

bool Network::ConnectAsyn(const char *ip, Port port, int *handle, unsigned long time_out)
{
	int cur_handle = m_cur_handle_id++;
	if (handle != 0)
	{
		*handle = cur_handle;

	}
	ConnectStruct cs;
	cs.handle = cur_handle;
	cs.ipname = ip;
	cs.port = port;
	cs.timeout = time_out;

	if (m_connect_req_queue.TryPush(cs))
	{
		m_connect_event.Signal();
		return true;
	}

	return false;
}

void Network::Disconnect(NetID id)
{
	m_basicnetwork.Remove(id);
}

const char *Network::GetPeerName(NetID netid)
{
	return m_basicnetwork.GetPeerName(netid);
}

DWORD Network::ConnectThread(void * param)
{
	Network *p_this = (Network*)param;
	p_this->ConnectThreadHelper();
	return 0;
}

void Network::ConnectThreadHelper()
{
	while (m_connect_thread_run)
	{
		ConnectStruct cs;
		if (!m_connect_req_queue.TryPop(&cs))
		{
			m_connect_event.Wait(100);
			continue;
		}

		NetID netid;
		bool ret = Connect(cs.ipname, cs.port, &netid, cs.timeout);

		ConnectResult cr;
		cr.handle = cs.handle;
		cr.netid = netid;
		cr.port = cs.port;
		cr.result = ret;
		m_connect_result_queue.Push(cr);
	}
}
