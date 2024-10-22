
#include "platform/socket/pisocketex.h"

#if defined(__unix) || defined(__APPLE__) || defined(__ANDROID__)
#include <netdb.h>
#include <memory.h>
#include <sys/time.h>
#include <string.h>
#include <sys/select.h>
#else
#include <Ws2tcpip.h>
#endif

#include "platform/system.h"

namespace PISocket
{
	int nFamily = AF_INET;

	//暂不支持ipv6
	int GetIPList(const char *nodename, const char *servername, sockaddr_in *iplist, int iplist_len)
	{
		addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		hint.ai_family = nFamily;
		hint.ai_protocol = IPPROTO_TCP;
		hint.ai_socktype = SOCK_STREAM;

		addrinfo* haddrinfo = 0;
		int ret = getaddrinfo(nodename, servername, &hint, &haddrinfo);
		if (ret != 0) return 0;

		addrinfo* paddrinfo = haddrinfo;
		int num = 0;
		while(paddrinfo != 0 && num < iplist_len)
		{
			if ( paddrinfo->ai_addrlen == sizeof(sockaddr_in) )
			{
				iplist[num++] = *((sockaddr_in*)paddrinfo->ai_addr);
			}
			paddrinfo = paddrinfo->ai_next;
		}

		freeaddrinfo(haddrinfo);

		return num;
	}

	//暂不支持ipv6
	bool HostnameToIPStr(const char *nodename, char *ip_str, int max_len)
	{
		sockaddr_in sock_addr;
		if (GetIPList(nodename, "", &sock_addr, 1) == 1)
		{
			const char *str = inet_ntoa(sock_addr.sin_addr);
			strncpy(ip_str, str, max_len);
			ip_str[max_len - 1] = 0;
			return true;
		}
		return false;
	}

	SOCKET Connect(const struct sockaddr *addr, int svraddr_len, unsigned int timeout)
	{
		SOCKET connect_sock = PISocket::Socket(nFamily, SOCK_STREAM, IPPROTO_TCP);
		if (connect_sock == SOCKET_ERROR)
		{
			return INVALID_SOCKET;
		}

		//设置为非阻塞
		unsigned long ul = 1;
		if (SOCKET_ERROR == PISocket::Ioctl(connect_sock, FIONBIO, &ul))
		{
			PISocket::Close(connect_sock);
			return INVALID_SOCKET;
		}

		PISocket::Connect(connect_sock, addr, svraddr_len);

		//检测可写，超时则返回
		struct timeval tv ;   
		tv.tv_sec = timeout / 1000;   
		tv.tv_usec = (timeout % 1000) * 1000;

		fd_set fdwrite;
		FD_ZERO(&fdwrite);
		FD_SET(connect_sock, &fdwrite);

		int ret = select((int)connect_sock+1, 0, &fdwrite, 0, &tv);

		if (ret > 0 && FD_ISSET(connect_sock, &fdwrite))
		{
			int error = 0;
			int size = sizeof(int);
			if (0 == PISocket::GetSockopt(connect_sock, SOL_SOCKET, SO_ERROR, (char *)&error, &size) && error == 0)
			{
				return connect_sock;
			}
		}

		PISocket::Close(connect_sock);
		return INVALID_SOCKET;
	}
}

