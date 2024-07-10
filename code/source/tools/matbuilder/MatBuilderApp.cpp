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
    return true;
}

//======================================================================================================================
bool MatBuilderApp::Process() {
    
    ToolMemStream stream;
    MatBuilder builder;
    
    xprintf("Building materials from %s\n", m_infilePath.c_str() );
    builder.Build( stream, m_infilePath.c_str() );
    
    xprintf("Writing materials to %s\n", m_outfilePath.c_str() );
    bool folderOk = CreateFolderAtPath( m_outfilePath.c_str() );
    xerror( folderOk == false, "Unable to create folder at %s\n", m_outfilePath.c_str() );
    
    
    stream.Save( m_outfilePath.c_str() );
    

    xprintf("Done.\n");
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


