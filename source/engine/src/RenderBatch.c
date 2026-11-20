/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "xe/RenderBatch.h"
#include "xe/Mem.h"
#include "xe/FrameHeap.h"
#include "xe/Sys.h"
#include "xe/MathLinear.h"

typedef struct render_batch_s {
    frame_heap_t* frameHeap;
    void* frameHeapMemory;
    size_t frameHeapMemorySize;

    batch_cmd_scene_t* currScene;
    batch_cmd_scene3d_t* currScene3d;
} render_batch_t;

/* A useful macro for appending a command to a single-linked list with a head and tail */
#define RB_AppendCmdList(listHead, listTail, cmd) \
    if ( ( listTail ) ) { \
        (listTail)->next = (cmd); \
        (listTail) = (cmd); \
    } else { \
        (listHead) = (listTail) = (cmd); \
    }\
    (cmd)->next = NULL;

/*==========================================================================================================*/
render_batch_t * RB_Create( size_t size ) {
    void * memory = Mem_Alloc( size );
    if ( !memory ) {
        return NULL;
    }
    render_batch_t * self_ = ( render_batch_t * ) memory;

    uintptr_t batchMemoryStart = ( ( uintptr_t ) memory + sizeof( render_batch_t ));
    size_t batchMemorySize = size - sizeof( render_batch_t );
    
    self_->frameHeapMemory = ( void * ) batchMemoryStart;
    self_->frameHeapMemorySize = batchMemorySize;
    self_->frameHeap = FrameHeap_Create( ( void * ) batchMemoryStart, batchMemorySize );
    self_->currScene = NULL;
    self_->currScene3d = NULL;
    return self_;
}

/*==========================================================================================================*/
void RB_Destroy( render_batch_t * self_ ) {
    if ( self_ ) {
        FrameHeap_Destroy( self_->frameHeap );
        Mem_Free( self_ );
    }
}

/*==========================================================================================================*/
void RB_Reset( render_batch_t * self_ ) {
    if ( self_ ) {
        FrameHeap_Reset( self_->frameHeap );
        self_->currScene = NULL;
        self_->currScene3d = NULL;
    }
}

/*==========================================================================================================*/
batch_cmd_scene_t * RB_BeginScene( render_batch_t * self_ ) {
    Sys_Assert( self_ );
    Sys_Assert( self_->currScene == NULL );

    self_->currScene = FrameHeap_Calloc( self_->frameHeap, sizeof( batch_cmd_scene_t ), 1 );
    return self_->currScene;
}

/*==========================================================================================================*/
void RB_EndScene( render_batch_t * self_ ) {
    Sys_Assert( self_ );
    Sys_Assert( self_->currScene );

    self_->currScene = NULL;
}

/*==========================================================================================================*/
void RB_BeginScene3d( render_batch_t * self_, const mat4_t * viewMatrix, const mat4_t * projectionMatrix, const float * viewport ) {
    Sys_Assert( self_ );
    Sys_Assert( viewMatrix );
    Sys_Assert( projectionMatrix );
    Sys_Assert( viewport );
    Sys_Assert( self_->currScene3d == NULL );

    self_->currScene3d = FrameHeap_Calloc( self_->frameHeap, sizeof( batch_cmd_scene3d_t ), 1 );
    self_->currScene3d->viewMatrix = *viewMatrix;
    self_->currScene3d->projectionMatrix = *projectionMatrix;
    
    Mat4_Inverse( &self_->currScene3d->viewMatrixInv, &self_->currScene3d->viewMatrix ); 

    RB_AppendCmdList( self_->currScene->scene3dListHead, self_->currScene->scene3dListTail, self_->currScene3d );

    memcpy( self_->currScene3d->viewport, viewport, sizeof( float ) * 4 ); 
}

/*==========================================================================================================*/
void RB_EndScene3d( render_batch_t * self_ ) {
    Sys_Assert( self_ );
    Sys_Assert( self_->currScene3d );
    self_->currScene3d = NULL;
}

/*==========================================================================================================*/
void RB_SubmitModel( render_batch_t * self_, render_model_t * model, const mat4_t * transform ) {  
    Sys_Assert( self_ );
    Sys_Assert( model );
    Sys_Assert( transform );
    Sys_Assert( self_->currScene3d );

    /* Alloc and copy the draw surfaces */
    const uint32_t surfaceCount = model->drawSurfaceCount;
    render_draw_surface_t * drawSurfaces = FrameHeap_Calloc( self_->frameHeap, sizeof( render_draw_surface_t ), surfaceCount );
    for ( uint32_t i = 0; i < surfaceCount; ++i ) {
        drawSurfaces[i] = model->drawsurfaces[ i ];
    }

    /* Create the draw model command 
       Although this is sort of redundant as we'll be able to draw the 3d scene with a single draw call.
    */
    batch_cmd_draw_model_t * cmd = FrameHeap_Calloc( self_->frameHeap, sizeof( batch_cmd_draw_model_t ), 1 );
    cmd->surfaces = drawSurfaces;
    cmd->surfaceCount = surfaceCount;

    RB_AppendCmdList( self_->currScene3d->opaqueDrawModelListHead, self_->currScene3d->opaqueDrawModelListTail, cmd );

    /* Create the static transform command for this model */
    batch_cmd_static_transform_t * tfCmd = FrameHeap_Calloc( self_->frameHeap, sizeof( batch_cmd_static_transform_t ), 1 );
    tfCmd->vertex_buffer = model->geometry.vertex_buffer;
    tfCmd->index_buffer = model->geometry.index_buffer;
    tfCmd->surfaces = drawSurfaces;
    tfCmd->surfaceCount = surfaceCount;
    Mat4_Concat( &tfCmd->modelViewTransform, &self_->currScene3d->viewMatrix, transform );

    RB_AppendCmdList( self_->currScene3d->transformListHead, self_->currScene3d->transformListTail, tfCmd );    
}

/*==========================================================================================================*/
void RB_SetGlobalLight( render_batch_t * self_, const vec3_t * direction, const vec3_t * color, const vec3_t * ambient ) {
    Sys_Assert( self_ );
    Sys_Assert( self_->currScene3d );
    Sys_Assert( direction );
    Sys_Assert( color );
    Sys_Assert( ambient );

    self_->currScene3d->globalLightDirection = *direction;
    self_->currScene3d->globalLightColor = *color;
    self_->currScene3d->globalLightAmbient = *ambient;
}

/*==========================================================================================================*/
void RB_QueueBufferWrite( render_batch_t * self_, render_gpu_buffer_t* srcBuffer, render_gpu_buffer_t* dstBuffer, size_t offset, bool canDiscardSrc ) {
    Sys_Assert( self_ );
    Sys_Assert( self_->currScene3d );
    Sys_Assert( srcBuffer );
    Sys_Assert( dstBuffer );
    Sys_Assert( offset >= 0 );

    batch_cmd_buffer_write_t * cmd = FrameHeap_Calloc( self_->frameHeap, sizeof( batch_cmd_buffer_write_t ), 1 );
    cmd->srcbuffer = srcBuffer;
    cmd->dstbuffer = dstBuffer;
    cmd->offset = offset;
    cmd->canDiscardSrc = canDiscardSrc;

    RB_AppendCmdList( self_->currScene3d->bufferWriteListHead, self_->currScene3d->bufferWriteListTail, cmd );
}