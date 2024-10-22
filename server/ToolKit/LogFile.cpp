#include "stdafx.h"
#include "LogFile.h"
#include <process.h>
#include <stdio.h>

unsigned int __stdcall FileLogThread(void* pParam)
{
	LogFile* pLogFile = (LogFile*)pParam;
	pLogFile->WriteLog();

	return 0;
}

// 格式化当前时间
// void FormatTimer(LPSTR lpBuffer)
// {
// 	time_t t;
// 	time(&t);
// 	tm newTime;
// 
// 	localtime_s(&newTime,&t);
// 
// 	wsprintfA(lpBuffer,"[%d %d:%d:%d] ",newTime.tm_mday,newTime.tm_hour,newTime.tm_min,newTime.tm_sec);
// }



LogFile::LogFile(void)
:	m_hWriteHandle(NULL),
	m_hThreadHandle(NULL),
	m_bShutDown(FALSE),
	m_hFile(INVALID_HANDLE_VALUE),
	m_dwCurFileSize(0)
{
	m_InputList = new std::list<std::string>;
	m_OutputList = new std::list<std::string>;
}

LogFile::~LogFile(void)
{
	if(m_hThreadHandle)
		WaitForSingleObject(m_hThreadHandle,INFINITE);

	if(m_hThreadHandle)
		CloseHandle(m_hThreadHandle);

	if(m_hWriteHandle)
		CloseHandle(m_hWriteHandle);

	if(m_hFile!=INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);

	if(m_InputList)
		delete m_InputList;

	if(m_OutputList)
		delete m_OutputList;
}

BOOL LogFile::Init(char* szFilePath,char* szFileName,LOG_LEVEL eLevel)
{
	if(!szFilePath||!szFileName)
		return FALSE;

	m_dwCurFileSize = 0;
	strncpy(m_FilePath,szFilePath,sizeof(m_FilePath));
	strncpy(m_FileName,szFileName,sizeof(m_FileName));

	CreateDirectory((LPCTSTR)m_FilePath,NULL);
	m_eLevel = eLevel;
	m_hWriteHandle = CreateEvent(NULL,FALSE,FALSE,NULL);

	CreateFileThread();

	return TRUE;
}

VOID LogFile::InsertLog(LOG_LEVEL eLevel, const char*szLog, const char* szFuncName)
{
	if(m_bShutDown)
		return;
	if ( eLevel < m_eLevel )
	{
		return;
	}

	std::string MyMsg;

	MyMsg = "[";
	if (eLevel == ERROR_LEVEL)
		MyMsg += "ERR -- ";
	else if (eLevel == RELEASE_LEVEL)
		MyMsg += "REL -- ";
	else if (eLevel == DEBUG_LEVEL)
		MyMsg += "DEG -- ";
	else if (eLevel == GUI_LEVEL)
		MyMsg += "GUI -- ";

	if (szFuncName)
		MyMsg += szFuncName;

	MyMsg += GetCurrTime();	
	MyMsg += "]  ";
	MyMsg += szLog;
	MyMsg += "\r\n";

	m_cs.Lock();

	m_InputList->push_back(MyMsg);
	SetEvent(m_hWriteHandle);

	m_cs.Unlock();
}

VOID LogFile::InsertFormatLog(LOG_LEVEL eLevel, const char* pszFormat, ...)
{
	if (pszFormat)
	{
		const int MAX_SIZE = 1024;
		char szBuffer[MAX_SIZE] = "";
		va_list args;
		va_start(args, pszFormat);
		vsnprintf(szBuffer, MAX_SIZE, pszFormat, args);
		va_end(args);

		InsertLog(eLevel, szBuffer);
	}
}

VOID LogFile::WriteLog()
{

	CreateLogFile();

	while(TRUE)
	{
		DWORD dwRet = WaitForSingleObject(m_hWriteHandle,INFINITE);
		if(dwRet==WAIT_FAILED)
			break;

		m_cs.Lock();
		
		std::list<std::string>*m_pTempList = m_InputList;
		m_InputList = m_OutputList;
		m_OutputList = m_pTempList;

		m_cs.Unlock();

		// 检测如果日志文件到达指定大小(500M = 1024 * 1024 * 500 = 524288000),则新建文件
		if (m_dwCurFileSize >= 524288000)
		{
			CreateLogFile();
			m_dwCurFileSize = 0;
		}

		//遍历list
		DWORD dwWriteCnt;
		SetFilePointer(m_hFile,0,NULL,FILE_END);
		std::list<std::string>::iterator it;
		for (it = m_OutputList->begin();it!=m_OutputList->end();++it)
		{
			DWORD dwLength = (DWORD)((*it).length());
			m_dwCurFileSize += dwLength;
			WriteFile(m_hFile, (*it).c_str(), dwLength, &dwWriteCnt, NULL);
		}
		m_OutputList->clear();

		if(m_bShutDown)
		{
			//如果关闭线程，必须把输入缓存区中的数据写入到log中
			m_cs.Lock();

			std::list<std::string>*m_pTempList = m_InputList;
			m_InputList = m_OutputList;
			m_OutputList = m_pTempList;

			m_cs.Unlock();

			//遍历list
			DWORD dwWriteCnt;
			SetFilePointer(m_hFile,0,NULL,FILE_END);
			std::list<std::string>::iterator it;
			for (it = m_OutputList->begin();it!=m_OutputList->end();++it)
			{
				WriteFile(m_hFile,(*it).c_str(),(DWORD)((*it).length()),&dwWriteCnt,NULL);
			}
			m_OutputList->clear();

			break;

		}

	}
}

BOOL LogFile::CreateFileThread()
{
	unsigned int dwThreadID;
	m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, FileLogThread, (void*)this, 0, &dwThreadID);
	if(m_hThreadHandle == NULL)
		return false;

	return true;
}

BOOL LogFile::CreateLogFile()
{
	if(m_hFile!=INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);

	std::string strFileName(m_FilePath);
	
	strFileName += "\\";
	strFileName += m_FileName;
	strFileName += "-";
	strFileName += GetCurrTime();
	strFileName += ".log";

	m_hFile = CreateFile( strFileName.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

std::string LogFile::GetCurrTime()
{
	char szTime[128];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(szTime,"%d-%d-%d-%d-%d-%d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
	return szTime;
}
