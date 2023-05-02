//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//=======================================================================================================================

#include "mem/Mem.h"
#include "core/Sys.h"
#include "core/Fs.h"
#include "core/Str.h"
#include "toolapp/ToolApp.h"
#include "toolapp/Lexer.h"

/*
class ToolPrintCallback : public SystemPrintCallback {
public:
	void PrintString( const char * str ) {
		printf( "%s", str );
	}
};

static ToolPrintCallback toolPrintCb; */


/*
class ToolReporter : public LogReporter {
public:
	ToolReporter() {
	}

	~ToolReporter() {

	}

	void Printf(const LogCat_t& cat, const char* fmt, va_list vargs) {
		// initialize use of the variable argument array
		va_list vaCopy;
    
		va_copy(vaCopy, vargs);
		printf("%s : ", cat.m_name);            // Print the log catgegory
		vprintf(fmt, vaCopy);                   // Use vprintf to print the log message
		va_end(vaCopy);
	}
};

static ToolReporter toolLogReorter;
*/

static const char * PLATFORM_STRINGS[] = {
    "macos",
    "ios",
    "tvos",
    nullptr,
};

extern void FxCore_RegisterCVars();

//=======================================================================================================================
void ToolAppInitialise() {
    Mem_Initialise( nullptr );
    Sys_Initialise();
    FS_Initialise();
}

//=======================================================================================================================
void ToolAppFinalise() {

}

//=======================================================================================================================
ToolApp::ToolApp() {
	PublishArgId( TOOL_ARG_ID_PARAMFILE, "params" );
	PublishArgId( TOOL_ARG_ID_INFILE, "infile" );
	PublishArgId( TOOL_ARG_ID_OUTFILE, "outfile" );
    PublishArgId( TOOL_ARG_ID_PLATFORM, "platform" );
    PublishArgId( TOOL_ARG_ID_PLATFORM_BEGIN, "platform_begin" );
    PublishArgId( TOOL_ARG_ID_PLATFORM_END, "platform_end" );
    PublishArgId( TOOL_ARG_ID_DEPENDS, "depends" );

}

//=======================================================================================================================
ToolApp::~ToolApp() {

}

//=======================================================================================================================
bool ToolApp::Run( int argc, const char ** argv ) {	
	// Build the argument list for the tool
	BuildArgs( argc, argv );

	uint32_t start = 0;
    currArgIndex = 0;

	// Handle each of the args that we found
	do {
		size_t count = m_args.size();

		for ( int n = start; n < count; ++n ) {
			int32_t argId = FindArgId( m_args[ n ]->m_name.c_str() );
			bool ok = HandleArgInternal( m_args[ n ], argId );

			if ( ok == false ) {
				return false;
			}
            
            ++currArgIndex;
		}

		// We may have added more args while processing, so check to see if we did
		// and loop around again
		if ( count == m_args.size() ) {
			// Didn't add any new args, so just bail.
			break;
		}
		else {
			// We added new args, so process them starting from the 
			// first new arg that was added.
			start = (uint32_t) count;
		}

	} while ( true );
    
    // Check for unterminated platform_begin/platform_end pairs
    xerror( currPlatform != PLATFORM_NONE, "Unterminated platform_begin\n" );

	return Process();
}

