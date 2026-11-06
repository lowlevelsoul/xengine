/*
==============================================================================================================
    This file is part of XEngine 
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __STR_H__
#define __STR_H__ 

#include "Platform.h"

typedef char * str_t;

X_API void Str_Resize(str_t * self_, size_t length );
X_API void Str_Reserve(str_t *self_, size_t reserve );
X_API void Str_Set( str_t * self_, str_t src );
X_API void Str_Destroy( str_t * self_);
X_API void Str_SetCStr( str_t * self_, const char* src);
X_API size_t Str_Length( const str_t self_);
X_API size_t Str_Capacity( const str_t self_ );

X_API void Str_AppendCStr(str_t * self_, const char* src);
X_API void Str_Append( str_t * self_, str_t src );

X_API void Str_Concat( str_t * self_, const str_t lhs, const str_t rhs );
X_API void Str_ConcatCStr( str_t * self_, const char * lhs, const char * rhs );
X_API void Str_Append( str_t * self_, str_t src);
X_API void Str_AppendCStr( str_t * self_, const char* src);

X_API void Str_PathJoinCStr( str_t * self_, const char* lhs, const char* rhs );
X_API void Str_PathJoin( str_t * self_, const str_t lhs, const str_t rhs );

X_API void Str_FormatV( str_t * self_, const char* fmt, ... );
X_API void Str_FormatArg( str_t * self_, const char* fmt, va_list args );

#endif // __STR_H__