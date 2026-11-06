/*
==============================================================================================================
    This file is part of XEngine 
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __PLATFORM_WIN_H__
#define __PLATFORM_WIN_H__

/* Note we don't include windows.h here to avoid polluting the global namespace */
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define XE_PLATFORM_WIN 1
#define XE_PLATFORM_NAME "Windows"
#define XE_PLATFORM_DESKTOP 1
#define XE_PATH_SEPARATOR "\\"

#define strcasecmp _stricmp

#define snprintf _snprintf
#define INLINE static inline 

#ifdef __cplusplus
#   define X_API extern "C"
#else
#   define X_API
#endif // __cplusplus   

#endif // __PLATFORM_WIN_H__