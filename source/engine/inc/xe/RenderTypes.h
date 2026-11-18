/*
==============================================================================================================
    This file is part of XEngine 
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __RENDERTYPES_H__
#define __RENDERTYPES_H__

#include "xe/Platform.h"
#include "xe/Str.h"

typedef enum render_texture_type_e {
    RENDER_TEXTURE_TYPE_DIFFUSE = 0,
    RENDER_TEXTURE_TYPE_NORMAL,
    RENDER_TEXTURE_TYPE_SPECULAR,
    RENDER_TEXTURE_TYPE_GLOW,
    RENDER_TEXTURE_TYPE_COUNT,
} render_texture_type_t;

typedef enum render_opacity_e {
    RENDER_OPACITY_OPAQUE = 0,
    RENDER_OPACITY_TRANSPARENT_TEXTURE,
    RENDER_OPACITY_TRANSPARENT_ALPHA,
    RENDER_OPACITY_COUNT,
} render_opacity_t;

#define RENDER_MODEL_DRAW_SURFACE_MAX 8

typedef struct render_window_s render_window_t;
typedef struct render_context_s render_context_t;  

typedef struct render_gpu_buffer_s render_gpu_buffer_t;
typedef struct render_gpu_texture_s render_gpu_texture_t;

typedef struct render_static_vertex_s {
    float positionTexU[ 4 ];
    float normalTexV[ 4 ];
    float tangent[ 4 ];
} render_static_vertex_t;

typedef struct render_skin_vertex_s {
    float texU;
    float texV;
    uint32_t weightStart;
    uint32_t weightCount;
} render_skin_vertex_t;

typedef struct render_skin_weight_s {
    float offset[ 3 ];
    float normal[ 3 ];
    float tangent[ 3 ];
    uint32_t jointIndex;
    float weight;
    float padding;
} render_skin_weight_t;

/**
 * @brief Render geometry encapsulates all of the buffers that we use for rendering geometry.
 */
typedef struct render_geometry_s {
    render_gpu_buffer_t* vertex_buffer;
    render_gpu_buffer_t* index_buffer;
    render_gpu_buffer_t* weight_buffer;

    uint32_t vertexCount;
    uint32_t vertexStride;
    uint32_t vertexDataSize;

    uint32_t indexCount;
    uint32_t indexStride;
    uint32_t indexDataSize;

    uint32_t weightCount;
    uint32_t weightStride;
    uint32_t weightDataSize;
} render_geometry_t;

/**
 * @brief Render draw surface represents a subset of geometry to be drawn with a specific material.
 */
typedef struct render_draw_surface_s {
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t vertexOffset;
    uint32_t vertexCount;
    uint32_t weightOffset;
    uint32_t weightCount;
    uint32_t materialIndex;
    uint32_t data;
} render_draw_surface_t;

/**
 * @brief Render material encapsulates the textures and properties used for rendering a surface.
 */
typedef struct render_material_s {
    render_gpu_texture_t* textures[ RENDER_TEXTURE_TYPE_COUNT ];
    render_opacity_t opacity;
    float alpha;
    float specularIntensity;
    float specularPower;
    float glowIntensity;
} render_material_t;

/**
 * @brief Render model represents a 3D model with geometry and draw surfaces.
 */
typedef struct render_model_s {
    str_t name;
    render_geometry_t geometry;
    render_draw_surface_t drawsurfaces[ RENDER_MODEL_DRAW_SURFACE_MAX ];
    uint32_t drawSurfaceCount;
    float boundingBox[ 6 ]; /**< Axis-aligned bounding box: minX, minY, minZ, maxX, maxY, maxZ */
} render_model_t;


typedef struct render_model_instance_s {
    render_model_t * model;
    render_draw_surface_t drawSurfaces[ RENDER_MODEL_DRAW_SURFACE_MAX ];
    render_draw_surface_t * drawSurfaceOpaque[ RENDER_MODEL_DRAW_SURFACE_MAX ];
    render_draw_surface_t * drawSurfaceTransparent[ RENDER_MODEL_DRAW_SURFACE_MAX ];
    uint32_t drawSurfaceCount;
    uint32_t drawSurfaceOpaqueCount;
    uint32_t drawSurfaceTransparentCount;
} render_model_instance_t;



typedef struct render_params_s {
    int32_t width;                            /**< Width of the render window */
    int32_t height;                           /**< Height of the render window */
    bool fullscreen;                         /**< Whether the render window should be fullscreen */
    uint32_t maxNumFramesInFlight;          /**< Maximum number of frames that can be in flight */
    uint32_t renderMemHeapSize;             /**< Size of the render memory heap */
} render_params_t;

#endif // __RENDERTYPES_H__