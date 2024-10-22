#include "LuaErrorProc.h"
#include "../ToolKit/Common.h"

#pragma warning( push, 2 )
extern "C" {
	#include "LuaPlus/src/lualib.h"
};
#pragma warning( pop )

static LuaErrorProc* s_errorProc = NULL;

void LuaSetErrorProc(LuaErrorProc* errorProc)
{
	IF_NOT (errorProc)
		return;
	s_errorProc = errorProc;
}

LuaErrorProc* LuaGetErrorProc()
{
	return s_errorProc;
}

void LuaErrorProc::BindLastError(lua_State* state)
{
	IF_NOT (state)
		return;

	IF_OK (lua_isstring(state, -1))
	{
		const char* error = lua_tostring(state, -1);
		IF_NOT (error)
			return;
		this->OnError(state, error);
		lua_pop(state, 1);
	}
	else
		this->OnError(state, "unknown lua error");
}

void LuaErrorProc::BindLastError(lua_State* state, const char* error)
{
	IF_NOT (state && error)
		return;
	this->OnError(state, error);
}

void LuaErrorProc::OnError(lua_State* state, const char* error) const
{
	IF_NOT (state && error)
		return;
	ASSERT(!error);
}
