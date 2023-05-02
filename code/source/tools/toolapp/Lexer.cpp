//==========================================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//==========================================================================================================================================

#include "toolapp/Lexer.h"

static const char* WHITESPACE_CHARS = " \v\t\f\r\n";

//==========================================================================================================================================
bool Lexer::IsCharOneOf( char value, const char* charList ) {
	const char* res = strchr( charList, value );
	return ( res != nullptr );
}

//==========================================================================================================================================
bool Lexer::IsCharWhiteSpace( char value ) {
	return IsCharOneOf( value, WHITESPACE_CHARS );
}

//==========================================================================================================================================
Lexer::RESULT Lexer::FindNextCharOf( const char** nextOf, const char* string, const char* match ) {

	const char* curr = string;

	do {
		if ( IsCharOneOf( *curr, match ) == true ) {
			*nextOf = curr;
			return Lexer::RESULT::RESULT_OK;
		}
		else if ( *curr == 0 ) {
			break;
		}
		else {
			++curr;
		}

	} while ( true );

	*nextOf = nullptr;
	return RESULT_EOF;
}

//==========================================================================================================================================
Lexer::RESULT Lexer::GetEndOfQuotedString( const char ** endOfStr, const char* string ) {
	const char* curr = string;
	char quoteChar = *curr;
	++curr;

	bool hitEscapeChar = false;

	do {
		if ( hitEscapeChar == true ) {
			// Hit an escape character. Check we're not at the end of the file and skip 
			if ( *curr == 0 ) {
				*endOfStr = nullptr;
				return RESULT_UNEXPECTED_EOF;
			}
			++curr;
			hitEscapeChar = false;
		}
		else if ( *curr == 0 ) {
			// End of string. 
			return RESULT_UNEXPECTED_EOF;
		}
		else if ( *curr == quoteChar ) {
			// Found the matching quote character so bail 
			*endOfStr = curr;
			return RESULT_OK;
		}
		else if ( *curr == '\\' ) {
			// Escape character - flag and handle in the next go around 
			++curr;
			hitEscapeChar = true;
		}
		else if ( *curr == '\n' || *curr == '\r' ) {
			// Hit a new line - which we can't do in a string constant 
			*endOfStr = nullptr;
			return RESULT_UNEXPECTED_EOL;
		}
		else {
			// Next character 
			++curr;
		}

	} while ( true );

	// We should never get to here - but just incase!
	*endOfStr = nullptr;
	return RESULT_UNEXPECTED_EOF;
}

//==========================================================================================================================================
Lexer::RESULT Lexer::EatWhiteSpace( const char ** ptr ) {

	const char * curr = *ptr;
	bool loop = true;

	do {
		if ( *curr == 0 ) {
			*ptr = curr;
			return RESULT_UNEXPECTED_EOF;
		}		
		else if ( IsCharWhiteSpace( *curr ) == true ) {
			++curr;
		}
		else {
			loop = false;
		}
	} while ( loop == true );

	*ptr = curr;
	return RESULT_OK;
}

//==========================================================================================================================================
Lexer::RESULT Lexer::GetEndOfIdentitifer( const char **endPtr, const char * string ) {
	*endPtr = string;

	do {
		char currChar = **endPtr;
		bool isIdentChar = (currChar == '_') || ((currChar >= 'a') && (currChar <= 'z')) || ((currChar >= 'A') && (currChar <= 'Z')) || ((currChar >= '0') && (currChar <= '9'));
		if ( isIdentChar == false ) {
			break;
		}
		else {
			(*endPtr)++;
		}
	} while ( true );

	return Lexer::RESULT_OK;
}
