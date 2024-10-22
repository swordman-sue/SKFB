package com.kuyue.webview;

import android.app.Activity;
import android.widget.LinearLayout;

public interface AdWebViewManagerInterface {

	public final static String TAG = "webview";

	public abstract void setActivity(Activity activity);
	
	public abstract void init();

	public abstract void setLinearLayout(LinearLayout linearLayout);
	
	public abstract void SetCacheDir();

	public abstract void SetCacheSize();

	public abstract boolean hasTag(Integer tag);

	public abstract void ShowWebUrl(final String url, final int cachetype, final int left, final int top, final int width, final int height, final int color, final int tag);
	
	public abstract boolean Refresh(int tag);

	public abstract boolean HideWebUrl(Integer tag, boolean isdestory);

	public abstract void Destroy();

}
