/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Fs.h"
#include "xe/Str.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define APPEND_THREAD_COUNT 4
#define APPEND_RECORD_COUNT 256
#define APPEND_RECORD_SIZE 16

typedef struct enum_expected_s {
    const char* path;
    bool isFile;
} enum_expected_t;

typedef struct enum_state_s {
    const enum_expected_t* expected;
    size_t expectedCount;
    size_t count;
    bool valid;
} enum_state_t;

typedef struct append_thread_s {
    const char* path;
    char token;
    HANDLE startEvent;
    bool success;
} append_thread_t;

static char* Utf8FromWide( const wchar_t* wide ) {
    int length = WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, NULL, 0, NULL, NULL );
    if ( length == 0 ) {
        return NULL;
    }

    char* result = malloc( ( size_t )length );
    if ( result == NULL ) {
        return NULL;
    }

    if ( WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, result, length, NULL, NULL ) == 0 ) {
        free( result );
        return NULL;
    }

    return result;
}

static char* CreateFixtureRoot( void ) {
    wchar_t temporaryDirectory[ MAX_PATH ];
    DWORD directoryLength = GetTempPathW( MAX_PATH, temporaryDirectory );
    assert( directoryLength != 0 && directoryLength < MAX_PATH );

    wchar_t fixturePath[ MAX_PATH ];
    UINT fileResult = GetTempFileNameW( temporaryDirectory, L"xef", 0, fixturePath );
    assert( fileResult != 0 );

    BOOL deleteResult = DeleteFileW( fixturePath );
    assert( deleteResult );

    BOOL createResult = CreateDirectoryW( fixturePath, NULL );
    assert( createResult );

    return Utf8FromWide( fixturePath );
}

static char* JoinPath( const char* parent, const char* name ) {
    size_t parentLength = strlen( parent );
    size_t nameLength = strlen( name );
    bool hasSeparator = parentLength != 0 && parent[ parentLength - 1 ] != '\\' && parent[ parentLength - 1 ] != '/';
    char* result = malloc( parentLength + ( hasSeparator ? 1 : 0 ) + nameLength + 1 );
    assert( result != NULL );

    memcpy( result, parent, parentLength );
    size_t offset = parentLength;
    if ( hasSeparator ) {
        result[ offset++ ] = '/';
    }

    memcpy( result + offset, name, nameLength + 1 );
    return result;
}

static bool CheckEnumerationEntry( const char* path, bool isFile, void* userData ) {
    enum_state_t* state = userData;
    bool matched = false;
    for ( size_t i = 0; i < state->expectedCount; ++i ) {
        if ( state->expected[ i ].isFile == isFile && strcmp( state->expected[ i ].path, path ) == 0 ) {
            matched = true;
            break;
        }
    }

    state->valid = state->valid && matched;
    state->valid = state->valid && ( isFile ? FS_FileExists( path ) : FS_DirectoryExists( path ) );
    ++state->count;
    return true;
}

static DWORD WINAPI AppendRecords( void* argument ) {
    append_thread_t* thread = argument;
    if ( WaitForSingleObject( thread->startEvent, INFINITE ) != WAIT_OBJECT_0 ) {
        thread->success = false;
        return 1;
    }

    file_t* file = FS_OpenFile( thread->path, "ab" );
    if ( file == NULL ) {
        thread->success = false;
        return 1;
    }

    char record[ APPEND_RECORD_SIZE ];
    memset( record, thread->token, sizeof( record ) );
    thread->success = true;
    for ( int i = 0; i < APPEND_RECORD_COUNT; ++i ) {
        if ( file->write( file, record, sizeof( record ) ) != sizeof( record ) ) {
            thread->success = false;
            break;
        }
    }

    FS_CloseFile( file );
    return thread->success ? 0 : 1;
}

