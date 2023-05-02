//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021-2023 James Steele. All Rights Reserved.
//======================================================================================================================

#include "modelbuilder/ModelBuilder.h"
#include "modelbuilder/SkeletonBuilder.h"
#include "toolapp/ToolMemStream.h"
#include "toolapp/ToolApp.h"
#include "scene/SceneImporterAssimp.h"
#include "scene/Scene.h"
#include "TextureScriptUtil.h"

const ModelBuilder::Options ModelBuilder::Options::DEFAULT;

//======================================================================================================================
ModelBuilder::Options & ModelBuilder::Options::operator=( const Options & rhs ) {
    
    scale = rhs.scale;
    flipFaces = rhs.flipFaces;
    genNormals = rhs.genNormals;
    stripMixamo = rhs.stripMixamo;
    meshFilter = rhs.meshFilter;
    meshIgnore = rhs.meshIgnore;
    center = rhs.center;
    forceMaterial = rhs.forceMaterial;
    forceMaterialName = rhs.forceMaterialName;
    rootNode = rhs.rootNode;
    
    return *this;
}

//======================================================================================================================
ModelBuilder::ModelBuilder( const Options & options_ ) {
    m_options = options_;
}

//======================================================================================================================
ModelBuilder::~ModelBuilder() {
    SAFE_DELETE( skeletonBuilder );
}

//======================================================================================================================
void ModelBuilder::Compile( const char * path ) {
    
    // Import the scene
    SceneImporter::Options importOptions;
    importOptions.scale = m_options.scale;
    importOptions.flipFaces = m_options.flipFaces;
    importOptions.importAnims = false;
    importOptions.importMeshes = true;
    
    if ( m_options.rootNode.empty() == false ) {
        importOptions.rootNode = m_options.rootNode;
    }
    
    SceneImporterAssImp importer( importOptions );
    
    scene = importer.Import( m_input.c_str() );
    xerror( scene == nullptr, "Uncaught error importing scene\n" );
    
    // Get a list of meshes that we want to import
    GatherMeshes();
    
    // If we want to build a skinned mesg
    bool writeSkinned = HasMeshListSkinnedMeshes();
    if ( writeSkinned == true ) {
        SkeletonBuilder::Options skelOpt;
        skelOpt.rootNode = m_options.rootNode;
        
        skeletonBuilder = new SkeletonBuilder( skelOpt );
        skeletonBuilder->Build( skeletonStream, scene );
    }
    
    // Process meshes for export
    ProcessMeshes();
    
    xprintf( "Building binary stream...\n" );
    ToolMemStream str;
    WriteToStream( str,  writeSkinned );
    xprintf("Done.\n");
    
    {
        xprintf("Writing stream to file\n");
        
        tool->CreateFolderAtPath( path );
        xeScopedFile file( path, "wb" );
        xerror( file.IsValid() == false, "Could not open %s for writing\n", path );
        
        file.Write( str.GetStream(), sizeof(uint8_t), str.Length());
    }
    
    if ( writeSkinned == true ) {
        WriteSkeleton( path, skeletonStream );
    }
    
    
    // Write out materials
    for( uint32_t m = 0; m < scene->materials.size(); ++m ) {
        WriteMaterial( m_input.c_str(), path, scene->materials[ m ] );
    }
    
    xprintf("Done.\n");
}

//======================================================================================================================
void ModelBuilder::ProcessMeshes() {
    bool hasSkinnedMeshes = HasMeshListSkinnedMeshes();
    bool generateSkins = (hasSkinnedMeshes == true );
    
    // Generate mesh shell geometry. This geometry is essentially the mesh as if it were
    // a static mesh with all of the transformers applied, regardless whether it's a static
    // or skinned mmesh ( since skinned meshes are static meshes with skinning weights )
    for ( auto me : meshList ) {
        me->srcMesh->MakeShell( me->shellVerts, me->shellNormals, me->shellTexCords, scene );
        
        if ( generateSkins == true ) {
            BuildVertexWeights( me );
        }
    }
}

//======================================================================================================================
bool ModelBuilder::HasMeshListSkinnedMeshes() {
    for ( auto me : meshList ) {
        if ( me->srcMesh->skin != nullptr ) {
            return true;
        }
    }
    
    return true;
}

//======================================================================================================================
void ModelBuilder::GatherMeshes() {
    
    scene->WalkScene( &meshList, [](SceneNode * node, void * ctxt) {
        if ( node->GetType() == SceneMesh::TYPE ) {
            MeshEntry * info = new MeshEntry;
            info->srcMesh = (SceneMesh*)node;
            ((MeshEntry::array_t*) ctxt)->push_back(  info );
        }
    } );
}

