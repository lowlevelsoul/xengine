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

#ifndef __RENDER3D_LOCAL_H__
#define __RENDER3D_LOCAL_H__

#include "core/Common.h"
#include "render/Model.h"
#include "render/Material.h"
#include "render/RenderCmd.h"
#include "mem/FrameHeap.h"
#include "render/Render3d.h"

typedef struct render3d_s {
    uint64_t                    materialTimestamp;
    render_cmd_scene3d_t *      currScene;
    frame_heap_t *              batchHeap;
    void *                      batchMem;
    size_t                      batchMemSize;
    size_t                      batchMaterialCapacity;
    int32_t                     maxBuffersInflight;
} render3d_t;

extern render3d_t * const render3d;

#endif
