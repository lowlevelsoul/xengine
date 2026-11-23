/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef RENDER_HEAP_POOL_METAL_H
#define RENDER_HEAP_POOL_METAL_H

#import "xe/Platform.h"
#import <Metal/Metal.h>

#define MAX_NUM_HEAPS 16

/* Pool instances must be zero-initialized before their first initialization. */
typedef struct heap_pool_s {
    id<MTLHeap> heaps[ MAX_NUM_HEAPS ];
    size_t numHeaps;
    size_t heapSize;
    size_t lastSuccessfulHeapIndex;
    MTLStorageMode storageMode;
    MTLCPUCacheMode cacheMode;
    MTLHazardTrackingMode hazardTrackingMode;
    bool isCreated;
} heap_pool_t;

void RenderHeapPool_Initialise( heap_pool_t* self_, size_t heapSize, MTLStorageMode storageMode,
                                MTLCPUCacheMode cacheMode, MTLHazardTrackingMode hazardTrackingMode );
void RenderHeapPool_Finalise( heap_pool_t* self_ );
id<MTLHeap> RenderHeapPool_GetHeap( heap_pool_t* self_ );
id<MTLBuffer> RenderHeapPool_CreateBuffer( heap_pool_t* self_, NSUInteger length, MTLResourceOptions options );
id<MTLTexture> RenderHeapPool_CreateTexture( heap_pool_t* self_, MTLTextureDescriptor* descriptor );

#endif // RENDER_HEAP_POOL_METAL_H
