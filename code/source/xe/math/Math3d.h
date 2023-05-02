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

#ifndef __MATH3D_H__
#define __MATH3D_H__

#include "core/Common.h"
#include "Scalar.h"
#include "Debug.h"

typedef struct Vec3_s {
    float x, y, z, pad;
} vec3_t;

typedef struct Vec4_s {
    float x, y, z, w;
} vec4_t;

typedef vec4_t quat_t;

typedef struct Mat3_s {
    vec3_t rows[3];
} mat3_t;

typedef struct Mat4_s {
    vec4_t  rows[4];
} mat4_t;

/*
  =========================================================================================================================================
  Vec3
  =========================================================================================================================================
*/
#define Vec3_Op(D, L, R, OP) (D).x = (L).x OP (R).x, (D).y = (L).y OP (R).y, (D).z = (L).z OP (R).z, (D).pad = 0
#define Vec3_Op_scalar(D, L, R, OP) (D).x = (L).x OP (R), (D).y = (L).y OP (R), (D).z = (L).z OP (R), (D).pad = 0

#define Vec3_GetX(D) (D).x
#define Vec3_GetY(D) (D).y
#define Vec3_GetZ(D) (D).z

#define Vec3_SetX(D, V) (D).x = (V)
#define Vec3_SetY(D, V) (D).y = (V)
#define Vec3_SetZ(D, V) (D).z = (V)
#define Vec3_Set(D, X, Y, Z) (D).x = (X), (D).y = (Y), (D).z = (Z), (D).pad = 0
#define Vec3_SetXyz(D, X, Y, Z) (D).x = (X), (D).y = (Y), (D).z = (Z), (D).pad = 0
#define Vec3_SetZero(D) Vec3_Set(D, 0, 0, 0)
#define Vec3_Copy( D, S ) Vec3_SetXyz( D, (S).x, (S).y, (S).z )

#define Vec3_Add(D, L, R) Vec3_Op(D, L, R, +)
#define Vec3_Sub(D, L, R) Vec3_Op(D, L, R, -)
#define Vec3_Mul(D, L, R) Vec3_Op(D, L, R, *)
#define Vec3_Div(D, L, R) Vec3_Op(D, L, R, /)
#define Vec3_Adds(D, L, R) Vec3_Op_scalar(D, L, R, +)
#define Vec3_Subs(D, L, R) Vec3_Op_scalar(D, L, R, -)
#define Vec3_Muls(D, L, R) Vec3_Op_scalar(D, L, R, *)
#define Vec3_Divs(D, L, R) Vec3_Op_scalar(D, L, R, /)

#define Vec3_Neg(D, S) Vec3_Set(D, -(S).x, -(S).y, -(S).z)

#define Vec3_Dot(L, R) ((L).x * (R).x + (L).y * (R).y + (L).z * (R).z)
#define Vec3_Cross(D, L, R) (D).x = (L).y * (R).z - (L).z * (R).y, (D).y = (L).z * (R).x - (L).x * (R).z, (D).z = (L).x * (R).y - (L).y * (R).x, (D).pad = 0
#define Vec3_Magnitude(V) scalar_Sqrt(Vec3_Dot(V, V))
#define Vec3_Normalise(D, S) _Vec3_Normalise(&(D), &(S))
#define Vec3_Mix(D, A, B, T) _Vec3_Mix(&(D), &(A), &(B), (T))
#define Vec3_MixCosine(D, A, B, T) _Vec3_MixCosine(&(D), &(A), &(B), (T))

/*
  =========================================================================================================================================
    Vec4
  =========================================================================================================================================
*/

#define Vec4_Op(D, L, R, OP) (D).x = (L).x OP (R).x, (D).y = (L).y OP (R).y, (D).z = (L).z OP (R).z, (D).w = (L).w OP (R).w
#define Vec4_Op_scalar(D, L, R, OP) (D).x = (L).x OP (R), (D).y = (L).y OP (R), (D).z = (L).z OP (R), (D).w = (L).w OP (R)

