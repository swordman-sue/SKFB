
#ifndef PISOCKETEX_H
#define PISOCKETEX_H

#include "pisocket.h"

namespace PISocket
{
	extern int nFamily;

	// 解析域名
	int GetIPList(const char *nodename, const char *servername, sockaddr_in *iplist, int iplist_len);

	bool HostnameToIPStr(const char *nodename, char *ip_str, int max_len);

	// 可设置超时连接addr
	// 注意：返回的SOCKET为异步模式, 即 FIONBIO 设置为 1
	SOCKET Connect(const struct sockaddr *addr, int svraddr_len, unsigned int timeout);
}

#endif





