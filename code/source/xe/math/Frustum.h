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

#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "math/Plane.h"

#define FRUSTUM_NUM_PLANES 6

typedef struct frustum_s {
    float       fov;
    float       aspect;
    float       near;
    float       far;            /* where ever you are....? */
    plane_t     planes[ FRUSTUM_NUM_PLANES ];
    mat4_t      transform;
} frustum_t;

typedef vec4_t sphere_t;


XE_API void Frustum_SetShape( frustum_t * self_, float fov, float aspect, float near, float far );
XE_API void Frustum_SetTransform( frustum_t * self_, const mat4_t * xform );
XE_API void Frustum_CalculatePlanes( frustum_t * self_, boolean_t useTransform );

XE_API boolean_t Frustum_ClipSegment( const frustum_t * self_, vec3_t *result , const vec3_t * p0, const vec3_t * p1 );
XE_API boolean_t Frustum_TestSphere( const frustum_t * self_, vec3_t * closestPoint, const sphere_t * sphere );

#endif
