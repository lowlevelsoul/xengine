/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_MPMCQUEUE_H
#define XE_MPMCQUEUE_H

#include "xe/Platform.h"
#include <stdatomic.h>

typedef struct mpmc_queue_state_s {
    _Atomic( uint64_t ) head;
    _Atomic( uint64_t ) committedHead;
    _Atomic( uint64_t ) tail;
    _Atomic( uint64_t ) committedTail;
} mpmc_queue_state_t;

/*==========================================================================================================*/
INLINE uint64_t MPMCQueue_PackVersionedIndex( uint32_t version, uint32_t index ) {
    return ( ( uint64_t )version << 32 ) | index;
}

/*==========================================================================================================*/
INLINE uint32_t MPMCQueue_GetIndex( uint64_t versionedIndex ) {
    return ( uint32_t )versionedIndex;
}

/*==========================================================================================================*/
INLINE uint32_t MPMCQueue_GetVersion( uint64_t versionedIndex ) {
    return ( uint32_t )( versionedIndex >> 32 );
}

/*==========================================================================================================*/
INLINE uint64_t MPMCQueue_NextIndex( size_t capacity, uint64_t position ) {
    uint32_t index = MPMCQueue_GetIndex( position ) + 1;
    uint32_t version = MPMCQueue_GetVersion( position );

    if ( index == capacity ) {
        index = 0;
        ++version;
    }

    return MPMCQueue_PackVersionedIndex( version, index );
}

/*==========================================================================================================*/
INLINE void MPMCQueue_InitState( mpmc_queue_state_t* state ) {
    atomic_init( &state->head, 0 );
    atomic_init( &state->committedHead, 0 );
    atomic_init( &state->tail, 0 );
    atomic_init( &state->committedTail, 0 );
}

/*==========================================================================================================*/
INLINE void MPMCQueue_ResetState( mpmc_queue_state_t* state ) {
    atomic_store( &state->head, 0 );
    atomic_store( &state->committedHead, 0 );
    atomic_store( &state->tail, 0 );
    atomic_store( &state->committedTail, 0 );
}

/*==========================================================================================================*/
INLINE bool MPMCQueue_ReserveWrite( mpmc_queue_state_t* state, size_t capacity, uint64_t* position ) {
    uint64_t tail = atomic_load( &state->tail );

    for ( ;; ) {
        uint64_t head = atomic_load( &state->committedHead );

        if ( MPMCQueue_GetIndex( tail ) == MPMCQueue_GetIndex( head ) && tail != head ) {
            uint64_t currentTail = atomic_load( &state->tail );

            if ( currentTail == tail ) {
                return false;
            }

            tail = currentTail;
            continue;
        }

        uint64_t next = MPMCQueue_NextIndex( capacity, tail );
        uint64_t reserved = tail;

        if ( atomic_compare_exchange_weak( &state->tail, &tail, next ) ) {
            *position = reserved;
            return true;
        }
    }
}

/*==========================================================================================================*/
INLINE bool MPMCQueue_ReserveRead( mpmc_queue_state_t* state, size_t capacity, uint64_t* position ) {
    uint64_t head = atomic_load( &state->head );

    for ( ;; ) {
        if ( head == atomic_load( &state->committedTail ) ) {
            return false;
        }

        uint64_t next = MPMCQueue_NextIndex( capacity, head );
        uint64_t reserved = head;

        if ( atomic_compare_exchange_weak( &state->head, &head, next ) ) {
            *position = reserved;
            return true;
        }
    }
}

/* Publish in reservation order so another thread never observes an unfinished copy. */
INLINE void MPMCQueue_Commit( _Atomic( uint64_t )* committed, size_t capacity, uint64_t position ) {
    while ( atomic_load( committed ) != position ) {
    }

    atomic_store( committed, MPMCQueue_NextIndex( capacity, position ) );
}

