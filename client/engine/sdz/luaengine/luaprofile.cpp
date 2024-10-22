extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "luaprofile.h"
#include <list>
#include <unordered_map>
#include <stdarg.h>
#include "luaengine/luaengine.h"
#include "filesystem/filesystemex.h"
#include "gameengine/gameengine.h"

USING_NS_CC;

static std::list<CallFuncInfo> s_callfunc_info_list;
static std::unordered_map<std::string, CallFuncInfo> s_callfunc_info_map;
static LuaProfile::LuaProfileType s_lua_profile_type = LuaProfile::LPT_UNKNOWN;

//函数调用
static void LuaHookCall(lua_State *lua, lua_Debug *ar)
{
	if (0 != strcmp(ar->what, "Lua"))
	{
		return;
	}

	unsigned long begin_time = GameEngine::GetInstance()->GetNowTimeMilli();
	CallFuncInfo cur_info;
	cur_info.src_filename.assign(ar->short_src);
	cur_info.func_type = ar->what;
	cur_info.func_linedef = ar->linedefined;
	cur_info.start_time = begin_time;

	s_callfunc_info_list.push_back(cur_info);
}

//函数返回
static void LuaHookRet(lua_State *lua, lua_Debug *ar)
{
	bool is_lua_call = strcmp(ar->what, "Lua") == 0;
	bool is_trail_call = strcmp(ar->what, "tail") == 0;
	if (!is_lua_call && !is_trail_call)
	{
		return;
	}

	if (s_callfunc_info_list.empty())
		return;

	CallFuncInfo callfunc_info = s_callfunc_info_list.back();
	
	switch (s_lua_profile_type)
	{
		case LuaProfile::LPT_PERFORMANCE_MONITOR:
		{
			if (is_lua_call)
			{
				if (strcmp(callfunc_info.src_filename.c_str(), ar->short_src) != 0 ||
					strcmp(callfunc_info.func_type.c_str(), ar->what) != 0 ||
					callfunc_info.func_linedef != ar->linedefined)
					return;
			}

			s_callfunc_info_list.pop_back();

			unsigned long end_time = GameEngine::GetInstance()->GetNowTimeMilli();
			unsigned long excute_time = end_time - callfunc_info.start_time;

			char temp_cstr[128] = { 0 };
			sprintf(temp_cstr, "%s%s%d", callfunc_info.src_filename.c_str(), callfunc_info.func_type.c_str(), callfunc_info.func_linedef);

			std::string callfunc_key_str(temp_cstr);

			std::unordered_map<std::string, CallFuncInfo>::iterator iter = s_callfunc_info_map.find(callfunc_key_str);
			if (iter == s_callfunc_info_map.end())
			{
				//说明是一个新的加入其中
				callfunc_info.call_num = 1;
				callfunc_info.accu_time = excute_time;
				s_callfunc_info_map.emplace(callfunc_key_str, callfunc_info);
			}
			else
			{
				//说明已经存在
				iter->second.call_num += 1;
				iter->second.accu_time += excute_time;
			}
		} break;

		case LuaProfile::LPT_LATEST_CALLFUNC_MONITOR:
		{
			if (s_callfunc_info_list.size() > 500)
			{
				s_callfunc_info_list.pop_front();
			}
		} break;
	}

	if (LuaProfile::LPT_PERFORMANCE_MONITOR == s_lua_profile_type)
	{
	}
}

static void LuaHook(lua_State *lua, lua_Debug *ar)
{
 	lua_getinfo (lua, "Sn", ar);

	switch (ar->event)
	{
	case LUA_HOOKCALL:
		LuaHookCall(lua, ar);
		break;
	
	case LUA_HOOKRET:
	case LUA_HOOKTAILRET:
		LuaHookRet(lua, ar);
		break;
	}
}

LuaProfile::LuaProfile() 
: m_mask(0)
, m_log_file(nullptr)
{
}

LuaProfile::~LuaProfile (void)
{
	if (nullptr != m_log_file)
	{
		fclose(m_log_file);
	}
}

void LuaProfile::Start(LuaProfileType type)
{
	if (LPT_UNKNOWN != s_lua_profile_type)
	{
		return;
	}
	s_lua_profile_type = type;

	s_callfunc_info_map.clear();
	s_callfunc_info_list.clear();

	if (LPT_PERFORMANCE_MONITOR == s_lua_profile_type || LPT_LATEST_CALLFUNC_MONITOR == s_lua_profile_type)
	{
		SetHooksFlag(LUA_MASKCALL | LUA_MASKRET);
	}
}

