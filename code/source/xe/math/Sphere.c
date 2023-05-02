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

#include "core/Sys.h"
#include "math/Sphere.h"

/*=======================================================================================================================================*/
void Sphere_Set( sphere_t * self_, const vec3_t * center, float radius ) {
    xassert( self_ != NULL );
    Vec4_SetXyzw( *self_, center->x, center->y, center->z, radius );
}

/*=======================================================================================================================================*/
void Sphere_SetXyzRadius( sphere_t * self_, float x, float y, float z, float radius ) {
    xassert( self_ != NULL );
    Vec4_SetXyzw( *self_, x, y, z, radius );
}

/*=======================================================================================================================================*/
void Sphere_GetCenter( const sphere_t * self_, vec3_t * center ) {
    xassert( self_ != NULL );
    Vec3_SetXyz( *center, self_->x, self_->y, self_->z );
}

/*=======================================================================================================================================*/
float Sphere_GetRadius( const sphere_t * self_ ) {
    return self_->w;
}
