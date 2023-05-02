//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#ifndef __CC_MATHSCALAR_H__
#define __CC_MATHSCALAR_H__

#include "core/Common.h"

namespace math {
    
    static const float kPI = 3.14159265359f;
    
    inline float DegToRad( float rhs ) { return rhs * (kPI / 180.0f); }
    inline float RadToDeg( float rhs ) { return rhs * (180.0f / kPI); }

	inline float Min( float lhs, float rhs ) { return fminf(lhs, rhs ); }
	inline float Max( float lhs, float rhs ) { return fmaxf(lhs, rhs ); }

	//======================================================================================================================
	inline float Abs( float rhs ) {
		return fabsf( rhs );
	}

	//======================================================================================================================
	inline bool IsValid( float rhs ) {
		return ( rhs == rhs );
	}
	//======================================================================================================================
	inline float Clamp( float val, float minVal, float maxVal ) {
		return Max( minVal, Min( maxVal, val ) );
	}

	//======================================================================================================================
	inline float Mix( float from, float to, float t ) {
		return ( from * ( 1 - t ) ) + ( to * t );
	}

	//======================================================================================================================
	inline float MixSafe( float from, float to, float t ) {
		float safeT = Clamp( t, 0, 1 );
		return Mix( from, to, safeT );
	}

	inline float Sqrt( float rhs ) { return sqrtf( rhs ); }
	inline float Cos( float rhs ) { return cosf( rhs ); }
	inline float Sin( float rhs ) { return sinf( rhs ); }
	inline float Tan( float rhs ) { return tanf( rhs ); }
	inline float ACos( float rhs ) { return acosf( rhs ); }
	inline float ASin( float rhs ) { return asinf( rhs ); }
	inline float ATan( float rhs ) { return atanf( rhs ); }
	inline float ATan2( float lhs, float rhs ) { return atan2f( lhs, rhs ); }
}

#endif

