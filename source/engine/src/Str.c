/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "Str.h"
#include "Mem.h"
#include "Sys.h"
#include <stdio.h>

typedef struct str_header_s {
    uint32_t length;
    uint32_t capacity;
    char* data;
} str_header_t;

#define STR_HEADER_SIZE ( Sys_AlignValue( sizeof( str_header_t ), sizeof( void* ) ) )

/*==========================================================================================================*/
static inline str_header_t* Str_GetHeader( str_t self_ ) {
    return ( str_header_t* )( ( char* )self_ - STR_HEADER_SIZE );
}

/*==========================================================================================================*/
/**
 * Ensures that the string has at least the specified length.
 * If the string is not initialized or its capacity is insufficient,
 * a new string buffer is allocated and the old data is copied if necessary.
 */
static void Str_EnsureCapacity( str_t* self_, uint32_t capacity ) {
    bool shouldAllocate = false;
    bool copyData = false;

    if ( *self_ == NULL ) {
        // String is not initialized, allocate new header and data
        shouldAllocate = true;
        copyData = false;
    } else {
        str_header_t* header = Str_GetHeader( *self_ );
        if ( header->capacity <= capacity ) {
            shouldAllocate = true;
            copyData = true;
        }
    }

    if ( !shouldAllocate ) {
        return;
    }

    size_t new_capacity = Sys_AlignValue( capacity + 1, sizeof( void* ) );
    str_header_t* new_header = ( str_header_t* )Mem_Alloc( STR_HEADER_SIZE + new_capacity );
    new_header->length = 0;
    new_header->capacity = new_capacity;
    new_header->data = ( char* )new_header + STR_HEADER_SIZE;

    if ( copyData == true ) {
        str_header_t* old_header = Str_GetHeader( *self_ );
        new_header->length = old_header->length;
        memcpy( new_header->data, old_header->data, old_header->length );
    }

    /* Null-terminate the string */
    new_header->data[ new_header->length ] = '\0';

    /* Free the old string if it exists */
    if ( *self_ != NULL ) {
        Mem_Free( Str_GetHeader( *self_ ) );
    }

    if ( shouldAllocate == true ) {
        *self_ = new_header->data;
    }
}

/*==========================================================================================================*/
static void Str_Free( str_t* self_ ) {
    if ( *self_ != NULL ) {
        Mem_Free( Str_GetHeader( *self_ ) );
        *self_ = NULL;
    }
}

/*==========================================================================================================*/
X_API void Str_Destroy( str_t* self_ ) {
    Str_Free( self_ );
}

/*==========================================================================================================*/
X_API void Str_Resize( str_t* self_, size_t length ) {

    Str_EnsureCapacity( self_, ( uint32_t )length );
    str_header_t* header = Str_GetHeader( *self_ );

    if ( header != NULL ) {
        header->length = ( uint32_t )length;
        header->data[ header->length ] = '\0';
    }
}

/*==========================================================================================================*/
X_API void Str_Reserve( str_t* self_, size_t reserve ) {
    Str_EnsureCapacity( self_, ( uint32_t )reserve );
}

/*==========================================================================================================*/
X_API void Str_Set( str_t* self_, str_t src ) {
    if ( src == NULL ) {
        /* If the source string is NULL, resize the string to 0 */
        Str_Resize( self_, 0 );
        return;
    }

    size_t src_length = Str_Length( src );
    Str_EnsureCapacity( self_, ( uint32_t )src_length );
    str_header_t* header = Str_GetHeader( *self_ );
    if ( header != NULL ) {
        header->length = ( uint32_t )src_length;
        memcpy( header->data, src, src_length );
        header->data[ header->length ] = '\0';
    }
}

/*==========================================================================================================*/
X_API void Str_SetCStr( str_t* self_, const char* src ) {
    if ( src == NULL ) {
        Str_Resize( self_, 0 );
        return;
    }

    size_t src_length = strlen( src );
    Str_EnsureCapacity( self_, ( uint32_t )src_length );
    str_header_t* header = Str_GetHeader( *self_ );
    if ( header != NULL ) {
        header->length = ( uint32_t )src_length;
        memcpy( header->data, src, src_length );
        header->data[ header->length ] = '\0';
    }
}

/*==========================================================================================================*/
X_API size_t Str_Length( const str_t self_ ) {
    str_header_t* header = Str_GetHeader( self_ );
    return ( header != NULL ) ? header->length : 0;
}

/*==========================================================================================================*/
X_API size_t Str_Capacity( const str_t self_ ) {
    str_header_t* header = Str_GetHeader( self_ );
    return ( header != NULL ) ? header->capacity : 0;
}

