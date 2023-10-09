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

#ifndef __PLANE_H__
#define __PLANE_H__

#include "core/Platform.h"
#include "math/Math3d.h"

typedef vec4_t plane_t;

typedef enum plane_result {
    PLANE_RESULT_HIT = 0,
    PLANE_RESULT_PARALLEL,
    PLANE_RESULT_MISS,
} plane_result_t;

XE_API void Plane_SetXYZD(plane_t * plane, float x, float y, float z, float d );
XE_API void Plane_Set( plane_t * plane, const vec3_t * normal, float dist );
XE_API float Plane_GetDistance( const plane_t * plane );
XE_API void Plane_GetNormal( const plane_t * plane, vec3_t * normal );
XE_API void Plane_SetFromVerts( plane_t * plane, const vec3_t * verts );
XE_API plane_result_t Plane_TestSegment( const plane_t * self_, vec3_t * q, float * t, const vec3_t * p0, const vec3_t * p1 );


#endif 
