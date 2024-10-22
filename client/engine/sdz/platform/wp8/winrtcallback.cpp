/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "winrtcallback.h"
#include "zqcommon/logutil.h"
#include "platform/sdktools.h"
#include "zqcommon/strutil.h"
#include "json/json.h"

namespace PhoneDirect3DXamlAppComponent
{
 namespace OpenXLiveHelper
 {
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Dialog delegate
	 XLiveDialogCallbackDelegate::XLiveDialogCallbackDelegate()
	 {

	 }

	 void XLiveDialogCallbackDelegate::SetDialogCallback(uint32 selecteditem)
	 {
		 switch (selecteditem)
		 {
		 case 0:
			 LogUtil::LogInfo("[XLiveDialogCallbackDelegate SetDialogCallback] Dialog Confirm");
			 break;
		 case 1:
			 LogUtil::LogInfo("[XLiveDialogCallbackDelegate SetDialogCallback] Dialog Cancel");
			 break;
		 case 2:
			 LogUtil::LogInfo("[XLiveDialogCallbackDelegate SetDialogCallback] Dialog Middle");
			 break;
		 default:
			 LogUtil::LogInfo("[XLiveDialogCallbackDelegate SetDialogCallback] Default");
			 break;
		 }
	 }

	 void XLiveDialogCallbackDelegate::SetDialogConfirmData(uint32 data)
	 {
		 mDialogConfirmData = data;
	 }
	 void XLiveDialogCallbackDelegate::SetDialogCancelData(uint32 data)
	 {
		 mDialogCancelData = data;
	 }
	 void XLiveDialogCallbackDelegate::SetDialogMiddleData(uint32 data)
	 {
		 mDialogMiddleData = data;
	 }
	 uint32 XLiveDialogCallbackDelegate::GetDialogConfirmData()
	 {
		 return mDialogConfirmData;
	 }
	 uint32 XLiveDialogCallbackDelegate::GetDialogCancelData()
	 {
		 return mDialogCancelData;
	 }
	 uint32 XLiveDialogCallbackDelegate::GetDialogMiddleData()
	 {
		 return mDialogMiddleData;
	 }

	 void XLiveDialogCallbackDelegate::callback()
	 {
		 // TODO: callback
	 }
	 ///////////////////////////////////////////////////////////////////////////////

	 //注释原版代码
	 /*
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK初始化代理
	 XLiveSDKInitCallbackDelegate::XLiveSDKInitCallbackDelegate(Platform::String^ initInfo)
	 {
		 this->SetInfo(initInfo);
	 }
	 void XLiveSDKInitCallbackDelegate::SetInfo(Platform::String^ initInfo)
	 {
		 this->info = initInfo;
	 }
	 Platform::String^ XLiveSDKInitCallbackDelegate::GetInfo()
	 {
		 return this->info;
	 }
	 uint32 XLiveSDKInitCallbackDelegate::GetIsInit()
	 {
	 	return isInited;
	 }
	 void XLiveSDKInitCallbackDelegate::SetIsInit(uint32 inited)
	 {
	 	isInited = inited;
	 }
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// 更新游戏.apk,.xap或者.ipa
	 XLiveCheckUpdateCallbackDelegate::XLiveCheckUpdateCallbackDelegate(Platform::String^ updateInfo)
	 {
	 	this->SetInfo(updateInfo);
	 }
	 void XLiveCheckUpdateCallbackDelegate::SetInfo(Platform::String^ updateInfo)
	 {
	 	this->info = updateInfo;
	 }
	 Platform::String^ XLiveCheckUpdateCallbackDelegate::GetInfo()
	 {
	 	return this->info;
	 }
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK登陆代理
	 XLiveSDKLoginCallbackDelegate::XLiveSDKLoginCallbackDelegate(Platform::String^ loginInfo)
	 {
		 this->SetInfo(loginInfo);
	 }
	 void XLiveSDKLoginCallbackDelegate::SetInfo(Platform::String^ loginInfo)
	 {
		 this->info = loginInfo;
	 }
	 Platform::String^ XLiveSDKLoginCallbackDelegate::GetInfo()
	 {
		 return this->info;
	 }
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK支付代理
	 XLiveSDKPayCallbackDelegate::XLiveSDKPayCallbackDelegate(Platform::String^ payInfo)
	 {
		 this->SetInfo(payInfo);
	 }
	 void XLiveSDKPayCallbackDelegate::SetInfo(Platform::String^ payInfo)
	 {
		 this->info = payInfo;
	 }
	 Platform::String^ XLiveSDKPayCallbackDelegate::GetInfo()
	 {
		 return this->info;
	 }
	 ///////////////////////////////////////////////////////////////////////////////


	 
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK通用方法代理
	 XLiveSDKCommonHandleCallbackDelegate::XLiveSDKCommonHandleCallbackDelegate(Platform::String^ commonInfo)
	 {
	 	this->SetInfo(commonInfo);
	 }
	 void XLiveSDKCommonHandleCallbackDelegate::SetInfo(Platform::String^ commonInfo)
	 {
		 this->info = commonInfo;
	 }
	 Platform::String^ XLiveSDKCommonHandleCallbackDelegate::GetInfo()
	 {
	 	return this->info;
	 }
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK退出回代理
	 XLiveSDKExitCallbackDelegate::XLiveSDKExitCallbackDelegate(Platform::String^ exitInfo)
	 {
	 	this->SetInfo(exitInfo);
	 }
	 void XLiveSDKExitCallbackDelegate::SetInfo(Platform::String^ exitInfo)
	 {
	 	this->info = exitInfo;
	 }
	 Platform::String^ XLiveSDKExitCallbackDelegate::GetInfo()
	 {
	 	return this->info;
	 }*/

