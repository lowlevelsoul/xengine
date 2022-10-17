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

#include "math/Math3d.h"
#include "render/ModelStream.h"

/*=======================================================================================================================================*/
const void * ModelStream_GetPointer( model_stream_t * self_, uint32_t offs ) {
    return ( const void * ) ( ( ( uintptr_t ) self_ ) + offs );
}

/*=======================================================================================================================================*/
const mesh_stream_t * ModelStream_GetMeshes( model_stream_t * self_ ) {
    return ModelStream_GetPointer( self_, self_->offsMeshes );
}

/*=======================================================================================================================================*/
const void * ModelStream_GetIndices( model_stream_t * self_ ) {
    return ModelStream_GetPointer( self_, self_->offsIndices );
}

/*=======================================================================================================================================*/
const void * ModelStream_GetVertices( model_stream_t * self_ ) {
    return ModelStream_GetPointer( self_, self_->offsVertices );
}

/*=======================================================================================================================================*/
const void * ModelStream_GetInfluences( model_stream_t * self_ ) {
    return ModelStream_GetPointer( self_, self_->offsInfluences );
}

/*=======================================================================================================================================*/
const uint64_t * ModelStream_GetMeshHashArray( model_stream_t * self_ ) {
    return (const uint64_t *) ModelStream_GetPointer( self_, self_->offsMeshHashArray );
}

/*=======================================================================================================================================*/
const uint32_t * ModelStream_GetMeshHashMap( model_stream_t * self_ ) {
    return (const uint32_t *) ModelStream_GetPointer( self_, self_->offsMeshHashMap );
}

/*=======================================================================================================================================*/
const char * ModelStream_GetMaterialNames( model_stream_t * self_ ) {
    return (const char *) ModelStream_GetPointer( self_, self_->offsMaterialNames );
}

/*=======================================================================================================================================*/
void ModelStream_GetBounds( model_stream_t * self_ , vec3_t * bmin, vec3_t * bmax ) {
    if ( bmin != NULL ) {
        Vec3_SetXyz( *bmin, self_->bounds[0][0], self_->bounds[0][1], self_->bounds[0][2] );
    }
    
    if ( bmax != NULL ) {
        Vec3_SetXyz( *bmax, self_->bounds[1][0], self_->bounds[1][1], self_->bounds[1][2] );
    }
}
