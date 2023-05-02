//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENENODE_H__
#define __SCENENODE_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include <vector>
#include <string>

class SceneNode : public RefObject {
public:
    typedef RefPointer<SceneNode> ptr_t;
    typedef std::vector<ptr_t> array_t;
    typedef uint64_t type_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, N, o, d, e, space, space, space );
    
    SceneNode();
    
    virtual ~SceneNode();
    
    type_t GetType() const { return type; }
    
    size_t GetChildCount() const { return children.size(); }
    
    SceneNode * GetChild( uint32_t index ) { xassert( index < GetChildCount() ); return children[ index ]; }
    
    const SceneNode * GetChild( uint32_t index ) const { xassert( index < GetChildCount() ); return children[ index ]; }
    
    SceneNode * GetParent() { return parent; }
    
    const SceneNode * GetParent() const { return parent; }
    
    void AddChild( SceneNode * child );
    
    const math ::Mat4 & GetTransform() const { return transform; }
    
    void SetTransform( const math ::Mat4 & transform_ ) { transform = transform_; }
    
    math ::Mat4 EvaulateWorldTransform() const;

    void RemoveChild( SceneNode * child );
    
public:
    std::string         name;                                       ///< Node name
    math ::Mat4      transform = math ::Mat4::IDENTITY;       ///< Local transform for node
    array_t             children;                                   ///< Array of children
    SceneNode *         parent = nullptr;                           ///< Pointer to the parent node
    type_t              type = SceneNode::TYPE;                     ///< Node type
};

#endif
