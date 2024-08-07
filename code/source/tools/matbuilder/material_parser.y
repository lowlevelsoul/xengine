%{
#define YYPARSER
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "core/Sys.h"
#include "core/Fs.h"
#include "mem/Mem.h"
#include "util/ParseLiteral.h"
#include "matbuilder/MatBuilder.h"

#undef yyparse
#undef yylex
#undef yyerror
#undef yylval
#undef yychar
#undef yydebug
#undef yynerrs
#undef yytext
#define yyparse matparse_parse
#define yylex   matlex_lex
#define yyerror matparse_error
#define yylval  matparse_lval
#define yychar  matparse_char
#define yydebug matparse_debug
#define yynerrs matparse_nerrs
#define yytext matlex_text
#define YYSTYPE parse_literal_t

typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE matlex__scan_buffer ( char *base, size_t size  );
void matlex__delete_buffer ( YY_BUFFER_STATE b  );
#define yytext matlex_text
extern char *yytext;

//extern int gmerror(const char * a_message);
extern int matlex_lex(void);
void matparse_error( const char* msg );
MatBuilder * parseMatBuilder;

#define GM_BISON_DEBUG
#ifdef GM_BISON_DEBUG
#define YYDEBUG 1
#define YYERROR_VERBOSE
#endif // GM_BISON_DEBUG


%}

%token KW_MATERIAL
%token KW_ALBEDO
%token KW_AMR
%token KW_GLOW
%token KW_END
%token KW_TRANSPARENCY
%token LITERAL_STRING
%token LITERAL_INT
%token LITERAL_FLOAT
%token IDENTIFIER
%token TOKEN_ERROR

%start material_list
%%

material_list
    : material
    | material_list material;


material: KW_MATERIAL lit_string '{' { parseMatBuilder->BeginMaterial( &$2 ); }
    mat_contents 
    '}' { parseMatBuilder->EndMaterial(); }
    ;

mat_contents: | mat_item_list;

mat_item_list:
    mat_item
    | mat_item_list mat_item
    ;

mat_item:
    tex_albedo 
    | tex_amr 
    | tex_glow
    ;

tex_albedo:
    KW_ALBEDO '=' lit_string {
        parseMatBuilder->SetTexture( MatBuilder::MaterialInfo::STAGE_ALBEDO, &$3 );
    }
    ;

tex_amr:
    KW_AMR '=' lit_string {
        parseMatBuilder->SetTexture( MatBuilder::MaterialInfo::STAGE_AMR, &$3 );
    }
    ;   

tex_glow:
    KW_GLOW '=' lit_string {
        parseMatBuilder->SetTexture( MatBuilder::MaterialInfo::STAGE_GLOW, &$3 );
    }
    ; 

lit_string: LITERAL_STRING
    {
        ParseLiteral_SetString( &$$, yytext );
    }
    ;
%%

/*====================================================================================================================*/
bool MaterialScript_Parse( file_t * file, MatBuilder * matBuilder_ ) {

    parseMatBuilder = matBuilder_;

    /* Get the length of the file and allocate a buffer with space for two 
     null terminators at the end. This is a requirement for certain 
     methods for scanning the string with flex. */
    size_t fileLen = FS_FileLength( file );
    char* buffer = (char *) Mem_Alloc( fileLen+2 );
    memset(buffer, 0, fileLen+2);

    /* Read in the text file */
    size_t amtRead = FS_FileRead( file, buffer, sizeof(char), fileLen) ;
    xassert(amtRead == fileLen);

    YY_BUFFER_STATE yyBuffer = matlex__scan_buffer(buffer, fileLen+2);

    int32_t token;
    do {
        token = yyparse();
    } while(token != 0 && parseMatBuilder->HasErrors() == false);

    matlex__delete_buffer(yyBuffer);
    Mem_Free( buffer );

    parseMatBuilder = nullptr;

    return ( parseMatBuilder->HasErrors() == false ) ? true : false;
}

/*====================================================================================================================*/
void matparse_error( const char* msg ) {
    parseMatBuilder->LogError( msg );
}