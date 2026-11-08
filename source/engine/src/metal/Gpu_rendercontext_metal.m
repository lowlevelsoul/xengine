/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "Gpu_metal.h"
#include "xe/Gpu.h"
#include "xe/Mem.h"
#include <stdio.h>

void Gpu_Metal_Fatal( const char* message ) {
    fprintf( stderr, "Metal: %s\n", message );
    Sys_FatalError( "Metal resource failure" );
    abort();
}

/* All allocation helpers require the context mutex. */
gpu_allocation_t* Gpu_Metal_AllocateBufferLocked( gpu_render_context_t* context, uint32_t size,
                                                  MTLResourceOptions options ) {
    options |= MTLResourceHazardTrackingModeTracked;
    MTLSizeAndAlign required = [gpu.device heapBufferSizeAndAlignWithLength:size options:options];
    if ( !required.size || required.size > context->heapAllocSize ) {
        Gpu_Metal_Fatal( "Metal buffer does not fit in a heap" );
    }

    uint32_t category = ( options & MTLResourceStorageModeMask ) == MTLResourceStorageModePrivate ? 1 : 0;
    uint32_t index = context->activeHeaps[ category ];
    if ( index != GPU_MAX_HEAPS &&
         [context->heaps[ index ].heap maxAvailableSizeWithAlignment:required.align] < required.size ) {
        index = GPU_MAX_HEAPS;
    }

    if ( index == GPU_MAX_HEAPS ) {
        for ( uint32_t i = 0; i < GPU_MAX_HEAPS; ++i ) {
            if ( !context->heaps[ i ].heap ) {
                index = i;
                break;
            }
        }

        if ( index == GPU_MAX_HEAPS ) {
            Gpu_Metal_Fatal( "Metal heap limit reached" );
        }

        MTLHeapDescriptor* descriptor = [[MTLHeapDescriptor alloc] init];
        descriptor.size = context->heapAllocSize;
        descriptor.storageMode = ( options & MTLResourceStorageModeMask ) >> MTLResourceStorageModeShift;
        descriptor.cpuCacheMode = ( options & MTLResourceCPUCacheModeMask ) >> MTLResourceCPUCacheModeShift;
        descriptor.hazardTrackingMode = MTLHazardTrackingModeTracked;
        context->heaps[ index ].heap = [gpu.device newHeapWithDescriptor:descriptor];
#if !__has_feature( objc_arc )
        [descriptor release];
#endif
        if ( !context->heaps[ index ].heap ) {
            Gpu_Metal_Fatal( "Metal heap allocation failed" );
        }

        context->activeHeaps[ category ] = index;
    }

    gpu_allocation_t* allocation = Mem_Alloc( sizeof( *allocation ) );
    if ( !allocation ) {
        Gpu_Metal_Fatal( "Metal allocation bookkeeping failed" );
    }

    Mem_Zero( allocation, sizeof( *allocation ) );
    allocation->buffer = [context->heaps[ index ].heap newBufferWithLength:size options:options];
    if ( !allocation->buffer ) {
        Mem_Free( allocation );
        Gpu_Metal_Fatal( "Metal heap buffer allocation failed" );
    }

    allocation->heapIndex = index;
    allocation->references = 1;
    ++context->heaps[ index ].allocations;
    return allocation;
}

void Gpu_Metal_ReleaseAllocationLocked( gpu_render_context_t* context, gpu_allocation_t* allocation ) {
    if ( !allocation || --allocation->references ) {
        return;
    }

    uint32_t heapIndex = allocation->heapIndex;
    gpu_heap_t* heap = &context->heaps[ heapIndex ];
#if !__has_feature( objc_arc )
    [allocation->buffer release];
#endif
    allocation->buffer = nil;
    Mem_Free( allocation );
    if ( --heap->allocations == 0 ) {
        for ( unsigned i = 0; i < 2; ++i ) {
            if ( context->activeHeaps[ i ] == heapIndex ) {
                context->activeHeaps[ i ] = GPU_MAX_HEAPS;
            }
        }

#if !__has_feature( objc_arc )
        [heap->heap release];
#endif
        heap->heap = nil;
    }
}

/*==========================================================================================================*/
X_API gpu_render_context_t* Gpu_RenderContext_Create( const gpu_render_context_params_t* params ) {
    if ( !gpu.device || gpu.currentContext || !params || !params->maxNumFramesInFlight || !params->heapAllocSize ) {
        fprintf( stderr, "Gpu_RenderContext_Create: invalid device, context, or parameters\n" );
        return NULL;
    }

    gpu_render_context_t* context = Mem_Alloc( sizeof( *context ) );
    if ( !context ) {
        fprintf( stderr, "Gpu_RenderContext_Create: allocation failed\n" );
        return NULL;
    }

    Mem_Zero( context, sizeof( *context ) );
    context->activeHeaps[ 0 ] = GPU_MAX_HEAPS;
    context->activeHeaps[ 1 ] = GPU_MAX_HEAPS;
    context->commandQueue = [gpu.device newCommandQueue];
    context->frameSlots = dispatch_semaphore_create( params->maxNumFramesInFlight );
    context->submittedFrames = dispatch_group_create();
    context->heapAllocSize = params->heapAllocSize;
    if ( !context->commandQueue || !context->frameSlots || !context->submittedFrames ) {
        fprintf( stderr, "Gpu_RenderContext_Create: Metal context allocation failed\n" );
#if !__has_feature( objc_arc )
        [context->commandQueue release];
        if ( context->frameSlots ) {
            dispatch_release( context->frameSlots );
        }

        if ( context->submittedFrames ) {
            dispatch_release( context->submittedFrames );
        }
#else
        context->commandQueue = nil;
        context->frameSlots = NULL;
        context->submittedFrames = NULL;
#endif
        Mem_Free( context );
        return NULL;
    }

    Sys_MuxCreate( &context->mutex );
    gpu.currentContext = context;
    return context;
}