static void TestFileModesAndOffsets( const char* root ) {
    char* filePath = JoinPath( root, "caf\xc3\xa9.txt" );
    file_t* file = FS_OpenFile( filePath, "r" );
    assert( file == NULL );
    assert( FS_OpenFile( filePath, "invalid" ) == NULL );

    file = FS_OpenFile( filePath, "w" );
    assert( file != NULL );
    assert( file->write( file, "hello", 5 ) == 5 );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "r" );
    assert( file != NULL );
    char buffer[ 8 ] = { 0 };
    assert( file->read( file, buffer, 5 ) == 5 );
    assert( memcmp( buffer, "hello", 5 ) == 0 );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "r+" );
    assert( file != NULL );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    assert( file->write( file, "H", 1 ) == 1 );
    assert( file->tell( file ) == 1 );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "r+b" );
    assert( file != NULL );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    assert( file->read( file, buffer, 1 ) == 1 );
    assert( buffer[ 0 ] == 'H' );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "w+" );
    assert( file != NULL );
    assert( file->write( file, "hello", 5 ) == 5 );
    assert( file->tell( file ) == 5 );
    assert( file->flush( file ) == 0 );
    assert( file->seek( file, -2, SEEK_END ) == 0 );
    memset( buffer, 0, sizeof( buffer ) );
    assert( file->read( file, buffer, 2 ) == 2 );
    assert( memcmp( buffer, "lo", 2 ) == 0 );
    assert( file->seek( file, -6, SEEK_SET ) == -1 );
    assert( file->tell( file ) == 5 );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "a+" );
    assert( file != NULL );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    assert( file->write( file, "!", 1 ) == 1 );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    memset( buffer, 0, sizeof( buffer ) );
    assert( file->read( file, buffer, 6 ) == 6 );
    assert( memcmp( buffer, "hello!", 6 ) == 0 );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "a+b" );
    assert( file != NULL );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    assert( file->write( file, "?", 1 ) == 1 );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    memset( buffer, 0, sizeof( buffer ) );
    assert( file->read( file, buffer, 7 ) == 7 );
    assert( memcmp( buffer, "hello!?", 7 ) == 0 );
    FS_CloseFile( file );

    file_t* first = FS_OpenFile( filePath, "a" );
    file_t* second = FS_OpenFile( filePath, "a" );
    assert( first != NULL && second != NULL );
    assert( first->write( first, "1", 1 ) == 1 );
    assert( second->write( second, "2", 1 ) == 1 );
    FS_CloseFile( first );
    FS_CloseFile( second );
    file = FS_OpenFile( filePath, "r" );
    assert( file != NULL );
    assert( file->seek( file, -2, SEEK_END ) == 0 );
    memset( buffer, 0, sizeof( buffer ) );
    assert( file->read( file, buffer, 2 ) == 2 );
    assert( memcmp( buffer, "12", 2 ) == 0 );
    FS_CloseFile( file );

    file = FS_OpenFile( filePath, "wb" );
    assert( file != NULL );
    assert( file->tell( file ) == 0 );
    FS_CloseFile( file );
    file = FS_OpenFile( filePath, "rb" );
    assert( file != NULL );
    assert( file->read( file, buffer, sizeof( buffer ) ) == 0 );
    FS_CloseFile( file );

    char* largeFilePath = JoinPath( root, "large.bin" );
    file = FS_OpenFile( largeFilePath, "w+b" );
    assert( file != NULL );
    const long step = 1073741824L;
    assert( step <= LONG_MAX );
    for ( int i = 0; i < 4; ++i ) {
        assert( file->seek( file, step, i == 0 ? SEEK_SET : SEEK_CUR ) == 0 );
    }

    assert( file->tell( file ) == UINT64_C( 4294967296 ) );
    assert( file->seek( file, -1, SEEK_CUR ) == 0 );
    assert( file->tell( file ) == UINT64_C( 4294967295 ) );
    FS_CloseFile( file );

    assert( FS_RemoveFile( filePath ) );
    assert( FS_RemoveFile( largeFilePath ) );
    free( largeFilePath );
    free( filePath );
}

