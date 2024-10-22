#include "platform/sdktools.h"

#include "platform/systemtools.h"
#include "gameengine/gameengine.h"
#include "zqcommon/StrUtil.h"
#include "platform/android/msgasynchandler.h"
#include "platform/android/msgdata.h"
#include "platform/android/basesdkjnihelper.h"
#include "filesystem/setting.h"
#include "zqcommon/logutil.h"

#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <string>

#define JAVA_INVOKE_CPP_SDKINIT_FINISH_CALLBACK    Java_com_kuyue_sdklib_BaseSdk_SDKInitFinishCallBack
#define JAVA_INVOKE_CPP_SDKLOGIN_CALLBACK          Java_com_kuyue_sdklib_BaseSdk_SDKLoginPanelCallBack
#define JAVA_INVOKE_CPP_SDKPAY_FINISH_CALLBACK     Java_com_kuyue_sdklib_BaseSdk_SDKPayFinishCallBack
#define JAVA_INVOKE_CPP_SDKCOMMON_CALLBACK         Java_com_kuyue_sdklib_BaseSdk_SDKCommonHandleCallBack
#define JAVA_INVOKE_CPP_DLG_CERTAIN   			   Java_com_kuyue_sdklib_PlatformUtil_DlgCertainCallBack
#define JAVA_INVOKE_CPP_DLG_CANCEL  			   Java_com_kuyue_sdklib_PlatformUtil_DlgCancelCallBack
#define JAVA_INVOKE_CPP_CLOSE_APP_IN_ENGINE  	   Java_com_kuyue_sdklib_PlatformUtil_NativeCloseEngineInApp

#define JAVA_INVOKE_CPP_LOCATION_CHANGE  		   Java_com_kuyue_gps_GpsLocation_onLocationChangedCallback
#define JAVA_INVOKE_CPP_START_PUSH_CALLBACK  	   Java_com_kuyue_pushsdk_MpushManager_StartMsgPushCallBack
#define JAVA_INVOKE_CPP_ADD_TAGS_CALLBACK  	   	   Java_com_kuyue_pushsdk_MpushManager_AddTagsCallBack
#define JAVA_INVOKE_CPP_DEL_TAGS_CALLBACK  	   	   Java_com_kuyue_pushsdk_MpushManager_DeleteTagsCallBack
#define JAVA_INVOKE_CPP_SET_ALIAS_CALLBACK  	   Java_com_kuyue_pushsdk_MpushManager_SetAliasCallBack

ZQ::UINT32 SdkTools::dlg_certain_fun_id = 0;
ZQ::UINT32 SdkTools::dlg_cancel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_init_finish_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_login_panel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_td_finish_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_common_fun_id = 0;
ZQ::UINT32 SdkTools::memory_warning_fun_id = 0;

dialog_callback SdkTools::dialog_call_back_pointer = nullptr;
dialog_callback SdkTools::network_dialog_call_back_pointer = nullptr;

bool SdkTools::sdk_is_inited = false;

extern
"C"
{

static void InvokeVoidMethod(const char* class_name,const char* method_name,const char* param_info)
{
	LogUtil::LogInfo("InverkVoidMethod class_name :  %s method_name : %s param_info : %s",class_name,method_name,param_info);
	if(NULL == method_name || strlen(method_name) == 0 || NULL == class_name || strlen(class_name) == 0)
	{
		LogUtil::LogError("InvokeVoidMethod class_name or method_name is empty");
		return;
	}
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, class_name, "InvokeVoidMethod", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring j_method_name = methodInfo.env->NewStringUTF(method_name);
		jstring j_param_info = methodInfo.env->NewStringUTF(param_info);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID,j_method_name,j_param_info);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(j_method_name);
		methodInfo.env->DeleteLocalRef(j_param_info);
	}
}

}

std::string SdkTools::GetSdkName()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetApplicationMetaData","SDK_NAME");
}

std::string SdkTools::GetStartImagePath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetApplicationMetaData", "START_IMAGE_PATH");
}

std::string SdkTools::GetLogoImagePath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetApplicationMetaData","LOGO_IMAGE_PATH");
}

std::string SdkTools::GetLoginImagePath()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetApplicationMetaData","LOGIN_IMAGE_PATH");
}

bool SdkTools::IsThirdAccountPlat()
{
	std::string IsThirdAccountPlat =  GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetApplicationMetaData","IS_THIRD_ACCOUNT_PLAT");

	return "true" == IsThirdAccountPlat;
}

