#include "platform/sdktools.h"
#include "platform/systemtools.h"
#include "luaengine/luaengine.h"
#include "gameengine/gameengine.h"
#include "zqcommon/strutil.h"
#include "platform/wp8/winrtcallback.h"
#include <string>
#include <stdlib.h>
#include <malloc.h>

using namespace PhoneDirect3DXamlAppComponent;
using namespace PhoneDirect3DXamlAppComponent::OpenXLiveHelper;

ZQ::UINT32 SdkTools::dlg_certain_fun_id = 0;
ZQ::UINT32 SdkTools::dlg_cancel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_login_panel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_td_finish_fun_id = 0;
ZQ::UINT32 SdkTools::memory_warning_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_common_fun_id = 0;

dialog_callback SdkTools::dialog_call_back_pointer = 0;
bool SdkTools::sdk_is_inited = true;


std::string SdkTools::GetSdkName()
{
	return "wpsoha";
}

bool SdkTools::IsShowUserCenter()
{
	return false;
}

std::string SdkTools::GetUserCenterImagePath()
{
	return "";
}

std::string SdkTools::GetLogoImagePath()
{
	return "";
}

std::string SdkTools::GetLoginImagePath()
{
	return "";
}

std::string SdkTools::GetStartImagePath()
{
	return "trdres/start_image.jpg";
}

bool SdkTools::IsThirdAccountPlat()
{
	return true;
}

std::string SdkTools::GetChannelId()
{
	return "";
}
std::string SdkTools::GetAppId()
{
	return "";
}
std::string SdkTools::GetAppKey()
{
	return "";
}
std::string SdkTools::GetPhpOrderUrl()
{
	return "gameapi/apple_exorderno.php";
}

std::string SdkTools::GetPhpUserLoginUrl()
{
	return "user/userlogin.php";
}

std::string SdkTools::GetPhpNotifyUrl()
{
	return "";
}

std::string SdkTools::GetPhpUserInfoUrl()
{
	return "user/userinfo.php";
}

std::string SdkTools::GetSdkVersion()
{
	return "1";
}

std::string SdkTools::GetUpdateUrlParams()
{
	std::string ret_var;
	std::string rnd;
	ZQ::StrUtil::BaseTypeToString(rand() % 1000, rnd);
	ret_var = "type=wp8";
	ret_var = ret_var + std::string("&rnd=") + rnd.c_str();
	ret_var = ret_var + std::string("&pkgname=") + SystemTools::GetPackageName().c_str();
	ret_var = ret_var + std::string("&fromid=") + SdkTools::GetChannelId().c_str();

	return ret_var;
}


bool SdkTools::IsInited()
{
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveSDKInitCallbackDelegate^ callbackDelegate = ref new XLiveSDKInitCallbackDelegate();
	SdkTools::sdk_is_inited = delegate->GlobalCallback->SDKIsInited(callbackDelegate);
	return SdkTools::sdk_is_inited;
}

void SdkTools::CheckUpdate(const char* param)
{
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveCheckUpdateCallbackDelegate^ updateCallbackDelegate = ref new XLiveCheckUpdateCallbackDelegate();
	//delegate->GlobalCallback->CheckUpdate(updateCallbackDelegate);
}

void SdkTools::LoginPanel()
{
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveSDKLoginCallbackDelegate^ loginDelegate = ref new XLiveSDKLoginCallbackDelegate();
	delegate->GlobalCallback->SDKLogin(loginDelegate);
}

void SdkTools::LoginPannelWithParam(const char* param)
{

}


void SdkTools::TD(const char* param)
{
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveSDKPayCallbackDelegate^ payDelegate = ref new XLiveSDKPayCallbackDelegate();
	payDelegate->Info = ZQ::StrUtil::GetJsonValueForKey(param, "order_no");
	delegate->GlobalCallback->SDKPay(payDelegate);
}

void SdkTools::ToUserCenter()
{
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveSDKToUserCenterCallbackDelegate^ userCenterDelegate = ref new XLiveSDKToUserCenterCallbackDelegate();
	delegate->GlobalCallback->SDK2UserCenter(userCenterDelegate);
}

void SdkTools::ExitSdk(const char* param)
{
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveSDKExitCallbackDelegate^ exitDelegate = ref new XLiveSDKExitCallbackDelegate();
	exitDelegate->Info = ZQ::StrUtil::Utf8ToWStringForWindowsPlatform(param);
	delegate->GlobalCallback->ExitSdk(exitDelegate);
}

void SdkTools::CommonHandle(const char* param)
{
	size_t len = strlen(param) + 1;
	size_t convertNum = 0;
	wchar_t* wParam = (wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&convertNum, wParam, len, param, _TRUNCATE);
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	XLiveSDKCommonHandleCallbackDelegate^ commonHandleDelegate = ref new XLiveSDKCommonHandleCallbackDelegate();
	delegate->GlobalCallback->SDKCommonHandle(commonHandleDelegate,ref new Platform::String(wParam));
	free(wParam);
}


void SdkTools::GotoBrowser(const char* url)
{
	// wp8_no_support_begin
	//GameEngine::GetInstance()->CloseApp();
	//TCHAR szBuf[MAX_PATH];
	//ZeroMemory(szBuf,MAX_PATH);
	//GetModuleFileName(NULL, szBuf, MAX_PATH);
	//CString dir(szBuf);
	//dir = dir.Left(dir.ReverseFind('\\'));
	//CString updir = dir.Left(dir.ReverseFind('\\'));
	//ShellExecute(NULL, L"open", L"explorer.exe", updir.GetBuffer(), NULL, SW_SHOWNORMAL);
	//
	//CString oper = L"update ";
	//oper.Append(dir);
	//ShellExecute(NULL, L"open", L"svn.exe", oper.GetBuffer(), NULL, SW_SHOWNORMAL);
	// wp8_no_support_end
	XLiveDelegate^ delegate = ref new XLiveDelegate();
	Platform::String ^ wstr = ZQ::StrUtil::Utf8ToWStringForWindowsPlatform(url);
	delegate->GlobalCallback->GotoBrowser(wstr);
}

void SdkTools::ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, ZQ::UINT32 cb_certain_fun, ZQ::UINT32 cb_cancel_fun)
{
	// TODO 未实现回调，简单提示
	cocos2d::MessageBox(tips, certain);
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

	dlg_certain_fun_id = cb_certain_fun;
	dlg_cancel_fun_id = cb_cancel_fun;
}

#ifdef _WIN32
std::string ConvertUtf8ToGBK(const char* strUtf8)
{
	if(strUtf8 == NULL)
	{
		return "";
	}

	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8, -1, NULL,0); 
	unsigned short * wszGBK = new unsigned short[len+1];       
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8, -1, (LPWSTR)wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL);
	std::string gbkString = szGBK;
	delete[] wszGBK;
	delete[] szGBK;
	return gbkString;
}
#endif

void SdkTools::ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel,dialog_callback call_back)
{
	cocos2d::MessageBox(tips, certain);
	call_back(true);
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
	bool ret_val = false;

	return ret_val;
}

std::string SdkTools::GetAppStoreProductId(int price)
{
	return "";
}


