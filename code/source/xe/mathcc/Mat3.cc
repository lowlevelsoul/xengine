//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021, 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#include <cmath>
#include "mathcc/Math3d.h"

namespace math {

    #define calcCofactor(a, b, c, d) ((a * d) - (b * c))

    #define a0 rhs.m_rows[ 0 ].X()
    #define a1 rhs.m_rows[ 0 ].Y()
    #define a2 rhs.m_rows[ 0 ].Z()
    #define a3 rhs.m_rows[ 1 ].X()
    #define a4 rhs.m_rows[ 1 ].Y()
    #define a5 rhs.m_rows[ 1 ].Z()
    #define a6 rhs.m_rows[ 2 ].X()
    #define a7 rhs.m_rows[ 2 ].Y()
    #define a8 rhs.m_rows[ 2 ].Z()
    /*
        Above macros define the 3x3 matrix as
        a0 a1 a2
        a3 a4 a5
        a6 a7 a8
    */

    //======================================================================================================================
    void Mat3::Concat( const Mat3 & lhs, const Mat3 & rhs ) {
        m_rows[ 0 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 0 ].X() +
                        rhs.m_rows[ 1 ] * lhs.m_rows[ 0 ].Y() +
                        rhs.m_rows[ 2 ] * lhs.m_rows[ 0 ].Z();

        m_rows[ 1 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 1 ].X() +
                        rhs.m_rows[ 1 ] * lhs.m_rows[ 1 ].Y() +
                        rhs.m_rows[ 2 ] * lhs.m_rows[ 1 ].Z();

        m_rows[ 2 ] =   rhs.m_rows[ 0 ] * lhs.m_rows[ 2 ].X() +
                        rhs.m_rows[ 1 ] * lhs.m_rows[ 2 ].Y() +
                        rhs.m_rows[ 2 ] * lhs.m_rows[ 2 ].Z();
    }

    //======================================================================================================================
    void Mat3::Transform( Vec3 & dst, const Vec3 & src ) const {
        dst =   m_rows[ 0 ] * src.X() +
                m_rows[ 1 ] * src.Y() +
                m_rows[ 2 ] * src.Z();
    }

    //======================================================================================================================
    void Mat3::Inverse(const Mat3 & rhs) {
        // Calculate the first three cofactors so we can also calculate the determinate of the matrix and early out if
        // need be.
        float c0 = calcCofactor( a4, a5, a7, a8 );
        float c1 = calcCofactor( a3, a5, a6, a8 );
        float c2 = calcCofactor( a3, a4, a6, a7 );
        
        float det = (a0 * c0) - (a1 * c1) + (a2 * c2);

        if ( math::Abs( det ) < 0.6e-5f ) {
            *this = Mat3::IDENTITY;
            return;
        }

        float a = 1.0f / det;

        // If we get to here, we have a large enough determinate that we can use to calculate the inverse
        // so we need to calculate the rest of the coefficients
        float c3 = calcCofactor( a1, a2, a7, a8 );
        float c4 = calcCofactor( a0, a2, a6, a8 );
        float c5 = calcCofactor( a0, a1, a6, a7 );
        
        float c6 = calcCofactor( a1, a2, a4, a5 );
        float c7 = calcCofactor( a0, a2, a3, a5 );
        float c8 = calcCofactor( a0, a1, a3, a4 );

        // Create the adjunct matrix from the cofactors
        Vec3  adjRow0(  c0, -c3,  c6 );
        Vec3  adjRow1( -c1,  c4, -c7 );
        Vec3  adjRow2(  c2, -c5,  c8 );

        m_rows[ 0 ] = adjRow0 * a;
        m_rows[ 1 ] = adjRow1 * a;
        m_rows[ 2 ] = adjRow2 * a;
    }

    //======================================================================================================================
    void Mat3::SetRotation( const Quat & rhs ) {
        
        float xx = float( 2.0f ) * rhs.X() * rhs.X();
        float yy = float( 2.0f ) * rhs.Y() * rhs.Y();
        float zz = float( 2.0f ) * rhs.Z() * rhs.Z();
        
        float xw = float( 2.0f ) * rhs.X() * rhs.W();
        float yw = float( 2.0f ) * rhs.Y() * rhs.W();
        float zw = float( 2.0f ) * rhs.Z() * rhs.W();
        float xy = float( 2.0f ) * rhs.X() * rhs.Y();
        float xz = float( 2.0f ) * rhs.X() * rhs.Z();
        float yz = float( 2.0f ) * rhs.Y() * rhs.Z();
        
        m_rows[ 0 ].Set   ( float(1) - (yy + zz), xy + zw, xz - yw );
        m_rows[ 1 ].Set   ( xy - zw, float(1) - (xx + zz), yz + xw );
        m_rows[ 2 ].Set   ( xz + yw, yz - xw, float(1) - (xx + yy) );
    }

    //======================================================================================================================
    void Mat3::SetRotation( const Vec3 & axis, float angleRad ) {
        // Method taken from :-
        // Graphics Gems (Glassner, Academic Press, 1990)

        float c = math::Cos( angleRad );
        float s = math::Sin( angleRad );
        float t = 1.0f - c;

        // Cache the axis components
        float x = axis.X();
        float y = axis.Y();
        float z = axis.Z();

        // Pre-calculate various terms to help with readability
        float sx = s * x;
        float sy = s * y;
        float sz = s * z;

        float tx2 = t * ( x * x );
        float ty2 = t * ( y * y );
        float tz2 = t * ( z * z );

        float txy = t * x * y;
        float txz = t * x * z;
        float tyz = t * y * z;

        // Set the rows of the matrix
        m_rows[ 0 ].Set( tx2 + c, txy + sz, txz - sy );
        m_rows[ 1 ].Set( txy - sz, ty2 + c, tyz + sx );
        m_rows[ 2 ].Set( txz + sy, tyz - sx, tz2 + c );
    }
}
