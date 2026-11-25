/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#include "Render_shared.h"
#include "metal/Render_metal.h"
#include "xe/Sys.h"

typedef struct render_pass_copy_buffers_s {
    render_pass_t super;
    render_gpu_fence_t* fence;
} render_pass_copy_buffers_t;

RENDER_PASS_DEFINE( CopyBuffers, CopyBuffers, render_pass_copy_buffers_t )

/*==========================================================================================================*/
void CopyBuffers_Initialise( struct render_pass_s* self_ ) {
    render_pass_copy_buffers_t* pass = RenderPass_Get( render_pass_copy_buffers_t, self_ );
    pass->fence = Render_GpuFenceCreate();
}

/*==========================================================================================================*/
void CopyBuffers_Finalise(struct render_pass_s* self_ ) {
    render_pass_copy_buffers_t* pass = RenderPass_Get( render_pass_copy_buffers_t, self_ );
    Render_GpuFenceDestroy( pass->fence );
}

/*==========================================================================================================*/
void CopyBuffers_Render( struct render_pass_s* self_, struct render_pass_s* prevPass, batch_cmd_scene3d_t * scene ) {
    
    /* Don't bother doing anything if we have no write requests */
    if ( BufferWriteQueue_IsEmpty( &renderLocal.bufferWriteQueue ) == true ) {
        return;
    }

    render_pass_copy_buffers_t* pass = RenderPass_Get( render_pass_copy_buffers_t, self_ );

    @autoreleasepool {

        id<MTLBlitCommandEncoder> blitEncoder = [ renderLocal.commandBuffer blitCommandEncoder];
        Sys_FatalErrorOn( ( blitEncoder == nil ), "Failed to create blit encoder" );

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

        RenderPass_UpdateFence( self_, blitEncoder, pass->fence );
        
        [ blitEncoder endEncoding ];
    }
}
