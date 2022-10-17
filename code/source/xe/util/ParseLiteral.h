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

#ifndef __PARSELITERAL_H__
#define __PARSELITERAL_H__

#include "core/Sys.h"
#include "core/Str.h"

typedef enum parse_literal_type_e {
    PARSE_LITERAL_STRING,
    PARSE_LITERAL_FLOAT,
    PARSE_LITERAL_INT,
    PARSE_LITERAL_VEC2,
    PARSE_LITERAL_VEC3,
    PARSE_LITERAL_VEC4
} parse_literal_type_t;

typedef struct parse_literal_s {
    parse_literal_type_t        type;
    union {
        int32_t     intVal;
        float       floatVal;
        str_t       stringVal;
        float       vecVal[4];
    } value;
} parse_literal_t;

XE_API void ParseLiteral_SetString( parse_literal_t * self_, const char * text );
XE_API void ParseLiteral_SetInt( parse_literal_t * self_, const char * text );
XE_API void ParseLiteral_SetFloat( parse_literal_t * self_, const char * text );
XE_API void ParseLiteral_SetVec2( parse_literal_t * self_, parse_literal_t * x, parse_literal_t * y );
XE_API void ParseLiteral_SetVec3( parse_literal_t * self_, parse_literal_t * x, parse_literal_t * y, parse_literal_t * z );
XE_API void ParseLiteral_SetVec4( parse_literal_t * self_, parse_literal_t * x, parse_literal_t * y, parse_literal_t * z, parse_literal_t * w );
XE_API void ParseLiteral_Clear( parse_literal_t * self_ );

#endif
