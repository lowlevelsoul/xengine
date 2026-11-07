/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Fs.h"
#include "xe/Mem.h"
#include "xe/Sys.h"
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

typedef struct macos_file_s {
    file_t base;
    FILE* handle;
    struct macos_file_s* next;
} macos_file_t;

typedef struct fs_macos_s {
    str_t basePath;
    str_t dataPath;
    sys_mux_t mux;
    macos_file_t* files;
} fs_macos_t;

static fs_macos_t fs;
static bool fs_initialized = false;

/*==========================================================================================================*/
static size_t MacosFile_Read( file_t* file, void* buffer, size_t size ) {
    return fread( buffer, 1, size, ( ( macos_file_t* )file )->handle );
}

/*==========================================================================================================*/
static size_t MacosFile_Write( file_t* file, const void* buffer, size_t size ) {
    return fwrite( buffer, 1, size, ( ( macos_file_t* )file )->handle );
}

/*==========================================================================================================*/
static int32_t MacosFile_Seek( file_t* file, long offset, int origin ) {
    return fseeko( ( ( macos_file_t* )file )->handle, offset, origin );
}

/*==========================================================================================================*/
static uint64_t MacosFile_Tell( file_t* file ) {
    off_t position = ftello( ( ( macos_file_t* )file )->handle );
    return position < 0 ? UINT64_MAX : ( uint64_t )position;
}

/*==========================================================================================================*/
static int32_t MacosFile_Flush( file_t* file ) {
    return fflush( ( ( macos_file_t* )file )->handle );
}

/*==========================================================================================================*/
static size_t MacosFile_Size( file_t* file ) {
    FILE* handle = ( ( macos_file_t* )file )->handle;
    off_t position = ftello( handle );
    if ( position < 0 || fseeko( handle, 0, SEEK_END ) != 0 ) {
        return 0;
    }

    off_t size = ftello( handle );
    if ( fseeko( handle, position, SEEK_SET ) != 0 || size < 0 ) {
        return 0;
    }

    return ( size_t )size;
}

/*==========================================================================================================*/
X_API void FS_Create( void ) {
    if ( fs_initialized ) {
        return;
    }

    Mem_Zero( &fs, sizeof( fs ) );
    Sys_MuxCreate( &fs.mux );
    FS_GetCurrentDirectory( &fs.basePath );
    if ( !fs.basePath ) {
        Sys_MuxDestroy( &fs.mux );
        return;
    }

    Str_PathJoinCStr( &fs.dataPath, fs.basePath, "base" );
    fs_initialized = true;
}

/*==========================================================================================================*/
X_API void FS_Destroy( void ) {
    if ( !fs_initialized ) {
        return;
    }

    while ( fs.files ) {
        FS_CloseFile( &fs.files->base );
    }

    Sys_MuxDestroy( &fs.mux );
    Str_Destroy( &fs.basePath );
    Str_Destroy( &fs.dataPath );
    fs_initialized = false;
}

/*==========================================================================================================*/
X_API void FS_GetCurrentDirectory( str_t* pathOut ) {
    if ( !pathOut ) {
        return;
    }

    @autoreleasepool {
        NSString* path = [[NSFileManager defaultManager] currentDirectoryPath];
        if ( path ) {
            Str_SetCStr( pathOut, [path fileSystemRepresentation] );
        }
    }
}

/*==========================================================================================================*/
X_API void FS_MakePath( str_t* pathOut, const char* path ) {
    if ( !fs_initialized || !pathOut || !path ) {
        return;
    }

    if ( path[ 0 ] == '~' ) {
        Str_PathJoinCStr( pathOut, fs.dataPath, path + 1 );
    } else {
        Str_SetCStr( pathOut, path );
    }
}

/* The returned string is owned by the caller's autorelease pool. */
static NSString* MacosFS_ResolvePath( const char* path ) {
    if ( !fs_initialized || !path ) {
        return nil;
    }

    str_t resolved = NULL;
    FS_MakePath( &resolved, path );
    NSString* result = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:resolved
                                                                                   length:strlen( resolved )];
    Str_Destroy( &resolved );
    return result;
}

