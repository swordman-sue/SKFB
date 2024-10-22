package com.kuyue.pushsdk;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

//import cn.jpush.android.api.JPushInterface;

import com.kuyue.pushsdk.PushUtils;

/**
 * Push消息处理receiver
 */
public class PushMessageReceiver extends BroadcastReceiver {

	protected static final String TAG = PushMessageReceiver.class.getSimpleName();

	private static boolean isRunning = false;
	
	/**
	 * @param context
	 *            Context
	 * @param intent
	 *            接收的intent
	 */
	@Override
	public void onReceive(final Context context, Intent intent) {		
	    isRunning = PushUtils.isAppRunning(context);
	    Bundle bundle = intent.getExtras();

	    // 开机广播
	    if (intent.getAction().equals(PushUtils.ACTION_BOOT)) {
	    	Log.i(TAG, "ACTION_BOOT");	    	

	    	Intent startPushServiceIntent = new Intent(context, MsgPushServiceLocal.class);
	    	if (Cocos2dxHelper.getSDKVersion() >= Build.VERSION_CODES.O)
	    	{
	    		context.startForegroundService(startPushServiceIntent);
	    	}
	    	else
	    	{
		    	context.startService(startPushServiceIntent);	    		
	    	}
	         
	    // 注册广播
		} /*else if (JPushInterface.ACTION_REGISTRATION_ID.equals(intent.getAction())) {			
	    	String regId = bundle.getString(JPushInterface.EXTRA_REGISTRATION_ID);
	    	PushInfoManager appinfo = new PushInfoManager(context);
	    	appinfo.SetRegistrationId(regId);
	    	appinfo.SetPushStatus(PushUtils.INVOKE_START);
	    	Log.i(TAG, "ACTION_REGISTRATION_ID Registration ID: " + regId);	             
        } 
		
	    // 接收到推送下来的自定义消息
		else if (JPushInterface.ACTION_MESSAGE_RECEIVED.equals(intent.getAction())) {        
	    	Log.i(TAG, "ACTION_MESSAGE_RECEIVED");
        } 
		
	    // 接收到推送下来的通知
		else if(JPushInterface.ACTION_NOTIFICATION_RECEIVED.equals(intent.getAction())) {
    		Log.i(TAG, "ACTION_NOTIFICATION_RECEIVED");
			
	    	if(isRunning) {
	    		JPushInterface.clearAllNotifications(context);
	    		Log.i(TAG, "clear all notifications");
	    	}
	    	else {
				int notifactionId = bundle.getInt(JPushInterface.EXTRA_NOTIFICATION_ID);				
				Log.i(TAG, "ACTION_NOTIFICATION_RECEIVED Notifaction ID: " + notifactionId);	    		
	    	}		    
	    } 
		
	    // 用户点击打开了通知
		else if(JPushInterface.ACTION_NOTIFICATION_OPENED.equals(intent.getAction())) {
	    	PackageManager pm = context.getPackageManager();
	    	Intent startIntent = pm.getLaunchIntentForPackage(context.getPackageName());
			startIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
			context.startActivity(startIntent);
			Log.i(TAG, "ACTION_NOTIFICATION_OPENED");	    		
	    } */
		
	    // 定时广播
		else if(intent.getAction().equals(PushUtils.ACTION_TIMER)){
			Log.i(TAG, "ACTION_TIMER");

			PushInfoManager appinfo = new PushInfoManager(context);
			int currentTime = PushUtils.getCurrentTime();
			int task_no = appinfo.IsTimeToTask(currentTime);
			appinfo.KillTimerTask(currentTime);
			
			if(!isRunning && 0 != task_no) {
				PackageManager pm = context.getPackageManager();
				NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);  				
				String app_name = pm.getApplicationLabel(context.getApplicationInfo()).toString();

				Intent startIntent = pm.getLaunchIntentForPackage(intent.getPackage());
				startIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				PendingIntent pi = PendingIntent.getActivity(context, 0, startIntent, 0);

				Notification notification = new Notification.Builder(context)    
		         .setAutoCancel(true)    
		         .setTicker(app_name)
		         .setContentTitle(app_name)    
		         .setContentText(appinfo.GetTaskName(task_no))    
		         .setContentIntent(pi)    
		         .setSmallIcon(context.getApplicationInfo().icon)
		         .setWhen(System.currentTimeMillis())
		         .setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE)
		         .build();					
				
				nm.notify(Integer.valueOf(PushUtils.NOTIFY_CODE_TIMER), notification);
				appinfo.SetActivityStatus(appinfo.GetTaskTime(task_no));					
			} 
			else {
				appinfo.SetActivityStatus("0");
			}
		} 
		
//		else if (JPushInterface.ACTION_RICHPUSH_CALLBACK.equals(intent.getAction())) {
//            Log.i(TAG, "用户收到到RICH PUSH CALLBACK: " + bundle.getString(JPushInterface.EXTRA_EXTRA));
//            
//        } 
		
		else {
        	Log.d(TAG, "Unknown intent: " + intent.getAction());
        }
	}
}
