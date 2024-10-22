#include "stdafx.h"
#include "Common.h"
#include <dbghelp.h>
#include <stdio.h>
#include <intrin.h>
#include "Me_StackWalker.h"

#ifdef _M_IX86
#	define ME_ADDRESS_FORMAT		"0x%.8x"
#else
#	define ME_ADDRESS_FORMAT		"0x%.16x"
#endif // _M_IX86
#define FOLDER_SEPERATOR			'\\'

#pragma comment(lib, "DbgHelp.lib")

// ------------------------------------------------------------------------------------------------
// Me_CStackWalker
// ------------------------------------------------------------------------------------------------
Me_CStackWalker::Me_CStackWalker()
: m_pStackFrame(0)
, m_nCapacity(CALL_STACK_CHUNK_SIZE)
, m_nSize(0)
, m_pTopchunk(0)
, m_nTopindex(0)
{
	m_Store.m_pNext = NULL;
	m_pTopchunk		= &m_Store;
}

Me_CStackWalker::~Me_CStackWalker()
{
	Clear();
}

void Me_CStackWalker::Clear()
{
	m_nSize     = 0;
	m_nCapacity	= CALL_STACK_CHUNK_SIZE;
	m_pTopchunk = &m_Store;
	m_nTopindex = 0;

	Chunk_t* pChunk = m_Store.m_pNext;
	while ( pChunk ) 
	{
		Chunk_t* pTemp	= pChunk;
		
		pChunk	= pTemp->m_pNext;
		delete pTemp;
	}

	m_Store.m_pNext = 0;
	if ( m_pStackFrame != NULL )
	{
		free(m_pStackFrame);
		m_pStackFrame = NULL;
	}
}

size_t Me_CStackWalker::operator[] (size_t nIndex) const
{
	const Chunk_t* pChunk = &m_Store;

	for (size_t i=0; i<nIndex / CALL_STACK_CHUNK_SIZE; ++i)
	{
		pChunk = pChunk->m_pNext;
	}

	return pChunk->m_nFrames[nIndex % CALL_STACK_CHUNK_SIZE];
}

void Me_CStackWalker::push_back(const size_t nCounter)
{
	if ( m_nSize == m_nCapacity )
	{
		Chunk_t* pChunk = new Chunk_t;
		ASSERT(pChunk);
		pChunk->m_pNext = NULL;
		m_pTopchunk->m_pNext = pChunk;
		m_pTopchunk = pChunk;
		m_nTopindex = 0;
		m_nCapacity += CALL_STACK_CHUNK_SIZE;
	}
	else if (m_nTopindex == CALL_STACK_CHUNK_SIZE)
	{
		m_pTopchunk = m_pTopchunk->m_pNext;
		m_nTopindex = 0;
	}

	m_pTopchunk->m_nFrames[m_nTopindex++] = nCounter;
	++m_nSize;
}

void Me_CStackWalker::Walk(size_t nMaxDepth)
{
	__try
	{
		RaiseException(0x1976, 0, 0, NULL);
	}
	__except( Walk((EXCEPTION_POINTERS *)GetExceptionInformation(), nMaxDepth),
		EXCEPTION_CONTINUE_EXECUTION )
	{

	}
}

void Me_CStackWalker::Walk(const _EXCEPTION_POINTERS* pExp, size_t nMaxDepth)
{
	Walk(pExp->ContextRecord, nMaxDepth);
}

void Me_CStackWalker::Walk(const _CONTEXT* pCtx, size_t nMaxDepth)
{
	Clear();
	
	CONTEXT context		= *pCtx;

	STACKFRAME64 sf		= { };
	sf.AddrFrame.Offset	= context.BPREG;
	sf.AddrFrame.Mode	= AddrModeFlat;
	sf.AddrPC.Offset	= context.IPREG;
	sf.AddrPC.Mode		= AddrModeFlat;
	sf.AddrStack.Offset	= context.SPREG;
	sf.AddrStack.Mode	= AddrModeFlat;
	sf.Virtual			= TRUE;

	size_t nCount = 0;
	while (nCount < nMaxDepth) 
	{
		if ( !::StackWalk64( 
			X86X64ARCHITECTURE, 
			::GetCurrentProcess(), 
			::GetCurrentThread(), 
			&sf, 
			&context,
			NULL, 
			::SymFunctionTableAccess64,
			::SymGetModuleBase64,
			NULL) ) 
		{
			break;
		}

		if ( sf.AddrFrame.Offset == 0 ) 
		{
			break;
		}

		push_back((SIZE_T)sf.AddrPC.Offset);
		++nCount;
	}
}

