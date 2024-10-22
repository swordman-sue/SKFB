package com.szba.skfb.quickpp;

import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.content.Intent;
import android.media.audiofx.LoudnessEnhancer;
import android.os.Bundle;
import android.util.Log;

import com.kuyue.contant.SdkCommonHandleFuncType;
import com.kuyue.contant.SdkCommonHandleType;
import com.kuyue.contant.SdkContants;
import com.kuyue.sdklib.BaseSdk;
import com.kuyue.sdklib.PlatformUtil;
import com.kuyue.sdklib.RoleInfo;
import com.quicksdk.Extend;
import com.quicksdk.FuncType;
import com.quicksdk.Payment;
import com.quicksdk.QuickSDK;
import com.quicksdk.Sdk;
import com.quicksdk.User;
import com.quicksdk.entity.GameRoleInfo;
import com.quicksdk.entity.OrderInfo;
import com.quicksdk.entity.UserInfo;
import com.quicksdk.notifier.ExitNotifier;
import com.quicksdk.notifier.InitNotifier;
import com.quicksdk.notifier.LoginNotifier;
import com.quicksdk.notifier.LogoutNotifier;
import com.quicksdk.notifier.PayNotifier;
import com.quicksdk.notifier.SwitchAccountNotifier;

public class SkyxSdk extends BaseSdk
{	
	private static final String TAG = "SZBA";
	private static final String TAG_QK = "QuickSDK";
	private final boolean isLandscape = true;	

	private final String productCode = "33332429532015072921019874893307";
	private final String productKey = "63098293";
	
	private String loginData = null;

	public SkyxSdk(Activity activity) 
	{
		super(activity, activity.getResources().getIdentifier("app_name", "string", activity.getPackageName()));
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		// 生命周期接口调用(必接)
		Sdk.getInstance().onCreate(GetActivity());
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();
		
		// 生命周期接口调用(必接)
        Sdk.getInstance().onDestroy(GetActivity());		
	}

	@Override
	public void onStart()
	{
		super.onStart();
		
		// 生命周期接口调用(必接)
		Sdk.getInstance().onStart(GetActivity());
	}

	@Override
	public void onRestart()
	{
		super.onRestart();

		// 生命周期接口调用(必接)
        Sdk.getInstance().onRestart(GetActivity());
	}

	@Override
	public void onPause()
	{
		super.onPause();
		
		// 生命周期接口调用(必接)
		Sdk.getInstance().onPause(GetActivity());		
	}

	@Override
	public void onResume()
	{
		super.onResume();
		
		// 生命周期接口调用(必接)
		Sdk.getInstance().onResume(GetActivity());		
	}

	@Override
	public void onStop()
	{
		super.onStop();
		
		// 生命周期接口调用(必接)
        Sdk.getInstance().onStop(GetActivity());		
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		super.onActivityResult(requestCode, resultCode, data);
		
		// 生命周期接口调用(必接)
		Sdk.getInstance().onActivityResult(GetActivity(), requestCode, resultCode, data);
	}

	@Override
	public void onNewIntent(Intent intent)
	{
		super.onNewIntent(intent);
		
		// 生命周期接口调用(必接)
		Sdk.getInstance().onNewIntent(intent);		
	}

	@Override
	public void SDKInit() 
	{
		Log.i(TAG,"----SDKInit-------");

		// 设置横竖屏，游戏横屏为true，游戏竖屏为false(必接)
		QuickSDK.getInstance().setIsLandScape(isLandscape);

		// 设置通知，用于监听初始化，登陆，注销，支付及退出功能的返回值(必接)
		initQkNotifiers();

		// 初始化接口，应在Activity的onCreate方法中调用(必接)
		// 请将下面语句中的第二与第三个参数，替换成QuickSDK后台申请的productCode和productKey值
		Sdk.getInstance().init(GetActivity(), productCode, productKey);
	}
	
