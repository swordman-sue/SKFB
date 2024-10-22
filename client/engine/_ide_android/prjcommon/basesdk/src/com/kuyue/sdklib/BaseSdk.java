package com.kuyue.sdklib;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.cocos2dx.lib.Cocos2dxRenderer;
import org.json.JSONException;
import org.json.JSONObject;

import com.kuyue.contant.SdkCommonHandleType;
import com.kuyue.contant.SdkContants;
import com.kuyue.contant.SdkKeyDataHandleType;
import com.kuyue.net.NetBroadcastReceiver;
import com.kuyue.pushsdk.PushUtils;
import com.kuyue.queue.MessageMagager;
import com.kuyue.queue.MessageQueues;
import com.tencent.bugly.crashreport.CrashReport;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public abstract class BaseSdk {	
	public static final String APP_NAME = "APP_NAME";
	public static final String APP_ID = "APP_ID";
	public static final String APP_KEY = "APP_KEY";
	public static final String CHANNEL_ID = "CHANNEL_ID";
	public static final String CHANNEL_ID_EX = "CHANNEL_ID_EX";
	
	protected static ArrayList<String> dynamicPermissionList = null;
	protected static ArrayList<String> toReqPermissionList = null;
	private static int toReqPermissionResultNum = 0;
	private static int toReqPermissionGrantedNum = 0;

	private static final String TAG = "BaseSdk";
	private static Activity activity = null;
	private static NetBroadcastReceiver netBCReceiver = null;
	private static Map<String,Object> properties = new HashMap<String,Object>();
			
	public BaseSdk(Activity act, int resId)
	{
		activity = act;
		properties.put(APP_ID, PlatformUtil.GetApplicationMetaData(APP_ID));
		properties.put(APP_KEY,  PlatformUtil.GetApplicationMetaData(APP_KEY));
		properties.put(CHANNEL_ID, PlatformUtil.GetApplicationMetaData(CHANNEL_ID));
		properties.put(CHANNEL_ID_EX, PlatformUtil.GetApplicationMetaData(CHANNEL_ID_EX));
		properties.put(APP_NAME,  PlatformUtil.GetResStringInfo(resId));
		
        // 监听网络切换
        IntentFilter filter = new IntentFilter();  
        filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        netBCReceiver = new NetBroadcastReceiver();
        activity.registerReceiver(netBCReceiver, filter);
        
        SetupDynamicPermissionList();
        CheckRequestDynamicPermissions();
	}

	public Activity GetActivity()
	{
		return activity;
	}

	public void onCreate(Bundle savedInstanceState){}
	
	public void onDestroy()
	{				    
		if (null != netBCReceiver) {
			activity.unregisterReceiver(netBCReceiver);
			netBCReceiver = null;
		}
	}
	
	public void onRestart(){}
	public void onPause(){}
	public void onResume(){}
	public void onStart(){}
	public void onStop(){}
	public void onSaveInstanceState(Bundle outState){}
	public void onNewIntent(Intent intent){}
	public boolean dispatchTouchEvent(MotionEvent ev){return false;}
	public void onLogoutResult(){}
	public boolean onKeyDown(int keyCode, KeyEvent event){return false;}
	public void onConfigurationChanged(Configuration newConfig){}
	public void onWindowFocusChanged(boolean hasFocus){}
	
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		Log.i(TAG,"requestCode:" + requestCode +  " resultCode:" + resultCode);		
		
		if (SdkContants.OPEN_SETTING_FOR_DYNAMIC_PERMISSION_CODE == requestCode)
		{
			CheckRequestDynamicPermissions();
		}
	}

	public void SDKInit()
	{
		SDKInitFinishCallBack(SdkContants.SUCCESS_CODE);
	}
	
	public void SDKLogin(String info){}
	
	public void SDKLogout(String info){}
	
	public void SDKSwitchAccount(String info){}
	
	public void SDKPay(final String info){}
	
	public void SDKCommonHandle(String info){}
	
	public void SDKToUserCenter(String info){}
	
	public void SDKCheckUpdate(String url)
	{
		PlatformUtil.GotoBrowser(url);
	}
	
	public void SDKExitApp(String info)
	{		
		PlatformUtil.ShowDlg(
				PlatformUtil.GetResStringInfo("zl_exit_info", "string"), 
				info, 
				PlatformUtil.GetResStringInfo("zl_exit_ok", "string"), 
				PlatformUtil.GetResStringInfo("zl_exit_cancel", "string"), 
				true);
	}

	public void SDKExitAppNotify(boolean isCustomSDK)
	{		
		//过滤游戏关闭过程中产生的崩溃
		CrashReport.enableBugly(false);

		//执行游戏关闭
		PlatformUtil.NativeCloseEngineInApp();
	}
	
	// 运营统计数据
	public void SDKKeyDataHandle(String info)
	{		
	}
		
	// 初始化相关回调
	public static native void SDKInitFinishCallBack(int code);

	// 登陆相关回调
	public static void SDKLoginPanelCallBack(int code,String data)
	{
		MsgSdkData msgdata = new MsgSdkData(code,data,MsgType.LOGIN);
		MessageQueues.Add(MessageMagager.MSG_SDK_TOOLS, msgdata);
	}
	public static void SDKLoginPanelCallBack(int code)
	{
		SDKLoginPanelCallBack(code,"");
	}
	
	// 支付相关回调
	public static void SDKPayFinishCallBack(int code,String data)
	{
		MsgSdkData msgdata = new MsgSdkData(code,data,MsgType.PAY);
		MessageQueues.Add(MessageMagager.MSG_SDK_TOOLS, msgdata);
	}
	public static void SDKPayFinishCallBack(int code)
	{
		SDKPayFinishCallBack(code,"");
	}

	// 通用操作相关回调
	public static void SDKCommonHandleCallBack(int code,String data)
	{
		MsgSdkData msgdata = new MsgSdkData(code,data,MsgType.COMMON);
		MessageQueues.Add(MessageMagager.MSG_SDK_TOOLS, msgdata);
	}
	public static void SDKCommonHandleCallBack(int code)
	{
		SDKCommonHandleCallBack(code,"");
	}
			
	public Object GetPropertie(String key)
	{
		if (null == key || key.isEmpty())
		{
			Log.e(TAG, "key is empty when call GetPropertie() method");
            return null;
		}
		return properties.get(key);
	}
	
	public void SetPropertie(String key,Object value)
	{
		if (null == key || key.isEmpty() || null == value)
		{
			Log.e(TAG, "value is empty when call SetPropertie() method");
            throw new IllegalArgumentException();
		}
		properties.put(key, value);
	}	
	
	/*
	 * 初始化需要动态申请的权限
	 */
	protected void SetupDynamicPermissionList()
	{		
		if (Cocos2dxHelper.getSDKVersion() < 23)
		{
			return;
		}
		
		if (null != dynamicPermissionList)
		{
			return;
		}
		
		dynamicPermissionList = new ArrayList<String>();
		dynamicPermissionList.add(Manifest.permission.READ_PHONE_STATE);
		dynamicPermissionList.add(Manifest.permission.READ_EXTERNAL_STORAGE);
	}
	
	/*
	 * 动态申请权限
	 */
	private void CheckRequestDynamicPermissions()
	{		
		if (null == dynamicPermissionList)
		{
			return;
		}
		
		toReqPermissionList = null;

		String toReqPermission;
		for (int i = 0; i < dynamicPermissionList.size(); ++i)
		{
			toReqPermission = dynamicPermissionList.get(i);
			int permissionCheck = ContextCompat.checkSelfPermission(GetActivity(), toReqPermission);
	        if (permissionCheck != PackageManager.PERMISSION_GRANTED) 
	        {
	        	if (null == toReqPermissionList)
	        	{
	        		toReqPermissionList = new ArrayList<String>();
	        	}
	        	toReqPermissionList.add(toReqPermission);
	        }               	
		}
		
		if (null != toReqPermissionList)
		{
			Cocos2dxRenderer.canDrawFrame.set(false);
			toReqPermissionResultNum = 0;
			toReqPermissionGrantedNum = 0;
			ActivityCompat.requestPermissions(GetActivity(), (String[])toReqPermissionList.toArray(new String[toReqPermissionList.size()]), SdkContants.DYNAMIC_PERMISSION_CODE);
		}
		else
		{
			Cocos2dxRenderer.canDrawFrame.set(true);			
		}
	}

	/*
	 * 权限设置回调
	 */
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
	{
		if (SdkContants.DYNAMIC_PERMISSION_CODE != requestCode)
		{
			return;
		}
		
		//记录权限设置结果
		for (int i = 0; i < grantResults.length; ++i)
		{			
			++toReqPermissionResultNum;
			if (grantResults[i] == PackageManager.PERMISSION_GRANTED)
			{
				++toReqPermissionGrantedNum;
			}
			
			toReqPermissionList.remove(toReqPermissionList.size() - 1);
		}
				
		if (toReqPermissionList.size() <= 0)
		{			
			if (toReqPermissionGrantedNum >= toReqPermissionResultNum)
			{
				//全部允许
				Cocos2dxRenderer.canDrawFrame.set(true);				
			}
			else
			{
				//非全部允许
				ShowRequestPermissionsDlg();
			}
		}
	}

	/*
	 * 打开应用设置弹窗
	 */
	private void ShowRequestPermissionsDlg()
	{
		String content = PlatformUtil.GetResStringInfo("zl_request_permissions", "string").toString();
		String positive_text = PlatformUtil.GetResStringInfo("zl_setting", "string").toString();
		String negative_text = PlatformUtil.GetResStringInfo("zl_cancel", "string").toString();
		AlertDialog.Builder ab = new AlertDialog.Builder(activity);
		ab.setCancelable(false);
		ab.setMessage(content);
		ab.setPositiveButton(positive_text, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				Uri uri = Uri.fromParts("package", activity.getPackageName(), null);
				Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS, uri);
				activity.startActivityForResult(intent, SdkContants.OPEN_SETTING_FOR_DYNAMIC_PERMISSION_CODE);
			}
		});
		ab.show();		
	}
}
