//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __CC_VEC3_H__
#define __CC_VEC3_H__

namespace math {
    
    class Vec4;

    class Vec3 {
    public:
        static const Vec3 UNIT_X;
        static const Vec3 UNIT_Y;
        static const Vec3 UNIT_Z;
        static const Vec3 ZERO;
    
        Vec3();
    
        Vec3( const Vec3 & rhs );
    
        Vec3( const Vec4 & rhs );
    
        Vec3( float x, float y, float z );
    
        float X() const;
        float Y() const;
        float Z() const;
    
        float & X();
        float & Y();
        float & Z();
    
        void Set( float x, float y, float z );
    
        void Set( const Vec4 & rhs );
    
        float Magnitude() const;
    
        float Normalise();
    
        float Normalise( const Vec3 & rhs );
    
        float Dot( const Vec3 & rhs ) const;
    
        void Cross( const Vec3 & lhs, const Vec3 & rhs );
    
        void Mix( const Vec3 & from, const Vec3 & to, float t );
    
        void Min( const Vec3 & lhs, const Vec3 & rhs);
    
        void Max( const Vec3 & lhs, const Vec3 & rhs);
    
        Vec3 operator - () const;
    
        Vec3 & operator = ( const Vec3& rhs );
    
        Vec3 & operator = ( const Vec4& rhs );
    
        Vec3 operator + ( const Vec3& rhs ) const;
        Vec3 operator - ( const Vec3& rhs ) const;
        Vec3 operator * ( const Vec3& rhs ) const;
        Vec3 operator / ( const Vec3& rhs ) const;
        Vec3 operator + ( float rhs ) const;
        Vec3 operator - ( float rhs ) const;
        Vec3 operator * ( float rhs ) const;
        Vec3 operator / ( float rhs ) const;
    
        Vec3 & operator += ( const Vec3 & rhs );
        Vec3 & operator -= ( const Vec3 & rhs );
        Vec3 & operator *= ( const Vec3 & rhs );
        Vec3 & operator /= ( const Vec3 & rhs );
        Vec3 & operator += ( float rhs );
        Vec3 & operator -= ( float rhs );
        Vec3 & operator *= ( float rhs );
        Vec3 & operator /= ( float rhs );
    
    protected:
        float m_x;
        float m_y;
        float m_z;
        float m_pad;
    };
}

#endif
