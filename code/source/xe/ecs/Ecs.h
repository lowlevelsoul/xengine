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

#ifndef __ECS_H__
#define __ECS_H__

#include "core/Common.h"
#include "ecs/EcsTypes.h"

#define Ecs_RegisterSystem( __system__, __struct__ ) Ecs_RegisterSystemNamed( __system__, #__struct__ )
#define Ecs_RegisterComponent( __struct__, __capacity__) Ecs_RegisterNamedComponent( #__struct__, __capacity__, sizeof(__struct__) )
#define Ecs_AddComponent( __ent__, __struct__ ) (__struct__*) Ecs_AddNamedComponent( __ent__, #__struct__ )
#define Ecs_GetEntityComponentIndex( __ent__, __struct__ ) Ecs_GetEntityNamedComponentIndex( __ent__, #__struct__ )

XE_API void Ecs_Initialise(void);
XE_API void Ecs_Finalise(void);
XE_API void Ecs_RegisterNamedComponent( const char * name, size_t capacity, size_t structSize );
XE_API ecs_entity_t Ecs_EntityAlloc(void);
XE_API void Ecs_EntityFree( ecs_entity_t ent );
XE_API void * Ecs_AddNamedComponent( ecs_entity_t ent, const char * compName );
XE_API void * Ecs_AddHashedNamedComponent( ecs_entity_t ent, const char * compName );
XE_API int32_t Ecs_GetComponentArrayIndex( const char * name );
XE_API ecs_system_id_t Ecs_RegisterSystemNamed( ecs_system_t * system, const char * componentName );
XE_API void Ecs_EntityConstructDefault( ecs_entity_t ent );
XE_API int32_t Ecs_GetEntityNamedComponentIndex( ecs_entity_t ent, const char * name );
XE_API void * Ecs_GetEntityIndexedComponent( ecs_entity_t ent, int32_t index );

XE_API void Ecs_SystemThink( int32_t systemIndex, ecs_think_params_t * params );
XE_API void Ecs_SendMessage( ecs_entity_t ent, uint16_t msgId, const void * data, size_t dataSize );

XE_API void Ecs_EndFrame(void);



#endif
