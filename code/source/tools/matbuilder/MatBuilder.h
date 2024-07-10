//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022, 2023 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __MATBUILDER_H__
#define __MATBUILDER_H__

#include "toolapp/ToolMemStream.h"
#include "render/MaterialStream.h"
#include "util/ParseLiteral.h"

class MatBuilder {
public:
    class MaterialInfo {
    public:
        enum STAGE {
            STAGE_ALBEDO=0,
            STAGE_AMR,
            STAGE_GLOW,
            STAGE_BUMP,
            STAGE_COUNT
        };
        
        std::string         name;
        std::string         texture[ STAGE_COUNT ];
        float               transparency = 1.0;
        
        MaterialInfo() {
            
        }
        
        ~MaterialInfo() {
            
        }

    };
    
    MatBuilder();

    ~MatBuilder();
    
    void Build( ToolMemStream & str, const char * scriptPath );
    
    void WriteHeader( ToolMemStream & str, material_stream_t & header );
    
    uintptr_t WriteString( const char * string );
    
    void BeginMaterial( parse_literal_t * name );
    
    void EndMaterial();
    
    void SetTexture( MaterialInfo::STAGE stage, parse_literal_t * texPath );

    void LogError( const char * fmt, ... );

    bool HasErrors() const { return hasErrors == true; }

    void IncLine() { ++line; }
    
    std::string ResolveTexturePath( const std::string & path );
        
public:
    int                             line = 1;
    bool                            hasErrors = false;
    std::string                     parseLog;
    std::vector<MaterialInfo*>      materials;
    
    ToolMemStream                   entries;
    ToolMemStream                   strings;
    
    MaterialInfo *                  currMaterial = nullptr;
};

#endif
