package com.kuyue.pushsdk;

import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.Context;

public class PushUtils {
	/**
	 * 
	 */
	public final static String APP_NAME = "com.kuyue.pushsdk";
	/**
	 * 
	 */
	public static final String KEY_APP_KEY = "JPUSH_APPKEY";
	
	/**
	 * 设备类型 ANDROID
	 */
	public final static int DEVICE_TYPE = 3;
		
	/**
	 * 动作类型
	 */
	public static final String ACTION_TIMER = "com.kuyue.TIME_TICK";
	public static final String ACTION_BOOT = "android.intent.action.BOOT_COMPLETED";
	public static final String ACTION_TIME_TICK = "android.intent.action.ACTION_TIME_TICK";

	/**
	 * 处理状态
	 */
	public final static int INVOKE_SUCCESS_STATUS = 1;
	public final static int INVOKE_FAIL_STATUS = 2;
	
	/**
	 * 处理方法
	 */
	public final static String INVOKE_START = "START";
	public final static String INVOKE_RESUME = "RESUME";
	public final static String INVOKE_STOP = "STOP";
	public final static String INVOKE_GETAPIKEY = "GETAPIKEY";
	public final static String INVOKE_SETAPIKEY = "SETAPIKEY";
	public final static String INVOKE_SETTAG = "SETTAG";
	public final static String INVOKE_ALIAS = "ALIAS";
	public final static String INVOKE_GETAPPINFO = "GETAPPINFO";
	public final static String INVOKE_GETACTIVITYSTATUS = "GETACSTATUS";
	public final static String INVOKE_CANCELTASK = "CANCELTASK";
	
	/**
	 * 返回参数
	 */
	public final static String INVOKE_STATUS = "Status";
	public final static String INVOKE_CONTENT = "Content";
	public final static String DEVICE_TYPE_NAME = "DeviceType";
	public final static String REGISTRATION_ID_NAME = "Regist_id";
	public final static String USER_ALIAS = "UserAlias";
	public final static String TIMER_TIME_NAME = "TimerTime";
	public final static String HANDLE_CODE = "HandleCode";
	public final static String ACTIVITY_STATUS = "AcStatus";
	public final static String PUSH_STATUS = "PushStatus";
	public final static String PUSH_TAG = "PushTag";
	public final static String TAGS_LEN = "TagsLen";
	public final static String PUSH_ALIAS = "PushAlias";
	
	/**
	 * 请求方法码
	 */
	public final static int MSG_CODE_SETTAG = 101;
	public final static int MSG_CODE_SETALIAS = 102;
	public final static int MSG_CODE_STARTPUSH = 103;
	
	/**
	 * 通知码
	 */
	public final static String NOTIFY_CODE_TIMER = "1";
	public final static String NOTIFY_CODE_PUSH_SERVICE = "2";
	
	/**
	 * 定时任务
	 */
	public final static String TASK_ID = "TaskID";
	public final static String TASK_LEN = "TaskLen";
	public final static String TASK_NAME = "TaskName";
	public final static String TASK_TIME = "TaskTime";
	public final static String TASK_TYPE = "TaskLast";
	public final static String TASK_PRE = "TaskPre";
	public final static String TASK_STATUS = "TaskStatus";
	public final static String TASK_WEEK = "TaskWeek";
	public final static String TASK_RESTART_TIME = "TaskRestartTime";
	public final static String TASK_KILL_TIME = "TaskKillTime";

	/**
	 * 定时广播间隔
	 */
    public final static int ACTION_TIMER_INTERVAL = 40;

	/**
	 * 定时推送的提前基准时间
	 */
    public final static int PUSH_TIMER_PRE_BASE = 100;

	// 标签、别名毁掉信息缓存表
    private static HashMap<Integer, TagAliasCallbackInfo> tagAliasCallbackInfoMap = new HashMap<Integer, TagAliasCallbackInfo>();
    
    // 用于推送的用户操作序列号
    private static int pushSequence = 0;
    	
	/**
	 * 校验Tag Alias 只能是数字,英文字母和中文
	 * @param s
	 * @return
	 */
	public static boolean isValidTagAndAlias(String s) {
        Pattern p = Pattern.compile("^[\u4E00-\u9FA50-9a-zA-Z_!@#$&*+=.|]+$");
        Matcher m = p.matcher(s);
        return m.matches();
	}
	
	/**
	 * 获取周数（星期日是1，星期一是2）
	 * @return
	 */
	public static int getDayOfWeek(long time) {
		if (0 == time) {
			time = System.currentTimeMillis();
		}

		Calendar c = Calendar.getInstance();
		c.setTime(new Date(time));
		int dayOfWeek = c.get(Calendar.DAY_OF_WEEK);
		return dayOfWeek;
	}
	
	/**
	 * 获取当前时间（每天从0开始计算）
	 * @return
	 */
	public static int getCurrentTime() {
		Date curDate = new Date(System.currentTimeMillis());
		int hour = curDate.getHours();  
        int minute = curDate.getMinutes();   
        int sec = curDate.getSeconds();
        int result = hour * 60 * 60 + minute * 60 + sec;
        return result;
	}
	
	/**
	 * 程序是否运行中
	 * @return
	 */	
	public static boolean isAppRunning(Context context) {
		if(context == null) {
			return false;
		}
		ActivityManager am = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
	    List<RunningTaskInfo> list = am.getRunningTasks(100);
	    boolean isRunning = false;
	    for (RunningTaskInfo info : list) {
	        if (info.topActivity.getPackageName().equals(context.getPackageName()) || 
	        		info.baseActivity.getPackageName().equals(context.getPackageName())) {
	        	isRunning = true;
	            break;
	        }
	    }
		return isRunning;
	}
		
	/**
	 * 生成用于推送的用户操作序列号
	 */	
	public static int genPushSequence()
	{
		return ++pushSequence;
	}
	
	/**
	 * 获取标签、别名回调信息
	 */	
    public static TagAliasCallbackInfo getTagAliasCallbackInfo(int sequence) {
        return tagAliasCallbackInfoMap.get(sequence);
    }
    
	/**
	 * 设置标签、别名回调信息
	 */	
    public static void putTagAliasCallbackInfo(TagAliasCallbackInfo tagAliasBean){
    	tagAliasBean.sequence = PushUtils.genPushSequence();
        tagAliasCallbackInfoMap.put(tagAliasBean.sequence, tagAliasBean);
    }	

	/**
	 * 标签、别名回调信息
	 */	
	public static class TagAliasCallbackInfo
	{
		int type;
		int lua_fun_id;
		int sequence;
	}
}
