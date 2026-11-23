/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef RENDER_METAL_H
#define RENDER_METAL_H

#import "xe/MPMCQueue.h"
#import "xe/Render.h"
#import "xe/RenderBatch.h"
#import "RenderHeapPool_metal.h"
#import "Render_metal.h"
#import <QuartzCore/QuartzCore.h>
#import <Metal/Metal.h>

extern void RenderWindow_Create( render_params_t* params );
extern void RenderWindow_Destroy( void );

typedef struct buffer_write_request_s {
    render_gpu_buffer_t* dstBuffer;
    render_gpu_buffer_t* srcBuffer;
    size_t offset;
    uint64_t canDisposeSrcBuffer : 1;
} buffer_write_request_t;

typedef struct render_pass_s {
    void ( *Initialise )( struct render_pass_s* self_ );
    void ( *Finalise )( struct render_pass_s* self_ );
    void ( *Render )( struct render_pass_s* self_, struct render_pass_s* prev, batch_cmd_scene3d_t* scene );
    void ( *Resize )( struct render_pass_s* self_, int width, int height );
    bool ( *HasFence )( struct render_pass_s* self_ );
    id<MTLFence> ( *GetFence )( struct render_pass_s* self_ );

} render_pass_t;

MPMC_QUEUE_DEFINE( BufferWriteQueue, buffer_write_request_t, 2048 )

#define RENDER_MAX_NUM_PASSES 64
#define RENDER_MAX_INFLIGHT_FRAMES 3

typedef struct render_constantbuffer_s {
    render_gpu_buffer_t* buffer;
    size_t size;
    uintptr_t currPtr;
} render_constantbuffer_t;

typedef struct render_metal_s {

    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<CAMetalDrawable> presentSurface;

    CAMetalLayer* metalLayer;
    heap_pool_t gpuHeapPool;
    heap_pool_t commonHeapPool;

    BufferWriteQueue bufferWriteQueue;
    dispatch_semaphore_t inflightSem;
    uint32_t maxInflightFrames;
    uint32_t currBufferIndex;

    render_constantbuffer_t constantBuffers[ RENDER_MAX_INFLIGHT_FRAMES ];

    render_pass_t* passes[ RENDER_MAX_NUM_PASSES ];
    uint32_t numPasses;
} render_metal_t;

typedef struct render_gpu_buffer_s {
    id<MTLBuffer> buffer;
    size_t size;
    uint64_t gpuPrivate : 1; /* if 1, this buffer is in gpu private memory and cannot be mapped */
    void* mappedMem;
    uint32_t mapCount;
} render_gpu_buffer_s;

extern render_metal_t renderLocal;

#endif
