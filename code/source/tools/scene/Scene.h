//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENE_H__
#define __SCENE_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "scene/SceneNode.h"
#include "scene/SceneMaterial.h"
#include "scene/SceneAnimation.h"

class Scene : public RefObject {
public:
    typedef RefPointer<Scene> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    typedef void (*WalkCallback_t)( SceneNode * node, void * ctxt );
    
    Scene();
    
    virtual ~Scene();
    
    void AddMaterial( SceneMaterial * material );
    
    SceneMaterial * FindMaterial( const char * name );
    
    SceneNode * FindNode( const char * name );
    
    SceneNode * FindNode_r( SceneNode * curr, const char * name );
    
    void GatherNodesOfType( std::vector<SceneNode*> & array,  SceneNode::type_t nodeType );
    
    void WalkScene( void * ctxt, WalkCallback_t cb );
    
    void WalkSceneFromNode( SceneNode * node, void * ctxt, WalkCallback_t cb);
    
protected:
    
    void GatherNodesOfType_r( std::vector<SceneNode*> & array,  SceneNode::type_t nodeType, SceneNode * node );
    
    void WalkScene_r( SceneNode * node, void * ctxt, WalkCallback_t cb );
    
    
public:
    SceneNode::ptr_t        rootNode;               ///< Root node for the scene
    SceneMaterial::array_t  materials;              ///< List fo material used in the scene
    SceneAnimation::array_t animations;             ///<  List of animations
};

#endif
