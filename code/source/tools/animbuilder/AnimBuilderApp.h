//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2023 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef __ANIMBUILDERAPP_H__
#define __ANIMBUILDERAPP_H__

#include "util/RefObject.h"
#include "util/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "container/Array.h"
#include "scene/Scene.h"

class AnimBuilderApp : public ToolApp {
public:
    AnimBuilderApp();

    virtual ~AnimBuilderApp();

    virtual bool HandleArg( Arg * arg, int32_t argId );

    virtual bool Process();

    virtual bool DisplayBuiltinArgHelp( TOOL_ARG_ID argId );

    void DisplayHelpText( int32_t argId );
    
    void SaveSingleAnims();
    
    void BuildAndSaveDictionary();
    
public:
    class AnimEntry : public fx::util::RefObject {
    public:
        typedef fx::util::RefPointer<AnimEntry> ref_ptr_t;
        typedef fx::Array<ref_ptr_t> array_t;
        
        AnimEntry() {
            scale = 1;
            startFrame = -1;
            endFrame = -1;
        }
        
        virtual ~AnimEntry() {
            
        }
                
        fx::String              path;                   ///< Input path of the animation that we want to create
        fx::String              name;                   ///< The name of the animation in the dictionaty
        fx::String              rootNode;
        float                   scale = 1;              ///< Amount to scale movement keys keys by
        Scene::ptr_t            scene;                  ///< Pointer to the animation that was loaded
        SceneAnimation::ptr_t   clipAnim;               ///< The animation clip that was created ( if this is a clip )
        AnimEntry *             parent = nullptr;       ///< For animation clips, this will point to the parent animation
        array_t                 children;               ///< List of child animation clips
        int32_t                 startFrame = 0;
        int32_t                 endFrame = 0;
        bool                    hasChidren = false;
        
        ToolMemStream           compiledBinary;         ///< Stream containing the compiled binary for the animation
    };
    
    AnimEntry::array_t      anims;                  ///< Array of anims that we want to load and build
    AnimEntry *             currAnim = nullptr;     ///< The current animation being setup by parameters
    AnimEntry *             currClip = nullptr;     ///< The current clip animation being setup up by parameters
    bool                    makeDictionary = false;
};

#endif
