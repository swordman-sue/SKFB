/*
** $Id: lundump.h,v 1.37.1.1 2007/12/27 13:02:25 roberto Exp $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#ifndef lundump_h
#define lundump_h

#include "lobject.h"
#include "lzio.h"

/* load one chunk; from lundump.c */
LUAI_FUNC Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name);

/* make header; from lundump.c */
LUAI_FUNC void luaU_header (char* h);

/* dump one chunk; from ldump.c */
LUAI_FUNC int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip);

#ifdef luac_c
/* print one chunk; from print.c */
LUAI_FUNC void luaU_print (const Proto* f, int full);
#endif

// »ìÏýLUAC_VERSION add by swordman sue 2017-9-7
/* for header of binary files -- this is Lua 5.1 */
#define LUAC_VERSION		0x88

/* for header of binary files -- this is the official format */
#define LUAC_FORMAT		0

/* size of header of binary files */
#define LUAC_HEADERSIZE		18

#define LUAC_SERECT1 0x11
#define LUAC_SERECT2 0x21
#define LUAC_SERECT3 0x31
#define LUAC_SERECT4 0x41
#define LUAC_SERECT5 0x51
#define LUAC_SERECT6 0x61

#endif
