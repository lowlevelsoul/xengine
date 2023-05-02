//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#ifndef __CC_MAT3_INL__
#define __CC_MAT3_INL__

namespace math {

	//========================================================================================================================
	inline Mat3::Mat3() {

	}

	inline Mat3::Mat3( float r0, float r1, float r2 ) {
		SetDiagonal( r0, r1, r2 );
	}

	inline Mat3::Mat3( const Mat3 & rhs ) {
		m_rows[ 0 ] = rhs.m_rows[ 0 ];
		m_rows[ 1 ] = rhs.m_rows[ 1 ];
		m_rows[ 2 ] = rhs.m_rows[ 2 ];
	}

	inline Mat3::Mat3( const Vec3 & r0, const Vec3 & r1, const Vec3 & r2 ) {
		m_rows[ 0 ] = r0;
		m_rows[ 1 ] = r1;
		m_rows[ 2 ] = r2;
	}

	inline void Mat3::SetDiagonal( float r0, float r1, float r2 ) {
		m_rows[ 0 ].Set( r0, 0, 0 );
		m_rows[ 1 ].Set( 0, r1, 0 );
		m_rows[ 2 ].Set( 0, 0, r2 );
	}
}


#endif
