//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022, 2023 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __MATBUILDER_H__
#define __MATBUILDER_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "render/MaterialStream.h"

class MatBuilder {
public:
    class MaterialInfo {
    public:
        std::string         albedoTexture;
        std::string         amrTexture;
        std::string         glowTexture;
        std::string         normalTexture;
        float               transparency = 1.0;
        
        MaterialInfo() {
            
        }
        
        ~MaterialInfo() {
            
        }

    };
    
    MatBuilder();

    ~MatBuilder();
    
    void Build( ToolMemStream & str, MaterialInfo &info );
    
    void WriteHeader( ToolMemStream & str, material_stream_t & header );
    
    uintptr_t WriteString( ToolMemStream & str, const char * string );
    
public:
    ToolMemStream       strings;
};

#endif
