#ifndef PLATFORM_PUSHTOOLS_H_
#define PLATFORM_PUSHTOOLS_H_


#include <string>

class PushTools
{
public:
	static void StartMsgPush(unsigned int lua_fun_id);
	static void SetTag(const char* tag, unsigned int lua_fun_id);
	static void SetAlias(const char* alias, unsigned int lua_fun_id);
	static std::string ResumeMsgPush();
	static std::string StopMsgPush();
	static std::string GetApiKey();
	static std::string GetAppInfo();
	static std::string GetActivityStatus();
	static std::string SetTimerTask(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer);
	static std::string CancelTimerTask(const char* taskID);
	static void DelegateSocket(unsigned int socket_id);
private:
	static void InvokeCallBackPushMethod(const char* method_type, unsigned int lua_fun_id, const char* extra_info);
	static std::string InvokePushTaskMethod(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer);
	static std::string InvokeVoidPushMethod(const char* method_type, const char* extra_info);
};


#endif
