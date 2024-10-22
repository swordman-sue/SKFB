#pragma once

// #define _DEBUG_API

#define _CHECK_GLOBAL_FUNCTION_REDEFINE

#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
	#define GLOBAL_FUNCTION_CHECKER "__global_function_checker_enabled"
#endif
