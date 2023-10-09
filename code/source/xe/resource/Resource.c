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

#include "resource/Resource.h"
#include "mem/Mem.h"
#include "core/Fs.h"
#include "core/Str.h"
#include "core/fh64.h"
#include "core/Bsearch.h"
#include "core/Sys.h"
#include "core/Array.h"
#include <string.h>
#include <assert.h>

#define MAX_RESOURCES 2048
#define MAX_FACTORIES 128
#define MAX_FACTORY_EXTENSIONS 128

typedef struct resource_data_s {
    uint64_t                pathHash;
    str_t                   path;
    void *                  data;
    resource_factory_t *    factory;
} resource_data_t;

typedef struct resource_mgr_s {
    uint64_t        resourceHashes[ MAX_RESOURCES ];
    resource_t *    resources[ MAX_RESOURCES ];
    size_t          resourceCount;
    
    uint64_t                factoryHashes[ MAX_FACTORY_EXTENSIONS ];
    uint32_t                factoryHashMap[MAX_FACTORY_EXTENSIONS ];
    resource_factory_t *    factories[ MAX_FACTORIES ];
    size_t                  factoryCount;
    size_t                  factoryExtCount;
    
    str_t                   pathTemp;               /* Used when wporking with paths */
    str_t                   extTemp;                /* Used when getting the extension */
    str_t                   tempStr;                /* A large temporary string used internally by the resource system */
} resource_mgr_t;

static resource_mgr_t res;
static bool_t resInit = false;

/*=======================================================================================================================================*/
void * Resource_GetData( resource_t * self_ ) {
    assert( self_ != NULL );
    return ((resource_data_t*) self_)->data;
}

/*=======================================================================================================================================*/
void Resource_MakePathCanonical( str_t path ) {
    Str_ToLower( path );
    Str_ReplaceChar( path, '\\', '/' );
}

/*=======================================================================================================================================*/
uint64_t Resource_CalcPathHashCStr( const char * path ) {
    uint64_t hash = 0;
    
    Str_CopyCStr( &res.tempStr, path );
    Str_ToLower( res.tempStr );
    Str_ReplaceChar( res.tempStr, '\\', '/' );
    
    hash = FH64_CalcStr( res.tempStr );
    
    return hash;
}

/*=======================================================================================================================================*/
uint64_t Resource_CalcExtensionHashCStr( const char * ext ) {
    
    if ( ext != res.extTemp ) {
        Str_CopyCStr( &res.extTemp, ext );
        Str_ToLower( res.extTemp );
    }

    return FH64_CalcStr( res.extTemp );
}

/*=======================================================================================================================================*/
str_t Resource_GetPathExtensionCStr( const char * path ) {
    Str_CopyCStr( &res.pathTemp, path );
    bool_t gotExt = Str_PathGetExtension( &res.extTemp, res.pathTemp );
    return ( gotExt == false ) ? NULL : res.extTemp;
}

/*=======================================================================================================================================*/
bool_t Resource_GetExtensionHashFromPathCStr( uint64_t * hashOut, const char * path ) {
    str_t extStr = Resource_GetPathExtensionCStr( path );
    if ( extStr == NULL ) {
        *hashOut = 0;
        return false;
    }
    
    *hashOut = FH64_CalcStr( extStr );
    return true;
}

/*=======================================================================================================================================*/
uint64_t Resource_CalcExtensionHashFromCStr( const char * ext ) {
    Str_CopyCStr( &res.extTemp, ext );
    Str_ToLower( res.extTemp );
    
    return FH64_CalcStr( res.extTemp );
}

/*=======================================================================================================================================*/
uint64_t Resource_CalcPathHash( str_t path ) {
    uint64_t hash = 0;
    
    Str_Copy( &res.tempStr, path );
    Str_ToLower( res.tempStr );
    Str_ReplaceChar( res.tempStr, '\\', '/' );
    
    hash = FH64_CalcStr( res.tempStr );
    
    return hash;
}

/*=======================================================================================================================================*/
resource_factory_t * Resource_FindFactory( const char * path ) {
    uint64_t hash = 0;
    bool_t gotExtHash = Resource_GetExtensionHashFromPathCStr( &hash, path );
    assert( gotExtHash == true );
    
    int32_t index = -1;
    bool_t found = Bsearch_FindUint64( &index,  hash, res.factoryHashes, res.factoryCount );
    if ( found == false ) {
        return NULL;
    }
    
    resource_factory_t * factory = res.factories[ res.factoryHashMap[ index ] ];
    return factory;
}