void Me_CStackWalker::FastWalk(size_t nMaxDepth)
{
	Clear();
	
	//size_t* pCounter = NULL;
	//FRAMEPOINTER(pCounter);

	context_t ct;
	CAPTURE_CONTEXT(ct);

	size_t* pCounter = ct.fp;
	size_t nCount = 0;

#if defined(_M_IX86)

	while ( nCount < nMaxDepth )
	{
		if ((size_t*)*pCounter < pCounter)
		{
			break;
		}

		if ((size_t)*pCounter & (sizeof(size_t*) - 1))
		{
			break;
		}

		if ( ::IsBadReadPtr((size_t*)*pCounter, sizeof(size_t*)) )
		{
			break;
		}

		++nCount;

		push_back(*(pCounter + 1));
		pCounter = (size_t*)*pCounter;
	}

#elif defined(_M_X64)

	UINT_PTR* frames = new UINT_PTR[MAX_STACK_Depth];
	ZeroMemory(frames, sizeof(UINT_PTR) * MAX_STACK_Depth);
	
	::RtlCaptureStackBackTrace(0, MAX_STACK_Depth, (PVOID*)frames, NULL);

	size_t nStartIndex = 0;
	while ( nCount < MAX_STACK_Depth )
	{
		if ( frames[nCount] == 0 )
		{
			break;
		}

		if ( frames[nCount] == *(pCounter + 1) )
		{
			nStartIndex = nCount;
		}

		++nCount;
	}

	nCount = nStartIndex;
	while ( nCount < nMaxDepth )
	{
		if ( frames[nCount] == 0 )
		{
			break;
		}

		push_back(frames[nCount]);
		++nCount;
	}
	delete [] frames;

#endif

}

void Me_CStackWalker::SafeWalk(size_t nMaxDepth)
{
	Clear();

	//Walk(nMaxDepth);

	//size_t* pCounter = NULL;
	//FRAMEPOINTER(pCounter);

	//CONTEXT context;
	//context.BPREG	= *pCounter;
	//context.IPREG	= *(pCounter + 1);
	//context.SPREG	= *pCounter;

	//Walk(&context, nMaxDepth);
	
	context_t ct;
	CAPTURE_CONTEXT(ct);

	CONTEXT context = { };
	context.SPREG  = ct.Rsp;
	context.BPREG  = (DWORD64)ct.fp;
	context.IPREG  = ct.Rip;

	Walk(&context, nMaxDepth);
}

//BOOL CALLBACK EnumSymbolsProc(PSYMBOL_INFO pSymInfo, ULONG, PVOID data)
//{
//	if ( pSymInfo->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
//	{
//		pSymInfo->Name;
//	}
//
//	return TRUE;
//}

void Me_CStackWalker::OnDump()
{
	if ( m_nSize == 0 )
	{
		return;
	}

	m_pStackFrame = (Me_CStackFrame*)malloc(sizeof(Me_CStackFrame) * m_nSize);
	ASSERT(m_pStackFrame);
	memset(m_pStackFrame, 0, sizeof(Me_CStackFrame) * m_nSize);

	DWORD			displacement;
	DWORD64			displacement64;

	SYMBOL_INFO*	pSymbolInfo;
	IMAGEHLP_LINE	LineInfo = { 0 };
	BYTE			symbolbuffer[sizeof(SYMBOL_INFO) + (MAX_PATH * sizeof(TCHAR)) - 1] = { 0 };

	bool			bFoundLine = false;

	pSymbolInfo = (SYMBOL_INFO*)&symbolbuffer;
	pSymbolInfo->SizeOfStruct	= sizeof(SYMBOL_INFO);
	pSymbolInfo->MaxNameLen		= MAX_PATH-2;
	LineInfo.SizeOfStruct		= sizeof(IMAGEHLP_LINE64);

	MEMORY_BASIC_INFORMATION mbi;

	HANDLE hCurrentProcess = ::GetCurrentProcess();

	Me_CStackFrame* pFrame = m_pStackFrame;
	for (size_t i=0; i<m_nSize; ++i, ++pFrame)
	{
		size_t nCounter = (*this)[i];

		if ( 0 != ::VirtualQuery((LPCVOID)nCounter, &mbi, sizeof(mbi)) )
		{
			if ( 0 != ::GetModuleFileName((HMODULE)mbi.AllocationBase, pFrame->m_strModule, MAX_PATH) )
			{

			}
		}

		if ( ::SymGetLineFromAddr(hCurrentProcess, nCounter, &displacement, &LineInfo) )
		{
			strcpy_s(pFrame->m_strFile, sizeof(pFrame->m_strFile)-1, LineInfo.FileName);
			pFrame->m_nLine		= LineInfo.LineNumber;
		}

		if ( ::SymFromAddr(hCurrentProcess, nCounter, &displacement64, pSymbolInfo) ) 
		{
			strcpy_s(pFrame->m_strFunc, sizeof(pFrame->m_strFunc)-1, pSymbolInfo->Name);
			pFrame->m_nOffset = (size_t)displacement64;
		}

		pFrame->m_pAddress		= (void*)nCounter;

		//IMAGEHLP_STACK_FRAME sf;
		//memset(&sf, 0, sizeof(sf));
		//sf.InstructionOffset = nCounter;
		//if ( ::SymSetContext(hCurrentProcess, &sf, 0) )
		//{
		//	if ( ::SymEnumSymbols(hCurrentProcess, NULL, NULL, EnumSymbolsProc, this) )
		//	{
		//		
		//	}
		//}
	}
}

