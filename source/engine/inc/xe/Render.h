/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __RENDER_H__
#define __RENDER_H__

#include "xe/RenderTypes.h"

typedef struct mat4_s mat4_t;
typedef struct vec3_s vec3_t;
typedef struct vec4_s vec4_t;
typedef struct render_model_s render_model_t;
typedef struct render_material_s render_material_t;
typedef struct batch_cmd_scene_s batch_cmd_scene_t;

X_API void RenderParams_Initialise( render_params_t* params );
X_API void Render_Initialise( render_params_t* params );
X_API void Render_Finalise( void );
X_API bool RenderWindow_PumpEvents( render_window_t* window );

/* Render Window */
X_API render_window_t* Render_GetWindow( void );
X_API void * RenderWindow_GetNativeHandle( void );

/* Rendering */
X_API void Render_SubmitScene( batch_cmd_scene_t* scene );

/* Model */
X_API void RenderModel_Create( render_model_t * self, uint32_t vertexCount, uint32_t indexCount, uint32_t surfaceCount, uint32_t weightCount );
X_API void RenderModel_Destroy( render_model_t * self_ );


/* GPU Buffer */
X_API render_gpu_buffer_t* Render_GpuBufferCreate( size_t size, uint64_t usage );
X_API void Render_GpuBufferDestroy( render_gpu_buffer_t* buffer );
X_API void Render_GpuBuffer_QueueWrite( render_gpu_buffer_t* buffer, render_gpu_buffer_t* srcBuffer, size_t offset,
                                  bool releaseSrcBuffer );
X_API void* Render_GpuBuffer_Map( render_gpu_buffer_t* buffer );
X_API void Render_GpuBuffer_Unmap( render_gpu_buffer_t* buffer );



#endif // __RENDER_H__
