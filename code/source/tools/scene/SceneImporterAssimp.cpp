//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "scene/SceneImporterAssimp.h"
#include "scene/SceneMesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/pbrmaterial.h"

//=======================================================================================================================
SceneImporterAssImp::SceneImporterAssImp( const Options & options_ ) : SceneImporter( options_ ) {
}

//=======================================================================================================================
SceneImporterAssImp::~SceneImporterAssImp() {
    SAFE_DELETE( importer );
}

//=======================================================================================================================
Scene::ptr_t SceneImporterAssImp::Import( const char * path ) {
    bool loadOk = LoadScene( path );
    xerror( loadOk == false, "Uncaught error loading scene\n" );
    
    scene = new Scene;
    WalkAiScene_r( importScene->mRootNode, nullptr );
    
    // Prune end marker nodes - we don't need those
    PruneEndNodes();
    
    // Print out hierarchy
    ReportHierarchy_r( scene->rootNode );
    
    if ( options.importAnims == true ) {
        xprintf("Importing animations...\n" );
        for ( uint32_t a = 0; a != importScene->mNumAnimations; ++a ) {
            ImportAnimation( importScene->mAnimations[ a ] );
        }
    }

    return scene;
}

//==========================================================================================================================================
bool SceneImporterAssImp::LoadScene( const char * path ) {
    
    std::string fullPath;
    str_t fullPathStr = NULL;
    FS_MakePath( &fullPathStr, path);
    fullPath = fullPathStr;
    Str_Destroy( &fullPathStr );
    
    uint32_t importFlags = /*aiProcess_GenSmoothNormals |  aiProcess_CalcTangentSpace*/ aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_MakeLeftHanded;
    
    importer = new Assimp::Importer;
    
    importScene = importer->ReadFile( fullPath.c_str(), importFlags);
    xerror( importScene == nullptr, "Error loading scene.\n%s\n", importer->GetErrorString() );
    
    return true;
}

//==========================================================================================================================================
void SceneImporterAssImp::WalkAiScene_r( aiNode * aiSrcNode, SceneNode * parent )  {
    
    SceneNode * thisNode = nullptr;
    bool addedNode = false;
    
    if ( aiSrcNode->mNumMeshes != 0 ) {        
        if ( options.importMeshes == true ) {
            // This node has a mesh, so we'll import it.
            thisNode = ImportMesh( aiSrcNode );
            addedNode = true;
        }
    }
    else {
        // Just a node
        thisNode = new SceneNode;
        thisNode->name = aiSrcNode->mName.C_Str();
        thisNode->transform = ConvertWithScale( aiSrcNode->mTransformation, options.scale );
        addedNode = true;
    }
    
    if ( addedNode == true ) {
        
        //xprintf("Adding node %s\n", aiSrcNode->mName.C_Str() );
        
        if (parent != nullptr ) {
            parent->AddChild( thisNode );
        }
        else {
            xerror( scene->rootNode != nullptr, "Scene already has a root node!\n" );
            scene->rootNode = thisNode;
        }
        
        // Walk children
        for ( int c = 0; c < aiSrcNode->mNumChildren; ++c ) {
            WalkAiScene_r( aiSrcNode->mChildren[ c ], thisNode );
        }
    }
}

//==========================================================================================================================================
void SceneImporterAssImp::SetRootNode( const char * rootName ) {
    
    SceneNode::ptr_t newRootNode = scene->FindNode( rootName );
    xerror( newRootNode == nullptr, "Could not find new root node '%s",  rootName );
    
    if ( newRootNode.Get() == scene->rootNode.Get() ) {
        // No need to do anything if the desired root node is already the root
        return;
    }
    
    xprintf("Setting root node to be '%s'", rootName );
    
    // Detach the node from its parent
    newRootNode->parent->RemoveChild( newRootNode );
    
    // Set new scene root
    scene->rootNode = nullptr;
    scene->rootNode = newRootNode;
}


//==========================================================================================================================================
void SceneImporterAssImp::PruneEndNodes() {
    PruneEndNodes_r( scene->rootNode );
}

//==========================================================================================================================================
void SceneImporterAssImp::PruneEndNodes_r( SceneNode * node ) {
    if ( node->GetType() == SceneNode::TYPE && node->GetChildCount() == 0 && node->parent != nullptr ) {
        
        std::string::size_type endPos = node->name.find( "_end" );
        if (endPos != std::string::npos && endPos == node->name.size() - 4 ) {
            node->parent->RemoveChild( node );
        }
    }
    else {
        for ( int c = 0; c < node->GetChildCount(); ++c ) {
            PruneEndNodes_r( node->GetChild( c ) );
        }
    }
}

