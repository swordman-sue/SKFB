#pragma once

#include "../LuaEngine/LuaInterface.h"

#define _LUA_DEBUG

#ifdef _LUA_DEBUG
#include "PlayerManager.h"
#endif

class MapSvrLuaErrorProc : public LuaErrorProc
{
public:
	virtual void OnError(lua_State* state, const char* error) const
	{
		ASSERT(!error);
		MAPLOG(RELEASE_LEVEL, "Lua Error: %s", error);

#ifdef _LUA_DEBUG
		printf("%s\n",error);
#endif
	}
};
