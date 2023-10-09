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

#include "core/Platform.h"
#include "core/Sys.h"
#include "mem/Mem.h"
#include "core/CVar.h"
#include "core/Bsearch.h"
#include "core/Array.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include "tlsf/tlsf.h"
//#include <pthread.h>
//#include <malloc/malloc.h>

typedef struct mem_s {
    mem_allocator_t     defaultAllocator;
    mem_allocator_t *   allocator;
    sys_mutex_t         mutex;
    uint64_t            numAllocs;
    uint64_t            numFrees;
    uint64_t            memSizeAlloc;
} mem_t;

mem_t mem;
bool_t memInit = false;

/*=======================================================================================================================================*/
void * default_malloc( size_t size ) {
    return malloc( size );
}

/*=======================================================================================================================================*/
void * default_mallocAligned(size_t size, size_t align) {
    return malloc( size );
}

/*=======================================================================================================================================*/
void * default_realloc( void * mem, size_t newSize ) {
    return realloc( mem, newSize );
}

/*=======================================================================================================================================*/
void default_free( void * memory ) {
    free( memory );
}

/*=======================================================================================================================================*//*=======================================================================================================================================*/
size_t default_get_block_size( void * memory ) {
    return malloc_size( memory );
}

/*=======================================================================================================================================*/
void Mem_Initialise( mem_allocator_t * allocator ) {
    if ( memInit == true ) {
        return;
    }
    
    Sys_MutexCreate( &mem.mutex );
    mem.defaultAllocator.malloc = default_malloc;
    mem.defaultAllocator.mallocAligned = default_mallocAligned;
    mem.defaultAllocator.realloc = default_realloc;
    mem.defaultAllocator.free = default_free;
    mem.defaultAllocator.get_block_size = default_get_block_size;
    
    mem.numAllocs = 0;
    mem.numFrees = 0;
    mem.memSizeAlloc = 0;
    
    mem.allocator = ( allocator == NULL ) ? &mem.defaultAllocator : allocator;
    
    memInit = true;
}

/*=======================================================================================================================================*/
void Mem_Finalise(void) {
    if (memInit == false ) {
        return;
    }
    
    Sys_MutexDestroy( &mem.mutex );
    memInit = false;
}

/*=======================================================================================================================================*/
void * Mem_Alloc( size_t size ) {
    void * ptr = NULL;
    assert( memInit == true );
    
    Sys_MutexLock( &mem.mutex );
    {
        ++mem.numAllocs;
        ptr =  mem.allocator->malloc( size );
        mem.memSizeAlloc += mem.allocator->get_block_size( ptr );
    }
    Sys_MutexUnlock( &mem.mutex );
    
    return ptr;
}

/*=======================================================================================================================================*/
void * Mem_AllocAligned( size_t size, size_t alignment ) {
    void * ptr = NULL;
    assert( memInit == true );
    
    Sys_MutexLock( &mem.mutex );
    {
        ++mem.numAllocs;
        ptr =  mem.allocator->mallocAligned( size, alignment );
        mem.memSizeAlloc += mem.allocator->get_block_size( ptr );
    }
    Sys_MutexUnlock( &mem.mutex );
    
    return ptr;
}

/*=======================================================================================================================================*/
void * Mem_CAlloc( size_t count, size_t size ) {
    return Mem_AllocAligned( count * size, 16 );
}

/*=======================================================================================================================================*/
void Mem_Free( void * block ) {
    assert( memInit == true );
    Sys_MutexLock( &mem.mutex );
    {
        ++mem.numFrees;
        mem.memSizeAlloc -= mem.allocator->get_block_size( block );
        mem.allocator->free( block );
    }
    Sys_MutexUnlock( &mem.mutex );
    
}

/*=======================================================================================================================================*/
void Mem_GetStats( mem_stats_t * stats ) {
    assert( memInit == true );
    
    Sys_MutexLock( &mem.mutex );
    {
        stats->numAllocs = mem.numAllocs;
        stats->numFrees = mem.numFrees;
        stats->sizeAlloc = mem.memSizeAlloc;
    }
    Sys_MutexUnlock( &mem.mutex );
}



#if 0

#define HEAP_CAPACITY 128

