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

#include "mem/FrameHeap.h"
#include "core/Id.h"
#include <assert.h>

#define MAGIC MAKE_ID( F, R, A, M, E, H, E, A, P, _, _, _ )
#define FRAME_HEAP_DEFAULT_ALIGNMENT 8

typedef struct frame_heap_s {
    uint64_t        magic;
    uintptr_t       memStart;
    uintptr_t       memEnd;
    uintptr_t       memPtr;
} frame_heap_t;

/*=======================================================================================================================================*/
frame_heap_t * FrameHeap_Create( uintptr_t mem, size_t memSize ) {
    uintptr_t newMemStart = mem + sizeof( frame_heap_t );
    size_t newMemSize = memSize - sizeof( frame_heap_t );
    frame_heap_t * heap = (frame_heap_t *) mem;
    
    heap->magic = MAGIC;
    heap->memStart = newMemStart;
    heap->memEnd = newMemStart + newMemSize;
    heap->memPtr = newMemStart;
    
    return heap;
}

/*=======================================================================================================================================*/
void * FrameHeap_Alloc( frame_heap_t * self_, size_t size ) {
    return FrameHeap_AllocAligned( self_, size, FRAME_HEAP_DEFAULT_ALIGNMENT );
}

/*=======================================================================================================================================*/
void * FrameHeap_AllocAligned( frame_heap_t * self_, size_t size, size_t alignment ) {
    uintptr_t ptr, rem;
    
    assert( self_ != NULL );
    assert( self_->magic == MAGIC );
        
    ptr = self_->memPtr;
    rem = ( ptr % alignment );
    ptr = ( rem == 0 ) ? ptr : ptr + alignment - rem;
        
    assert( ptr + size <= self_->memEnd );
    self_->memPtr = ptr + size;
    
    return (void *) ptr;
}

/*=======================================================================================================================================*/
void FrameHeap_Reset( frame_heap_t * self_ ) {
    assert( self_ != NULL );
    assert( self_->magic == MAGIC );
    self_->memPtr = self_->memStart;
}
