package com.kuyue.net;

import com.kuyue.sdklib.PlatformUtil;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.util.Log;

/**
 * 自定义检查手机网络状态是否切换的广播接受器
 * 
 */
public class NetBroadcastReceiver extends BroadcastReceiver {

	private static final String TAG = "NetBroadcastReceiver";

	@Override
	public void onReceive(Context context, Intent intent) {
		// 如果相等的话就说明网络状态发生了变化
		if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
			PlatformUtil.NetworkStateChange();
		}
	}
}