//======================================================================================================================
void ModelBuilder::WriteSkeleton( const char * modelPath, ToolMemStream & str  ) {
    
    xprintf( "Writing model skeleton\n" );
    
    std::string skelBinPath = modelPath;
    std::string::size_type extPos = skelBinPath.rfind(".mdl");

    if ( extPos != std::string::npos) {
        skelBinPath = skelBinPath.substr(0, extPos );
    }

    skelBinPath.append( ".skel" );

    xeScopedFile file( skelBinPath.c_str(), "wb" );
    xerror( file.IsValid() == false, "Unable to open %s for writing\n", skelBinPath.c_str() );
    
    file.Write( str.GetStream(), str.Length() );
}

static const char * albedo_fmt          = "    +albedo \"%s\"\n";
static const char * amr_fmt             = "    +amr \"%s\"\n";
static const char * glow_fmt            = "    +glow \"%s\"\n";

//======================================================================================================================
void ModelBuilder::WriteMaterial( const char * srcModelPath, const char * dstPath, SceneMaterial * mat ) {
#if 1
    std::string matText;
    
    std::string basePath = srcModelPath;
    std::string fileName;
    
    PathUtil::GetLastPathElement(fileName, basePath );
    
    std::string matPath = basePath;
    matPath.append( mat->name.c_str() );
    matPath.append( ".bmat" );
    
    std::string matParamsPath = matPath;
    matParamsPath[0] = '@';
    matParamsPath.append( ".params" );
    
    std::string matDestPath = dstPath;

    PathUtil::RemoveLastPathElement( matDestPath );
    PathUtil::AppendPath( matDestPath, mat->name.c_str() );
    matDestPath.append( ".bmat" );
    
    TextureScriptUtil util;
    util.Initialise( srcModelPath, m_input.c_str() );
    
    matText = ToolApp::VFormat("+material \"%s\"\n", matDestPath.c_str() );
    
    //
    // Albedo texture
    //
    if ( mat->albedo != nullptr && mat->albedo->texturePath.empty() == false ) {
        std::string texPath;
        util.MakeMaterialTextureDestPath( texPath, mat->albedo->texturePath.c_str() );
        
        matText.append( ToolApp::VFormat( albedo_fmt, texPath.c_str() ) );
    }
    
    //
    // AMR texture
    //
    bool haveMetallic = ( mat->metallic != nullptr && mat->metallic->texturePath.empty() == false );
    bool haveRoughness = ( mat->roughness != nullptr && mat->roughness->texturePath.empty() == false );
    bool haveOcclusion = ( mat->occlusion != nullptr && mat->occlusion->texturePath.empty() == false );
    bool haveAmr = false;
    std::string amrTexPath;
    
    if ( haveMetallic == true || haveRoughness == true || haveOcclusion == true ) {
        util.MakeMaterialTextureDestPath( amrTexPath, mat->name.c_str(), "_amr" );
        matText.append( ToolApp::VFormat( amr_fmt, amrTexPath.c_str()  ) );
        haveAmr = true;
    }
    
    //
    // Glow Texture
    //
    if ( mat->glow && mat->glow->texturePath.empty() == false ) {
        std::string texPath;
        util.MakeMaterialScriptTexturePath( texPath, mat->albedo );
        matText.append( ToolApp::VFormat( albedo_fmt, texPath.c_str() ) );
    }
    
    xprintf("Writing material script \"%s\"\n", matParamsPath.c_str() );
    
    // Write out the material script
    {
        xeScopedFile scriptFile( matParamsPath.c_str(), "wb" );
        xerror( scriptFile.IsValid() == false, "Could not opn material script \"%s\" for writing\n", matParamsPath.c_str() );
        scriptFile.Write( matText.c_str(), sizeof(char), matText.size() );
    }
#endif
    
    // TODO - Write out texture build params
    WriteMaterialTextureParams( dstPath );
}


//======================================================================================================================
void ModelBuilder::WriteMaterialTextureParams( const char * modelDestPath ) {
    

    
    for( uint32_t m = 0; m < scene->materials.size(); ++m ) {
        SceneMaterial * mat = scene->materials[ m ];
        
        if ( mat->HasAlbedoTexture() == true ) {
            TextureScriptUtil texScriptUtil;
            texScriptUtil.Initialise( m_input.c_str(), modelDestPath );
            
            texScriptUtil.WriteSingleTextureParams( mat->albedo->texturePath.c_str(), "abledo" );
        }
        
        if ( mat->HasMetallicTexture() || mat->HasOcclusionTexture() || mat->HasRoughnessTexture() ) {
            TextureScriptUtil texScriptUtil;
            texScriptUtil.Initialise( m_input.c_str(), modelDestPath );
            
            texScriptUtil.WriteAmrTexture( mat );
        }
        
        if ( mat->HasGlowTexture() == true ) {
            TextureScriptUtil texScriptUtil;
            texScriptUtil.Initialise( m_input.c_str(), modelDestPath );
            
            texScriptUtil.WriteSingleTextureParams( mat->glow->texturePath.c_str(), "glow" );
        }
    }
}

