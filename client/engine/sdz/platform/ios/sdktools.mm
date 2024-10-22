#include "platform/sdktools.h"
#include "platform/systemtools.h"
#include "platform/ios/basesdkhelper.h"
#include "luaengine/luaengine.h"
#include "platform/CCCommon.h"
#include "SdkManager.h"
#import "PlatformUtil.h"
#include <string>
#include "zqcommon/strutil.h"
#import "PushManager.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/logutil.h"

ZQ::UINT32 SdkTools::dlg_certain_fun_id = 0;
ZQ::UINT32 SdkTools::dlg_cancel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_init_finish_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_login_panel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_td_finish_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_common_fun_id = 0;
ZQ::UINT32 SdkTools::memory_warning_fun_id = 0;

dialog_callback SdkTools::dialog_call_back_pointer = 0;
dialog_callback SdkTools::network_dialog_call_back_pointer = 0;

bool SdkTools::sdk_is_inited = false;

std::string SdkTools::GetSdkName()
{
	return GetSdkStringPropertie("SDK_NAME");
}

std::string SdkTools::GetLoginImagePath()
{
	return GetSdkStringPropertie("LOGIN_IMAGE_PATH");
}

std::string SdkTools::GetStartImagePath()
{
	return GetSdkStringPropertie("START_IMAGE_PATH");
}
std::string SdkTools::GetLogoImagePath()
{
	return GetSdkStringPropertie("LOGO_IMAGE_PATH");
}

bool SdkTools::IsThirdAccountPlat()
{
	return GetSdkBooleanPropertie("IS_THIRD_ACCOUNT_PLAT");
}

std::string SdkTools::GetChannelId()
{
	return GetSdkStringPropertie("CHANNEL_ID");
}

std::string SdkTools::GetChannelIdEx()
{
	return GetSdkStringPropertie("CHANNEL_ID_EX");
}

std::string SdkTools::GetKeyDataChannelId()
{
    return GetSdkStringPropertie("SUB_CHANNEL");
}

std::string SdkTools::GetAppKey()
{
	return GetSdkStringPropertie("APP_KEY");
}

std::string SdkTools::GetAppId()
{
	return GetSdkStringPropertie("APP_ID");
}

std::string SdkTools::GetSdkVersion()
{
	return GetSdkStringPropertie("SDK_VERSION");
}

std::string SdkTools::GetPhpTDOrderUrl()
{
    return GetSdkStringPropertie("PHP_TD_ORDER_URL");
}

std::string SdkTools::GetPhpTDNotifyUrl()
{
    return GetSdkStringPropertie("PHP_TD_NOTIFY_URL");
}


std::string SdkTools::GetUpdateUrlParams()
{
	std::string ret_var;
	std::string rnd;
	ZQ::StrUtil::BaseTypeToString(arc4random() % 1000, rnd);
	ret_var = "type=apple";
	ret_var = ret_var + std::string("&rnd=") + rnd.c_str();
	ret_var = ret_var + std::string("&pkgname=") + SystemTools::GetPackageName().c_str();
	ret_var = ret_var + std::string("&fromid=") + SdkTools::GetChannelId().c_str();
	ret_var = ret_var + std::string("&forceplist=") + (SystemTools::IsSupportAppAutoInstall()?"0":"1");
	return ret_var;
}


bool SdkTools::IsInited()
{
	return SdkTools::sdk_is_inited;
}

void SdkTools::CheckUpdate(const char* param)
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_CHECK_UPDATE, "");
}

void SdkTools::LoginPanel()
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_LOGIN, "");
}

void SdkTools::LoginPannelWithParam(const char* param)
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_LOGIN, param);
}

void SdkTools::Logout(const char* param)
{
    SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_LOGOUT, param);
}

void SdkTools::SwitchAccount(const char* param)
{
    SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_SWITCH_ACCOUNT, param);
}

void SdkTools::TD(const char* param)
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_TD, param);
}

void SdkTools::ToUserCenter()
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::TO_USER_CENTER, "");
}

void SdkTools::ExitSdk(const char* param)
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::SDK_EXIT_APP, param);
}

void SdkTools::CommonHandle(const char* param)
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::COMMON_HANDLE, param);
}

