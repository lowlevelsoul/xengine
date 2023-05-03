//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __ANIMBUILDER_H__
#define __ANIMBUILDER_H__

#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "anim/AnimStream.h"
#include "scene/SceneAnimation.h"
#include "modelbuilder/SkeletonBuilder.h"

class AnimBuilder : public RefObject {
public:
    struct Options {
        float scale = 1;
        std::string rootNode;
        
        Options() {
        }
        
        static const Options DEFAULT;
    };
    
    AnimBuilder( const Options & options_ = Options::DEFAULT );
    
    ~AnimBuilder();
    
    void Build( ToolMemStream & str, Scene * scene, SceneAnimation * anim );
    
    void Write( ToolMemStream & str, SceneAnimation * anim );

    uintptr_t WriteString( const char * string );
    
    void WriteHeader( ToolMemStream & str, fx::AnimStream & header );
    
    void BuildAnimJoints( fx::Array<SceneAnimJoint*> & joints );
    
    void BuildFrameData( SceneAnimation * anim, fx::Array<SceneAnimJoint*> & sortedJoints );
    
    void WriteNodes();
    
    
    
    
protected:
    Options             options;
    ToolMemStream       animDataStream;
    ToolMemStream       nodeStream;
    ToolMemStream       nodeNameArrayStream;
    ToolMemStream       nodeNodeMapStream;
    ToolMemStream       strings;
    ToolMemStream       frameRotationOffsets;
    ToolMemStream       framePositionOffsets;
    ToolMemStream       frameRotations;
    ToolMemStream       framePositions;
    fx::math::Mat4      baseTransform = fx::math::Mat4::IDENTITY;      
    
    fx::Array<fx::AnimStreamNode> animNodes;
    
    SkeletonBuilder *   skeletonBuilder;
    
};

#endif
