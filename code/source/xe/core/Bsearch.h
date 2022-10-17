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

#ifndef __BSEARCH_H__
#define __BSEARCH_H__

#include "core/Common.h"

/*
 Macro used for implementing the body of a binary seach. It determines the index
 of the value that we are looking for defined by VALUE. If the item does not exist
 it will give the index in the array at which to insert the fine
 */
#define BSEARCH_BODY(INDEX_TYPE, VALUE_TYPE, INDEX_PTR, VALUE, ARRAY, ARRAY_SIZE)\
    INDEX_TYPE first = 0;\
    INDEX_TYPE last = (INDEX_TYPE)( ARRAY_SIZE - 1 );\
    boolean_t found = FALSE;\
    while ( first <= last ) {\
        INDEX_TYPE mid = ( ( last - first ) / 2 ) + first;\
        if ( ARRAY[ mid ] == VALUE ) {\
            found = TRUE;\
            first = mid;\
            break;\
        } else if ( ARRAY[ mid ] < VALUE ) {\
            first = mid + 1;\
        } else {\
            last = mid - 1;\
        }\
    }\
    if (INDEX_PTR != NULL) {\
        *INDEX_PTR = first;\
    }\
    return found;

XE_API boolean_t Bsearch_FindUint16( int32_t * indexOut, uint16_t value, const uint16_t * ValueArray, size_t arraySize);

XE_API boolean_t Bsearch_FindUint32( int32_t * indexOut, uint32_t value, const uint32_t * ValueArray, size_t arraySize);

XE_API boolean_t Bsearch_FindUint64( int32_t * indexOut, uint64_t value, const uint64_t * ValueArray, size_t arraySize);

XE_API boolean_t Bsearch_FindUintPtr( int32_t * indexOut, uintptr_t value, const uintptr_t * ValueArray, size_t arraySize);

XE_API boolean_t Bsearch_FindVoidPtr( int32_t * indexOut, void * value, const void ** ValueArray, size_t arraySize);

#endif
