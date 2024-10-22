
#include "luamessageadapter.h"

#include "netadapter/netadapter.h"
#include "zqcommon/logutil.h"
#include "basicnetwork/basicnetwork.h"
#include "basicnetwork/tcphandler.h"

extern "C"
{
	#include "lauxlib.h"
	#include "pbc.h"
}

#ifdef __ANDROID__
#include <stdlib.h>
#endif

#define APR_ALIGN(size, boundary) (((size)+ ((boundary) - 1)) &~((boundary) - 1))

static const luaL_Reg LuaMessageAdapterFuncs[] = {
	{ "ReadHeader",		LuaMessageAdapter::ReadHeader},
	{ "ReadChar",		LuaMessageAdapter::ReadChar},
	{ "ReadUChar",		LuaMessageAdapter::ReadUChar},
	{ "ReadShort",		LuaMessageAdapter::ReadShort},
	{ "ReadUShort",		LuaMessageAdapter::ReadUShort},
	{ "ReadInt",		LuaMessageAdapter::ReadInt},
	{ "ReadUInt",		LuaMessageAdapter::ReadUInt},
	{ "ReadFloat",		LuaMessageAdapter::ReadFloat},
	{ "ReadStr",		LuaMessageAdapter::ReadStr},
	{ "ReadFMT",		LuaMessageAdapter::ReadFMT},
	{ "ReadVoiceMem",	LuaMessageAdapter::ReadVoiceMem},
	{ "ReadBytes",		LuaMessageAdapter::ReadBytes },
	{ "ReadResult",		LuaMessageAdapter::ReadResult },

	{ "WriteHeader",	LuaMessageAdapter::WriteHeader},
	{ "WriteBegin",		LuaMessageAdapter::WriteBegin},
	{ "WriteChar",		LuaMessageAdapter::WriteChar},
	{ "WriteUChar",		LuaMessageAdapter::WriteUChar},
	{ "WriteShort",		LuaMessageAdapter::WriteShort},
	{ "WriteUShort",	LuaMessageAdapter::WriteUShort},
	{ "WriteInt",		LuaMessageAdapter::WriteInt},
	{ "WriteUInt",		LuaMessageAdapter::WriteUInt},
	{ "WriteFloat",		LuaMessageAdapter::WriteFloat},
	{ "WriteStr",		LuaMessageAdapter::WriteStr},
	{ "WriteFMT",		LuaMessageAdapter::WriteFMT},
	{ "WriteVoiceMem",  LuaMessageAdapter::WriteVoiceMem },
	{ "WriteBytes",		LuaMessageAdapter::WriteBytes },

	{ "Connect",		LuaMessageAdapter::Connect},
	{ "Send",			LuaMessageAdapter::Send},
	{ "Disconnect",		LuaMessageAdapter::Disconnect},

#ifdef USE_PROTOBUF_FOR_NET_MSG
	{ "SetupPBDataForNetMsgHead", LuaMessageAdapter::SetupPBDataForNetMsgHead},
#endif

	{0, 0}
};

#ifdef USE_PROTOBUF_FOR_NET_MSG
char* LuaMessageAdapter::s_send_message_header_data = nullptr;
short LuaMessageAdapter::s_send_message_header_data_len = 0;
std::string LuaMessageAdapter::s_send_message_name;
std::string LuaMessageAdapter::s_recv_message_name;
int LuaMessageAdapter::s_recv_message_type = 2;
int LuaMessageAdapter::s_recv_message_ret = 0;

#else
unsigned short LuaMessageAdapter::s_send_message_name = 0;
unsigned short LuaMessageAdapter::s_recv_message_name = 0;
#endif 

char LuaMessageAdapter::s_send_message[MAX_MSG_LENGTH] = {0};
char* LuaMessageAdapter::s_send_message_body = 0;
int LuaMessageAdapter::s_send_message_body_index = 0;

const char* LuaMessageAdapter::s_recv_message = 0;
int LuaMessageAdapter::s_recv_message_len = 0;
int LuaMessageAdapter::s_recv_message_index = 0;
bool LuaMessageAdapter::s_read_result = true;

