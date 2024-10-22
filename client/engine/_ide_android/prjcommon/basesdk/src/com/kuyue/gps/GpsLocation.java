package com.kuyue.gps;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;

import com.kuyue.contant.SdkContants;
import com.kuyue.queue.MessageData;
import com.kuyue.queue.MessageInterface;
import com.kuyue.queue.MessageMagager;
import com.kuyue.queue.MessageQueues;
import com.kuyue.sdklib.PlatformUtil;

public class GpsLocation implements LocationListener,MessageInterface{
	
	private static final String  TAG = "GpsLocation";
	private static Activity activity = null;
	private static GpsLocation gps= null;
	private  LocationManager locationManager;
	private  Location currentLocation = null;
	private  boolean isUsefulGps = true;
	private  boolean isUsefulNetWork = true;
	private  int gpsStatus = LocationProvider.AVAILABLE;
	private  int networkStatus = LocationProvider.AVAILABLE;
	private  boolean isListern = false;
	private  Timer timer = null;
	private  MsgLocationData  msgdata = null;
	 
	class MyTimerTask extends TimerTask{
		@Override
		public void run() {
			
			if(null == currentLocation)
			{
				onLocationChangedCallback(-2,"{\"msg\":\"定位超时\"}");
				cancelTimer();
			}
		}
	 }
	
	private GpsLocation()
	{
		MessageQueues.CreateVector(MessageMagager.MSG_LOCATION);
	}
	
	public static void initContext(Activity act)
	{
		activity = act;
	}
	
	public static GpsLocation GetInstance()
	{
		if (null == gps)
		{
			gps = new GpsLocation();
		}
		return gps;
	}

	//位置改变后回调
	protected  void onLocationChangedCallback(int status,String localJson)
	{
		if(null != msgdata)
		{
			try{
				msgdata.setCode(status);
				msgdata.setMsg(localJson);
				MessageQueues.Add(MessageMagager.MSG_LOCATION, msgdata);
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		}
	}
	
	public boolean checkSetting()
	{
		if (!PlatformUtil.IsOpenLocationService())
		{
			AlertDialog.Builder ab = new AlertDialog.Builder(activity);
			ab.setMessage("没有开启定位,请开启定位设置");
			ab.setPositiveButton("设置", new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int which) {
					Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
					activity.startActivityForResult(intent,SdkContants.ACT_RESULT_GPS_CODE); //此为设置完成后返回到获取界面
				}
			});
			ab.setNegativeButton("取消", new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int whith) {
					onLocationChangedCallback(-1,"{\"msg\":\"定位服务未打开\"}");
				}
			});
			ab.show();
			return false;
		}else
		{
			return true;
		}
	}
	
	public void onDestroy(){
		onStopLocation();
	}
	
	public void onActivityResult(int requestCode, int resultCode, Intent data){
		if(requestCode==SdkContants.ACT_RESULT_GPS_CODE)
		{
			//onStartLocation(msgdata.getTimeout());
		}
	}
	
	public final void onStartLocation(final MsgLocationData msg)
	{
		try{
			if( null == activity)
			{
				Log.e(TAG,"GpsLocation activity is null");
				return ;
			}
			if(!checkSetting())
			{
				return ;
			}
			if(isListern)
			{
				return ;
			}
			isListern = true;
			this.msgdata = msg;
			int timeout = msg.getTimeout();
			
			//定时timeout
			startTimer(timeout);
	 
			locationManager = (LocationManager) activity.getSystemService(Context.LOCATION_SERVICE);
			
			if(null == locationManager)
			{
				return;
			}
			String networkProvider = locationManager.getProvider(LocationManager.NETWORK_PROVIDER).getName();
			
			String gpsProvider = locationManager.getProvider(LocationManager.GPS_PROVIDER).getName();
			//监听网络定位
			locationManager.requestLocationUpdates(networkProvider, 5,50,this);
			//监听GPS定位
			locationManager.requestLocationUpdates(gpsProvider, 5, 50,this);
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public final void startTimer(int timeout)
	{
		try{
			this.cancelTimer();
			timer =  new Timer();
			timer.schedule(new MyTimerTask(), timeout * 1000);
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public final void cancelTimer()
	{
		if(null != timer)
		{
			timer.cancel();
			timer = null;
		}
	}
	
	public final void onStopLocation()
	{
		try{
			this.cancelTimer();
			if(null != locationManager)
			{
				locationManager.removeUpdates(this);
			}
			locationManager = null;
			isListern = false;
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}

	@Override
	public void onLocationChanged(final Location location) {
		
		currentLocation = location;

		if (null != currentLocation){
			final String locatJson = "{\"latitude\":"+currentLocation.getLatitude()+",\"longitude\":"+currentLocation.getLongitude()+"}";
			onLocationChangedCallback(0,locatJson);
		}
	}

	@Override
	public void onProviderDisabled(String provider) {
		
		if(LocationManager.GPS_PROVIDER.equals(provider))
		{
			isUsefulGps = false;
		}else if (LocationManager.NETWORK_PROVIDER.equals(provider))
		{
			isUsefulNetWork = false;
		}
		//GPS定位和网络定位都关闭
		if(!isUsefulGps && !isUsefulNetWork)
		{
			onLocationChangedCallback(-1,"{\"msg\":\"定位服务已关闭\"}");
			return;
		}
	}

	@Override
	public void onProviderEnabled(String provider) {
		
		if(LocationManager.GPS_PROVIDER.equals(provider))
		{
			isUsefulGps = true;
		}else if (LocationManager.NETWORK_PROVIDER.equals(provider))
		{
			isUsefulNetWork = true;
		}
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		// TODO Auto-generated method stub
		if(LocationManager.GPS_PROVIDER.equals(provider))
    	{
    		gpsStatus = status;
    	}else if(LocationManager.NETWORK_PROVIDER.equals(provider))
    	{
    		networkStatus = status;
    	}
	}
	

	private  Handler handler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
				case LocationOpType.START:
					onStartLocation((MsgLocationData)msg.obj);
					break;
				case LocationOpType.STOP:
					onStopLocation();
					break;
				default:
					break;
			}
		};
	};

	@Override
	public boolean Add(MessageData msg) {
		boolean res = true;
		try{
			MsgLocationData msgData = (MsgLocationData) msg;
			Message message = handler.obtainMessage();
			message.what = msgData.getOpType();
			message.obj = msgData;
			handler.sendMessage(message);
		}catch(Exception e)
		{
			e.printStackTrace();
			res = false;
		}
		return res;
	}
}