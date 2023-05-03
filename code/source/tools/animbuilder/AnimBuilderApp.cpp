//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "AnimBuilderApp.h"
#include "AnimBuilder.h"
#include "scene/SceneImporterAssimp.h"
#include "toolapp/DictionaryBuilder.h"

TOOL_APP( AnimBuilderApp )

enum ARG {
    ARG_ANIM,
    ARG_SCALE,
    ARG_NAME,
    ARG_ROOTNODE,
    ARG_BEGINCLIP,
    ARG_ENDCLIP,
    ARG_START,
    ARG_END,
    ARG_DICTIONARY,
};

//=======================================================================================================================
AnimBuilderApp::AnimBuilderApp() {
    PublishArgId( ARG_ANIM, "anim");
    PublishArgId( ARG_SCALE, "scale");
    PublishArgId( ARG_NAME, "name");
    PublishArgId( ARG_ROOTNODE, "rootnode");
    PublishArgId( ARG_BEGINCLIP, "clip");
    PublishArgId( ARG_ENDCLIP, "endclip");
    PublishArgId( ARG_START, "start");
    PublishArgId( ARG_END, "end");
    PublishArgId( ARG_DICTIONARY, "dictionary");
    
    currAnim = nullptr;
}

//=======================================================================================================================
AnimBuilderApp::~AnimBuilderApp() {
    
}

//=======================================================================================================================
bool AnimBuilderApp::HandleArg( Arg * arg, int32_t argId ) {
    switch( argId ) {
            
        case ARG_ANIM : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_ANIM );
                return false;
            }
            
            AnimEntry * entry = new AnimEntry;
            entry->path = arg->m_params[0].c_str();
            anims.Append( entry );
            
            currAnim = entry;
            break;
        }
            
        case ARG_SCALE : {
            if ( currAnim == nullptr || arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_SCALE );
                return false;
            }
            
            currAnim->scale = (float) atof( arg->m_params[0].c_str() );
            break;
        }
            
        case ARG_NAME : {
            if ( currAnim == nullptr || arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_NAME );
                return false;
            }
            
            currAnim->name = arg->m_params[0].c_str();
            break;
        }
            
        case ARG_ROOTNODE : {
            if ( currAnim == nullptr || arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_ROOTNODE );
                return false;
            }
            
            currAnim->rootNode = arg->m_params[0].c_str();
            
            break;
        }
            
        case ARG_BEGINCLIP : {
            fxerror( currAnim == nullptr, "No animation has been specified before clip\n");
            
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_BEGINCLIP );
                return false;
            }
            
            currClip = new AnimEntry;
            currClip->name      = arg->m_params[ 0 ].c_str();
            currClip->parent    = currAnim;
            currClip->scale     = currAnim->scale;
            currClip->rootNode  = currAnim->rootNode;
            
            anims.Append( currClip );
            
            currAnim->children.Append( currClip );
            currAnim->hasChidren = true;
                        
            break;
        }
            
        case ARG_ENDCLIP : {
            fxerror( currClip == nullptr, "No beginclip / endclip mismatch\n");
            
            if ( arg->m_params.size() != 0 ) {
                DisplayHelpText( ARG_ENDCLIP );
                return false;
            }
            
            currClip = nullptr;
                        
            break;
        }
            
        case ARG_START: {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_START );
                return false;
            }
            
            if ( currClip != nullptr ) {
                currClip->startFrame = atoi( arg->m_params[ 0 ].c_str() );
            }
            
            break;
        }
            
        case ARG_END: {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( ARG_END );
                return false;
            }
            
            if ( currClip != nullptr ) {
                currClip->endFrame = atoi( arg->m_params[ 0 ].c_str() );
            }
            
            break;
        }
            
        case ARG_DICTIONARY : {
            if ( arg->m_params.empty() == false ) {
                DisplayHelpText( ARG_DICTIONARY );
                return false;
            }
            
            makeDictionary = true;
            break;
        }
            
            
    }
    return true;
}

