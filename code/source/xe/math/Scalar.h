/*
===========================================================================================================================================

    Copyright 2019 James Steele

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
    files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

===========================================================================================================================================
*/

#ifndef __SCALAR_H__
#define __SCALAR_H__

#include <math.h>
#include "core/Common.h"

#define PI 3.14159265359f

#define scalar_IsValid(V) ((V) == (V))
#define scalar_Min(A, B) ((A) < (B)) ? (A) : (B)
#define scalar_Max(A, B) ((A) > (B)) ? (A) : (B)
#define scalar_Clamp(V, MI, MA) scalar_Max(MI, (scalar_Min(MA, V)))
#define scalar_Mix(A, B, T) ((A) * (1.0f - (T))) + ((B) * (T))

#define scalar_Abs(A) fabsf((A))
#define scalar_Sqrt(A) sqrtf((A))
#define scalar_SqrtFast(A) sqrtf((A))
#define scalar_Cos(A) cosf((A))
#define scalar_Sin(A) sinf((A))
#define scalar_Tan(A) tanf((A))

#define scalar_Acos(A) acosf((A))
#define scalar_Asin(A) asinf((A))
#define scalar_Atan(A) atanf((A))
#define scalar_Atan2(A, B) atan2f((A), (B))

#define scalar_DegToRad(X) ((X) * (PI / 180.0f))
#define scalar_RadToDeg(X) ((X) * (180.0f / PI))

/*=======================================================================================================================================*/
X_INLINE float scalar_MixSafe(float a, float b, float t) {
    t = scalar_Clamp( t, 0, 1 );
    return scalar_Mix( a, b, t );
}

#endif
