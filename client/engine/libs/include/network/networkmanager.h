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
	 ��������acceptʱ��ص��ú���
	 @netid			�����ӵ�netid
	 @ip			Զ�˵�ַ��������
	 @port			Զ�˶˿ں�
	*/
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port)=0;

	/*
	 ���յ�������Ϣʱ�ص��ú���
	 @netid			������Ϣ��Դ��netid
	 @data			��������
	 @length		���ݳ���
	*/
#ifdef USE_PROTOBUF_FOR_NET_MSG
	virtual void OnRecv(NetID netid, NetMsgHead* pMsgHead, const char *data, unsigned int length) = 0;
#else
	virtual void OnRecv(NetID netid, unsigned short cmd, const char *data, unsigned int length) = 0;
#endif

	/*
	 ��������Ͽ���ʱ��ص��ú���
	 @netid			�Ͽ����������ӵ�netid
	*/
	virtual void OnDisconnect(NetID netid)=0;

	/*
	 �������첽ConnectAsyn������غ�ص�
	 @result		���ӽ���Ƿ�ɹ�
	 @handle		ConnectAsynʱ���������handle
	 @netid			���ӳɹ�ʱ��netidʱ�������ӵ������id
	 @ip			����ip
	 @port			����port
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