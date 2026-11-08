/*
 * Run from the repository root:
 * clang -Wall -Wextra -Werror -Wno-unused-const-variable -I source/engine/inc \
 *     source/engine/tests/Gpu_metal_buffer_test.m source/engine/src/Mem.c -framework Metal -framework Foundation \
 *     -o /tmp/xengine-metal-buffer-test && /tmp/xengine-metal-buffer-test
 */
#include <assert.h>
#include <string.h>
#include "../src/Gpu_metal.m"

int main( void ) {
    @autoreleasepool {
        Gpu_Initialise();
        if ( !gpu_initialized ) {
            fprintf( stderr, "Metal device required for buffer tests\n" );
            return 1;
        }

        gpu_buffer_t* buffer = Gpu_BufferCreate();
        assert( buffer );
        for ( uint32_t usage = GPU_BUFFER_USAGE_STATIC; usage <= GPU_BUFFER_USAGE_STREAM; ++usage ) {
            Gpu_Buffer_Initialise( buffer, 16, usage );
            assert( buffer->buffer && buffer->buffer.storageMode == MTLStorageModeShared );
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
        mapped = buffer;
        Gpu_Buffer_Map( NULL, buffer, &mapped );
        assert( !mapped );
        Gpu_Buffer_Write( NULL, buffer, "private", 0, 8 );
        Gpu_Buffer_Write( NULL, buffer, "abc", 1, 3 );
        Gpu_Buffer_Write( NULL, buffer, "Z", 7, 1 );
        Gpu_Buffer_Write( NULL, buffer, "Q", 15, 1 );
        Gpu_Buffer_Write( NULL, buffer, "x", 15, 2 );
        Gpu_Buffer_Write( NULL, buffer, "x", UINT32_MAX, UINT32_MAX );
        Gpu_Buffer_Write( NULL, buffer, NULL, 16, 0 );

        id<MTLDevice> device = buffer->buffer.device;
        id<MTLBuffer> readback = [device newBufferWithLength:16 options:MTLResourceStorageModeShared];
        id<MTLCommandQueue> queue = [device newCommandQueue];
        assert( readback && queue );
        id<MTLCommandBuffer> command = [queue commandBuffer];
        id<MTLBlitCommandEncoder> blit = [command blitCommandEncoder];
        assert( command && blit );
        [blit copyFromBuffer:buffer->buffer sourceOffset:0 toBuffer:readback destinationOffset:0 size:16];
        [blit endEncoding];
        [command commit];
        [command waitUntilCompleted];
        assert( command.status == MTLCommandBufferStatusCompleted );
        assert( memcmp( [readback contents], "pabcateZ", 8 ) == 0 );
        assert( ( ( const char* )[readback contents] )[ 15 ] == 'Q' );
#if !__has_feature( objc_arc )
        [queue release];
        [readback release];
#endif
        Gpu_BufferDestroy( buffer );
        Gpu_Finalise();
        Gpu_Initialise();
        assert( gpu_initialized );
        Gpu_Finalise();
    }

    return 0;
}
