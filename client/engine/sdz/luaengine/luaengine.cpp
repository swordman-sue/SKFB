//
//  luaengine.cpp
//
//  Created by swordman sue
//  Copyright (c) ZLGame All rights reserved.
//
#include "zip/zip.h"
#include "common/platform/system.h"

#include "luaengine.h"
#include "luaregister.h"
#include "luamessageadapter.h"
#include "luatinker/lua_tinker.h"
#include "luaerrorreport.h"
#include "luaprofile.h"

#include "filesystem/setting.h"
#include "filesystem/filehelper.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"

#include "view/startup/startlogic.h"
#include "resmanager/resmanager.h"

#include "cocos2d.h"
#include "renderer/CCTextureCache.h"

#ifdef _WIN32
#include <conio.h>
static char * Gets(char * str, int num)
{
	fgets(str, num, stdin);
	if (str[0] == '\n' || str[0] == '\r')
		str[0] = 0;

	size_t len = strlen(str);
	if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
		str[len - 1] = 0;
	if (len > 1 && (str[len - 2] == '\n' || str[len - 2] == '\r'))
		str[len - 2] = 0;

	return str;
}
#endif

static int PackageLoader(lua_State *lua_state);
static int GetLuaFileBuff(lua_State *lua_state);

static LuaEngine* s_instance = nullptr;

LuaEngine* LuaEngine::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new LuaEngine;
	}
	return s_instance;
}

void LuaEngine::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

LuaEngine::~LuaEngine()
{
	if (m_script_zip!=0)
	{
		delete m_script_zip;
	}
}

LuaEngine::LuaEngine()
	:m_script_zip(0)
	,m_lua_main_start(false)
	,m_lua_state(nullptr)
{}

bool LuaEngine::Init()
{
	LuaErrorReport::GetInstance()->Init();

	if (nullptr == m_lua_state)
	{
		m_lua_state = lua_open();
		luaL_openlibs(m_lua_state);
	}
	
	LuaRegister::Regist(m_lua_state);
	LuaMessageAdapter::Regist(m_lua_state);
	LuaMessageAdapter::Init(m_lua_state);

	if (!LoadConfigFromLua())
	{
		LogUtil::LogError("[LuaEngine::Start LoadConfigFromLuaError]");
		return false;
	}

	if (!LoadLanguageStringFromLua())
	{
		LogUtil::LogError("[LuaEngine::Start LoadLanguageStringFromLuaError]");
		return false;
	}

	if (!RegistCppToLua())
	{
		LogUtil::LogError("[LuaEngine::Init RegistCppToLuaError]");
		return false;
	}

	if (!RegistPackageLoad())
	{
		LogUtil::LogError("[LuaEngine::CallLuaMainFunc RegistPackageLoaderError]");
		return false;
	}

	return true;
}

bool LuaEngine::Start()
{
	return true;
}

bool LuaEngine::Update(double now_time, double now_time_milli, float elapse_time)
{
	LuaErrorReport::GetInstance()->Update(elapse_time);

	if (!m_lua_main_start)
	{
		return true;
	}

	if (0 != CallFunction("Update", "ddd", now_time, now_time_milli, elapse_time))
	{
		//LogUtil::LogError("[LuaEngine::Update CallLuaUpdateError][now_time:%f elapse_time:%f]", now_time, elapse_time);
		CallFunction("UpdateErr", "");
		return false;
	}

	CheckConsoleCommand();

	return true;
}
	
bool LuaEngine::Stop()
{
	bool ret = true;

	if (m_lua_main_start)
	{
		if (0 != CallFunction("Stop", ""))
		{
			LogUtil::LogError("[LuaEngine::Stop CallLuaStopError]");
			ret = false;
		}
		m_lua_main_start = false;
	}

	LuaProfileAgent::GetInstance()->Stop();

	LuaErrorReport::GetInstance()->Stop();

	return ret;
}

bool LuaEngine::Pause()
{
	if (m_lua_main_start)
	{
		CallFunction("OnEnterBackground", "");
	}
	return true;
}

