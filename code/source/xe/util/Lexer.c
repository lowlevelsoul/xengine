/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#include "core/Sys.h"
#include "core/Str.h"
#include "util/Lexer.h"
#include <string.h>
#include <stdarg.h>

#define MAX_NUM_LEXER_STATE 8

typedef enum log_type_s {
    LOG_TYPE_WARN = 0,
    LOG_TYPE_ERROR,
    LOG_TYPE_INFO,
} log_type_t;

typedef struct lexer_state_s {
    file_t *        file;
    str_t           text;
    size_t          pos;
    size_t          length;
    int32_t         line;
} lexer_state_t;

typedef struct lexer_s {
    lexer_state_t       states[ MAX_NUM_LEXER_STATE ];
    size_t              stateTos;
    lexer_state_t *     curr;
    str_t               log;
} lexer_t;

lexer_t lexer;
bool_t lexerInit = false;

static const char * WHITESPACE_CHARS = " \r\t\n\v\f";
static const char * PUNC_CHARS = ";<>()[],.{}=";

#define LEXER_TEXT lexer.curr->text[ lexer.curr->pos ]
#define LEXER_POS lexer.curr->pos
#define LEXER_LENGTH lexer.curr->length

/*=======================================================================================================================================*/
void Lexer_Initialise(void) {
    if ( lexerInit == true ) {
        return;
    }
    
    memset( &lexer, 0, sizeof(lexer) );
    Str_SetCapacity( &lexer.log, 2048 );
    lexerInit = true;
}

/*=======================================================================================================================================*/
void Lexer_Finalise(void) {
    if ( lexerInit == false ) {
        return;
    }
    
    lexerInit = false;
}

/*=======================================================================================================================================*/
void Lexer_Begin( file_t * file ) {
    size_t amtRead;
    
    xassert( lexerInit == true );
    xerror( lexer.stateTos >= MAX_NUM_LEXER_STATE, "Lexer state stack overflow" );

    /* Allocate a lexer state from the stack */
    lexer.curr = &lexer.states[ lexer.stateTos ];
    ++lexer.stateTos;
    memset( lexer.curr, 0, sizeof( lexer_state_t) );

    lexer.curr->file = file;
    
    /* Load the text into a string */
    Str_SetLength( & lexer.curr->text, FS_FileLength( file ) );
    lexer.curr->length = Str_GetLength( lexer.curr->text );
    
    amtRead = FS_FileRead( file, lexer.curr->text, 1, lexer.curr->length );
    
    lexer.curr->pos = 0;
    lexer.curr->line = 1;
}

/*=======================================================================================================================================*/
void Lexer_End(void) {
    /* Clean up the current lexer state */
    Str_Destroy( &lexer.curr->text );
    --lexer.stateTos;
    
    /* Get the top most lexer on the stack if one exists */
    if ( lexer.stateTos >= 0 ) {
        lexer.curr = &lexer.states[ lexer.stateTos ];
    }
    else {
        lexer.curr = NULL;
    }
}

/*=======================================================================================================================================*/
void Lexer_LogPrint( log_type_t logType, const char * fmt, ... ) {
    str_t header = NULL;
    str_t msg = NULL;
    const char * headerText = "Line %i : \n";
    
    switch ( logType ) {
        case LOG_TYPE_WARN:
            headerText = "WARNING : Line %i : \n";
            break;
            
        case LOG_TYPE_ERROR:
            headerText = "ERROR : Line %i : \n";
            break;
            
        case LOG_TYPE_INFO:
            headerText = "Line %i : \n";
            break;
    }
    
    Str_VFormat( &header, headerText, lexer.curr->line );
    
    va_list va;
    va_start( va, fmt );
    Str_VFormatArgs( &msg, fmt, va );
    
    Str_Append(&lexer.log, header );
    Str_Append(&lexer.log, msg );
    Str_Destroy( &header );
    Str_Destroy( &msg );
}

/*=======================================================================================================================================*/
lexer_result_t Lexer_EatWhiteSpace(void) {
    char currChar = 0;
    bool_t loop = true;
    
    do {
        if ( lexer.curr->pos >= lexer.curr->length ) {
            loop = false;
        }
        else {
            bool_t foundChar = false;
            currChar = lexer.states->text[ lexer.states->pos ];
            
            for ( const char * testChar = WHITESPACE_CHARS; *testChar != 0; ++testChar ) {
                if ( currChar != *testChar ) {
                    foundChar = true;
                }
            }
            
            if ( foundChar == false ) {
                /* The current character wasn't in the whitespace list, so return */
                return LEXER_RESULT_OK;
            }
            
            if ( currChar == '\n') {
                ++lexer.states->line;
            }
            
            /* Next char */
            ++lexer.states->pos;
        }
        
    } while ( loop == true );
    
    return LEXER_RESULT_EOF;
}

