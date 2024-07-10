//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022, 2023 James Steele. All Rights Reserved.
//======================================================================================================================

#include "matbuilder/MatBuilder.h"

extern bool MaterialScript_Parse( file_t * file, MatBuilder * matBuilder_ );

//======================================================================================================================
MatBuilder::MatBuilder() {
    line = 1;
}

//======================================================================================================================
MatBuilder::~MatBuilder() {
    
}

//======================================================================================================================
void MatBuilder::Build( ToolMemStream & str, const char * scriptPath ) {
    
    file_t scriptFile;
    bool_t openOk = FS_FileOpen( &scriptFile, scriptPath, "rb" );
    xerror( openOk == false, "Unable to open material script file \"%s\"", scriptPath );
    
    bool parseResult = MaterialScript_Parse( &scriptFile, this );
    xerror( parseResult == false, "Error parsing material script\n%s\n", this->parseLog.c_str() );
    
    material_stream_t header;
    memset( &header, 0, sizeof( header ) );
    header.version      = MATERIAL_STREAM_VERSION;
    header.count        = (uint32_t ) materials.size();
    
    for ( uint32_t m = 0; m < materials.size(); ++m ) {
        MaterialInfo * info = materials[ m ];
        material_stream_entry_s entry;
        memset( &entry, 0, sizeof( entry ) );
        entry.alpha = 1;
        
        entry.offsName =  entry.offsName = ( uint32_t ) WriteString( info->name.c_str() );
        
        uint32_t offsets[ MaterialInfo::STAGE_COUNT ];
        for ( uint32_t t = 0; t < MaterialInfo::STAGE_COUNT; ++t ) {
            offsets[ t ] = 0;
            
            if ( info->texture[ t ].empty() == false ) {
                std::string path = ResolveTexturePath( info->texture[ MaterialInfo::STAGE(t) ].c_str() );
                offsets[ t ] = ( uint32_t ) WriteString( path.c_str() );
            }
        }
        
        entry.offsAlbedoTexture = offsets[ MaterialInfo::STAGE_ALBEDO ];
        entry.offsAmrTexture = offsets[ MaterialInfo::STAGE_AMR ];
        entry.offsGlowTexture = offsets[ MaterialInfo::STAGE_GLOW ];
        entry.offsBumpTexture = offsets[ MaterialInfo::STAGE_BUMP ];
        
        entries.Write( ((uint8_t*) &entry), sizeof( entry ) );
    }
    
    
    uintptr_t offsHeader = str.Tell();
    WriteHeader( str, header );
    
    str.PadToAlignment( 64 );
    header.offsMaterials = ( uint32_t ) ( str.Tell() - offsHeader );
    str.Write( entries );
    
    str.PadToAlignment( 64 );
    header.offsStrings = ( uint32_t ) ( str.Tell() - offsHeader );
    str.Write( strings );
    
    uintptr_t endPos = str.Length();
    str.Seek( offsHeader );
    WriteHeader( str, header );
    str.Seek( endPos );
    
    str.PadToAlignment( 64 );
}

//======================================================================================================================
std::string MatBuilder::ResolveTexturePath( const std::string & path ) {
    str_t newPath = nullptr;
    
    if ( path[ 0 ] == '~') {
        Str_CopyCStr( &newPath, path.c_str() );
    }
    else if ( path[ 0 ] == '@' ) {
        Str_CopyCStr( &newPath, path.c_str() );
        newPath[ 0 ] = '~';
    } else {
        Str_CopyCStr( &newPath, "~/textures/");
        Str_AppendPathCStr( &newPath, path.c_str() );
    }
    
    Str_PathRemoveExtension( &newPath );
    Str_AppendCStr( &newPath, ".btex" );
    
    std::string retStr = newPath;
    Str_Destroy( &newPath );
    
    return retStr;
}

//======================================================================================================================
void MatBuilder::WriteHeader( ToolMemStream & str, material_stream_t & header ) {
    str.Write( &header.version );
    str.Write( &header.flags );
    str.Write( &header.count );
    str.Write( &header.offsMaterials );
    str.Write( &header.offsStrings );
    str.Write( &header.pad );
}

//======================================================================================================================
uintptr_t MatBuilder::WriteString( const char * string ) {

    size_t len = strlen( string );
    uintptr_t offs = (uint32_t) strings.Tell();
    strings.Write( string, len + 1 );
    
    
    return offs;
}

//======================================================================================================================
void MatBuilder::LogError( const char * fmt, ... ) {
    /* initialize use of the variable argument array */
    va_list vaArgs;
    va_start( vaArgs, fmt );

    std::string msg = ToolApp::VFormat( fmt, vaArgs );
    va_end( vaArgs );
    
    std::string logMsg = ToolApp::VFormat("%i : ", line );
    logMsg.append( msg );
    logMsg.append("\n");
    
    parseLog.append( logMsg );
    hasErrors = true;
}

//======================================================================================================================
void MatBuilder::BeginMaterial( parse_literal_t * name ) {
    currMaterial = new MaterialInfo;
    materials.push_back( currMaterial );
    
    xassert( name->type == PARSE_LITERAL_STRING );
    currMaterial->name = name->value.stringVal;
    
    xprintf("Adding material '%s'\n", name->value.stringVal );
}

//======================================================================================================================
void MatBuilder::EndMaterial() {
    currMaterial = nullptr;
}

//======================================================================================================================
void MatBuilder::SetTexture( MaterialInfo::STAGE stage, parse_literal_t * texPath ) {
    xassert( texPath->type == PARSE_LITERAL_STRING );
    currMaterial->texture[ stage ] = texPath->value.stringVal;
}
