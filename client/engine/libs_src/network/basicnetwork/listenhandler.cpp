
#include "listenhandler.h"
#include <memory.h>
#include "jobaccept.h"
#include "basicnetwork.h"
#include "jobdisconnect.h"
#include "tcphandler.h"

ListenHandler::ListenHandler(int accept_tcp_max_pack_szie)
:BasicNetworkHandler(SOCKET_ERROR, HT_LISTEN), m_listen_port(0), m_accept_tcp_max_package_size(accept_tcp_max_pack_szie)
{
}

ListenHandler::~ListenHandler()
{
	Close();
}

SOCKET ListenHandler::Listen(Port port, int backlog, const char *ip_bind)
{
	if ( m_socket != SOCKET_ERROR )
	{
		return m_socket;
	}

	SOCKET sock = PISocket::Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( sock == INVALID_SOCKET )
	{
		return SOCKET_ERROR;
	}

	unsigned long enable = 1;
	if ( SOCKET_ERROR == PISocket::SetSockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(unsigned long)) )
	{
		PISocket::Close(sock);
		return SOCKET_ERROR;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (ip_bind == 0)
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		unsigned long ip_n = inet_addr(ip_bind);
		if (ip_n == INADDR_NONE)
		{
			ip_n = htonl(INADDR_ANY);
		}
		addr.sin_addr.s_addr = ip_n;
	}
	
	if ( SOCKET_ERROR == PISocket::Bind(sock, (sockaddr*)&addr) )
	{
		PISocket::Close(sock);
		return SOCKET_ERROR;
	}


	if ( SOCKET_ERROR == PISocket::Listen(sock, backlog) )
	{
		PISocket::Close(sock);
		return SOCKET_ERROR;
	}

	// 设为非阻塞
	unsigned long b = 1;
	if ( SOCKET_ERROR == PISocket::Ioctl(sock, FIONBIO, &b) )
	{
		PISocket::Close(sock);
		return SOCKET_ERROR;
	}

	m_socket = sock;
	m_listen_port = port;

	
	return sock;
}

void ListenHandler::OnCanRead()
{
	for (;;)
	{
		sockaddr_in addr;
		SOCKET sock = PISocket::Accept(m_socket, (sockaddr*)&addr);
		if (sock == INVALID_SOCKET)
		{
			return;
		}

		// 所有类型的Job考虑用池
		if (m_basic_network != 0)
		{
			TcpHandler *tcphandler = new TcpHandler(sock, m_accept_tcp_max_package_size);
			NetID netid = m_basic_network->Add(tcphandler);

			JobAccept *jobaccept = new JobAccept(netid, sock, m_listen_port, m_basic_network);
			m_basic_network->PushJob(jobaccept);
		}
	}
}

void ListenHandler::OnClose()
{
	Close();
}

void ListenHandler::Close()
{
	if ( m_socket != SOCKET_ERROR )
	{
		PISocket::Shutdown(m_socket, PI_SD_BOTH);
		PISocket::Close(m_socket);
		m_socket = SOCKET_ERROR;

		if (m_basic_network != 0)
		{
			JobDisconect *jobdisconnect = new JobDisconect(m_netid);
			m_basic_network->PushJob(jobdisconnect);
		}

		m_listen_port = 0;
	}
}

