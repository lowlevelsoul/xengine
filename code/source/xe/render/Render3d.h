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

#ifndef __RENDER3D_H__
#define __RENDER3D_H__

#include "core/Common.h"
#include "render/Model.h"
#include "render/Material.h"
#include "render/RenderCmd.h"
#include "mem/FrameHeap.h"

typedef struct render_params_s {
    void *      nativeView;
    int32_t     displayWidth;
    int32_t     displayHeight;
    int32_t     maxBuffersInflight;
} render_params_t;

typedef struct camera_s camera_t;

XE_API void Render_Initialise( render_params_t * params );

XE_API void Render_SetResolution( uint32_t dispWidth, uint32_t dispHeight );

XE_API void Render_GetDisplaySize( uint32_t * dispWidth, uint32_t * dispHeight );

XE_API float Render_GetDisplayScale(void);

XE_API void Render_Calc3dProjectionMat( mat4_t * mat, float fov, float aspect, float nearClip, float farClip );

XE_API void Render_Finalise(void);

XE_API void Render_Begin( camera_t * camera, const int32_t * viewport );

XE_API void Render_End(void);

XE_API void Render_SubmitModel( model_t * model, material_t * mat, const mat4_t * xform );

#endif
