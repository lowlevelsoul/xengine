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

#ifndef __LEXER_H__
#define __LEXER_H__

#include "core/Platform.h"
#include "core/Fs.h"

typedef struct lexer_token_s {
    const char *        text;
    size_t              length;
} lexer_token_t;

typedef enum lexer_result_e {
    LEXER_RESULT_OK = 0,
    LEXER_RESULT_ERROR,
    LEXER_RESULT_EOF
} lexer_result_t;

XE_API void Lexer_Initialise(void);
XE_API void Lexer_Finalise(void);
XE_API void Lexer_Begin( file_t * file );
XE_API void Lexer_End(void);
XE_API lexer_result_t Lexer_GetNextToken( lexer_token_t * tokenOut );

#endif
