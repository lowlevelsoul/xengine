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

#include <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>

#include "mem/Mem.h"
#include "math/Math3d.h"
#include "metal/Render_metal.h"
#include "metal/Model_metal.h"
#include "render/Material_local.h"
#include "metal/Texture_metal.h"
#include "shaders/Draw3d_lit.h"
#include "resource/Resource.h"

uint8_t render3dMemory[sizeof(render3d_metal_t)];
render3d_metal_t * const render3dMetal = (render3d_metal_t*) render3dMemory;
render3d_t * const render3d = (render3d_t*) render3dMemory;
bool_t render3dInit = bool_false;

static void Render_CreateRenderStates();
static void Render_CreateAllPipelines();
static id<MTLRenderPipelineState> Render_CreatePipeline( MTLVertexDescriptor * vertexDesc,
                                                         NSString * vertexFunc, NSString * pixelFunc,
                                                         MTKView * view, NSString * label );

static void Render_PassDrawLit( render_cmd_scene3d_t * scene, id<MTLRenderCommandEncoder> renderEncoder );
static void Render_SetMaterial( material_t * mat , id<MTLRenderCommandEncoder> renderEncoder );

/*=======================================================================================================================================*/
id<MTLDevice> Render_GetDevice() {
    return render3dMetal->mtlDevice;
}

/*=======================================================================================================================================*/
void Render_Initialise( render_params_t * params ) {
    if ( render3dInit == true ) {
        /* Nothing to do */
        return;
    }
    
    new( render3dMemory ) render3d_t;
    render3dMetal->mtkView = (__bridge MTKView* ) params->nativeView;
    render3dMetal->mtlDevice = render3dMetal->mtkView.device;
    
    render3dMetal->base.currScene           = NULL;
    render3dMetal->base.materialTimestamp   = 0;
    render3dMetal->base.batchMemSize        = 1024 * 1024 * 2;
    render3dMetal->base.batchMaterialCapacity = 128;
    render3dMetal->base.batchMem = Mem_Alloc( render3d->batchMemSize );
    render3dMetal->base.batchHeap = FrameHeap_Create( (uintptr_t) render3d->batchMem, render3d->batchMemSize );
    
    assert( params->maxBuffersInflight > 0 && params->maxBuffersInflight <= 3);
    render3d->maxBuffersInflight = params->maxBuffersInflight;
    render3dMetal->inflightSemaphore = dispatch_semaphore_create( params->maxBuffersInflight );
    
    render3dMetal->commandQueue = [render3dMetal->mtlDevice newCommandQueue];
    
    Render_CreateAllPipelines();
    Render_CreateRenderStates();
    
    render3dMetal->defaultTextures[ DEFAULT_TEX_BLACK ] = (texture_t *) Resource_GetData( Resource_Load( "~/textures/system/black.btex" ) );
    render3dMetal->defaultTextures[ DEFAULT_TEX_RED ] = (texture_t *) Resource_GetData( Resource_Load( "~/textures/system/red.btex" ) );
    render3dMetal->defaultTextures[ DEFAULT_TEX_WHITE ] = (texture_t *) Resource_GetData( Resource_Load( "~/textures/system/white.btex" ) );
}

/*=======================================================================================================================================*/
void Render_SetResolution( uint32_t dispWidth, uint32_t dispHeight ) {
    
}

/*=======================================================================================================================================*/
void Render_GetDisplaySize( uint32_t * dispWidth, uint32_t * dispHeight ) {
    if ( dispWidth != NULL ) {
        *dispWidth =  render3dMetal->mtkView.drawableSize.width; //[[UIScreen mainScreen] bounds].size.width;
    }
    
    if ( dispHeight != NULL ) {
        *dispHeight = render3dMetal->mtkView.drawableSize.height;
    }
}

/*=======================================================================================================================================*/
float Render_GetDisplayScale(void) {
    return render3dMetal->mtkView.layer.contentsScale;
}