//==========================================================================================================================================
SceneMesh * SceneImporterAssImp::ImportMesh( aiNode * srcNode ) {
    
    xerror( srcNode->mNumMeshes != 1, "Node % has more than one mesh\n", srcNode->mName.C_Str() );
    aiMesh * srcMesh = importScene->mMeshes[ srcNode->mMeshes[ 0 ] ];
    
    xprintf( "Importing mesh %s\n", srcNode->mName.C_Str() );
    
    SceneMesh * mesh = new SceneMesh;
    mesh->geometry      = new SceneGeometry;
    mesh->name          = srcNode->mName.C_Str();
    mesh->transform     = ConvertWithScale( srcNode->mTransformation, options.scale );
    
    // Basic mesh geometry
    mesh->geometry->positions.reserve( srcMesh->mNumVertices );
    mesh->geometry->normals.reserve( srcMesh->mNumVertices );
    mesh->geometry->texCoords.reserve( srcMesh->mNumVertices );
    mesh->geometry->triangles.reserve( srcMesh->mNumFaces );
    
    for ( int v = 0; v < srcMesh->mNumVertices; ++v ) {
        math ::Vec3 p = ConvertWithScale( srcMesh->mVertices[ v ], options.scale );
        math ::Vec3 n = Convert ( srcMesh->mNormals[ v ] );
        math ::Vec3 t = Convert( srcMesh->mTextureCoords[0][ v ] );
        
        mesh->geometry->positions.push_back( p );
        mesh->geometry->normals.push_back( n );
        mesh->geometry->texCoords.push_back( t );
    }
    
    // If the mesh is skinned, import the skinning info
    if ( srcMesh->mNumBones != 0 ) {
        ImportSkin( mesh, srcMesh );
    }
    
    // Import triangles
    for ( int t = 0; t < srcMesh->mNumFaces; ++t ) {
        aiFace * currFace = &srcMesh->mFaces[ t ];
        xerror( currFace->mNumIndices != 3, "Face is not a triangle\n");
        
        SceneTriangle tri;
        if ( options.flipFaces == true ) {
            tri.Set( currFace->mIndices[2], currFace->mIndices[1], currFace->mIndices[0] );
        }
        else {
            tri.Set( currFace->mIndices[0], currFace->mIndices[1], currFace->mIndices[2] );
        }
        
        mesh->geometry->triangles.push_back( tri );
    }
    
    // Import the material
    SceneMaterial * mat = ImportMaterial( importScene->mMaterials[ srcMesh->mMaterialIndex ] );
    mesh->geometry->material = mat;
    
    return mesh;
}

//==========================================================================================================================================
static bool GetAlbedoTexture( aiString * pathOut, aiMaterial * mat ) {
    
    uint32_t diffuseCount = mat->GetTextureCount( aiTextureType_DIFFUSE );
    if ( diffuseCount > 0 ) {
        aiReturn res = mat->GetTexture( aiTextureType_DIFFUSE, 0, pathOut, NULL, NULL, NULL, NULL, NULL);
        if ( res == AI_SUCCESS ) {
            return true;
        }
    }
    
    return false;
}

//==========================================================================================================================================
static bool GetOcclusionTexture( aiString * pathOut, aiMaterial * mat ) {

    aiReturn res = mat->GetTexture( aiTextureType_SPECULAR, 0, pathOut, NULL, NULL, NULL, NULL, NULL);
    if ( res == AI_SUCCESS ) {
        return true;
    }
    
    return false;
}

//==========================================================================================================================================
static bool GetRouchnessTexture( aiString * pathOut, aiMaterial * mat ) {
    
    uint32_t diffuseCount = mat->GetTextureCount( aiTextureType_DIFFUSE_ROUGHNESS );
    if ( diffuseCount > 0 ) {
        aiReturn res = mat->GetTexture( aiTextureType_DIFFUSE_ROUGHNESS, 0, pathOut, NULL, NULL, NULL, NULL, NULL);
        if ( res == AI_SUCCESS ) {
            return true;
        }
    }
    
    return false;
}

//==========================================================================================================================================
static bool GetMetalTexture( aiString * pathOut, aiMaterial * mat ) {
    
    uint32_t diffuseCount = mat->GetTextureCount( aiTextureType_METALNESS );
    if ( diffuseCount > 0 ) {
        aiReturn res = mat->GetTexture( aiTextureType_METALNESS, 0, pathOut, NULL, NULL, NULL, NULL, NULL);
        if ( res == AI_SUCCESS ) {
            return true;
        }
    }
    
    return false;
}


