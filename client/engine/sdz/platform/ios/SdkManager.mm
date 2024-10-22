
#import "SdkManager.h"

#import "zqcommon/logutil.h"

#import <Foundation/Foundation.h>


SdkManager::SdkManager(NSString * classname)
{

    Class clazz = NSClassFromString(classname);
    
    if(nil != clazz)
    {
        basesdk = [clazz alloc];
        [basesdk init];
    }else
    {
        LogUtil::LogError("then project not exist class : %s",classname);
        return;
    }

    location = [[GpsLocation alloc] init];

}
SdkManager::~SdkManager()
{
    if(basesdk != nil)
    {
        [basesdk release];
        basesdk = nil;
    }
    if(location != nil)
    {
        [location release];
        location = nil;
    }
}

SdkManager * SdkManager::sharedInstance()
{
    static SdkManager pSdkManager(@"Sdk");
    return &pSdkManager;
}
BaseSdk * SdkManager::GetSdk()
{
    if (basesdk == NULL)
    {
        LogUtil::LogError("basesdk is NULL");
    }
    return basesdk;
}

void SdkManager::InvokeVoidMethod(METHOD_NAME_CODE methodCode,const char * param)
{
    NSString * paramStr = @"";
    if (nullptr != param)
    {
        paramStr = [NSString stringWithUTF8String:param];
    }
    
    switch (methodCode) {
        case SDK_LOGIN:
            [basesdk SDKLogin:paramStr];
            break;
        case SDK_TD:
            [basesdk SDKTD:paramStr];
             break;
        case SDK_CHECK_UPDATE:
            [basesdk SDKCheckUpdate:paramStr];
            break;
        case SDK_EXIT_APP:
            [basesdk SDKExitApp:paramStr];
            break;
        case TO_USER_CENTER:
            [basesdk SDKToUserCenter:paramStr];
             break;
        case COMMON_HANDLE:
            [basesdk SDKCommonHandle:paramStr];
             break;
        case KEY_DATA_HANDLE:
            [basesdk SDKKeyDataHandle:paramStr];
            break;
        case SDK_LOGOUT:
            [basesdk SDKLogout:paramStr];
            break;
        case SDK_SWITCH_ACCOUNT:
            [basesdk SDKSwitchAccount:paramStr];
             break;
        case START_LOCATION:
            [location StartLocation:paramStr];
            break;
        case STOP_LOCATION:
            [location StopLocation];
            break;
        default:
            break;
    }
}

void* SdkManager::GetPropertie(const char * proName)
{
    if (proName)
    {
        NSString * proNameStr = [NSString stringWithUTF8String:proName];
    
        return [basesdk GetPropertie:proNameStr];
    }else
    {
        LogUtil::LogError("property name is not exist when call SdkManager::GetPropertie(const char *)");
    }
}

