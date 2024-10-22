#include "stdafx.h"
#include "MiniDump.h"
#include <Windows.h>
#include <string>
#include "Common.h"

#pragma comment(lib, "DbgHelp.lib")

void CreateMiniDump( LPEXCEPTION_POINTERS lpExceptionInfo) {
	// Open a file	
	char szFileName[128] = {0};
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(szFileName,"MiniDump_%d-%d-%d-%d-%d-%d.dmp",
		sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

	HANDLE hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile != NULL &&  hFile != INVALID_HANDLE_VALUE ) {

		// Create the minidump 
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId          = GetCurrentThreadId();
		mdei.ExceptionPointers = lpExceptionInfo;
		mdei.ClientPointers    = FALSE;

		MINIDUMP_TYPE mdt      = MiniDumpNormal;
		BOOL retv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
			hFile, mdt, ( lpExceptionInfo != 0 ) ? &mdei : 0, 0, 0);

		if ( !retv ) {
			printf("MiniDumpWriteDump failed. Error: %u \n", GetLastError() ); 
		} else { 
			printf("dump created[%s].\n", szFileName); 
		}

		// Close the file
		CloseHandle( hFile );

#ifdef _DEBUG
   		ASSERT(0);
#endif

	} else {
		printf("CreateFile failed. Error: %u \n", GetLastError() ); 
	}
}

// void CreateMiniDump( LPEXCEPTION_POINTERS lpExceptionInfo) {
// 	// Open a file
// 	HANDLE hFile = CreateFile(_T("MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE,
// 		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
// 
// 	if ( hFile != NULL &&  hFile != INVALID_HANDLE_VALUE ) {
// 
// 		// Create the minidump 
// 		MINIDUMP_EXCEPTION_INFORMATION mdei;
// 		mdei.ThreadId          = GetCurrentThreadId();
// 		mdei.ExceptionPointers = lpExceptionInfo;
// 		mdei.ClientPointers    = FALSE;
// 
// 		MINIDUMP_TYPE mdt      = MiniDumpNormal;
// 		BOOL retv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
// 			hFile, mdt, ( lpExceptionInfo != 0 ) ? &mdei : 0, 0, 0);
// 
// 		if ( !retv ) {
// 			_tprintf( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() ); 
// 		} else { 
// 			_tprintf( _T("Minidump created.\n") ); 
// 		}
// 
// 		// Close the file
// 		CloseHandle( hFile );
// 
// 	} else {
// 		_tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
// 	}
// }