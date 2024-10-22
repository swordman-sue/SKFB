
#ifndef JOBRECV_H
#define JOBRECV_H

#include <string>
#include "def.h"
#include "job.h"
#include "../../common/platform/socket/pisocket.h"
#include "tcphandler.h"

class MsgMem;

class JobRecv :public Job
{
public:
#ifdef USE_PROTOBUF_FOR_NET_MSG
	JobRecv(NetID netid, NetMsgHead* stMsgHead, MsgMem *data, MsgLen len);
#else
	JobRecv(NetID netid, unsigned short cmd, MsgMem *data, MsgLen len);
#endif

	virtual ~JobRecv();
	virtual void Invoke(INetworkCallback *callback);

	void *operator new(size_t c);
	void operator delete(void *m);

protected:
	MsgMem *m_data;
	MsgLen m_length;
	NetID m_netid;

#ifdef USE_PROTOBUF_FOR_NET_MSG
	NetMsgHead m_stMsgHead;
#else
	unsigned short m_cmd;
#endif
};


#endif


