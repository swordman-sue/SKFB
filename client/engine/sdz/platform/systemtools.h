#ifndef SDK_INFO_TOOLS_H_
#define SDK_INFO_TOOLS_H_

#include "base/ccUTF8.h"
#include "zqcommon/typedef.h"

#include <string>

//Sdk信息获取
class SystemTools
{
public:
	//网络连接状态 (0 无，1 WIFI，2 2G网络，3 3G网络，4 4G网络，5 其他)
	enum NETWORKSTATE
	{
		NETWORK_NONE = 0,		//无网络连接
		NETWORK_WIFI,			//wifi连接
		NETWORK_MOBILE_2G,		//移动网络2G
		NETWORK_MOBILE_3G,		//移动网络3G
		NETWORK_MOBILE_4G,		//移动网络4G
		NETWORK_UNKNOW			//未知连接
	};

	// 获取系统类型
	static const std::string GetSystemType()
	{
#if defined(WP8)
		return "wp8";
#elif defined(_WIN32)
		return "win32";
#elif defined(__APPLE__)
		return "apple";
#elif defined(__ANDROID__)
		return "android";
#endif
		return "";
	}

	// 网络状态变化回调
	typedef void (*NETWORK_STATE_CHANGED_CALLBACK) (void*);
	static ZQ::UINT32 network_change_notify_fun_id;
	static NETWORK_STATE_CHANGED_CALLBACK network_change_notify_fun;
	static void* network_change_notify_fun_data;

    static ZQ::UINT32 device_back_pressed_fun_id;

    // 获取内存大小
	static ZQ::UINT32 GetMemorySize();

    // 获取应用名
	static std::string GetProductName();
	// 获取应用路径
	static const std::string GetAppPath();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // 获取IOS系统版本
    static float GetIOSVersion();
#endif
    
	// 获取程序路径
	static const std::string GetAppPackPath();
	// 获取应用名称（游戏名称）
	static std::string GetAppName();
	// 是否开启定位服务
	static bool IsOpenLocationService();
	// 获取网络状态   0.无网络连接  1.wifi连接 2.移动网路连接 3.未知连接
	static NETWORKSTATE GetNetWorkState();
	// 网络是否有效
	static bool IsNetworkAvailable();
	// 检测网络状态
	static void CheckNetWork(const char* content, const char* positive_text, const char* negative_text);
	// 获取设备ID
	static std::string GetDeviceId();
	// 获取IMSI码
	static std::string GetImsi();
	// 获取包名称
	static std::string GetPackageName();
	// 获取版本号(1.0.0)
	static std::string GetAppVersion();
	// 获取版本号(100)
	static std::string GetAppVersionCode();
	// 获取机型
	static std::string GetDeviceModel();
	// 获取MAC
	static std::string GetMacAddress();
	// 获取IP
	static std::string GetIPAddress();

	// 是否是苹果版的包
	static bool IsApptorePackage();

	// 是否支持自安装
	static bool IsSupportAppAutoInstall();
	// 获取ifa(ios6+)
	static std::string GetIdentifierForAdvertisers();
	// 获取ifv
	static std::string GetIdentifierForVendor();
	// 获取扩展文件路径(googleplay)
	static std::string GetApkExpansionPath();
	// 判断某个包名的安装包是否安装
	static bool IsAppInstalled(std::string packageName);
	// 获取日志目录路径
	static std::string GetLogPath();

	// 设置网络状态变化回调
	static void SetNetworkStateChangeNotifyFunc(ZQ::UINT32 fun_id){ network_change_notify_fun_id = fun_id; }
	static void SetNetworkStateChangeNotifyFuncEx(NETWORK_STATE_CHANGED_CALLBACK func, void* func_data)
	{ 
		network_change_notify_fun = func; 
		network_change_notify_fun_data = func_data;
	}
	// 网络状态变化通知
	static void NetworkStateChangeNotify();

	// 获取国家代码
	static std::string GetCountryCode();
	// 获取货币代码
	static std::string GetCurrencyCode();

	// 将维语文本从基本区转换为扩展区
	static std::string ConvertUygurToEx(const std::string& orign);

	// 将维语文本从基本区转换为扩展区并通过Fribidi库进行语序解析
	static std::string ConvertUygur(const std::string& orign);
};

#endif
