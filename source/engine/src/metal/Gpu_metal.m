/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "xe/Gpu.h"
#import "xe/Mem.h"
#import "xe/Str.h"
#import "Gpu_metal.h"

#include <stdio.h>

static const texture_format_info_t texture_format_info[] = {
    /* Metal's three-component formats are restricted to texture buffers, not ordinary 2D textures. */
    { GPU_TEXTURE_FORMAT_NONE, MTLPixelFormatInvalid, 0, -1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_U8, MTLPixelFormatInvalid, 3, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_S8, MTLPixelFormatInvalid, 3, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_U8_NORM, MTLPixelFormatInvalid, 3, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_S8_NORM, MTLPixelFormatInvalid, 3, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_U16, MTLPixelFormatInvalid, 3, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_S16, MTLPixelFormatInvalid, 3, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_U16_NORM, MTLPixelFormatInvalid, 3, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_S16_NORM, MTLPixelFormatInvalid, 3, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_U32, MTLPixelFormatInvalid, 3, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_S32, MTLPixelFormatInvalid, 3, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_U32_NORM, MTLPixelFormatInvalid, 3, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_S32_NORM, MTLPixelFormatInvalid, 3, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_U8, MTLPixelFormatRGBA8Uint, 4, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_S8, MTLPixelFormatRGBA8Sint, 4, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_U8_NORM, MTLPixelFormatRGBA8Unorm, 4, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_S8_NORM, MTLPixelFormatRGBA8Snorm, 4, 1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_U16, MTLPixelFormatRGBA16Uint, 4, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_S16, MTLPixelFormatRGBA16Sint, 4, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_U16_NORM, MTLPixelFormatRGBA16Unorm, 4, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_S16_NORM, MTLPixelFormatRGBA16Snorm, 4, 2, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_U32, MTLPixelFormatRGBA32Uint, 4, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_S32, MTLPixelFormatRGBA32Sint, 4, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_U32_NORM, MTLPixelFormatInvalid, 4, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_S32_NORM, MTLPixelFormatInvalid, 4, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGB_F32, MTLPixelFormatInvalid, 3, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_RGBA_F32, MTLPixelFormatRGBA32Float, 4, 4, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8, MTLPixelFormatDepth24Unorm_Stencil8, 2, -1, -1, -1, -1 },
    { GPU_TEXTURE_FORMAT_DEPTH32, MTLPixelFormatDepth32Float, 1, 4, -1, -1, -1 },
    /* BC1 RGB requires opaque-alpha decoding, which Metal's BC1 RGBA format does not guarantee. */
    { GPU_TEXTURE_FORMAT_BC1_RGB, MTLPixelFormatInvalid, 3, -1, 8, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC1_RGBA, MTLPixelFormatBC1_RGBA, 4, -1, 8, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC2_RGBA, MTLPixelFormatBC2_RGBA, 4, -1, 16, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC3_RGBA, MTLPixelFormatBC3_RGBA, 4, -1, 16, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC4_R, MTLPixelFormatBC4_RUnorm, 1, -1, 8, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC5_RG, MTLPixelFormatBC5_RGUnorm, 2, -1, 16, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC6H_RGB, MTLPixelFormatBC6H_RGBUfloat, 3, -1, 16, 4, 4 },
    { GPU_TEXTURE_FORMAT_BC7_RGBA, MTLPixelFormatBC7_RGBAUnorm, 4, -1, 16, 4, 4 },
};

gpu_t gpu;
static bool gpu_initialized = false;

/*==========================================================================================================*/
X_API void Gpu_Initialise() {
    if ( gpu_initialized ) {
        return;
    }

    gpu.device = MTLCreateSystemDefaultDevice();
    if ( !gpu.device ) {
        fprintf( stderr, "Gpu_Initialise: no Metal device is available\n" );
        return;
    }

    gpu_initialized = true;
}

/*==========================================================================================================*/
X_API void Gpu_Finalise() {
    if ( !gpu_initialized ) {
        return;
    }

    if ( gpu.currentContext ) {
        fprintf( stderr, "Gpu_Finalise: destroy the render context first\n" );
        return;
    }

#if !__has_feature( objc_arc )
    [gpu.device release];
#endif
    gpu.device = nil;
    gpu_initialized = false;
}
