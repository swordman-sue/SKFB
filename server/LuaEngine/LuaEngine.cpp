#include "LuaEngine.h"

LuaEngine* LuaEngine::Create()
{
	LuaEngine* pEngine = new LuaEngine();
	if (!pEngine->Init())
	{
		delete pEngine;
		pEngine = NULL;
	}

	return pEngine;
}

LuaEngine::LuaEngine()
{
	// make a default error processor
	if (::LuaGetErrorProc() == NULL)
		LuaEngine::CreateErrorProc<LuaErrorProc>();

	m_state = LuaState::Create();
#ifdef LUA_RESETCOROUTINE
	m_threadMgr = new LuaThreadMgr(m_state->GetCState());
#endif
}

LuaEngine::~LuaEngine()
{
	ASSERT(m_state->GetTop() == 0);
#ifdef LUA_RESETCOROUTINE
	delete m_threadMgr;
	m_threadMgr = NULL;
#endif
	LuaState::Destroy(m_state);
}

bool LuaEngine::Init()
{
	bool success = this->OpenLibs();

#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
	this->SetGlobalFunctionChecker();
	this->EnableGlobalFunctionChecker();
#endif

#ifdef LUA_RESETCOROUTINE
	lua_State* L = m_state->GetCState();
	lua_pushlightuserdata(L, m_threadMgr);
	lua_setfield(L, LUA_REGISTRYINDEX, REGISTRY_THREAD_MGR);
#endif

	return success;
}

#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
void LuaEngine::SetGlobalFunctionChecker()
{
	//const char* code = "\
	//local global = {}\
	//setmetatable(_G, {\
	//	__index = function(t, key)\
	//		return global[key]\
	//	end,\
	//	__newindex = function(t, key, value)\
	//		if " GLOBAL_FUNCTION_CHECKER " then\
	//			local v = global[key]\
	//			if v and type(v) == 'function' then\
	//				error('cannot redefine global function `' .. key .. '`', 2)\
	//			end\
	//		end\
	//		global[key] = value\
	//	end\
	//})";

	const char* code = "\
	local global = {}\
	setmetatable(_G, {\
		__index = function(t, key)\
			return global[key]\
		end,\
		__newindex = function(t, key, value)\
			if " GLOBAL_FUNCTION_CHECKER " then\
				local v = global[key]\
				if v then\
					error('cannot redefine global function `' .. key .. '`', 2)\
				end\
			end\
			global[key] = value\
		end\
	})";

	this->DoString(code);
}

void LuaEngine::EnableGlobalFunctionChecker()
{
	lua_State* L = m_state->GetCState();
	LuaAutoBlock autoBlock(L);
	lua_pushboolean(L, 1);
	lua_setglobal(L, GLOBAL_FUNCTION_CHECKER);
}

void LuaEngine::DisableGlobalFunctionChecker()
{
	lua_State* L = m_state->GetCState();
	LuaAutoBlock autoBlock(L);
	lua_pushboolean(L, 0);
	lua_setglobal(L, GLOBAL_FUNCTION_CHECKER);
}
#endif

bool LuaEngine::CPCall(lua_CFunction cFunc, void* pUd)
{
	if (m_state->CPCall(cFunc, pUd))
	{
		::LuaGetErrorProc()->BindLastError((lua_State*)m_state);
		return false;
	}

	return true;
}

bool LuaEngine::OpenLibs()
{
	bool bRet = true;
	bRet &= this->CPCall(luaopen_base, NULL);
	bRet &= this->CPCall(luaopen_table, NULL);
	bRet &= this->CPCall(luaopen_math, NULL);
	bRet &= this->CPCall(luaopen_string, NULL);
	// incompatible!
	// bRet &= this->CPCall(luaopen_package, NULL);
	return bRet;
}

bool LuaEngine::DoFile(const char* fileName)
{
	IF_NOT (fileName)
		return false;

	if (m_state->DoFile(fileName))
	{
		::LuaGetErrorProc()->BindLastError((lua_State*)m_state);
		return false;
	}
	return true;
}

bool LuaEngine::DoString(const char* pszString)
{
	IF_NOT (pszString)
		return false;

	if (m_state->DoString(pszString))
	{
		::LuaGetErrorProc()->BindLastError((lua_State*)m_state);
		return false;
	}
	return true;
}

int LuaEngine::GetUsedMemory() const
{
	return m_state->GC(LUA_GCCOUNT, 0);
}

void LuaEngine::GC() const
{
	m_state->GC(LUA_GCCOLLECT, 0);
}

void LuaEngine::StopGC() const
{
	m_state->GC(LUA_GCSTOP, 0);
}

void LuaEngine::RestartGC() const
{
	m_state->GC(LUA_GCRESTART, 0);
}

void LuaEngine::SetMemoryAlloc(lua_Alloc alloc, void* ud)
{
	lua_setdefaultallocfunction(alloc, ud);
}
