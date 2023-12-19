//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#ifndef __CC_MAT3_H__
#define __CC_MAT3_H__

#include "core/Platform.h"
#include "mathcc/MathScalar.h"

namespace math {

	class Quat;

	class Mat3 {
	public:
		static constexpr size_t ROW_COUNT = 3;
		static const Mat3 IDENTITY;

		Mat3();

		Mat3( float r0, float r1, float r2 );

		Mat3( const Mat3 & rhs );

		Mat3( const Vec3 & r0, const Vec3 & r1, const Vec3 & r2 );

		void SetDiagonal( float r0, float r1, float r2 );

		void SetRotation( const Vec3 & axis, float angleRad );

		void SetRotation( const Quat & rot );

		void Concat( const Mat3 & lhs, const Mat3 & rhs );

		void Transform( Vec3 & dst, const Vec3 & src ) const;

		void Inverse( const Mat3 & rhs );
        
        Vec3 operator*( const Vec3 & p ) const {
            Vec3 r = m_rows[0] * p.X() +
                     m_rows[1] * p.Y() +
                     m_rows[2] * p.Z();
            
            return r;
        }

	public:
		Vec3		m_rows[ ROW_COUNT ];
	};
}


#endif
