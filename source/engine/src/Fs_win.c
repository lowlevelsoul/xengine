/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Fs.h"
#include "xe/Str.h"
#include "xe/Mem.h"
#include "xe/Sys.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct win_file_s {
    file_t base;
    HANDLE handle;
} win_file_t;

typedef struct fs_win_s {
    str_t       basePath; 
    str_t       dataPath;
    str_t       tempPath;
    str_t       tmpString;
    sys_mux_t   mux;
} fs_win_t;

static fs_win_t fs;
static bool fs_initialized = false;

/*==========================================================================================================*/
static size_t WinFile_Read( struct file_s * file , void* buffer, size_t size ) {
    win_file_t* winFile = (win_file_t*)file;
    DWORD bytesRead;
    if( ReadFile(winFile->handle, buffer, (DWORD)size, &bytesRead, NULL) ) {
        return (size_t)bytesRead;
    }
    return 0;

}

/*==========================================================================================================*/
static size_t WinFile_Write( struct file_s* file, const void* buffer, size_t size ) {
    win_file_t* winFile = (win_file_t*)file;
    DWORD bytesWritten;
    if( WriteFile(winFile->handle, buffer, (DWORD)size, &bytesWritten, NULL) ) {
        return (size_t)bytesWritten;
    }
    return 0;
}

/*==========================================================================================================*/
static int32_t WinFile_Seek( struct file_s* file, long offset, int origin ) {
    win_file_t* winFile = (win_file_t*)file;
    return SetFilePointer(winFile->handle, offset, NULL, origin) != INVALID_SET_FILE_POINTER ? 0 : -1;
}

/*==========================================================================================================*/
static uint64_t WinFile_Tell( struct file_s* file ) {
    win_file_t* winFile = (win_file_t*)file;
    return SetFilePointer(winFile->handle, 0, NULL, FILE_CURRENT);
}

/*==========================================================================================================*/
static int32_t WinFile_Flush( struct file_s* file ) {
    win_file_t* winFile = (win_file_t*)file;
    return FlushFileBuffers(winFile->handle) ? 0 : -1;
}

/*==========================================================================================================*/
static void WinFile_Close( struct file_s* file ) {
    win_file_t* winFile = (win_file_t*)file;
    if( winFile->handle != INVALID_HANDLE_VALUE ) {
        CloseHandle(winFile->handle);
        winFile->handle = INVALID_HANDLE_VALUE;
    }
    Mem_Free( winFile );
}

/*==========================================================================================================*/
static win_file_t* WinFile_Create( HANDLE handle ) {
    win_file_t* winFile = (win_file_t*)Mem_Alloc(sizeof(win_file_t));
    winFile->handle = handle;
    winFile->base.read = WinFile_Read;
    winFile->base.write = WinFile_Write;
    winFile->base.seek = WinFile_Seek;
    winFile->base.tell = WinFile_Tell;
    winFile->base.flush = WinFile_Flush;
    return winFile;
}

/*==========================================================================================================*/
static void WinFile_Destroy(file_t* file) {
    WinFile_Close(file);
}

/*==========================================================================================================*/
X_API void FS_Create(void) {
    if( fs_initialized ) {
        return;
    }
    Mem_Zero(&fs, sizeof(fs_win_t));

    Str_Reserve(&fs.basePath, MAX_PATH);
    Str_Reserve(&fs.dataPath, MAX_PATH);
    Str_Reserve(&fs.tempPath, 1024);
    Str_Reserve(&fs.tmpString, 1024);

    Sys_Mux_Init(&fs.mux);

    FS_GetCurrentDirectory(&fs.basePath);
    Str_PathJoin(&fs.dataPath, &fs.basePath, "base");

    fs_initialized = true;
}

/*==========================================================================================================*/
X_API void FS_Destroy(void) {
    if( !fs_initialized ) {
        return;
    }

    Sys_Mux_Destroy(&fs.mux);
    Str_Destroy(&fs.basePath);
    Str_Destroy(&fs.dataPath);
    Str_Destroy(&fs.tempPath);
    Str_Destroy(&fs.tmpString);

    fs_initialized = false;
}

/*==========================================================================================================*/
X_API void FS_GetCurrentDirectory( str_t * pathOut ) {
    DWORD length = GetCurrentDirectoryA(0, NULL);
    if( length > 0 && length < MAX_PATH ) {
        Str_Resize(pathOut, length);
        GetCurrentDirectoryA(length, *pathOut );
    }
}