/*=======================================================================================================================================*/
resource_t * Resource_FindInternal( const char * path ) {
    uint64_t hash = Resource_CalcPathHashCStr( path );
    
    int32_t index = -1;
    bool_t found = Bsearch_FindUint64( &index, hash, res.resourceHashes, res.resourceCount );
    if ( found == false ) {
        return NULL;
    }
    
    resource_t * resource = res.resources[ index ];
    return resource;
}

/*=======================================================================================================================================*/
void Resource_Initialise( void ) {
    if ( resInit == true ) {
        return;
    }
    
    memset( &res, 0, sizeof(res));
    
    Str_SetCapacity( &res.tempStr, 2048 );
    Str_SetCapacity( &res.pathTemp, 2048 );
    Str_SetCapacity( &res.extTemp, 64 );
    
    xprintf("=== Resource Init ==============\n");
    
    resInit = true;
}

/*=======================================================================================================================================*/
void Resource_Finalise( void ) {
    if ( resInit == false ) {
        return;
    }
    
    resInit = false;
}

/*=======================================================================================================================================*/
void Resource_RegisterFactory( resource_factory_t * factory, const char * ext ) {
    
    xprintf("Registering resource factory %s for extension '%s'\n", factory->desc, ext );
    
    /* Calculate the hash for the extension */
    uint64_t hash = Resource_CalcExtensionHashFromCStr( ext );
    
    /* Get the insertion position of the hash in the sorted array */
    int32_t index = -1;
    bool_t found = Bsearch_FindUint64( &index,  hash, res.factoryHashes, res.factoryCount );
    assert( found == false );
    
    uint32_t factoryPos = (uint32_t) res.factoryCount;
    
    /* Add the factory to the resource system and also the hash /hash map index */
    Array_InsertAtPosUint64( index, hash, res.factoryHashes, res.factoryCount, MAX_FACTORIES );
    Array_InsertAtPosUint32( index, factoryPos, res.factoryHashMap, res.factoryCount, MAX_FACTORIES );
    res.factories[res.factoryCount] = factory;
    ++res.factoryCount;
}

/*=======================================================================================================================================*/
void Resource_Add( resource_data_t * resToAdd ) {
    int32_t index = -1;
    bool_t found = Bsearch_FindUint64( &index, resToAdd->pathHash, res.resourceHashes, res.resourceCount );
    assert( found == false );
    
    Array_InsertAtPosUint64( index, resToAdd->pathHash, res.resourceHashes, res.resourceCount, MAX_RESOURCES );
    Array_InsertAtPosVoidPtr( index, (void*) resToAdd, (void**) res.resources, res.resourceCount, MAX_RESOURCES );
    ++res.resourceCount;
}

/*=======================================================================================================================================*/
resource_t * Resource_LoadInternal( const char * path, bool_t loadNow ) {
    xprintf("Loading resource %s\n", path);
    
    /* If the resource already exists, we'll just return it */
    resource_t * resource = Resource_FindInternal( path );
    if ( resource != NULL ) {
        xprintf("    Already loaded\n" );
        return resource;
    }
    
    xerror( res.resourceCount >= MAX_RESOURCES, "Maximum number of resources reached\n" );
    
    /* Resouce does not exist so we want to load it */
    
    /* Get the appropriate factory and create the resource from it */
    resource_factory_t * factory = Resource_FindFactory( path );
    xerror( factory == NULL, "Could not determine resource factory for '%s'\n", path );
    
    xprintf("    Resource factory type  '%s'\n", factory->desc );
    
    static_assert( sizeof(resource_data_t) <= sizeof( resource_t ), "resource_t.data is too small for implementation" );
    
    resource_data_t * resData = Mem_Alloc( sizeof( resource_t ) );
    memset(resData, 0, sizeof( resource_data_t ) );
    
    Str_CopyCStr( &resData->path, path );
    resData->pathHash = Resource_CalcPathHash( resData->path );
    resData->factory = factory;
    resData->data = factory->alloc();
    
    /* Add the resource to the internal list */
    Resource_Add( resData );
    
    resource = (resource_t *) resData;
    
    if ( loadNow == true ) {
        xprintf("    Loading now\n");
        /* Load the resource now if required */
        file_t file;
        bool_t openOk = FS_FileOpen( &file, path, "rb" );
        xerror( openOk == false, "Could not open resource file '%s' for reading\n", path );
        
        factory->load( resource, &file, path );
        
        FS_FileClose( &file );
    }
    else {
        assert(0);
    }
    
    return resource;
}

/*=======================================================================================================================================*/
resource_t * Resource_Load( const char * path ) {
    return Resource_LoadInternal( path, true );
}

/*=======================================================================================================================================*/
resource_t * Find( const char * path ) {
    return Resource_FindInternal( path );
}
