package com.kuyue.pushsdk;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import com.kuyue.contant.SdkContants;
import com.kuyue.pushsdk.PushUtils.TagAliasCallbackInfo;
import com.kuyue.queue.MessageData;
import com.kuyue.queue.MessageInterface;
import com.kuyue.queue.MessageMagager;
import com.kuyue.queue.MessageQueues;
import com.kuyue.sdk.Sdk;

//import cn.jpush.android.api.JPushInterface;
//import cn.jpush.android.api.JPushMessage;


import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

/**
 * 消息推送管理类
 */
public class MsgPush implements MessageInterface {

	private final static String TAG = "MsgPush";

	private static MsgPush pushManager = null;

	private static Context m_context = null;

	private MsgPush(Context context) {
		m_context = context;
	}

	/**
	 * 初始化
	 * 
	 * @param arg_activity
	 * @param arg_icon
	 * @return
	 */
	public static MsgPush InitPush(Context context) {
		if (null == pushManager) {
			pushManager = new MsgPush(context);
			NotificationManager nm = (NotificationManager) m_context
					.getSystemService(Context.NOTIFICATION_SERVICE);
			nm.cancelAll();
			// JPushInterface.setDebugMode(true);
			// JPushInterface.init(context);
			MessageQueues.CreateVector(MessageMagager.MSG_PUSH);
		}
		return pushManager;
	}

	/**
	 * 
	 */
	public static void DestoryPush() {
		m_context = null;
	}

	/**
	 * 
	 * @param InvokeType
	 * @param ExtraInfo
	 * @return
	 */
	public static String InvokeVoidPushService(String InvokeType,
			String ExtraInfo) {
		if (null == InvokeType) {
			Log.e(TAG, "InvokePushService Invoke Type is null");
			return "";
		}
		if (null == m_context) {
			m_context = Sdk.getContext();
		}
		if (PushUtils.INVOKE_RESUME.equals(InvokeType)) {
			return ResumePush();

		} else if (PushUtils.INVOKE_STOP.equals(InvokeType)) {
			return StopPush();

		} else if (PushUtils.INVOKE_GETAPIKEY.equals(InvokeType)) {
			return GetApiKey();

		} else if (PushUtils.INVOKE_GETAPPINFO.equals(InvokeType)) {
			return GetAppInfo();

		} else if (PushUtils.INVOKE_GETACTIVITYSTATUS.equals(InvokeType)) {
			return GetActivityStatus();

		} else if (PushUtils.INVOKE_CANCELTASK.equals(InvokeType)) {
			return InvokeCancelTimerTask(ExtraInfo);
		}
		return "";
	}

	/**
	 * 
	 * @param InvokeType
	 * @param ExtraInfo
	 * @return
	 */
	public static void InvokeCallBackPushService(String InvokeType,
			int lua_fun_id, String ExtraInfo) {
		if (null == m_context) {
			m_context = Sdk.getContext();
		}
		if (PushUtils.INVOKE_START.equals(InvokeType)) {
			StartPush(lua_fun_id);

		} else if (PushUtils.INVOKE_SETTAG.equals(InvokeType)) {
			SetTag(lua_fun_id, ExtraInfo);

		} else if (PushUtils.INVOKE_ALIAS.equals(InvokeType)) {
			SetAlias(lua_fun_id, ExtraInfo);
		}
	}

