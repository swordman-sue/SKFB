package com.kuyue.webview;

import android.webkit.WebSettings;

public class AdWebViewTools {

	public static final int CacheTypeUseProtocolCachePolicy = 0; 		// Use Protocol Cache Policy
	public static final int CacheTypeReturnCacheDataElseLoad = 1; 		// Return Cache Data Else Load
	public static final int CacheTypeReturnCacheDataDontLoad = 2; 		// Return Cache Data
	public static final int CacheTypeReloadIgnoringLocalCacheData = 3;  // Ignoring Cache
	public static final int CacheTypeDefault = CacheTypeUseProtocolCachePolicy;
	
	public static final int CacheManageClearHistory = 4;    				// Clear Cache
	
	public static int GetCacheModeByType(int type)
	{
		int cacheMode = WebSettings.LOAD_DEFAULT;
		switch (type)
		{
			case CacheTypeUseProtocolCachePolicy:
				cacheMode = WebSettings.LOAD_DEFAULT;
				break;
			case CacheTypeReturnCacheDataElseLoad:
				cacheMode = WebSettings.LOAD_CACHE_ELSE_NETWORK;
				break;
			case CacheTypeReturnCacheDataDontLoad:
				cacheMode = WebSettings.LOAD_CACHE_ONLY;
				break;
			case CacheTypeReloadIgnoringLocalCacheData:
				cacheMode = WebSettings.LOAD_NO_CACHE;
				break;
			default:
				cacheMode = WebSettings.LOAD_DEFAULT;
				break;
		}
		return cacheMode;
	}
}
