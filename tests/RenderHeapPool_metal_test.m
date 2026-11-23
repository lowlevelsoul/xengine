#import "../source/engine/src/metal/RenderHeapPool_metal.h"
#import "../source/engine/src/metal/Render_metal.h"
#include <assert.h>
#include <stdio.h>

render_metal_t render;

int main( void ) {
    @autoreleasepool {
        render.device = MTLCreateSystemDefaultDevice();
        if ( !render.device ) {
            fprintf( stderr, "Metal device unavailable\n" );
            return 1;
        }

        heap_pool_t first = { 0 };
        heap_pool_t second = { 0 };
        RenderHeapPool_Initialise( &first, 1024 * 1024, MTLStorageModePrivate, MTLCPUCacheModeDefaultCache,
                                   MTLHazardTrackingModeUntracked );
        RenderHeapPool_Initialise( &second, 2 * 1024 * 1024, MTLStorageModeShared, MTLCPUCacheModeWriteCombined,
                                   MTLHazardTrackingModeTracked );

        id<MTLBuffer> firstBuffer = RenderHeapPool_CreateBuffer(
            &first, 4096, MTLResourceStorageModePrivate | MTLResourceHazardTrackingModeUntracked );
        id<MTLBuffer> secondBuffer =
            RenderHeapPool_CreateBuffer( &second, 4096,
                                         MTLResourceStorageModeShared | MTLResourceCPUCacheModeWriteCombined |
                                             MTLResourceHazardTrackingModeTracked );
        assert( firstBuffer && secondBuffer );
        assert( first.numHeaps == 1 && second.numHeaps == 1 );
        assert( RenderHeapPool_GetHeap( &first ) != RenderHeapPool_GetHeap( &second ) );
        assert( first.heaps[ 0 ].storageMode == MTLStorageModePrivate );
        assert( first.heaps[ 0 ].cpuCacheMode == MTLCPUCacheModeDefaultCache );
        assert( first.heaps[ 0 ].hazardTrackingMode == MTLHazardTrackingModeUntracked );
        assert( second.heaps[ 0 ].storageMode == MTLStorageModeShared );
        assert( second.heaps[ 0 ].cpuCacheMode == MTLCPUCacheModeWriteCombined );
        assert( second.heaps[ 0 ].hazardTrackingMode == MTLHazardTrackingModeTracked );

        MTLTextureDescriptor* descriptor =
            [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                               width:16
                                                              height:16
                                                           mipmapped:NO];
        descriptor.storageMode = MTLStorageModePrivate;
        descriptor.hazardTrackingMode = MTLHazardTrackingModeUntracked;
        id<MTLTexture> texture = RenderHeapPool_CreateTexture( &first, descriptor );
        assert( texture && texture.heap == first.heaps[ 0 ] );

        RenderHeapPool_Finalise( &first );
        assert( !first.isCreated && first.numHeaps == 0 );
        assert( second.isCreated && second.numHeaps == 1 );
        assert( RenderHeapPool_CreateBuffer( &second, 4096,
                                             MTLResourceStorageModeShared | MTLResourceCPUCacheModeWriteCombined |
                                                 MTLResourceHazardTrackingModeTracked ) );
        assert( second.numHeaps == 1 );

        RenderHeapPool_Initialise( &first, 1024 * 1024, MTLStorageModeShared, MTLCPUCacheModeDefaultCache,
                                   MTLHazardTrackingModeTracked );
        assert( RenderHeapPool_CreateBuffer( &first, 4096,
                                             MTLResourceStorageModeShared | MTLResourceHazardTrackingModeTracked ) );
        assert( first.heaps[ 0 ].storageMode == MTLStorageModeShared );
        assert( first.heaps[ 0 ].hazardTrackingMode == MTLHazardTrackingModeTracked );
        RenderHeapPool_Finalise( &first );
        RenderHeapPool_Finalise( &second );
    }

    return 0;
}
