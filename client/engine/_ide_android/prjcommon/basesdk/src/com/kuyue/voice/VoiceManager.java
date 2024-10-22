package com.kuyue.voice;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import android.annotation.SuppressLint;
import android.media.AudioFormat;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.util.Log;

import com.kuyue.queue.MessageMagager;
/**
 * 
 * @author deepsea
 * return code
 */
class Code
{
	static final int SUCCESS = 0;
	static final int ERROR = 1;
	static final int FILE_ERROR = 2;
	static final int CHANGE_CODE_ERROR = 3;
	static final int VOICE_PARAMETER_ERROR = 4;		// 参数错误
	static final int NOT_SUPPORT_INIT= 5;		// 实例化错误
}
class TimerRecord { 
	public static final int DEFAULT_SECOND = 20;
	public static int max_secord = 20;
    Timer timer;  
    long record_time = System.currentTimeMillis();
    public TimerRecord(int secord) {     
        timer = new Timer();   
        if (secord == 0)
		{
        	secord = DEFAULT_SECOND;
		}
        max_secord = secord;
        timer.schedule(new TimerRecordTask(), secord*1000); 
    }
    class TimerRecordTask extends TimerTask {     
        public void run() { 
			stopCurrentTime();
        	if (VoiceManager.mrecorder != null)
    		{
        		VoiceManager.mrecorder.stop();
        		VoiceManager.mrecorder.release();
        		VoiceManager.mrecorder = null;
    		}
        }     
    }     
    public void stopCurrentTime()
    {
    	if (timer != null) 
    	{
    		timer.cancel(); 
    		timer = null;
    	}
    }
    public long getRecord_time() {
		return record_time;
	}
}
/**
 * 
 * @author DeepSea
 *
 */
public class VoiceManager {
	private static final String TAG = "VoiceManager";
	//record setting 
	private static final  int audioEncodingBitRate = 5000;
	private static final int audioSource = MediaRecorder.AudioSource.MIC;
	//MediaRecorder.OutputFormat.RAW_AMR,This constant was deprecated in API level 16
	//and no matter AMR_NB or AMR_NB,value is 3
	private static final int outputFormat = 3;
	private static final int audioChannels = 1;
	private static final int audioSamplingRate = 8000;
	private static final int audioEncoder = MediaRecorder.AudioEncoder.AMR_NB;
	
	private static final int MAX_SIZE = 20;
	//init variable
	public  static MediaRecorder mrecorder = null;
	private static MediaPlayer mplay = null;
	private static int recorde_time = 0;
	private static TimerRecord timerrecorder = null;
	private static FileInputStream playstream = null;
	//callback variable
	private static int play_voice_id = 0;
	private static int play_lua_fun_id = 0;
	
