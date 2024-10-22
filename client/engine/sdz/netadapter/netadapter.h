
#ifndef NetManager_H__
#define NetManager_H__

#include "networkmanager.h"

class NetAdapter;

//由这个类来操作ProtocalAnalyzer
class ClientNetworkCallback : public IEngineNetCallback
{
	static const char *ON_NET_RECV;
	static const char *ON_NET_RECV_ERR;
	static const char *ON_NET_DISCONNCT;
	static const char *ON_NET_CONNECTED;

public:
	ClientNetworkCallback() 
	{
	}

	virtual ~ClientNetworkCallback()
	{}

	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port){}

#ifdef USE_PROTOBUF_FOR_NET_MSG
	virtual void OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length);
#else
	virtual void OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length);
#endif

	virtual void OnDisconnect(NetID netid);
	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port);

private:
	int m_scene_handle_type;
};

// 对底层NetworkManager的封装
class NetAdapter
{
public:
	static NetAdapter* GetInstance();
	static void DelInstance();
	~NetAdapter();

	bool Init();
	bool Start(bool regist_callback = true);
	bool Update();
	bool Pause() { return true; }
	bool Resume() { return true; }
	bool Stop(bool clear_callback = true);
	bool Release();

	int Listen(Port port, int backlog, const char *ip_bind=0);
	int Connect(IP ip, Port port, unsigned long time_out=3000);
	int Connect(const char *ip, Port port, unsigned long time_out=3000);
	int ConnectAsyn(const char *ip, Port port, unsigned long time_out=3000);

#ifdef USE_PROTOBUF_FOR_NET_MSG
	bool Send(NetID netid, const char* header_data, unsigned short header_data_len, const char *data, unsigned short len);
#else
	bool Send(NetID netid, unsigned short cmd, const char *data, unsigned int len);
#endif

	void Disconnect(NetID id);
	const char *GetPeerName(NetID netid);
	int GetEnCodeTime(long int time_stamp,unsigned int skill_id);

protected:
	NetAdapter();

protected:
	// 底层NetworkManager实例
	NetworkManager*			m_network_mgr;	
	ClientNetworkCallback*	m_callback;
};

#endif