//======================================================================================================================
void ModelBuilder::WriteHeader( ToolMemStream & str, model_stream_t & header ) {

    str.Write( &header.version, 1 );
    str.Write( &header.flags, 1 );
    str.Write( &header.vertexCount, 1 );
    str.Write( &header.indexCount, 1 );
    str.Write( &header.meshCount, 1 );
    str.Write( &header.vertexDataSize, 1 );
    str.Write( &header.indexDataSize, 1 );
    str.Write( &header.materialNamesSize, 1 );
    str.Write( &header.offsMeshes, 1 );
    str.Write( &header.offsMeshHashArray, 1 );
    str.Write( &header.offsMeshHashMap, 1 );
    str.Write( &header.offsVertices, 1 );
    str.Write( &header.offsIndices, 1 );
    str.Write( &header.offsInfluences, 1 );
    str.Write( &header.offsMaterialNames, 1 );
    str.Write( &header.pad, 1 );
    str.Write( header.bounds[ 0 ], 3 );
    str.Write( header.bounds[ 1 ], 3 );

}

//======================================================================================================================
void ModelBuilder::WriteToStream( ToolMemStream & str, bool skinned ) {
    math::Vec3 bmin = math::Vec3::ZERO;
    math::Vec3 bmax = math::Vec3::ZERO;
    CalcShellBounds( bmin, bmax );
    
    xprintf(" == Model Stats ==\n");
    xprintf("   Mesh Count : %u\n", meshList.size() );
    xprintf("      Skinned : %s\n", skinned == true ? "YES" : "NO" );
    xprintf("   Min Bounds : <%4.4f, %4.4f, %4.4f>\n", bmin.X(), bmin.Y(), bmin.Z() );
    xprintf("   Max Bounds : <%4.4f, %4.4f, %4.4f>\n", bmax.X(), bmax.Y(), bmin.Z() );
    xprintf("   Dimensions : <%4.4f, %4.4f, %4.4f>\n", bmax.X() -  bmin.X(), bmax.Y() -  bmin.Y(), bmax.Z() -  bmin.Z() );
    
    // Mesh data relies on material names being written, so
    // do this now.
    WriteMaterials();
    
    // Write out the mesh dats to individual streams, which we'll
    // then merge into a single stream.
    uint32_t indexCount = 0;
    uint32_t vertexCount = 0;
    
    for ( int m = 0; m < meshList.size(); ++m ) {
        WriteMesh( meshList[ m ], indexCount, vertexCount, skinned );
    }
    
    // Write out the complete model stream
    
    model_stream_t header;
    memset( &header, 0, sizeof( header ) );
    header.version        = MODEL_STREAM_VERSION;
    header.indexCount     = indexCount;
    header.vertexCount    = vertexCount;
    header.bounds[ 0 ][ 0 ] = bmin.X();
    header.bounds[ 0 ][ 1 ] = bmin.Y();
    header.bounds[ 0 ][ 2 ] = bmin.Z();
    header.bounds[ 1 ][ 0 ] = bmax.X();
    header.bounds[ 1 ][ 1 ] = bmax.Y();
    header.bounds[ 1 ][ 2 ] = bmax.Z();
    header.meshCount      = ( uint32_t ) meshList.size();
    header.indexDataSize  = ( uint32_t ) m_indexStream.Length();
    header.vertexDataSize = ( uint32_t ) m_vertexStream.Length();
    header.materialNamesSize = (uint32_t) m_materialNames.Length();
    
    if ( skinned == true ) {
        header.flags |= F_MODEL_STREAM_SKINNED;
    }
    
    uintptr_t headerPos = str.Tell();
    WriteHeader( str, header );
    
    str.PadToAlignment( 16 );
    header.offsMeshes = ( uint32_t ) ( str.Tell() - headerPos );
    str.Write( m_meshStream.GetStream(), m_meshStream.Length() );
    
    str.PadToAlignment( 16 );
    header.offsMaterialNames = ( uint32_t ) ( str.Tell() - headerPos );
    str.Write( m_materialNames.GetStream(), m_materialNames.Length() );
    
    str.PadToAlignment( 16 );
    header.offsVertices = ( uint32_t ) ( str.Tell() - headerPos );
    str.Write( m_vertexStream.GetStream(), m_vertexStream.Length() );
    
    if ( skinned == true ) {
        str.PadToAlignment( 16 );
        header.offsInfluences = ( uint32_t ) ( str.Tell() - headerPos );
        str.Write( m_influenceStream );
    }

    str.PadToAlignment( 16 );
    header.offsIndices = ( uint32_t ) ( str.Tell() - headerPos );
    str.Write( m_indexStream.GetStream(), m_indexStream.Length() );

    // Need to re-write the model header because we filled in some info
    // such as offsets to geometry.
    uintptr_t end = str.Tell();
    str.Seek( headerPos );
    WriteHeader( str, header );
    str.Seek( end );
}

