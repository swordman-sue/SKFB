// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <Windows.h>
#include "../Common/CompileWarning.h"
#	if defined(_M_IX86)	
#		pragma comment(lib,"../MySql/lib/libmysql.lib")
#	elif defined(_M_X64)
#		pragma comment(lib,"../MySql/lib/libmysql64.lib")
#	else	
#		error "Can not supported on this architecture."
#	endif	