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

/*=======================================================================================================================================*/
void _Quat_Concat( quat_t* dst, const quat_t* lhs, const quat_t* rhs ) {
    dst->x = rhs->x * rhs->w + rhs->y * rhs->z - rhs->z * rhs->y + rhs->w * rhs->x;
    dst->y = -rhs->x * rhs->z + rhs->y * rhs->w + rhs->z * rhs->x + rhs->w * rhs->y;
    dst->z = rhs->x * rhs->y - rhs->y * rhs->x + rhs->z * rhs->w + rhs->w * rhs->z;
    dst->w = -rhs->x * rhs->x - rhs->y * rhs->y - rhs->z * rhs->z + rhs->w * rhs->w;
}

/*=======================================================================================================================================*/
void _Quat_Slerp( quat_t* dst, const quat_t* from, const quat_t* to, float t ) {
    float cosOmega;
    float q1x;
    float q1y;
    float q1z;
    float q1w;
    float k0, k1;
    float sinOmega, omega, oneOverSinOmega;

    // Limit interval to 0 and 1
    if ( t <= 0 ) {
        *dst = *from;
        return;
    }

    if ( t >= 1 ) {
        *dst = *to;
        return;
    }

    // Compute "cosine of angle between quaternions" using dot product
    cosOmega = Vec3_Dot(*from, *to );

    // If negative dot, use -q1.  Two quaternions q and -q represent
    // the same rotation, but may produce different slerp.  We chose
    // q or -q to rotate using the acute angle.
    q1x = to->x;
    q1y = to->y;
    q1z = to->z;
    q1w = to->w;

    if ( cosOmega < 0.0f ) {
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        q1w = -q1w;
        cosOmega = -cosOmega;
    }

    // Compute interpolation fraction, checking for quaternions almost exactly the same
    if ( cosOmega > 0.9999f ) {
        // Very close - just use linear interpolation, which will protect against a divide by zero
        k0 = 1.0f - t;
        k1 = t;
    } else {
        // Compute the sin of the angle using the trig identity sin^2(omega) + cos^2(omega) = 1
        sinOmega = scalar_Sqrt( 1.0f - ( cosOmega * cosOmega ) );

        // Compute the angle from its sin and cosine
        omega = scalar_Atan2( sinOmega, cosOmega );

        // Compute inverse of denominator, so we only have to divide once
        oneOverSinOmega = 1.0f / sinOmega;

        // Compute interpolation parameters
        k0 = scalar_Sin( ( 1.0f - t ) * omega ) * oneOverSinOmega;
        k1 = scalar_Sin( t * omega ) * oneOverSinOmega;
    }

    // Interpolate and return new quaternion
    dst->x = ( k0 * from->x ) + ( k1 * q1x );
    dst->y = ( k0 * from->y ) + ( k1 * q1y );
    dst->z = ( k0 * from->z ) + ( k1 * q1z );
    dst->w = ( k0 * from->w ) + ( k1 * q1w );
}

/*=======================================================================================================================================*/
void _Quat_SetAA( quat_t* dst, const vec3_t* axis, float angle ) {
    float c = scalar_Cos( angle / 2.0f );
    float s = scalar_Sin( angle / 2.0f );

    dst->x = Vec3_GetX( *axis ) * s;
    dst->y = Vec3_GetY( *axis ) * s;
    dst->z = Vec3_GetZ( *axis ) * s;
    dst->w = c;
}
