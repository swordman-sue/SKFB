package com.kuyue.queue;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import android.app.Activity;
import android.util.Log;

import com.kuyue.contant.SdkContants;
import com.kuyue.gps.GpsLocation;
import com.kuyue.pushsdk.MsgPush;
import com.kuyue.voice.PlayFinishManager;

public class MessageMagager 
{
	private static final String TAG = "MessageMagager";
	private static Map<Integer ,MessageInterface> interfaceMap = Collections.synchronizedMap(new HashMap<Integer ,MessageInterface>());
	
	public static int MSG_LOCATION = 1;
	public static int MSG_PLAY_FINISH = 2;
	public static int MSG_PUSH = 3;
	public static int MSG_SDK_TOOLS = 4;
	
	public static void initContext(Activity act)
	{
//		interfaceMap.put(MSG_LOCATION, GpsLocation.GetInstance());
		interfaceMap.put(MSG_PLAY_FINISH, PlayFinishManager.GetInstance());
		interfaceMap.put(MSG_PUSH, MsgPush.InitPush(act));
	}

	
	public static boolean  Add(int msg_code,MessageData msg)
	{
		if(msg_code < 0)
		{
			return false;
		}

		MessageInterface msgInterface = interfaceMap.get(msg_code);
		if(null != msgInterface)
		{
			return msgInterface.Add(msg);
		}else
		{
			Log.e(TAG, "not find handle message implements by msg_code " + msg_code);
			return false;
		}
	}

	public static Vector<MessageData>  Get(int msg_code)
	{
		return MessageQueues.Get(msg_code);
	}
}
