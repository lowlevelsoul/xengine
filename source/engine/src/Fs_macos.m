/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "xe/Fs.h"
#import "xe/Mem.h"
#import "xe/Str.h"
#import <Foundation/Foundation.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct macos_file_s {
    file_t base;
    void* handle;
} macos_file_t;

typedef struct fs_macos_s {
    str_t basePath;
    str_t dataPath;
} fs_macos_t;

static fs_macos_t fs;
static bool fs_initialized = false;
static pthread_rwlock_t fs_lock = PTHREAD_RWLOCK_INITIALIZER;

/*==========================================================================================================*/
static NSFileHandle* MacFile_Handle( file_t* file ) {
    return ( __bridge NSFileHandle* )( ( macos_file_t* )file )->handle;
}

/*==========================================================================================================*/
static size_t MacFile_Read( file_t* file, void* buffer, size_t size ) {
    @autoreleasepool {
        if ( size == 0 ) {
            return 0;
        }

        NSData* data = [MacFile_Handle( file ) readDataUpToLength:size error:nil];
        if ( data == nil || data.length == 0 ) {
            return 0;
        }

        Mem_Copy( buffer, data.bytes, data.length );
        return data.length;
    }
}

/*==========================================================================================================*/
static size_t MacFile_Write( file_t* file, const void* buffer, size_t size ) {
    @autoreleasepool {
        if ( size == 0 ) {
            return 0;
        }

        NSData* data = [NSData dataWithBytesNoCopy:( void* )buffer length:size freeWhenDone:NO];
        return [MacFile_Handle( file ) writeData:data error:nil] ? size : 0;
    }
}

/*==========================================================================================================*/
static int32_t MacFile_Seek( file_t* file, long offset, int origin ) {
    @autoreleasepool {
        NSFileHandle* handle = MacFile_Handle( file );
        unsigned long long base = 0;
        if ( origin == SEEK_CUR ) {
            if ( ![handle getOffset:&base error:nil] ) {
                return -1;
            }
        } else if ( origin == SEEK_END ) {
            unsigned long long current;
            if ( ![handle getOffset:&current error:nil] || ![handle seekToEndReturningOffset:&base error:nil] ) {
                return -1;
            }

            if ( ![handle seekToOffset:current error:nil] ) {
                return -1;
            }
        } else if ( origin != SEEK_SET ) {
            return -1;
        }

        uint64_t position;
        if ( offset < 0 ) {
            uint64_t magnitude = ( uint64_t )( -( offset + 1 ) ) + 1;
            if ( magnitude > base ) {
                return -1;
            }

            position = base - magnitude;
        } else {
            if ( ( uint64_t )offset > UINT64_MAX - base ) {
                return -1;
            }

            position = base + ( uint64_t )offset;
        }

        return [handle seekToOffset:position error:nil] ? 0 : -1;
    }
}

/*==========================================================================================================*/
static uint64_t MacFile_Tell( file_t* file ) {
    @autoreleasepool {
        unsigned long long offset;
        return [MacFile_Handle( file ) getOffset:&offset error:nil] ? offset : UINT64_MAX;
    }
}

/*==========================================================================================================*/
static int32_t MacFile_Flush( file_t* file ) {
    @autoreleasepool {
        return [MacFile_Handle( file ) synchronizeAndReturnError:nil] ? 0 : -1;
    }
}

/*==========================================================================================================*/
X_API void FS_Initialise( void ) {
    pthread_rwlock_wrlock( &fs_lock );
    if ( fs_initialized ) {
        pthread_rwlock_unlock( &fs_lock );
        return;
    }

    FS_GetCurrentDirectory( &fs.basePath );
    if ( fs.basePath == NULL ) {
        NSLog( @"Failed to determine the current directory while initializing the filesystem" );
        pthread_rwlock_unlock( &fs_lock );
        return;
    }

    Str_PathJoinCStr( &fs.dataPath, fs.basePath, "base" );
    fs_initialized = true;
    pthread_rwlock_unlock( &fs_lock );
}

/*==========================================================================================================*/
X_API void FS_Finalise( void ) {
    pthread_rwlock_wrlock( &fs_lock );
    if ( !fs_initialized ) {
        pthread_rwlock_unlock( &fs_lock );
        return;
    }

    Str_Destroy( &fs.basePath );
    Str_Destroy( &fs.dataPath );
    fs_initialized = false;
    pthread_rwlock_unlock( &fs_lock );
}

