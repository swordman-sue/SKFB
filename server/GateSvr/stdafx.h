// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <Windows.h>
#include "../Common/CompileWarning.h"
#include "../ToolKit/Common.h"
#include "GateServer.h"


void MessageOut(LOG_LEVEL eLevel, /*const char *szFuncName,*/ const char *szMsg,...);
#define GATELOG MessageOut