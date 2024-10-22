#include "platform/pushtools.h"
#include "luaengine/luaengine.h"

void PushTools::InvokeCallBackPushMethod(const char* method_type, unsigned int lua_fun_id, const char* extra_info)
{
	if (lua_fun_id != 0 )
	{
		LuaEngine::GetInstance()->CallFunction(lua_fun_id, "s", "win");
	}
}

std::string PushTools::InvokePushTaskMethod(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
{
	return "win";
}

std::string PushTools::InvokeVoidPushMethod(const char* method_type, const char* extra_info)
{
	return "win";
}

void PushTools::StartMsgPush(unsigned int lua_fun_id)
{
	InvokeCallBackPushMethod("START", lua_fun_id, "");
}

void PushTools::SetTag(const char* tag, unsigned int lua_fun_id)
{
	InvokeCallBackPushMethod("SETTAG", lua_fun_id, tag);
}

void PushTools::SetAlias(const char* alias, unsigned int lua_fun_id)
{
	InvokeCallBackPushMethod("ALIAS", lua_fun_id, alias);
}

std::string PushTools::ResumeMsgPush()
{
	return InvokeVoidPushMethod("RESUME", "");
}

std::string PushTools::StopMsgPush()
{
	return InvokeVoidPushMethod("STOP", "");
}

std::string PushTools::GetApiKey()
{
	return InvokeVoidPushMethod("GETAPIKEY", "");
}

std::string PushTools::GetAppInfo()
{
	return InvokeVoidPushMethod("GETAPPINFO", "");
}

std::string PushTools::GetActivityStatus()
{
	return InvokeVoidPushMethod("GETACSTATUS", "");
}

std::string PushTools::SetTimerTask(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
{
	return InvokePushTaskMethod(taskID, taskTime, taskName, taskLast, preTime, timer);
}

std::string PushTools::CancelTimerTask(const char* taskID)
{
	return InvokeVoidPushMethod("CANCELTASK", taskID);
}

void  PushTools::DelegateSocket(unsigned int socket_id)
{
}

