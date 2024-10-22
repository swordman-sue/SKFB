#include "platform/systemtools.h"
#include <windows.h>
#include <Wininet.h>  
#include <iphlpapi.h>
#include <string>
#include <stdlib.h>
//#include <atlstr.h>
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include "fribidi-bidi-types.h"
#include "fribidi-deprecated.h"

#pragma comment (lib, "Wininet.lib")  

ZQ::UINT32 SystemTools::network_change_notify_fun_id = 0;

SystemTools::NETWORK_STATE_CHANGED_CALLBACK SystemTools::network_change_notify_fun = nullptr;

void* SystemTools::network_change_notify_fun_data = nullptr;

std::string SystemTools::GetProductName()
{
	return "skyx";
}
 const std::string SystemTools::GetAppPath()
 {
	 std::string path = GetAppPackPath();
	 return path+"../";
 }

 const std::string SystemTools::GetAppPackPath()
 {
	 // ���뱣֤��cocos2dһ��
	 WCHAR  wszPath[MAX_PATH] = {0};
	 char app_path[MAX_PATH] = {0};
	 WideCharToMultiByte(CP_ACP, 0, wszPath, GetCurrentDirectoryW(sizeof(wszPath), wszPath), app_path, MAX_PATH, NULL, NULL);
	 std::string app_path_str(app_path);
	 app_path_str += "\\";
	 return app_path_str;
 }

std::string SystemTools::GetAppName()
{
	return  "ʱ��Ӣ��";
}

bool SystemTools::IsOpenLocationService()
{
	return true;
}

SystemTools::NETWORKSTATE SystemTools::GetNetWorkState()
{
	DWORD flags;
	BOOL m_bOnline = TRUE;
	m_bOnline = InternetGetConnectedState(&flags, 0);
	if (m_bOnline)
	{
		return NETWORK_WIFI;
	}
	return NETWORK_NONE;
}

bool SystemTools::IsNetworkAvailable()
{
	DWORD flags;
	BOOL m_bOnline = TRUE;
	m_bOnline = InternetGetConnectedState(&flags, 0);
	return m_bOnline == TRUE;
}

void SystemTools::CheckNetWork(const char* content, const char* positive_text, const char* negative_text)
{
}

std::string SystemTools::GetDeviceId()
{
	PIP_ADAPTER_INFO adapter_info;
	ULONG out_len = sizeof(IP_ADAPTER_INFO);
	adapter_info = (IP_ADAPTER_INFO *)malloc(out_len);

	DWORD ret_code = GetAdaptersInfo(adapter_info, &out_len);
	if(ret_code == ERROR_BUFFER_OVERFLOW)
	{
		free(adapter_info);
		adapter_info = (IP_ADAPTER_INFO *)malloc(out_len);
		ret_code = GetAdaptersInfo(adapter_info, &out_len);
	}

	if(ret_code != NO_ERROR)
	{
		free(adapter_info);
		LogUtil::LogError("SystemTools::GetDeviceId Error:%d", ret_code);
		return "";
	}

	std::string device_id = adapter_info->AdapterName; 
	free(adapter_info);
	return device_id;
}

std::string SystemTools::GetImsi()
{
	return "";
}

std::string SystemTools::GetPackageName()
{
	return "win.zlgame.skyx";
}

std::string SystemTools::GetAppVersion()
{
	return "1.0.0";
}

std::string SystemTools::GetAppVersionCode()
{
	return "100";
}

std::string SystemTools::GetDeviceModel()
{
	return "pc_windows";
}

ZQ::UINT32 SystemTools::GetMemorySize()
{
	MEMORYSTATUSEX mem;
	mem.dwLength = sizeof(MEMORYSTATUSEX); 
	
	if(!GlobalMemoryStatusEx(&mem))
	{
		return 0; 
	}

	return (ZQ::UINT32)(mem.ullTotalPhys / 1048576); // MB;
}

std::string SystemTools::GetMacAddress()
{
	std::string ret_mac_address = "";
	PIP_ADAPTER_INFO adapter_info;
	ULONG out_len = sizeof(IP_ADAPTER_INFO);
	adapter_info = (IP_ADAPTER_INFO *)malloc(out_len);

	DWORD ret_code = GetAdaptersInfo(adapter_info, &out_len);
	if(ret_code == ERROR_BUFFER_OVERFLOW)
	{
		free(adapter_info);
		adapter_info = (IP_ADAPTER_INFO *)malloc(out_len);
		if (adapter_info == NULL) 
			return ret_mac_address;
		ret_code = GetAdaptersInfo(adapter_info, &out_len);
	}

	if(ret_code == NO_ERROR)
	{
		for(PIP_ADAPTER_INFO pAdapter = adapter_info; pAdapter != NULL; pAdapter = pAdapter->Next)
		{
			// ȷ������̫��
			if(pAdapter->Type != MIB_IF_TYPE_ETHERNET)
				continue;
			// ȷ��MAC��ַ�ĳ���Ϊ 00-00-00-00-00-00
			if(pAdapter->AddressLength != 6)
				continue;
			char ac_mac[32];
			sprintf(ac_mac, "%02X-%02X-%02X-%02X-%02X-%02X",
				int (pAdapter->Address[0]),
				int (pAdapter->Address[1]),
				int (pAdapter->Address[2]),
				int (pAdapter->Address[3]),
				int (pAdapter->Address[4]),
				int (pAdapter->Address[5]));
			ret_mac_address = ac_mac;
			break;
		}
	}

	free(adapter_info);
	return ret_mac_address;
}

std::string SystemTools::GetIPAddress()
{
	return "";
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
	return GetAppPackPath() + "res_ex/";
}

// �ж�ĳ�������İ�װ���Ƿ�װ
bool SystemTools::IsAppInstalled(std::string packageName){return false;}

// ��ȡsdcard��չ·��
std::string SystemTools::GetLogPath()
{
	return SystemTools::GetAppPath();
}

void SystemTools::NetworkStateChangeNotify()
{
}

// ��ȡ���Ҵ���
std::string SystemTools::GetCountryCode() 
{ 
	return "CN"; 
}

// ��ȡ���Ҵ���
std::string SystemTools::GetCurrencyCode() 
{
	return "CNY"; 
}

// ��ά���ı��ӻ�����ת��Ϊ��չ��
std::string SystemTools::ConvertUygurToEx(const std::string& orign)
{
	return orign;
}

// ��ά���ı��ӻ�����ת��Ϊ��չ����ͨ��Fribidi������������
std::string SystemTools::ConvertUygur(const std::string& orign)
{
	if (orign.empty()) {
		return "";
	}

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
			// ���������⴦��
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