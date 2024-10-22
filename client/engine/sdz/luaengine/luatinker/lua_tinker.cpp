// lua_tinker.cpp
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 

#include <iostream>

#include "lua_tinker.h"

#include "luaengine/luaerrorreport.h"

/*---------------------------------------------------------------------------*/ 
/* init                                                                      */ 
/*---------------------------------------------------------------------------*/ 
void lua_tinker::init(lua_State *L)
{
	init_s64(L);
	init_u64(L);
}

/*---------------------------------------------------------------------------*/ 
/* __s64                                                                     */ 
/*---------------------------------------------------------------------------*/ 
static int tostring_s64(lua_State *L)
{
	char temp[64];
	sprintf(temp, "%I64d", *(long long*)lua_topointer(L, 1));
	lua_pushstring(L, temp);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
static int eq_s64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(long long)) == 0);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
static int lt_s64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(long long)) < 0);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
static int le_s64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(long long)) <= 0);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
void lua_tinker::init_s64(lua_State *L)
{
	const char* name = "__s64";
	lua_pushstring(L, name);
	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	lua_pushstring(L, "__tostring");
	lua_pushcclosure(L, tostring_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__eq");
	lua_pushcclosure(L, eq_s64, 0);
	lua_rawset(L, -3);	

	lua_pushstring(L, "__lt");
	lua_pushcclosure(L, lt_s64, 0);
	lua_rawset(L, -3);	

	lua_pushstring(L, "__le");
	lua_pushcclosure(L, le_s64, 0);
	lua_rawset(L, -3);	

	lua_settable(L, LUA_GLOBALSINDEX);
}

/*---------------------------------------------------------------------------*/ 
/* __u64                                                                     */ 
/*---------------------------------------------------------------------------*/ 
static int tostring_u64(lua_State *L)
{
	char temp[64];
	sprintf(temp, "%I64u", *(unsigned long long*)lua_topointer(L, 1));
	lua_pushstring(L, temp);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
static int eq_u64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(unsigned long long)) == 0);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
static int lt_u64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(unsigned long long)) < 0);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
static int le_u64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(unsigned long long)) <= 0);
	return 1;
}

