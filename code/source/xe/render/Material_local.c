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

#include "render/Material.h"
#include "render/Material_local.h"
#include "util/ParseLiteral.h"
#include "render/MaterialStream.h"
#include "core/Id.h"
#include "core/Sys.h"
#include "core/Fs.h"
#include "core/fh64.h"
#include "core/Bsearch.h"
#include "core/Array.h"
#include "mem/Mem.h"
#include <assert.h>
#include <string.h>
#include <stdarg.h>


#define MATERIAL_ID MAKE_ID( X, E, M, a, t, e, r, i, a, l, _, _ )

#define MATERIAL_CAPACITY 1024

typedef struct material_lib_s {
    material_t          materials[ MATERIAL_CAPACITY ];
    uint64_t            materialsFree[ MATERIAL_CAPACITY ];
    uint64_t            materialHashArray[ MATERIAL_CAPACITY ];
    uint64_t            materialHashMap[ MATERIAL_CAPACITY ];
    size_t              materialFreeCount;
    size_t              materialAllocCount;
    sys_mutex_t         mutex;
} material_lib_t;

bool_t materialLibInit = false;
material_lib_t materialLib;


/*=======================================================================================================================================*/
material_t * Material_FindInternal( const char * name ) {
    uint64_t nameHash = FH64_CalcFromCStr( name );
    int32_t index = -1;
    
    bool_t found = Bsearch_FindUint64( &index, nameHash, materialLib.materialHashArray, materialLib.materialAllocCount );
    if ( found == false ) {
        return NULL;
    }
    
    return &materialLib.materials[ materialLib.materialHashMap[ index ] ];
}


/*=======================================================================================================================================*/
material_t * Material_Find( const char * name ) {
    material_t * mat = NULL;
    
    Sys_MutexLock( &materialLib.mutex );
    mat = Material_FindInternal( name );
    Sys_MutexUnlock( &materialLib.mutex );
    
    return mat;
}

/*=======================================================================================================================================*/
void Material_Initialise() {
    if ( materialLibInit == true ) {
        return;
    }
    
    memset( &materialLib, 0, sizeof( materialLib ) );
    
    uint64_t * freeListCurr = &materialLib.materialsFree[ MATERIAL_CAPACITY - 1];
    for ( uint32_t m = 0; m < MATERIAL_CAPACITY; ++m ) {
        
        static_assert( sizeof( material_t ) >= sizeof( material_local_t), "Size of material_r.data is too small for implementation" );
        ((material_local_t*) &materialLib.materials[ m ])->index = m;
        *freeListCurr = m;
        --freeListCurr;
    }
    
    materialLib.materialFreeCount = MATERIAL_CAPACITY;
    materialLibInit = true;
}

/*=======================================================================================================================================*/
material_t * Material_Alloc( const char * name ) {
    int32_t insertIndex = -1;
    uint64_t matNameHash = 0;
    uint64_t matIndex = 0;
    material_local_t * matLocal = NULL;
    
    Sys_MutexLock( &materialLib.mutex );
    
    material_t * mat = Material_FindInternal( name );
    if ( mat != NULL ) {
        Sys_MutexUnlock( &materialLib.mutex );
        return mat;
    }
    
    matNameHash = FH64_CalcFromCStr( name );
    
    insertIndex = -1;
    Bsearch_FindUint64( &insertIndex, matNameHash, materialLib.materialHashArray, materialLib.materialAllocCount );
    
    matIndex = materialLib.materialsFree[ materialLib.materialFreeCount ];
    --materialLib.materialFreeCount;
    
    Array_InsertAtPosUint64( insertIndex, matNameHash, materialLib.materialHashArray, materialLib.materialAllocCount, MATERIAL_CAPACITY );
    Array_InsertAtPosUint64( insertIndex, matIndex, materialLib.materialHashMap, materialLib.materialAllocCount, MATERIAL_CAPACITY );
    ++materialLib.materialAllocCount;

    Sys_MutexUnlock( &materialLib.mutex );
    
    mat = &materialLib.materials[ matIndex ];
    matLocal = ( material_local_t * ) mat;
    matLocal->name = matNameHash;
    matLocal->magic = MATERIAL_ID;
    
    return mat;
}


