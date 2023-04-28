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

#ifndef __MATERIALSTREAM_H__
#define __MATERIALSTREAM_H__

#include "core/Common.h"

#define  MATERIAL_STREAM VERSION 1

typedef struct material_stream_s {
    uint32_t        version;
    uint32_t        flags;
    uint32_t        offsAlbedoTexture;
    uint32_t        offsAmrTexture;
    uint32_t        offsGlowTexture;
    float           transparency;
} material_stream_t;

XE_API const char * MaterialStream_GetAlbedo( const material_stream_t * str );
XE_API const char * MaterialStream_GetAmr( const material_stream_t * str );
XE_API const char * MaterialStream_GetGlow( const material_stream_t * str );

#endif
