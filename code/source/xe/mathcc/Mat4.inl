//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __CC_MATRIX4_INL__
#define __CC_MATRIX4_INL__

namespace math {

    //======================================================================================================================
    inline Mat4::Mat4() {
        // Empty - does nothing
    }

    //======================================================================================================================
    inline Mat4::Mat4(const Mat4& rhs) {
        m_rows[ 0 ] = rhs.m_rows[ 0 ];
        m_rows[ 1 ] = rhs.m_rows[ 1 ];
        m_rows[ 2 ] = rhs.m_rows[ 2 ];
        m_rows[ 3 ] = rhs.m_rows[ 3 ];
    }

    //======================================================================================================================
    inline Mat4::Mat4(const Vec4& right, const Vec4& up, const Vec4& at, const Vec4& pos) {
        m_rows[ 0 ] = right;
        m_rows[ 1 ] = up;
        m_rows[ 2 ] = at;
        m_rows[ 3 ] = pos;
    }

    //======================================================================================================================
    inline Mat4::Mat4(float right, float up, float at, float pos) {
        m_rows[ 0 ].Set(right, 0, 0, 0);
        m_rows[ 1 ].Set(0, up, 0, 0);
        m_rows[ 2 ].Set(0, 0, at, 0);
        m_rows[ 3 ].Set(0, 0, 0, pos);
    }

    //======================================================================================================================
    inline void Mat4::Set(const Vec4& right, const Vec4& up, const Vec4& at, const Vec4& pos) {
        m_rows[ 0 ] = right;
        m_rows[ 1 ] = up;
        m_rows[ 2 ] = at;
        m_rows[ 3 ] = pos;
    }

    //======================================================================================================================
    inline void Mat4::Set(float right, float up, float at, float pos) {
        m_rows[ 0 ].Set(right, 0, 0, 0);
        m_rows[ 1 ].Set(0, up, 0, 0);
        m_rows[ 2 ].Set(0, 0, at, 0);
        m_rows[ 3 ].Set(0, 0, 0, pos);
    }

    //======================================================================================================================
    inline void Mat4::Concat(const Mat4& lhs, const Mat4& rhs) {
        m_rows[ 0 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 0 ].X() +
                    rhs.m_rows[ 1 ] * lhs.m_rows[ 0 ].Y() +
                    rhs.m_rows[ 2 ] * lhs.m_rows[ 0 ].Z() +
                    rhs.m_rows[ 3 ] * lhs.m_rows[ 0 ].W();
    
        m_rows[ 1 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 1 ].X() +
                    rhs.m_rows[ 1 ] * lhs.m_rows[ 1 ].Y() +
                    rhs.m_rows[ 2 ] * lhs.m_rows[ 1 ].Z() +
                    rhs.m_rows[ 3 ] * lhs.m_rows[ 1 ].W();
    
        m_rows[ 2 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 2 ].X() +
                    rhs.m_rows[ 1 ] * lhs.m_rows[ 2 ].Y() +
                    rhs.m_rows[ 2 ] * lhs.m_rows[ 2 ].Z() +
                    rhs.m_rows[ 3 ] * lhs.m_rows[ 2 ].W();
    
        m_rows[ 3 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 3 ].X() +
                    rhs.m_rows[ 1 ] * lhs.m_rows[ 3 ].Y() +
                    rhs.m_rows[ 2 ] * lhs.m_rows[ 3 ].Z() +
                    rhs.m_rows[ 3 ] * lhs.m_rows[ 3 ].W();
    }

    //======================================================================================================================
    inline void Mat4::Transform(Vec4& dst, const Vec4& src) const {
        dst =   m_rows[ 0 ] * src.X() +
                m_rows[ 1 ] * src.Y() +
                m_rows[ 2 ] * src.Z() +
                m_rows[ 3 ] * src.W();
    }

    //======================================================================================================================
    inline void Mat4::Transform(Vec3& dst, const Vec3& src) const {
        Vec4 tmp =   m_rows[ 0 ] * src.X() +
                m_rows[ 1 ] * src.Y() +
                m_rows[ 2 ] * src.Z() +
                m_rows[ 3 ];
    
        dst.Set(tmp.X(), tmp.Y(), tmp.Z());
    }

    //======================================================================================================================
    inline void Mat4::TransformVec(Vec3& dst, const Vec3& src) const {
        Vec4 tmp =  m_rows[ 0 ] * src.X() +
                    m_rows[ 1 ] * src.Y() +
                    m_rows[ 2 ] * src.Z();
    
        dst.Set(tmp.X(), tmp.Y(), tmp.Z());
    }

    //======================================================================================================================
    inline void Mat4::Transform3(Vec3& dst, const Vec3& src) const {
        Vec4 tmp =   m_rows[ 0 ] * src.X() +
                m_rows[ 1 ] * src.Y() +
                m_rows[ 2 ] * src.Z();
    
        dst.Set(tmp.X(), tmp.Y(), tmp.Z());
    }


    //======================================================================================================================
    inline void Mat4::Transpose( const Mat4 & rhs ) {
        m_rows[ 0 ].Set ( rhs.m_rows[ 0 ].X(),   rhs.m_rows[ 1 ].X(),   rhs.m_rows[ 2 ].X(),   rhs.m_rows[ 3 ].X() );
        m_rows[ 1 ].Set ( rhs.m_rows[ 0 ].Y(),   rhs.m_rows[ 1 ].Y(),   rhs.m_rows[ 2 ].Y(),   rhs.m_rows[ 3 ].Y() );
        m_rows[ 2 ].Set ( rhs.m_rows[ 0 ].Z(),   rhs.m_rows[ 1 ].Z(),   rhs.m_rows[ 2 ].Z(),   rhs.m_rows[ 3 ].Z() );
        m_rows[ 3 ].Set ( rhs.m_rows[ 0 ].W(),   rhs.m_rows[ 1 ].W(),   rhs.m_rows[ 2 ].W(),   rhs.m_rows[ 3 ].W() );
    }

    //======================================================================================================================
    inline void Mat4::SetTranslation( const Vec3& trans ) {
        m_rows[ 3 ].Set(trans, 1);
    }

    //======================================================================================================================
    inline Mat4& Mat4::operator=(const Mat4& rhs) {
        m_rows[ 0 ] = rhs.m_rows[ 0 ];
        m_rows[ 1 ] = rhs.m_rows[ 1 ];
        m_rows[ 2 ] = rhs.m_rows[ 2 ];
        m_rows[ 3 ] = rhs.m_rows[ 3 ];
        return *this;
    }

    //======================================================================================================================
    inline Mat4 Mat4::operator*(const Mat4& rhs) const {
        Mat4 res;
        res.Concat(*this, rhs);
        return res;
    }
    
    //======================================================================================================================
    inline Mat4 & Mat4::operator*=(const Mat4& rhs) {
        Mat4 tmp;
        tmp.Concat( *this, rhs);
        *this = tmp;
        return *this;
    }

    //======================================================================================================================
    inline Vec4 Mat4::operator*(const Vec4& rhs) const {
        Vec4 tmp;
        Transform(tmp, rhs);
        return tmp;
    }

    //======================================================================================================================
    inline Vec3 Mat4::operator*(const Vec3& rhs) const {
        Vec4 rhs4;
        rhs4.Set(rhs, 1);
    
        Vec4 tmp;
        Transform(tmp, rhs4);
    
        Vec3 res(tmp.X(), tmp.Y(), tmp.Z());
    
        return res;
    }
}

#endif
