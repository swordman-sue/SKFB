package com.szba.skfb.quickpp;

import java.util.HashMap;
import java.util.Map;

import com.kuyue.contant.SdkCommonHandleFuncType;

/**
 * QuickSDK������������
 */
public class QKFuncType {
	public static final int FuncType_UNDEFINED = 0;   //Unused
	public static final int FuncType_ENTER_BBS = 101; /*������̳*/
	public static final int FuncType_ENTER_USER_CENTER = 102; /*�����û�����*/
	public static final int FuncType_SHOW_TOOLBAR = 103; /*��ʾ����������*/
	public static final int FuncType_HIDE_TOOLBAR = 104; /*���ظ���������*/
	public static final int FuncType_Customer_Service = 130; /* �ͷ����� */
	public static final int FuncType_SWITCH_ACCOUNT = 107; /*�л��˺� ��android��*/
	public static final int FuncType_REAL_NAME_REGISTER = 105; /*ʵ����֤ ��android��*/
	public static final int FuncType_ANTI_ADDICTION_QUERY = 106; /*������ ��android��*/
	public static final int FuncType_SHARE = 108; /* ���� ��android��*/
	public static final int FuncType_NOTICE = 120; /* ���� ��android��*/		
	
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
		}
		
		return methodMap.get(var);
	};
}
