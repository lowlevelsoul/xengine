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

#ifndef __Mem_H__
#define __Mem_H__

#include "core/Platform.h"

typedef struct mem_allocator_s {
    void *      (* malloc )(size_t size);
    void *      (* mallocAligned )(size_t size, size_t align);
    void *      (* realloc )( void * mem, size_t newSize );
    void        (* free )( void* memory );
    size_t      (* get_block_size )( void * memory );
} mem_allocator_t;

typedef struct mem_stats_s {
    uint64_t        numAllocs;
    uint64_t        numFrees;
    uint64_t        sizeAlloc;
} mem_stats_t;

XE_API void Mem_Initialise( mem_allocator_t * allocator );
XE_API void Mem_Finalise(void);
XE_API void * Mem_Alloc( size_t size );
XE_API void * Mem_CAlloc( size_t count, size_t size );
XE_API void * Mem_AllocAligned( size_t size, size_t alignment );
XE_API void Mem_Free( void * mem );
XE_API void Mem_GetStats( mem_stats_t * stats );

#endif
