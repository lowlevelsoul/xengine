/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "xe/Render.h"
#import "xe/RenderBatch.h"
#import "xe/Sys.h"
#import "xe/Mem.h"
#import "Render_metal.h"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

extern void RenderWindow_Create( render_params_t* params );
extern void RenderWindow_Destroy( void );

render_metal_t renderLocal;
static bool isRenderCreated = false;
MTLClearColor color;

static void Render_CreateSwapchain( render_params_t* params );

/*==========================================================================================================*/
void Render_Initialise( render_params_t* params ) {

    if ( isRenderCreated == true ) {
        return;
    }

    renderLocal.device = MTLCreateSystemDefaultDevice();

    RenderWindow_Create( params );
    Render_CreateSwapchain( params );

    color = MTLClearColorMake( 0, 0, 0, 1 );
}

/*==========================================================================================================*/
void Render_Finalise( void ) {
    RenderWindow_Destroy();
    if ( isRenderCreated ) {
        isRenderCreated = false;
    }
}

/*==========================================================================================================*/
void Render_CreateSwapchain( render_params_t* params ) {
    isRenderCreated = true;

    NSWindow* cocoaWindow = ( __bridge NSWindow* )RenderWindow_GetNativeHandle();
    NSView* cocoaView = cocoaWindow.contentView;
    bool isValidView = [cocoaView isKindOfClass:[NSView class]];

    Sys_FatalErrorOn( isValidView == false, "Contentview is not derived from NSview" );
    Sys_FatalErrorOn( cocoaView == nil, "Could not get Cocoa view from window" );

    renderLocal.metalLayer = [CAMetalLayer layer];
    Sys_FatalErrorOn( ( renderLocal.metalLayer == nil ), "Did not create metal layer" );

    renderLocal.metalLayer.device = renderLocal.device;
    renderLocal.metalLayer.opaque = YES;
    renderLocal.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    renderLocal.metalLayer.framebufferOnly = YES;

    cocoaView.wantsLayer = YES;
    cocoaView.layer = renderLocal.metalLayer;

    renderLocal.inflightSem = dispatch_semaphore_create( params->maxNumFramesInFlight );
    renderLocal.maxInflightFrames = params->maxNumFramesInFlight;

    renderLocal.commandQueue = [renderLocal.device newCommandQueue];
}

#if 0
/*==========================================================================================================*/
static void Render_ProcessBufferWrites( id<MTLBlitCommandEncoder> blitEncoder ) {

    buffer_write_request_t request;
    while ( BufferWriteQueue_Read( &renderLocal.bufferWriteQueue, &request ) ) {

        [blitEncoder copyFromBuffer:request.srcBuffer->buffer
                       sourceOffset:0
                           toBuffer:request.dstBuffer->buffer
                  destinationOffset:request.offset
                               size:request.srcBuffer->size];

        if ( request.canDisposeSrcBuffer && request.srcBuffer ) {
            /* Dispose of the buffer if possible. We can just delete the buffer
               becausse Metal will keep the reference to the buffer internally until
               it's no longer needed.
            */
            Render_GpuBufferDestroy( request.srcBuffer );
        }
    }
}


/*==========================================================================================================*/
static void Render_ProceesWriteRequests( id<MTLCommandBuffer> commandBuffer ) {

    @autoreleasepool {
        MTLBlitPassDescriptor* blitDescriptor = [MTLBlitPassDescriptor blitPassDescriptor];
        Sys_FatalErrorOn( ( blitDescriptor == nil ), "Failed to create blit descriptor" );
        id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoderWithDescriptor:blitDescriptor];
        Sys_FatalErrorOn( ( blitEncoder == nil ), "Failed to create blit encoder" );

        Render_ProcessBufferWrites( blitEncoder );
        [blitEncoder endEncoding];
    }
}

#endif

/*==========================================================================================================*/
X_API void Render_DidUpdateFence( render_gpu_fence_t * fence ) {
    renderLocal.lastPassFence = fence;
    renderLocal.lastPassHadFence = true;
}


