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

#ifndef __RTTITYPE_H__
#define __RTTITYPE_H__

#include "core/Common.h"
#include "mem/Mem.h"

typedef struct rtti_prop_s {
    uint32_t        type;
    uint32_t        offset;
    uint32_t        dim;
    uint32_t        flags;
} rtti_prop_t;

typedef struct rtti_type_s {
    const char *        name;
    void                * (*create)();
    uint64_t *          propHashArray;
    uint32_t *          propHashMap;
    size_t              propCount;
    rtti_prop_t *       props;
} rtti_type_t;


#define RTTI_TYPE_DECLARE(TYPE, CONTENTS)\
    typedef TYPE_##s {\
        rtti_type_t * type;\
        CONTENTS\
    } TYPE_##t;\
    extern rtti_type_t TYPE##_type;

#define RTTI_TYPE_BEGIN(TYPE)

#define RTTI_TYPE_END(TYPE)\
    extern rtti_type_t TYPE##_type;\
    static void * TYPE##_factory(void) { return Mem_Alloc(sizeof(TYPE##_t); }\
    rtti_type_t TYPE##_type {\
        #TYPE"_t",\
        TYPE##_factory,\
        NULL, NULL, 0,\
        NULL\
    };

#endif
