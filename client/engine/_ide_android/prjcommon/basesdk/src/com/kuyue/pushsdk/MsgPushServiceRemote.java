package com.kuyue.pushsdk;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.SystemClock;
import android.util.Log;
import android.widget.Toast;

public class MsgPushServiceRemote extends Service {	
	protected static final String TAG = MsgPushServiceRemote.class.getSimpleName();

	/*
    MyBinder myBinder;
	MyServiceConnection myServiceConnection;
	*/

	@Override
	public void onCreate() {
		/*
		if (myBinder == null) {
            myBinder = new MyBinder();
        }
        myServiceConnection = new MyServiceConnection();
        */
				
		super.onCreate();
	}

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
//        this.bindService(new Intent(this, MsgPushServiceLocal.class), myServiceConnection, Context.BIND_IMPORTANT);
        return START_STICKY;
    }

	@Override
	public IBinder onBind(Intent arg0) {
//		return myBinder;
		return null;
	}

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
    
    /*
    class MyServiceConnection implements ServiceConnection {

        @Override
        public void onServiceConnected(ComponentName arg0, IBinder arg1) {
            Log.i("castiel", "本地服务连接成功");
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            // 连接出现了异常断开了，LocalCastielService被杀死了
            Toast.makeText(MsgPushServiceRemote.this, "本地服务Local被干掉", Toast.LENGTH_LONG).show();

            // 启动LocalCastielService
            MsgPushServiceRemote.this.startService(new Intent(MsgPushServiceRemote.this, MsgPushServiceLocal.class));
            MsgPushServiceRemote.this.bindService(new Intent(MsgPushServiceRemote.this, MsgPushServiceLocal.class),
                    myServiceConnection, Context.BIND_IMPORTANT);
        }
    }

    class MyBinder extends MsgPushServiceBinder.Stub {
        @Override
        public String getProName() throws RemoteException {
            return "push_service_remote";
        }
    }        
    */
}
