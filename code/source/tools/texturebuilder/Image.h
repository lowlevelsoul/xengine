//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"

class Image : public RefObject {
public:
    typedef RefPointer<Image> ptr_t;
    
    enum FORMAT {
        FORMAT_NULL = 0,
        
        FORMAT_RGB_BYTE,
        FORMAT_RGBA_UBYTE,
        
        FORMAT_RGB_SHORT,
        FORMAT_RGBA_USHORT,
        
        FORMAT_RGB_INT,
        FORMAT_RGBA_UINT,
        
        FORMAT_RGB_FLOAT,
        FORMAT_RGBA_FLOAT,
        
        FORMAT_BLOCK_ETC,
        FORMAT_BLOCK_BC3
    };
    
    Image();

    virtual ~Image();
    
    void Create( FORMAT format_, uint32_t w, uint32_t h );
    
    static bool IsBlockFormat( FORMAT format ) {
        return ( format >= FORMAT_BLOCK_ETC && format <= FORMAT_BLOCK_BC3 );
    }
    
    void Load( const char * path, const char * transform = nullptr );
    
protected:
    
    uint32_t GetFilterIndex( const char filterChar );
    
    void TransformPixels( uint8_t * dst, const uint8_t * src, const char * transform );
    
    void TransformPixel( uint8_t * dstRgba, const uint8_t * srcRgba, const bool * invert, const int32_t * srcIndex );
    
public:
    FORMAT                  format = FORMAT_NULL;
    uint32_t                width = 0;              ///< Image width
    uint32_t                height = 0;             ///< Image height
    uint32_t                bpp = 0;                ///< Bytes perpixel
    uint32_t                pitch = 0;              ///< Number of bytes per image row
    uint32_t                blockWidth = 0;         ///< For block compressed formats, this is the width of a single block in pixels
    uint32_t                blockHeight = 0;        ///< For block compressed formats, this is the height of a single block in pixels
    uint32_t                blockCount = 0;         ///< Number of blocks in the image
    uint32_t                blockSize = 0;          ///< Block size in bytes
    uint32_t                blockCountX = 0;
    uint32_t                blockCountY = 0;
    uint32_t                size = 0;               ///< Total size of the image pixel data

    std::vector<uint8_t>      pixels;
    std::vector<uint8_t*>     rows;
    std::vector<uint8_t*>     blocks;
};

#endif