void LuaMessageAdapter::Init(lua_State *L)
{
	s_recv_message = 0;
	s_recv_message_len = 0;
	s_recv_message_index = 0;
	s_read_result = true;

	memset(s_send_message, 0, sizeof(s_send_message));

#ifdef USE_PROTOBUF_FOR_NET_MSG
	s_send_message_name.clear();
	s_recv_message_name.clear();

	lua_getfield(L, LUA_GLOBALSINDEX, "MessageAdapter");
	lua_pushboolean(L, true);
	lua_setfield(L, -2, "USE_PROTOBUF_FOR_NET_MSG");
	lua_pop(L, 1);

#else
	s_send_message_name = 0;
	s_recv_message_name = 0;
#endif 

	s_send_message_body = s_send_message;
	s_send_message_body_index = 0;
}

void LuaMessageAdapter::Regist(lua_State *L)
{
	luaopen_protobuf_c(L);

	luaL_register(L, "MessageAdapter", LuaMessageAdapterFuncs);
	lua_pop(L, 1);
}

void LuaMessageAdapter::Release()
{
#ifdef USE_PROTOBUF_FOR_NET_MSG
	if (nullptr != s_send_message_header_data)
	{
		delete[] s_send_message_header_data;
		s_send_message_header_data = nullptr;
	}
#endif
}

#ifdef USE_PROTOBUF_FOR_NET_MSG
void LuaMessageAdapter::SetRecvMsg(NetMsgHead* pMsgHead, const char* msg, unsigned int len)
{
	s_recv_message_type = pMsgHead->msgType;
	s_recv_message_name = pMsgHead->msgName;
	s_recv_message_ret = pMsgHead->msgRet;
	s_recv_message = msg;
	s_recv_message_len = len;
	s_recv_message_index = 0;
	s_read_result = true;
}

std::string LuaMessageAdapter::GetSendMsgHeaderType()
{
	return s_send_message_name;
}

std::string LuaMessageAdapter::GetRecvMsgHeaderType()
{
	return s_recv_message_name;
}

int LuaMessageAdapter::ReadHeader(lua_State *L)
{
	lua_pushnumber(L, s_recv_message_type);
	lua_pushstring(L, s_recv_message_name.c_str());
	lua_pushnumber(L, s_recv_message_ret);
	lua_pushnumber(L, (lua_Number)s_recv_message_len);
	return 4;
}

int LuaMessageAdapter::WriteHeader(lua_State *L)
{
	if (!lua_isstring(L, 1))
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteHeader] param1 is not string");
		lua_pushboolean(L, false);
		return 1;
	}

	if (!lua_islightuserdata(L, 2))
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteHeader] param2 is not light userdata");
		lua_pushboolean(L, false);
		return 1;
	}

	if (!lua_isnumber(L, 3))
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteHeader] param3 is not number");
		lua_pushboolean(L, false);
		return 1;
	}

	s_send_message_name = lua_tostring(L, 1);
	char* header_data = (char*)lua_touserdata(L, 2);
	s_send_message_header_data_len = (unsigned int)lua_tonumber(L, 3);

	if (nullptr != s_send_message_header_data)
	{
		delete[] s_send_message_header_data;
	}
	s_send_message_header_data = new char[s_send_message_header_data_len];
	memcpy(s_send_message_header_data, header_data, s_send_message_header_data_len);

	lua_pushboolean(L, true);
	return 1;
}

int LuaMessageAdapter::Send(lua_State *L)
{
	if (!lua_isnumber(L, 1))
	{
		LogUtil::LogError("LuaMessageAdapter::Send error:param is not number");
		lua_pushboolean(L, false);
		return 1;
	}

	unsigned short user_netid = (unsigned short)lua_tonumber(L, 1);

	bool ret = NetAdapter::GetInstance()->Send(user_netid, s_send_message_header_data, s_send_message_header_data_len, s_send_message, (unsigned short)s_send_message_body_index);
	if (nullptr != s_send_message_header_data)
	{
		delete[] s_send_message_header_data;
		s_send_message_header_data = nullptr;
	}
	if (!ret)
	{
		LogUtil::LogError("LuaMessageAdapter::Send invoke NetAdapter::GetInstance().NetSend fail.");
		lua_pushboolean(L, false);
		return 1;
	}

	lua_pushboolean(L, true);
	return 1;
}

