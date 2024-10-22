package com.kuyue.pushsdk;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.os.SystemClock;

public class MsgPushServiceLocal extends Service {	
	protected static final String TAG = MsgPushServiceLocal.class.getSimpleName();

	@Override
	public void onCreate() {		
		super.onCreate();
				
    	if (Cocos2dxHelper.getSDKVersion() >= Build.VERSION_CODES.O)
    	{
    		NotificationChannel channel = new NotificationChannel(PushUtils.NOTIFY_CODE_PUSH_SERVICE, null, NotificationManager.IMPORTANCE_HIGH);

		    NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		    manager.createNotificationChannel(channel);
		
		    Notification notification = new Notification.Builder(getApplicationContext(), PushUtils.NOTIFY_CODE_PUSH_SERVICE).build();
		    startForeground(Integer.valueOf(PushUtils.NOTIFY_CODE_PUSH_SERVICE), notification);
    	}		
	 		
    	//创建定时广播
		Intent intent = new Intent(this, PushMessageReceiver.class);
		intent.setAction(PushUtils.ACTION_TIMER);
		intent.setPackage(this.getPackageName());
		PendingIntent sender = PendingIntent.getBroadcast(this, 0, intent, 0);

		//启动定时器
		long startTime = SystemClock.elapsedRealtime();
		AlarmManager alarmManager = (AlarmManager) getSystemService(Context.ALARM_SERVICE);
		alarmManager.setRepeating(AlarmManager.ELAPSED_REALTIME_WAKEUP, startTime, PushUtils.ACTION_TIMER_INTERVAL * 1000, sender);
	}

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
