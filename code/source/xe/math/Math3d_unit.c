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

#define DEG90_RAD 1.5708f

/*=======================================================================================================================================*/
bool_t vec3_testSimilarity( const vec3_t* a, const vec3_t* b, float tol) {
    vec3_t delta;
    float deltaMag;

    Vec3_Sub( delta, *a, *b );
    deltaMag = Vec3_Magnitude( delta );
    return scalar_Abs( deltaMag ) < tol;
}

/*=======================================================================================================================================*/
bool_t mat3_testSimilarity( const mat3_t* a, const mat3_t* b, float tol ) {
    vec3_t tstVec;
    vec3_t resA, resB;

    Vec3_Set( tstVec, 0, 5, 6 );

    _Mat3_Transform( &resA, a, &tstVec );
    _Mat3_Transform( &resB, b, &tstVec );

    bool_t same = vec3_testSimilarity( &resA, &resB, tol );
    return same;
}

/*=======================================================================================================================================*/
bool_t mat3_testInverse(float tol) {
    vec3_t rotAxis, srcVec, rotVec, rotVecInv;
    mat3_t rotMat, rotMatInv;

    /* Create the rotation matrix and the inverse rotation matrix */
    Vec3_Set( rotAxis, 0, 1, 0 );
    _Mat3_SetRotationAA( &rotMat, &rotAxis, 0.5f );
    _Mat3_Inverse( &rotMatInv, &rotMat );

    /* Rotate srcvec by the rotation matrix and then rotate it back using the inverse matrix */
    Vec3_Set( srcVec, 3, 4, 5 );
    _Mat3_Transform( &rotVec, &rotMat, &srcVec );
    _Mat3_Transform( &rotVecInv, &rotMatInv, &rotVec );

    bool_t same = vec3_testSimilarity( &rotVecInv, &srcVec, tol );
    return same;
}

/*=======================================================================================================================================*/
bool_t mat3_testRotationAA(float tol) {
    vec3_t rotAxis, srcVec, rotVec, tstVec;
    mat3_t rotMat;

    Vec3_Set( rotAxis, 0, 1, 0 );
    Vec3_Set( srcVec, 5, 5, 0 );
    _Mat3_SetRotationAA( &rotMat, &rotAxis, DEG90_RAD );
    _Mat3_Transform( &rotVec, &rotMat, &srcVec );

    Vec3_Set( tstVec, 0, 5, -5 );
    bool_t same = vec3_testSimilarity( &tstVec, &rotVec, tol );
    return same;
}
