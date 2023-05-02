//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "scene/SceneAnimation.h"

//=======================================================================================================================
// SceneAnimJoint
//=======================================================================================================================

//=======================================================================================================================
void SceneAnimJoint::ConvertToFrames() {
    
    if ( positionKeyValues.size() < anim->frameCount ) {
        
        std::vector<math ::Vec3> posFrames;
        
        ConvertVec3KeysToFrames( posFrames, positionKeyValues, positionTimes );
        positionKeyValues = posFrames;
        positionTimes.clear();
    }
    
    if ( scaleKeyValues.size() < anim->frameCount ) {
        
        std::vector<math ::Vec3> scaleFrames;
        
        ConvertVec3KeysToFrames( scaleFrames, scaleKeyValues, scaleTimes );
        scaleKeyValues = scaleFrames;
        scaleTimes.clear();
    }
    
    if ( rotationKeyValues.size() < anim->frameCount ) {
        
        std::vector<math ::Quat> rotFrames;
        
        ConvertQuatKeysToFrames( rotFrames, rotationKeyValues, rotationTimes );
        rotationKeyValues = rotFrames;
        rotationTimes.clear();
    }
}

//=======================================================================================================================
void SceneAnimJoint::ConvertQuatKeysToFrames( std::vector<math ::Quat> & framesOut,
                                              std::vector<math ::Quat> & keyFrameValues,
                                              std::vector<float> & keyFrameTimes ) {
    if ( anim->frameCount != 0 ) {
        
        framesOut.reserve( anim->frameCount );
        
        int32_t frameIndex0 = 0;
        int32_t frameIndex1 = 1;
        
        do {
            if ( frameIndex1 == keyFrameValues.size() ) {
                framesOut.push_back( keyFrameValues[ frameIndex0 ] );
            }
            else {
                int32_t frameTime0 = keyFrameTimes[ frameIndex0 ];
                int32_t frameTime1 = keyFrameTimes[ frameIndex1 ];
                float frameDelta = ( float ) ( frameTime1 - frameTime0 );
                float frameTime0Flt = keyFrameTimes[ frameIndex0 ];
                float t = 0;
                
                for( int32_t f = frameTime0; f < frameTime1; ++f ) {
                    t = ( ( ( float ) f ) -  frameTime0Flt ) / frameDelta;
                    xassert( t >= 0 && t<= 1);
                    math ::Quat q;
                    q.Slerp( keyFrameValues[ frameIndex0 ], keyFrameValues[ frameIndex1 ], t );
                    framesOut.push_back( q );
                }
            }
            
            ++frameIndex0;
            ++frameIndex1;
            
        } while ( frameIndex1 <= keyFrameValues.size() );
    }
    else {
        
        int currKey=0, nextKey=1;
        float currTime = 0;
        float timeDelta = ( 1.0f / anim->fps );
        
        for( uint32_t i = 0; i < anim->frameCount; ++i ) {
            
            if ( currTime >= keyFrameTimes[ nextKey ] ) {
                ++currKey;
                ++nextKey;
            }
            
            float keyCurrTime  = keyFrameTimes[ currKey ];
            float keyNextTime  = keyFrameTimes[ nextKey ];
            float keyDeltaTime = keyNextTime - keyCurrTime;
            float t = ( currTime - keyCurrTime ) / keyDeltaTime;
            xassert( t>= 0 && t <= 1.0f);
            
            math ::Quat q;
            q.Slerp( keyFrameValues[ currKey ], keyFrameValues[ nextKey ], t );
            framesOut.push_back( q );
            
            currTime += timeDelta;
        }
    }
}

