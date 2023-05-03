//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022, 2023 James Steele. All Rights Reserved.
//======================================================================================================================

#include "matbuilder/MatBuilder.h"

//======================================================================================================================
MatBuilder::MatBuilder() {
    
}

//======================================================================================================================
MatBuilder::~MatBuilder() {
    
}

//======================================================================================================================
void MatBuilder::Build( ToolMemStream & str, MaterialInfo &info ) {
    
    material_stream_t header;
    memset( &header, 0, sizeof( header ) );
    header.version      = MATERIAL_STREAM_VERSION;
    header.transparency = info.transparency;
    
    uintptr_t offsHeader = str.Tell();
    WriteHeader( str, header );
    
    if ( info.albedoTexture.size() != 0 ) {
        uintptr_t offs = WriteString( str, info.albedoTexture.c_str() );
        offs -= offsHeader;
                            
        header.offsAlbedoTexture = (uint32_t) offs;
    }
    
    if ( info.amrTexture.size() != 0 ) {
        uintptr_t offs = WriteString( str, info.amrTexture.c_str() );
        offs -= offsHeader;
                            
        header.offsAmrTexture = (uint32_t) offs;
    }
    
    if ( info.glowTexture.size() != 0 ) {
        uintptr_t offs = WriteString( str, info.glowTexture.c_str() );
        offs -= offsHeader;
                            
        header.offsGlowTexture = (uint32_t) offs;
    }
    
    uintptr_t endPos = str.Length();
    str.Seek( offsHeader );
    WriteHeader( str, header );
    str.Seek( endPos );    
}

//======================================================================================================================
void MatBuilder::WriteHeader( ToolMemStream & str, material_stream_t & header ) {
    str.Write( &header.version );
    str.Write( &header.flags );
    str.Write( &header.offsAlbedoTexture );
    str.Write( &header.offsAmrTexture );
    str.Write( &header.offsGlowTexture );
    str.Write( &header.transparency );
}

//======================================================================================================================
uintptr_t MatBuilder::WriteString( ToolMemStream & str, const char * string ) {
    size_t len = strlen( string );
    uintptr_t offs = (uint32_t) str.Tell();
    
    str.Write( string, len + 1 );
    return offs;
}
