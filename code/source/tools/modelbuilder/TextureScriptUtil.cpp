//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021-2023 James Steele. All Rights Reserved.
//======================================================================================================================

#include "TextureScriptUtil.h"

//======================================================================================================================
TextureScriptUtil::TextureScriptUtil() {
    
}

//======================================================================================================================
TextureScriptUtil::~TextureScriptUtil() {
    
}

//======================================================================================================================
void TextureScriptUtil::Initialise( const char * inputPath_, const char * outputPath_ ) {
    
    str_t tmp = NULL;
    Str_CopyCStr( &tmp, inputPath_ );
    Str_PathRemoveLastElement( &tmp );
    
    inputPath = tmp;
    
    Str_CopyCStr( &tmp, outputPath_ );
    Str_PathRemoveLastElement( &tmp );
    
    outputPath = tmp;

    Str_Destroy( &tmp );
}

//======================================================================================================================
void TextureScriptUtil::MakeMaterialTextureSourcePath( std::string & pathOut, const char * matTexName ) {
    str_t tmp = NULL;
    Str_CopyCStr( &tmp, inputPath.c_str() );
    Str_AppendPathCStr( &tmp, matTexName );
    pathOut = tmp;
    
    Str_Destroy( &tmp );
}

//======================================================================================================================
void TextureScriptUtil::MakeMaterialTextureDestPath( std::string & pathOut, const char * texName, const char * postfix ) {
    std::string texPath = inputPath;
    PathUtil::AppendPath( texPath, texName );
        
    PathUtil::RemoveExtension( texPath );

    if ( postfix != nullptr ) {
        texPath.append( postfix );
    }
    
    texPath.append( ".btex" );
    
    pathOut = texPath;
    pathOut[0] = '~';
}

//======================================================================================================================
void TextureScriptUtil::MakeMaterialScriptTexturePath( std::string & pathOut, SceneMaterialStage * stage ) {
    MakeMaterialScriptTexturePath( pathOut, stage->name.c_str() );
}

//======================================================================================================================
void TextureScriptUtil::MakeMaterialScriptTexturePath( std::string & pathOut, const char * texName ) {
    std::string texPath = inputPath;
    PathUtil::RemoveLastPathElement( texPath );
    PathUtil::AppendPath( texPath, texName );
    PathUtil::RemoveExtension( texPath );
    texPath.append(".btex");
    
    pathOut = texPath;
    pathOut[0] = '~';
}

//======================================================================================================================
void TextureScriptUtil::MakeTextureScriptPath( std::string & pathOut, const char * matTexName ) {    
    // Make sure we only have the name of the material and not containg folder
    // or file extension
    std::string tmp = matTexName;
    std::string matNameOnly;
    
    PathUtil::GetLastPathElement( matNameOnly, tmp );
    PathUtil::RemoveExtension( matNameOnly );
    
    // Now create the name of the material script, from model soruce path
    // and the material name
    pathOut = inputPath;
    PathUtil::AppendPath( pathOut, matNameOnly.c_str() );
    pathOut.append( ".btex.params" );
}

//======================================================================================================================
void TextureScriptUtil::WriteSingleTextureParams( const char * textureName, const char * texType ) {
    
    std::string textureDestPath;
    std::string textureSrcPath;
    std::string textureScriptPath;
    
    MakeMaterialTextureSourcePath( textureSrcPath, textureName );
    MakeMaterialTextureDestPath( textureDestPath, textureName );
    MakeTextureScriptPath( textureScriptPath, textureName );
    
    std::string scriptText = ToolApp::VFormat( "+image \"%s\"\n", textureSrcPath.c_str() );
    scriptText.append( "    +params \"@/textures.params\"\n" );
    scriptText.append( ToolApp::VFormat( "    +outfile \"%s\"\n", textureDestPath.c_str() ) );
    
    std::string scriptPath;
    MakeTextureScriptPath( scriptPath, textureName );
    
    {
        xeScopedFile file( scriptPath.c_str(), "wb" );
        xerror( file.IsValid() == false, "Error opening %s for writing\n", textureScriptPath.c_str() );
        file.Write( &scriptText[0], 1, scriptText.size() );
    }
}

//======================================================================================================================
void TextureScriptUtil::WriteAmrTexture( SceneMaterial * mat ) {
    bool hasMetallic = mat->HasMetallicTexture();
    bool hasRoughness = mat->HasRoughnessTexture();
    bool hasOcclusion = mat->HasOcclusionTexture();
    
    if ( hasMetallic == false && hasRoughness == false && hasOcclusion == false ) {
        // Nothing to do.
        return;
    }
    
    std::string scriptText = ToolApp::VFormat( "+amr_image\n" );
    
    // Write out outfile param
    std::string amrTextureName = mat->name.c_str();
    amrTextureName.append( "_amr" );
    
    std::string amrTextureDestPath;
    MakeMaterialTextureDestPath( amrTextureDestPath, amrTextureName.c_str() );
    scriptText.append( ToolApp::VFormat( "    +outfile \"%s\"\n",  amrTextureDestPath.c_str() ) );
    scriptText.append( "    +params \"@/textures.params\"\n" );
    
    // Write out available amr texture components
    
    // Occlusion
    if ( hasOcclusion == true ) {
        std::string textureSrcPath;
        MakeMaterialTextureSourcePath( textureSrcPath, mat->occlusion->texturePath.c_str() );
        
        std::string text = ToolApp::VFormat( "    +occlusion \"%s\"\n", textureSrcPath.c_str() );
        scriptText.append( text );
    }
    
    // Metallic
    if ( hasMetallic == true ) {
        std::string textureSrcPath;
        MakeMaterialTextureSourcePath( textureSrcPath, mat->metallic->texturePath.c_str() );
        
        std::string text = ToolApp::VFormat( "    +metallic \"%s\"\n", textureSrcPath.c_str() );
        scriptText.append( text );
    }
    
    // Roughness
    if ( hasRoughness == true ) {
        std::string textureSrcPath;
        MakeMaterialTextureSourcePath( textureSrcPath, mat->roughness->texturePath.c_str() );
        
        std::string text = ToolApp::VFormat( "    +roughness \"%s\"\n", textureSrcPath.c_str() );
        scriptText.append( text );
    }
    else if ( hasMetallic == true ) {
        // If there's no roughness texture, but there is a metallic texture, we'll
        // use that and invert the texture
        
        std::string textureSrcPath;
        MakeMaterialTextureSourcePath( textureSrcPath, mat->metallic->texturePath.c_str() );
        
        std::string text = ToolApp::VFormat( "    +roughness \"%s\"\n        +transform irigibia\n", textureSrcPath.c_str() );
        scriptText.append( text );
    }
    
    // Write out the texture script
    std::string scriptPath;
    std::string textureScriptName = mat->name.c_str();
    textureScriptName.append( "_amr");
    MakeTextureScriptPath( scriptPath, textureScriptName.c_str() );
    
    {
        xeScopedFile file( scriptPath.c_str(), "wb" );
        xerror( file.IsValid() == false, "Error opening %s for writing\n", scriptPath.c_str() );
        
        file.Write( &scriptText[0], 1, scriptText.size() );
    }
}
