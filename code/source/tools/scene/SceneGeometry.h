//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENEGEOMETRY_H__
#define __SCENEGEOMETRY_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "scene/SceneNode.h"
#include "scene/SceneMaterial.h"

class SceneTriangle {
public:
    typedef std::vector<SceneTriangle> array_t;
    
    static constexpr size_t NUM_VERTS = 3;
    
    uint32_t        verts[ NUM_VERTS ];
    
    SceneTriangle() {
        
    }
    
    SceneTriangle( uint32_t v0, uint32_t v1, uint32_t v2 ) {
        Set( v0, v1, v2 );
    }
    
    void Set( uint32_t v0, uint32_t v1, uint32_t v2 ) {
        verts[ 0 ] = v0;
        verts[ 1 ] = v1;
        verts[ 2 ] = v2;
    }
};

class SceneGeometry : public SceneNode {
public:
    typedef RefPointer<SceneGeometry> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, G, e, o, m, space, space, space );
    
    SceneGeometry();
    
    virtual ~SceneGeometry();

public:
    std::vector<math ::Vec3>       positions;
    std::vector<math ::Vec3>       normals;
    std::vector<math ::Vec3>       texCoords;
    SceneTriangle::array_t          triangles;
    SceneMaterial::ptr_t            material;
};

#endif