/*==========================================================================================================*/
X_API void FS_MakePath( str_t * pathOut, const char * path ) {
    if ( ( !fs_initialized ) || ( path == NULL ) ) {
        return;
    }

    switch( path[ 0 ] ) {
        case '~':            
            Str_Joint( pathOut, fs.dataPath, path + 1);
            break;
        default:
            Str_Copy(pathOut, path);
            break;
    }  
}

/*==========================================================================================================*/
X_API file_t* FS_OpenFile(const char* path, const char* mode) {
    if( !fs_initialized ) {
        return NULL;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);

    HANDLE handle = CreateFileA(
        fs.tempPath,
        (strchr(mode, 'r') != NULL) ? GENERIC_READ : GENERIC_WRITE,
        0,
        NULL,
        (strchr(mode, 'r') != NULL) ? OPEN_EXISTING : CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    Sys_Mux_Unlock(&fs.mux);

    if( handle == INVALID_HANDLE_VALUE ) {
        return NULL;
    }

    return (file_t*)WinFile_Create(handle);
}

/*==========================================================================================================*/
X_API void FS_CloseFile(file_t* file) {
    if( file == NULL ) {
        return;
    }

    WinFile_Destroy(file);
}

/*==========================================================================================================*/
X_API bool FS_FileExists(const char* path) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);

    DWORD attributes = GetFileAttributesA(fs.tempPath);
    Sys_Mux_Unlock(&fs.mux);

    return (attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);

}

/*==========================================================================================================*/
X_API bool FS_DirectoryExists(const char* path) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);

    DWORD attributes = GetFileAttributesA(fs.tempPath);
    Sys_Mux_Unlock(&fs.mux);

    return (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

/*==========================================================================================================*/
X_API bool FS_CreateDirectory(const char* path) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);
    BOOL result = CreateDirectoryA(fs.tempPath, NULL);
    Sys_Mux_Unlock(&fs.mux);

    return result != 0;
}

/*==========================================================================================================*/
X_API bool FS_RemoveFile(const char* path) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);
    BOOL result = DeleteFileA(fs.tempPath);
    Sys_Mux_Unlock(&fs.mux);

    return result != 0;
}

/*==========================================================================================================*/
X_API bool FS_RemoveDirectory(const char* path) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);
    BOOL result = RemoveDirectoryA(fs.tempPath);
    Sys_Mux_Unlock(&fs.mux);

    return result != 0;
}

/*==========================================================================================================*/
X_API bool FS_EnumDirectoryFiles(const char* path, fs_enum_callback_t callback, void* userData, const char* filter ) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);
    Str_PathAppendCStr(&fs.tempPath, "*.*");

    bool foundMatch = false;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(fs.tmpString, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        Sys_Mux_Unlock(&fs.mux);
        return false;
    }
    do {
        if (callback) {
            if(FS_MatchFilter(findData.cFileName, filter)) {
                foundMatch = true;
                
                /* Report the full path of the found file */
                Str_PathJoinCStr( fs.tmpString, &fs.tempPath, findData.cFileName);
                callback( fs.tmpString, false, userData );
            }
        } 
    } while ( FindNextFileA(hFind, &findData));

    FindClose(hFind);

    Sys_Mux_Unlock(&fs.mux);

    return foundMatch;
}

/*==========================================================================================================*/
X_API bool FS_EnumDirectories(const char* path, fs_enum_callback_t callback, void* userData ) {
    if( !fs_initialized ) {
        return false;
    }

    Sys_Mux_Lock(&fs.mux);
    FS_MakePath(&fs.tempPath, path);
    Str_PathAppendCStr(&fs.tempPath, "*.*");

    bool foundMatch = false;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(fs.tmpString, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        Sys_Mux_Unlock(&fs.mux);
        return false;
    }
    do {
        if (callback) {
            if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {                
                /* Report the full path of the found directory */
                Str_PathJoinCStr( fs.tmpString, &fs.tempPath, findData. cFileName);
                callback( fs.tmpString, true, userData );
                foundMatch = true;
            }
        } 
    } while ( FindNextFileA(hFind, &findData));

    FindClose(hFind);

    Sys_Mux_Unlock(&fs.mux);

    return foundMatch;
}
