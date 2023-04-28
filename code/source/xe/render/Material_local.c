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

#include "render/Material_local.h"
#include "util/ParseLiteral.h"
#include "render/MaterialStream.h"
#include "core/Id.h"
#include "core/Sys.h"
#include "core/Fs.h"
#include "mem/Mem.h"
#include <assert.h>
#include <string.h>
#include <stdarg.h>

static void         MaterialResource_Load( resource_t * self_, file_t * file, const char * path );
static void *       MaterialResource_Alloc( void );
static void         MaterialResource_Free( void * data );

static void         MaterialResource_LoadParseText( material_t * mat, file_t * file );

static void         MaterialResource_LoadBinary( material_t * mat, file_t * file, const char * path );

DEFINE_RESOURCE_FACTORY( "Material", Material, material )

typedef struct material_parse_s {
    material_t *    mat;
    int             line;
    str_t           parseLog;
    bool_t       hasErrors;
} material_parse_t;

material_parse_t matParse;
bool_t matParseInit = false;

extern bool_t MaterialResource_Parse( file_t * file );

#define MATERIAL_ID MAKE_ID( X, E, M, a, t, e, r, i, a, l, _, _ )

/*=======================================================================================================================================*/
void Material_Create( material_t * self_ ) {
    material_local_t * matLocal = (material_local_t*) self_;
    
    static_assert( sizeof( material_t ) >= sizeof( material_local_t ), "Size of material_t.data is too small for implementation" );
    assert( self_ != NULL );
    assert( matLocal->magic != MATERIAL_ID );
    
    matLocal->magic = MATERIAL_ID;
    matLocal->timeStamp = -1;
    matLocal->batchIndex = -1;
    matLocal->textureAlbedo = NULL;
    matLocal->textureGlow = NULL;
    matLocal->textureAmr = NULL;
}

/*=======================================================================================================================================*/
void Material_Destroy( material_t * self_ ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    memset(matLocal, 0, sizeof( material_local_t) );
}

/*=======================================================================================================================================*/
void Material_SetTextureAlbedo( material_t * self_, texture_t * tex ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    matLocal->textureAlbedo = tex;
}

/*=======================================================================================================================================*/
void Material_SetTextureGlow( material_t * self_, texture_t * tex ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    matLocal->textureGlow = tex;
}

/*=======================================================================================================================================*/
void Material_SetTextureAmr( material_t * self_, texture_t * tex ) {
    material_local_t * matLocal = (material_local_t*) self_;
    assert( self_ != NULL );
    assert( matLocal->magic == MATERIAL_ID );
    
    matLocal->textureAmr = tex;
}



/*=======================================================================================================================================*/
void MaterialResource_StartParsing( material_t * mat ) {
    if ( matParseInit == false ) {
        memset(&matParse, 0, sizeof(matParse) );
        matParseInit = true;
    }

    xassert( matParse.mat == NULL );
    
    matParse.mat = mat;
    matParse.parseLog = NULL;
    matParse.line = 1;
    matParse.hasErrors = false;
}

/*=======================================================================================================================================*/
void MaterialResource_EndParsing(void) {
    xassert( matParse.mat != NULL );
    matParse.mat = NULL;
    
    if ( matParse.parseLog != NULL ) {
        Str_Destroy( &matParse.parseLog );
    }
}

/*=======================================================================================================================================*/
void MaterialResource_SetAlbedoTexture( parse_literal_t * path ) {
    resource_t * texRes = NULL;
    
    xassert( matParse.mat != NULL );
    xassert( path->type == PARSE_LITERAL_STRING );
    
    texRes = Resource_Load( path->value.stringVal );
    Material_SetTextureAlbedo( matParse.mat, (texture_t*) Resource_GetData( texRes ) );
}