//======================================================================================================================
void Render_Calc3dProjectionMat( mat4_t * mat, float fov, float aspect, float nearClip, float farClip ) {
    /*
        Coordinate system defined by projection matrix is:-
     
        +y
        |  +z
        | /
        |/_ _ _ +x
     
        */
    
    float yScale = 1.0f / scalar_Tan(fov * 0.5f);
    float xScale = yScale / aspect;
    
    Vec4_Set( mat->rows[0],           xScale,         0,          0,                              0 );
    Vec4_Set( mat->rows[1],              0,              yScale,     0,                              0 );
    Vec4_Set( mat->rows[2],              0,              0,          farClip / (farClip-nearClip),   1 );
    Vec4_Set( mat->rows[3],             0,              0,          -nearClip*farClip / (farClip-nearClip),     0);
}

/*=======================================================================================================================================*/
void Render_Finalise(void) {
    
}

/*=======================================================================================================================================*/
void Render_CreateRenderStates() {
    MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStateDesc.depthCompareFunction = MTLCompareFunctionLessEqual;
    depthStateDesc.depthWriteEnabled = YES;
    render3dMetal->depthStateDrawLit = [ Render_GetDevice() newDepthStencilStateWithDescriptor:depthStateDesc ];
}

/*=======================================================================================================================================*/
void Render_CreateAllPipelines() {
    MTLVertexDescriptor * modelVertexDesc = [[MTLVertexDescriptor alloc] init];

    modelVertexDesc.attributes[0].format = MTLVertexFormatFloat4;
    modelVertexDesc.attributes[0].offset = 0;
    modelVertexDesc.attributes[0].bufferIndex = 0;
    
    modelVertexDesc.attributes[1].format = MTLVertexFormatFloat4;
    modelVertexDesc.attributes[1].offset = 16;
    modelVertexDesc.attributes[1].bufferIndex = 0;
    
    modelVertexDesc.layouts[0].stride = 32;
    modelVertexDesc.layouts[0].stepRate = 1;
    modelVertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    
    render3dMetal->plDrawLit = Render_CreatePipeline( modelVertexDesc, @"Draw3dLit_Vertex", @"Draw3dLit_Pixel",
                                                      render3dMetal->mtkView, @"Draw3dLit");
}

/*=======================================================================================================================================*/
static id<MTLRenderPipelineState> Render_CreatePipeline( MTLVertexDescriptor * vertexDesc,
                                                         NSString * vertexFunc, NSString * pixelFunc,
                                                         MTKView * view, NSString * label )
{
    id<MTLLibrary> defaultLibrary = [Render_GetDevice() newDefaultLibrary];
    id <MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:vertexFunc];
    id <MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:pixelFunc];
    
    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = label;
    pipelineStateDescriptor.rasterSampleCount = view.sampleCount;
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.vertexDescriptor = vertexDesc;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    pipelineStateDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
    pipelineStateDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;
    
    NSError *error = NULL;
    id<MTLRenderPipelineState> pl = [Render_GetDevice() newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
    if (!pl) {
        NSLog(@"Failed to created pipeline state, error %@", error );
    }
    
    return pl;
}

/*=======================================================================================================================================*/
extern "C" void Render_SubmitScene( render_cmd_scene3d_t * scene ) {
    dispatch_semaphore_wait( render3dMetal->inflightSemaphore, DISPATCH_TIME_FOREVER);

    id <MTLCommandBuffer> commandBuffer = [render3dMetal->commandQueue commandBuffer];
    commandBuffer.label = @"MyCommand";

    __block dispatch_semaphore_t block_sema = render3dMetal->inflightSemaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
     {
         dispatch_semaphore_signal(block_sema);
    }];
    
    /// Delay getting the currentRenderPassDescriptor until we absolutely need it to avoid
    ///   holding onto the drawable and blocking the display pipeline any longer than necessary
    MTLRenderPassDescriptor* renderPassDescriptor = render3dMetal->mtkView.currentRenderPassDescriptor;

    if( renderPassDescriptor != nil ) {
        
        id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        renderEncoder.label = @"MyRenderEncoder";

        Render_PassDrawLit( scene, renderEncoder );

        [renderEncoder endEncoding];

        [commandBuffer presentDrawable: render3dMetal->mtkView.currentDrawable ];
    }
    
    [ commandBuffer commit ];
}


