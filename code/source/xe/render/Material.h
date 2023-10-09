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

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "core/Platform.h"
#include "resource/Resource.h"

typedef struct texture_s texture_t;
typedef struct material_s {
    uint64_t    data[32];
} material_t;

XE_API void Material_Create( material_t * self_ );
XE_API void Material_Destroy( material_t * self_ );
XE_API void Material_SetTextureAlbedo( material_t * self_, texture_t * tex );
XE_API void Material_SetTextureGlow( material_t * self_, texture_t * tex );
XE_API void Material_SetTextureAmr( material_t * self_, texture_t * tex );

extern resource_factory_t * material_resource_factory;

#endif
