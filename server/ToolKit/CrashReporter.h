#pragma once

#include "StackWalker.h"

#define INIT_CRASH_REPORTER(func) CrashReporter::EnableReporter(func);
#define MAX_MESSAGE_BUFFER_LEN 1024 * 10

class CrashReporter
{
public:
	typedef void (*ReportPrinter)(const char* szText);
	static void EnableReporter(ReportPrinter printer);
	static void DisableReporter();
private:
	static void RecvExceptionReason(DWORD ExceptionCode, ULONG_PTR ExceptionInformation[]);

	friend LONG WINAPI ProcCrashHandler(__in struct _EXCEPTION_POINTERS *ExceptionInfo);
	class CrashStackWalker : public StackWalker
	{
	protected:
		virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName) { }
		virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion) { }
		virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr) { }
		virtual void OnOutput(LPCSTR szText)
		{
			AddMessage(szText);
		}
	};

private:
	static void AddMessage(const char* format, ...);
	static ReportPrinter m_printer;
	static char m_message[MAX_MESSAGE_BUFFER_LEN];
	static char* m_messageIndex;
};