	public static int StartRecord(String fullfilename,int secord)
	{
		//before recording voice,release the resource
		ReleaseVoice();
		PalyFinishCallBack(Code.SUCCESS);
		if (null == fullfilename || "".equals(fullfilename))
		{
			Log.e(TAG, "error! source file path is empty!");
			return Code.VOICE_PARAMETER_ERROR;
		}
		File soundFile = null;
		// check file dir
		try {
			soundFile = new File(fullfilename);
			soundFile.createNewFile();
			if(!soundFile.exists())
			{
				Log.e(TAG, "error! source file can not be created!");
				return Code.FILE_ERROR;
			}
			
		} catch (Exception e1) {
			Log.e(TAG, "error, can not create source file!");
			return Code.FILE_ERROR;
		}
        //begin to record the video
		try {
			// MediaRecorder setting
			mrecorder = new MediaRecorder();  
			if(null == mrecorder)
			{
				return Code.NOT_SUPPORT_INIT;
			}
			
			mrecorder.setAudioSource(audioSource);  
			mrecorder.setOutputFormat(outputFormat);  
	        mrecorder.setAudioChannels(audioChannels);  
	        mrecorder.setAudioEncodingBitRate(audioEncodingBitRate);
	        mrecorder.setAudioSamplingRate(audioSamplingRate);  
	        mrecorder.setAudioEncoder(audioEncoder);  
	        
	        mrecorder.setOutputFile(soundFile.getAbsolutePath());
	        mrecorder.setMaxDuration(1000*MAX_SIZE);
	        timerrecorder = new TimerRecord(secord);
	   
			mrecorder.prepare();
			mrecorder.start();
			return Code.SUCCESS;
		} catch (IllegalStateException e) {
			Log.e(TAG, "error, can not create recorder!");
			VoiceRecordRelease();
			return Code.NOT_SUPPORT_INIT;
		} catch (IOException e) {
			Log.e(TAG, "error, can not create recorder!");
			VoiceRecordRelease();
			return Code.NOT_SUPPORT_INIT;
		}
	}
	/**
	 * 
	 * @return code
	 */
	@SuppressLint("UseValueOf")
	public static int StopRecord()
	{
		recorde_time = 0;
		if (timerrecorder!= null)
		{
			long stop_record_time = System.currentTimeMillis();
			long start_recorde_time = timerrecorder.getRecord_time();
			if (stop_record_time - start_recorde_time > 0)
			{
				long all_record_time = stop_record_time - start_recorde_time;
				int all_record_time_int = new Long(all_record_time).intValue();
				recorde_time = all_record_time_int/1000;
				if(recorde_time > TimerRecord.max_secord)
				{
					recorde_time = TimerRecord.max_secord;
				}
			}
			
		}
		VoiceRecordRelease();
		return recorde_time;
	}
	/**
	 * 
	 * @param fullfilename
	 * @param voice_id
	 * @param lua_fun_id
	 * @return code
	 */
	public static int StartPlay(String fullfilename,int voice_id,int lua_fun_id)
	{
		VoicePlayRelease();
		PalyFinishCallBack(Code.SUCCESS);
		play_voice_id = voice_id;
		play_lua_fun_id = lua_fun_id;
		try {
			if (playstream != null)
			  {
				  playstream.close();
				  playstream = null;
			  }
		} catch (IOException e1) {
			Log.e(TAG, "error, release playing source when you want to play it!");
			PalyFinishCallBack(Code.ERROR);
			return Code.ERROR;
		}
	  try { 
		  File file = new File(fullfilename);
		 if(!file.exists() || file.length() <= 0)
			{
			 	Log.e(TAG, "error, source file not exist when you want to play file!");
			 	PalyFinishCallBack(Code.FILE_ERROR);
	 			return Code.FILE_ERROR;
			}
		  if (mplay == null)
		  {
			  mplay = new MediaPlayer();
		  }
		  if (mplay == null)
		  {
			  Log.e(TAG, "error, can not new MediaPlayer instance!");
			  PalyFinishCallBack(Code.NOT_SUPPORT_INIT);
			  return Code.NOT_SUPPORT_INIT;
		  }
		  playstream =  new FileInputStream(file); 
		  mplay.setDataSource(playstream.getFD());  
		  mplay.setOnCompletionListener(new MediaPlayer.OnCompletionListener() 
		  	{
	            @Override  
	            public void onCompletion(MediaPlayer mp)
	            {  
            	try {
        			if (playstream != null)
        			    {
        				  playstream.close();
        				  playstream = null;
        			    }
        				VoicePlayRelease();
        				PalyFinishCallBack(Code.SUCCESS);
	        		} catch (IOException e1) {
	        			VoicePlayRelease();
	        			PalyFinishCallBack(Code.ERROR);
	        		}
	            }  
		  	}); 
		  mplay.setOnErrorListener(new MediaPlayer.OnErrorListener()
		  {
			@Override
			public boolean onError(MediaPlayer mp, int what, int extra) {
				VoicePlayRelease();
				PalyFinishCallBack(Code.ERROR);
            	return false;
			}
		  });
		  mplay.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
			@Override
			public void onPrepared(MediaPlayer mp) {
				 mplay.start();  
			}
		  	});
		  mplay.prepare();
        } catch (IllegalArgumentException e) {  
        	VoicePlayRelease();
        	PalyFinishCallBack(Code.ERROR);
        	Log.e(TAG, "error, create play error!");
        	return Code.ERROR;
        } catch (IllegalStateException e) { 
        	VoicePlayRelease();
        	PalyFinishCallBack(Code.ERROR);
        	Log.e(TAG, "error, create play error!");
        	return Code.ERROR;
        } catch (IOException e) { 
        	VoicePlayRelease();
        	PalyFinishCallBack(Code.ERROR);
        	Log.e(TAG, "error, create play error!");
        	return Code.ERROR;
        }  
	  return Code.SUCCESS;
	}
	/**
	 * 
	 * @return code
	 */
	public static int StopPlay()
	{
		PalyFinishCallBack(Code.SUCCESS);
		VoicePlayRelease();
		return Code.SUCCESS; 
	}
	/**
	 * 
	 */
	public static int ReleaseVoice()
	{
		VoiceRecordRelease();
		VoicePlayRelease();
		return Code.SUCCESS;
		
	}

	public static void VoicePlayRelease()
	{
		
		if (mplay != null)
		{
			if(mplay.isPlaying())
				mplay.stop();
			mplay.release();
			mplay = null;
		}
		
	}
	
	public static void VoiceRecordRelease()
	{
		if (timerrecorder!= null)
		{
	    	timerrecorder.stopCurrentTime();
	    	timerrecorder = null;
		}
		try {
			if (mrecorder != null)
			{
				// 停止录音
				mrecorder.stop();
				// 释放资源
				mrecorder.release();
				mrecorder = null;
			}
		} catch (IllegalStateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	public static synchronized void PalyFinishCallBack(int code)
	{
		if(play_voice_id != 0 && play_lua_fun_id != 0)
		{			
			MsgPlayFinishData msg_data = new MsgPlayFinishData();
			msg_data.setCode(code);
			msg_data.setVoiceid(play_voice_id);
			msg_data.setLuafunid(play_lua_fun_id);
			MessageMagager.Add(MessageMagager.MSG_PLAY_FINISH, msg_data);
			play_voice_id = 0;
			play_lua_fun_id = 0;
		}
	}
}

