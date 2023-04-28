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

#include "core/Bsearch.h"

/*=======================================================================================================================================*/
bool_t Bsearch_FindUint16(int32_t* indexOut, uint16_t value, const uint16_t* valueArray, size_t arraySize) {
    BSEARCH_BODY(int16_t, uint16_t, indexOut, value, valueArray, arraySize)
}

/*=======================================================================================================================================*/
bool_t Bsearch_FindUint32(int32_t* indexOut, uint32_t value, const uint32_t* valueArray, size_t arraySize) {
   BSEARCH_BODY(int32_t, uint32_t, indexOut, value, valueArray, arraySize)
}

/*=======================================================================================================================================*/
bool_t Bsearch_FindUint64(int32_t* indexOut, uint64_t value, const uint64_t* valueArray, size_t arraySize) {
   BSEARCH_BODY(int32_t, uint64_t, indexOut, value, valueArray, arraySize)
}

/*=======================================================================================================================================*/
bool_t Bsearch_FindUintPtr(int32_t* indexOut, uintptr_t value, const uintptr_t* valueArray, size_t arraySize) {
   BSEARCH_BODY(int32_t, uintptrt, indexOut, value, valueArray, arraySize)
}

/*=======================================================================================================================================*/
bool_t Bsearch_FindVoidPtr( int32_t * indexOut, void * value, const void ** valueArray, size_t arraySize) {
    BSEARCH_BODY(int32_t, void*, indexOut, value, valueArray, arraySize );
}
