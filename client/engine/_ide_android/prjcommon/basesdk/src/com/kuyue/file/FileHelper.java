package com.kuyue.file;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

import android.content.Context;
import android.util.Log;

public class FileHelper {

	private static final String TAG = "FileHelper";


	public static boolean Read(FileInfo fileInfo,Context context)
	{
		boolean res = true;
		int read_len = 0;
		InputStream inStream = null;
		if(null == fileInfo.getFilePath() || fileInfo.getFilePath().isEmpty())
		{
			fileInfo.setOpResult(FileOpResult.FOR_READ_ERR);
			return false;
		}
		try
		{
			inStream = context.getResources().getAssets().open(fileInfo.getFilePath()); 
			if (null != inStream)
			{ 	
				read_len = inStream.available();
				byte[] inOutb = new byte[read_len]; 
				//读入流,保存在byte数组 
				inStream.read(inOutb);
				fileInfo.setLen(read_len);
				fileInfo.setData(inOutb);
			}else
			{
				fileInfo.setOpResult(FileOpResult.FOR_READ_ERR);
				res = false;
			}
		}catch (Exception e) 
		{
			e.printStackTrace();
			
			if(null != inStream)
			{
				try {
					inStream.close();
					inStream = null;
				} catch (IOException e1) {
					
				}
				
			}
			res = false;
		}finally
		{
			try {
				if(null != inStream)
				{
					inStream.close();
					inStream = null;
				}
			} catch (IOException e1) {
				
			}
		}
		return 	res;
	}
	
	public static boolean Write(FileInfo fileInfo)
	{
		File file = new File(fileInfo.getFilePath());
		FileOutputStream outStream = null;
		try
		{
			if (null != file &&  file.canWrite())
			{
				outStream = new FileOutputStream(file);

				outStream.write(fileInfo.getData());
			}
		}catch (Exception e) 
		{
			if(null != outStream)
			{
				try {
					outStream.close();
					outStream = null;
				} catch (IOException e1) {
					
				}
				
			}
			return false;
		}finally
		{
			try {
				if(null != outStream)
				{
					outStream.close();
					outStream = null;
				}
			} catch (IOException e1) {
				
			}
		}
		return true;
	}
	
	public static boolean Copy(final String filePath, final String destFilePath,Context context)
	{
		boolean ret = true;
		
		if(null == filePath || filePath.isEmpty() || null == destFilePath || destFilePath.isEmpty())
		{
			Log.w(TAG, "filepath or destFilePath is empty : filePath = " + filePath + " destFilePath = " + destFilePath);
			return false;
		}
		
		InputStream inputStream = null ;
		FileOutputStream outputStream = null;
		byte[] buffer = null;
		try
		{
			inputStream = context.getAssets().open(filePath);
			final int read_len= inputStream.available();
			buffer = new byte[read_len]; 
			//读入流,保存在byte数组 
			inputStream.read(buffer);
			
			//拷贝到目标位置
			File file = new File(destFilePath);
			if(!file.exists())
			{
				String parent = file.getParent();
				File parent_path = new File(parent);
				if(!parent_path.exists())
				{
					parent_path.mkdirs();
				}
			}
			outputStream = new FileOutputStream(file);
			outputStream.write(buffer);
		}
		catch(IOException e)
		{
			try{
				if(null != inputStream)
				{
					inputStream.close();
					inputStream = null;
				}
				if(null != outputStream)
				{
					outputStream.close();
					inputStream = null;
				}
				buffer = null;
			}catch(Exception ee)
			{}
			ret = false;
			e.printStackTrace();
		}finally
		{
			try {
				if(null != inputStream)
				{
					inputStream.close();
					inputStream = null;
				}
				if(null != outputStream)
				{
					outputStream.close();
					inputStream = null;
				}
			} catch (IOException e) {
				
			}
			buffer = null;
			System.gc();
		}
		return ret;
	}
	
	public static boolean Copy(FileInfo fileInfo,Context context)
	{
		return Copy(fileInfo.getFilePath(),fileInfo.getDestPath(),context);
	}
	
	public static int Unzip(FileInfo fileInfo, ZipFile zipFile)
	{ 
		int result = FileOpResult.FOR_UNZIP_FILE_INVALID_ERR;
		
		try {
			ZipEntry entry = zipFile.getEntry(fileInfo.filePath);
			if(entry != null)
			{
				File fileOut = new File(fileInfo.destPath);
				if(!fileOut.exists())
				{
					File fileOutParent = new File(fileOut.getParent());
					fileOutParent.mkdirs();
				}
				
				BufferedInputStream inputStream = new BufferedInputStream(zipFile.getInputStream(entry));
				FileOutputStream fileOutputStream = new FileOutputStream(fileOut);

				int uncomoress_size = (int)entry.getSize();
				byte[] buffer = new byte[uncomoress_size];
				
				int read_size = inputStream.read(buffer, 0, uncomoress_size);
				int left_size = uncomoress_size - read_size;
				fileOutputStream.write(buffer, 0, read_size);
				
				while(left_size > 0)
				{
					read_size = inputStream.read(buffer, 0, left_size);
					if(read_size == -1)
						break;
					
					fileOutputStream.write(buffer, 0, read_size);
					left_size = uncomoress_size - read_size;
				}	
							
				fileOutputStream.flush();
				fileOutputStream.close();
				inputStream.close();
				
				buffer = null;
				System.gc();
				
				result = FileOpResult.FOR_SUCC;
			}
			else
			{
				result = FileOpResult.FOR_UNZIP_FILE_INVALID_ERR;
			}
		}
		catch (ZipException e)
		{
			// TODO Auto-generated catch block
			result = FileOpResult.FOR_UNZIP_IO_ERR;
			e.printStackTrace();
		} 
		catch (IOException e)
		{
			// TODO Auto-generated catch block
			result = FileOpResult.FOR_UNZIP_IO_ERR;
			e.printStackTrace();
		} 

		return result;
	}
	
}
