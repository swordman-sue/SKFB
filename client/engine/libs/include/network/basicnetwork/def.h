
#ifndef DEF_H
#define DEF_H

typedef	unsigned int		NetID;
typedef unsigned int		IP;
typedef unsigned short		Port;
typedef unsigned int		MsgLen;

struct NetMsgHead;

class INetworkCallback
{
public:
	virtual ~INetworkCallback(){}
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port)=0;

#ifdef USE_PROTOBUF_FOR_NET_MSG
	virtual void OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length) = 0;
#else
	virtual void OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length)=0;
#endif

	virtual void OnDisconnect(NetID netid)=0;
	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port) = 0;
};

struct NetworkConfig
{
	NetworkConfig():job_queue_length(1024 * 16),			// 64K
		max_package_size(1024 * 1024)
	{

	}
	int job_queue_length;
	int max_package_size;
};

#endif

