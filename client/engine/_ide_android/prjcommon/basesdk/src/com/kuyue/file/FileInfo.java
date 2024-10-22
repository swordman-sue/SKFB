package com.kuyue.file;

class FileOpType
{
	public static final int FOT_INVALID = 0;
	public static final int FOT_READ = 1;
	public static final int FOT_WRITE =2; 
	public static final int FOT_COPY = 3;
	public static final int FOT_VERSION_FILE_LOAD = 4;
	public static final int FOT_VERSION_FILE_WRITE = 5;
	public static final int FOT_SETTING_FILE_WRITE = 6;
	public static final int FOT_SCRIPT_IOS_UNZIP = 7;
	public static final int FOT_UNZIP = 8;
	public static final int FOT_CLOSE_ZIP_FILE = 9;
	
	public static final int FOT_COUNT = 10;
} 

class FileOpResult
{
	public static final int FOR_SUCC = 0;
	public static final int FOR_OP_TYPE_ERR = 1;
	public static final int FOR_READ_ERR = 2;
	public static final int FOR_WRITE_ERR = 3;
	public static final int FOR_COPY_ERR = 4;
	public static final int FOR_VERSION_FILE_LOAD_ERR = 5;
	public static final int FOR_VERSION_FILE_WRITE_ERR = 6;
	public static final int FOR_SETTING_FILE_WRITE_ERR = 7;
	public static final int FOR_SCRIPT_IOS_ERR = 8;
	public static final int FOR_UNZIP_ERR = 9;
	public static final int FOR_UNZIP_FILE_INVALID_ERR = 10;
	public static final int FOR_UNZIP_IO_ERR = 11;
	public static final int FOR_UNZIP_FILENOTFOUND_ERR = 12;
	public static final int FOR_OPEN_ZIP_ERR = 13;
	public static final int FOR_CLOSE_ZIP_ERR = 14;
	
	public static final int FOR_COUNT = 15;
}

public class FileInfo
{

	 int    opResult = FileOpResult.FOR_SUCC;
	 int    opType;
	 byte[] data = null;
	 int    len = 0;
	 String filePath;
	 String destPath;
	 int    asyncId;
	 String zipPath;
	
	@Override
	public boolean equals(Object obj)
	{
		if(!(obj instanceof FileInfo)) 
		{
			return false; 
		}
		if(this == obj)
		{
			return true;
		}
		FileInfo f = (FileInfo)obj; 
		return  f.asyncId == this.asyncId; 
	}
	 
	public FileInfo() {
		super();
	}
	
	public FileInfo(int opType, String filePath) {
		super();
		this.opType = opType;
		this.filePath = filePath;
	}

	public FileInfo(int opType, String filePath, int asyncId) {
		super();
		this.opType = opType;
		this.filePath = filePath;
		this.asyncId = asyncId;
	}
	
	public FileInfo(int opType, int asyncId, String filePath, String destPath) {
		super();
		this.opType = opType;
		this.asyncId = asyncId;
		this.filePath = filePath;
		this.destPath = destPath;
	}
	
	public FileInfo(int opType, int asyncId, String filePath, String destPath, String zipPath) {
		super();
		this.opType = opType;
		this.asyncId = asyncId;
		this.filePath = filePath;
		this.destPath = destPath;
		this.zipPath = zipPath;
	}
	
	public int getOpResult() {
		return opResult;
	}
	public void setOpResult(int opResult) {
		this.opResult = opResult;
	}
	public String getFilePath() {
		return filePath;
	}
	public void setFilePath(String filePath) {
		this.filePath = filePath;
	}
	public byte[] getData() {
		return data;
	}
	public void setData(byte[] data) {
		this.data = data;
	}
	public int getLen() {
		return len;
	}
	public void setLen(int len) {
		this.len = len;
	}
	public int getOpType() {
		return opType;
	}
	public void setOpType(int opType) {
		this.opType = opType;
	}
	public String getDestPath() {
		return destPath;
	}
	public void setDestPath(String destPath) {
		this.destPath = destPath;
	}
	public int getAsyncId() {
		return asyncId;
	}
	public void setAsyncId(int asyncId) {
		this.asyncId = asyncId;
	}
	public String getZipPath() {
		return zipPath;
	}
	public void setZipPath(String zipPath) {
		this.zipPath = zipPath;
	}
}


