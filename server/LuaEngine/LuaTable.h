#pragma once

#pragma warning( push, 2 )
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaPlusCD.h"
#pragma warning( pop )

using namespace LuaPlus;

#include "../ToolKit/Common.h"

class LuaTable;
namespace LPCD
{
	USING_NAMESPACE_LUA;
	inline bool Check(lua_State* L, LuaTable& value);
	inline void Push(lua_State* L, LuaTable& value);
	inline LuaTable Get(TypeWrapper<LuaTable>, lua_State* L, int idx);
	inline bool	Match(TypeWrapper<LuaTable>, lua_State* L, int idx);
}

class LuaTable
{
public:
	LuaTable() { }
	~LuaTable() { }
	LuaTable(const LuaTable& src);
	const LuaTable& operator=(const LuaTable& src);
	// compare the references
	bool operator==(const LuaTable& right) const;

	template <typename T>
	bool Get(const char* key, T& value) const
	{
		IF_NOT (key)
			return false;
		lua_State* L = m_table.GetCState();
		IF_NOT (L)
			return false;

		LuaAutoBlock autoBlock(L);
		m_table.Push();
		int table = lua_gettop(L);
		lua_pushstring(L, key);
		lua_rawget(L, table);
		// match the type
		if (!LPCD::Match(LPCD::TypeWrapper<T>(), L, -1))
			return false;
		// get the value
		value = LPCD::Get(LPCD::TypeWrapper<T>(), L, -1);
		return true;
	}

	template <typename T>
	bool Get(int key, T& value) const
	{
		lua_State* L = m_table.GetCState();
		IF_NOT (L)
			return false;

		LuaAutoBlock autoBlock(L);
		m_table.Push();
		int table = lua_gettop(L);
		lua_rawgeti(L, table, key);
		// match the type
		if (!LPCD::Match(LPCD::TypeWrapper<T>(), L, -1))
			return false;
		// get the value
		value = LPCD::Get(LPCD::TypeWrapper<T>(), L, -1);
		return true;
	}

	template <typename T>
	void Set(const char* key, T value)
	{
		IF_NOT (key)
			return;
		lua_State* L = m_table.GetCState();
		IF_NOT (L)
			return;

		IF_NOT (LPCD::Check(L, value))
			return;
		LuaAutoBlock autoBlock(L);
		m_table.Push();
		int table = lua_gettop(L);
		lua_pushstring(L, key);
		LPCD::Push(L, value);
		lua_settable(L, table);
	}

	template <typename T>
	void Set(int key, T value)
	{
		lua_State* L = m_table.GetCState();
		IF_NOT (L)
			return;

		IF_NOT (LPCD::Check(L, value))
			return;
		LuaAutoBlock autoBlock(L);
		m_table.Push();
		int table = lua_gettop(L);
		lua_pushnumber(L, key);
		LPCD::Push(L, value);
		lua_settable(L, table);
	}

	LuaTable Clone();
	LuaTable DeepClone();
	int GetN() const;
	bool IsValid() const;

	friend bool LPCD::Check(lua_State* L, LuaTable& value);
	friend void LPCD::Push(lua_State* L, LuaTable& value);
	friend LuaTable LPCD::Get(TypeWrapper<LuaTable>, lua_State* L, int idx);
private:
	LuaTable(lua_State* L, int stackIndex);
	LuaTable(const LuaObject& src);
	void Push(lua_State* L);
	LuaObject m_table;
};

namespace LPCD
{
	USING_NAMESPACE_LUA;

	inline bool Check(lua_State* L, LuaTable& table)
	{
		return table.IsValid();
	}

	inline void Push(lua_State* L, LuaTable& value)
	{
		value.Push(L);
	}

	inline bool	Match(TypeWrapper<LuaTable>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TTABLE;
	}

	inline LuaTable	Get(TypeWrapper<LuaTable>, lua_State* L, int idx)
	{
		return LuaTable(L, idx);
	}
}
