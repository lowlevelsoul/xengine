/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/Render.h"
#include "xe/Mem.h"
#include "xe/MathLinear.h"

/* Cube vertices are defined in clockwise order for the front and back face*/
static const vec3_t CUBE_VERTICES[8] = {
    { -0.5f,  0.5f, -0.5f },
    {  0.5f,  0.5f, -0.5f },
    {  0.5f, -0.5f, -0.5f },
    { -0.5f, -0.5f, -0.5f },
    { -0.5f,  0.5f, -0.5f },
    {  0.5f,  0.5f, -0.5f },
    {  0.5f, -0.5f, -0.5f },
    { -0.5f, -0.5f, -0.5f },
};

/* Cube indices are defined in each triangle in counter-clockwise order */
static const uint32_t CUBE_INDICES[36] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    0, 4, 7, 7, 3, 0,
    1, 5, 6, 6, 2, 1,
    0, 1, 5, 5, 4, 0,
    3, 2, 6, 6, 7, 3 
};


/*==========================================================================================================*/
void RenderParams_Initialise( render_params_t* params ) {
    if ( params ) {
        params->width = 800;
        params->height = 600;
        params->fullscreen = false;
        params->debug = false;
        params->vsync = true;
        params->maxNumFramesInFlight = 2;
        params->renderMemHeapSize = 64 * 1024 * 1024; // 64 MB
    }
}

/*==========================================================================================================*/
X_API void RenderModel_Create( render_model_t * self_, uint32_t vertexCount, uint32_t indexCount, uint32_t surfaceCount, uint32_t weightCount ) {
    Mem_Zero( self_, sizeof( render_model_t ) );

    self_->geometry.vertexCount = vertexCount;
    self_->geometry.indexCount = indexCount;
    self_->geometry.weightCount = weightCount;

    self_->geometry.vertexStride = ( weightCount == 0 ) ? sizeof( render_static_vertex_t ) : sizeof( render_skin_vertex_t );
    self_->geometry.weightStride = ( weightCount == 0 ) ? 0 : sizeof( render_skin_weight_t );
    self_->geometry.indexStride = sizeof( uint32_t );

    self_->geometry.vertexDataSize = self_->geometry.vertexCount * self_->geometry.vertexStride;
    self_->geometry.indexDataSize = self_->geometry.indexCount * self_->geometry.indexStride;
    self_->geometry.weightDataSize = self_->geometry.weightCount * self_->geometry.weightStride;

    self_->geometry.vertexBuffer = Render_GpuBufferCreate( self_->geometry.vertexDataSize, RENDER_GPU_BUFFER_FLAGS_USAGE_GPU_ONLY );
    self_->geometry.indexBuffer = Render_GpuBufferCreate( self_->geometry.indexDataSize, RENDER_GPU_BUFFER_FLAGS_USAGE_GPU_ONLY );

    if ( self_->geometry.weightDataSize > 0 ) {
        self_->geometry.weightBuffer = Render_GpuBufferCreate( self_->geometry.weightDataSize, RENDER_GPU_BUFFER_FLAGS_USAGE_GPU_ONLY );
    }
}

/*==========================================================================================================*/
X_API void RenderModel_Destroy( render_model_t * self_ ) {
    if ( self_->geometry.vertexBuffer ) {
        Render_GpuBufferDestroy( self_->geometry.vertexBuffer );
        self_->geometry.vertexBuffer = NULL;
    }

    if ( self_->geometry.indexBuffer ) {
        Render_GpuBufferDestroy( self_->geometry.indexBuffer );
        self_->geometry.indexBuffer = NULL;
    }

    if ( self_->geometry.weightBuffer ) {
        Render_GpuBufferDestroy( self_->geometry.weightBuffer );
        self_->geometry.weightBuffer = NULL;
    }
}

/*==========================================================================================================*/
X_API void RenderModel_CreateCube( render_model_t * self_, float sizeX, float sizeY, float sizeZ ) {
    RenderModel_Create( self_, 8, 36, 6, 0 );
    
    render_static_vertex_t vertices[8];
    Mem_Zero( vertices, sizeof(vertices) ); 

    vec3_t scaledVertex;
    vec3_t normal;

    const vec3_t boxScale = { sizeX, sizeY, sizeZ };
    for ( uint32_t i = 0; i < 8; ++i ) {
        Vec3_Mul( &scaledVertex, &CUBE_VERTICES[i], &boxScale );

        vertices[i].positionTexU[0] = scaledVertex.x;
        vertices[i].positionTexU[1] = scaledVertex.y;
        vertices[i].positionTexU[2] = scaledVertex.z;
        vertices[i].positionTexU[3] = 1.0f;

        // Calculate normal
        Vec3_Normalize( &normal, &scaledVertex );
        vertices[i].normalTexV[0] = normal.x;
        vertices[i].normalTexV[1] = normal.y;
        vertices[i].normalTexV[2] = normal.z;
        vertices[i].normalTexV[3] = 1.0f;
    }

    /* Create transfer buffers, copy over the contents ( although we could have just written the 
        vertex data directly to the transfer buffer ) and queue the upload to the gpu buffers 
    */
    render_gpu_buffer_t* vertexTransferBuffer = Render_GpuBufferCreate( sizeof(vertices), 0 );
    Sys_Assert( vertexTransferBuffer != NULL );
    
    render_gpu_buffer_t* indexTransferBuffer = Render_GpuBufferCreate( sizeof(CUBE_INDICES), 0 );
    Sys_Assert( indexTransferBuffer != NULL );

    render_static_vertex_t * mappedVerts = ( render_static_vertex_t * ) Render_GpuBuffer_Map( vertexTransferBuffer );
    Sys_Assert( mappedVerts != NULL );

    uint32_t * mappedIndices = ( uint32_t * ) Render_GpuBuffer_Map( indexTransferBuffer );
    Sys_Assert( mappedIndices != NULL );

    Mem_Copy( mappedVerts, vertices, sizeof(vertices) );
    Mem_Copy( mappedIndices, CUBE_INDICES, sizeof(CUBE_INDICES) );

    Render_GpuBuffer_Unmap( vertexTransferBuffer );
    Render_GpuBuffer_Unmap( indexTransferBuffer );

    Render_GpuBuffer_QueueWrite( self_->geometry.vertexBuffer, vertexTransferBuffer, 0, true );
    Render_GpuBuffer_QueueWrite( self_->geometry.indexBuffer, indexTransferBuffer, 0, true );
}