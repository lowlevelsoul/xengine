%option yylineno
%option never-interactive
%option nounistd

DIGIT       [0-9]
LETTER      [_a-zA-Z]
HEX         [a-fA-F0-9]
BINARY      [0-1]
SCI         [Ee][+-]?{DIGIT}+

%{

/*
  ======================================================================================================================
  CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
  Copyright (C) 2021 James Steele. All Rights Reserved.
  ======================================================================================================================
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "MatBuilder.h"
#include "MaterialParser.hpp"

#define register

extern MatBuilder * parseMatBuilder;

%}

%% 

"/*" {
        int c;

        while((c = yyinput()) != 0 && c != EOF) {
            if(c == '*') {
                if((c = yyinput()) == '/')
                    break;
                else
                    unput(c);
            }
        }
    }

"//"[^\n]*                          { /* Eat up comments */ }
"material"                          { return KW_MATERIAL; }
"albedo"                            { return KW_ALBEDO; }
"amr"                               { return KW_AMR; }
"glow"                              { return KW_GLOW; }
"transparency"                      { return KW_TRANSPARENCY; }
"<"                                 { return '<'; }
">"                                 { return '>'; }
"{"                                 { return '{'; }
"}"                                 { return '}'; }
":"                                 { return ':'; }
"="                                 { return '='; }
"["                                 { return '['; }
"]"                                 { return ']'; }
","                                 { return ','; }
"("                                 { return '('; }
")"                                 { return ')'; }


{LETTER}({LETTER}|{DIGIT})*         { return IDENTIFIER; }
[-]?{DIGIT}+                        { return LITERAL_INT; }
\"(\\.|[^\\"])*\"                   { return LITERAL_STRING; }
`([^`]|`{2})*`                      { return LITERAL_STRING; }
[-]?{DIGIT}*"."{DIGIT}+	            { return LITERAL_FLOAT; }
[-]?{DIGIT}+"."{DIGIT}*	            { return LITERAL_FLOAT; }

[ \t\v\r\f]                         { }
.                                   { return TOKEN_ERROR; }
\n                                  { parseMatBuilder->IncLine(); }

%%

//===============================================================================================================================
int yywrap() {
    return 1;
}

#if 0
//===============================================================================================================================
void * yyalloc (size_t bytes) {
    return engine.memory->Malloc(bytes);
}

//===============================================================================================================================
void*  yyrealloc (void * ptr, size_t bytes) {
    return engine.memory->Realloc(ptr, bytes);
}

//===============================================================================================================================
void yyfree (void * ptr) {
    engine.memory->Free(ptr);
}

//===============================================================================================================================
void * yyalloc (size_t bytes, void * yyscanner) {
    return ti::memory->Malloc(bytes, ti::HEAP_PARSE);
}

//===============================================================================================================================
void * yyrealloc (void * ptr, size_t bytes, void * yyscanner) {
    return ti::memory->Realloc(ptr, bytes, ti::HEAP_PARSE);
}

//===============================================================================================================================
void yyfree (void * ptr, void * yyscanner) {
    ti::memory->Free(ptr);
}
#endif