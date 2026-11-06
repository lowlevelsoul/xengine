/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_FS_H
#define XE_FS_H

#include "Platform.h"

typedef struct str_s str_t;
typedef bool (*fs_enum_callback_t)(const char* filename, bool isFile, void* userData );

typedef struct file_s {
    size_t (*read)( struct file_s*, void* buffer, size_t size );
    size_t (*write)( struct file_s*, const void* buffer, size_t size );
    int32_t (*seek)( struct file_s*, long offset, int origin );
    uint64_t (*tell)( struct file_s* );
    int32_t (*flush)( struct file_s* );
} file_t;

X_API void FS_Create(void);
X_API void FS_Destroy(void);
X_API file_t* FS_OpenFile(const char* path, const char* mode);
X_API void FS_MakePath( str_t * pathOut, const char * path );
X_API void FS_GetCurrentDirectory( str_t * pathOut );
X_API bool FS_FileExists(const char* path);
X_API bool FS_DirectoryExists(const char* path);
X_API bool FS_CreateDirectory(const char* path);
X_API bool FS_RemoveFile(const char* path);
X_API bool FS_RemoveDirectory(const char* path);
X_API bool FS_EnumDirectoryFiles(const char* path, fs_enum_callback_t callback, void* userData, const char* filter );
X_API bool FS_EnumDirectories(const char* path, fs_enum_callback_t callback, void* userData );
X_API bool FS_MatchFilter(const char* fileName, const char* filter);


#endif // XE_FS_H