/*==========================================================================================================*/
X_API void Gpu_RenderContext_Destroy( gpu_render_context_t* context ) {
    if ( !context || context != gpu.currentContext ) {
        fprintf( stderr, "Gpu_RenderContext_Destroy: invalid context\n" );
        return;
    }

    if ( context->currentCommandBuffer ) {
        fprintf( stderr, "Gpu_RenderContext_Destroy: end the current frame first\n" );
        return;
    }

    dispatch_group_wait( context->submittedFrames, DISPATCH_TIME_FOREVER );
    Sys_MuxLock( &context->mutex );
    for ( uint32_t i = 0; i < context->writeCount; ++i ) {
        gpu_pending_write_t* write = &context->writes[ i ];
        --write->owner->pendingWrites;
        Gpu_Metal_ReleaseAllocationLocked( context, write->destination );
        Gpu_Metal_ReleaseAllocationLocked( context, write->staging );
    }

    context->writeCount = 0;
    if ( context->liveBuffers ) {
        Sys_MuxUnlock( &context->mutex );
        fprintf( stderr, "Gpu_RenderContext_Destroy: destroy live buffers first\n" );
        return;
    }

    Sys_MuxUnlock( &context->mutex );
    Sys_MuxDestroy( &context->mutex );
    gpu.currentContext = NULL;
#if !__has_feature( objc_arc )
    [context->commandQueue release];
    dispatch_release( context->frameSlots );
    dispatch_release( context->submittedFrames );
#else
    context->commandQueue = nil;
    context->frameSlots = NULL;
    context->submittedFrames = NULL;
#endif
    Mem_Free( context );
}

/*==========================================================================================================*/
X_API void Gpu_RenderContext_BeginFrame( gpu_render_context_t* context ) {
    if ( !context || context != gpu.currentContext || context->currentCommandBuffer ) {
        fprintf( stderr, "Gpu_RenderContext_BeginFrame: invalid context or frame already active\n" );
        return;
    }

    dispatch_semaphore_wait( context->frameSlots, DISPATCH_TIME_FOREVER );
    id<MTLCommandBuffer> commandBuffer = [context->commandQueue commandBuffer];
    if ( !commandBuffer ) {
        dispatch_semaphore_signal( context->frameSlots );
        fprintf( stderr, "Gpu_RenderContext_BeginFrame: failed to create Metal command buffer\n" );
        return;
    }

#if !__has_feature( objc_arc )
    [commandBuffer retain];
#endif
    context->currentCommandBuffer = commandBuffer;
    Sys_MuxLock( &context->mutex );
    if ( context->writeCount ) {
        uint32_t count = context->writeCount;
        gpu_pending_write_t* batch = Mem_Alloc( count * sizeof( *batch ) );
        if ( !batch ) {
            Gpu_Metal_Fatal( "Metal upload bookkeeping allocation failed" );
        }

        id<MTLBlitCommandEncoder> blit = [commandBuffer blitCommandEncoder];
        if ( !blit ) {
            Gpu_Metal_Fatal( "Metal upload encoder creation failed" );
        }

        for ( uint32_t i = 0; i < count; ++i ) {
            gpu_pending_write_t* write = &context->writes[ i ];
            batch[ i ] = *write;
            [blit copyFromBuffer:write->staging->buffer
                     sourceOffset:0
                         toBuffer:write->destination->buffer
                destinationOffset:write->offset
                             size:write->size];
            --write->owner->pendingWrites;
        }

        [blit endEncoding];
        context->writeCount = 0;
        dispatch_group_enter( context->submittedFrames );
        [commandBuffer addCompletedHandler:^( id<MTLCommandBuffer> completed ) {
          ( void )completed;
          Sys_MuxLock( &context->mutex );
          for ( uint32_t i = 0; i < count; ++i ) {
              Gpu_Metal_ReleaseAllocationLocked( context, batch[ i ].destination );
              Gpu_Metal_ReleaseAllocationLocked( context, batch[ i ].staging );
          }

          Sys_MuxUnlock( &context->mutex );
          Mem_Free( batch );
          dispatch_group_leave( context->submittedFrames );
        }];
    }

    Sys_MuxUnlock( &context->mutex );
}

/*==========================================================================================================*/
X_API void Gpu_RenderContext_EndFrame( gpu_render_context_t* context ) {
    if ( !context || context != gpu.currentContext || !context->currentCommandBuffer ) {
        fprintf( stderr, "Gpu_RenderContext_EndFrame: invalid context or no active frame\n" );
        return;
    }

    if ( context->currentRenderEncoder ) {
        [context->currentRenderEncoder endEncoding];
        context->currentRenderEncoder = nil;
    }

    id<MTLCommandBuffer> commandBuffer = context->currentCommandBuffer;
    dispatch_semaphore_t slots = context->frameSlots;
    dispatch_group_t frames = context->submittedFrames;
    dispatch_group_enter( frames );
    [commandBuffer addCompletedHandler:^( id<MTLCommandBuffer> completed ) {
      if ( completed.status != MTLCommandBufferStatusCompleted ) {
          fprintf( stderr, "Gpu_RenderContext_EndFrame: Metal command buffer failed: %s\n",
                   completed.error ? completed.error.localizedDescription.UTF8String : "unknown error" );
      }

      dispatch_semaphore_signal( slots );
      dispatch_group_leave( frames );
    }];
    [commandBuffer commit];
    context->currentCommandBuffer = nil;
#if !__has_feature( objc_arc )
    [commandBuffer release];
#endif
}
