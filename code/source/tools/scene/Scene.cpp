//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "scene/Scene.h"

//=======================================================================================================================
Scene::Scene() {
    
}

//================================================= ======================================================================
Scene::~Scene() {
    
}

//=======================================================================================================================
void Scene::AddMaterial( SceneMaterial * material ) {
    materials.push_back( material );
}


//=======================================================================================================================
SceneMaterial * Scene::FindMaterial( const char * name ) {
    for ( int n = 0; n < materials.size(); ++n ) {
        if ( materials[ n ]->name == name  ) {
            return materials[ n ];
        }
    }
    
    return nullptr;
}


//==========================================================================================================================================
void Scene::GatherNodesOfType( std::vector<SceneNode*> & array,  SceneNode::type_t nodeType ) {
    GatherNodesOfType_r( array, nodeType, rootNode );
}

//==========================================================================================================================================
void Scene::GatherNodesOfType_r( std::vector<SceneNode*> & array,  SceneNode::type_t nodeType, SceneNode * node ) {
    if ( node->GetType() == nodeType ) {
        array.push_back( node );
    }
    
    for ( int c = 0; c < node->GetChildCount(); ++c ) {
        GatherNodesOfType_r( array, nodeType, node );
    }
}

//==========================================================================================================================================
SceneNode * Scene::FindNode( const char * name ) {
    return FindNode_r( rootNode, name );
}


//==========================================================================================================================================
SceneNode * Scene::FindNode_r( SceneNode * curr, const char * name ) {
    if ( curr->name == name ) {
        return curr;
    }
    
    for ( int c = 0; c < curr->GetChildCount(); ++c ) {
        SceneNode * found = FindNode_r( curr->GetChild( c ), name );
        if ( found != nullptr ) {
            return found;
        }
    }
    
    return nullptr;
}

//==========================================================================================================================================
void Scene::WalkScene( void * ctxt, WalkCallback_t cb) {
    WalkScene_r( rootNode, ctxt, cb );
}

//==========================================================================================================================================
void Scene::WalkSceneFromNode( SceneNode * node, void * ctxt, WalkCallback_t cb) {
    WalkScene_r( node, ctxt, cb );
}

//==========================================================================================================================================
void Scene::WalkScene_r( SceneNode * node, void * ctxt, WalkCallback_t cb ) {
    cb( node, ctxt );
    
    for( int c = 0; c < node->GetChildCount(); ++c ) {
        WalkScene_r( node->GetChild( c ), ctxt, cb );
    }
}
