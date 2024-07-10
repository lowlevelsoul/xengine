//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#ifndef __TOOLAPP_H__
#define __TOOLAPP_H__

#include "mathcc/Math3d.h"
#include "toolapp/RefObject.h"
#include "toolapp/RefPointer.h"
#include "toolapp/ToolMemStream.h"
#include "toolapp/PathUtil.h"
#include "toolapp/Lexer.h"

class ToolApp {
public:

	enum TOOL_ARG_ID {
		TOOL_ARG_ID_PARAMFILE = 65536,
		TOOL_ARG_ID_INFILE,
		TOOL_ARG_ID_OUTFILE,
        TOOL_ARG_ID_PLATFORM,
        TOOL_ARG_ID_PLATFORM_BEGIN,
        TOOL_ARG_ID_PLATFORM_END,
        TOOL_ARG_ID_DEPENDS,
	};
    
    enum PLATFORM {
        PLATFORM_MACOS,
        PLATFORM_IOS,
        PLATFORM_TVOS,
        
        PLATFORM_NONE,
    };

	class Arg : public RefObject {
	public:
		typedef RefPointer<Arg> ref_ptr_t;
		typedef std::vector<ref_ptr_t>  vector_t;

		Arg() { 
		}

		virtual ~Arg() {
		}

		std::string					m_name;
		std::vector<std::string>	m_params;
	};

	ToolApp();

	virtual ~ToolApp();

	bool Run( int argc, const char ** argv );

	bool HandleArgInternal( Arg * arg, int32_t argId );

	virtual bool HandleArg( Arg * arg, int32_t argId );

	virtual bool Process();

	virtual bool DisplayBuiltinArgHelp( TOOL_ARG_ID argId );

	void BuildArgs( int argc, const char ** argv );

	void PublishArgId( int32_t argId, const char * argName );

	int32_t FindArgId( const char * argName );

	bool CreateFolderAtPath( const char * path );

	void LoadParamFile( const char * path );

	void ParseParams( const char* buffer );
    
    PLATFORM PlatformStringToId( const char * ) const;
    
    static const std::string VFormat( const char * fmt, ... );
    
    static const std::string VFormat( const char * fmt, va_list args );
    
    static void SplitPath( std::vector<std::string> & result, const char * path );

protected:
	typedef std::map<uint64_t, int32_t> arg_id_map_t;

	Arg::vector_t			m_args;
	arg_id_map_t			m_argIdMap;

	std::string				m_infilePath;
	std::string				m_outfilePath;
    std::string             paramsFilesPath;
    PLATFORM                currPlatform = PLATFORM_NONE;
    PLATFORM                targetPlatform = PLATFORM_NONE;
    bool                    outputDepends = false;
    uint32_t                currArgIndex = 0;
};

#define TOOL_APP(CLASS)\
	static uint8_t toolAppMem[ sizeof( CLASS ) ];\
	ToolApp * const tool = (ToolApp*) toolAppMem;\
	extern void ToolAppInitialise();\
	extern void ToolAppFinalise();\
	int main(int argc, char ** argv ) {\
		ToolAppInitialise();\
		new(toolAppMem) CLASS;\
		int retCode = tool->Run( argc, (const char **) argv );\
		tool->~ToolApp();\
		ToolAppFinalise();\
		return retCode;\
	}

extern ToolApp * const tool;

#ifndef SAFE_DELETE
#   define SAFE_DELETE(X) if ( ( X ) != nullptr ) { delete ( X ); ( X ) = nullptr; }
#endif

#endif
