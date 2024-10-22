package com.kuyue.file;

import java.io.File;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;

import android.app.Activity;
import android.util.Log;

public class FileAsyncHander implements Runnable{
	
	private static final String TAG = "FileAsyncHander";
	private static long total_mem_len = 5 * 1024;
	private static long used_mem_len = 0;
	private static final int sleep_time = 1000;
	
	private  Activity activity = null;
	
	private Vector<FileInfo> unhandle_files =  new Vector<FileInfo>();
	private Vector<FileInfo> handle_files =  new Vector<FileInfo>();
	
	private Vector<FileInfo> temp_unhandle_files = new Vector<FileInfo>();
	private Vector<FileInfo> temp_handle_files = new Vector<FileInfo>();
	
	private Semaphore semp = new Semaphore(0);
	
	private String zip_file_path = "";
	private ZipFile zip_file = null;
	
	private Timer zip_file_close_timer = null;
	private int zip_file_close_tick = 0;
	
	private static int ZIP_FILE_CLOSE_TICK_MAX = 60 * 1000;
	private static int ZIP_FILE_CLOSE_TIMER_PERIOD = 30 * 1000;
	
	private class CloseZipFileTask extends TimerTask {
		public void run()
		{
			if(zip_file_path.length() <= 0)
			{
				return;
			}
			
			zip_file_close_tick += ZIP_FILE_CLOSE_TIMER_PERIOD; 
			if(zip_file_close_tick >= ZIP_FILE_CLOSE_TICK_MAX)
			{				
				FileInfo fileInfo = new FileInfo();
				fileInfo.opType = FileOpType.FOT_CLOSE_ZIP_FILE;
				fileInfo.zipPath = zip_file_path;
				fileInfo.asyncId = 0;
				Add(fileInfo);
			}
		}
	}
	
	public FileAsyncHander(Activity act) {
		this.activity = act;
	}
	
	public final boolean Add(FileInfo fileInfo)
	{
		boolean res = true;
		try 
		{
			unhandle_files.add(fileInfo);
		} catch (Exception e) {
			Log.e(TAG, "RequestRead:put file info to the blocking queue error and error info :" + e.getMessage());
			res = false;
		}
		semp.release();
		return res;
	}

	public final Vector<FileInfo> Get()
	{
		synchronized (handle_files) {
			temp_handle_files = (Vector<FileInfo>) handle_files.clone();
			handle_files.clear();
		}
		return temp_handle_files;
	}
	
	public final  void SetMemLimit(final int mem_limit)
	{
		total_mem_len = mem_limit;
	}
	
	public final synchronized void ReleasedMemSize(final int mem_size)
	{
		total_mem_len = total_mem_len - mem_size;
	}
	
	private final int OpenZipFile(final String path)
	{
		//清空关闭zip文件流计时
		zip_file_close_tick = 0;
		
		int result = FileOpResult.FOR_SUCC;
		if(zip_file_path.equals(path))
			return result;
		zip_file_path = path;
		
		try {
			if(zip_file != null)
			{
				zip_file.close();
				zip_file = null;
			}
			
			zip_file = new ZipFile( new File(path));
			
		} catch (ZipException e) {
			// TODO Auto-generated catch block
			result = FileOpResult.FOR_OPEN_ZIP_ERR;
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			result = FileOpResult.FOR_OPEN_ZIP_ERR;
			e.printStackTrace();
		}
		
		//启动定时器，延迟关闭zip文件流。
		if(zip_file_close_timer == null)
		{
			zip_file_close_timer = new Timer();
			zip_file_close_timer.schedule(new CloseZipFileTask(), ZIP_FILE_CLOSE_TIMER_PERIOD, ZIP_FILE_CLOSE_TIMER_PERIOD);
		}
		
		return result;
	}
	
	private final int CloseZipFile(final String path)
	{
		int result = FileOpResult.FOR_SUCC;
		if(!zip_file_path.equals(path))
			return result;
		zip_file_path = "";
		
		try {
			if(zip_file != null)
			{
				zip_file.close();
				zip_file = null;
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			result = FileOpResult.FOR_CLOSE_ZIP_ERR;
			e.printStackTrace();
		}
		
		return result;
	}
	
	public final boolean Cancel(final int asyncId)
	{
		FileInfo fileInfo = new FileInfo();
		fileInfo.asyncId = asyncId;
		boolean remove_res = unhandle_files.remove(fileInfo);
		return remove_res;
	}
	
	@Override
	public void run() {
		while (true) {
			try {
				
				if(used_mem_len > total_mem_len)
				{
					Thread.sleep(sleep_time);
					continue;
				}
				//获取许可
				if(!semp.tryAcquire(1, TimeUnit.SECONDS))
				{
					continue;
				}
				
				if (unhandle_files.isEmpty())
				{
					continue;
				}
				
				synchronized (unhandle_files) {
					temp_unhandle_files = (Vector<FileInfo>) unhandle_files.clone();
					unhandle_files.clear();
				}
				
				final int queue_size =  temp_unhandle_files.size();
				int result = 0;
				
				for(int i = 0 ; i < queue_size; ++i)
				{
					FileInfo fileInfo = temp_unhandle_files.get(i);
					
					switch(fileInfo.getOpType())
					{
					case FileOpType.FOT_READ :
						boolean readRes = FileHelper.Read(fileInfo,activity);
						if(readRes)
						{
							used_mem_len = used_mem_len + fileInfo.getLen();
							fileInfo.setOpResult(FileOpResult.FOR_SUCC);
						}else
						{
							fileInfo.setOpResult(FileOpResult.FOR_READ_ERR);
						}
						break;
					case FileOpType.FOT_COPY :
						boolean copyRes = FileHelper.Copy(fileInfo,activity);
						if(copyRes)
						{
							fileInfo.setOpResult(FileOpResult.FOR_SUCC);
						}else
						{
							fileInfo.setOpResult(FileOpResult.FOR_COPY_ERR);
						}
						break;
					case FileOpType.FOT_WRITE :
						break;
					case FileOpType.FOT_UNZIP :
						result = OpenZipFile(fileInfo.zipPath);
						if(result == FileOpResult.FOR_SUCC)
						{
							result = FileHelper.Unzip(fileInfo, zip_file);
							fileInfo.setOpResult(result);
						}
						else
						{
							fileInfo.setOpResult(result);
						}
						break;
					case FileOpType.FOT_CLOSE_ZIP_FILE :
						result = CloseZipFile(fileInfo.zipPath);
						fileInfo.setOpResult(result);
						break;
					default :
						fileInfo.setOpResult(FileOpResult.FOR_OP_TYPE_ERR);
						break;
					}
					
					if(fileInfo.getOpType() != FileOpType.FOT_CLOSE_ZIP_FILE)
					{
						handle_files.add(fileInfo);
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
		        Log.e(TAG, "handle file error:"+ e.getMessage());
			}
		}
	}
	
}