//=======================================================================================================================
void SceneAnimJoint::ConvertVec3KeysToFrames( std::vector<math ::Vec3> & framesOut,
                                              std::vector<math ::Vec3> & keyFrameValues,
                                              std::vector<float> & keyFrameTimes ) {
    
    if ( anim->frameCount != 0 ) {
        
        framesOut.reserve( anim->frameCount );
        
        int32_t frameIndex0 = 0;
        int32_t frameIndex1 = 1;
        
        do {
            if ( frameIndex1 == keyFrameValues.size() ) {
                framesOut.push_back( keyFrameValues[ frameIndex0 ] );
            }
            else {
                int32_t frameTime0 = keyFrameTimes[ frameIndex0 ];
                int32_t frameTime1 = keyFrameTimes[ frameIndex1 ];
                float frameDelta = ( float ) ( frameTime1 - frameTime0 );
                float frameTime0Flt = keyFrameTimes[ frameIndex0 ];
                float t = 0;
                
                for( int32_t f = frameTime0; f < frameTime1; ++f ) {
                    t = ( ( ( float ) f ) -  frameTime0Flt ) / frameDelta;
                    math ::Vec3 v;
                    v.Mix( keyFrameValues[ frameIndex0 ], keyFrameValues[ frameIndex1 ], t );
                    framesOut.push_back( v );
                }
            }
            
            ++frameIndex0;
            ++frameIndex1;
            
        } while ( frameIndex1 <= keyFrameValues.size() );
    }
    else {
        
        int currKey=0, nextKey=1;
        float currTime = 0;
        float timeDelta = ( 1.0f / anim->fps );
        
        for( uint32_t i = 0; i < anim->frameCount; ++i ) {
            
            if ( currTime >= keyFrameTimes[ nextKey ] ) {
                ++currKey;
                ++nextKey;
            }
            
            float keyCurrTime  = keyFrameTimes[ currKey ];
            float keyNextTime  = keyFrameTimes[ nextKey ];
            float keyDeltaTime = keyNextTime - keyCurrTime;
            float t = ( currTime - keyCurrTime ) / keyDeltaTime;
            xassert( t>= 0 && t <= 1.0f);
            
            math ::Vec3 v;
            v.Mix( keyFrameValues[ currKey ], keyFrameValues[ nextKey ], t );
                    
            framesOut.push_back( v );
            
            currTime += timeDelta;
        }
    }
}

//=======================================================================================================================
// SceneAnimation
//=======================================================================================================================

//=======================================================================================================================
SceneAnimation::SceneAnimation() {
    
}

//=======================================================================================================================
SceneAnimation::~SceneAnimation() {
    
}

//=======================================================================================================================
void SceneAnimation::ConvertToFrames() {
    for ( uint32_t j = 0; j < animJoints.size(); ++j ) {
        animJoints[ j ]->ConvertToFrames();
    }
}

//=======================================================================================================================
SceneAnimation * SceneAnimation::CreateFrameClip( int32_t frameStart, int32_t frameEnd ) {
    SceneAnimation * clip = new SceneAnimation;
    
    int frameCount = frameEnd - frameStart + 1;
    
    clip->frameCount    = frameCount;
    clip->fps           = fps;
    clip->duration      = (frameCount - 1) * (1.0f / (float) fps );
    
    for ( uint32_t j = 0; j < animJoints.size(); ++j ) {
        
        const SceneAnimJoint * srcJoint = animJoints[ j ];
        
        // Create joint and add to the animation
        SceneAnimJoint * dstJoint = new SceneAnimJoint( clip );
        
        dstJoint->name = srcJoint->name;
        dstJoint->transform = srcJoint->transform;
        
        clip->animJoints.push_back( dstJoint );
        
        // Copy rotation frames
        dstJoint->rotationKeyValues.reserve( frameCount );
        for( int32_t f = frameStart; f <= frameEnd; ++f ) {
            const math ::Quat & q = srcJoint->rotationKeyValues[ f ];
            dstJoint->rotationKeyValues.push_back( q );
        }
        
        // Copy position frames
        dstJoint->positionKeyValues.reserve( frameCount );
        for( int32_t f = frameStart; f <= frameEnd; ++f ) {
            const math ::Vec3 & p = srcJoint->positionKeyValues[ f ];
            dstJoint->positionKeyValues.push_back( p );
        }
        
        // Copy scale frames
        dstJoint->scaleKeyValues.reserve( frameCount );
        for( int32_t f = frameStart; f <= frameEnd; ++f ) {
            const math ::Vec3 & s = srcJoint->scaleKeyValues[ f ];
            dstJoint->scaleKeyValues.push_back( s );
        }
        
    }
    
    return clip;
}
