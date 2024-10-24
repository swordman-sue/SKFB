/* Minimal - the simplest thing that could possibly work
 * Copyright (C) 2007  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MINIMAL_STDLIB_H
#define MINIMAL_STDLIB_H

#ifdef __i386__
#define _breakpoint() \
    __asm__ ("int $0x03")
#else
#define _breakpoint()
#endif

#ifdef __cplusplus
#define _assert_(e) \
    throw e
#else
#define _assert_(e) \
    exit(1)
#endif

#define _assert(expr) \
    do if (!(expr)) { \
        fprintf(stderr, "%s(%u): _assert(%u:%s)\n", __FILE__, __LINE__, errno, #expr); \
        _breakpoint(); \
        _assert_(#expr); \
    } while (false)

#define _syscall(expr) \
    do if ((long) (expr) != -1) \
        break; \
    else switch (errno) { \
        case EINTR: \
            continue; \
        default: \
            _assert(false); \
    } while (true)

#define _forever \
    for (;;)

#define _trace() \
    fprintf(stderr, "_trace(%s:%u): %s\n", __FILE__, __LINE__, __FUNCTION__)

#define _not(type) \
    ((type) ~ (type) 0)

#define _disused \
    __attribute__((unused))

#define _label__(x) _label ## x
#define _label_(y) _label__(y)
#define _label _label_(__LINE__)

#define _packed \
    __attribute__((packed))

#ifdef __cplusplus

template <typename Type_>
struct Iterator_ {
    typedef typename Type_::const_iterator Result;
};

#define _foreach(item, list) \
    for (bool _stop(true); _stop; ) \
        for (const __typeof__(list) &_list = (list); _stop; _stop = false) \
            for (Iterator_<__typeof__(list)>::Result item = _list.begin(); item != _list.end(); ++item)

#define _foreach2(item, list) \
	for (bool _stop(true); _stop; ) \
		for (const __typeof__(list) &_list = (list); _stop; _stop = false) \
			for (item = _list.begin(); item != _list.end(); ++item)

#endif

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#endif/*MINIMAL_STDLIB_H*/
