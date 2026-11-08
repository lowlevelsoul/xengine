/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __GPU_TYPES_H__
#define __GPU_TYPES_H__

#include "Platform.h"

#define GPU_MAX_NUM_VERTEX_ATTRIBUTES 16
#define GPU_MAX_NUM_VERTEX_STREAM 8

typedef struct gpu_device_s gpu_device_t;
typedef struct gpu_render_context_s gpu_render_context_t;
typedef struct gpu_buffer_s gpu_buffer_t;
typedef struct gpu_texture_s gpu_texture_t;
typedef struct gpu_renderpass_s gpu_renderpass_t;

typedef enum gpu_buffer_usage_e {
    GPU_BUFFER_USAGE_STATIC,
    GPU_BUFFER_USAGE_DYNAMIC,
    GPU_BUFFER_USAGE_STREAM,
    GPU_BUFFER_USAGE_GPU_ONLY
} gpu_buffer_usage_t;

typedef enum gpu_texture_usage_e {
    GPU_TEXTURE_USAGE_STATIC,
    GPU_TEXTURE_USAGE_DYNAMIC,
    GPU_TEXTURE_USAGE_STREAM
} gpu_texture_usage_t;

typedef enum gpu_index_type_e {
    GPU_INDEX_TYPE_UINT16,
    GPU_INDEX_TYPE_UINT32
} gpu_index_type_t;

typedef enum gpu_primitive_type_e {
    GPU_PRIMITIVE_TYPE_POINTS,
    GPU_PRIMITIVE_TYPE_LINES,
    GPU_PRIMITIVE_TYPE_LINE_STRIP,
    GPU_PRIMITIVE_TYPE_TRIANGLES,
    GPU_PRIMITIVE_TYPE_TRIANGLE_STRIP
} gpu_primitive_type_t;

typedef enum gpu_shader_stage_e {
    GPU_SHADER_STAGE_VERTEX,
    GPU_SHADER_STAGE_FRAGMENT,
    GPU_SHADER_STAGE_COMPUTE
} gpu_shader_stage_t;

typedef enum gpu_winding_e {
    GPU_WINDING_CLOCKWISE,
    GPU_WINDING_COUNTER_CLOCKWISE
} gpu_winding_t;

typedef enum gpu_cull_mode_e {
    GPU_CULL_MODE_NONE,
    GPU_CULL_MODE_FRONT,
    GPU_CULL_MODE_BACK
} gpu_cull_mode_t;

typedef enum gpu_vertex_attribute_e {
    GPU_VERTEX_ATTRIBUTE_NIL,
    GPU_VERTEX_ATTRIBUTE_FLOAT,
    GPU_VERTEX_ATTRIBUTE_INT16,
    GPU_VERTEX_ATTRIBUTE_INT32,
    GPU_VERTEX_ATTRIBUTE_INT64,
} gpu_vertex_attribute_t;

typedef enum gpu_texture_format_e {
    GPU_TEXTURE_FORMAT_NONE = 0,

    GPU_TEXTURE_FORMAT_RGB_U8,
    GPU_TEXTURE_FORMAT_RGB_S8,
    GPU_TEXTURE_FORMAT_RGB_U8_NORM,
    GPU_TEXTURE_FORMAT_RGB_S8_NORM,

    GPU_TEXTURE_FORMAT_RGB_U16,
    GPU_TEXTURE_FORMAT_RGB_S16,
    GPU_TEXTURE_FORMAT_RGB_U16_NORM,
    GPU_TEXTURE_FORMAT_RGB_S16_NORM,

    GPU_TEXTURE_FORMAT_RGB_U32,
    GPU_TEXTURE_FORMAT_RGB_S32,
    GPU_TEXTURE_FORMAT_RGB_U32_NORM,
    GPU_TEXTURE_FORMAT_RGB_S32_NORM,

    GPU_TEXTURE_FORMAT_RGBA_U8,
    GPU_TEXTURE_FORMAT_RGBA_S8,
    GPU_TEXTURE_FORMAT_RGBA_U8_NORM,
    GPU_TEXTURE_FORMAT_RGBA_S8_NORM,

    GPU_TEXTURE_FORMAT_RGBA_U16,
    GPU_TEXTURE_FORMAT_RGBA_S16,
    GPU_TEXTURE_FORMAT_RGBA_U16_NORM,
    GPU_TEXTURE_FORMAT_RGBA_S16_NORM,

    GPU_TEXTURE_FORMAT_RGBA_U32,
    GPU_TEXTURE_FORMAT_RGBA_S32,
    GPU_TEXTURE_FORMAT_RGBA_U32_NORM,
    GPU_TEXTURE_FORMAT_RGBA_S32_NORM,

    GPU_TEXTURE_FORMAT_RGB_F32,
    GPU_TEXTURE_FORMAT_RGBA_F32,

    GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8,
    GPU_TEXTURE_FORMAT_DEPTH32,

    /* Block compressed formats*/
    GPU_TEXTURE_FORMAT_BC1_RGB,
    GPU_TEXTURE_FORMAT_BC1_RGBA,
    GPU_TEXTURE_FORMAT_BC2_RGBA,
    GPU_TEXTURE_FORMAT_BC3_RGBA,
    GPU_TEXTURE_FORMAT_BC4_R,
    GPU_TEXTURE_FORMAT_BC5_RG,
    GPU_TEXTURE_FORMAT_BC6H_RGB,
    GPU_TEXTURE_FORMAT_BC7_RGBA,
} gpu_texture_format_t;

typedef struct gpu_vertex_element {
    gpu_vertex_attribute_t type; /* Primitive type of the vertex attribute */
    uint32_t offset;             /* Offset in bytes from the start of the vertex */
    uint32_t dim;                /* Number of components */
} gpu_vertex_element_t;

typedef struct gpu_vertex_layout {
    gpu_vertex_element_t elements[ GPU_MAX_NUM_VERTEX_ATTRIBUTES ];
    uint32_t count;
    uint32_t stride; /* Stride in bytes between consecutive vertices */
} gpu_vertex_layout_t;

typedef struct gpu_vertex_stream_layout_s {
    gpu_vertex_layout_t vertexLayouts[ GPU_MAX_NUM_VERTEX_STREAM ];
    uint32_t count;
} gpu_vertex_stream_layout_t;

/* Gpu context params */
typedef struct gpu_render_context_params_s {
    uint32_t width;
    uint32_t height;
    uint32_t maxNumFramesInFlight;
    uint32_t heapAllocSize;
} gpu_render_context_params_t;

/* Params that we pass to configure a render pass */
typedef struct gpu_renderpass_params_s {
    gpu_vertex_layout_t vertexLayout;
    const char* vertexShaderName;
    const char* fragmentShaderName;
    gpu_winding_t frontFace;
    gpu_cull_mode_t cullMode;
} gpu_renderpass_params_t;

#endif // __GPU_TYPES_H__
