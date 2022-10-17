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

#ifndef __RENDER3D_METAL_H__
#define __RENDER3D_METAL_H__

#import "render/Render3d_local.h"
#import "render/RenderCmd.h"
#import "math/Math3d.h"
#import "FrameHeap.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

typedef struct render3d_metal_s {
    render3d_t                 base;               /* Base data for the common render functionality */
    
    MTKView *                   mtkView;
    id<MTLDevice>               mtlDevice;
    id<MTLRenderPipelineState>  plDrawLit;
    id<MTLDepthStencilState>    depthStateDrawLit;
    id <MTLCommandQueue>        commandQueue;    
    dispatch_semaphore_t        inflightSemaphore;
} render3d_metal_t;

extern render3d_metal_t * const render3dMetal;

XE_API id<MTLDevice> Render_GetDevice();

#endif 