int LuaMessageAdapter::SetupPBDataForNetMsgHead(lua_State *L)
{
	if (nullptr == BasicNetwork::pb_data_for_net_msg_head)
	{
		if (!lua_isstring(L, 1))
		{
			LogUtil::LogError("LuaMessageAdapter::SetupPBDataForNetMsgHead error:param is not string");
			lua_pushboolean(L, false);
			return 1;
		}

		size_t sz = 0;
		const char * buffer = luaL_checklstring(L, 1, &sz);
		if (nullptr == buffer)
		{
			LogUtil::LogError("LuaMessageAdapter::SetupPBDataForNetMsgHead error:param invalid");
			lua_pushboolean(L, false);
			return 1;
		}

		BasicNetwork::pb_data_for_net_msg_head = new char[sz];
		BasicNetwork::pb_data_len_for_net_msg_head = sz;
		if (nullptr == BasicNetwork::pb_data_for_net_msg_head || 0 >= BasicNetwork::pb_data_len_for_net_msg_head)
		{
			LogUtil::LogError("LuaMessageAdapter::SetupPBDataForNetMsgHead invalid PB_DATA_FOR_NET_MSG_HEAD");
			lua_pushboolean(L, false);
			return 1;
		}
		memcpy(BasicNetwork::pb_data_for_net_msg_head, buffer, sz);
	}

	lua_pushboolean(L, true);
	return 1;
}

#else
void LuaMessageAdapter::SetRecvMsg(unsigned short cmd, const char* msg, unsigned int len)
{
	s_recv_message_name = cmd;
	s_recv_message = msg;
	s_recv_message_len = len;
	s_recv_message_index = 0;
	s_read_result = true;
}

std::string LuaMessageAdapter::GetSendMsgHeaderType()
{
	char tmpbuf[64];
	memset(tmpbuf, 0, 64);
	sprintf(tmpbuf, "%d", (int)s_send_message_name);
	return tmpbuf;
}

std::string LuaMessageAdapter::GetRecvMsgHeaderType()
{
	char tmpbuf[64];
	memset(tmpbuf, 0, 64);
	sprintf(tmpbuf, "%d", (int)s_recv_message_name);
	return tmpbuf;
}

int LuaMessageAdapter::ReadHeader(lua_State *L)
{
	lua_pushnumber(L, (lua_Number)s_recv_message_name);
	return 1;
}

int LuaMessageAdapter::WriteHeader(lua_State *L)
{
	if (!lua_isnumber(L, 1))
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteHeader] param is not number");
		lua_pushboolean(L, false);
		return 1;
	}
	s_send_message_name = (unsigned short)lua_tonumber(L, 1);

	lua_pushboolean(L, true);
	return 1;
}

int LuaMessageAdapter::Send(lua_State *L)
{
	if (!lua_isnumber(L, 1))
	{
		LogUtil::LogError("LuaMessageAdapter::Send error:param is not number");
		lua_pushboolean(L, false);
		return 1;
	}

	unsigned short user_netid = (unsigned short)lua_tonumber(L, 1);

	bool ret = NetAdapter::GetInstance()->Send(user_netid, s_send_message_name, s_send_message, s_send_message_body_index);
	if (!ret)
	{
		LogUtil::LogError("LuaMessageAdapter::Send invoke NetAdapter::GetInstance().NetSend fail.");
		lua_pushboolean(L, false);
		return 1;
	}
	lua_pushboolean(L, true);
	return 1;
}
#endif

// 当读取长度不足时应该抛异常
#define READNUM(T)\
	if (s_recv_message_len < (int)sizeof(T) + s_recv_message_index)\
	{\
		LogUtil::LogError("LuaMessageAdapter::READNUM "#T" length error:msg_type[%s] msg_len[%d] msg_index[%d]", \
		GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index); \
		s_read_result = false;\
		lua_pushnumber(L, 0);\
		return 1;\
	}\
	\
    T num = 0;\
	num = ReadItemFromBuffer(num);\
	lua_pushnumber(L, (lua_Number)num);\
	return 1;

