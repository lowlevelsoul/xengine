/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_MPMCQUEUE_HPP
#define XE_MPMCQUEUE_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

// Values are copied by object representation. Init and Reset require exclusive access.
template <typename T, std::size_t Capacity> class CMPMCQueue {
    static_assert( Capacity > 0 && Capacity <= std::numeric_limits<std::uint32_t>::max(),
                   "Invalid MPMC queue capacity" );
    static_assert( std::is_trivially_copyable<T>::value, "MPMC queue values must be trivially copyable" );
    static_assert( std::is_default_constructible<T>::value, "MPMC queue values must be default constructible" );
    static_assert( !std::is_const<T>::value && !std::is_volatile<T>::value, "MPMC queue values must be writable" );

public:
    CMPMCQueue() = default;

    void Init() {
        Reset();
    }

    void Reset() {
        head_.store( 0 );
        committedHead_.store( 0 );
        tail_.store( 0 );
        committedTail_.store( 0 );
    }

    bool Write( const T& value ) {
        std::uint64_t position;
        if ( !ReserveWrite( position ) ) {
            return false;
        }

        std::memcpy( &elements_[ GetIndex( position ) ], &value, sizeof( T ) );
        Commit( committedTail_, position );
        return true;
    }

    bool Read( T& value ) {
        std::uint64_t position;
        if ( !ReserveRead( position ) ) {
            return false;
        }

        std::memcpy( &value, &elements_[ GetIndex( position ) ], sizeof( T ) );
        Commit( committedHead_, position );
        return true;
    }

    std::size_t Size() const {
        std::uint64_t head;
        std::uint64_t tail;

        do {
            head = committedHead_.load();
            tail = tail_.load();
        } while ( head != committedHead_.load() );

        const std::uint32_t headIndex = GetIndex( head );
        const std::uint32_t tailIndex = GetIndex( tail );
        if ( GetVersion( head ) == GetVersion( tail ) ) {
            return static_cast<std::size_t>( tailIndex - headIndex );
        }

        return Capacity - headIndex + tailIndex;
    }

    bool IsEmpty() const {
        const std::uint64_t head = head_.load();
        return head == committedTail_.load();
    }

    bool IsFull() const {
        return Size() == Capacity;
    }

private:
    static std::uint64_t PackVersionedIndex( std::uint32_t version, std::uint32_t index ) {
        return ( static_cast<std::uint64_t>( version ) << 32 ) | index;
    }

    static std::uint32_t GetIndex( std::uint64_t versionedIndex ) {
        return static_cast<std::uint32_t>( versionedIndex );
    }

    static std::uint32_t GetVersion( std::uint64_t versionedIndex ) {
        return static_cast<std::uint32_t>( versionedIndex >> 32 );
    }

    static std::uint64_t NextIndex( std::uint64_t position ) {
        std::uint32_t index = GetIndex( position ) + 1;
        std::uint32_t version = GetVersion( position );
        if ( index == Capacity ) {
            index = 0;
            ++version;
        }

        return PackVersionedIndex( version, index );
    }

    bool ReserveWrite( std::uint64_t& position ) {
        std::uint64_t tail = tail_.load();
        for ( ;; ) {
            const std::uint64_t head = committedHead_.load();
            if ( GetIndex( tail ) == GetIndex( head ) && tail != head ) {
                const std::uint64_t currentTail = tail_.load();
                if ( currentTail == tail ) {
                    return false;
                }

                tail = currentTail;
                continue;
            }

            const std::uint64_t next = NextIndex( tail );
            const std::uint64_t reserved = tail;
            if ( tail_.compare_exchange_weak( tail, next ) ) {
                position = reserved;
                return true;
            }
        }
    }

    bool ReserveRead( std::uint64_t& position ) {
        std::uint64_t head = head_.load();
        for ( ;; ) {
            if ( head == committedTail_.load() ) {
                return false;
            }

            const std::uint64_t next = NextIndex( head );
            const std::uint64_t reserved = head;
            if ( head_.compare_exchange_weak( head, next ) ) {
                position = reserved;
                return true;
            }
        }
    }

    static void Commit( std::atomic<std::uint64_t>& committed, std::uint64_t position ) {
        while ( committed.load() != position ) {
        }

        committed.store( NextIndex( position ) );
    }

    std::atomic<std::uint64_t> head_{ 0 };
    std::atomic<std::uint64_t> committedHead_{ 0 };
    std::atomic<std::uint64_t> tail_{ 0 };
    std::atomic<std::uint64_t> committedTail_{ 0 };
    T elements_[ Capacity ]{};
};

#endif // XE_MPMCQUEUE_HPP