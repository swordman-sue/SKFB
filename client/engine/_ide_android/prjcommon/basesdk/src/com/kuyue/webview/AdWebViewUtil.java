package com.kuyue.webview;

import java.util.Random;

import android.app.Activity;
import android.util.Log;
import android.widget.LinearLayout;


public class AdWebViewUtil {
	private static AdWebViewManagerInterface mWebViewManager = null;
	
	public static AdWebViewManagerInterface CreateManager(Activity activity, LinearLayout layout) {
		if (null == mWebViewManager)
		{
			mWebViewManager = new AdWebViewManager();
		}
		mWebViewManager.setActivity(activity);
		mWebViewManager.setLinearLayout(layout);
		mWebViewManager.init();
		return mWebViewManager;
	}
	
	public static int Show(String url, int cachetype, int x, int y, int width, int height, int color)
	{
		Random rd = new Random();
		int tag;
		do {
			tag = rd.nextInt();
		} while (tag == 0 || mWebViewManager.hasTag(tag));

		mWebViewManager.ShowWebUrl(url, cachetype, x, y, width, height, color, tag);
		return tag;
	}
	
	public static void Refresh(int tag)
	{
		mWebViewManager.Refresh(tag);
	}
	
	public static void Close(int tag)
	{

		mWebViewManager.HideWebUrl(tag, true);
	}
	
	public static void CloseAll()
	{
		mWebViewManager.HideWebUrl(0, true);
	}
	public static void Destroy()
	{
		mWebViewManager.Destroy();
		mWebViewManager = null;
	}
}
