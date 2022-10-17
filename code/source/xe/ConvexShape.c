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

#include "ConvexShape.h"
#include "core/Sys.h"
#include "mem/Mem.h"

static const vec3_t BOX_VERTEX_POS[] = {
    { -1,  1,  1 },
    {  1,  1,  1 },
    {  1, -1,  1 },
    { -1, -1,  1 }
};

/*=======================================================================================================================================*/
void ConvexShape_CreateBox( convex_shape_t * self_, size_t size ) {
    float halfSize = size / 2.f;
    
    self_->verts = Mem_Alloc( sizeof( vec3_t ) * 8 );
    self_->planes = Mem_Alloc( sizeof( plane_t )  * 8 );
    self_->numVerts = 8;
    self_->numPlanes = 6;
    
    for ( int n = 0; n < 6; ++n ) {
        Vec3_Muls( self_->verts[ n ], BOX_VERTEX_POS[ n ], size / 2.0f );
    }
    
    Plane_SetXYZD( &self_->planes[0], 1, 0, 0, halfSize );
    Plane_SetXYZD( &self_->planes[1], -1, 0, 0, halfSize );
    Plane_SetXYZD( &self_->planes[2], 0, 1, 0, halfSize );
    Plane_SetXYZD( &self_->planes[3], 0, -1, 0, halfSize );
    Plane_SetXYZD( &self_->planes[4], 0, 0, 1, halfSize );
    Plane_SetXYZD( &self_->planes[5], 0, 0, -1, halfSize );
}

/*=======================================================================================================================================*/
void ConvexShape_Destroy( convex_shape_t * self_ ) {
    Mem_Free( self_->verts );
    Mem_Free( self_->planes );
}
