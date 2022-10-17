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

#include "Plane.h"

/*======================================================================================================================================= */
void Plane_SetXyzd(plane_t * plane, float x, float y, float z, float d ) {
    plane->x = x;
    plane->y = y;
    plane->z = z;
    plane->w = d;
}

/*======================================================================================================================================= */
void Plane_Set( plane_t * plane, const vec3_t * normal, float dist ) {
    plane->x = normal->x;
    plane->y = normal->y;
    plane->z = normal->z;
    plane->w = dist;
}

/*======================================================================================================================================= */
float Plane_GetDistance( const plane_t * plane ) {
    return plane->w;
}

/*======================================================================================================================================= */
void Plane_GetNormal( const plane_t * plane, vec3_t * normal ) {
    normal->x = plane->x;
    normal->y = plane->y;
    normal->z = plane->z;
}

/*======================================================================================================================================= */
void Plane_SetFromVerts( plane_t * plane, const vec3_t * verts ) {
    vec3_t e0, e1, n;
    float d;
    
    Vec3_Sub( e0, verts[1], verts[0] );
    Vec3_Sub( e1, verts[2], verts[0] );
    Vec3_Cross( n, e0, e1);
    Vec3_Normalise( n, n );
    d = Vec3_Dot( n, verts[0] );
    
    Plane_SetXyzd( plane, n.x, n.y, n.z, d );
}


/*======================================================================================================================================= */
plane_result_t Plane_TestSegment( const plane_t * self_, vec3_t * q, float * t, const vec3_t * p0, const vec3_t *p1 )  {
    
    vec3_t ab;
    vec3_t n;
    float d;
    float nDotAb = -1;
    
    Plane_GetNormal( self_, &n );
    Vec3_Sub(ab, *p1, *p0 );
    nDotAb = Vec3_Dot( n, ab );
    
    if ( scalar_Abs( nDotAb ) < 6.0e-6f ) {
        return PLANE_RESULT_PARALLEL;
    }
    
    d = Plane_GetDistance( self_ );
    *t = ( d - Vec3_Dot( n, *p0 ) );
    *t /= nDotAb;
    
    if ( ( *t >= 0 ) && ( *t <= 1.0f ) ) {
        
        /* q = p0 + ( ab * t ); */
        Vec3_Muls( *q, ab, *t);
        Vec3_Add( *q, *q, *p0 );
        return PLANE_RESULT_HIT;
    }
    
    return PLANE_RESULT_MISS;
}
