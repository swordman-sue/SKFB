package com.kuyue.sdklib;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Currency;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicStampedReference;

import org.apache.http.conn.util.InetAddressUtils;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import com.kuyue.contant.SdkCommonHandleFuncType;
import com.kuyue.contant.SdkContants;
import com.kuyue.sdk.Sdk;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.database.Cursor;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.provider.MediaStore;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.os.Environment;

public class PlatformUtil
{
	private static final String TAG = "PlatformUtil";
	
	private static Activity m_activity = null;
	
	private static Dialog _dlg = null;
	
	private static AtomicBoolean m_network_available;
		
	public static void SetContext(Activity activity)
	{
		m_activity = activity;
		m_network_available = new AtomicBoolean(true);
	}
	
	public static String GetDeviceModel()
	{
		if (null == android.os.Build.MODEL)
		{
			return "";
		}
		return android.os.Build.MODEL;
	}
	
	public static void ShowDlgMainLoop(final String str_title,final String str_tips,final String str_certain,final String str_cancel,final boolean is_exit_app)
	{
		Looper looper = Looper.getMainLooper();
		Handler messageHandler = new Handler(looper)
		{
			@Override
			public void handleMessage(Message msg) 
			{
				ShowDlg(str_title, str_tips, str_certain, str_cancel, is_exit_app);
			}

		};
		messageHandler.sendEmptyMessage(1);
	}
	
