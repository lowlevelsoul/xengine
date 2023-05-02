//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENEMATERIAL_H__
#define __SCENEMATERIAL_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "scene/SceneNode.h"

class SceneMaterialStage : public SceneNode {
public:
    typedef RefPointer<SceneMaterialStage> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, M, a, t, S, t, a, g );
    
    SceneMaterialStage();
    
    virtual ~SceneMaterialStage();
    
public:
    math ::Vec4         colour = math ::Vec4::ZERO;
    std::string         texturePath;
    
};

class SceneMaterial : public SceneNode {
public:
    typedef RefPointer<SceneMaterial> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, M, a, t, e, r, i, a );
    
    SceneMaterial();
    
    virtual ~SceneMaterial();
    
    bool HasTexture( const SceneMaterialStage * stage ) const {
        return stage != nullptr && stage->texturePath.empty() == false;
    }
    
    bool HasAlbedoTexture() const { return HasTexture( albedo ); }
    
    bool HasRoughnessTexture() const { return HasTexture( roughness ); }
    
    bool HasMetallicTexture() const { return HasTexture( metallic ); }
    
    bool HasOcclusionTexture() const { return HasTexture( occlusion ); }
    
    bool HasGlowTexture() const { return HasTexture( glow ); }

public:
    SceneMaterialStage::ptr_t           albedo;
    SceneMaterialStage::ptr_t           roughness;
    SceneMaterialStage::ptr_t           metallic;
    SceneMaterialStage::ptr_t           occlusion;
    SceneMaterialStage::ptr_t           glow;
    
};

#endif
