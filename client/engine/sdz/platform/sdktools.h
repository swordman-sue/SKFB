#ifndef SDK_OPERATE_TOOLS_H_
#define SDK_OPERATE_TOOLS_H_

#include "zqcommon/typedef.h"
#include <string>
#include "luaengine/luaengine.h"

typedef void (*dialog_callback)(int);

class SdkTools
{
public:
	static dialog_callback dialog_call_back_pointer;
    static dialog_callback network_dialog_call_back_pointer;

	static ZQ::UINT32 dlg_certain_fun_id;
	static ZQ::UINT32 dlg_cancel_fun_id;
	static ZQ::UINT32 sdk_init_finish_fun_id;
	static ZQ::UINT32 sdk_login_panel_fun_id;
	static ZQ::UINT32 sdk_td_finish_fun_id;
	static ZQ::UINT32 sdk_common_fun_id;
	static ZQ::UINT32 memory_warning_fun_id;

	static bool sdk_is_inited;

	// 获取SDK名称
	static std::string GetSdkName();
    //start背景图
    static std::string GetStartImagePath();
	//logo图
	static std::string GetLogoImagePath();
	//登陆背景图片
    static std::string GetLoginImagePath();
    //是否使用了第三方账号平台
    static bool IsThirdAccountPlat();
	//获取渠道ID
    static std::string GetChannelId();

	//获取渠道ID
	static std::string GetChannelIdEx();

	//获取SDK的APP ID
	static std::string GetAppId();
    //应用appkey
	static std::string GetAppKey();
	//获取SDK版本号
	static std::string GetSdkVersion();
    //获取充值订单号地址
	static std::string GetPhpTDOrderUrl();
	//获取充值通知地址
	static std::string GetPhpTDNotifyUrl();
	//获取安装包更新URL参数
	static std::string GetUpdateUrlParams();
	//获取渠道ID(云顶)
	static std::string GetKeyDataChannelId();

    static bool IsInited();
	static void LoginPanel();
	static void LoginPannelWithParam(const char* info);
	static void Logout(const char* param);
	static void SwitchAccount(const char* param);
	static void	TD(const char* info);
	static void CheckUpdate(const char* param);
	static void CommonHandle(const char* param);
	static void ExitSdk(const char* param);
	static void ToUserCenter();
	static void GotoBrowser(const char* url);
	static void KeyDataHandle(const char* param);

    //TODO:迁移到SystemTools
	static void ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, ZQ::UINT32 cb_certain_fun, ZQ::UINT32 cb_cancel_fun);

	static void ShowDialog(const char* title, const char* tips, const char* certain, const char* cancel, dialog_callback callback_fun);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	static void ShowNetworkDialog(const char* title, const char* tips, const char* certain, const char* cancel, dialog_callback callback_fun);
#endif

	static void SetLuaMemoryWarnCallback(ZQ::UINT32 fun_id){memory_warning_fun_id = fun_id;}

	static void RunMemoryWarning();

	static void SetLuaInitCallback(ZQ::UINT32 fun_id){ sdk_init_finish_fun_id = fun_id; }

	static void SetLuaLoginCallback(ZQ::UINT32 fun_id){sdk_login_panel_fun_id = fun_id;}
	
	static void SetLuaTDCallback(ZQ::UINT32 fun_id){sdk_td_finish_fun_id = fun_id;}
	
	static void SetLuaCommonHandleCallback(ZQ::UINT32 fun_id){sdk_common_fun_id = fun_id;}

	static bool OpenFile(std::string path, std::string actiontype, std::string mimetype);

	static bool Install(std::string installeroffullpath);
    
    static std::string GetAppStoreProductId(int price);
};
#endif
