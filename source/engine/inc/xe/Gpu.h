/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __GPU_H__
#define __GPU_H__

#include "Platform.h"
#include "Gpu_types.h"

X_API void Gpu_Initialise( void );
X_API void Gpu_Finalise( void );

X_API gpu_render_context_t* Gpu_RenderContextCreate( const gpu_render_context_params_t* params );
X_API void Gpu_ContextDestroy( gpu_render_context_t* context );

X_API gpu_buffer_t* Gpu_BufferCreate( void );
X_API void Gpu_BufferDestroy( gpu_buffer_t* buffer );

X_API gpu_texture_t* Gpu_TextureCreate( void );
X_API void Gpu_TextureDestroy( gpu_texture_t* texture );

X_API gpu_renderpass_t* Gpu_RenderpassCreate( void );
X_API void Gpu_RenderpassDestroy( gpu_renderpass_t* renderpass );

/* Buffer operations */
/* STATIC, DYNAMIC and STREAM buffers are CPU-visible. GPU_ONLY buffers are private and cannot be mapped.
 * Writes begin at the specified byte offset; GPU_ONLY writes use a synchronous staging upload.
 * The offset and size must fit within the buffer; a zero-size write is allowed at its end.
 * The device argument is reserved; the Metal backend uses the device selected by Gpu_Initialise.
 * Callers must synchronize access with GPU work and unmap before writing or reinitializing.
 * Errors are reported to stderr; a failed map sets *data to NULL.
 */
X_API void Gpu_Buffer_Initialise( gpu_buffer_t* buffer, uint32_t size, uint32_t usage );
X_API void Gpu_Buffer_Write( gpu_device_t* device, gpu_buffer_t* buffer, const void* data, uint32_t offset,
                             uint32_t size );
X_API void Gpu_Buffer_Map( gpu_device_t* device, gpu_buffer_t* buffer, void** data );
X_API void Gpu_Buffer_Unmap( gpu_device_t* device, gpu_buffer_t* buffer );

/* Texture operations */
X_API void Gpu_Texture_Initialise( gpu_texture_t* texture, uint32_t width, uint32_t height, uint32_t mipCount,
                                   uint32_t format, uint32_t usage );
X_API void Gpu_Texture_Write( gpu_device_t* device, gpu_texture_t* texture, uint32_t mip, const void* data );

#endif // __GPU_H__
