/*
 * clang -Wall -Wextra -Werror -Wno-unused-const-variable -I source/engine/inc -I source/engine/inc/xe \
 *   source/engine/tests/Gpu_metal_context_test.m source/engine/src/metal/Gpu_metal.m \
 *   source/engine/src/metal/Gpu_rendercontext_metal.m source/engine/src/Mem.c source/engine/src/Str.c \
 *   source/engine/src/Sys_macos.m \
 *   -framework Metal -framework Foundation -o /tmp/xengine-metal-context-test && /tmp/xengine-metal-context-test
 */
#include "xe/Gpu.h"
#include "../src/metal/Gpu_metal.h"
#include <assert.h>

int main( void ) {
    @autoreleasepool {
        gpu_render_context_params_t params = {
            .width = 64, .height = 64, .maxNumFramesInFlight = 2, .heapAllocSize = 65536 };
        assert( !Gpu_RenderContext_Create( &params ) );
        Gpu_Initialise();
        assert( gpu.device );
        gpu_render_context_params_t invalid = params;
        invalid.heapAllocSize = 0;
        assert( !Gpu_RenderContext_Create( &invalid ) );
        invalid = params;
        invalid.maxNumFramesInFlight = 0;
        assert( !Gpu_RenderContext_Create( &invalid ) );

        gpu_render_context_t* context = Gpu_RenderContext_Create( &params );
        assert( context && gpu.currentContext == context );
        assert( context->heapAllocSize == params.heapAllocSize );
        assert( !Gpu_RenderContext_Create( &params ) );
        Gpu_Finalise();
        assert( gpu.device && gpu.currentContext == context );
        Gpu_RenderContext_EndFrame( context );

        for ( unsigned i = 0; i < 10; ++i ) {
            @autoreleasepool {
                Gpu_RenderContext_BeginFrame( context );
                assert( context->currentCommandBuffer );
            }

            assert( context->currentCommandBuffer.status == MTLCommandBufferStatusNotEnqueued );
            Gpu_RenderContext_EndFrame( context );
            assert( !context->currentCommandBuffer );
        }

        Gpu_RenderContext_BeginFrame( context );
        assert( context->currentCommandBuffer );
        Gpu_RenderContext_BeginFrame( context );
        Gpu_RenderContext_Destroy( context );
        assert( gpu.currentContext == context );
        id<MTLBuffer> result = [gpu.device newBufferWithLength:64 options:MTLResourceStorageModeShared];
        assert( result );
        id<MTLBlitCommandEncoder> blit = [context->currentCommandBuffer blitCommandEncoder];
        assert( blit );
        [blit fillBuffer:result range:NSMakeRange( 0, 64 ) value:0x5a];
        [blit endEncoding];
        Gpu_RenderContext_EndFrame( context );
        Gpu_RenderContext_Destroy( context );
        assert( !gpu.currentContext );
        const unsigned char* contents = [result contents];
        for ( unsigned i = 0; i < 64; ++i ) {
            assert( contents[ i ] == 0x5a );
        }

#if !__has_feature( objc_arc )
        [result release];
#endif
        Gpu_Finalise();
        assert( !gpu.device );
    }

    return 0;
}
