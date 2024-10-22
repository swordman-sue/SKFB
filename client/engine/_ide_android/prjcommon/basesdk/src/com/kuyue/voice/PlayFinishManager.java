package com.kuyue.voice;

import com.kuyue.queue.MessageData;
import com.kuyue.queue.MessageInterface;
import com.kuyue.queue.MessageMagager;
import com.kuyue.queue.MessageQueues;

public class PlayFinishManager implements MessageInterface{
	
	private static PlayFinishManager instance;
	
	public static PlayFinishManager GetInstance()
	{
		if(instance == null)
		{
			instance = new PlayFinishManager();
		}
		return instance;
	}
	
	private PlayFinishManager()
	{
		 MessageQueues.CreateVector(MessageMagager.MSG_PLAY_FINISH);
	}
	
	@Override
	public boolean Add(MessageData msg) {
		MessageQueues.Add(MessageMagager.MSG_PLAY_FINISH,msg);
		return true;
	}
}