CVAR_INT( mem_allocEngineHeaps, "Tells the memory system to allocate the core engine heaps (true by default)", TRUE );
CVAR_INT(mem_renderHeapSize, "Size of the heap to use for the render system in KB", 1024 * 10);
CVAR_INT(mem_renderHeapSizeRes, "Size of the heap to use for the render objects in KB", 1024 * 2);

/* Internal struct used to describe a memory heap managed by the system */
typedef struct heap_s {
    void*       memory;
    size_t      memorySize;
    tlsf_t      tlsf;
} heap_t;

/* Internal struct used to hold the memory system state */
typedef struct mem_manager_s {
    pthread_sys_mutex_t mutex;
    heap_t          heaps[HEAP_CAPACITY];
    uintptr_t       heapAddresSorted[HEAP_CAPACITY];
    uint32_t        heapAddressMap[HEAP_CAPACITY];
    size_t          heapCount;
    allocator_t     globalAlloctor;
} mem_manager_t;

static bool_t memManagerInit = FALSE;
static mem_manager_t memManager;

static void* AllocatorDefault_MallocAligned( size_t size, size_t alignAmt );
static void* AllocatorDefault_Malloc( size_t size );
static void AllocatorDefault_Free( void* memory );
static uintptr_t AllocatorDefault_Align(uintptr_t rhs, uintptr_t alignAmt);

static heap_t* Mem_FindHeapByAddress(void* address);

/*=======================================================================================================================================*/
uintptr_t AllocatorDefault_Align(uintptr_t rhs, uintptr_t alignAmt) {
    uintptr_t rem = rhs % alignAmt;
    uintptr_t pad = alignAmt - rem;
    return rhs + pad;
}
    
/*=======================================================================================================================================*/
void* AllocatorDefault_Malloc( size_t size ) {
    return AllocatorDefault_MallocAligned( size, 16 );
}

/*=======================================================================================================================================*/
void* AllocatorDefault_MallocAligned( size_t size, size_t alignAmt ) {
    size_t addressSize = sizeof( uintptr_t );
    size_t actualSize = size + addressSize + alignAmt;

    uintptr_t memAllocated = ( uintptr_t ) malloc( actualSize );
    uintptr_t memReturned = AllocatorDefault_Align( memAllocated + addressSize, alignAmt );
    uintptr_t memAllocatedAddrPtr = memReturned - addressSize;

    *( (uintptr_t*)memAllocatedAddrPtr ) = memAllocated;

    return (void*) memReturned;
}

/*=======================================================================================================================================*/
void AllocatorDefault_Free( void* memory ) {
    if (memory == NULL) {
        return;
    }

    uintptr_t* memoryUintPtr = (uintptr_t*)memory;
    uintptr_t * actualMemoryAddPtr = memoryUintPtr - 1;
    void* actualMemoryAlloced = (void*)( *actualMemoryAddPtr );

    free( actualMemoryAlloced );
}

/*=======================================================================================================================================*/
void Mem_Initialise() {
    xassert(memManagerInit == FALSE);
    
    memset(&memManager, 0, sizeof(memManager));
    
    memManager.globalAlloctor.malloc = AllocatorDefault_Malloc;
    memManager.globalAlloctor.mallocAligned = AllocatorDefault_MallocAligned;
    memManager.globalAlloctor.free = AllocatorDefault_Free;
    
    pthread_mutex_init(&memManager.mutex, NULL);
    
    memManagerInit = TRUE;
}

/*=======================================================================================================================================*/
void Mem_Finalise() {
}

/*=======================================================================================================================================*/
void Mem_createHeaps() {
#if 0
    HeapCreateInfo hci;

    bool allocEngineHeaps = mem_allocEngineHeaps.getValueBool();
    
    hci.addEntry(HEAP_STL,              mem_stlHeapSize.getValueInt() * 1024);
    hci.addEntry(HEAP_STL_STRING,       mem_stlStringHeapSize.getValueInt() * 1024);

    if ( allocEngineHeaps == true ) {
        hci.addEntry( HEAP_GX, mem_gxHeapSize.getValueInt() * 1024 );
        hci.addEntry( HEAP_GX_RES, mem_gxHeapSizeRes.getValueInt() * 1024 );

        hci.addEntry( HEAP_RENDER, mem_renderHeapSize.getValueInt() * 1024 );
        hci.addEntry( HEAP_RENDER_RES, mem_renderHeapSizeRes.getValueInt() * 1024 );
    }
    
    for(int32_t i=0; i < (int32_t) hci.m_count; ++i) {
        HeapCreateEntry& entry = hci.m_entries[i];
        addHeap(entry.m_heapId, entry.m_size);
    }
#endif
}

