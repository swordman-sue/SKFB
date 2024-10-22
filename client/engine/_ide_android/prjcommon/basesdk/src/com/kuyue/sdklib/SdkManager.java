package com.kuyue.sdklib;

import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Map;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.kuyue.gps.GpsLocation;
import com.kuyue.queue.MessageMagager;
import com.kuyue.queue.MessageQueues;

public class SdkManager {	
	private static final String TAG = "SdkManage";
	
	private static BaseSdk sdk = null;
//	private static GpsLocation gps = null;
	private static Activity activity = null;
	private static SdkManager sdkManager = null;
	private static final int SDK_LOGIN = 2;	
	private static final int SDK_PAY = 3;
	private static final int TO_USER_CENTER = 4;
	private static final int COMMON_HANDLE = 5;	
	private static final int SDK_CHECK_UPDATE = 6;
	private static final int SDK_EXIT_APP = 7;
	private static final int SDK_LOGOUT = 8;
	private static final int SDK_SWITCH_ACCOUNT = 9;
	private static final int KEY_DATA_HANDLE = 10;

	private static final Map<String,Integer> methodMap = new HashMap<String,Integer>();
	
	static
	{
		methodMap.put("SDK_LOGIN", SDK_LOGIN);
		methodMap.put("SDK_PAY", SDK_PAY);
		methodMap.put("TO_USER_CENTER", TO_USER_CENTER);
		methodMap.put("COMMON_HANDLE", COMMON_HANDLE);
		methodMap.put("SDK_CHECK_UPDATE", SDK_CHECK_UPDATE);
		methodMap.put("SDK_EXIT_APP", SDK_EXIT_APP);
		methodMap.put("SDK_LOGOUT", SDK_LOGOUT);		
		methodMap.put("SDK_SWITCH_ACCOUNT", SDK_SWITCH_ACCOUNT);
		methodMap.put("KEY_DATA_HANDLE", KEY_DATA_HANDLE);
	}
	
	private SdkManager(Activity act,Class<?> clazz)
	{
		activity = act;
		//创建sdk回调消息队列
		MessageQueues.CreateVector(MessageMagager.MSG_SDK_TOOLS);

		if (null == activity || null == clazz)
		{
			Log.e(TAG, "activity or class is null when call CreateSdk method");
			return;
		}
		try {
			
			Constructor<?> constractor = clazz.getConstructor(Activity.class);
			
			sdk = (BaseSdk)constractor.newInstance(activity);

		}catch(Exception e)
		{
			Log.e(TAG, "create sdk fail ,sdk name is :" + clazz.getName());
			e.printStackTrace();
		}
		
		if (null != sdk)
		{
			try {
				sdk.SDKInit();
			} catch (Exception e) {
				Log.e(TAG," init sdk failed " + e.getMessage());
				e.printStackTrace();
			}
		}else
		{
			Log.e(TAG, "sdk is null ");
		}
	}

	public void onCreate(Bundle savedInstanceState)
	{
		if(null != sdk)
		{
			sdk.onCreate(savedInstanceState);
		}	
	}
	public void onDestroy()
	{
		if(null != sdk)
		{
			sdk.onDestroy();
		}	
	}
	public void onRestart()
	{
		if(null != sdk)
		{
			sdk.onRestart();
		}	
	}	
	public void onPause()
	{
		if(null != sdk)
		{
			sdk.onPause();
		}	
	}
	public void onResume()
	{
		if(null != sdk)
		{
			sdk.onResume();
		}	
	}
	public void onStart()
	{
		if(null != sdk)
		{
			sdk.onStart();
		}	
	}
	public void onStop()
	{
		if(null != sdk)
		{
			sdk.onStop();
		}	
	}

	public void SDKExitAppNotify(boolean isCustomSDK)
	{
		if(null != sdk)
		{
			sdk.SDKExitAppNotify(isCustomSDK);
		}
	}

	public static SdkManager CreateSdk(Activity activity,Class<?> clazz)
	{
		 if (null == sdkManager)
		 {
			 sdkManager = new SdkManager(activity,clazz);
		 }
		 return sdkManager;
	}
	
	public static SdkManager getInstence()
	{
		if (null == sdkManager)
		{
			Log.e(TAG, "SdkManager not init");
		}
		return sdkManager;
	}
	
	public static BaseSdk GetSdk()
	{
		if (null == sdk) 
		{
			Log.e(TAG, "Sdk not create");
		}
		return sdk;
	}
	
	
	public static void InvokeVoidMethod(String methodName,String param)
	{
		if (null == methodName || methodName.isEmpty())
		{
			Log.e(TAG, "InvokMethod method name is empty");
			return ;
		}
		Message msg = handler.obtainMessage();
		msg.what = methodMap.get(methodName);
		msg.obj = param;
		handler.sendMessage(msg);
	}

	private static Handler handler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			 String param_str = "";
			if (null != msg.obj)
			{
				param_str =  msg.obj.toString();
			}else
			{
				Log.w(TAG, "method param is empty");
			}
			switch (msg.what) {
				case SDK_LOGIN:
					sdk.SDKLogin(param_str);
					break;
				case SDK_PAY:
					sdk.SDKPay(param_str);
					break;
				case SDK_CHECK_UPDATE:
					sdk.SDKCheckUpdate(param_str);
					break;
				case SDK_EXIT_APP:
					sdk.SDKExitApp(param_str);
					break;
				case TO_USER_CENTER:
					sdk.SDKToUserCenter(param_str);
					break;
				case COMMON_HANDLE:
					sdk.SDKCommonHandle(param_str);
					break;
				case SDK_LOGOUT:
					sdk.SDKLogout(param_str);
					break;
				case SDK_SWITCH_ACCOUNT:
					sdk.SDKSwitchAccount(param_str);
					break;
				case KEY_DATA_HANDLE:
					sdk.SDKKeyDataHandle(param_str);
					break;
				default:
					break;
			}
		};
	};
	
	public static Object GetPropertie(String proName)
	{
		return sdk.GetPropertie(proName);
	}
	
	public void onLogoutResult()
	{
		if(null != sdk)
		{
			sdk.onLogoutResult();
		}	
	}	
	
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if(null != sdk)
		{
			sdk.onKeyDown( keyCode , event );
		}	
		return false;
	}	
	
	public void onSaveInstanceState(Bundle outState) {
		if(null != sdk)
		{
			sdk.onSaveInstanceState(outState);
		}
	}
	
	public void onNewIntent(Intent intent)
	{
		if(null != sdk)
		{
			sdk.onNewIntent(intent);
		}	
	}	
	
	public void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		if(null != sdk)
		{
			sdk.onActivityResult(requestCode, resultCode, data);
		}
	}
	
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
	{
		if(null != sdk)
		{
			sdk.onRequestPermissionsResult(requestCode, permissions, grantResults);
		}
	}
	
	public void onWindowFocusChanged(boolean hasFocus)
	{
		if(null != sdk)
		{
			sdk.onWindowFocusChanged( hasFocus );
		}
	}
	
	public void onConfigurationChanged(Configuration newConfig)
	{
		if(null != sdk)
		{
			sdk.onConfigurationChanged( newConfig );
		}
	}
	
	public boolean dispatchTouchEvent( MotionEvent ev )
	{
		return true;
	}
	
}


