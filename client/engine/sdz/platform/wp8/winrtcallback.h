
#ifndef __I_CALL_BACK_H__
#define __I_CALL_BACK_H__

namespace PhoneDirect3DXamlAppComponent
{

 namespace OpenXLiveHelper
 {

	 public enum class XLiveDialogCallbackStatus
	 {
		 XLiveDialogCallbackStatusConfirm = 0,		// 确定
		 XLiveDialogCallbackStatusCancel = 1,		// 取消
		 XLiveDialogCallbackStatusMiddle = 2		// 其他扩展按钮
	 };

	 public enum class XLiveSDKCallbackDelegateType 
	 {
		 XLiveSDKCallbackDelegateTypeUnknown,
		 XLiveSDKCallbackDelegateTypeInit,
		 XLiveSDKCallbackDelegateTypeUpdate,
		 XLiveSDKCallbackDelegateTypeLogin,
		 XLiveSDKCallbackDelegateTypePay,
		 XLiveSDKCallbackDelegateTypeUserCenter,
		 XLiveSDKCallbackDelegateTypeCommonHandle,
		 XLiveSDKCallbackDelegateTypeExit
	 };

	 public enum class XLiveSDKCallbackDelegateStatus 
	 {
		 XLiveSDKCallbackDelegateStatusSuccessCode = 0,	 // 成功
		 XLiveSDKCallbackDelegateStatusFailureCode = -1,    // 失败的回调码
		 XLiveSDKCallbackDelegateStatusActResultGpsCode = 100,	 // 定位设置的onActivityResult 码
		 XLiveSDKCallbackDelegateStatusNormalExitCode = 1,	 // 正常关闭登录界面
		 XLiveSDKCallbackDelegateStatusLogoutCode = 2,	 // 登出
		 XLiveSDKCallbackDelegateStatusRechargeCode = 5,	 // 充值成功回调
		 XLiveSDKCallbackDelegateStatusOpenMusicCode = 401,   // 其他返回码处理 如：移动基地充值开启声音
		 XLiveSDKCallbackDelegateStatusCloseMusicCode = 402,   // 其他返回码处理 如：移动基地充值关闭声音
		 XLiveSDKCallbackDelegateStatusGetBalanceCode = 501,   // 获取49点金易积分总数
		 XLiveSDKCallbackDelegateStatusGetResultChanncelCode = 601    // 获取49点金易渠道是否可用
	 };

	 public enum class XLiveSDKExitCallbackDelegateStatus
	 {
		 XLiveSDKExitCallbackDelegateStatusExitOk,
		 XLiveSDKExitCallbackDelegateStatusExitCancel
	 };

	 ////////////////////////////////////////////////////////////////////////////
	 /// SDK Dialog Callback delegate
	 public ref class XLiveDialogCallbackDelegate sealed
	 {
	 public:
		 XLiveDialogCallbackDelegate();
		 void SetDialogCallback(uint32 selecteditem);
		 void SetDialogConfirmData(uint32 data);
		 void SetDialogCancelData(uint32 data);
		 void SetDialogMiddleData(uint32 data);
		 uint32 GetDialogConfirmData();
		 uint32 GetDialogCancelData();
		 uint32 GetDialogMiddleData();

		 void callback();

	 private:
		 property uint32 mDialogConfirmData;
		 property uint32 mDialogCancelData;
		 property uint32 mDialogMiddleData;
	 };
	 ///////////////////////////////////////////////////////////////////////////

