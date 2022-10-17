/*
===========================================================================================================================================

    Copyright 2019 James Steele

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

#include "core/Common.h"
#include "Debug.h"
#include "math/Math3d.h"

#define calcCofactor3x3(a, b, c, d) ((a * d) - (b * c))

/*
a b c
d e f
g h i
*/
#define calcCofactor(a, b, c, d, e, f, g, h, i)\
    ((a * calcCofactor3x3(e, f, h, i)) -\
     (b * calcCofactor3x3(d, f, g, i)) +\
     (c * calcCofactor3x3(d, e, g, h)))

#define a0 src->rows[0].x
#define a1 src->rows[0].y
#define a2 src->rows[0].z
#define a3 src->rows[0].w

#define a4 src->rows[1].x
#define a5 src->rows[1].y
#define a6 src->rows[1].z
#define a7 src->rows[1].w

#define a8  src->rows[2].x
#define a9  src->rows[2].y
#define a10 src->rows[2].z
#define a11 src->rows[2].w

#define a12 src->rows[3].x
#define a13 src->rows[3].y
#define a14 src->rows[3].z
#define a15 src->rows[3].w
/*
a0  a1  a2  a3
a4  a5  a6  a7
a8  a9 a10 a11
a12 a13 a14 a15
*/

/*=======================================================================================================================================*/
void _Mat4_SetIdentity( mat4_t* dst ) {
    Vec4_Set( dst->rows[0], 1, 0, 0, 0 );
    Vec4_Set( dst->rows[1], 0, 1, 0, 0 );
    Vec4_Set( dst->rows[2], 0, 0, 1, 0 );
    Vec4_Set( dst->rows[3], 0, 0, 0, 1 );
}

/*=======================================================================================================================================*/
void _Mat4_Concat( mat4_t* dst, const mat4_t* lhs, const mat4_t* rhs ) {
    vec4_t tmp0, tmp1, tmp2, tmp3;

    // Right
    Vec4_Muls( tmp0, rhs->rows[0], Vec4_GetX( lhs->rows[0] ) );
    Vec4_Muls( tmp1, rhs->rows[1], Vec4_GetY( lhs->rows[0] ) );
    Vec4_Muls( tmp2, rhs->rows[2], Vec4_GetZ( lhs->rows[0] ) );
    Vec4_Muls( tmp3, rhs->rows[3], Vec4_GetW( lhs->rows[0] ) );

    Vec4_Add( tmp0, tmp0, tmp1 );
    Vec4_Add( tmp2, tmp2, tmp3 );
    Vec4_Add( dst->rows[0], tmp0, tmp2 );

    // Up
    Vec4_Muls( tmp0, rhs->rows[0], Vec4_GetX( lhs->rows[1] ) );
    Vec4_Muls( tmp1, rhs->rows[1], Vec4_GetY( lhs->rows[1] ) );
    Vec4_Muls( tmp2, rhs->rows[2], Vec4_GetZ( lhs->rows[1] ) );
    Vec4_Muls( tmp3, rhs->rows[3], Vec4_GetW( lhs->rows[1] ) );

    Vec4_Add( tmp0, tmp0, tmp1 );
    Vec4_Add( tmp2, tmp2, tmp3 );
    Vec4_Add( dst->rows[1], tmp0, tmp2 );

    // At
    Vec4_Muls( tmp0, rhs->rows[0], Vec4_GetX( lhs->rows[2] ) );
    Vec4_Muls( tmp1, rhs->rows[1], Vec4_GetY( lhs->rows[2] ) );
    Vec4_Muls( tmp2, rhs->rows[2], Vec4_GetZ( lhs->rows[2] ) );
    Vec4_Muls( tmp3, rhs->rows[3], Vec4_GetW( lhs->rows[2] ) );

    Vec4_Add( tmp0, tmp0, tmp1 );
    Vec4_Add( tmp2, tmp2, tmp3 );
    Vec4_Add( dst->rows[2], tmp0, tmp2 );

    // Pos
    Vec4_Muls( tmp0, rhs->rows[0], Vec4_GetX( lhs->rows[3] ) );
    Vec4_Muls( tmp1, rhs->rows[1], Vec4_GetY( lhs->rows[3] ) );
    Vec4_Muls( tmp2, rhs->rows[2], Vec4_GetZ( lhs->rows[3] ) );
    Vec4_Muls( tmp3, rhs->rows[3], Vec4_GetW( lhs->rows[3] ) );

    Vec4_Add( tmp0, tmp0, tmp1 );
    Vec4_Add( tmp2, tmp2, tmp3 );
    Vec4_Add( dst->rows[3], tmp0, tmp2 );
}

