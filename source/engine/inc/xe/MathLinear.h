/*
==============================================================================================================
    This file is part of XEngine 
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __MATH_LINEAR_H__
#define __MATH_LINEAR_H__

#include "Platform.h"
#include "MathScalar.h"

typedef struct vec2_s {
    float x;
    float y;
} vec2_t;

typedef struct vec3_s {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct vec4_s {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

typedef vec4_t quat_t;

typedef struct mat3_s {
    vec3_t rows[3];
} mat3_t;

typedef struct mat4_s {
    vec4_t rows[4];
} mat4_t;

/* 
Vec2 Functions
*/

/*==========================================================================================================*/
INLINE void Vec2_Copy(vec2_t* v, const vec2_t* a) {
    v->x = a->x;
    v->y = a->y;
}

/*==========================================================================================================*/
INLINE void Vec2_SetXy(vec2_t* v, float x, float y) {
    v->x = x;
    v->y = y;
}

/*==========================================================================================================*/
INLINE void Vec2_Zero(vec2_t* v) {
    v->x = 0.0f;
    v->y = 0.0f;
}

/*==========================================================================================================*/
INLINE void Vec2_Add(vec2_t* v, const vec2_t* a, const vec2_t* b) {
    v->x = a->x + b->x;
    v->y = a->y + b->y;
}

/*==========================================================================================================*/
INLINE void Vec2_Sub(vec2_t* v, const vec2_t* a, const vec2_t* b) {
    v->x = a->x - b->x;
    v->y = a->y - b->y;
}

/*==========================================================================================================*/
INLINE void Vec2_Mul(vec2_t* v, const vec2_t* a, const vec2_t * b ) {
    v->x = a->x * b->x;
    v->y = a->y * b->y;
}

/*==========================================================================================================*/
INLINE void Vec2_Scale(vec2_t* v, const vec2_t* a, float s) {
    v->x = a->x * s;
    v->y = a->y * s;
}

/*==========================================================================================================*/
INLINE float Vec2_Dot(const vec2_t* a, const vec2_t* b) {
    return a->x * b->x + a->y * b->y;
}

/*==========================================================================================================*/
INLINE float Vec2_Length(const vec2_t* v) {
    return Scalar_Sqrt( Vec2_Dot(v, v) );
}

/*==========================================================================================================*/
INLINE float Vec2_Normalize(vec2_t* v) {
    float len = Vec2_Length(v);
    if (len > 0.0f) {
        v->x /= len;
        v->y /= len;
    }

    return len;
}

/*==========================================================================================================*/
INLINE void Vec2_Negate(vec2_t* v) {
    v->x = -v->x;
    v->y = -v->y;
}
/*==========================================================================================================*/      
INLINE void Vec2_Mix(vec2_t* v, const vec2_t* a, const vec2_t* b, float t) {
    v->x = a->x * (1.0f - t) + b->x * t;
    v->y = a->y * (1.0f - t) + b->y * t;
}

/*==========================================================================================================*/
INLINE void Vec2_MixSin(vec2_t* v, const vec2_t* a, const vec2_t* b, float t) {
    float s = Scalar_Sin(t * XE_PI * 0.5f);
    v->x = a->x * (1.0f - s) + b->x * s;
    v->y = a->y * (1.0f - s) + b->y * s;
}

/* 
Vec3 Functions
*/

