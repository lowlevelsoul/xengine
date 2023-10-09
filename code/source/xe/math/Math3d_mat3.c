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

#include "core/Platform.h"
#include "math/Math3d.h"

#define calcCofactor(a, b, c, d) ((a * d) - (b * c))

#define a0 Vec3_GetX(src->rows[0])
#define a1 Vec3_GetY(src->rows[0])
#define a2 Vec3_GetZ(src->rows[0])
#define a3 Vec3_GetX(src->rows[1])
#define a4 Vec3_GetY(src->rows[1])
#define a5 Vec3_GetZ(src->rows[1])
#define a6 Vec3_GetX(src->rows[2])
#define a7 Vec3_GetY(src->rows[2])
#define a8 Vec3_GetZ(src->rows[2])
/*
    Above macros define the 3x3 matrix as
    a0 a1 a2
    a3 a4 a5
    a6 a7 a8
*/

/*=======================================================================================================================================*/
void _Mat3_SetIdentity( mat3_t* dst ) {
    Vec3_Set( dst->rows[0],    1, 0, 0 );
    Vec3_Set( dst->rows[1],        0, 1, 0 );
    Vec3_Set( dst->rows[2],        0, 0, 1 );
}

/*=======================================================================================================================================*/
void _Mat3_concat( mat3_t* dst, const mat3_t* lhs, const mat3_t* rhs ) {
    vec3_t tmp0, tmp1, tmp2;

    /* dst->rows[0] */
    Vec3_Muls( tmp0, rhs->rows[0], Vec3_GetX( lhs->rows[0] ) );
    Vec3_Muls( tmp1, rhs->rows[1], Vec3_GetY( lhs->rows[0] ) );
    Vec3_Muls( tmp2, rhs->rows[2], Vec3_GetZ( lhs->rows[0] ) );

    Vec3_Add( tmp0, tmp0, tmp1 );
    Vec3_Add( dst->rows[0], tmp0, tmp2 );

    /* dst->rows[1] */
    Vec3_Muls( tmp0, rhs->rows[0], Vec3_GetX( lhs->rows[1] ) );
    Vec3_Muls( tmp1, rhs->rows[1], Vec3_GetY( lhs->rows[1] ) );
    Vec3_Muls( tmp2, rhs->rows[2], Vec3_GetZ( lhs->rows[1] ) );

    Vec3_Add( tmp0, tmp0, tmp1 );
    Vec3_Add( dst->rows[1], tmp0, tmp2 );

    /* dst->rows[2] */
    Vec3_Muls( tmp0, rhs->rows[0], Vec3_GetX( lhs->rows[2] ) );
    Vec3_Muls( tmp1, rhs->rows[1], Vec3_GetY( lhs->rows[2] ) );
    Vec3_Muls( tmp2, rhs->rows[2], Vec3_GetZ( lhs->rows[2] ) );

    Vec3_Add( tmp0, tmp0, tmp1 );
    Vec3_Add( dst->rows[2], tmp0, tmp2 );
}

/*=======================================================================================================================================*/
void _Mat3_inverse( mat3_t* dst, const mat3_t* src ) {
    // Calculate the first three cofactors so we can also calculate the determinate of the matrix and early out if
    // need be.
    float c0 = calcCofactor( a4, a5, a7, a8 );
    float c1 = calcCofactor( a3, a5, a6, a8 );
    float c2 = calcCofactor( a3, a4, a6, a7 );

    float det = ( a0 * c0 ) - ( a1 * c1 ) + ( a2 * c2 );

    if ( scalar_Abs( det ) < 0.6e-5f ) {
        _Mat3_SetIdentity( dst );
        return;
    }

    float a = 1.0f / det;

    // If we get to here, we have a large enough determinate that we can use to calculate the inverse
    // so we need to calculate the rest of the coefficients
    float c3 = calcCofactor( a1, a2, a7, a8 );
    float c4 = calcCofactor( a0, a2, a6, a8 );
    float c5 = calcCofactor( a0, a1, a6, a7 );

    float c6 = calcCofactor( a1, a2, a4, a5 );
    float c7 = calcCofactor( a0, a2, a3, a5 );
    float c8 = calcCofactor( a0, a1, a3, a4 );

    // Create the adjunct matrix from the cofactors
    vec3_t adjRight, adjUp, adjAt;
    Vec3_Set(adjRight, c0, -c3, c6 );
    Vec3_Set( adjUp, -c1, c4, -c7 );
    Vec3_Set( adjAt, c2, -c5, c8 );

    Vec3_Muls( dst->rows[0], adjRight, a );
    Vec3_Muls( dst->rows[1], adjUp, a );
    Vec3_Muls( dst->rows[2], adjAt, a );
}

/*=======================================================================================================================================*/
void _Mat3_SetRotationAA( mat3_t* dst, const vec3_t* axis, float angleRad ) {
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

    Vec3_Set( dst->rows[0], tx2 + c, txy + sz, txz - sy );
    Vec3_Set( dst->rows[1], txy - sz, ty2 + c, tyz + sx );
    Vec3_Set( dst->rows[2], txz + sy, tyz - sx, tz2 + c );
}

//=========================================================================================================================================
void _Mat3_SetRotationQ( mat3_t* dst, const quat_t* q ) {
    float xx = 2.0f * q->x * q->x;
    float yy = 2.0f * q->y * q->y;
    float zz = 2.0f * q->z * q->z;

    float xw = 2.0f * q->z * q->w;
    float yw = 2.0f * q->y * q->w;
    float zw = 2.0f * q->z * q->w;
    float xy = 2.0f * q->x * q->y;
    float xz = 2.0f * q->x * q->z;
    float yz = 2.0f * q->y * q->z;

    Vec3_Set(dst->rows[0],    1.0f - ( yy + zz ), xy + zw, xz - yw );
    Vec3_Set(dst->rows[1],        xy - zw, 1.0f - ( xx + zz ), yz + xw );
    Vec3_Set(dst->rows[2],        xz + yw, yz - xw, 1.0f - ( xx + yy ) );
}

//=========================================================================================================================================
void _Mat3_Transform( vec3_t* dst, const mat3_t* xform, const vec3_t* src ) {
    vec3_t tmp0, tmp1, tmp2;

    Vec3_Muls( tmp0, xform->rows[0], Vec3_GetX( *src ) );
    Vec3_Muls( tmp1, xform->rows[1], Vec3_GetY( *src ) );
    Vec3_Muls( tmp2, xform->rows[2], Vec3_GetZ( *src ) );

    Vec3_Add( tmp0, tmp0, tmp1 );
    Vec3_Add( *dst, tmp0, tmp2 );
}