bool LuaEngine::Resume()
{
	if (m_lua_main_start)
	{
		CallFunction("OnEnterForeground", "");
	}
	return true;
}

bool LuaEngine::Release()
{
	if (nullptr != m_lua_state)
	{
		lua_close(m_lua_state);
		m_lua_state = nullptr;
	}
	return true;
}

bool LuaEngine::Restart()
{
	if (0 != CallFunction("Restart", ""))
	{
		LogUtil::LogError("[LuaEngine::Restart CallLuaRestartError]");
		return false;
	}

	if (!Stop())
	{
		LogUtil::LogError("[LuaEngine::Restart CallLuaStopError]");
		return false;
	}

	ClearPackageLoaded();

	m_lua_main_start = false;

	return true;
}

bool LuaEngine::LoadLanguageStringFromLua()
{
	char *lua_buf = 0;
	unsigned int buf_len = 0;

	if (!FileSystemEx::GetInstance()->ReadInitialFile("language_string.cnf", &lua_buf, &buf_len))
	{
		return false;
	}

	bool ret = false;
	do
	{
		if (luaL_loadbuffer(m_lua_state, lua_buf, buf_len, 0) || lua_pcall(m_lua_state, 0, 0, 0))
		{
			std::string error = "[LuaEngine::LoadLanguageStringFromLua] ";
			error += lua_tostring(m_lua_state, -1);
			LuaErrorReport::GetInstance()->Check(error);
			lua_pop(m_lua_state, 1);
			break;
		}

		lua_getglobal(m_lua_state, "LanguageStringMap");
		if (!lua_istable(m_lua_state, -1))
		{
			LogUtil::LogError("[LuaEngine::LoadLanguageStringFromLua GetString TableError]");
			break;
		}

		bool iter_error = false;
		lua_pushnil(m_lua_state);
		while (0 != lua_next(m_lua_state, -2))
		{
			if (lua_type(m_lua_state, -1) != LUA_TSTRING || lua_type(m_lua_state, -2) != LUA_TSTRING)
			{
				LogUtil::LogError("[LuaEngine::LoadLanguageStringFromLua trave iter error, no string]");
				iter_error = true;
				break;
			}

			const char* k_value = lua_tostring(m_lua_state, -2);
			const char* v_value = lua_tostring(m_lua_state, -1);

			m_language_config.insert(std::make_pair(k_value, v_value));
			lua_pop(m_lua_state, 1);
		}

		if (iter_error)
		{
			break;
		}

		ret = true;
	} while (false);

	free(lua_buf);
	return ret;
}

void LuaEngine::LoadStrCnfFromLuaExternal()
{
	if (!LoadStrCnfFromLua())
	{
		LogUtil::LogError("[LuaEngine::Start LoadStrCnfFromLuaError]");
	}
}

bool LuaEngine::LoadStrCnfFromLua()
{
	m_string_config.clear();
	char *lua_buf = 0;
	unsigned int buf_len = 0;

	if(!FileSystemEx::GetInstance()->ReadInitialFile("start_string.cnf", &lua_buf, &buf_len))
	{
		return false;
	}

	bool ret = false;
	do 
	{
		if(luaL_loadbuffer(m_lua_state, lua_buf, buf_len, 0) || lua_pcall(m_lua_state, 0, 0, 0))
		{
			std::string error = "[LuaEngine::LoadStrCnfFromLua] ";
			error += lua_tostring(m_lua_state, -1);
			LuaErrorReport::GetInstance()->Check(error);
			break;
		}

		lua_getglobal(m_lua_state, "StartStringMap");
		if (!lua_istable(m_lua_state, -1))
		{
			LogUtil::LogError("[LuaEngine::LoadStrCnfFromLua GetString TableError]");
			break;
		}

		std::string language = Setting::GetInstance()->Get("Language");
		lua_pushstring(m_lua_state, language.c_str());
		lua_gettable(m_lua_state, -2);
		
		if(!lua_istable(m_lua_state, -1))
		{
			LogUtil::LogError("[LuaEngine::LoadStrCnfFromLua GetString TableError [language %s]]", language.c_str());
			break;
		}

		bool iter_error = false;
		lua_pushnil(m_lua_state);
		while(0 != lua_next(m_lua_state, -2))
		{
			if (lua_type(m_lua_state, -1) != LUA_TSTRING || lua_type(m_lua_state, -2) != LUA_TSTRING)
			{
				LogUtil::LogError("[LuaEngine::LoadStrCnfFromLua trave iter error, no string]");
				iter_error = true;
				break;
			}

			const char* k_value = lua_tostring(m_lua_state, -2);
			const char* v_value = lua_tostring(m_lua_state, -1);

			m_string_config.insert(std::make_pair(k_value, v_value));
			lua_pop(m_lua_state, 1);
		}
		
		if(iter_error)
		{
			break;
		}

		ret = true;

	} while (false);

	free(lua_buf);
	return ret;
}