/*==========================================================================================================*/
INLINE size_t MPMCQueue_StateSize( mpmc_queue_state_t* state, size_t capacity ) {
    uint64_t head;
    uint64_t tail;

    do {
        head = atomic_load( &state->committedHead );
        tail = atomic_load( &state->tail );
    } while ( head != atomic_load( &state->committedHead ) );

    uint32_t headIndex = MPMCQueue_GetIndex( head );
    uint32_t tailIndex = MPMCQueue_GetIndex( tail );

    if ( MPMCQueue_GetVersion( head ) == MPMCQueue_GetVersion( tail ) ) {
        return ( size_t )( tailIndex - headIndex );
    }

    return capacity - headIndex + tailIndex;
}

/*==========================================================================================================*/
INLINE bool MPMCQueue_StateIsEmpty( mpmc_queue_state_t* state ) {
    uint64_t head = atomic_load( &state->head );
    return head == atomic_load( &state->committedTail );
}

/* Define a queue type and name_Init, name_Reset, name_Write, name_Read, name_Size,
 * name_IsEmpty and name_IsFull functions. Type must be a writable, non-atomic,
 * complete object type, and capacity a constant between 1 and UINT32_MAX. Values
 * are copied by their object representation, including pointers. Init and reset
 * require exclusive access. Input/output objects must not overlap queue storage.
 *
 * Size and IsFull include slots reserved by writers or still being copied by readers;
 * IsEmpty reports whether a published value can be reserved. These are snapshots.
 * A stalled reader or writer can delay later operations. A thread stalled for 2^32
 * complete traversals can encounter version wraparound and ABA.
 */
#define MPMC_QUEUE_DEFINE( name, type, capacity )                                                                      \
    _Static_assert( ( capacity ) > 0 && ( uint64_t )( capacity ) <= UINT32_MAX, "Invalid MPMC queue capacity" );       \
    typedef type name##_value_t;                                                                                       \
    typedef struct name {                                                                                              \
        mpmc_queue_state_t state;                                                                                      \
        name##_value_t elements[ capacity ];                                                                           \
    } name;                                                                                                            \
    INLINE void name##_Init( name* self_ ) {                                                                           \
        MPMCQueue_InitState( &self_->state );                                                                          \
    }                                                                                                                  \
    INLINE void name##_Reset( name* self_ ) {                                                                          \
        MPMCQueue_ResetState( &self_->state );                                                                         \
    }                                                                                                                  \
    INLINE bool name##_Write( name* self_, const name##_value_t* value ) {                                             \
        uint64_t position;                                                                                             \
        if ( !MPMCQueue_ReserveWrite( &self_->state, ( capacity ), &position ) ) {                                     \
            return false;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        memcpy( &self_->elements[ MPMCQueue_GetIndex( position ) ], value, sizeof( name##_value_t ) );                 \
        MPMCQueue_Commit( &self_->state.committedTail, ( capacity ), position );                                       \
        return true;                                                                                                   \
    }                                                                                                                  \
    INLINE bool name##_Read( name* self_, name##_value_t* value ) {                                                    \
        uint64_t position;                                                                                             \
        if ( !MPMCQueue_ReserveRead( &self_->state, ( capacity ), &position ) ) {                                      \
            return false;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        memcpy( value, &self_->elements[ MPMCQueue_GetIndex( position ) ], sizeof( name##_value_t ) );                 \
        MPMCQueue_Commit( &self_->state.committedHead, ( capacity ), position );                                       \
        return true;                                                                                                   \
    }                                                                                                                  \
    INLINE size_t name##_Size( name* self_ ) {                                                                         \
        return MPMCQueue_StateSize( &self_->state, ( capacity ) );                                                     \
    }                                                                                                                  \
    INLINE bool name##_IsEmpty( name* self_ ) {                                                                        \
        return MPMCQueue_StateIsEmpty( &self_->state );                                                                \
    }                                                                                                                  \
    INLINE bool name##_IsFull( name* self_ ) {                                                                         \
        return MPMCQueue_StateSize( &self_->state, ( capacity ) ) == ( size_t )( capacity );                           \
    }

#endif // XE_MPMCQUEUE_H
