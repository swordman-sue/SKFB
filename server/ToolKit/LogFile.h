#pragma once

#include "CriticalSection.h"
#include <list>
#include <string>


enum LOG_LEVEL
{
	DEBUG_LEVEL,			//������־
	RELEASE_LEVEL,			//����֮�����־
	ERROR_LEVEL,			//������־
	WARNING_LEVEL,			//������־
	GUI_LEVEL,				//��ʾ��GUI�ϵ���־
	IMPORTANT_LEVEL,		//��Ҫ��־
	SPECAIL_LEVEL,			//������־(ֻ��¼���ļ����㶨λ����)
};

unsigned int __stdcall FileLogThread(void* pParam);

class LogFile
{
	friend unsigned int __stdcall FileLogThread(void* pParam);
public:
	LogFile(void);
	~LogFile(void);

public:
	//szFilePath��ʾ�ļ���ŵ�·������szFileName��ʾ�ļ�������Ҫ��׺
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

	HANDLE						m_hWriteHandle;			//eventͬ���ź���
	HANDLE						m_hThreadHandle;		//�߳̾��
	HANDLE						m_hFile;				//�ļ����
	BOOL						m_bShutDown;

	CriticalSection				m_cs;
	std::list<std::string>*		m_InputList;
	std::list<std::string>*		m_OutputList;

	LOG_LEVEL					m_eLevel;				//��־�ȼ�
	DWORD						m_dwCurFileSize;		//��ǰ�ļ�����
};