#define READITEMFROMBUFFER(T)\
	if (s_recv_message_len < (int)sizeof(T) + s_recv_message_index)\
	{\
		LogUtil::LogError("LuaMessageAdapter::READITEMFROMBUFFER "#T" length error:msg_type[%s] msg_len[%d] msg_index[%d]", \
		GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index); \
		s_read_result = false; \
		return 0; \
	}\
	\
	T num = 0; \
	num = ReadItemFromBuffer(num);\
	return num;

int LuaMessageAdapter::ReadChar(lua_State *L)		{ READNUM(char); }
int LuaMessageAdapter::ReadUChar(lua_State *L)		{ READNUM(unsigned char); }
int LuaMessageAdapter::ReadShort(lua_State *L)		{ READNUM(short); }
int LuaMessageAdapter::ReadUShort(lua_State *L)		{ READNUM(unsigned short); }
int LuaMessageAdapter::ReadInt(lua_State *L)		{ READNUM(int); }
int LuaMessageAdapter::ReadUInt(lua_State *L)		{ READNUM(unsigned int); }
int LuaMessageAdapter::ReadFloat(lua_State *L)		{ READNUM(float); }
int LuaMessageAdapter::ReadDouble(lua_State *L)		{ READNUM(double); }
unsigned short LuaMessageAdapter::ReadUShortEx(lua_State *L)	{ READITEMFROMBUFFER(unsigned short); }

void LuaMessageAdapter::ReadStrN(lua_State* L, unsigned short str_len)
{
	if (str_len < 0 || s_recv_message_len < str_len + s_recv_message_index)
	{
		LogUtil::LogError("LuaMessageAdapter::ReadStrN length error:msg_type[%s] msg_len[%d] msg_index[%d] str_len[%d]",
			GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index, str_len);
		s_read_result = false;
		lua_pushstring(L, "");
		return;
	}

	lua_pushlstring(L, s_recv_message + s_recv_message_index, str_len);
	s_recv_message_index += str_len;
}

int LuaMessageAdapter::ReadStr(lua_State *L)
{
	ReadStrN(L, ReadUShortEx(L));
	return 1;
}

int LuaMessageAdapter::ReadFMT(lua_State *L)
{
	if (!lua_isstring(L, 1))
	{
		LogUtil::LogError("LuaMessageAdapter::ReadFMT format error:msg_type[%s] msg_len[%d] msg_index[%d]",
			GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index);
		s_read_result = false;
		return 0;
	}

	const char *fmt = lua_tostring(L, 1);
	const char *beg_fmt = fmt;
	int count = 0;
	while (*fmt)
	{
		char c = *fmt++;
		switch (c)
		{
		case 'i':
			ReadInt(L);	
			break;
		case 'I': 
			ReadUInt(L);	
			break;
		case 'h': 
			ReadShort(L);	
			break;
		case 'H': 
			ReadUShort(L);
			break;
		case 'c': 
			ReadChar(L);	
			break;
		case 'C': 
			ReadUChar(L);	
			break;
		case 'f': 
			ReadFloat(L);
			break;
		case 'd': 
			ReadDouble(L);
			break;
		case 's':
			ReadStr(L);
			break;
		default:
			LogUtil::LogError("LuaMessageAdapter::ReadFMT format[%s] param type error:msg_type[%s] msg_len[%d] msg_index[%d]",
				beg_fmt, GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index);
			s_read_result = false;
			return count;
		}
		++count;
	}

	return count;
}

