//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __MUSE_SKELETONCOMPILER_H__
#define __MUSE_SKELETONCOMPILER_H__

#include "toolapp/ToolMemStream.h"
#include "render/SkeletonStream.h"
#include "scene/Scene.h"
#include "scene/SceneNode.h"

class SkeletonBuilder {
public:
    struct Options {
        std::string         rootNode;
        
        Options() {
            
        }
        
        ~Options() {
            
        }
        
        Options & operator=( const Options & rhs ) {
            rootNode = rhs.rootNode;
            return *this;
        }
        
        static const Options DEFAULT;
    };
    
    SkeletonBuilder( const Options & options_ = Options::DEFAULT );

    ~SkeletonBuilder();
    
    void Build( ToolMemStream & str, Scene * scene );
    
    void WriteToStream( ToolMemStream & str, size_t jointCount );
    
    void WriteHeader( ToolMemStream & str,  skeleton_stream_t & header );
    
    uint32_t WriteString( const char * name );
    
    int32_t FindJointIndex( const char * name );

public:
    Options                             options;
    
    ToolMemStream                       m_jointParentIndices;
    ToolMemStream                       m_jointMatrices;
    ToolMemStream                       m_jointBindMatrices;
    ToolMemStream                       m_jointHashArray;
    ToolMemStream                       m_jointHashMap;
    ToolMemStream                       m_jointStrings;
    ToolMemStream                       m_jointNames;
    
    // The joint name hash array/map is stored in the builder instance, so we can
    // reuase the data for joint name searches for things such as skin building
    std::vector<uint64_t>               nameHashArray;
    std::vector<uint32_t>               nameHashMap;
    math::Mat4                          offsetTransform = math::Mat4::IDENTITY;
};

#endif
