#pragma once

#include "Config.h"
#include "LuaErrorProc.h"

#pragma warning( push, 2 )
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaPlusCD.h"
#pragma warning( pop )

extern "C" {
	#include "LuaPlus/src/lualib.h"
};

#include "LuaString.h"
#include "LuaTable.h"
#ifdef LUA_RESETCOROUTINE
#include "LuaThreadMgr.h"
#endif

#include "../ToolKit/Common.h"

// export LuaNil
typedef LPCD::LuaNil LuaNil;

using namespace LuaPlus;

class LuaEngine
{
public:
	static LuaEngine* Create();
	virtual ~LuaEngine();

	// load, compile and execute a file every time
	bool DoFile(const char* fileName);
	// compile and execute a string every time
	bool DoString(const char* pszString);

	// get globals
	template <typename T>
	bool GetGlobal(const char* name, T& var) const
	{
		IF_NOT (name)
			return false;
		lua_State* L = (lua_State*)(m_state);
		LuaAutoBlock autoBlock(L);
		lua_getglobal(L, name);
		// not exist
		if (lua_isnil(L, -1))
			return false;
		// match the type
		if (!LPCD::Match(LPCD::TypeWrapper<T>(), L, -1))
			return false;
		// found a global variable
		var = LPCD::Get(LPCD::TypeWrapper<T>(), L, -1);
		return true;
	}

	// set globals
	template <typename T>
	void SetGlobal(const char* name, T var)
	{
		IF_NOT (name)
			return;
		lua_State* L = (lua_State*)(m_state);
		// check the arguments
		IF_NOT (LPCD::Check(L, var))
			return;
		LPCD::Push(L, var);
		lua_setglobal(L, name);
	}
	
#ifdef LUA_RESETCOROUTINE
	// yield a thread
	int CoYield()
	{
		lua_State* L = m_state->GetCState();

		// get the pending coroutine
		lua_getfield(L, LUA_REGISTRYINDEX, REGISTRY_PENDING_COROUTINE);
		IF_NOT (lua_isthread(L, -1))
		{
			::LuaGetErrorProc()->BindLastError(L, "you must yield a coroutine function.");
			return LUA_ERRRUN;
		}
		lua_State* NL = lua_tothread(L, -1);
		lua_pop(L, 1);

		// set yield status
		lua_pushboolean(L, 1);
		lua_setfield(L, LUA_REGISTRYINDEX, REGISTRY_PENDING_COROUTINE_YIELD);

		return (int)NL;
	}

	// resume a thread
	template <typename RT>
	bool CoResume(int handle, RT rt)
	{
		lua_State* NL = (lua_State*)handle;
		IF_NOT (m_threadMgr->IsUsed(NL))
			return false;

		// argument
		if (!LPCD::Check(NL, rt))
		{
			::LuaGetErrorProc()->BindLastError(NL, ARG_NOT_INIT_ERROR);
			return false;
		}
		LPCD::Push(NL, rt);

		// resume the thread
		int status = lua_resume(NL, 1);
		if (status != 0 && status != LUA_YIELD)
		{
			::LuaGetErrorProc()->BindLastError(NL);
			m_threadMgr->Free(NL);
			return false;
		} else if (status == 0)
			m_threadMgr->Free(NL);
		return true;
	}
#endif
	
	template <typename ErrorProc>
	static void CreateErrorProc()
	{
		static ErrorProc errorProc;
		::LuaSetErrorProc(&errorProc);
	}

	// register global functions
	// panic if you register a function twice
	template <typename Func>
	inline bool Register(const char* funcName, Func func, unsigned int nupvalues = 0)
	{
		IF_NOT (funcName)
			return false;

		m_state->GetGlobals().RegisterDirect(funcName, func, nupvalues);
		return true;
	}

#ifdef LUA_RESETCOROUTINE
	// register coroutines
	// caller should be a Lua function
	// panic if you register a function twice
	template <typename Func>
	inline bool RegisterCoFunction(const char* funcName, Func func, unsigned int nupvalues = 0)
	{
		IF_NOT (funcName)
			return false;

		m_state->GetGlobals().RegisterCoFunction(funcName, func, nupvalues);
		return true;
	}
#endif

	// register class
	template <typename Callee, typename Method>
	inline bool Register(const char* className, const char* methodName, Method method, const Callee* callee, unsigned int nupvalues = 0)
	{
		IF_NOT (methodName && className)
			return false;

		LuaObject objs = m_state->GetGlobals();
		LuaObject mt = objs[className];
		if (mt.IsNil())
		{
			mt = objs.CreateTable(className);
			mt.SetMetaTable(mt);
			mt.SetObject("__index", mt);
		}

		// check
		IF_OK (mt.IsTable())
		{
			mt.RegisterObjectDirect(methodName, callee, method, nupvalues);
			return true;
		}

		return false;
	}

	// bind objects for a lua class
	template <typename Callee>
	bool BindObject(const char* className, const char* objName, const Callee* callee)
	{
		IF_NOT (className && objName && callee)
			return false;

		// lock the stack
		lua_State* L = (lua_State*)(m_state);
		LuaAutoBlock autoBlock(L);

		LuaObject objs = m_state->GetGlobals();
#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
		LuaObject mt = objs.GetByName(className);
#else
		LuaObject mt = objs[className];
#endif
		IF_NOT (mt.IsTable())
			return false;

		/*
		 * 第一次调用 BindObject 后，被绑定的对象类型无法修改，例如：
		 * BindObject("CUser", "user", pUser); // 假定第一次调用
		 * BindObject("CRole", "user", pRole); // 此次调用不能改变 user 的类型到 CRole
		 */
#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
		LuaObject obj = objs.GetByName(objName);
#else
		LuaObject obj = objs[objName];
#endif
		if (obj.IsNil())
		{
			obj = objs.CreateTable(objName);
			obj.SetMetaTable(mt);
		}
		IF_OK (obj.IsTable())
		{
			obj.SetLightUserData("__object", (Callee*)callee);
			return true;
		}

		return false;
	}

	// gc
	// the GC values are expressed in Kbytes
	int GetUsedMemory() const;
	void GC() const;
	void StopGC() const;
	void RestartGC() const;
	static void SetMemoryAlloc(lua_Alloc alloc, void* ud);

	// global function checker
#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
	void SetGlobalFunctionChecker();
	void EnableGlobalFunctionChecker();
	void DisableGlobalFunctionChecker();
#endif

#ifdef _DEBUG_API
	lua_State* GetCStatus() { return m_state->GetCState(); }
#ifdef LUA_RESETCOROUTINE
	void DumpLuaThreadPool() { m_threadMgr->DumpList(); }
#endif
#endif
protected:
	LuaEngine();
	virtual bool OpenLibs();
	bool Init();
	bool CPCall(lua_CFunction cFunc, void* pUd);
private:
	LuaState* m_state;
#ifdef LUA_RESETCOROUTINE
	LuaThreadMgr* m_threadMgr;
#endif

private:
	LuaEngine(const LuaEngine&);
	void operator=(const LuaEngine&);
};
