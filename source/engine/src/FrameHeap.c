/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/FrameHeap.h"
#include <stdatomic.h>

typedef struct frame_heap_s {
    void* mem;
    size_t size;
    atomic_size_t offset;
} frame_heap_t;

/*==========================================================================================================*/
frame_heap_t* FrameHeap_Create( void* mem, size_t size ) {
    frame_heap_t* heap = ( frame_heap_t* )mem;
    heap->mem = ( void* )( ( uintptr_t )mem + sizeof( frame_heap_t ) );
    heap->size = size - sizeof( frame_heap_t );
    atomic_init( &heap->offset, 0 );
    return heap;
}

/*==========================================================================================================*/
void FrameHeap_Destroy( frame_heap_t* heap ) {
    ( void )heap;
}

/*==========================================================================================================*/
void* FrameHeap_Alloc( frame_heap_t* heap, size_t size ) {
    /* The offset reserves space only; callers synchronize publication of allocated data. */
    size_t offset = atomic_load_explicit( &heap->offset, memory_order_relaxed );

    for ( ;; ) {
        if ( size > heap->size - offset ) {
            return NULL;
        }

        if ( atomic_compare_exchange_weak_explicit( &heap->offset, &offset, offset + size, memory_order_relaxed,
                                                    memory_order_relaxed ) ) {
            return ( void* )( ( uintptr_t )heap->mem + offset );
        }
    }
}

/*==========================================================================================================*/
void* FrameHeap_Calloc( frame_heap_t* heap, size_t size, size_t count ) {
    size_t total_size = size * count;
    void* ptr = FrameHeap_Alloc( heap, total_size );
    if ( ptr ) {
        memset( ptr, 0, total_size );
    }

    return ptr;
}

/*==========================================================================================================*/
void FrameHeap_Reset( frame_heap_t* heap ) {
    atomic_store_explicit( &heap->offset, 0, memory_order_relaxed );
}
