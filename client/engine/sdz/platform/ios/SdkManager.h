//
//  SdkManager.h
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//
#ifndef _IOS_SDK_MANAGER_H
#define _IOS_SDK_MANAGER_H

#import "GpsLocation.h"
#import "BaseSdk.h"

class SdkManager
{
public:
    enum METHOD_NAME_CODE
	{
		SDK_LOGIN = 0,		//登陆
        SDK_LOGOUT,         //注销账号
        SDK_SWITCH_ACCOUNT, //切换账号
		SDK_TD,			//支付
        SDK_CHECK_UPDATE,	//检查更新
        SDK_EXIT_APP,		//退出
		TO_USER_CENTER,		//用户中心
		COMMON_HANDLE,		//通用操作
        KEY_DATA_HANDLE,    //运营数据统计
        START_LOCATION,
        STOP_LOCATION,
	};
    static SdkManager *sharedInstance();
    BaseSdk *GetSdk();
    void InvokeVoidMethod(METHOD_NAME_CODE methodCode,const char * param);
    void* GetPropertie(const char * proName);

protected:
    virtual ~SdkManager();

private:
     SdkManager(NSString * classname);
     BaseSdk * basesdk;
     GpsLocation * location;
};
#endif
