/*
 * Run from the repository root:
 * clang -Wall -Wextra -Werror -I source/engine/inc source/engine/tests/Gpu_metal_formats_test.m source/engine/src/Mem.c
 * \ -framework Metal -framework Foundation -o /tmp/xengine-metal-formats-test && /tmp/xengine-metal-formats-test
 */
#include <assert.h>
#include "../src/metal/Gpu_metal.m"

int main( void ) {
    const MTLPixelFormat expected[] = { MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatRGBA8Uint,
                                        MTLPixelFormatRGBA8Sint,    MTLPixelFormatRGBA8Unorm,
                                        MTLPixelFormatRGBA8Snorm,   MTLPixelFormatRGBA16Uint,
                                        MTLPixelFormatRGBA16Sint,   MTLPixelFormatRGBA16Unorm,
                                        MTLPixelFormatRGBA16Snorm,  MTLPixelFormatRGBA32Uint,
                                        MTLPixelFormatRGBA32Sint,   MTLPixelFormatInvalid,
                                        MTLPixelFormatInvalid,      MTLPixelFormatInvalid,
                                        MTLPixelFormatRGBA32Float,  MTLPixelFormatDepth24Unorm_Stencil8,
                                        MTLPixelFormatDepth32Float, MTLPixelFormatInvalid,
                                        MTLPixelFormatBC1_RGBA,     MTLPixelFormatBC2_RGBA,
                                        MTLPixelFormatBC3_RGBA,     MTLPixelFormatBC4_RUnorm,
                                        MTLPixelFormatBC5_RGUnorm,  MTLPixelFormatBC6H_RGBUfloat,
                                        MTLPixelFormatBC7_RGBAUnorm };
    const size_t count = sizeof( texture_format_info ) / sizeof( texture_format_info[ 0 ] );
    assert( count == GPU_TEXTURE_FORMAT_BC7_RGBA + 1 );
    assert( count == sizeof( expected ) / sizeof( expected[ 0 ] ) );

    for ( size_t i = 0; i < count; ++i ) {
        const texture_format_info_t* info = &texture_format_info[ i ];
        assert( info->format == i );
        assert( info->pixelFormat == expected[ i ] );
        if ( i >= GPU_TEXTURE_FORMAT_BC1_RGB ) {
            assert( info->componentSize == -1 );
            assert( info->blockWidth == 4 && info->blockHeight == 4 );
            assert( info->blockSize == ( i == GPU_TEXTURE_FORMAT_BC1_RGB || i == GPU_TEXTURE_FORMAT_BC1_RGBA ||
                                                 i == GPU_TEXTURE_FORMAT_BC4_R
                                             ? 8
                                             : 16 ) );
        } else {
            assert( info->blockSize == -1 && info->blockWidth == -1 && info->blockHeight == -1 );
        }
    }

    assert( texture_format_info[ GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8 ].componentCount == 2 );
    assert( texture_format_info[ GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8 ].componentSize == -1 );
    return 0;
}
