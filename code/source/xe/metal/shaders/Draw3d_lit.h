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

#ifndef __DRAW3D_LIT_H__
#define __DRAW3D_LIT_H__

#ifdef __METAL_VERSION__
#define NS_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#define NSInteger metal::int32_t
#else
#import <Foundation/Foundation.h>
#endif

typedef NS_ENUM(NSInteger, Draw3dLit) {
    Draw3dLit_PosTexU = 0,
    Draw3dLit_NormTexV = 1,
    
    Draw3dLit_BufferSceneConst = 1,
    Draw3dLit_BufferModelConst = 2,
    
    Draw3dLit_Pixel_BufferSceneConst = 0,
    
    Draw3dLit_Pixel_TextureAlbedo = 0,
    Draw3dLit_Pixel_TextureGlow = 2
};

typedef struct {
    matrix_float4x4     projMat;
    matrix_float4x4     viewMat;
    
    vector_float4       globalLightDir;
    vector_float4       globalLightColour;
    vector_float4       eyePosition;
    
} Draw3dLit_SceneConstants;

typedef struct {
    matrix_float4x4     worldMat;
} Draw3dLit_ModelConstants;


#endif
