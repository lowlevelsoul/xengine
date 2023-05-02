//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#ifndef __LEXER_H__
#define __LEXER_H__

class Lexer {
public:
	enum RESULT {
		RESULT_OK, 
		RESULT_EOF, 
		RESULT_UNEXPECTED_EOF,
		RESULT_UNEXPECTED_EOL
	};

	static bool IsCharOneOf( char value, const char* charList );

	static bool IsCharWhiteSpace( char value );

	static RESULT FindNextCharOf( const char** nextOf, const char* string, const char* match );

	static RESULT GetEndOfQuotedString( const char ** endOfStr, const char* string );

	static RESULT EatWhiteSpace( const char ** ptr );

	static RESULT GetEndOfIdentitifer( const char **endPtr, const char * string );
};



#endif
