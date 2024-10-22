///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2010 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
//
// Modified by Liang Guodong ( 2011/11/10 )
///////////////////////////////////////////////////////////////////////////////
#ifndef LUAFUNCTION_H
#define LUAFUNCTION_H

#include "LuaPlusInternal.h"
#include "LuaAutoBlock.h"
#include "../LuaErrorProc.h"
#ifdef LUA_RESETCOROUTINE
#include "../LuaThreadMgr.h"
#include "../LuaRegistryKey.h"
#endif

#if LUAPLUS_EXTENSIONS

namespace LuaPlus
{
	class LuaFunction;
	class LuaFunctionVoid;
#ifdef LUA_RESETCOROUTINE
	class LuaCoroutine;
#endif
}

namespace LPCD
{
	USING_NAMESPACE_LUA;

	// LuaFunction
	inline bool Check(lua_State* L, LuaFunction& value);
	inline void Push(lua_State* L, LuaFunction& value);
	inline LuaFunction Get(TypeWrapper<LuaFunction>, lua_State* L, int idx);
	inline bool	Match(TypeWrapper<LuaFunction>, lua_State* L, int idx);

	// LuaFunctionVoid
	inline bool Check(lua_State* L, LuaFunctionVoid& value);
	inline void Push(lua_State* L, LuaFunctionVoid& value);
	inline LuaFunctionVoid Get(TypeWrapper<LuaFunctionVoid>, lua_State* L, int idx);
	inline bool	Match(TypeWrapper<LuaFunctionVoid>, lua_State* L, int idx);

#ifdef LUA_RESETCOROUTINE
	// LuaCoroutine
	inline bool Check(lua_State* L, LuaCoroutine& value);
	inline void Push(lua_State* L, LuaCoroutine& value);
	inline LuaCoroutine Get(TypeWrapper<LuaCoroutine>, lua_State* L, int idx);
	inline bool	Match(TypeWrapper<LuaCoroutine>, lua_State* L, int idx);
#endif
}

namespace LuaPlus {

#define ARG_NOT_INIT_ERROR "The arguments must be initialized."
#define RETURN_VALUE_ERROR "The return type and value do not match."

class LuaFunctionBase
{
public:
	LuaFunctionBase() { }
	bool IsValid() const {
		return functionObj.GetCState() != NULL;
	}

protected:
	void Push(lua_State* L) {
		functionObj.Push(L);
	}

protected:
	LuaObject functionObj;
};

#define LUAFUNCTION_PRECALL() \
		if (!functionObj.GetCState()) \
			return false; \
		lua_State* L = functionObj.GetCState(); \
		LuaAutoBlock autoBlock(L); \
		functionObj.Push();

#define LUAFUNCTION_POSTCALL(numArgs) \
		if (lua_pcall(L, numArgs, 1, 0)) { \
			::LuaGetErrorProc()->BindLastError(L); \
			return false; \
		} \
		if (!LPCD::Match(LPCD::TypeWrapper<RT>(), L, -1)) { \
			::LuaGetErrorProc()->BindLastError(L, RETURN_VALUE_ERROR); \
			return false; \
		} \
		rt = LPCD::Get(LPCD::TypeWrapper<RT>(), L, -1); \
		return true;


/**
**/
class LuaFunction : public LuaFunctionBase
{
public:
	LuaFunction() { }

	template <typename RT>
	bool operator()(RT& rt) {
		LUAFUNCTION_PRECALL();
		LUAFUNCTION_POSTCALL(0);
	}

	template <typename RT, typename P1>
	bool operator()(P1 p1, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LUAFUNCTION_POSTCALL(1);
	}

	template <typename RT, typename P1, typename P2>
	bool operator()(P1 p1, P2 p2, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LUAFUNCTION_POSTCALL(2);
	}

	template <typename RT, typename P1, typename P2, typename P3>
	bool operator()(P1 p1, P2 p2, P3 p3, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LUAFUNCTION_POSTCALL(3);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LUAFUNCTION_POSTCALL(4);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)
			|| !LPCD::Check(L, p5)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LPCD::Push(L, p5);
		LUAFUNCTION_POSTCALL(5);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)
			|| !LPCD::Check(L, p5)
			|| !LPCD::Check(L, p6)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LPCD::Push(L, p5);
		LPCD::Push(L, p6);
		LUAFUNCTION_POSTCALL(6);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, RT& rt) {
		LUAFUNCTION_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)
			|| !LPCD::Check(L, p5)
			|| !LPCD::Check(L, p6)
			|| !LPCD::Check(L, p7)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LPCD::Push(L, p5);
		LPCD::Push(L, p6);
		LPCD::Push(L, p7);
		LUAFUNCTION_POSTCALL(7);
	}

	friend bool LPCD::Check(lua_State* L, LuaFunction& value);
	friend void LPCD::Push(lua_State* L, LuaFunction& value);
	friend LuaFunction LPCD::Get(TypeWrapper<LuaFunction>, lua_State* L, int idx);
