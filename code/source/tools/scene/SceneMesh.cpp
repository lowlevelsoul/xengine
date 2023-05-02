//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "SceneMesh.h"
#include "Scene.h"

//=======================================================================================================================
SceneMesh::SceneMesh() {
    type = SceneMesh::TYPE;
}

//=======================================================================================================================
SceneMesh::~SceneMesh() {
    
}

//=======================================================================================================================
void SceneMesh::MakeShell( std::vector<math ::Vec3> & posOut, std::vector<math ::Vec3> & normOut,
                           std::vector<math ::Vec3> & texCoords, Scene * scene ) {
    
    if ( skin != nullptr ) {
        MakeShellSkin( posOut, normOut, texCoords, scene );
    }
    else {
        MakeShellStatic( posOut, normOut, texCoords, scene );
    }
}

//=======================================================================================================================
void SceneMesh::MakeShellStatic( std::vector<math ::Vec3> & posOut, std::vector<math ::Vec3> & normOut, std::vector<math ::Vec3> & texCoords, Scene * scene ) {
    
    math ::Mat4 worldTransform = EvaulateWorldTransform();
    
    for ( int v = 0; v < geometry->positions.size(); ++v ) {
        math ::Vec4 worldP = worldTransform * math ::Vec4( geometry->positions[ v ], 1 );
        math ::Vec4 worldN = worldTransform * math ::Vec4( geometry->normals[ v ], 0 );
        
        posOut.push_back( worldP );
        normOut.push_back( worldN );
        texCoords.push_back( geometry->texCoords[ v ] );
    }
}

//=======================================================================================================================
void SceneMesh::MakeShellSkin( std::vector<math ::Vec3> & posOut, std::vector<math ::Vec3> & normOut, std::vector<math ::Vec3> & texCoords, Scene * scene ) {
    
    posOut.resize( geometry->positions.size() );
    normOut.resize( geometry->positions.size() );
    
    for ( int n = 0; n < geometry->positions.size(); ++n ) {
        posOut[ n ] = math ::Vec3::ZERO;
        normOut[ n ] = math ::Vec3::ZERO;
    }
    
    for ( int c = 0; c != skin->clusters.size(); ++c ) {
        SceneSkinCluster * cluster = skin->clusters[ c ];
        
        SceneNode * linkNode = scene->FindNode( cluster->linkNode.c_str() );
        xerror( linkNode == nullptr, "Could not find skin link node '%s'\n", cluster->linkNode.c_str() );
        
        math ::Mat4 linkNodeBody = linkNode->EvaulateWorldTransform();
        
        for( int w = 0; w < cluster->weights.size(); ++w ) {
            uint32_t vertexId =  cluster->vertices[  w ];
            float vertexWeight = cluster->weights[ w ];
            
            const math ::Vec3 & localP = cluster->linkTransform * math ::Vec4( geometry->positions[ vertexId ], 1 );
            const math ::Vec3 & localN = cluster->linkTransform * math ::Vec4( geometry->normals[ vertexId ], 0 );
            
            const math ::Vec3 skinLocalP =  linkNodeBody * math ::Vec4( localP, 1 );
            const math ::Vec3 skinLocalN =  linkNodeBody * math ::Vec4( localN, 0 );
            
            posOut[ vertexId ] += ( skinLocalP * vertexWeight );
            normOut[ vertexId ] += skinLocalN;
        }
    }
    
    for ( int n = 0; n < posOut.size(); ++n ) {
        normOut[ n ].Normalise();
        texCoords.push_back( geometry->texCoords[ n ] );
    }
}
