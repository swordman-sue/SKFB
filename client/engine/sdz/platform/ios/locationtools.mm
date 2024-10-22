#include "platform/locationtools.h"
#include "SdkManager.h"

void LocationTools::StartLocation(int timeout)
{
	std::string parame= [[NSString stringWithFormat:@"%d",timeout] UTF8String];
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::START_LOCATION, parame.c_str());
};

void LocationTools::StopLocation()
{
	SdkManager::sharedInstance()->InvokeVoidMethod(SdkManager::STOP_LOCATION, "");
};