	/**
	 * 
	 * @param taskName
	 * @param taskTime
	 * @param taskType
	 * @param preTime
	 * @param weekTime
	 * @return
	 */
	public static String InvokeTimerTask(String taskID, String taskTime,
			String taskName, String taskType, String preTime, String weekTime) {
		try {
			if (null == m_context) {
				m_context = Sdk.getContext();
			}

			PushInfoManager appInfo = new PushInfoManager(m_context);
			int taskNo = appInfo.SetTimerTask(Integer.valueOf(taskID),
					taskTime, taskName, taskType, preTime, weekTime);

			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);
			if (taskNo > 0) {
				info.put(PushUtils.INVOKE_STATUS,
						PushUtils.INVOKE_SUCCESS_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, taskNo);
			} else {
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "set timer fail");
			}
			return info.toString();

		} catch (JSONException e) {
			e.printStackTrace();
			return "";
		}
	}

	/**
	 * 
	 * @param taskTime
	 * @return
	 */
	public static String InvokeCancelTimerTask(String taskID) {
		try {
			PushInfoManager appInfo = new PushInfoManager(m_context);
			int noResult = appInfo.CancelTimerTask(taskID);
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);
			if (noResult > 0) {
				info.put(PushUtils.INVOKE_STATUS,
						PushUtils.INVOKE_SUCCESS_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "success");
			} else {
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "no timer task");
			}
			return info.toString();

		} catch (JSONException e) {
			e.printStackTrace();
			return "";
		}
	}

	/**
	 * 
	 * @return
	 */
	public static String GetActivityStatus() {
		try {
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);

			if (null == m_context) {
				Log.e(TAG, "MpushManager is not init...");
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "MpushManager is not init");
			} else {
				PushInfoManager appInfo = new PushInfoManager(m_context);
				info.put(PushUtils.INVOKE_STATUS,
						PushUtils.INVOKE_SUCCESS_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, appInfo.GetActivityStatus());
			}
			return info.toString();

		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
	}

	/**
	 * 
	 * @return
	 */
	public static String GetAppInfo() {
		try {
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);
			PushInfoManager appInfo = new PushInfoManager(m_context);
			info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_SUCCESS_STATUS);
			info.put(PushUtils.INVOKE_CONTENT, appInfo.GetRegistrationId());
			info.put(PushUtils.PUSH_ALIAS, appInfo.GetAlias());
			return info.toString();

		} catch (JSONException e) {
			e.printStackTrace();
			return "";
		}
	}

	/**
	 * @return
	 */
	public static String GetApiKey() {
		try {
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);

			if (null == m_context) {
				Log.e(TAG, "MpushManager is not init...");
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "MpushManager is not init");
			} else {
				PushInfoManager appInfo = new PushInfoManager(m_context);
				info.put(PushUtils.INVOKE_STATUS,
						PushUtils.INVOKE_SUCCESS_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, appInfo.GetApiKey());
			}
			return info.toString();

		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
	}

	/**
	 * 设置标签
	 */
	public static void SetTag(int lua_fun_id, final String add_tag) {
		try {
			PushInfoManager appInfo = new PushInfoManager(m_context);
			String status = appInfo.GetPushStatus();
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);

			// 清空所有tag
			if (null == add_tag || "".equals(add_tag)) {
				appInfo.ClearTags();

				TagAliasCallbackInfo callbackInfo = new TagAliasCallbackInfo();
				callbackInfo.type = PushUtils.MSG_CODE_SETTAG;
				callbackInfo.lua_fun_id = lua_fun_id;
				PushUtils.putTagAliasCallbackInfo(callbackInfo);

				Set<String> tagSet = new LinkedHashSet<String>();

				// JPushInterface.setTags(m_context, callbackInfo.sequence,
				// tagSet);
				return;
			}

			// 设置tag
			if (PushUtils.INVOKE_START.equals(status)) {
				int len = appInfo.GetTagsLen();
				String[] sAddTagArray = add_tag.split(";");
				String serverTag = sAddTagArray[0];
				String[] serverArr = serverTag.split("_");

				if (serverArr.length <= 1) {
					info.put(PushUtils.INVOKE_STATUS,
							PushUtils.INVOKE_FAIL_STATUS);
					info.put(PushUtils.INVOKE_CONTENT, "tag position error");
					MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG,
							lua_fun_id, info.toString());
					return;
				}

				// 服务器ID
				String server = serverArr[1];
				// 该服务器的所有TAG
				List<String> currServerTags = appInfo.GetCurrentServerTag(
						server, len);

				Set<String> tagSet = new LinkedHashSet<String>();
				Set<String> addSet = new LinkedHashSet<String>();
				for (String ns : sAddTagArray) {
					if (!MsgPush.IsTagExist(currServerTags, ns)) {
						tagSet.add(ns);
					}
					addSet.add(ns);
				}
				if (!currServerTags.isEmpty() && tagSet.isEmpty()) {
					info.put(PushUtils.INVOKE_STATUS,
							PushUtils.INVOKE_SUCCESS_STATUS);
					info.put(PushUtils.INVOKE_CONTENT, "tag have");
					MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG,
							lua_fun_id, info.toString());
				} else {
					TagAliasCallbackInfo callbackInfo = new TagAliasCallbackInfo();
					callbackInfo.type = PushUtils.MSG_CODE_SETTAG;
					callbackInfo.lua_fun_id = lua_fun_id;
					PushUtils.putTagAliasCallbackInfo(callbackInfo);

					Set<String> haveSet = appInfo.GetOtherServerTags(server,
							len);
					addSet.addAll(haveSet);

					// JPushInterface.setTags(m_context, callbackInfo.sequence,
					// addSet);
				}
			} else {
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "service stop");
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG, lua_fun_id,
						info.toString());
			}

		} catch (Exception e) {
			e.printStackTrace();
			MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG, lua_fun_id, "");
		}
	}

	/**
	 * 设置别名
	 */
	public static void SetAlias(int lua_fun_id, final String add_alias) {
		try {
			PushInfoManager appInfo = new PushInfoManager(m_context);
			String status = appInfo.GetPushStatus();
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);

			if (null == add_alias || "".equals(add_alias)) {
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "arg_Tag is null");
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS,
						lua_fun_id, info.toString());
			}

			else if (PushUtils.INVOKE_START.equals(status)) {
				if (appInfo.IsHaveAlias(add_alias)) {
					info.put(PushUtils.INVOKE_STATUS,
							PushUtils.INVOKE_SUCCESS_STATUS);
					info.put(PushUtils.INVOKE_CONTENT, "have");
					MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS,
							lua_fun_id, info.toString());
				} else {
					TagAliasCallbackInfo callbackInfo = new TagAliasCallbackInfo();
					callbackInfo.type = PushUtils.MSG_CODE_SETALIAS;
					callbackInfo.lua_fun_id = lua_fun_id;
					PushUtils.putTagAliasCallbackInfo(callbackInfo);

					// JPushInterface.setAlias(m_context, callbackInfo.sequence,
					// add_alias);
				}
			}

			else {
				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "service is not start");
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS,
						lua_fun_id, info.toString());
			}
		}

		catch (Exception e) {
			e.printStackTrace();
			MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS, lua_fun_id, "");
		}
	}

	/**
	 * 设置标签回调
	 */
	/*
	public static void onTagOperatorResult(Context context,
			JPushMessage jPushMessage) {
		if (!PushUtils.isAppRunning(MsgPush.m_context)) {
			return;
		}

		int sequence = jPushMessage.getSequence();
		int errorCode = jPushMessage.getErrorCode();
		Set<String> tags = jPushMessage.getTags();
		if (null == tags) {
			return;
		}
		Log.i(TAG,
				"[onTagOperatorResult] action - onTagOperatorResult, sequence:"
						+ sequence + ",tags:" + tags);
		Log.i(TAG, "[onTagOperatorResult] tags size:" + tags.size());

		// 根据sequence从之前操作缓存中获取缓存记录
		TagAliasCallbackInfo callbackInfo = PushUtils
				.getTagAliasCallbackInfo(sequence);
		if (callbackInfo == null) {
			Log.i(TAG, "[onTagOperatorResult] 获取缓存记录失败");
			return;
		}
		if (callbackInfo.type != PushUtils.MSG_CODE_SETTAG) {
			Log.i(TAG, "[onTagOperatorResult] 无效的请求类型");
			return;
		}

		String logs = "";

		try {
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);

			// 设置成功
			if (errorCode == 0) {
				PushInfoManager appInfo = new PushInfoManager(MsgPush.m_context);
				appInfo.ClearTags();
				for (String tag : tags) {
					appInfo.SetTags(tag);
				}

				info.put(PushUtils.INVOKE_STATUS,
						PushUtils.INVOKE_SUCCESS_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, "success");
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG,
						callbackInfo.lua_fun_id, info.toString());
			}

			// 设置失败
			else {
				logs = "errorCode:" + errorCode;
				if (errorCode == 6018) {
					// tag数量超过限制,需要先清除一部分再add
					logs += ", tags is exceed limit need to clean";
				}
				Log.i(TAG, logs);

				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, String.valueOf(errorCode));
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG,
						callbackInfo.lua_fun_id, info.toString());
			}
		}

		catch (Exception e) {
			e.printStackTrace();
			// MpushManager.MsgPushCallBack(PushUtils.MSG_CODE_SETTAG,
			// callbackInfo.lua_fun_id, "");
		}
	}
	*/

	/**
	 * 设置别名回调
	 */
	/*
	public static void onAliasOperatorResult(Context context,
			JPushMessage jPushMessage) {
		if (!PushUtils.isAppRunning(MsgPush.m_context)) {
			return;
		}

		int sequence = jPushMessage.getSequence();
		int errorCode = jPushMessage.getErrorCode();
		String alias = jPushMessage.getAlias();
		if (null == alias) {
			return;
		}

		Log.i(TAG, "action - onAliasOperatorResult, sequence:" + sequence
				+ ",alias:" + alias);

		// 根据sequence从之前操作缓存中获取缓存记录
		TagAliasCallbackInfo callbackInfo = PushUtils
				.getTagAliasCallbackInfo(sequence);
		if (callbackInfo == null) {
			Log.i(TAG, "[onAliasOperatorResult] 获取缓存记录失败");
			return;
		}
		if (callbackInfo.type != PushUtils.MSG_CODE_SETALIAS) {
			Log.i(TAG, "[onAliasOperatorResult] 无效的请求类型");
			return;
		}

		String logs = "";

		try {
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);

			if (errorCode == 0) {
				PushInfoManager appInfo = new PushInfoManager(MsgPush.m_context);
				appInfo.SetAlias(alias);
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS,
						callbackInfo.lua_fun_id, info.toString());
			} else {
				logs = "errorCode:" + errorCode;
				Log.i(TAG, logs);

				info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_FAIL_STATUS);
				info.put(PushUtils.INVOKE_CONTENT, String.valueOf(errorCode));
				MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS,
						callbackInfo.lua_fun_id, info.toString());
			}
		}

		catch (Exception e) {
			e.printStackTrace();
			// MpushManager.MsgPushCallBack(PushUtils.MSG_CODE_SETALIAS,
			// callbackInfo.lua_fun_id, "");
		}
	}
	*/

	public static synchronized void MsgPushCallBack(int method, int lua_fun_id,
			String callbackdata) {
		if (0 != lua_fun_id) {
			MsgPushData msg_data = new MsgPushData();
			msg_data.setCode(method);
			msg_data.setLuafunid(lua_fun_id);
			msg_data.setData(callbackdata);
			MessageMagager.Add(MessageMagager.MSG_PUSH, msg_data);
		}
	}

	@Override
	public boolean Add(MessageData msg) {
		MessageQueues.Add(MessageMagager.MSG_PUSH, msg);
		return true;
	}

	/**
	 * 是否存在相同tag
	 */
	private static boolean IsTagExist(List<String> array, String obj) {
		for (String cs : array) {
			if (PushUtils.isValidTagAndAlias(cs) && cs.equals(obj)) {
				return true;
			}
		}
		return false;
	}

	/**
	 * 启动
	 */
	private static void StartPush(int lua_fun_id) {
		Log.i(TAG, "启动推送服务");
		
		Intent startPushServiceIntent = new Intent(m_context, MsgPushServiceLocal.class);
		if (Cocos2dxHelper.getSDKVersion() >= Build.VERSION_CODES.O)
		{
			m_context.startForegroundService(startPushServiceIntent);
		}
		else
		{
			m_context.startService(startPushServiceIntent);			
		}
		PushInfoManager app = new PushInfoManager(m_context);

		if (PushUtils.INVOKE_STOP.equals(app.GetPushStatus())) {
//			JPushInterface.resumePush(m_context);
		}
		try {
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);
			info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_SUCCESS_STATUS);
			info.put(PushUtils.INVOKE_CONTENT, "success");
			MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_STARTPUSH, lua_fun_id,
					info.toString());

		} catch (JSONException e) {
			e.printStackTrace();
			MsgPush.MsgPushCallBack(PushUtils.MSG_CODE_STARTPUSH, lua_fun_id,
					"");
		}
	}

	/**
	 * 停止
	 */
	private static String StopPush() {
		try {
			// JPushInterface.stopPush(m_context);
			PushInfoManager appInfo = new PushInfoManager(m_context);
			appInfo.SetPushStatus(PushUtils.INVOKE_STOP);
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);
			info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_SUCCESS_STATUS);
			info.put(PushUtils.INVOKE_CONTENT, "success");
			return info.toString();

		} catch (JSONException e) {
			e.printStackTrace();
			return "";
		}
	}

	/**
	 * 重启
	 */
	private static String ResumePush() {
		try {
			// JPushInterface.resumePush(m_context);
			PushInfoManager appInfo = new PushInfoManager(m_context);
			appInfo.SetPushStatus(PushUtils.INVOKE_START);
			JSONObject info = new JSONObject();
			info.put(PushUtils.DEVICE_TYPE_NAME, PushUtils.DEVICE_TYPE);
			info.put(PushUtils.INVOKE_STATUS, PushUtils.INVOKE_SUCCESS_STATUS);
			info.put(PushUtils.INVOKE_CONTENT, "success");
			return info.toString();

		} catch (JSONException e) {
			e.printStackTrace();
			return "";
		}
	}
}
