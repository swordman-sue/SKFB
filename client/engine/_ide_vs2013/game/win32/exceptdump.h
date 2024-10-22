
#ifndef __EXCEPTION_DUMP_H__
#define __EXCEPTION_DUMP_H__

#include <windows.h>
#include <dbghelp.h>


#define DEFAULT_DUMP_TYPE \
	MiniDumpWithIndirectlyReferencedMemory

void start_except_catch();
void set_dump_type(int dump_type = DEFAULT_DUMP_TYPE);
void stop_except_catch();

#endif //__EXCEPTION_DUMP_H__