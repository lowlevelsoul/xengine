/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Platform.h"

extern uint64_t farmhash64( const char* s, size_t len );

/*==========================================================================================================*/
X_API uint64_t Hash_BytesFh64( const void* data, size_t size ) {
    return farmhash64( ( const char* )data, size );
}

/*==========================================================================================================*/
X_API uint64_t Hash_StringFh64( const char* str ) {
    return farmhash64( str, strlen( str ) );
}