/*=======================================================================================================================================*/
void Material_Free( material_t * mat ) {
    
    Sys_MutexLock( &materialLib.mutex );
    
    material_local_t * matLocal = ( material_local_t * ) mat;
    
    int32_t index = -1;
    bool_t found = Bsearch_FindUint64( &index, matLocal->name, materialLib.materialHashArray, materialLib.materialAllocCount );
    xassert( found == true );
    
    Array_RemoveAtPosUint64( index, materialLib.materialHashArray, materialLib.materialAllocCount );
    Array_RemoveAtPosUint64( index, materialLib.materialHashMap, materialLib.materialAllocCount );
    --materialLib.materialAllocCount;
    
    materialLib.materialsFree[ materialLib.materialFreeCount ] = matLocal->index;
    ++ materialLib.materialFreeCount;    
    
    Sys_MutexUnlock( &materialLib.mutex );
}


/*=======================================================================================================================================*/
void Material_LoadLibrary( const char * path ) {
    file_t file;
    bool_t opened = FS_FileOpen( &file, path, "rb" );
    xerror( opened == false, "Unable to open material library %s\n", path );
    
    uint8_t * buffer = Mem_Alloc( FS_FileLength( &file ) );
    FS_FileRead( &file, buffer, 1, FS_FileLength( &file ) );
    FS_FileClose( &file );
    
    material_stream_t * matLibStr = ( material_stream_t * ) buffer;
    const char * matLibStrings = MaterialStream_GetStrings( matLibStr );
    const material_stream_entry_t * matEntry = MaterialStream_GetMaterials( matLibStr );
    
    for ( uint32_t m = 0; m < matLibStr->count; ++m ) {
        const char * name = &matLibStrings[ matEntry->offsName ];
        material_t * mat = Material_Find( name );
        
        if ( mat == NULL ) {
            resource_t * texRes = NULL;
            mat = Material_Alloc( name );
            
            const char * albedoPath = ( matEntry->offsAlbedoTexture == 0 ) ? NULL : matLibStrings + matEntry->offsAlbedoTexture;
            const char * glowPath = ( matEntry->offsGlowTexture == 0 ) ? NULL : matLibStrings + matEntry->offsGlowTexture;
            const char * amrPath = ( matEntry->offsAmrTexture == 0 ) ? NULL : matLibStrings + matEntry->offsAmrTexture;
                        
            if ( albedoPath != NULL ) {
                texRes = Resource_Load( albedoPath );
                Material_SetTextureAlbedo( mat, (texture_t*) Resource_GetData( texRes ) );
            }
            
            if ( amrPath != NULL ) {
                texRes = Resource_Load( amrPath );
                Material_SetTextureAmr( mat, (texture_t*) Resource_GetData( texRes ) );
            }
            
            if ( amrPath != NULL ) {
                texRes = Resource_Load( amrPath );
                Material_SetTextureAmr( mat, (texture_t*) Resource_GetData( texRes ) );
            }
        }
        
        /* Next material entry */
        ++matEntry;
    }
    
    Mem_Free( buffer );
}



/*=======================================================================================================================================*/
void Material_Create( material_t * self_ ) {
}

/*=======================================================================================================================================*/
void Material_Destroy( material_t * self_ ) {
}

/*=======================================================================================================================================*/
void Material_SetTextureAlbedo( material_t * self_, texture_t * tex ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    matLocal->textureAlbedo = tex;
}

/*=======================================================================================================================================*/
void Material_SetTextureGlow( material_t * self_, texture_t * tex ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    matLocal->textureGlow = tex;
}

/*=======================================================================================================================================*/
void Material_SetTextureAmr( material_t * self_, texture_t * tex ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    matLocal->textureAmr = tex;
}
