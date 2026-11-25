/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/
#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "xe/Sys.h"

#define ARRAY_DECLARE( name, type ) \
    X_API bool Array_##name##_Append( type * elements, size_t count, size_t capacity, const type * value ); \
    X_API bool Array_##name##_Insert( type * elements, size_t count, size_t capacity, size_t index, const type * value ); \
    X_API bool Array_##name##_InsertSorted( type * elements, size_t count, size_t capacity, size_t * indexOut, const type * value ); \
    X_API bool Array_##name##_RemoveAt( type * elements, size_t count, size_t capacity, size_t index ); \
    X_API bool Array_##name##_RemoveAtFast( type * elements, size_t count, size_t capacity, size_t index ); \
    X_API bool Array_##name##_Find( const type * elements, size_t count, const type * value, size_t * indexOut ); \
    X_API bool Array_##name##_FindSorted( const type * elements, size_t count, const type * value, size_t * indexOut ); \

#define ARRAY_IMPLEMENT( name, type )\
    X_API bool Array_##name##_Insert( type * elements, size_t count, size_t capacity, size_t index, const type * value ) {\
        if (count >= capacity) {\
            return false;\
        }\
        for (size_t i = count; i > index; --i) {\
            elements[i] = elements[i - 1];\
        }\
        elements[index] = *value;\
        return true;\
    }\
    X_API bool Array_##name##_Append( type * elements, size_t count, size_t capacity, const type * value ) {\
        if (count >= capacity) {\
            return false;\
        }\
        elements[count] = *value;\
        return true;\
    }\
    X_API bool Array_##name##_RemoveAt( type * elements, size_t count, size_t capacity, size_t index ) {\
        if (index >= count) {\
            return false;\
        }\
        for (size_t i = index; i < count - 1; ++i) {\
            elements[i] = elements[i + 1];\
        }\
        return true;\
    }\
    X_API bool Array_##name##_RemoveAtFast( type * elements, size_t count, size_t capacity, size_t index ) {\
        if (index >= count) {\
            return false;\
        }\
        elements[index] = elements[count - 1];\
        return true;\
    }\
    X_API bool Array_##name##_Find( const type * elements, size_t count, const type * value, size_t * indexOut ) {\
        for (size_t i = 0; i < count; ++i) {\
            if (elements[i] == *value) {\
                if (indexOut) {\
                    *indexOut = i;\
                }\
                return true;\
            }\
        }\
        return false;\
    }\
    X_API bool Array_##name##_FindSorted( const type * elements, size_t count, const type * value, size_t * indexOut ) {\
        size_t left = 0;\
        size_t right = count;\
        while (left < right) {\
            size_t mid = left + (right - left) / 2;\
            if (elements[mid] < *value) {\
                left = mid + 1;\
            } else if (elements[mid] > *value) {\
                right = mid;\
            } else {\
                if (indexOut) {\
                    *indexOut = mid;\
                }\
                return true;\
            }\
        }\
        if ( indexOut ) {\
            *indexOut = left;\
        }\
        return false;\
    }\
    X_API bool Array_##name##_InsertSorted( type* elements, size_t count, size_t capacity, size_t* indexOut, const type* value ) {\
        size_t insertIndex = 0;\
        bool found = Array_##name##_FindSorted( elements, count, value, &insertIndex );\
        if ( found ) {\
            if ( indexOut ) {\
                *indexOut = insertIndex;\
            }\
            return false;\
        }\
        if ( count >= capacity ) {\
            return false;\
        }\
        for ( size_t i = count; i > insertIndex; --i ) {\
            elements[i] = elements[i - 1];\
        }\
        elements[insertIndex] = *value;\
        if ( indexOut ) {\
            *indexOut = insertIndex;\
        }\
        return true;\
    } \




ARRAY_DECLARE( Int32, int32_t )
ARRAY_DECLARE( UInt32, uint32_t )
ARRAY_DECLARE( Int64, int64_t )
ARRAY_DECLARE( UInt64, uint64_t )
ARRAY_DECLARE( Float, float )
ARRAY_DECLARE( Double, double )


#endif /* __ARRAY_H__ */