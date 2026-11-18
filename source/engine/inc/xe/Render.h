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


void Render_Create( render_params_t* params );
void Render_Destroy( void );

render_window_t* Render_GetWindow( void );
bool RenderWindow_PumpEvents( render_window_t* window );


/* Rendering */

void Render_BeginFrame( void );
void Render_Begin3d( const mat4_t* projMat, const mat4_t* viewMat, const float * viewport );
void Render_SubmitModel( const render_model_t* model, const render_material_t* materials, const mat4_t* modelMat );
void Render_SumbitGlobalLight( const vec3_t* direction, const vec3_t* color, const vec3_t* ambient );
void Render_End3d( const mat4_t* projMat, const mat4_t* viewMat, const float * viewport );
void Render_EndFrame( void );

/* GPU Buffer */
render_gpu_buffer_t * Render_GpuBufferCreate( size_t size, uint64_t usage );
void Render_GpuBufferDestroy( render_gpu_buffer_t* buffer );
void Render_GpuBuffer_QueueWrite( render_gpu_buffer_t* buffer, render_gpu_buffer_t* srcBuffer, size_t offset );
void * Render_GpuBuffer_Map( render_gpu_buffer_t* buffer );
void Render_GpuBuffer_Unmap( render_gpu_buffer_t* buffer );

#endif // __RENDER_H__