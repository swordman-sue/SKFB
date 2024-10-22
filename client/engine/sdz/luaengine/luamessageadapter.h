
#ifndef LUAMESSAGEADAPTER_h
#define LUAMESSAGEADAPTER_h

#include "zqcommon/typedef.h"

#include <assert.h>
#include <stdlib.h>
#include <memory.h>

struct lua_State;

class LuaMessageAdapter;
struct NetMsgHead;

struct MessageMem
{
	friend class LuaMessageAdapter;
public:
	MessageMem():m_buffer(0),m_buffer_len(0)
	{

	}
	void Create(const void* src_prt, unsigned int len)
	{
		// 很大问题！
		assert(m_buffer==0 && m_buffer_len==0);

		m_buffer_len = len;
		m_buffer = malloc(len);
		memcpy(m_buffer, src_prt, len);
	}
	void Destroy()
	{
		if (m_buffer!=0)
		{
			free(m_buffer);
			m_buffer = 0;
			m_buffer_len = 0;
		}
	}

	void* GetBufferPtr() const
	{
		return m_buffer;
	}

	unsigned int GetBufferLen() const
	{
		return m_buffer_len;
	}

private:
	void*			m_buffer;
	unsigned int	m_buffer_len;
};

class LuaMessageAdapter
{
public:
	static void Init(lua_State *L);
	static void Regist(lua_State *L);
	static void Release();

#ifdef USE_PROTOBUF_FOR_NET_MSG
	static void SetRecvMsg(NetMsgHead* pMsgHead, const char* msg, unsigned int len);
#else
	static void SetRecvMsg(unsigned short cmd, const char* msg, unsigned int len);
#endif

	static std::string GetSendMsgHeaderType();
	static std::string GetRecvMsgHeaderType();

	static int ReadHeader(lua_State *L);
	static int ReadChar(lua_State *L);
	static int ReadUChar(lua_State *L);
	static int ReadShort(lua_State *L);
	static int ReadUShort(lua_State *L);
	static unsigned short ReadUShortEx(lua_State *L);
	static int ReadInt(lua_State *L);
	static int ReadUInt(lua_State *L);
	static int ReadFloat(lua_State *L);
	static int ReadDouble(lua_State *L);
	static void ReadStrN(lua_State* L, unsigned short str_len);
	static int ReadStr(lua_State *L);
	static int ReadFMT(lua_State *L);
	static int ReadVoiceMem(lua_State *L);
	static int ReadBytes(lua_State *L);
	static int ReadResult(lua_State *L);

	static int WriteHeader(lua_State *L);
	static int WriteBegin(lua_State *L);
	static int WriteChar(lua_State *L);
	static int WriteUChar(lua_State *L);
	static int WriteShort(lua_State *L);
	static int WriteUShort(lua_State *L);
	static int WriteInt(lua_State *L);
	static int WriteUInt(lua_State *L);
	static int WriteFloat(lua_State *L);
	static bool WriteStrN(const char* str, unsigned short str_len);
	static int WriteStr(lua_State *L);
	static int WriteFMT(lua_State *L);
	static int WriteVoiceMem(lua_State *L);
	static int WriteBytes(lua_State *L);

	static int Connect(lua_State *L);
	static int Send(lua_State *L);
	static int Disconnect(lua_State *L);

#ifdef USE_PROTOBUF_FOR_NET_MSG
	static int SetupPBDataForNetMsgHead(lua_State *L);
#endif 

	template<typename T>
	static void WriteItemToBuffer(T val)
	{
		//若依据网络字节序的传输标准，此处应参考lua5.3 string.pack中的字节序转换处理
		char *v_p = s_send_message_body + s_send_message_body_index;
		memcpy(v_p, &val, sizeof(T)); 
		s_send_message_body_index += sizeof(T);
	}

	template<typename T>
	static T ReadItemFromBuffer(T default_val)
	{
		//若依据网络字节序的传输标准，此处应参考lua5.3 string.unpack中的字节序转换处理
		T val;
		memcpy(&val, s_recv_message + s_recv_message_index, sizeof(T));
		s_recv_message_index += sizeof(T);
		return val;
	}

#ifdef USE_PROTOBUF_FOR_NET_MSG
	static std::string s_send_message_name;
	static char* s_send_message_header_data;
	static short s_send_message_header_data_len;
	static std::string s_recv_message_name;
	static int s_recv_message_type;
	static int s_recv_message_ret;
#else
	static unsigned short	s_send_message_name;
	static unsigned short	s_recv_message_name;
#endif 

	const static int		MAX_MSG_LENGTH = 1024 * 128;
	static char				s_send_message[MAX_MSG_LENGTH];
	static char*			s_send_message_body;
	static int				s_send_message_body_index;

	static const char*		s_recv_message;
	static int				s_recv_message_len;
	static int				s_recv_message_index;
	static bool				s_read_result;
};

#endif