//==========================================================================================================================================
SceneMaterial * SceneImporterAssImp::ImportMaterial( aiMaterial * srcMat ) {
    SceneMaterial * mat = scene->FindMaterial( srcMat->GetName().C_Str() );
    if ( mat != nullptr ) {
        return mat;
    }
    
    mat = new SceneMaterial;
    mat->name = srcMat->GetName().C_Str();
    
    scene->AddMaterial( mat );
    
    aiString fileAlbedo;
    bool gotAlbedo = GetAlbedoTexture( &fileAlbedo, srcMat );
    if ( gotAlbedo == true ) {
        mat->albedo = new SceneMaterialStage;
        mat->albedo->name = "albedo";
        mat->albedo->texturePath = fileAlbedo.C_Str();
    }
    
    aiString fileRoughness;
    bool gotRoughness = GetRouchnessTexture( &fileRoughness, srcMat );
    if ( gotRoughness == true ) {
        mat->roughness = new SceneMaterialStage;
        mat->roughness->name = "roughness";
        mat->roughness->texturePath = fileRoughness.C_Str();
    }
    
    aiString fileOcclusion;
        bool gotOcclusion = GetOcclusionTexture( &fileOcclusion, srcMat );
    if ( gotOcclusion == true ) {
        mat->occlusion = new SceneMaterialStage;
        mat->occlusion->name = "ambient occlusion";
        mat->occlusion->texturePath = fileOcclusion.C_Str();
    }
    
    aiString fileMetal;
    bool gotMetal = GetMetalTexture( &fileMetal, srcMat );
    if ( gotMetal == true ) {
        mat->metallic = new SceneMaterialStage;
        mat->metallic->name = "metallic";
        mat->metallic->texturePath = fileMetal.C_Str();
    }
    
    return mat;
}

//==========================================================================================================================================
void SceneImporterAssImp::ImportSkin( SceneMesh * mesh, aiMesh * srcMesh ) {
    SceneSkin * skin = new SceneSkin;
    mesh->skin = skin;
    
    for( int c = 0; c < srcMesh->mNumBones; ++c ) {
        aiBone * bone = srcMesh->mBones[ c ];
        
        SceneSkinCluster * cluster = new SceneSkinCluster;
        skin->clusters.push_back( cluster );
        
        cluster->linkNode = bone->mName.C_Str();
        cluster->linkTransform = ConvertWithScale( bone->mOffsetMatrix, options.scale );
        
        for ( int n = 0; n < bone->mNumWeights; ++n ) {
            cluster->vertices.push_back( bone->mWeights[n].mVertexId );
            cluster->weights.push_back( bone->mWeights[n].mWeight );
        }
    }
}

//==========================================================================================================================================
void SceneImporterAssImp::ReportHierarchy_r( SceneNode * node, int level ) {
    for ( int l =0; l < level; ++l ) {
        xprintf(" ");
    }
    
    xprintf( "%s\n", node->name.c_str() );
    for ( int c = 0; c < node->GetChildCount(); ++c ) {
        ReportHierarchy_r( node->GetChild( c ), level + 1 );
    }
}

//==========================================================================================================================================
SceneAnimation * SceneImporterAssImp::ImportAnimation( aiAnimation * srcAnim ) {
    
    xprintf("    '%s'\n", srcAnim->mName.C_Str() );
    
    SceneAnimation * anim = new SceneAnimation;
    scene->animations.push_back( anim );
    
    anim->name = srcAnim->mName.C_Str();
    anim->animJoints.reserve( srcAnim->mNumChannels );
    anim->fps           = srcAnim->mTicksPerSecond;
    anim->frameCount    = (uint32_t) srcAnim->mDuration;
    
    for( uint32_t c = 0; c < srcAnim->mNumChannels; ++c ) {
        aiNodeAnim * srcNode = srcAnim->mChannels[ c ];
        
        SceneAnimJoint * joint = new SceneAnimJoint( anim );
        xassert( joint != nullptr );
        
        joint->name = srcNode->mNodeName.C_Str();
        anim->animJoints.push_back( joint );
        
        // Import position keys if any
        if ( srcNode->mNumPositionKeys != 0 ) {
            
            joint->positionKeyValues.reserve( srcNode->mNumPositionKeys );
            joint->positionTimes.reserve( srcNode->mNumPositionKeys );
            
            for ( uint32_t k = 0; k < srcNode->mNumPositionKeys; ++k ) {
                joint->positionKeyValues.push_back( ConvertWithScale( srcNode->mPositionKeys[ k ].mValue, options.scale ) );
                joint->positionTimes.push_back( srcNode->mPositionKeys[ k ].mTime );
            }
        }
        
        // Import scale keeys if any
        if ( srcNode->mNumScalingKeys != 0 ) {
            
            joint->positionKeyValues.reserve( srcNode->mNumScalingKeys );
            joint->positionTimes.reserve( srcNode->mNumScalingKeys );
            
            for ( uint32_t k = 0; k < srcNode->mNumScalingKeys; ++k ) {
                joint->scaleKeyValues.push_back( Convert( srcNode->mScalingKeys[ k ].mValue ) );
                joint->scaleTimes.push_back( srcNode->mScalingKeys[ k ].mTime );
            }
        }
        
        // Import rotation keys if any
        if ( srcNode->mNumRotationKeys != 0 ) {
            
            joint->rotationKeyValues.reserve( srcNode->mNumRotationKeys );
            joint->rotationTimes.reserve( srcNode->mNumRotationKeys );
            
            for ( uint32_t k = 0; k < srcNode->mNumRotationKeys; ++k ) {
                joint->rotationKeyValues.push_back( Convert( srcNode->mRotationKeys[ k ].mValue ) );
                joint->rotationTimes.push_back( srcNode->mRotationKeys[ k ].mTime );
            }
        }
    }
    
    return anim;
}
