
#include "tcphandler.h"
#include <vector>
#include "basicnetwork.h"
#include "jobrecv.h"
#include "jobdisconnect.h"
#include "../../common/memory/msgmem.h"

#define MIN_MSG_TO_WRITE_BUFF 2048
#define MIN_CHECK_REDUCE_WRITE_BUFF_SIZE	(2 * 1024 * 1024)
#define WRITE_BUFF_REDUCE_RATE				(0.3)
#define WRITE_BUFF_REDUCE_TO_RATE			(0.6)
#define WRITE_BUFF_INCREASE_RATE			(1.5)

#define FLASH_POLICY_STR "<policy-file-request/>"
const int FLASH_POLICY_STR_LEN = sizeof(FLASH_POLICY_STR);

#define FLASH_POLICY_XML_RET "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>"
const int FLASH_POLICY_XML_RET_LEN = sizeof(FLASH_POLICY_XML_RET);

TcpHandler::MsgBuffItem::MsgBuffItem()
: buff_data(0), buff_capacity(0), buff_length(0), buff_offset(0)
{

}

void TcpHandler::MsgBuffItem::CheckSize(int len)
{
	if (len <= buff_capacity) return;

	// 此处不能用realloc，因为MsgMem重载了new[]和delete[]
	MsgMem *new_buff = new MsgMem[len];
	if (buff_data != 0)
	{
		if (buff_length != 0)
		{
			memcpy(new_buff, buff_data, buff_length);
		}
		delete []buff_data;
	}
	buff_data = new_buff;
	buff_capacity = len;
}

void TcpHandler::MsgBuffItem::ReduceSize(int len)
{
	// 此处不能用realloc，因为MsgMem重载了new[]和delete[]
	MsgMem *new_buff = new MsgMem[len];
	if (buff_data != 0)
	{
		// 此处不理会是否有内容
		delete []buff_data;
	}
	buff_data = new_buff;
	buff_capacity = len;
}

void TcpHandler::MsgBuffItem::Clear()
{
	if (buff_data != 0)
	{
		delete []buff_data;
		buff_data = 0;
		buff_capacity = 0;
		buff_length = 0;
	}
}

void TcpHandler::MsgBuffItem::Append( const char* data, int data_len )
{
	if(data_len > 0)
	{
		CheckSize(data_len + buff_length);
		memcpy(buff_data + buff_length, data, data_len);
		buff_length += data_len;
	}
}

