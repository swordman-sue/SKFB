package com.kuyue.sdk;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.LinearLayout;

import com.kuyue.contant.SdkContants;
import com.kuyue.file.FileAsyncHanderManager;
import com.kuyue.gps.GpsLocation;
import com.kuyue.pushsdk.MsgPush;
import com.kuyue.queue.MessageMagager;
import com.kuyue.sdklib.PlatformUtil;
import com.kuyue.sdklib.SdkManager;
import com.kuyue.webview.AdWebViewUtil;

public class Sdk {
	private static Activity activity = null;
	private static Sdk sdk = new Sdk();
	private Sdk() {}
	public static void initContext(Activity act,Class<?> clazz, LinearLayout layout)
	{
		activity = act;
		PlatformUtil.SetContext(activity);
//		GpsLocation.initContext(activity);
		AdWebViewUtil.CreateManager(activity, layout);
		FileAsyncHanderManager.initContext(activity);
		MessageMagager.initContext(activity);
		SdkManager.CreateSdk(activity, clazz);
	}
	public static Sdk getInstance()
	{
		return sdk;
	}
	public static Context getContext() 
	{
		return activity;
	}

	public void onCreate(Bundle savedInstanceState)
	{
		SdkManager.getInstence().onCreate(savedInstanceState);
	}
	
	public void onDestroy()
	{
		SdkManager.getInstence().onDestroy();
		MsgPush.DestoryPush();
//		GpsLocation.GetInstance().onDestroy();
	}

	public void onRestart()
	{
		SdkManager.getInstence().onRestart();
	}
	
	public void onPause()
	{
		SdkManager.getInstence().onPause();
	}
	
	public void onResume()
	{
		SdkManager.getInstence().onResume();
	}

	public void onStart()
	{
		SdkManager.getInstence().onStart();
	}

	public void onStop()
	{
		SdkManager.getInstence().onStop();
	}
	public void SDKExitAppNotify(boolean isCustomSDK)
	{
		SdkManager.getInstence().SDKExitAppNotify(isCustomSDK);
	}	
	public void onLogoutResult()
	{
		SdkManager.getInstence().onLogoutResult();
	}
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		SdkManager.getInstence().onKeyDown( keyCode , event  );
		return false;
	}
	
	
	public void onSaveInstanceState(Bundle outState)
	{
		SdkManager.getInstence().onSaveInstanceState(outState);
	}
	public void onNewIntent(Intent intent)
	{
		SdkManager.getInstence().onNewIntent(intent);
	}
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		SdkManager.getInstence().onActivityResult(requestCode, resultCode, data);
//		GpsLocation.GetInstance().onActivityResult(requestCode, resultCode, data);
	}
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
	{
		SdkManager.getInstence().onRequestPermissionsResult(requestCode, permissions, grantResults);
	}
	public void onWindowFocusChanged( boolean hasFocus )
	{
		SdkManager.getInstence().onWindowFocusChanged( hasFocus );
	}
	public void onConfigurationChanged(Configuration newConfig)
	{
		SdkManager.getInstence().onConfigurationChanged( newConfig );
	}

	public boolean dispatchTouchEvent(MotionEvent ev)
	{
		SdkManager.getInstence().dispatchTouchEvent(ev);
		return true;
	}	
}

