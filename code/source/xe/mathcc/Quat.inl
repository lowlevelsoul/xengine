//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#ifndef __CC_QUAT_INL__
#define __CC_QUAT_INL__

namespace math {

	//======================================================================================================================
	inline Quat::Quat() {

	}

	//======================================================================================================================
	inline Quat::Quat( float x, float y, float z, float w ) {
		Set( x, y, z, w );
	}

	//======================================================================================================================
	inline Quat::Quat( const Vec3 & axis, float anglerad ) {
		// TODO
		assert( 0 );
	}

	//======================================================================================================================
	inline Quat::Quat( const Quat & rhs ) {
		m_x = rhs.m_x;
		m_y = rhs.m_y;
		m_z = rhs.m_z;
		m_w = rhs.m_w;
	}

	//======================================================================================================================
	inline void Quat::Set( float x, float y, float z, float w ) {
		m_x = x;
		m_y = y;
		m_z = z;
		m_w = w;
	}
    
    //======================================================================================================================
    inline void Quat::Set( const Vec3 & axis, float angleRads ) {
        float c = math::Cos( angleRads / 2.0f );
        float s = math::Sin( angleRads / 2.0f );

        m_x = axis.X() * s;
        m_y = axis.Y() * s;
        m_z = axis.Z() * s;
        m_w = c;
    }
    
    //======================================================================================================================
    inline void Quat::Set( float x_, float y_, float z_ ) {
        float t = 1.0f - (x_ * x_) - (y_ * y_) - (z_ * z_);
        
        if (t < 0.0f) {
            m_w = 0.0f;
        } else {
            m_w = - math::Sqrt( t );
        }
        
        
        m_x = x_;
        m_y = y_;
        m_z = z_;
    }

	//======================================================================================================================
	inline float Quat::Magnitude() const {
		return math::Sqrt( Dot( *this ) );
	}

	//======================================================================================================================
	inline float Quat::Dot( const Quat & rhs ) const {
		return ( m_x * rhs.m_x ) + ( m_y * rhs.m_y ) + ( m_z * rhs.m_z ) + ( m_w * rhs.m_w );
	}

	//======================================================================================================================
	inline float Quat::Normalise( const Quat & rhs ) {
		float mag = Magnitude();
		float fac = 1.0f / mag;
		assert( math::IsValid( fac ) );
		*this *= fac;
		return mag;
	}
         
	//======================================================================================================================
    inline void Quat::Conjugate( const Quat & rhs ) {
        Set( -rhs.X(), -rhs.Y(), -rhs.Z(), rhs.W() );
    }
        
	//======================================================================================================================
    inline void Quat:: Conjugate() {
        Conjugate( * this );
    }
        
	//======================================================================================================================
	inline void Quat::Inverse( const Quat & rhs ) {
		m_x = rhs.X();
		m_y = rhs.Y();
		m_z = rhs.Z();
		m_w = -rhs.Z();
	}
        
	//======================================================================================================================
	inline void Quat::Inverse() {
		Inverse( *this );
	}

	//======================================================================================================================
	inline Quat Quat::operator-() const {
		Quat res;
		res.Inverse( res );
		return res;
	}
    
	//======================================================================================================================
	inline Quat & Quat::operator=( const Quat & rhs ) {
		m_x = rhs.m_x;
		m_y = rhs.m_y;
		m_z = rhs.m_z;
		m_w = rhs.m_w;
		return *this;
	}
    
	//======================================================================================================================
	inline Quat Quat::operator*( const Quat & rhs ) const {
		Quat res;
		res.Concat( *this, rhs );
		return res;	
	}

	//======================================================================================================================
	inline Quat & Quat::operator*=( const Quat & rhs ) {
		Quat tmp = *this;
		Concat( tmp, rhs );
		return *this;
	}

	//======================================================================================================================
	inline Quat Quat::operator*( float rhs ) const {
		return Quat( m_x * rhs, m_y * rhs, m_z * rhs, m_w * rhs );
	}

	//======================================================================================================================
	inline Quat & Quat::operator*=( float rhs ) {
		m_x *= rhs;
		m_y *= rhs;
		m_z *= rhs;
		m_w *= rhs;
		return * this;
	}
        
	//======================================================================================================================
    inline Quat Quat::operator / ( const float & rhs ) const {
        return Quat( m_x / rhs, m_y / rhs, m_z / rhs, m_w / rhs );
    }
}

#endif
