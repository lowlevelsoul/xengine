/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef XE_RB_H
#define XE_RB_H

#include "xe/Platform.h"
#include "xe/RenderTypes.h"
#include "xe/MathLinear.h"

/* A command to write data from one GPU buffer to another within a batch chain The

*/
typedef struct batch_cmd_buffer_write_s {
    render_gpu_buffer_t* srcbuffer;        /* Source buffer - can be cpu or gpu*/
    render_gpu_buffer_t* dstbuffer;        /* Destination buffer - must be GPU */
    size_t offset;                         /* Offset into the destination buffer */
    bool canDiscardSrc;                    /* Indicates if the source buffer can be discarded after the write */
    struct batch_cmd_buffer_write_s* next; /* Pointer to the next buffer write command in the batch chain */
} batch_cmd_buffer_write_t;

/* Defines a batch command to some static geometry that is to be transfomed into the transform buffers*/
typedef struct batch_cmd_static_transform_s {
    render_gpu_buffer_t* vertex_buffer;
    render_gpu_buffer_t* index_buffer;
    render_draw_surface_t* surfaces;
    mat4_t modelViewTransform;
    uint32_t surfaceCount;
    struct batch_cmd_static_transform_s* next;
} batch_cmd_static_transform_t;

/* A model that we want to draw. The surfaces point to the vertex and index data that has been transformed into clip &
 * view space */
typedef struct batch_cmd_draw_model_s {
    render_draw_surface_t* surfaces;
    uint32_t surfaceCount;
    struct batch_cmd_draw_model_s* next;
} batch_cmd_draw_model_t;

/* A 3D scene containing static geometry and models to be drawn */
typedef struct batch_cmd_scene3d_s {
    mat4_t viewMatrix;
    mat4_t projectionMatrix;
    mat4_t viewMatrixInv;
    float viewport[ 4 ];

    vec3_t globalLightDirection;
    vec3_t globalLightColor;
    vec3_t globalLightAmbient;

    batch_cmd_static_transform_t* transformListHead;
    batch_cmd_static_transform_t* transformListTail;

    batch_cmd_draw_model_t* opaqueDrawModelListHead;
    batch_cmd_draw_model_t* opaqueDrawModelListTail;

    batch_cmd_buffer_write_t* bufferWriteListHead;
    batch_cmd_buffer_write_t* bufferWriteListTail;

    struct batch_cmd_scene3d_s* next;
} batch_cmd_scene3d_t;

typedef struct batch_cmd_scene_s {
    batch_cmd_scene3d_t* scene3dListHead;
    batch_cmd_scene3d_t* scene3dListTail;
} batch_cmd_scene_t;

/* The instance information for the batcher code*/
typedef struct render_batch_s render_batch_t;

render_batch_t* RB_Create( size_t size );
void RB_Destroy( render_batch_t* self_ );
void RB_Reset( render_batch_t* self_ );

batch_cmd_scene_t* RB_BeginScene( render_batch_t* self_ );
void RB_EndScene( render_batch_t* self_ );
void RB_BeginScene3d( render_batch_t* self_, const mat4_t* viewMatrix, const mat4_t* projectionMatrix,
                      const float* viewport );
void RB_EndScene3d( render_batch_t* self_ );
void RB_SubmitModel( render_batch_t* self_, render_model_t* model, const mat4_t* transform );
void RB_SetGlobalLight( render_batch_t* self_, const vec3_t* direction, const vec3_t* color, const vec3_t* ambient );

/* Copies the entire source to the GPU-only destination at offset. Both buffers must remain
   valid until GPU completion. When canDiscardSrc is true, the renderer takes ownership
   after submission; the source must not be used or destroyed by the caller afterward. */
void RB_QueueBufferWrite( render_batch_t* self_, render_gpu_buffer_t* srcBuffer, render_gpu_buffer_t* dstBuffer,
                          size_t offset, bool canDiscardSrc );

#endif // XE_RB_H