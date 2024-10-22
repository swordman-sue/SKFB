
#ifndef LUAENGINE_H
#define LUAENGINE_H

extern "C"
{
#include "lua/lua.h"
}

#include "zqcommon/typedef.h"

#include <string>

struct MessageMem;
class Zip;
class StartLogic;
namespace cocos2d
{
    class Data;
}

class LuaEngine
{
public:
	static LuaEngine* GetInstance();
	static void DelInstance();
    ~LuaEngine();

private:
	LuaEngine();
    
public:
	bool Init();
	bool Start();
	bool Update(double now_time, double now_time_milli, float elapse_time);
	bool Pause();
	bool Resume();
	bool Stop();
	bool Release();
	bool Restart();

	void LoadStrCnfFromLuaExternal();
	void StartLoadScriptIos(StartLogic *logic);
	void OnLoadScriptIosFinish(Zip *zip);
	bool GetScriptIosBuffer(const std::string & script_name, void **buff, int *len);
	bool GetScriptZCnfBuffer(Zip* zip, std::string script_name, void **buff, int *len);
    bool GetScriptCnfBuffer(std::string script_name, cocos2d::Data* fileData);
	bool RegistPackageLoad();
	std::string GetConfig(const std::string &key) const;
	void SetConfig(const std::string &key, const std::string &value);
	std::string GetStringConfig(const std::string &key) const;
	std::string GetLanguageConfig(const std::string &key) const;

	bool CallLuaMainFunc();
	ZQ::INT32 SetVariable(const char *format, ...);
	ZQ::INT32 CallFunction(const char *name, const char *arg_fmt, ...);
	ZQ::INT32 CallFunction(ZQ::UINT32 call_back_func_id, const char *arg_fmt, ...);
	ZQ::INT32 DelLuaFuncID(ZQ::UINT32 call_back_func_id);
	void VoiceRecordCallBack(ZQ::UINT32 call_back_func_id, unsigned int call_back_id, int result, unsigned int voicd_id, int voice_len, MessageMem& msg_me);
	void UpdateFileListCallBack(unsigned int async_or_http_id, unsigned int func_id, const std::string &file_name, bool is_abort, bool result, int ret_code, int http_ret_code, int op_code, const char* custom_ret_msg, unsigned int data_len);
	void CallUrlCallBack(unsigned int http_id, unsigned int func_id, bool is_abort, bool result, int ret_code, const std::string &url, const char *url_data, unsigned int len, int http_ret_code, int op_code);

	lua_State* GetLuaState();

	bool IsLuaMainStart() { return m_lua_main_start;  }

private:
	// 注册类或者功能接口给Lua
	bool RegistCppToLua();
	bool LoadConfigFromLua();
	bool LoadStrCnfFromLua();
	bool LoadLanguageStringFromLua();

	// 支持控制台命令 add by swordman sue 2016-6-13
	void CheckConsoleCommand();

	// 清空脚本加载记录 add by swordman sue 2017-3-7
	void ClearPackageLoaded();

private:
	Zip *m_script_zip;
	bool m_lua_main_start;
	GameType::StrKVMap m_config;
	GameType::StrKVMap m_string_config;
	GameType::StrKVMap m_language_config;
    lua_State* m_lua_state;
};


#endif
