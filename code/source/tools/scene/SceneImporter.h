//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENEIMPORTER_H__
#define __SCENEIMPORTER_H__

#include "scene/Scene.h"

class SceneImporter {
public:
    struct Options {
        float           scale = 1;
        bool            flipFaces = false;
        bool            importMeshes = true;
        bool            importAnims = true;
        std::string     rootNode;
        
        static const Options DEFAULT;
    };
    
    SceneImporter( const Options & options_ = Options::DEFAULT );
    
    virtual ~SceneImporter();
    
    virtual Scene::ptr_t Import( const char * path ) = 0;
    
protected:
    Options         options;
};

#endif
