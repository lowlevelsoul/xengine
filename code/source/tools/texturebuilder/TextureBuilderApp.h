//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __TEXTUREBUILDERAPP_H__
#define __TEXTUREBUILDERAPP_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "texturebuilder/Image.h"
#include "TextureBuilder.h"

class TextureBuilderApp : public ToolApp {
public:
    TextureBuilderApp();

    virtual ~TextureBuilderApp();

    virtual bool HandleArg( Arg * arg, int32_t argId );

    virtual bool Process();

    virtual bool DisplayBuiltinArgHelp( TOOL_ARG_ID argId );
    
    void DisplayHelpText( int32_t argId );
    
    void LoadImages();
    
    void BuildAmrImages();
    
  
    
    void PushImage() {
        xerror( currImage == nullptr, "No currentky active image\n" );
        imageStack.push_back( currImage );
        currImage = nullptr;
    }
    
    void PopImage() {
        xerror( imageStack.empty() == true, "Image stack is empty\n" );
        currImage = imageStack[ imageStack.size() - 1 ];
        imageStack.pop_back();
    }
    
public:
    class ImageEntry : public RefObject {
    public:
        typedef RefPointer<ImageEntry> ptr_t;
        typedef std::vector<ptr_t> array_t;
        
        enum TYPE {
            TYPE_IMAGE,
            TYPE_IMAGE_AMR,
            TYPE_ROUGHNESS,
            TYPE_METALLIC,
            TYPE_OCCLUSION,
        };
        
        ImageEntry() {
            
        }
        
        virtual ~ImageEntry() {
            
        }
        
        ImageEntry * GetChildTextureOfType( TYPE type_ ) {
            for ( uint32_t i = 0; i < children.size(); ++i ) {
                if ( children[i]->type == type_ ) {
                    return children[i];
                }
            }
            
            return nullptr;
        }
        
        ImageEntry * GetAmrOcclusion() {
            return GetChildTextureOfType( ImageEntry::TYPE_OCCLUSION );
        }
        
        ImageEntry * GetAmrMetallic() {
            return GetChildTextureOfType( ImageEntry::TYPE_METALLIC );
        }
        
        ImageEntry * GetAmrRoughness() {
            return GetChildTextureOfType( ImageEntry::TYPE_ROUGHNESS );
        }
        
        TYPE                            type = TYPE_IMAGE;
        std::string                     srcPath;            ///< Path that the image
        std::string                     dstPath;            ///< Path to the dest image
        std::string                     transform;          
        Image::ptr_t                    baseImage;          ///< The base image that was loaded
        std::vector<ImageEntry::ptr_t>  children;           ///< List of
        uint32_t                        maxSize = 0;
        uint32_t                        mipCount = 0;
        TextureBuilder::COMPRESSION     compression = TextureBuilder::COMPRESSION_NONE;
        ToolMemStream                   str;
        bool                            blockCompressSet = false;
        bool                            genMipsSet = false;
    };
    
    void LoadImage( ImageEntry * ie );

    
    ImageEntry *                currImage = nullptr;
    ImageEntry *                currAmrImage = nullptr;
    ImageEntry::array_t         images;
    ImageEntry::array_t         imageStack;

    
};

#endif