/*==========================================================================================================*/
X_API void Str_Concat( str_t* self_, const str_t lhs, const str_t rhs ) {

    if ( lhs == NULL ) {
        Str_Set( self_, rhs );
        return;
    }

    if ( rhs == NULL ) {
        Str_Set( self_, lhs );
        return;
    }

    size_t lhs_length = Str_Length( lhs );
    size_t rhs_length = Str_Length( rhs );
    Str_EnsureCapacity( self_, ( uint32_t )( lhs_length + rhs_length ) );
    str_header_t* header = Str_GetHeader( *self_ );
    if ( header != NULL ) {
        memcpy( header->data, lhs, lhs_length );
        memcpy( header->data + lhs_length, rhs, rhs_length );
        header->length = ( uint32_t )( lhs_length + rhs_length );
        header->data[ header->length ] = '\0';
    }
}

/*==========================================================================================================*/
X_API void Str_ConcatCStr( str_t* self_, const char* lhs, const char* rhs ) {
    size_t lhs_length = strlen( lhs );
    size_t rhs_length = strlen( rhs );
    Str_EnsureCapacity( self_, ( uint32_t )( lhs_length + rhs_length ) );
    str_header_t* header = Str_GetHeader( *self_ );
    if ( header != NULL ) {
        memcpy( header->data, lhs, lhs_length );
        memcpy( header->data + lhs_length, rhs, rhs_length );
        header->length = ( uint32_t )( lhs_length + rhs_length );
        header->data[ header->length ] = '\0';
    }
}

/*==========================================================================================================*/
X_API void Str_Append( str_t* self_, str_t src ) {
    if ( src == NULL ) {
        return;
    }

    size_t src_length = Str_Length( src );
    size_t self_length = Str_Length( *self_ );
    Str_EnsureCapacity( self_, ( uint32_t )( self_length + src_length ) );
    str_header_t* header = Str_GetHeader( *self_ );
    if ( header != NULL ) {
        memcpy( header->data + self_length, src, src_length );
        header->length = ( uint32_t )( self_length + src_length );
        header->data[ header->length ] = '\0';
    }
}

/*==========================================================================================================*/
X_API void Str_AppendCStr( str_t* self_, const char* src ) {
    if ( src == NULL ) {
        return;
    }

    size_t src_length = strlen( src );
    size_t self_length = Str_Length( *self_ );
    Str_EnsureCapacity( self_, ( uint32_t )( self_length + src_length ) );
    str_header_t* header = Str_GetHeader( *self_ );
    if ( header != NULL ) {
        memcpy( header->data + self_length, src, src_length );
        header->length = ( uint32_t )( self_length + src_length );
        header->data[ header->length ] = '\0';
    }
}


/*==========================================================================================================*/
X_API void Str_FormatV( str_t * self_, const char* fmt, ... ) {
    va_list args;
    va_start( args, fmt );
    Str_FormatArg( self_, fmt, args );
    va_end( args );
}

/*==========================================================================================================*/
X_API void Str_FormatArg( str_t * self_, const char* fmt, va_list args ) {
    va_list args_copy;
    int length = vsnprintf( NULL, 0, fmt, args );

    if ( length <= 0 ) {
        return;
    }

    Str_Resize( self_, ( uint32_t )length );
;
    va_copy( args_copy, args );
    vsnprintf( *self_, length+1, fmt, args_copy );
    va_end( args_copy );
}

/*==========================================================================================================*/
X_API void Str_PathJoinCStr( str_t* self_, const char* lhs, const char* rhs ) {
    /* Only add a path separator between the two parts if the last character of lhs and first character if rhs is not
     * already a path separator
     */
    size_t rhs_length = rhs == NULL ? 0 : strlen( rhs );
    size_t lhs_length = lhs == NULL ? 0 : strlen( lhs );

    bool rhs_starts_with_separator = ( rhs_length > 0 && ( rhs[ 0 ] == '\\' || rhs[ 0 ] == '/' ) );
    bool lhs_ends_with_separator =
        ( lhs_length > 0 && ( lhs[ lhs_length - 1 ] == '\\' || lhs[ lhs_length - 1 ] == '/' ) );

    size_t total_length = lhs_length + rhs_length +
                          ( ( lhs_ends_with_separator == false ) && ( rhs_starts_with_separator == false ) ? 1 : 0 );

    Str_EnsureCapacity( self_, ( uint32_t )total_length );
    Str_Resize( self_, 0 );

    if ( ( lhs_ends_with_separator == false ) && ( rhs_starts_with_separator == false ) ) {
        Str_SetCStr( self_, lhs );
        Str_AppendCStr( self_, "/" );
        Str_AppendCStr( self_, rhs );
    } else {
        Str_SetCStr( self_, lhs );
        Str_AppendCStr( self_, rhs );
    }
}

/*==========================================================================================================*/
X_API void Str_PathJoin( str_t* self_, const str_t lhs, const str_t rhs ) {
    const char* lhs_cstr = lhs;
    const char* rhs_cstr = rhs;

    Str_PathJoinCStr( self_, lhs_cstr, rhs_cstr );
}