bool LuaEngine::LoadConfigFromLua()
{
	char *lua_str = 0;
	unsigned int str_len = 0;

	if (!FileSystemEx::GetInstance()->ReadInitialFile("start_config.cnf", &lua_str, &str_len))
	{
		return false;
	}

	bool ret = false;
	do 
	{
		if(luaL_loadbuffer(m_lua_state, lua_str, str_len, 0) || lua_pcall(m_lua_state, 0, 0, 0))
		{
			std::string error = "[LuaEngine::LoadConfigFromLua] ";
			error += lua_tostring(m_lua_state, -1);
			LuaErrorReport::GetInstance()->Check(error);
			lua_pop(m_lua_state, 1);
			break;
		}

		lua_getglobal(m_lua_state, "StartConfig");
		if (!lua_istable(m_lua_state, -1))
		{
			lua_pop(m_lua_state, 1);
			LogUtil::LogError("[LuaEngine::LoadConfigFromLua GetConfigTableError]");
			break;
		}

		lua_pushnil(m_lua_state);
		while(0 != lua_next(m_lua_state, -2))
		{
			lua_pushvalue(m_lua_state, -2);
			std::string key = lua_tostring(m_lua_state, -1);
			std::string value = "";
			switch(lua_type(m_lua_state, -2))
			{
			case LUA_TSTRING:
				value = lua_tostring(m_lua_state, -2);
				break;

			case LUA_TBOOLEAN:
				{
					bool tmp = lua_toboolean(m_lua_state, -2);
					ZQ::StrUtil::BaseTypeToString(tmp, value);
				}
				break;

			case LUA_TNUMBER:
				{
					int tmp = lua_tonumber(m_lua_state, -2);
					ZQ::StrUtil::BaseTypeToString(tmp, value);
				}
				break;

			default:
				LogUtil::LogError("[LuaEngine::LoadConfigFromLua ValueTypeError]");
				free(lua_str);
				return false;
			}
			m_config.insert(std::make_pair(key,value));
			lua_pop(m_lua_state, 2);	//将value出栈
		}
		lua_pop(m_lua_state, 1);		//将table出栈
		ret = true;
	} while (0);

	free(lua_str);
	return ret;
}

std::string LuaEngine::GetConfig(const std::string &key) const
{
	GameType::StrKVMap_CIter iter = m_config.find(key);
	if (iter == m_config.end())
	{
		return "";
	}
	return iter->second;
}

std::string LuaEngine::GetStringConfig(const std::string &key) const
{
	GameType::StrKVMap_CIter iter = m_string_config.find(key);
	if(iter == m_string_config.end())
	{
		LogUtil::LogError("can' t find string [%s]", key.c_str());
		return "";
	}

	return iter->second;
}

std::string LuaEngine::GetLanguageConfig(const std::string &key) const
{
	GameType::StrKVMap_CIter iter = m_language_config.find(key);
	if (iter == m_language_config.end())
	{
		LogUtil::LogError("can' t find language string [%s]", key.c_str());
		return "";
	}

	return iter->second;
}

void LuaEngine::SetConfig(const std::string &key, const std::string &value)
{
	GameType::StrKVMap_CIter iter = m_config.find(key);
	if (iter == m_config.end() || iter->second != value)
	{
		m_config[key] = value;
	}
}