/*=======================================================================================================================================*/
void Render_PassDrawLit( render_cmd_scene3d_t * scene, id<MTLRenderCommandEncoder> renderEncoder ) {
    [ renderEncoder setFrontFacingWinding:MTLWindingClockwise ];
    [ renderEncoder setCullMode:MTLCullModeBack ];
    [ renderEncoder setRenderPipelineState: render3dMetal->plDrawLit ];
    [ renderEncoder setDepthStencilState: render3dMetal->depthStateDrawLit ];
    
    
    Draw3dLit_SceneConstants sceneConst;
    memcpy( &sceneConst.projMat, &scene->matProj, sizeof(mat4_t) );
    memcpy( &sceneConst.viewMat, &scene->matView, sizeof(mat4_t) );
    memcpy( &sceneConst.globalLightDir, &scene->globalLightDir, sizeof( sceneConst.globalLightDir ) );
    memcpy( &sceneConst.globalLightColour, &scene->globalLightColour, sizeof( sceneConst.globalLightColour ) );
    memcpy( &sceneConst.eyePosition, &scene->matViewWorld.rows[3], sizeof(sceneConst.eyePosition ) );
    
    [ renderEncoder setVertexBytes: &sceneConst length:sizeof(sceneConst) atIndex:Draw3dLit_BufferSceneConst ];
    [ renderEncoder setFragmentBytes: &sceneConst length:sizeof(sceneConst) atIndex: Draw3dLit_Pixel_BufferSceneConst ];

    for ( render_cmd_material_t * matCmd = scene->materials; matCmd != &scene->materials[scene->materialCount]; ++matCmd ) {
        /* Setup the material */
        Render_SetMaterial( matCmd->material, renderEncoder );
        
        /* Draw all of the models that use this material */
        for ( render_cmd_draw_t * drawCmd = matCmd->head; drawCmd != NULL; drawCmd = drawCmd->next ) {
            model_metal_t * modelMetal = (model_metal_t *) drawCmd->model;
            Draw3dLit_ModelConstants modelConst;
            memcpy( &modelConst.worldMat, &drawCmd->xform, sizeof( mat4_t ) );
            
            [ renderEncoder setVertexBuffer: modelMetal->vertices
                                     offset: 0
                                    atIndex: 0 ];
            
            [ renderEncoder setVertexBytes: &modelConst
                                    length: sizeof(modelConst)
                                   atIndex: Draw3dLit_BufferModelConst ];
            
            MTLIndexType indexType_ = ( modelMetal->indexStride == 2 ) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
            size_t indexOffset = drawCmd->indexStart * modelMetal->indexStride;
            
            [renderEncoder drawIndexedPrimitives: MTLPrimitiveTypeTriangle
                                indexCount: drawCmd->indexCount
                                 indexType: indexType_
                               indexBuffer: modelMetal->indices
                         indexBufferOffset: indexOffset ];
        }
    }
}

/*=======================================================================================================================================*/
void Render_SetMaterial( material_t * mat , id<MTLRenderCommandEncoder> renderEncoder ) {
    texture_metal_t * texAlbedo;
    texture_metal_t * texGlow;
    material_local_t * matLocal = ( material_local_t * ) mat;
    
    texAlbedo = (texture_metal_t *) matLocal->textureAlbedo;
    texGlow = (matLocal->textureGlow != NULL) ? (texture_metal_t *) matLocal->textureGlow : (texture_metal_t *) render3dMetal->defaultTextures[ DEFAULT_TEX_BLACK ];
    
    [ renderEncoder setFragmentTexture: texAlbedo->m_texture atIndex: Draw3dLit_Pixel_TextureAlbedo ];
    [ renderEncoder setFragmentTexture: texGlow->m_texture atIndex: Draw3dLit_Pixel_TextureGlow ];
}
