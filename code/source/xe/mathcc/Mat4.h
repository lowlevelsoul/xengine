//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 James Steele. All Rights Reserved.
//======================================================================================================================

#ifndef __CC_MAT4_H__
#define __CC_MAT4_H__

namespace math {

    class Mat4 {
    public:
        static const size_t ROW_COUNT = 4;
        static const Mat4 IDENTITY;

        Mat4();

        Mat4( const Mat4 & rhs );

        Mat4( const Vec4 & right, const Vec4 & up, const Vec4 & at, const Vec4 & pos );

        Mat4( float  right, float  up, float  at, float  pos );

        void Set( const Vec4 & right, const Vec4 & up, const Vec4 & at, const Vec4 & pos );

        void Set( float  right, float  up, float  at, float  pos );

        void Concat( const Mat4 & lhs, const Mat4 & rhs );

        void Transform( Vec4 & dst, const Vec4 & src ) const;

        void Transform( Vec3 & dst, const Vec3 & src ) const;

        void TransformVec( Vec3 & dst, const Vec3 & src ) const;

        void Transform3( Vec3 & dst, const Vec3 & src ) const;

        void Inverse( const Mat4 & rhs );
    
        void Transpose( const Mat4 & rhs );

        void SetRotationAA( const Vec3 & axis, float  angle );

        void SetRotationQ( const Quat & rhs );

        /// Set this matrix to represent a left-handed perspective projection
        ///@param fov The vertical filed of view defined in radians
        ///@param aspect The vertocal aspect ratioj
        ///@param nearClip The distance of the near clip plane from the viewer
        ///@param farClip The distance of the far clip plane from the viewer
        void SetProjectionPerspLH(float  fov, float  aspect, float  nearClip, float  farClip);

        void SetProjectionPerspLHInf(float  fov, float  aspect, float  nearClip, float  farClip);

        /// Set this matrxi to represent a left-handed orthographic projection
        ///@param width The horizontal scale
        ///@param height The horizontal scale
        ///@param nearClip The distance from the viewer to the near clip plane
        ///@param farClip The distance from the viewer to the far clip plane
        void SetProjectionOrthoLH(float  width, float  height, float  nearClip, float  farClip);

        /// Set this matrix to represent a 3d-transfrom that gives as a view matrix from a position and a target
        ///@param eye The location of the viewer
        ///@param lookAt The point the view is looking at
        ///@param up Natural up vector
        void SetLookAt(const Vec3& eye, const Vec3& lookAt, const Vec3& up);
    
        void SetTranslation( const Vec3& trans );
        
        Mat4 GetInverse() const {
            Mat4 tmp;
            tmp.Inverse( *this );
            return tmp;
        }

        Mat4& operator=(const Mat4& rhs);

        Mat4 operator*(const Mat4& rhs) const;
        
        Mat4 & operator*=(const Mat4& rhs);

        Vec4 operator*(const Vec4& rhs) const;

        Vec3 operator*(const Vec3& rhs) const;
    
    public:
        Vec4         m_rows[ ROW_COUNT ];
    };
}

#endif
