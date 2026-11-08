/*
 * Run from the repository root:
 * clang -Wall -Wextra -Werror -Wno-unused-const-variable -I source/engine/inc \
 *     -I source/engine/inc/xe source/engine/tests/Gpu_metal_buffer_test.m source/engine/src/Mem.c \
 *     source/engine/src/Str.c source/engine/src/Sys_macos.m source/engine/src/metal/Gpu_rendercontext_metal.m \
 *     -framework Metal -framework Foundation \
 *     -o /tmp/xengine-metal-buffer-test && /tmp/xengine-metal-buffer-test
 */
#include <assert.h>
#include <string.h>
#include "../src/metal/Gpu_metal.m"
#include "../src/metal/Gpu_buffer_metal.m"

int main( int argc, char** argv ) {
    @autoreleasepool {
        Sys_Initialise();
        Gpu_Initialise();
        if ( !gpu_initialized ) {
            fprintf( stderr, "Metal device required for buffer tests\n" );
            return 1;
        }

        gpu_render_context_params_t params = { .maxNumFramesInFlight = 2, .heapAllocSize = 65536 };
        gpu_render_context_t* context = Gpu_RenderContext_Create( &params );
        assert( context );
        if ( argc > 1 ) {
            gpu_buffer_t* doomed = Gpu_BufferCreate();
            if ( strcmp( argv[ 1 ], "oversized" ) == 0 ) {
                Gpu_Buffer_Initialise( doomed, 65537, GPU_BUFFER_USAGE_GPU_ONLY );
            } else if ( strcmp( argv[ 1 ], "heap-limit" ) == 0 ) {
                for ( unsigned i = 0; i <= GPU_MAX_HEAPS; ++i ) {
                    gpu_buffer_t* entry = Gpu_BufferCreate();
                    Gpu_Buffer_Initialise( entry, 50000, GPU_BUFFER_USAGE_GPU_ONLY );
                }
            } else {
                Gpu_Buffer_Initialise( doomed, 16, GPU_BUFFER_USAGE_GPU_ONLY );
                Gpu_Buffer_Write( NULL, doomed, "z", 0, 1 );
                if ( strcmp( argv[ 1 ], "destroy-pending" ) == 0 ) {
                    Gpu_BufferDestroy( doomed );
                } else if ( strcmp( argv[ 1 ], "reinitialize-pending" ) == 0 ) {
                    Gpu_Buffer_Initialise( doomed, 16, GPU_BUFFER_USAGE_GPU_ONLY );
                } else if ( strcmp( argv[ 1 ], "queue-full" ) == 0 ) {
                    for ( unsigned i = 0; i < GPU_MAX_PENDING_WRITES; ++i ) {
                        Gpu_Buffer_Write( NULL, doomed, "z", 0, 1 );
                    }
                } else {
                    assert( false );
                }
            }

            assert( false );
        }

        gpu_buffer_t* large1 = Gpu_BufferCreate();
        gpu_buffer_t* large2 = Gpu_BufferCreate();
        Gpu_Buffer_Initialise( large1, 50000, GPU_BUFFER_USAGE_GPU_ONLY );
        Gpu_Buffer_Initialise( large2, 50000, GPU_BUFFER_USAGE_GPU_ONLY );
        assert( large1->buffer.heap && large2->buffer.heap && large1->buffer.heap != large2->buffer.heap );
        Gpu_BufferDestroy( large1 );
        Gpu_BufferDestroy( large2 );
        for ( unsigned i = 0; i < GPU_MAX_HEAPS; ++i ) {
            assert( !context->heaps[ i ].heap );
        }

        gpu_buffer_t* buffer = Gpu_BufferCreate();
        assert( buffer );
        for ( uint32_t usage = GPU_BUFFER_USAGE_STATIC; usage <= GPU_BUFFER_USAGE_STREAM; ++usage ) {
            Gpu_Buffer_Initialise( buffer, 16, usage );
            assert( buffer->buffer && buffer->buffer.storageMode == MTLStorageModeShared );
            assert( buffer->buffer.heap );
            assert( buffer->buffer.heap.hazardTrackingMode == MTLHazardTrackingModeTracked );
            const char input[] = "test";
            Gpu_Buffer_Write( NULL, buffer, input, 0, sizeof( input ) );
            Gpu_Buffer_Write( NULL, buffer, "XY", 1, 2 );
            void* mapped = NULL;
            Gpu_Buffer_Map( NULL, buffer, &mapped );
            assert( mapped && memcmp( mapped, "tXYt", sizeof( input ) ) == 0 );
            memcpy( mapped, "next", 5 );
            Gpu_Buffer_Unmap( NULL, buffer );
            Gpu_Buffer_Map( NULL, buffer, &mapped );
            assert( memcmp( mapped, "next", 5 ) == 0 );
            Gpu_Buffer_Unmap( NULL, buffer );
        }

        id<MTLBuffer> previous = buffer->buffer;
        Gpu_Buffer_Initialise( buffer, 16, 999 );
        assert( buffer->buffer == previous );
        Gpu_Buffer_Write( NULL, buffer, "x", 0, 17 );
        Gpu_Buffer_Write( NULL, buffer, "x", 16, 1 );
        Gpu_Buffer_Write( NULL, buffer, "x", UINT32_MAX, UINT32_MAX );
        Gpu_Buffer_Write( NULL, buffer, NULL, 16, 0 );
        Gpu_Buffer_Write( NULL, buffer, "Z", 15, 1 );
        void* mapped = NULL;
        Gpu_Buffer_Map( NULL, buffer, &mapped );
        assert( mapped && memcmp( mapped, "next", 5 ) == 0 );
        assert( ( ( char* )mapped )[ 15 ] == 'Z' );
        Gpu_Buffer_Unmap( NULL, buffer );

        Gpu_Buffer_Initialise( buffer, 16, GPU_BUFFER_USAGE_GPU_ONLY );
        assert( buffer->buffer && buffer->buffer.storageMode == MTLStorageModePrivate );
        assert( buffer->buffer.heap );
        mapped = buffer;
        Gpu_Buffer_Map( NULL, buffer, &mapped );
        assert( !mapped );
        char input[] = "private";
        Gpu_Buffer_Write( NULL, buffer, input, 0, 8 );
        memset( input, 'x', sizeof( input ) );
        Gpu_Buffer_Write( NULL, buffer, "abc", 1, 3 );
        Gpu_Buffer_Write( NULL, buffer, "Z", 7, 1 );
        Gpu_Buffer_Write( NULL, buffer, "Q", 15, 1 );
        Gpu_Buffer_Write( NULL, buffer, "x", 15, 2 );
        Gpu_Buffer_Write( NULL, buffer, "x", UINT32_MAX, UINT32_MAX );
        Gpu_Buffer_Write( NULL, buffer, NULL, 16, 0 );

        id<MTLDevice> device = buffer->buffer.device;
        id<MTLBuffer> readback = [device newBufferWithLength:16 options:MTLResourceStorageModeShared];
        assert( readback && context->writeCount == 4 && buffer->pendingWrites == 4 );
        Gpu_RenderContext_BeginFrame( context );
        assert( context->writeCount == 0 && buffer->pendingWrites == 0 );
        id<MTLCommandBuffer> command = context->currentCommandBuffer;
        id<MTLBlitCommandEncoder> blit = [command blitCommandEncoder];
        assert( command && blit );
        [blit copyFromBuffer:buffer->buffer sourceOffset:0 toBuffer:readback destinationOffset:0 size:16];
        [blit endEncoding];
        Gpu_BufferDestroy( buffer );
        Gpu_RenderContext_EndFrame( context );
        [command waitUntilCompleted];
        assert( command.status == MTLCommandBufferStatusCompleted );
        assert( memcmp( [readback contents], "pabcateZ", 8 ) == 0 );
        assert( ( ( const char* )[readback contents] )[ 15 ] == 'Q' );
#if !__has_feature( objc_arc )
        [readback release];
#endif
        gpu_buffer_t* concurrent = Gpu_BufferCreate();
        Gpu_Buffer_Initialise( concurrent, 128, GPU_BUFFER_USAGE_GPU_ONLY );
        dispatch_apply( 128, dispatch_get_global_queue( QOS_CLASS_DEFAULT, 0 ), ^( size_t i ) {
          @autoreleasepool {
              unsigned char value = ( unsigned char )i;
              Gpu_Buffer_Write( NULL, concurrent, &value, ( uint32_t )i, 1 );
          }
        } );
        assert( concurrent->pendingWrites == 128 && context->writeCount == 128 );
        Gpu_RenderContext_BeginFrame( context );
        id<MTLBuffer> concurrentReadback = [device newBufferWithLength:128 options:MTLResourceStorageModeShared];
        id<MTLBlitCommandEncoder> concurrentBlit = [context->currentCommandBuffer blitCommandEncoder];
        assert( concurrentReadback && concurrentBlit );
        [concurrentBlit copyFromBuffer:concurrent->buffer
                          sourceOffset:0
                              toBuffer:concurrentReadback
                     destinationOffset:0
                                  size:128];
        [concurrentBlit endEncoding];
        Gpu_BufferDestroy( concurrent );
        Gpu_RenderContext_EndFrame( context );
        dispatch_group_wait( context->submittedFrames, DISPATCH_TIME_FOREVER );
        const unsigned char* bytes = [concurrentReadback contents];
        for ( unsigned i = 0; i < 128; ++i ) {
            assert( bytes[ i ] == i );
        }

#if !__has_feature( objc_arc )
        [concurrentReadback release];
#endif
        gpu_buffer_t* pending = Gpu_BufferCreate();
        Gpu_Buffer_Initialise( pending, 16, GPU_BUFFER_USAGE_GPU_ONLY );
        Gpu_Buffer_Write( NULL, pending, "z", 0, 1 );
        Gpu_RenderContext_Destroy( context );
        assert( gpu.currentContext == context && pending->pendingWrites == 0 && context->writeCount == 0 );
        Gpu_BufferDestroy( pending );
        Gpu_RenderContext_Destroy( context );
        Gpu_Finalise();
        Gpu_Initialise();
        assert( gpu_initialized );
        Gpu_Finalise();
    }

    return 0;
}