#define Vec4_GetX(D) (D).x
#define Vec4_GetY(D) (D).y
#define Vec4_GetZ(D) (D).z
#define Vec4_GetW(D) (D).w

#define Vec4_SetX(D, V) (D).x = (V)
#define Vec4_SetY(D, V) (D).y = (V)
#define Vec4_SetZ(D, V) (D).z = (V)
#define Vec4_SetW(D, V) (D).w = (V)
#define Vec4_Set(D, X, Y, Z, W) (D).x = (X), (D).y = (Y), (D).z = (Z), (D).w = (W)
#define Vec4_SetZero(D) Vec4_Set(D, 0, 0, 0)
#define Vec4_SetXyzw( D, X, Y, Z, W ) (D).x = (X), (D).y = (Y), (D).z = (Z), (D).w = (W)
#define Vec4_SetFromVec3( D, V, W) (D).x = (V).x, (D).y = (V).y, (D).z = (V).z, (D).w = (W)

#define Vec4_Add(D, L, R) Vec4_Op(D, L, R, +)
#define Vec4_Sub(D, L, R) Vec4_Op(D, L, R, -)
#define Vec4_Mul(D, L, R) Vec4_Op(D, L, R, *)
#define Vec4_Div(D, L, R) Vec4_Op(D, L, R, /)
#define Vec4_Adds(D, L, R) Vec4_Op_scalar(D, L, R, +)
#define Vec4_Subs(D, L, R) Vec4_Op_scalar(D, L, R, -)
#define Vec4_Muls(D, L, R) Vec4_Op_scalar(D, L, R, *)
#define Vec4_Divs(D, L, R) Vec4_Op_scalar(D, L, R, /)

#define Vec4_Neg(D, S) Vec4_Set(D, -(S).x, -(S).y, -(S).z, -(S).w)

#define Vec4_Dot(L, R) ((L).x * (R).x + (L).y * (R).y + (L).z * (R).z + (L).w * (R).w)
#define Vec4_Magnitude(V) scalar_Sqrt(Vec4_Dot(V, V))
#define Vec4_Normalise(D, S) _Vec4_Normalise(&(D), &(S))
#define Vec4_Mix(D, A, B, T) _Vec4_Mix(&(D), &(A), &(B), (T))


/*
=========================================================================================================================================
 Quat
=========================================================================================================================================
*/
#define Quat_SetIdentity(Q) Vec4_Set(Q, 0, 0, 0, 1)
#define Quat_Normalise(DST, SRC) _Vec4_Normalise( &(DST), &(src) )
#define Quat_Inverse(DST, SRC) Vec4_Set(DST, (SRC).x, (SRC).y, (SRC).z, -(SRC).w)

#define Quat_Concat(DST, LHS, RHS) _Quat_Concat(&(DST), &(LHS), &(RHS))
#define Quat_Slerp(DST, LHS, RHS, T) _Quat_Concat(&(DST), &(LHS), &(RHS), (T))
#define Quat_SetAA(DST, AXIS, ANGLE) _Quat_SetAA( &(DST), &(AXIS), (ANGLE) )

/*
=========================================================================================================================================
 Vec3
=========================================================================================================================================
*/

XE_API float _Vec3_Normalise(vec3_t* dst, const vec3_t* src);
XE_API void _Vec3_Mix(vec3_t* dst, const vec3_t* a, const vec3_t* b, float t);
XE_API void _Vec3_MixCosine( vec3_t * dst, const vec3_t * from, const vec3_t * to, float t );

/*
=========================================================================================================================================
Vec4
=========================================================================================================================================
*/

XE_API float _Vec4_Normalise( vec4_t* dst, const vec4_t* src );
XE_API void _Vec4_Mix( vec4_t* dst, const vec4_t* a, const vec4_t* b, float t );

/*
=========================================================================================================================================
 Mat3
=========================================================================================================================================
*/

