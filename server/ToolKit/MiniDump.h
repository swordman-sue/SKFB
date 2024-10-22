#include <DbgHelp.h>

void CreateMiniDump( LPEXCEPTION_POINTERS lpExceptionInfo);

#define ENTER_DUMP_FUNCTION __try {
#define LEAVE_DUMP_FUNCTION }\
	__except(CreateMiniDump(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) {\
}