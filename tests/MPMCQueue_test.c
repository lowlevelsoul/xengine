#include "xe/MPMCQueue.h"
#include <windows.h>
#include <assert.h>
#include <stdlib.h>

#define PRODUCERS 4
#define CONSUMERS 4
#define ITEMS_PER_PRODUCER 15000
#define ITEM_COUNT ( PRODUCERS * ITEMS_PER_PRODUCER )

typedef struct packet_s {
    uint32_t id;
    uint32_t inverse;
    uint64_t stamp;
    unsigned char padding[ 32 ];
} packet_t;

typedef uint32_t triple_t[ 3 ];

MPMC_QUEUE_DEFINE( u32_queue, uint32_t, 3 )
MPMC_QUEUE_DEFINE( single_queue, uint32_t, 1 )
MPMC_QUEUE_DEFINE( pointer_queue, int*, 2 )
MPMC_QUEUE_DEFINE( array_queue, triple_t, 2 )
MPMC_QUEUE_DEFINE( packet_queue_1, packet_t, 1 )
MPMC_QUEUE_DEFINE( packet_queue_2, packet_t, 2 )
MPMC_QUEUE_DEFINE( packet_queue_17, packet_t, 17 )

#define TEST_ADAPTERS( name )                                                                                          \
    static bool name##_WriteAdapter( void* queue, const packet_t* value ) {                                            \
        return name##_Write( ( name* )queue, value );                                                                  \
    }                                                                                                                  \
    static bool name##_ReadAdapter( void* queue, packet_t* value ) {                                                   \
        return name##_Read( ( name* )queue, value );                                                                   \
    }

TEST_ADAPTERS( packet_queue_1 )
TEST_ADAPTERS( packet_queue_2 )
TEST_ADAPTERS( packet_queue_17 )

typedef struct worker_s {
    void* queue;
    bool ( *write )( void*, const packet_t* );
    bool ( *read )( void*, packet_t* );
    HANDLE start;
    _Atomic( uint32_t )* seen;
    uint32_t producer;
} worker_t;

static DWORD WINAPI Produce( void* arg ) {
    worker_t* worker = ( worker_t* )arg;
    assert( WaitForSingleObject( worker->start, INFINITE ) == WAIT_OBJECT_0 );

    for ( uint32_t i = 0; i < ITEMS_PER_PRODUCER; ++i ) {
        packet_t value;
        value.id = worker->producer * ITEMS_PER_PRODUCER + i;
        value.inverse = ~value.id;
        value.stamp = ( uint64_t )value.id * 123456789;
        memset( value.padding, ( int )( value.id & 255 ), sizeof( value.padding ) );

        while ( !worker->write( worker->queue, &value ) ) {
            SwitchToThread();
        }
    }

    return 0;
}

static DWORD WINAPI Consume( void* arg ) {
    worker_t* worker = ( worker_t* )arg;
    assert( WaitForSingleObject( worker->start, INFINITE ) == WAIT_OBJECT_0 );

    for ( ;; ) {
        packet_t value;

        if ( !worker->read( worker->queue, &value ) ) {
            SwitchToThread();
            continue;
        }

        if ( value.id == UINT32_MAX ) {
            return 0;
        }

        assert( value.id < ITEM_COUNT );
        assert( value.inverse == ~value.id );
        assert( value.stamp == ( uint64_t )value.id * 123456789 );

        for ( size_t i = 0; i < sizeof( value.padding ); ++i ) {
            assert( value.padding[ i ] == ( unsigned char )value.id );
        }

        assert( atomic_fetch_add( &worker->seen[ value.id ], 1 ) == 0 );
    }
}

static void TestSequential( void ) {
    u32_queue queue;
    u32_queue_Init( &queue );
    assert( u32_queue_IsEmpty( &queue ) );
    assert( !u32_queue_IsFull( &queue ) );
    assert( u32_queue_Size( &queue ) == 0 );
    uint32_t output = 99;
    assert( !u32_queue_Read( &queue, &output ) );
    assert( output == 99 );

    for ( uint32_t value = 0; value < 3; ++value ) {
        assert( u32_queue_Write( &queue, &value ) );
    }

    assert( u32_queue_IsFull( &queue ) );
    assert( u32_queue_Size( &queue ) == 3 );
    uint32_t value = 3;
    assert( !u32_queue_Write( &queue, &value ) );
    assert( u32_queue_Read( &queue, &output ) && output == 0 );
    assert( u32_queue_Write( &queue, &value ) );

    for ( uint32_t expected = 1; expected <= 3; ++expected ) {
        assert( u32_queue_Read( &queue, &output ) && output == expected );
    }

    assert( u32_queue_IsEmpty( &queue ) );
    assert( u32_queue_Size( &queue ) == 0 );
    assert( u32_queue_Write( &queue, &value ) );
    u32_queue_Reset( &queue );
    assert( u32_queue_IsEmpty( &queue ) );
    assert( u32_queue_Size( &queue ) == 0 );

    uint64_t last = MPMCQueue_PackVersionedIndex( UINT32_MAX, 2 );
    atomic_store( &queue.state.head, last );
    atomic_store( &queue.state.committedHead, last );
    atomic_store( &queue.state.tail, last );
    atomic_store( &queue.state.committedTail, last );

    for ( uint32_t i = 0; i < 3; ++i ) {
        assert( u32_queue_Write( &queue, &i ) );
        assert( u32_queue_Size( &queue ) == i + 1 );
    }

    assert( u32_queue_IsFull( &queue ) );

    for ( uint32_t i = 0; i < 3; ++i ) {
        assert( u32_queue_Read( &queue, &output ) && output == i );
        assert( u32_queue_Size( &queue ) == 2 - i );
    }

    single_queue single;
    single_queue_Init( &single );
    atomic_store( &single.state.head, MPMCQueue_PackVersionedIndex( UINT32_MAX, 0 ) );
    atomic_store( &single.state.committedHead, MPMCQueue_PackVersionedIndex( UINT32_MAX, 0 ) );
    atomic_store( &single.state.tail, MPMCQueue_PackVersionedIndex( UINT32_MAX, 0 ) );
    atomic_store( &single.state.committedTail, MPMCQueue_PackVersionedIndex( UINT32_MAX, 0 ) );
    assert( single_queue_Write( &single, &value ) );
    assert( single_queue_IsFull( &single ) );
    assert( single_queue_Read( &single, &output ) && output == value );
    assert( single_queue_IsEmpty( &single ) );

    single_queue_Reset( &single );
    uint64_t position;
    assert( MPMCQueue_ReserveWrite( &single.state, 1, &position ) );
    assert( single_queue_IsEmpty( &single ) && single_queue_IsFull( &single ) );
    assert( single_queue_Size( &single ) == 1 );
    assert( !single_queue_Read( &single, &output ) );
    single.elements[ 0 ] = value;
    MPMCQueue_Commit( &single.state.committedTail, 1, position );
    assert( MPMCQueue_ReserveRead( &single.state, 1, &position ) );
    assert( single_queue_IsEmpty( &single ) && single_queue_IsFull( &single ) );
    uint32_t replacement = 4;
    assert( !single_queue_Write( &single, &replacement ) );
    assert( single.elements[ 0 ] == value );
    MPMCQueue_Commit( &single.state.committedHead, 1, position );
    assert( single_queue_Write( &single, &replacement ) );
    assert( single_queue_Read( &single, &output ) && output == replacement );
}

