//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __CC_VECTOR4_H__
#define __CC_VECTOR4_H__

namespace math {

    class Vec4 {
    public:
        static const Vec4 UNIT_X;
        static const Vec4 UNIT_Y;
        static const Vec4 UNIT_Z;
        static const Vec4 UNIT_W;
        static const Vec4 ZERO;
    
        Vec4();
    
        Vec4(const Vec4& rhs);
    
        Vec4(const Vec3& rhs, float w = 0);
    
        Vec4( float x, float y, float z, float w );
    
        float X() const;
        float Y() const;
        float Z() const;
        float W() const;
    
        float& X();
        float& Y();
        float& Z();
        float& W();
    
        void Set( float x, float y, float z, float w );
    
        void Set(const Vec4& rhs);
    
        void Set(const Vec3& rhs, float w = 1);
    
        Vec4 operator-() const;
    
        Vec4& operator=(const Vec4& rhs);
    
        Vec4& operator=(const Vec3& rhs);
    
        float Magnitude() const;
    
        float Normalise(const Vec4& rhs);
    
        float Dot(const Vec4& rhs) const;
    
        void Mix(const Vec4& from, const Vec4& to, float t);
    
        void Min(const Vec4& lhs, const Vec4& rhs);
    
        void Max(const Vec4& lhs, const Vec4& rhs);
    
        Vec4 operator+( const Vec4 & rhs ) const;
        Vec4 operator-( const Vec4 & rhs ) const;
        Vec4 operator*( const Vec4 & rhs ) const;
        Vec4 operator/( const Vec4 & rhs ) const;
        Vec4 operator+( float rhs ) const;
        Vec4 operator-( float rhs ) const;
        Vec4 operator*( float rhs ) const;
        Vec4 operator/( float rhs ) const;
    
        Vec4 & operator+=( const Vec4 & rhs );
        Vec4 & operator-=( const Vec4 & rhs );
        Vec4 & operator*=( const Vec4 & rhs );
        Vec4 & operator/=( const Vec4 & rhs );
        Vec4 & operator+=( float rhs );
        Vec4 & operator-=( float rhs );
        Vec4 & operator*=( float rhs );
        Vec4 & operator/=( float rhs );
    
    protected:
        float m_x;
        float m_y;
        float m_z;
        float m_w;
    };
}

#endif