	public synchronized static void ShowDlg(String str_title,String str_tips,String str_certain,String str_cancel,final boolean is_exit_app)
	{
		if(_dlg != null)
		{
			return;
		}
		AlertDialog.Builder builder = new Builder(m_activity);
		builder.setTitle(str_title);
		builder.setMessage(str_tips);
		builder.setPositiveButton(str_certain, new OnClickListener() 
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				((Cocos2dxActivity)m_activity).runOnGLThread(new Runnable() {					
					@Override
					public void run() {
						if (!is_exit_app)
						{
							DlgCertainCallBack();
						}
						else
						{
							// 关闭游戏
							Sdk.getInstance().SDKExitAppNotify(true);
						}						
					}
				});
				
				dialog.dismiss();
				_dlg = null;
			}}
		);
		
		builder.setNegativeButton(str_cancel, new OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				if (!is_exit_app)
				{
					((Cocos2dxActivity)m_activity).runOnGLThread(new Runnable() {						
						@Override
						public void run() {
							DlgCancelCallBack();							
						}
					});					
				}

				dialog.dismiss();
				_dlg = null;
			}}
		);
		
		Dialog dlg =  builder.create();
		_dlg = dlg;
		dlg.setCancelable(false);
		dlg.show();
	}
	
	//获取版本号(1.0.0)
	public static String GetAppVersion()
	{
		String version = "";
		try
		{
			PackageManager packageManager = m_activity.getPackageManager();
			PackageInfo packInfo = packageManager.getPackageInfo(m_activity.getPackageName(), PackageManager.GET_CONFIGURATIONS);
			version = packInfo.versionName;			
		}
		catch (Exception e)
		{			
			e.printStackTrace();
		}
		return version;
	}
	
	//获取版本号(100)
	public static String GetAppVersionCode()
	{
		String version = "";
		try
		{
			PackageManager packageManager = m_activity.getPackageManager();
			PackageInfo packInfo = packageManager.getPackageInfo(m_activity.getPackageName(), PackageManager.GET_CONFIGURATIONS);
			version = String.valueOf(packInfo.versionCode);			
		}
		catch (Exception e)
		{			
			e.printStackTrace();
		}
		return version;
	}
	
	public static String GetPackageName()
	{
		return m_activity.getPackageName();
	}
	
	public static PackageInfo GetPackageInfo()
	{
		PackageInfo packInfo = null;
		try
		{
			PackageManager packageManager = m_activity.getPackageManager();
			packInfo = packageManager.getPackageInfo(m_activity.getPackageName(), PackageManager.GET_CONFIGURATIONS);
		}
		catch (Exception e)
		{			
			e.printStackTrace();
		}
		return packInfo;
	}

	public static String GetDeviceId()
	{
		String id = "";
		if (null == m_activity)
		{
			Log.e("PlatformUtil", "未初始化PlatformUtil的activity");
			return id;
		}
		
		TelephonyManager telephonyManager = (TelephonyManager)m_activity.getSystemService(Context.TELEPHONY_SERVICE);
		String imei = telephonyManager.getDeviceId();
		String serialNumber = android.os.Build.SERIAL;
		id = (imei != null ? imei : "") + (serialNumber != null ? serialNumber : "");
		if (id.isEmpty())
		{
			id = Settings.System.getString(m_activity.getContentResolver(), Settings.System.ANDROID_ID);
		}
		
		return id;
	}

	public static String GetImsi()
	{
		if (null == m_activity)
		{
			Log.e("PlatformUtil", "未初始化PlatformUtil的activity");
			return "";
		}
		TelephonyManager telephonyManager=(TelephonyManager) m_activity.getSystemService(Context.TELEPHONY_SERVICE);
		if (telephonyManager.getSubscriberId() != null)
		{
			return  telephonyManager.getSubscriberId();
		}
		return "";
	}

	public static boolean IsNetworkAvailable() 
	{
	    ConnectivityManager cm = (ConnectivityManager) m_activity.getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);

	    NetworkInfo info = cm.getActiveNetworkInfo();
	    
	    return  info != null && info.getState() == NetworkInfo.State.CONNECTED;   	
	}
	
	/**   
     * 判断GPS是否开启，GPS或者AGPS开启一个就认为是开启的    
     * @return true 表示开启   
     */   
    public static boolean IsOpenLocationService() 
    {   
    	if (null == m_activity)
    	{
    		return false;
    	}
    	LocationManager locationManager = (LocationManager) m_activity.getSystemService(Context.LOCATION_SERVICE);
        if (null == locationManager)
        {
        	return false;
        }
    	// 通过GPS卫星定位，定位级别可以精确到街（通过24颗卫星定位，在室外和空旷的地方定位准确、速度快）    
        boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);    
        // 通过WLAN或移动网络(3G/2G)确定的位置（也称作AGPS，辅助GPS定位。主要用于在室内或遮盖物（建筑群或茂密的深林等）密集的地方定位）    
        boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);    
        return gps || network; 
     }
	
	public static String GetApplicationMetaData(String metaName)
	{
		Object metaData = "";
		
		if (null == metaName || metaName.isEmpty())
		{
			Log.e(TAG, "metaName is empty when call GetApplicationMetaData");
			return "";
		}
		try {
			ApplicationInfo info = m_activity.getPackageManager().getApplicationInfo(m_activity.getPackageName(), PackageManager.GET_META_DATA);
			metaData = info.metaData.get(metaName);
			//是否有引用
			if(null != metaData && metaData.toString().contains("ref="))
			{
				String newMetaName = metaData.toString().split("=")[1];
				metaData = GetApplicationMetaData(newMetaName);
			}
		} catch (NameNotFoundException e) {
			Log.e(TAG, "must config application meta data : " + metaName);
		}
		if(null == metaData)
		{
			metaData = "";
		}
		return metaData.toString();
	}
	
	public static String GetActivityMetaData(String metaName)
	{
		String metaData = "";
		
		if (null == metaName || metaName.isEmpty())
		{
			return metaData;
		}
		try {
			ActivityInfo info = m_activity.getPackageManager()
			        .getActivityInfo(m_activity.getComponentName(),PackageManager.GET_META_DATA);
			metaData = info.metaData.getString(metaName);
			//是否有引用
			if(null != metaData && metaData.toString().contains("ref="))
			{
				String newMetaName = metaData.toString().split("=")[1];
				metaData = GetActivityMetaData(newMetaName);
			}
		} catch (NameNotFoundException e) {
			e.printStackTrace();
		}
		if(null == metaData)
		{
			metaData = "";
		}
		return metaData;
	}
		
	public static String GetResStringInfo(int resId)
	{
		return m_activity.getApplicationContext().getString(resId);
	}
	
	public static String GetResStringInfo(String resName, String type)
	{
		int resID = m_activity.getResources().getIdentifier(resName, type, m_activity.getPackageName());
		return m_activity.getApplicationContext().getString(resID);
	}		
	
	public static String GetProductName()
	{
		return GetPackageInfo().applicationInfo.processName;
	}
	
	public static boolean isEndWith(final String src_str, final String end_str)
	{
		if(null == src_str || null == end_str)
		{
			Log.e(TAG, " src_str or end_str is null when call method isEndWith");
			return false;
		}
		return src_str.endsWith(end_str);
	}
	
	public static String GetAppPath()
	{
		String path = m_activity.getApplicationContext().getFilesDir().getParent();
		if(!isEndWith(path,"/"))
		{
			path = path+"/";
		}
		return path;
	}
	
	public static String GetAppPackPath()
	{
		 return m_activity.getApplicationContext().getPackageCodePath();
	}
	
	public static String GetWritablePath()
	{
		String path = m_activity.getApplicationContext().getFilesDir().getAbsolutePath();
		if(!isEndWith(path,"/"))
		{
			path = path+"/";
		}
		return path;
	}

	public static String GetApkExpansionPath()
	{
		final String EX_PATH = "/Android/obb/";
		final String EX_TYPE = "main.";
		//判断外部存储(sd卡)是否可用
		if(!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
		{
			Log.d(TAG,"extenal Storage cant be used!");
			return "";
		}
		
		Log.d(TAG,"state:" + Environment.getExternalStorageState());
		File root =  Environment.getExternalStorageDirectory();
		String packageName = m_activity.getPackageName();
		File expPath = new File(root.toString() + EX_PATH + packageName);
		if(!expPath.exists())
		{
			Log.d(TAG,"path:" + expPath.toString() + " is not exist");
			return "";
		}
		String versionCode = "";
		try {
			versionCode = GetApplicationMetaData("OBB_VERSION");
			if (versionCode.isEmpty())
			{
				PackageInfo pi = m_activity.getPackageManager().getPackageInfo(packageName, PackageManager.GET_META_DATA);
				versionCode = String.valueOf(pi.versionCode);				
			}
		} catch (NameNotFoundException e) {
			Log.e(TAG,"packageName not found:" + packageName);
		}
		
		File expFile = new File(expPath.toString() + File.separator + EX_TYPE + versionCode + "." + packageName + ".obb");
		if(expFile.exists())
			return expFile.toString();
		return "";
	}
	
	public static void CheckNetWork(String content, String positive_text, String negative_text)
	{
		if (false == IsNetworkAvailable())
		{			
			content = (content != null && content != "") ? content : GetResStringInfo("zl_bad_network", "string").toString();
			positive_text = (positive_text != null && positive_text != "") ? positive_text : GetResStringInfo("zl_setting", "string").toString();
			negative_text = (negative_text != null && negative_text != "") ? negative_text : GetResStringInfo("zl_cancel", "string").toString();
			AlertDialog.Builder ab = new AlertDialog.Builder(m_activity);
			ab.setCancelable(false);
			ab.setMessage(content);
			ab.setPositiveButton(positive_text, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int which) {
					Intent intent = new Intent(Settings.ACTION_SETTINGS);
					m_activity.startActivityForResult(intent, 0);
				}
			});
			ab.setNegativeButton(negative_text, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int whith) {
					System.exit(0);
				}
			});
			ab.show();
		}
	}

	public static void CheckNetWorkAsync(String content, String positive_text, String negative_text)
	{
		Looper looper=Looper.getMainLooper();
		Handler messageHandler=new Handler(looper)
		{
			@Override
			public void handleMessage(Message msg) 
			{
				 String param_str = msg.obj.toString();
				 String[] param_list = param_str.split("@");
				 if (param_list.length == 3)
				 {
					 CheckNetWork(param_list[0], param_list[1], param_list[2]);
				 }
				 else
				 {
					 CheckNetWork(null, null, null);					 
				 }
			}
		};
		Message msg = messageHandler.obtainMessage();
		msg.obj = content + "@" + positive_text + "@" + negative_text;
		messageHandler.sendMessage(msg);	
	}
		
	/***
	 * open browser
	 * @author Administrator
	 * @param url 跳转地址
	 */
	public static void GotoBrowser(String url)
	{
		if (null == url || url.isEmpty())
		{
			Log.e(TAG, "url is empty when call method GotoBrowser");
			return ;
		}
		Intent intent = new Intent();
		intent.setAction("android.intent.action.VIEW");
		Uri content_url = Uri.parse(url);
		intent.setData(content_url);
		m_activity.startActivity(intent);
	}
	
	/***
	 * 
	 * @return (0 无，1 WIFI，2 2G网络，3 3G网络，4 4G网络，5 其他)
	 */
	public static int GetNetWorkState()
	{		
		int netType = 0;
		
		Context context = m_activity.getApplicationContext();
		ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);		
		if (null == connectivityManager)
		{
			return netType;
		}		
		NetworkInfo activeNetInfo = connectivityManager.getActiveNetworkInfo();
		if (null == activeNetInfo)
		{
			return netType;
		}
		if (!activeNetInfo.isAvailable())
		{
			return netType;
		}
		
		int srcNetType = activeNetInfo.getType();
		//获取网络的连接情况
		if (srcNetType == ConnectivityManager.TYPE_WIFI)
		{
			// WIFI
			netType = 1;
		}
		else if(srcNetType == ConnectivityManager.TYPE_MOBILE) 
		{
			// 移动数据
           int nSubType = activeNetInfo.getSubtype();  
           TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);  
            if (nSubType == TelephonyManager.NETWORK_TYPE_LTE && !telephonyManager.isNetworkRoaming()) {  
                //4G  
                netType = 4;  
                
            } else if ((nSubType == TelephonyManager.NETWORK_TYPE_UMTS  
                    || nSubType == TelephonyManager.NETWORK_TYPE_HSDPA  
                    || nSubType == TelephonyManager.NETWORK_TYPE_EVDO_0) 
                    && !telephonyManager.isNetworkRoaming()) {  
               //3G   联通的3G为UMTS或HSDPA 电信的3G为EVDO  
                netType = 3;  
                
            } else if ((nSubType == TelephonyManager.NETWORK_TYPE_GPRS  
                    || nSubType == TelephonyManager.NETWORK_TYPE_EDGE  
                    || nSubType == TelephonyManager.NETWORK_TYPE_CDMA)  
                    && !telephonyManager.isNetworkRoaming()) {  
                //2G 移动和联通的2G为GPRS或EGDE，电信的2G为CDMA  
                netType = 2;  
                
           } else {  
                netType = 2;   
           }  
		}
		else
		{
			netType = 5;			
		}
		
		return netType;
	}	
	
	public static int GetMemorySize()
	{		
		ActivityManager am = (ActivityManager) m_activity.getSystemService(Context.ACTIVITY_SERVICE);
		MemoryInfo mi = new MemoryInfo();
	    am.getMemoryInfo(mi);

		return (int)(mi.availMem / 1048576);	//MB
	}
	
	public static String readAssertFileWithKey(String file_name, String key)
	{
		String web_str = "";
		Context context = m_activity.getApplicationContext();
		
		try{
			InputStream input_stream = context.getAssets().open(file_name);
			
			InputStreamReader inputStreamReader = new InputStreamReader(input_stream);
			
			BufferedReader reader = new BufferedReader(inputStreamReader);
			
			String tempString = null;
			
			while((tempString = reader.readLine())!= null){
			
				if(tempString.contains(key)){
				
					
					web_str = tempString.substring(tempString.indexOf("\"")+1, tempString.lastIndexOf("\""));
					break;
				}			
			}		
			reader.close();
		}
		catch(IOException e)
		{
			Log.e(TAG,e.getMessage());
		}
		
		return web_str;
	}

	public static int readConfigIntTypeWithKey(String file_name, String key)
	{
		String web_str = "0";
		Context context = m_activity.getApplicationContext();
		
		try{
			InputStream input_stream = context.getAssets().open(file_name);
			
			InputStreamReader inputStreamReader = new InputStreamReader(input_stream);
			
			BufferedReader reader = new BufferedReader(inputStreamReader);
			
			String tempString = null;
			
			while((tempString = reader.readLine())!= null){
			
				if(tempString.contains(key)){
				
					web_str = tempString.substring(tempString.indexOf("=")+1, tempString.indexOf(","));
					break;
				}			
			}		
			reader.close();
		}
		catch(IOException e)
		{
			Log.e(TAG,e.getMessage());
		}
		
		return Integer.parseInt(web_str.trim());
	}
	
	public static boolean CopyAssertFile(String file_name, String local_file_name)
	{
		boolean read_ret = true;
		Context context = m_activity.getApplicationContext();
		
		try
		{
			//从assert中读取文件
			InputStream input_stream = context.getAssets().open(file_name);
			int file_length  = input_stream.available();
			byte[] file_buf = new byte[file_length];
			input_stream.read(file_buf);
			input_stream.close();
			
			//拷贝到目标位置
			File file = new File(local_file_name);
			if(!file.exists())
			{
				String parent = file.getParent();
				File parent_path = new File(parent);
				if(!parent_path.exists())
				{
					parent_path.mkdirs();
				}
			}
			FileOutputStream output_stream = new FileOutputStream(file);
			output_stream.write(file_buf);
			output_stream.close();
		}
		catch(IOException e)
		{
			read_ret = false;
		}
		
		return read_ret;
	}
	
	/**
	 * 打开文件
	 * @param path 文件路径
	 * @param actiontype intent的响应类型
	 * @param mimetype 文件类型
	 * @return
	 */
	public static boolean OpenFile(String path, String actiontype, String mimetype) {
		Uri uri = Uri.fromFile(new File(path));
		Intent intent = new Intent(actiontype);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK); 
		intent.setDataAndType(uri, mimetype);
		try {
			m_activity.startActivity(intent);
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	public static boolean InstallApk(String path, String actiontype, String mimetype)
	{
		Process p;
		try {
			p = Runtime.getRuntime().exec("chmod 777 " + GetDir(path));
			int status = p.waitFor();    
			if (status == 0) {
			    return OpenFile(path, actiontype, mimetype);
			} else {
			    return false;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}    
		return false;
	}

	public static String GetMacAddress()
	{
		String filePath = "/proc/net/arp";
		String ret_str = "";
		try
		{
			FileReader localFileReader = new FileReader(filePath);
			BufferedReader localBufferedReader = new BufferedReader(localFileReader, 1024);
			String lines = localBufferedReader.readLine();  // read table header
			String line = null;
			while (null != (line  = localBufferedReader.readLine()))
			{
				String[] linearray = line.split("\\s+");
				if (linearray.length < 4)
				{
					Log.i("cocos", "error: line array:"+line);
				}
				else
				{
					ret_str = String.valueOf(linearray[3]);
				}
			}
			
			localBufferedReader.close();
			
			ret_str = ret_str.replace(':', '-');
		} 
		catch (IOException e) 
		{
			Log.i("cocos", "error: read /proc/net/arp");
		}
		return ret_str;
	}
	private static String GetDir(String path)
	{
		String 		ret_var = "";
		String[] 	strs 	= path.split("/");
		int 		index 	= 1;
		if (strs.length > 0)
		{
			index = path.indexOf(strs[strs.length-1]);
		}
		
		ret_var = ret_var + path.substring(0,index);
		return ret_var;
	}
	
	public static String getMD5(String content) {
		String s = null;
		char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				'a', 'b', 'c', 'd', 'e', 'f' };
		try {
			java.security.MessageDigest md = java.security.MessageDigest
					.getInstance("MD5");
			md.update(content.getBytes());
			byte tmp[] = md.digest();
			char str[] = new char[16 * 2];
			int k = 0;
			for (int i = 0; i < 16; i++) {
				byte byte0 = tmp[i];
				str[k++] = hexDigits[byte0 >>> 4 & 0xf];
				str[k++] = hexDigits[byte0 & 0xf];
			}
			s = new String(str);

		} catch (Exception e) {
			e.printStackTrace();
		}
		return s;
	}
	
	public static boolean isAppInstalled(String packageName) {
		if (null == m_activity)
		{
			Log.e("PlatformUtil", "未初始化PlatformUtil的activity");
			return true;
		}
		PackageManager pm = m_activity.getPackageManager();
		boolean installed =false;
		try {
			pm.getPackageInfo(packageName,PackageManager.GET_ACTIVITIES);
			installed =true;
		} catch(PackageManager.NameNotFoundException e) {
			// 捕捉到异常,说明未安装
			installed =false;
		}
		return installed;
	}

	public static String GetLogPath() {
		String logDir = Environment.getExternalStorageDirectory().getAbsolutePath();
		return logDir;
	}
	
	public static String GetIPAddress() 
	{
		String ip = "";  
		
		ConnectivityManager conMann = (ConnectivityManager)m_activity.getSystemService(Context.CONNECTIVITY_SERVICE);  
        NetworkInfo mobileNetworkInfo = conMann.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);  
        NetworkInfo wifiNetworkInfo = conMann.getNetworkInfo(ConnectivityManager.TYPE_WIFI);  
		          
        if (mobileNetworkInfo.isConnected()) 
        {  
            ip = getLocalIpAddress();     
            Log.i(TAG, "GetIPAddress 本地IP：" + ip);
        }
        else if(wifiNetworkInfo.isConnected())  
        {  
            WifiManager wifiManager = (WifiManager)m_activity.getApplicationContext().getSystemService(Context.WIFI_SERVICE);    
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();         
            int ipAddress = wifiInfo.getIpAddress();     
            ip = IntToIp(ipAddress);   
            Log.i(TAG, "GetIPAddress WIFI IP：" + ip);
        }     		
        
        return ip;
	}
	
	private static String getLocalIpAddress() 
	{    
        try 
        {    
            String ipv4;    
            ArrayList<NetworkInterface>  nilist = Collections.list(NetworkInterface.getNetworkInterfaces());    
            for (NetworkInterface ni: nilist)     
            {    
                ArrayList<InetAddress>  ialist = Collections.list(ni.getInetAddresses());    
                for (InetAddress address: ialist)
                {    
                    if (!address.isLoopbackAddress() && InetAddressUtils.isIPv4Address(ipv4=address.getHostAddress()))     
                    {     
                        return ipv4;    
                    }    
                }    
            }         
        } 
        catch (SocketException ex) 
        {    
            Log.e("localip", ex.toString());    
        }    
        return null;    
    }    	
	
	private static String IntToIp(int ipInt) 
	{    
        StringBuilder sb = new StringBuilder();    
        sb.append(ipInt & 0xFF).append(".");    
        sb.append((ipInt >> 8) & 0xFF).append(".");    
        sb.append((ipInt >> 16) & 0xFF).append(".");    
        sb.append((ipInt >> 24) & 0xFF);    
        return sb.toString();    
    }    	
	
	// 网络状态改变
	public static void NetworkStateChange()
	{
        ((Cocos2dxActivity)m_activity).runOnGLThread(new Runnable() {
            @Override
            public void run() {
            	NetworkStateChangeNotify();
            }
        });		
	}
	
	// 获取国家代码
	public static String GetCountryCode() 
	{
		String countryCode = Locale.getDefault().getCountry();		
		return countryCode;
	}	
	
	// 获取货币代码
	public static String GetCurrencyCode() 
	{
		Currency curCurrency = Currency.getInstance(Locale.getDefault());
		String currencyCode = curCurrency.getCurrencyCode();
		return currencyCode;
	}		
	
	// 将维语文本从基本区转换为扩展区
	public static String ConvertUygur(String srcStr)
	{
		String dstStr = UyBaseAndEx.toEx(srcStr);
		return dstStr;
	}
	
	// 打开相册
	public static void OpenAlbum()
	{
        Intent intent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        m_activity.startActivityForResult(intent, SdkContants.ACT_RESULT_ALBUM_CODE);
	}

	// 选中相册图片回调
	public static void onAlblumResult(int requestCode, int resultCode, Intent data) 
	{		
		Log.i(TAG, "onAlblumResult reqCode: " + requestCode + ", retCode: " + resultCode);
		
       if (requestCode == SdkContants.ACT_RESULT_ALBUM_CODE && resultCode == Activity.RESULT_OK && data != null) {

    	   //查询数据库，获取图片路径
           Uri selectedImage = data.getData();
           String[] filePathColumns = {MediaStore.Images.Media.DATA};                          
           Cursor c = m_activity.getContentResolver().query(selectedImage, filePathColumns, null, null, null);
           c.moveToFirst();                      
           int columnIndex = c.getColumnIndex(filePathColumns[0]);
           String imagePath = c.getString(columnIndex);
           c.close();
           
           //回调给游戏并上传图片
           try
           {        	   
				String retData;
				JSONObject retDataJObj = new JSONObject();
				retDataJObj.put("type", SdkCommonHandleFuncType.FuncType_PICTRUE_EVENT_SEND);
				retDataJObj.put("path", imagePath);
				retData = retDataJObj.toString();									
				BaseSdk.SDKCommonHandleCallBack(SdkContants.COMMON_CALLFUNC_SUCCESS, retData);      
          }
          catch(JSONException e)
          {
        	  
          }
       }
   }
	
	private static native void NetworkStateChangeNotify();
	
	private static native void DlgCertainCallBack();
	
	private static native void DlgCancelCallBack();
	
	public static native void NativeCloseEngineInApp();
	
	public static native void NativeLoginfo(String info);
	
	public static native String GetSettingValue(String settingName);
}
