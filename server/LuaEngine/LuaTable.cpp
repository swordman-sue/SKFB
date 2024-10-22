#include "LuaTable.h"
#include "LuaPlus/LuaStackObject.h"
#include "LuaPlus/LuaAutoBlock.h"
#include "../ToolKit/Common.h"

LuaTable::LuaTable(lua_State* L, int stackIndex)
{
	ASSERT(L);
	ASSERT(lua_istable(L, stackIndex));
	m_table = LuaObject(L, stackIndex);
}

LuaTable::LuaTable(const LuaTable& src)
{
	m_table = src.m_table;
}

LuaTable::LuaTable(const LuaObject& src)
{
	ASSERT(src.IsTable());
	m_table = src;
}

const LuaTable& LuaTable::operator=(const LuaTable& src)
{
	m_table = src.m_table;
	return *this;
}

bool LuaTable::operator==(const LuaTable& right) const
{
	if (!m_table.GetCState())
		return false;
	return m_table == right.m_table;
}

void LuaTable::Push(lua_State* L)
{
	m_table.Push(L);
}

LuaTable LuaTable::Clone()
{
	return m_table.Clone();
}

LuaTable LuaTable::DeepClone()
{
	LuaObject table(m_table.GetCState());
	m_table.DeepClone(table);
	return table;
}

int LuaTable::GetN() const
{
	lua_State* L = m_table.GetCState();
	IF_NOT (L)
		return 0;

	LuaAutoBlock autoBlock(L);
	m_table.Push();
	return luaL_getn(L, -1);
}

bool LuaTable::IsValid() const
{
	return m_table.GetCState() != NULL;
}