	 /* 原版代码注释
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Init Delegate
	 public ref class XLiveSDKInitCallbackDelegate sealed
	 {
	 public:
		 XLiveSDKInitCallbackDelegate(Platform::String^ initInfo);
		 void SetInfo(Platform::String ^initInfo);
		 Platform::String^ GetInfo();
		 uint32 GetIsInit();
		 void SetIsInit(uint32 inited);	// 0 初始化成功 // 其他 初始化失败
	 private:
		 Platform::String^ info;
		 uint32 isInited;
	 };
	 ///////////////////////////////////////////////////////////////////////////////
	 
	 ///////////////////////////////////////////////////////////////////////////////
	 /// Check Update .apk or .ipa Callback Delegate
	 public ref class XLiveCheckUpdateCallbackDelegate sealed 
	 {
	 public:
	 	XLiveCheckUpdateCallbackDelegate(Platform::String^ updateInfo);
	 	void SetInfo(Platform::String^ updateInfo);
	 	Platform::String^ GetInfo();
	 private:
		 Platform::String^ info;
	 };
	 ////////////////////////////////////////////////////////////////////////////
	 */

	 public interface class XLiveSDKCallbackDelegate
	 {
	 public:
		 property Platform::String^ Info;
		 property XLiveSDKCallbackDelegateType DelegateType;
		 /*
		 property Platform::String^ Info
		 {
		 Platform::String^ get() { return info; }
		 void set(Platform::String^ aInfo)
		 {
		 info = aInfo;
		 }
		 }
		 property XLiveSDKCallbackDelegateType DelegateType
		 {
		 XLiveSDKCallbackDelegateType get() { return delegateType; }
		 void set(XLiveSDKCallbackDelegateType type)
		 {
		 delegateType = type;
		 }
		 }
		 private:
		 Platform::String^ info;
		 XLiveSDKCallbackDelegateType delegateType;
		 */
	 };

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Init Delegate
	 public ref class XLiveSDKInitCallbackDelegate sealed : public XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveSDKInitCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;