class LoadScriptIosListener : public FileListener
{
public:
	LoadScriptIosListener(StartLogic *logic)
		:m_logic(logic)
	{

	}
	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info)
	{
		if (file_info.op_result != FileAsync::FOR_SUCC)
		{
			assert(m_logic!=0);
			m_logic->OnUnzipFinish_ScriptIos(false, file_info.path);
		}
		else
		{
			LuaEngine::GetInstance()->OnLoadScriptIosFinish(file_info.script_ios_zip);

			assert(m_logic!=0);
			m_logic->OnUnzipFinish_ScriptIos(true);
		}
	};
	virtual void Free()
	{
		delete this;
	}

protected:
	StartLogic *m_logic;
};

void LuaEngine::StartLoadScriptIos(StartLogic *logic)
{
	GameType::StrVect script_ios_file_list = FileVersion::GetInstance()->GetScriptIosFileList();
	LoadScriptIosListener *listener = new LoadScriptIosListener(logic);
	FileSystemEx::GetInstance()->ReadScriptIosAsync(script_ios_file_list, listener);
}

void LuaEngine::OnLoadScriptIosFinish(Zip *zip)
{
	m_script_zip = zip;
}

bool LuaEngine::GetScriptIosBuffer(const std::string & script_name, void **buff, int *len)
{
	if (m_script_zip == 0)
	{
		return false;
	}
	return m_script_zip->GetUzFileData(script_name+".lua", buff, len);
}

bool LuaEngine::CallLuaMainFunc()
{
	LogUtil::LogInfo("LuaEngine::CallLuaMainFunc");

	if (0 != CallFunction("require", "s", "main"))
	{
		LogUtil::LogError("[LuaEngine::CallLuaMainFunc RequireError][require lua file main.lua error]");
		return false;
	}

	if (0 != CallFunction("Start", ""))
	{
		LogUtil::LogError("[LuaEngine::CallLuaMainFunc InvokeMainFunctionError]");
		return false;
	}
	
	m_lua_main_start = true;
	return true;
}

bool LuaEngine::RegistCppToLua()
{
	std::string initial_path = FileSystemEx::GetInstance()->GetInitialPath();

	std::string all_script_path = "";

	if (GetConfig("IsLocalRes") == "true" && CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		all_script_path += initial_path + "../../../script/?.lua;";
		all_script_path += initial_path + "../../../cdnfiles/?.cnf;";
	}
	else
	{
		all_script_path += FileSystemEx::GetInstance()->GetDownLoadPath() + "?.cnf;";
	}
    
    all_script_path += initial_path + "?.cnf;";

	if (SetVariable("ss>s", "package", "path", all_script_path.c_str()) != 0)
	{
		return false;
	}

	return true;
}

static int PackageLoader(lua_State *lua_state)
{
	lua_getfield(lua_state, LUA_ENVIRONINDEX, "package");
	if (!lua_istable(lua_state, -1))
	{
		lua_pushnumber(lua_state, 0);
		return 1;
	}

	lua_getfield(lua_state, -1, "loaders");
	if (!lua_istable(lua_state, -1))
	{
		lua_pushnumber(lua_state, 0);	
		return 1;
	}

	int n = lua_objlen(lua_state, -1);
	lua_pushcfunction(lua_state, GetLuaFileBuff);
	lua_rawseti(lua_state, -2, n+1);

	//lua_pop(lua_state, 2);

	lua_pushnumber(lua_state, 1);
	return 1;
}

bool LuaEngine::RegistPackageLoad()
{
	int ret = 0;
	lua_register(m_lua_state, "CPPPackageLoader", PackageLoader);
	CallFunction("CPPPackageLoader", ">i", &ret);
	return ret==1;
}

