/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#import "xe/Gpu.h"
#import "xe/Mem.h"
#import "xe/Str.h"
#import <Metal/Metal.h>
#include <stdio.h>

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
} gpu_buffer_t;

typedef struct gpu_texture_s {
    id<MTLTexture> texture;
    uint32_t width;
    uint32_t height;
    uint32_t mipCount;
    gpu_texture_format_t format;
    const texture_format_info_t* formatInfo;
} gpu_texture_t;

typedef struct gpu_render_context_s {
    /* TODO - add stuff here */
} gpu_render_context_t;

typedef struct gpu_s {
    id<MTLDevice> device;
} gpu_t;




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

static gpu_t gpu;
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

#if !__has_feature( objc_arc )
    [gpu.device release];
#endif
    gpu.device = nil;
    gpu_initialized = false;
}

/*==========================================================================================================*/
X_API gpu_render_context_t* Gpu_RenderContextCreate( const gpu_context_params_t* params ) {
    
}

/*==========================================================================================================*/
X_API void Gpu_ContextDestroy( gpu_render_context_t * context ) {
    
}

/*==========================================================================================================*/
X_API gpu_buffer_t* Gpu_BufferCreate( void ) {
    gpu_buffer_t* buffer = Mem_Alloc( sizeof( *buffer ) );
    if ( !buffer ) {
        fprintf( stderr, "Gpu_BufferCreate: allocation failed\n" );
        return NULL;
    }

    Mem_Zero( buffer, sizeof( *buffer ) );
    return buffer;
}

/*==========================================================================================================*/
X_API void Gpu_BufferDestroy( gpu_buffer_t* buffer ) {
    if ( !buffer ) {
        return;
    }

#if !__has_feature( objc_arc )
    [buffer->buffer release];
#endif
    buffer->buffer = nil;
    Mem_Free( buffer );
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Initialise( gpu_buffer_t* buffer, uint32_t size, uint32_t usage ) {
    if ( !buffer || !gpu_initialized || !size || usage > GPU_BUFFER_USAGE_GPU_ONLY || buffer->mapped ) {
        fprintf( stderr, "Gpu_Buffer_Initialise: invalid buffer, device, size, usage, or mapped state\n" );
        return;
    }

    MTLResourceOptions options =
        usage == GPU_BUFFER_USAGE_GPU_ONLY ? MTLResourceStorageModePrivate : MTLResourceStorageModeShared;
    id<MTLBuffer> metalBuffer = [gpu.device newBufferWithLength:size options:options];
    if ( !metalBuffer ) {
        fprintf( stderr, "Gpu_Buffer_Initialise: Metal buffer allocation failed\n" );
        return;
    }

#if !__has_feature( objc_arc )
    [buffer->buffer release];
#endif
    buffer->buffer = metalBuffer;
    buffer->size = size;
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Write( gpu_device_t* device, gpu_buffer_t* buffer, const void* data, uint32_t offset,
                             uint32_t size ) {
    ( void )device;
    if ( !buffer || !buffer->buffer || buffer->mapped || offset > buffer->size || size > buffer->size - offset ||
         ( size && !data ) ) {
        fprintf( stderr, "Gpu_Buffer_Write: invalid buffer, data, range, or mapped state\n" );
        return;
    }

    if ( !size ) {
        return;
    }

    if ( buffer->buffer.storageMode != MTLStorageModePrivate ) {
        void* contents = [buffer->buffer contents];
        if ( !contents ) {
            fprintf( stderr, "Gpu_Buffer_Write: Metal buffer has no CPU-accessible contents\n" );
            return;
        }

        memcpy( ( char* )contents + offset, data, size );
        return;
    }

    @autoreleasepool {
        id<MTLDevice> metalDevice = buffer->buffer.device;
        id<MTLBuffer> staging = [metalDevice newBufferWithBytes:data length:size options:MTLResourceStorageModeShared];
        if ( !staging ) {
            fprintf( stderr, "Gpu_Buffer_Write: staging buffer allocation failed\n" );
            return;
        }

        id<MTLCommandQueue> queue = [metalDevice newCommandQueue];
        id<MTLCommandBuffer> command = [queue commandBuffer];
        id<MTLBlitCommandEncoder> blit = [command blitCommandEncoder];
        if ( !queue || !command || !blit ) {
            fprintf( stderr, "Gpu_Buffer_Write: failed to create Metal upload commands\n" );
#if !__has_feature( objc_arc )
            [queue release];
            [staging release];
#endif
            return;
        }

        [blit copyFromBuffer:staging sourceOffset:0 toBuffer:buffer->buffer destinationOffset:offset size:size];
        [blit endEncoding];
        [command commit];
        [command waitUntilCompleted];
        if ( command.status != MTLCommandBufferStatusCompleted ) {
            fprintf( stderr, "Gpu_Buffer_Write: Metal upload failed: %s\n",
                     command.error ? command.error.localizedDescription.UTF8String : "unknown error" );
        }

#if !__has_feature( objc_arc )
        [queue release];
        [staging release];
#endif
    }
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Map( gpu_device_t* device, gpu_buffer_t* buffer, void** data ) {
    ( void )device;
    if ( !data ) {
        fprintf( stderr, "Gpu_Buffer_Map: output pointer is NULL\n" );
        return;
    }

    *data = NULL;
    if ( !buffer || !buffer->buffer || buffer->mapped || buffer->buffer.storageMode == MTLStorageModePrivate ) {
        fprintf( stderr, "Gpu_Buffer_Map: buffer is invalid, already mapped, or GPU-only\n" );
        return;
    }

    *data = [buffer->buffer contents];
    if ( !*data ) {
        fprintf( stderr, "Gpu_Buffer_Map: Metal buffer has no CPU-accessible contents\n" );
        return;
    }

    buffer->mapped = true;
}

/*==========================================================================================================*/
X_API void Gpu_Buffer_Unmap( gpu_device_t* device, gpu_buffer_t* buffer ) {
    ( void )device;
    if ( !buffer || !buffer->buffer || !buffer->mapped ) {
        fprintf( stderr, "Gpu_Buffer_Unmap: buffer is not mapped\n" );
        return;
    }

    buffer->mapped = false;
}
