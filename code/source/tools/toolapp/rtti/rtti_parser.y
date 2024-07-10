%{
#define YYPARSER
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/Sys.h"
#include "core/Fs.h"
#include "rtti/ScriptLoader.h"

#undef yyparse
#undef yylex
#undef yyerror
#undef yylval
#undef yychar
#undef yydebug
#undef yynerrs
#undef yytext
#define yyparse rttiparse_parse
#define yylex   rttilex_lex
#define yyerror rttiparse_error
#define yylval  rttiparse_lval
#define yychar  rttiparse_char
#define yydebug rttiparse_debug
#define yynerrs rttiparse_nerrs
#define yytext rttilex_text
#define YYSTYPE re::rtti::ParseItem

typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE rttilex__scan_buffer ( char *base, size_t size  );
void rttilex__delete_buffer ( YY_BUFFER_STATE b  );
#define yytext rttilex_text
extern char *yytext;

//extern int gmerror(const char * a_message);
extern int rttilex_lex(void);

void rttiparse_error( const char* msg );

#define GM_BISON_DEBUG
#ifdef GM_BISON_DEBUG
#define YYDEBUG 1
#define YYERROR_VERBOSE
#endif // GM_BISON_DEBUG


namespace rttiparse {
    re::rtti::ScriptLoader * loader = nullptr;
}

%}

%token KW_OBJECT
%token KW_INSTANCE
%token KW_TRUE
%token KW_FALSE
%token KW_PLATFORM
%token KW_ANGLE_AXIS
%token LITERAL_INT
%token LITERAL_HEX
%token LITERAL_FLOAT
%token LITERAL_STRING
%token IDENTIFIER
%token TOKEN_ERROR

%start object
%%

object_header_bare: KW_OBJECT lit_string
    { 
        rttiparse::loader->BeginObject( $2 );              
        $2.Clear(); 
    }
    ;

object_header_named: KW_OBJECT lit_string lit_string
    { 
        rttiparse::loader->BeginNamedObject( $2, $3 );
        $2.Clear(); 
        $3.Clear(); 
    }
    ;   

object_header
    : object_header_bare
    | object_header_named
    ;

object: object_header 
    '{'
        object_contents
    '}' 
    { 
        $$.SetObject( rttiparse::loader->GetCurrObject() );   
        rttiparse::loader->EndObject();  
    }
    ;

/* Note - The prefixed or here allows us to have no items */
object_contents: | object_item_list;

assignment_list:
    assignment
    | assignment_list assignment;

object_item_list:
    object_item
    | object_item_list object_item;

object_item: assignment | platform_spec;

platform_spec: KW_PLATFORM lit_string { rttiparse::loader->SetPlatform($2); $2.Clear(); } 
    '{'
        assignment_list 
    '}' { rttiparse::loader->ClearPlatform(); }
    ;

assignment: assign_scalar | assign_scalar_simple | assign_array_simple | assign_scalar_inst;

assign_scalar_inst: ident '=' KW_INSTANCE lit_string {
    rttiparse::loader->AssignProperty( $1, $4, true );
    $1.Clear();
    $4.Clear();
}
;

assign_scalar: ident '=' assignment_rhs {
    rttiparse::loader->AssignProperty( $1, $3);
    $1.Clear();
    $3.Clear();
}
;

assign_scalar_simple: ident assignment_rhs {
    rttiparse::loader->AssignProperty( $1, $2);
    $1.Clear();
    $2.Clear();
}
;

assign_array_simple: 
    ident '[' { rttiparse::loader->BeginProperty($1); $1.Clear(); }  
    array_list 
    ']' { rttiparse::loader->EndProperty(); }
    ;


array_list: array_element
    | array_list ','  array_element
    ;

array_element: array_element_value | array_element_inst;

array_element_inst: KW_INSTANCE lit_string
    {
        rttiparse::loader->SetProperty( $2, true );
        $2.Clear();
    }
    ;

array_element_value: assignment_rhs 
    {
         rttiparse::loader->SetProperty( $1 );
         $1.Clear();
    }
    ;

assignment_rhs: lit_int | lit_float | lit_vector2 | lit_vector3 | lit_vector4 | lit_string | lit_true | lit_false | object | angle_axis;


ident: IDENTIFIER
    {
        $$.SetLiteralString( yytext );        
    }
    ;

angle_axis: KW_ANGLE_AXIS '(' lit_vector3 lit_number ')'
    {
        $$.SetAngleAxis( $3, $4 );
    }
    ;

lit_vector4: '<' lit_number ',' lit_number ',' lit_number ',' lit_number '>' 
    {
        $$.SetLiteralVec4( $2, $4, $6, $8);
        $2.Clear();
        $4.Clear();
        $6.Clear();
        $8.Clear();
    }
    ;      

lit_vector3: '<' lit_number ',' lit_number ',' lit_number '>' 
    {
        $$.SetLiteralVec3( $2, $4, $6 );
        $2.Clear();
        $4.Clear();
        $6.Clear();
    }
    ;    

lit_vector2: '<' lit_number ',' lit_number '>' 
    {
        $$.SetLiteralVec2( $2, $4 );
        $2.Clear();
        $4.Clear();
    }
    ;

lit_number: 
    lit_int { 
        $$.type = re::rtti::ParseItem::TYPE_LITERAL;
        $$.litType = re::rtti::ParseItem::LIT_FLOAT;
        $$.value.litFloat = (float) $1.value.litInt;
        $1.Clear();
    }
    | lit_float {
        $$.type = re::rtti::ParseItem::TYPE_LITERAL;
        $$.litType = re::rtti::ParseItem::LIT_FLOAT;
        $$.value.litFloat = $1.value.litFloat;
        $1.Clear();
    }
    ;

lit_true: KW_TRUE
{
    $$.SetLiteralInt("1");
}
;

lit_false: KW_FALSE
{
    $$.SetLiteralInt("0");
}
;

lit_float: LITERAL_FLOAT
{
    $$.SetLiteralFloat( yytext );
}
;    

lit_int: LITERAL_INT
{
    $$.SetLiteralInt( yytext );
}
;

lit_string: LITERAL_STRING
    {
        $$.SetLiteralString( yytext );
    }
    ;
%%

namespace rttiparse {

    bool ParseRttiFile( re::fs::File * file, re::rtti::ScriptLoader * loader ) {

        rttiparse::loader = loader;

        // Get the length of the file and allocate a buffer with space for two 
        // null terminators at the end. This is a requirement for certain 
        // methods for scanning the string with flex.
        size_t fileLen = file->Length();
        char* buffer = new char[fileLen+2];
        memset(buffer, 0, fileLen+2);

        // Read in the text file
        size_t amtRead = file->Read( buffer, sizeof(char), fileLen );
        re_assert(amtRead == fileLen);

        YY_BUFFER_STATE yyBuffer = rttilex__scan_buffer(buffer, fileLen+2);

        int32_t token;
        do {
            token = yyparse();
        } while(token != 0 && loader->HasError() == false);

        rttilex__delete_buffer(yyBuffer);
        delete[] buffer;

        rttiparse::loader = nullptr;

        return ( loader->HasError() == false );
    }
}

//======================================================================================================================
void rttiparse_error(const char* msg) {
    rttiparse::loader->PrintError( msg );
}