protected:
	LuaFunction::LuaFunction(lua_State* L, int stackIndex)
	{
		functionObj = LuaObject(L, stackIndex);
	}
};


#define LUAFUNCTIONVOID_PRECALL() \
		if (!functionObj.GetCState()) \
			return false; \
		lua_State* L = functionObj.GetCState(); \
		LuaAutoBlock autoBlock(L); \
		functionObj.Push();

#define LUAFUNCTIONVOID_POSTCALL(numArgs) \
		if (lua_pcall(L, numArgs, 1, 0)) { \
			::LuaGetErrorProc()->BindLastError(L); \
			return false; \
		} \
		return true;


/**
**/
class LuaFunctionVoid : public LuaFunctionBase
{
public:
	LuaFunctionVoid() { }

	bool operator()() {
		LUAFUNCTIONVOID_PRECALL();
		LUAFUNCTIONVOID_POSTCALL(0);
	}

	template <typename P1>
	bool operator()(P1 p1) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LUAFUNCTIONVOID_POSTCALL(1);
	}

	template <typename P1, typename P2>
	bool operator()(P1 p1, P2 p2) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LUAFUNCTIONVOID_POSTCALL(2);
	}

	template <typename P1, typename P2, typename P3>
	bool operator()(P1 p1, P2 p2, P3 p3) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LUAFUNCTIONVOID_POSTCALL(3);
	}

	template <typename P1, typename P2, typename P3, typename P4>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LUAFUNCTIONVOID_POSTCALL(4);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)
			|| !LPCD::Check(L, p5)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LPCD::Push(L, p5);
		LUAFUNCTIONVOID_POSTCALL(5);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)
			|| !LPCD::Check(L, p5)
			|| !LPCD::Check(L, p6)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LPCD::Push(L, p5);
		LPCD::Push(L, p6);
		LUAFUNCTIONVOID_POSTCALL(6);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
		LUAFUNCTIONVOID_PRECALL();

		if (!LPCD::Check(L, p1)
			|| !LPCD::Check(L, p2)
			|| !LPCD::Check(L, p3)
			|| !LPCD::Check(L, p4)
			|| !LPCD::Check(L, p5)
			|| !LPCD::Check(L, p6)
			|| !LPCD::Check(L, p7)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(L, p1);
		LPCD::Push(L, p2);
		LPCD::Push(L, p3);
		LPCD::Push(L, p4);
		LPCD::Push(L, p5);
		LPCD::Push(L, p6);
		LPCD::Push(L, p7);
		LUAFUNCTIONVOID_POSTCALL(7);
	}

	friend bool LPCD::Check(lua_State* L, LuaFunctionVoid& value);
	friend void LPCD::Push(lua_State* L, LuaFunctionVoid& value);
	friend LuaFunctionVoid LPCD::Get(TypeWrapper<LuaFunctionVoid>, lua_State* L, int idx);
protected:
	LuaFunctionVoid::LuaFunctionVoid(lua_State* L, int stackIndex)
	{
		functionObj = LuaObject(L, stackIndex);
	}
};

#ifdef LUA_RESETCOROUTINE
#define LUACOROUTINE_PRECALL() \
		if (!functionObj.GetCState()) return false; \
		lua_State* L = functionObj.GetCState(); \
		lua_getfield(L, LUA_REGISTRYINDEX, REGISTRY_THREAD_MGR); \
		LuaThreadMgr* threadMgr = (LuaThreadMgr*)lua_touserdata(L, -1); \
		lua_pop(L, 1); \
		if (!threadMgr) return false; \
		lua_State* NL = threadMgr->Alloc(); \
		functionObj.Push(NL);

#define LUACOROUTINE_POSTCALL(numArgs) \
		int status = lua_resume(NL, numArgs); \
		if (status != 0 && status != LUA_YIELD) { \
			::LuaGetErrorProc()->BindLastError(NL); \
			threadMgr->Free(NL); \
			return false; \
		} else if (status == 0) \
			threadMgr->Free(NL); \
		return true;

/**
**/
class LuaCoroutine : public LuaFunctionBase
{
public:
	LuaCoroutine() { }

	bool operator()() {
		LUACOROUTINE_PRECALL();
		LUACOROUTINE_POSTCALL(0);
	}

