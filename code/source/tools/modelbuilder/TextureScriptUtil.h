//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021-2023 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __TEXTURESCRIPTBUILDER_H__
#define __TEXTURESCRIPTBUILDER_H__

#include "toolapp/ToolMemStream.h"
#include "render/ModelStream.h"
#include "modelbuilder/SkeletonBuilder.h"

#include "scene/Scene.h"
#include "scene/SceneMesh.h"
#include "scene/SceneNode.h"

class TextureScriptUtil {
public:
    TextureScriptUtil();
    
    ~TextureScriptUtil();
    
    void Initialise( const char * inputPath, const char * outputPath );
    
    void MakeMaterialScriptTexturePath( std::string & pathOut, SceneMaterialStage * stage );
    
    void MakeMaterialScriptTexturePath( std::string & pathOut, const char * texName );
    
    void MakeTextureScriptPath( std::string & pathOut, const char * matTexName);
    
    void MakeMaterialTextureSourcePath( std::string & pathOut, const char * matTexName );
    
    void MakeMaterialTextureDestPath( std::string & pathOut, const char * texName, const char * postfix = nullptr );
    
    void WriteSingleTextureParams( const char * textureName, const char * texType );
    
    void WriteAmrTexture( SceneMaterial * mat );
    
protected:
    std::string      inputPath;          ///> Path to the input textures
    std::string      outputPath;         ///< Path to the output textures
};



#endif 