#define Mat3_SetDiagonal( D, R0, R1, R2) ( Vec3_SetXyz( (D).rows[0], (R0), 0, 0 ), Vec3_SetXyz( (D).rows[1], 0, (R1), 0 ), Vec3_SetXyz( (D).rows[2], 0, 0, (R2) ) )
#define Mat3_SetIdentity(M) _Mat3_SetIdentity( &( M ) )
#define Mat3_Concat( D, L, R ) _Mat3_Concat( &(D), &(L), &(R) )
#define Mat3_Inverse( L, R ) _Mat3_Inverse( &(L), &(R) )
#define Mat3_SetRotationAA( L, AXIS, ANGLE ) _Mat3_SetRotationAA( &(L), &(AXIS), (ANGLE) )
#define Mat3_SetRotationQ( L, Q ) _Mat3_SetRotationQ( &(L), &(Q) )
#define Mat3_Transform( D, M, S) _Mat3_Transform( &(D), &(M), &(S) );

XE_API void _Mat3_SetIdentity( mat3_t* dst );
XE_API void _Mat3_Concat( mat3_t* dst, const mat3_t* lhs, const mat3_t* rhs );
XE_API void _Mat3_Inverse( mat3_t* dst, const mat3_t* src );
XE_API void _Mat3_SetRotationAA( mat3_t* dst, const vec3_t* axis, float angleRad );
XE_API void _Mat3_SetRotationQ( mat3_t* dst, const quat_t* q );
XE_API void _Mat3_Transform( vec3_t* dst, const mat3_t* xform, const vec3_t* src );

/*
=========================================================================================================================================
Mat4
=========================================================================================================================================
*/

#define Mat4_SetDiagonal( D, R0, R1, R2, R3) ( Vec4_SetXyzw( (D).rows[0], (R0), 0, 0, 0 ), Vec4_SetXyzw( (D).rows[1], 0, (R1), 0, 0 ), Vec4_SetXyzw( (D).rows[2], 0, 0, (R2), 0 ), Vec4_SetXyzw( (D).rows[3], 0, 0, (0, (R3) ) )
#define Mat4_SetIdentity(M) _Mat4_SetIdentity( &( M ) )
#define Mat4_Concat( D, L, R ) _Mat4_Concat( &(D), &(L), &(R) )
#define Mat4_Inverse( L, R ) _Mat4_Inverse( &(L), &(R) )
#define Mat4_SetRotationAA( L, AXIS, ANGLE ) _Mat4_SetRotationAA( &(L), &(AXIS), (ANGLE) )
#define Mat4_SetRotationQ( L, Q ) _Mat4_SetRotationQ( &(L), &(Q) )
#define Mat4_SetTranslationVec3( L, T ) Vec4_SetFromVec3( (L).rows[3], (T), 1 )
#define Mat4_SetTranslation( L, T ) Vec4_Set( (L).rows[3], (T) )

XE_API void _Mat4_SetIdentity( mat4_t* dst );
XE_API void _Mat4_Concat( mat4_t* dst, const mat4_t* lhs, const mat4_t* rhs );
XE_API void _Mat4_Inverse( mat4_t* dst, const mat4_t* src );
XE_API void _Mat4_SetRotationAA( mat4_t* dst, const vec3_t* axis, float angleRad );
XE_API void _Mat4_SetRotationQ( mat4_t* dst, const quat_t* q );
XE_API void _Mat4_Transform( vec4_t* dst, const mat4_t* xform, const vec4_t* src );

/*
=========================================================================================================================================
Mat3
=========================================================================================================================================
*/
XE_API void _Quat_Concat( quat_t* dst, const quat_t* lhs, const quat_t* rhs );
XE_API void _Quat_Slerp( quat_t* dst, const quat_t* lhs, const quat_t* rhs, float t );
XE_API void _Quat_SetAA( quat_t* dst, const vec3_t* axis, float angle );


#endif