/*==========================================================================================================*/
X_API void FS_GetCurrentDirectory( str_t* pathOut ) {
    @autoreleasepool {
        if ( pathOut == NULL ) {
            return;
        }

        const char* path = [[NSFileManager defaultManager].currentDirectoryPath fileSystemRepresentation];
        if ( path != NULL ) {
            Str_SetCStr( pathOut, path );
        }
    }
}

/*==========================================================================================================*/
static bool FS_ResolvePath( str_t* pathOut, const char* path ) {
    if ( pathOut == NULL || path == NULL ) {
        return false;
    }

    pthread_rwlock_rdlock( &fs_lock );
    bool initialized = fs_initialized;
    if ( initialized ) {
        if ( path[ 0 ] == '~' ) {
            Str_PathJoinCStr( pathOut, fs.dataPath, path + 1 );
        } else {
            Str_SetCStr( pathOut, path );
        }
    }

    pthread_rwlock_unlock( &fs_lock );
    return initialized;
}

/*==========================================================================================================*/
X_API void FS_MakePath( str_t* pathOut, const char* path ) {
    FS_ResolvePath( pathOut, path );
}

/*==========================================================================================================*/
static NSString* FS_MacPath( const char* path ) {
    str_t resolved = NULL;
    if ( !FS_ResolvePath( &resolved, path ) ) {
        return nil;
    }

    NSString* result = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:resolved
                                                                                   length:strlen( resolved )];
    Str_Destroy( &resolved );
    return result;
}

/*==========================================================================================================*/
X_API file_t* FS_OpenFile( const char* path, const char* mode ) {
    @autoreleasepool {
        if ( mode == NULL || ( mode[ 0 ] != 'r' && mode[ 0 ] != 'w' && mode[ 0 ] != 'a' ) ) {
            return NULL;
        }

        bool update = false;
        bool binary = false;
        for ( const char* c = mode + 1; *c; ++c ) {
            if ( *c == '+' && !update ) {
                update = true;
            } else if ( *c == 'b' && !binary ) {
                binary = true;
            } else {
                return NULL;
            }
        }

        NSString* resolved = FS_MacPath( path );
        if ( resolved == nil ) {
            return NULL;
        }

        NSFileManager* manager = [NSFileManager defaultManager];
        BOOL isDirectory = NO;
        BOOL exists = [manager fileExistsAtPath:resolved isDirectory:&isDirectory];
        if ( isDirectory || ( mode[ 0 ] == 'r' && !exists ) ) {
            return NULL;
        }

        if ( !exists && mode[ 0 ] != 'r' ) {
            NSURL* url = [NSURL fileURLWithPath:resolved];
            if ( ![[NSData data] writeToURL:url options:NSDataWritingWithoutOverwriting error:nil] ) {
                return NULL;
            }
        }

        NSFileHandle* handle;
        if ( update ) {
            handle = [NSFileHandle fileHandleForUpdatingAtPath:resolved];
        } else if ( mode[ 0 ] == 'r' ) {
            handle = [NSFileHandle fileHandleForReadingAtPath:resolved];
        } else {
            handle = [NSFileHandle fileHandleForWritingAtPath:resolved];
        }

        if ( handle == nil ) {
            return NULL;
        }

        if ( mode[ 0 ] == 'a' ) {
            int descriptor = handle.fileDescriptor;
            int flags = fcntl( descriptor, F_GETFL );
            if ( flags == -1 || fcntl( descriptor, F_SETFL, flags | O_APPEND ) == -1 ) {
                [handle closeAndReturnError:nil];
                return NULL;
            }
        }

        if ( ( mode[ 0 ] == 'w' && ![handle truncateAtOffset:0 error:nil] ) ||
             ( mode[ 0 ] == 'a' && ![handle seekToEndReturningOffset:NULL error:nil] ) ) {
            [handle closeAndReturnError:nil];
            return NULL;
        }

        macos_file_t* file = Mem_Alloc( sizeof( *file ) );
        if ( file == NULL ) {
            [handle closeAndReturnError:nil];
            return NULL;
        }

        file->handle = ( void* )CFBridgingRetain( handle );
        file->base.read = MacFile_Read;
        file->base.write = MacFile_Write;
        file->base.seek = MacFile_Seek;
        file->base.tell = MacFile_Tell;
        file->base.flush = MacFile_Flush;
        return &file->base;
    }
}