std::string SdkTools::GetSdkVersion()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL, "GetApplicationMetaData", "SDK_VERSION");
}

std::string SdkTools::GetChannelId()
{
	return GetSdkStringPropertie("CHANNEL_ID");
}

std::string SdkTools::GetChannelIdEx()
{
	return GetSdkStringPropertie("CHANNEL_ID_EX");
}

std::string SdkTools::GetAppId()
{
	return GetSdkStringPropertie("APP_ID");
}
std::string SdkTools::GetAppKey()
{
	return GetSdkStringPropertie("APP_KEY");
}

std::string SdkTools::GetPhpTDOrderUrl()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetApplicationMetaData","PHP_TD_ORDER_URL");
}

std::string SdkTools::GetPhpTDNotifyUrl()
{
	return GetCSdkStringPropertie(CPP_INVOKE_JAVA_PLATFORMUTIL,"GetApplicationMetaData","PHP_TD_NOTIFY_URL");
}

bool SdkTools::IsInited()
{
	return SdkTools::sdk_is_inited;
}

void SdkTools::CheckUpdate(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"SDK_CHECK_UPDATE",param);
}

void SdkTools::LoginPanel()
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"SDK_LOGIN","");
}

void SdkTools::LoginPannelWithParam(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"SDK_LOGIN", param);
}

void SdkTools::Logout(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER, "SDK_LOGOUT", param);
}

void SdkTools::SwitchAccount(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER, "SDK_SWITCH_ACCOUNT", param);
}

void SdkTools::TD(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"SDK_PAY",param);
}

void SdkTools::ToUserCenter()
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"TO_USER_CENTER","");
}

void SdkTools::ExitSdk(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"SDK_EXIT_APP",param);
}

void SdkTools::CommonHandle(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER,"COMMON_HANDLE",param);
}

void SdkTools::KeyDataHandle(const char* param)
{
	InvokeVoidMethod(CPP_INVOKE_JAVA_SDKMANAGER, "KEY_DATA_HANDLE", param);
}

void SdkTools::GotoBrowser(const char* url)
{
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,CPP_INVOKE_JAVA_PLATFORMUTIL, "GotoBrowser","(Ljava/lang/String;)V");
	if (isHave)
	{
		jstring strbrowserurl = minfo.env->NewStringUTF(url);
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, strbrowserurl);
		minfo.env->DeleteLocalRef(strbrowserurl);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
}

void SdkTools::ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, ZQ::UINT32 cb_certain_fun, ZQ::UINT32 cb_cancel_fun)
{
	if(dlg_certain_fun_id != 0 && cb_certain_fun != dlg_certain_fun_id)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(dlg_certain_fun_id);
		dlg_certain_fun_id = 0;
	}

	if(dlg_cancel_fun_id != 0 && cb_cancel_fun != dlg_cancel_fun_id)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(dlg_cancel_fun_id);
		dlg_cancel_fun_id = 0;
	}
	dialog_call_back_pointer = 0;

	dlg_certain_fun_id = cb_certain_fun;
	dlg_cancel_fun_id = cb_cancel_fun;

	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
		CPP_INVOKE_JAVA_PLATFORMUTIL, "ShowDlgMainLoop", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

	if (isHave)
	{
		jstring strtitle = minfo.env->NewStringUTF(title);
		jstring strtips = minfo.env->NewStringUTF(tips);
		jstring strcertain = minfo.env->NewStringUTF(certain);
		jstring strcancel = minfo.env->NewStringUTF(cancel);

		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, strtitle, strtips, strcertain, strcancel, false);
		minfo.env->DeleteLocalRef(strtitle);
		minfo.env->DeleteLocalRef(strtips);
		minfo.env->DeleteLocalRef(strcertain);
		minfo.env->DeleteLocalRef(strcancel);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
}

void SdkTools::ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, dialog_callback call_back)
{
	if(call_back != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(dlg_certain_fun_id);
		dlg_certain_fun_id = 0;
	}
	if(call_back != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(dlg_cancel_fun_id);
		dlg_cancel_fun_id = 0;
	}
	dialog_call_back_pointer = call_back;

	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
		CPP_INVOKE_JAVA_PLATFORMUTIL, "ShowDlgMainLoop", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

	if (isHave)
	{
		jstring strtitle = minfo.env->NewStringUTF(title);
		jstring strtips = minfo.env->NewStringUTF(tips);
		jstring strcertain = minfo.env->NewStringUTF(certain);
		jstring strcancel = minfo.env->NewStringUTF(cancel);

		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, strtitle, strtips, strcertain, strcancel, false);
		minfo.env->DeleteLocalRef(strtitle);
		minfo.env->DeleteLocalRef(strtips);
		minfo.env->DeleteLocalRef(strcertain);
		minfo.env->DeleteLocalRef(strcancel);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
}

