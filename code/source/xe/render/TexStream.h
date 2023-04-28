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

#ifndef __TEXTURESTREAM_H__
#define __TEXTURESTREAM_H__

#include "core/Common.h"

#define TEXSTREAM_VERSION 2
#define TEXSTREAM_MAX_MIP_COUNT 32

#define TEXSTREAM_F_IMAGE_BIN = 0x00000001;

typedef enum tex_stream_format_e {
    TEXSTREAM_FORMAT_RGB_U8 = 0,
    TEXSTREAM_FORMAT_RGBA_U8,
    TEXSTREAM_FORMAT_RGB_BC1,
    TEXSTREAM_FORMAT_RGBA_BC1,
    TEXSTREAM_FORMAT_RGBA_BC2,
    TEXSTREAM_FORMAT_RGBA_BC3,
    
    TEXSTREAM_FORMAT_RGB_ETC2,
    TEXSTREAM_FORMAT_RGBA_ETC2,
    
    TEXSTREAM_FORMAT_NONE = 0xffffffff
} tex_stream_format_t;

typedef struct tex_stream_s {
        uint32_t        version;
        uint32_t        flags;
        uint32_t        format;
        uint32_t        width;
        uint32_t        height;
        uint32_t        mipCount;
        uint32_t        offsImages[ TEXSTREAM_MAX_MIP_COUNT ];
} tex_stream_t;

XE_API const uint8_t * TexStream_GetImage( const tex_stream_t * str, uint32_t index );

#endif