void LuaProfile::Stop()
{
	if (LPT_UNKNOWN == s_lua_profile_type)
	{
		return;
	}

	WriteProfileLog();
	if (nullptr != m_log_file)
	{
		fclose(m_log_file);
	}

	SetHooksFlag(0);
	s_callfunc_info_map.clear();
	s_callfunc_info_list.clear();

	s_lua_profile_type = LPT_UNKNOWN;
}

void LuaProfile::SetHooksFlag(int mask)
{
	//设置钩子
	lua_sethook (LuaEngine::GetInstance()->GetLuaState(), LuaHook, mask, 0);
}

void LuaProfile::WriteProfileLog()
{
	switch (s_lua_profile_type)
	{
		case LPT_PERFORMANCE_MONITOR:
		{
			if (s_callfunc_info_map.empty())
			{
				return;
			}

			WriteProfileLog("file_name | func_type | func_linedef | call_num | avag_time | accu_time");

			std::unordered_map<std::string, CallFuncInfo>::iterator iter = s_callfunc_info_map.begin();
			std::multimap<unsigned long, CallFuncInfo> temp_map;
			while (iter != s_callfunc_info_map.end())
			{
				temp_map.insert(std::pair<unsigned long, CallFuncInfo>(iter->second.accu_time, iter->second));
				iter++;
			}

			std::multimap<unsigned long, CallFuncInfo>::const_reverse_iterator r_it = temp_map.rbegin();
			int count = 0;
			while (r_it != temp_map.rend() && count < 100)
			{
				const CallFuncInfo& callfunc_info = r_it->second;

				WriteProfileLog("%s | %s | %d | %d | %f | %f", 
					callfunc_info.src_filename.c_str(), callfunc_info.func_type.c_str(),
					callfunc_info.func_linedef, callfunc_info.call_num,
					float(callfunc_info.accu_time) / callfunc_info.call_num / 1000.0f, callfunc_info.accu_time / 1000.0f);

				++r_it;
				count += 1;
			}
		} break;

		case LPT_LATEST_CALLFUNC_MONITOR:
		{
			if (s_callfunc_info_list.empty())
			{
				return;
			}

			WriteProfileLog("file_name | func_type | func_linedef");

			std::list<CallFuncInfo>::const_reverse_iterator r_it = s_callfunc_info_list.rbegin();
			while (r_it != s_callfunc_info_list.rend())
			{
				const CallFuncInfo& callfunc_info = *r_it;

				WriteProfileLog("%s | %s | %d",
					callfunc_info.src_filename.c_str(), callfunc_info.func_type.c_str(), callfunc_info.func_linedef);

				++r_it;
			}
		} break;
	}
}

void LuaProfile::WriteProfileLog(const char *fmt, ...)
{
	if (nullptr == m_log_file)
	{
		std::string log_path = FileSystemEx::GetInstance()->GetNonVersionPath() + "luaprofile.log";
		m_log_file = fopen(log_path.c_str(), "wb");
	}

	va_list argp;
	va_start(argp, fmt);

	const static int BUFFER_MAX_LEN = 1024 * 16;
	static char buffer[BUFFER_MAX_LEN];
	memset(buffer, 0, sizeof(buffer));

	int msg_length = ::vsprintf(buffer, fmt, argp);
	int tail_length = ::sprintf(buffer + msg_length, "\n");

	fwrite(buffer, 1, msg_length + tail_length, m_log_file);
	fflush(m_log_file);

	va_end(argp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static LuaProfileAgent* s_pLuaProfileAgent = NULL;

LuaProfileAgent::LuaProfileAgent() 
: m_lua_profile(nullptr)
{
}

LuaProfileAgent::~LuaProfileAgent()
{
	if (nullptr != m_lua_profile)
	{
		delete m_lua_profile;
		m_lua_profile = 0;
	}
}

LuaProfileAgent* LuaProfileAgent::GetInstance()
{
	if (nullptr == s_pLuaProfileAgent)
	{
		s_pLuaProfileAgent = new LuaProfileAgent();
	}
	return s_pLuaProfileAgent;
}

void LuaProfileAgent::DelInstance()
{
	if (nullptr != s_pLuaProfileAgent)
	{
		delete s_pLuaProfileAgent;
	}
}

void LuaProfileAgent::Start(LuaProfile::LuaProfileType type)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (nullptr == m_lua_profile)
	{
		m_lua_profile = new LuaProfile();
	}
	m_lua_profile->Start(type);
#endif
}

void LuaProfileAgent::Stop()
{
	if (nullptr != m_lua_profile)
	{
		m_lua_profile->Stop();
	}
}
