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

#include "render/Model.h"
#include "render/ModelStream.h"
#include "render/Material.h"
#include "core/Fs.h"
#include "mem/Mem.h"
#include "resource/Resource.h"
#include <assert.h>
#include <string.h>

static void         ModelResource_Load( resource_t * self_, file_t * file, const char * path );
static void *       ModelResource_Alloc( void );
static void         ModelResource_Free( void * data );

static void         Model_LoadMaterials( model_t * self_, model_stream_t * str, const char * path );

DEFINE_RESOURCE_FACTORY("Model", Model, model )

/*=======================================================================================================================================*/
void Model_Load( model_t * self_, file_t * file, const char * path ) {
    model_stream_t * str = NULL;
    const void * vertexStr = NULL;
    const void * indexStr = NULL;
    vec3_t bmin, bmax;
    size_t amtRead;
    
    /* Read the data fom the file */
    size_t dataLength = FS_FileLength( file );
    void * data = Mem_Alloc( dataLength );
    assert( data != NULL );
    
    amtRead = FS_FileRead( file, data, sizeof(uint8_t) , dataLength );
    assert( amtRead == dataLength );
    
    /* We only load the most recent model versions */
    str = (model_stream_t *) data;
    assert( str->version == MODEL_STREAM_VERSION );
    
    vertexStr = ModelStream_GetVertices( str );
    indexStr = ModelStream_GetIndices( str );
    
    Model_Create( self_, str->vertexCount, str->indexCount, str->meshCount, 0 );
    
    /* Write the vertex data */
    Model_WriteVertexData( self_, vertexStr, 0, str->vertexCount );
    
    /* Write the index Data */    
    Model_WriteIndexData( self_, indexStr, 0, str->indexCount );
    
    /* Write the mesh data */
    Model_WriteMeshData( self_, ModelStream_GetMeshes( str ), 0, str->meshCount );
    
    /* Set model bounds */
    ModelStream_GetBounds( str, &bmin, &bmax );
    Model_SetBounds( self_, &bmin, &bmax );
    
    /* Load materials */
    if ( path != NULL ) {
        Model_LoadMaterials( self_, str, path );
    }
    
    /* All done. Clean up! */
    Mem_Free( data );
}

/*=======================================================================================================================================*/
void Model_LoadMaterials( model_t * self_, model_stream_t * str, const char * path ) {
    
    const mesh_stream_t * meshStr = ModelStream_GetMeshes( str );
    resource_t * res = NULL;
    
    uint32_t modelsStrIndex = 0;
    const char * modelsStr = strstr( path, "models" );
    const char * matPathBase = NULL;
    if ( modelsStr == NULL) {
        return;
    }
     
    modelsStrIndex = (uint32_t) ( modelsStr - path );
    if ( modelsStrIndex > 3 ) {
        return;
    }
    
    matPathBase = path + modelsStrIndex + 6;
    
    str_t matLibPath = NULL;
    Str_CopyCStr( &matLibPath, "~/materials" );
    Str_AppendPathCStr( &matLibPath, matPathBase );
    Str_PathRemoveExtension( &matLibPath );
    Str_AppendCStr( &matLibPath, ".bmat" );
    
    Material_LoadLibrary( matLibPath );
    
    Str_Destroy( &matLibPath );
    
    for ( uint32_t m = 0; m < str->meshCount; ++m ) {
        const char * meshMatName = ModelStream_GetMaterialNames( str ) + meshStr->material;
        material_t * mat = Material_Find( meshMatName );
        Model_SetMaterial( self_, m, mat );
    }
}

/*=======================================================================================================================================*/
void ModelResource_Load( resource_t * self_, file_t * file, const char * path ) {
    model_t * model = (model_t*) Resource_GetData( self_ );
    Model_Load( model, file, path );
}

/*=======================================================================================================================================*/
void * ModelResource_Alloc(void) {
    return Mem_Alloc(sizeof(model_t));
}

/*=======================================================================================================================================*/
void  ModelResource_Free( void * data ) {
    Mem_Free( data );
}
