// CopyRight(c) 2010, Trans Studio. All rights reserved.

// ----------------------------------------------------------------------------
// Purpose : ∂—’ª
// Create  : 2010.06.18   11:00  –Ï÷æª‘
// ----------------------------------------------------------------------------
#ifndef __Me_StackWalker_h__
#define __Me_StackWalker_h__

#include <string>

#if !(defined(_M_IX86) || defined (_M_X64))
#	error "Can not supported on this architecture."
#endif

struct context_t
{
	UINT_PTR* fp;
#if defined(_M_IX86)
	DWORD32 Rsp;
	DWORD32 Rip;
#elif defined(_M_X64)
	DWORD64 Rsp;
	DWORD64 Rip;
#endif
};

#if defined(_M_IX86)

#	define SIZEOFPTR 4
#	define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_I386
#	define AXREG Eax
#	define BPREG Ebp
#	define IPREG Eip
#	define SPREG Esp
#	define CAPTURE_CONTEXT(context)									\
		context.fp	= ((UINT_PTR*)_AddressOfReturnAddress()) - 1;	\
		context.Rsp = (*context.fp) - 256 * sizeof(void*);			\
		context.Rip	= *(context.fp + 1);
#	define GET_RETURN_ADDRESS(context)  *(context.fp + 1)

#elif defined(_M_X64)

#	define SIZEOFPTR 8
#	define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_AMD64
#	define AXREG Rax
#	define BPREG Rbp
#	define IPREG Rip
#	define SPREG Rsp
#	define CAPTURE_CONTEXT(context)									\
		CONTEXT _ctx;												\
		RtlCaptureContext(&_ctx);									\
		context.Rsp = _ctx.Rsp;										\
		context.Rip = _ctx.Rip;										\
		context.fp	= ((UINT_PTR*)_AddressOfReturnAddress()) - 1;
#	define GET_RETURN_ADDRESS(context)  *(context.fp + 1)
#else
#	error "Can not supported on this architecture."
#endif // _M_IX86

//#if defined(_M_IX86)
//#	define FRAMEPOINTER(fp) __asm mov fp, BPREG
//#elif defined(_M_X64)
//#	define FRAMEPOINTER(fp) fp = ((UINT_PTR*)_AddressOfReturnAddress()) - 1;
//#else
//#	error "Can not supported on this architecture."
//#endif

#define CALL_STACK_CHUNK_SIZE	16
#define MAX_STACK_Depth			255

// ------------------------------------------------------------------------------------------------
// Me_CStackFrame
// ------------------------------------------------------------------------------------------------
struct Me_CStackFrame
{
	char	m_strModule[MAX_PATH];
	char	m_strFunc[MAX_PATH];
	char	m_strFile[MAX_PATH];

	size_t	m_nLine;

	void*	m_pAddress;
	size_t	m_nOffset;

	Me_CStackFrame()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ------------------------------------------------------------------------------------------------
// Me_CStackWalker
// ------------------------------------------------------------------------------------------------
class Me_CStackWalker
{
public:
	Me_CStackWalker();
	virtual ~Me_CStackWalker();

	void Clear();

	void Walk(size_t nMaxDepth = -1);
	void Walk(const _CONTEXT* pCtx, size_t nMaxDepth = -1);
	void Walk(const _EXCEPTION_POINTERS* pExp, size_t nMaxDepth = -1);

	void FastWalk(size_t nMaxDepth = -1);
	void SafeWalk(size_t nMaxDepth = -1);

	Me_CStackFrame* GetFrames()	{ return m_pStackFrame; }
	size_t GetFrameCount()		{ return m_nSize;		}

	//virtual void Dump(std::string& str, size_t nSkip = 1) = 0;

protected:
	void OnDump();

protected:
	Me_CStackFrame*	m_pStackFrame;

protected:
	size_t operator[] (size_t nIndex) const;
	void push_back(const size_t nCounter);

protected:
	struct Chunk_t 
	{
		struct Chunk_t*	m_pNext;
		size_t			m_nFrames[CALL_STACK_CHUNK_SIZE];

		Chunk_t()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	size_t				m_nCapacity;
	size_t				m_nSize;

	Chunk_t				m_Store;
	Chunk_t*			m_pTopchunk;
	size_t				m_nTopindex;
};

// ------------------------------------------------------------------------------------------------
// Me_CStringListWalker
// ------------------------------------------------------------------------------------------------
class Me_CStringListWalker : public Me_CStackWalker
{
public:
	void Dump(std::string& str, size_t nSkip = 1);
};

// ------------------------------------------------------------------------------------------------
// Me_CStringLineWalker
// ------------------------------------------------------------------------------------------------
class Me_CStringLineWalker : public Me_CStackWalker
{
public:
	void Dump(std::string& str, size_t nSkip = 1);
};

// ------------------------------------------------------------------------------------------------
#endif // __Me_StackWalker_h__
// EOF
