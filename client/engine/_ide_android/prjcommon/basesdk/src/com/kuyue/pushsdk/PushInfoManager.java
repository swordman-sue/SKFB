/**
 * 
 */
package com.kuyue.pushsdk;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.Vector;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.util.Log;

/**
 * 推送数据管理
 *
 */
public class PushInfoManager {

	private static final String TAG = "AppInfoManager";

	private SharedPreferences settings = null;

	private SharedPreferences.Editor editor = null;
	
	private Context m_context = null;
	/**
	 * constructor
	 * 
	 * @param context
	 */
	public PushInfoManager(Context context) {
		settings = context.getSharedPreferences(PushUtils.APP_NAME, Application.MODE_APPEND);
		editor = settings.edit();
		m_context = context;
	}
	/**
	 * 获得ApiKey
	 * @return String
	 */
	public String GetApiKey() {
        Bundle metaData = null;
        String appKey = null;
        try {
            ApplicationInfo ai = m_context.getPackageManager().getApplicationInfo(
            		m_context.getPackageName(), PackageManager.GET_META_DATA);
            if (null != ai)
                metaData = ai.metaData;
            if (null != metaData) {
                appKey = metaData.getString(PushUtils.KEY_APP_KEY);
                if ((null == appKey) || appKey.length() != 24) {
                    appKey = null;
                }
            }
        } catch (NameNotFoundException e) {

        }
        return appKey;
	}
	/**
	 * 
	 * @param app_id
	 * @param channel_id
	 * @param user_id
	 */
	public void SetRegistrationId(final String Registration_id) {
		editor.putString(PushUtils.REGISTRATION_ID_NAME, Registration_id);
		editor.commit();
	}
	/**
	 * 
	 * @return
	 */
	public String GetRegistrationId() {
		return settings.getString(PushUtils.REGISTRATION_ID_NAME, "");
	}
	/**
	 * 
	 * @return
	 */
	public int GetTimerTaskLen() {
		return settings.getInt(PushUtils.TASK_LEN, 0);
	}
	/**
	 * 
	 * @param taskLen
	 */
	public void SetTimerTaskLen(int taskLen) {
		editor.putInt(PushUtils.TASK_LEN, taskLen);
		editor.commit();
	}
	/**
	 * 
	 * @return
	 */
	public String GetActivityStatus() {
		return settings.getString(PushUtils.ACTIVITY_STATUS, "0");
	}
	/**
	 * 
	 * @param status
	 */
	public void SetActivityStatus(String status) {
		editor.putString(PushUtils.ACTIVITY_STATUS, status);
		editor.commit();
	}
	/**
	 * 
	 * @return
	 */
	public String GetPushStatus() {
		return settings.getString(PushUtils.PUSH_STATUS, PushUtils.INVOKE_START);
	}
	/**
	 * 
	 * @param status
	 */
	public void SetPushStatus(String status) {
		editor.putString(PushUtils.PUSH_STATUS, status);
		editor.commit();
	}
	/**
	 * 
	 * @param taskID (任务ID)
	 * 
	 * @param taskTime 
	 *            (second)
	 * @param taskType
	 *            1为不重复, 2为每天重复, 3为每周重复, 4延时触发(preTime < 0)
	 * @param preTime
	 *            为提前提示时间(second) 小于0为当前的延后时间
	 * @return -1时间已过, -2不存在模式, -3异常
	 */
	public int SetTimerTask(int taskID, String taskTime, String taskName, String taskType, String preTime, String weekTime) {
		try {
			Integer i_pre_time = Integer.valueOf(preTime);

			// 延时触发的任务
			if(i_pre_time < 0) {			
				preTime = "0";
				weekTime = "";
				taskType = "4";
				taskTime = String.valueOf(System.currentTimeMillis() / 1000 - i_pre_time);						
			}
			
			int task_len = GetTimerTaskLen();
			int task = HaveTimerTask(task_len, taskID);
			if(0 == task){
				// 新增任务
				SetTimerTaskLen(++task_len);
				
				editor.putString(PushUtils.TASK_ID + task_len, String.valueOf(taskID));
				editor.putString(PushUtils.TASK_NAME + task_len, taskName);
				editor.putString(PushUtils.TASK_TIME + task_len, taskTime);
				editor.putString(PushUtils.TASK_TYPE + task_len, taskType);
				editor.putString(PushUtils.TASK_PRE + task_len, preTime);
				editor.putString(PushUtils.TASK_STATUS + task_len, PushUtils.INVOKE_START);
				editor.putString(PushUtils.TASK_WEEK + task_len, weekTime);
				editor.putLong(PushUtils.TASK_RESTART_TIME + task_len, 0);
				editor.putLong(PushUtils.TASK_KILL_TIME + task_len, 0);
				editor.commit();
				return 1;
				
			} else {				
				// 更新任务
				editor.putString(PushUtils.TASK_NAME + task, taskName);
				editor.putString(PushUtils.TASK_TIME + task, taskTime);
				editor.putString(PushUtils.TASK_TYPE + task, taskType);
				editor.putString(PushUtils.TASK_PRE + task, preTime);
				editor.putString(PushUtils.TASK_WEEK + task, weekTime);
				editor.commit();
				return 1;
			}
			
		} catch(Exception e) {
			e.printStackTrace();
		}
		return 0;
	}
	/**
	 * 
	 * @param taskTime
	 * @return
	 */
	public int CancelTimerTask(String taskID) {
		try {
			int task_len = GetTimerTaskLen();
			if("ALL".equals(taskID)) {
				for(int i = 1; i <= task_len; i++) {
					editor.remove(PushUtils.TASK_ID + i);
					editor.remove(PushUtils.TASK_NAME + i);
					editor.remove(PushUtils.TASK_TIME + i);
					editor.remove(PushUtils.TASK_TYPE + i);
					editor.remove(PushUtils.TASK_PRE + i);
					editor.remove(PushUtils.TASK_STATUS + i);
					editor.remove(PushUtils.TASK_WEEK + i);
					editor.remove(PushUtils.TASK_RESTART_TIME + i);
					editor.remove(PushUtils.TASK_KILL_TIME + i);
				}
				SetTimerTaskLen(0);
				return 1;
		
			} else {
				for(int i = 1; i <= task_len; i++) {
					String task = settings.getString(PushUtils.TASK_ID + i, "0");
					if(task.equals(taskID)) {						
						editor.remove(PushUtils.TASK_ID + i);
						editor.remove(PushUtils.TASK_NAME + i);
						editor.remove(PushUtils.TASK_TIME + i);
						editor.remove(PushUtils.TASK_TYPE + i);
						editor.remove(PushUtils.TASK_PRE + i);
						editor.remove(PushUtils.TASK_STATUS + i);
						editor.remove(PushUtils.TASK_WEEK + i);
						editor.remove(PushUtils.TASK_RESTART_TIME + i);
						editor.remove(PushUtils.TASK_KILL_TIME + i);

						// 删除项不是最后一个，则将最后一个覆盖到删除项
						if (i != task_len) {
							String taskIDEnd = settings.getString(PushUtils.TASK_ID + task_len, "0");
							String taskNameEnd = settings.getString(PushUtils.TASK_NAME + task_len, "");
							String taskTimeEnd = settings.getString(PushUtils.TASK_TIME + task_len, "0");
							String taskPreEnd = settings.getString(PushUtils.TASK_PRE + task_len, "0");
							String taskTypeEnd = settings.getString(PushUtils.TASK_TYPE + task_len, "0");
							String taskStatusEnd = settings.getString(PushUtils.TASK_STATUS + task_len, PushUtils.INVOKE_START);
							String taskWeekEnd = settings.getString(PushUtils.TASK_WEEK + task_len, "0");
							Long taskKillTimeEnd = settings.getLong(PushUtils.TASK_KILL_TIME + task_len, 0);
							Long taskRestartTimeEnd = settings.getLong(PushUtils.TASK_RESTART_TIME + task_len, 0);							
							
							editor.putString(PushUtils.TASK_ID + i, taskIDEnd);
							editor.putString(PushUtils.TASK_NAME + i, taskNameEnd);
							editor.putString(PushUtils.TASK_TIME + i, taskTimeEnd);
							editor.putString(PushUtils.TASK_TYPE + i, taskTypeEnd);
							editor.putString(PushUtils.TASK_PRE + i, taskPreEnd);
							editor.putString(PushUtils.TASK_STATUS + i, taskStatusEnd);
							editor.putString(PushUtils.TASK_WEEK + i, taskWeekEnd);
							editor.putLong(PushUtils.TASK_KILL_TIME + i, taskKillTimeEnd);
							editor.putLong(PushUtils.TASK_RESTART_TIME + i, taskRestartTimeEnd);
						}
						SetTimerTaskLen(task_len - 1);
						break;
					}
				}
				return 1;
			}
			
		} catch(Exception e) {
			e.printStackTrace();
			return 0;
		}
	}
	/**
	 * 
	 * @param task_len
	 * @param taskTime
	 * @return
	 */
	public int HaveTimerTask(int task_len, int taskID) {
		String nTaskID = String.valueOf(taskID);
		
		for(int i = 1; i <= task_len; i++) {
			String task = settings.getString(PushUtils.TASK_ID + i, "");
			if(task.equals(nTaskID)){
				return i;
			}
		}
		return 0;
	}