	 ///////////////////////////////////////////////////////////////////////////////

	 XLiveSDKInitCallbackDelegate::XLiveSDKInitCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypeInit;
	 }
	 XLiveSDKExitCallbackDelegate::XLiveSDKExitCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypeExit;
	 }
	 XLiveSDKCommonHandleCallbackDelegate::XLiveSDKCommonHandleCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypeCommonHandle;
	 }
	 XLiveSDKPayCallbackDelegate::XLiveSDKPayCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypePay;
	 }
	 XLiveSDKToUserCenterCallbackDelegate::XLiveSDKToUserCenterCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypeUserCenter;
	 }
	 XLiveSDKLoginCallbackDelegate::XLiveSDKLoginCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypeLogin;
	 }
	 void XLiveSDKLoginCallbackDelegate::callback(XLiveSDKCallbackDelegateStatus code, Platform::String^ info)
	 {
		 if (SdkTools::sdk_login_panel_fun_id != 0)
		 {
			 std::string datainfo = ZQ::StrUtil::WindowsPlatformToUTF8(info);

			 LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_login_panel_fun_id, "is", code, datainfo.c_str());
		 }
	 }
	 XLiveCheckUpdateCallbackDelegate::XLiveCheckUpdateCallbackDelegate()
	 {
		 DelegateType = XLiveSDKCallbackDelegateType::XLiveSDKCallbackDelegateTypeUpdate;
	 }

	 ///////////////////////////////////////////////////////////////////////////////
	 /// 在WinRT组件中（C++）创建符合WinRT编程规范的接口ICallback
	 XLiveDelegate::XLiveDelegate()
	{

	}

	void XLiveDelegate::SetCallback(ICallback^ callback)
	{
		GlobalCallback = callback;
	}
	///////////////////////////////////////////////////////////////////////////////

	Platform::String^ XLiveHelperTools::GetJsonValueForKey(Platform::String^ json, Platform::String^ key)
	{

		Json::Reader json_reader;
		Json::Value root;
		std::string jsonstr = ZQ::StrUtil::WindowsPlatformToUTF8(json);
		std::string keystr = ZQ::StrUtil::WindowsPlatformToUTF8(key);

		if (!json_reader.parse(jsonstr, root))
		{
			LogUtil::LogError("[GetJsonValueForKey JsonDecodeError][data:%s len:%d]", jsonstr, jsonstr.length());
			return "";
		}

		const char * ret = root[keystr].asCString();

		return ZQ::StrUtil::Utf8ToWStringForWindowsPlatform(ret);
	}

	void XLiveHelperTools::LogInfo(Platform::String^ info)
	{
		std::string infostr = ZQ::StrUtil::WindowsPlatformToUTF8(info);
		LogUtil::LogInfo("%s", infostr.c_str());
	}
 }
}

