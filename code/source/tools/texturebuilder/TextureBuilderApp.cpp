//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#include "TextureBuilderApp.h"
#include "TextureBuilder.h"
#include "AmrBuilder.h"

TOOL_APP( TextureBuilderApp )

enum ARG {
    ARG_IMAGE = 0,
    ARG_MAX_SIZE,
    ARG_OUTFILE,
    ARG_MIPCOUNT,
    ARG_BLOCKCOMPRESS,
    ARG_AMR_IMAGE,
    ARG_METALLIC,
    ARG_ROUGHNESS,
    ARG_OCCLUSION,
    ARG_TRANSFORM,
};

static const char * HELP_TEXT_IMAGE =
    "+image <src path>           Specifies an image to be loaded and built\n";

static const char * HELP_OUTFILE =
    "+outfile <size>            Set the path to the output image for the last image that was specified\n\n";

static const char * HELP_MAX_SIZE =
    "+max_size <size>           Sets the maximum size of the last image that was specified\n";

static const char * HELP_MIPCOUNT =
    "+mip_count <count>         Number of mip maps to generate for the last image that was specified\n";

static const char * HELP_BLOCKCOMPRESS =
    "+block_compress <format>   Last specified texture is block compessed in the specific block format\n"
    "                           Valid formats are: etc2|bc3";

static const char * HELP_AMR_IMAGE =
    "+amr_image                 Specifies an image that we want to build from several texture inputs\n";

static const char * HELP_METALLIC =
    "+metallic <src_path>        Soecifies an image that we want use as the metallic input for an amr image\n";

static const char * HELP_ROUGHNESS =
    "+metallic <src_path>        Specifies an image that we want use as theroughness input for an amr image\n";

static const char * HELP_OCCLUSION =
    "+occlusion <src_path>       Specifies an image that we want use as the ambient-occlusion input for an amr image\n";

static const char * HELP_TRANSFORM =
    "+transform <string>         Transforms the loaded image in a specific manner\n";


static const char * HELP_TEXT[] = {
    HELP_TEXT_IMAGE,
    HELP_OUTFILE,
    HELP_MAX_SIZE,
    HELP_MIPCOUNT,
    HELP_BLOCKCOMPRESS,
    HELP_METALLIC,
    HELP_ROUGHNESS,
    HELP_OCCLUSION,
    HELP_TRANSFORM,
    nullptr,
    nullptr,
    nullptr
};

//======================================================================================================================
TextureBuilderApp::TextureBuilderApp() {
    PublishArgId( ARG_IMAGE, "image" );
    PublishArgId( ARG_OUTFILE, "outfile" );
    PublishArgId( ARG_MAX_SIZE, "max_size" );
    PublishArgId( ARG_MIPCOUNT, "mip_count" );
    PublishArgId( ARG_BLOCKCOMPRESS, "block_compress" );
    PublishArgId( ARG_AMR_IMAGE, "amr_image" );
    PublishArgId( ARG_OCCLUSION, "occlusion" );
    PublishArgId( ARG_METALLIC, "metallic" );
    PublishArgId( ARG_ROUGHNESS, "roughness" );
    PublishArgId( ARG_TRANSFORM, "transform" );
}

//======================================================================================================================
TextureBuilderApp::~TextureBuilderApp() {
    
}

//======================================================================================================================
bool TextureBuilderApp::HandleArg( Arg * arg, int32_t argId ) {
    switch( argId ) {
        case ARG_IMAGE : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            currImage = new ImageEntry;
            currImage->srcPath = arg->m_params[ 0 ].c_str();
            currAmrImage = nullptr;
            
            images.push_back( currImage );
            
            break;
        }
            
        case ARG_MAX_SIZE : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currImage == nullptr, "No source image specified\n" );
            currImage->maxSize = atoi( arg->m_params[0].c_str() );
            break;
        }
            
        case ARG_OUTFILE : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currImage == nullptr, "No source image specified\n" );
            currImage->dstPath = arg->m_params[ 0 ].c_str();
            break;
        }

        case ARG_MIPCOUNT : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currImage == nullptr, "No source image specified\n" );
            currImage->mipCount = atoi( arg->m_params[0].c_str() );
            currImage->genMipsSet = true;
            
            break;
        }
            
        case ARG_BLOCKCOMPRESS : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currImage == nullptr, "No source image specified\n" );
            currImage->compression = TextureBuilder::CompressionStringToId( arg->m_params[0].c_str() );
            xerror( currImage->compression == TextureBuilder::COMPRESSION_NONE, "Invalid compression type '%s'\n", arg->m_params[0].c_str() );            
            currImage->blockCompressSet = true;
            
            break;
        }
            
        case ARG_AMR_IMAGE : {
            if ( arg->m_params.size() != 0 ) {
                DisplayHelpText( argId );
            }
            
            currImage = new ImageEntry;
            currImage->type = ImageEntry::TYPE_IMAGE_AMR;
            currAmrImage = currImage;
            images.push_back( currImage );
        
            break;
        }
            
        case ARG_METALLIC : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currAmrImage == nullptr, "No AMR image has been created for matallic image '%s'\n", arg->m_params[0].c_str() );
            
            currImage = new ImageEntry;
            currImage->srcPath = arg->m_params[0].c_str();
            currImage->type = ImageEntry::TYPE_METALLIC;
            
            currAmrImage->children.push_back( currImage );
            
            break;
        }
            
        case ARG_ROUGHNESS : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currAmrImage == nullptr, "No AMR image has been created for roughness image '%s'\n", arg->m_params[0].c_str() );
            
            currImage = new ImageEntry;
            currImage->srcPath = arg->m_params[0].c_str();
            currImage->type = ImageEntry::TYPE_ROUGHNESS;
            
            currAmrImage->children.push_back( currImage );
            break;
        }
            
        case ARG_OCCLUSION : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currAmrImage == nullptr, "No AMR image has been created for ambient-occulsion image '%s'\n", arg->m_params[0].c_str() );
            
            currImage = new ImageEntry;
            currImage->srcPath = arg->m_params[0].c_str();
            currImage->type = ImageEntry::TYPE_OCCLUSION;
            
            currAmrImage->children.push_back( currImage );
            break;
        }
            
        case ARG_TRANSFORM : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            currImage->transform = arg->m_params[0].c_str();
           
            break;
        }
            
    }
    return true;
}

