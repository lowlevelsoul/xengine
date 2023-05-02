//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#include "AmrBuilder.h"

//======================================================================================================================
AmrBuilder::AmrBuilder() {
    
}

//======================================================================================================================
AmrBuilder::~AmrBuilder() {
    
}

//======================================================================================================================
Image * AmrBuilder::Build( Image * occlusion, Image * metallic, Image * roughness ) {
    
    xerror( occlusion == nullptr, "AMR texture is missing the ambient occlusion source image\n");
    xerror( metallic == nullptr, "AMR texture is missing the metallic source image\n");
    xerror( roughness == nullptr, "AMR texture is missing the roughness source image\n");
    
    bool sizeOk = VerifySizes( occlusion, metallic, roughness );
    xerror( sizeOk == false, "Not all source images for amr texture are the same size\n");
    
    Image * amr = new Image;
    amr->Create( Image::FORMAT_RGBA_UBYTE, occlusion->width, occlusion->height );
    
    for ( uint32_t y = 0; y < amr->height; ++y ) {
        const uint8_t * occlusionRow = occlusion->rows[ y ];
        const uint8_t * metallicRow = metallic->rows[ y ];
        const uint8_t * roughnessRow = roughness->rows[ y ];
        
        uint8_t * dstRow = amr->rows[ y ];
        
        for ( uint32_t x = 0; x < amr->width; ++x ) {
            dstRow[ 0 ] = (uint8_t) ((uint32_t) ( occlusionRow[0] + occlusionRow[1] + occlusionRow[2] ) / 3 );
            dstRow[ 1 ] = (uint8_t) ((uint32_t) ( metallicRow[0] + metallicRow[1] + metallicRow[2] ) / 3 );
            dstRow[ 2 ] = (uint8_t) ((uint32_t) ( roughnessRow[0] + roughnessRow[1] + roughnessRow[2] ) / 3 );
            dstRow[ 3 ] = 0;
            
            occlusionRow += 4;
            metallicRow += 4;
            roughnessRow += 4;
            dstRow += 4;
        }
    }
    
    return amr;
}

//======================================================================================================================
bool AmrBuilder::VerifySizes( Image * occlusion, Image * metallic, Image * roughness ) {
    Image * srcImages[ 3 ] = {
        occlusion, metallic, roughness
    };
    
    uint32_t w = 0; // occlusion->width;
    uint32_t h = 0; // occlusion->height;
    
    for ( int n = 0; n < 3; ++n ) {
        if ( srcImages[ n ] != nullptr && w == 0 ) {
            w = srcImages[ n ]->width;
            h = srcImages[ n ]->height;
        }
        else if ( srcImages[ n ] != nullptr ) {
            if ( w != srcImages[ n ]->width || h != srcImages[ n ]->height ) {
                return false;
            }
        }
    }
    
    return ( w != 0 );
}
