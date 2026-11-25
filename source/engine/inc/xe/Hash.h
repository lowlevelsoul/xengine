/*
==============================================================================================================
    This file is part of XEngine
    Copyright (C) 2026 James Steele. All rights reserved.
==============================================================================================================
*/

#ifndef __HASH_H__
#define __HASH_H__

#include "xe/Platform.h"

X_API uint64_t Hash_BytesFh64( const void* data, size_t size );
X_API uint64_t Hash_StringFh64( const char* str );

#endif // __HASH_H__