/*==========================================================================================================*/
INLINE void Vec3_SetXyz(vec3_t* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

/*==========================================================================================================*/
INLINE void Vec3_Copy(vec3_t* v, const vec3_t* a) {
    v->x = a->x;
    v->y = a->y;
    v->z = a->z;
}

/*==========================================================================================================*/
INLINE void Vec3_Zero(vec3_t* v) {
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
}

/*==========================================================================================================*/
INLINE void Vec3_Add(vec3_t* v, const vec3_t* a, const vec3_t* b) {
    v->x = a->x + b->x;
    v->y = a->y + b->y;
    v->z = a->z + b->z;
}

/*==========================================================================================================*/
INLINE void Vec3_Sub(vec3_t* v, const vec3_t* a, const vec3_t* b) {
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}

/*==========================================================================================================*/
INLINE void Vec3_Mul(vec3_t* v, const vec3_t* a, const vec3_t* b) {
    v->x = a->x * b->x;
    v->y = a->y * b->y;
    v->z = a->z * b->z;
}

/*==========================================================================================================*/
INLINE void Vec3_Scale(vec3_t* v, const vec3_t* a, float s) {
    v->x = a->x * s;
    v->y = a->y * s;
    v->z = a->z * s;
}

/*==========================================================================================================*/
INLINE float Vec3_Dot(const vec3_t* a, const vec3_t* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

/*==========================================================================================================*/
INLINE void Vec3_Cross(vec3_t* v, const vec3_t* a, const vec3_t* b) {
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    float z = a->x * b->y - a->y * b->x;
    v->x = x;
    v->y = y;
    v->z = z;
}

/*==========================================================================================================*/
INLINE float Vec3_Length(const vec3_t* v) {
    return Scalar_Sqrt(Vec3_Dot(v, v));
}

/*==========================================================================================================*/
INLINE float Vec3_Normalize(vec3_t* v) {
    float len = Vec3_Length(v);
    if (len > 0.0f) {
        v->x /= len;
        v->y /= len;
        v->z /= len;
    }

    return len;
}

/*==========================================================================================================*/
INLINE void Vec3_Negate(vec3_t* v) {
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
}

/*==========================================================================================================*/
INLINE void Vec3_Mix(vec3_t* v, const vec3_t* a, const vec3_t* b, float t) {
    v->x = a->x * (1.0f - t) + b->x * t;
    v->y = a->y * (1.0f - t) + b->y * t;
    v->z = a->z * (1.0f - t) + b->z * t;
}

/*==========================================================================================================*/
INLINE void Vec3_MixSin(vec3_t* v, const vec3_t* a, const vec3_t* b, float t) {
    float s = Scalar_Sin(t * XE_PI * 0.5f);
    v->x = a->x * (1.0f - s) + b->x * s;
    v->y = a->y * (1.0f - s) + b->y * s;
    v->z = a->z * (1.0f - s) + b->z * s;
}
/*==========================================================================================================*/





/* 
Vec4 Functions
*/

INLINE void Vec4_SetXyzw(vec4_t* v, float x, float y, float z, float w) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
}

/*==========================================================================================================*/
INLINE void Vec4_Copy(vec4_t* v, const vec4_t* a) {
    v->x = a->x;
    v->y = a->y;
    v->z = a->z;
    v->w = a->w;
}

/*==========================================================================================================*/
INLINE void Vec4_Zero(vec4_t* v) {
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
    v->w = 0.0f;
}

/*==========================================================================================================*/
INLINE void Vec4_Add(vec4_t* v, const vec4_t* a, const vec4_t* b) {
    v->x = a->x + b->x;
    v->y = a->y + b->y;
    v->z = a->z + b->z;
    v->w = a->w + b->w;
}

/*==========================================================================================================*/
INLINE void Vec4_Sub(vec4_t* v, const vec4_t* a, const vec4_t* b) {
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
    v->w = a->w - b->w;
}

/*==========================================================================================================*/
INLINE void Vec4_Mul(vec4_t* v, const vec4_t* a, const vec4_t* b) {
    v->x = a->x * b->x;
    v->y = a->y * b->y;
    v->z = a->z * b->z;
    v->w = a->w * b->w;
}

/*==========================================================================================================*/
INLINE void Vec4_Scale(vec4_t* v, const vec4_t* a, float s) {
    v->x = a->x * s;
    v->y = a->y * s;
    v->z = a->z * s;
    v->w = a->w * s;
}

/*==========================================================================================================*/
INLINE float Vec4_Dot(const vec4_t* a, const vec4_t* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

/*==========================================================================================================*/
INLINE float Vec4_Length(const vec4_t* v) {
    return Scalar_Sqrt(Vec4_Dot(v, v));
}

/*==========================================================================================================*/
INLINE float Vec4_Normalize(vec4_t* v) {
    float len = Vec4_Length(v);
    if (len > 0.0f) {
        v->x /= len;
        v->y /= len;
        v->z /= len;
        v->w /= len;
    }

    return len;
}

/*==========================================================================================================*/
INLINE void Vec4_Negate(vec4_t* v) {
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
    v->w = -v->w;
}

/*==========================================================================================================*/
INLINE void Vec4_Mix(vec4_t* v, const vec4_t* a, const vec4_t* b, float t) {
    v->x = a->x * (1.0f - t) + b->x * t;
    v->y = a->y * (1.0f - t) + b->y * t;
    v->z = a->z * (1.0f - t) + b->z * t;
    v->w = a->w * (1.0f - t) + b->w * t;
}

/*==========================================================================================================*/
INLINE void Vec4_MixSin(vec4_t* v, const vec4_t* a, const vec4_t* b, float t) {
    float s = Scalar_Sin(t * XE_PI * 0.5f);
    v->x = a->x * (1.0f - s) + b->x * s;
    v->y = a->y * (1.0f - s) + b->y * s;
    v->z = a->z * (1.0f - s) + b->z * s;
    v->w = a->w * (1.0f - s) + b->w * s;
}

/* 
Quat Functions
*/

/*==========================================================================================================*/
INLINE void Quat_SetXyzw(quat_t* q, float x, float y, float z, float w) {
    q->x = x;
    q->y = y;
    q->z = z;
    q->w = w;
}

/*==========================================================================================================*/
INLINE void Quat_SetXyz(quat_t* q, float x, float y, float z) {
    q->x = x;
    q->y = y;
    q->z = z;
    float t = 1.0f - (x * x + y * y + z * z);
    q->w = t > 0.0f ? Scalar_Sqrt(t) : 0.0f;
}

/*==========================================================================================================*/
INLINE void Quat_SetAxisAngle(quat_t* q, const vec3_t* axis, float angle) {
    float len = Vec3_Length(axis);
    float half = angle * 0.5f;
    float s = len > 0.0f ? Scalar_Sin(half) / len : 0.0f;
    q->x = axis->x * s;
    q->y = axis->y * s;
    q->z = axis->z * s;
    q->w = Scalar_Cos(half);
}

/*==========================================================================================================*/
INLINE void Quat_Copy(quat_t* q, const quat_t* a) {
    q->x = a->x;
    q->y = a->y;
    q->z = a->z;
    q->w = a->w;
}

/*==========================================================================================================*/
INLINE void Quat_Zero(quat_t* q) {
    q->x = 0.0f;
    q->y = 0.0f;
    q->z = 0.0f;
    q->w = 0.0f;
}

/*==========================================================================================================*/
INLINE void Quat_Add(quat_t* q, const quat_t* a, const quat_t* b) {
    q->x = a->x + b->x;
    q->y = a->y + b->y;
    q->z = a->z + b->z;
    q->w = a->w + b->w;
}

/*==========================================================================================================*/
INLINE void Quat_Sub(quat_t* q, const quat_t* a, const quat_t* b) {
    q->x = a->x - b->x;
    q->y = a->y - b->y;
    q->z = a->z - b->z;
    q->w = a->w - b->w;
}

/*==========================================================================================================*/
INLINE void Quat_Mul(quat_t* q, const quat_t* a, const quat_t* b) {
    float x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    float y = a->w * b->y - a->x * b->z + a->y * b->w + a->z * b->x;
    float z = a->w * b->z + a->x * b->y - a->y * b->x + a->z * b->w;
    float w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    q->x = x;
    q->y = y;
    q->z = z;
    q->w = w;
}

/*==========================================================================================================*/
INLINE void Quat_Scale(quat_t* q, const quat_t* a, float s) {
    q->x = a->x * s;
    q->y = a->y * s;
    q->z = a->z * s;
    q->w = a->w * s;
}

/*==========================================================================================================*/
INLINE float Quat_Dot(const quat_t* a, const quat_t* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

/*==========================================================================================================*/
INLINE float Quat_Length(const quat_t* q) {
    return Scalar_Sqrt(Quat_Dot(q, q));
}

/*==========================================================================================================*/
INLINE float Quat_Normalize(quat_t* q) {
    float len = Quat_Length(q);
    if (len > 0.0f) {
        q->x /= len;
        q->y /= len;
        q->z /= len;
        q->w /= len;
    }

    return len;
}

/*==========================================================================================================*/
INLINE void Quat_Negate(quat_t* q) {
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
    q->w = -q->w;
}

/*==========================================================================================================*/
INLINE void Quat_Mix(quat_t* q, const quat_t* a, const quat_t* b, float t) {
    q->x = a->x * (1.0f - t) + b->x * t;
    q->y = a->y * (1.0f - t) + b->y * t;
    q->z = a->z * (1.0f - t) + b->z * t;
    q->w = a->w * (1.0f - t) + b->w * t;
}

/*==========================================================================================================*/
INLINE void Quat_MixSin(quat_t* q, const quat_t* a, const quat_t* b, float t) {
    float s = Scalar_Sin(t * XE_PI * 0.5f);
    q->x = a->x * (1.0f - s) + b->x * s;
    q->y = a->y * (1.0f - s) + b->y * s;
    q->z = a->z * (1.0f - s) + b->z * s;
    q->w = a->w * (1.0f - s) + b->w * s;
}

/*
==========================================================================================================
Mat3
==========================================================================================================
*/

/*==========================================================================================================*/
INLINE void Mat3_SetIdentity(mat3_t* m) {
    Vec3_SetXyz(&m->rows[ 0 ], 1.0f, 0.0f, 0.0f);
    Vec3_SetXyz(&m->rows[ 1 ], 0.0f, 1.0f, 0.0f);   
    Vec3_SetXyz(&m->rows[ 2 ], 0.0f, 0.0f, 1.0f);
}

/*==========================================================================================================*/
INLINE void Mat3_SetDiagonal(mat3_t* m, float r0, float r1, float r2 ) {
    Vec3_SetXyz(&m->rows[ 0 ], r0, 0.0f, 0.0f);
    Vec3_SetXyz(&m->rows[ 1 ], 0.0f, r1, 0.0f);   
    Vec3_SetXyz(&m->rows[ 2 ], 0.0f, 0.0f, r2);
}

/*==========================================================================================================*/
INLINE void Mat3_SetZero(mat3_t* m) {
    Vec3_SetXyz(&m->rows[ 0 ], 0.0f, 0.0f, 0.0f);
    Vec3_SetXyz(&m->rows[ 1 ], 0.0f, 0.0f, 0.0f);   
    Vec3_SetXyz(&m->rows[ 2 ], 0.0f, 0.0f, 0.0f);
}

/*==========================================================================================================*/
INLINE void Mat3_Copy(mat3_t* dst, const mat3_t* src) {
    Vec3_Copy(&dst->rows[ 0 ], &src->rows[ 0 ]);
    Vec3_Copy(&dst->rows[ 1 ], &src->rows[ 1 ]);
    Vec3_Copy(&dst->rows[ 2 ], &src->rows[ 2 ]);
}

/*==========================================================================================================*/
INLINE void Mat3_Transpose( mat3_t * dst, const mat3_t* src) {
    Vec3_SetXyz(&dst->rows[ 0 ], src->rows[ 0 ].x, src->rows[ 1 ].x, src->rows[ 2 ].x);
    Vec3_SetXyz(&dst->rows[ 1 ], src->rows[ 0 ].y, src->rows[ 1 ].y, src->rows[ 2 ].y);
    Vec3_SetXyz(&dst->rows[ 2 ], src->rows[ 0 ].z, src->rows[ 1 ].z, src->rows[ 2 ].z);
}

/*==========================================================================================================*/  
INLINE void Mat3_MulVec3(vec3_t* dst, const mat3_t* m, const vec3_t* v) {
    vec3_t tmp0, tmp1, tmp2;
    Vec3_Scale(&tmp0, &m->rows[ 0 ], v->x);
    Vec3_Scale(&tmp1, &m->rows[ 1 ], v->y);
    Vec3_Scale(&tmp2, &m->rows[ 2 ], v->z);

    Vec3_Add(dst, &tmp0, &tmp1);
    Vec3_Add(dst, dst, &tmp2);
}

/*==========================================================================================================*/
INLINE void Mat3_Concat( mat3_t * dst, const mat3_t * a, const mat3_t * b ) {
    Mat3_MulVec3(&dst->rows[ 0 ], b, &a->rows[ 0 ]);
    Mat3_MulVec3(&dst->rows[ 1 ], b, &a->rows[ 1 ]);
    Mat3_MulVec3(&dst->rows[ 2 ], b, &a->rows[ 2 ]);
}

/*==========================================================================================================*/
INLINE float Mat3_Det(const mat3_t* m) {
    return m->rows[ 0 ].x * (m->rows[ 1 ].y * m->rows[ 2 ].z - m->rows[ 1 ].z * m->rows[ 2 ].y) -
           m->rows[ 0 ].y * (m->rows[ 1 ].x * m->rows[ 2 ].z - m->rows[ 1 ].z * m->rows[ 2 ].x) +
           m->rows[ 0 ].z * (m->rows[ 1 ].x * m->rows[ 2 ].y - m->rows[ 1 ].y * m->rows[ 2 ].x);
}

/*==========================================================================================================*/  
INLINE void Mat3_Inverse(mat3_t* dst, const mat3_t* m) {    
    float det = Mat3_Det(m);
    if (det <= XE_EPSILON ) {
        Mat3_SetIdentity(dst);
        return; 
    }

    float invDet = 1.0f / det;

    dst->rows[ 0 ].x =  (m->rows[ 1 ].y * m->rows[ 2 ].z - m->rows[ 1 ].z * m->rows[ 2 ].y) * invDet;   
    dst->rows[ 0 ].y = -(m->rows[ 0 ].y * m->rows[ 2 ].z - m->rows[ 0 ].z * m->rows[ 2 ].y) * invDet;
    dst->rows[ 0 ].z =  (m->rows[ 0 ].y * m->rows[ 1 ].z - m->rows[ 0 ].z * m->rows[ 1 ].y) * invDet;

    dst->rows[ 1 ].x = -(m->rows[ 1 ].x * m->rows[ 2 ].z - m->rows[ 1 ].z * m->rows[ 2 ].x) * invDet;
    dst->rows[ 1 ].y =  (m->rows[ 0 ].x * m->rows[ 2 ].z - m->rows[ 0 ].z * m->rows[ 2 ].x) * invDet;
    dst->rows[ 1 ].z = -(m->rows[ 0 ].x * m->rows[ 1 ].z - m->rows[ 0 ].z * m->rows[ 1 ].x) * invDet;

    dst->rows[ 2 ].x =  (m->rows[ 1 ].x * m->rows[ 2 ].y - m->rows[ 1 ].y * m->rows[ 2 ].x) * invDet;
    dst->rows[ 2 ].y = -(m->rows[ 0 ].x * m->rows[ 2 ].y - m->rows[ 0 ].y * m->rows[ 2 ].x) * invDet;
    dst->rows[ 2 ].z =  (m->rows[ 0 ].x * m->rows[ 1 ].y - m->rows[ 0 ].y * m->rows[ 1 ].x) * invDet;
}
/*==========================================================================================================*/  
INLINE void Mat3_SetRotationAA( mat3_t * dst, const vec3_t * axis, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;

    float x = axis->x;
    float y = axis->y;
    float z = axis->z;

    dst->rows[ 0 ].x = t * x * x + c;
    dst->rows[ 0 ].y = t * x * y + s * z;
    dst->rows[ 0 ].z = t * x * z - s * y;

    dst->rows[ 1 ].x = t * x * y - s * z;
    dst->rows[ 1 ].y = t * y * y + c;
    dst->rows[ 1 ].z = t * y * z + s * x;

    dst->rows[ 2 ].x = t * x * z + s * y;
    dst->rows[ 2 ].y = t * y * z - s * x;
    dst->rows[ 2 ].z = t * z * z + c;
}

/*==========================================================================================================*/ 
INLINE void Mat3_SetRotationQ( mat3_t * dst, const quat_t * q) {
    float x = q->x;
    float y = q->y;
    float z = q->z;
    float w = q->w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    dst->rows[ 0 ].x = 1.0f - 2.0f * (yy + zz);
    dst->rows[ 0 ].y = 2.0f * (xy + wz);
    dst->rows[ 0 ].z = 2.0f * (xz - wy);

    dst->rows[ 1 ].x = 2.0f * (xy - wz);
    dst->rows[ 1 ].y = 1.0f - 2.0f * (xx + zz);
    dst->rows[ 1 ].z = 2.0f * (yz + wx);

    dst->rows[ 2 ].x = 2.0f * (xz + wy);
    dst->rows[ 2 ].y = 2.0f * (yz - wx);
    dst->rows[ 2 ].z = 1.0f - 2.0f * (xx + yy);
}

/*
==========================================================================================================
Mat4
==========================================================================================================
*/

/*==========================================================================================================*/
INLINE void Mat4_SetIdentity(mat4_t* m) {
    Vec4_SetXyzw(&m->rows[ 0 ], 1.0f, 0.0f, 0.0f, 0.0f);
    Vec4_SetXyzw(&m->rows[ 1 ], 0.0f, 1.0f, 0.0f, 0.0f);
    Vec4_SetXyzw(&m->rows[ 2 ], 0.0f, 0.0f, 1.0f, 0.0f);
    Vec4_SetXyzw(&m->rows[ 3 ], 0.0f, 0.0f, 0.0f, 1.0f);
}

/*==========================================================================================================*/
INLINE void Mat4_SetDiagonal(mat4_t* m, float r0, float r1, float r2, float r3) {
    Vec4_SetXyzw(&m->rows[ 0 ], r0, 0.0f, 0.0f, 0.0f);
    Vec4_SetXyzw(&m->rows[ 1 ], 0.0f, r1, 0.0f, 0.0f);
    Vec4_SetXyzw(&m->rows[ 2 ], 0.0f, 0.0f, r2, 0.0f);
    Vec4_SetXyzw(&m->rows[ 3 ], 0.0f, 0.0f, 0.0f, r3);
}

/*==========================================================================================================*/
INLINE void Mat4_SetZero(mat4_t* m) {
    Vec4_Zero(&m->rows[ 0 ]);
    Vec4_Zero(&m->rows[ 1 ]);
    Vec4_Zero(&m->rows[ 2 ]);
    Vec4_Zero(&m->rows[ 3 ]);
}

/*==========================================================================================================*/
INLINE void Mat4_Copy(mat4_t* dst, const mat4_t* src) {
    Vec4_Copy(&dst->rows[ 0 ], &src->rows[ 0 ]);
    Vec4_Copy(&dst->rows[ 1 ], &src->rows[ 1 ]);
    Vec4_Copy(&dst->rows[ 2 ], &src->rows[ 2 ]);
    Vec4_Copy(&dst->rows[ 3 ], &src->rows[ 3 ]);
}

/*==========================================================================================================*/
INLINE void Mat4_Transpose(mat4_t* dst, const mat4_t* src) {
    mat4_t tmp;
    Vec4_SetXyzw(&tmp.rows[ 0 ], src->rows[ 0 ].x, src->rows[ 1 ].x, src->rows[ 2 ].x, src->rows[ 3 ].x);
    Vec4_SetXyzw(&tmp.rows[ 1 ], src->rows[ 0 ].y, src->rows[ 1 ].y, src->rows[ 2 ].y, src->rows[ 3 ].y);
    Vec4_SetXyzw(&tmp.rows[ 2 ], src->rows[ 0 ].z, src->rows[ 1 ].z, src->rows[ 2 ].z, src->rows[ 3 ].z);
    Vec4_SetXyzw(&tmp.rows[ 3 ], src->rows[ 0 ].w, src->rows[ 1 ].w, src->rows[ 2 ].w, src->rows[ 3 ].w);
    Mat4_Copy(dst, &tmp);
}

/*==========================================================================================================*/
INLINE void Mat4_MulVec4(vec4_t* dst, const mat4_t* m, const vec4_t* v) {
    vec4_t tmp0, tmp1, tmp2, tmp3;
    Vec4_Scale(&tmp0, &m->rows[ 0 ], v->x);
    Vec4_Scale(&tmp1, &m->rows[ 1 ], v->y);
    Vec4_Scale(&tmp2, &m->rows[ 2 ], v->z);
    Vec4_Scale(&tmp3, &m->rows[ 3 ], v->w);

    Vec4_Add(&tmp0, &tmp0, &tmp1);
    Vec4_Add(&tmp2, &tmp2, &tmp3);
    Vec4_Add(dst, &tmp0, &tmp2);
}

/*==========================================================================================================*/
INLINE void Mat4_Concat(mat4_t* dst, const mat4_t* a, const mat4_t* b) {
    mat4_t tmp;
    Mat4_MulVec4(&tmp.rows[ 0 ], b, &a->rows[ 0 ]);
    Mat4_MulVec4(&tmp.rows[ 1 ], b, &a->rows[ 1 ]);
    Mat4_MulVec4(&tmp.rows[ 2 ], b, &a->rows[ 2 ]);
    Mat4_MulVec4(&tmp.rows[ 3 ], b, &a->rows[ 3 ]);
    Mat4_Copy(dst, &tmp);
}

/*==========================================================================================================*/
INLINE float Mat4_Det(const mat4_t* m) {
    const float* a = &m->rows[ 0 ].x;

    float s0 = a[ 0 ] * a[ 5 ] - a[ 4 ] * a[ 1 ];
    float s1 = a[ 0 ] * a[ 6 ] - a[ 4 ] * a[ 2 ];
    float s2 = a[ 0 ] * a[ 7 ] - a[ 4 ] * a[ 3 ];
    float s3 = a[ 1 ] * a[ 6 ] - a[ 5 ] * a[ 2 ];
    float s4 = a[ 1 ] * a[ 7 ] - a[ 5 ] * a[ 3 ];
    float s5 = a[ 2 ] * a[ 7 ] - a[ 6 ] * a[ 3 ];

    float c5 = a[ 10 ] * a[ 15 ] - a[ 14 ] * a[ 11 ];
    float c4 = a[ 9 ] * a[ 15 ] - a[ 13 ] * a[ 11 ];
    float c3 = a[ 9 ] * a[ 14 ] - a[ 13 ] * a[ 10 ];
    float c2 = a[ 8 ] * a[ 15 ] - a[ 12 ] * a[ 11 ];
    float c1 = a[ 8 ] * a[ 14 ] - a[ 12 ] * a[ 10 ];
    float c0 = a[ 8 ] * a[ 13 ] - a[ 12 ] * a[ 9 ];

    return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
}

/*==========================================================================================================*/
INLINE void Mat4_Inverse(mat4_t* dst, const mat4_t* m) {
    const float* a = &m->rows[ 0 ].x;

    float s0 = a[ 0 ] * a[ 5 ] - a[ 4 ] * a[ 1 ];
    float s1 = a[ 0 ] * a[ 6 ] - a[ 4 ] * a[ 2 ];
    float s2 = a[ 0 ] * a[ 7 ] - a[ 4 ] * a[ 3 ];
    float s3 = a[ 1 ] * a[ 6 ] - a[ 5 ] * a[ 2 ];
    float s4 = a[ 1 ] * a[ 7 ] - a[ 5 ] * a[ 3 ];
    float s5 = a[ 2 ] * a[ 7 ] - a[ 6 ] * a[ 3 ];

    float c5 = a[ 10 ] * a[ 15 ] - a[ 14 ] * a[ 11 ];
    float c4 = a[ 9 ] * a[ 15 ] - a[ 13 ] * a[ 11 ];
    float c3 = a[ 9 ] * a[ 14 ] - a[ 13 ] * a[ 10 ];
    float c2 = a[ 8 ] * a[ 15 ] - a[ 12 ] * a[ 11 ];
    float c1 = a[ 8 ] * a[ 14 ] - a[ 12 ] * a[ 10 ];
    float c0 = a[ 8 ] * a[ 13 ] - a[ 12 ] * a[ 9 ];

    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    if (Scalar_Abs(det) <= XE_EPSILON) {
        Mat4_SetIdentity(dst);
        return;
    }

    float invDet = 1.0f / det;
    mat4_t tmp;

    Vec4_SetXyzw(&tmp.rows[ 0 ],
        ( a[ 5 ] * c5 - a[ 6 ] * c4 + a[ 7 ] * c3) * invDet,
        (-a[ 1 ] * c5 + a[ 2 ] * c4 - a[ 3 ] * c3) * invDet,
        ( a[ 13 ] * s5 - a[ 14 ] * s4 + a[ 15 ] * s3) * invDet,
        (-a[ 9 ] * s5 + a[ 10 ] * s4 - a[ 11 ] * s3) * invDet);

    Vec4_SetXyzw(&tmp.rows[ 1 ],
        (-a[ 4 ] * c5 + a[ 6 ] * c2 - a[ 7 ] * c1) * invDet,
        ( a[ 0 ] * c5 - a[ 2 ] * c2 + a[ 3 ] * c1) * invDet,
        (-a[ 12 ] * s5 + a[ 14 ] * s2 - a[ 15 ] * s1) * invDet,
        ( a[ 8 ] * s5 - a[ 10 ] * s2 + a[ 11 ] * s1) * invDet);

    Vec4_SetXyzw(&tmp.rows[ 2 ],
        ( a[ 4 ] * c4 - a[ 5 ] * c2 + a[ 7 ] * c0) * invDet,
        (-a[ 0 ] * c4 + a[ 1 ] * c2 - a[ 3 ] * c0) * invDet,
        ( a[ 12 ] * s4 - a[ 13 ] * s2 + a[ 15 ] * s0) * invDet,
        (-a[ 8 ] * s4 + a[ 9 ] * s2 - a[ 11 ] * s0) * invDet);

    Vec4_SetXyzw(&tmp.rows[ 3 ],
        (-a[ 4 ] * c3 + a[ 5 ] * c1 - a[ 6 ] * c0) * invDet,
        ( a[ 0 ] * c3 - a[ 1 ] * c1 + a[ 2 ] * c0) * invDet,
        (-a[ 12 ] * s3 + a[ 13 ] * s1 - a[ 14 ] * s0) * invDet,
        ( a[ 8 ] * s3 - a[ 9 ] * s1 + a[ 10 ] * s0) * invDet);

    Mat4_Copy(dst, &tmp);
}

/*==========================================================================================================*/
INLINE void Mat4_SetFromMat3(mat4_t* dst, const mat3_t* r) {
    Vec4_SetXyzw(&dst->rows[ 0 ], r->rows[ 0 ].x, r->rows[ 0 ].y, r->rows[ 0 ].z, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 1 ], r->rows[ 1 ].x, r->rows[ 1 ].y, r->rows[ 1 ].z, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 2 ], r->rows[ 2 ].x, r->rows[ 2 ].y, r->rows[ 2 ].z, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 3 ], 0.0f, 0.0f, 0.0f, 1.0f);
}

