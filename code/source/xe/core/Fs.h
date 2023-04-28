/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#ifndef __FS_H__
#define __FS_H__

#include "core/Common.h"
#include "Str.h"

typedef struct file_s {
    uint64_t        data[8];
} file_t;

XE_API void        FS_Initialise       ( void );
XE_API void        FS_Finalise         ( void );

XE_API bool_t   FS_FileOpen         ( file_t * self_, const char* path, const char* mode );
XE_API void        FS_FileClose        ( file_t * file );
XE_API size_t      FS_FileLength       ( file_t * file );
XE_API uintptr_t   FS_FileTell         ( file_t * file );
XE_API bool_t   FS_FileSeek         ( file_t * file, uintptr_t pos );
XE_API size_t      FS_FileRead         ( file_t * file, void* buffer, size_t elementSize, size_t elementCount );
XE_API size_t      FS_FileWrite        ( file_t * file, const void* buffer, size_t elementSize, size_t elementCount );

XE_API const char* FS_GetExt           ( const char* pathIn );
XE_API char        FS_FolderSep        ( void );
XE_API char        FS_FolderSepOther   ( void );
XE_API void        FS_GetCurrentFolder ( str_t * pathOut );

#endif