/*==========================================================================================================*/
X_API void FS_CloseFile( file_t* file ) {
    if ( file == NULL ) {
        return;
    }

    @autoreleasepool {
        macos_file_t* macFile = ( macos_file_t* )file;
        NSError* error = nil;
        if ( ![MacFile_Handle( file ) closeAndReturnError:&error] ) {
            NSLog( @"Failed to close file: %@", error );
        }

        CFRelease( macFile->handle );
        Mem_Free( macFile );
    }
}

/*==========================================================================================================*/
static bool FS_MacExists( const char* path, bool directory ) {
    @autoreleasepool {
        NSString* resolved = FS_MacPath( path );
        if ( resolved == nil ) {
            return false;
        }

        BOOL isDirectory = NO;
        return [[NSFileManager defaultManager] fileExistsAtPath:resolved isDirectory:&isDirectory] &&
               ( ( isDirectory != NO ) == directory );
    }
}

/*==========================================================================================================*/
X_API bool FS_FileExists( const char* path ) {
    return FS_MacExists( path, false );
}

/*==========================================================================================================*/
X_API bool FS_DirectoryExists( const char* path ) {
    return FS_MacExists( path, true );
}

/*==========================================================================================================*/
X_API bool FS_CreateDirectory( const char* path ) {
    @autoreleasepool {
        NSString* resolved = FS_MacPath( path );
        if ( resolved == nil ) {
            return false;
        }

        NSFileManager* manager = [NSFileManager defaultManager];
        return ![manager fileExistsAtPath:resolved] && [manager createDirectoryAtPath:resolved
                                                           withIntermediateDirectories:NO
                                                                            attributes:nil
                                                                                 error:nil];
    }
}

/*==========================================================================================================*/
static bool FS_MacRemove( const char* path, bool directory ) {
    @autoreleasepool {
        NSString* resolved = FS_MacPath( path );
        if ( resolved == nil ) {
            return false;
        }

        NSFileManager* manager = [NSFileManager defaultManager];
        BOOL isDirectory = NO;
        if ( ![manager fileExistsAtPath:resolved isDirectory:&isDirectory] || ( ( isDirectory != NO ) != directory ) ) {
            return false;
        }

        /* Foundation removes directory trees recursively; these calls never remove children. */
        const char* fileSystemPath = resolved.fileSystemRepresentation;
        if ( fileSystemPath == NULL ) {
            return false;
        }

        return directory ? rmdir( fileSystemPath ) == 0 : unlink( fileSystemPath ) == 0;
    }
}

/*==========================================================================================================*/
X_API bool FS_RemoveFile( const char* path ) {
    return FS_MacRemove( path, false );
}

/*==========================================================================================================*/
X_API bool FS_RemoveDirectory( const char* path ) {
    return FS_MacRemove( path, true );
}

/*==========================================================================================================*/
static bool FS_MacEnum( const char* path, fs_enum_callback_t callback, void* userData, const char* filter,
                        bool directories ) {
    @autoreleasepool {
        NSString* resolved = FS_MacPath( path );
        if ( resolved == nil || callback == NULL ) {
            return false;
        }

        NSFileManager* manager = [NSFileManager defaultManager];
        NSArray<NSString*>* contents = [manager contentsOfDirectoryAtPath:resolved error:nil];
        if ( contents == nil ) {
            return false;
        }

        bool found = false;
        for ( NSString* name in contents ) {
            NSString* fullPath = [resolved stringByAppendingPathComponent:name];
            BOOL isDirectory = NO;
            if ( ![manager fileExistsAtPath:fullPath isDirectory:&isDirectory] ||
                 ( ( isDirectory != NO ) != directories ) ) {
                continue;
            }

            const char* fileName = name.fileSystemRepresentation;
            const char* fullName = fullPath.fileSystemRepresentation;
            if ( fileName == NULL || fullName == NULL || ( !directories && !FS_MatchFilter( fileName, filter ) ) ) {
                continue;
            }

            found = true;
            callback( fullName, !directories, userData );
        }

        return found;
    }
}

/*==========================================================================================================*/
X_API bool FS_EnumDirectoryFiles( const char* path, fs_enum_callback_t callback, void* userData, const char* filter ) {
    return FS_MacEnum( path, callback, userData, filter, false );
}

/*==========================================================================================================*/
X_API bool FS_EnumDirectories( const char* path, fs_enum_callback_t callback, void* userData ) {
    return FS_MacEnum( path, callback, userData, NULL, true );
}
