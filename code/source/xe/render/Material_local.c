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
