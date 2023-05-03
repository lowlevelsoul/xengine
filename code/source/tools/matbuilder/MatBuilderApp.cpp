//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022, 2023 James Steele. All Rights Reserved.
//======================================================================================================================

#include "matbuilder/MatBuilderApp.h"
#include "matbuilder/MatBuilder.h"

TOOL_APP( MatBuilderApp )

enum ARG {
    ARG_MATERIAL = 0,
    ARG_ALBEDO,
    ARG_AMR,
    ARG_GLOW,
    ARG_TRANSPARENCY,
};

static const char * HELP_TEXT_MATERIAL =
    "+material <path>          Defines the start of a material and the path we want to save to\n";

static const char * HELP_TEXT_ALBEDO =
    "+albedo <path>            Sets the path to the albedo texture\n";

static const char * HELP_TEXT_AMR =
    "+amr <path>               Sets the path to the amr texture\n";

static const char * HELP_TEXT_GLOW =
    "+glow <path>              Sets the path to the glow texture\n";

static const char * HELP_TEXT_TRANSPARENCY =
    "+transparency <alpha>     Sets the transparency value for the material\n";

static const char * HELP_TEXT[] = {
    HELP_TEXT_MATERIAL,
    HELP_TEXT_ALBEDO,
    HELP_TEXT_AMR,
    HELP_TEXT_GLOW,
    HELP_TEXT_TRANSPARENCY,
    nullptr,
    nullptr
};

//======================================================================================================================
MatBuilderApp::MatBuilderApp() {
    PublishArgId( ARG_MATERIAL, "material" );
    PublishArgId( ARG_ALBEDO, "albedo" );
    PublishArgId( ARG_AMR, "amr" );
    PublishArgId( ARG_GLOW, "glow" );
    PublishArgId( ARG_TRANSPARENCY, "transparency" );
}

//======================================================================================================================
MatBuilderApp::~MatBuilderApp() {
    
}

//======================================================================================================================
bool MatBuilderApp::HandleArg( Arg * arg, int32_t argId ) {
    switch ( argId ) {
        case ARG_MATERIAL : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            currMat = new MaterialDef;
            currMat->path = arg->m_params[0].c_str();
            
            materialDefs.push_back( currMat );
            
            break;
        }
            
        case ARG_ALBEDO : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currMat == nullptr, "Mission material before texture specifier\n");
            
            currMat->materialInfo.albedoTexture = arg->m_params[0].c_str();
            
            break;
        }

        case ARG_AMR: {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currMat == nullptr, "Mission material before texture specifier\n");
            
            currMat->materialInfo.amrTexture = arg->m_params[0].c_str();
            
            break;
        }
            
        case ARG_GLOW : {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currMat == nullptr, "Mission material before texture specifier\n");
            
            currMat->materialInfo.glowTexture = arg->m_params[0].c_str();
            break;
        }
            
        case ARG_TRANSPARENCY: {
            if ( arg->m_params.size() != 1 ) {
                DisplayHelpText( argId );
            }
            
            xerror( currMat == nullptr, "Mission material before transparency specifier\n");
            
            currMat->materialInfo.transparency = (float) atof( arg->m_params[0].c_str() );
            break;
        }
    }
    
    return true;
}

//======================================================================================================================
bool MatBuilderApp::Process() {
    
    for ( int m = 0; m < materialDefs.size(); ++m ) {
        
        MaterialDef * matDef = materialDefs[ m ];

        ToolMemStream str;
        MatBuilder builder;
        builder.Build( str, matDef->materialInfo );
        
        bool folderOk = CreateFolderAtPath( matDef->path.c_str() );
        xerror( folderOk == false, "Error creating folder for %", matDef->path.c_str() );
        
        xprintf("Writing material '%s'\n", matDef->path.c_str() );
        
        {
            xeScopedFile file( matDef->path.c_str(), "wb" );
            xerror( file.IsValid() == false, "Error opening '%s' for writing\n", matDef->path.c_str() );
            file.Write( str.GetStream(), 1, str.Length() );
        }
    }
        
    return true;
}

//======================================================================================================================
bool MatBuilderApp::DisplayBuiltinArgHelp( TOOL_ARG_ID argId ) {
    return true;
}

//======================================================================================================================
void MatBuilderApp::DisplayHelpText( int32_t argId ) {
    if ( argId < 0 ) {
        for ( int n = 0; HELP_TEXT[ n ] != nullptr; ++n ) {
            xprintf("%s", HELP_TEXT[ n ] );
        }
    }
    else {
        xprintf("%s", HELP_TEXT[ argId ] );
    }
    
    Sys_Exit( -1 );
}