static void TestConcurrentAppend( const char* root ) {
    char* filePath = JoinPath( root, "concurrent.log" );
    file_t* file = FS_OpenFile( filePath, "w" );
    assert( file != NULL );
    FS_CloseFile( file );

    append_thread_t threads[ APPEND_THREAD_COUNT ];
    HANDLE handles[ APPEND_THREAD_COUNT ];
    HANDLE startEvent = CreateEventW( NULL, TRUE, FALSE, NULL );
    assert( startEvent != NULL );
    for ( int i = 0; i < APPEND_THREAD_COUNT; ++i ) {
        threads[ i ].path = filePath;
        threads[ i ].token = ( char )( 'A' + i );
        threads[ i ].startEvent = startEvent;
        threads[ i ].success = false;
        handles[ i ] = CreateThread( NULL, 0, AppendRecords, &threads[ i ], 0, NULL );
        assert( handles[ i ] != NULL );
    }

    BOOL releaseResult = SetEvent( startEvent );
    assert( releaseResult );
    DWORD waitResult = WaitForMultipleObjects( APPEND_THREAD_COUNT, handles, TRUE, INFINITE );
    assert( waitResult == WAIT_OBJECT_0 );
    CloseHandle( startEvent );
    for ( int i = 0; i < APPEND_THREAD_COUNT; ++i ) {
        assert( threads[ i ].success );
        CloseHandle( handles[ i ] );
    }

    file = FS_OpenFile( filePath, "rb" );
    assert( file != NULL );
    unsigned char contents[ APPEND_THREAD_COUNT * APPEND_RECORD_COUNT * APPEND_RECORD_SIZE ];
    size_t contentSize = sizeof( contents );
    assert( file->read( file, contents, contentSize ) == contentSize );
    FS_CloseFile( file );

    size_t tokenCounts[ APPEND_THREAD_COUNT ] = { 0 };
    for ( size_t offset = 0; offset < contentSize; offset += APPEND_RECORD_SIZE ) {
        unsigned char token = contents[ offset ];
        assert( token >= 'A' && token < 'A' + APPEND_THREAD_COUNT );
        for ( size_t i = 1; i < APPEND_RECORD_SIZE; ++i ) {
            assert( contents[ offset + i ] == token );
        }

        ++tokenCounts[ token - 'A' ];
    }

    for ( size_t i = 0; i < APPEND_THREAD_COUNT; ++i ) {
        assert( tokenCounts[ i ] == APPEND_RECORD_COUNT );
    }

    assert( FS_RemoveFile( filePath ) );
    free( filePath );
}

static void TestEnumeration( const char* root ) {
    char* directoryPath = JoinPath( root, "enumeration" );
    char* textPath = JoinPath( directoryPath, "notes.TXT" );
    char* logPath = JoinPath( directoryPath, "events.log" );
    char* binaryPath = JoinPath( directoryPath, "skip.bin" );
    char* nestedPath = JoinPath( directoryPath, "nested" );

    assert( FS_CreateDirectory( directoryPath ) );
    const char* filePaths[] = { textPath, logPath, binaryPath };
    for ( size_t i = 0; i < sizeof( filePaths ) / sizeof( filePaths[ 0 ] ); ++i ) {
        file_t* file = FS_OpenFile( filePaths[ i ], "w" );
        assert( file != NULL );
        FS_CloseFile( file );
    }

    assert( FS_CreateDirectory( nestedPath ) );
    enum_expected_t fileExpected[] = { { textPath, true }, { logPath, true } };
    enum_state_t fileState = { fileExpected, sizeof( fileExpected ) / sizeof( fileExpected[ 0 ] ), 0, true };
    assert( FS_EnumDirectoryFiles( directoryPath, CheckEnumerationEntry, &fileState, "txt;log" ) );
    assert( fileState.valid && fileState.count == 2 );

    enum_expected_t directoryExpected[] = { { nestedPath, false } };
    enum_state_t directoryState = { directoryExpected, 1, 0, true };
    assert( FS_EnumDirectories( directoryPath, CheckEnumerationEntry, &directoryState ) );
    assert( directoryState.valid && directoryState.count == 1 );

    enum_state_t noMatchState = { NULL, 0, 0, true };
    assert( !FS_EnumDirectoryFiles( directoryPath, CheckEnumerationEntry, &noMatchState, "missing" ) );
    assert( noMatchState.count == 0 );
    assert( !FS_EnumDirectoryFiles( directoryPath, NULL, NULL, NULL ) );

    assert( FS_RemoveFile( textPath ) );
    assert( FS_RemoveFile( logPath ) );
    assert( FS_RemoveFile( binaryPath ) );
    assert( FS_RemoveDirectory( nestedPath ) );
    assert( FS_RemoveDirectory( directoryPath ) );
    free( nestedPath );
    free( binaryPath );
    free( logPath );
    free( textPath );
    free( directoryPath );
}

