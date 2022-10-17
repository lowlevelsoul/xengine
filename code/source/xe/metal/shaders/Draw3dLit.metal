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

#include <metal_stdlib>
#include <simd/simd.h>
#include "Draw3d_lit.h"

using namespace metal;

typedef struct {
    float4      posTexU     [ [ attribute( Draw3dLit_PosTexU ) ] ];
    float4      normTexV    [ [ attribute( Draw3dLit_NormTexV ) ] ];
} Draw3dLitVertex;

// In/out struct
typedef struct {
    float4 positionClip             [[ position ]];       // Position in clip space
    float4 posWorldTexU;
    float4 normWorldTexV;
} Draw3dLitInOut;

//======================================================================================================================
vertex Draw3dLitInOut Draw3dLit_Vertex( Draw3dLitVertex in [[stage_in]],
                                        constant Draw3dLit_SceneConstants & sceneConst [[ buffer(Draw3dLit_BufferSceneConst) ]],
                                        constant Draw3dLit_ModelConstants & modelConst [[ buffer(Draw3dLit_BufferModelConst) ]] ) {
    
    Draw3dLitInOut out;
    float4 pos4 = modelConst.worldMat * float4( in.posTexU.xyz, 1 );
    float4 norm4 = modelConst.worldMat * float4( in.normTexV.xyz, 0 );
    float4 viewPos4 = sceneConst.viewMat * pos4;
    
    out.positionClip = sceneConst.projMat * viewPos4;
    out.posWorldTexU = float4( pos4.xyz, in.posTexU.w );
    out.normWorldTexV = float4( norm4.xyz, in.normTexV.w );
    
    return out;
}

//======================================================================================================================
fragment float4 Draw3dLit_Pixel( Draw3dLitInOut in [[stage_in]],
                                constant Draw3dLit_SceneConstants & scene [[ buffer( Draw3dLit_Pixel_BufferSceneConst ) ]],
                                texture2d<float> albedoTexture            [[ texture( Draw3dLit_Pixel_TextureAlbedo ) ]],
                                texture2d<float> glowTexture              [[ texture( Draw3dLit_Pixel_TextureGlow ) ]]) {
    
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);
    
    float2 texCoord = float2( in.posWorldTexU.w, in.normWorldTexV.w );
    float4 albedo = albedoTexture.sample( textureSampler, texCoord );
    float4 glow = glowTexture.sample( textureSampler, texCoord );
    
    float3 ln = scene.globalLightDir.xyz;
    float3 n = normalize(in.normWorldTexV.xyz);
    float3 v = normalize(scene.eyePosition.xyz - in.posWorldTexU.xyz);
    
    float d = -dot(n, float3(scene.globalLightDir.xyz) );
    if ( d < 0.5 )
        d = 0.1;
    else
        d = 1.0;
    float3 color = (albedo.xyz * d) + glow.xyz;
    
    //color = color / (color + float3(1.0));
    //color = pow(color, float(1.0/1.9));
   
    return float4( color.xyz, 1.0 );
}