static void TestTypes( void ) {
    pointer_queue pointers;
    pointer_queue_Init( &pointers );
    int number = 42;
    int* input = &number;
    int* output = NULL;
    assert( pointer_queue_Write( &pointers, &input ) );
    assert( pointer_queue_Read( &pointers, &output ) && output == &number );

    array_queue arrays;
    array_queue_Init( &arrays );
    triple_t in = { 7, 8, 9 };
    triple_t out = { 0 };
    assert( array_queue_Write( &arrays, &in ) );
    assert( array_queue_Read( &arrays, &out ) );
    assert( memcmp( in, out, sizeof( in ) ) == 0 );
}

static void TestConcurrent( void* queue, bool ( *write )( void*, const packet_t* ),
                            bool ( *read )( void*, packet_t* ) ) {
    _Atomic( uint32_t )* seen = malloc( ITEM_COUNT * sizeof( *seen ) );
    assert( seen != NULL );

    for ( size_t i = 0; i < ITEM_COUNT; ++i ) {
        atomic_init( &seen[ i ], 0 );
    }

    HANDLE start = CreateEvent( NULL, TRUE, FALSE, NULL );
    assert( start != NULL );
    HANDLE producers[ PRODUCERS ];
    HANDLE consumers[ CONSUMERS ];
    worker_t workers[ PRODUCERS + CONSUMERS ];

    for ( size_t i = 0; i < PRODUCERS + CONSUMERS; ++i ) {
        workers[ i ].queue = queue;
        workers[ i ].write = write;
        workers[ i ].read = read;
        workers[ i ].start = start;
        workers[ i ].seen = seen;
        workers[ i ].producer = ( uint32_t )i;

        if ( i < PRODUCERS ) {
            producers[ i ] = CreateThread( NULL, 0, Produce, &workers[ i ], 0, NULL );
            assert( producers[ i ] != NULL );
        } else {
            consumers[ i - PRODUCERS ] = CreateThread( NULL, 0, Consume, &workers[ i ], 0, NULL );
            assert( consumers[ i - PRODUCERS ] != NULL );
        }
    }

    assert( SetEvent( start ) );
    assert( WaitForMultipleObjects( PRODUCERS, producers, TRUE, INFINITE ) == WAIT_OBJECT_0 );
    packet_t stop = { 0 };
    stop.id = UINT32_MAX;

    for ( size_t i = 0; i < CONSUMERS; ++i ) {
        while ( !write( queue, &stop ) ) {
            SwitchToThread();
        }
    }

    assert( WaitForMultipleObjects( CONSUMERS, consumers, TRUE, INFINITE ) == WAIT_OBJECT_0 );

    for ( size_t i = 0; i < ITEM_COUNT; ++i ) {
        assert( atomic_load( &seen[ i ] ) == 1 );
    }

    for ( size_t i = 0; i < PRODUCERS; ++i ) {
        assert( CloseHandle( producers[ i ] ) );
    }

    for ( size_t i = 0; i < CONSUMERS; ++i ) {
        assert( CloseHandle( consumers[ i ] ) );
    }

    assert( CloseHandle( start ) );
    free( ( void* )seen );
}

int main( void ) {
    TestSequential();
    TestTypes();
    packet_queue_1 q1;
    packet_queue_2 q2;
    packet_queue_17 q17;
    packet_queue_1_Init( &q1 );
    packet_queue_2_Init( &q2 );
    packet_queue_17_Init( &q17 );
    TestConcurrent( &q1, packet_queue_1_WriteAdapter, packet_queue_1_ReadAdapter );
    TestConcurrent( &q2, packet_queue_2_WriteAdapter, packet_queue_2_ReadAdapter );
    TestConcurrent( &q17, packet_queue_17_WriteAdapter, packet_queue_17_ReadAdapter );
    assert( packet_queue_1_IsEmpty( &q1 ) );
    assert( packet_queue_2_IsEmpty( &q2 ) );
    assert( packet_queue_17_IsEmpty( &q17 ) );
    return 0;
}