//=======================================================================================================================
bool ToolApp::HandleArgInternal( Arg * arg, int32_t argId ) {

	bool handled = false;

	switch ( argId ) {
		case TOOL_ARG_ID_PARAMFILE:
		{
			if ( arg->m_params.size() != 1 ) {
				if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_PARAMFILE ) == false ) {
					xerror( arg->m_params.size() != 1, "Invalid number of params for arg 'params'\n" );
				}
				return false;
			}

			LoadParamFile( arg->m_params[ 0 ].c_str() );
			handled = true;
			break;
		}			

		case TOOL_ARG_ID_INFILE:
			if ( arg->m_params.size() != 1 ) {
				if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_INFILE ) == false ) {
					xerror( arg->m_params.size() != 1, "Invalid number of params for arg 'infile'\n" );
				}
				return false;
			}
			m_infilePath = arg->m_params[ 0 ].c_str();
			handled = true;
			break;

		case TOOL_ARG_ID_OUTFILE:
			if ( arg->m_params.size() != 1 ) {
				if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_OUTFILE ) == false ) {
					xerror( arg->m_params.size() != 1, "Invalid number of params for arg 'outfile'\n" );
				}
				return false;
			}
			m_outfilePath = arg->m_params[ 0 ].c_str();
			handled = true;
			break;
            
        case TOOL_ARG_ID_PLATFORM: {
            if ( arg->m_params.size() != 1 ) {
                if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_OUTFILE ) == false ) {
                    xerror( arg->m_params.size() != 1, "Invalid number of params for arg 'platform'\n" );
                }
                return false;
            }
            
            PLATFORM platId = PlatformStringToId( arg->m_params[0].c_str() );
            xerror( platId == PLATFORM_NONE, "Unknown target platform '%s'\n", arg->m_params[0].c_str() );
            
            xprintf("Setting target platform to '%s'\n", arg->m_params[0].c_str() );
            targetPlatform = platId;
                  
            handled = true;
            break;
        }
            
        case TOOL_ARG_ID_PLATFORM_BEGIN: {
            if ( arg->m_params.size() != 1 ) {
                if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_PLATFORM_BEGIN ) == false ) {
                    xerror( arg->m_params.size() != 1, "Invalid number of params for arg 'platform_begin'\n" );
                }
                return false;
            }
            
            xerror( currPlatform != PLATFORM_NONE, "Nested 'platform_begin' is not allowed!\n" );
            
            PLATFORM platId = PlatformStringToId( arg->m_params[0].c_str() );
            xerror( platId == PLATFORM_NONE, "Unknown platform '%s'\n", arg->m_params[0].c_str() );
            
        
            currPlatform = platId;
                 
            handled = true;
            break;
        }
            
        case TOOL_ARG_ID_PLATFORM_END: {
            if ( arg->m_params.size() != 0 ) {
                if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_PLATFORM_END ) == false ) {
                    xerror( arg->m_params.size() != 0, "Invalid number of params for arg 'platform_end'\n" );
                }
                break;
            }
            
            xerror( currPlatform == PLATFORM_NONE, "Mismtached platform_begin/platform_end\n" );
            currPlatform = PLATFORM_NONE;
            
            handled = true;
            break;
        }
            
        case TOOL_ARG_ID_DEPENDS: {
            if ( arg->m_params.size() != 0 ) {
                if ( DisplayBuiltinArgHelp( TOOL_ARG_ID_PLATFORM_END ) == false ) {
                    xerror( arg->m_params.size() != 0, "Invalid number of params for arg 'depends'\n" );
                }
                return false;
            }
            
            outputDepends = true;
            handled = true;
            break;
        }
	}

    // If we handled this argument, just bail out now
	if ( handled == true ) {
		return true;
	}
    
    // Determine if this argument shoukd be handled by the tool or not. Filter out
    // args based on the current and target platforms
    bool processThisArg = true;
    if ( ( currPlatform != PLATFORM_NONE ) && ( currPlatform != targetPlatform ) ) {
        processThisArg = false;
    }
    
    if ( processThisArg == true ) {
        return HandleArg( arg, argId );
    }
    
    return true;
}

//=======================================================================================================================
bool ToolApp::HandleArg( Arg * arg, int32_t argId ) {
	return true;
}

//=======================================================================================================================
bool ToolApp::Process() {
	return false;
}

//=======================================================================================================================
void ToolApp::BuildArgs( int argc, const char ** argv ) {

	Arg * currArg = nullptr;

	for ( int a = 0; a < argc; ++a ) {

		const char * currArgv = argv[ a ];

		if ( currArgv[ 0 ] == '+' ) {
			// An argument - add a new argument 
			currArg = new Arg;
			currArg->m_name = currArgv + 1;

			m_args.push_back( currArg );
		}
		else if ( currArg != nullptr ) {
			// A possible argument parameter - add a parameter to the current argument
			std::string tmp = currArgv;

			bool hasStartQuote = ( tmp.front() == '\'' && tmp.back() == '\"' );
			
			if ( hasStartQuote == true ) {
				// Parameter has a starting quote, so we remove these from the  argument
				bool hasEndQuote = ( tmp.front() == '\'' && tmp.back() == '\"' );
				size_t subStrSize = tmp.size() - ( ( hasEndQuote == true ) ? 2 : 1 );

				tmp = tmp.substr( 1, subStrSize );
				currArg->m_params.push_back( tmp );
			}
			else {
				// A pain ol' paramater
				currArg->m_params.push_back( tmp );
			}			
		}
	}
}

//=======================================================================================================================
void ToolApp::PublishArgId( int32_t argId, const char * argName ) {
	uint64_t argNameHash = FH64_CalcFromCStr( argName );
	m_argIdMap[ argNameHash ] = argId;
}

//=======================================================================================================================
int32_t ToolApp::FindArgId( const char * argName ) {
	uint64_t argNameHash = FH64_CalcFromCStr( argName );

	arg_id_map_t::iterator findIt = m_argIdMap.find( argNameHash );
	if ( findIt == m_argIdMap.end() ) {
		return -1;
	}

	return findIt->second;
}

//======================================================================================================================
bool ToolApp::DisplayBuiltinArgHelp( TOOL_ARG_ID argId ) {
	return false;
}

