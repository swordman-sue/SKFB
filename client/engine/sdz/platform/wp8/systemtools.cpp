#include "platform/systemtools.h"
#include "luaengine/luaengine.h"
#include <windows.h>
#include <string>
#include <stdlib.h>
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"

ZQ::UINT32 SystemTools::device_back_pressed_fun_id = 0;

std::string SystemTools::GetProductName()
{
	return "sdz";
}

std::string PlatformStringToString(Platform::String^ s)
{
	std::wstring t = std::wstring(s->Data());
	return std::string(t.begin(),t.end());
}

 const std::string SystemTools::GetAppPath()
{
	Windows::Storage::ApplicationData^ data_package = Windows::Storage::ApplicationData::Current; 
	std::string app_data_path = PlatformStringToString(data_package->LocalFolder->Path);
	app_data_path += "\\Library\\";

	return app_data_path;
 }


 const std::string SystemTools::GetAppPackPath()
 {
	 Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
	 std::string app_path_str(PlatformStringToString(package->InstalledLocation->Path));
	 app_path_str += "\\assets\\";

	 return app_path_str;
}

 //const std::string SystemTools::GetWritablePath()
 //{
//		return "./Library";
 //}


std::string SystemTools::GetAppName()
{
	return  "у╫ои";
}

bool SystemTools::IsOpenLocationService()
{
	return true;
}

SystemTools::NETWORKSTATE SystemTools::GetNetWorkState()
{
	return NETWORK_WIFI;
}

std::string SystemTools::GetDeviceId()
{
	return "wp8_device_id";
}

std::string SystemTools::GetImsi()
{
	return "wp8_imsi";
}

std::string SystemTools::GetPackageName()
{
	return "wp8_package_name";
}

std::string SystemTools::GetAppVersion()
{
	return "2.20.151028";
}

std::string SystemTools::GetDeviceModel()
{
	return "pc_windows";
}

ZQ::UINT32 SystemTools::GetMemorySize()
{
	return 0;
}

std::string SystemTools::GetMacAddress()
{
	return "wp8_mac_address";
}

bool SystemTools::IsApptorePackage()
{
    return false;
}

bool SystemTools::IsSupportAppAutoInstall()
{
	return true;
}
bool SystemTools::IsAppInstalled(std::string packageName){ return false; }

std::string SystemTools::GetIdentifierForAdvertisers(){return "";}

std::string SystemTools::GetIdentifierForVendor(){return "";}