/*=======================================================================================================================================*/
lexer_result_t Lexer_GetString( lexer_token_t * tokenOut ) {
    char currChar = LEXER_TEXT;
    uintptr_t endPos = LEXER_POS;
    uintptr_t startPos = endPos;
    bool_t loop = true;

    /* If we encounter the opening quote, move past it */
    if ( currChar == '\'' || currChar == '\"' ) {
        ++LEXER_POS;
    }
    
    startPos = LEXER_POS;
    
    do {
        /* If we've hit the end of the text, then we bail with an error */
        if ( LEXER_POS >= LEXER_LENGTH ) {
            Lexer_LogPrint( LOG_TYPE_ERROR, "Encountered unterminated string \n " );
            return LEXER_RESULT_ERROR;
        }
        
        /* Get the current char */
        currChar = LEXER_TEXT;
        
        if (currChar == '\n') {
            /* Found a newline character in the string which is bad */
            Lexer_LogPrint( LOG_TYPE_ERROR, "Newline in string \n " );
            return LEXER_RESULT_ERROR;
        }
        
        if ( currChar == '\'' || currChar == '\"' ) {
            /* We've encountered the closing quote - get the token info and exit with success */
            tokenOut->text = lexer.curr->text + startPos;
            tokenOut->length = endPos + startPos;
            loop = false;
            
            /* Move past closing quote char */
            ++LEXER_POS;
        }
        else {
            /* Next char */
            ++LEXER_POS;
        }
        
    } while ( loop == true );
    
    return LEXER_RESULT_OK;
}

/*=======================================================================================================================================*/
lexer_result_t Lexer_EatCommentSingle(void) {
    /* Move past // */
    LEXER_POS += 2;
    
    do {
        if ( LEXER_POS >= LEXER_LENGTH ) {
            /* We'ce hit the end of the file */
            return LEXER_RESULT_EOF;
        }
        
        if ( LEXER_TEXT == '\n') {
            return LEXER_RESULT_OK;
        }
        else {
            ++LEXER_POS;
        }
                
    } while ( true );
    
    return LEXER_RESULT_OK;
}

/*=======================================================================================================================================*/
bool_t Lexer_IsPunctuation(void) {
    for ( const char * p = PUNC_CHARS; *p != 0; ++ p ) {
        if ( *p == LEXER_TEXT ) {
            return true;
        }
    }
    
    return false;
}

/*=======================================================================================================================================*/
bool_t Lexer_IsSingleLineComment(void) {
    if ( LEXER_TEXT == '/' ) {
        if ( lexer.curr->text[ LEXER_LENGTH + 1 ]  == '/' ) {
            return true;
        }
    }
    
    return false;
}

/*=======================================================================================================================================*/
lexer_result_t Lexer_GetNextToken( lexer_token_t * tokenOut ) {
    lexer_result_t res;
    bool_t loop = false;
    
    do {
        if ( lexer.curr->pos >= lexer.curr->length ) {
            /* Early out if we're at EOF */
            return LEXER_RESULT_EOF;
        }
        
        res = Lexer_EatWhiteSpace();
        if ( res != LEXER_RESULT_OK ) {
            return res;
        }
        
        if ( LEXER_TEXT == '\'' || LEXER_TEXT == '\"' ) {
            /* encountered a String */
            res = Lexer_GetString( tokenOut );
            if ( res != LEXER_RESULT_OK ) {
                return res;
            }
            
            loop = false;
        }
        else if ( Lexer_IsSingleLineComment() == true ) {
            res = Lexer_EatCommentSingle();
            if ( res != LEXER_RESULT_OK ) {
                return res;
            }
        }
        else if ( Lexer_IsPunctuation() == true ) {
            /* A punctuation character */
            tokenOut->text = lexer.curr->text;
            tokenOut->length = 1;
            ++LEXER_POS;
            loop = false;
        }
        else {
            /* Some other type of token */
            if ( LEXER_TEXT == '-' || ( LEXER_TEXT >= '0' && LEXER_TEXT <= '9' ) ) {
                /* A number */
            }
            else if ( LEXER_TEXT == '_' || ( LEXER_TEXT >= 'a' && LEXER_TEXT <= 'z') || ( LEXER_TEXT >= 'A' && LEXER_TEXT <= 'Z') ) {
                /* A keyword or identifier */
                
            }
        }
        
    } while ( loop == true );
    
    return LEXER_RESULT_OK;
}
