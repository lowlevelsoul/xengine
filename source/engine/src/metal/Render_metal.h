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
#import "Render_shared.h"
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
    id<MTLCommandBuffer> commandBuffer;

    CAMetalLayer* metalLayer;
    heap_pool_t gpuHeapPool;
    heap_pool_t commonHeapPool;

    BufferWriteQueue bufferWriteQueue;
    dispatch_semaphore_t inflightSem;
    uint32_t maxInflightFrames;
    uint32_t currBufferIndex;
    
    render_gpu_fence_t * lastPassFence;
    bool lastPassHadFence;

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

typedef struct render_gpu_fence_s {
    id<MTLFence> fence;
    uint64_t value;
} render_gpu_fence_t;

extern render_metal_t renderLocal;


X_API void Render_DidUpdateFence( render_gpu_fence_t * fence );

/* Evil, evil macros.... */
#define RenderPass_UpdateFence( pass_, encoder_, fence_ )\
    [encoder_ updateFence: fence_->fence ];\
    pass_->fence = fence_;\
    pass_->isFenceValid = true;\

#define RenderPass_HasFence( pass_ ) (pass_->isFenceValid == true )

#define RenderPass_WaitForFence( pass_, encoder_ )\
    [encoder_ waitForFence:pass_->fence ];\
    pass_->fence = NULL;\
    pass_->isFencevalid = false;

#define RenderPass_ClearFence( pass_ )\
    pass_->fence = NULL;\
    pass_->isFenceValid = false;

#endif
