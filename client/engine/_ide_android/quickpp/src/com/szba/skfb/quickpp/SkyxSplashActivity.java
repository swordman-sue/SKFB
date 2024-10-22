package com.szba.skfb.quickpp;

import android.content.Intent;

import com.quicksdk.QuickSdkSplashActivity;

public class SkyxSplashActivity extends QuickSdkSplashActivity {

	@Override
	public int getBackgroundColor() {
		return 0;
	}

	@Override
	public void onSplashStop() {
		Intent intent = new Intent(this, Skyx.class);
        startActivity(intent);
        this.finish();
	}

}