// 把从协议获取的语音数据压栈传入lua中
int LuaMessageAdapter::ReadVoiceMem(lua_State *L)
{
	const size_t us_len = sizeof(unsigned short);
	// 获取最大长度
	if (s_recv_message_len < (int)us_len + s_recv_message_index)
	{
		LogUtil::LogError("LuaMessageAdapter::ReadVoiceMem mem total length error:msg_type[%s] msg_len[%d] msg_index[%d]",
			GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index);
		return 0;
	}
	unsigned short total_len = *(unsigned short*)(s_recv_message + s_recv_message_index);
	s_recv_message_index += us_len;

	// 获取实际长度
	if (s_recv_message_len < (int)us_len + s_recv_message_index)
	{
		LogUtil::LogError("LuaMessageAdapter::ReadVoiceMem mem real length error:msg_type[%s] msg_len[%d] msg_index[%d]",
			GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index);
		return 0;
	}
	unsigned short mem_len = *(unsigned short*)(s_recv_message + s_recv_message_index);
	s_recv_message_index += us_len;

	// 读取内存
	if (mem_len <= 0 || mem_len>total_len || total_len <= 0 || s_recv_message_len < total_len + s_recv_message_index)
	{
		LogUtil::LogError("LuaMessageAdapter::ReadVoiceMem length error:msg_type[%s] msg_len[%d] msg_index[%d] total_len[%d] mem_len[%d]",
			GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index, (int)total_len, (int)mem_len);
		return 0;
	}

	void *buffer = malloc(mem_len);
	memcpy(buffer, (void *)(s_recv_message + s_recv_message_index), mem_len);
	lua_pushlstring(L, (const char *)buffer, mem_len);
	lua_pushnumber(L, (lua_Number)mem_len);
	free(buffer);
	buffer = 0;

	s_recv_message_index += total_len;

	return 2;
}

int LuaMessageAdapter::ReadBytes(lua_State *L)
{
	if (!lua_isnumber(L, 1))
	{
		LogUtil::LogError("[LuaMessageAdapter::ReadBytes] param is not number:msg_type[%s]",
			GetRecvMsgHeaderType().c_str());
		return 0;
	}

	int len = lua_tonumber(L, 1);
	if (len <= 0)
	{
		LogUtil::LogError("[LuaMessageAdapter::ReadBytes] length error:msg_type[%s] len[%d]",
			GetRecvMsgHeaderType().c_str(), len);
		return 0;
	}

	if (s_recv_message_len < len + s_recv_message_index)
	{
		LogUtil::LogError("[LuaMessageAdapter::ReadBytes] mem total length error:msg_type[%s] msg_len[%d] msg_index[%d] len[%d]",
			GetRecvMsgHeaderType().c_str(), s_recv_message_len, s_recv_message_index, len);
		return 0;
	}

	const char* bytes_ptr = s_recv_message + s_recv_message_index;
	s_recv_message_index += len;
	lua_pushlightuserdata(L, (void*)bytes_ptr);
	return 1;
}

int LuaMessageAdapter::ReadResult(lua_State *L)
{
	lua_pushboolean(L, s_read_result);
	return 1;
}

int LuaMessageAdapter::WriteBegin(lua_State *L)
{
	s_send_message_body_index = 0;
	return 0;
}

#define WRITENUM(T, INDEX)\
	if (MAX_MSG_LENGTH < (int)sizeof(T) + s_send_message_body_index)\
	{\
		LogUtil::LogError("LuaMessageAdapter::WRITENUM "#T" length error:msg_type[%s] msg_index[%d]", \
		GetSendMsgHeaderType().c_str(), s_send_message_body_index); \
		lua_pushboolean(L, false);\
		return 1;\
	}\
	T v = (T)0;\
	if (!lua_isnumber(L, INDEX))\
	{\
		LogUtil::LogError("LuaMessageAdapter::WRITENUM "#T" param type error, is not number:msg_type[%s] msg_index[%d]", \
		GetSendMsgHeaderType().c_str(), s_send_message_body_index); \
		lua_pushboolean(L, false);\
		return 1;\
	}\
	else\
	{\
		v = (T)lua_tonumber(L, INDEX);\
	}\
	WriteItemToBuffer(v);\

#define WRITENUMEX(T, INDEX)\
	WRITENUM(T, INDEX)\
	lua_pushboolean(L, true);\
	return 1;

