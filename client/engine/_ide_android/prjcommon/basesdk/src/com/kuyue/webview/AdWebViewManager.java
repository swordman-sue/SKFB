package com.kuyue.webview;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import android.annotation.SuppressLint;
import android.app.Activity;
//import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebStorage;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.graphics.Bitmap;

public class AdWebViewManager implements AdWebViewManagerInterface {
	private static Activity mActivity = null;
	private Map<Integer, RelativeLayout> mWebSummaryMap = null;
	private Map<Integer, WebView> mShowingViewMap = null;
	protected LinearLayout mLinearLayout = null;
	protected String packageName = "";
	protected String mCacheDir = "";
	protected Integer mAppCacheSize = 15*1025*1024;
	
	AppCacheWebChromeClient webchromeclient = null;
	WebViewClient webviewclient = null;
	
	public static Activity getActivity() {
		if (null == mActivity)
		{
			Log.e(TAG, "mActivity not init");
		}
		return mActivity;
	}

	@Override
	public void SetCacheDir()
	{
		
	}

	@Override
	public void SetCacheSize()
	{
		
	}
	
	@SuppressLint("SdCardPath")
	@Override
	public void init() {
		if (mActivity!=null)
		{
			packageName = mActivity.getApplicationContext().getPackageName();
		}
		mCacheDir = String.valueOf("/data/data/"+packageName+"/Caches");
		resetWebMap();
	}
	
	@Override
	public void setActivity(Activity activity) {
		mActivity = activity;
	}
	@Override
	public void setLinearLayout(LinearLayout linearLayout) {
		mLinearLayout = linearLayout;
	}

	@Override
	public void ShowWebUrl(final String url, final int cachetype, final int left, final int top, final int width, final int height, final int color, final int tag) {

		new Handler (Looper.getMainLooper()).post(new Runnable()
		{
			public void run()
			{
				WebView webview = new WebView(AdWebViewManager.getActivity()); 
				
				WebSettings settings = webview.getSettings();
				settings.setCacheMode(AdWebViewTools.GetCacheModeByType(cachetype));
		        settings.setAppCachePath(mCacheDir);
		        settings.setAppCacheEnabled(true);
		        settings.setAppCacheMaxSize(mAppCacheSize);
		        settings.setJavaScriptEnabled(true);
		        settings.setJavaScriptCanOpenWindowsAutomatically(true);
				
		        if (webviewclient == null)
		        {
		        	initClient();
		        }
		        
		        webview.setWebChromeClient(webchromeclient);
				webview.setWebViewClient(webviewclient);
		        
		        webview.loadUrl(url);
				webview.setBackgroundColor(color);
				
				RelativeLayout.LayoutParams layoutparams = new RelativeLayout.LayoutParams(width, height);
				layoutparams.leftMargin = left;
				layoutparams.topMargin = top;
				
				AddWebView(webview, layoutparams, tag);
				
				layoutparams = null;

				mShowingViewMap.put(tag, webview);
			}
		});
	}
	
	private void AddWebView(WebView webview, RelativeLayout.LayoutParams layoutparams, int tag)
	{
		RelativeLayout m_webview_layout = new RelativeLayout(AdWebViewManager.getActivity());
		m_webview_layout.addView(webview, layoutparams);
		
		mLinearLayout.addView(m_webview_layout);
		
		mWebSummaryMap.put(tag, m_webview_layout);
		m_webview_layout = null;
	}

	@Override
	public boolean Refresh(int tag) {
		boolean ret_val = false;
		WebView webview = null;
		if (null != (webview = mShowingViewMap.get(tag)))
		{
			webview.reload();
			ret_val = true;
		}
		return ret_val;
	}

	@Override
	public boolean HideWebUrl(final Integer tag, final boolean isdestory) {
		new Handler (Looper.getMainLooper()).post(new Runnable()
		{
			public void run()
			{
				boolean isForceHide = false;
				if (tag == 0)
				{
					isForceHide = true;
				}
				Set<Integer> keys = mWebSummaryMap.keySet();
				for( Integer  key : keys )
				{
	
					if (isForceHide || key.equals(tag))

					{
						mLinearLayout.removeView(mWebSummaryMap.get(key));
						//mWebSummaryMap.remove(key);
						//mShowingViewMap.remove(key);
					}
				}
			}
		});
		return false;
	}

	@Override
	public boolean hasTag(Integer tag)
	{
		return mShowingViewMap.get(tag) != null;
	}
	
	@Override
	public void Destroy()
	{
		if (null != mLinearLayout)
		{
			mLinearLayout.removeAllViews();
		}
		if (null != mWebSummaryMap)
		{
			for( Integer  key : mWebSummaryMap.keySet() )
			{
				mWebSummaryMap.remove(key);
			}
		}
		if (null != mShowingViewMap)
		{
			for( Integer  key : mShowingViewMap.keySet() )
			{
				mShowingViewMap.remove(key);
			}
		}
		webchromeclient = null;
		webviewclient = null;
	}
	
	@SuppressLint("UseSparseArrays")
	private boolean resetWebMap() {
		if (null == mWebSummaryMap)
		{
			mWebSummaryMap = new HashMap<Integer, RelativeLayout>();
		}
		if (null == mShowingViewMap)
		{
			mShowingViewMap = new HashMap<Integer, WebView>();
		}
		return true;
	}
	
	private void initClient()
	{
		webchromeclient = new AppCacheWebChromeClient();
		webviewclient = new WebViewClient() {
            @Override
            public void onLoadResource(WebView pWebView, String pUrl) {
                Log.i(TAG, "onLoadResource: " + pUrl);
            }

            @Override
            public void onPageStarted(WebView pView, String pUrl, Bitmap pBit) {
                Log.i(TAG,"onPageStarted: " + pUrl);

                // OK, we're on the correct thread here for callbacks
                // If not (such as in various parts of the Activity lifecycle) these WebStorage.* methods will never work
                WebStorage.getInstance().getOrigins(new ValueCallback<Map> () {
                    @Override
                    public void onReceiveValue(Map map) {
                    	
                    }
                });

                // This should blow away this individual origin's manifests
                WebStorage.getInstance().deleteOrigin("http://people.opera.com");
                WebStorage.getInstance().setQuotaForOrigin("http://people.opera.com", 10);

                // This should blow away everything, but does not on disk
                WebStorage.getInstance().deleteAllData();

            }
        };
	}

	private class AppCacheWebChromeClient extends WebChromeClient {

        @Override
        public void onReachedMaxAppCacheSize(long spaceNeeded, long totalUsedQuota, WebStorage.QuotaUpdater quotaUpdater) {
            quotaUpdater.updateQuota(spaceNeeded * 2);
        }
    }
}
