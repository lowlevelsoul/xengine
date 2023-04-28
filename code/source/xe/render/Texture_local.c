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

#include "render/Texture.h"
#include "render/TexStream.h"
#include "stb_image.h"
#include "Fs.h"
#include "mem/Mem.h"
#include <assert.h>
#include <string.h>

DEFINE_RESOURCE_FACTORY( "Texture", Texture, texture )

static const SURFACE_FORMAT TEX_FORMAT_TABLE[] = {
    SURFACE_FORMAT_RGB_U8,            // FORMAT_RGB_U8
    SURFACE_FORMAT_RGBA_U8,           // FORMAT_RGBA_U8
    SURFACE_FORMAT_RGB_BC1,           // FORMAT_RGB_BC1
    SURFACE_FORMAT_NIL,               // FORMAT_RGBA_BC1
    SURFACE_FORMAT_RGBA_BC2,          // FORMAT_RGBA_BC2
    SURFACE_FORMAT_RGBA_BC3,          // FORMAT_RGBA_BC3
    SURFACE_FORMAT_RGB_ETC2,          // FORMAT_RGB_ETC2
    SURFACE_FORMAT_RGBA_ETC2,         // FORMAT_RGB_ETC2
    SURFACE_FORMAT_NIL,
    SURFACE_FORMAT_NIL,
    SURFACE_FORMAT_NIL,
    SURFACE_FORMAT_NIL,
    SURFACE_FORMAT_NIL,
    SURFACE_FORMAT_NIL
};

/*=======================================================================================================================================*/
void TextureResource_Load( resource_t * self_, file_t * file, const char * path ) {
    texture_t * tex = (texture_t*) Resource_GetData( self_ );
    
    const char * btexExt = strstr( path, ".btex" );
    
    if ( btexExt != NULL ) {
        Texture_LoadBtex( tex, file, path );
    }
    else {
        Texture_LoadImage( tex, file, path );
    }
}

/*=======================================================================================================================================*/
void * TextureResource_Alloc(void) {
    return Mem_Alloc(sizeof(texture_t));
}

/*=======================================================================================================================================*/
void  TextureResource_Free( void * data ) {
    Mem_Free( data );
}

/*=========================================================================================================================================*/
bool_t Texture_LoadStbi( texture_t * self_, file_t * file ) {
    void * buffer  = NULL;
    size_t bufferLen = 0;
    size_t amtRead=  0;
    int32_t numChannels = 0;
    int32_t width = 0;
    int32_t height = 0;
    unsigned char * image = NULL;
    
    bufferLen = FS_FileLength( file );
    buffer = Mem_Alloc( bufferLen );
    
    amtRead = FS_FileRead( file, buffer, 1, bufferLen );
    assert( amtRead == bufferLen );

    image = stbi_load_from_memory(buffer, (int) bufferLen, &width, &height, &numChannels, 4 );
    assert( image != NULL );
    
    Texture_Create( self_, SURFACE_FORMAT_RGBA_U8, width, height, 0, TEXTURE_USAGE_SHADER_READ );
    Texture_Write( self_, image, 0 );
    
    stbi_image_free( image );
    Mem_Free( buffer );
    
    return true;
}

/*=========================================================================================================================================*/
bool_t Texture_LoadImage( texture_t * self_, file_t * file, const char * path  ) {
    /* TODO: Add support for compiled image loading with mips from a tex stream */
    return Texture_LoadStbi( self_, file );
}

/*=========================================================================================================================================*/
bool_t Texture_LoadBtex( texture_t * self_, file_t * file, const char * path ) {
    
    size_t dataLength = FS_FileLength( file );
    void * data = Mem_Alloc( dataLength );
    assert( data != NULL );
    
    size_t amtRead = FS_FileRead( file, data, sizeof(uint8_t) , dataLength );
    assert( amtRead == dataLength );
    
    tex_stream_t * str = ( tex_stream_t * ) data;
    
    SURFACE_FORMAT texFmt = TEX_FORMAT_TABLE[ str->format ];
    
    Texture_Create( self_, texFmt, str->width, str->height, str->mipCount, TEXTURE_USAGE_SHADER_READ );
    
    for( uint32_t i = 0; i <= str->mipCount; ++i ) {
        const uint8_t * buffer = TexStream_GetImage( str, i );
        Texture_Write( self_, buffer, i );
    }
    
    Mem_Free( data );
    
    return true;
}
