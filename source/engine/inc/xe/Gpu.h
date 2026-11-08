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

X_API gpu_render_context_t* Gpu_RenderContext_Create( const gpu_render_context_params_t* params );
X_API void Gpu_RenderContext_Destroy( gpu_render_context_t* context );

X_API gpu_buffer_t* Gpu_BufferCreate( void );
X_API void Gpu_BufferDestroy( gpu_buffer_t* buffer );

X_API gpu_texture_t* Gpu_TextureCreate( void );
X_API void Gpu_TextureDestroy( gpu_texture_t* texture );

X_API gpu_renderpass_t* Gpu_RenderpassCreate( void );
X_API void Gpu_RenderpassDestroy( gpu_renderpass_t* renderpass );

/* Buffer initialization requires a live render context. Shared writes are immediate; GPU-only writes are copied
 * into staging storage and encoded at the next BeginFrame. The caller must synchronize GPU access. A buffer with
 * pending writes cannot be destroyed or reinitialized. Context destruction discards unsubmitted writes, but leaves
 * the context alive if initialized buffers remain; destroy those buffers and retry. Callers must serialize context
 * and buffer destruction against other operations that use those objects.
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

X_API void Gpu_RenderContext_BeginFrame( gpu_render_context_t* context );
X_API void Gpu_RenderContext_EndFrame( gpu_render_context_t* context );

#endif // __GPU_H__
