/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __MEM_H__
#define __MEM_H__

#include "xe/Platform.h"

X_API void Mem_Initialise( void );
X_API void Mem_Finalise( void );
X_API void* Mem_Alloc( size_t size );
X_API void* Mem_AllocAligned( size_t size, size_t alignment );
X_API void* Mem_Realloc( void* ptr, size_t size );
X_API void* Mem_Calloc( size_t size, size_t count );
X_API void Mem_Free( void* ptr );

X_API void Mem_Set( void* ptr, int value, size_t size );
X_API void Mem_Copy( void* dest, const void* src, size_t size );
X_API void Mem_Zero( void* ptr, size_t size );

#define Mem_CallocType( type, count ) ( ( type* )Mem_Calloc( sizeof( type ), count ) )

#endif // __MEM_H__
