//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __SCENEIMPORTERASSIMP_H__
#define __SCENEIMPORTERASSIMP_H__

#include "scene/Scene.h"
#include "scene/SceneImporter.h"

class SceneMesh;

class SceneImporterAssImp : public SceneImporter {
public:
    SceneImporterAssImp( const Options & options_ = Options::DEFAULT );
    
    virtual ~SceneImporterAssImp();
    
    virtual Scene::ptr_t Import( const char * path ) override;
    
    bool LoadScene( const char * path );
    
    void WalkAiScene_r( aiNode * aiSrcNode, SceneNode * parent );
    
    SceneMesh * ImportMesh( aiNode * srcNode );
    
    SceneMaterial * ImportMaterial( aiMaterial * srcMat );
    
    SceneAnimation * ImportAnimation( aiAnimation * srcAnim );
    
    void ImportSkin( SceneMesh * mesh, aiMesh * srcMesh );
    
    math ::Mat4 Convert( const aiMatrix4x4 & aiMat ) const;
    
    math ::Vec3 Convert( const aiVector3D & rhs ) const;

    math ::Quat Convert( const aiQuaternion & rhs ) const;
    
    math ::Mat4 ConvertWithScale( const aiMatrix4x4 & aiMat, float scale ) const;
    
    math ::Vec3 ConvertWithScale( const aiVector3D & rhs, float scale  ) const;
    
    void SetRootNode( const char * rootName );
    
    void PruneEndNodes();
    
    void PruneEndNodes_r( SceneNode * node );
    
    void ReportHierarchy_r( SceneNode * node, int level = 0 );

public:
    Assimp::Importer *      importer = nullptr;
    const aiScene*          importScene = nullptr;
    Scene::ptr_t            scene;
};


//================================================================================================================================
inline math ::Mat4 SceneImporterAssImp::Convert( const aiMatrix4x4 & aiMat ) const {
    math ::Mat4 mat;
    
    mat.m_rows[0].Set(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1);
    mat.m_rows[1].Set(aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2);
    mat.m_rows[2].Set(aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3);
    mat.m_rows[3].Set(aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
    
    return mat;
}

//================================================================================================================================
inline math ::Vec3 SceneImporterAssImp::Convert( const aiVector3D & rhs ) const{
    return math ::Vec3( rhs.x, rhs.y, rhs.z );
}

//================================================================================================================================
inline math ::Quat SceneImporterAssImp::Convert( const aiQuaternion & rhs ) const {
    return math ::Quat( rhs.x, rhs.y, rhs.z, rhs.w );
}

//================================================================================================================================
inline math ::Mat4 SceneImporterAssImp::ConvertWithScale( const aiMatrix4x4 & aiMat, float scale ) const {
    
    math ::Mat4 mat;
    
    mat.m_rows[0].Set(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1);
    mat.m_rows[1].Set(aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2);
    mat.m_rows[2].Set(aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3);
    mat.m_rows[3].Set(aiMat.a4 * scale,
                      aiMat.b4 * scale,
                      aiMat.c4 * scale,
                      aiMat.d4);
    
    return mat;
}

//================================================================================================================================
inline math ::Vec3 SceneImporterAssImp::ConvertWithScale( const aiVector3D & rhs, float scale  ) const {
    return math ::Vec3( rhs.x * scale, rhs.y * scale, rhs.z * scale );
}

#endif
