/*
===========================================================================================================================================

    Copyright 2016 - 2022 James Steele

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

#ifndef __STR_H__
#define __STR_H__

#include "core/Platform.h"

typedef char * str_t;

#define STR_NOT_FOUND -1

/**
    /brief Determines if the string is empty or not
    /param rhs The string that is being queried
    /returns true if th string is empty, false if not. The string is assumed to be emtpy if rhs is NULL
 */
XE_API bool_t Str_IsEmpty( const str_t rhs );

/**
    /brief Returns the length of a dynamic string
    /param rhs The string that is being queried
    /returns The length of the string not including the terminating zero. If rhs is NULL zero is returned.
 */
XE_API size_t Str_GetLength( const str_t rhs );

XE_API void Str_Destroy( str_t * self_ );

/**
    /brief Sets the length of a dynamic string
    /param self_ Pointer to the dynamic string we want to set the length of
    /param length The length of the string, not including the terminating zero
 */
XE_API void Str_SetLength( str_t * self_, size_t length );

/**
    /brief Sets the amount of memory that is actually used by the string
    /param self_ Pointer to the dynamic string we want to set the length of
    /param length The desired capacity of the string
 */
XE_API void Str_SetCapacity( str_t * self_, size_t cpacity );


XE_API void Str_CopyCStr( str_t * self_, const char * rhs );
XE_API void Str_Copy( str_t * self_, const str_t rhs );
XE_API void Str_Append(str_t * self_, const str_t rhs );

/**
    /brief Append a normal c-string to a dynamic string.
    /param self_ Pointer to the string that we're appending.
    /param rhs Const pointer to the normal c-string that we want to append to the end of the dynamic string.
 */
XE_API void Str_AppendCStr(str_t * self_, const char * rhs );

/**
    /brief Concatonates two dynamic strings
    /param self_ Pointer to thr string that will hold the concatonated string
    /param lhs The first string to concatonate that will be on the "left" of the resulting string
    /param rhs The first string to concatonate that will be on the "right" of the resulting string
 */
XE_API void Str_Concat(str_t * dst, const str_t lhs, const str_t rhs );


/**
    /brief Concatonates two normal c-strings
    /param self_ Pointer to thr string that will hold the concatonated string
    /param lhs The first string to concatonate that will be on the "left" of the resulting string
    /param rhs The first string to concatonate that will be on the "right" of the resulting string
 */
XE_API void Str_ConcatCStr(str_t * dst, const char * lhs, const char * rhs );


/**
    /brief Gets a substring of a string
    /param lhs Pointer to the dynamic string that will hold the resulting substring
    /param rhs The string that contins the string we want to extract the sub string from
    /param start Index of the firxt character of the sub string in rhs
    /para count The number of characters in the substring
 */
XE_API void Str_SubStr( str_t * lhs, const str_t rhs, uintptr_t start, size_t count );

XE_API void Str_Left(  str_t * lhs, const str_t rhs, size_t size );

XE_API void Str_Right(  str_t * lhs, const str_t rhs, size_t size );

XE_API void Str_ToLower(  str_t self_ );

XE_API void Str_ToUpper(  str_t self_ );

XE_API void Str_ReplaceChar( str_t self_, char replaceChar, char replaceWith );

XE_API intptr_t Str_FindChar(const str_t lhs, const char rhs );

XE_API intptr_t Str_RFindChar(const str_t lhs, const char rhs );

XE_API void Str_AppendPathCStr( str_t * self_, const char * pathItem );

XE_API void Str_AppendPath( str_t * self_, const str_t rhs );

XE_API intptr_t Str_PathFindExtension( const str_t self_ );

XE_API bool_t Str_PathRemoveExtension( str_t * self_ );

XE_API bool_t Str_PathGetExtension( str_t * ext, const str_t path );

XE_API void Str_VFormatArgs( str_t * self_, const char * fmt, va_list vaArgs );

XE_API void Str_VFormat( str_t * self_, const char * fmt, ... );

XE_API bool_t Str_PathRemoveLastElement( str_t * self_ );

#endif