/*=======================================================================================================================================*/
void Mem_AllocateHeap(uint32_t heapId, size_t heapMemSize) {
    void* heapMem = memManager.globalAlloctor.malloc(heapMemSize);
    Mem_AddHeap(heapId, heapMem, heapMemSize);
}

/*=======================================================================================================================================*/
void Mem_AddHeap(uint32_t heapId, void* heapMem, size_t heapMemSize) {
    xasserte(memManager.heapCount < HEAP_CAPACITY, "Too many heaps");
    xasserte(memManager.heaps[heapId] == 0, "Heap with id %u has already been added", heapId);
    
    /* Setup the heap info and create the tlsf allocator */
    heap_t* info = &memManager.heaps[heapId];
    info->memory = heapMem;
    info->memorySize = heapMemSize;
    info->tlsf = tlsf_create_with_pool(heapMem, heapMemSize);
    
    /* Add the heap into the arrays sorted by the heap base address */
    int32_t index = - 1;
    bool_t found = Bsearch_FindUintPtr(&index, (uintptr_t)heapMem, memManager.heapAddresSorted, memManager.heapCount);
    xasserte(found == false, "This heap address is already in use");
    
    Array_InsertAtPosUintPtr(index, (uintptr_t) heapMem, memManager.heapAddresSorted, memManager.heapCount, HEAP_CAPACITY);
    Array_InsertAtPosUint32(index, heapId, memManager.heapAddressMap, memManager.heapCount, HEAP_CAPACITY);
    
    ++memManager.heapCount;
}

/*=======================================================================================================================================*/
heap_t* Mem_FindHeapByAddress(void* address) {
    int32_t index = - 1;
    Bsearch_FindUintPtr(&index, (uintptr_t)address, memManager.heapAddresSorted, memManager.heapCount);
   
    if (index == 0) {
       /* Out of range of managed memory, so return NULL
          Note: this is because we expect to get the index of the heap ABOVE the heap
          the memory was allocated from. So an index of zero means that the address
          was not allocated from one of the heaps. */
       return NULL;
    }
   
    /* We have a valid heap index -BUT- it may not be the case that the address is
       in that heap. We support deleting and adding of heaps, so there may have
       been a naughty allocation between the heaps being moved around / added. So
       check that the address is really within the heap. */
    uint32_t heapId = memManager.heapAddressMap[index-1];
    heap_t* info = &memManager.heaps[heapId];
    uintptr_t endOfHeap = ((uintptr_t) info->memory) + info->memorySize;
    uintptr_t addressUint = (uintptr_t) address;
    
    if ((addressUint < (uintptr_t) info->memory) || (addressUint >= endOfHeap)) {
       return NULL;
    }
   
    return info;
}

/*=======================================================================================================================================*/
void* Mem_Malloc(uint32_t heapId, size_t size) {
    return Mem_MallocAligned(heapId, size, 8);
}

/*=======================================================================================================================================*/
void* Mem_MallocAligned(uint32_t heapId, size_t size, size_t alignment) {
    pthread_mutex_lock(&memManager.mutex);
    
    void* mem = NULL;
    heap_t* info = &memManager.heaps[heapId];
    
    if (info->memorySize == 0) {
        /* If the heap has not been added yet (or it's the default heap)
         then just use the global allocator */
        pthread_mutex_unlock(&memManager.mutex);
        return memManager.globalAlloctor.mallocAligned(size, alignment);
    }
    
    /* heapId references a valid heap, so allocate from it */
    mem = tlsf_memalign(info->tlsf, alignment, size);
    pthread_mutex_unlock(&memManager.mutex);
    return mem;
}

/*=======================================================================================================================================*/
void Mem_Free(void* memory) {
    pthread_mutex_lock(&memManager.mutex);
    
    heap_t* info = Mem_FindHeapByAddress(memory);
    if (info != NULL) {
        tlsf_free(info->tlsf, memory);
    } else {
        memManager.globalAlloctor.free(memory);
    }
    
    pthread_mutex_unlock(&memManager.mutex);
}

#endif
