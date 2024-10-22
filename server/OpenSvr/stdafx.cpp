// stdafx.cpp : source file that includes just the standard includes
// OpenSvr.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void MessageOut(LOG_LEVEL eLevel, const char *szMsg,...)
{
	const int MAX_LOG_LEN = 10240;
	char strBuffer[MAX_LOG_LEN]; 

	va_list args; 
	va_start(args, szMsg); 
	_vsnprintf( strBuffer, MAX_LOG_LEN, szMsg, args ); 
	va_end(args); 
	strBuffer[MAX_LOG_LEN - 1] = 0;

	g_OpenServer.writeLog(eLevel,strBuffer);
}