#pragma once

struct lua_State;
class LuaErrorProc
{
public:
	LuaErrorProc() { }
	virtual ~LuaErrorProc() { }
	virtual void OnError(lua_State* state, const char* error) const;
	void BindLastError(lua_State* state);
	void BindLastError(lua_State* state, const char* error);
private:
	LuaErrorProc(const LuaErrorProc&);
	void operator=(const LuaErrorProc&);
};

extern void LuaSetErrorProc(LuaErrorProc* errorProc);
extern LuaErrorProc* LuaGetErrorProc();
