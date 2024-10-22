package com.szba.skfb.dev;

import android.app.Application;
import android.content.Context;

import com.tencent.bugly.crashreport.CrashReport;

public class SkyxApp extends Application 
{
	@Override
	public void onCreate() 
	{
		super.onCreate();

		Context context = getApplicationContext();
		CrashReport.initCrashReport(context, "", true);
		CrashReport.enableBugly(true);
	}
}