	@Override
	public void SDKLogin(String info) 
	{		
		if (null != loginData)
		{
			// 逻辑上不允许这种情况，若出现了，直接注销账号
			SDKLogout("");
		}
		else
		{
			Log.i(TAG,"----SDKLogin-------" + info);
			
			final Activity act = (Activity)GetActivity();			
			Cocos2dxHelper.runOnUIThread(new Runnable() {
				
				@Override
				public void run() {					
					// 登陆账号
					User.getInstance().login(act);
				}
			});
		}
	}
	
	@Override
	public void SDKLogout(String info) 
	{
		if (null == loginData)
		{
			return;
		}
		
		Log.i(TAG,"----SDKLogout-------" + info);
		
		final Activity act = (Activity)GetActivity();			
		Cocos2dxHelper.runOnUIThread(new Runnable() {
			
			@Override
			public void run() {					
				// 注销账号
				User.getInstance().logout(act);
			}
		});
	}
	
	@Override
	public void SDKSwitchAccount(String info) 
	{
		if (null == loginData)
		{
			return;
		}
		
		Log.i(TAG,"----SDKSwitchAccount-------" + info);
		
		// 切换账号
		if (!callQKFunction(SdkCommonHandleFuncType.FuncType_SWITCH_ACCOUNT))
		{
			// 不支持切换账号，注销账号
			SDKLogout("");
		}
	}

	@Override
	public void SDKPay(String info) 
	{
		Log.i(TAG,"----SDKPay-------" + info);
		
		pay(info);
	}

	@Override
	public void SDKCommonHandle(String info)
	{
		Log.i(TAG,"----SDKCommonHandle-------" + info);
		
		try 
		{
			JSONObject jsonObj = new JSONObject(info);		
			int type = jsonObj.getInt("type");
			switch(type)
			{
			case SdkCommonHandleType.CT_CREATE_ROLE:
			case SdkCommonHandleType.CT_ENTER_GAME:
			case SdkCommonHandleType.CT_LEVEL_UP:
				// 创建角色、进入游戏、角色升级，上传角色信息
				setUserInfo(type == SdkCommonHandleType.CT_CREATE_ROLE, info);
				break;
			
			case SdkCommonHandleType.CT_CALL_FUNC:
				String funcType = jsonObj.getString("func_type");
				
				// 调用操作函数
				if (callQKFunction(funcType))
				{
					BaseSdk.SDKCommonHandleCallBack(SdkContants.COMMON_CALLFUNC_SUCCESS, funcType);
				}
				else
				{
					BaseSdk.SDKCommonHandleCallBack(SdkContants.COMMON_CALLFUNC_FAILED, funcType);					
				}
				break;
			}
		}
		catch (JSONException e)
		{
			e.printStackTrace();
		}
	}

	@Override
	public Object GetPropertie(String key)
	{
		if (key.equals("CHANNEL_ID"))
		{
			// 调用QuickSDK接口获取渠道号
			return String.valueOf(Extend.getInstance().getChannelType());
		}
		return super.GetPropertie(key);
	}

	@Override
	public void SDKToUserCenter(String info)
	{
		Log.i(TAG,"----SDKToUserCenter-------" + info);	
		
		callQKFunction(SdkCommonHandleFuncType.FuncType_ENTER_USER_CENTER);
	}

	@Override
	public void SDKExitApp(String info)
	{		
		// 先判断渠道是否有退出框，如果有则直接调用quick的exit接口
		if (QuickSDK.getInstance().isShowExitDialog()) 
		{
			final Activity act = GetActivity();
			Cocos2dxHelper.runOnUIThread(new Runnable() {
				
				@Override
				public void run() {				
					Sdk.getInstance().exit(act);
				}
			});			
		} 
		else 
		{
			// 游戏调用自身的退出对话框，点击确定后，调用quick的exit接口
			super.SDKExitApp(info);
		}
	}