static int GetLuaFileBuff(lua_State *lua_state)
{
    const char *filename = luaL_checkstring(lua_state, 1);
    if(filename == 0)
    {
        LogUtil::LogError("GetLuaFileBuff::file name error.");
        return 0;
    }
    
    Zip zip;
    cocos2d::Data fileData;
    void *file_buff = nullptr;
    ZQ::INT32 buff_len = 0;
    
    do
    {
		// 读取脚本压缩包(.ios)
		if (LuaEngine::GetInstance()->GetScriptIosBuffer(filename, &file_buff, &buff_len))
		{
			break;
		}

        bool notify = cocos2d::FileUtils::getInstance()->isPopupNotify();
        cocos2d::FileUtils::getInstance()->setPopupNotify(false);

		// 读取配置文件(.zcnf)
		if (LuaEngine::GetInstance()->GetScriptZCnfBuffer(&zip, filename, &file_buff, &buff_len))
		{
			cocos2d::FileUtils::getInstance()->setPopupNotify(notify);
			break;
		}

        // 读取配置文件(.cnf)
        if (LuaEngine::GetInstance()->GetScriptCnfBuffer(filename, &fileData))
        {
            file_buff = fileData.getBytes();
            buff_len = (int)fileData.getSize();
            cocos2d::FileUtils::getInstance()->setPopupNotify(notify);
            break;
        }
                
        cocos2d::FileUtils::getInstance()->setPopupNotify(notify);
                
    } while(false);
    
    if(luaL_loadbuffer(lua_state, (const char*)file_buff, buff_len, filename) != 0)
    {
        luaL_error(lua_state, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s", lua_tostring(lua_state, 1), filename, lua_tostring(lua_state, -1));
        return 0;
    }
    
    return 1;
}

bool LuaEngine::GetScriptZCnfBuffer(Zip* zip, std::string script_name, void **buff, int *len)
{
	if(zip == NULL)
		return false;

	std::string suffix_name = ".zcnf";
	script_name = script_name + suffix_name;
    cocos2d::Data fileData = cocos2d::FileUtils::getInstance()->getDataFromFile(script_name);
    if (fileData.isNull())
    {
        return false;
    }

	std::string::size_type last_index = script_name.find_last_of("/");
	std::string script_file_name = script_name.substr(last_index + 1, script_name.length() - last_index - suffix_name.length() - 1);
	script_file_name = script_file_name + ".cnf";

    if (!zip->UnzipBuffer((const char*)fileData.getBytes(), (int)fileData.getSize(), NULL))
    {
        return false;
    }
    
	if (!zip->GetUzFileData(script_file_name, buff, len))
	{
		return false;
	}

	return true;
}

bool LuaEngine::GetScriptCnfBuffer(std::string script_name, cocos2d::Data* fileData)
{
    if (nullptr == fileData)
    {
        return false;
    }
    
    std::string suffix_name = ".cnf";
    script_name = script_name + suffix_name;
    *fileData = cocos2d::FileUtils::getInstance()->getDataFromFile(script_name);
    if (fileData->isNull())
    {
        return false;
    }
    
    return true;
}

lua_State* LuaEngine::GetLuaState()
{
	return m_lua_state;
}

void LuaEngine::CheckConsoleCommand()
{
	if (!m_lua_main_start)
	{
		return;
	}

#ifdef _WIN32
	static char command[128];
	if (kbhit())
	{
		Gets(command, sizeof(command));

		if (0 != CallFunction("OnConsoleCommand", "s", command))
		{
			LogUtil::LogError("LuaEngine::CheckConsoleCommand error");
		}
	}
#endif
}

void LuaEngine::ClearPackageLoaded()
{
	//清空package.loaded
	lua_getglobal(m_lua_state, "package");
	lua_pushstring(m_lua_state, "loaded");
	lua_gettable(m_lua_state, -2);

	int nIndex = lua_gettop(m_lua_state);				// 取table索引值 
	lua_pushnil(m_lua_state);							// nil入栈作为初始key 
	while (0 != lua_next(m_lua_state, nIndex))
	{
		// 现在栈顶（-1）是value，-2位置是对应的key 
		// 这里可以判断key是什么并且对value进行各种处理 
		lua_pushvalue(m_lua_state, -2);
		lua_pushnil(m_lua_state);
		lua_settable(m_lua_state, nIndex);

		// 弹出value，让key留在栈顶 
		lua_pop(m_lua_state, 1);
	}

	lua_pop(m_lua_state, 2);
}