void SdkTools::RunMemoryWarning()
{
	if (memory_warning_fun_id != 0 )
	{
		LuaEngine::GetInstance()->CallFunction(SdkTools::memory_warning_fun_id, "");
	}
}

bool SdkTools::OpenFile(std::string path, std::string actiontype, std::string mimetype)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_PLATFORMUTIL,
			"OpenFile", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");

	if (is_get)
	{
		jstring j_path = method_info.env->NewStringUTF(path.c_str());
		jstring j_actiontype = method_info.env->NewStringUTF(actiontype.c_str());
		jstring j_mimetype = method_info.env->NewStringUTF(mimetype.c_str());

		ret_val = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID, j_path, j_actiontype, j_mimetype);
		method_info.env->DeleteLocalRef(method_info.classID);
		method_info.env->DeleteLocalRef(j_path);
		method_info.env->DeleteLocalRef(j_actiontype);
		method_info.env->DeleteLocalRef(j_mimetype);
	}
	return ret_val;
}

bool SdkTools::Install(std::string installeroffullpath)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, CPP_INVOKE_JAVA_PLATFORMUTIL,
			"InstallApk", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");

	if (is_get)
	{
		jstring j_path = method_info.env->NewStringUTF(installeroffullpath.c_str());
		jstring j_actiontype = method_info.env->NewStringUTF("android.intent.action.VIEW");
		jstring j_mimetype = method_info.env->NewStringUTF("application/vnd.android.package-archive");

		ret_val = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID, j_path, j_actiontype, j_mimetype);
		method_info.env->DeleteLocalRef(method_info.classID);
		method_info.env->DeleteLocalRef(j_path);
		method_info.env->DeleteLocalRef(j_actiontype);
		method_info.env->DeleteLocalRef(j_mimetype);
	}
	return ret_val;
	//return SdkTools::OpenFile(installeroffullpath, "android.intent.action.VIEW", "application/vnd.android.package-archive");
}


std::string SdkTools::GetUpdateUrlParams()
{
	std::string ret_var;
	std::string rnd;
	ZQ::StrUtil::BaseTypeToString(arc4random() % 1000, rnd);
	ret_var = "type=android";
	ret_var = ret_var + std::string("&rnd=") + rnd.c_str();
	ret_var = ret_var + std::string("&pkgname=") + SystemTools::GetPackageName().c_str();
	ret_var = ret_var + std::string("&fromid=") + SdkTools::GetChannelId().c_str();

	return ret_var;
}

std::string SdkTools::GetAppStoreProductId(int price)
{
	return "";
}

std::string SdkTools::GetKeyDataChannelId()
{
	return GetSdkStringPropertie("SUB_CHANNEL");
}

