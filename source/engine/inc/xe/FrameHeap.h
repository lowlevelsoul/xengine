/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_FRAMEHEAP_H
#define XE_FRAMEHEAP_H

#include "xe/Platform.h"

typedef struct frame_heap_s frame_heap_t;

/* The backing memory must be suitably aligned and large enough for the heap header. */
frame_heap_t* FrameHeap_Create( void* mem, size_t size );
void FrameHeap_Destroy( frame_heap_t* heap );

/* Concurrent allocations are safe; callers must synchronize access to allocated memory.
 * Zero-size allocations can return the same address.
 */
void* FrameHeap_Alloc( frame_heap_t* heap, size_t size );
void* FrameHeap_Calloc( frame_heap_t* heap, size_t size, size_t count );
/* Reset and destruction require all users of the heap and its allocations to have finished. */
void FrameHeap_Reset( frame_heap_t* heap );

#endif // XE_FRAMEHEAP_H