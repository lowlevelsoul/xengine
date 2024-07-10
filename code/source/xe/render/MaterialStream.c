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

#include "MaterialStream.h"

/*=======================================================================================================================================*/
const char * MaterialStream_GetStrings( const material_stream_t * str ) {
    return ( const char * ) ( ( ( uintptr_t ) str ) + str->offsStrings );
}

/*=======================================================================================================================================*/
const material_stream_entry_t * MaterialStream_GetMaterials( const material_stream_t * str ) {
    return ( const material_stream_entry_t * ) ( ( ( uintptr_t ) str ) + str->offsMaterials );
}

/*=======================================================================================================================================*/
const char * MaterialStream_GetAlbedo( const material_stream_t * str ) {
    return NULL; //'XE_CALC_OFFSET_PTR(const char *, str, str->offsAlbedoTexture );
}

/*=======================================================================================================================================*/
const char * MaterialStream_GetAmr( const material_stream_t * str ) {
    return NULL;    //XE_CALC_OFFSET_PTR(const char *, str, str->offsAmrTexture );
}

/*=======================================================================================================================================*/
const char * MaterialStream_GetGlow( const material_stream_t * str ) {
    return NULL;    //XE_CALC_OFFSET_PTR(const char *, str, str->offsGlowTexture );
}
