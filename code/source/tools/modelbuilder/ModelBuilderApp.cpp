//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#include "modelbuilder/ModelBuilderApp.h"

TOOL_APP( ModelBuilderApp )

static const char HELP_INFILE[] = {
		"+infile <filename>\n\n"
		" Specifies the path and file name of the input model file to be converted\n"
};

static const char HELP_OUTFILE[] = {
		"+outfile <filename>\n\n"
		" Specifies the path and file name of the output model file\n"
};

static const char HELP_PARAMFILE[] = {
		"+prams <filename>\n\n"
		" Specifies the path to a file containing arguments for the tool\n"
};

static const char HELP_SCALE[] = {
		"+scale <scale>\n\n"
		"Scales the model by the value given in <scale>\n"
};

static const char HELP_FLIPFACES[] = {
		"+flipfaces\n\n"
		"Reverses the winding order of the triangles\n"
};

static const char HELP_INFO[] = {
		"+info\n\n"
		"Logs out a load of imporant information about the loaded model\n"
};

static const char HELP_MESHFILTER[] = {
		"+meshfilter <mesh 1>...<mesh n>\n\n"
		"Define a list of meshes in the source model to load\n"
};

static const char HELP_GENNORMALS[] = {
		"+gennormals\n"
		"Generate smooth normals\n"
};

static const char HELP_SKELETON[] = {
        "+skeleton\n"
        "Builds a skeleton and write to the file specified by +outfile\n"
};

static const char HELP_MESHIGNORE[] = {
		"+meshignore <mesh 1>...<mesh n>\n\n"
		"Define a list of meshes in the source model to ignore from source\n"
};

static const char HELP_STRIPMIXAMO[] = {
		"+stripmixamo\n\n"
		"Strip any Mixamo end-marker joints\n"
};

static const char HELP_FORCEMATERIAL[] = {
        "+forcematerial <material name>\n"
        "Force the model meshes to use a single material with a given name\n"
        "which also means that multi-material meshes will not be split up.\n"
};



static const char * HELP_TEXT[] = {
	HELP_INFILE,
	HELP_OUTFILE,
	HELP_PARAMFILE,
	HELP_SCALE,
	HELP_FLIPFACES,
	HELP_INFO,
	HELP_MESHFILTER,
	HELP_GENNORMALS,
    HELP_SKELETON,
	HELP_MESHIGNORE,
	HELP_STRIPMIXAMO,
	nullptr
};

enum ARG {
	ARG_INFILE,		// Dummy for 
	ARG_OUTFILE,
	ARG_PARAMFILE,
	ARG_SCALE,
	ARG_FLIPFACES,
	ARG_INFO,
	ARG_MESHFILTER,
	ARG_GENNORMALS,
    ARG_SKELETON,
	ARG_MESHIGNORE,
	ARG_STRIPMIXAMO,
    ARG_ROOT,
};

//==========================================================================================================================================
ModelBuilderApp::ModelBuilderApp() {
	PublishArgId( ARG_SCALE, "scale" );
	PublishArgId( ARG_FLIPFACES, "flipfaces" );
	PublishArgId( ARG_INFO, "info" );
	PublishArgId( ARG_MESHFILTER, "meshfilter" );
	PublishArgId( ARG_GENNORMALS, "gennormals" );
    PublishArgId( ARG_SKELETON, "skeleton" );
	PublishArgId( ARG_MESHIGNORE, "meshignore" );
	PublishArgId( ARG_STRIPMIXAMO, "stripmixamo" );
    PublishArgId( ARG_ROOT, "rootnode" );

	m_scale = 1;
	m_flipFaces = false;
	m_logInfo = false;
	m_genNormals = false;
    m_mode = MODE_MODEL;
	m_cullMixamo = false;
}

//==========================================================================================================================================
ModelBuilderApp::~ModelBuilderApp() {

}

//==========================================================================================================================================
void ModelBuilderApp::DisplayHelpText( int32_t argId ) {
	if ( argId < 0 ) {
		for ( int32_t i = 0; HELP_TEXT[ i ] != nullptr; ++i ) {
			printf( "%s", HELP_TEXT[ i ] );
		}
	}
	else {
		printf( "%s", HELP_TEXT[ argId ] );
	}
}

//==========================================================================================================================================
bool ModelBuilderApp::DisplayBuiltinArgHelp( TOOL_ARG_ID argId ) {
	switch ( argId ) {
		case TOOL_ARG_ID_PARAMFILE:
			DisplayHelpText( ARG_PARAMFILE );
			return true;
		case TOOL_ARG_ID_INFILE:
			DisplayHelpText( ARG_INFILE );
			return true;
		case TOOL_ARG_ID_OUTFILE :
			DisplayHelpText( ARG_OUTFILE );
			return true;
	}

	return false;
}

//==========================================================================================================================================
bool ModelBuilderApp::HandleArg( Arg * arg, int32_t argId ) {

	switch ( argId ) {	
		case ARG_SCALE:
			if ( arg->m_params.size() != 1 ) {
				DisplayHelpText( ARG_SCALE );
				return false;
			}

			m_buildOptions.scale = ( float ) atof( arg->m_params[0].c_str() );
			break;

		case ARG_FLIPFACES:
			if ( arg->m_params.empty() == false ) {
				DisplayHelpText( ARG_FLIPFACES );
				return false;
			}
			m_buildOptions.flipFaces = true;

			break;

		case ARG_INFO:
			if ( arg->m_params.empty() == false ) {
				DisplayHelpText( ARG_INFO );
				return false;
			}
            m_mode = MODE_INFO;
			break;

		case ARG_MESHFILTER:
			if ( arg->m_params.empty() == true ) {
				DisplayHelpText( ARG_MESHFILTER );
				return false;
			}

			for ( auto p : arg->m_params ) {
				m_buildOptions.meshFilter.push_back( p );
			}

			break;

		case ARG_GENNORMALS:
			if ( arg->m_params.empty() == false ) {
				DisplayHelpText( ARG_GENNORMALS );
				return false;
			}

			m_buildOptions.genNormals = true;
			break;
            
        case ARG_SKELETON:
            if ( arg->m_params.empty() == false ) {
                DisplayHelpText( ARG_SKELETON );
                return false;
            }

            m_mode = MODE_SKELETON;
            break;

		case ARG_MESHIGNORE:
			if ( arg->m_params.empty() == true ) {
				DisplayHelpText( ARG_MESHIGNORE );
				return false;
			}

			for ( auto p : arg->m_params ) {
				m_buildOptions.meshIgnore.push_back( p );
			}

			break;

		case ARG_STRIPMIXAMO:
			m_buildOptions.stripMixamo = true;
			break;
            
        case ARG_ROOT:
            m_buildOptions.rootNode = arg->m_params[0];
            break;
            
		default:
			DisplayHelpText( -1 );
			return false;
	}

	return true;
}

//==========================================================================================================================================
bool ModelBuilderApp::Process() {
    
    if ( m_infilePath.empty() == true || m_outfilePath.empty() == true ) {
        DisplayHelpText( -1 );
        return false;
    }
    
    BuildModel();

	return true;
}

//==========================================================================================================================================
void ModelBuilderApp::BuildModel() {
    
    if ( m_outfilePath.empty() == true ) {
        DisplayHelpText( -1 );
        Sys_Exit( -1 );
    }
    
    ModelBuilder builder( m_buildOptions );
    
    builder.m_input = m_infilePath;
    builder.Compile( m_outfilePath.c_str() );
}
