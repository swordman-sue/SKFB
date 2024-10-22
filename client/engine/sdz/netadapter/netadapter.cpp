
#include "netadapter.h"
#include <math.h>
#include "basicnetwork/tcphandler.h"
#include "luamessageadapter.h"
#include "luaengine.h"
#include "cocos2d.h"

USING_NS_CC;

const char *ClientNetworkCallback::ON_NET_CONNECTED = "OnNetConnected";
const char *ClientNetworkCallback::ON_NET_RECV = "OnNetRecv";
const char *ClientNetworkCallback::ON_NET_RECV_ERR = "OnNetRecvErr";
const char *ClientNetworkCallback::ON_NET_DISCONNCT = "OnNetDisconnect";

static NetAdapter* s_instance = nullptr;

NetAdapter::NetAdapter()
{
}

NetAdapter::~NetAdapter()
{
}

NetAdapter* NetAdapter::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new NetAdapter;
	}
	return s_instance;
}

void NetAdapter::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool NetAdapter::Init()
{
	m_network_mgr = NetworkManager::GetSingleton();
	m_callback = new ClientNetworkCallback();

	return true;
}

bool NetAdapter::Start(bool regist_callback)
{
	if (regist_callback)
	{
		m_network_mgr->RegisterCallback(m_callback);
	}
	m_network_mgr->Start();

	return true;
}

bool NetAdapter::Update()
{
	if (m_network_mgr)
	{
		m_network_mgr->Update();
		return true;
	}
	return false;
}

bool NetAdapter::Stop(bool clear_callback)
{
	if (m_network_mgr)
	{
		m_network_mgr->Stop(clear_callback);
		return true;
	}
	return false;
}

bool NetAdapter::Release()
{
	CC_SAFE_DELETE(m_callback);
	CC_SAFE_DELETE(m_network_mgr);

	return true;
}

int NetAdapter::Listen( Port port, int backlog, const char *ip_bind/*=0*/ )
{
	NetID handle = -1;
	if( !m_network_mgr->Listen(port, backlog, &handle, ip_bind))
	{
		handle = -1;
	}
	return handle;
}

int NetAdapter::Connect( const char *ip, Port port, unsigned long time_out/*=3000*/ )
{
	NetID handle = -1;
	if(!m_network_mgr->Connect(ip, port, &handle, time_out))
	{
		handle = -1;
	}
	return handle;
}

int NetAdapter::ConnectAsyn( const char *ip, Port port, unsigned long time_out/*=3000*/ )
{
	int handle = -1;
	if(!m_network_mgr->ConnectAsyn(ip, port, &handle, time_out))
	{
		handle = -1;
	}
	return handle;
}

void NetAdapter::Disconnect( NetID id )
{
	return m_network_mgr->Disconnect(id);
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
bool NetAdapter::Send(NetID netid, const char* header_data, unsigned short header_data_len, const char *data, unsigned short len)
{
	return m_network_mgr->Send(netid, header_data, header_data_len, data, len);
}
#else
bool NetAdapter::Send(NetID netid, unsigned short cmd, const char *data, unsigned int len)
{
	return m_network_mgr->Send(netid, cmd, data, len);
}
#endif

void ClientNetworkCallback::OnConnect(bool result, int handle, NetID netid, IP ip, Port port)
{
	LuaEngine::GetInstance()->CallFunction(ON_NET_CONNECTED, "iiI", (int)result, handle, netid);
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
void ClientNetworkCallback::OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length )
{
	if (length > LuaMessageAdapter::MAX_MSG_LENGTH)
	{
		CCLOG("[ClientNetworkCallback::OnRecv] msg length[%d] is too long", length);
		return ;
	}

	LuaMessageAdapter::SetRecvMsg(pMsgHead, data, length);

	if (0 != LuaEngine::GetInstance()->CallFunction(ON_NET_RECV, "sI", pMsgHead->msgName.c_str(), netid))
	{
		LuaEngine::GetInstance()->CallFunction(ON_NET_RECV_ERR, "sI", pMsgHead->msgName.c_str(), netid);
	}
}
#else
void ClientNetworkCallback::OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length )
{
	if (length > LuaMessageAdapter::MAX_MSG_LENGTH)
	{
		CCLOG("[ClientNetworkCallback::OnRecv] msg length[%d] is too long", length);
		return ;
	}

	LuaMessageAdapter::SetRecvMsg(cmd, data, length);

	if (0 != LuaEngine::GetInstance()->CallFunction(ON_NET_RECV, "iI", cmd, netid))
	{
		LuaEngine::GetInstance()->CallFunction(ON_NET_RECV_ERR, "iI", cmd, netid);
	}
}
#endif

void ClientNetworkCallback::OnDisconnect( NetID netid )
{
	LuaEngine::GetInstance()->CallFunction(ON_NET_DISCONNCT, "I", netid);
}

int NetAdapter::GetEnCodeTime(long int time_stamp, unsigned int skill_id)
{
	int L1 = time_stamp % 1000;
	int L2 = ((time_stamp % 1000000) - L1) / 1000;
	int L3 = (skill_id % 100);
	int L4 = (skill_id - L3) / 100;
	int result = (L2 - L4 + L3)*(L1 - L3 + L4);
	return result;
}

const char * NetAdapter::GetPeerName(NetID netid)
{
	return m_network_mgr->GetPeerName(netid);
}