		 // 0 初始化成功 // 其他(-1) 初始化失败
		 property XLiveSDKCallbackDelegateStatus Init
		 {
			 XLiveSDKCallbackDelegateStatus get() { return isInited; }
			 void set(XLiveSDKCallbackDelegateStatus inited)
			 {
				 isInited = inited;
			 }
		 }
	 private:
		 XLiveSDKCallbackDelegateStatus isInited;
	 };
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// Check Update .apk or .ipa Callback Delegate
	 public ref class XLiveCheckUpdateCallbackDelegate sealed : public  XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveCheckUpdateCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;
	 };
	 ////////////////////////////////////////////////////////////////////////////

	 
	 /*原版代码注释
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Login Callback Delegate
	 public ref class XLiveSDKLoginCallbackDelegate sealed
	 {
	 public:
		 XLiveSDKLoginCallbackDelegate(Platform::String^ loginInfo);
		 void SetInfo(Platform::String ^loginInfo);
		 Platform::String^ GetInfo();
	 private:
		 Platform::String^ info;
	 };
	 ///////////////////////////////////////////////////////////////////////////////
	 
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Pay  Callback Delegate
	 public ref class XLiveSDKPayCallbackDelegate sealed
	 {
	 public:
		 XLiveSDKPayCallbackDelegate(Platform::String^ payInfo);
		 void SetInfo(Platform::String^ payInfo);
		 Platform::String^ GetInfo();
	 private:
		 Platform::String^ info;
	 };
	 ///////////////////////////////////////////////////////////////////////////////
	 
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Common Handle Callback Delegate
	 public ref class XLiveSDKCommonHandleCallbackDelegate sealed
	 {
	 public:
		 XLiveSDKCommonHandleCallbackDelegate(Platform::String^ commonInfo);
		 void SetInfo(Platform::String ^commonInfo);
		 Platform::String^ GetInfo();
	 private:
		 Platform::String^ info;
	 };
	 ///////////////////////////////////////////////////////////////////////////////
	 
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Exit Callback Delegate
	 public ref class XLiveSDKExitCallbackDelegate sealed
	 {
	 public:
		 XLiveSDKExitCallbackDelegate(Platform::String ^exitInfo);
		 void SetInfo(Platform::String ^exitInfo);
		 Platform::String^ GetInfo();
	 private:
		 Platform::String^ info;
	 };
	 ///////////////////////////////////////////////////////////////////////////////
	 */
	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Login Callback Delegate
	 public ref class XLiveSDKLoginCallbackDelegate sealed : public XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveSDKLoginCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;
		 void callback(XLiveSDKCallbackDelegateStatus code, Platform::String^ info);
	 };
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// To SDK User Center
	 public ref class XLiveSDKToUserCenterCallbackDelegate sealed : public XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveSDKToUserCenterCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;
	 };
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Pay  Callback Delegate
	 public ref class XLiveSDKPayCallbackDelegate sealed : public XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveSDKPayCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;
	 };
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Common Handle Callback Delegate
	 public ref class XLiveSDKCommonHandleCallbackDelegate sealed : public XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveSDKCommonHandleCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;
	 };
	 ///////////////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////////////////////////////////////////////////
	 /// SDK Exit Callback Delegate
	 public ref class XLiveSDKExitCallbackDelegate sealed : public XLiveSDKCallbackDelegate
	 {
	 public:
		 XLiveSDKExitCallbackDelegate();
		 virtual property Platform::String^ Info;
		 virtual property XLiveSDKCallbackDelegateType DelegateType;
		 // 0 表示退出 其他(-1) 表示取消
		 property XLiveSDKExitCallbackDelegateStatus Flag;
	 };
	 ///////////////////////////////////////////////////////////////////////////////
	 
	 public enum class BaseMsgDataType
	 {
		 LOGIN = 1,
		 PAY,
		 COMMON,
	 };

	 public interface class BaseMsgData
	 {
		 virtual int getAsyncId();
		 virtual int getCode();
		 virtual Platform::String^  getData();
		 virtual BaseMsgDataType getMsgType();
	 };

	//////////////////////////////////////////////////////////////////////////////////////
	//  在WinRT组件中（C++）创建符合WinRT编程规范的接口ICallback
	public interface class ICallback
	{
		// 
		void WriteLine(Platform::String^ wstr);
		// 
		void MessageBox(Platform::String^ wstrTitle, Platform::String^ wstrTips, Platform::String^ wstrCertain, Platform::String^ wstrCancel, XLiveDialogCallbackDelegate^ dialogDelegate);

		void GotoBrowser(Platform::String^ wstr);

		Platform::String^ GetDeviceId();

		Platform::String^ GetUserId();

		Platform::String^ GetDeviceModel();

		uint32 GetMemorySize();

		Platform::String^ GetAllMemoryInfo();

		void MemoryGC();

		bool SDKIsInited(XLiveSDKInitCallbackDelegate^ aDelegate);

		void CheckUpdate(XLiveCheckUpdateCallbackDelegate^ aDelegate);

		void SDKLogin(XLiveSDKLoginCallbackDelegate^ aDelegate);

		void SDKPay(XLiveSDKPayCallbackDelegate^ aDelegate);

		void SDK2UserCenter(XLiveSDKToUserCenterCallbackDelegate^ aDelegate);

		void ExitSdk(XLiveSDKExitCallbackDelegate^ aDelegate);

		void SDKCommonHandle(XLiveSDKCommonHandleCallbackDelegate^ aDelegate,Platform::String^ param);

		Platform::Array <BaseMsgData^>^ GetAyncMessage(int msg_code);
	};

	// 
	public ref class XLiveDelegate sealed
	{
	public:
		XLiveDelegate();

		void SetCallback(ICallback^ callbackobject);
		property static ICallback^ GlobalCallback;
	};
	//////////////////////////////////////////////////////////////////////////////////////
	public ref class XLiveHelperTools sealed
	{
	public:
		static Platform::String^ GetJsonValueForKey(Platform::String^ json, Platform::String^ key);
		static void LogInfo(Platform::String^ info);
	};
 }
}

#endif // #ifndef __I_CALL_BACK_H__

