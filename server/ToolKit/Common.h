#pragma once

#include <crtdbg.h>

#undef ASSERT
// 不使用 _DEBUG 而是通过 NDEBUG 来控制 ASSERT
#ifdef NDEBUG
#	define ASSERT(x) ((void)0)
#else
#	define ASSERT(x) _ASSERT((x))
#endif

// #define CHECK_POINTER(p) if((p) == NULL) { DebugMsgOutA(CON_ERROR, "指针为空[%s:%u]", __FUNCTION__, __LINE__); return; }
// #define CHECK_POINTER_RET(p, ret) if((p) == NULL) { DebugMsgOutA(CON_ERROR, "指针为空[%s:%u]", __FUNCTION__, __LINE__); return (ret); }

#define IF_NOT(x) if( !(x) ? ( ASSERT(!(#x)), 1 ) : 0 )
#define IF_OK(x) if( (x) ? 1 : ( ASSERT(!(#x)), 0 ) )
#define DEAD_LOOP_BREAK(i, maxValue) { if (++(i) > (maxValue)) { ASSERT(!"Dead Lock"); break; } }
#define SAFE_DELETE(p) { if(p) { try{ delete p; }catch(...){ ASSERT(!"Catch: SAFE_DELETE()"); } p = 0; } }
#define SAFE_RELEASE(p) { if(p) { try{ p->Release(); }catch(...){ ASSERT(!"Catch: SAFE_RELEASE()"); } p = 0; } }
#define	MAKEDWORD(a, b) ((DWORD)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))

template<int> struct __StaticAssertHelper;
template<> struct __StaticAssertHelper<true> {};
#define STATIC_ASSERT(expr) { __StaticAssertHelper<((expr) != 0)> Static_Assert; (void)Static_Assert; } 
