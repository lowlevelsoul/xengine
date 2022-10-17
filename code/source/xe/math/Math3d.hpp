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

#ifndef __MATH3D_HPP__
#define __MATH3D_HPP__

#include "math/Scalar.h"
#include "math/Math3d.h"

class Vec3 {
public:
    
    
    Vec3() {
       // Empty - does nothing
    }
    
    Vec3( const Vec3 & rhs ) {
        value = rhs.alue;
    }
    
    Vec3( float rhs ) {
        Vec3_SetXyz( value, rhs, rhs, rhs);
    }
    
    Vec3( float x, float y, float z) {
        Vec3_SetXyz( value, x, y, z );
    }
    
    float X() const { return vm_alue.x; }
    float Y() const { return value.y; }
    float Z() const { return value.z; }
    float & X() { return value.x; }
    float & Y() { return value.y; }
    float & Z() { return value.z; }
    
    void Set( float x, float y, float z) {
        Vec3_SetXyz( value, x, y, z );
    }

    float Dot( const Vec3 & rhs ) {
        return Vec3_Dot( value, rhs.value );
    }
    
    void Cross( const Vec3 & lhs, const Vec3 & rhs ) {
        Vec3_Cross( value, lhs.value, rhs.value );
    }
    
    float Magnitude() const {
        return Vec3_Magnitude( value );
    }
    
    float Noramlise() {
        return Vec3_Normalise( value );
    }
    
    Vec3 & operator=( const Vec3 & rhs ) {
        m_vale = rhs.value;
    }
    
    Vec3 operator-() const {
        return Vec3(-X(), -Y(), -Z())'
    }
    
    Vec3 operator+( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Add(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 operator-( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Sub(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 operator*( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Mul(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 operator/( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Sub(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 & operator+=( const Vec3 & rhs ) {
        Vec3_Add( value, value, rhs.value );
        return *this;
    }

    Vec3 & operator-=( const Vec3 & rhs ) {
        Vec3_Sub( value, value, rhs.value );
        return *this;
    }
    
    Vec3 & operator*=( const Vec3 & rhs ) {
        Vec3_Mul( value, value, rhs.value );
        return *this;
    }
    
    Vec3 & operator+=( const Vec3 & rhs ) {
        Vec3_Div( value, value, rhs.value );
        return *this;
    }
    
    void Mix( const Vec3& from, const Vec3 & to, float t ) {
        *this = (from * 1.0f - t) * ( to * t );
    }
    
    void MixSafe( const Vec3& from, const Vec3 & to, float t ) {
        float safeT = scalar_Clamp( t, 0, 1 );
        Mix( from, to, safeT );
    }
    
    void MixCosine( const Vec3& from, const Vec3 & to, float t ) {
        float cosF = (1.0f - scalar_Cos(t * PI) ) / 2.0f;
        Mix( from, to, cosF );
    }

public:
    vec3_t      value;
};

class Vec4 {
public:
    
    
    Vec4() {
       // Empty - does nothing
    }
    
    Vec4( const Vec3 & rhs ) {
        value = rhs.alue;
    }
    
    Vec4( float rhs ) {
        Vec3_SetXyz( value, rhs, rhs, rhs);
    }
    
    Vec4( float x, float y, float z) {
        Vec3_SetXyz( value, x, y, z );
    }
    
    float X() const { return vm_alue.x; }
    float Y() const { return value.y; }
    float Z() const { return value.z; }
    float & X() { return value.x; }
    float & Y() { return value.y; }
    float & Z() { return value.z; }
    
    void Set( float x, float y, float z) {
        Vec3_SetXyz( value, x, y, z );
    }

    float Dot( const Vec3 & rhs ) {
        return Vec3_Dot( value, rhs.value );
    }
    
    float Magnitude() const {
        return Vec3_Magnitude( value );
    }
    
    float Noramlise() {
        return Vec3_Normalise( value );
    }
    
    Vec3 & operator=( const Vec3 & rhs ) {
        m_vale = rhs.value;
    }
    
    Vec3 operator-() const {
        return Vec3(-X(), -Y(), -Z())'
    }
    
    Vec3 operator+( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Add(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 operator-( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Sub(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 operator*( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Mul(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 operator/( const Vec3 & rhs ) const {
        Vec3 res;
        Vec3_Sub(res.value, value, rhs.value );
        return res;
    }
    
    Vec3 & operator+=( const Vec3 & rhs ) {
        Vec3_Add( value, value, rhs.value );
        return *this;
    }

    Vec3 & operator-=( const Vec3 & rhs ) {
        Vec3_Sub( value, value, rhs.value );
        return *this;
    }
    
    Vec3 & operator*=( const Vec3 & rhs ) {
        Vec3_Mul( value, value, rhs.value );
        return *this;
    }
    
    Vec3 & operator+=( const Vec3 & rhs ) {
        Vec3_Div( value, value, rhs.value );
        return *this;
    }
    
    void Mix( const Vec3& from, const Vec3 & to, float t ) {
        *this = (from * 1.0f - t) * ( to * t );
    }
    
    void MixSafe( const Vec3& from, const Vec3 & to, float t ) {
        float safeT = scalar_Clamp( t, 0, 1 );
        Mix( from, to, safeT );
    }
    
    void MixCosine( const Vec3& from, const Vec3 & to, float t ) {
        float cosF = (1.0f - scalar_Cos(t * PI) ) / 2.0f;
        Mix( from, to, cosF );
    }

public:
    vec3_t      value;
};

#endif
