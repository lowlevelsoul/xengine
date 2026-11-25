/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __BSEARCH_H__
#define __BSEARCH_H__

#include "xe/Platform.h"

#define BSEARCH_DECLARE( NAME, TYPE ) \
    bool BSearch_##NAME( size_t * indexOut, const TYPE * array, size_t size, TYPE value )

#define BSEARCH_IMPLEMENT( NAME, TYPE ) \
    bool BSearch_##NAME( size_t * indexOut, const TYPE * array, size_t size, TYPE value ) { \
        size_t left = 0; \
        size_t right = size; \
        while (left < right) { \
            size_t mid = left + (right - left) / 2; \
            if (array[mid] == value) { \
                *indexOut = mid; \
                return true; \
            } else if (array[mid] < value) { \
                left = mid + 1; \
            } else { \
                right = mid; \
            } \
        } \
        *indexOut = left; \
        return false; \
    }

BSEARCH_DECLARE( Int32, int32_t );
BSEARCH_DECLARE( UInt32, uint32_t );
BSEARCH_DECLARE( Int64, int64_t );
BSEARCH_DECLARE( UInt64, uint64_t );
BSEARCH_DECLARE( VoidPtr, void * );


#endif // __BSEARCH_H__