// ------------------------------------------------------------------------------------------------
// Me_CStringListWalker
// ------------------------------------------------------------------------------------------------
void Me_CStringListWalker::Dump(std::string& str, size_t nSkip)
{
	const size_t nFirstLen = 60;
	char buff[512];

	OnDump();

	Me_CStackFrame* pFrame = m_pStackFrame;
	for (size_t i=0; i<m_nSize; ++i, ++pFrame)
	{
		if ( i < nSkip )
		{
			continue;
		}
		std::string strStack = "";

		sprintf_s(buff, sizeof(buff), "[%02d] ", i-nSkip+1);
		strStack += buff;

		if ( 0 != strcmp("", pFrame->m_strModule) )
		{
			const char* pStr = strrchr(pFrame->m_strModule, FOLDER_SEPERATOR);
			sprintf_s(buff, sizeof(buff), "%s!", pStr ? pStr + 1 : pFrame->m_strModule);
		}
		else
		{
			sprintf_s(buff, sizeof(buff), "%s!", "UnKnow");
		}
		strStack += buff;

		if ( 0 != strcmp("", pFrame->m_strFunc) )
		{
			sprintf_s(buff, sizeof(buff), "%s", pFrame->m_strFunc);
		}
		else
		{
			sprintf_s(buff, sizeof(buff), "0x%p", pFrame->m_pAddress);
		}
		strStack += buff;

		if ( strStack.length() < nFirstLen )
		{
			strStack.insert(strStack.end(), nFirstLen - strStack.length(), ' ');
		}

		if ( 0 != strcmp("", pFrame->m_strFile) )
		{
			sprintf_s(buff, sizeof(buff), " %s(%d)", pFrame->m_strFile, pFrame->m_nLine);
		}
		else
		{
			sprintf_s(buff, sizeof(buff), " " ME_ADDRESS_FORMAT " (File and line number not available)", 
				pFrame->m_pAddress);
		}
		strStack += buff;

		str += strStack;
		str += "\n";
	}
}

// ------------------------------------------------------------------------------------------------
// Me_CStringLineWalker
// ------------------------------------------------------------------------------------------------
void Me_CStringLineWalker::Dump(std::string& str, size_t nSkip)
{
	char buff[512];

	OnDump();

	Me_CStackFrame* pFrame = m_pStackFrame;
	for (size_t i=0; i<m_nSize; ++i, ++pFrame)
	{
		if ( i < nSkip )
		{
			continue;
		}

		if ( 0 != strcmp("", pFrame->m_strFile) )
		{
			sprintf_s(buff, sizeof(buff), "    %s(%d):", pFrame->m_strFile, pFrame->m_nLine);
		}
		else
		{
			sprintf_s(buff, sizeof(buff), "    " ME_ADDRESS_FORMAT "(File and line number not available):", 
				pFrame->m_pAddress);
		}
		str += buff;

		if ( 0 != strcmp("", pFrame->m_strFunc) )
		{
			sprintf_s(buff, sizeof(buff), " %s", pFrame->m_strFunc);
		}
		else
		{
			sprintf_s(buff, sizeof(buff), " (Function name unavailable)");
		}
		str += buff;

		str += "\n";
	}
}

// ------------------------------------------------------------------------------------------------
// EOF