/*=======================================================================================================================================*/
void MaterialResource_SetGlowTexture( parse_literal_t * path ) {
    resource_t * texRes = NULL;
    
    xassert( matParse.mat != NULL );
    xassert( path->type == PARSE_LITERAL_STRING );
    
    texRes = Resource_Load( path->value.stringVal );
    Material_SetTextureGlow( matParse.mat, (texture_t*) Resource_GetData( texRes ) );
}

/*=======================================================================================================================================*/
void MaterialResource_Load( resource_t * self_, file_t * file, const char * path ) {
    material_t * mat = (material_t*) Resource_GetData( self_ );
    
    str_t ext = NULL;
    Str_PathGetExtension( &ext, (char*) path );
    Str_ToLower( ext );
    
    if ( strcasecmp( ext, "bmat" ) == 0 ) {
        MaterialResource_LoadBinary( mat, file, path );
    }
    else {
        MaterialResource_LoadParseText( mat, file );
    }
    
    Str_Destroy( &ext );
}

/*=======================================================================================================================================*/
void * MaterialResource_Alloc(void) {
    material_t * mat = (material_t*) Mem_Alloc(sizeof(material_t));
    Material_Create( mat );
    return mat;
}

/*=======================================================================================================================================*/
void  MaterialResource_Free( void * data ) {
    Mem_Free( data );
}

/*=======================================================================================================================================*/
void MaterialResource_LoadParseText( material_t * mat, file_t * file ) {
    MaterialResource_StartParsing( mat );
    
    bool_t parseResult = MaterialResource_Parse( file );
    xerror( parseResult == false, "%s\n", matParse.parseLog );
    
    MaterialResource_EndParsing();
}

/*=======================================================================================================================================*/
void MaterialResource_LogError( const char * fmt, ... ) {
    str_t header = NULL;
    str_t msg = NULL;

    va_list va;
    va_start( va, fmt );
    
    Str_VFormatArgs( &msg, fmt, va );
    Str_VFormat( &header, "Error : %i : ", matParse.line );
    
    Str_Append( &matParse.parseLog, header );
    Str_Append( &matParse.parseLog, msg );
    Str_Destroy( &header );
    Str_Destroy( &msg );
    
    matParse.hasErrors = true;
}

/*=======================================================================================================================================*/
bool_t MaterialResource_HasErrors( void ) {
    return matParse.hasErrors;
}

/*=======================================================================================================================================*/
void MaterialResource_IncLine( void ) {
    ++matParse.line;
}

/*=======================================================================================================================================*/
void MaterialResource_LoadBinary( material_t * mat, file_t * file, const char * path ) {
    /* Read the data fom the file */
    size_t dataLength = FS_FileLength( file );
    void * data = Mem_Alloc( dataLength );
    assert( data != NULL );
    
    size_t amtRead = FS_FileRead( file, data, sizeof(uint8_t) , dataLength );
    assert( amtRead == dataLength );
    
    material_stream_t * str = ( material_stream_t * ) data;
    
    resource_t * texRes = NULL;

    const char * albedoPath = ( str->offsAlbedoTexture == 0 ) ? NULL : MaterialStream_GetAlbedo( str );
    const char * glowPath = ( str->offsGlowTexture == 0 ) ? NULL : MaterialStream_GetGlow( str );
    const char * amrPath = ( str->offsAmrTexture == 0 ) ? NULL : MaterialStream_GetAmr( str );
    
    if ( albedoPath != NULL ) {
        texRes = Resource_Load( albedoPath );
        Material_SetTextureAlbedo( matParse.mat, (texture_t*) Resource_GetData( texRes ) );
    }
    
    if ( amrPath != NULL ) {
        texRes = Resource_Load( amrPath );
        Material_SetTextureAmr( matParse.mat, (texture_t*) Resource_GetData( texRes ) );
    }
    
    if ( amrPath != NULL ) {
        texRes = Resource_Load( amrPath );
        Material_SetTextureAmr( matParse.mat, (texture_t*) Resource_GetData( texRes ) );
    }
    
}
