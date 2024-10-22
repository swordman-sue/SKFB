#include "platform/android/basesdkjnihelper.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "luaengine/luaengine.h"
#include <string>
#include "zqcommon/logutil.h"


 void* GetSdkPropertie(const char* pro_name)
{
	if(NULL == pro_name || strlen(pro_name) == 0)
	{
		LogUtil::LogError("GetSdkPropertie parame is empty");
		return NULL;
	}
	cocos2d::JniMethodInfo methodInfo;
	jobject  obj;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_SDKMANAGER, "GetPropertie", "(Ljava/lang/String;)Ljava/lang/Object;"))
	{
		jstring j_pro_name = methodInfo.env->NewStringUTF(pro_name);
		obj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID,j_pro_name);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(j_pro_name);
	}
	return obj;
}

 std::string GetCSdkStringPropertie(const char* class_name,const char* method_name,const char* meta_name)
{
	if(NULL == method_name || strlen(method_name) == 0 || NULL == class_name || strlen(class_name) == 0)
	{
		LogUtil::LogError("GetCSdkStringPropertie parame class_name or method_name is empty");
		return NULL;
	}
	cocos2d::JniMethodInfo methodInfo;
	jstring  obj;
	std::string propertie = "";
	if (NULL != meta_name && strlen(meta_name) != 0)
	{
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, class_name, method_name, "(Ljava/lang/String;)Ljava/lang/String;"))
		{
			jstring j_meta_name = methodInfo.env->NewStringUTF(meta_name);
			obj = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID,j_meta_name);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			methodInfo.env->DeleteLocalRef(j_meta_name);
		}
	}else
	{
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, class_name, method_name, "()Ljava/lang/String;"))
		{
			obj = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
	}
	if(obj)
	{
		propertie = cocos2d::JniHelper::jstring2string(obj);
		methodInfo.env->DeleteLocalRef(obj);
	}
	return propertie;
}

 bool GetCSdkBooleanPropertie(const char* class_name,const char* method_name)
{
	if(NULL == method_name || strlen(method_name) == 0 || NULL == class_name || strlen(class_name) == 0)
	{
		LogUtil::LogError("GetCSdkBooleanPropertie parame class_name or method_name is empty");
		return NULL;
	}
	cocos2d::JniMethodInfo methodInfo;
	bool propertie = false;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, class_name,method_name, "()Z"))
	{
		propertie = (bool)methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
	return propertie;
}

std::string GetSdkStringPropertie(const char* pro_name)
{
	if(NULL == pro_name || strlen(pro_name) == 0)
	{
		LogUtil::LogError("GetSdkPropertie parame is empty");
		return NULL;
	}
	cocos2d::JniMethodInfo methodInfo;
	jobject  obj;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_SDKMANAGER, "GetPropertie", "(Ljava/lang/String;)Ljava/lang/Object;"))
	{
		jstring j_pro_name = methodInfo.env->NewStringUTF(pro_name);
		obj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID,j_pro_name);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(j_pro_name);
	}
	std::string propertie = "";
	if (obj)
	{
		propertie = cocos2d::JniHelper::jstring2string((jstring)obj);
	}
	methodInfo.env->DeleteLocalRef(obj);
	return propertie;
}

bool  GetSdkBooleanPropertie(const char* pro_name)
{
	bool propertie = false;
	std::string boolstr =  GetSdkStringPropertie(pro_name);
	if (!boolstr.empty() && boolstr == "true")
	{
		propertie = true;
	}
	return propertie;
}

