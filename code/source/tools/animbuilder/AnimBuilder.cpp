//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "AnimBuilder.h"
#include "container/Array.h"

const AnimBuilder::Options AnimBuilder::Options::DEFAULT;

//=======================================================================================================================
AnimBuilder::AnimBuilder( const Options & options_  ) {
    options = options_;
}

//=======================================================================================================================
AnimBuilder::~AnimBuilder() {
    
}

//=======================================================================================================================
void AnimBuilder::Build( ToolMemStream & str, Scene * scene, SceneAnimation * anim ) {
    
    // We need to build the reference skeleton so that we can ensure the animation nodes are in the correct order
    SkeletonBuilder::Options skelOpt;
    skelOpt.rootNode = options.rootNode;
    
    ToolMemStream skelStr;
    skeletonBuilder = new SkeletonBuilder( skelOpt );
    skeletonBuilder->Build( skelStr, scene);
    
    fx::Array<SceneAnimJoint*> sortedAnimJoints;
    sortedAnimJoints.SetLength( anim->animJoints.GetLength() );
    
    for ( uint32_t n = 0; n != anim->animJoints.GetLength(); ++n ) {
        SceneAnimJoint * animJoint = anim->animJoints[ n ];
        
        int32_t skelIndex = skeletonBuilder->FindJointIndex( animJoint->name.c_str() );
        fxerror( skelIndex < 0, "Could not find skeleton joint for anim joint '%'\n", animJoint->name.c_str() );
        
        sortedAnimJoints[ skelIndex ] = animJoint;
    }
    
    BuildAnimJoints( sortedAnimJoints );
    
    BuildFrameData( anim, sortedAnimJoints );
    
    Write( str, anim );
}

//=======================================================================================================================
void AnimBuilder::BuildAnimJoints( fx::Array<SceneAnimJoint*> & joints ) {
    
    fx::Array<uint64_t>     hashArray;
    fx::Array<uint32_t>     hashMap;
    
    for ( uint32_t n = 0; n < joints.GetLength(); ++n ) {
        SceneAnimJoint * joint = joints[ n ];
        
        fx::AnimStreamNode strNode;
        strNode.name            = fx::fh64::CalcFromString( joint->name.c_str() );
        strNode.flags           = fx::AnimStreamNode::F_POSITIONS | fx::AnimStreamNode::F_ROTATION_QUAT_XYZ;
        strNode.offsNameHr      = (uint32_t) WriteString( joint->name.c_str() );
        strNode.offsPositions   = 0;
        strNode.offsRotations   = 0;
        
        animNodes.Append( strNode );
        
        if ( hashArray.IsEmpty() == true ) {
            hashArray.Append( strNode.name );
            hashMap.Append( n );
        }
        else {
            fx::Array<uint64_t>::iterator_t insertPos = hashArray.InsertSorted( strNode.name );
            uint32_t insertIndex = (uint32_t) (insertPos - hashArray.Begin());
            
            hashMap.Insert( hashMap.begin() + insertIndex, n );
        }
    }
    
    nodeNameArrayStream.Write( &hashArray[ 0 ], hashArray.GetLength() );
    nodeNodeMapStream.Write( &hashMap[ 0 ], hashMap.GetLength() );
}

//=======================================================================================================================
void AnimBuilder::BuildFrameData( SceneAnimation * anim, fx::Array<SceneAnimJoint*> & sortedJoints ) {
    
    for ( uint32_t f = 0; f < anim->frameCount; ++f ) {
        
        // Write out rotations for this frame
        uint32_t rotationOffset = (uint32_t) frameRotations.Length();
        frameRotationOffsets.Write( &rotationOffset );
        
        for ( uint32_t j= 0; j < sortedJoints.GetLength(); ++ j ) {
            
            SceneAnimJoint * joint = sortedJoints[ j ];
            fx::math::Quat q = joint->rotationKeyValues[ f ];
            float rotXyz[ 3 ];
            
            if ( q.W() <= 0 ) {
                rotXyz[0] = q.X();
                rotXyz[1] = q.Y();
                rotXyz[2] = q.Z();
            } else {
                rotXyz[0] = -q.X();
                rotXyz[1] = -q.Y();
                rotXyz[2] = -q.Z();
            }
            
            frameRotations.Write( rotXyz, 3 );
        }
        
        // Write out positions for this frame
        uint32_t posOffset = (uint32_t) framePositions.Length();
        framePositionOffsets.Write( &posOffset );
        
        for ( uint32_t j= 0; j < sortedJoints.GetLength(); ++ j ) {
            SceneAnimJoint * joint = sortedJoints[ j ];
            fx::math::Vec3 p = joint->positionKeyValues[ f ];
            float posXyz[ 3 ] = { p.X(), p.Y(), p.Z() };
            framePositions.Write( posXyz, 3 );
        }
    }
}

