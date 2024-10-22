package com.kuyue.queue;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import com.kuyue.contant.SdkContants;
import com.kuyue.voice.MsgPlayFinishData;

import android.util.Log;

public class MessageQueues 
{
	private static final String TAG = "MessageQueues";
	
	private static Map<Integer,Vector<MessageData>> queueMap = Collections.synchronizedMap(
			new HashMap<Integer,Vector<MessageData>>());
	
	
	public static Vector<MessageData> CreateVector(int msg_code)
	{
		Vector<MessageData> vector = queueMap.get(msg_code);
		if(null == vector)
		{
			vector = new Vector<MessageData>();
			queueMap.put(msg_code, vector);
		}
		return vector;
	}
	
	public static boolean Add(int msg_code,MessageData msg)
	{
		Vector<MessageData> vector = queueMap.get(msg_code);
		if(null != vector)
		{
			synchronized (MessageQueues.class) 
			{
				vector.add(msg);
			}
		}else
		{
			Log.e(TAG, "the vector is not exist , msg_code is :" + msg_code);
		}
		return true;
	}
	
	/**
	 * 返回对应名称的vector的引用
	 * @param name
	 * @return Vector<MessageData>
	 */
	public static Vector<MessageData> GetVector(int msg_code)
	{
		return queueMap.get(msg_code);
	}

	/**
	 *  主要是用于返回clone的一个vector到jni
	 * @param name vector 名称
	 * @return TempVector  :
	 */
	public static  Vector<MessageData>  Get(int msg_code)
	{		
		Vector<MessageData> v = queueMap.get(msg_code);
		Vector<MessageData> temp_v = null;
		if(null != v)
		{
			synchronized (MessageQueues.class) 
			{
				temp_v = (Vector<MessageData>)v.clone();
				v.clear();
			}
		}else
		{
			temp_v = new Vector<MessageData>();
			Log.e(TAG, "vector is null when call GetTempVector and msg_code is :" + msg_code);			
		}
		return temp_v;
	}

	
}
