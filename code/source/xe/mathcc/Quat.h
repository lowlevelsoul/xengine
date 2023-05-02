//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#ifndef __CC_QUAT_H__
#define __CC_QUAT_H__

#include "core/Common.h"
#include "mathcc/MathScalar.h"
#include "mathcc/Vec3.h"

namespace math {
    
    class Mat3;
    class Mat4;

	class Quat {
	public:
		static const Quat IDENTITY;

		Quat();

		Quat( float x, float y, float z, float w );

		Quat( const Vec3 & axis, float anglerad );

		Quat( const Quat & rhs );

		void Set( float x, float y, float z, float w );
        
        void Set( const Vec3 & axis, float angleRad );
        
        void Set( float x_, float y_, float z_ );
        
        void SetFromMatrix( const Mat4 & rhs );
        
        void SetFromMatrix( const Mat3 & rhs );

		float X() const { return m_x; }
		float Y() const { return m_y; }
		float Z() const { return m_z; }
		float W() const { return m_w; }

		float & X() { return m_x; }
		float & Y() { return m_y; }
		float & Z() { return m_z; }
		float & W() { return m_w; }

		float Magnitude() const;

		float Dot( const Quat & rhs ) const;

		float Normalise( const Quat & rhs );
                
		void Conjugate( const Quat & rhs );

		void Conjugate();
        
		void Inverse( const Quat & rhs );
        
		void Inverse();

		Quat & Concat( const Quat & lhs, const Quat & rhs );
        
        Quat & Slerp( const Quat & from, const Quat & to, float t );
    
		Quat operator-() const;
    
		Quat & operator=( const Quat & rhs );
    
		Quat operator*( const Quat & rhs ) const;

		Quat & operator*=( const Quat & rhs );

		Quat operator*( float rhs ) const;

		Quat & operator*=( float rhs );
        
		Quat operator / ( const float & rhs ) const;

	protected:
		float		m_x;
		float		m_y;
		float		m_z;
		float		m_w;
	};
}


#endif
