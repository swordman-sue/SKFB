package com.kuyue.contant;
/**
 * SDK相关常量
 *
 */
public class SdkContants {
	//成功的回调码
	public static final int SUCCESS_CODE = 0;
	//失败的回调码
	public static final int FAILURE_CODE = 1;
	//取消的回调码
	public static final int CANCEL_CODE = 2;
	//登出回调
	public static int LOGOUT_CODE = 3;
	//充值成功回调
	public static int RECHARGE_CODE = 4;
	//切换账号回调
	public static int SWITCH_ACCOUNT_CODE = 5;
	
	//调用QK操作函数成功
	public static int COMMON_CALLFUNC_SUCCESS = 6;

	//调用QK操作函数失败
	public static int COMMON_CALLFUNC_FAILED = 7;

	//动态申请权限
	public static final int DYNAMIC_PERMISSION_CODE = 8;
	
	//打开应用设置(为了申请权限)
	public static final int OPEN_SETTING_FOR_DYNAMIC_PERMISSION_CODE = 9;

	//定位设置的onActivityResult码
	public static final int ACT_RESULT_GPS_CODE = 100;
	//选中相册的onActivityResult码
	public static final int ACT_RESULT_ALBUM_CODE = 110;	
}