/*==========================================================================================================*/
X_API file_t* FS_OpenFile( const char* path, const char* mode ) {
    if ( !mode ) {
        return NULL;
    }

    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        if ( !resolved ) {
            return NULL;
        }

        FILE* handle = fopen( [resolved fileSystemRepresentation], mode );
        if ( !handle ) {
            return NULL;
        }

        macos_file_t* file = Mem_Alloc( sizeof( *file ) );
        if ( !file ) {
            fclose( handle );
            return NULL;
        }

        file->handle = handle;
        file->base.read = MacosFile_Read;
        file->base.write = MacosFile_Write;
        file->base.seek = MacosFile_Seek;
        file->base.tell = MacosFile_Tell;
        file->base.flush = MacosFile_Flush;
        file->base.size = MacosFile_Size;
        Sys_MuxLock( &fs.mux );
        file->next = fs.files;
        fs.files = file;
        Sys_MuxUnlock( &fs.mux );
        return &file->base;
    }
}

/*==========================================================================================================*/
X_API void FS_CloseFile( file_t* file ) {
    if ( !fs_initialized || !file ) {
        return;
    }

    Sys_MuxLock( &fs.mux );
    macos_file_t** cursor = &fs.files;
    while ( *cursor && &( *cursor )->base != file ) {
        cursor = &( *cursor )->next;
    }

    if ( *cursor ) {
        macos_file_t* entry = *cursor;
        *cursor = entry->next;
        fclose( entry->handle );
        Mem_Free( entry );
    }

    Sys_MuxUnlock( &fs.mux );
}

/*==========================================================================================================*/
X_API bool FS_FileExists( const char* path ) {
    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        BOOL isDirectory = NO;
        return resolved && [[NSFileManager defaultManager] fileExistsAtPath:resolved isDirectory:&isDirectory] &&
               !isDirectory;
    }
}

/*==========================================================================================================*/
X_API bool FS_DirectoryExists( const char* path ) {
    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        BOOL isDirectory = NO;
        return resolved && [[NSFileManager defaultManager] fileExistsAtPath:resolved isDirectory:&isDirectory] &&
               isDirectory;
    }
}

/*==========================================================================================================*/
X_API bool FS_CreateDirectory( const char* path ) {
    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        if ( !resolved || [[NSFileManager defaultManager] fileExistsAtPath:resolved] ) {
            return false;
        }

        return [[NSFileManager defaultManager] createDirectoryAtPath:resolved
                                         withIntermediateDirectories:NO
                                                          attributes:nil
                                                               error:nil];
    }
}

/*==========================================================================================================*/
X_API bool FS_RemoveFile( const char* path ) {
    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        return resolved && unlink( [resolved fileSystemRepresentation] ) == 0;
    }
}

/*==========================================================================================================*/
X_API bool FS_RemoveDirectory( const char* path ) {
    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        return resolved && rmdir( [resolved fileSystemRepresentation] ) == 0;
    }
}

/*==========================================================================================================*/
static bool MacosFS_EnumDirectory( const char* path, fs_enum_callback_t callback, void* userData, const char* filter,
                                   bool directories ) {
    if ( !callback ) {
        return false;
    }

    @autoreleasepool {
        NSString* resolved = MacosFS_ResolvePath( path );
        if ( !resolved ) {
            return false;
        }

        NSFileManager* manager = [NSFileManager defaultManager];
        NSArray<NSString*>* entries = [manager contentsOfDirectoryAtPath:resolved error:nil];
        if ( !entries ) {
            return false;
        }

        bool found = false;
        for ( NSString* name in entries ) {
            NSString* fullPath = [resolved stringByAppendingPathComponent:name];
            BOOL isDirectory = NO;
            if ( ![manager fileExistsAtPath:fullPath isDirectory:&isDirectory] || ( bool )isDirectory != directories ) {
                continue;
            }

            if ( !directories && !FS_MatchFilter( [name fileSystemRepresentation], filter ) ) {
                continue;
            }

            found = true;
            if ( !callback( [fullPath fileSystemRepresentation], !directories, userData ) ) {
                break;
            }
        }

        return found;
    }
}

/*==========================================================================================================*/
X_API bool FS_EnumDirectoryFiles( const char* path, fs_enum_callback_t callback, void* userData, const char* filter ) {
    return MacosFS_EnumDirectory( path, callback, userData, filter, false );
}

/*==========================================================================================================*/
X_API bool FS_EnumDirectories( const char* path, fs_enum_callback_t callback, void* userData ) {
    return MacosFS_EnumDirectory( path, callback, userData, NULL, true );
}