//======================================================================================================================
void ModelBuilder::WriteMaterials() {
    for ( int n = 0; n < scene->materials.size(); ++n ) {
        uint64_t nameHash = FH64_CalcFromCStr( scene->materials[ n ]->name.c_str() );
        
        std::map<uint64_t, uint32_t>::iterator findIt = m_materialMap.find( nameHash );
        if ( findIt == m_materialMap.end() ) {
            m_materialMap[ nameHash ] = (uint32_t) m_materialNames.Length();
            WriteMaterialName( scene->materials[ n ]->name.c_str() );
        }
    }
}

//======================================================================================================================
void ModelBuilder::WriteMesh( MeshEntry * mesh, uint32_t & currIndexCount, uint32_t & currVertexCount, bool skinned ) {
    
    WriteVerts( mesh, skinned );
    
    WriteIndices( mesh, currIndexCount, currVertexCount );
    
    mesh_stream_t meshInfo;
    meshInfo.indexStart     = currIndexCount;
    meshInfo.indexCount     = (uint32_t) mesh->srcMesh->geometry->triangles.size() * 3;
    meshInfo.vertexStart    = currVertexCount;
    meshInfo.vertexCount    = (uint32_t) mesh->shellVerts.size();
    meshInfo.material       = 0;
    meshInfo.pad            = 0;
    
    SceneMaterial * mat = mesh->srcMesh->geometry->material;
    
    bool foundMat = FindMaterialOffset( meshInfo.material, mat->name.c_str() );
    xerror( foundMat == false, "Could not find material %s\n", mat->name.c_str() );
    
    m_meshStream.Write( &meshInfo.vertexStart, 1 );
    m_meshStream.Write( &meshInfo.vertexCount, 1 );
    m_meshStream.Write( &meshInfo.indexStart, 1 );
    m_meshStream.Write( &meshInfo.indexCount, 1 );
    m_meshStream.Write( &meshInfo.material, 1 );
    m_meshStream.Write( &meshInfo.pad, 1 );
    
    currVertexCount += (uint32_t) meshInfo.vertexCount;
    currIndexCount += (uint32_t) meshInfo.indexCount;
}

//======================================================================================================================
void ModelBuilder::WriteIndices( MeshEntry * mesh, uint32_t currIndexCount, uint32_t currVertexCount ) {
    
    SceneGeometry * geom = mesh->srcMesh->geometry;
    
    for ( int t = 0; t < geom->triangles.size(); ++t ) {
        const SceneTriangle & tri = geom->triangles[ t ];
        
        uint32_t verts[ 3 ] = {
            tri.verts[ 0 ] + currVertexCount,
            tri.verts[ 1 ] + currVertexCount,
            tri.verts[ 2 ] + currVertexCount,
        };
        
        m_indexStream.Write( verts, 3 );
    }
}

//======================================================================================================================
void ModelBuilder::WriteVerts( MeshEntry * mesh, bool skinned ) {
    
    SceneGeometry * geom = mesh->srcMesh->geometry;
    
    for ( int v = 0; v != geom->positions.size(); ++v ) {
        
        const math::Vec3 & pos = mesh->shellVerts[ v ];
        const math::Vec3 & norm = mesh->shellNormals[ v ];
        const math::Vec3 & texCoord = mesh->shellTexCords[ v ];
        
        math::Vec4 d0( pos, texCoord.X() );
        math::Vec4 d1( norm, 1.0f - texCoord.Y() );
        
        m_vertexStream.Write( (float*) &d0, 4 );
        m_vertexStream.Write( (float*) &d1, 4 );
    }
    
    if ( skinned == true ) {
        for ( int v = 0; v != mesh->shellWeights.size(); ++v ) {
            VertexWeightList & weightList = mesh->shellWeights[ v ];
            
            float influences[ 4 ] = {0, 0, 0, 0};
            uint32_t joints[ 4 ] = {0, 0, 0, 0};
            uint32_t params[ 4 ] = { (uint32_t) weightList.weights.size(), 0, 0, 0 };
            
            for( uint32_t w = 0; w < weightList.weights.size(); ++w ) {
                influences[ w ] = weightList.weights[ w ].influence;
                joints[ w ] = weightList.weights[ w ].joint;
            }
            
            this->m_influenceStream.Write( joints, 4 );
            this->m_influenceStream.Write( influences, 4 );
            this->m_influenceStream.Write( params, 4 );
        }
    }
}

