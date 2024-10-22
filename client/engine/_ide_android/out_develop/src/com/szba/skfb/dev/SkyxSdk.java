package com.szba.skfb.dev;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import com.szba.skfb.dev.R;
import com.kuyue.contant.SdkContants;
import com.kuyue.sdklib.BaseSdk;

public class SkyxSdk extends BaseSdk{
	
	private static final String TAG = "SZBA";
	
	public SkyxSdk(Activity activity) {
		super(activity, R.string.app_name);
	}
	
	@Override
	public void SDKInit() {
		Log.i(TAG,"----SDKInit-------");
		
		BaseSdk.SDKInitFinishCallBack(SdkContants.SUCCESS_CODE);
	}
	
	@Override
	public void SDKLogin(String info) {
		Log.i(TAG,"----SDKLogin-------" + info);
		
	}

	@Override
	public void SDKPay(String info) {
		Log.i(TAG,"----SDKPay-------" + info);
	}
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
	}
}
