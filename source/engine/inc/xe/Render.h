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


void Render_Initialise( render_params_t* params );
void Render_Finalise( void );
bool RenderWindow_PumpEvents( render_window_t* window );

/* Render Window */
render_window_t* Render_GetWindow( void );

/* Rendering */
void Render_SubmitScene( batch_cmd_scene_t * scene );

/* GPU Buffer */
render_gpu_buffer_t * Render_GpuBufferCreate( size_t size, uint64_t usage );
void Render_GpuBufferDestroy( render_gpu_buffer_t* buffer );
void Render_GpuBuffer_QueueWrite( render_gpu_buffer_t* buffer, render_gpu_buffer_t* srcBuffer, size_t offset );
void * Render_GpuBuffer_Map( render_gpu_buffer_t* buffer );
void Render_GpuBuffer_Unmap( render_gpu_buffer_t* buffer );

#endif // __RENDER_H__