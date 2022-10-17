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
#include "Fs.h"
#include "mem/Mem.h"
#include "resource/Resource.h"
#include <assert.h>

static void         ModelResource_Load( resource_t * self_, file_t * file, const char * path );
static void *       ModelResource_Alloc( void );
static void         ModelResource_Free( void * data );

DEFINE_RESOURCE_FACTORY("Model", Model, model )

/*=======================================================================================================================================*/
void Model_Load( model_t * self_, file_t * file ) {
    model_stream_t * str = NULL;
    const void * vertexStr = NULL;
    const void * indexStr = NULL;
    uint16_t * tempIndices = NULL;
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
    
    /* Can only have a single mesh */
    assert( str->meshCount == 1 );
    
    vertexStr = ModelStream_GetVertices( str );
    indexStr = ModelStream_GetIndices( str );
    
    Model_Create( self_, str->vertexCount, str->indexCount, 0 );
    
    /* Write the vertex data */
    Model_WriteVertexData( self_, vertexStr, 0, str->vertexCount );
    
    /* Write the index Data */
    /* Note: Have to convert to uint16 for now */
    assert( str->vertexCount < 65535 );
    tempIndices = (uint16_t *) Mem_Alloc( sizeof(uint16_t) * str->indexCount );
    
    for ( int n = 0; n < str->indexCount; ++n ) {
        tempIndices[n] = (uint16_t) ((const uint32_t *) indexStr)[n];
    }
    
    Model_WriteIndexData( self_, tempIndices, 0, str->indexCount );
    Mem_Free( tempIndices );
    
    /* Set model bounds */
    ModelStream_GetBounds( str, &bmin, &bmax );
    Model_SetBounds( self_, &bmin, &bmax );
    
    /* All done. Clean up! */
    Mem_Free( data );
}

/*=======================================================================================================================================*/
void ModelResource_Load( resource_t * self_, file_t * file, const char * path ) {
    model_t * model = (model_t*) Resource_GetData( self_ );
    Model_Load( model, file );
}

/*=======================================================================================================================================*/
void * ModelResource_Alloc(void) {
    return Mem_Alloc(sizeof(model_t));
}

/*=======================================================================================================================================*/
void  ModelResource_Free( void * data ) {
    Mem_Free( data );
}
