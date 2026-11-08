/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __GPU_TYPES_METAL_H__
#define __GPU_TYPES_METAL_H__

#import <Metal/Metal.h>
#include "Gpu_types.h"
#include "xe/Sys.h"
#include <stdatomic.h>

#define GPU_MAX_HEAPS 256
#define GPU_MAX_PENDING_WRITES 4096

typedef struct gpu_heap_s {
    id<MTLHeap> heap;
    uint32_t allocations;
} gpu_heap_t;

typedef struct gpu_allocation_s {
    id<MTLBuffer> buffer;
    uint32_t heapIndex;
    uint32_t references;
} gpu_allocation_t;

typedef struct texture_format_info_s {
    gpu_texture_format_t format;
    MTLPixelFormat pixelFormat;
    int32_t componentCount; /* Number of components in the texture format */
    int32_t componentSize;  /* Bytes per component, or -1 for compressed or packed formats */
    int32_t blockSize;      /* Size of the block in bytes for block compressed formats, -1 in non compressed formats */
    int32_t blockWidth;  /* Width of the block in pixels for block compressed formats, -1 in non compressed formats */
    int32_t blockHeight; /* Height of the block in pixels for block compressed formats, -1 in non compressed formats */
} texture_format_info_t;

typedef struct gpu_buffer_s {
    id<MTLBuffer> buffer;
    size_t size;
    bool mapped;
    atomic_uint pendingWrites;
    gpu_allocation_t* allocation;
} gpu_buffer_t;

typedef struct gpu_pending_write_s {
    gpu_buffer_t* owner;
    gpu_allocation_t* destination;
    gpu_allocation_t* staging;
    uint32_t offset;
    uint32_t size;
} gpu_pending_write_t;

typedef struct gpu_texture_s {
    id<MTLTexture> texture;
    uint32_t width;
    uint32_t height;
    uint32_t mipCount;
    gpu_texture_format_t format;
    const texture_format_info_t* formatInfo;
} gpu_texture_t;

typedef struct gpu_render_context_s {
    id<MTLCommandQueue> commandQueue;
    id<MTLCommandBuffer> currentCommandBuffer;
    id<MTLRenderCommandEncoder> currentRenderEncoder;
    dispatch_semaphore_t frameSlots;
    dispatch_group_t submittedFrames;
    uint32_t heapAllocSize;
    sys_mux_t mutex;
    gpu_heap_t heaps[ GPU_MAX_HEAPS ];
    uint32_t activeHeaps[ 2 ];
    gpu_pending_write_t writes[ GPU_MAX_PENDING_WRITES ];
    uint32_t writeCount;
    uint32_t liveBuffers;
} gpu_render_context_t;

typedef struct gpu_s {
    id<MTLDevice> device;
    gpu_render_context_t* currentContext;
} gpu_t;

extern gpu_t gpu;

void Gpu_Metal_Fatal( const char* message );
gpu_allocation_t* Gpu_Metal_AllocateBufferLocked( gpu_render_context_t* context, uint32_t size,
                                                  MTLResourceOptions options );
void Gpu_Metal_ReleaseAllocationLocked( gpu_render_context_t* context, gpu_allocation_t* allocation );

#endif // __GPU_TYPES_METAL_H__