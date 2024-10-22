#include "platform/systemtools.h"
#include <jni.h>
#include <stdlib.h>
#include <string>
#include "platform/android/jni/JniHelper.h"
#include "platform/android/basesdkjnihelper.h"
#include "zqcommon/strutil.h"
#include "filesystem/setting.h"
#include "luaengine/luaengine.h"
#include "fribidi-bidi-types.h"
#include "fribidi-deprecated.h"

#include "zqcommon/logutil.h"

#define JAVA_INVOKE_CPP_NETWORK_STATE_CHANGE_NOTIFY		Java_com_kuyue_sdklib_PlatformUtil_NetworkStateChangeNotify

ZQ::UINT32 SystemTools::network_change_notify_fun_id = 0;

SystemTools::NETWORK_STATE_CHANGED_CALLBACK SystemTools::network_change_notify_fun = nullptr;

void* SystemTools::network_change_notify_fun_data = nullptr;

extern "C"
{
	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_NETWORK_STATE_CHANGE_NOTIFY(JNIEnv* env, jobject thiz)
	{			
		SystemTools::NetworkStateChangeNotify();
	}
}

std::string SystemTools::GetProductName()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetProductName");
}

const std::string SystemTools::GetAppPath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetAppPath");
}

const std::string SystemTools::GetAppPackPath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetAppPackPath");
}

std::string SystemTools::GetAppName()
{
	return GetSdkStringPropertie("APP_NAME");
}


bool SystemTools::IsOpenLocationService()
{
	return GetCSdkBooleanPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"IsOpenLocationService");
}

SystemTools::NETWORKSTATE SystemTools::GetNetWorkState()
{
	cocos2d::JniMethodInfo methodInfo;
	ZQ::UINT32 networkType = 1;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PLATFORMUTIL, "GetNetWorkState", "()I"))
	{
		networkType = (ZQ::UINT32)methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
	return static_cast<NETWORKSTATE>(networkType);
}

bool SystemTools::IsNetworkAvailable()
{
	return GetCSdkBooleanPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "IsNetworkAvailable");
}

void SystemTools::CheckNetWork(const char* content, const char* positive_text, const char* negative_text)
{
	cocos2d::JniMethodInfo methodInfo;
	if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PLATFORMUTIL, "CheckNetWorkAsync", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring j_content = methodInfo.env->NewStringUTF(content);
		jstring j_positive_text = methodInfo.env->NewStringUTF(positive_text);
		jstring j_negative_text = methodInfo.env->NewStringUTF(negative_text);

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, j_content, j_positive_text, j_negative_text);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(j_content);
		methodInfo.env->DeleteLocalRef(j_positive_text);
		methodInfo.env->DeleteLocalRef(j_negative_text);
	}
}

std::string SystemTools::GetDeviceId()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetDeviceId");
}

std::string SystemTools::GetImsi()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetImsi");
}

std::string SystemTools::GetPackageName()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetPackageName");
}

std::string SystemTools::GetAppVersion()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetAppVersion");
}

std::string SystemTools::GetAppVersionCode()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetAppVersionCode");
}

std::string SystemTools::GetDeviceModel()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetDeviceModel");
}

ZQ::UINT32 SystemTools::GetMemorySize()
{
	cocos2d::JniMethodInfo methodInfo;
	ZQ::UINT32 memory_size = 0;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PLATFORMUTIL, "GetMemorySize", "()I"))
	{
		memory_size = (ZQ::UINT32)methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}

	return memory_size;
}

std::string SystemTools::GetMacAddress()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetMacAddress");
}

std::string SystemTools::GetIPAddress()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetIPAddress");
}

bool SystemTools::IsApptorePackage()
{
	return false;
}

bool SystemTools::IsSupportAppAutoInstall()
{
	return true;
}

std::string SystemTools::GetIdentifierForAdvertisers(){return "";}

std::string SystemTools::GetIdentifierForVendor(){return "";}

std::string SystemTools::GetApkExpansionPath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetApkExpansionPath");
}

// 判断某个包名的安装包是否安装
bool SystemTools::IsAppInstalled(std::string packageName)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_PLATFORMUTIL,
			"isAppInstalled", "(Ljava/lang/String;)Z");

	if (is_get)
	{
		jstring j_pkgname = method_info.env->NewStringUTF(packageName.c_str());

		ret_val = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID, j_pkgname);
		method_info.env->DeleteLocalRef(method_info.classID);
		method_info.env->DeleteLocalRef(j_pkgname);
	}
	return ret_val;
}

// 获取sdcard扩展路径
std::string SystemTools::GetLogPath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetLogPath");
}

void SystemTools::NetworkStateChangeNotify()
{
	if (0 != network_change_notify_fun_id)
	{
		LuaEngine::GetInstance()->CallFunction(network_change_notify_fun_id, "");
	}

	if (nullptr != network_change_notify_fun)
	{
		network_change_notify_fun(network_change_notify_fun_data);
	}
}

// 获取国家代码
std::string SystemTools::GetCountryCode()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetCountryCode");
}

// 获取货币代码
std::string SystemTools::GetCurrencyCode()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetCurrencyCode");
}

// 将维语文本从基本区转换为扩展区
std::string SystemTools::ConvertUygurToEx(const std::string& orign)
{
	const std::string& district_language = Setting::GetInstance()->Get("DistrictLanguage");
	if (district_language == "dl_wy" && orign.length() > 0)
	{
		return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "ConvertUygur", orign.c_str());
	}
	return orign;
}

// 将维语文本从基本区转换为扩展区并通过Fribidi库进行语序解析
std::string SystemTools::ConvertUygur(const std::string& param_str)
{
	if (param_str.empty()) {
		return "";
	}

	std::string orign = ConvertUygurToEx(param_str);

	std::u32string str32;

	cocos2d::StringUtils::UTF8ToUTF32(orign, str32);
	std::u32string strVisual, t;
	FriBidiParType base_dir = (FriBidiParType)FRIBIDI_TYPE_ON;

	int length = str32.length();
	strVisual.resize(length);
	fribidi_log2vis((FriBidiChar*)&str32[0], length, &base_dir, (FriBidiChar*)&strVisual[0], 0, 0, 0);

	for (auto it = strVisual.begin(); it != strVisual.end(); ++it){
		int ch = *it;
		if (ch == 1652) {
			// 音符的特殊处理
			t.push_back(0x0621);
		}
		else if (*it != 0xFEFF) {
			t.push_back(*it);
		}
	}
	strVisual.swap(t);

	std::string ret;
	cocos2d::StringUtils::UTF32ToUTF8(strVisual, ret);
	return ret;
}
