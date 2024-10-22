#pragma once

#include "../ToolKit/Singleton.h"
#include "LuaEngine.h"

#define LUA_EXPORT_BEGIN					bool LuaFuncExport() {\
												LuaEngine* engine = LuaInterface::Instance()->m_luaEngine;\
												bool res = true;
#define LUA_FUNC_EXPORT(luaFuncName, CFunc)		res &= engine->Register(luaFuncName, CFunc);
#define LUA_VAR_EXPORT(luaVarName, luaVar)		engine->SetGlobal(luaVarName, luaVar);
#define LUA_EXPORT_END							return res;\
											}

class LuaInterface : public ::Singleton<LuaInterface>
{
	friend bool LuaFuncExport();
public:
	LuaInterface();
	virtual ~LuaInterface();
	// 重启虚拟机，所有内容被清空
	// 此接口不安全，暂不给予使用
	// void Reboot();
	bool RegisterAll();
	bool LoadDir(const char* dir, bool bFirstLoad);
	bool ReloadFile(const char* file);
	template <typename ErrorProc>
	void CreateErrorProc() { LuaEngine::CreateErrorProc<ErrorProc>(); }
	template <typename T>
	bool GetGlobal(const char* name, T& var) { return m_luaEngine->GetGlobal(name, var); }
	template <typename T>
	void SetGlobal(const char* name, T var) { m_luaEngine->SetGlobal(name, var); }
	bool GetGlobal(const char* funcName, LuaFunction& func) { return m_luaEngine->GetGlobal(funcName, func); }
	bool GetGlobal(const char* funcName, LuaFunctionVoid& func) { return m_luaEngine->GetGlobal(funcName, func); }
private:
	LuaEngine* m_luaEngine;
};
