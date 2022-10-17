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

#include "util/ParseLiteral.h"
#include "core/Sys.h"
#include "core/Str.h"
#include <stdio.h>
#include <stdlib.h>

/*=======================================================================================================================================*/
void ParseLiteral_SetString( parse_literal_t * self_, const char * text ) {
    self_->type = PARSE_LITERAL_STRING;
    self_->value.stringVal = NULL;
    
    if ( text[0] == '\"' || text[0] == '\'' ) {
        Str_CopyCStr( &self_->value.stringVal, text+1 );
        Str_SetLength( &self_->value.stringVal, Str_GetLength( self_->value.stringVal ) - 1 );
    }
    else {
        Str_CopyCStr( &self_->value.stringVal, text );
    }
    
}

/*=======================================================================================================================================*/
void ParseLiteral_SetInt( parse_literal_t * self_, const char * text ) {
    self_->type = PARSE_LITERAL_INT;
    self_->value.intVal = atoi( text );
}

/*=======================================================================================================================================*/
void ParseLiteral_SetFloat( parse_literal_t * self_, const char * text ) {
    self_->type = PARSE_LITERAL_FLOAT;
    self_->value.intVal = (float) atof( text );
}

/*=======================================================================================================================================*/
void ParseLiteral_SetVec2( parse_literal_t * self_, parse_literal_t * x, parse_literal_t * y ) {
    self_->type = PARSE_LITERAL_VEC2;

    self_->value.vecVal[0] = x->value.floatVal;
    self_->value.vecVal[1] = y->value.floatVal;
    self_->value.vecVal[2] = 0;
    self_->value.vecVal[3] = 0;
}

/*=======================================================================================================================================*/
void ParseLiteral_SetVec3( parse_literal_t * self_, parse_literal_t * x, parse_literal_t * y, parse_literal_t * z ) {
    self_->type = PARSE_LITERAL_VEC3;

    self_->value.vecVal[0] = x->value.floatVal;
    self_->value.vecVal[1] = y->value.floatVal;
    self_->value.vecVal[2] = z->value.floatVal;
    self_->value.vecVal[3] = 0;
}

/*=======================================================================================================================================*/
void ParseLiteral_SetVec4( parse_literal_t * self_, parse_literal_t * x, parse_literal_t * y, parse_literal_t * z, parse_literal_t * w ) {
    self_->type = PARSE_LITERAL_VEC4;

    self_->value.vecVal[0] = x->value.floatVal;
    self_->value.vecVal[1] = y->value.floatVal;
    self_->value.vecVal[2] = z->value.floatVal;
    self_->value.vecVal[3] = w->value.floatVal;
}

/*=======================================================================================================================================*/
void ParseLiteral_Clear( parse_literal_t * self_ ) {
    if ( self_->type == PARSE_LITERAL_STRING ) {
        Str_Destroy( &self_->value.stringVal );
    }
}
