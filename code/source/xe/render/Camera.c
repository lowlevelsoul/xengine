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
#include "render/Camera.h"
#include "render/Render3d.h"

/*=======================================================================================================================================*/
void Camera_Initialise( camera_t * self_ ) {
    vec3_t eye = { 0, 0, 0 };
    vec3_t target = { 0, 0, 100 };
    vec3_t up = { 0, 1, 0 };
    
    Camera_SetMode( self_, CAMERA_MODE_TARGET );
    Camera_SetShape( self_, 80, 16.0f / 9.0f, 5.0f, 1000.0f );
    Camera_SetLookAt( self_, &eye, &target, &up );
}

/*=======================================================================================================================================*/
void Camera_SetMode( camera_t * self_, camera_mode_t mode ) {
    xassert( mode != CAMERA_MODE_ANGLES );
    self_->mode = mode;
    self_->transformStale = true;
}

/*=======================================================================================================================================*/
void Camera_SetLookAt( camera_t * self_, const vec3_t * eye, const vec3_t * target, const vec3_t * up ) {
    xassert( self_ != NULL );
    self_->eye = *eye;
    self_->target = *target;
    self_->up = *up;
    self_->transformStale = true;
}

/*=======================================================================================================================================*/
void Camera_SetEye( camera_t * self_, const vec3_t * eye ) {
    xassert( self_ != NULL );
    self_->eye = *eye;
    self_->transformStale = true;
}

/*=======================================================================================================================================*/
void Camera_SetTarget( camera_t * self_, const vec3_t * target ) {
    xassert( self_ != NULL );
    self_->target = *target;
    self_->transformStale = true;
}

/*=======================================================================================================================================*/
void Camera_SetUp( camera_t * self_, const vec3_t * up ) {
    xassert( self_ != NULL );
    self_->up = *up;
    self_->transformStale = true;
}

/*=======================================================================================================================================*/
void Camera_UpdateTransform( camera_t * self_ ) {
    xassert( self_ != NULL );
    xassert( self_->mode != CAMERA_MODE_ANGLES );
    
    if ( self_->transformStale == false ) {
        return;
    }
    
    if ( self_->mode == CAMERA_MODE_TARGET ) {
        vec3_t at, actualUp, right;
        
        Vec3_Sub( at, self_->target, self_->eye );

        Vec3_Cross( right, self_->up, at );
        Vec3_Cross( actualUp, at, right );
        
        Vec3_Normalise( at, at );
        Vec3_Normalise( right, right );
        Vec3_Normalise( actualUp, actualUp );
       
        
        Vec4_SetFromVec3( self_->transform.rows[0], right, 0);
        Vec4_SetFromVec3( self_->transform.rows[1], actualUp, 0);
        Vec4_SetFromVec3( self_->transform.rows[2], at, 0);
        Vec4_SetFromVec3( self_->transform.rows[3], self_->eye, 1);
        
        self_->transformStale = false;
    }
    else {
        // TODO: Implement me!
    }
}

/*=======================================================================================================================================*/
void Camera_UpdateProjection( camera_t * self_ ) {
    xassert( self_ != NULL );
    
    if ( self_->projectionScale == false ) {
        return;
    }
    
    Render_Calc3dProjectionMat( &self_->projection, scalar_DegToRad( self_->fovDegrees ), self_->aspect, self_->near, self_->far );
    self_->projectionScale = true;
}

/*=======================================================================================================================================*/
void Camera_UpdateMatrices( camera_t * self_ ) {
    if ( self_->projectionScale == true ) {
        Camera_UpdateProjection( self_ );
    }
    
    if ( self_->transformStale == true ) {
        Camera_UpdateTransform( self_ );
    }
}

/*=======================================================================================================================================*/
void Camera_SetShape( camera_t * self_, float fov, float aspect, float near, float far ) {
    self_->fovDegrees = fov;
    self_->aspect = aspect;
    self_->near = near;
    self_->far = far;
    self_->projectionScale = true;
}

/*=======================================================================================================================================*/
void Camera_GetEye( const camera_t * self_, vec3_t * eye ) {
    xassert( self_ != NULL );
    xassert( eye != NULL );
    *eye = self_->eye;
}

/*=======================================================================================================================================*/
void Camera_GetTarget( const camera_t * self_, vec3_t * target ) {
    xassert( self_ != NULL );
    xassert( target != NULL );
    *target = self_->target;
}

/*=======================================================================================================================================*/
void Camera_GetShape( const camera_t * self_, float * fov, float * aspect, float * near, float * far ) {
    xassert( self_ != NULL );
    
    *fov = self_->fovDegrees;
    *aspect = self_->aspect;
    *near = self_->near;
    *far = self_->far;
}