/*==========================================================================================================*/
X_API void Render_SubmitScene( batch_cmd_scene_t* scene ) {

    @autoreleasepool {

        dispatch_semaphore_wait( renderLocal.inflightSem, DISPATCH_TIME_FOREVER );

        /* Get the next surface we'll be drawing to */
        id<CAMetalDrawable> surface = renderLocal.metalLayer.nextDrawable;
        if ( surface == nil ) {
            dispatch_semaphore_signal( renderLocal.inflightSem );
            return;
        }

        renderLocal.presentSurface = surface;

        id<MTLCommandBuffer> commandBuffer = [renderLocal.commandQueue commandBuffer];
        commandBuffer.label = @"MyCommand";
        renderLocal.commandBuffer = commandBuffer;

        __block dispatch_semaphore_t block_sema = renderLocal.inflightSem;
        [commandBuffer addCompletedHandler:^( id<MTLCommandBuffer> buffer ) {
          dispatch_semaphore_signal( block_sema );
        }];

        /* TEMP - Just want to display something to ensure it's all working. */
        color.red = ( color.red > 1.0 ) ? 0 : color.red + 0.01;

        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[ 0 ].clearColor = color;
        pass.colorAttachments[ 0 ].loadAction = MTLLoadActionClear;
        pass.colorAttachments[ 0 ].storeAction = MTLStoreActionStore;
        pass.colorAttachments[ 0 ].texture = surface.texture;

        id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:pass];
        [encoder endEncoding];
        
        /* Perform the render passes */
        render_pass_t * prevPass = NULL;
        render_pass_t * currPass = NULL;
        for ( uint32_t passIndex = 0; passIndex != renderLocal.numPasses; ++passIndex ) {
            currPass = renderLocal.passes[ passIndex ];
            
            RenderPass_ClearFence( currPass );
            currPass->Render( currPass, prevPass, scene->scene3dListHead );
            prevPass = currPass;
        }

        /* Present the drawable and commit the command buffer */
        [commandBuffer presentDrawable:surface];
        [commandBuffer commit];
    }

    /* Update the buffer index for the next frame */
    renderLocal.currBufferIndex = ( renderLocal.currBufferIndex + 1 ) % renderLocal.maxInflightFrames;
}

/*
==========================================================================================================
 Render GPU fence management functions
==========================================================================================================
*/

/*
==========================================================================================================
 Render GPU buffer management functions
==========================================================================================================
*/

/*==========================================================================================================*/
render_gpu_buffer_t* Render_GpuBufferCreate( size_t size, uint64_t flags ) {
    render_gpu_buffer_t* buffer = Mem_CallocType( render_gpu_buffer_t, 1 );

    buffer->size = size;
    buffer->gpuPrivate = ( flags & RENDER_GPU_BUFFER_FLAGS_USAGE_GPU_ONLY ) ? 1 : 0;
    buffer->mappedMem = NULL;
    buffer->mapCount = 0;

    if ( !buffer->gpuPrivate ) {
        buffer->buffer = RenderHeapPool_CreateBuffer( &renderLocal.commonHeapPool, size, MTLResourceStorageModeShared );
    } else {
        buffer->buffer = RenderHeapPool_CreateBuffer( &renderLocal.gpuHeapPool, size, MTLResourceStorageModePrivate );
    }

    return buffer;
}

/*==========================================================================================================*/
void Render_GpuBufferDestroy( render_gpu_buffer_t* self_ ) {
    Sys_FatalErrorOn( self_->mapCount != 0, "Buffer is still mapped" );

    self_->buffer = nil;
    Mem_Free( self_ );
}

/*==========================================================================================================*/
void Render_GpuBuffer_QueueWrite( render_gpu_buffer_t* buffer, render_gpu_buffer_t* srcBuffer, size_t offset,
                                  bool releaseSrcBuffer ) {
    buffer_write_request_t request = { .dstBuffer = buffer,
                                       .srcBuffer = srcBuffer,
                                       .offset = offset,
                                       .canDisposeSrcBuffer = releaseSrcBuffer ? 1 : 0 };

    bool writeOk = BufferWriteQueue_Write( &renderLocal.bufferWriteQueue, &request );
    Sys_FatalErrorOn( !writeOk, "Failed to write to buffer write queue" );
}

/*==========================================================================================================*/
void* Render_GpuBuffer_Map( render_gpu_buffer_t* buffer ) {
    Sys_FatalErrorOn( buffer->gpuPrivate, "Cannot map GPU private buffer" );

    if ( buffer->mapCount == 0 ) {
        buffer->mappedMem = buffer->buffer.contents;
    }

    buffer->mapCount++;
    return buffer->mappedMem;
}

/*==========================================================================================================*/
void Render_GpuBuffer_Unmap( render_gpu_buffer_t* buffer ) {
    if ( buffer->mapCount > 0 ) {
        buffer->mapCount--;
        if ( buffer->mapCount == 0 ) {
            buffer->mappedMem = NULL;
        }
    }
}