int LuaMessageAdapter::WriteChar(lua_State *L)		{ WRITENUMEX(char, 1); }
int LuaMessageAdapter::WriteUChar(lua_State *L)		{ WRITENUMEX(unsigned char, 1); }
int LuaMessageAdapter::WriteShort(lua_State *L)		{ WRITENUMEX(short, 1); }
int LuaMessageAdapter::WriteUShort(lua_State *L)	{ WRITENUMEX(unsigned short, 1); }
int LuaMessageAdapter::WriteInt(lua_State *L)		{ WRITENUMEX(int, 1); }
int LuaMessageAdapter::WriteUInt(lua_State *L)		{ WRITENUMEX(unsigned int, 1); }
int LuaMessageAdapter::WriteFloat(lua_State *L)		{ WRITENUMEX(float, 1); }

bool LuaMessageAdapter::WriteStrN(const char* str, unsigned short str_len)
{
	if (str_len < 0 || MAX_MSG_LENGTH < s_send_message_body_index + str_len + sizeof(unsigned short))
	{
		LogUtil::LogError("LuaMessageAdapter::WriteStrN length error:msg_type[%s] msg_index[%d] str_len[%d]", \
			GetSendMsgHeaderType().c_str(), s_send_message_body_index, str_len); \
		return false;
	}

	// 写入字符串长度
	WriteItemToBuffer(str_len);

	// 写入字符串内容
	if (str_len > 0)
	{
		char *v_p = s_send_message_body + s_send_message_body_index;
		strncpy(v_p, str, str_len);
		s_send_message_body_index += str_len;
	}

	return true;
}

int LuaMessageAdapter::WriteStr(lua_State *L)
{
	if (!lua_isstring(L, 1))
	{
		LogUtil::LogError("LuaMessageAdapter::WriteStr error:param is not number");
		lua_pushboolean(L, false);
		return 1;
	}

	const char *str = lua_tostring(L, 1);
	unsigned short str_len = (unsigned short)strlen(str);
	bool ret = WriteStrN(str, str_len);
	lua_pushboolean(L, ret);
	return 1;
}

int LuaMessageAdapter::WriteFMT(lua_State *L)
{
	int n = lua_gettop(L);
	if (!lua_isstring(L, 1))
	{
		LogUtil::LogError("LuaMessageAdapter::WriteFMT format error:msg_type[%s] msg_index[%d]",
			GetSendMsgHeaderType().c_str(), s_send_message_body_index);
		lua_pushboolean(L, false);
		return 1;
	}

	const char *fmt = lua_tostring(L, 1);
	const char *beg_fmt = fmt;

	int index = 1;
	while (*fmt && index < n)
	{
		++index;
		char c = *fmt++;
		switch (c)
		{
		case 'i': { WRITENUM(int, index) } break;
		case 'I': { WRITENUM(unsigned int, index) } break;
		case 'h': { WRITENUM(short, index) } break;
		case 'H': { WRITENUM(unsigned short, index) } break;
		case 'c': { WRITENUM(char, index) } break;
		case 'C': { WRITENUM(unsigned char, index) } break;
		case 'f': { WRITENUM(float, index) } break;
		case 'd': { WRITENUM(double, index) } break;
		case 's':
			{
				const char* tmp_str = lua_tostring(L, index);
				unsigned short str_len = (unsigned short)strlen(tmp_str);
				if (!WriteStrN(tmp_str, str_len))
				{
					lua_pushboolean(L, false);
					return 1;
				}
			}
			break;
		default:
			LogUtil::LogError("LuaMessageAdapter::WriteFMT format[%s] param type error:msg_type[%s] msg_index[%d]",
				beg_fmt, GetSendMsgHeaderType().c_str(), s_send_message_body_index);
			lua_pushboolean(L, false);
			return 1;
		}
	}

	if (*fmt)
	{
		LogUtil::LogError("LuaMessageAdapter::WriteFMT format[%s] param type error, last string is not '\0':msg_type[%s] msg_index[%d]",
			beg_fmt, GetSendMsgHeaderType().c_str(), s_send_message_body_index);
		lua_pushboolean(L, false);
		return 1;
	}

	lua_pushboolean(L, true);
	return 1;
}