//======================================================================================================================
void ModelBuilder::WriteMaterialName( const char * name ) {
    std::string matName = name;
    matName.append( ".bmat" );
    
    const char * curr = matName.c_str();
    do {
        m_materialNames.Write( curr );
        ++curr;
    } while( *curr != 0);
    
    char nullChar = 0;
    m_materialNames.Write( &nullChar );
}

//======================================================================================================================
bool ModelBuilder::FindMaterialOffset( uint32_t & offset, const char * name ) {
    uint64_t nameHash = FH64_CalcFromCStr( name );
    std::map<uint64_t, uint32_t>::iterator findIt = m_materialMap.find( nameHash );
    if ( findIt == m_materialMap.end() ) {
        return false;
    }
    
    offset = findIt->second;
    return true;
}

//======================================================================================================================
void ModelBuilder::BuildVertexWeights( MeshEntry * entry ) {
    if ( entry->srcMesh->skin != nullptr ) {
        xprintf("Building vertex weights for skinned mesh '%s'\n", entry->srcMesh->name.c_str() );
        BuildVertexWeightsFromSkin( entry );
    }
    else {
        xprintf("Building vertex weights for static mesh '%s'\n", entry->srcMesh->name.c_str() );
        BuildVertexWeightsFromParent( entry );
    }
}

//======================================================================================================================
void ModelBuilder::BuildVertexWeightsFromSkin( MeshEntry * entry ) {
    entry->shellWeights.resize( entry->srcMesh->geometry->positions.size() );
    
    SceneSkin * skin = entry->srcMesh->skin;
    
    for( uint32_t c = 0; c < skin->clusters.size(); ++c ) {
        SceneSkinCluster * cluster = skin->clusters[ c ];
        
        int32_t clusterJointIndex = skeletonBuilder->FindJointIndex( cluster->linkNode.c_str() );
        xerror( clusterJointIndex < 0, "Could not find joint for skin cluster '%s'\n", cluster->linkNode.c_str() );
        
        for ( uint32_t w = 0; w < cluster->weights.size(); ++w ) {
            VertexWeightList & weights = entry->shellWeights[ cluster->vertices[ w ] ];
            weights.AddWeight( clusterJointIndex, cluster->weights[ w ] );
        }
    }
    
    for ( uint32_t w = 0; w < entry->shellWeights.size(); ++w ) {
        VertexWeightList & weights = entry->shellWeights[ w ];
        weights.Trim( 4 );
    }
}

//======================================================================================================================
void ModelBuilder::BuildVertexWeightsFromParent( MeshEntry * entry ) {
    
    // If a static mesh doesnt have a parent, we'll just assume it's attached to the root
    // node
    int32_t parentIndex = 0;
    if ( entry->srcMesh->GetParent() != nullptr ) {
        parentIndex = skeletonBuilder->FindJointIndex( entry->srcMesh->GetParent()->name.c_str() );
        xerror( parentIndex < 0, "Unable to find parent joint for mesh '%'\n", entry->srcMesh->GetParent()->name.c_str() );
    }
    
    entry->shellWeights.resize( entry->srcMesh->geometry->positions.size() );
    
    for ( uint32_t n = 0; n < entry->shellWeights.size(); ++n ) {
        VertexWeightList & vwl = entry->shellWeights[ n ];
        vwl.AddWeight( parentIndex, 1.0f );
    }
}


//======================================================================================================================
void ModelBuilder::CalcShellBounds( math::Vec3 & bmin, math::Vec3 & bmax ) {
    meshList[0]->CalcBounds( bmin, bmax );
    
    for( int n = 1; n < meshList.size(); ++n ) {
        math::Vec3 thisMin, thisMax;
        meshList[ n ]->CalcBounds( thisMin, thisMax );
        
        bmin.Min( bmin, thisMin );
        bmax.Max( bmax, thisMax );
    }
    
}
