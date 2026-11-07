/*
 * Run from the repository root:
 * clang -I source/engine/inc -I source/engine/inc/xe source/engine/tests/Fs_macos_test.m \
 *     source/engine/src/Fs_macos.m source/engine/src/Fs_shared.c source/engine/src/Str.c \
 *     source/engine/src/Mem.c -framework Foundation -o /tmp/xengine-fs-test && /tmp/xengine-fs-test
 */
#include "xe/Fs.h"
#include "xe/Sys.h"
#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void Sys_MuxCreate( sys_mux_t* mux ) {
    assert( sizeof( *mux ) >= sizeof( pthread_mutex_t ) );
    assert( pthread_mutex_init( ( pthread_mutex_t* )mux, NULL ) == 0 );
}

void Sys_MuxDestroy( sys_mux_t* mux ) {
    assert( pthread_mutex_destroy( ( pthread_mutex_t* )mux ) == 0 );
}

void Sys_MuxLock( sys_mux_t* mux ) {
    assert( pthread_mutex_lock( ( pthread_mutex_t* )mux ) == 0 );
}

void Sys_MuxUnlock( sys_mux_t* mux ) {
    assert( pthread_mutex_unlock( ( pthread_mutex_t* )mux ) == 0 );
}

typedef struct enum_state_s {
    int count;
    bool isFile;
    bool stop;
} enum_state_t;

static bool CheckEntry( const char* path, bool isFile, void* userData ) {
    enum_state_t* state = userData;
    assert( isFile == state->isFile );
    assert( isFile ? FS_FileExists( path ) : FS_DirectoryExists( path ) );
    ++state->count;
    return !state->stop;
}

int main( void ) {
    char tmp[] = "/tmp/xengine-fs-test-XXXXXX";
    char* directory = mkdtemp( tmp );
    assert( directory );
    char canonical[ PATH_MAX ];
    assert( realpath( directory, canonical ) );
    assert( chdir( directory ) == 0 );

    assert( !FS_FileExists( "missing" ) );
    FS_Create();
    FS_Create();

    str_t path = NULL;
    FS_GetCurrentDirectory( &path );
    assert( strcmp( path, canonical ) == 0 );
    FS_MakePath( &path, "~/hello.TXT" );
    assert( strstr( path, "/base/hello.TXT" ) );
    FS_MakePath( &path, "relative" );
    assert( strcmp( path, "relative" ) == 0 );
    Str_Destroy( &path );
    Str_Reserve( &path, 7 );
    Str_SetCStr( &path, "12345678" );
    assert( strcmp( path, "12345678" ) == 0 );
    Str_Destroy( &path );

    assert( FS_CreateDirectory( "base" ) );
    assert( !FS_CreateDirectory( "base" ) );
    assert( !FS_CreateDirectory( "missing/child" ) );
    assert( FS_DirectoryExists( "base" ) );
    assert( !FS_FileExists( "base" ) );

    file_t* file = FS_OpenFile( "~/hello.TXT", "w+" );
    assert( file );
    assert( file->write( file, "hello", 5 ) == 5 );
    assert( file->flush( file ) == 0 );
    assert( file->tell( file ) == 5 );
    assert( file->size( file ) == 5 );
    assert( file->tell( file ) == 5 );
    assert( file->seek( file, 0, SEEK_SET ) == 0 );
    char buffer[ 16 ] = { 0 };
    assert( file->read( file, buffer, sizeof( buffer ) ) == 5 );
    assert( strcmp( buffer, "hello" ) == 0 );
    FS_CloseFile( file );
    assert( FS_FileExists( "~/hello.TXT" ) );
    assert( !FS_DirectoryExists( "~/hello.TXT" ) );

    file = FS_OpenFile( "~/hello.TXT", "a" );
    assert( file );
    assert( file->write( file, "!", 1 ) == 1 );
    FS_CloseFile( file );
    file = FS_OpenFile( "~/hello.TXT", "r" );
    assert( file );
    assert( file->size( file ) == 6 );
    FS_CloseFile( file );
    assert( !FS_OpenFile( "~/missing", "r" ) );

    file = FS_OpenFile( "~/another.bin", "w" );
    assert( file );
    FS_CloseFile( file );
    file = FS_OpenFile( "~/caf\xc3\xa9.dat", "w" );
    assert( file );
    FS_CloseFile( file );
    assert( FS_FileExists( "~/caf\xc3\xa9.dat" ) );
    assert( FS_RemoveFile( "~/caf\xc3\xa9.dat" ) );
    assert( FS_CreateDirectory( "~/child" ) );
    assert( FS_CreateDirectory( "~/child2" ) );

    enum_state_t state = { 0, true, false };
    assert( FS_EnumDirectoryFiles( "~/", CheckEntry, &state, "txt;cpp" ) );
    assert( state.count == 1 );
    state.count = 0;
    assert( FS_EnumDirectoryFiles( "~/", CheckEntry, &state, NULL ) );
    assert( state.count == 2 );
    state.count = 0;
    state.stop = true;
    assert( FS_EnumDirectoryFiles( "~/", CheckEntry, &state, NULL ) );
    assert( state.count == 1 );
    state.count = 0;
    state.isFile = false;
    state.stop = false;
    assert( FS_EnumDirectories( "~/", CheckEntry, &state ) );
    assert( state.count == 2 );
    assert( !FS_EnumDirectoryFiles( "~/", CheckEntry, &state, "nomatch" ) );
    assert( !FS_EnumDirectories( "~/missing", CheckEntry, &state ) );
    assert( !FS_EnumDirectories( "~/", NULL, &state ) );

    assert( !FS_RemoveFile( "~/child" ) );
    assert( !FS_RemoveDirectory( "~/hello.TXT" ) );
    assert( !FS_RemoveDirectory( "base" ) );
    assert( FS_RemoveDirectory( "~/child" ) );
    assert( FS_RemoveDirectory( "~/child2" ) );
    assert( FS_RemoveFile( "~/hello.TXT" ) );
    assert( FS_RemoveFile( "~/another.bin" ) );
    assert( FS_RemoveDirectory( "base" ) );

    file = FS_OpenFile( "open-on-destroy", "w" );
    assert( file );
    FS_Destroy();
    FS_Destroy();
    assert( !FS_OpenFile( "open-on-destroy", "r" ) );
    FS_Create();
    assert( FS_FileExists( "open-on-destroy" ) );
    FS_Destroy();
    assert( unlink( "open-on-destroy" ) == 0 );
    assert( chdir( "/" ) == 0 );
    assert( rmdir( directory ) == 0 );
    puts( "macOS filesystem tests passed" );
    return 0;
}
