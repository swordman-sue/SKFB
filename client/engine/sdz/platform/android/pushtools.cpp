#include "platform/pushtools.h"
#include <string>
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <string>
#include "cocos2d.h"
#include "luaengine/luamessageadapter.h"

#define CPP_INVOKE_JAVA_PUSHMANAGER   "com/kuyue/pushsdk/MsgPush"

void PushTools::InvokeCallBackPushMethod(const char* method_type, unsigned int lua_fun_id, const char* extra_info)
{
	cocos2d::CCLog("InvokeCallBackPushMethod method_type : %s", method_type);
	if(NULL == method_type || strlen(method_type) == 0)
	{
		cocos2d::CCLog("InvokeCallBackPushMethod method_type is empty");
		return;
	}
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
					CPP_INVOKE_JAVA_PUSHMANAGER,"InvokeCallBackPushService","(Ljava/lang/String;ILjava/lang/String;)V");
	if (isHave)
	{
		jstring j_method_type = minfo.env->NewStringUTF(method_type);
		jstring j_extra_info = minfo.env->NewStringUTF(extra_info);
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, j_method_type, lua_fun_id, j_extra_info);
		minfo.env->DeleteLocalRef(j_method_type);
		minfo.env->DeleteLocalRef(j_extra_info);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
}

std::string PushTools::InvokePushTaskMethod(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
{
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PUSHMANAGER, "InvokeTimerTask", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
	{
		jstring j_taskID = methodInfo.env->NewStringUTF(taskID);
		jstring j_taskTime = methodInfo.env->NewStringUTF(taskTime);
		jstring j_taskName = methodInfo.env->NewStringUTF(taskName);
		jstring j_taskLast = methodInfo.env->NewStringUTF(taskLast);
		jstring j_preTime = methodInfo.env->NewStringUTF(preTime);
		jstring j_timer = methodInfo.env->NewStringUTF(timer);
		jstring data = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, j_taskID, j_taskTime, j_taskName, j_taskLast, j_preTime, j_timer);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(j_taskID);
		methodInfo.env->DeleteLocalRef(j_taskTime);
		methodInfo.env->DeleteLocalRef(j_taskName);
		methodInfo.env->DeleteLocalRef(j_taskLast);
		methodInfo.env->DeleteLocalRef(j_preTime);
		methodInfo.env->DeleteLocalRef(j_timer);
		std::string datainfo = cocos2d::JniHelper::jstring2string(data);
		methodInfo.env->DeleteLocalRef(data);
		return datainfo;
	}
	return "";
}

std::string PushTools::InvokeVoidPushMethod(const char* method_type, const char* extra_info)
{
	cocos2d::CCLog("InvokeVoidPushMethod method_type : %s", method_type);

	if(NULL == method_type || strlen(method_type) == 0)
	{
		cocos2d::CCLog("InvokeVoidPushMethod method_type is empty");
		return "";
	}
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PUSHMANAGER, "InvokeVoidPushService", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
	{
		jstring j_method_type = methodInfo.env->NewStringUTF(method_type);
		jstring j_extra_info = methodInfo.env->NewStringUTF(extra_info);
		jstring data = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, j_method_type, j_extra_info);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(j_method_type);
		methodInfo.env->DeleteLocalRef(j_extra_info);
		std::string datainfo = cocos2d::JniHelper::jstring2string(data);
		methodInfo.env->DeleteLocalRef(data);
		return datainfo;
	}
	return "";
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

void PushTools::DelegateSocket(unsigned int socket_id)
{
}

