//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENESKIN_H__
#define __SCENESKIN_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "scene/SceneNode.h"

class SceneSkinCluster : public SceneNode {
public:
    typedef RefPointer<SceneSkinCluster> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, S, k, i, n, C, l, u );
    
    SceneSkinCluster();
    
    virtual ~SceneSkinCluster();

public:
    std::string             linkNode;
    math ::Mat4          linkTransform = math ::Mat4::IDENTITY;
    std::vector<uint32_t>     vertices;
    std::vector<float>        weights;
    
};

class SceneSkin : public SceneNode {
public:
    typedef RefPointer<SceneSkin> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, S, k, i, n, space, space, space );
    
    SceneSkin();
    
    virtual ~SceneSkin();

public:
    SceneSkinCluster::array_t        clusters;
};

#endif
