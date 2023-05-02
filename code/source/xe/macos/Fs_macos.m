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

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Foundation/Foundation.h>

#include "Fs.h"
#include "Str.h"
#include "core/Sys.h"

#define FS_MAX_PATH_LENGTH 512
#define FS_MAX_FILES 64

typedef struct file_data_s {
    uint64_t    index;
    FILE*       file;
    bool_t       free;
} file_data_t;

typedef struct fs_s {
    
    // char        dataFolderPath[FS_MAX_PATH_LENGTH];
    //char        filePathTmp[FS_MAX_PATH_LENGTH];

    str_t       basePath;
    str_t       filePathTmp;
    str_t       dataFolderPath;
    str_t       assetFolderPath;
    
} fs_t;

fs_t fsLocal;
fs_t* fileSystem = NULL;

static bool_t FS_FindFreeFile(int32_t* indexOut);

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void FS_Initialise(void) {
    if ( fileSystem != NULL ) {
        return;
    }
    
    memset(&fsLocal, 0, sizeof(fsLocal) );
    fileSystem = &fsLocal;
    
    Str_SetCapacity( &fsLocal.basePath, 2048 );
    Str_SetCapacity( &fsLocal.filePathTmp, 2048 );
    Str_SetCapacity( &fsLocal.dataFolderPath, 2048 );
    Str_SetCapacity( &fsLocal.assetFolderPath, 2048 );
    
    /* Setup the data folder based on the current folder */
    /* TODO - check for config or water mark in app Data folder */
    FS_GetCurrentFolder( &fileSystem->basePath );
    
    Str_Copy( &fsLocal.dataFolderPath, fileSystem->basePath );
    Str_AppendPathCStr( &fsLocal.dataFolderPath, "/data" );
    
    Str_Copy( &fsLocal.assetFolderPath, fileSystem->basePath );
    Str_AppendPathCStr( &fsLocal.assetFolderPath, "../../assets" );
    
    xprintf("=== FS Init ====================\n");
    xprintf("     Working path: %s\n", fileSystem->basePath );
    xprintf("        Data path: %s\n", fileSystem->dataFolderPath );
    xprintf("       Asset path: %s\n", fileSystem->assetFolderPath );
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void FS_Finalise(void) {
    assert(fileSystem != NULL);
    fileSystem = NULL;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
bool_t FS_FileOpen( file_t * self_, const char* path, const char* mode) {
    bool_t makePathOk = FALSE;
    
    FILE*  file = NULL;
    
    assert(fileSystem != NULL);
    assert(path != NULL);
    assert(mode != NULL);

    /* Make the full path to the file */
    makePathOk = FS_MakePath( &fileSystem->filePathTmp, path );
    if (makePathOk == FALSE) {
        return false;
    }
    
    /* Attempt tp open the file */
    file = fopen( fileSystem->filePathTmp, mode );
    if ( file == NULL ) {
        return false;
    }
    
    /* File was opened, so use the xfile */
    static_assert( sizeof( file_t ) >= sizeof(file_data_t), "Size of file_t.data is too small for implementation " );
    
    file_data_t * fileData = (file_data_t * ) self_->data;

    fileData->file = file;
    fileData->free = FALSE;
    
    return true;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void FS_FileClose( file_t * file) {
    assert(file != NULL);
    
    file_data_t * fileData = (file_data_t * ) file->data;
    fclose( fileData->file );
    fileData->file = NULL;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
size_t FS_FileLength(file_t* file) {
    struct stat buf;
    assert(file != NULL);
    
    fstat(fileno( ((file_data_t*)file)->file), &buf);
    off_t size = buf.st_size;
    
    return size;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
uintptr_t FS_FileTell(file_t* file) {
    file_data_t * fileData = (file_data_t * ) file->data;
    
    assert(file != NULL);
    
    return ftello(fileData->file);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
bool_t FS_FileSeek(file_t* file, uintptr_t pos) {
    file_data_t * fileData = (file_data_t * ) file->data;
    int res;
    
    assert(file != NULL);
    res = fseek( fileData->file, pos, SEEK_SET );
    
    return (res == 0);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
size_t FS_FileRead(file_t* file, void* buffer, size_t elementSize, size_t elementCount) {
    file_data_t * fileData = (file_data_t * ) file->data;
    assert(file != NULL);
    
    return fread( buffer, elementSize, elementCount, fileData->file );
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
size_t FS_FileWrite(file_t* file, const void* buffer, size_t elementSize, size_t elementCount) {
    file_data_t * fileData = (file_data_t * ) file->data;
    assert(file != NULL);
    
    return fwrite( buffer, elementSize, elementCount, fileData->file );
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
bool_t FS_MakePath( str_t * pathOut, const char * path ) {
    if (path[0] == '~') {
        /* Path has a ~ prefix - so make an absolute path based on the data folder */
        
        Str_Copy( pathOut, fileSystem->dataFolderPath );
        Str_AppendPathCStr( pathOut, path+1 );
    } else if (path[0] == '@') {
        /* Path has a ~ prefix - so make an absolute path based on the data folder */
        
        Str_Copy( pathOut, fileSystem->assetFolderPath );
        Str_AppendPathCStr( pathOut, path+1 );
    } else {
        /* Path has no prefix - just treat it as a normal path and copy */
        Str_CopyCStr( pathOut, path );
    }
    
    return TRUE;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
void FS_GetCurrentFolder( str_t * pathOut ) {
    NSFileManager* fileMgr = [NSFileManager defaultManager];
    NSString* currDirPath = [fileMgr currentDirectoryPath];
    
    if (currDirPath != nil) {
        const char * pathStrUtf8 = [currDirPath UTF8String];
        Str_SetLength( pathOut, currDirPath.length );
        Str_CopyCStr( pathOut, pathStrUtf8 );
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
bool_t FS_CreateFolder( const char * path ) {
    FS_MakePath( &fileSystem->filePathTmp, path );
    
    NSFileManager* fileMgr = [ NSFileManager defaultManager ];
    NSString *pathNs = [ NSString stringWithUTF8String:fileSystem->filePathTmp ];
    
    BOOL res = [ fileMgr createDirectoryAtPath: pathNs
                   withIntermediateDirectories: true
                                    attributes: nil
                                         error: nil ];
    
    return ( res == YES ) ? bool_true : bool_false ;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
char FS_FolderSep() {
    return '/';
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
char FS_FolderSepOther() {
    return '/';
}
