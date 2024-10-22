#ifndef __NetworkManager_h__
#define __NetworkManager_h__

#include "basicnetwork/def.h"

class Network;
class GsNetCallback;

class IEngineNetCallback
{
public:
	virtual ~IEngineNetCallback() {}

	/*
	 单有连接accept时则回调该函数
	 @netid			该连接的netid
	 @ip			远端地址，主机序
	 @port			远端端口号
	*/
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port)=0;

	/*
	 当收到网络消息时回调该函数
	 @netid			网络消息来源的netid
	 @data			网络数据
	 @length		数据长度
	*/
#ifdef USE_PROTOBUF_FOR_NET_MSG
	virtual void OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length) = 0;
#else
	virtual void OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length) = 0;
#endif

	/*
	 当有网络断开的时候回调该函数
	 @netid			断开的网络连接的netid
	*/
	virtual void OnDisconnect(NetID netid)=0;

	/*
	 当调用异步ConnectAsyn结果返回后回调
	 @result		连接结果是否成功
	 @handle		ConnectAsyn时的输出参数handle
	 @netid			连接成功时，netid时所得连接的网络层id
	 @ip			连接ip
	 @port			连接port
	*/
	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port)=0;
};

class NetworkManager
{
	friend class GsNetCallback;

public:
	~NetworkManager();

	static NetworkManager* GetSingleton();

	void RegisterCallback(IEngineNetCallback * callback);
	void Start();
	void Update();
	void Stop(bool clear_callback = true);
	
	bool Listen(Port port, int backlog, NetID *netid_out=0, const char *ip_bind=0);
	bool Connect(const char *ip, Port port, NetID *netid_out, unsigned long time_out=3000);
	bool ConnectAsyn(const char *ip, Port port, int *handle, unsigned long time_out=3000);

#ifdef USE_PROTOBUF_FOR_NET_MSG
	bool Send(NetID netid, const char* header_data, unsigned short header_data_len, const char *data, unsigned short len);
#else
	bool Send(NetID netid, unsigned short cmd, const char *data, unsigned int len);
#endif

	void Disconnect(NetID id);
	const char *GetPeerName(NetID netid);

protected:
	NetworkManager();

protected:
	NetworkConfig m_network_config;
	Network *m_network;
	GsNetCallback *m_network_callback;

	static const int MAX_CALL_BACK_LIST_SIZE = 32;
	IEngineNetCallback *m_callback_list[MAX_CALL_BACK_LIST_SIZE];
	int m_callback_list_size;
};
#endif