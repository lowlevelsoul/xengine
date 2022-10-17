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

#import <Foundation/Foundation.h>
#import "Render_metal.h"
#import "Texture_metal.h"
#import "Id.h"
#include <assert.h>

#define TEXTURE_MAGIC MAKE_ID(X,e,T,e,x,t,u,r,e,_,_,_)

#if TARGET_OS_OSX == 1
#   define FORMAT_MACOS_ONLY(VAL) VAL
#else
#   define FORMAT_MACOS_ONLY(VAL) MTLPixelFormatInvalid
#endif

static const size_t PIXEL_SIZE_TABLE[] = {
    0,          // FORMAT_NONE = 0,
    
    3,          // FORMAT_RGB_S8,
    3,          // FORMAT_RGB_U8,
    4,          // FORMAT_RGBA_S8,
    4,          // FORMAT_RGBA_U8,
    
    4,          // DEPTH_F32,
    5,          // DEPTH_STENCIL_F32_U8
};

static const MTLPixelFormat FORMAT_TABLE[] = {
    MTLPixelFormatInvalid,                          // FORMAT_NONE = 0,

    MTLPixelFormatInvalid,                          // FORMAT_RGB_S8
    MTLPixelFormatInvalid,                          // FORMAT_RGB_U8

    MTLPixelFormatRGBA8Snorm,                       // FORMAT_RGBA_S8
    MTLPixelFormatRGBA8Unorm,                       // FORMAT_RGBA_U8,
    
    MTLPixelFormatDepth32Float,
    MTLPixelFormatDepth32Float_Stencil8,
    
    FORMAT_MACOS_ONLY(MTLPixelFormatBC1_RGBA),      // FORMAT_RGBA_BC1,
    FORMAT_MACOS_ONLY(MTLPixelFormatBC2_RGBA),      // FORMAT_RGBA_BC2,
    FORMAT_MACOS_ONLY(MTLPixelFormatBC3_RGBA),      // FORMAT_RGBA_BC3
    
    MTLPixelFormatETC2_RGB8,                        // FORMAT_RGB_ETC2
    MTLPixelFormatInvalid,                          // FORMAT_RGBA_ETC2
};

/*=======================================================================================================================================*/
static void Texture_Clear( texture_metal_t * self_ ) {
    self_->m_format = SURFACE_FORMAT_NIL;
    self_->m_width = 0;
    self_->m_height = 0;
    self_->m_mipCount = 0;
    self_->m_flags = 0;
    self_->m_timestamp = 0;
    self_->m_poolIndex = 0;
    self_->m_heapTexture = 0;
    self_->m_heapTexturePending = 0;
    self_->m_blockSizeX = 0;
    self_->m_blockSizeY = 0;
    self_->m_pitch = 0;
    self_->m_blockCountX = 0;
    self_->m_blockCountY = 0;
    self_->m_blockSize = 0;
}



//======================================================================================================================
static size_t Texture_GetPixelSize( SURFACE_FORMAT fmt ) {
    
    if ( fmt >= SURFACE_FORMAT_RGB_S8 && fmt <= SURFACE_FORMAT_DEPTH_STENCIL_F32_U8 ) {
        return PIXEL_SIZE_TABLE[ (uint32_t) fmt];
    }
    else if ( fmt >= SURFACE_FORMAT_RGB_BC1 && fmt <= SURFACE_FORMAT_RGBA_BC2 ) {
        return 8;
    }
    else if ( fmt == SURFACE_FORMAT_RGBA_BC3 ) {
        return 16;
    }
    else if ( fmt >= SURFACE_FORMAT_RGB_ETC2 && fmt <= SURFACE_FORMAT_RGBA_ETC2 ) {
        return 8;
    }
          
    return 0;
}

//======================================================================================================================
static size_t Texture_GetByteSize( SURFACE_FORMAT fmt, uint32_t width, uint32_t height ) {
  size_t pixelSize = Texture_GetPixelSize( fmt );
  
  if ( fmt >= SURFACE_FORMAT_RGB_BC1 && fmt <= SURFACE_FORMAT_RGBA_BC3 ) {
      size_t size = ( ( width * height ) / 16 ) * pixelSize;
      return size;
  }
  else if ( fmt >= SURFACE_FORMAT_RGB_ETC2 && fmt <= SURFACE_FORMAT_RGB_ETC2 ) {
      size_t size = ( ( width * height ) / 16 ) * pixelSize;
      return size;
  }
  
  size_t size = ( width * height ) * pixelSize;
  return size;
}

//======================================================================================================================
static bool Texture_IsBlockCompressed( SURFACE_FORMAT format) {
    return (format >= SURFACE_FORMAT_RGB_BC1 && format <= SURFACE_FORMAT_RGBA_ETC2);
}