	@Override
	public void SDKExitAppNotify(boolean isCustomSDK)
	{
		if (isCustomSDK)
		{
			Sdk.getInstance().exit(GetActivity());					
		}
		else
		{			
			super.SDKExitAppNotify(isCustomSDK);
		}
	}
		
	/**
	 * 支付
	 */
	private void pay(String info) 
	{
		final Activity act = (Activity)GetActivity();
		final GameRoleInfo roleInfo = QKGameInfo.parseBaseRoleInfo(info);
		final OrderInfo orderInfo = QKGameInfo.parseOrderInfo(info);
		
		Cocos2dxHelper.runOnUIThread(new Runnable() {
			
			@Override
			public void run() {				
				Payment.getInstance().pay(act, orderInfo, roleInfo);
			}
		});
	}

	/**
	 * 向渠道提交用户信息。 在创建游戏角色、进入游戏和角色升级3个地方调用此接口，当创建角色时最后一个参数值为true，其他两种情况为false。
	 * GameRoleInfo所有字段均不能传null，游戏没有的字段请传一个默认值或空字符串。
	 */
	public void setUserInfo(final boolean isCreateRole, String info) 
	{		
		final Activity act = (Activity)GetActivity();
		final GameRoleInfo roleinfo = QKGameInfo.parseDetailRoleInfo(info);
		
		Cocos2dxHelper.runOnUIThread(new Runnable() {
			
			@Override
			public void run() {
				User.getInstance().setGameRoleInfo(act, roleinfo, isCreateRole);				
			}
		});
	}
	