	/**
	 * 是否可执行的任务 
	 */	
	public int IsTimeToTask(int currentTime) 
	{
		int spaceTime = 10;
		long currentTimeSys = System.currentTimeMillis() / 1000;
				
		int task_len = GetTimerTaskLen();		
		for (int i = 1; i <= task_len; i++) {
			try {
				String taskID = settings.getString(PushUtils.TASK_ID + i, "0");
				String taskTime = settings.getString(PushUtils.TASK_TIME + i, "0");
				String taskPre = settings.getString(PushUtils.TASK_PRE + i, "0");
				String taskType = settings.getString(PushUtils.TASK_TYPE + i, "0");
				String taskStatus = settings.getString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_START);
				String taskWeek = settings.getString(PushUtils.TASK_WEEK + i, "0");
				Long taskRestartTime = settings.getLong(PushUtils.TASK_RESTART_TIME + i, 0);
				Integer nTaskPre = Integer.valueOf(taskPre);
				
				String[] sTimeArray = taskTime.split(";");
								
				for(String sTime : sTimeArray) {
					Integer nTaskTime = Integer.valueOf(sTime);
					
					// 检测任务触发				
					if ("4".equals(taskType)) {
						// 延时触发任务
						if ((nTaskTime - currentTimeSys) <= nTaskPre + PushUtils.PUSH_TIMER_PRE_BASE && (nTaskTime - currentTimeSys) > 0 && PushUtils.INVOKE_START.equals(taskStatus)) {										
							editor.putString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_STOP);
							editor.putLong(PushUtils.TASK_KILL_TIME + i, nTaskTime + spaceTime);						
							editor.commit();		
							return i;
						}
						
					} else {
						// 一次性任务、每天任务、每周任务
						if ((nTaskTime - currentTime) <= nTaskPre + PushUtils.PUSH_TIMER_PRE_BASE && (nTaskTime - currentTime) > 0 && PushUtils.INVOKE_START.equals(taskStatus)) {										
							if("1".equals(taskType)) {
								editor.putString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_STOP);
								editor.putLong(PushUtils.TASK_KILL_TIME + i, currentTimeSys + nTaskTime - currentTime + spaceTime);						
								editor.commit();
								return i;
								
							} else if("2".equals(taskType)) {
								editor.putString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_STOP);
								editor.putLong(PushUtils.TASK_RESTART_TIME + i, currentTimeSys + nTaskTime - currentTime + spaceTime);						
								editor.commit();
								return i;
								
							} else if("3".equals(taskType)) {
								String[] sWeekArray = taskWeek.split(";");
								int curWeek = PushUtils.getDayOfWeek(0);								
								for(String week : sWeekArray) {
									if(String.valueOf(curWeek).equals(week)) {
										editor.putString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_STOP);
										editor.putLong(PushUtils.TASK_RESTART_TIME + i, currentTimeSys + nTaskTime - currentTime + spaceTime);						
										editor.commit();
										return i;
									}
								}
								
							}
						} 											
					}					
				}
								
				// 重启可重复触发的已触发任务
				if (taskRestartTime > 0 && taskRestartTime <= currentTimeSys && PushUtils.INVOKE_STOP.equals(taskStatus)) {
					editor.putString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_START);
					editor.putLong(PushUtils.TASK_RESTART_TIME + i, 0);
					editor.commit();
				}
				
			} catch(Exception e) {
				e.printStackTrace();
				return 0;
			}
		}
				
		return 0;
	}

	/**
	 * 移除不可重复触发的已触发任务
	 */	
	public void KillTimerTask(int currentTime) {				
		Vector<String> delTaskList = new Vector<String>();

		long currentTimeSys = System.currentTimeMillis() / 1000;

		int task_len = GetTimerTaskLen();
		for (int i = 1; i <= task_len; i++) {
			try {
				String taskID = settings.getString(PushUtils.TASK_ID + i, "0");
				String taskStatus = settings.getString(PushUtils.TASK_STATUS + i, PushUtils.INVOKE_START);
				Long taskKillTime = settings.getLong(PushUtils.TASK_KILL_TIME + i, 0);

				// 记录待移除的任务
				if (taskKillTime > 0 && taskKillTime <= currentTimeSys && PushUtils.INVOKE_STOP.equals(taskStatus)) {
					delTaskList.add(taskID);
				}								
			
			} catch(Exception e) {
				e.printStackTrace();
			}
		}	
		
		// 移除不可重复触发的已触发任务				
		for (int i = 0; i < delTaskList.size(); ++i) {			
			CancelTimerTask(delTaskList.get(i));
		}
	}
	
	/**
	 * 
	 * @param taskNo
	 * @return
	 */
	public String GetTaskName(int taskNo) {
		return settings.getString(PushUtils.TASK_NAME + taskNo, "");
	}
	/**
	 * 
	 * @param taskNo
	 * @return
	 */
	public String GetTaskTime(int taskNo) {
		return settings.getString(PushUtils.TASK_TIME + taskNo, "0");
	}
	
	public void SetTags(String argTag) {
		int len = GetTagsLen();
		int tag_len = len + 1;
		editor.putString(PushUtils.PUSH_TAG + tag_len, argTag);
		editor.commit();
		SetTagsLen(tag_len);
	}
	
	public void SetAlias(String argAlias) {
		editor.putString(PushUtils.PUSH_ALIAS, argAlias);
		editor.commit();
	}
	
	public String GetAlias(){
		return settings.getString(PushUtils.PUSH_ALIAS, "");
	}
	
	public boolean IsHaveAlias(String argAlias) {
		String alias = settings.getString(PushUtils.PUSH_ALIAS, "");
		if(alias.equals(argAlias)){
			return true;
		}
		return false;
	}
	
	public boolean IsHaveTag(String argTag, int len) {
		for(int i = 1; i <= len; i++){
			String tag = settings.getString(PushUtils.PUSH_TAG + i, "");
			if(tag.equals(argTag)){
				return true;
			}
		}
		return false;
	}
	
	public List<String> GetCurrentServerTag(String server, int len) {
		List<String> result = new ArrayList<String>();
		for(int i = 1; i <= len; i++){
			String tag = settings.getString(PushUtils.PUSH_TAG + i, "");
			if(null != tag && !"".equals(tag)) {
				String[] tm_server_tag = tag.split("_");
				if(null != server && tm_server_tag.length == 1){
					result.add(tag);
					
				} else if(null != server && tm_server_tag.length > 1 && server.equals(tm_server_tag[1])){
					result.add(tag);
				}
			}
		}
		return result;
	}
	public Set<String> GetOtherServerTags(String server, int len) {
		Set<String> set = new LinkedHashSet<String>();
		for(int i = 1; i <= len; i++){
			String tag = settings.getString(PushUtils.PUSH_TAG + i, "");
			if(null != tag && !"".equals(tag)) {
				String[] tm_server_tag = tag.split("_");
				if(null == server || tm_server_tag.length <= 1 || !server.equals(tm_server_tag[1])) {
					set.add(tag);
				}
			}
		}
		return set;
	}
	
	public void SetTagsLen(int argLen) {
		editor.putInt(PushUtils.TAGS_LEN, argLen);
		editor.commit();
	}
	
	public int GetTagsLen() {
		return settings.getInt(PushUtils.TAGS_LEN, 0);
	}
	
	public void ClearTags() {
		int len = GetTagsLen();
		for(int i = 1; i <= len; i++){
			editor.remove(PushUtils.PUSH_TAG + i);
		}
		editor.commit();
		SetTagsLen(0);
	}
}
