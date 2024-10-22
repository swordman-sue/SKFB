#pragma once

#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <memory.h>
#include <assert.h>
#include <string>

#include "def.h"
#include "basicnetworkhandler.h"
#include "../../common/platform/thread/mutex.h"

class MsgMem;

#pragma pack(push)
#pragma pack(1)

#ifdef USE_PROTOBUF_FOR_NET_MSG
struct NetMsgBasic
{
	unsigned short len;			//�ܰ���(�����������ֽڳ���)
	unsigned short headLen;		//protobuf��Ϣͷ��
};

//protobuf��Ϣͷ
struct NetMsgHead
{
	//Э���������� 1��ʾ����� 2��ʾ��Ӧ�� 3 ���Ͱ�
	int msgType;

	//Э��ṹ��
	std::string msgName;

	//ͨѶЭ������� ֻ������Ӧ���� ��0��ʾ���� ���������û��PBBody
	int msgRet;
};

#else
struct NetMsgBasic
{
	unsigned int len;			//�ܰ���(�����������ֽڳ���)
	unsigned int order_id;		//����ID
	unsigned short cmd;	//Э���

	unsigned int GetMsgContentLen() { return len - sizeof(order_id) - sizeof(cmd); }
};
#endif 

#pragma pack(pop)

class TcpHandler : public BasicNetworkHandler
{
public:
	TcpHandler(SOCKET socket, int max_package_size);
	virtual ~TcpHandler();

	void OnCanRead();
	void OnCanWrite();
	void OnClose();

protected:
	friend class BasicNetwork;

#ifdef USE_PROTOBUF_FOR_NET_MSG
	void SendPackage(const char* header_buffer, unsigned short hearder_buffer_len, const char *buffer, unsigned short len)
	{
		m_buff_mutex.Lock();

		MsgLen total_len = m_buff.buff_length + hearder_buffer_len + len + sizeof(NetMsgBasic);
		CheckCurBuffSize(total_len);
		m_buff.WriteClientMsg(header_buffer, hearder_buffer_len, buffer, len);

		m_buff_mutex.Unlock();
	}
#else
	void SendPackage(short cmd, const char *buffer, unsigned int len)
	{
		m_buff_mutex.Lock();

		MsgLen total_len = m_buff.buff_length + len + sizeof(NetMsgBasic);
		CheckCurBuffSize(total_len);
		m_buff.WriteClientMsg(++m_order_id, cmd, buffer, len);

		m_buff_mutex.Unlock();
	}
#endif

private:
	TcpHandler(const TcpHandler&);
	TcpHandler& operator=(const TcpHandler&);

	class MsgBuffItem
	{
	public:
		MsgBuffItem();
		void CheckSize(int len);
		void ReduceSize(int len);
		void Clear();

		//add data
		void Append(const char* data, int data_len);

		template<typename T>
		void AppendItem(T val)
		{
			//�����������ֽ���Ĵ����׼���˴�Ӧ�ο�lua5.3 string.pack�е��ֽ���ת������
			CheckSize(buff_length + sizeof(T));
			char* tmp_write_buf = (char*)buff_data + buff_length;
			memcpy(tmp_write_buf, &val, sizeof(T));
			buff_length += sizeof(T);
		}

		template<typename T>
		T ReadItem(T default_val)
		{
			//�����������ֽ���Ĵ����׼���˴�Ӧ�ο�lua5.3 string.unpack�е��ֽ���ת������
			T val = default_val;
			assert((unsigned int)sizeof(T)+buff_offset <= buff_length);

			char* cur_ptr = (char*)buff_data + buff_offset;
			memcpy(&val, cur_ptr, sizeof(T));
			buff_offset += sizeof(T);

			return val;
		}

#ifdef USE_PROTOBUF_FOR_NET_MSG
		bool WriteClientMsg(const char* header_data, unsigned short header_data_len, const char* data, unsigned short data_len);
		bool ReadServerMsgHead(unsigned int header_data_len, NetMsgHead* net_msg_head);
#else
		void WriteClientMsg(int order_id, short cmd, const char* data, unsigned int data_len);
#endif

		void ReadServerMsgBasic(NetMsgBasic* msg_basic);

		MsgLen GetRealLength(){ return buff_length - buff_offset; }

		char* GetOffsetPtr(){ return (char*)buff_data + buff_offset; }

		void AddOffset(MsgLen offset){ buff_offset += offset; }

		//offset to zero
		void ResetOffset();

		MsgMem*		buff_data;
		MsgLen		buff_capacity;
		MsgLen		buff_length;
		MsgLen		buff_offset;
	};

	void CheckCurBuffSize(MsgLen len);

	void HandleOnCanRead();

	//��ǰд������
	MsgBuffItem	m_buff_to_write;
	MsgBuffItem m_buff;
	Mutex		m_buff_mutex;
	// ��д����
	MsgLen m_cur_write_length;		
	// һ���������ô�С����Ϊ�ͻ��˲��ɿ������ϵضϿ���
	int m_max_package_size;			

	enum ReadMsgState
	{
		RMS_READ_MSG_BASIC = 0,
		RMS_READ_MSG_HEAD = 1,
		RMS_READ_CONTENT = 2,
	};

	//��ǰ��ȡ״̬
	ReadMsgState m_cur_read_state;		
	//��ǰ��ȡ������
	MsgBuffItem  m_cur_read_buffer;
	//ʣ����Ϣ����
	MsgLen m_cur_msg_content_len;
	//��ǰ��Ϣ������Ϣ
	NetMsgBasic m_cur_msg_basic;

	int m_order_id;	//����ID

#ifdef USE_PROTOBUF_FOR_NET_MSG
	//protobuf��Ϣͷ
	NetMsgHead m_cur_net_msg_head;
#endif
};

#endif