//=======================================================================================================================
void AnimBuilder::Write( ToolMemStream & str, SceneAnimation * anim ) {

    // Setup header with known info and write it out
    fx::AnimStream header;
    memset( &header, 0, sizeof( header ) );
    
    header.magic        = fx::AnimStream::MAGIC;
    header.version      = fx::AnimStream::VERSION;
    header.fps          = anim->fps;
    header.frameCount   = anim->frameCount ;
    header.nodeCount    = (uint32_t) anim->animJoints.GetLength();
    // Only supporting absolute frames for the time being
    header.flags        = fx::AnimStream::F_ABSOLUTE_FRAMES;
    
    // Write out initial header
    str.PadToAlignment( 64 );
    uintptr_t headerOffs = str.Tell();
    WriteHeader( str, header );
    
    // Write out node data
    str.PadToAlignment( 64 );
    header.offsNodes = ( uint32_t) (str.Tell() - headerOffs) ;
    str.Write( nodeStream );
    
    str.PadToAlignment( 64 );
    header.offsNodeNameArray = ( uint32_t ) (str.Tell() - headerOffs);
    str.Write( nodeNameArrayStream );
    
    str.PadToAlignment( 64 );
    header.offsNodeNameMap = ( uint32_t ) (str.Tell() - headerOffs);
    str.Write( nodeNodeMapStream );
    
    str.PadToAlignment( 64 );
    header.offsFrameRotations = ( uint32_t) (str.Tell() - headerOffs);
    str.Write( frameRotationOffsets );
    
    // Write animation data
    str.PadToAlignment( 64 );
    header.offsFrameRotations = ( uint32_t) (str.Tell() - headerOffs) ;
    str.Write( frameRotations );
    
    str.PadToAlignment( 64 );
    header.offsFramePositions = ( uint32_t) (str.Tell() - headerOffs) ;
    str.Write( framePositions );
    
    str.PadToAlignment( 64 );
    header.offsFramePositionOffsets = ( uint32_t) (str.Tell() - headerOffs) ;
    str.Write( framePositionOffsets );
    
    str.PadToAlignment( 64 );
    header.offsFrameRotationOffsets = ( uint32_t) (str.Tell() - headerOffs) ;
    str.Write( frameRotationOffsets );

    // Write out hr strings data
    str.PadToAlignment( 64 );
    header.offsStrings = ( uint32_t) (str.Tell() - headerOffs) ;
    str.Write( strings );
    
    // Re-write the completed header
    uintptr_t pos = str.Tell();
    str.Seek( headerOffs );
    WriteHeader( str, header );
    str.Seek( pos );
}

//=======================================================================================================================
void AnimBuilder::WriteHeader( ToolMemStream & str, fx::AnimStream & header ) {
    str.Write( &header.magic );
    str.Write( &header.flags );
    str.Write( &header.version );
    str.Write( &header.nodeCount );
    str.Write( &header.fps );
    str.Write( &header.frameCount );
    str.Write( &header.offsBaseOffsetTransform );
    str.Write( &header.offsNodeNameArray );
    str.Write( &header.offsNodeNameMap );
    str.Write( &header.offsNodes );
    str.Write( &header.offsFrameRotations );
    str.Write( &header.offsFramePositions );
    str.Write( &header.offsFrameRotationOffsets );
    str.Write( &header.offsFramePositionOffsets );
    str.Write( &header.offsStrings );
    str.Write( &header.padding );
}

//=======================================================================================================================
void AnimBuilder::WriteNodes() {
    for ( uint32_t n = 0; n < animNodes.GetLength(); ++n ) {
        fx::AnimStreamNode & node = animNodes[ n ];
        
        nodeStream.Write( &node.name );
        nodeStream.Write( &node.flags );
        nodeStream.Write( &node.offsPositions );
        nodeStream.Write( &node.offsRotations );
        nodeStream.Write( &node.offsTimes );
        nodeStream.Write( &node.offsNameHr );
        nodeStream.Write( &node.pad );
    }
}

//=======================================================================================================================
uintptr_t AnimBuilder::WriteString( const char * string ) {
    uintptr_t pos = strings.Length();
    size_t writeLen = strlen( string ) + 1;
    strings.Write( string, writeLen );
    return pos;    
}
