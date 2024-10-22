#pragma once

#include "LuaPlus/LuaObject.h"

using namespace LuaPlus;

class LuaString;
namespace LPCD
{
	USING_NAMESPACE_LUA;
	inline bool Check(lua_State* L, LuaString& str);
	inline void Push(lua_State* L, LuaString& value);
	inline LuaString Get(TypeWrapper<LuaString>, lua_State* L, int idx);
	inline bool	Match(TypeWrapper<LuaString>, lua_State* L, int idx);
}

class LuaString
{
public:
	LuaString() { }
	~LuaString() { }
	LuaString(const LuaString& src);
	const LuaString& operator=(const LuaString& src);
	bool operator==(const LuaString& right) const;
	operator const char*() const;
	bool IsValid() const;
	
	friend bool LPCD::Check(lua_State* L, LuaString& str);
	friend void LPCD::Push(lua_State* L, LuaString& value);
	friend LuaString LPCD::Get(TypeWrapper<LuaString>, lua_State* L, int idx);
private:
	LuaString(lua_State* L, int stackIndex);
	void Push(lua_State* L);
	LuaObject m_string;
};

namespace LPCD
{
	USING_NAMESPACE_LUA;

	inline bool Check(lua_State* L, LuaString& str)
	{
		return str.IsValid();
	}

	inline void Push(lua_State* L, LuaString& value)
	{
		value.Push(L);
	}

	inline LuaString Get(TypeWrapper<LuaString>, lua_State* L, int idx)
	{
		return LuaString(L, idx);
	}

	inline bool	Match(TypeWrapper<LuaString>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TSTRING;
	}
}
