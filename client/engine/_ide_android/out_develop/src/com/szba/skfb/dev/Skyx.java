/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.szba.skfb.dev;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.app.Application.ActivityLifecycleCallbacks;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.net.ConnectivityManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.view.WindowManager;

import com.kuyue.net.NetBroadcastReceiver;
import com.kuyue.sdk.Sdk;
import com.tencent.bugly.crashreport.CrashReport;

public class Skyx extends Cocos2dxActivity implements ActivityCompat.OnRequestPermissionsResultCallback
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{		
		super.onCreate(savedInstanceState);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		Sdk.initContext(this, SkyxSdk.class, getLinearLayout());
		Sdk.getInstance().onCreate(savedInstanceState);
	}

	@Override
	protected void onDestroy()
	{
		Sdk.getInstance().onDestroy();
		super.onDestroy();
	}
	
	protected void onStart()
	{
		super.onStart();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		super.onActivityResult(requestCode, resultCode, data);
		Sdk.getInstance().onActivityResult(requestCode, resultCode, data);
	}
	
	@Override
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) 
	{
		Sdk.getInstance().onRequestPermissionsResult(requestCode, permissions, grantResults);
	}	
}
