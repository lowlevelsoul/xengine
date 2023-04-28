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
#include "metal/Model_metal.h"
#include "metal/Render_metal.h"
#include "mem/Mem.h"
#include <new>

/*=======================================================================================================================================*/
void Model_Create( model_t * self_, size_t vertexCount, size_t indexCount, size_t meshCount, uint64_t flags) {
    static_assert( sizeof(model_t) >= sizeof(model_metal_t), "Size of model_t is too small for implementation" );
    
    model_metal_t * modelMtl = new( (void*)self_ ) model_metal_t;
    
    modelMtl->vertexCount = vertexCount;
    modelMtl->vertexStride = sizeof(vertex_t);
    modelMtl->vertexSize = modelMtl->vertexCount * modelMtl->vertexStride;
    
    modelMtl->indexCount = indexCount;
    modelMtl->indexStride = sizeof(uint32_t);
    modelMtl->indexSize = modelMtl->indexCount * modelMtl->indexStride;
    
    modelMtl->meshCount = meshCount;
    modelMtl->meshes = (mesh_t *) Mem_Alloc( sizeof(mesh_t) * meshCount );
    modelMtl->materials = (material_t**) Mem_Alloc( sizeof(void*) * meshCount );
    memset( modelMtl->materials, 0, sizeof(void*) * meshCount );
    
    id<MTLDevice> mtlDevice = Render_GetDevice();
    modelMtl->vertices = [ mtlDevice newBufferWithLength:modelMtl->vertexSize options:0 ];
    modelMtl->indices = [ mtlDevice newBufferWithLength:modelMtl->indexSize options:0 ];
    
    Vec3_Set( modelMtl->boundsMin, 0, 0, 0);
    Vec3_Set( modelMtl->boundsMax, 0, 0, 0);
}

/*=======================================================================================================================================*/
void Model_Destroy( model_t * self_ ) {
    model_metal_t * modelMtl = (model_metal_t *) self_;
    assert( self_ != nullptr );
    
    modelMtl->vertices = nil;
    modelMtl->indices = nil;
    
    Mem_Free( modelMtl->meshes );
    Mem_Free( modelMtl->materials );
    
    modelMtl->~model_metal_t();
}

/*=======================================================================================================================================*/
void Model_WriteMeshData( model_t * self_, const void * src, uintptr_t start, size_t count ) {
    model_metal_t * modelMtl = (model_metal_t *) self_;
    
    assert( self_ != nullptr );
    assert( start + count <= modelMtl->meshCount );
    
    size_t countBytes = count * sizeof( mesh_t );
    memcpy( &modelMtl->meshes[ start ], src, countBytes );
}

/*=======================================================================================================================================*/
void Model_WriteVertexData( model_t * self_, const void * src, uintptr_t start, size_t count ) {
    assert( self_ != nullptr );
    model_metal_t * modelMtl = (model_metal_t *) self_;
    
    assert( start + count <= modelMtl->vertexCount );
    uintptr_t startBytes = start * modelMtl->vertexStride;
    size_t countBytes = count * modelMtl->vertexStride;
    memcpy( ((uint8_t*) modelMtl->vertices.contents) + startBytes, src, countBytes );
}

/*=======================================================================================================================================*/
void Model_WriteIndexData( model_t * self_, const void * src, uintptr_t start, size_t count ) {
    assert( self_ != nullptr );
    model_metal_t * modelMtl = (model_metal_t *) self_;
    
    assert( start + count <= modelMtl->indexCount );
    uintptr_t startBytes = start * modelMtl->indexStride;
    size_t countBytes = count * modelMtl->indexStride;
    memcpy( ((uint8_t*) modelMtl->indices.contents) + startBytes, src, countBytes );
}

/*=======================================================================================================================================*/
void Model_SetBounds( model_t * self_, const vec3_t * boundsMin, const vec3_t * boundsMax ) {
    assert( self_ != nullptr );
    model_metal_t * modelMtl = (model_metal_t *) self_;
    Vec3_Copy( modelMtl->boundsMin, *boundsMin );
    Vec3_Copy( modelMtl->boundsMax, *boundsMax );
}

/*=======================================================================================================================================*/
void Model_GetBounds( model_t * self_, vec3_t * boundsMin, vec3_t * boundsMax ) {
    assert( self_ != nullptr );
    model_metal_t * modelMtl = (model_metal_t *) self_;
    if ( boundsMin != nullptr ) {
        Vec3_Copy( *boundsMin,  modelMtl->boundsMin );
    }
    
    if ( boundsMax != nullptr ) {
        Vec3_Copy( *boundsMax,  modelMtl->boundsMax );
    }
}


/*=======================================================================================================================================*/
void Model_SetMaterial( model_t * self_, uint32_t index, material_t * mat ) {
    assert( self_ != nullptr );
    model_metal_t * modelMtl = (model_metal_t *) self_;
    
    assert( index < modelMtl->meshCount );
    modelMtl->materials[ index ] = mat;
}

/*=======================================================================================================================================*/
material_t ** Model_GetMaterials( model_t * self_ ) {
    model_metal_t * modelMtl = (model_metal_t *) self_;
    return modelMtl->materials;
}

/*=======================================================================================================================================*/
size_t Model_GetMeshCount( model_t * self_ ) {
    model_metal_t * modelMtl = (model_metal_t *) self_;
    return modelMtl->meshCount;
}

/*=======================================================================================================================================*/
const mesh_t *  Model_GetMeshes( model_t * self_ ) {
    model_metal_t * modelMtl = (model_metal_t *) self_;
    return modelMtl->meshes;
}
