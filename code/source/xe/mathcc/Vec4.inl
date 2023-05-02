//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __CC_VEC4_INL__
#define __CC_VEC4_INL__

namespace math {
    
    //======================================================================================================================
    inline Vec4::Vec4() {
        // Empty - does nothing
    }

    //======================================================================================================================
    inline Vec4::Vec4( const Vec4 & rhs ) {
        m_x = rhs.m_x;
        m_y = rhs.m_y;
        m_z = rhs.m_z;
        m_w = rhs.m_w;
    }

    //======================================================================================================================
    inline Vec4::Vec4( const Vec3& rhs, float w ) {
        Set( rhs, w );
    }

    //======================================================================================================================
    inline Vec4::Vec4( float x, float y, float z, float w ) {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }

    //======================================================================================================================
    inline float Vec4::X() const { return m_x; }

    //======================================================================================================================
    inline float Vec4::Y() const { return m_y; }

    //======================================================================================================================
    inline float Vec4::Z() const { return m_z; }

    //======================================================================================================================
    inline float Vec4::W() const { return m_w; }

    //======================================================================================================================
    inline float& Vec4::X() { return m_x; }

    //======================================================================================================================
    inline float& Vec4::Y() { return m_y; }

    //======================================================================================================================
    inline float& Vec4::Z() { return m_z; }

    //======================================================================================================================
    inline float& Vec4::W() { return m_w; }

    //======================================================================================================================
    inline void Vec4::Set( float x, float y, float z, float w ) {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }

    //======================================================================================================================
    inline void Vec4::Set( const Vec3 & v, float w ) {
        m_x = v.X();
        m_y = v.Y();
        m_z = v.Z();
        m_w = w;
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator-() const {
        return Vec4(-m_x, -m_y, -m_z, -m_w);
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator=( const Vec4 & rhs ) {
        m_x = rhs.m_x;
        m_y = rhs.m_y;
        m_z = rhs.m_z;
        m_w = rhs.m_w;
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator=( const Vec3& rhs ) {
        m_x = rhs.X();
        m_y = rhs.Y();
        m_z = rhs.Z();
        m_w = 0;
        return *this;
    }

    //======================================================================================================================
    inline float Vec4::Magnitude() const {
        return math::Sqrt( Dot( *this ) );
    }

    //======================================================================================================================
    inline float Vec4::Normalise( const Vec4 & rhs ) {
        float mag = rhs.Magnitude();
        float fac = 1.0f / mag;
    
        *this *= fac;
        return mag;
    }

    //======================================================================================================================
    inline float Vec4::Dot( const Vec4 & rhs ) const {
        return (m_x * rhs.m_x) + (m_y * rhs.m_y) + (m_z * rhs.m_z) + (m_w * rhs.m_w);
    }

    //======================================================================================================================
    inline void Vec4::Mix(const Vec4 & from, const Vec4 & to, float t) {
        float tClamped = math::Clamp( t, 0, 1 );
    
        *this = (from * ( 1.0f - tClamped ) ) + ( to * tClamped ) ;
    }

    //======================================================================================================================
    inline void Vec4::Min( const Vec4 & lhs, const Vec4 & rhs ) {
        m_x = math::Min( lhs.m_x, rhs.m_x );
        m_y = math::Min( lhs.m_y, rhs.m_y );
        m_z = math::Min( lhs.m_z, rhs.m_z );
        m_w = math::Min( lhs.m_w, rhs.m_w );
    }

    //======================================================================================================================
    inline void Vec4::Max(const Vec4 & lhs, const Vec4 & rhs) {
        m_x = math::Max( lhs.m_x, rhs.m_x );
        m_y = math::Max( lhs.m_y, rhs.m_y );
        m_z = math::Max( lhs.m_z, rhs.m_z );
        m_w = math::Max( lhs.m_w, rhs.m_w );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator+( const Vec4 & rhs ) const {
        return Vec4( m_x + rhs.m_x,
                        m_y + rhs.m_y,
                        m_z + rhs.m_z,
                        m_w + rhs.m_w );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator-( const Vec4 & rhs ) const {
        return Vec4( m_x - rhs.m_x,
                        m_y - rhs.m_y,
                        m_z - rhs.m_z,
                        m_w - rhs.m_w );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator*( const Vec4 & rhs ) const {
        return Vec4( m_x * rhs.m_x,
                        m_y * rhs.m_y,
                        m_z * rhs.m_z,
                        m_w * rhs.m_w );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator/( const Vec4 & rhs ) const {
        return Vec4( m_x / rhs.m_x,
                        m_y / rhs.m_y,
                        m_z / rhs.m_z,
                        m_w / rhs.m_w );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator+( float rhs ) const {
        return *this + Vec4( rhs, rhs, rhs, rhs );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator-( float rhs ) const {
        return *this - Vec4( rhs, rhs, rhs, rhs );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator*( float rhs ) const {
        return *this * Vec4( rhs, rhs, rhs, rhs );
    }

    //======================================================================================================================
    inline Vec4 Vec4::operator/( float rhs ) const {
        return *this / Vec4( rhs, rhs, rhs, rhs );
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator+=( const Vec4 & rhs ) {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        m_z += rhs.m_z;
        m_w += rhs.m_w;
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator-=( const Vec4 & rhs ) {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        m_z -= rhs.m_z;
        m_w -= rhs.m_w;
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator*=( const Vec4 & rhs ) {
        m_x *= rhs.m_x;
        m_y *= rhs.m_y;
        m_z *= rhs.m_z;
        m_w *= rhs.m_w;
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator/=( const Vec4 & rhs ) {
        m_x /= rhs.m_x;
        m_y /= rhs.m_y;
        m_z /= rhs.m_z;
        m_w /= rhs.m_w;
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator+=( float rhs ) {
        *this += Vec4( rhs, rhs, rhs, rhs );
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator-=( float rhs ) {
        *this -= Vec4( rhs, rhs, rhs, rhs );
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator*=( float rhs ) {
        *this *= Vec4( rhs, rhs, rhs, rhs );
        return *this;
    }

    //======================================================================================================================
    inline Vec4 & Vec4::operator/=( float rhs ) {
        *this /= Vec4( rhs, rhs, rhs, rhs );
        return *this;
    }
}

#endif
