/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __ATOMICSTRING_H__
#define __ATOMICSTRING_H__

#include "xe/Platform.h"
#include "xe/Str.h"

#define ASTR_INVALID -1

typedef int32_t astr_t;

X_API void AStr_Initialise( void );
X_API void AStr_Finalise( void );
X_API astr_t AStr_Set( const char* );
X_API const char* AStr_GetString( astr_t astr );

#endif // __ATOMICSTRING_H__