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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "core/Platform.h"
#include "resource/Resource.h"
#include "core/Fs.h"

typedef struct texture_s {
    uint64_t        data[32];
} texture_t;

#define TEXTURE_USAGE_SHADER_READ 1
#define TEXTURE_USAGE_SHADER_WRITE 2
#define TEXTURE_USAGE_RENDER_TARGET 4
#define TEXTURE_USAGE_PRIVATE 8

typedef enum SURFACE_FORMAT_E {
    SURFACE_FORMAT_NIL = 0,
    
    SURFACE_FORMAT_RGB_S8,
    SURFACE_FORMAT_RGB_U8,
    
    SURFACE_FORMAT_RGBA_S8,
    SURFACE_FORMAT_RGBA_U8,
    
    SURFACE_FORMAT_DEPTH_F32,
    SURFACE_FORMAT_DEPTH_STENCIL_F32_U8,
    
    SURFACE_FORMAT_RGB_BC1,
    SURFACE_FORMAT_RGBA_BC2,
    SURFACE_FORMAT_RGBA_BC3,
    
    SURFACE_FORMAT_RGB_ETC2,
    SURFACE_FORMAT_RGBA_ETC2,
    
    SURFACE_FORMAT_NATIVE,
    
    SURFACE_FORMAT_COUNT,
} SURFACE_FORMAT;

XE_API void Texture_Create( texture_t * self_, SURFACE_FORMAT format, uint32_t width, uint32_t height, uint32_t mipCount, uint64_t flags );
XE_API void Texture_Destroy( texture_t * self_ );
XE_API void Texture_Write( texture_t * self_, const void * srcBuffer, uint32_t mip );
XE_API SURFACE_FORMAT Texture_GetFormat( texture_t * self_ );
XE_API uint32_t Texture_GetWidth( texture_t * self_ );
XE_API uint32_t Texture_GetHeight( texture_t * self_ );
XE_API uint32_t Texture_GetMipCount( texture_t * self_ );

XE_API bool_t Texture_LoadImage( texture_t * self_, file_t * file, const char * path );
XE_API bool_t Texture_LoadBtex( texture_t * self_, file_t * file, const char * path );

extern resource_factory_t * texture_resource_factory;


#endif 
