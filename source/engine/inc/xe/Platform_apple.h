/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __PLATFORM_APPLE_H__
#define __PLATFORM_APPLE_H__

/* Note we don't include windows.h here to avoid polluting the global namespace */
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <TargetConditionals.h>

#define XE_PLATFORM_APPLE 1
#define XE_PATH_SEPARATOR "/"

#if TARGET_OS_IPHONE
#define XE_PLATFORM_IOS 1
#define XE_PLATFORM_NAME "iOS"
#define XE_PLATFORM_MOBILE 1
#elif TARGET_OS_MAC
#define XE_PLATFORM_MAC 1
#define XE_PLATFORM_NAME "macOS"
#define XE_PLATFORM_DESKTOP 1
#endif

#define INLINE static inline

#ifdef __cplusplus
#define X_API extern "C"
#else
#define X_API
#endif // __cplusplus

#endif // __PLATFORM_APPLE_H__