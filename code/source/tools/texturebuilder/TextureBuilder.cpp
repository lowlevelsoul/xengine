//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#include "texturebuilder/TextureBuilder.h"
#include "avir.h"
#include "squish.h"


const TextureBuilder::Options TextureBuilder::Options::DEFAULT;

//======================================================================================================================
TextureBuilder::TextureBuilder( const Options & options_ ) {
    options = options_;
}

//======================================================================================================================
TextureBuilder::~TextureBuilder() {
    
}

//======================================================================================================================
void TextureBuilder::Build( ToolMemStream & str, Image * srcImage ) {
    SetBaseImage( srcImage );
    
    GenerateMips();
    
    if ( options.compression != COMPRESSION_NONE ) {
        // We want compressed images, so generate block compressed versions based on
        // on the desired compression type
        CompressImages();
        
        WriteToStream( str, imagesCompressed );
    }
    else {
        // No compression - just write out the normal base image and mips
        WriteToStream( str, images );
    }
}

//======================================================================================================================
void TextureBuilder::SetBaseImage( Image * srcImage ) {
    if ( options.maxSize > 0 && ( srcImage->width > options.maxSize || srcImage->height > options.maxSize ) ) {
        // Resize base image
        
        // Calc hew width and size of the base image
        uint32_t newWidth = srcImage->width;
        uint32_t newHeight = srcImage->height;
        
        if ( srcImage->width > srcImage->height ) {
            newWidth = options.maxSize;
            newHeight = (uint32_t) ( ( float ) options.maxSize * ( (float) srcImage->height / (float) srcImage->width ) );
        }
        else if ( srcImage->height > srcImage->width ) {
            newHeight = options.maxSize;
            newWidth = (uint32_t) ( ( float ) options.maxSize * ( (float) srcImage->width / (float) srcImage->height ) );
        }
        else {
            newWidth = options.maxSize;
            newHeight = options.maxSize;
        }
        
        xprintf(" Resizing base image to %u x %u\n", newWidth, newHeight );
        
        // Instance fot the resized image
        Image * resizedImage = new Image;
        resizedImage->Create( srcImage->format, newWidth, newHeight );
        
        // Resize using avid from srcImage into resizedImage
        avir::CImageResizer<> ImageResizer( 8 );
        ImageResizer.resizeImage( &srcImage->pixels[0], srcImage->width, srcImage->height, 0,
                                  &resizedImage->pixels[0], newWidth, newHeight, 4, 0 );
        
        // Resized image is our new base image
        images.push_back( resizedImage );
    }
    else {
        // Not resizing the base image. So just make the src image the base image
        images.push_back( srcImage );
    }
}

//======================================================================================================================
void TextureBuilder::CompressImages() {
    if ( options.compression == COMPRESSION_BC3 ) {
        
        xprintf(" Compressing images to BC3\n" );
        
        for ( uint32_t i = 0; i < images.size(); ++i ) {
            xprintf("   Image %u (%u x %u)\n", i, images[ i ]->width, images[ i ]->height );
            
            Image * compImage = CompresToBC3( images[ i ] );
            imagesCompressed.push_back( compImage );
        }
    }
    else {
        xerror( true, "Unsupported compresion type\n");
    }
}


//======================================================================================================================
void TextureBuilder::GenerateMips() {
    // No mips to generate? Just bail...
    if ( options.mipCount == 0 ) {
        return;
    }
    
    xprintf("Generating mips...\n");
    
    Image * base = images[ 0 ];
    
    uint32_t mipWidth = base->width / 2;
    uint32_t mipHeight = base->height / 2;
    
    for ( uint32_t m = 0; m < options.mipCount; ++m ) {
        xprintf("  %u (%u x %u)\n", m+ 1, mipWidth, mipHeight );
        
        // Instance for the resized image
        Image * mipImage = new Image;
        mipImage->Create( base->format, mipWidth, mipHeight );
        
        // Resize using avid from srcImage into resizedImage
        avir::CImageResizer<> ImageResizer( 8 );
        ImageResizer.resizeImage( &base->pixels[0], base->width, base->height, 0,
                                  &mipImage->pixels[0], mipWidth, mipHeight, 4, 0 );
        
        images.push_back( mipImage );
        
        mipWidth /= 2;
        mipHeight /= 2;
        
        // Guard against making very, very small mips
        if ( mipWidth < options.minMipSize || mipHeight < options.minMipSize ) {
            break;
        }
    }
}


//======================================================================================================================
void TextureBuilder::WriteToStream( ToolMemStream & str, std::vector<Image::ptr_t> & images ) {
    
    tex_stream_t header;
    memset( &header, 0, sizeof(header) );
    header.version        = TEXSTREAM_VERSION;
    header.width          = images[ 0 ]->width;
    header.height         = images[ 0 ]->height;
    header.mipCount       = ( uint32_t ) ( images.size() - 1 );
    
    if ( options.compression == COMPRESSION_NONE ) {
        header.format = TEXSTREAM_FORMAT_RGBA_U8;
    }
    else {
        if ( options.compression == COMPRESSION_ETC ) {
            header.format = TEXSTREAM_FORMAT_RGBA_ETC2;
        }
        else if ( options.compression == COMPRESSION_BC3 ) {
            header.format = TEXSTREAM_FORMAT_RGBA_BC3;
        }
    }
    
    // Write out the partially completed header
    uintptr_t offsHeader = str.Tell();
    WriteHeader( str, header );
    
    // Write out the images
    for ( uint32_t i = 0; i < images.size(); ++i ) {
        header.offsImages[ i ] = ( uint32_t ) ( str.Tell() - offsHeader );
        str.Write( &images[i]->pixels[0], images[i]->pixels.size() );
    }
    
    // Now that the header has been completely filled out, we need to re-write it
    uintptr_t endPos = str.Tell();
    str.Seek( offsHeader );
    WriteHeader( str, header );
    str.Seek( endPos );
}

//======================================================================================================================
void TextureBuilder::WriteHeader( ToolMemStream & str, tex_stream_t & header ) {
    str.Write( &header.version );
    str.Write( &header.flags );
    str.Write( &header.format );
    str.Write( &header.width );
    str.Write( &header.height );
    str.Write( &header.mipCount );
    str.Write( header.offsImages, TEXSTREAM_MAX_MIP_COUNT );
}

//======================================================================================================================
Image * TextureBuilder::CompresToBC3( Image * src ) {
    Image * squished = new Image;
    squished->Create( Image::FORMAT_BLOCK_BC3, src->width, src->height );
    
    squish::CompressImage( &src->pixels[0], src->width, src->height, &squished->pixels[0], squish::kDxt5 );
    
    return squished;
    
}

//======================================================================================================================
TextureBuilder::COMPRESSION TextureBuilder::CompressionStringToId( const char * str ) {
    if ( strcasecmp( str, "bc3") == 0) {
        return COMPRESSION_BC3;
    }
    else  if ( strcasecmp( str, "etc2") == 0) {
        return COMPRESSION_ETC;
    }
    
    return COMPRESSION_NONE;
}