extern
"C"
{
	static void InvokeVoidPushMethod(const char* method_type, const char* extra_info)
	{
		LogUtil::LogInfo("InvokeVoidPushMethod method_type : %s", method_type);
		if(NULL == method_type || strlen(method_type) == 0)
		{
			LogUtil::LogError("InvokeVoidPushMethod method_type is empty");
		}
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PUSHMANAGER, "InvokeVoidPushService", "(Ljava/lang/String;Ljava/lang/String;)V"))
		{
			jstring j_method_type = methodInfo.env->NewStringUTF(method_type);
			jstring j_extra_info = methodInfo.env->NewStringUTF(extra_info);
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, j_method_type, j_extra_info);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			methodInfo.env->DeleteLocalRef(j_method_type);
			methodInfo.env->DeleteLocalRef(j_extra_info);
		}
	}
}
extern
"C"
{
	static std::string InvokePushMethod(const char* method_type, const char* extra_info)
	{
		LogUtil::LogInfo("InvokePushMethod method_type : %s", method_type);
		if(NULL == method_type || strlen(method_type) == 0)
		{
			LogUtil::LogError("InvokePushMethod method_type is empty");
			return "";
		}
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PUSHMANAGER, "InvokePushService", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
		{
			jstring j_method_type = methodInfo.env->NewStringUTF(method_type);
			jstring j_extra_info = methodInfo.env->NewStringUTF(extra_info);
			jstring data = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, j_method_type, j_extra_info);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			methodInfo.env->DeleteLocalRef(j_method_type);
			methodInfo.env->DeleteLocalRef(j_extra_info);
			std::string datainfo = cocos2d::JniHelper::jstring2string(data);
			return datainfo;
		}
		return "";
	}
}
extern
"C"
{
	static std::string InvokePushTaskMethod(const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
	{
		if(NULL == taskName || strlen(taskName) == 0)
		{
			LogUtil::LogError("InvokePushMethod taskName is empty");
			return "";
		}
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CPP_INVOKE_JAVA_PUSHMANAGER, "InvokeTimerTask", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
		{
			jstring j_taskTime = methodInfo.env->NewStringUTF(taskTime);
			jstring j_taskName = methodInfo.env->NewStringUTF(taskName);
			jstring j_taskLast = methodInfo.env->NewStringUTF(taskLast);
			jstring j_preTime = methodInfo.env->NewStringUTF(preTime);
			jstring j_timer = methodInfo.env->NewStringUTF(timer);
			jstring data = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, j_taskTime, j_taskName, j_taskLast, j_preTime, j_timer);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			methodInfo.env->DeleteLocalRef(j_taskTime);
			methodInfo.env->DeleteLocalRef(j_taskName);
			methodInfo.env->DeleteLocalRef(j_taskLast);
			methodInfo.env->DeleteLocalRef(j_preTime);
			methodInfo.env->DeleteLocalRef(j_timer);
			std::string datainfo = cocos2d::JniHelper::jstring2string(data);
			return datainfo;
		}
		return "";
	}
}

extern
"C"
{
	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_SDKINIT_FINISH_CALLBACK(JNIEnv* env, jobject thiz,jint code)
	{
		if (code == 0)
		{
			SdkTools::sdk_is_inited = true;
			Setting::GetInstance()->Set("IsSDKInitFailed", "0");
		}
		else
		{
			LogUtil::LogError("sdk init fail.");
			Setting::GetInstance()->Set("IsSDKInitFailed", "1");
		}

		if (SdkTools::sdk_init_finish_fun_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_init_finish_fun_id, "i", code);
		}
	}

	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_SDKLOGIN_CALLBACK(JNIEnv* env, jobject thiz,jint code,jstring data)
	{
		if (SdkTools::sdk_login_panel_fun_id != 0)
		{
			std::string datainfo = cocos2d::JniHelper::jstring2string(data);
			LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_login_panel_fun_id ,"is",code,datainfo.c_str());
		}
	}

	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_SDKPAY_FINISH_CALLBACK(JNIEnv* env, jobject thiz,jint code,jstring data)
	{
		if (SdkTools::sdk_td_finish_fun_id != 0)
		{
			std::string datainfo = "";
			if (data)
			{
				datainfo = cocos2d::JniHelper::jstring2string(data);
			}
			LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_td_finish_fun_id , "is", code,datainfo.c_str());
		}
	}

	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_SDKCOMMON_CALLBACK(JNIEnv* env,jobject thiz, jint code,jstring strinfo)
	{
		if (SdkTools::sdk_common_fun_id != 0)
		{
			std::string datainfo = cocos2d::JniHelper::jstring2string(strinfo);
			LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_common_fun_id , "is",code,datainfo.c_str());
		}
	}

	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_DLG_CERTAIN(JNIEnv* env, jobject thiz)
	{
		if (SdkTools::dlg_certain_fun_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(SdkTools::dlg_certain_fun_id , "");
		}
		if(SdkTools::dialog_call_back_pointer != 0)
		{
			SdkTools::dialog_call_back_pointer(1);
		}
	}

	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_DLG_CANCEL(JNIEnv* env, jobject thiz)
	{
		if (SdkTools::dlg_cancel_fun_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(SdkTools::dlg_cancel_fun_id , "");
		}
		if(SdkTools::dialog_call_back_pointer != 0)
		{
			SdkTools::dialog_call_back_pointer(0);
		}
	}

	JNIEXPORT void JNICALL JAVA_INVOKE_CPP_CLOSE_APP_IN_ENGINE(JNIEnv* env, jobject thiz)
	{
		GameEngine::GetInstance()->CloseApp();
	}

}

