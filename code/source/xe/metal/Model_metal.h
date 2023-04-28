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

#ifndef __MODEL_METAL_H__
#define __MODEL_METAL_H__

#import <Metal/Metal.h>
#include "render/Model.h"

typedef struct material_s material_t;

typedef struct model_metal_s {
    id<MTLBuffer>       vertices;
    id<MTLBuffer>       indices;
    id<MTLBuffer>       influences;
    
    size_t              vertexCount;
    size_t              vertexStride;
    size_t              vertexSize;
    size_t              indexCount;
    size_t              indexStride;
    size_t              indexSize;
    size_t              meshCount;
    
    mesh_t *            meshes;
    material_t **       materials;
    
    vec3_t              boundsMin;
    vec3_t              boundsMax;
    
} model_metal_t;

typedef struct vertex_s {
    float   posTexU[4];
    float   normTexV[4];
} vertex_t;



#endif
