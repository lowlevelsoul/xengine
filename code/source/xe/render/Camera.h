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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/Math3d.h"

typedef enum camera_mode_e {
    CAMERA_MODE_TARGET = 0,
    CAMERA_MODE_ANGLES,
} camera_mode_t;

typedef struct camera_s {
    vec3_t      eye;
    vec3_t      target;
    vec3_t      up;

    float       pitch;
    float       yaw;
    float       roll;
    
    float       fovDegrees;
    float       aspect;
    float       near;
    float       far;
    
    mat4_t      transform;
    mat4_t      projection;
    
    camera_mode_t mode;
    bool_t   transformStale;
    bool_t   projectionScale;
} camera_t;


XE_API void Camera_Initialise( camera_t * self_ );
XE_API void Camera_SetMode( camera_t * self_, camera_mode_t mode );
XE_API void Camera_SetLookAt( camera_t * self_, const vec3_t * eye, const vec3_t * target, const vec3_t * up );
XE_API void Camera_SetEye( camera_t * self_, const vec3_t * eye );
XE_API void Camera_SetTarget( camera_t * self_, const vec3_t * target );
XE_API void Camera_SetUp( camera_t * self_, const vec3_t * up );
XE_API void Camera_UpdateTransform( camera_t * self_ );
XE_API void Camera_UpdateProjection( camera_t * self_ );
XE_API void Camera_UpdateMatrices( camera_t * self_ );
XE_API void Camera_SetShape( camera_t * self_, float fov, float aspect, float near, float far );

XE_API void Camera_GetEye( const camera_t * self_, vec3_t * eye );
XE_API void Camera_GetTarget( const camera_t * self_, vec3_t * target );
XE_API void Camera_GetShape( const camera_t * self_, float * fov, float * aspect, float * near, float * far );

#endif
