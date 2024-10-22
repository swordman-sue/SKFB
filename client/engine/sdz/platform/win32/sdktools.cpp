#include "platform/sdktools.h"
#include "platform/systemtools.h"
//#include "atlstr.h"
#include "luaengine/luaengine.h"
#include "gameengine/gameengine.h"
#include <string>
#include <stdlib.h>
#include "zqcommon/strutil.h"
#include <windows.h>

ZQ::UINT32 SdkTools::dlg_certain_fun_id = 0;
ZQ::UINT32 SdkTools::dlg_cancel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_init_finish_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_login_panel_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_td_finish_fun_id = 0;
ZQ::UINT32 SdkTools::sdk_common_fun_id = 0;
ZQ::UINT32 SdkTools::memory_warning_fun_id = 0;

dialog_callback SdkTools::dialog_call_back_pointer = nullptr;
dialog_callback SdkTools::network_dialog_call_back_pointer = nullptr;

bool SdkTools::sdk_is_inited = true;

std::string SdkTools::GetSdkName()
{
	return "zlgame";
}

std::string SdkTools::GetStartImagePath()
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

bool SdkTools::IsThirdAccountPlat()
{
	return false;
}

std::string SdkTools::GetChannelId()
{
	return "0";
}

std::string SdkTools::GetChannelIdEx()
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

std::string SdkTools::GetPhpTDOrderUrl()
{
	return "";
}

std::string SdkTools::GetPhpTDNotifyUrl()
{
	return "";
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
	ret_var = "type=win32";
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
	return "";
}

bool SdkTools::IsInited()
{
	return SdkTools::sdk_is_inited;
}

void SdkTools::CheckUpdate(const char* param)
{
	
}

void SdkTools::LoginPanel()
{

}

void SdkTools::LoginPannelWithParam(const char* param)
{

}

void SdkTools::Logout(const char* param)
{

}

void SdkTools::SwitchAccount(const char* param)
{

}

void SdkTools::TD(const char* param)
{
	
}

void SdkTools::ToUserCenter()
{
	
}

void SdkTools::ExitSdk(const char* param)
{
	
}

void SdkTools::CommonHandle(const char* param)
{
	
}

void SdkTools::KeyDataHandle(const char* param)
{

}

void SdkTools::GotoBrowser(const char* url)
{
	//GameEngine::GetInstance()->CloseApp();
	//TCHAR szBuf[MAX_PATH];
	//ZeroMemory(szBuf,MAX_PATH);
	//GetModuleFileName(NULL, szBuf, MAX_PATH);
	//CString dir(szBuf);
	//dir = dir.Left(dir.ReverseFind('\\'));
	//CString updir = dir.Left(dir.ReverseFind('\\'));
	//ShellExecute(NULL, "open", "explorer.exe", updir.GetBuffer(), NULL, SW_SHOWNORMAL);
	//
	//CString oper = L"update ";
	//oper.Append(dir);
	//ShellExecute(NULL, "open", "svn.exe", oper.GetBuffer(), NULL, SW_SHOWNORMAL);

	return;
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

	dlg_certain_fun_id = cb_certain_fun;
	dlg_cancel_fun_id = cb_cancel_fun;
}

#ifdef _WIN32
std::string ConvertUtf8ToGBK(const char* strUtf8)
{
	//不是utf8编码，返回原串
	unsigned char code_mark = strUtf8[0];
	if(code_mark <= 0xE0)
	{
		return strUtf8;
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
	ZQ::UINT32 btn_mark = MB_OK;
	if(cancel != NULL)
	{
		btn_mark = MB_OKCANCEL;
	}

	int ret = MessageBoxA(NULL, ConvertUtf8ToGBK(tips).c_str(), ConvertUtf8ToGBK(title).c_str(), btn_mark);
	ret = (ret == IDOK ? 1 : 0);
	
	call_back(ret);
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