	template <typename P1>
	bool operator()(P1 p1) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LUACOROUTINE_POSTCALL(1);
	}

	template <typename P1, typename P2>
	bool operator()(P1 p1, P2 p2) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)
			|| !LPCD::Check(NL, p2)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LPCD::Push(NL, p2);
		LUACOROUTINE_POSTCALL(2);
	}

	template <typename P1, typename P2, typename P3>
	bool operator()(P1 p1, P2 p2, P3 p3) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)
			|| !LPCD::Check(NL, p2)
			|| !LPCD::Check(NL, p3)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LPCD::Push(NL, p2);
		LPCD::Push(NL, p3);
		LUACOROUTINE_POSTCALL(3);
	}

	template <typename P1, typename P2, typename P3, typename P4>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)
			|| !LPCD::Check(NL, p2)
			|| !LPCD::Check(NL, p3)
			|| !LPCD::Check(NL, p4)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LPCD::Push(NL, p2);
		LPCD::Push(NL, p3);
		LPCD::Push(NL, p4);
		LUACOROUTINE_POSTCALL(4);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)
			|| !LPCD::Check(NL, p2)
			|| !LPCD::Check(NL, p3)
			|| !LPCD::Check(NL, p4)
			|| !LPCD::Check(NL, p5)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LPCD::Push(NL, p2);
		LPCD::Push(NL, p3);
		LPCD::Push(NL, p4);
		LPCD::Push(NL, p5);
		LUACOROUTINE_POSTCALL(5);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)
			|| !LPCD::Check(NL, p2)
			|| !LPCD::Check(NL, p3)
			|| !LPCD::Check(NL, p4)
			|| !LPCD::Check(NL, p5)
			|| !LPCD::Check(NL, p6)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LPCD::Push(NL, p2);
		LPCD::Push(NL, p3);
		LPCD::Push(NL, p4);
		LPCD::Push(NL, p5);
		LPCD::Push(NL, p6);
		LUACOROUTINE_POSTCALL(6);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	bool operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
		LUACOROUTINE_PRECALL();

		if (!LPCD::Check(NL, p1)
			|| !LPCD::Check(NL, p2)
			|| !LPCD::Check(NL, p3)
			|| !LPCD::Check(NL, p4)
			|| !LPCD::Check(NL, p5)
			|| !LPCD::Check(NL, p6)
			|| !LPCD::Check(NL, p7)) {
			::LuaGetErrorProc()->BindLastError(L, ARG_NOT_INIT_ERROR);
			return false;
		}

		LPCD::Push(NL, p1);
		LPCD::Push(NL, p2);
		LPCD::Push(NL, p3);
		LPCD::Push(NL, p4);
		LPCD::Push(NL, p5);
		LPCD::Push(NL, p6);
		LPCD::Push(NL, p7);
		LUACOROUTINE_POSTCALL(7);
	}

	friend bool LPCD::Check(lua_State* L, LuaCoroutine& value);
	friend void LPCD::Push(lua_State* L, LuaCoroutine& value);
	friend LuaCoroutine LPCD::Get(TypeWrapper<LuaCoroutine>, lua_State* L, int idx);
protected:
	LuaCoroutine::LuaCoroutine(lua_State* L, int stackIndex)
	{
		functionObj = LuaObject(L, stackIndex);
	}
};
#endif

} // namespace LuaPlus

namespace LPCD
{
	USING_NAMESPACE_LUA;

	// LuaFunction
	inline bool Check(lua_State* L, LuaFunction& value)
	{
		return value.IsValid();
	}

	inline void Push(lua_State* L, LuaFunction& value)
	{
		value.Push(L);
	}

	inline bool	Match(TypeWrapper<LuaFunction>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TFUNCTION;
	}

	inline LuaFunction Get(TypeWrapper<LuaFunction>, lua_State* L, int idx)
	{
		return LuaFunction(L, idx);
	}

	// LuaFunctionVoid
	inline bool Check(lua_State* L,LuaFunctionVoid& value)
	{
		return value.IsValid();
	}

	inline void Push(lua_State* L,LuaFunctionVoid& value)
	{
		value.Push(L);
	}

	inline bool	Match(TypeWrapper<LuaFunctionVoid>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TFUNCTION;
	}

	inline LuaFunctionVoid Get(TypeWrapper<LuaFunctionVoid>, lua_State* L, int idx)
	{
		return LuaFunctionVoid(L, idx);
	}

#ifdef LUA_RESETCOROUTINE
	// LuaCoroutine
	inline bool Check(lua_State* L, LuaCoroutine& value)
	{
		return value.IsValid();
	}

	inline void Push(lua_State* L, LuaCoroutine& value)
	{
		value.Push(L);
	}

	inline bool	Match(TypeWrapper<LuaCoroutine>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TFUNCTION;
	}

	inline LuaCoroutine Get(TypeWrapper<LuaCoroutine>, lua_State* L, int idx)
	{
		return LuaCoroutine(L, idx);
	}
#endif
}

#endif // LUAPLUS_EXTENSIONS

#endif // LUAFUNCTION_H
