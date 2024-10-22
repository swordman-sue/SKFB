#include "StdAfx.h"
#include "CrashReporter.h"
#include <stdio.h>

const char* CRASH_BEGIN_INFO	= "\r\n*------------------------------CRASH BEGIN------------------------------*\r\n";
const char* CRASH_END_INFO		= "*------------------------------CRASH END------------------------------*\r\n";

static LONG WINAPI ProcCrashHandler(__in struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	CrashReporter::AddMessage(CRASH_BEGIN_INFO);
	CrashReporter::CrashStackWalker sw;
	sw.ShowCallstack(GetCurrentThread(), ExceptionInfo->ContextRecord);
	CrashReporter::AddMessage("\n");

	PEXCEPTION_RECORD exceptionRecord = ExceptionInfo->ExceptionRecord;
	CrashReporter::RecvExceptionReason(exceptionRecord->ExceptionCode, exceptionRecord->ExceptionInformation);
	CrashReporter::AddMessage("\n");
	CrashReporter::AddMessage(CRASH_END_INFO);

	CrashReporter::m_printer(CrashReporter::m_message);
	return EXCEPTION_CONTINUE_SEARCH;
}

CrashReporter::ReportPrinter CrashReporter::m_printer = NULL;
char* CrashReporter::m_messageIndex = CrashReporter::m_message;
char CrashReporter::m_message[MAX_MESSAGE_BUFFER_LEN];

void CrashReporter::EnableReporter(ReportPrinter printer)
{
	m_printer = printer;
	memset(m_message, 0, MAX_MESSAGE_BUFFER_LEN);
	m_messageIndex = m_message;
	SetUnhandledExceptionFilter(ProcCrashHandler);
}

void CrashReporter::DisableReporter()
{
	m_printer = NULL;
	memset(m_message, 0, MAX_MESSAGE_BUFFER_LEN);
	m_messageIndex = m_message;
	SetUnhandledExceptionFilter(NULL);
}

void CrashReporter::AddMessage(const char* format, ...)
{
	size_t size = MAX_MESSAGE_BUFFER_LEN - (m_messageIndex - m_message);
	if (size <= 0)
		return;

	va_list args;
	va_start(args, format);
	int writtenSize = vsnprintf(m_messageIndex, size, format, args);
	va_end(args);

	if (writtenSize >= 0)
		m_messageIndex += writtenSize;
	else
		m_messageIndex = m_message + MAX_MESSAGE_BUFFER_LEN;
	m_message[MAX_MESSAGE_BUFFER_LEN - 1] = 0;
}

void CrashReporter::RecvExceptionReason(DWORD ExceptionCode, ULONG_PTR ExceptionInformation[])
{
	switch (ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		AddMessage("ACCESS VIOLATION: Addr(0x%p): ", ExceptionInformation[1]);
		if (ExceptionInformation[0] == 0)
			AddMessage("The thread attempted to read the inaccessible data.");
		else if (ExceptionInformation[0] == 1)
			AddMessage("The thread attempted to write to an inaccessible address.");
		else if (ExceptionInformation[0] == 8)
			AddMessage("The thread causes a user-mode data execution prevention (DEP) violation.");
		else
			AddMessage("Unknown exception.");
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		AddMessage("ARRAY BOUNDS EXCEEDED: The thread tried to access an array element "
			"that is out of bounds and the underlying hardware supports bounds checking.");
		break;
	case EXCEPTION_BREAKPOINT:
		AddMessage("BREAKPOINT: A breakpoint was encountered.");
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		AddMessage("DATATYPE MISALIGNMENT: The thread tried to read or write data "
			"that is misaligned on hardware that does not provide alignment.");
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		AddMessage("FLOAT DENORMAL OPERAND: One of the operands in a floating-point operation is denormal. "
			"A denormal value is one that is too small to represent as a standard floating-point value.");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		AddMessage("FLOAT DIVIDE BY ZERO: The thread tried to divide a floating-point value "
			"by a floating-point divisor of zero.");
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		AddMessage("FLOAT INEXACT RESULT: The result of a floating-point operation "
			"cannot be represented exactly as a decimal fraction.");
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		AddMessage("FLOAT INVALID OPERATION: Unknown float exception.");
		break;
	case EXCEPTION_FLT_OVERFLOW:
		AddMessage("FLOAT OVERFLOW: The exponent of a floating-point operation "
			"is greater than the magnitude allowed by the corresponding type.");
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		AddMessage("FLOAT STACK CHECK: The stack overflowed or underflowed "
			"as the result of a floating-point operation.");
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		AddMessage("FLOAT UNDERFLOW: The exponent of a floating-point operation "
			"is less than the magnitude allowed by the corresponding type.");
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		AddMessage("ILLEGAL INSTRUCTION: The thread tried to execute an invalid instruction.");
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		AddMessage("PAGE ERROR: Addr(0x%p): ", ExceptionInformation[1]);
		if (ExceptionInformation[0] == 0)
			AddMessage("The thread attempted to read the inaccessible data.");
		else if (ExceptionInformation[0] == 1)
			AddMessage("The thread attempted to write to an inaccessible address.");
		else if (ExceptionInformation[0] == 8)
			AddMessage("The thread causes a user-mode data execution prevention (DEP) violation.");
		else
			AddMessage("Unknown exception.");
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		AddMessage("INT DIVIDE BY ZERO: The thread tried to divide an integer value by an integer divisor of zero.");
		break;
	case EXCEPTION_INT_OVERFLOW:
		AddMessage("INT OVERFLOW: The result of an integer operation caused a carry out of "
			"the most significant bit of the result.");
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		AddMessage("NONCONTINUABLE EXCEPTION: The thread tried to continue execution after a "
			"noncontinuable exception occurred.");
		break;
	case EXCEPTION_PRIV_INSTRUCTION: 
		AddMessage("PRIV INSTRUCTION: The thread tried to execute an instruction "
			"whose operation is not allowed in the current machine mode.");
		break;
	case EXCEPTION_SINGLE_STEP:
		AddMessage("SINGLE STEP: A trace trap or other single-instruction mechanism signaled "
			"that one instruction has been executed.");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		AddMessage("STACK OVERFLOW: The thread used up its stack.");
		break;
	default:
		AddMessage("Unknown exception code.");
		break;
	}
}