//======================================================================================================================
bool ToolApp::CreateFolderAtPath( const char * path ) {
    
    str_t actualPath = nullptr;
    Str_CopyCStr( &actualPath, path );
    Str_PathRemoveLastElement( &actualPath);
    
    bool_t createOk = FS_CreateFolder( actualPath );
    return createOk == bool_true;
}

//======================================================================================================================
void ToolApp::LoadParamFile( const char * path ) {
    std::string contents;
    
	/* Open and read the file */
    {
        xeScopedFile file( path, "rb" );
        xerror( file.IsValid() == false, "Unable to open tool params file at '%s'\n", path );
        
        size_t length = file.Length();
        
        contents.resize( length );
        
        size_t amtRead = file.Read<char>( &contents[ 0 ], length );
        xerror( amtRead != length, "Error reading params file at '%s'\n", path );
    }

	ParseParams( contents.c_str() );
}


//======================================================================================================================
void ToolApp::ParseParams( const char * buffer ) {

	const char* curr = buffer;
	Arg * currArg = nullptr;
	Lexer::RESULT lexRes;

	int line = 1;
    uint32_t currArgInsert = currArgIndex + 1;

	do {
		if ( *curr == 0 ) {
			break;
		}
		else if ( *curr == '\n' ) {
			// Deal with lines
			++line;
			++curr;
		}
		else if ( *curr == '#' ) {
			// Comment - eat up until new line
		}
		else {
			// Eat any white space chars
			lexRes = Lexer::EatWhiteSpace( &curr );
			if ( lexRes == Lexer::RESULT_EOF ) {
				return;
			}

			if ( *curr == '+' ) {
				// Argument
				++curr;
				const char* argEndPtr;
				const char* argStartPtr = curr;

				Lexer::GetEndOfIdentitifer( &argEndPtr, curr );

				if ( argEndPtr != argStartPtr ) {			
					size_t length = (size_t) (argEndPtr - argStartPtr);
					std::string argName( argStartPtr, length );

					currArg = new Arg;
					currArg->m_name = argName;

					//m_args.push_back( currArg );
                    m_args.insert( m_args.begin() + currArgInsert, currArg );
                    ++currArgInsert;

					// Advance curr ptr past the argument name
					curr = argEndPtr;
				}
			}
			else if ( *curr == '\"'  || *curr == '\'' ) {
				// quoted string paramater
				const char* stringStart = curr;
				const char* stringEnd = curr;

				Lexer::RESULT res = Lexer::GetEndOfQuotedString( &stringEnd, stringStart );
				xerror( res != Lexer::RESULT_OK, "Unterminated or newline in string literal at line %u\n", line );

				if ( currArg != nullptr ) {
					// Get rid of the starting quote character
					++stringStart;

					// Add the string to the arg
					size_t stringLen = ( size_t ) (stringEnd - stringStart);
					std::string str( stringStart, stringLen );

					currArg->m_params.push_back( str );
				}

				// move past the end quote char for the next iteration of the loop
				curr = stringEnd + 1;
			}
			else {
				static const char * eoArgChars = " \r\n\v\t+\'\"";
				const char* endParam = nullptr;
				Lexer::RESULT res = Lexer::FindNextCharOf( &endParam, curr, eoArgChars );

				if ( endParam != nullptr ) {
					size_t len = ( size_t ) (endParam - curr);
					if ( len > 0 ) {
						std::string param( curr, len );
						currArg->m_params.push_back( param );
					}
				}

				curr = endParam;
			}
		}

	} while ( true );
}

//======================================================================================================================
ToolApp::PLATFORM ToolApp::PlatformStringToId( const char * str ) const {
    for ( int i = 0; PLATFORM_STRINGS[ i ] != nullptr; ++i ) {
        if (strcasecmp( PLATFORM_STRINGS[i], str ) == 0 ) {
            return (PLATFORM(i));
        }
    }
    
    return PLATFORM_NONE;
}


//==========================================================================================================================================
const std::string ToolApp::VFormat( const char * fmt, ... ) {
    /* initialize use of the variable argument array */
    va_list vaArgs;
    va_list vaArgsCopy;
    
    va_start( vaArgs, fmt );

    /* reliably acquire the size
       from a copy of the variable argument array
       and a functionally reliable call to mock the formatting */
    va_copy( vaArgsCopy, vaArgs );
    size_t len = vsnprintf( NULL, 0, fmt, vaArgsCopy );
    va_end( vaArgsCopy );
    
    std::string str;
    str.resize( len );
   
    /* return a formatted string without risking memory mismanagement
       and without assuming any compiler or platform specific behavior */
    vsnprintf( &str[0], len + 1, fmt, vaArgs );
    va_end( vaArgs );
    
    return str;
}
