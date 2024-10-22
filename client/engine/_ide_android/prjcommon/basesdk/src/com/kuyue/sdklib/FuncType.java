package com.kuyue.sdklib;

import java.util.HashMap;
import java.util.Map;

import com.kuyue.contant.SdkCommonHandleFuncType;

/**
 * SDKCommonHandle中的操作函数类型
 */
public class FuncType {
	public static final int FuncType_UNDEFINED = 0;   //Unused
	public static final int FuncType_ENTER_BBS = 1; /*进入论坛*/
	public static final int FuncType_ENTER_USER_CENTER = 2; /*进入用户中心*/
	public static final int FuncType_SHOW_TOOLBAR = 3; /*显示浮动工具栏*/
	public static final int FuncType_HIDE_TOOLBAR = 4; /*隐藏浮动工具栏*/
	public static final int FuncType_Customer_Service = 5; /* 客服中心 */
	public static final int FuncType_SWITCH_ACCOUNT = 6; /*切换账号 */
	public static final int FuncType_REAL_NAME_REGISTER = 7; /*实名认证 */
	public static final int FuncType_ANTI_ADDICTION_QUERY = 8; /*防沉迷 */
	public static final int FuncType_SHARE = 9; /* 分享 */
	public static final int FuncType_NOTICE = 10; /* 公告 */	
	public static final int FuncType_BIND_ACCOUNT = 11; /* 绑定账号 */		

	public static final int FuncType_FB_GET_INVITABLE_FRIENDS = 12; /* 获取Facebook可邀请好友列表 */		
	public static final int FuncType_FB_GET_INACTIVE_FRIENDS = 13; /* 获取Facebook可召回好友列表 */		
	public static final int FuncType_FB_INVITE_FRIENDS = 14; /* 邀请Facebook好友 */		
	public static final int FuncType_FB_RECALL_FRIENDS = 15; /* 召回Facebook可好友 */		
	
	public static final int FuncType_ADJUST_EVENT_TRACK = 16; /* adjust事件跟踪 */		
	public static final int FuncType_CUSTOM_EVENT_TRACK = 17; /* 自定义事件跟踪 */
	
	public static final int FuncType_PICTRUE_EVENT_OPEN = 18; /* 打开相册 */
	public static final int FuncType_PICTRUE_EVENT_SEND = 19; /*选择完毕*/

	public static final int FuncType_UPLOAD_ADVERTISING  = 20; /*加载广告*/
	public static final int FuncType_PLAY_ADVERTISING  = 21; /*播放广告*/
	public static final int FuncType_END_ADVERTISING  = 22; /*播放广告*/

	private static final Map<String, Integer> methodMap = new HashMap<String,Integer>();
	
	public static int parseCommonHandleFuncType(String var)
	{
		if (methodMap.isEmpty())
		{
			methodMap.put(SdkCommonHandleFuncType.FuncType_UNDEFINED, FuncType_UNDEFINED);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_ENTER_BBS, FuncType_ENTER_BBS);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_ENTER_USER_CENTER, FuncType_ENTER_USER_CENTER);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_SHOW_TOOLBAR, FuncType_SHOW_TOOLBAR);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_HIDE_TOOLBAR, FuncType_HIDE_TOOLBAR);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_Customer_Service, FuncType_Customer_Service);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_SWITCH_ACCOUNT, FuncType_SWITCH_ACCOUNT);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_REAL_NAME_REGISTER, FuncType_REAL_NAME_REGISTER);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_ANTI_ADDICTION_QUERY, FuncType_ANTI_ADDICTION_QUERY);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_SHARE, FuncType_SHARE);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_NOTICE, FuncType_NOTICE);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_BIND_ACCOUNT, FuncType_BIND_ACCOUNT);			

			methodMap.put(SdkCommonHandleFuncType.FuncType_FB_GET_INVITABLE_FRIENDS, FuncType_FB_GET_INVITABLE_FRIENDS);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_FB_GET_INACTIVE_FRIENDS, FuncType_FB_GET_INACTIVE_FRIENDS);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_FB_INVITE_FRIENDS, FuncType_FB_INVITE_FRIENDS);			
			methodMap.put(SdkCommonHandleFuncType.FuncType_FB_RECALL_FRIENDS, FuncType_FB_RECALL_FRIENDS);			
			
			methodMap.put(SdkCommonHandleFuncType.FuncType_ADJUST_EVENT_TRACK, FuncType_ADJUST_EVENT_TRACK);						
			methodMap.put(SdkCommonHandleFuncType.FuncType_CUSTOM_EVENT_TRACK, FuncType_CUSTOM_EVENT_TRACK);		
			
			methodMap.put(SdkCommonHandleFuncType.FuncType_PICTRUE_EVENT_OPEN, FuncType_PICTRUE_EVENT_OPEN);	
			methodMap.put(SdkCommonHandleFuncType.FuncType_PICTRUE_EVENT_SEND, FuncType_PICTRUE_EVENT_SEND);	

			methodMap.put(SdkCommonHandleFuncType.FuncType_UPLOAD_ADVERTISING, FuncType_UPLOAD_ADVERTISING);
			methodMap.put(SdkCommonHandleFuncType.FuncType_PLAY_ADVERTISING, FuncType_PLAY_ADVERTISING);
			methodMap.put(SdkCommonHandleFuncType.FuncType_END_ADVERTISING, FuncType_END_ADVERTISING);

		}
		
		return methodMap.get(var);
	};
}
