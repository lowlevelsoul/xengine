/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "Mem.h"
#include <stdlib.h>
#include <string.h>

#if XE_PLATFORM_WIN
#include <malloc.h>
#endif

/*==========================================================================================================*/
void Mem_Initialise() { }

/*==========================================================================================================*/
void Mem_Finalise() { }

/*==========================================================================================================*/
void Mem_Set( void* ptr, int value, size_t size ) {
    memset( ptr, value, size );
}

/*==========================================================================================================*/
void Mem_Zero( void * ptr, size_t size ) {
    memset( ptr, 0, size );
}

/*==========================================================================================================*/
void Mem_Copy( void* dest, const void* src, size_t size ) {
    memcpy( dest, src, size );
}

/*==========================================================================================================*/
void* Mem_Alloc( size_t size ) {
    return Mem_AllocAligned( size, 16 );
}

/*==========================================================================================================*/
void* Mem_AllocAligned( size_t size, size_t alignment ) {
#if XE_PLATFORM_WIN
    return _aligned_malloc( size, alignment );
#else
    void* ptr = NULL;
    if ( posix_memalign( &ptr, alignment, size ) != 0 ) {
        return NULL;
    }
    return ptr;
#endif
}

/*==========================================================================================================*/
void* Mem_Realloc( void* ptr, size_t size ) {
#if XE_PLATFORM_WIN
    return _aligned_realloc( ptr, size, 16 );
#else
    return realloc( ptr, size );
#endif
}

/*==========================================================================================================*/
void Mem_Free( void* ptr ) {
#if XE_PLATFORM_WIN
    _aligned_free( ptr );
#else
    free( ptr );
#endif
}