void SdkTools::KeyDataHandle(const char* param)
{
    SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::KEY_DATA_HANDLE, param);
}

void SdkTools::GotoBrowser(const char* url)
{
	NSString* tmp_url = [NSString stringWithUTF8String:url];
	[PlatformUtil GotoBrowser:tmp_url];
}

void SdkTools::ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, ZQ::UINT32 cb_certain_fun, ZQ::UINT32 cb_cancel_fun)
{
    if(title == NULL || certain == NULL || tips == NULL)
    {
        LogUtil::LogError("dialog title or certain or tips is NULL");
        return;
    }
    
    if(cancel == NULL)
    {
        cancel = "";
    }

	if (dlg_certain_fun_id != 0 && cb_certain_fun != dlg_certain_fun_id)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(dlg_certain_fun_id);
	}
    if (dlg_cancel_fun_id != 0 && cb_cancel_fun != dlg_cancel_fun_id)
    {
        LuaEngine::GetInstance()->DelLuaFuncID(dlg_cancel_fun_id);
    }
	dlg_certain_fun_id = cb_certain_fun;
	dlg_cancel_fun_id = cb_cancel_fun;
    dialog_call_back_pointer = nullptr;
    
    NSString *temp_title = [NSString  stringWithUTF8String:title];
    NSString *temp_tips = [NSString  stringWithUTF8String:tips];
    NSString *temp_certain = [NSString  stringWithUTF8String:certain];
    NSString *temp_cancel = [NSString  stringWithUTF8String:cancel];
    
    [PlatformUtil ShowDialog:temp_title setMessage:temp_tips setCancelText :temp_cancel setCertainText :temp_certain];
}

void SdkTools::ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, dialog_callback callback_fun)
{
    if(title == NULL || certain == NULL || tips == NULL)
    {
        LogUtil::LogError("dialog title or certain or tips is NULL");
        return;
    }
    
    if(cancel == NULL)
    {
        cancel = "";
    }
    
    LuaEngine::GetInstance()->DelLuaFuncID(dlg_certain_fun_id);
    LuaEngine::GetInstance()->DelLuaFuncID(dlg_cancel_fun_id);
    dlg_certain_fun_id = 0;
    dlg_cancel_fun_id = 0;
    dialog_call_back_pointer = callback_fun;
    
    NSString *temp_title = [NSString  stringWithUTF8String:title];
    NSString *temp_tips = [NSString  stringWithUTF8String:tips];
    NSString *temp_certain = [NSString  stringWithUTF8String:certain];
    NSString *temp_cancel = [NSString  stringWithUTF8String:cancel];
    
   [PlatformUtil ShowDialog:temp_title setMessage:temp_tips setCancelText :temp_cancel setCertainText :temp_certain];
}

void SdkTools::ShowNetworkDialog(const char* title, const char* tips, const char* certain, const char* cancel, dialog_callback callback_fun)
{
    if(title == NULL || certain == NULL || tips == NULL)
    {
        LogUtil::LogError("dialog title or certain or tips is NULL");
        return;
    }
    
    if(cancel == NULL)
    {
        cancel = "";
    }
    
    network_dialog_call_back_pointer = callback_fun;
    
    NSString *temp_title = [NSString  stringWithUTF8String:title];
    NSString *temp_tips = [NSString  stringWithUTF8String:tips];
    NSString *temp_certain = [NSString  stringWithUTF8String:certain];
    NSString *temp_cancel = [NSString  stringWithUTF8String:cancel];
    
    [PlatformUtil ShowNetworkDialog:temp_title setMessage:temp_tips setCancelText :temp_cancel setCertainText :temp_certain];
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
	
	return ret_val;
}

bool SdkTools::Install(std::string installeroffullpath)
{
    //ios提审，暂时屏蔽可疑代码 add by swordman sue
//    return [IpaInstall IPAInstall:[NSString stringWithFormat:@"%s", installeroffullpath.c_str()]];
}

std::string SdkTools::GetAppStoreProductId(int price)
{
    
    char product_id[64] = {0};
    LuaEngine::GetInstance()->CallFunction("GetAppStoreProductId", "i>s", price, product_id);
    return std::string(product_id);
}







