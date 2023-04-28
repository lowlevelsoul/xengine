/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/


#ifndef __RENDERCMD_H__
#define __RENDERCMD_H__

#include "core/Common.h"
#include "math/Math3d.h"

#define RENDER_CMD_ADD_ITEM( LIST, ITEM )\
    if ( (LIST)->head == NULL ) {\
        (LIST)->head = (ITEM);\
    } else {\
        (LIST)->tail->next = (ITEM);\
    }\
    (LIST)->tail = (ITEM);\
    (ITEM)->next = NULL;
    

typedef struct model_s model_t;
typedef struct material_s material_t;

typedef struct render_cmd_draw_s {
    model_t *   model;
    mat4_t      xform;
    uint32_t    indexStart;
    uint32_t    indexCount;
    
    struct render_cmd_draw_s * next;
} render_cmd_draw_t;

typedef struct render_cmd_material_s {
    material_t *        material;
    render_cmd_draw_t * head;
    render_cmd_draw_t * tail;
} render_cmd_material_t;

typedef struct render_cmd_scene3d_s {
    mat4_t      matProj;
    mat4_t      matView;
    mat4_t      matViewWorld;
    
    vec3_t      globalLightDir;
    vec3_t      globalLightColour;
    
    render_cmd_material_t * materials;
    uintptr_t   materialCapacity;
    uintptr_t   materialCount;
} render_cmd_scene3d_t;

#endif
