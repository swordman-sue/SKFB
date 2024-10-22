#pragma once

#include "CriticalSection.h"
#include <list>
#include <string>


enum LOG_LEVEL
{
	DEBUG_LEVEL,			//调试日志
	RELEASE_LEVEL,			//发布之后的日志
	ERROR_LEVEL,			//错误日志
	WARNING_LEVEL,			//警告日志
	GUI_LEVEL,				//显示在GUI上的日志
	IMPORTANT_LEVEL,		//重要日志
	SPECAIL_LEVEL,			//特殊日志(只记录在文件方便定位问题)
};

unsigned int __stdcall FileLogThread(void* pParam);

class LogFile
{
	friend unsigned int __stdcall FileLogThread(void* pParam);
public:
	LogFile(void);
	~LogFile(void);

public:
	//szFilePath表示文件存放的路径名，szFileName表示文件名不需要后缀
	BOOL			Init(char* szFilePath,char* szFileName,LOG_LEVEL eLevel);
	VOID			InsertLog(LOG_LEVEL eLevel, const char* szLog, const char* szFuncName = NULL);
	VOID			InsertFormatLog(LOG_LEVEL eLevel, const char* pszFormat, ...);
	VOID			ShutDown() { m_bShutDown = TRUE; if(m_hWriteHandle) SetEvent(m_hWriteHandle); }

private:
	VOID			WriteLog();
	BOOL			CreateFileThread();
	BOOL			CreateLogFile();
	std::string		GetCurrTime();

private:
	char						m_FilePath[256];
	char						m_FileName[256];

	HANDLE						m_hWriteHandle;			//event同步信号量
	HANDLE						m_hThreadHandle;		//线程句柄
	HANDLE						m_hFile;				//文件句柄
	BOOL						m_bShutDown;

	CriticalSection				m_cs;
	std::list<std::string>*		m_InputList;
	std::list<std::string>*		m_OutputList;

	LOG_LEVEL					m_eLevel;				//日志等级
	DWORD						m_dwCurFileSize;		//当前文件长度
};