void TcpHandler::MsgBuffItem::ResetOffset()
{
	if(buff_offset > 0)
	{
		if(buff_length > buff_offset)
		{
			buff_length -= buff_offset;
			memcpy(buff_data, (char*)buff_data + buff_offset, buff_length);
			buff_offset = 0;
		}
		else
		{
			buff_offset = 0;
			buff_length = 0;
		}
	}
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
bool TcpHandler::MsgBuffItem::WriteClientMsg(const char* header_data, unsigned short header_data_len, const char* data, unsigned short data_len)
{
	NetMsgBasic msg_basic;
	msg_basic.len = 2 + header_data_len + 2 + data_len + 1;
	msg_basic.headLen = header_data_len;

	AppendItem(htons(msg_basic.len));
	AppendItem(htons(msg_basic.headLen));
	Append(header_data, header_data_len);
	AppendItem(htons(data_len));
	Append(data, data_len);
	Append("t", 1);

	return true;
}

void TcpHandler::MsgBuffItem::ReadServerMsgBasic(NetMsgBasic* server_head)
{
	assert((unsigned int)sizeof(NetMsgBasic) + buff_offset <= buff_length);
	server_head->len = ntohs(ReadItem((unsigned short)0));
	server_head->headLen = ntohs(ReadItem((unsigned short)0));
}

bool TcpHandler::MsgBuffItem::ReadServerMsgHead(unsigned int header_data_len, NetMsgHead* net_msg_head)
{
	//注册protobuf消息头协议文件数据
	if (nullptr == BasicNetwork::pbc_env_for_net_msg_head)
	{
		if (nullptr == BasicNetwork::pb_data_for_net_msg_head || 0 >= BasicNetwork::pb_data_len_for_net_msg_head)
		{
			printf("[TcpHandler::MsgBuffItem::ReadServerMsgBasic] PB_DATA_FOR_NET_MSG_HEAD is invalid");
			return false;
		}

		pbc_slice slice;
		slice.buffer = BasicNetwork::pb_data_for_net_msg_head;
		slice.len = BasicNetwork::pb_data_len_for_net_msg_head;

		BasicNetwork::pbc_env_for_net_msg_head = pbc_new();
		int ret = pbc_register(BasicNetwork::pbc_env_for_net_msg_head, &slice);
		if (ret)
		{
			printf("[TcpHandler::MsgBuffItem::ReadServerMsgBasic] pbc_register faild %s", pbc_error(BasicNetwork::pbc_env_for_net_msg_head));
			return false;
		}

		delete[] BasicNetwork::pb_data_for_net_msg_head;
		BasicNetwork::pb_data_for_net_msg_head = nullptr;
	}

	//读取protobuf消息头数据
	char* header_buffer_data = GetOffsetPtr();
	buff_offset += header_data_len;

	pbc_slice header_slice;
	header_slice.buffer = header_buffer_data;
	header_slice.len = header_data_len;

	pbc_rmessage* rmsg = pbc_rmessage_new(BasicNetwork::pbc_env_for_net_msg_head, "PbHead.MsgHead", &header_slice);
	if (NULL == rmsg)
	{
		printf("[TcpHandler::MsgBuffItem::ReadServerMsgBasic] pbc_rmessage_new faild %s", pbc_error(BasicNetwork::pbc_env_for_net_msg_head));
		return false;
	}
	net_msg_head->msgType = pbc_rmessage_integer(rmsg, "msgtype", 0, NULL);
	net_msg_head->msgName = pbc_rmessage_string(rmsg, "msgname", 0, NULL);
	net_msg_head->msgRet = pbc_rmessage_integer(rmsg, "msgret", 0, NULL);
	pbc_rmessage_delete(rmsg);

	return true;
}

#else
void TcpHandler::MsgBuffItem::WriteClientMsg(int order_id, short cmd, const char* data, unsigned int data_len)
{
	NetMsgBasic msg_basic;
	msg_basic.len = sizeof(msg_basic.order_id) + sizeof(msg_basic.cmd) + data_len;
	msg_basic.order_id = order_id;
	msg_basic.cmd = cmd;

	AppendItem(msg_basic.len);
	AppendItem(msg_basic.order_id);
	AppendItem(msg_basic.cmd);
	Append(data, data_len);
}

void TcpHandler::MsgBuffItem::ReadServerMsgBasic(NetMsgBasic* server_head)
{
	assert((unsigned int)sizeof(NetMsgBasic)+buff_offset <= buff_length);
	server_head->len = ReadItem((unsigned int)0);
	server_head->order_id = ReadItem((unsigned int)0);
	server_head->cmd = ReadItem((unsigned short)0);
}
#endif

TcpHandler::TcpHandler(SOCKET socket, int max_package_size):
BasicNetworkHandler(socket, HT_TCP),
m_cur_write_length(0),
m_max_package_size(max_package_size),
m_cur_read_state(RMS_READ_MSG_BASIC),
m_cur_msg_content_len(0),
m_order_id(0)
{
	if (m_socket != SOCKET_ERROR)
	{
		// 设为非阻塞
		unsigned long ul = 1;
		PISocket::Ioctl(m_socket, FIONBIO, (unsigned long*)&ul);
	}

	m_buff_to_write.CheckSize(MIN_MSG_TO_WRITE_BUFF);
	m_buff.CheckSize(MIN_MSG_TO_WRITE_BUFF);
}


TcpHandler::~TcpHandler()
{
	OnClose();
}

#define MSG_READ_BUFF_LEN	2048

void TcpHandler::OnCanRead()
{
	if (m_socket == SOCKET_ERROR)
	{
		return;
	}

	char buff[MSG_READ_BUFF_LEN];

	while(1)
	{
		int ret = PISocket::Recv(m_socket, buff, MSG_READ_BUFF_LEN, 0);
		if (ret <= 0)
		{
			if (ret == SOCKET_ERROR && PISocket::Errno() == EWOULDBLOCK)
			{
				// 读缓冲区空，等待下次可读时间
				break;
			}
			else	
			{
				// 出错
				printf("[TcpHandler::OnCanRead] Socket read error. error_code:%d error_msg:%s\n", PISocket::Errno(), PISocket::GetErrStr(PISocket::Errno(), 0, 0));
				if (m_basic_network != 0)
				{
					m_basic_network->Remove(m_netid);
				}
				return;	//直接退出处理流程
			}
		}
		else
		{
			m_cur_read_buffer.Append(buff, ret);
		}
	}

	HandleOnCanRead();
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
void TcpHandler::HandleOnCanRead()
{
	bool need_contiue = false;
	do
	{
		need_contiue = false;

		//读取消息基础信息
		if (m_cur_read_state == RMS_READ_MSG_BASIC)
		{
			if (m_cur_read_buffer.GetRealLength() >= sizeof(NetMsgBasic))
			{
				m_cur_read_buffer.ReadServerMsgBasic(&m_cur_msg_basic);
				m_cur_read_state = RMS_READ_MSG_HEAD;
				need_contiue = (m_cur_read_buffer.GetRealLength() >= m_cur_msg_basic.headLen);
			}
		}

		//读取protobuf消息头
		else if (m_cur_read_state == RMS_READ_MSG_HEAD)
		{
			if (m_cur_read_buffer.GetRealLength() >= m_cur_msg_basic.headLen)
			{
				if (!m_cur_read_buffer.ReadServerMsgHead(m_cur_msg_basic.headLen, &m_cur_net_msg_head))
				{
					printf("[TcpHandler::OnCanRead] ReadServerMsgHead failed");

					//消息头读取失败，断开连接
					if (m_basic_network != 0)
					{
						m_basic_network->Remove(m_netid);
					}
					return;
				}

				//通讯协议出错码，非0表示出错，出错情况下消息体为空
				if (2 == m_cur_net_msg_head.msgType && 0 != m_cur_net_msg_head.msgRet)
				{
					JobRecv *jobrecv = new JobRecv(m_netid, &m_cur_net_msg_head, nullptr, 0);
					m_basic_network->PushJob(jobrecv);

					m_cur_msg_content_len = 0;
					m_cur_read_state = RMS_READ_MSG_BASIC;
					m_cur_read_buffer.AddOffset(1);
					need_contiue = (m_cur_read_buffer.GetRealLength() >= sizeof(NetMsgBasic));
				}
				else
				{
					m_cur_msg_content_len = ntohs(m_cur_read_buffer.ReadItem((unsigned short)0));
					m_cur_read_state = RMS_READ_CONTENT;
					need_contiue = (m_cur_read_buffer.GetRealLength() >= m_cur_msg_content_len + 1) || m_cur_msg_content_len == 0;
				}
			}
		}

		//读取消息内容
		else if (m_cur_read_state == RMS_READ_CONTENT)
		{
			if (m_cur_msg_content_len == 0)
			{
				//消息体为空
				JobRecv *jobrecv = new JobRecv(m_netid, &m_cur_net_msg_head, nullptr, m_cur_msg_content_len);
				m_basic_network->PushJob(jobrecv);

				m_cur_read_state = RMS_READ_MSG_BASIC;
				need_contiue = (m_cur_read_buffer.GetRealLength() >= sizeof(NetMsgBasic));
			}
			else if (m_cur_read_buffer.GetRealLength() >= m_cur_msg_content_len + 1)
			{
				//消息体不为空
				MsgMem* mem = new MsgMem[m_cur_msg_content_len];
				memcpy(mem, m_cur_read_buffer.GetOffsetPtr(), m_cur_msg_content_len);
				m_cur_read_buffer.AddOffset(m_cur_msg_content_len);
				m_cur_read_buffer.AddOffset(1);

				JobRecv *jobrecv = new JobRecv(m_netid, &m_cur_net_msg_head, mem, m_cur_msg_content_len);
				m_basic_network->PushJob(jobrecv);

				m_cur_read_state = RMS_READ_MSG_BASIC;
				need_contiue = (m_cur_read_buffer.GetRealLength() >= sizeof(NetMsgBasic));
			}
		}
	} while (need_contiue);

	//重置offset
	m_cur_read_buffer.ResetOffset();
}

#else
void TcpHandler::HandleOnCanRead()
{
	bool need_contiue = false;
	do
	{
		need_contiue = false;

		//读取消息基础信息
		if (m_cur_read_state == RMS_READ_MSG_BASIC)
		{
			if (m_cur_read_buffer.GetRealLength() >= sizeof(NetMsgBasic))
			{
				m_cur_read_buffer.ReadServerMsgBasic(&m_cur_msg_basic);

				//检测消息内容长度
				m_cur_msg_content_len = m_cur_msg_basic.GetMsgContentLen();
				if (m_cur_msg_content_len < 0)
				{
					printf("[TcpHandler::OnCanRead] m_cur_msg_content_len invalid [cmd:%d]", m_cur_msg_basic.cmd);

					//消息内容长度无效，断开连接
					if (m_basic_network != 0)
					{
						m_basic_network->Remove(m_netid);
					}
					return;
				}

				m_cur_read_state = RMS_READ_CONTENT;
				need_contiue = (m_cur_read_buffer.GetRealLength() >= m_cur_msg_content_len) || m_cur_msg_content_len == 0;
			}
		}

		//读取消息内容
		else if (m_cur_read_state == RMS_READ_CONTENT)
		{
			if (m_cur_msg_content_len == 0)
			{
				//消息体为空
				JobRecv *jobrecv = new JobRecv(m_netid, m_cur_msg_basic.cmd, 0, m_cur_msg_content_len);
				m_basic_network->PushJob(jobrecv);

				m_cur_read_state = RMS_READ_MSG_BASIC;
				need_contiue = (m_cur_read_buffer.GetRealLength() > 0);
			}
			else if (m_cur_read_buffer.GetRealLength() >= m_cur_msg_content_len)
			{
				//消息体不为空
				MsgMem* mem = new MsgMem[m_cur_msg_content_len];
				memcpy(mem, m_cur_read_buffer.GetOffsetPtr(), m_cur_msg_content_len);
				m_cur_read_buffer.AddOffset(m_cur_msg_content_len);

				JobRecv *jobrecv = new JobRecv(m_netid, m_cur_msg_basic.cmd, mem, m_cur_msg_content_len);
				m_basic_network->PushJob(jobrecv);

				m_cur_read_state = RMS_READ_MSG_BASIC;
				need_contiue = (m_cur_read_buffer.GetRealLength() > 0);
			}
		}
	} while (need_contiue);

	//重置offset
	m_cur_read_buffer.ResetOffset();
}
#endif

void TcpHandler::OnCanWrite()
{
	if (m_socket == SOCKET_ERROR)
	{
		return;
	}

	for (;;)
	{
		if (m_buff_to_write.buff_length == 0)
		{
			m_buff_mutex.Lock();

			MsgBuffItem msg_tmp = m_buff;
			m_buff = m_buff_to_write;
			m_buff_to_write = msg_tmp;
			m_buff.buff_length = 0;

			m_buff_mutex.Unlock();

			if (m_buff_to_write.buff_length == 0)
			{
				// 没有待写的块
				return;
			}

			if (m_basic_network != 0 && m_buff_to_write.buff_length > 1)
			{
				m_basic_network->UnregisterWrite(m_netid, m_buff_to_write.buff_length - 1);
			}
		}

		while(m_cur_write_length < m_buff_to_write.buff_length)
		{
			int ret = PISocket::Send(m_socket, m_buff_to_write.buff_data + m_cur_write_length, m_buff_to_write.buff_length - m_cur_write_length, 0);
			if (ret <= 0)
			{
				if (ret == SOCKET_ERROR && PISocket::Errno() == EWOULDBLOCK)
				{
					// 写缓冲区满，包内容未发完，等待下次可写时间
					return;
				}

				// 出错
				printf("[TcpHandler::OnCanWrite] Socket read error. error_code:%d error_msg:%s\n", PISocket::Errno(), PISocket::GetErrStr(PISocket::Errno(), 0, 0));
				if (m_basic_network != 0)
				{
					m_basic_network->Remove(m_netid);
				}
				return;
			}
			m_cur_write_length += ret;
		}

		if ( m_basic_network != 0 )
		{
			m_basic_network->UnregisterWrite(m_netid, 1);
		}

		if (m_buff_to_write.buff_capacity > MIN_CHECK_REDUCE_WRITE_BUFF_SIZE && m_buff_to_write.buff_length < m_buff_to_write.buff_capacity * WRITE_BUFF_REDUCE_RATE)
		{
			// 空闲空间太多
			m_buff_to_write.ReduceSize((int)(m_buff_to_write.buff_capacity * WRITE_BUFF_REDUCE_TO_RATE));
		}

		m_cur_write_length = 0;
		m_buff_to_write.buff_length = 0;
	}
}

void TcpHandler::OnClose()
{
	if (m_socket != SOCKET_ERROR)
	{
		// 强制干掉一个socket的两种办法，不过暂时应该不用用到，先放着
		/*
		linger l;
		l.l_onoff = 1;
		l.l_linger = 0;
		int ret = PISocket::SetSockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char *)&l, sizeof(linger));
		*/
		/*
		int b = 0;
		int ret = PISocket::SetSockopt(conn_socket, SOL_SOCKET, SO_DONTLINGER, (const char *)&b, sizeof(bool));
		*/

		PISocket::Shutdown(m_socket, PI_SD_BOTH);
		PISocket::Close(m_socket);
		
		if (m_basic_network != 0)
		{
			JobDisconect *jobdisconnect = new JobDisconect(m_netid);
			m_basic_network->PushJob(jobdisconnect);
		}

		m_socket = SOCKET_ERROR;
	}

	// 从此处开始m_socket已经无效
	m_buff_to_write.Clear();
	m_buff.Clear();
	
	m_cur_read_buffer.Clear();
}

void TcpHandler::CheckCurBuffSize(MsgLen len)
{
	while (len > m_buff.buff_capacity)
	{
		m_buff.CheckSize((int)(m_buff.buff_capacity * WRITE_BUFF_INCREASE_RATE));
	}
}
