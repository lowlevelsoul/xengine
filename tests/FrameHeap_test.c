#include "xe/FrameHeap.h"
#include <windows.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define THREAD_COUNT 8
#define HEAP_SIZE 65536
#define ALLOCATION_SIZE 16
#define MAX_ALLOCATIONS ( HEAP_SIZE / ALLOCATION_SIZE )

typedef struct worker_s {
    frame_heap_t* heap;
    HANDLE start;
    void* allocations[ MAX_ALLOCATIONS ];
    size_t count;
} worker_t;

static DWORD WINAPI AllocateUntilFull( void* arg ) {
    worker_t* worker = ( worker_t* )arg;
    DWORD wait_result = WaitForSingleObject( worker->start, INFINITE );
    assert( wait_result == WAIT_OBJECT_0 );

    while ( worker->count < MAX_ALLOCATIONS ) {
        void* allocation = FrameHeap_Alloc( worker->heap, ALLOCATION_SIZE );

        if ( allocation == NULL ) {
            break;
        }

        worker->allocations[ worker->count++ ] = allocation;
    }

    return 0;
}

static int CompareAddresses( const void* lhs, const void* rhs ) {
    uintptr_t a = ( uintptr_t )*( void* const* )lhs;
    uintptr_t b = ( uintptr_t )*( void* const* )rhs;
    return ( a > b ) - ( a < b );
}

int main( void ) {
    void* memory = malloc( HEAP_SIZE );
    assert( memory != NULL );
    frame_heap_t* heap = FrameHeap_Create( memory, HEAP_SIZE );
    char* start = ( char* )FrameHeap_Alloc( heap, 0 );
    size_t capacity = HEAP_SIZE - ( size_t )( start - ( char* )memory );
    void* allocation = FrameHeap_Alloc( heap, SIZE_MAX );
    assert( allocation == NULL );
    allocation = FrameHeap_Alloc( heap, 1 );
    assert( allocation == start );
    allocation = FrameHeap_Alloc( heap, capacity );
    assert( allocation == NULL );
    FrameHeap_Reset( heap );

    worker_t* workers = ( worker_t* )calloc( THREAD_COUNT, sizeof( worker_t ) );
    void** allocations = ( void** )malloc( MAX_ALLOCATIONS * sizeof( void* ) );
    assert( workers != NULL && allocations != NULL );

    for ( int round = 0; round < 20; ++round ) {
        HANDLE start_event = CreateEvent( NULL, TRUE, FALSE, NULL );
        HANDLE threads[ THREAD_COUNT ];
        assert( start_event != NULL );

        for ( size_t i = 0; i < THREAD_COUNT; ++i ) {
            workers[ i ].heap = heap;
            workers[ i ].start = start_event;
            workers[ i ].count = 0;
            threads[ i ] = CreateThread( NULL, 0, AllocateUntilFull, &workers[ i ], 0, NULL );
            assert( threads[ i ] != NULL );
        }

        BOOL signalled = SetEvent( start_event );
        assert( signalled );
        DWORD wait_result = WaitForMultipleObjects( THREAD_COUNT, threads, TRUE, INFINITE );
        assert( wait_result == WAIT_OBJECT_0 );
        size_t count = 0;

        for ( size_t i = 0; i < THREAD_COUNT; ++i ) {
            assert( count + workers[ i ].count <= MAX_ALLOCATIONS );

            for ( size_t j = 0; j < workers[ i ].count; ++j ) {
                allocations[ count++ ] = workers[ i ].allocations[ j ];
            }

            BOOL closed = CloseHandle( threads[ i ] );
            assert( closed );
        }

        BOOL closed = CloseHandle( start_event );
        assert( closed );
        assert( count == capacity / ALLOCATION_SIZE );
        qsort( allocations, count, sizeof( void* ), CompareAddresses );

        for ( size_t i = 0; i < count; ++i ) {
            assert( allocations[ i ] == start + i * ALLOCATION_SIZE );
        }

        allocation = FrameHeap_Alloc( heap, ALLOCATION_SIZE );
        assert( allocation == NULL );
        size_t remaining = capacity % ALLOCATION_SIZE;
        allocation = FrameHeap_Alloc( heap, remaining );
        assert( allocation == start + count * ALLOCATION_SIZE );
        allocation = FrameHeap_Alloc( heap, 1 );
        assert( allocation == NULL );
        FrameHeap_Reset( heap );
    }

    FrameHeap_Destroy( heap );
    free( allocations );
    free( workers );
    free( memory );
    return 0;
}
