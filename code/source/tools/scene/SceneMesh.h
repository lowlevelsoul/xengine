//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENEMESH_H__
#define __SCENEMESH_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "scene/SceneNode.h"
#include "scene/SceneGeometry.h"
#include "scene/SceneSkin.h"

class Scene;

class SceneMesh : public SceneNode {
public:
    typedef RefPointer<SceneMesh> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, M, e, s, h, space, space, space );
    
    SceneMesh();
    
    virtual ~SceneMesh();
    
    void MakeShell( std::vector<math ::Vec3> & posOut, std::vector<math ::Vec3> & normOut, std::vector<math ::Vec3> & texCoords, Scene * scene );
    
    
protected:
    
    void MakeShellStatic( std::vector<math ::Vec3> & posOut, std::vector<math ::Vec3> & normOut, std::vector<math ::Vec3> & texCoords, Scene * scene );
    
    void MakeShellSkin( std::vector<math ::Vec3> & posOut, std::vector<math ::Vec3> & normOut, std::vector<math ::Vec3> & texCoords, Scene * scene );

public:
    SceneGeometry::ptr_t    geometry;
    SceneSkin::ptr_t        skin;
};

#endif
