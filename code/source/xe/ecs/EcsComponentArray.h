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

#ifndef __COMPONENTARRAY_H__
#define __COMPONENTARRAY_H__

#include "core/Common.h"
#include "ecs/EcsTypes.h"

typedef struct ecs_component_array_s {
    uint64_t        data[16];
} ecs_component_array_t;

typedef struct ecs_component_list_s {
    void ** componentData;
    ecs_entity_t * entities;
    size_t count;
} ecs_component_list_t;

XE_API void EcsComponentArray_Create( ecs_component_array_t * self_, size_t capacity, size_t componentSize, const char * name );
XE_API void EcsComponentArray_Destroy( ecs_component_array_t * self_ );
XE_API void * EcsComponentArray_AddEntity( ecs_component_array_t * self_, ecs_entity_t ent );
XE_API void EcsComponentArray_RemoveEntity( ecs_component_array_t * self_, ecs_entity_t ent );
XE_API void * EcsComponentArray_GetComponentForEntity( ecs_component_array_t * self_, ecs_entity_t ent );
XE_API bool_t EcsComponentArray_HasEntity( const ecs_component_array_t * self_, ecs_entity_t ent );
XE_API const char * EcsComponentArray_GetName( const ecs_component_array_t * self_ );
XE_API void EcsComponentArray_GetActiveComponents( ecs_component_array_t * self_, ecs_component_list_t * activeListOut );


#endif