/*---------------------------------------------------------------------------*/ 
void lua_tinker::init_u64(lua_State *L)
{
	const char* name = "__u64";
	lua_pushstring(L, name);
	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	lua_pushstring(L, "__tostring");
	lua_pushcclosure(L, tostring_u64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__eq");
	lua_pushcclosure(L, eq_u64, 0);
	lua_rawset(L, -3);	

	lua_pushstring(L, "__lt");
	lua_pushcclosure(L, lt_u64, 0);
	lua_rawset(L, -3);	

	lua_pushstring(L, "__le");
	lua_pushcclosure(L, le_u64, 0);
	lua_rawset(L, -3);	

	lua_settable(L, LUA_GLOBALSINDEX);
}

/*---------------------------------------------------------------------------*/ 
/* excution                                                                  */ 
/*---------------------------------------------------------------------------*/ 
void lua_tinker::dofile(lua_State *L, const char *filename)
{
	lua_pushcclosure(L, on_error, 0);
	int errfunc = lua_gettop(L);

    if(luaL_loadfile(L, filename) == 0)
	{
		lua_pcall(L, 0, 1, errfunc);
	}
	else
	{
		print_error(L, "%s", lua_tostring(L, -1));
	}

	lua_remove(L, errfunc);
	lua_pop(L, 1);
}

/*---------------------------------------------------------------------------*/ 
void lua_tinker::dostring(lua_State *L, const char* buff)
{
	lua_tinker::dobuffer(L, buff, strlen(buff));
}

/*---------------------------------------------------------------------------*/ 
void lua_tinker::dobuffer(lua_State *L, const char* buff, size_t len)
{
	lua_pushcclosure(L, on_error, 0);
	int errfunc = lua_gettop(L);

    if(luaL_loadbuffer(L, buff, len, "lua_tinker::dobuffer()") == 0)
	{
		lua_pcall(L, 0, 1, errfunc);
	}
	else
	{
		print_error(L, "%s", lua_tostring(L, -1));
	}

	lua_remove(L, errfunc);
	lua_pop(L, 1);
}

/*---------------------------------------------------------------------------*/ 
/* debug helpers                                                             */ 
/*---------------------------------------------------------------------------*/ 
static void call_stack(lua_State* L, int n)
{
    lua_Debug ar;
    if(lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent;
		if(n == 0)
		{
			indent = "->\t";
			lua_tinker::print_error(L, "\t<call stack>");
		}
		else
		{
			indent = "\t";
		}

		if(ar.name)
			lua_tinker::print_error(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			lua_tinker::print_error(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

		call_stack(L, n+1);
	}
}

/*---------------------------------------------------------------------------*/
// 缓存Lua报错信息 add by swordman sue 2016-3-6
static std::string s_error_info;
static bool s_error_trace_toggle = false;
// end

/*---------------------------------------------------------------------------*/ 
int lua_tinker::on_error(lua_State *L)
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

/*---------------------------------------------------------------------------*/ 
void lua_tinker::print_error(lua_State *L, const char* fmt, ...)
{
	char text[4096];

	va_list args;
	va_start(args, fmt);
	vsprintf(text, fmt, args);
	va_end(args);

	lua_pushstring(L, "_ALERT");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isfunction(L, -1))
	{
		lua_pushstring(L, text);
		lua_call(L, 1, 0);
	}
	else
	{
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
		lua_pop(L, 1);
	}
}

/*---------------------------------------------------------------------------*/ 
void lua_tinker::enum_stack(lua_State *L)
{
	int top = lua_gettop(L);
	print_error(L, "Type:%d", top);
	for(int i=1; i<=lua_gettop(L); ++i)
	{
		switch(lua_type(L, i))
		{
		case LUA_TNIL:
			print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
			break;
		case LUA_TBOOLEAN:
			print_error(L, "\t%s	%s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i)?"true":"false");
			break;
		case LUA_TLIGHTUSERDATA:
			print_error(L, "\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TNUMBER:
			print_error(L, "\t%s	%f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			print_error(L, "\t%s	%s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
			break;
		case LUA_TTABLE:
			print_error(L, "\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TFUNCTION:
			print_error(L, "\t%s()	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TUSERDATA:
			print_error(L, "\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TTHREAD:
			print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
			break;
		}
	}
}
 
/*---------------------------------------------------------------------------*/ 
/* read                                                                      */ 
/*---------------------------------------------------------------------------*/ 
template<>
char* lua_tinker::read(lua_State *L, int index)
{
	return (char*)lua_tostring(L, index);				
}

template<>
const char* lua_tinker::read(lua_State *L, int index)
{
	return (const char*)lua_tostring(L, index);		
}

template<>
char lua_tinker::read(lua_State *L, int index)
{
	return (char)lua_tonumber(L, index);				
}

template<>
unsigned char lua_tinker::read(lua_State *L, int index)
{
	return (unsigned char)lua_tonumber(L, index);		
}

template<>
unsigned char* lua_tinker::read(lua_State *L, int index)
{
	return (unsigned char*)lua_tostring(L, index);
}

template<>
short lua_tinker::read(lua_State *L, int index)
{
	return (short)lua_tonumber(L, index);				
}

template<>
unsigned short lua_tinker::read(lua_State *L, int index)
{
	return (unsigned short)lua_tonumber(L, index);	
}

template<>
long lua_tinker::read(lua_State *L, int index)
{
	return (long)lua_tonumber(L, index);				
}

template<>
unsigned long lua_tinker::read(lua_State *L, int index)
{
	return (unsigned long)lua_tonumber(L, index);		
}

template<>
int lua_tinker::read(lua_State *L, int index)
{
	int number = (int)lua_tonumber(L, index);
	return (int)lua_tonumber(L, index);				
}

template<>
unsigned int lua_tinker::read(lua_State *L, int index)
{
	return (unsigned int)lua_tonumber(L, index);		
}

template<>
float lua_tinker::read(lua_State *L, int index)
{
	return (float)lua_tonumber(L, index);				
}

template<>
double lua_tinker::read(lua_State *L, int index)
{
	return (double)lua_tonumber(L, index);			
}

template<>
bool lua_tinker::read(lua_State *L, int index)
{
	if(lua_isboolean(L, index))
		return lua_toboolean(L, index) != 0;				
	else
		return lua_tonumber(L, index) != 0;
}

template<>
void lua_tinker::read(lua_State *L, int index)
{
	return;											
}

template<>
long long lua_tinker::read(lua_State *L, int index)
{
	if(lua_isnumber(L,index))
		return (long long)lua_tonumber(L, index);
	else
		return *(long long*)lua_touserdata(L, index);
}
template<>
unsigned long long lua_tinker::read(lua_State *L, int index)
{
	if(lua_isnumber(L,index))
		return (unsigned long long)lua_tonumber(L, index);
	else
		return *(unsigned long long*)lua_touserdata(L, index);
}

template<>
std::string lua_tinker::read(lua_State *L, int index)
{
	return std::string((char*)lua_tostring(L, index));				
}

template<>
const std::string& lua_tinker::read(lua_State *L, int index)
{
	const int MAX_TMP_STR_NUM = 10;
	static std::string tmp_str_arr[MAX_TMP_STR_NUM];
	static int cur_use_index = 0;

	std::string& cur_str = tmp_str_arr[cur_use_index];
	cur_str = (const char*)lua_tostring(L, index);	
	cur_use_index = (cur_use_index + 1)%MAX_TMP_STR_NUM;
	return cur_str;
}

template<>
lua_tinker::table lua_tinker::read(lua_State *L, int index)
{
	return table(L, index);
}

/*---------------------------------------------------------------------------*/ 
/* push                                                                      */ 
/*---------------------------------------------------------------------------*/ 
template<>
int lua_tinker::push(lua_State *L, char ret)
{
	lua_pushnumber(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, unsigned char ret)
{
	lua_pushnumber(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, short ret)
{
	lua_pushnumber(L, ret);	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, unsigned short ret)
{
	lua_pushnumber(L, ret);	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, long ret)
{
	lua_pushnumber(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, unsigned long ret)
{
	lua_pushnumber(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, int ret)
{
	lua_pushnumber(L, ret);	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, unsigned int ret)
{
	lua_pushnumber(L, ret);	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, float ret)
{
	lua_pushnumber(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, double ret)
{
	lua_pushnumber(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, char* ret)
{
	lua_pushstring(L, ret);		
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, const char* ret)
{
	lua_pushstring(L, ret);	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, bool ret)
{
	lua_pushboolean(L, ret);
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, lua_value* ret)
{
	if(ret) ret->to_lua(L); else lua_pushnil(L);	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, long long ret)			
{ 
	*(long long*)lua_newuserdata(L, sizeof(long long)) = ret;
	lua_pushstring(L, "__s64");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, unsigned long long ret)
{
	*(unsigned long long*)lua_newuserdata(L, sizeof(unsigned long long)) = ret;
	lua_pushstring(L, "__u64");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, lua_tinker::table ret)
{
	lua_pushvalue(L, ret.m_obj->m_index);
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, std::string ret)
{
	lua_pushstring(L, ret.c_str());	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, const std::string& ret)
{
	lua_pushstring(L, ret.c_str());	
	return 1;
}

template<>
int lua_tinker::push(lua_State *L, cocos2d::Vec2 ret)
{
	lua_pushnumber(L, ret.x);
	lua_pushnumber(L, ret.y);
	return 2;
}

/*---------------------------------------------------------------------------*/ 
/* pop                                                                       */ 
/*---------------------------------------------------------------------------*/ 
template<>
void lua_tinker::pop(lua_State *L)
{
	lua_pop(L, 1);
}

template<>	
lua_tinker::table lua_tinker::pop(lua_State *L)
{
	return table(L, lua_gettop(L));
}

/*---------------------------------------------------------------------------*/ 
/* Tinker Class Helper                                                       */ 
/*---------------------------------------------------------------------------*/ 
static void invoke_parent(lua_State *L, const char* funcname)
{
	lua_pushstring(L, "__parent");
	lua_rawget(L, -2);
	if(lua_istable(L,-1))
	{
		lua_pushstring(L, funcname);
		lua_rawget(L, -2);

		if (lua_istable(L, -1) || lua_isfunction(L, -1))
		{
			lua_remove(L, -2);
		}
		else
		{
			lua_remove(L, -1);
			invoke_parent(L, funcname);
			lua_remove(L, -2);
		}
	}
}

void lua_tinker::invoke_child(lua_State* L, const char* func_name)
{
	lua_pushstring(L, "getDescription");
	lua_rawget(L, -2);
	if (lua_isfunction(L, -1))
	{
		lua_pushvalue(L, 1);
		lua_pcall(L, 1, 1, 0);
		const char* description = nullptr;
		description = lua_tostring(L, -1);
		push_meta(L, lua_tostring(L, -1));
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			push_meta(L, space_name::name());
			lua_getfield(L, -1, description);
		}
		//得到类的表
		if (lua_istable(L, -1))
		{
			lua_pushstring(L, func_name);
			lua_rawget(L, -2);
			if (lua_isnil(L, -1))
			{
				lua_remove(L, -1);
				invoke_parent(L, func_name);
				if (lua_isnil(L, -1))
				{
					lua_pushfstring(L, "%s can't find '%s' class variable. (forgot registering class variable ?)", description, func_name);
					on_error(L);
					lua_pop(L, 1);
				}
			}
			lua_replace(L, -3);
			lua_pop(L, 1);
		}
		else
		{
			lua_pushfstring(L, "%s isn't a table ", description);
			on_error(L);
			lua_pop(L, 3);
			lua_pushnil(L);
		}
	}
	else
	{
		lua_getmetatable(L, 1);
		lua_pushstring(L, "__name");
		lua_rawget(L, -2);
		lua_pushfstring(L, "%s can't find %s --- %s can't find getDescription function variable. (forgot registering class variable ?)", 
			lua_tostring(L, -1), func_name, lua_tostring(L, -1));
		on_error(L);
		lua_pop(L, 4);
		lua_pushnil(L);
	}
}

/*---------------------------------------------------------------------------*/ 
/* 增加函数分发，根据参数的个数和类型进行查找重载函数					 	*/ 
/*---------------------------------------------------------------------------*/ 
int lua_tinker::dispatcher(lua_State *L)
{
	int top = lua_gettop(L);
	
	get_currfuncname(L);
	
	std::string newname = lua_tostring(L, -1);
	for (int i = 2; i<= top; i++)
	{
		const char* type = nullptr;
		type = lua_typename(L, lua_type(L, i));
		newname.append(type, 1);
	}
	//获取当前函数table
	get_currtable(L);
	
	lua_pushstring(L, newname.c_str());
	lua_rawget(L, -2);

	if (lua_isfunction(L, -1))
	{
		for (int i = 1; i <= top; i++)
		{
			lua_pushvalue(L, i);
		}
		if (0 != lua_pcall(L, top, 1, 0))
		{
			on_error(L);
			lua_pushnil(L);
		}
	}
	else
	{
		lua_getmetatable(L, 1);
		lua_pushstring(L, "__name");
		lua_rawget(L, -2);

		lua_pushfstring(L, "%s can't find '%s' function. (forgot registering class variable ?)", lua_tostring(L, -1), newname.c_str());
		on_error(L);
		lua_pushnil(L);
	}

	return 1;
}

int lua_tinker::static_dispatcher(lua_State *L)
{
	int top = lua_gettop(L);

	get_currfuncname(L);

	const char* func_name = nullptr;
	func_name = lua_tostring(L, -1);
	std::string newname = func_name;
	for (int i = 1; i<= top; i++)
	{
		const char* type = nullptr;
		type = lua_typename(L, lua_type(L, i));
		newname.append(type, 1);
	}

	get_currtable(L);

	lua_pushstring(L, newname.c_str());
	lua_rawget(L, -2);

	if(lua_isfunction(L, -1))
	{
		for (int i = 1; i <= top; i++)
		{
			lua_pushvalue(L, i);
		}
		if (0 != lua_pcall(L, top, 1, 0))
		{
			on_error(L);
			lua_pushnil(L);
		}
	}
	else
	{
		lua_pushfstring(L, "can't find '%s' function.", newname.c_str());
		on_error(L);
		lua_pushnil(L);
	}

	return 1;
}

int lua_tinker::static_meta_get(lua_State *L)
{
	lua_getmetatable(L, 1);

	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if (lua_istable(L, -1))
	{
		push_currtable(L);
		push_currfuncname(L, lua_tostring(L, 2));
		push_staticdispatcher(L);
	}
	else
	{
		lua_pushstring(L, "__name");
		lua_rawget(L, -3);

		lua_pushfstring(L, "%s can't find '%s' class variable. (forgot registering class variable ?)", lua_tostring(L, -1), lua_tostring(L, 2));
		on_error(L);
		lua_pushnil(L);
	}

	return 1;
}

/*---------------------------------------------------------------------------*/ 
int lua_tinker::meta_get(lua_State *L)
{
	lua_getmetatable(L,1);

	lua_pushvalue(L,2);
	lua_rawget(L,-2);

	bool is_dispatcher = false;
	const char *func_name = nullptr;
	func_name = lua_tostring(L, 2);

	if(lua_isuserdata(L,-1))
	{
		user2type<var_base*>::invoke(L,-1)->get(L);
		lua_remove(L, -2);
	}
	else if (lua_istable(L, -1))
	{
		push_currtable(L);
		is_dispatcher = true;
	}
	else if (lua_isnil(L, -1))
	{
		lua_remove(L, -1);
		get_register(L, func_name);

		if (!lua_isnil(L, -1))
		{
			lua_remove(L, -1);
			invoke_parent(L, func_name);
			if (lua_isnil(L, -1))
			{
				lua_remove(L, -1);
				invoke_child(L, func_name);
			}

			if (lua_istable(L, -1))
			{
				push_currtable(L);
				is_dispatcher = true;
			}
			else if(lua_isnil(L, -1))
			{
				lua_pushstring(L, "__name");
				lua_rawget(L, -3);

				lua_pushfstring(L, "%s can't find '%s' class variable. (forgot registering class variable ?)", lua_tostring(L, -1), lua_tostring(L, 2));
				on_error(L);
				lua_pushnil(L);
			}
		}
	}

	//函数分发
	if (is_dispatcher)
	{
		push_currfuncname(L, func_name);
		
		push_dispatcher(L);
	}

	return 1;
}

/*---------------------------------------------------------------------------*/ 
int lua_tinker::meta_set(lua_State *L)
{
	lua_getmetatable(L,1);
	lua_pushvalue(L,2);
	lua_rawget(L,-2);

	if(lua_isuserdata(L,-1))
	{
		user2type<var_base*>::invoke(L,-1)->set(L);
	}
	else if(lua_isnil(L, -1))
	{
		lua_pushvalue(L,2);
		lua_pushvalue(L,3);
		lua_rawset(L, -4);
	}
	lua_settop(L, 3);
	return 0;
}

/*---------------------------------------------------------------------------*/ 
void lua_tinker::push_meta(lua_State *L, const char* name)
{
	lua_pushstring(L, name);
	lua_gettable(L, LUA_GLOBALSINDEX);
}

void lua_tinker::push_register(lua_State *L, const char* keys, const char* values)
{
	lua_pushstring(L, "engine");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_isnil(L, -1))
	{
		lua_pushstring(L, "engine");
		lua_newtable(L);
		lua_pushstring(L, keys);
		lua_pushstring(L, values);
		lua_rawset(L, -3);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}
	else if (lua_istable(L, -1))
	{
		lua_pushstring(L, keys);
		lua_pushstring(L, values);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
}

void lua_tinker::get_register(lua_State *L, const char* keys)
{
	lua_pushstring(L, "engine");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_istable(L, -1))
	{
		lua_pushstring(L, keys);
		lua_rawget(L, -2);
		lua_remove(L, -2);
	}
	else
	{
		lua_remove(L, -1);
		lua_pushnil(L);
	}
}

void lua_tinker::push_dispatcher(lua_State* L)
{
	lua_pushstring(L, "__dispatcher");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_isnil(L, -1))
	{
		lua_remove(L, -1);
		lua_pushstring(L, "__dispatcher");
		lua_pushcfunction(L, dispatcher);
		lua_rawset(L, LUA_REGISTRYINDEX);
		push_dispatcher(L);
	}
}

void lua_tinker::push_staticdispatcher(lua_State* L)
{
	lua_pushstring(L, "__static_dispatcher");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_isnil(L, -1))
	{
		lua_remove(L, -1);
		lua_pushstring(L, "__static_dispatcher");
		lua_pushcfunction(L, static_dispatcher);
		lua_rawset(L, LUA_REGISTRYINDEX);
		push_staticdispatcher(L);
	}
}

void lua_tinker::push_currfuncname(lua_State* L, const char* func_name)
{
	lua_pushstring(L, "__currfunc");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_isnil(L, -1))
	{
		lua_pushstring(L, "__currfunc");
		lua_newtable(L);
		lua_pushinteger(L, 1);
		lua_pushstring(L, func_name);
		lua_rawset(L, -3);
		lua_settable(L, LUA_REGISTRYINDEX);
	}
	else if(lua_istable(L, -1))
	{
		lua_pushinteger(L, lua_objlen(L, -1) + 1);
		lua_pushstring(L, func_name);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
}

void lua_tinker::get_currfuncname(lua_State *L)
{
	lua_pushstring(L, "__currfunc");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(lua_istable(L, -1))
	{
		int len = lua_objlen(L, -1);
		lua_rawgeti(L, -1, len);
		lua_pushnumber(L, len);
		lua_pushnil(L);
		lua_rawset(L, -4);
		lua_remove(L, -2);
	}
	else
	{
		lua_pushstring(L, "lua_tinker::dispatcher: __currfunc table is nil");
		on_error(L);
	}
}

void lua_tinker::push_currtable(lua_State* L)
{
	lua_pushstring(L, "__currtable");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_isnil(L, -1))
	{
		lua_pushstring(L, "__currtable");
		lua_newtable(L);
		lua_pushinteger(L, 1);
		lua_pushvalue(L, -5);
		lua_rawset(L, -3);
		lua_settable(L, LUA_REGISTRYINDEX);
	}
	else if(lua_istable(L, -1))
	{
		lua_pushinteger(L, lua_objlen(L, -1) + 1);
		lua_pushvalue(L, -3);
		lua_rawset(L, -3);
	}
	lua_pop(L, 2);
}

void lua_tinker::get_currtable(lua_State* L)
{
	lua_pushstring(L, "__currtable");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(lua_istable(L, -1))
	{
		int len = lua_objlen(L, -1);
		lua_rawgeti(L, -1, len);
		lua_pushnumber(L, len);
		lua_pushnil(L);
		lua_rawset(L, -4);
		lua_remove(L, -2);
	}
	else
	{
		lua_pushstring(L, "lua_tinker::get_currtable: __currtable table is nil");
		on_error(L);
	}
}

/*---------------------------------------------------------------------------*/ 
/* table object on stack                                                     */ 
/*---------------------------------------------------------------------------*/ 
lua_tinker::table_obj::table_obj(lua_State* L, int index, const char* name)
	:m_L(L)
	,m_index(index)
	,m_ref(0)
	,m_name(name)
{
	if(lua_isnil(m_L, m_index))
	{
		m_pointer = NULL;
		lua_remove(m_L, m_index);
	}
	else
	{
		m_pointer = lua_topointer(m_L, m_index);
	}
}

lua_tinker::table_obj::~table_obj()
{
	if(validate())
	{
		lua_remove(m_L, m_index);
		if (m_name)
		{
			lua_pushstring(m_L, m_name);
			lua_pushnil(m_L);
			lua_settable(m_L, LUA_GLOBALSINDEX);
		}
	}
}

void lua_tinker::table_obj::inc_ref()
{
	++m_ref;
}

void lua_tinker::table_obj::dec_ref()
{
	if(--m_ref == 0)
		delete this;
}

bool lua_tinker::table_obj::validate()
{
	if(m_pointer != NULL)
	{
		if(m_pointer == lua_topointer(m_L, m_index))
		{
			return true;
		}
		else
		{
			int top = lua_gettop(m_L);

			for(int i=1; i<=top; ++i)
			{
				if(m_pointer == lua_topointer(m_L, i))
				{
					m_index = i;
					return true;
				}
			}

			m_pointer = NULL;
			return false;
		}
	}
	else
	{
        return false;
	}
}

/*---------------------------------------------------------------------------*/ 
/* Table Object Holder                                                       */ 
/*---------------------------------------------------------------------------*/ 
lua_tinker::table::table(lua_State* L)
{
	lua_newtable(L);

	m_obj = new table_obj(L, lua_gettop(L));

	m_obj->inc_ref();
}

lua_tinker::table::table(lua_State* L, const char* name)
{
	lua_pushstring(L, name);
	lua_gettable(L, LUA_GLOBALSINDEX);

	if(lua_istable(L, -1) == 0)
	{
		lua_pop(L, 1);

		lua_newtable(L);
		lua_pushstring(L, name);
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	m_obj = new table_obj(L, lua_gettop(L), name);

	m_obj->inc_ref();
}

lua_tinker::table::table(lua_State* L, int index)
{
	if(index < 0)
	{
		index = lua_gettop(L) + index + 1;
	}

	m_obj = new table_obj(L, index);

	m_obj->inc_ref();
}

lua_tinker::table::table(const table& input)
{
	m_obj = input.m_obj;

	m_obj->inc_ref();
}

lua_tinker::table::~table()
{
	m_obj->dec_ref();
}

/*---------------------------------------------------------------------------*/ 

void lua_tinker::module_addEx(lua_State* L, const char* namespaces) 
{
	lua_tinker::space_name::name(namespaces);

	lua_pushstring(L, namespaces);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if (!lua_istable(L, -1))
	{
		lua_pushstring(L, namespaces);
		lua_newtable(L);
		lua_settable(L, LUA_GLOBALSINDEX);
	}
	lua_pop(L, 1);
}

bool lua_tinker::is_table(table& t)
{
	lua_State* L = t.m_obj->m_L;
	if ( 5 == lua_type(L, t.m_obj->m_index) )
	{
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------------*/ 
