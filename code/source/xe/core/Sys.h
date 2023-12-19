/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#ifndef __SYS_H__
#define __SYS_H__

#include "core/Platform.h"

typedef struct sys_mutex_s {
    uint64_t data[8];
} sys_mutex_t;

#define Sys_Align(V, A) ((V )% (A) == 0 ) ? (V) : (V) + ((A) - ((V) % (A)))

typedef void (*sys_print_listener_t)( const char * buff, void * context );

XE_API void Sys_Initialise(void);
XE_API void Sys_Finalise(void);
XE_API uint64_t Sys_GetTicks(void);

XE_API void Sys_Printf( const char * fmt, ... );
XE_API void Sys_AssertPrintf( const char * file, int line, const char * fmt, ... );

XE_API void Sys_Breakpoint(void);
XE_API void Sys_Exit( int code );

XE_API void Sys_MutexCreate( sys_mutex_t * self_ );
XE_API void Sys_MutexDestroy( sys_mutex_t * self_ );
XE_API void Sys_MutexLock( sys_mutex_t * self_ );
XE_API void Sys_MutexTryLock( sys_mutex_t * self_ );
XE_API void Sys_MutexUnlock( sys_mutex_t * self_ );

#if defined( DEBUG ) || defined( _DEBUG ) || defined( XENGINE_TOOLS )
#   define xassert(C) (void)((C) || ( Sys_AssertPrintf( __FILE__, __LINE__, #C), Sys_Breakpoint(), 0))
#   define xassertmsg(C, ...) (void)((C) || ( Sys_AssertPrintf( __FILE__, __LINE__, __VA_ARGS__ ), Sys_Breakpoint(), 0))
#   define xerror(C, ...) (void)((!(C)) || ( Sys_AssertPrintf( __FILE__, __LINE__, __VA_ARGS__ ), Sys_Exit( 0 ), 0 ) )
#   define xprintf( ... ) Sys_Printf( __VA_ARGS__ )
#else
#   define xassert(C)
#   define xassertmsg(C, ...)
#   define xerror(C, ...)
#   define xprintf( ... )
#endif



#endif
