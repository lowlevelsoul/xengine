//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "scene/SceneNode.h"

//=======================================================================================================================
SceneNode::SceneNode() {
    
}

//=======================================================================================================================
SceneNode::~SceneNode() {
    
}

//=======================================================================================================================
void SceneNode::AddChild( SceneNode * child ) {
    xassert( child->GetParent() == nullptr );
    child->parent = this;
    children.push_back( child );
}

//=======================================================================================================================
math ::Mat4 SceneNode::EvaulateWorldTransform() const {
    if ( parent == nullptr ) {
        // No parent, so just return the world transform
        return transform;
    }
    
    // Node has a parent - so evaluate parent world transform,
    // add parent to local transform for this node and return result.
    
    math ::Mat4 parentWorld = parent->EvaulateWorldTransform();
    math ::Mat4 worldTransform;
    worldTransform.Concat( GetTransform(), parentWorld );
    
    return worldTransform;
}

//=======================================================================================================================
void SceneNode::RemoveChild( SceneNode * child ) {
    int pos = -1;
    
    if ( child->GetParent() != this ) {
        return;
    }
    
    for ( int n = 0; n < GetChildCount(); ++n ) {
        if ( GetChild( n ) == child ) {
            pos = n;
            break;
        }
    }
    
    if ( pos >= 0 ) {
        child->parent = nullptr;
        children.erase( children.begin() + pos );
    }
}
