//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#include "modelbuilder/SkeletonBuilder.h"

const SkeletonBuilder::Options SkeletonBuilder::Options::DEFAULT;

//======================================================================================================================
SkeletonBuilder::SkeletonBuilder(  const Options & options_ ) {
    options = options_;
}

//======================================================================================================================
SkeletonBuilder::~SkeletonBuilder() {

}

//======================================================================================================================
void SkeletonBuilder::Build( ToolMemStream & str, Scene * scene ) {
    
    SceneNode * root = scene->rootNode;
    bool definedRootNode = ( options.rootNode.empty() == false );
    if ( definedRootNode == true ) {
        root = scene->FindNode( options.rootNode.c_str() );
        xerror( root == nullptr, "Unable to find root node %s\n", options.rootNode.c_str() );
    }
    
    // Build an array of nodes from the scene
    std::vector<SceneNode*> nodeArray;
    scene->WalkSceneFromNode( root, &nodeArray, []( SceneNode * node, void * ctxt ) {
        if ( node->type == SceneNode::TYPE ) {
            // Only interested in scene nodes - we don;t want to add anything that
            // is a mesh
            ((std::vector<SceneNode*> *) ctxt)->push_back( node );
        }
    });
    
    // Create the joint name hash tables
    for ( uint32_t j = 0; j < nodeArray.size(); ++j ) {
        uint64_t hash = FH64_CalcFromCStr( nodeArray[ j ]->name.c_str() );
        
        int32_t index = -1;
        Bsearch_FindUint64( &index, hash, &nameHashArray[0], nameHashArray.size() );
        
        nameHashArray.insert( nameHashArray.begin() + index, hash );
        nameHashMap.insert( nameHashMap.begin() + index, j );

    }
    
    m_jointHashArray.Write( &nameHashArray[0], nameHashArray.size() );
    m_jointHashMap.Write( &nameHashMap[0], nameHashMap.size() );
    
    // Get the parent index for each joint
    std::vector <int32_t> parentIndices;
    
    for ( uint32_t j = 0; j < nodeArray.size(); ++j ) {
        int32_t parent = -1;
        
        if ( nodeArray[ j ]->parent != nullptr ) {
            
            parent = FindJointIndex( nodeArray[ j ]->parent->name.c_str() );
            if ( parent == -1) {
                xerror( j != 0, "Could not get parent for joint '%'\n",  nodeArray[ j ]->name.c_str() );
            }
        }
        
        parentIndices.push_back( parent );
    }
    
    // Set the offset transform. If we have a user defined root bone, this
    // will be the world transform of the user-defined root's parent node
    offsetTransform = math::Mat4::IDENTITY;
    if ( definedRootNode == true ) {
        if ( root->GetParent() != nullptr ) {
            offsetTransform = root->GetParent()->EvaulateWorldTransform();
        }
    }
    
    // Write out joint info to streams
    for ( uint32_t n = 0; n < nodeArray.size(); ++n ) {
        SceneNode * node = nodeArray[ n ];
        
        math::Mat4 bodyMat = node->EvaulateWorldTransform();
        math::Mat4 bodyMatInv;
        bodyMatInv.Inverse( bodyMat );
        
        m_jointMatrices.Write( ( float * ) ( &node->transform), 16 );
        
        m_jointBindMatrices.Write( (float *) &bodyMatInv, 16  );
        m_jointParentIndices.Write( &parentIndices[ n ] );
        
        uint32_t nameOffs = WriteString( node->name.c_str() );
        m_jointNames.Write( &nameOffs );

    }
    
    WriteToStream( str, nodeArray.size() );
}

//======================================================================================================================
int32_t SkeletonBuilder::FindJointIndex( const char * name ) {
    uint64_t hash = FH64_CalcFromCStr( name );
    
    int32_t index = -1;
    bool_t found = Bsearch_FindUint64( &index, hash, &nameHashArray[0], nameHashArray.size() );
    if ( found == bool_false ) {
        return -1;
    }
    
    int32_t mapIndex = nameHashMap[ index ];
    return index;
}

//======================================================================================================================
uint32_t SkeletonBuilder::WriteString( const char * rhs ) {
    uint32_t offset = (uint32_t) m_jointStrings.Length();
    
    size_t len = strlen( rhs );
    m_jointStrings.Write( rhs, len + 1);
    
    return offset;
}

//======================================================================================================================
void SkeletonBuilder::WriteToStream( ToolMemStream & str, size_t jointCount ) {
    
    skeleton_stream_t header;
    memset( &header, 0, sizeof(header) );
    header.version = SKELETON_STREAM_VERSION;
    header.jointCount = (uint32_t) jointCount;
    
    uint64_t headerPos = str.Tell();
    WriteHeader( str, header );
    
    header.offsJointParents = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointParentIndices );
    
    header.offsJointHashArray = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointHashArray );
    
    header.offsJointHashMap = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointHashMap );
    
    header.offsOffsetTransform = (uint32_t) ( str.Tell() - headerPos );
    str.Write( (float*) &offsetTransform, 16 );
    
    header.offsJointMatrices = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointMatrices );
    
    header.offsJointBindMatrices = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointBindMatrices );
    
    header.offsNameStrings = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointStrings );
    
    header.offsNames = (uint32_t) ( str.Tell() - headerPos );
    str.Write( m_jointNames );
    
    
    uint64_t endPos = str.Tell();
    str.Seek( headerPos );
    WriteHeader( str, header );
    str.Seek( endPos );
}

//======================================================================================================================
void SkeletonBuilder::WriteHeader( ToolMemStream & str,  skeleton_stream_t & header ) {
    str.Write( &header.version );
    str.Write( &header.flags );
    str.Write( &header.jointCount );
    str.Write( &header.offsJointParents );
    str.Write( &header.offsJointHashArray );
    str.Write( &header.offsJointHashMap );
    str.Write( &header.offsJointMatrices );
    str.Write( &header.offsJointBindMatrices );
    str.Write( &header.offsNameStrings );
    str.Write( &header.offsNames );
    str.Write( &header.offsOffsetTransform );
    str.Write( &header.pad );
}

