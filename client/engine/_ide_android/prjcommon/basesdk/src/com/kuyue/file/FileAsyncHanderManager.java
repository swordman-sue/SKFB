package com.kuyue.file;

import java.util.Vector;

import android.app.Activity;
import android.util.Log;

public class FileAsyncHanderManager 
{
	private static final String TAG  = "FileAsyncHanderManager";
	private static FileAsyncHander fileAsyncHander = null;
	private static FileAsyncHanderManager fileAsyncReadManager = null;
	private static Activity activity = null;
	private Thread thread = null;
	private FileAsyncHanderManager(Activity act)
	{
		activity = act;
		fileAsyncHander = new FileAsyncHander(act);
		thread = new Thread(fileAsyncHander);
		thread.start();
	}
	
	public static void initContext(Activity act)
	{
		if (null == fileAsyncReadManager)
		{
			fileAsyncReadManager = new FileAsyncHanderManager(act);
		}
	}
	
	public static boolean Add(FileInfo fileInfo)
	{
		return fileAsyncHander.Add(fileInfo);
	}
	
	public static Vector<FileInfo> Get()
	{
		return fileAsyncHander.Get();
	}
	
	public static boolean Cancel(final int asyncId)
	{
		return fileAsyncHander.Cancel(asyncId);
	}
	public static FileInfo ReadFile(final String path)
	{
		FileInfo fileInfo = new FileInfo(FileOpType.FOT_READ,path);
		boolean res = FileHelper.Read(fileInfo, activity);
		if(!res)
		{
			fileInfo.setOpResult(FileOpResult.FOR_READ_ERR);
		}
		return fileInfo;
	}
	
	public static boolean Copy(final String path,final String destPaht)
	{
		return FileHelper.Copy(path,destPaht,activity);
	}
	
	public static void SetMemLimit(final int mem_limit)
	{
		 fileAsyncHander.SetMemLimit(mem_limit);
	}
	
	public static void ReleasedMemSize(final int mem_size)
	{
		fileAsyncHander.ReleasedMemSize(mem_size);
	}
	
	protected void finalize()
	{
		try {
			thread.join();
		} catch (InterruptedException e) 
		{
			e.printStackTrace();
		}
	}

	
	
}