/*==========================================================================================================*/
INLINE void Mat4_SetRotationAA(mat4_t* dst, const vec3_t* axis, float angle) {
    mat3_t r;
    Mat3_SetRotationAA(&r, axis, angle);
    Mat4_SetFromMat3(dst, &r);
}

/*==========================================================================================================*/
INLINE void Mat4_SetRotationQ(mat4_t* dst, const quat_t* q) {
    mat3_t r;
    Mat3_SetRotationQ(&r, q);
    Mat4_SetFromMat3(dst, &r);
}

/*==========================================================================================================*/
INLINE void Mat4_SetPerspectiveLH(mat4_t* dst, float fovY, float aspect, float zNear, float zFar) {
    float yScale = 1.0f / Scalar_Tan(fovY * 0.5f);
    float xScale = yScale / aspect;
    float range = zFar / (zFar - zNear);

    Vec4_SetXyzw(&dst->rows[ 0 ], xScale, 0.0f, 0.0f, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 1 ], 0.0f, yScale, 0.0f, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 2 ], 0.0f, 0.0f, range, 1.0f);
    Vec4_SetXyzw(&dst->rows[ 3 ], 0.0f, 0.0f, -zNear * range, 0.0f);
}

/*==========================================================================================================*/
INLINE void Mat4_SetPerspectiveInfiniteLH(mat4_t* dst, float fovY, float aspect, float zNear) {
    float yScale = 1.0f / Scalar_Tan(fovY * 0.5f);
    float xScale = yScale / aspect;

    Vec4_SetXyzw(&dst->rows[ 0 ], xScale, 0.0f, 0.0f, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 1 ], 0.0f, yScale, 0.0f, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 2 ], 0.0f, 0.0f, 1.0f, 1.0f);
    Vec4_SetXyzw(&dst->rows[ 3 ], 0.0f, 0.0f, -zNear, 0.0f);
}

/*==========================================================================================================*/
INLINE void Mat4_SetLookAtLH(mat4_t* dst, const vec3_t* eye, const vec3_t* target, const vec3_t* up) {
    vec3_t xAxis, yAxis, zAxis;

    Vec3_Sub(&zAxis, target, eye);
    Vec3_Normalize(&zAxis);
    Vec3_Cross(&xAxis, up, &zAxis);
    Vec3_Normalize(&xAxis);
    Vec3_Cross(&yAxis, &zAxis, &xAxis);

    Vec4_SetXyzw(&dst->rows[ 0 ], xAxis.x, yAxis.x, zAxis.x, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 1 ], xAxis.y, yAxis.y, zAxis.y, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 2 ], xAxis.z, yAxis.z, zAxis.z, 0.0f);
    Vec4_SetXyzw(&dst->rows[ 3 ], -Vec3_Dot(&xAxis, eye), -Vec3_Dot(&yAxis, eye), -Vec3_Dot(&zAxis, eye), 1.0f);
}

#endif // __MATH_LINEAR_H__
