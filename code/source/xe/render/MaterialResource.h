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


#ifndef __MATERIALRESOURCE_H__
#define __MATERIALRESOURCE_H__

#include "core/Platform.h"
#include "util/ParseLiteral.h"
#include "render/Material.h"
#include "resource/Resource.h"

XE_API void MaterialResource_StartParsing( material_t * mat );
XE_API void MaterialResource_EndParsing(void);
XE_API void MaterialResource_SetAlbedoTexture( parse_literal_t * path );
XE_API void MaterialResource_SetGlowTexture( parse_literal_t * path );
XE_API void MaterialResource_LogError( const char * fmt, ... );
XE_API bool_t MaterialResource_HasErrors( void );
XE_API void MaterialResource_IncLine( void );

#endif
