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

#include "math/Frustum.h"
#include "core/Sys.h"
#include "math/Sphere.h"

static const vec3_t PLANE_VERTEX_POS[] = {
    { -1,  1,  1 },
    {  1,  1,  1 },
    {  1, -1,  1 },
    { -1, -1,  1 }
};

static const uint32_t PLANE_FACES[ FRUSTUM_NUM_PLANES ][ 4 ] = {
    0, 1, 2, 3,
    7, 6, 5, 4,
    1, 5, 6, 2,
    4, 0, 3, 7,
    4, 5, 1, 0,
    3, 2, 6, 7
};


/*=======================================================================================================================================*/
void Frustum_SetShape( frustum_t * self_, float fov, float aspect, float near, float far ) {
    xassert( self_ != NULL );
    self_->fov = fov;
    self_->aspect = aspect;
    self_->near = near;
    self_->far = far;
}

/*=======================================================================================================================================*/
void Frustum_SetTransform( frustum_t * self_, const mat4_t * xform ) {
    xassert( self_ != NULL );
    self_->transform = *xform;
}

/*=======================================================================================================================================*/
void Frustum_CalculatePlanes( frustum_t * self_, bool_t useTransform ) {
    /* Construct the frustum vertices in local space */
    xassert( self_ != NULL );
    
    float scaleY = scalar_Tan( scalar_DegToRad( self_->fov ) / 2.0f );
    float scaleX = scaleY * self_->aspect;
    
    vec3_t nearVert = { scaleX * self_->near, scaleY * self_->near, self_->near };
    vec3_t farVert = { scaleX * self_->far, scaleY * self_->far, self_->far };
    
    vec3_t localVerts[8];
    for ( int n = 0; n < 8; ++n ) {
        Vec3_Mul( localVerts[n], nearVert, PLANE_VERTEX_POS[n] );
        Vec3_Mul( localVerts[n+4], farVert, PLANE_VERTEX_POS[n] );
    }
    
    /* Calculate the planes */
    if ( useTransform == true ) {
    
        /* Transform the frustum verts into world space */
        vec3_t xformVerts[8];
        for ( uint32_t i = 0; i < 8; ++i ) {
            vec4_t local, transformed;
            Vec4_SetFromVec3( local, localVerts[ i ], 1 );
            
            _Mat4_Transform( &transformed, &self_->transform, &local );
            Vec3_SetXyz( xformVerts[ i ], transformed.x, transformed.y, transformed.z );
        }
        
        /* Calculate the planes */
        for ( uint32_t i = 0; i < FRUSTUM_NUM_PLANES; ++i ) {
            vec3_t verts[3];
            verts[ 0 ] = xformVerts[ PLANE_FACES[ i ][ 0 ] ];
            verts[ 1 ] = xformVerts[ PLANE_FACES[ i ][ 1 ] ];
            verts[ 2 ] = xformVerts[ PLANE_FACES[ i ][ 2 ] ];
            
            Plane_SetFromVerts( &self_->planes[ i ], verts );
        }
    }
    else {
        /* Calculate the planes */
        for ( uint32_t i = 0; i < FRUSTUM_NUM_PLANES; ++i ) {
            vec3_t verts[3];
            verts[ 0 ] = localVerts[ PLANE_FACES[ i ][ 0 ] ];
            verts[ 1 ] = localVerts[ PLANE_FACES[ i ][ 1 ] ];
            verts[ 2 ] = localVerts[ PLANE_FACES[ i ][ 2 ] ];
            
            Plane_SetFromVerts( &self_->planes[ i ], verts );
        }
    }
}

//======================================================================================================================
void Frustum_ClipPoint( const frustum_t * self_, vec3_t * result, const vec3_t * point ) {
    vec3_t  n;
    vec3_t delta;
    const plane_t * p = NULL;
    float pd = -1;
    
    xassert( self_ != NULL );
    xassert( result != NULL );
    xassert( point != NULL );
    
    *result = *point;
    
    for ( int ip = 0; ip < FRUSTUM_NUM_PLANES; ++ip ) {
        p = &self_->planes[ ip ];
        
        pd = Plane_GetDistance( p );
        Plane_GetNormal( p, &n );
        
        float d = Vec3_Dot( n, *p ) - pd;
        if ( d > 0 ) {
            /* *result -= *result * d */
            Vec3_Muls( delta, n, d );
            Vec3_Sub( *result, *result, delta );
        }
    }
}

//======================================================================================================================
bool_t Frustum_ClipSegment( const frustum_t * self_, vec3_t *result , const vec3_t * p0, const vec3_t * p1 ) {
    
    float closestT = 1;
    bool_t haveHit = false;
    plane_result_t hit;
    vec3_t q;
    float t;
    
    xassert( self_ != NULL );
    xassert( result != NULL );
    xassert( p0 != NULL );
    xassert( p1 != NULL );
    
    /* Clip the line segment to all of the planes, giving us the closest
       intersection point along the segment
     */
    for( int i = 0; i < FRUSTUM_NUM_PLANES; ++i ) {
        hit = Plane_TestSegment( &self_->planes[ i ], &q, &t, p0, p1);
        if (hit == PLANE_RESULT_HIT &&  t < closestT ) {
            closestT = t;
            haveHit = true;
        }
    }
    
    if ( haveHit == true ) {
        /* If the segment was hit by something, calculate the hit point on the line segment */
        xassert( scalar_IsValid( closestT ) );
        Vec3_Mix( *result, *p0, *p1, closestT );
    }
    
    return haveHit;
}

//======================================================================================================================
bool_t Frustum_TestSphere( const frustum_t * self_, vec3_t * closestPoint, const sphere_t * sphere ) {
    
    vec3_t center;
    vec3_t delta;
    float radiusSqr, distSqr;
    
    xassert( self_ != NULL );
    xassert( closestPoint != NULL );
    xassert( sphere != NULL );
    
    Sphere_GetCenter( sphere, &center );
    radiusSqr = Sphere_GetRadius( sphere );
    radiusSqr *= radiusSqr;     /* Note - we use squared distances */
    
    /* Get the clostest point on the frustum to the sphere center */
    Frustum_ClipPoint( self_, closestPoint, &center );
    
    /* Calculate the squared distance of the closest point from the outside of the sphere */
    Vec3_Sub( delta, *closestPoint, center );
    distSqr = Vec3_Dot( delta, delta ) - radiusSqr;
    xassert( scalar_IsValid( distSqr ) );
    
    /* Sphere is touching squared distance is l-equal zero Not touching if above zero. */
    return ( distSqr <= 0.0f ) ? true : false;
}