/*=======================================================================================================================================*/
void Texture_Create( texture_t * self_, SURFACE_FORMAT format, uint32_t width, uint32_t height, uint32_t mipCount, uint64_t flags ) {
    static_assert( sizeof(texture_t) >= sizeof(texture_metal_t), "Size of texture_t.data is too small for implementation" );
    
    texture_metal_t * texMetal = new(self_->data ) texture_metal_t;
    texMetal->magic = TEXTURE_MAGIC;
    Texture_Clear( texMetal );
        
    // What's the metal format?
    MTLPixelFormat mtlFormat = FORMAT_TABLE[ (uint32_t) format];
    assert( mtlFormat != MTLPixelFormatInvalid );
    
    uint32_t usageFlags = 0;
    if (flags & TEXTURE_USAGE_RENDER_TARGET) {
        usageFlags |= MTLTextureUsageRenderTarget;
    }
    
    if (flags & TEXTURE_USAGE_SHADER_READ) {
        usageFlags |= MTLTextureUsageShaderRead;
    }
    
    if (flags & TEXTURE_USAGE_SHADER_WRITE) {
        usageFlags |= MTLTextureUsageShaderWrite;
    }
    
#if TARGET_OS_OSX == 1
    MTLStorageMode storageMode = MTLStorageModeManaged;
#else
    MTLStorageMode storageMode = MTLStorageModeShared;
#endif
    
    if ( format >= SURFACE_FORMAT_DEPTH_F32 && format <= SURFACE_FORMAT_DEPTH_STENCIL_F32_U8 ) {
        storageMode = MTLStorageModePrivate;
    }
    
    texMetal->m_format    = format;
    texMetal->m_width     = width;
    texMetal->m_height    = height;
    texMetal->m_mipCount  = mipCount;
    texMetal->m_pitch     = width * (uint32_t) Texture_GetPixelSize( format );
    texMetal->m_flags     = flags;
    
    // Create the metal texture object
    MTLTextureDescriptor* desc = [ MTLTextureDescriptor new ];
    
    desc.textureType            = MTLTextureType2D;
    desc.width                  = width;
    desc.height                 = height;
    desc.pixelFormat            = mtlFormat;
    desc.usage                  = usageFlags;
    desc.mipmapLevelCount       = mipCount + 1;
    desc.storageMode            = storageMode;
    
    texMetal->m_formatMetal = mtlFormat;
    texMetal->m_texture = [ Render_GetDevice() newTextureWithDescriptor:desc ];
    
    bool isBc = Texture_IsBlockCompressed( format );
    if (isBc == true) {
        texMetal->m_blockSizeX = 4;
        texMetal->m_blockSizeY = 4;
        texMetal->m_blockCountX = texMetal->m_width / texMetal->m_blockSizeX;
        texMetal->m_blockCountY = texMetal->m_height / texMetal->m_blockSizeY;
        texMetal->m_blockSize = (uint32_t) Texture_GetPixelSize( format );
    }
    else {
        texMetal->m_blockSizeX = 0;
        texMetal->m_blockSizeY = 0;
        texMetal->m_blockCountX = 0;
        texMetal->m_blockCountY = 0;
        texMetal->m_blockSize = 0;
    }
}

/*=======================================================================================================================================*/
void Texture_Destroy( texture_t * self_ ) {
    texture_metal_t * texMetal = (texture_metal_t *) self_;
    
    assert( self_ != NULL );
    assert( texMetal->magic == TEXTURE_MAGIC );
    
    texMetal->~texture_metal_t();
}

/*=======================================================================================================================================*/
void Texture_Write( texture_t * self_, const void * srcBuffer, uint32_t mip ) {
    texture_metal_t * texMetal = (texture_metal_t *) self_;
    
    assert( self_ != NULL );
    assert( texMetal->magic == TEXTURE_MAGIC );
    
    MTLRegion region;
    region.origin.x = 0;
    region.origin.y = 0;
    region.origin.z = 0;

    region.size.width = (texMetal->m_width >> mip);
    region.size.height = (texMetal->m_height >> mip);
    region.size.depth = 1;
   
    NSUInteger bytesPerRow = ( texMetal->m_pitch >> mip);
    
    if (Texture_IsBlockCompressed( texMetal->m_format )) {
        bytesPerRow = (region.size.width / texMetal->m_blockSizeX) * texMetal->m_blockSize;
    }
    
    [texMetal->m_texture replaceRegion: region
                           mipmapLevel: mip
                             withBytes: srcBuffer
                           bytesPerRow: bytesPerRow];
}

/*=======================================================================================================================================*/
SURFACE_FORMAT Texture_GetFormat( texture_t * self_ ) {
    texture_metal_t * texMetal = (texture_metal_t *) self_;
    
    assert( self_ != NULL );
    assert( texMetal->magic == TEXTURE_MAGIC );
    
    return texMetal->m_format;
}

/*=======================================================================================================================================*/
uint32_t Texture_GetWidth( texture_t * self_ ) {
    texture_metal_t * texMetal = (texture_metal_t *) self_;
    
    assert( self_ != NULL );
    assert( texMetal->magic == TEXTURE_MAGIC );
    
    return texMetal->m_width;
}

/*=======================================================================================================================================*/
uint32_t Texture_GetHeight( texture_t * self_ ) {
    texture_metal_t * texMetal = (texture_metal_t *) self_;
    
    assert( self_ != NULL );
    assert( texMetal->magic == TEXTURE_MAGIC );
    
    return texMetal->m_height;
}

/*=======================================================================================================================================*/
uint32_t Texture_GetMipCount( texture_t * self_ ) {
    texture_metal_t * texMetal = (texture_metal_t *) self_;
    
    assert( self_ != NULL );
    assert( texMetal->magic == TEXTURE_MAGIC );
    
    return texMetal->m_mipCount;
}