	/**
	 * 设置通知，用于监听初始化，登陆，注销，支付及退出功能的返回值
	 */
	private void initQkNotifiers() 
	{
		QuickSDK.getInstance()
		// 1.设置初始化通知(必接)
		.setInitNotifier(new InitNotifier() 
		{
			@Override
			public void onSuccess() {
				// 初始化成功
				Log.i(TAG_QK, "初始化成功");				
				
				BaseSdk.SDKInitFinishCallBack(SdkContants.SUCCESS_CODE);
			}

			@Override
			public void onFailed(String message, String trace) {
				// 初始化失败
				Log.e(TAG_QK, "初始化失败");
				
				BaseSdk.SDKInitFinishCallBack(SdkContants.FAILURE_CODE);
			}
		})
		
		// 2.设置登陆通知(必接)
		.setLoginNotifier(new LoginNotifier() 
		{
			@Override
			public void onSuccess(UserInfo userInfo) {
				if (userInfo != null) 
				{
					// 登陆成功
					Log.i(TAG_QK, "登陆成功" + "\n\r" + "UserID:  " + userInfo.getUID() + "\n\r" + "UserName:  " + userInfo.getUserName() + "\n\r" + "Token:  " + userInfo.getToken());

					loginData = "{\"user_id\":\"" + userInfo.getUID() + 
			        			"\",\"user_name\":\"" + userInfo.getUserName() + 
			        			"\",\"token\":\"" + userInfo.getToken() + "\"}";					
					
					// 登陆成功回调
					BaseSdk.SDKLoginPanelCallBack(SdkContants.SUCCESS_CODE, loginData);
				}
			}

			@Override
			public void onCancel() 
			{
				// 登陆取消
				Log.i(TAG_QK, "取消登陆");
				
				BaseSdk.SDKLoginPanelCallBack(SdkContants.CANCEL_CODE);
			}

			@Override
			public void onFailed(final String message, String trace) 
			{
				// 登陆失败
				Log.e(TAG_QK, "登陆失败：" + message);

				BaseSdk.SDKLoginPanelCallBack(SdkContants.FAILURE_CODE);
			}
		})

		// 3.设置注销通知(必接)
		.setLogoutNotifier(new LogoutNotifier() 
		{
			@Override
			public void onSuccess() 
			{
				// 注销成功
				Log.i(TAG_QK, "注销成功");

				// 清空登陆数据
				loginData = null;

				// 注销成功回调
				BaseSdk.SDKLoginPanelCallBack(SdkContants.LOGOUT_CODE);				
			}

			@Override
			public void onFailed(String message, String trace) 
			{
				// 注销失败，不做处理
				Log.e(TAG_QK, "注销失败：" + message);
			}
		})

		// 4.设置切换账号通知(必接)
		.setSwitchAccountNotifier(new SwitchAccountNotifier() 
		{
			@Override
			public void onSuccess(UserInfo userInfo) 
			{
				if (userInfo != null) 
				{
					// 切换账号成功
					Log.i(TAG_QK, "切换账号成功" + "\n\r" + "UserID:  " + userInfo.getUID() + "\n\r" + "UserName:  " + userInfo.getUserName() + "\n\r" + "Token:  " + userInfo.getToken());

					loginData = "{\"user_id\":\"" + userInfo.getUID() + 
		        			"\",\"user_name\":\"" + userInfo.getUserName() + 
		        			"\",\"token\":\"" + userInfo.getToken() + "\"}";					

					// 切换账号成功的回调
					BaseSdk.SDKLoginPanelCallBack(SdkContants.SWITCH_ACCOUNT_CODE, loginData);				
				}
			}

			@Override
			public void onFailed(String message, String trace) 
			{
				// 切换账号失败
				Log.e(TAG_QK, "切换账号失败：" + message);
			}

			@Override
			public void onCancel() 
			{
				// 切换账号取消
				Log.i(TAG_QK, "取消切换账号");
			}
		})
		
		// 5.设置支付通知(必接)
		.setPayNotifier(new PayNotifier() 
		{
			@Override
			public void onSuccess(String sdkOrderID, String cpOrderID, String extrasParams)
			{
				// 支付成功
				Log.i(TAG_QK, "支付成功，sdkOrderID:" + sdkOrderID + ",cpOrderID:" + cpOrderID);

				String payData = "";
				try 
				{
					JSONObject payDataJObj = new JSONObject();
					payDataJObj.put("sdk_order_id", sdkOrderID);
					payDataJObj.put("cp_order_id", cpOrderID);
					payDataJObj.put("extra_params", extrasParams);
					payData = payDataJObj.toString();
				}
				catch (JSONException e)
				{
					e.printStackTrace();
				}

				// 支付成功的回调
				BaseSdk.SDKPayFinishCallBack(SdkContants.SUCCESS_CODE, payData);				
			}

			@Override
			public void onCancel(String cpOrderID) 
			{
				// 支付取消
				Log.i(TAG_QK, "支付取消");

				// 支付取消的回调
				BaseSdk.SDKPayFinishCallBack(SdkContants.CANCEL_CODE);				
			}

			@Override
			public void onFailed(String cpOrderID, String message, String trace) 
			{
				// 支付失败
				Log.e(TAG_QK, "支付失败:" + "pay failed,cpOrderID:" + cpOrderID + ",message:" + message);

				// 支付失败的回调
				BaseSdk.SDKPayFinishCallBack(SdkContants.FAILURE_CODE);				
			}
		})

		// 6.设置退出通知(必接)
		.setExitNotifier(new ExitNotifier() 
		{
			@Override
			public void onSuccess() 
			{
				// 退出成功，进行游戏本身的退出操作
				SDKExitAppNotify(false);
			}

			@Override
			public void onFailed(String message, String trace) 
			{
				// 退出失败
				Log.e(TAG_QK, "退出失败");
			}
		});
	}	
	
	// 调用QuickSDK扩展函数
	private boolean callQKFunction(String func_type)
	{
		int qkFuncType = QKFuncType.parseCommonHandleFuncType(func_type);
		if (Extend.getInstance().isFunctionSupported(qkFuncType))
		{
			Extend.getInstance().callFunction(GetActivity(), qkFuncType);
			return true;
		}						
		return false;
	}
}
