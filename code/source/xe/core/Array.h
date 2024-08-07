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

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "core/Platform.h"

#ifdef __cplusplus
extern "C" {
#endif

void Array_RemoveAtPosInt32( int32_t pos, int32_t* valueArray, size_t valueArraySize );

void Array_RemoveAtPosUInt32( int32_t pos, uint32_t* valueArray, size_t valueArraySize );

void Array_RemoveAtPosInt64( int32_t pos, int64_t* valueArray, size_t valueArraySize );

void Array_RemoveAtPosUint64( int32_t pos, uint64_t* valueArray, size_t valueArraySize );

void Array_InsertAtPosUint32(int32_t pos, uint32_t value, uint32_t* valueArray, size_t valueArraySize, size_t valueArrayCapacity);

void Array_InsertAtPosUint64(int32_t pos, uint64_t value, uint64_t* valueArray, size_t valueArraySize, size_t valueArrayCapacity);

void Array_InsertAtPosUintPtr(int32_t pos, uintptr_t value, uintptr_t* valueArray, size_t valueArraySize, size_t valueArrayCapacity);
    
void Array_InsertAtPosVoidPtr(int32_t pos, void * value, void ** valueArray, size_t valueArraySize, size_t valueArrayCapacity);

#ifdef __cplusplus
}
#endif


#endif
