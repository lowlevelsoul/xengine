//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022, 2023 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __MATBUILDERAPP_H__
#define __MATBUILDERAPP_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "scene/Scene.h"
#include "matbuilder/MatBuilder.h"

class MatBuilderApp : public ToolApp {
public:
    MatBuilderApp();

    virtual ~MatBuilderApp();

    virtual bool HandleArg( Arg * arg, int32_t argId );

    virtual bool Process();

    virtual bool DisplayBuiltinArgHelp( TOOL_ARG_ID argId );

    void DisplayHelpText( int32_t argId );
    
public:
    class MaterialDef : public RefObject {
    public:
        typedef RefPointer<MaterialDef> ptr_t;
        typedef std::vector<ptr_t> array_t;
    
        std::string                     path;
        MatBuilder::MaterialInfo        materialInfo;
    };
    
    MaterialDef::array_t    materialDefs;
    MaterialDef *           currMat = nullptr;
    
};

#endif