// 把lua中传来的语音数据发送给服务端
int LuaMessageAdapter::WriteVoiceMem(lua_State *L)
{
	if (!lua_isstring(L, -2) || !lua_isnumber(L, -1))
	{
		LogUtil::LogError("LuaMessageAdapter::WriteVoiceMem error:param is not valid");
		lua_pushboolean(L, false);
		return 1;
	}

	const char* msg_buffer = lua_tostring(L, -2);
	int msg_buffer_len = lua_tonumber(L, -1);
	unsigned short mem_len = msg_buffer_len;
	unsigned short total_len = APR_ALIGN(mem_len, 4);
	const size_t us_len = sizeof(unsigned short);

	if (MAX_MSG_LENGTH < total_len + s_send_message_body_index)
	{
		LogUtil::LogError("LuaMessageAdapter::WriteVoiceMem length error:msg_type[%s] msg_index[%d] total_len[%d]",
			GetSendMsgHeaderType().c_str(), s_send_message_body_index, total_len);
		lua_pushboolean(L, false);
		return 1;
	}

	memcpy(s_send_message_body + s_send_message_body_index, &total_len, us_len);
	s_send_message_body_index += us_len;

	memcpy(s_send_message_body + s_send_message_body_index, &mem_len, us_len);
	s_send_message_body_index += us_len;

	memset(s_send_message_body + s_send_message_body_index, 0, total_len);
	memcpy(s_send_message_body + s_send_message_body_index, msg_buffer, mem_len);

	s_send_message_body_index += total_len;
	lua_pushboolean(L, true);
	return 1;
}

int LuaMessageAdapter::WriteBytes(lua_State *L)
{
	if (!lua_islightuserdata(L, 1))
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteBytes] param1 is not light userdata:msg_type[%s]",
			GetSendMsgHeaderType().c_str());
		return 0;
	}

	if (!lua_isnumber(L, 2))
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteBytes] param2 is not number:msg_type[%s]",
			GetSendMsgHeaderType().c_str());
		return 0;
	}

	char* bytes_ptr = (char*)lua_touserdata(L, 1);
	if (nullptr == bytes_ptr)
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteBytes] invalid bytes:msg_type[%s]",
			GetSendMsgHeaderType().c_str());
	}

	int len = (int)lua_tonumber(L, 2);
	if (len <= 0 || MAX_MSG_LENGTH < s_send_message_body_index + len)
	{
		LogUtil::LogError("[LuaMessageAdapter::WriteBytes] length error:msg_type[%s] msg_index[%d] len[%d]",
			GetSendMsgHeaderType().c_str(), s_send_message_body_index, len);
		return 0;
	}

	memcpy(s_send_message_body + s_send_message_body_index, bytes_ptr, len);
	s_send_message_body_index += len;
	lua_pushboolean(L, true);
	return 1;
}

int LuaMessageAdapter::Connect(lua_State *L)
{
	if (!lua_isstring(L, 1) || !lua_isnumber(L,2) || !lua_isnumber(L, 3))
	{
		LogUtil::LogError("LuaMessageAdapter::Send error:param is not number");
		lua_pushboolean(L, false);
		lua_pushnumber(L, -1);
		return 2;
	}

	const char* hostname = lua_tostring(L, 1);

	unsigned short port = (unsigned short)lua_tonumber(L, 2);
	unsigned int time_out = (unsigned int)lua_tonumber(L, 3);
	int handle = NetAdapter::GetInstance()->ConnectAsyn(hostname, port, time_out);
	if (-1 == handle)
	{
		LogUtil::LogError("LuaMessageAdapter::Connect invoke ConnectAsyn fail.");
		lua_pushboolean(L, false);
		lua_pushnumber(L, -1);
		return 2; 
	}

	lua_pushboolean(L, true);
	lua_pushnumber(L, (lua_Number)handle);
	return 2;
}

int LuaMessageAdapter::Disconnect(lua_State *L)
{
	if (!lua_isnumber(L,1))
	{
		LogUtil::LogError("LuaMessageAdapter::Disconnect error:param is not number");
		lua_pushboolean(L, false);
		return 1;
	}

	unsigned int net_id = (unsigned int)lua_tonumber(L, 1);
	NetAdapter::GetInstance()->Disconnect(net_id);

	lua_pushboolean(L, true);
	return 1;
}

