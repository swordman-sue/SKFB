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
package com.szba.skfb.quickpp;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.WindowManager;

import com.kuyue.sdk.Sdk;
import com.tencent.bugly.crashreport.CrashReport;

public class Skyx extends Cocos2dxActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{		
		super.onCreate(savedInstanceState);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		// 自定义SDK
		Sdk.initContext(this, SkyxSdk.class, getLinearLayout());
		Sdk.getInstance().onCreate(savedInstanceState);
	}

	@Override
	protected void onDestroy()
	{
		// 自定义SDK
        Sdk.getInstance().onDestroy();

        super.onDestroy();
    }

	@Override
	protected void onStart()
	{
		super.onStart();
		
		// 自定义SDK
        Sdk.getInstance().onStart();
	}
	
	@Override    
	protected void onRestart() {
		super.onRestart();
		
		// 自定义SDK
		Sdk.getInstance().onRestart();
	}

	@Override   
	protected void onPause() 
	{
        super.onPause();

		// 自定义SDK
		Sdk.getInstance().onPause();
	}
	
	@Override    
	protected void onResume() 
	{
        super.onResume();

		// 自定义SDK
		Sdk.getInstance().onResume();		
	}
	
	@Override    
	protected void onStop() 
	{		
        super.onStop();

		// 自定义SDK
		Sdk.getInstance().onStop();		
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		super.onActivityResult(requestCode, resultCode, data);

		// 自定义SDK
		Sdk.getInstance().onActivityResult(requestCode, resultCode, data);
	}

	@Override    
	protected void onNewIntent(Intent intent) 
	{
        super.onNewIntent(intent);

        // 自定义SDK
        Sdk.getInstance().onNewIntent(intent);
	}
}
