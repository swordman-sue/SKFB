#include "platform/systemtools.h"
#include <string>
#include <stdlib.h>
#include <AdSupport/AdSupport.h>
#include "luaengine/luaengine.h"
#include "platform/CCCommon.h"
#include "cocos2d.h"
#include "SdkManager.h"
#include "zqcommon/strutil.h"
#include "platform/sdktools.h"
#include "platform/ios/ldid/ldid.h"
#include "platform/ios/PlatformUtil.h"
#include "platform/ios/basesdkhelper.h"
#include "gameengine/gameengine.h"
#include "AnotherReachability.h"
#include "macaddress.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <AssetsLibrary/ALAsset.h>

//网络不连通提示框回调
static void OnNetworkAlertCallback(int btn_id)
{
    //confirm
    if(btn_id == 1)
    {
        NSURL* url =[NSURL URLWithString:UIApplicationOpenSettingsURLString];
        if ([[UIApplication sharedApplication] canOpenURL:url])
        {
            [[UIApplication sharedApplication] openURL:url];
        }
    }
    
    //cancel
    else if(btn_id == 0)
    {
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

ZQ::UINT32 SystemTools::network_change_notify_fun_id = 0;

SystemTools::NETWORK_STATE_CHANGED_CALLBACK SystemTools::network_change_notify_fun = nullptr;

void* SystemTools::network_change_notify_fun_data = nullptr;

std::string SystemTools::GetProductName()
{
    GetAppPath();
    GetAppPackPath();
	static std::string tmp_str = "";
	if (tmp_str=="")
	{
		NSDictionary* infodict = [[NSBundle mainBundle] infoDictionary];
		NSString* appname = [infodict objectForKey:(NSString*)kCFBundleNameKey];
		if (nil == appname)
		{
			appname = [[NSProcessInfo processInfo] processName];
		}
		tmp_str = [appname UTF8String];
	}
	return tmp_str;
}

const std::string SystemTools::GetAppPath()
{
	NSString* path = NSHomeDirectory();
    if(![PlatformUtil isEndWith:path :@"/"])
    {
        path = [path stringByAppendingString:@"/"];
    }
	std::string path_str = [path UTF8String];
	return path_str;
}

const std::string SystemTools::GetAppPackPath()
{
	NSString* path = [[NSBundle mainBundle] resourcePath];
    if(![PlatformUtil isEndWith:path :@"/"])
    {
        path = [path stringByAppendingString:@"/"];
    }
	std::string path_str = [path UTF8String];
    
	return path_str;
}

bool SystemTools::IsOpenLocationService()
{
    return [PlatformUtil IsOpenLocationService];
}

std::string SystemTools::GetAppName()
{
	return GetSdkStringPropertie("APP_NAME");
}

SystemTools::NETWORKSTATE SystemTools::GetNetWorkState()
{
    const char * host_name = "www.baidu.com";
    if (host_name == 0)
	{
		return NETWORK_UNKNOW;
	}
    
	NETWORKSTATE result = NETWORK_UNKNOW;
	NSString* tmp_host_name = [NSString stringWithUTF8String:host_name];
    
    AnotherReachability *r = [AnotherReachability reachabilityWithHostName:tmp_host_name];    
	switch ([r currentReachabilityStatus])
	{
        // 没有网络连接
		case NotReachable:
			result = NETWORK_NONE;
			break;

        // 使用WiFi网络
        case ReachableViaWiFi:
			result = NETWORK_WIFI;
			break;
		
        // 使用移动网络
        case ReachableViaWWAN:
        {
            // 获取手机网络类型
            CTTelephonyNetworkInfo *info = [[CTTelephonyNetworkInfo alloc] init];
            
            NSString *currentStatus = info.currentRadioAccessTechnology;
            
            if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyGPRS"]) {
                
                result = NETWORK_MOBILE_2G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyEdge"]) {
                
                result = NETWORK_MOBILE_2G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyWCDMA"]){
                
                result = NETWORK_MOBILE_3G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyHSDPA"]){
                
                result = NETWORK_MOBILE_3G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyHSUPA"]){
                
                result = NETWORK_MOBILE_3G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyCDMA1x"]){
                
                result = NETWORK_MOBILE_2G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyCDMAEVDORev0"]){
                
                result = NETWORK_MOBILE_3G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyCDMAEVDORevA"]){
                
                result = NETWORK_MOBILE_3G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyCDMAEVDORevB"]){
                
                result = NETWORK_MOBILE_3G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyeHRPD"]){
                
                result = NETWORK_MOBILE_2G;
            }else if ([currentStatus isEqualToString:@"CTRadioAccessTechnologyLTE"]){
                
                result = NETWORK_MOBILE_4G;
            }
            break;
        }
        
        //未知网络
		default:
			result = NETWORK_UNKNOW;
	}
    
	return result;
}

bool SystemTools::IsNetworkAvailable()
{
    NETWORKSTATE state = SystemTools::GetNetWorkState();
    return (state == NETWORK_WIFI ||
            state == NETWORK_MOBILE_2G ||
            state == NETWORK_MOBILE_3G ||
            state == NETWORK_MOBILE_4G);
}

void SystemTools::CheckNetWork(const char* content, const char* positive_text, const char* negative_text)
{
    if (nullptr == content || nullptr == positive_text || nullptr == negative_text)
    {
        return;
    }

    if (!IsNetworkAvailable())
    {
        std::string strContent = content;
        std::string strPositiveText = positive_text;
        std::string strNegativeText = negative_text;
        const char* tmp_title = GetSdkStringPropertie("EXIT_INFO").c_str();
        const char* tmp_content = strcmp(content, "") != 0 ? content : GetSdkStringPropertie("BAD_NETWORK").c_str();
        const char* tmp_positive_text = strcmp(positive_text, "") != 0 ? positive_text : GetSdkStringPropertie("SETTING").c_str();
        const char* tmp_negative_text = strcmp(negative_text, "") ? negative_text : GetSdkStringPropertie("CANCEL").c_str();
        
        SdkTools::ShowDialog(tmp_title, tmp_content, tmp_positive_text, tmp_negative_text, OnNetworkAlertCallback);
    }
}

std::string SystemTools::GetDeviceId()
{
	return GetSdkStringPropertie("DEVICE_ID");
}

std::string SystemTools::GetImsi()
{
	return GetSdkStringPropertie("DEVICE_IMSI");
}

std::string SystemTools::GetPackageName()
{
	return GetSdkStringPropertie("PACKAGE_NAME");
}

float SystemTools::GetIOSVersion()
{
    float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    return version;
}

std::string SystemTools::GetAppVersion()
{
	return GetSdkStringPropertie("APP_VERSION");
}

std::string SystemTools::GetAppVersionCode()
{
    return GetSdkStringPropertie("APP_VERSION_CODE");
}

std::string SystemTools::GetDeviceModel()
{
	return GetSdkStringPropertie("DEVICE_MODEL");
}


ZQ::UINT32 SystemTools::GetMemorySize()
{
	return (NSRealMemoryAvailable() / 1048576); //MB
}

std::string SystemTools::GetMacAddress()
{
	return MacAddress::GetMacAddress();
}

std::string SystemTools::GetIPAddress()
{
    return "";
}

bool SystemTools::IsApptorePackage()
{
#ifdef __ZL_APPSTORE__
	return true;
#endif
    return false;
}

bool SystemTools::IsSupportAppAutoInstall()
{
    //ios提审，暂时屏蔽可疑代码 add by swordman sue
    /*
    std::string allowedSPIstr = "com.apple.private.mobileinstall.allowedSPI";
    std::string installstr = "Install";
    std::string apppath = std::string([[[NSBundle mainBundle] resourcePath] UTF8String]) + std::string("/")+ SystemTools::GetProductName();

    std::string entitlementsStr = Ldid::ldid(apppath);


    if (entitlementsStr.find(allowedSPIstr) == std::string::npos || entitlementsStr.find(installstr) == std::string::npos)
    {
        return false;
    }

    bool ret_var = [[NSFileManager defaultManager] fileExistsAtPath:@"/Library/MobileSubstrate/DynamicLibraries/AppSync.plist"];
    return ret_var;
     */
    return false;
}


// 获取ifa(ios6+)
std::string SystemTools::GetIdentifierForAdvertisers()
{
    if (NSClassFromString(@"ASIdentifierManager") == nil) {
        return std::string("");
    }
    Class aSIdentifierManger = NSClassFromString(@"ASIdentifierManager");
    return std::string([[[[aSIdentifierManger sharedManager] advertisingIdentifier] UUIDString] UTF8String]);
}

// 获取ifv(ios6+)
std::string SystemTools::GetIdentifierForVendor()
{
    if (![[UIDevice currentDevice] respondsToSelector:@selector(identifierForVendor)]) {
        return std::string("");
    }
    return std::string([[[[UIDevice currentDevice] identifierForVendor] UUIDString] UTF8String]);
}


std::string SystemTools::GetApkExpansionPath()
{
    return std::string("");
}

bool SystemTools::IsAppInstalled(std::string packageName)
{
    return false;
}

std::string SystemTools::GetLogPath()
{
    return SystemTools::GetAppPath();
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

//获取国家代码
std::string SystemTools::GetCountryCode()
{
    NSLocale* currentLocale = [NSLocale currentLocale];
    NSString* countryCode = [currentLocale objectForKey:NSLocaleCountryCode];
    return [countryCode UTF8String];
}

//获取货币代码
std::string SystemTools::GetCurrencyCode()
{
    NSLocale* currentLocale = [NSLocale currentLocale];
    NSString* currencyCode;
    float epsilon = 1e-6;
    float version = GetIOSVersion();
    if (version - 10.0f > epsilon || fabs(version - 10.0f) <= epsilon)
    {
        currencyCode = [currentLocale currencyCode];
    }
    else
    {
        currencyCode = [currentLocale objectForKey:NSLocaleCurrencyCode];
    }
    return [currencyCode UTF8String];
}

std::string SystemTools::ConvertUygurToEx(const std::string& orign)
{
    return orign;
}

// 将维语文本从基本区转换为扩展区并通过Fribidi库进行语序解析
std::string SystemTools::ConvertUygur(const std::string& orign)
{
    return orign;
}
