/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/AtomicString.h"
#include "xe/Sys.h"
#include "xe/Hash.h"
#include "xe/BSearch.h"
#include "xe/Mem.h"
#include "xe/Array.h"

#define MAX_NUM_STRINGS 4096
#define MAX_STRING_BUFFER_SIZE 65536

typedef struct astr_table_s {
    char* stringBuffer;
    uint32_t stringCount;
    uint32_t stringBufferSize;
    uint32_t stringBufferOffset;
    uint32_t* stringOffsets;
    uint64_t* stringHashes;
    sys_mux_t mutex;
} astr_table_t;

static astr_table_t astrTable;
static bool astrInitialised = false;

/*==========================================================================================================*/
X_API void AStr_Initialise( void ) {
    if ( astrInitialised == true ) {
        return;
    }

    astrTable.stringBuffer = ( char* )Mem_Alloc( MAX_STRING_BUFFER_SIZE );
    astrTable.stringCount = 0;
    astrTable.stringBufferSize = MAX_STRING_BUFFER_SIZE;
    astrTable.stringOffsets = 0;
    astrTable.stringOffsets = ( uint32_t* )Mem_Calloc( sizeof( uint32_t ), MAX_NUM_STRINGS );
    astrTable.stringHashes = ( uint64_t* )Mem_Calloc( sizeof( uint64_t ), MAX_NUM_STRINGS );
    Sys_MuxCreate( &astrTable.mutex );

    astrInitialised = true;
}

/*==========================================================================================================*/
X_API void AStr_Finalise( void ) {
    if ( astrInitialised == false ) {
        return;
    }

    Sys_MuxDestroy( &astrTable.mutex );
    Mem_Free( astrTable.stringBuffer );
    Mem_Free( astrTable.stringOffsets );
    Mem_Free( astrTable.stringHashes );

    astrInitialised = false;
}

/*==========================================================================================================*/
X_API astr_t AStr_Set( const char* str ) {
    uint64_t hash = Hash_StringFh64( str );

    Sys_MuxLock( &astrTable.mutex );

    /* Find the string hash. If it's already there, we'll just return the offset of the string */
    size_t index = 0;
    bool found = Array_UInt64_FindSorted( astrTable.stringHashes, astrTable.stringCount, &hash, &index );
    if ( found == true ) {
        Sys_MuxUnlock( &astrTable.mutex );
        return astrTable.stringOffsets[ index ];
    }

    /* We didn't find the string, so let's add it */

    /* Add the hash and string offset */
    size_t strLen = strlen( str );
    uint32_t strOffset = astrTable.stringBufferOffset;
    uint32_t newStrOffset = ( uint32_t )( astrTable.stringBufferOffset + strLen + 1 );

    Sys_FatalErrorOn( astrTable.stringCount >= MAX_NUM_STRINGS, "Too many atomic strings" );
    Sys_FatalErrorOn( newStrOffset >= MAX_STRING_BUFFER_SIZE, "Atomic string buffer exausted" );

    size_t insertIndex;
    Array_UInt64_InsertSorted( astrTable.stringHashes, astrTable.stringCount, MAX_NUM_STRINGS, &insertIndex, &hash );
    Array_UInt32_Insert( astrTable.stringOffsets, astrTable.stringCount, MAX_NUM_STRINGS, insertIndex, &strOffset );
    ++astrTable.stringCount;

    /* Copy the string to the buffer */
    strcpy( astrTable.stringBuffer + strOffset, str );
    astrTable.stringBufferOffset = newStrOffset;

    Sys_MuxUnlock( &astrTable.mutex );

    return ( astr_t )strOffset;
}

/*==========================================================================================================*/
X_API const char* AStr_GetString( astr_t astr ) {
    // Sys_AssertMsg( index < astrTable.stringBufferOffset, "Invalid atomic string" );
    if ( ( astr < 0 ) || ( ( uint32_t )astr >= astrTable.stringBufferOffset ) ) {
        return "";
    }

    return astrTable.stringBuffer + astr;
}