//=======================================================================================================================
bool AnimBuilderApp::Process() {
    
    for ( AnimEntry::array_t::iterator_t entryIt = anims.begin(); entryIt != anims.end(); ++entryIt ) {

        AnimEntry * entry = *entryIt;
        
        if ( entry->parent == nullptr ) {
            
            // Loading an animation from a file
            
            fxprintf( "Loading animation %s\n", entry->path.GetCStr() );
            
            SceneImporter::Options importerOptions;
            importerOptions.scale = entry->scale;
            importerOptions.importAnims = true;
            importerOptions.importMeshes = false;
            
            if ( entry->rootNode.Empty() == false ) {
                importerOptions.rootNode = entry->rootNode.GetCStr();
            }
            
            SceneImporterAssImp importer( importerOptions );
            bool loadOk = importer.Import( entry->path.GetCStr() );
            fxerror(loadOk == false, "Uncaught error whilst loading scene %s\n", entry->path.GetCStr() );
            
            entry->scene = importer.scene;
            fxerror( entry->scene->animations.IsEmpty() == true, "Scene %s does not contain any animations", entry->path.GetCStr() );
            
            SceneAnimation * srcAnim = entry->scene->animations[ 0 ];
            srcAnim->ConvertToFrames();
            
            fxprintf("          FPS : %i\n", srcAnim->fps );
            fxprintf("  Frame Count : %i\n", srcAnim->frameCount );
            fxprintf("  Joint Count : %i\n", srcAnim->animJoints.GetLength() );
        }
        else {
            // Creating a clip from a loaded animation
            SceneAnimation * srcAnim = entry->parent->scene->animations[ 0 ];
            
            fxprintf(" Creating Clip : %s\n", entry->name.GetCStr() );
            fxprintf("   Start Frame : %i\n", entry->startFrame );
            fxprintf("     End Frame : %i\n", entry->endFrame );
            
            SceneAnimation * clipAnim = srcAnim->CreateFrameClip( entry->startFrame, entry->endFrame );
            clipAnim->name      = entry->name.GetCStr();
            
            entry->name         = entry->name.GetCStr();
            entry->clipAnim     = clipAnim;
            entry->scene        = entry->parent->scene;
        }
    }

    for ( AnimEntry::array_t::iterator_t entryIt = anims.begin(); entryIt != anims.end(); ++entryIt ) {
        
        AnimEntry * entry = *entryIt;
        if ( entry->hasChidren == false ) {
            
            AnimBuilder::Options opt;
            opt.scale = entry->scale;
            opt.rootNode = entry->rootNode.GetCStr();
            
            SceneAnimation * srcAnim = ( entry->clipAnim == nullptr ) ? entry->scene->animations[ 0 ] : entry->clipAnim;
            
            fxprintf("    Compiling binary for %s \n", entry->name.GetCStr() );
            
            AnimBuilder builder( opt );
            builder.Build( entry->compiledBinary, entry->scene, srcAnim );
        }
    }
    
    // Write out individual anims or a dictionary
    if ( makeDictionary == true ) {
        BuildAndSaveDictionary();
    }
    else {
        SaveSingleAnims();
    }
    
    return true;
}

//=======================================================================================================================
void AnimBuilderApp::SaveSingleAnims() {
    
    fxprintf("Writing individual anims\n");
    
    for ( AnimEntry::array_t::iterator_t entryIt = anims.begin(); entryIt != anims.end(); ++entryIt ) {
        
        AnimEntry * entry = *entryIt;
        
        if ( entry->hasChidren == false ) {
            
            std::string outPath = m_outfilePath;
            sys->AppendPath(outPath, entry->name.GetCStr() );
            outPath.append( ".anim" );
            
            fxprintf("    Writing %s\n", outPath.c_str() );
            this->CreateFolderAtPath( outPath.c_str() );
            fx::ScopedFile file( outPath.c_str(), "wb" );
            
            fxerror( file.IsValid() == false, "Error opening %s for writing\n", outPath.c_str() );
            
            file->Write( entry->compiledBinary.GetStream() , sizeof(uint8_t), entry->compiledBinary.Length() );
        }
    }
}

//=======================================================================================================================
void AnimBuilderApp::BuildAndSaveDictionary() {
    
    fxprintf("Building dictionary\n");
    
    DictionaryBuilder builder;
    builder.SetDictionaryType( FX_MAKE_ID(A, n, i, m, a, t, i, o, n, s, _, _ ) );
    
    for ( AnimEntry::array_t::iterator_t entryIt = anims.begin(); entryIt != anims.end(); ++entryIt ) {
        
        AnimEntry * entry = *entryIt;
        if ( entry->hasChidren == false ) {
            builder.AddEntry( entry->name.GetCStr(), entry->compiledBinary.GetStream(), entry->compiledBinary.Length() );
        }
    }
    
    ToolMemStream dictionaryStream;
    builder.WriteToStream( dictionaryStream );
    
    fxprintf("Writing dictionary '%s'\n", m_outfilePath.c_str() );
    
    CreateFolderAtPath( m_outfilePath.c_str() );
    {
        fx::ScopedFile file( m_outfilePath.c_str(), "wb" );
        fxerror( file.IsValid() == false, "Error opening %s for writing\n", m_outfilePath.c_str() );
        file->Write( dictionaryStream.GetStream() , sizeof(uint8_t), dictionaryStream.Length() );
    }
}

//=======================================================================================================================
bool AnimBuilderApp::DisplayBuiltinArgHelp( TOOL_ARG_ID argId ) {
    
    
    return true;
}

//=======================================================================================================================
void AnimBuilderApp::DisplayHelpText( int32_t argId ) {
    
    
}