static void TestDirectoryBehavior( const char* root ) {
    char* parentPath = JoinPath( root, "parent" );
    char* childPath = JoinPath( parentPath, "child" );
    assert( FS_CreateDirectory( parentPath ) );
    assert( FS_CreateDirectory( childPath ) );
    assert( !FS_RemoveDirectory( parentPath ) );
    assert( FS_DirectoryExists( childPath ) );
    assert( FS_RemoveDirectory( childPath ) );
    assert( FS_RemoveDirectory( parentPath ) );
    free( childPath );
    free( parentPath );

    assert( FS_FileExists( NULL ) == false );
    assert( FS_DirectoryExists( NULL ) == false );
    assert( FS_CreateDirectory( NULL ) == false );
    assert( FS_RemoveFile( NULL ) == false );
    assert( FS_RemoveDirectory( NULL ) == false );
    assert( FS_OpenFile( NULL, "r" ) == NULL );
    assert( FS_OpenFile( "unused", NULL ) == NULL );
    assert( FS_EnumDirectoryFiles( NULL, CheckEnumerationEntry, NULL, NULL ) == false );
    assert( FS_EnumDirectories( NULL, CheckEnumerationEntry, NULL ) == false );
    FS_GetCurrentDirectory( NULL );
    str_t resolvedPath = NULL;
    FS_MakePath( &resolvedPath, NULL );
    assert( resolvedPath == NULL );
}

static void TestCurrentDirectoryLength( void ) {
    str_t currentDirectory = NULL;
    FS_GetCurrentDirectory( &currentDirectory );
    assert( currentDirectory != NULL );

    DWORD capacity = GetCurrentDirectoryW( 0, NULL );
    assert( capacity != 0 );
    wchar_t* wideCurrentDirectory = malloc( ( size_t )capacity * sizeof( *wideCurrentDirectory ) );
    assert( wideCurrentDirectory != NULL );
    DWORD length = GetCurrentDirectoryW( capacity, wideCurrentDirectory );
    assert( length != 0 && length < capacity );
    char* expected = Utf8FromWide( wideCurrentDirectory );
    assert( expected != NULL );
    assert( strlen( currentDirectory ) == strlen( expected ) );
    assert( strcmp( currentDirectory, expected ) == 0 );

    free( expected );
    free( wideCurrentDirectory );
    Str_Destroy( &currentDirectory );
}

int main( void ) {
    char* root = CreateFixtureRoot();
    assert( root != NULL );
    FS_Initialise();
    FS_Initialise();

    TestCurrentDirectoryLength();
    TestFileModesAndOffsets( root );
    TestConcurrentAppend( root );
    TestEnumeration( root );
    TestDirectoryBehavior( root );

    assert( FS_RemoveDirectory( root ) );
    free( root );
    FS_Finalise();
    FS_Finalise();
    return 0;
}