//======================================================================================================================
bool TextureBuilderApp::Process() {
    
    // Load any images from files
    LoadImages();
    
    // Build any AMR images that are specified
    BuildAmrImages();
    
    // Process the images into mips, performaing compression and creating a btex stream
    // for each one
    for( uint32_t i = 0; i < images.size(); ++i ) {
        ImageEntry * ie = images[ i ];
        
        // By default, we block compress textures unless specified
        if ( ie->blockCompressSet == false && ie->compression == TextureBuilder::COMPRESSION_NONE ) {
            
            switch( targetPlatform ) {
                    
                case PLATFORM_NONE:
                    break;
                    
                case PLATFORM_MACOS:
                    ie->compression = TextureBuilder::COMPRESSION_BC3;
                    break;
                    
                case PLATFORM_IOS:
                case PLATFORM_TVOS:
                    ie->compression = TextureBuilder::COMPRESSION_ETC;
                    break;
            }
        }
        
        // We generate the maximum number of mips for a texture by default.
        if ( ie->genMipsSet == false && ie->mipCount == 0 ) {
            ie->mipCount = 32;
        }
        
        TextureBuilder::Options opt;
        opt.maxSize = ie->maxSize;
        opt.mipCount = ie->mipCount;
        opt.compression = ie->compression;
        
        TextureBuilder builder( opt );
        builder.Build( ie->str, ie->baseImage );
    }
    
    // Write out texture streams
    for( uint32_t i = 0; i < images.size(); ++i ) {
        ImageEntry * ie = images[ i ];
        
        std::string dstPath;
        
        if ( ie->dstPath.empty() == false ) {
            dstPath = ie->dstPath;
        }
        else {
            xerror( images.size() != 1, "Image destination path has not been specified when loading multiple images\n");
        }
        
        xprintf("Writing texture '%s'\n", dstPath.c_str() );
        
        CreateFolderAtPath( dstPath.c_str() );
        
        xeScopedFile file( dstPath.c_str(), "wb" );
        xerror( file.IsValid() == false, "Could not open file '%s' for writing\n", dstPath.c_str() );
        
        size_t writeAmt = file.Write( ie->str.GetStream(), 1, ie->str.Length() );
        xerror( writeAmt != ie->str.Length(), "Error writing to texture stream '%s'\n", dstPath.c_str() );
    }
    
    return true;
}

//======================================================================================================================
bool TextureBuilderApp::DisplayBuiltinArgHelp( TOOL_ARG_ID argId ) {
    return true;
}

//======================================================================================================================
void TextureBuilderApp::DisplayHelpText( int32_t argId ) {
    if ( argId < 0 ) {
        for ( int n = 0; HELP_TEXT[ n ] != nullptr; ++n ) {
            xprintf("%s", HELP_TEXT[ n ] );
        }
    }
    else {
        xprintf("%s", HELP_TEXT[ argId ] );
    }
    
   Sys_Exit( -1 );
}


//======================================================================================================================
void TextureBuilderApp::BuildAmrImages() {
    
    for( uint32_t i = 0; i < images.size(); ++i ) {
        ImageEntry * ie = images[ i ];
        
        if ( ie->type == ImageEntry::TYPE_IMAGE_AMR ) {
            xerror( ie->children.size() != 3, "AMR texture does not have the required number of source textures\n" );
            
            xprintf("Building AMR image from source images ");
            
            ImageEntry * occlusion = ie->GetAmrOcclusion();
            ImageEntry * roughness = ie->GetAmrRoughness();
            ImageEntry * metallic = ie->GetAmrMetallic();
            
            Image * occlusionImg = occlusion->baseImage;
            Image * roughnessImg = roughness->baseImage;
            Image * metallicImg = metallic->baseImage;
            
            AmrBuilder builder;
            ie->baseImage = builder.Build( occlusionImg, metallicImg, roughnessImg );
        }
    }
}

//======================================================================================================================
void TextureBuilderApp::LoadImages() {
    for( int n = 0; n < images.size(); ++n ) {
        ImageEntry * ie = images[ n ];
        LoadImage( ie );
    }
}
    
//======================================================================================================================
void TextureBuilderApp::LoadImage( ImageEntry * ie ) {
    
    if ( ie->srcPath.empty() == false ) {
        
        const char * transformStr = nullptr;
        if (ie->transform.empty() == false && ie->transform.size() != 0 ) {
            transformStr = &ie->transform[0];
        }
        
        ie->baseImage = new Image;
        ie->baseImage->Load( ie->srcPath.c_str(), transformStr );
    }
    
    for( int n = 0; n < ie->children.size(); ++n ) {
        ImageEntry * child = ie->children[ n ];
        LoadImage ( child );
    }
}
