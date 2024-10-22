package com.szba.skfb.quickpp;

import android.content.Context;

import com.quicksdk.QuickSdkApplication;
import com.tencent.bugly.crashreport.CrashReport;

public class SkyxApplication extends QuickSdkApplication 
{
	@Override
	public void onCreate() 
	{
		super.onCreate();
		
		// 初始化Bugly
		Context context = getApplicationContext();
		CrashReport.initCrashReport(context, "", false);		
	}
}

