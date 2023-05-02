//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __TEXTUREBUILDER_H__
#define __TEXTUREBUILDER_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "texturebuilder/Image.h"
#include "render/TexStream.h"

class TextureBuilder {
public:
    enum COMPRESSION {
        COMPRESSION_NONE = 0,
        COMPRESSION_ETC,
        COMPRESSION_BC3,
    };
    
    enum AMR {
        AMR_NONE = 0,
        AMR_AMBIENT_OCCLUSION,
        AMR_METALLIC,
        AMR_ROUGHNESS,
    };
    
    struct Options {
        
        Options() {
        }
        
        ~Options() {
        }

        
        COMPRESSION     compression = COMPRESSION_NONE;
        uint32_t        maxSize = 0;
        uint32_t        mipCount = 0;
        uint32_t        minMipSize = 64;
        
        static const Options DEFAULT;
    };
    
    TextureBuilder( const Options & options_ = Options::DEFAULT );

    ~TextureBuilder();
    
    void Build( ToolMemStream & str, Image * srcImage );
    
    void SetBaseImage( Image * srcImage );
    
    void GenerateMips();
    
    void WriteHeader( ToolMemStream & str, tex_stream_t & header );
    
    void WriteToStream( ToolMemStream & str, std::vector<Image::ptr_t> & images );
    
    void CompressImages();
    
    Image * CompresToBC3( Image * src );
    
    static COMPRESSION CompressionStringToId( const char * str );
    
public:
    Options                         options;
    std::vector<Image::ptr_t>     images;                     ///< List of images that we want to save in the tex
    std::vector<Image::ptr_t>     imagesCompressed;           ///< List of compressed versions of the images
};

#endif
