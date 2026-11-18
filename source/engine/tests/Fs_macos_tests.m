/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "xe/Fs.h"
#import "xe/Str.h"
#import <Foundation/Foundation.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

typedef struct enum_result_s {
    int files;
    int directories;
} enum_result_t;

static bool CountEntry( const char* path, bool isFile, void* userData ) {
    enum_result_t* result = userData;
    if ( isFile ) {
        assert( FS_FileExists( path ) );
        ++result->files;
    } else {
        assert( FS_DirectoryExists( path ) );
        ++result->directories;
    }

    return true;
}

static void* CycleLifecycle( void* arg ) {
    const char* path = arg;
    for ( int i = 0; i < 500; ++i ) {
        FS_Finalise();
        FS_Initialise();
        FS_FileExists( path );
    }

    return NULL;
}

int main( void ) {
    @autoreleasepool {
        NSFileManager* manager = [NSFileManager defaultManager];
        NSString* root = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSUUID UUID].UUIDString];
        assert( [manager createDirectoryAtPath:root withIntermediateDirectories:NO attributes:nil error:nil] );
        NSString* filePath = [root stringByAppendingPathComponent:@"file.TXT"];
        NSString* directoryPath = [root stringByAppendingPathComponent:@"nested"];
        const char* fileName = filePath.fileSystemRepresentation;
        const char* directoryName = directoryPath.fileSystemRepresentation;
        const char* rootName = root.fileSystemRepresentation;

        assert( FS_OpenFile( fileName, "w" ) == NULL );
        FS_Initialise();
        FS_Initialise();

        str_t current = NULL;
        str_t path = NULL;
        FS_GetCurrentDirectory( &current );
        FS_MakePath( &path, "~/test" );
        NSString* expected = [[manager.currentDirectoryPath stringByAppendingPathComponent:@"base"]
            stringByAppendingPathComponent:@"test"];
        assert( strcmp( path, expected.fileSystemRepresentation ) == 0 );
        assert( strcmp( current, manager.currentDirectoryPath.fileSystemRepresentation ) == 0 );
        Str_Destroy( &path );
        Str_Destroy( &current );

        assert( FS_OpenFile( fileName, "r" ) == NULL );
        assert( FS_OpenFile( fileName, "invalid" ) == NULL );
        file_t* file = FS_OpenFile( fileName, "w+" );
        assert( file != NULL );
        assert( file->write( file, "hello", 5 ) == 5 );
        assert( file->tell( file ) == 5 );
        assert( file->flush( file ) == 0 );
        assert( file->seek( file, -2, SEEK_END ) == 0 );
        char buffer[ 6 ] = { 0 };
        assert( file->read( file, buffer, 2 ) == 2 );
        assert( strcmp( buffer, "lo" ) == 0 );
        assert( file->seek( file, -6, SEEK_SET ) == -1 );
        assert( file->tell( file ) == 5 );
        FS_CloseFile( file );

        file = FS_OpenFile( fileName, "a+" );
        assert( file != NULL );
        assert( file->seek( file, 0, SEEK_SET ) == 0 );
        assert( file->write( file, "!", 1 ) == 1 );
        assert( file->seek( file, 0, SEEK_SET ) == 0 );
        assert( file->read( file, buffer, sizeof( buffer ) ) == sizeof( buffer ) );
        assert( memcmp( buffer, "hello!", sizeof( buffer ) ) == 0 );
        FS_CloseFile( file );

        file_t* first = FS_OpenFile( fileName, "a" );
        file_t* second = FS_OpenFile( fileName, "a" );
        assert( first != NULL && second != NULL );
        assert( first->write( first, "1", 1 ) == 1 );
        assert( second->write( second, "2", 1 ) == 1 );
        FS_CloseFile( first );
        FS_CloseFile( second );
        file = FS_OpenFile( fileName, "r" );
        assert( file != NULL );
        assert( file->seek( file, -2, SEEK_END ) == 0 );
        assert( file->read( file, buffer, 2 ) == 2 );
        assert( memcmp( buffer, "12", 2 ) == 0 );
        FS_CloseFile( file );

        assert( FS_CreateDirectory( directoryName ) );
        assert( !FS_CreateDirectory( directoryName ) );
        assert( FS_FileExists( fileName ) );
        assert( !FS_DirectoryExists( fileName ) );
        assert( FS_DirectoryExists( directoryName ) );
        assert( !FS_FileExists( directoryName ) );
        assert( !FS_RemoveFile( directoryName ) );
        assert( !FS_RemoveDirectory( fileName ) );
        assert( !FS_RemoveDirectory( rootName ) );

        enum_result_t result = { 0 };
        assert( FS_EnumDirectoryFiles( rootName, CountEntry, &result, "png;txt" ) );
        assert( result.files == 1 && result.directories == 0 );
        assert( !FS_EnumDirectoryFiles( rootName, CountEntry, &result, "png" ) );
        assert( FS_EnumDirectories( rootName, CountEntry, &result ) );
        assert( result.files == 1 && result.directories == 1 );
        assert( !FS_EnumDirectoryFiles( rootName, NULL, NULL, NULL ) );

        pthread_t threads[ 4 ];
        for ( size_t i = 0; i < 4; ++i ) {
            assert( pthread_create( &threads[ i ], NULL, CycleLifecycle, ( void* )fileName ) == 0 );
        }

        for ( size_t i = 0; i < 4; ++i ) {
            assert( pthread_join( threads[ i ], NULL ) == 0 );
        }

        FS_Initialise();
        assert( FS_FileExists( fileName ) );

        file = FS_OpenFile( fileName, "wb" );
        assert( file != NULL );
        assert( file->tell( file ) == 0 );
        FS_CloseFile( file );
        file = FS_OpenFile( fileName, "rb" );
        assert( file != NULL );
        assert( file->read( file, buffer, sizeof( buffer ) ) == 0 );
        FS_CloseFile( file );

        assert( FS_RemoveFile( fileName ) );
        assert( FS_RemoveDirectory( directoryName ) );
        assert( FS_RemoveDirectory( rootName ) );
        FS_Finalise();
        FS_Finalise();
        assert( !FS_FileExists( fileName ) );
        FS_Initialise();
        FS_Finalise();
    }

    return 0;
}
