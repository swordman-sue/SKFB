
#include "luaengine.h"

#include "zqcommon/logutil.h"
#include "luaengine/luamessageadapter.h"
#include "luaengine/luaerrorreport.h"

#include "ui/UIWidget.h"
#include "view/action/actiontimelineex.h"

extern "C"
{
	#include "lualib.h"
	#include "lauxlib.h"
}

#define VA_PUSH_NUMBER(T) \
	++ nargs; \
	if (nullptr != m_lua_state) { \
		lua_pushnumber(m_lua_state, (double)va_arg(args_list, T)); \
	} \
	break;

#define VA_PUSH_BOOLEAN()\
	++ nargs;\
	if (nullptr != m_lua_state) { \
		lua_pushboolean(m_lua_state, (ZQ::INT32)va_arg(args_list, ZQ::INT32)); \
	} \
	break;

#define VA_POP_NUMBER(T) \
	if (nullptr != m_lua_state) { \
		if (!lua_isnumber(m_lua_state, nresult))\
		{\
			LogUtil::LogError("CallFunction[%s] result fmt error: %s.", name, result_fmt); \
			break; \
		}\
		*va_arg(args_list, T*) = (T)lua_tonumber(m_lua_state, nresult); \
	} \
	break;

// Lua报错堆栈信息 add by swordman sue 2016-3-6
static std::string s_error_info;
static bool s_error_trace_toggle = false;
// end

static void print_error(lua_State *L, const char* fmt, ...)
{
	char text[4096];

	va_list args;
	va_start(args, fmt);
	vsprintf(text, fmt, args);
	va_end(args);

	// Lua报错堆栈信息 add by swordman sue 2016-3-6
	if (s_error_trace_toggle)
	{
		s_error_info += (std::string("\n") + text);
	}
	// end
	else
	{
#if defined (_WIN32)
		LogUtil::LogError("%s\n", text);
#endif
	}
}

