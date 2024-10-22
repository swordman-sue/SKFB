#include "platform/ios/basesdkhelper.h"
#include "SdkManager.h"
std::string GetSdkStringPropertie(const char* pro_name)
{
	std::string propertie = "";
	void* obj = SdkManager::sharedInstance()->GetPropertie(pro_name);
	if (nil != obj)
	{
		propertie = [[NSString stringWithFormat:@"%@",obj] UTF8String];
	}
	return propertie;
}

bool GetSdkBooleanPropertie(const char* pro_name)
{
	bool propertie = false;
    
	void* obj = SdkManager::sharedInstance()->GetPropertie(pro_name);
    
	if (nil != obj && obj)
	{
	propertie = [[NSString stringWithFormat:@"%@",obj] boolValue];
	}
	return propertie;
}



