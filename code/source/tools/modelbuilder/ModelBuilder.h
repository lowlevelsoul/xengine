//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __MODELBUILDER_H__
#define __MODELBUILDER_H__

#include "toolapp/ToolMemStream.h"
#include "render/ModelStream.h"
#include "modelbuilder/SkeletonBuilder.h"

#include "scene/Scene.h"
#include "scene/SceneMesh.h"
#include "scene/SceneNode.h"

class ModelBuilder {
public:
    class Options {
    public:
        static const Options DEFAULT;
        
        Options() {
            scale = 1;
            flipFaces = false;
            forceStatic = true;
            genNormals = false;
            stripMixamo = false;
            center = false;
            forceMaterial = false;
        }

        ~Options() {

        }
        
        Options & operator=( const Options & rhs );

    public:
        float                       scale;
        bool                        flipFaces;
        bool                        forceStatic;
        bool                        genNormals;
        bool                        stripMixamo;
        bool                        center;
        std::vector<std::string>    meshFilter;
        std::vector<std::string>    meshIgnore;
        bool                        forceMaterial;
        std::string                 forceMaterialName;
        std::string                 rootNode;
    };
    
    class VertexWeightList {
    public:
        struct Weight {
            uint32_t        joint;
            float           influence;
            
            typedef std::vector<Weight> array_t;
        };
        
        void AddWeight( uint32_t joint, float influence ) {
            Weight weight = { joint, influence };
            
            if ( weights.empty() == true ) {
                // No need to look for an insertion position as the list is empty.
                // Just append.
                weights.push_back( weight );
            }
            else {
                // Look for the insertion position, which is the point were the
                // influence us smaller than the one we're trying to add
                int32_t insertPos = -1;
                for ( int n = 0; n < weights.size(); ++n ) {
                    if ( weights[n].influence < influence ) {
                        insertPos = n ;
                        break;
                    }
                }
                
                if ( insertPos < 0 ) {
                    // Influence was smaller than any in the list, so just append
                    weights.push_back( weight );
                }
                else {
                    weights.insert( weights.begin() + insertPos, weight );
                }
            }
        }
        
        void Trim( size_t maxWeights ) {
            if ( weights.size() > maxWeights ) {
                weights.resize( maxWeights );
                
                // Calcuate the total influnences (should be < 1 )
                float total = 0;
                for ( int n = 0; n < maxWeights; ++n ) {
                    total += weights[ n ].influence;
                }
                
                // Adjust the infuences so that they have a total of 1
                float factor = 1.0f / total;
                for ( int n = 0; n < maxWeights; ++n ) {
                    weights[ n ].influence *= factor;
                }
            }
        }
        
        
        Weight::array_t        weights;
    };
    
    class MeshEntry : public RefObject {
    public:
        typedef RefPointer<MeshEntry> ptr_t;
        typedef std::vector<ptr_t> array_t;
        
        MeshEntry() {}
        
        virtual ~MeshEntry() {}
        
        void CalcBounds( math::Vec3 & bmin, math::Vec3 & bmax ) {
            bmin = shellVerts[ 0 ];
            bmax = bmin;
            
            for ( int n = 1; n < shellVerts.size(); ++n ) {
                bmin.Min( bmin, shellVerts[ n ] );
                bmax.Max( bmax, shellVerts[ n ] );
            }
        }
        
        SceneMesh *                     srcMesh;
        std::vector<math::Vec3>         shellVerts;
        std::vector<math::Vec3>         shellNormals;
        std::vector<math::Vec3>         shellTexCords;
        std::vector<uint32_t>           shellIndices;
        std::vector<VertexWeightList>   shellWeights;
    };
    
    ModelBuilder( const Options & options_ = Options::DEFAULT );
    
    ~ModelBuilder();
    
    void Compile( const char * path );
    
    void GatherMeshes( );
    
    void WriteHeader( ToolMemStream & str, model_stream_t & header );
    
    void WriteMaterialName( const char * name );
    
    bool FindMaterialOffset( uint32_t & offset, const char * name );
    
    bool HasMeshListSkinnedMeshes();
    
    void ProcessMeshes();
    
    void WriteToStream( ToolMemStream & str, bool skinned );
    
    void WriteMaterials();
    
    void WriteMesh( MeshEntry * mesh, uint32_t & currIndexCount, uint32_t & currVertexCount, bool skinned );
    
    void WriteIndices( MeshEntry * mesh, uint32_t currIndexCount, uint32_t currVertexCount );
    
    void WriteVerts( MeshEntry * mesh, bool skinned );
    
    void WriteSkeleton( const char * modelPath, ToolMemStream & str );
    
    void BuildVertexWeights( MeshEntry * entry );
    
    void BuildVertexWeightsFromSkin( MeshEntry * entry );
    
    void BuildVertexWeightsFromParent( MeshEntry * entry );
    
    void WriteMaterial( const char * srcModelPath, const char * dstPath, SceneMaterial * mat );
    
    void CalcShellBounds( math::Vec3 & bmin, math::Vec3 & bmax );
    
    void WriteMaterialTextureParams( const char * modelDestPath );
    
public:
    std::string                         m_input;
    
    Options                             m_options;
    ToolMemStream                       m_vertexStream;
    ToolMemStream                       m_influenceStream;
    ToolMemStream                       m_indexStream;
    ToolMemStream                       m_meshStream;
    ToolMemStream                       m_materialNames;
    ToolMemStream                       skeletonStream;
    
    Scene::ptr_t                        scene;
    MeshEntry::array_t                  meshList;       ///< List of meshes we want to export to the model
    SkeletonBuilder *                   skeletonBuilder = nullptr;
    
    std::map<uint64_t, uint32_t>        m_materialMap;
};

#endif 
