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

#ifndef __TEXTURE_METAL_H__
#define __TEXTURE_METAL_H__

#include "core/Common.h"
#include "render/Texture.h"
#import <Metal/Metal.h>

typedef struct texture_metal_s {
    uint64_t                magic;
    SURFACE_FORMAT          m_format;
    uint32_t                m_width;
    uint32_t                m_height;
    uint32_t                m_mipCount;
    uint64_t                m_flags;
    uint64_t                m_timestamp;
    uint64_t                m_poolIndex;
    id<MTLTexture>          m_texture;
    uint32_t                m_heapTexture : 1;
    uint32_t                m_heapTexturePending : 1;
    MTLPixelFormat          m_formatMetal;
    uint32_t                m_blockSizeX;
    uint32_t                m_blockSizeY;
    uint32_t                m_pitch;
    uint32_t                m_blockCountX;
    uint32_t                m_blockCountY;
    uint32_t                m_blockSize;
} texture_metal_t;

#endif
