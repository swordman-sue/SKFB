
#ifndef PISOCKETEX_H
#define PISOCKETEX_H

#include "pisocket.h"

namespace PISocket
{
	extern int nFamily;

	// ��������
	int GetIPList(const char *nodename, const char *servername, sockaddr_in *iplist, int iplist_len);

	bool HostnameToIPStr(const char *nodename, char *ip_str, int max_len);

	// �����ó�ʱ����addr
	// ע�⣺���ص�SOCKETΪ�첽ģʽ, �� FIONBIO ����Ϊ 1
	SOCKET Connect(const struct sockaddr *addr, int svraddr_len, unsigned int timeout);
}

#endif





