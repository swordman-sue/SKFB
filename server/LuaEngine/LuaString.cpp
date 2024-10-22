#include "LuaString.h"
#include "LuaPlus/LuaStackObject.h"
#include "../ToolKit/Common.h"

LuaString::LuaString(lua_State* L, int stackIndex)
{
	ASSERT(L);
	ASSERT(lua_isstring(L, stackIndex));
	m_string = LuaObject(L, stackIndex);
}

LuaString::LuaString(const LuaString& src)
{
	m_string = src.m_string;
}

const LuaString& LuaString::operator=(const LuaString& src)
{
	m_string = src.m_string;
	return *this;
}

bool LuaString::operator==(const LuaString& right) const
{
	if (!m_string.GetCState())
		return false;
	return m_string == right.m_string;
}

LuaString::operator const char*() const
{
	if (!m_string.GetCState())
		return NULL;
	return m_string.GetString();
}

void LuaString::Push(lua_State* L)
{
	m_string.Push(L);
}

bool LuaString::IsValid() const
{
	return m_string.GetCState() != NULL;
}