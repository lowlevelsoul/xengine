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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined( _WIN32 )
#   include "core/Platform_win.h"
#elif defined( __APPLE__ )
#   include "core/Platform_apple.h"
#else
#   error Unsupported platform
#endif

#define X_INLINE inline

typedef enum bool_e {
#if defined(__cplusplus) || defined(__OBJC__)
    bool_false=0,
    bool_true,
#else
    false = 0,
    true
#endif
} bool_t;

#ifdef __cplusplus
#   define XE_API extern "C"
#else
#   define XE_API
#endif

#define XE_CALC_OFFSET_PTR( TYPE, PTR, OFFS ) (TYPE) (((uintptr_t)PTR) + OFFS)

#endif
