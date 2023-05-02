//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#include "texturebuilder/Image.h"
#include "stb_image.h"

static const uint32_t PIXEL_SIZE[] = {
    0,
    
    3,
    4,
    
    6,
    8,
    
    12,
    16,
    
    12,
    16,
    
    0,
    0,
    
};

static const uint32_t ETC_BLOCK_WIDTH = 4;
static const uint32_t ETC_BLOCK_HEIGHT = 4;
static const uint32_t ETC_BLOCK_SIZE = 8;

static const uint32_t BC3_BLOCK_WIDTH = 4;
static const uint32_t BC3_BLOCK_HEIGHT = 4;
static const uint32_t BC3_BLOCK_SIZE = 16;

//======================================================================================================================
Image::Image() {
    
}

//======================================================================================================================
Image::~Image() {
    
}

//======================================================================================================================
void Image::Create( FORMAT format_, uint32_t w, uint32_t h ) {
    
    format = format_;
    width = w;
    height = h;
    
    bool blockFormat = IsBlockFormat( format_ );
    if ( blockFormat == true ) {
        switch( format_ ) {
            case FORMAT_BLOCK_ETC: {
                blockWidth = ETC_BLOCK_WIDTH;
                blockHeight = ETC_BLOCK_HEIGHT;
                blockSize = ETC_BLOCK_SIZE;
                break;
            }
                
            case FORMAT_BLOCK_BC3: {
                blockWidth = BC3_BLOCK_WIDTH;
                blockHeight = BC3_BLOCK_HEIGHT;
                blockSize = BC3_BLOCK_SIZE;
                break;
            }
                
            default:
                xerror( true, "Unknown block format" );
        }
        
        blockCountX = width / blockWidth;
        blockCountY = height / blockHeight;
        blockCount = blockCountX * blockCountY;
        size = blockCount * blockSize;
    }
    else {
        bpp = PIXEL_SIZE[ format_ ];
        pitch = bpp * width;
        size = pitch * height;
    }
    
    pixels.resize( size );
    
    if ( blockFormat == true ) {
        blocks.resize( blockCount );
        
        uint8_t * currBlock = &pixels[ 0 ];
        for ( uint32_t b = 0; b < blockCount; ++b ) {
            blocks[ b ] = currBlock;
            currBlock += blockSize;
        }
    }
    else {
        
        rows.resize( height );
        
        uint8_t * currRow = &pixels[ 0 ];
        for ( uint32_t r = 0; r < height; ++r ) {
            rows[ r ] = currRow;
            currRow += pitch;
        }
    }
}


//======================================================================================================================
void Image::Load( const char * path, const char * transform ) {
    
    str_t pathTmp = NULL;
    FS_MakePath( &pathTmp, path );
    
    std::string fullPath = pathTmp;
    Str_Destroy( &pathTmp );
    
    xprintf("Loading image '%s'\n", path );
    
    int w;
    int h;
    int numChannelsInFile = 0;
    
    stbi_uc * imageBuffer = stbi_load( fullPath.c_str(), &w, &h, &numChannelsInFile, 4 );
    xerror( imageBuffer == nullptr, "Unable to load image from \"%s\"\n", path );
    
    Create( Image::FORMAT_RGBA_UBYTE, w, h );
    
    if ( transform == nullptr || transform[0] == 0) {
        memcpy( &pixels[ 0 ], imageBuffer, size );
    }
    else {
        // We have a (probably) valid transform string. So copy over the pixels
        // from the image source
        TransformPixels( &pixels[ 0 ], imageBuffer, transform );
    }
    
    stbi_image_free( imageBuffer );
    
}

//======================================================================================================================
uint32_t Image::GetFilterIndex( const char filterChar ) {
    if ( filterChar == 'r' || filterChar == 'R' ) {
        return 0;
    }
    else if ( filterChar == 'g' || filterChar == 'G' ) {
        return 1;
    }
    else if ( filterChar == 'b' || filterChar == 'B' ) {
        return 2;
    }
    else if ( filterChar == 'a' || filterChar == 'A' ) {
        return 3;
    }

    return 0xff;
}


//======================================================================================================================
void Image::TransformPixels( uint8_t * dst, const uint8_t * src, const char * transform ) {
    
    bool invert[ 4 ] = {false, false, false };
    int32_t srcIndex[ 4 ] = { -1, -1, -1, -1 };
    
    // Parse the transform string
    uint32_t currDstIndex = 0;
    for ( const char * currTf = transform; *currTf != 0; ++currTf ) {
        // Do we want to invert the current component?
        if ( *currTf == 'i' || *currTf == 'I') {
            invert[ currDstIndex ] = true;
        }
        else {
            uint32_t index = GetFilterIndex( *currTf );
            xerror( index > 3, "Unknown transform specifier '%c'\n", *currTf );
            srcIndex[ currDstIndex ] = index;
            
            ++currDstIndex;
        }
    }
    
    uint32_t pixelCount = width * height;
    for ( uint32_t p = 0; p < pixelCount; ++p) {
        TransformPixel( dst, src, invert, srcIndex );
        dst += 4;
        src += 4;
    }
}

//======================================================================================================================
void Image::TransformPixel( uint8_t * dstRgba, const uint8_t * srcRgba, const bool * invert, const int32_t * srcIndex ) {
    
    for( uint32_t i = 0; i < 4; ++i ) {
        if ( srcIndex[ i ] < 0 ) {
            dstRgba[ i ] = 0;
        }
        else {
            uint8_t srcValue = srcRgba[ srcIndex[ i ] ];
            if ( invert[ i ] == true ) {
                srcValue = 255 - srcValue;
            }
            
            dstRgba[ i ] = srcValue;
        }
    }    
}