/*=======================================================================================================================================*/
void _Mat4_Inverse( mat4_t* dst, const mat4_t* src ) {
    // Calculate the first 4 cofactors so we can calculate the determinate and early out
    // if it is too small
    float c0 = calcCofactor( a5, a6, a7, a9, a10, a11, a13, a14, a15 );
    float c1 = calcCofactor( a4, a6, a7, a8, a10, a11, a12, a14, a15 );
    float c2 = calcCofactor( a4, a5, a7, a8, a9, a11, a12, a13, a15 );
    float c3 = calcCofactor( a4, a5, a6, a8, a9, a10, a12, a13, a14 );

    float det = ( a0 * c0 ) - ( a1 * c1 ) + ( a2 * c2 ) - ( a3 * c3 );
    if ( scalar_Abs( det ) < 0.6e-5f ) {
        _Mat4_SetIdentity( dst );
        return;
    }

    float a = 1.0f / det;

    // If we get to here, we have a large enough determinate that we can use to calculate the inverse
    // so we need to calculate the rest of the coefficients
    float c4 = calcCofactor( a1, a2, a3, a9, a10, a11, a13, a14, a15 );
    float c5 = calcCofactor( a0, a2, a3, a8, a10, a11, a12, a14, a15 );
    float c6 = calcCofactor( a0, a1, a3, a8, a9, a11, a12, a13, a15 );
    float c7 = calcCofactor( a0, a1, a2, a8, a9, a10, a12, a13, a14 );

    float c8 = calcCofactor( a1, a2, a3, a5, a6, a7, a13, a14, a15 );
    float c9 = calcCofactor( a0, a2, a3, a4, a6, a7, a12, a14, a15 );
    float c10 = calcCofactor( a0, a1, a3, a4, a5, a7, a12, a13, a15 );
    float c11 = calcCofactor( a0, a1, a2, a4, a5, a6, a12, a13, a14 );

    float c12 = calcCofactor( a1, a2, a3, a5, a6, a7, a9, a10, a11 );
    float c13 = calcCofactor( a0, a2, a3, a4, a6, a7, a8, a10, a11 );
    float c14 = calcCofactor( a0, a1, a3, a4, a6, a7, a8, a9, a11 );
    float c15 = calcCofactor( a0, a1, a2, a4, a5, a6, a8, a9, a10 );

    // Create the adjunct matrix as four vectors
    vec4_t adjRight, adjUp, adjAt, adjPos;
    Vec4_Set( adjRight, c0, -c4,  c8,  -c12 );
    Vec4_Set( adjUp,   -c1,  c5, -c9,   c13 );
    Vec4_Set( adjAt,    c2, -c6,  c10, -c14 );
    Vec4_Set( adjPos,  -c3,  c7, -c11,  c15 );

    // Calculate the inverse from the adjunct and the inverse determinate
    Vec4_Muls( dst->rows[0], adjRight, a);
    Vec4_Muls( dst->rows[1], adjUp, a );
    Vec4_Muls( dst->rows[2], adjAt, a );
    Vec4_Muls( dst->rows[3], adjPos, a );
}

/*=======================================================================================================================================*/
void _Mat4_SetRotationAA( mat4_t* dst, const vec3_t* axis, float angleRad ) {
    /* Method taken from :-
    Graphics Gems (Glassner, Academic Press, 1990) */

    float c = scalar_Cos( angleRad );
    float s = scalar_Sin( angleRad );
    float t = 1.0f - c;

    float x = Vec3_GetX( *axis );
    float y = Vec3_GetY( *axis );
    float z = Vec3_GetZ( *axis );

    float sx = s * x;
    float sy = s * y;
    float sz = s * z;

    float tx2 = t * ( x * x );
    float ty2 = t * ( y * y );
    float tz2 = t * ( z * z );

    float txy = t * x * y;
    float txz = t * x * z;
    float tyz = t * y * z;

    Vec4_Set( dst->rows[0], tx2 + c, txy + sz, txz - sy, 0 );
    Vec4_Set( dst->rows[1], txy - sz, ty2 + c, tyz + sx, 0 );
    Vec4_Set( dst->rows[2], txz + sy, tyz - sx, tz2 + c, 0 );
}

/*=======================================================================================================================================*/
void _Mat4_SetRotationQ( mat4_t* dst, const quat_t* q ) {
    float xx = 2.0f * q->x * q->x;
    float yy = 2.0f * q->y * q->y;
    float zz = 2.0f * q->z * q->z;

    float xw = 2.0f * q->z * q->w;
    float yw = 2.0f * q->y * q->w;
    float zw = 2.0f * q->z * q->w;
    float xy = 2.0f * q->x * q->y;
    float xz = 2.0f * q->x * q->z;
    float yz = 2.0f * q->y * q->z;

    Vec4_Set( dst->rows[0], 1.0f - ( yy + zz ), xy + zw, xz - yw, 0 );
    Vec4_Set( dst->rows[1], xy - zw, 1.0f - ( xx + zz ), yz + xw, 0 );
    Vec4_Set( dst->rows[2], xz + yw, yz - xw, 1.0f - ( xx + yy ), 0 );
}

/*=======================================================================================================================================*/
void _mat4_Transform( vec4_t* dst, const mat4_t* xform, const vec4_t* src ) {
    vec4_t tmp0, tmp1, tmp2, tmp3;

    Vec4_Muls( tmp0, xform->rows[0], Vec4_GetX( *src ) );
    Vec4_Muls( tmp1, xform->rows[1], Vec4_GetY( *src ) );
    Vec4_Muls( tmp2, xform->rows[2], Vec4_GetZ( *src ) );
    Vec4_Muls( tmp3, xform->rows[3], Vec4_GetW( *src ) );

    Vec4_Add( tmp0, tmp0, tmp1 );
    Vec4_Add( tmp2, tmp2, tmp3 );
    Vec4_Add( *dst, tmp0, tmp2 );
}
