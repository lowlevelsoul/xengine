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

#include "math/Math3d.h"

/*=======================================================================================================================================*/
void _Vec3_MixCosine( vec3_t * dst, const vec3_t * from, const vec3_t * to, float t ) {
    float cosF = 0;
    
    t = scalar_Clamp( t, 0, 1 );
    cosF = (1.0f - scalar_Cos(t * PI) ) / 2.0f;
    
    Vec3_Mix( *dst, *from, *to, t );
}

/*=======================================================================================================================================*/
float _Vec3_Normalise(vec3_t* dst, const vec3_t* src) {
    float mag = Vec3_Magnitude( *src );
    float fac = 1.0f / mag;
    xassert( scalar_IsValid( fac ) == ztrue );
    
    Vec3_Muls( *dst, *src, fac );
    return mag;
}

/*=======================================================================================================================================*/
void _Vec3_Mix(vec3_t* dst, const vec3_t* a, const vec3_t* b, float t) {
    t = scalar_Clamp( t, 0, 1 );
    dst->x = scalar_Mix( a->x, b->x, t );
    dst->y = scalar_Mix( a->y, b->y, t );
    dst->z = scalar_Mix( a->z, b->z, t );
    dst->pad = 0;
}

/*
  =========================================================================================================================================
  Vec4
  =========================================================================================================================================
*/

/*=======================================================================================================================================*/
float _Vec4_Normalise( vec4_t* dst, const vec4_t* src ) {
    float mag = Vec4_Magnitude( *src );
    float fac = 1.0f / mag;
    xassert( scalar_valid( fac ) == ztrue );

    Vec4_Muls( *dst, *src, fac );
    return mag;
}

/*=======================================================================================================================================*/
void _Vec4_Mix( vec4_t* dst, const vec4_t* a, const vec4_t* b, float t ) {
    t = scalar_Clamp( t, 0, 1 );
    dst->x = scalar_Mix( a->x, b->x, t );
    dst->y = scalar_Mix( a->y, b->y, t );
    dst->z = scalar_Mix( a->z, b->z, t );
    dst->w = scalar_Mix( a->w, b->w, t );
}
