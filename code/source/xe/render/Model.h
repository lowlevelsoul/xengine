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

#ifndef __MODEL_H__
#define __MODEL_H__

#include "core/Common.h"
#include "math/Math3d.h"

typedef struct file_s file_t;
typedef struct material_s material_t;

typedef struct mesh_s {
    uint32_t        vertexStart;
    uint32_t        vertexCount;
    uint32_t        indexStart;
    uint32_t        indexCount;
    uint32_t        material;
    uint32_t        pad;
} mesh_t;

typedef struct model_s {
    uint64_t        data[64];
} model_t;

#define MODEL_TANGENTS          0x0000000000000001
#define MODEL_MESHNAMELOOKUP    0x0000000000000002

XE_API void        Model_Create( model_t * self_, size_t vertexCount, size_t indexCount, size_t meshCount, uint64_t flags );
XE_API void        Model_WriteVertexData( model_t * self_, const void * src, uintptr_t start, size_t count );
XE_API void        Model_WriteIndexData( model_t * self_, const void * src, uintptr_t start, size_t count );
XE_API void        Model_WriteMeshData( model_t * self_, const void * sec, uintptr_t start, size_t count );
XE_API void        Model_SetBounds( model_t * self_, const vec3_t * boundsMin, const vec3_t * boundsMax );
XE_API void        Model_GetBounds( model_t * self_, vec3_t * boundsMin, vec3_t * boundsMax );
XE_API void        Model_Load( model_t * self_, file_t * file,  const char * path );
XE_API void        Model_SetMaterial( model_t * self_, uint32_t index, material_t * mat );
XE_API material_t ** Model_GetMaterials( model_t * self_ );
XE_API size_t       Model_GetMeshCount( model_t * self_ );
XE_API const mesh_t * Model_GetMeshes( model_t * self_ );

typedef struct resource_factory_s resource_factory_t;
extern resource_factory_t * model_resource_factory;

#endif