static void call_stack(lua_State* L, int n)
{
	lua_Debug ar;
	if (lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent;
		if (n == 0)
		{
			indent = "->\t";
			print_error(L, "\t<call stack>");
		}
		else
		{
			indent = "\t";
		}

		if (ar.name)
			print_error(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			print_error(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

		call_stack(L, n + 1);
	}
}

static int on_error(lua_State *L)
{
	// Lua报错堆栈信息 add by swordman sue 2016-3-6
	s_error_info = "\n";
	s_error_trace_toggle = true;
	// end 

	print_error(L, "%s", lua_tostring(L, -1));

	call_stack(L, 0);

	// Lua报错堆栈信息 add by swordman sue 2016-3-6
	LuaErrorReport::GetInstance()->Check(s_error_info);
	s_error_trace_toggle = false;
	// end

	return 0;
}

ZQ::INT32 LuaEngine::CallFunction(const char *name, const char *arg_fmt, ...)
{
	if (nullptr == m_lua_state)
	{
		return -1;
	}

	lua_getglobal(m_lua_state, name);
	int func_index = lua_gettop(m_lua_state);

	int nargs = 0;
	int nresult = 0;
#ifdef WP8
	va_list args_list = 0;
#else
	va_list args_list;
#endif // _WP8
	const char *result_fmt = 0;
	if (arg_fmt != 0)
	{
		va_start(args_list, arg_fmt);
		const char * p = (const char *)arg_fmt;

		int in_num = 0;
		while (*p != 0 && *p != '>')
		{
			if (*p++ != '#') ++in_num;
		}
		luaL_checkstack(m_lua_state, in_num, "CallFunction Error");

		p = arg_fmt;
		while (*p != 0 && *p != '>')
		{
			switch (*p++)
			{
			case 'i':
				VA_PUSH_NUMBER(ZQ::INT32)
			case 'I':
				VA_PUSH_NUMBER(ZQ::UINT32)
			//case 'l':
			//	VA_PUSH_NUMBER(ZQ::INT64)
			//case 'L':
			//	VA_PUSH_NUMBER(ZQ::UINT64)
			case 'h':
				VA_PUSH_NUMBER(ZQ::INT16)
			case 'H':
				VA_PUSH_NUMBER(ZQ::UINT16)
			case 'd':
				VA_PUSH_NUMBER(double)
			case 'b':
				VA_PUSH_BOOLEAN()
			case 's':
				if (*p == '#')
				{
					++p;
					++ nargs;
					const char *str = va_arg(args_list, char *);
					lua_pushlstring(m_lua_state, str, va_arg(args_list, int));
				}
				else
				{
					++ nargs;
					lua_pushstring(m_lua_state, va_arg(args_list, char *));
				}
				break;
			default:
				LogUtil::LogError("CallFunction[%s] args_fmt error: %s", name, arg_fmt);
				break;
			}
		}
		if (*p == '>') ++p;
		if (*p != 0)
		{
			result_fmt = p;
		}

		while (*p != 0)
		{
			if (*p++ != '#') ++ nresult;
		}
	}

	lua_pushcfunction(m_lua_state, on_error);
	lua_insert(m_lua_state, func_index);
	if (lua_pcall(m_lua_state, nargs, nresult, func_index) != 0)
	{
		//LogUtil::LogError("Call Function[%s] Error.", name);
		lua_remove(m_lua_state, func_index);
		lua_pop(m_lua_state, 1);
		return -1;
	}
	lua_remove(m_lua_state, func_index);

	if (result_fmt != 0)
	{
		int n = nresult;
		nresult = -nresult;
		const char *p = result_fmt;
		while (*p != 0)
		{
			switch (*p++)
			{
			case 'i':
				VA_POP_NUMBER(ZQ::INT32)
			case 'I':
				VA_POP_NUMBER(ZQ::UINT32)
			//case 'l':
			//	VA_POP_NUMBER(ZQ::INT64)
			//case 'L':
			//	VA_POP_NUMBER(ZQ::UINT64)
			case 'h':
				VA_POP_NUMBER(ZQ::INT16)
			case 'H':
				VA_POP_NUMBER(ZQ::UINT16)
			case 'd':
				VA_POP_NUMBER(double)
			case 's':
				if (!lua_isstring(m_lua_state, nresult))
				{
					LogUtil::LogError("CallFunction[%s] result fmt error: %s.", name, result_fmt);
					break;
				}
				if (*p == '#')
				{
					++p;
					char *out_str = va_arg(args_list, char*);
					int *out_len = va_arg(args_list, int*);
					size_t size_out = 0;
					const char *str = lua_tolstring(m_lua_state, nresult, &size_out);
					(size_out >= (size_t)*out_len) ? (size_out = (size_t)(*out_len - 1)) : 0;
					memcpy(out_str, str, size_out);
					out_str[size_out] = 0;
					*out_len = (int)size_out;
				}
				else
				{
					const char *str = lua_tostring(m_lua_state, nresult);
					strcpy(va_arg(args_list, char*), str);
				}
				break;
			default:
				LogUtil::LogError("CallFunction[%s] result_fmt error: %s", name, result_fmt);
				break;
			}
			++nresult;
		}
		lua_pop(m_lua_state, n);
	}

	if (arg_fmt != 0) va_end(args_list);

	return 0;
}

ZQ::INT32 LuaEngine::CallFunction(ZQ::UINT32 call_back_func_id, const char *arg_fmt, ...)
{
	if (nullptr == m_lua_state)
	{
		return -1;
	}

	if (call_back_func_id == 0)
	{
		return -1;
	}

	const char* name = "RunCallBackFunc";

	int nargs = 0;
	int nresult = 0;

#ifdef WP8
	va_list args_list = 0;
#else
		va_list args_list;
#endif // _WP8

	const char *result_fmt = 0;
	lua_getglobal(m_lua_state, name);
	int func_index = lua_gettop(m_lua_state);

	if (arg_fmt != 0)
	{
		va_start(args_list, arg_fmt);
		const char * p = (const char *)arg_fmt;

		int in_num = 1;
		while (*p != 0 && *p != '>')
		{
			if (*p++ != '#') ++in_num;
		}
		luaL_checkstack(m_lua_state, in_num, "CallFunction Error");

		++nargs;
		lua_pushnumber(m_lua_state, call_back_func_id);

		p = arg_fmt;
		while (*p != 0 && *p != '>')
		{
			switch (*p++)
			{
			case 'i':
				VA_PUSH_NUMBER(ZQ::INT32)
			case 'I':
				VA_PUSH_NUMBER(ZQ::UINT32)
			//case 'l':
			//	VA_PUSH_NUMBER(ZQ::INT64)
			//case 'L':
			//	VA_PUSH_NUMBER(ZQ::UINT64)
			case 'h':
				VA_PUSH_NUMBER(ZQ::INT16)
			case 'H':
				VA_PUSH_NUMBER(ZQ::UINT16)
			case 'd':
				VA_PUSH_NUMBER(double)
			case 'b':
				VA_PUSH_BOOLEAN()
			case 'm':
				{
					++ nargs;
					lua_pushlightuserdata(m_lua_state, (MessageMem*)va_arg(args_list, MessageMem*));
					break;
				}
			case 's':
				{
					if (*p == '#')
					{
						++p;
						++ nargs;
						const char *str = va_arg(args_list, char *);
						lua_pushlstring(m_lua_state, str, va_arg(args_list, int));
					}
					else
					{
						++ nargs;
						lua_pushstring(m_lua_state, va_arg(args_list, char *));
					}
					break;
				}
			default:
				LogUtil::LogError("CallFunction[%s] args_fmt error: %s", name, arg_fmt);
				break;
			}
		}
		if (*p == '>') ++p;
		if (*p != 0)
		{
			result_fmt = p;
		}

		while (*p != 0)
		{
			if (*p++ != '#') ++ nresult;
		}
	}

	lua_pushcfunction(m_lua_state, on_error);
	lua_insert(m_lua_state, func_index);
	if (lua_pcall(m_lua_state, nargs, nresult, func_index) != 0)
	{
		//LogUtil::LogError("Call Function[%s] Error.", name);
		lua_remove(m_lua_state, func_index);
		lua_pop(m_lua_state, 1);
		return -1;
	}
	lua_remove(m_lua_state, func_index);

	if (result_fmt != 0)
	{
		int n = nresult;
		nresult = -nresult;
		const char *p = result_fmt;
		while (*p != 0)
		{
			switch (*p++)
			{
			case 'i':
				VA_POP_NUMBER(ZQ::INT32)
			case 'I':
				VA_POP_NUMBER(ZQ::UINT32)
			//case 'l':
			//	VA_POP_NUMBER(ZQ::INT64)
			//case 'L':
			//	VA_POP_NUMBER(ZQ::UINT64)
			case 'h':
				VA_POP_NUMBER(ZQ::INT16)
			case 'H':
				VA_POP_NUMBER(ZQ::UINT16)
			case 'd':
				VA_POP_NUMBER(double)
			case 's':
				if (!lua_isstring(m_lua_state, nresult))
				{
					LogUtil::LogError("CallFunction[%s] result fmt error: %s.", name, result_fmt);
					break;
				}
				if (*p == '#')
				{
					++p;
					char *out_str = va_arg(args_list, char*);
					int *out_len = va_arg(args_list, int*);
					size_t size_out = 0;
					const char *str = lua_tolstring(m_lua_state, nresult, &size_out);
					(size_out >= (size_t)*out_len) ? (size_out = (size_t)(*out_len - 1)) : 0;
					memcpy(out_str, str, size_out);
					out_str[size_out] = 0;
					*out_len = (int)size_out;
				}
				else
				{
					const char *str = lua_tostring(m_lua_state, nresult);
					strcpy(va_arg(args_list, char*), str);
				}
				break;
			default:
				LogUtil::LogError("CallFunction[%s] result_fmt error: %s", name, result_fmt);
				break;
			}
			++nresult;
		}
		lua_pop(m_lua_state, n);
	}

	if (arg_fmt != 0) va_end(args_list);

	return 0;
}

void LuaEngine::VoiceRecordCallBack(ZQ::UINT32 call_back_func_id, unsigned int call_back_id, int result, unsigned int voicd_id, int voice_len, MessageMem& msg_me)
{
	if (call_back_func_id==0)
	{
		LogUtil::LogError("VoiceRecordCallBack call_back_func_id error");
		return;
	}
}

void LuaEngine::UpdateFileListCallBack(unsigned int async_or_http_id, unsigned int func_id, const std::string &file_name, bool is_abort, bool result, int ret_code, int http_ret_code, int op_code, const char* custom_ret_msg, unsigned int data_len)
{
	CallFunction(func_id, "IIbbisIiis", async_or_http_id, func_id, is_abort, result, ret_code, file_name.c_str(), data_len, http_ret_code, op_code, custom_ret_msg);
}

void LuaEngine::CallUrlCallBack(unsigned int http_id, unsigned int func_id, bool is_abort, bool result, int ret_code, const std::string &url, const char *url_data, unsigned int len, int http_ret_code, int op_code)
{
	CallFunction(func_id, "IIbbissIii", http_id, func_id, is_abort, result, ret_code, url.c_str(), url_data, len, http_ret_code, op_code);
}

ZQ::INT32 LuaEngine::DelLuaFuncID(ZQ::UINT32 call_back_func_id)
{
	if (call_back_func_id == 0)
	{
		return -1;
	}
	CallFunction("DelCallBackFunc", "I", call_back_func_id);
	return 0;
}

#define VA_SET_GLOBAL(T) \
	{\
		if (nullptr != m_lua_state)\
		{\
			const char *str = va_arg(args_list, char*); \
			lua_pushnumber(m_lua_state, (double)va_arg(args_list, T)); \
			lua_setglobal(m_lua_state, str); \
		}\
		break;\
	}
#define VA_SET_TABLE(T) \
	if (*result_key == 's')\
	{\
		if (nullptr != m_lua_state)\
		{\
			lua_pushstring(m_lua_state, va_arg(args_list, char*)); \
			lua_pushnumber(m_lua_state, (double)va_arg(args_list, T)); \
			lua_settable(m_lua_state, -3); \
		}\
	}\
	else if(*result_key == 'i')\
	{\
		if (nullptr != m_lua_state)\
		{\
			lua_pushnumber(m_lua_state, va_arg(args_list, int)); \
			lua_pushnumber(m_lua_state, (double)va_arg(args_list, T)); \
			lua_rawset(m_lua_state, -3); \
		}\
	}\
	else\
	{\
		if (nullptr != m_lua_state)\
		{\
			LogUtil::LogError("SetGlobal Format Error![%s]", format); \
			lua_settop(m_lua_state, top_index); \
		}\
		return 1; \
	}\
	break;

ZQ::INT32 LuaEngine::SetVariable(const char *format, ...)
{
	if (nullptr == m_lua_state)
	{
		return 1;
	}

	if (format == 0)
	{
		return 1;
	}

	int top_index = lua_gettop(m_lua_state);

#ifdef _WP8
	va_list args_list = 0;
#else
	va_list args_list;
#endif // _WP8

	va_start(args_list, format);
	const char *result_key = 0;
	const char *result_value = 0;

	const char *p = (const char *)format;

	if (*(p+1) == '>')
	{
		result_key = p;
		if (*result_key != 's')
		{
			LogUtil::LogError("SetGlobal Format Error![%s]", format);
			lua_settop(m_lua_state, top_index);
			return 1;
		}

		result_value = p + 2;
		switch (*result_value)
		{
		case 'i':
			VA_SET_GLOBAL(ZQ::INT32)
		case 'I':
			VA_SET_GLOBAL(ZQ::UINT32)
		//case 'l':
		//	VA_SET_GLOBAL(ZQ::INT64)
		//case 'L':
		//	VA_SET_GLOBAL(ZQ::UINT64)
		case 'd':
			VA_SET_GLOBAL(double)
		case 's':
			if (*(result_value+1) == '#')
			{
				const char *name = va_arg(args_list, char *);
				const char *str = va_arg(args_list, char *);
				lua_pushlstring(m_lua_state, str, va_arg(args_list, int));
				lua_setglobal(m_lua_state, name);
			}
			else
			{
				const char *name = va_arg(args_list, char *);
				lua_pushstring(m_lua_state, va_arg(args_list, char *));
				lua_setglobal(m_lua_state, name);
			}
			break;
		default:
			LogUtil::LogError("SetGlobal Format Error![%s]", format);
			lua_settop(m_lua_state, top_index);
			return 1;
		}
		lua_settop(m_lua_state, top_index);
		return 0;
	}

	if (*p++ != 's')
	{
		LogUtil::LogError("SetGlobal Format Error![%s]", format);
		lua_settop(m_lua_state, top_index);
		return 1;
	}
	else
	{
		const char *str = va_arg(args_list, char *);
		lua_getglobal(m_lua_state, str);
		if (!lua_istable(m_lua_state, -1))
		{
			lua_pop(m_lua_state, -1);

			lua_newtable(m_lua_state);
			lua_setglobal(m_lua_state, str);
			lua_getglobal(m_lua_state, str);
		}
	}

	while (*p != 0 && *(p+1) != '>')
	{
		switch (*p++)
		{
		case 'i':
			{
				int array_index = va_arg(args_list, int);
				lua_pushnumber(m_lua_state, array_index);
				lua_rawget(m_lua_state, -2);

				if (!lua_istable(m_lua_state, -1))
				{
					lua_pop(m_lua_state, 1);
					lua_pushnumber(m_lua_state, array_index);
					lua_newtable(m_lua_state);

					lua_settable(m_lua_state, -3);

					lua_pushnumber(m_lua_state, array_index);
					lua_rawget(m_lua_state, -2);
				}
				break;
			}
		case 's':
			{
				const char *str = va_arg(args_list, char *);
				lua_pushstring(m_lua_state, str);
				lua_gettable(m_lua_state, -2);
				if (!lua_istable(m_lua_state, -1))
				{
					lua_pop(m_lua_state, -1);
					lua_pushstring(m_lua_state, str);
					lua_newtable(m_lua_state);

					lua_settable(m_lua_state, -3);

					lua_pushstring(m_lua_state, str);
					lua_gettable(m_lua_state, -2);
				}
				break;
			}
		default:
			LogUtil::LogError("SetGlobal Format Error![%s]", format);
			lua_settop(m_lua_state, top_index);
			return 1;
		}

	}

	if (*(p+1) == '>') 
	{
		result_key = p;
		result_value = p+2;
	}

	if (result_key == 0 || result_value == 0)
	{
		LogUtil::LogError("SetGlobal Format Error![%s]", format);
		lua_settop(m_lua_state, top_index);
		return 1;
	}

	switch (*result_value)
	{
	case 'i':
		VA_SET_TABLE(ZQ::INT32)
	case 'I':
		VA_SET_TABLE(ZQ::UINT32)
	//case 'l':
	//	VA_SET_TABLE(ZQ::INT64)
	//case 'L':
	//	VA_SET_TABLE(ZQ::UINT64)
	case 'd':
		VA_SET_TABLE(double)
	case 's':
		if (*(result_value+1) == '#')
		{
			if (*result_key == 's')
			{
				lua_pushstring(m_lua_state, va_arg(args_list, char *));
				lua_pushlstring(m_lua_state, va_arg(args_list, char *), va_arg(args_list, ZQ::INT32));
				lua_settable(m_lua_state, -3);
			}
			else if(*result_key == 'i')
			{
				lua_pushnumber(m_lua_state, va_arg(args_list, int));
				lua_pushlstring(m_lua_state, va_arg(args_list, char *), va_arg(args_list, ZQ::INT32));
				lua_rawset(m_lua_state, -3);
			}
			else
			{
				LogUtil::LogError("SetGlobal Format Error![%s]", format);
				lua_settop(m_lua_state, top_index);
				return 1;
			}

		}
		else
		{
			if (*result_key == 's')
			{
				lua_pushstring(m_lua_state, va_arg(args_list, char *));
				lua_pushstring(m_lua_state, va_arg(args_list, char *));
				lua_settable(m_lua_state, -3);
			}
			else if (*result_key == 'i')
			{
				lua_pushnumber(m_lua_state, va_arg(args_list, ZQ::INT32));
				lua_pushstring(m_lua_state, va_arg(args_list, char *));
				lua_rawset(m_lua_state, -3);
			}
			else
			{
				LogUtil::LogError("SetGlobal Format Error![%s]", format);
				lua_settop(m_lua_state, top_index);
				return 1;
			}
		}
		break;
	default:
		LogUtil::LogError("SetGlobal Format Error![%s]", format);
		lua_settop(m_lua_state, top_index);
		return 1;
	}

	lua_settop(m_lua_state, top_index);

	return 0;
}

