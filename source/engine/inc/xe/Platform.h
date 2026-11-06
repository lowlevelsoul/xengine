/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined( WIN32 ) || defined( _WIN32 )
#include "Platform_win.h"
#elif defined( __APPLE__ )
#include "Platform_apple.h"
#else
#error Unsupported platform
#endif

#endif