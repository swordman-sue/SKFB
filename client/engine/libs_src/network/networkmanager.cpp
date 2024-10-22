
#include "networkmanager.h"
#include "basicnetwork/network.h"
#include "basicnetwork/tcphandler.h"

static NetworkManager* s_network_mgr = 0;

class GsNetCallback: public INetworkCallback
{
public:
	GsNetCallback(NetworkManager *network_module):m_network_module(network_module)
	{
	}

	virtual ~GsNetCallback(){}
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port)
	{
		int s = m_network_module->m_callback_list_size;
		IEngineNetCallback **cl = m_network_module->m_callback_list;
		for (int i = 0; i < s; ++i)
		{
			cl[i]->OnAccept(listen_port, netid, ip, port);
		}
	}

#ifdef USE_PROTOBUF_FOR_NET_MSG
	virtual void OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length)
	{
		int s = m_network_module->m_callback_list_size;
		IEngineNetCallback **cl = m_network_module->m_callback_list;
		for (int i = 0; i < s; ++i)
		{
			cl[i]->OnRecv(netid, pMsgHead, data, length);
		}
	}
#else
	virtual void OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length)
	{
		int s = m_network_module->m_callback_list_size;
		IEngineNetCallback **cl = m_network_module->m_callback_list;
		for (int i = 0; i < s; ++i)
		{
			cl[i]->OnRecv(netid, cmd, data, length);
		}
	}
#endif

	virtual void OnDisconnect(NetID netid)
	{
		int s = m_network_module->m_callback_list_size;
		IEngineNetCallback **cl = m_network_module->m_callback_list;
		for (int i = 0; i < s; ++i)
		{
			cl[i]->OnDisconnect(netid);
		}
	}

	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port)
	{
		int s = m_network_module->m_callback_list_size;
		IEngineNetCallback **cl = m_network_module->m_callback_list;
		for (int i = 0; i < s; ++i)
		{
			cl[i]->OnConnect(result, handle, netid, ip, port);
		}
	}

private:
	NetworkManager *m_network_module;
};

NetworkManager::NetworkManager()
	: m_callback_list_size(0)
{
	memset(m_callback_list, 0, sizeof(m_callback_list));

	m_network_callback = new GsNetCallback(this);

	m_network_config.job_queue_length = 1024 * 10;
	m_network_config.max_package_size = 1024 * 1024;

	m_network = new Network(m_network_config);
	m_network->RegisterCallback(m_network_callback);
}

NetworkManager::~NetworkManager()
{
	if(m_network_callback != 0)
	{
		delete m_network_callback;
		m_network_callback = 0;
	}
	if(m_network != 0)
	{
		delete m_network;
		m_network = 0;
	}
	s_network_mgr = 0;
}

NetworkManager* NetworkManager::GetSingleton()
{
	if (!s_network_mgr)
	{
		s_network_mgr = new NetworkManager();
	}
	return s_network_mgr;
}

void NetworkManager::RegisterCallback(IEngineNetCallback * callback)
{
	if (m_callback_list_size < MAX_CALL_BACK_LIST_SIZE)
	{
		m_callback_list[m_callback_list_size++] = callback;
	}
}

void NetworkManager::Start()
{
	m_network->Start();
}

void NetworkManager::Update()
{
	m_network->Update();
}

void NetworkManager::Stop(bool clear_callback)
{
	m_network->Stop();
	if(clear_callback)
	{
		m_network->RegisterCallback(0);
	}
}

bool NetworkManager::Listen( Port port, int backlog, NetID *netid_out/*=0*/, const char *ip_bind/*=0*/ )
{
	return m_network->Listen(port, backlog, netid_out, ip_bind);
}

bool NetworkManager::Connect( const char *ip, Port port, NetID *netid_out, unsigned long time_out/*=3000*/ )
{
	return m_network->Connect(ip, port, netid_out, time_out);
}

bool NetworkManager::ConnectAsyn( const char *ip, Port port, int *handle, unsigned long time_out/*=3000*/ )
{
	return m_network->ConnectAsyn(ip, port, handle, time_out);
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
bool NetworkManager::Send(NetID netid, const char* header_data, unsigned short header_data_len, const char *data, unsigned short len)
{
	return m_network->Send(netid, header_data, header_data_len, data, len);
}
#else
bool NetworkManager::Send(NetID netid, unsigned short cmd, const char *data, unsigned int len)
{
	return m_network->Send(netid, cmd, data, len);
}
#endif

void NetworkManager::Disconnect( NetID id )
{
	m_network->Disconnect(id);
}

const char * NetworkManager::GetPeerName( NetID netid )
{
	return m_network->GetPeerName(netid);
}
