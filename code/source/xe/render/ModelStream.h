
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

#ifndef __MODELSTREAM_H__
#define __MODELSTREAM_H__

#include "core/Common.h"

// Version 5 - skin influences
// Version 6 - built-in materials
#define MODEL_STREAM_VERSION 7
#define F_MODEL_STREAM_SKINNED 1
    
typedef struct mesh_stream_s {
    uint32_t vertexStart;
    uint32_t vertexCount;
    uint32_t indexStart;
    uint32_t indexCount;
    uint32_t material;            /* Offet into the material name list for the material that this mesh uses */
    uint32_t pad;                 /* Pad to 8-bytes */
} mesh_stream_t;
    

typedef struct model_stream_s {
    uint32_t        version;
    uint32_t        flags;
    uint32_t        vertexCount;
    uint32_t        indexCount;
    uint32_t        meshCount;
    uint32_t        vertexDataSize;
    uint32_t        indexDataSize;
    uint32_t        materialNamesSize;
    uint32_t        offsMeshes;
    uint32_t        offsMeshHashArray;
    uint32_t        offsMeshHashMap;
    uint32_t        offsVertices;
    uint32_t        offsIndices;
    uint32_t        offsInfluences;
    uint32_t        offsMaterialNames;
    uint32_t        pad;
    float           bounds[ 2 ][ 3 ];
} model_stream_t;

const void * ModelStream_GetPointer( model_stream_t * self_, uint32_t offs );
const mesh_stream_t * ModelStream_GetMeshes( model_stream_t * self_ );
const void * ModelStream_GetIndices( model_stream_t * self_ );
const void * ModelStream_GetVertices( model_stream_t * self_ );
const void * ModelStream_GetInfluences( model_stream_t * self_ );
const uint64_t * ModelStream_GetMeshHashArray( model_stream_t * self_ );
const uint32_t * ModelStream_GetMeshHashMap( model_stream_t * self_ );
const char * ModelStream_GetMaterialNames( model_stream_t * self_ );
void ModelStream_GetBounds( model_stream_t * self_ , vec3_t * bmin, vec3_t * bmax );

#endif
