//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENEANIMATION_H__
#define __SCENEANIMATION_H__

#include "scene/SceneNode.h"

class SceneAnimation;

class SceneAnimJoint : public SceneNode {
public:
    typedef RefPointer<SceneAnimJoint> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, A, n, i, m, J, n, t );
    
    SceneAnimJoint( SceneAnimation * anim_ ) {
        anim = anim_;
        type = TYPE;
    }
    
    virtual ~SceneAnimJoint() {
        
    }
    
    void ConvertToFrames();
    
    void ConvertVec3KeysToFrames( std::vector<math ::Vec3> & framesOut,
                                  std::vector<math ::Vec3> & keyFrameValues,
                                  std::vector<float> & keyFrameTimes );
    
    void ConvertQuatKeysToFrames( std::vector<math ::Quat> & framesOut,
                                  std::vector<math ::Quat> & keyFrameValues,
                                  std::vector<float> & keyFrameTimes );

public:
    SceneAnimation *                anim;
    
    std::vector<math ::Vec3>       positionKeyValues;
    std::vector<float>                positionTimes;
    
    std::vector<math ::Vec3>       scaleKeyValues;
    std::vector<float>                scaleTimes;
    
    std::vector<math ::Quat>       rotationKeyValues;
    std::vector<float>                rotationTimes;
};

class SceneAnimation : public SceneNode {
public:
    typedef RefPointer<SceneAnimation> ptr_t;
    typedef std::vector<ptr_t> array_t;
    
    static constexpr type_t TYPE = MAKE_ID( S, c, e, n, e, A, n, i, m, space, space, space );
    
    SceneAnimation();
    
    virtual ~SceneAnimation();
    
    void ConvertToFrames();
    
    
    SceneAnimation * CreateFrameClip( int32_t frameStart, int32_t frameEnd );

public:
    uint32_t                        frameCount = 0;     ///< Number of frames for frame animations
    uint32_t                        fps = 0;            ///< Number of frames per second for frame animations
    float                           duration = 0;       ///< Duration of the animation in seconds
    SceneAnimJoint::array_t         animJoints;         ///< List of animaed joints
};

#endif
